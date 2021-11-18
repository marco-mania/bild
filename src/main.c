/* BILD - Wavelet based image compression
 * All rights reserved (since 2004). Marco Nelles.
 * <https://gitlab.com/wavelets/bild>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "image.h"
#include "bild.h"

#include "types.h"

#define DEFAULT_QUALITY 4

/* Remove file extension */
char* remove_ext(const char *filename)
{
  char *result;
  if (!filename) return NULL;
  if (!(result = malloc(strlen(filename) + 1))) return NULL;
  strcpy(result, filename);
  char *lastdot = strrchr(result, '.');
  if (lastdot) *lastdot = '\0';
  return result;
}

void help(const char *bin)
{

  fprintf(stdout, "BILD Version %d: Wavelet based image compression tool.\n\n", VERSION);
  fprintf(stdout, "Usage: %s <command> [options] <input file> <output file>\n\n", bin);

  fprintf(stdout, "Example: %s -c -t %u image.bmp # Compress image.bmp to image.bild with quality parameter %u\n", bin, DEFAULT_QUALITY, DEFAULT_QUALITY);
  fprintf(stdout, "Example: %s -d image.bild reconstruction.bmp # Decompress image.bild to picture.bmp\n\n", bin);

  fprintf(stdout, "Commands:\n");
  fprintf(stdout, "  -c              Compress image.\n");
  fprintf(stdout, "  -d              Decompress BILD image.\n");
  fprintf(stdout, "  -i              Show BILD image information.\n");
  fprintf(stdout, "  -h              Show this help.\n");
  fprintf(stdout, "  -v              Print version.\n\n");

  fprintf(stdout, "Compression options:\n");
  fprintf(stdout, "  -q <N>          Quality parameter. N is an integer (0..7).\n");
  fprintf(stdout, "                  0: lossless compression.\n");
  fprintf(stdout, "                  %u: standard value.\n", DEFAULT_QUALITY);

}

int main(int argc, const char **argv)
{

  const char *input_filename = NULL;
  const char *output_filename = NULL;
  char *input_filename_noext = NULL;

  unsigned int quality = DEFAULT_QUALITY;

  int arg = 1;
  bool bWrongArgs = (argc < 2);
  enum Command {Compress, Decompress, Information, Help, Version} command = Help;
  bool flag = true;

  while ((!bWrongArgs) && (arg < argc))
  {

    if (argv[arg][0] == '-')
    {

      switch (argv[arg][1])
      {

        case 'c': command = Compress; arg++; break;
        case 'd': command = Decompress; arg++; break;
        case 'i': command = Information; arg++; break;
        case 'h': command = Help; arg++; break;
        case 'v': command = Version; arg++; break;

        case 'q':
          arg++;
          if (arg == argc)
          {
            bWrongArgs = true;
          }
          else
          {
            quality = atoi(argv[arg]); arg++;
            bWrongArgs = ((quality < 0) || (quality > 7));
          }
          break;

        default: arg++; bWrongArgs = true; break;

      }

    }
    else
    {

      if (flag)
      {
        input_filename = argv[arg];
        flag = false;
      }
      else
      {
        output_filename = argv[arg];
      }
      arg++;

    }

  }

  if (bWrongArgs || (command == Help))
  {
    help(argv[0]);
    return 0;
  }

  if (command == Version)
  {
    fprintf(stdout, "%d\n", VERSION);
    return 0;
  }

  if (command == Information)
  {
    BILDPrintInformation(input_filename);
    return 0;
  }

  input_filename_noext = remove_ext(input_filename);

  char output_filename_buffer[256];

  if (command == Compress)
  {

    if (output_filename)
      sprintf(output_filename_buffer, "%s", output_filename);
    else
      sprintf(output_filename_buffer, "%s.bild", input_filename_noext);

    fprintf(stdout, "Compressing %s to %s ...\n", input_filename, output_filename_buffer);
    fflush(stdout);

    Image *image = ImageLoadFromBMPFileAndCreate(input_filename);

    if (!image)
    {
      printf("Failed.\n");
      return 1;
    }

    ImageSaveAsBILDFile(image, output_filename_buffer, quality);
    ImageDestroy(image);

    printf("Done.\n");

  }
  else if (command == Decompress)
  {

    if (output_filename)
      sprintf(output_filename_buffer, "%s", output_filename);
    else
      sprintf(output_filename_buffer, "%s.bmp", input_filename_noext);

    fprintf(stdout, "Decompressing %s to %s ...\n", input_filename, output_filename_buffer);
    fflush(stdout);

    Image *image = ImageLoadFromBILDFileAndCreate(input_filename);

    if (!image)
    {
      printf("Failed.\n");
      return 1;
    }

    ImageSaveAsBMPFile(image, output_filename_buffer);
    ImageDestroy(image);

    printf("Done.\n");

  }

  return 0;

}

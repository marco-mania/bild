/* BILD - Wavelet based image compression
 * All rights reserved (2014-2018). Marco Nelles.
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

#include "image.h"

Image* ImageCreate(const int width, const int height, const ColourSpace cs)
{

  Image *image = malloc(sizeof(Image));
  image->width = width;
  image->height = height;
  image->colour_space = cs;

  switch (image->colour_space) {
    case Grayscale : image->channel_count = 1; break;
    case RGB :
    case YCbCr411 : image->channel_count = 3; break;
  }

  image->channels = malloc(sizeof(Signal2D*)*image->channel_count);

  int i;
  if ((width == 0) || (height == 0))
  {
    for (i = 0; i < image->channel_count; ++i) image->channels[i] = NULL;
  }
  else
  {
    for (i = 0; i < image->channel_count; ++i)
      image->channels[i] = Signal2DCreate(image->width, image->height);
  }

  return image;

}

void ImageDestroy(Image *image) {

  int i;
  for (i = 0; i < image->channel_count; ++i)
    Signal2DDestroy(image->channels[i]);

  free(image->channels);

  free(image);

}

Image* ImageLoadFromBMPFileAndCreate(const char *filename) {

  FILE *bmp_file = fopen(filename, "rb");

  if (bmp_file == NULL)
  {
    printf("Unable to open BMP file for reading.\n");
    return NULL;
  }

  BMPFileHeader bmp_fileheader;
  if (!fread(&bmp_fileheader, sizeof(BMPFileHeader), 1, bmp_file))
  {
    printf("Unable to read BMP file header.\n");
    return NULL;
  }

  if (bmp_fileheader.bfType != BMP_MAGIC)
  {
    printf("No valid BMP file.\n");
    return NULL;
  }

  BMPInfoHeader bmp_infoheader;
  if (!fread(&bmp_infoheader, sizeof(BMPInfoHeader), 1, bmp_file))
  {
    printf("Unable to read BMP info header.\n");
    return NULL;
  }

  if (bmp_infoheader.biBitCount != 24)
  {
    printf("Only bitmaps of 24 bit color depth processable.\n");
    return NULL;
  }

  if (bmp_infoheader.biCompression != 0)
  {
    printf("Unable to process compressed bitmaps.\n");
    return NULL;
  }

  const int32_t w = bmp_infoheader.biWidth;
  int32_t h = bmp_infoheader.biHeight;

  bool topdown = false;
  if (h < 0)
  {
    h = ABS(h);
    topdown = true;
  }

  Image *result = ImageCreate(w, h, RGB);

  const int data_size = 3*w*h;

  uint8_t *buf = malloc(data_size);

  if (fseek(bmp_file, bmp_fileheader.bfOffBits, SEEK_SET))
  {
    printf("Unable to jump to data position in BMP.\n");
    return NULL;
  }

  if (!fread(buf, sizeof(uint8_t), data_size, bmp_file))
  {
    printf("Unable to read BMP data.\n");
    return NULL;
  }

  uint8_t *p = buf;
  if (!topdown) p += (h-1)*w*3; /* set pointer to start of last line. */

  int padBytes = w % 4;

  int rewind = 2*w*3;
  if (padBytes != 0) rewind += padBytes;

  int i, j;
  for (j = 0; j < h; ++j)
  {
    for (i = 0; i < w; ++i)
    {
      result->channels[2]->data[result->channels[2]->data_pos++] = *p++; /* B */
      result->channels[1]->data[result->channels[1]->data_pos++] = *p++; /* G */
      result->channels[0]->data[result->channels[0]->data_pos++] = *p++; /* R */
    }
    if (!topdown) {
      p -= rewind; /* set pointer to start of previous line. */
    } else {
      if (padBytes != 0) p += padBytes;
    }
  }

  free(buf);

  result->channels[0]->data_pos = 0;
  result->channels[1]->data_pos = 0;
  result->channels[2]->data_pos = 0;

  fclose(bmp_file);

  return result;

}

void ImageSaveAsBMPFile(Image *image, const char *filename) {

  if (image->colour_space != RGB)
  {
    printf("Only RGB can be written to BMP yet.\n");
    return;
  }

  FILE *bmp_file = fopen(filename, "wb");

  if (bmp_file == NULL)
  {
    printf("Unable to open BMP file for writing.\n");
    return;
  }

  BMPFileHeader bmp_fileheader;
  BMPInfoHeader bmp_infoheader;

  bmp_fileheader.bfType = BMP_MAGIC;
  bmp_fileheader.bfSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + (3*image->width*image->height);
  bmp_fileheader.bfReserved = 0;
  bmp_fileheader.bfOffBits = 54;

  bmp_infoheader.biSize = 40;
  bmp_infoheader.biWidth = image->width;
  bmp_infoheader.biHeight = image->height;
  bmp_infoheader.biPlanes = 1;
  bmp_infoheader.biBitCount = 24;
  bmp_infoheader.biCompression = 0;
  bmp_infoheader.biSizeImage = 0;
  bmp_infoheader.biXPelsPerMeter = 0;
  bmp_infoheader.biYPelsPerMeter = 0;
  bmp_infoheader.biClrUsed = 0;
  bmp_infoheader.biClrImportant = 0;

  if (fwrite(&bmp_fileheader, sizeof(BMPFileHeader), 1, bmp_file) < 1)
  {
    printf("Unable to write BMP file header.\n");
    return;
  }

  if (fwrite(&bmp_infoheader, sizeof(BMPInfoHeader), 1, bmp_file) < 1)
  {
    printf("Unable to write BMP info header.\n");
    return;
  }

  const int data_size = 3*image->width*image->height;

  uint8_t *buf = malloc(data_size);

  const bool topdown = false;

  uint8_t *p = buf;
  if (!topdown) p += (image->height-1)*image->width*3; /* set pointer to start of last line. */

  int padBytes = image->width % 4;

  int rewind = 2*image->width*3;
  if (padBytes != 0) rewind += padBytes;

  const int dp0 = image->channels[0]->data_pos;
  const int dp1 = image->channels[1]->data_pos;
  const int dp2 = image->channels[2]->data_pos;

  image->channels[0]->data_pos = 0;
  image->channels[1]->data_pos = 0;
  image->channels[2]->data_pos = 0;

  int i, j;
  for (j = 0; j < image->height; ++j)
  {
    for (i = 0; i < image->width; ++i)
    {
      *p++ = image->channels[2]->data[image->channels[2]->data_pos++]; /* B */
      *p++ = image->channels[1]->data[image->channels[1]->data_pos++]; /* G */
      *p++ = image->channels[0]->data[image->channels[0]->data_pos++]; /* R */
    }
    if (!topdown) {
      p -= rewind; /* set pointer to start of previous line. */
    } else {
      if (padBytes != 0) p += padBytes;
    }
  }

  image->channels[0]->data_pos = dp0;
  image->channels[1]->data_pos = dp1;
  image->channels[2]->data_pos = dp2;

  if (fwrite(buf, sizeof(uint8_t), data_size, bmp_file) < 1)
  {
    printf("Unable to write BMP data.\n");
    return;
  }

  free(buf);

  fclose(bmp_file);

}

void ImageTransformColourSpace(Image *image, const ColourSpace new_cs)
{

  if (image->colour_space == new_cs) return;

  int i;

  if ((image->colour_space == YCbCr411) && (new_cs == RGB)) /* YCbCr411 -> RGB */
  {

    Signal2DUpsample2(image->channels[1], image->width, image->height);
    Signal2DUpsample2(image->channels[2], image->width, image->height);

    for (i = 0; i < image->width*image->height; ++i)
    {

      const int32_t Y = image->channels[0]->data[i];
      const int32_t Cb = image->channels[1]->data[i];
      const int32_t Cr = image->channels[2]->data[i];
      const int32_t G = Y - ((Cb + Cr) >> 2) + 128;

      image->channels[0]->data[i] = Cb + G;
      image->channels[1]->data[i] = G;
      image->channels[2]->data[i] = Cr + G;

    }

  }
  else if ((image->colour_space == RGB) && (new_cs == YCbCr411)) /* RGB -> YCbCr411 */
  {

    for (i = 0; i < image->width*image->height; ++i)
    {

      const int32_t R = image->channels[0]->data[i];
      const int32_t G = image->channels[1]->data[i];
      const int32_t B = image->channels[2]->data[i];

      image->channels[0]->data[i] = ((R + (G<<1) + B) >> 2) - 128;
      image->channels[1]->data[i] = R - G;
      image->channels[2]->data[i] = B - G;

    }

    Signal2DDownsample2(image->channels[1]);
    Signal2DDownsample2(image->channels[2]);

  }
  else if ((image->colour_space == RGB) && (new_cs == Grayscale)) /* RGB -> Grayscale */
  {

    for (i = 0; i < image->width*image->height; ++i)
    {

      const int32_t R = image->channels[0]->data[i];
      const int32_t G = image->channels[1]->data[i];
      const int32_t B = image->channels[2]->data[i];

      image->channels[0]->data[i] = (R * 11 + G * 16 + B * 5) >> 5;

    }

    free(image->channels[1]);
    image->channels[1] = NULL;
    free(image->channels[2]);
    image->channels[2] = NULL;
    image->channel_count = 1;

  }
  else if ((image->colour_space == YCbCr411) && (new_cs == Grayscale)) /* YCbCr411 -> Grayscale */
  {

    free(image->channels[1]);
    image->channels[1] = NULL;
    free(image->channels[2]);
    image->channels[2] = NULL;
    image->channel_count = 1;

  }

  image->colour_space = new_cs;

}

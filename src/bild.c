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

#include "bild.h"

Levels2D* p_FileToLevels(FILE *f, const bool rle_compression)
{

  BILDLevelsHeader levels_header;
  fread(&levels_header, sizeof(byte), sizeof(BILDLevelsHeader), f);

  Levels2D *result = Levels2DCreate(levels_header.level_count, levels_header.width, levels_header.height);
  result->root_value = levels_header.root_value;
  result->level_count = levels_header.level_count;
  result->width = levels_header.width;
  result->height = levels_header.height;

  BILDLevelHeader level_header;

  int i;
  for (i = 0; i < levels_header.level_count; ++i)
  {

    fread(&level_header, sizeof(byte), sizeof(BILDLevelHeader), f);

    result->levels[i] = Level2DCreate(level_header.ll_width, level_header.ll_height,
                                      Signal2DCreate(level_header.lh_width, level_header.lh_height),
                                      Signal2DCreate(level_header.hl_width, level_header.hl_height),
                                      Signal2DCreate(level_header.hh_width, level_header.hh_height));

  }

  int8_t *buf1 = malloc(levels_header.width*levels_header.height*2);
  int buf1_size;

  int8_t *buf2 = malloc(levels_header.width*levels_header.height*2);
  int buf2_size;

  int32_t *overflow_buf = malloc(levels_header.overflow_buffer_size*sizeof(int32_t));
  int overflow_buf_pos = 0;

  fread(buf1, sizeof(int8_t), levels_header.coded_size, f);

  if (levels_header.overflow_buffer_size > 0)
    fread(overflow_buf, sizeof(int32_t), levels_header.overflow_buffer_size, f);

  huffmanDecode(buf1, levels_header.coded_size, buf2, &buf2_size);

  int8_t *src_buf = buf1;
  int src_buf_pos = 0;

  if (rle_compression)
  {

    rleDecode8(buf2, buf2_size, buf1, &buf1_size);

    src_buf = buf1;

  }
  else
  {

    src_buf = buf2;

  }

  int j;
  for (i = 0; i < levels_header.level_count; ++i)
  {

    for (j = 0; j < result->levels[i]->lh->width*result->levels[i]->lh->height; ++j)
      result->levels[i]->lh->data[result->levels[i]->lh->data_pos++] = unpack8_32(src_buf[src_buf_pos++], overflow_buf, &overflow_buf_pos);

    for (j = 0; j < result->levels[i]->hl->width*result->levels[i]->hl->height; ++j)
      result->levels[i]->hl->data[result->levels[i]->hl->data_pos++] = unpack8_32(src_buf[src_buf_pos++], overflow_buf, &overflow_buf_pos);

    for (j = 0; j < result->levels[i]->hh->width*result->levels[i]->hh->height; ++j)
      result->levels[i]->hh->data[result->levels[i]->hh->data_pos++] = unpack8_32(src_buf[src_buf_pos++], overflow_buf, &overflow_buf_pos);

  }

  free(buf1);
  free(buf2);

  free(overflow_buf);

  return result;

}

Image *ImageLoadFromBILDFileAndCreate(const char *filename)
{

  struct stat st;
  stat(filename, &st);
  if (st.st_size < sizeof(BILDHeader))
  {

    printf("No BILD file.\n");

    return NULL;

  }

  FILE *f = fopen(filename, "rb");

  BILDHeader header;
  fread(&header, sizeof(byte), sizeof(BILDHeader), f);

  if (header.type != BILD_TYPE)
  {

    printf("No BILD file.\n");

    fclose(f);
    return NULL;

  }

  if (header.version != VERSION)
  {

    printf("Wrong BILD file version: Found %d but expected %d.\n", header.version, VERSION);

    fclose(f);
    return NULL;

  }

  clock_t start, end;

  start = clock();

  Levels2D *l1 = p_FileToLevels(f, (header.quality > 2));
  Levels2D *l2 = p_FileToLevels(f, (header.quality > 2));
  Levels2D *l3 = p_FileToLevels(f, (header.quality > 2));

  end = clock();

  printf("Reading and decoding bitstream time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));

  Image *result;

  if (header.quality > 0)
    result = ImageCreate(0, 0, YCbCr411);
  else
    result = ImageCreate(0, 0, RGB);

  result->width = header.width;
  result->height = header.height;

  start = clock();

  result->channels[0] = Reconstruct2D(l1, header.quality);
  result->channels[1] = Reconstruct2D(l2, header.quality);
  result->channels[2] = Reconstruct2D(l3, header.quality);

  end = clock();

  printf("Reconstruction time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));

  Levels2DDestroy(l1);
  Levels2DDestroy(l2);
  Levels2DDestroy(l3);

  if (header.quality > 0)
  {
    start = clock();
    ImageTransformColourSpace(result, RGB);
    end = clock();
    printf("Colour transformation time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));
  }
  else
  {
    start = clock();
    Signal2DAdd(result->channels[1], result->channels[0]);
    Signal2DAdd(result->channels[2], result->channels[0]);
    end = clock();
    printf("Plane addition time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));
  }

  fclose(f);

  return result;

}

void p_LevelsToFile(Levels2D *l, FILE *f, const bool rle_compression)
{

  int8_t *buf1 = malloc(l->width*l->height*2);
  int buf1_size = 0;

  int8_t *buf2 = calloc(l->width*l->height*2, sizeof(int8_t));
  int buf2_size = 0;

  int32_t *overflow_buf = malloc(l->width*l->height*sizeof(int32_t));
  int overflow_buf_size = 0;

  int i, j;
  for (i = 0; i < l->level_count; ++i)
  {

    for (j = 0; j < l->levels[i]->lh->width*l->levels[i]->lh->height; ++j)
      buf1[buf1_size++] = pack32_8(l->levels[i]->lh->data[l->levels[i]->lh->data_pos++], overflow_buf, &overflow_buf_size);

    for (j = 0; j < l->levels[i]->hl->width*l->levels[i]->hl->height; ++j)
      buf1[buf1_size++] = pack32_8(l->levels[i]->hl->data[l->levels[i]->hl->data_pos++], overflow_buf, &overflow_buf_size);

    for (j = 0; j < l->levels[i]->hh->width*l->levels[i]->hh->height; ++j)
      buf1[buf1_size++] = pack32_8(l->levels[i]->hh->data[l->levels[i]->hh->data_pos++], overflow_buf, &overflow_buf_size);

  }

  int8_t *trg_buf;
  int trg_buf_size;
  if (rle_compression)
  {

    rleEncode8(buf1, buf1_size, buf2, &buf2_size, NULL);
    memset(buf1, 0, l->width*l->height*2);
    huffmanEncode(buf2, buf2_size, buf1, &buf1_size, NULL);

    trg_buf = buf1;
    trg_buf_size = buf1_size;

  }
  else
  {

    huffmanEncode(buf1, buf1_size, buf2, &buf2_size, NULL);

    trg_buf = buf2;
    trg_buf_size = buf2_size;

  }

  BILDLevelsHeader levels_header;
  levels_header.root_value = l->root_value;
  levels_header.level_count = l->level_count;
  levels_header.width = l->width;
  levels_header.height = l->height;
  levels_header.coded_size = trg_buf_size;
  levels_header.overflow_buffer_size = overflow_buf_size;

  fwrite(&levels_header, sizeof(byte), sizeof(BILDLevelsHeader), f);

  BILDLevelHeader level_header;
  for (i = 0; i < l->level_count; ++i)
  {
    level_header.ll_width = l->levels[i]->ll_width;
    level_header.ll_height = l->levels[i]->ll_height;
    level_header.lh_width = l->levels[i]->lh->width;
    level_header.lh_height = l->levels[i]->lh->height;
    level_header.hl_width = l->levels[i]->hl->width;
    level_header.hl_height = l->levels[i]->hl->height;
    level_header.hh_width = l->levels[i]->hh->width;
    level_header.hh_height = l->levels[i]->hh->height;
    fwrite(&level_header, sizeof(byte), sizeof(BILDLevelHeader), f);
  }

  fwrite(trg_buf, sizeof(int8_t), trg_buf_size, f);

  if (overflow_buf_size > 0)
    fwrite(overflow_buf, sizeof(int32_t), overflow_buf_size, f);

  free(buf2);
  free(buf1);

  free(overflow_buf);

}

void ImageSaveAsBILDFile(Image *image, const char *filename, const int quality)
{

  clock_t start, end;

  if (quality > 0) {

    start = clock();

    ImageTransformColourSpace(image, YCbCr411);

    end = clock();

    printf("Colour transformation time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));

  }
  else
  {

    start = clock();

    Signal2DSub(image->channels[1], image->channels[0]);
    Signal2DSub(image->channels[2], image->channels[0]);

    end = clock();

    printf("Plane substraction time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));

  }

  start = clock();

  Levels2D *l1 = Decompose2D(image->channels[0], quality);
  Levels2D *l2 = Decompose2D(image->channels[1], quality);
  Levels2D *l3 = Decompose2D(image->channels[2], quality);

  end = clock();

  printf("Decomposition time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));

  FILE *f = fopen(filename, "wb");

  BILDHeader header;
  header.type = BILD_TYPE;
  header.version = VERSION;
  header.width = image->width;
  header.height = image->height;
  header.quality = quality;
  fwrite(&header, sizeof(byte), sizeof(BILDHeader), f);

  start = clock();

  p_LevelsToFile(l1, f, (quality > 2));
  p_LevelsToFile(l2, f, (quality > 2));
  p_LevelsToFile(l3, f, (quality > 2));

  end = clock();

  printf("Creating and saving bitstream time: %f sec\n", (double)(((double)end - (double)start) / CLOCKS_PER_SEC));

  fclose(f);

  Levels2DDestroy(l1);
  Levels2DDestroy(l2);
  Levels2DDestroy(l3);

}

void BILDPrintInformation(const char *filename)
{

  FILE *f = fopen(filename, "rb");

  struct stat st;
  stat(filename, &st);
  if (st.st_size < sizeof(BILDHeader))
  {

    printf("No BILD file.\n");

    fclose(f);
    return;

  }

  BILDHeader header;
  fread(&header, sizeof(byte), sizeof(BILDHeader), f);

  if (header.type != BILD_TYPE)
  {

    printf("No BILD file.\n");

  }
  else
  {

    printf("BILD version............... %d\n", header.version);
    printf("Image size (bytes)........ %d\n", header.width*header.height*3);
    printf("Image dimension (pixels).. %d x %d (width x height)\n", header.width, header.height);
    printf("Quality................... %d\n", header.quality);

  }

  fclose(f);

}

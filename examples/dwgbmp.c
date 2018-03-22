/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * get_bmp.c: get the bmp thumbnail in a dwg file
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include <stdio.h>
#include <string.h>

#include "../src/config.h"
#include <dwg.h>
#include "suffix.c"

int
get_bmp(char *filename)
{
  char *outfile;
  unsigned char *data;
  int success;
  BITCODE_RL size;
  size_t retval;
  FILE *fh;
  Dwg_Data dwg;
  struct _BITMAP_HEADER
  {
    char magic[2];
    long file_size;
    long reserved;
    long offset;
  } bmp_h;

  /* Read dwg data */
  success = dwg_read_file(filename, &dwg);
  if (success != 0) {
    fprintf(stderr, "Unable to read file %s\n", filename);
    return success;
  }

  /* Get DIB bitmap data */
  data = dwg_bmp(&dwg, &size);

  if (!data) {
    fprintf(stderr, "No thumb in dwg file\n");
    return -2;
  }
  if (size < 1) {
    fprintf(stderr, "No thumb data in dwg file\n");
    return -3;
  }

  outfile = suffix (filename, "bmp");
  fh = fopen (outfile, "w");
  if (!fh) {
    fprintf(stderr, "Unable to write BMP file '%s'\n", outfile);
    free (outfile);
    return -4;
  }

  /* Write bmp file header */
  bmp_h.magic[0] = 'B';
  bmp_h.magic[1] = 'M';
  bmp_h.file_size = 14 + size; // file header + DIB data
  bmp_h.reserved = 0;
  bmp_h.offset = 14 + 40 + 4 * 256; // file header + DIB header + color table
  retval = fwrite(&bmp_h.magic[0], 2, sizeof(char), fh);
  if (!retval) {
    perror("writing BMP magic"); return 1;
  }
  retval = fwrite(&bmp_h.file_size, 3, sizeof(long), fh);
  if (!retval) {
    perror("writing BMP file_size"); return 1;
  }

  /* Write data (DIB header + bitmap) */
  retval = fwrite(data, 1, size, fh);
  if (!retval) {
    perror("writing BMP header"); return 1;
  }
  fclose(fh);

  printf ("Success. Written preview image to '%s'\n", outfile);
  free (outfile);
  dwg_free(&dwg);
  return success;
}

int
main (int argc, char *argv[])
{
  REQUIRE_INPUT_FILE_ARG (argc);
  return get_bmp (argv[1]);
}


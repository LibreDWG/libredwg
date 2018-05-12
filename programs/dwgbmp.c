/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009, 2018 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwgbmp.c: get the bmp thumbnail in a dwg file
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dwg.h>
#include "suffix.inc"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

static int usage(void) {
  printf("\nUsage: dwgbmp [-v[0-9]] DWGFILE [BMPFILE]\n");
  return 1;
}
static int opt_version(void) {
  printf("dwgbmp %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwgbmp [OPTION]... DWGFILE [BMPFILE]\n");
  printf("Extract the DWG preview image as BMP.\n");
  printf("Default BMPFILE: DWGFILE with .bmp extension.\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("           --help           display this help and exit\n");
  printf("           --version        output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

static int
get_bmp(char *dwgfile, char *bmpfile, unsigned int opts)
{
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

  dwg.opts = opts;
  /* Read dwg data */
  success = dwg_read_file(dwgfile, &dwg);
  if (success != 0) {
    fprintf(stderr, "Unable to read file %s\n", dwgfile);
    return success;
  }

  /* Get DIB bitmap data */
  data = dwg_bmp(&dwg, &size);

  if (!data) {
    fprintf(stderr, "No thumb in dwg file\n");
    return 0;
  }
  if (size < 1) {
    fprintf(stderr, "Empty thumb data in dwg file\n");
    return -3;
  }

  fh = fopen (bmpfile, "w");
  if (!fh) {
    fprintf(stderr, "Unable to write BMP file '%s'\n", bmpfile);
    free (bmpfile);
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
    free (bmpfile);
    perror("writing BMP file_size"); return 1;
  }

  /* Write data (DIB header + bitmap) */
  retval = fwrite(data, 1, size, fh);
  if (!retval) {
    free (bmpfile);
    perror("writing BMP header"); return 1;
  }
  fclose(fh);

  printf ("Success. Written preview image to '%s'\n", bmpfile);
  free (bmpfile);
  dwg_free(&dwg);
  return success;
}

int
main (int argc, char *argv[])
{
  int i = 1;
  unsigned int opts = 1;
  char *dwgfile, *bmpfile;

  if (argc < 2)
    return usage();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv("LIBREDWG_TRACE", "1", 0);
#endif
  if (argc > 2 &&
      (!strcmp(argv[i], "--verbose") ||
       !strncmp(argv[i], "-v", 2)))
    {
      int num_args = verbosity(argc, argv, i, &opts);
      argc -= num_args;
      i += num_args;
    }
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();
  REQUIRE_INPUT_FILE_ARG (argc);

  dwgfile = argv[i];
  bmpfile = suffix (dwgfile, "bmp");
  return get_bmp (dwgfile, bmpfile, opts);
}


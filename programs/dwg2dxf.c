/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2dxf.c: save a DWG as DXF.
 * optionally as a different version.
 *
 * written by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <dwg.h>
#include "../src/common.h"
#include "../src/bits.h"
#include "../src/logging.h"
#include "suffix.inc"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

#include "../src/out_dxf.h"

int minimal = 0;
int binary = 0;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;

static int usage(void) {
  printf("\nUsage: dwg2dxf [-v[N]] [-as-rNNNN] [-m|--minimal] [-b|--binary] <input_file.dwg> [<output_file.dxf>]\n");
  return 1;
}
static int opt_version(void) {
  printf("dwg2dxf %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwg2dxf [OPTION]... DWGFILE [DXFFILE]\n");
  printf("Converts the DWG to a DXF.\n");
  printf("Default DXFFILE: DWGFILE with .dxf extension.\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  -as-rNNNN                 save as version\n");
  printf("           Valid versions:\n");
  printf("             r12, r14, r2000, r2004, r2007, r2010, r2013\n");
  printf("           Planned versions:\n");
  printf("             r9, r10, r11, r2018\n");
  printf("  -m, --minimal             only $ACADVER, HANDSEED and ENTITIES\n");
  printf("  -b, --binary              save as binary DXF\n");
  printf("      --help                display this help and exit\n");
  printf("      --version             output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

int
main (int argc, char *argv[])
{
  int i = 1;
  unsigned int opts = 1; //loglevel 1
  int error;
  Dwg_Data dwg;
  char* filename_in;
  const char *version = NULL;
  char* filename_out = NULL;
  Dwg_Version_Type dwg_version;
  Bit_Chain dat;

  // check args
  if (argc < 2)
    return usage();
  memset(&dwg, 0, sizeof(Dwg_Data));
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv("LIBREDWG_TRACE", "1", 0);
#endif

  if (argc > 2 &&
      (!strcmp(argv[i], "--verbose") ||
       !strncmp(argv[i], "-v", 2)))
    {
      int num_args = verbosity(argc, argv, i, &opts);
      dwg.opts = opts;
      argc -= num_args;
      i += num_args;
    }
  if (argc > 2 && !strncmp(argv[i], "-as-r", 5))
    {
      const char *opt = argv[i];
      dwg_version = dwg_version_as(&opt[4]);
      if (dwg_version == R_INVALID)
        {
          fprintf(stderr, "Invalid version %s\n", opt);
          return usage();
        }
      version = &opt[4];
      argc--;
      i++;
    }
  if (argc > 2 && (!strcmp(argv[i], "-m") || !strcmp(argv[i], "--minimal")))
    {
      minimal = 1;
      argc--;
      i++;
    }
  if (argc > 2 && (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--binary")))
    {
      binary = 1;
      argc--;
      i++;
    }
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();

  filename_in = argv[i];
  if (argc > 2)
    filename_out = argv[i+1];
  else
    filename_out = suffix (filename_in, "dxf");
  
  if (strcmp(filename_in, filename_out) == 0)
    {
      if (filename_out != argv[2])
        free (filename_out);
      return usage();
    }

  dwg.opts = opts;
  fprintf(stderr, "Reading DWG file %s\n", filename_in);
  error = dwg_read_file(filename_in, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    fprintf(stderr, "READ ERROR 0x%x\n", error);

  printf("Writing DXF file %s", filename_out);
  if (version)
    {
      printf(" as %s\n", version);
      if (dwg.header.from_version != dwg.header.version)
        dwg.header.from_version = dwg.header.version;
      //else keep from_version = 0
      dwg.header.version = dwg_version;
    }
  else
    {
      printf("\n");
    }
  dat.version = dwg.header.version;
  dat.from_version = dwg.header.from_version;

  //fprintf(stderr, "WARNING: BLOCKS missing\n");
  if (minimal)
    dwg.opts |= 0x10;
  if (binary) {
    dat.fh = fopen(filename_out, "wb");
    if (!dat.fh)
      fprintf(stderr, "WRITE ERROR %s\n", filename_out);
    else
      error = dwg_write_dxfb(&dat, &dwg);
  } else {
    dat.fh = fopen(filename_out, "w");
    if (!dat.fh)
      fprintf(stderr, "WRITE ERROR %s\n", filename_out);
    else
      error = dwg_write_dxf(&dat, &dwg);
  }
  if (error >= DWG_ERR_CRITICAL)
    fprintf(stderr, "WRITE ERROR\n");
  if (dat.fh)
    fclose(dat.fh);

  if (filename_out != argv[2])
    free (filename_out);
  // forget about valgrind. really huge DWG's need endlessly here.
  //dwg_free(&dwg);
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}

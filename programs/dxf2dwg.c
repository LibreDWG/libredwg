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
 * dxf2dwg.c: save a DXF as DWG. Detect ascii/binary, minimal/full.
 * Optionally as a different version. WIP
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

int minimal = 0;
int binary = 0;
char buf[4096];
/* the current version per spec block */
static unsigned int cur_ver = 0;

static int usage(void) {
  printf("\nUsage: dxf2dwg [-v[N]] [-as-rNNNN] [-m|--minimal] <input_file.dxf> [<output_file.dwg>]\n");
  return 1;
}
static int opt_version(void) {
  printf("dxf2dwg %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dxf2dwg [OPTION]... DXFFILE [DWGFILE]\n");
  printf("Converts the DXF to a DWG. Accepts ascii and binary DXF.\n");
  printf("Default DWGFILE: DXFFILE with .dwg extension.\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  -as-rNNNN                 save as version\n");
  printf("           Valid versions:\n");
  printf("             r12, r14, r2000\n");
  printf("           Planned versions:\n");
  printf("             r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf("       --help               display this help and exit\n");
  printf("       --version            output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

//TODO read code/name pair from fh until found, and store in obj

int
main (int argc, char *argv[])
{
  int i = 1;
  int error;
  unsigned int opts = 1; //loglevel
  Dwg_Data dwg;
  char* filename_in;
  const char *version = NULL;
  char* filename_out = NULL;
  Dwg_Version_Type dwg_version = R_INVALID;

  // check args
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
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();

  filename_in = argv[i];
  if (argc > 2)
    filename_out = argv[2];
  else
    filename_out = suffix (filename_in, "dwg");
  
  if (strcmp(filename_in, filename_out) == 0) {
    if (filename_out != argv[2])
      free (filename_out);
    return usage();
  }

  printf("Reading DXF file %s\n", filename_in);
  printf("TODO: reading DXF not yet done\n");
  dwg.opts = opts;
  error = dxf_read_file(filename_in, &dwg);
  if (error)
    {
      printf("READ ERROR\n");
      if (filename_out != argv[2])
        free (filename_out);
      dwg_free(&dwg);
      exit(error);
    }
  
  printf("Writing DWG file %s", filename_out);
  if (version) {
    printf(" as %s\n", version);
    if (dwg.header.from_version != dwg.header.version)
      dwg.header.from_version = dwg.header.version;
    //else keep from_version = 0
    dwg.header.version = dwg_version;
  } else {
    printf("\n");
  }
#ifdef USE_WRITE
  error = dwg_write_file(filename_out, &dwg);
#else
  error = 1;
# error no write support
#endif  
  if (error)
      printf("WRITE ERROR\n");

  if (filename_out != argv[2])
    free (filename_out);
  dwg_free(&dwg);
  return error;
}

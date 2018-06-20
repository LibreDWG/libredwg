/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010, 2018 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwgrewrite.c: load a DWG file and rewrite it,
 * optionally as a different version.
 *
 * written by Anderson Pierre Cardoso
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dwg.h>
#include "../src/common.h"
#include "suffix.inc"
static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "common.inc"

static int usage(void) {
  printf("\nUsage: dwgrewrite [-v[N]] [-as-rNNNN] <dwg_input_file.dwg> [<dwg_output_file.dwg>]\n");
  return 1;
}
static int opt_version(void) {
  printf("dwgrewrite %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwgrewrite [OPTION]... INFILE [OUTFILE]\n");
  printf("Rewrites the DWG as another DWG.\n");
  printf("Default OUTFILE: INFILE with <-rewrite.dwg> appended.\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("  -as-rNNNN                 save as version\n");
  printf("           Valid versions:\n");
  printf("             r12, r14, r2000\n");
  printf("           Planned versions:\n");
  printf("             r9, r10, r11, r2004, r2007, r2010, r2013, r2018\n");
  printf("           --help           display this help and exit\n");
  printf("           --version        output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

int
main (int argc, char *argv[])
{
  int error;
  int i = 1;
  unsigned int opts = 1; //loglevel 1
  Dwg_Data dwg;
  char* filename_in;
  const char *version = NULL;
  char* filename_out = NULL;
  Dwg_Version_Type dwg_version;
  BITCODE_BL num_objects;

  // check args
  if (argc < 2)
    return usage();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv("LIBREDWG_TRACE", "1", 0);
#endif

  memset(&dwg, 0, sizeof(Dwg_Data));
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
          fprintf(stderr, "Invalid version %s\n", argv[1]);
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
  if (argc > i+1)
    filename_out = argv[i+1];
  else
    filename_out = suffix (filename_in, "-rewrite.dwg");

  if (!filename_out || !strcmp(filename_in, filename_out)) {
    if (filename_out != argv[2])
      free (filename_out);
    return usage();
  }

  /*
   * some very simple testing
   */
  printf("Reading DWG file %s\n", filename_in);
  error = dwg_read_file(filename_in, &dwg);
  if (error >= DWG_ERR_CRITICAL)
    fprintf(stderr, "READ ERROR 0x%x\n", error);
  num_objects = dwg.num_objects;
  if (!num_objects) {
    printf("Read 0 objects\n");
    if (error >= DWG_ERR_CRITICAL)
      return error;
  }

  if (opts)
    printf("\n");
  printf("Writing DWG file %s", filename_out);
#ifndef USE_WRITE
  error = 1;
#else
  if (version) { // forced -as-rXXX
    printf(" as %s\n", version);
    if (dwg.header.from_version != dwg.header.version)
      dwg.header.from_version = dwg.header.version;
    //else keep from_version
    dwg.header.version = dwg_version;
  } else if (dwg.header.version < R_13 || dwg.header.version > R_2000) {
    // we cannot yet write pre-r13 or 2004+
    printf(" as r2000\n");
    dwg.header.version = R_2000;
  } else {
    printf("\n");
  }
  error = dwg_write_file(filename_out, &dwg);
#endif
  if (error >= DWG_ERR_CRITICAL) {
    printf("WRITE ERROR 0x%x\n", error);
    return error;
  }
  dwg_free(&dwg); // this is slow, but on needed on low memory systems

#ifdef USE_WRITE
  // try to read again
  if (opts)
    printf("\n");
  printf("Re-reading created file %s\n", filename_out);
  error = dwg_read_file(filename_out, &dwg);
  if (error >= DWG_ERR_CRITICAL)
      printf("re-READ ERROR 0x%x\n", error);
  if (num_objects && (num_objects != dwg.num_objects))
    printf("re-READ num_objects: %lu, should be %lu\n",
           (unsigned long)dwg.num_objects, (unsigned long)num_objects);
#endif
  //if (filename_out != argv[2])
  //  free (filename_out);
  return error >= DWG_ERR_CRITICAL ? error : 0;
}

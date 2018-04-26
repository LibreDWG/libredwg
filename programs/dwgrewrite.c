/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010 Free Software Foundation, Inc.                        */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/config.h"
#include <dwg.h>
#include "../src/common.h"
#include "suffix.c"

int usage(void);

int usage(void) {
  printf("\nUsage:\trewrite [-as-rxxxx] <dwg_input_file.dwg> [<dwg_output_file.dwg>]\n");
  return 1;
}

int
main (int argc, char *argv[])
{
  int error;
  Dwg_Data dwg;
  char* filename_in;
  const char *version = NULL;
  char* filename_out = NULL;
  Dwg_Version_Type dwg_version;
  long unsigned int num_objects;

  // check args
  if (argc < 2)
    return usage();
  filename_in = argv[1];
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv("LIBREDWG_TRACE", "1", 0);
#endif

  if (argc > 2 && !strncmp(argv[1], "-as-r", 5))
    {
      const char *opt = argv[1];
      dwg_version = dwg_version_as(&opt[4]);
      if (dwg_version == R_INVALID)
        {
          fprintf(stderr, "Invalid option %s\n", argv[1]);
          return usage();
        }
      version = &opt[4];
      filename_in = argv[2];
      argc--;
    }
  if (argc > 2)
    filename_out = argv[2];
  else
    filename_out = suffix (filename_in, "-rewrite.dwg");
  
  if (strcmp(filename_in, filename_out) == 0) {
    if (filename_out != argv[2])
      free (filename_out);
    return usage();
  }

  /*
   * some very simple testing
   */
  printf("Reading DWG file %s\n", filename_in);
  error = dwg_read_file(filename_in, &dwg);
  if (error)
      printf("READ ERROR\n");
  num_objects = dwg.num_objects;
  if (!num_objects) {
    printf("Read 0 objects\n");
    if (error)
      return error;
  }

  printf("Writing DWG file %s", filename_out);
#ifndef USE_WRITE
  error = 1;
#else
  if (version) {
    printf(" as %s\n", version);
    if (dwg.header.from_version != dwg.header.version)
      dwg.header.from_version = dwg.header.version;
    //else keep from_version
    dwg.header.version = dwg_version;
  } else {
    printf("\n");
  }
  error = dwg_write_file(filename_out, &dwg);
#endif
  if (error) {
      printf("WRITE ERROR\n");
  }
  dwg_free(&dwg);

#ifdef USE_WRITE
  // try to read again
  printf("Re-reading created file %s\n", filename_out);
  error = dwg_read_file(filename_out, &dwg);
  if (error)
      printf("re-READ ERROR\n");
  if (num_objects && (num_objects != dwg.num_objects))
    printf("re-READ num_objects: %lu, should be %lu\n", dwg.num_objects, num_objects);
#endif
  if (filename_out != argv[2])
    free (filename_out);
  return error;
}

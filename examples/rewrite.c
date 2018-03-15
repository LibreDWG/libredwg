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
 * rewrite.c: load a DWG file and rewrite it,
 * optionally as a different version.
 *
 * written by Anderson Pierre Cardoso
 * modified by Reini Urban
 */

#include <stdio.h>
#include <string.h>

#include "../src/config.h"
#include <dwg.h>
#include "../src/common.h"
#include "suffix.c"

int usage() {
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

  // check args
  if (argc < 2)
    return usage();
  filename_in = argv[1];

  if (argc > 2 && !strncmp(argv[1], "-as-r", 5))
    {
      const char *opt = argv[1];
      if (!strcmp(opt, "-as-r13") ||
          !strcmp(opt, "-as-r14") ||
          !strcmp(opt, "-as-r2000") ||
          !strcmp(opt, "-as-r2004") ||
          !strcmp(opt, "-as-r2007") ||
          !strcmp(opt, "-as-r2010") ||
          !strcmp(opt, "-as-r2013") ||
          !strcmp(opt, "-as-r2018"))
        {
          version = &opt[4];
          if (!strcmp(version, "r13"))
            dwg_version = R_13;
          else if (!strcmp(version, "r14"))
            dwg_version = R_14;
          else if (!strcmp(version, "r2000"))
            dwg_version = R_2000;
          else if (!strcmp(version, "r2004"))
            dwg_version = R_2004;
          else if (!strcmp(version, "r2007"))
            dwg_version = R_2007;
          else if (!strcmp(version, "r2010"))
            dwg_version = R_2010;
          else if (!strcmp(version, "r2013"))
            dwg_version = R_2013;
          else if (!strcmp(version, "r2018"))
            dwg_version = R_2018;
          filename_in = argv[2];
          argc--;
        }
      else
        {
          fprintf(stderr, "Invalid option %s\n", opt);
          return usage();
        }
    }
  if (argc > 2)
    filename_out = argv[2];
  else
    {
      filename_out = strdup(filename_in);
      filename_out = strcat(filename_out, "-rewrite.dwg\0");
    }
  
  if (strcmp(filename_in, filename_out) == 0)
    return usage();

  /*
   * some very simple testing
   */
  // reads the file
  printf("Reading DWG file %s\n", filename_in);
  error = dwg_read_file(filename_in, &dwg);

  if (error)
      printf("READ ERROR\n");
  else
      printf("READ SUCCESS\n");

  // rewrite it
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
  error = dwg_write_file(filename_out, &dwg);

  if (error)
      printf("WRITE ERROR\n");
  else
      printf("WRITE SUCCESS\n");
  
  dwg_free(&dwg);

  // try to read again
  printf("Re-reading created file %s\n", filename_out);
  error = dwg_read_file(filename_out, &dwg);
  if (error)
      printf("re-READ ERROR\n");
  else
      printf("re-READ SUCCESS\n");

  return error;
}

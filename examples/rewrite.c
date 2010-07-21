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
 * Rewrite.c: load a DWG file and rewrite it
 * written by Anderson Pierre Cardoso
 */

#include <stdio.h>
#include <dwg.h>
#include "suffix.c"
#include "../src/common.h"

// @deprecated
/*int
load_and_rewrite_dwg(char *filenameIn, char *filenameOut)
{
  unsigned int i;
  int error;
  Dwg_Data dwg;

  error = dwg_read_file(filenameIn, &dwg);
  if (!error){

	  error = dwg_write_file(filenameOut, &dwg);
	  if (error) printf("\nERROR!\n");
	  else printf("\nSUCCESS!!\n");
  }
  else {
    printf("\nCould not read the file!");
  }
 // dwg_free(&dwg);
  return error;
}*/

int
main (int argc, char *argv[])
{
  REQUIRE_INPUT_FILE_ARG (argc);
  //load_and_rewrite_dwg (argv[1], argv[2]);

  int error;
  Dwg_Data dwg_struct;
  char* filename_in = argv[1];
  char* filename_out = argv[2];

  error = dwg_read_file(filename_in, &dwg_struct);

  //dwg_free(&dwg_struct);

  if (error)
    {
      printf("\nREAD ERROR!\n\n");
    }
  else
    {
      printf("\nREAD SUCCESS!\n\n");
    }

  error = dwg_write_file(filename_out, &dwg_struct);

  if (error)
    {
      printf("\nWRITE ERROR!\n\n");
    }
  else
    {
      printf("\nWRITE SUCCESS!\n\n");
    }

  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error ? 1 : 0;
 // dwg_free(&dwg);
 
  //return 0;
}

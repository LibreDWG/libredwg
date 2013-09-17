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
#include <string.h>


int
main (int argc, char *argv[])
{
 // check entry
 if (argc <= 2 || strcmp(argv[1],argv[2]) == 0 )
   {
    printf("\n\nUsage: \t ./rewrite <dwg_input_file>.dwg <dwg_output_file>.dwg \n \t  Do not subscribe the input file! Avoid subscribe the output file either (it will probably cause an error!\n\n");
    return 1;
  }
  
  
  int error;
  Dwg_Data dwg_struct;
  char* filename_in = argv[1];
  char* filename_out = argv[2];
  /*
   * some very simple testing
   */

  // reads the file
  printf("\n ===== \n Reading original file \n =====\n");
  error = dwg_read_file(filename_in, &dwg_struct);

  if (error)
    {
      printf("\nREAD ERROR!\n\n");
    }
  else
    {
      printf("\nREAD SUCCESS!\n\n");
    }

  // rewrite it
  printf("\n ===== \n Writing new file \n =====\n");
  error = dwg_write_file(filename_out, &dwg_struct);

  if (error)
    {
      printf("\nWRITE ERROR!\n\n");
    }
  else
    {
      printf("\nWRITE SUCCESS!\n\n");
    }
  
  dwg_free(&dwg_struct);

  // try to read again
  printf("\n ===== \n Reading created file \n =====\n");
  error = dwg_read_file(filename_out, &dwg_struct);

  if (error)
    {
      printf("\nre-READ ERROR!\n\n");
    }
  else
    {
      printf("\nre-READ SUCCESS!\n\n");
    }


  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error ? 1 : 0;
 // dwg_free(&dwg);
 
  //return 0;
}

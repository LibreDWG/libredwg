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
 * test.c: reading and loading a DWG file to memory
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Thien-Thi Nguyen
 */

#include <stdio.h>
#include <dwg.h>
#include "suffix.c"

int
test_dwg_c(char *filename);

int
main(int argc, char *argv[])
{
  REQUIRE_INPUT_FILE_ARG (argc);
  return test_dwg_c (argv[1]);
}

int
test_dwg_c(char *filename)
{
  int error;
  Dwg_Data dwg_struct;

  error = dwg_read_file(filename, &dwg_struct);

  dwg_free(&dwg_struct);

  if (error)
    {
      printf("\nERROR!\n\n");
    }
  else
    {
      printf("\nSUCCESS!\n\n");
    }

  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error ? 1 : 0;
}


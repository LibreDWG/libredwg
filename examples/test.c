/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Felipe Corrêa da Silva Sanches <juca@members.fsf.org> */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#include <stdio.h>
#include <dwg.h>
#include "suffix.c"

int
test_dwg_c(char *filename);

int
main(int argc, char *argv[])
{
  REQUIRE_INPUT_FILE_ARG (argc);
  test_dwg_c (argv[1]);
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


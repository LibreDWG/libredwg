/*****************************************************************************/
/*  LibDWG - Free DWG read-only library                                      */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bits.h"
#include "dwg.h"

#define CXEN_LONGO (80)

Bit_Cxeno dat;
Bit_Cxeno dat_2;

int test_dwg_c (char *filename);

int
main (int argc, char *argv[])
{
	if (argc > 1)
		return (test_dwg_c (argv[1]));
	else
		return (test_dwg_c (NULL));
}

#define FILENAME "example"
int
test_dwg_c (char *filename)
{
	int error = 0;
	Dwg_Structure dwg_struct;

	/* Komenci testojn
	 */
	puts (" ---------------------------------------> Oni testas: \"dwg.c\"");

	if (filename)
		error = error || dwg_read_file (filename, &dwg_struct);
	else
		error = error || dwg_read_file (FILENAME ".dwg", &dwg_struct);

	if (!error)
	{
		dwg_print (&dwg_struct);
		/*
		   unlink ("new_result.dwg");
		   error = error || dwg_write_file ("new_result.dwg", &dwg_struct);
		 */
	}

	if (error)
		puts (" \"dwg.c\" ==> Error...");
	else
	{
		puts (" \"dwg.c\" ==> SUCCESS!");
	}

	return error;
}

/**********************************************************************************/
/*  LibreDWG - Free DWG read-only library                                         */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                       */
/*  Copyright (C) 2009 Felipe Corrêa da Silva Sanches <felipe.sanches@gmail.com>  */
/*  This library is free software, licensed under the terms of the GNU            */
/*  General Public License as published by the Free Software Foundation,          */
/*  either version 3 of the License, or (at your option) any later version.       */
/*  You should have received a copy of the GNU General Public License             */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.         */
/**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bits.h"
#include "dwg.h"

Bit_Chain dat;
Bit_Chain dat_2;

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
	int error;
	Dwg_Structure dwg_struct;

	/* Beginning tests */
	puts (" ---------------------------------------> Test one: \"dwg.c\"");

	if (filename)
		error = dwg_read_file (filename, &dwg_struct);
	else
		error = dwg_read_file (FILENAME ".dwg", &dwg_struct);

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

/**********************************************************************************/
/*  LibreDWG - Free DWG read-only library                                         */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                       */
/*  Copyright (C) 2009 Felipe Corrêa da Silva Sanches <felipe.sanches@gmail.com>  */
/*  This library is free software, licensed under the terms of the GNU            */
/*  General Public License as published by the Free Software Foundation,          */
/*  either versionn 3 of the License, or (at your option) any later versionn.       */
/*  You should have received a copy of the GNU General Public License             */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.         */
/**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bits.h"
#include "common.h"
#include "dwg.h"

int test_dwg_c (char *filename);
void test_map_R2000();

int
main (int argc, char *argv[])
{
    printf("Starting to test the type-map for DWG version R2000.\n");
    test_map_R2000();
    printf("End of type-map test.\n");

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

    dwg_free(&dwg_struct);

	if (error)
		puts (" \"dwg.c\" ==> Error...");
	else
	{
		puts (" \"dwg.c\" ==> SUCCESS!");
	}

	return error;
}

void test_map_R2000(){
    int i;
    static int map_R2000[] = {
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_T,
                    DWG_DT_T,
                    DWG_DT_T,
                    DWG_DT_T,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_H,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_T,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_BL,
                    DWG_DT_CMC,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_2RD,
                    DWG_DT_2RD,
                    DWG_DT_BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_BS,
                    DWG_DT_H,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_2RD,
                    DWG_DT_2RD,
                    DWG_DT_BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_BS,
                    DWG_DT_H,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_3BD,
                    DWG_DT_T,
                    DWG_DT_T,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_BD,
                    DWG_DT_B,
                    DWG_DT_BS,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_CMC,
                    DWG_DT_CMC,
                    DWG_DT_CMC,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_B,
                    DWG_DT_B,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_B,
                    DWG_DT_BS,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_T,
                    DWG_DT_T,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_BL,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_H,
                    DWG_DT_T,
                    DWG_DT_T,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_H,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS,
                    DWG_DT_BS
            };
   
    
    for (i=0;i<233;i++){
        if (map_R2000[i] != dwg_var_map(R_2000, i)) {
            fprintf(stderr, "map_R2000[%d]=%d\t\tdwg_var_map(R_2000, %d)=%d\n", i, map_R2000[i], i, dwg_var_map(R_2000, i));
            fprintf(stderr, "Did not pass the test.\nFailed at index=%d\n\n", i);
            return;
        }
    }
}

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
#include "dwg.h"

int test_SVG (char *filename);
void output_SVG(Dwg_Structure* dwg_struct);

int
main (int argc, char *argv[])
{
	if (argc > 1)
		return (test_SVG (argv[1]));
	else
		return (test_SVG (NULL));
}

#define FILENAME "example"
int
test_SVG (char *filename)
{
	int error;
	Dwg_Structure dwg_struct;

	if (filename)
		error = dwg_read_file (filename, &dwg_struct);
	else
		error = dwg_read_file (FILENAME ".dwg", &dwg_struct);

	if (!error)
	{
		output_SVG(&dwg_struct);
	}

	return error;
}


void output_SVG(Dwg_Structure* dwg_struct){
	int i;
	Dwg_Object *obj;

	printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\
<svg\
   xmlns:svg=\"http://www.w3.org/2000/svg\"\
   xmlns=\"http://www.w3.org/2000/svg\"\
   version=\"1.0\"\
   width=\"210mm\"\
   height=\"297mm\"\
>");

	for (i = 0; i < dwg_struct->num_objects; i++)
	{
		obj = &dwg_struct->object[i];


		if (obj->type == DWG_TYPE_LINE){
			Dwg_Estajxo_LINE* line;
			line = obj->tio.estajxo->tio.LINE;
			printf("<path d=\"M %f,%f L %f,%f\" />\n", line->x0, line->y0, line->x1, line->y1);		
		}
	}
	printf("</svg>");
}

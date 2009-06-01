/**************************************************************************/
/*  LibDWG - Free DWG read-only library                                   */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.               */
/*                                                                        */
/*  This program is free software, licensed under the terms of the GNU    */
/*  General Public License as published by the Free Software Foundation,  */
/*  which is also included in this package, in a file named "COPYING".    */
/**************************************************************************/

/* Simple functions to show how to read lines, circles and texts
 */

#include <dwg.h>

#define INPUT_FILE "sample.dwg"
void add_line (double x1, double y1, double x2, double y2)
{
	// Make something with that
}

void add_circle (double x, double y, double R)
{
	// Make something with that
}

void add_text (double x, double y, char *txt)
{
	// Make something with that
}

int load_dwg (char *filename)
{
	unsigned int i;
	int success;
	Dwg_Structure dwg;

	dwg.num_objects = 0;
	success = dwg_read_file (filename, &dwg);
	for (i = 0; i < dwg.num_objects; i++)
	{
		Dwg_Entity_LINE *line;
		Dwg_Entity_CIRCLE *circle;
		Dwg_Entity_TEXT *text;

		switch (dwg.object[i].type)
		{
		case DWG_TYPE_LINE:
			line = dwg.object[i].tio.entity->tio.LINE;
			add_line (line->x0, line->x1, line->y0, line->y1);
			break;
		case DWG_TYPE_CIRCLE:
			circle = dwg.object[i].tio.entity->tio.CIRCLE;
			add_circle (circle->x0, circle->y0, circle->radius);
			break;
		case DWG_TYPE_TEXT:
			text = dwg.object[i].tio.entity->tio.TEXT;
			add_text (text->x0, text->y0, text->text);
			break;
		}
	}
	return success;
}

int main ()
{
    load_dwg (INPUT_FILE);
	return 0;
}

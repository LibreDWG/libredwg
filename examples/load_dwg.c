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
	int i;
	int success;
	Dwg_Structure dwg;

	dwg.objekto_kiom = 0;
	success = dwg_read_file (filename, &dwg);
	for (i = 0; i < dwg.objekto_kiom; i++)
	{
		Dvg_Estajxo_LINE *line;
		Dvg_Estajxo_CIRCLE *circle;
		Dvg_Estajxo_TEXT *text;

		switch (dwg.objekto[i].tipo)
		{
		case DVG_OT_LINE:
			line = dwg.objekto[i].tio.estajxo->tio.LINE;
			add_line (line->x0, line->x1, line->y0, line->y1);
			break;
		case DVG_OT_CIRCLE:
			circle = dwg.objekto[i].tio.estajxo->tio.CIRCLE;
			add_circle (circle->x0, circle->y0, circle->radiuso);
			break;
		case DVG_OT_TEXT:
			text = dwg.objekto[i].tio.estajxo->tio.TEXT;
			add_text (text->x0, text->y0, text->teksto);
			break;
		}
	}
	return success;
}

int main ()
{
	load_dwg ("sample.dwg");
	return 0;
}

/**************************************************************************/
/*  LibreDWG - Free DWG read-only library                                 */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.               */
/*  Copyright (C) 2009       Felipe Corrêa da Silva Sanches               */
/*                                                                        */
/*  This program is free software, licensed under the terms of the GNU    */
/*  General Public License as published by the Free Software Foundation,  */
/*  which is also included in this package, in a file named "COPYING".    */
/**************************************************************************/

/* Little program to create a PostScript file from a DWG one
 */

#include <stdio.h>
#include <dwg.h>
#include <libps/pslib.h>

#define INPUT_FILE "sample.dwg"
#define OUTPUT_FILE "sample.ps"

void
create_postscript (Dwg_Structure *dwg, char *output)
{
	float dx;
	float dy;
	float scale_x;
	float scale_y;
	float scale;
	long unsigned i;
	FILE *fh;
	PSDoc *ps;

	/* Initialization
	 */
	PS_boot ();
	ps = PS_new ();
	if (PS_open_file (ps, output) < 0)
	{
		puts ("Cannot write PostScript file");
		return;
	}

	PS_set_info (ps, "Creator", "dwg_ps");
	PS_set_info (ps, "Author", "LibreDWG example");
	PS_set_info (ps, "Title", "DWG to Postscript example");
	PS_set_info (ps, "Keywords", "dwg, postscript, conversion, CAD, plot");

	/* First page: Model Space (?)
	 */
	dx = (dwg_model_page_x_max(dwg) - dwg_model_page_x_min(dwg));
	dy = (dwg_model_page_y_max(dwg) - dwg_model_page_y_min (dwg));
	scale_x = dx / (dwg_model_x_max(dwg) - dwg_model_x_min(dwg));
	scale_y = dy / (dwg_model_y_max(dwg) - dwg_model_y_min(dwg));
    scale = 25.4/72; // pt:mm
	PS_begin_page (ps, dx / scale, dy / scale);
	scale *= (scale_x > scale_y ? scale_x : scale_y);
	PS_scale (ps, scale, scale);
	PS_translate (ps, -dwg_model_x_min(dwg), -dwg_model_y_min(dwg));
	//printf ("%f (%f, %f)\n", scale, scale_x, scale_y);

	/* Mark the origin with a crossed circle
	 */
#	define H 2000
	PS_circle (ps, 0, 0, H);
	PS_moveto (ps, 0, H);
	PS_lineto (ps, 0, -H);
	PS_moveto (ps, -H, 0);
	PS_lineto (ps, H, 0);
	PS_stroke (ps);

	/* Iterate all entities
	 */
	Dwg_Object *obj;
	for (i = 0; i < dwg->num_objects; i++)
	{
    	obj = &dwg->object[i];
		if (obj->supertype = DWG_SUPERTYPE_UNKNOWN) // unknown
			continue;
		if (obj->type = DWG_SUPERTYPE_OBJECT) // not entity
			continue;
		if (obj->tio.entity->regime == 0) // belongs to block
			continue;
		if (obj->type == DWG_TYPE_LINE){
    		Dwg_Entity_LINE* line;
			line = obj->tio.entity->tio.LINE;
			PS_moveto (ps, line->x0, line->y0);
			PS_lineto (ps, line->x1, line->y1);
			PS_stroke (ps);
		}
	}

	/* End Model Space */
	PS_end_page(ps);

	PS_close (ps);
	PS_delete (ps);
	PS_shutdown ();
}

main (int argc, char *argv[])
{
	int success;
	Dwg_Structure dwg;

	if (argc > 1)
		success = dwg_read_file (argv[1], &dwg);
	else
		success = dwg_read_file (INPUT_FILE, &dwg);
	if (success)
	{
		puts ("Not able to read dwg file!");
		return -1;
	}

	create_postscript (&dwg, OUTPUT_FILE);
    dwg_free(&dwg);
    
	puts ("Success! See the '" OUTPUT_FILE "'file");
	return 0;
}


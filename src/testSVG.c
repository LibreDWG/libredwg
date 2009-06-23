/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*  http://code.google.com/p/libredwg/                                       */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Felipe Sanches <jucablues@users.sourceforge.net>      */
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
#include <math.h>

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

    dwg_free(&dwg_struct);
	return error;
}

#define MAXVALUE 100000
void output_SVG(Dwg_Structure* dwg_struct){
	int i;
	Dwg_Object *obj;

	double page_width = dwg_model_x_max(dwg_struct) - dwg_model_x_min(dwg_struct);
	double page_height = dwg_model_y_max(dwg_struct) - dwg_model_y_min(dwg_struct);
	printf(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<svg\n"
        "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
        "   xmlns=\"http://www.w3.org/2000/svg\"\n"
	"   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
        "   version=\"1.1\"\n"
        "   width=\"%f\"\n"
        "   height=\"%f\"\n"
	">\n", page_width, page_height
    );

	int lines = 0, bad_lines = 0;
	int circles = 0, bad_circles = 0;
	int arcs = 0, bad_arcs = 0;
	int texts = 0, bad_texts = 0;

	for (i = 0; i < dwg_struct->num_objects; i++)
	{
		obj = &dwg_struct->object[i];


		if (obj->type == DWG_TYPE_BLOCK_HEADER){
			printf("\t<g id=\"dwg-handle-%d\" >\n", obj->handle);
		}


		if (obj->type == DWG_TYPE_ENDBLK){
			printf("\t</g>\n");
		}


		if (obj->type == DWG_TYPE_INSERT){
			Dwg_Entity_INSERT* insert;
			insert = obj->tio.entity->tio.INSERT;
			printf("\t<use x=\"%f\" y=\"%f\" xlink:href=\"#dwg-handle-%d\" />\n", insert->x0, page_height - insert->y0, insert->block_header_handle.value);
		}

		if (obj->type == DWG_TYPE_LINE){
			lines++;
			Dwg_Entity_LINE* line;
			line = obj->tio.entity->tio.LINE;
			if ((line->x0 < MAXVALUE) && (line->x0 > -MAXVALUE) &&
			    (line->y0 < MAXVALUE) && (line->y0 > -MAXVALUE) &&
			    (line->x1 < MAXVALUE) && (line->x1 > -MAXVALUE) &&
			    (line->y1 < MAXVALUE) && (line->y1 > -MAXVALUE)){
				printf("\t<path id=\"dwg-%d\" d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n", i, line->x0, page_height - line->y0, line->x1, page_height - line->y1);
			} else {
				bad_lines++;
			}
		}


		if (obj->type == DWG_TYPE_LINE){
			lines++;
			Dwg_Entity_LINE* line;
			line = obj->tio.entity->tio.LINE;
			if ((line->x0 < MAXVALUE) && (line->x0 > -MAXVALUE) &&
			    (line->y0 < MAXVALUE) && (line->y0 > -MAXVALUE) &&
			    (line->x1 < MAXVALUE) && (line->x1 > -MAXVALUE) &&
			    (line->y1 < MAXVALUE) && (line->y1 > -MAXVALUE)){
				printf("\t<path id=\"dwg-%d\" d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n", i, line->x0, page_height - line->y0, line->x1, page_height - line->y1);
			} else {
				bad_lines++;
			}
		}


		if (obj->type == DWG_TYPE_CIRCLE){
			circles++;
			Dwg_Entity_CIRCLE* circle;
			circle = obj->tio.entity->tio.CIRCLE;
			if ((circle->x0 < MAXVALUE) && (circle->x0 > -MAXVALUE) &&
			    (circle->y0 < MAXVALUE) && (circle->y0 > -MAXVALUE) &&
			    (circle->radius < MAXVALUE) && (circle->radius > -MAXVALUE)){
				printf("\t<circle id=\"dwg-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n", i, circle->x0, page_height - circle->y0, circle->radius);
			} else {
				bad_circles++;
			}
		}

		if (obj->type == DWG_TYPE_ARC){
			arcs++;
			Dwg_Entity_ARC* arc;
			arc = obj->tio.entity->tio.ARC;
			if ((arc->x0 < MAXVALUE) && (arc->x0 > -MAXVALUE) &&
			    (arc->y0 < MAXVALUE) && (arc->y0 > -MAXVALUE) &&
			    (arc->radius < MAXVALUE) && (arc->radius > -MAXVALUE)){
				double x_start = arc->x0 + arc->radius * cos(arc->start_angle);
				double y_start = arc->y0 + arc->radius * sin(arc->start_angle);
				double x_end = arc->x0 + arc->radius * cos(arc->end_angle);
				double y_end = arc->y0 + arc->radius * sin(arc->end_angle);
				//Assuming clockwise arcs.
				int large_arc = (arc->end_angle - arc->start_angle < 3.1415) ? 0 : 1;
				printf(	"\t<path id=\"dwg-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n", i, x_start, page_height - y_start, arc->radius, arc->radius, large_arc, x_end, page_height - y_end, 0.1);
			} else {
				bad_arcs++;
			}
		}

		if (obj->type == DWG_TYPE_TEXT){
			texts++;
			Dwg_Entity_TEXT* text;
			text = obj->tio.entity->tio.TEXT;
			if ((text->x0 < MAXVALUE) && (text->x0 > -MAXVALUE) &&
			    (text->y0 < MAXVALUE) && (text->y0 > -MAXVALUE) && text->text
/*TODO: Juca, fix it properly: */ && text->text[0] != '&'
			    ){
				printf("\t<text id=\"dwg-%d\" x=\"%f\" y=\"%f\" font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n", i, text->x0, page_height - text->y0, text->height /* fontsize ? */, text->text);
			} else {
				bad_texts++;
			}
		}


	}
	printf("</svg>\n");
	if (lines>0) fprintf(stderr, "%d lines. %d bad lines (%.2f)\n", lines, bad_lines, 100 * ((double) bad_lines)/lines);
	if (circles>0) fprintf(stderr, "%d circles. %d bad circles (%.2f)\n", circles, bad_circles, 100 * ((double) bad_circles)/circles);
	if (arcs>0) fprintf(stderr, "%d arcs. %d bad arcs (%.2f)\n", arcs, bad_arcs, 100 * ((double) bad_arcs)/arcs);
	if (texts>0) fprintf(stderr, "%d texts. %d bad texts (%.2f)\n", texts, bad_texts, 100 * ((double) bad_texts)/texts);
	fprintf(stderr, "%d bad objects (%.2f)\n", bad_circles + bad_texts + bad_lines + bad_arcs, 100 * ((double) (bad_lines + bad_arcs + bad_circles + bad_lines))/dwg_struct->num_objects);
}

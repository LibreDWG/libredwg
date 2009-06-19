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

	printf(
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<svg\n"
        "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
        "   xmlns=\"http://www.w3.org/2000/svg\"\n"
        "   version=\"1.1\"\n"
        "   width=\"210mm\"\n"
        "   height=\"297mm\"\n"
	">\n"
    );

	int lines = 0, bad_lines = 0;
	int circles = 0, bad_circles = 0;
	int arcs = 0, bad_arcs = 0;
	int texts = 0, bad_texts = 0;

	for (i = 0; i < dwg_struct->num_objects; i++)
	{
		obj = &dwg_struct->object[i];

		if (obj->type == DWG_TYPE_LINE){
			lines++;
			Dwg_Entity_LINE* line;
			line = obj->tio.entity->tio.LINE;
			if ((line->x0 < MAXVALUE) && (line->x0 > -MAXVALUE) &&
			    (line->y0 < MAXVALUE) && (line->y0 > -MAXVALUE) &&
			    (line->x1 < MAXVALUE) && (line->x1 > -MAXVALUE) &&
			    (line->y1 < MAXVALUE) && (line->y1 > -MAXVALUE)){
				printf("\t<path d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n", line->x0, -line->y0, line->x1, -line->y1);
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
				printf("\t<circle cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n", circle->x0, -circle->y0, circle->radius);
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
				double x_start = arc->x0 + arc->radius*cos(arc->start_angle);
				double y_start = arc->y0 + arc->radius*sin(arc->start_angle);
				double x_end = arc->x0 + arc->radius*cos(arc->end_angle);
				double y_end = arc->y0 + arc->radius*sin(arc->end_angle);

				printf(	"\t<path d=\"M %f,%f, a%f,%f 0 %d %d %f,%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n", x_start, -y_start, arc->radius, arc->radius, 1, 1, x_end, -y_end);
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
			    ){
				printf("\t<text x=\"%f\" y=\"%f\" font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n", text->x0, -text->y0, text->height /* fontsize ? */, text->text);
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

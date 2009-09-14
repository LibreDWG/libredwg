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

#include "../src/bits.h"
#include <dwg.h>

int
test_SVG(char *filename);

void
output_SVG(Dwg_Structure* dwg_struct);

int
main(int argc, char *argv[])
{
  if (argc > 1)
    return (test_SVG(argv[1]));
  else
    return (test_SVG(NULL));
}

#define FILENAME "example"

int
test_SVG(char *filename)
{
  int error;
  Dwg_Structure dwg_struct;

  if (filename)
    error = dwg_read_file(filename, &dwg_struct);
  else
    error = dwg_read_file(FILENAME ".dwg", &dwg_struct);

  if (!error)
    {
      output_SVG(&dwg_struct);
    }

  dwg_free(&dwg_struct);
  return error;
}

void
output_SVG(Dwg_Structure* dwg_struct)
{
  unsigned int i;
  Dwg_Object *obj;

  double page_width = dwg_model_x_max(dwg_struct) - dwg_model_x_min(dwg_struct);
  double page_height = dwg_model_y_max(dwg_struct)
      - dwg_model_y_min(dwg_struct);
  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<svg\n"
    "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
    "   version=\"1.1\"\n"
    "   width=\"%f\"\n"
    "   height=\"%f\"\n"
    ">\n", page_width, page_height);

  int lines = 0, arcs = 0, circles = 0, texts = 0;

  for (i = 0; i < dwg_struct->num_objects; i++)
    {
      obj = &dwg_struct->object[i];

      if (!obj)
        {
          fprintf(stderr, "dwg_struct->object[%d] == NULL\n", i);
          continue;
        }

      if (obj->type == DWG_TYPE_BLOCK_HEADER)
        {
          Dwg_Object_BLOCK_HEADER* hdr;
          hdr = obj->tio.object->tio.BLOCK_HEADER;
          //      if (hdr->block_entity->handleref.code == 3){
          printf(
              "\t<g id=\"dwg-handle-%lu\" >\n<!--\n\tBLOCK_HEADER obj->handle: %d.%d.%d\n\treferenced BLOCK: %d.%d.%d -->\n",
              obj->handle.value, obj->handle.code, obj->handle.size,
              obj->handle.value, hdr->block_entity->handleref.code,
              hdr->block_entity->handleref.size,
              hdr->block_entity->handleref.value);
          /*      } else {
           printf("\n\n<g id=\"wrong-handle-%lu\" >\n\t<!-- WRONG BLOCK reference (%d.%d.%lu - code should be 3) in BLOCK_HEADER (%d.%d.%lu) -->\n",
           obj->handle.value,
           hdr->block_entity->handleref.code,
           hdr->block_entity->handleref.size,
           hdr->block_entity->handleref.value,
           obj->handle.code,
           obj->handle.size,
           obj->handle.value);
           }
           */
        }

      if (obj->type == DWG_TYPE_BLOCK)
        {
          //      printf("\t<g id=\"dwg-handle-%lu\" ><!-- BLOCK obj->handle: %d.%d.%d -->\n", obj->handle.value, obj->handle.code, obj->handle.size, obj->handle.value);
        }

      if (obj->type == DWG_TYPE_ENDBLK)
        {
          printf("\t</g>\n");
        }

      if (obj->type == DWG_TYPE_INSERT)
        {
          Dwg_Entity_INSERT* insert;
          insert = obj->tio.entity->tio.INSERT;
          if (insert->block_header->handleref.code == 5)
            {
              printf(
                  "\t<use transform=\"translate(%f %f) rotate(%f) scale(%f %f)\" xlink:href=\"#dwg-handle-%lu\" /><!-- block_header->handleref: %d.%d.%lu -->\n",
                  insert->ins_pt.x, page_height - insert->ins_pt.y, (180.0 / M_PI)
                      * insert->rotation_ang, insert->scale.x, insert->scale.y,
                  insert->block_header->handleref.value,
                  insert->block_header->handleref.code,
                  insert->block_header->handleref.size,
                  insert->block_header->handleref.value);

            }
          else
            {

              printf(
                  "\n\n<!-- WRONG INSERT(%d.%d.%lu): handleref = %d.%d.%lu -->\n",
                  obj->handle.code, obj->handle.size, obj->handle.value,
                  insert->block_header->handleref.code,
                  insert->block_header->handleref.size,
                  insert->block_header->handleref.value);
            }
        }

      if (obj->type == DWG_TYPE_LINE)
        {
          lines++;
          Dwg_Entity_LINE* line;
          line = obj->tio.entity->tio.LINE;
          printf(
              "\t<path id=\"dwg-%d\" d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
              i, line->x0, page_height - line->y0, line->x1, page_height
                  - line->y1);
        }

      if (obj->type == DWG_TYPE_CIRCLE)
        {
          circles++;
          Dwg_Entity_CIRCLE* circle;
          circle = obj->tio.entity->tio.CIRCLE;
          printf(
              "\t<circle id=\"dwg-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
              i, circle->x0, page_height - circle->y0, circle->radius);
        }

      if (obj->type == DWG_TYPE_ARC)
        {
          arcs++;
          Dwg_Entity_ARC* arc;
          arc = obj->tio.entity->tio.ARC;
          double x_start = arc->x0 + arc->radius * cos(arc->start_angle);
          double y_start = arc->y0 + arc->radius * sin(arc->start_angle);
          double x_end = arc->x0 + arc->radius * cos(arc->end_angle);
          double y_end = arc->y0 + arc->radius * sin(arc->end_angle);
          //Assuming clockwise arcs.
          int large_arc = (arc->end_angle - arc->start_angle < 3.1415) ? 0 : 1;
          printf(
              "\t<path id=\"dwg-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n",
              i, x_start, page_height - y_start, arc->radius, arc->radius,
              large_arc, x_end, page_height - y_end, 0.1);
        }

      if (obj->type == DWG_TYPE_TEXT)
        {
          texts++;
          Dwg_Entity_TEXT* text;
          text = obj->tio.entity->tio.TEXT;
          /*TODO: Juca, fix it properly: */
          if (text->text[0] != '&')
            {
              printf(
                  "\t<text id=\"dwg-%d\" x=\"%f\" y=\"%f\" font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n",
                  i, text->x0, page_height - text->y0,
                  text->height /* fontsize */, text->text);
            }
        }
    }
  printf("</svg>\n");
  if (lines > 0)
    fprintf(stderr, "Lines: %d ", lines);
  if (circles > 0)
    fprintf(stderr, "Circles: %d ", circles);
  if (arcs > 0)
    fprintf(stderr, "Arcs: %d ", arcs);
  if (texts > 0)
    fprintf(stderr, "Texts: %d ", texts);
  fprintf(stderr, "\n\n");
}

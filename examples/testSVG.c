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

double page_width;
double page_height;

int
test_SVG(char *filename);

void
output_SVG(Dwg_Data* dwg_struct);

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
  Dwg_Data dwg_struct;

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
output_TEXT(Dwg_Object* obj)
{
  Dwg_Entity_TEXT* text;
  text = obj->tio.entity->tio.TEXT;

  /*TODO: Juca, fix it properly: */
  if (text->text_value[0] == '&') return;

  printf(
      "\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n",
      obj->index, text->insertion_pt.x, page_height - text->insertion_pt.y,
      text->height /* fontsize */, text->text_value);
}


void
output_LINE(Dwg_Object* obj)
{
  Dwg_Entity_LINE* line;
  line = obj->tio.entity->tio.LINE;
  printf(
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
      obj->index, line->start.x, page_height - line->start.y, line->end.x, page_height
          - line->end.y);
}

void
output_CIRCLE(Dwg_Object* obj)
{
  Dwg_Entity_CIRCLE* circle;
  circle = obj->tio.entity->tio.CIRCLE;
  printf(
      "\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
      obj->index, circle->center.x, page_height - circle->center.y, circle->radius);
}

void
output_ARC(Dwg_Object* obj)
{
  Dwg_Entity_ARC* arc;
  arc = obj->tio.entity->tio.ARC;
  double x_start = arc->center.x + arc->radius * cos(arc->start_angle);
  double y_start = arc->center.y + arc->radius * sin(arc->start_angle);
  double x_end = arc->center.x + arc->radius * cos(arc->end_angle);
  double y_end = arc->center.y + arc->radius * sin(arc->end_angle);
  //Assuming clockwise arcs.
  int large_arc = (arc->end_angle - arc->start_angle < 3.1415) ? 0 : 1;
  printf(
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n",
      obj->index, x_start, page_height - y_start, arc->radius, arc->radius,
      large_arc, x_end, page_height - y_end, 0.1);
}

void
output_INSERT(Dwg_Object* obj)
{
  Dwg_Entity_INSERT* insert;
  insert = obj->tio.entity->tio.INSERT;
  //if (insert->block_header->handleref.code == 5)
  if(42) //XXX did this to test the new handleref.code handling "code"
    {
      printf(
          "\t<use id=\"dwg-object-%d\" transform=\"translate(%f %f) rotate(%f) scale(%f %f)\" xlink:href=\"#symbol-%lu\" /><!-- block_header->handleref: %d.%d.%lu -->\n",
          obj->index,
          insert->ins_pt.x, page_height - insert->ins_pt.y, (180.0 / M_PI)
              * insert->rotation_ang, insert->scale.x, insert->scale.y,
          insert->block_header->absolute_ref,
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

void
output_object(Dwg_Object* obj){
  if (!obj)
    {
      fprintf(stderr, "object is NULL\n");
      return;
    }

  if (obj->type == DWG_TYPE_INSERT)
    {
      output_INSERT(obj);
    }

  if (obj->type == DWG_TYPE_LINE)
    {
      output_LINE(obj);
    }

  if (obj->type == DWG_TYPE_CIRCLE)
    {
      output_CIRCLE(obj);
    }

  if (obj->type == DWG_TYPE_TEXT)
    {
      output_TEXT(obj);
    }

  if (obj->type == DWG_TYPE_ARC)
    {
      output_ARC(obj);
    }
}

void output_BLOCK_HEADER(Dwg_Object_Ref* ref)
{
  Dwg_Object* obj;

  Dwg_Object_BLOCK_HEADER* hdr;
  hdr = ref->obj->tio.object->tio.BLOCK_HEADER;

  printf(
      "\t<g id=\"symbol-%lu\" >\n\t\t<!-- %s -->\n", ref->absolute_ref, hdr->entry_name);

  //TODO:still not quite right I think...
  obj = hdr->first_entity->obj;
  while(obj && obj != hdr->last_entity->obj)
    {
      output_object(obj);
      obj = dwg_next_object(obj);
    }
  //output the last one:
  if (obj) output_object(obj);

  printf("\t</g>\n");
}

void
output_SVG(Dwg_Data* dwg_struct)
{
  unsigned int i;
  Dwg_Object *obj;
  page_width = dwg_model_x_max(dwg_struct) - dwg_model_x_min(dwg_struct);
  page_height = dwg_model_y_max(dwg_struct) - dwg_model_y_min(dwg_struct);

  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<svg\n"
    "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
    "   version=\"1.1\"\n"
    "   width=\"%f\"\n"
    "   height=\"%f\"\n"
    ">\n", page_width, page_height);

  obj = &dwg_struct->object[0];
  if (obj->type != DWG_TYPE_BLOCK_CONTROL)
    {
      fprintf(stderr, "ERROR: First object is not a BLOCK_CONTROL\n");
      return;
    }

  Dwg_Object_BLOCK_CONTROL* block_control;
  block_control = obj->tio.object->tio.BLOCK_CONTROL;
  printf("\t<defs>\n");
  for (i=0; i<block_control->num_entries; i++)
    {
      output_BLOCK_HEADER(block_control->block_headers[i]);
    }
  printf("\t</defs>\n");

  output_BLOCK_HEADER(block_control->model_space);
  output_BLOCK_HEADER(block_control->paper_space);

/*
  for (i = 0; i < dwg_struct->num_objects; i++)
    {
      obj = &dwg_struct->object[i];
      output_object(obj);
    }
*/
  printf("</svg>\n");
}

/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * testSVG.c: convert a DWG to SVG
 * written by Felipe Corrêa da Silva Sances
 * modified by Thien-Thi Nguyen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <dwg.h>
#include "../src/bits.h"
#include "suffix.c"

double model_xmin, model_ymin;
double page_width, page_height, scale;

double transform_X(double x){
  return x - model_xmin;
}

double transform_Y(double y){
  return page_height - (y - model_ymin);
}

int
test_SVG(char *filename);

void
output_SVG(Dwg_Data* dwg);

int
main(int argc, char *argv[])
{
  REQUIRE_INPUT_FILE_ARG (argc);
  return test_SVG (argv[1]);
}

int
test_SVG(char *filename)
{
  int error;
  Dwg_Data dwg;

  error = dwg_read_file(filename, &dwg);

  if (!error)
    {
      output_SVG(&dwg);
    }

  dwg_free(&dwg);
  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error ? 1 : 0;
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
      obj->index, transform_X(text->insertion_pt.x), transform_Y(text->insertion_pt.y),
      text->height /* fontsize */, text->text_value);
}


void
output_LINE(Dwg_Object* obj)
{
  Dwg_Entity_LINE* line;
  line = obj->tio.entity->tio.LINE;
  printf(
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
      obj->index, transform_X(line->start.x), transform_Y(line->start.y), transform_X(line->end.x), transform_Y(line->end.y));
}

void
output_CIRCLE(Dwg_Object* obj)
{
  Dwg_Entity_CIRCLE* circle;
  circle = obj->tio.entity->tio.CIRCLE;
  printf(
      "\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
      obj->index, transform_X(circle->center.x), transform_Y(circle->center.y), circle->radius);
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
      obj->index, transform_X(x_start), transform_Y(y_start), arc->radius, arc->radius,
      large_arc, transform_X(x_end), transform_Y(y_end), 0.1);
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
          transform_X(insert->ins_pt.x), transform_Y(insert->ins_pt.y), (180.0 / M_PI)
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

  if (!ref)
    {
      fprintf(stderr, "Found null object reference. Could not output an SVG symbol for this BLOCK_HEADER\n");
      return;
    }
  if (!ref->obj)
    {
      fprintf(stderr, "Found null ref->obj\n");
      return;
    }

  /* TODO: Review.  (This check avoids a segfault, but it is
     still unclear whether or not the condition is valid.)  */
  if (!ref->obj->tio.object)
    {
      fprintf(stderr, "Found null ref->obj->tio.object\n");
      return;
    }

  hdr = ref->obj->tio.object->tio.BLOCK_HEADER;
  printf(
      "\t<g id=\"symbol-%lu\" >\n\t\t<!-- %s -->\n", ref->absolute_ref, hdr->entry_name);

  obj = get_first_owned_object(ref->obj, hdr);

  while(obj)
    {
      output_object(obj);
      obj = get_next_owned_object(ref->obj, obj, hdr);
    }

  printf("\t</g>\n");
}


void
output_SVG(Dwg_Data* dwg)
{
  unsigned int i;
  Dwg_Object *obj;

  model_xmin = dwg_model_x_min(dwg);
  model_ymin = dwg_model_y_min(dwg);

  double dx = (dwg_model_x_max(dwg) - dwg_model_x_min(dwg));
  double dy = (dwg_model_y_max(dwg) - dwg_model_y_min(dwg));
  double scale_x = dx / (dwg_page_x_max(dwg) - dwg_page_x_min(dwg));
  double scale_y = dy / (dwg_page_y_max(dwg) - dwg_page_y_min(dwg));
  //scale = 25.4 / 72; // pt:mm
  page_width = dx;
  page_height = dy;
  //scale *= (scale_x > scale_y ? scale_x : scale_y);

  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<svg\n"
    "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
    "   version=\"1.1\"\n"
    "   width=\"%f\"\n"
    "   height=\"%f\"\n"
    ">\n", page_width, page_height);

  obj = &dwg->object[0];
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
  for (i = 0; i < dwg->num_objects; i++)
    {
      obj = &dwg->object[i];
      output_object(obj);
    }
*/
  printf("</svg>\n");
}

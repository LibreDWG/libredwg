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
#include <dwg_api.h>
//#include "../src/bits.h"
//#include "suffix.c"

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
output_SVG(dwg_data* dwg);

int
main(int argc, char *argv[])
{
//  REQUIRE_INPUT_FILE_ARG (argc);
  return test_SVG (argv[1]);
}

int
test_SVG(char *filename)
{
  int error;
  dwg_data dwg;

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
output_TEXT(dwg_object* obj)
{
  int error, index;
  dwg_point_2d ins_pt;
  Dwg_Entity_TEXT* text;
  char * text_value;
  index = dwg_obj_object_get_index(obj, &error);
  text = dwg_object_to_TEXT(obj);
  text_value = dwg_ent_text_get_text(text, &error);
  dwg_ent_text_get_insertion_point(text, &ins_pt, &error);
  /*TODO: Juca, fix it properly: */
  if (text_value[0] == '&') return;

  printf(
      "\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n",
      index, transform_X(ins_pt.x), transform_Y(ins_pt.y),
      dwg_ent_text_get_height(text, &error) /* fontsize */, text_value);
}

void
output_LINE(dwg_object* obj)
{
  int error, index;
  Dwg_Entity_LINE* line;
  dwg_point_3d start, end;
  index = dwg_obj_object_get_index(obj, &error);
  line = dwg_object_to_LINE(obj);
  dwg_ent_line_get_start_point(line, &start, &error);
  dwg_ent_line_get_end_point(line, &end, &error);
  printf(
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
      index, transform_X(start.x), transform_Y(start.y), transform_X(end.x), 
      transform_Y(end.y));
}

void
output_CIRCLE(dwg_object* obj)
{
  Dwg_Entity_CIRCLE* circle;
  int error, index;
  float radius;
  dwg_point_3d center;
  index = dwg_obj_object_get_index(obj, &error);
  circle = dwg_object_to_CIRCLE(obj);
  radius = dwg_ent_circle_get_radius(circle, &error);
  printf(
      "\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
      index, transform_X(center.x), transform_Y(center.y), radius);
}

void
output_ARC(dwg_object* obj)
{
  Dwg_Entity_ARC* arc;
  int error, index;
  float radius, start_angle, end_angle;
  dwg_point_3d center;
  index = dwg_obj_object_get_index(obj, &error);
  arc = dwg_object_to_ARC(obj);
  radius = dwg_ent_arc_get_radius(arc, &error);
  start_angle = dwg_ent_arc_get_start_angle(arc, &error);
  end_angle = dwg_ent_arc_get_end_angle(arc, &error);
  dwg_ent_arc_get_center(arc, &center, &error);
  double x_start = center.x + radius * cos(start_angle);
  double y_start = center.y + radius * sin(start_angle);
  double x_end = center.x + radius * cos(end_angle);
  double y_end = center.y + radius * sin(end_angle);
  //Assuming clockwise arcs.
  int large_arc = (end_angle - start_angle < 3.1415) ? 0 : 1;
  printf(
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n",
      index, transform_X(x_start), transform_Y(y_start), radius, radius,
      large_arc, transform_X(x_end), transform_Y(y_end), 0.1);
}

void
output_INSERT(dwg_object* obj)
{
  int index, error;
  unsigned long abs_ref;
  double rotation_ang;
  dwg_ent_insert* insert;
  dwg_point_3d ins_pt, scale;
  dwg_handle obj_handle, ins_handle; 
  insert = dwg_object_to_INSERT(obj);
  index = dwg_obj_object_get_index(obj, &error);
  rotation_ang = dwg_ent_insert_get_rotation_angle(insert, &error);
  dwg_ent_insert_get_ins_pt(insert, &ins_pt, &error);
  dwg_ent_insert_get_scale(insert, &scale, &error);
  obj_handle = dwg_obj_get_handle(obj, &error);
  ins_handle = dwg_ent_insert_get_ref_handle(insert, &error);
  abs_ref = dwg_ent_insert_get_abs_ref(insert, &error);
  //if (insert->block_header->handleref.code == 5)
  if(42) //XXX did this to test the new handleref.code handling "code"
    {
      printf(
          "\t<use id=\"dwg-object-%d\" transform=\"translate(%f %f) rotate(%f) scale(%f %f)\" xlink:href=\"#symbol-%lu\" /><!-- block_header->handleref: %d.%d.%lu -->\n",
          index,
          transform_X(ins_pt.x), transform_Y(ins_pt.y), (180.0 / M_PI)
            * rotation_ang, scale.x, scale.y, abs_ref, ins_handle.code,
          ins_handle.size, ins_handle.value);
    }
  else
    {
      printf(
          "\n\n<!-- WRONG INSERT(%d.%d.%lu): handleref = %d.%d.%lu -->\n",
          obj_handle.code, obj_handle.size, obj_handle.value,
          ins_handle.code, ins_handle.size, ins_handle.value);
    }
}

void
output_object(dwg_object* obj){
  if (!obj)
    {
      fprintf(stderr, "object is NULL\n");
      return;
    }

  if (dwg_get_type(obj) == DWG_TYPE_INSERT)
    {
      output_INSERT(obj);
    }

  if (dwg_get_type(obj)== DWG_TYPE_LINE)
    {
      output_LINE(obj);
    }

  if (dwg_get_type(obj)== DWG_TYPE_CIRCLE)
    {
      output_CIRCLE(obj);
    }

  if (dwg_get_type(obj)== DWG_TYPE_TEXT)
    {
      output_TEXT(obj);
    }

  if (dwg_get_type(obj)== DWG_TYPE_ARC)
    {
      output_ARC(obj);
    }
}

void output_BLOCK_HEADER(dwg_object_ref* ref)
{
  dwg_object* obj, *variable_obj;
  dwg_obj_block_header* hdr;
  int error;
  unsigned long abs_ref;
  obj = dwg_obj_reference_get_object(ref, &error);
  abs_ref = dwg_obj_ref_get_abs_ref(ref, &error);
  if (!ref)
    {
      fprintf(stderr, "Found null object reference. Could not output an SVG symbol for this BLOCK_HEADER\n");
      return;
    }
  if (!obj)
    {
      fprintf(stderr, "Found null ref->obj\n");
      return;
    }

  /* TODO: Review.  (This check avoids a segfault, but it is
     still unclear whether or not the condition is valid.)  */
  if (!dwg_object_to_object(obj, &error))
    {
      fprintf(stderr, "Found null ref->obj->tio.object\n");
      return;
    }

  hdr = dwg_object_to_BLOCK_HEADER(obj);
  printf(
      "\t<g id=\"symbol-%lu\" >\n\t\t<!-- %s -->\n", abs_ref, 
      dwg_obj_block_header_get_name(hdr, &error));

  variable_obj = get_first_owned_object(obj, hdr);

  while(variable_obj)
    {
      output_object(variable_obj);
      variable_obj = get_next_owned_object(obj, variable_obj, hdr);
    }

  printf("\t</g>\n");
}


void
output_SVG(dwg_data* dwg)
{
  unsigned int i, num_hdr_objs, error;
  dwg_object *obj;
  dwg_obj_block_header *hdr;
  dwg_obj_block_control *ctrl;
  dwg_object_ref **hdr_refs; 
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

  hdr = dwg_get_block_header(dwg, &error);
  ctrl = dwg_block_header_get_block_control(hdr, &error);
  
  hdr_refs = dwg_obj_block_control_get_block_headers(ctrl, &error);

  num_hdr_objs = dwg_obj_block_control_get_num_entries(ctrl, &error);
  printf("\t<defs>\n");
    for (i=0; i<num_hdr_objs; i++)
      {
        output_BLOCK_HEADER(hdr_refs[i]);
      }
  printf("\t</defs>\n");

  output_BLOCK_HEADER(dwg_obj_block_control_get_model_space(ctrl, &error));
  output_BLOCK_HEADER(dwg_obj_block_control_get_paper_space(ctrl, &error));

  printf("</svg>\n");
}

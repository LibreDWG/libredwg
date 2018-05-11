/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2013, 2018 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2svg2.c: convert a DWG to SVG via the API
 * written by Gaganjyot Singh
 * modified by Reini Urban
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "../src/config.h"
#include <dwg.h>
#include <dwg_api.h>

static dwg_data g_dwg;
static double model_xmin, model_ymin;
static double page_width, page_height, scale;

static int help(void);
int verbosity(int argc, char **argv, int i, unsigned int *opts);
#include "../programs/common.inc"

static int usage(void) {
  printf("\nUsage: dwg2svg2 [-v[0-9]] DWGFILE\n");
  return 1;
}
static int opt_version(void) {
  printf("dwg2svg2 %s\n", PACKAGE_VERSION);
  return 0;
}
static int help(void) {
  printf("\nUsage: dwg2svg2 [OPTION]... DWGFILE >file.svg\n");
  printf("Example to use the DWG api\n"
         "\n");
  printf("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf("           --help           display this help and exit\n");
  printf("           --version        output version information and exit\n"
         "\n");
  printf("GNU LibreDWG online manual: <https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

#define log_if_error(msg) \
  if (error) { fprintf(stderr, "ERROR: %s", msg); exit(1); }

static double transform_X(double x){
  return x - model_xmin;
}

static double transform_Y(double y){
  return page_height - (y - model_ymin);
}

static void
output_SVG(dwg_data* dwg);

static int
test_SVG(char *filename, unsigned int opts)
{
  int error;

  memset(&g_dwg, 0, sizeof(dwg_data));
  g_dwg.opts = opts;
  error = dwg_read_file(filename, &g_dwg);
  if (!error)
    output_SVG(&g_dwg);

  dwg_free(&g_dwg);
  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error ? 1 : 0;
}

static void
output_TEXT(dwg_object* obj)
{
  int error, index;
  dwg_point_2d ins_pt;
  Dwg_Entity_TEXT* text;
  char * text_value;
  double fontsize;

  index = dwg_obj_object_get_index(obj, &error);
  log_if_error("object_get_index");
  text = dwg_object_to_TEXT(obj);
  if (!text) {
    error = 1; log_if_error("dwg_object_to_TEXT");
  }
  text_value = dwg_ent_text_get_text(text, &error);
  log_if_error("text_get_text");
  dwg_ent_text_get_insertion_point(text, &ins_pt, &error);
  log_if_error("text_get_insertion_point");
  fontsize = dwg_ent_text_get_height(text, &error);
  log_if_error("text_get_height");

  /*TODO: Juca, fix it properly: */
  if (text_value[0] == '&') return;

  printf(
      "\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n",
      index, transform_X(ins_pt.x), transform_Y(ins_pt.y),
      fontsize, text_value);
}

static void
output_LINE(dwg_object* obj)
{
  int error, index;
  Dwg_Entity_LINE* line;
  dwg_point_3d start, end;
  
  index = dwg_obj_object_get_index(obj, &error);
  log_if_error("object_get_index");
  line = dwg_object_to_LINE(obj);
  if (!line) {
    error = 1; log_if_error("dwg_object_to_LINE");
  }
  dwg_ent_line_get_start_point(line, &start, &error);
  log_if_error("line_get_start_point");
  dwg_ent_line_get_end_point(line, &end, &error);
  log_if_error("line_get_end_point");
  printf(
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f %f,%f\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
      index, transform_X(start.x), transform_Y(start.y), transform_X(end.x), 
      transform_Y(end.y));
}

static void
output_CIRCLE(dwg_object* obj)
{
  Dwg_Entity_CIRCLE* circle;
  int error, index;
  double radius;
  dwg_point_3d center;
  
  index = dwg_obj_object_get_index(obj, &error);
  log_if_error("object_get_index");
  circle = dwg_object_to_CIRCLE(obj);
  if (!circle) {
    error = 1; log_if_error("dwg_object_to_LINE");
  }
  dwg_ent_circle_get_center(circle, &center, &error);
  log_if_error("circle_get_center");
  radius = dwg_ent_circle_get_radius(circle, &error);
  log_if_error("circle_get_radius");
  printf(
      "\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
      index, transform_X(center.x), transform_Y(center.y), radius);
}

static void
output_ARC(dwg_object* obj)
{
  Dwg_Entity_ARC* arc;
  int error, index;
  double radius, start_angle, end_angle;
  dwg_point_3d center;
  double x_start, y_start, x_end, y_end;
  int large_arc;
  
  index = dwg_obj_object_get_index(obj, &error);
  log_if_error("object_get_index");
  arc = dwg_object_to_ARC(obj);
  if (!arc) {
    error = 1; log_if_error("dwg_object_to_ARC");
  }
  radius = dwg_ent_arc_get_radius(arc, &error);
  log_if_error("arc_get_radius");
  start_angle = dwg_ent_arc_get_start_angle(arc, &error);
  log_if_error("arc_get_start_angle");
  end_angle = dwg_ent_arc_get_end_angle(arc, &error);
  log_if_error("arc_get_end_angle");
  dwg_ent_arc_get_center(arc, &center, &error);
  log_if_error("arc_get_center");
      
  x_start = center.x + radius * cos(start_angle);
  y_start = center.y + radius * sin(start_angle);
  x_end = center.x + radius * cos(end_angle);
  y_end = center.y + radius * sin(end_angle);
  //Assuming clockwise arcs.
  large_arc = (end_angle - start_angle < 3.1415) ? 0 : 1;
  printf(
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n",
      index, transform_X(x_start), transform_Y(y_start), radius, radius,
      large_arc, transform_X(x_end), transform_Y(y_end), 0.1);
}

static void
output_INSERT(dwg_object* obj)
{
  int index, error;
  unsigned long abs_ref;
  double rotation;
  dwg_ent_insert* insert;
  dwg_point_3d ins_pt, _scale;
  dwg_handle *obj_handle, *ins_handle;

  insert = dwg_object_to_INSERT(obj);
  if (!insert) {
    error = 1; log_if_error("dwg_object_to_INSERT");
  }
  index = dwg_obj_object_get_index(obj, &error);
  log_if_error("object_get_index");
  rotation = dwg_ent_insert_get_rotation(insert, &error);
  log_if_error("insert_get_rotation");
  dwg_ent_insert_get_ins_pt(insert, &ins_pt, &error);
  log_if_error("insert_get_ins_pt");
  dwg_ent_insert_get_scale(insert, &_scale, &error);
  log_if_error("insert_get_scale");
  obj_handle = dwg_obj_get_handle(obj, &error);
  log_if_error("get_handle");
  ins_handle = dwg_ent_insert_get_ref_handle(insert, &error);
  log_if_error("insert_get_ref_handle");
  abs_ref = dwg_ent_insert_get_abs_ref(insert, &error);
  log_if_error("insert_get_abs_ref");
  
  if (insert->block_header->handleref.code == 5)
    {
      printf(
          "\t<use id=\"dwg-object-%d\" transform=\"translate(%f %f) rotate(%f) scale(%f %f)\" xlink:href=\"#symbol-%lu\" /><!-- block_header->handleref: %d.%d.%lu -->\n",
          index,
          transform_X(ins_pt.x), transform_Y(ins_pt.y),
          (180.0 / M_PI) * rotation, _scale.x, _scale.y, abs_ref,
          ins_handle->code, ins_handle->size, ins_handle->value);
    }
  else
    {
      printf(
          "\n\n<!-- WRONG INSERT(%d.%d.%lu): handleref = %d.%d.%lu -->\n",
          obj_handle->code, obj_handle->size, obj_handle->value,
          ins_handle->code, ins_handle->size, ins_handle->value);
    }
}

static void
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

static void
output_BLOCK_HEADER(dwg_object_ref* ref)
{
  dwg_object* obj, *variable_obj;
  dwg_obj_block_header* hdr;
  int error;
  unsigned long abs_ref;
  char *name;
  
  obj = dwg_obj_reference_get_object(ref, &error);
  log_if_error("reference_get_object");
  abs_ref = dwg_obj_ref_get_abs_ref(ref, &error);
  log_if_error("ref_get_abs_ref");
  if (!ref)
    {
      fprintf(stderr, "Found null object reference."
              " Could not output an SVG symbol for this BLOCK_HEADER\n");
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
  log_if_error("object_to_object");

  hdr = dwg_object_to_BLOCK_HEADER(obj);
  name = dwg_obj_block_header_get_name(hdr, &error);
  log_if_error("block_header_get_name");
  printf(
      "\t<g id=\"symbol-%lu\" >\n\t\t<!-- %s -->\n", abs_ref, name);

  variable_obj = get_first_owned_object(obj, hdr);
  while (variable_obj)
    {
      output_object(variable_obj);
      variable_obj = get_next_owned_object(obj, variable_obj, hdr);
    }

  printf("\t</g>\n");
}


static void
output_SVG(dwg_data *dwg)
{
  unsigned int i, num_hdr_objs;
  int error;
  dwg_obj_block_header *hdr;
  dwg_obj_block_control *ctrl;
  dwg_object_ref **hdr_refs; 

  double dx = dwg_model_x_max(dwg) - dwg_model_x_min(dwg);
  double dy = dwg_model_y_max(dwg) - dwg_model_y_min(dwg);
  double pdx = dwg->header_vars.PLIMMAX.x - dwg->header_vars.PLIMMIN.x;
  double pdy = dwg->header_vars.PLIMMAX.y - dwg->header_vars.PLIMMIN.y;
  double scale_x = dx / (pdx == 0.0 ? 1.0 : pdx);
  double scale_y = dy / (pdy == 0.0 ? 1.0 : pdy);
  scale = 25.4 / 72.0; // pt:mm TODO

  model_xmin = dwg_model_x_min(dwg);
  model_ymin = dwg_model_y_min(dwg);
  page_width = dx;
  page_height = dy;
  scale *= (scale_x > scale_y ? scale_x : scale_y);

  hdr = dwg_get_block_header(dwg, &error);
  log_if_error("get_block_header");
  ctrl = dwg_block_header_get_block_control(hdr, &error);
  log_if_error("block_header_get_block_control");
  
  hdr_refs = dwg_obj_block_control_get_block_headers(ctrl, &error);
  log_if_error("block_control_get_block_headers");
  num_hdr_objs = dwg_obj_block_control_get_num_entries(ctrl, &error);
  log_if_error("block_control_get_num_entries");

  printf("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
    "<svg\n"
    "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns=\"http://www.w3.org/2000/svg\"\n"
    "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
    "   version=\"1.1\"\n"
    "   width=\"%f\"\n"
    "   height=\"%f\"\n"
    ">\n", page_width, page_height);
  printf("\t<defs>\n");
  for (i=0; i < num_hdr_objs; i++)
    {
      output_BLOCK_HEADER(hdr_refs[i]);
    }
  printf("\t</defs>\n");

  output_BLOCK_HEADER(dwg_obj_block_control_get_model_space(ctrl, &error));
  log_if_error("block_control_get_model_space");

  output_BLOCK_HEADER(dwg_obj_block_control_get_paper_space(ctrl, &error));
  log_if_error("block_control_get_paper_space");

  printf("</svg>\n");
}

int
main(int argc, char *argv[])
{
  int i = 1;
  unsigned int opts = 1;

  if (argc < 2)
    return usage();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
  setenv("LIBREDWG_TRACE", "1", 0);
#endif
  if (argc > 2 &&
      (!strcmp(argv[i], "--verbose") ||
       !strncmp(argv[i], "-v", 2)))
    {
      int num_args = verbosity(argc, argv, i, &opts);
      argc -= num_args;
      i += num_args;
    }
  if (argc > 1 && !strcmp(argv[i], "--help"))
    return help();
  if (argc > 1 && !strcmp(argv[i], "--version"))
    return opt_version();
  if (argc >= 1)
    return test_SVG(argv[i], opts);
  return 1;
}

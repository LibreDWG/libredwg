/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2013-2020,2023 Free Software Foundation, Inc.              */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2svg2.c: convert a DWG to SVG via the API.
   if there are paperspace entities, only output them. else all modelspace
   entities.
 * written by Gaganjyot Singh
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>

#ifdef ENABLE_MIMALLOC
#  include <mimalloc-override.h>
#endif

#include <dwg.h>
#include <dwg_api.h>
#include "../src/common.h"
#include "geom.h"

static int opts = 0;
static dwg_data g_dwg;
static double model_xmin, model_ymin;
static double page_width, page_height, scale;

static int
usage (void)
{
  printf ("\nUsage: dwg2svg2 [-v[0-9]] DWGFILE\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwg2svg2 %s\n", PACKAGE_VERSION);
  return 0;
}

static int
help (void)
{
  printf ("\nUsage: dwg2svg2 [OPTION]... DWGFILE >file.svg\n");
  printf ("Example to use the DWG api\n"
          "\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -h          display this help and exit\n");
  printf ("  -i          output version information and exit\n"
          "\n");
#endif
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

#define log_if_error(msg)                                                     \
  if (error)                                                                  \
    {                                                                         \
      fprintf (stderr, "ERROR: %s\n", msg);                                   \
      exit (1);                                                               \
    }
#define log_error(msg)                                                        \
  {                                                                           \
    fprintf (stderr, "ERROR: %s\n", msg);                                     \
    exit (1);                                                                 \
  }
#define dynget(obj, name, field, var)                                         \
  if (!dwg_dynapi_entity_value (obj, "" name, "" field, var, NULL))           \
    {                                                                         \
      fprintf (stderr, "ERROR: %s.%s\n", name, field);                        \
      exit (1);                                                               \
    }
#define dynget_utf8(obj, name, field, var)                                    \
  if (!dwg_dynapi_entity_utf8text (obj, "" name, "" field, var, &isnew,       \
                                   NULL))                                     \
    {                                                                         \
      fprintf (stderr, "ERROR: %s.%s\n", name, field);                        \
      exit (1);                                                               \
    }

static double
transform_X (double x)
{
  return x - model_xmin;
}

static double
transform_Y (double y)
{
  return page_height - (y - model_ymin);
}

static void output_SVG (dwg_data *dwg);

static int
test_SVG (char *filename)
{
  int error;

  memset (&g_dwg, 0, sizeof (dwg_data));
  g_dwg.opts = opts;
  error = dwg_read_file (filename, &g_dwg);
  if (error < DWG_ERR_CRITICAL)
    output_SVG (&g_dwg);

  dwg_free (&g_dwg);
  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error < DWG_ERR_CRITICAL ? 0 : 1;
}

static void
output_TEXT (dwg_object *obj)
{
  int error, index;
  dwg_point_2d ins_pt;
  Dwg_Entity_TEXT *text;
  char *text_value;
  double fontsize;
  const Dwg_Version_Type dwg_version = obj->parent->header.version;
  int isnew = 0;

  index = dwg_object_get_index (obj, &error);
  log_if_error ("object_get_index");
  text = dwg_object_to_TEXT (obj);
  if (!text)
    log_error ("dwg_object_to_TEXT");
  dynget_utf8 (text, "TEXT", "text_value", &text_value);
  dynget (text, "TEXT", "ins_pt", &ins_pt);
  dynget (text, "TEXT", "height", &fontsize);

  printf ("\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" "
          "font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n",
          index, transform_X (ins_pt.x), transform_Y (ins_pt.y), fontsize,
          text_value);

  if (text_value && isnew)
    FREE (text_value);
}

static void
output_LINE (dwg_object *obj)
{
  int error, index;
  Dwg_Entity_LINE *line;
  dwg_point_3d start, end;

  index = dwg_object_get_index (obj, &error);
  log_if_error ("object_get_index");
  line = dwg_object_to_LINE (obj);
  if (!line)
    log_error ("dwg_object_to_LINE");
  if (!dwg_get_LINE (line, "start", &start))
    log_error ("LINE.start");
  if (!dwg_get_LINE (line, "end", &end))
    log_error ("LINE.end");

  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f %f,%f\" "
          "style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
          index, transform_X (start.x), transform_Y (start.y),
          transform_X (end.x), transform_Y (end.y));
}

static void
output_CIRCLE (dwg_object *obj)
{
  Dwg_Entity_CIRCLE *circle;
  int error, index;
  double radius;
  dwg_point_3d center;

  index = dwg_object_get_index (obj, &error);
  log_if_error ("object_get_index");
  circle = dwg_object_to_CIRCLE (obj);
  if (!circle)
    log_error ("dwg_object_to_CIRCLE");
  if (!dwg_get_CIRCLE (circle, "center", &center))
    log_error ("CIRCLE.center");
  if (!dwg_get_CIRCLE (circle, "radius", &radius))
    log_error ("CIRCLE.radius");

  printf ("\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" "
          "fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
          index, transform_X (center.x), transform_Y (center.y), radius);
}

static void
output_ARC (dwg_object *obj)
{
  Dwg_Entity_ARC *arc;
  int error, index;
  double radius, start_angle, end_angle;
  dwg_point_3d center;
  double x_start, y_start, x_end, y_end;
  int large_arc;

  index = dwg_object_get_index (obj, &error);
  log_if_error ("object_get_index");
  arc = dwg_object_to_ARC (obj);
  if (!arc)
    log_error ("dwg_object_to_ARC");
  dynget (arc, "ARC", "radius", &radius);
  dynget (arc, "ARC", "center", &center);
  dynget (arc, "ARC", "start_angle", &start_angle);
  dynget (arc, "ARC", "end_angle", &end_angle);

  x_start = center.x + radius * cos (start_angle);
  y_start = center.y + radius * sin (start_angle);
  x_end = center.x + radius * cos (end_angle);
  y_end = center.y + radius * sin (end_angle);
  // Assuming clockwise arcs.
  large_arc = (end_angle - start_angle < M_PI) ? 0 : 1;

  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" "
          "fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n",
          index, transform_X (x_start), transform_Y (y_start), radius, radius,
          large_arc, transform_X (x_end), transform_Y (y_end), 0.1);
}

static void
output_INSERT (dwg_object *obj)
{
  int index, error;
  BITCODE_RL abs_ref;
  double rotation;
  dwg_ent_insert *insert;
  dwg_point_3d ins_pt, _scale;
  dwg_handle *obj_handle, *ins_handle;
  Dwg_Data *dwg;

  insert = dwg_object_to_INSERT (obj);
  if (!insert)
    log_error ("dwg_object_to_INSERT");
  dwg = obj->parent;
  index = dwg_object_get_index (obj, &error);
  log_if_error ("object_get_index");
  dynget (insert, "INSERT", "rotation", &rotation);
  dynget (insert, "INSERT", "ins_pt", &ins_pt);
  dynget (insert, "INSERT", "scale", &_scale);
  obj_handle = dwg_object_get_handle (obj, &error);
  log_if_error ("get_handle");
  if (!insert->block_header)
    log_error ("insert->block_header");
  abs_ref = insert->block_header->absolute_ref;

  if (insert->block_header->handleref.code == 5 || dwg->header.version < R_13)
    {
      printf ("\t<use id=\"dwg-object-%d\" transform=\"translate(%f %f) "
              "rotate(%f) scale(%f %f)\" xlink:href=\"#symbol-%X\" /><!-- "
              "block_header->handleref: " FORMAT_H " -->\n",
              index, transform_X (ins_pt.x), transform_Y (ins_pt.y),
              (180.0 / M_PI) * rotation, _scale.x, _scale.y, abs_ref,
              ARGS_H (*obj_handle));
    }
  else
    {
      printf ("\n\n<!-- WRONG INSERT(" FORMAT_H ") -->\n",
              ARGS_H (*obj_handle));
    }
}

static int
output_object (dwg_object *obj)
{
  int i = 0;
  if (!obj)
    {
      fprintf (stderr, "object is NULL\n");
      return 0;
    }

  if (dwg_object_get_fixedtype (obj) == DWG_TYPE_INSERT)
    {
      i++;
      output_INSERT (obj);
    }
  else if (dwg_object_get_fixedtype (obj) == DWG_TYPE_LINE)
    {
      i++;
      output_LINE (obj);
    }
  else if (dwg_object_get_fixedtype (obj) == DWG_TYPE_CIRCLE)
    {
      i++;
      output_CIRCLE (obj);
    }
  else if (dwg_object_get_fixedtype (obj) == DWG_TYPE_TEXT)
    {
      i++;
      output_TEXT (obj);
    }
  else if (dwg_object_get_fixedtype (obj) == DWG_TYPE_ARC)
    {
      i++;
      output_ARC (obj);
    }
  return i;
}

static int
output_BLOCK_HEADER (dwg_object_ref *ref)
{
  dwg_object *hdr, *obj;
  dwg_obj_block_header *_hdr;
  int error;
  BITCODE_RL abs_ref;
  char *name;
  int i = 0;

  if (!ref)
    {
      fprintf (stderr,
               "Empty BLOCK."
               " Could not output an SVG symbol for this BLOCK_HEADER\n");
      return 0;
    }
  hdr = dwg_ref_get_object (ref, &error);
  if (!hdr || error)
    {
      abs_ref = dwg_ref_get_absref (ref, &error);
      fprintf (stderr, "Failed to resolve BLOCK handle %X.\n",
               (unsigned)abs_ref);
      return 0;
    }
  abs_ref = dwg_ref_get_absref (ref, &error);

  _hdr = dwg_object_to_BLOCK_HEADER (hdr);
  if (_hdr)
    {
      i++;
      dynget (_hdr, "BLOCK_HEADER", "name", &name);
      // name = dwg_obj_block_header_get_name (_hdr, &error);
      printf ("\t<g id=\"symbol-%X\" >\n\t\t<!-- %s -->\n",
              abs_ref ? abs_ref : 0, name ? name : "");
      if (name != NULL && name != _hdr->name
          && hdr->parent->header.version >= R_2007)
        FREE (name);
    }
  else
    printf ("\t<g id=\"symbol-%X\" >\n\t\t<!-- ? -->\n",
            abs_ref ? abs_ref : 0);

  obj = get_first_owned_entity (hdr);
  while (obj)
    {
      i += output_object (obj);
      obj = get_next_owned_entity (hdr, obj);
    }
  printf ("\t</g>\n");
  return i;
}

static void
output_SVG (dwg_data *dwg)
{
  unsigned int i, num_hdr_objs;
  int error;
  dwg_obj_block_control *_ctrl;
  dwg_object_ref *hdr;
  dwg_object_ref **hdr_refs;
  dwg_object_ref *ms = dwg_model_space_ref (dwg);
  dwg_object_ref *ps = dwg_paper_space_ref (dwg);

  double dx = dwg_model_x_max (dwg) - dwg_model_x_min (dwg);
  double dy = dwg_model_y_max (dwg) - dwg_model_y_min (dwg);
  double pdx = dwg->header_vars.PLIMMAX.x - dwg->header_vars.PLIMMIN.x;
  double pdy = dwg->header_vars.PLIMMAX.y - dwg->header_vars.PLIMMIN.y;
  double scale_x = dx / (pdx == 0.0 ? 1.0 : pdx);
  double scale_y = dy / (pdy == 0.0 ? 1.0 : pdy);
  scale = 25.4 / 72.0; // pt:mm TODO

  model_xmin = dwg_model_x_min (dwg);
  model_ymin = dwg_model_y_min (dwg);
  page_width = dx;
  page_height = dy;
  scale *= (scale_x > scale_y ? scale_x : scale_y);

  _ctrl = dwg_block_control (dwg);
  hdr_refs = dwg_obj_block_control_get_block_headers (_ctrl, &error);
  log_if_error ("block_control_get_block_headers");
  num_hdr_objs = dwg_obj_block_control_get_num_entries (_ctrl, &error);
  log_if_error ("block_control_get_num_entries");

  printf ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
          "<svg\n"
          "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
          "   xmlns=\"http://www.w3.org/2000/svg\"\n"
          "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
          "   version=\"1.1\"\n"
          "   width=\"%f\"\n"
          "   height=\"%f\"\n"
          ">\n",
          page_width, page_height);

  printf ("\t<defs>\n");
  for (i = 0; i < num_hdr_objs; i++)
    {
      hdr = hdr_refs[i];
      if (hdr == ms || hdr == ps)
        continue;
      output_BLOCK_HEADER (hdr_refs[i]);
    }
  printf ("\t</defs>\n");

  if (ps)
    i = output_BLOCK_HEADER (ps);
  if (!ps || !i)
    output_BLOCK_HEADER (ms);
  FREE (hdr_refs);

  printf ("</svg>\n");
}

int
main (int argc, char *argv[])
{
  int i = 1;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "help", 0, 0, 0 },
          { "version", 0, 0, 0 },
          { NULL, 0, NULL, 0 } };
#endif

  GC_INIT ();
  if (argc < 2)
    return usage ();

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, ":v::h", long_options, &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, ":v::hi")) != -1)
#endif
    {
      if (c == -1)
        break;
      switch (c)
        {
        case ':': // missing arg
          if (optarg && !strcmp (optarg, "v"))
            {
              opts = 1;
              break;
            }
          fprintf (stderr, "%s: option '-%c' requires an argument\n", argv[0],
                   optopt);
          break;
#ifdef HAVE_GETOPT_LONG
        case 0:
          /* This option sets a flag */
          if (!strcmp (long_options[option_index].name, "verbose"))
            {
              if (opts < 0 || opts > 9)
                return usage ();
#  if defined(USE_TRACING) && defined(HAVE_SETENV)
              {
                char v[2];
                *v = opts + '0';
                *(v + 1) = 0;
                setenv ("LIBREDWG_TRACE", v, 1);
              }
#  endif
              break;
            }
          if (!strcmp (long_options[option_index].name, "version"))
            return opt_version ();
          if (!strcmp (long_options[option_index].name, "help"))
            return help ();
          break;
#else
        case 'i':
          return opt_version ();
#endif
        case 'v': // support -v3 and -v
          i = (optind > 0 && optind < argc) ? optind - 1 : 1;
          if (!memcmp (argv[i], "-v", 2))
            {
              opts = argv[i][2] ? argv[i][2] - '0' : 1;
            }
          if (opts < 0 || opts > 9)
            return usage ();
#if defined(USE_TRACING) && defined(HAVE_SETENV)
          {
            char v[2];
            *v = opts + '0';
            *(v + 1) = 0;
            setenv ("LIBREDWG_TRACE", v, 1);
          }
#endif
          break;
        case 'h':
          return help ();
        case '?':
          fprintf (stderr, "%s: invalid option '-%c' ignored\n", argv[0],
                   optopt);
          break;
        default:
          return usage ();
        }
    }
  i = optind;
  if (i >= argc)
    return usage ();

  return test_SVG (argv[i]);
}

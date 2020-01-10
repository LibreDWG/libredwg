/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2019 Free Software Foundation, Inc.                   */
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
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRCASESTR
#  undef __DARWIN_C_LEVEL
#  define __DARWIN_C_LEVEL __DARWIN_C_FULL
#  ifndef __USE_GNU
#    define __USE_GNU
#  endif
#endif
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>

#include <dwg.h>
#include <dwg_api.h>
#include "bits.h"
#include "escape.h"
#include "geom.h"
#include "suffix.inc"

static int opts = 0;
Dwg_Data g_dwg;
double model_xmin, model_ymin;
double page_width, page_height, scale;

static void output_SVG (Dwg_Data *dwg);

static int
usage (void)
{
  printf ("\nUsage: dwg2SVG [-v[0-9]] DWGFILE >SVGFILE\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwg2SVG %s\n", PACKAGE_VERSION);
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dwg2SVG [OPTION]... DWGFILE >SVGFILE\n");
  printf ("Converts some 2D elements of the DWG to a SVG.\n"
          "\n");
  // TODO: -p for paperspace only, -m for modelspace only
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

static bool
isnan_2BD (BITCODE_2BD pt)
{
  return isnan (pt.x) || isnan (pt.y);
}

static bool
isnan_2pt (dwg_point_2d pt)
{
  return isnan (pt.x) || isnan (pt.y);
}

static bool
isnan_3BD (BITCODE_3BD pt)
{
  return isnan (pt.x) || isnan (pt.y) || isnan (pt.z);
}

static void
output_TEXT (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_TEXT *text = obj->tio.entity->tio.TEXT;
  char *escaped;
  const char *fontfamily;
  BITCODE_H style_ref = text->style;
  Dwg_Object *o = style_ref ? dwg_ref_object_silent (dwg, style_ref) : NULL;
  Dwg_Object_STYLE *style = o ? o->tio.object->tio.STYLE : NULL;
  BITCODE_2DPOINT pt;

  if (!text->text_value)
    return;
  if (isnan_2BD (text->insertion_pt) || isnan_3BD (text->extrusion))
    return;
  if (dwg->header.version >= R_2007)
    escaped = htmlwescape ((BITCODE_TU)text->text_value);
  else
    escaped = htmlescape (text->text_value, (int)dwg->header.codepage);

  if (style && o->fixedtype == DWG_TYPE_STYLE && style->font_name
      && *style->font_name
#ifdef HAVE_STRCASESTR
      && strcasestr (style->font_name, ".ttf")
#else
      && (strstr (style->font_name, ".ttf")
          || strstr (style->font_name, ".TTF"))
#endif
  )
    {
#ifdef HAVE_STRCASESTR
      if (strcasestr (style->font_name, "Arial"))
#else
      if ((strstr (style->font_name, "arial"))
          || strstr (style->font_name, "Arial"))
#endif
        {
          fontfamily = "Arial";
        }
      else
        fontfamily = "Verdana";
    }
  else
    fontfamily = "Courier";

  transform_OCS_2d (&pt, text->insertion_pt, text->extrusion);
  printf ("\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" "
          "font-family=\"%s\" font-size=\"%f\" fill=\"blue\">%s</text>\n",
          obj->index, transform_X (pt.x), transform_Y (pt.y), fontfamily,
          text->height /* fontsize */, escaped ? escaped : "");
  free (escaped);
}

static void
output_LINE (Dwg_Object *obj)
{
  Dwg_Entity_LINE *line = obj->tio.entity->tio.LINE;
  BITCODE_3DPOINT start, end;

  if (isnan_3BD (line->start) || isnan_3BD (line->end)
      || isnan_3BD (line->extrusion))
    return;
  transform_OCS (&start, line->start, line->extrusion);
  transform_OCS (&end, line->end, line->extrusion);
  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f %f,%f\" "
          "style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
          obj->index, transform_X (start.x), transform_Y (start.y),
          transform_X (end.x), transform_Y (end.y));
}

static void
output_CIRCLE (Dwg_Object *obj)
{
  Dwg_Entity_CIRCLE *circle = obj->tio.entity->tio.CIRCLE;
  BITCODE_3DPOINT center;

  if (isnan_3BD (circle->center) || isnan_3BD (circle->extrusion)
      || isnan (circle->radius))
    return;
  transform_OCS (&center, circle->center, circle->extrusion);
  printf ("\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" "
          "fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
          obj->index, transform_X (center.x), transform_Y (center.y),
          circle->radius);
}

static void
output_ARC (Dwg_Object *obj)
{
  Dwg_Entity_ARC *arc = obj->tio.entity->tio.ARC;
  BITCODE_3DPOINT center;
  double x_start, y_start, x_end, y_end;
  int large_arc;

  if (isnan_3BD (arc->center) || isnan_3BD (arc->extrusion)
      || isnan (arc->radius) || isnan (arc->start_angle)
      || isnan (arc->start_angle))
    return;
  transform_OCS (&center, arc->center, arc->extrusion);

  x_start = center.x + arc->radius * cos (arc->start_angle);
  y_start = center.y + arc->radius * sin (arc->start_angle);
  x_end = center.x + arc->radius * cos (arc->end_angle);
  y_end = center.y + arc->radius * sin (arc->end_angle);
  // Assuming clockwise arcs.
  large_arc = (arc->end_angle - arc->start_angle < M_PI) ? 0 : 1;

  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" "
          "fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n",
          obj->index, transform_X (x_start), transform_Y (y_start),
          arc->radius, arc->radius, large_arc, transform_X (x_end),
          transform_Y (y_end), 0.1);
}

static void
output_POLYLINE_2D (Dwg_Object *obj)
{
  int error;
  Dwg_Entity_POLYLINE_2D *pline = obj->tio.entity->tio.POLYLINE_2D;
  BITCODE_RL numpts = dwg_object_polyline_2d_get_numpoints (obj, &error);

  if (numpts && !error)
    {
      BITCODE_2DPOINT pt, ptin;
      dwg_point_2d *pts = dwg_object_polyline_2d_get_points (obj, &error);
      BITCODE_RL j;

      if (error || isnan_2pt (pts[0]) || isnan_3BD (pline->extrusion))
        return;
      ptin.x = pts[0].x;
      ptin.y = pts[0].y;
      transform_OCS_2d (&pt, ptin, pline->extrusion);
      printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f", obj->index,
              transform_X (pt.x), transform_Y (pt.y));
      // TODO curve_types, C for Bezier
      for (j = 1; j < numpts; j++)
        {
          ptin.x = pts[j].x;
          ptin.y = pts[j].y;
          if (isnan_2BD (ptin))
            continue;
          transform_OCS_2d (&pt, ptin, pline->extrusion);
          // TODO bulge -> arc, widths
          printf (" L %f,%f", transform_X (pt.x), transform_Y (pt.y));
        }
      if (pline->flag & 1) // closed
        printf (" Z");
      printf ("\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n");
      free (pts);
    }
}

static void
output_LWPOLYLINE (Dwg_Object *obj)
{
  int error;
  Dwg_Entity_LWPOLYLINE *pline = obj->tio.entity->tio.LWPOLYLINE;
  BITCODE_RL numpts = dwg_ent_lwpline_get_numpoints (pline, &error);

  if (numpts && !error)
    {
      BITCODE_2DPOINT pt, ptin;
      dwg_point_2d *pts = dwg_ent_lwpline_get_points (pline, &error);
      BITCODE_RL j;

      if (error || isnan_2pt (pts[0]) || isnan_3BD (pline->extrusion))
        return;
      ptin.x = pts[0].x;
      ptin.y = pts[0].y;
      transform_OCS_2d (&pt, ptin, pline->extrusion);
      printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f", obj->index,
              transform_X (pt.x), transform_Y (pt.y));
      // TODO curve_types, C for Bezier
      for (j = 1; j < numpts; j++)
        {
          ptin.x = pts[j].x;
          ptin.y = pts[j].y;
          if (isnan_2BD (ptin))
            continue;
          transform_OCS_2d (&pt, ptin, pline->extrusion);
          // TODO bulge -> arc, widths
          printf (" L %f,%f", transform_X (pt.x), transform_Y (pt.y));
        }
      if (pline->flag & 512) // closed
        printf (" Z");
      printf ("\" style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n");
      free (pts);
    }
}

static void
output_INSERT (Dwg_Object *obj)
{
  Dwg_Entity_INSERT *insert = obj->tio.entity->tio.INSERT;
  if (insert->block_header && insert->block_header->handleref.value)
    {
      BITCODE_3DPOINT ins_pt;
      if (isnan_3BD (insert->ins_pt) || isnan_3BD (insert->extrusion)
          || isnan (insert->rotation) || isnan_3BD (insert->scale))
        return;
      transform_OCS (&ins_pt, insert->ins_pt, insert->extrusion);
      printf ("\t<use id=\"dwg-object-%d\" transform=\"translate(%f %f) "
              "rotate(%f) scale(%f %f)\" xlink:href=\"#symbol-%lX\" />"
              "<!-- block_header->handleref: " FORMAT_H " -->\n",
              obj->index, transform_X (ins_pt.x), transform_Y (ins_pt.y),
              (180.0 / M_PI) * insert->rotation, insert->scale.x,
              insert->scale.y, insert->block_header->absolute_ref,
              ARGS_H (insert->block_header->handleref));
    }
  else
    {
      printf ("\n\n<!-- WRONG INSERT(" FORMAT_H ") -->\n",
              ARGS_H (obj->handle));
    }
}

static void
output_object (Dwg_Object *obj)
{
  if (!obj)
    {
      fprintf (stderr, "object is NULL\n");
      return;
    }

  switch (obj->type)
    {
    case DWG_TYPE_INSERT:
      output_INSERT (obj);
      break;
    case DWG_TYPE_LINE:
      output_LINE (obj);
      break;
    case DWG_TYPE_CIRCLE:
      output_CIRCLE (obj);
      break;
    case DWG_TYPE_TEXT:
      output_TEXT (obj);
      break;
    case DWG_TYPE_ARC:
      output_ARC (obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      output_POLYLINE_2D (obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      output_LWPOLYLINE (obj);
      break;
    default:
      // ignored
      break;
    }
}

static void
output_BLOCK_HEADER (Dwg_Object_Ref *ref)
{
  Dwg_Object *obj;
  Dwg_Object_BLOCK_HEADER *hdr;

  if (!ref) // silently ignore empty pspaces
    return;
  if (!ref->obj)
    return;
  obj = ref->obj;
  if (obj->type != DWG_TYPE_BLOCK_HEADER)
    {
      fprintf (stderr, "Argument not a BLOCK_HEADER reference\n");
      return;
    }
  /* TODO: Review.  (This check avoids a segfault, but it is
     still unclear whether or not the condition is valid.)  */
  if (!obj->tio.object)
    {
      fprintf (stderr, "Found null obj->tio.object\n");
      return;
    }

  hdr = obj->tio.object->tio.BLOCK_HEADER;
  if (hdr->name)
    {
      char *escaped;
      Dwg_Data *dwg = obj->parent;
      if (dwg->header.version >= R_2007)
        escaped = htmlwescape ((BITCODE_TU)hdr->name);
      else
        escaped = htmlescape (hdr->name, dwg->header.codepage);
      printf ("\t<g id=\"symbol-%lX\" >\n\t\t<!-- %s -->\n", ref->absolute_ref,
              escaped ? escaped : "");
      if (escaped)
        free (escaped);
    }

  obj = get_first_owned_entity (ref->obj);
  while (obj)
    {
      output_object (obj);
      obj = get_next_owned_entity (ref->obj, obj);
    }

  printf ("\t</g>\n");
}

static void
output_SVG (Dwg_Data *dwg)
{
  BITCODE_BS i;
  Dwg_Object *obj;
  Dwg_Object_Ref *ref;
  Dwg_Object_BLOCK_CONTROL *block_control;
  double dx, dy;

  model_xmin = dwg_model_x_min (dwg);
  model_ymin = dwg_model_y_min (dwg);

  dx = (dwg_model_x_max (dwg) - dwg_model_x_min (dwg));
  dy = (dwg_model_y_max (dwg) - dwg_model_y_min (dwg));
  // double scale_x = dx / (dwg_page_x_max(dwg) - dwg_page_x_min(dwg));
  // double scale_y = dy / (dwg_page_y_max(dwg) - dwg_page_y_min(dwg));
  // scale = 25.4 / 72; // pt:mm
  if (isnan (dx))
    dx = 100.0;
  if (isnan (dy))
    dy = 100.0;
  page_width = dx;
  page_height = dy;
  // scale *= (scale_x > scale_y ? scale_x : scale_y);

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

  if ((ref = dwg_model_space_ref (dwg)))
    output_BLOCK_HEADER (ref);
  if ((ref = dwg_paper_space_ref (dwg)))
    output_BLOCK_HEADER (ref);
  printf ("\t<defs>\n");
  for (i = 0; i < dwg->block_control.num_entries; i++)
    {
      if ((ref = dwg->block_control.entries[i]))
        output_BLOCK_HEADER (ref);
    }
  printf ("\t</defs>\n");
  printf ("</svg>\n");
}

int
main (int argc, char *argv[])
{
  int error;
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

  memset (&g_dwg, 0, sizeof (Dwg_Data));
  g_dwg.opts = opts;
  error = dwg_read_file (argv[i], &g_dwg);
  if (error < DWG_ERR_CRITICAL)
    output_SVG (&g_dwg);

  dwg_free (&g_dwg);
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}

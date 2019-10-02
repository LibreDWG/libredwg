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
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <getopt.h>

#include <dwg.h>
#include "bits.h"
#include "escape.h"
#include "suffix.inc"

#ifndef M_PI
#  define M_PI 3.14159265358979323846264338327950288
#endif

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

static void
output_TEXT (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_TEXT *text = obj->tio.entity->tio.TEXT;
  char *escaped;

  if (dwg->header.version >= R_2007)
    escaped = htmlwescape ((BITCODE_TU)text->text_value);
  else
    escaped = htmlescape (text->text_value, (int)dwg->header.codepage);

  printf ("\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" "
          "font-family=\"Verdana\" font-size=\"%f\" fill=\"blue\">%s</text>\n",
          obj->index, transform_X (text->insertion_pt.x),
          transform_Y (text->insertion_pt.y), text->height /* fontsize */,
          escaped ? escaped : "");
  free (escaped);
}

static void
output_LINE (Dwg_Object *obj)
{
  Dwg_Entity_LINE *line = obj->tio.entity->tio.LINE;
  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f %f,%f\" "
          "style=\"fill:none;stroke:blue;stroke-width:0.1px\" />\n",
          obj->index, transform_X (line->start.x), transform_Y (line->start.y),
          transform_X (line->end.x), transform_Y (line->end.y));
}

static void
output_CIRCLE (Dwg_Object *obj)
{
  Dwg_Entity_CIRCLE *circle = obj->tio.entity->tio.CIRCLE;
  printf ("\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\" "
          "fill=\"none\" stroke=\"blue\" stroke-width=\"0.1px\" />\n",
          obj->index, transform_X (circle->center.x),
          transform_Y (circle->center.y), circle->radius);
}

static void
output_ARC (Dwg_Object *obj)
{
  Dwg_Entity_ARC *arc = obj->tio.entity->tio.ARC;
  double x_start = arc->center.x + arc->radius * cos (arc->start_angle);
  double y_start = arc->center.y + arc->radius * sin (arc->start_angle);
  double x_end = arc->center.x + arc->radius * cos (arc->end_angle);
  double y_end = arc->center.y + arc->radius * sin (arc->end_angle);
  // Assuming clockwise arcs.
  int large_arc = (arc->end_angle - arc->start_angle < 3.1415) ? 0 : 1;

  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d 0 %f,%f\" "
          "fill=\"none\" stroke=\"blue\" stroke-width=\"%f\" />\n",
          obj->index, transform_X (x_start), transform_Y (y_start),
          arc->radius, arc->radius, large_arc, transform_X (x_end),
          transform_Y (y_end), 0.1);
}

static void
output_INSERT (Dwg_Object *obj)
{
  Dwg_Entity_INSERT *insert = obj->tio.entity->tio.INSERT;
  if (insert->block_header && insert->block_header->handleref.value)
    {
      printf ("\t<use id=\"dwg-object-%d\" transform=\"translate(%f %f) "
              "rotate(%f) scale(%f %f)\" xlink:href=\"#symbol-%lX\" />"
              "<!-- block_header->handleref: " FORMAT_H " -->\n",
              obj->index, transform_X (insert->ins_pt.x),
              transform_Y (insert->ins_pt.y),
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

  if (obj->type == DWG_TYPE_INSERT)
    output_INSERT (obj);
  else if (obj->type == DWG_TYPE_LINE)
    output_LINE (obj);
  else if (obj->type == DWG_TYPE_CIRCLE)
    output_CIRCLE (obj);
  else if (obj->type == DWG_TYPE_TEXT)
    output_TEXT (obj);
  else if (obj->type == DWG_TYPE_ARC)
    output_ARC (obj);
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

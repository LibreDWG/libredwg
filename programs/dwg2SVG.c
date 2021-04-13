/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2025 Free Software Foundation, Inc.                   */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2SVG.c: convert a DWG to SVG
 * written by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 *
 * TODO: all entities: 3DSOLID, SHAPE, ARC_DIMENSION, ATTRIB, DIMENSION*,
 *         *SURFACE, GEOPOSITIONMARKER/CAMERA/LIGHT, HATCH, HELIX,
 *         IMAGE/WIPEOUT/UNDERLAY, LEADER, MESH, MINSERT, MLINE, MTEXT,
 * MULTILEADER, OLE2FRAME, OLEFRAME, POLYLINE_3D, POLYLINE_MESH,
 * POLYLINE_PFACE, RAY, XLINE, SPLINE, TABLE, TOLERANCE, VIEWPORT?
 *       common_entity_data: ltype, ltype_scale.
 *       PLINE: widths, bulges.
 */

#define _GNU_SOURCE /* make musl expose strcasestr */
#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_STRCASESTR
#  undef __DARWIN_C_LEVEL
#  define __DARWIN_C_LEVEL __DARWIN_C_FULL
#  ifndef __USE_GNU
#    define __USE_GNU
#  endif
#  ifndef __BSD_VISIBLE
#    define __BSD_VISIBLE 1
#  endif
#endif
#include <string.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include "my_getopt.h"
#include <math.h>
#ifdef HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#include <dwg.h>
#include <dwg_api.h>
#include "bits.h"
#include "common.h"
#include "escape.h"
#include "geom.h"
#include "suffix.inc"
#include "my_getopt.h"

static int opts = 0;
static int mspace = 0; // only mspace, even when pspace is defined
Dwg_Data g_dwg;
double model_xmin, model_ymin, model_xmax, model_ymax;
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
  printf ("           --mspace         only model-space, no paper-space\n");
  printf ("           --force-free     force free\n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
#else
  printf ("  -v[0-9]     verbosity\n");
  printf ("  -m          only model-space, no paper-space\n");
  printf ("  -h          display this help and exit\n");
  printf ("  -i          output version information and exit\n"
          "\n");
#endif
  printf ("GNU LibreDWG online manual: "
          "<https://www.gnu.org/software/libredwg/>\n");
  return 0;
}

static double
transform_ANGLE (double angle)
{
  return 180 - angle;
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

static bool
entity_invisible (Dwg_Object *obj)
{
  BITCODE_BS invisible = obj->tio.entity->invisible;
  Dwg_Object *layer;
  Dwg_Object_LAYER *_obj;
  if (invisible)
    return true;

  if (!obj->tio.entity->layer || !obj->tio.entity->layer->obj)
    return false;
  layer = obj->tio.entity->layer->obj;
  if (layer->fixedtype != DWG_TYPE_LAYER)
    return false;
  _obj = layer->tio.object->tio.LAYER;
  // pre-r13 it is set if the layer color is negative
  return _obj->off == 1 ? true : false;
}

static double
entity_lweight (Dwg_Object_Entity *ent)
{
  // TODO: resolve BYLAYER 256, see above.
  // stroke-width:%0.1fpx. 100th of a mm
  int lw = dxf_cvt_lweight (ent->linewt);
  return lw < 0 ? 0.1 : (double)(lw * 0.001);
}

static char *
entity_color (Dwg_Object_Entity *ent)
{
  // TODO: alpha?
  if (ent->color.index >= 8 && ent->color.index < 256)
    {
      const Dwg_RGB_Palette *palette = dwg_rgb_palette ();
      const Dwg_RGB_Palette *rgb = &palette[ent->color.index];
      char *s = (char *)MALLOC (8);
      sprintf (s, "#%02x%02x%02x", rgb->r, rgb->g, rgb->b);
      return s;
    }
  else if (ent->color.flag & 0x80 && !(ent->color.flag & 0x40))
    {
      char *s = (char *)MALLOC (8);
      sprintf (s, "#%06x", ent->color.rgb & 0x00ffffff);
      return s;
    }
  else
    switch (ent->color.index)
      {
      case 1:
        return (char *)"red";
      case 2:
        return (char *)"yellow";
      case 3:
        return (char *)"green";
      case 4:
        return (char *)"cyan";
      case 5:
        return (char *)"blue";
      case 6:
        return (char *)"magenta";
      case 7:
        return (char *)"white";
      case 0:   // ByBlock
      case 256: // ByLayer
      default:
        return (char *)"black";
      }
}

static void
common_entity (Dwg_Object_Entity *ent)
{
  double lweight;
  char *color;
  lweight = entity_lweight (ent);
  color = entity_color (ent);
  printf ("      style=\"fill:none;stroke:%s;stroke-width:%.1fpx\" />\n",
          color, lweight);
  if (*color == '#')
    FREE (color);
}

// TODO: MTEXT
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

  if (!text->text_value || entity_invisible (obj))
    return;
  if (isnan_2BD (text->ins_pt) || isnan_3BD (text->extrusion))
    return;
  if (dwg->header.version >= R_2007)
    escaped = htmlwescape ((BITCODE_TU)text->text_value);
  else
    escaped = htmlescape (text->text_value, dwg->header.codepage);

  if (style && o->fixedtype == DWG_TYPE_STYLE && style->font_file
      && *style->font_file
#ifdef HAVE_STRCASESTR
      && strcasestr (style->font_file, ".ttf")
#else
      && (strstr (style->font_file, ".ttf")
          || strstr (style->font_file, ".TTF"))
#endif
  )
    {
#ifdef HAVE_STRCASESTR
      if (strcasestr (style->font_file, "Arial"))
#else
      if ((strstr (style->font_file, "arial"))
          || strstr (style->font_file, "Arial"))
#endif
        {
          fontfamily = "Arial";
        }
      else
        fontfamily = "Verdana";
    }
  else
    fontfamily = "Courier";

  transform_OCS_2d (&pt, text->ins_pt, text->extrusion);
  printf ("\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" "
          "font-family=\"%s\" font-size=\"%f\" fill=\"%s\">%s</text>\n",
          obj->index, transform_X (pt.x), transform_Y (pt.y), fontfamily,
          text->height /* fontsize */, entity_color (obj->tio.entity),
          escaped ? escaped : "");
  if (escaped)
    FREE (escaped);
}

static void
output_LINE (Dwg_Object *obj)
{
  Dwg_Entity_LINE *line = obj->tio.entity->tio.LINE;
  BITCODE_3DPOINT start, end;

  if (isnan_3BD (line->start) || isnan_3BD (line->end)
      || isnan_3BD (line->extrusion) || entity_invisible (obj))
    return;
  transform_OCS (&start, line->start, line->extrusion);
  transform_OCS (&end, line->end, line->extrusion);
  printf ("\t<!-- line-%d -->\n", obj->index);
  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f L %f,%f\"\n\t", obj->index,
          transform_X (start.x), transform_Y (start.y), transform_X (end.x),
          transform_Y (end.y));
  common_entity (obj->tio.entity);
}

static void
output_XLINE (Dwg_Object *obj)
{
  Dwg_Entity_XLINE *xline = obj->tio.entity->tio.XLINE;
  BITCODE_3DPOINT invvec;
  static BITCODE_3DPOINT box[2];
  int sign[3];
  double txmin, txmax, tymin, tymax, tzmin, tzmax;

  if (isnan_3BD (xline->point) || isnan_3BD (xline->vector)
      || entity_invisible (obj))
    return;

  invvec.x = 1.0 / xline->vector.x;
  invvec.y = 1.0 / xline->vector.y;
  invvec.z = 1.0 / xline->vector.z;
  sign[0] = (invvec.x < 0.0);
  sign[1] = (invvec.y < 0.0);
  sign[2] = (invvec.z < 0.0);
  box[0].x = model_xmin;
  box[0].y = model_ymin;
  box[1].x = model_xmax;
  box[1].y = model_ymin;
  printf ("\t<!-- xline-%d -->\n", obj->index);

  // untested!
  /* intersect xline with model_xmin, model_ymin, model_xmax, model_ymax */
  txmin = (box[sign[0]].x - xline->point.x) * invvec.x;
  txmax = (box[1 - sign[0]].x - xline->point.x) * invvec.x;
  tymin = (box[sign[1]].x - xline->point.y) * invvec.y;
  tymax = (box[1 - sign[1]].x - xline->point.y) * invvec.y;
  if ((txmin > tymax) || (tymin > txmax))
    return;
  if (tymin > txmin)
    txmin = tymin;
  if (tymax > txmax)
    txmax = tymax;
  tzmin = (box[sign[0]].z - xline->point.z) * invvec.z;
  tzmax = (box[1 - sign[0]].z - xline->point.z) * invvec.z;
  if ((txmin > tzmax) || (tzmin > txmax))
    return;

  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f L %f,%f\"\n\t", obj->index,
          txmin, tymin, txmax, tymax);
  common_entity (obj->tio.entity);
}

static void
output_RAY (Dwg_Object *obj)
{
  Dwg_Entity_XLINE *xline = obj->tio.entity->tio.RAY;
  BITCODE_3DPOINT point, invvec;
  static BITCODE_3DPOINT box[2];
  int sign[3];
  double txmin, txmax, tymin, tymax, tzmin, tzmax;

  if (isnan_3BD (xline->point) || isnan_3BD (xline->vector)
      || entity_invisible (obj))
    return;

  invvec.x = 1.0 / xline->vector.x;
  invvec.y = 1.0 / xline->vector.y;
  invvec.z = 1.0 / xline->vector.z;
  sign[0] = (invvec.x < 0.0);
  sign[1] = (invvec.y < 0.0);
  sign[2] = (invvec.z < 0.0);
  box[0].x = model_xmin;
  box[0].y = model_ymin;
  box[1].x = model_xmax;
  box[1].y = model_ymin;
  printf ("\t<!-- ray-%d -->\n", obj->index);

  // untested!
  /* intersect ray from point with box (model_xmin, model_ymin, model_xmax,
   * model_ymax) */
  txmin = (box[sign[0]].x - xline->point.x) * invvec.x;
  txmax = (box[1 - sign[0]].x - xline->point.x) * invvec.x;
  tymin = (box[sign[1]].x - xline->point.y) * invvec.y;
  tymax = (box[1 - sign[1]].x - xline->point.y) * invvec.y;
  if ((txmin > tymax) || (tymin > txmax))
    return;
  if (tymin > txmin)
    txmin = tymin;
  if (tymax > txmax)
    txmax = tymax;
  point.x = (xline->point.x > txmax) ? txmax : xline->point.x;
  if (point.x < txmin)
    point.x = txmin;
  point.y = (xline->point.y > tymax) ? tymax : xline->point.y;
  if (point.y < tymin)
    point.y = tymin;

  tzmin = (box[sign[0]].z - xline->point.z) * invvec.z;
  tzmax = (box[1 - sign[0]].z - xline->point.z) * invvec.z;
  if ((txmin > tzmax) || (tzmin > txmax))
    return;

  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f L %f,%f\"\n\t", obj->index,
          point.x, point.y, txmax, tymax);
  common_entity (obj->tio.entity);
}

static void
output_CIRCLE (Dwg_Object *obj)
{
  Dwg_Entity_CIRCLE *circle = obj->tio.entity->tio.CIRCLE;
  BITCODE_3DPOINT center;

  if (isnan_3BD (circle->center) || isnan_3BD (circle->extrusion)
      || isnan (circle->radius) || entity_invisible (obj))
    return;
  transform_OCS (&center, circle->center, circle->extrusion);
  printf ("\t<!-- circle-%d -->\n", obj->index);
  printf ("\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"%f\"\n\t",
          obj->index, transform_X (center.x), transform_Y (center.y),
          circle->radius);
  common_entity (obj->tio.entity);
}

// CIRCLE with radius 0.1
static void
output_POINT (Dwg_Object *obj)
{
  Dwg_Entity_POINT *point = obj->tio.entity->tio.POINT;
  BITCODE_3DPOINT pt, pt1;

  pt.x = point->x;
  pt.y = point->y;
  pt.z = point->z;
  if (isnan_3BD (pt) || isnan_3BD (point->extrusion) || entity_invisible (obj))
    return;
  transform_OCS (&pt1, pt, point->extrusion);
  printf ("\t<!-- point-%d -->\n", obj->index);
  printf ("\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"0.1\"\n\t",
          obj->index, transform_X (pt1.x), transform_Y (pt1.y));
  common_entity (obj->tio.entity);
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
      || isnan (arc->end_angle) || entity_invisible (obj))
    return;
  transform_OCS (&center, arc->center, arc->extrusion);

  x_start = center.x + arc->radius * cos (arc->start_angle);
  y_start = center.y + arc->radius * sin (arc->start_angle);
  x_end = center.x + arc->radius * cos (arc->end_angle);
  y_end = center.y + arc->radius * sin (arc->end_angle);
  // Assuming clockwise arcs.
  large_arc = (arc->end_angle - arc->start_angle < M_PI) ? 0 : 1;

  printf ("\t<!-- arc-%d -->\n", obj->index);
  printf (
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d,0 %f,%f\"\n\t",
      obj->index, transform_X (x_start), transform_Y (y_start), arc->radius,
      arc->radius, large_arc, transform_X (x_end), transform_Y (y_end));
  common_entity (obj->tio.entity);
}

// FIXME
static void
output_ELLIPSE (Dwg_Object *obj)
{
  Dwg_Entity_ELLIPSE *ell = obj->tio.entity->tio.ELLIPSE;
  BITCODE_2DPOINT radius;
  double angle_rad, angle_dec;
  // BITCODE_3DPOINT center, sm_axis;
  // double x_start, y_start, x_end, y_end;

  if (isnan_3BD (ell->center) || isnan_3BD (ell->extrusion)
      || isnan_3BD (ell->sm_axis) || isnan (ell->axis_ratio)
      || isnan (ell->start_angle) || isnan (ell->end_angle)
      || entity_invisible (obj))
    return;
  /* The 2 points are already WCS */
  // transform_OCS (&center, ell->center, ell->extrusion);
  // transform_OCS (&sm_axis, ell->sm_axis, ell->extrusion);
  radius.x = sqrt (ell->sm_axis.x * ell->sm_axis.x + ell->sm_axis.y * ell->sm_axis.y);
  radius.y = radius.x * ell->axis_ratio;

  /*
  x_start = ell->center.x + radius.x * cos (ell->start_angle);
  y_start = ell->center.y + radius.y * sin (ell->start_angle);
  x_end = ell->center.x + radius.x * cos (ell->end_angle);
  y_end = ell->center.y + radius.y * sin (ell->end_angle);
  */

  angle_rad = atan2(ell->sm_axis.y, ell->sm_axis.x);
  angle_dec = angle_rad * 180.0 / M_PI;

  // TODO: start,end_angle => pathLength
  printf ("\t<!-- ellipse-%d -->\n", obj->index);
  printf ("\t<!-- sm_axis=(%f,%f,%f) axis_ratio=%f start_angle=%f "
          "end_angle=%f-->\n",
          ell->sm_axis.x, ell->sm_axis.y, ell->sm_axis.z, ell->axis_ratio,
          ell->start_angle, ell->end_angle);
  printf ("\t<ellipse id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" rx=\"%f\" "
          "ry=\"%f\" transform=\"rotate(%f %f %f)\"\n\t",
          obj->index, transform_X (ell->center.x), transform_Y (ell->center.y),
          radius.x, radius.y,
          transform_ANGLE (angle_dec), transform_X (ell->center.x), transform_Y (ell->center.y));
  common_entity (obj->tio.entity);
}

// untested
static void
output_SOLID (Dwg_Object *obj)
{
  Dwg_Entity_SOLID *sol = obj->tio.entity->tio.SOLID;
  BITCODE_2DPOINT c1, c2, c3, c4;
  BITCODE_2DPOINT s1, s2, s3, s4;

  memcpy (&s1, &sol->corner1, sizeof s1);
  memcpy (&s2, &sol->corner2, sizeof s1);
  memcpy (&s3, &sol->corner3, sizeof s1);
  memcpy (&s4, &sol->corner4, sizeof s1);
  if (isnan_2BD (s1) || isnan_2BD (s2) || isnan_2BD (s3) || isnan_2BD (s4)
      || entity_invisible (obj))
    return;
  transform_OCS_2d (&c1, s1, sol->extrusion);
  transform_OCS_2d (&c2, s2, sol->extrusion);
  transform_OCS_2d (&c3, s3, sol->extrusion);
  transform_OCS_2d (&c4, s4, sol->extrusion);

  printf ("\t<!-- solid-%d -->\n", obj->index);
  printf ("\t<polygon id=\"dwg-object-%d\" "
          "points=\"%f,%f %f,%f %f,%f %f,%f\"\n\t",
          obj->index, transform_X (c1.x), transform_Y (c1.y),
          transform_X (c2.x), transform_Y (c2.y), transform_X (c3.x),
          transform_Y (c3.y), transform_X (c4.x), transform_Y (c4.y));
  common_entity (obj->tio.entity);
}

// untested
static void
output_3DFACE (Dwg_Object *obj)
{
  Dwg_Entity__3DFACE *ent = obj->tio.entity->tio._3DFACE;

  if (isnan_3BD (ent->corner1) || isnan_3BD (ent->corner2)
      || isnan_3BD (ent->corner3) || isnan_3BD (ent->corner4)
      || entity_invisible (obj))
    return;
  printf ("\t<!-- 3dface-%d -->\n", obj->index);
  if (ent->invis_flags)
    {
      // move to 1
      printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f", obj->index,
              ent->corner1.x, ent->corner1.y);
      printf (" %s %f,%f", ent->invis_flags & 1 ? "M" : "L", ent->corner2.x,
              ent->corner2.y);
      printf (" %s %f,%f", ent->invis_flags & 2 ? "M" : "L", ent->corner3.x,
              ent->corner3.y);
      printf (" %s %f,%f", ent->invis_flags & 4 ? "M" : "L", ent->corner4.x,
              ent->corner4.y);
      printf (" %s %f,%f\"\n\t", ent->invis_flags & 8 ? "M" : "L",
              ent->corner1.x, ent->corner1.y);
    }
  else
    printf ("\t<polygon id=\"dwg-object-%d\" "
            "points=\"%f,%f %f,%f %f,%f %f,%f\"\n\t",
            obj->index, ent->corner1.x, ent->corner1.y, ent->corner2.x,
            ent->corner2.y, ent->corner3.x, ent->corner3.y, ent->corner4.x,
            ent->corner4.y);
  common_entity (obj->tio.entity);
}

static void
output_POLYLINE_2D (Dwg_Object *obj)
{
  int error;
  Dwg_Entity_POLYLINE_2D *pline = obj->tio.entity->tio.POLYLINE_2D;
  BITCODE_RL numpts;

  if (entity_invisible (obj))
    return;
  numpts = dwg_object_polyline_2d_get_numpoints (obj, &error);
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
      printf ("\t<!-- polyline_2d-%d -->\n", obj->index);
      printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f", obj->index,
              transform_X (pt.x), transform_Y (pt.y));
      // TODO curve_types, C for Bezier https://svgwg.org/specs/paths/#PathData
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
      printf ("\"\n\t");
      common_entity (obj->tio.entity);
      FREE (pts);
    }
}

static void
output_LWPOLYLINE (Dwg_Object *obj)
{
  int error;
  Dwg_Entity_LWPOLYLINE *pline = obj->tio.entity->tio.LWPOLYLINE;
  BITCODE_RL numpts;

  if (entity_invisible (obj))
    return;
  numpts = dwg_ent_lwpline_get_numpoints (pline, &error);
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
      printf ("\t<!-- lwpolyline-%d -->\n", obj->index);
      printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f", obj->index,
              transform_X (pt.x), transform_Y (pt.y));
      // TODO curve_types, C for Bezier https://svgwg.org/specs/paths/#PathData
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
      printf ("\"\n\t");
      common_entity (obj->tio.entity);
      FREE (pts);
    }
}

// TODO: MINSERT
static void
output_INSERT (Dwg_Object *obj)
{
  Dwg_Entity_INSERT *insert = obj->tio.entity->tio.INSERT;
  if (entity_invisible (obj))
    return;
  if (insert->block_header && insert->block_header->handleref.value)
    {
      BITCODE_3DPOINT ins_pt;
      if (isnan_3BD (insert->ins_pt) || isnan_3BD (insert->extrusion)
          || isnan (insert->rotation) || isnan_3BD (insert->scale))
        return;
      transform_OCS (&ins_pt, insert->ins_pt, insert->extrusion);
      printf ("\t<!-- insert-%d -->\n", obj->index);
      printf ("\t<use id=\"dwg-object-%d\" transform=\"translate(%f %f) "
              "rotate(%f) scale(%f %f)\" xlink:href=\"#symbol-" FORMAT_HV
              "\" />"
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

static int
output_object (Dwg_Object *obj)
{
  int num = 1;
  if (!obj)
    {
      fprintf (stderr, "object is NULL\n");
      return 0;
    }

  switch (obj->fixedtype)
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
    case DWG_TYPE_POINT:
      output_POINT (obj);
      break;
    case DWG_TYPE_ELLIPSE:
      output_ELLIPSE (obj);
      break;
    case DWG_TYPE_SOLID:
      output_SOLID (obj);
      break;
    case DWG_TYPE__3DFACE:
      output_3DFACE (obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      output_POLYLINE_2D (obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      output_LWPOLYLINE (obj);
      break;
    case DWG_TYPE_RAY:
      output_RAY (obj);
      break;
    case DWG_TYPE_XLINE:
      output_XLINE (obj);
      break;
    case DWG_TYPE_SEQEND:
    case DWG_TYPE_VIEWPORT:
      break;
    default:
      num = 0;
      if (obj->supertype == DWG_SUPERTYPE_ENTITY)
        fprintf (stderr, "%s ignored\n", obj->name);
      // all other non-graphical objects are silently ignored
      break;
    }
  return num;
}

static int
output_BLOCK_HEADER (Dwg_Object_Ref *ref)
{
  Dwg_Object *obj;
  Dwg_Object_BLOCK_HEADER *hdr;
  int is_g = 0;
  int num = 0;

  if (!ref) // silently ignore empty pspaces
    return 0;
  if (!ref->obj)
    return 0;
  obj = ref->obj;
  if (obj->type != DWG_TYPE_BLOCK_HEADER)
    {
      fprintf (stderr, "Argument not a BLOCK_HEADER reference\n");
      return 0;
    }
  if (!obj->tio.object)
    { // TODO could be an assert also
      fprintf (stderr, "Found null obj->tio.object\n");
      return 0;
    }
  if (!obj->tio.object->tio.BLOCK_HEADER)
    { // TODO could be an assert also
      fprintf (stderr, "Found null obj->tio.object->tio.BLOCK_HEADER\n");
      return 0;
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
      // fatal: The string "--" is not permitted within comments.
      if (escaped && strstr (escaped, "--"))
        {
          char *s;
          while ((s = strstr (escaped, "--")))
            {
              *s = '_';
              *(s + 1) = '_';
            }
        }
      // don't group *Model_Space
      if (!escaped || strcmp (escaped, "*Model_Space") != 0)
        {
          is_g = 1;
          printf ("\t<g id=\"symbol-" FORMAT_HV "\" >\n\t\t<!-- %s -->\n",
                  ref->absolute_ref, escaped ? escaped : "");
        }
      else
        printf ("\t<!-- %s -->\n", escaped);
      if (escaped)
        FREE (escaped);
    }

  obj = get_first_owned_entity (ref->obj);
  while (obj)
    {
      num += output_object (obj);
      obj = get_next_owned_entity (ref->obj, obj);
    }

  if (is_g)
    printf ("\t</g>\n");
  return num;
}

static void
output_SVG (Dwg_Data *dwg)
{
  BITCODE_BS i;
  int num = 0;
  Dwg_Object *obj;
  Dwg_Object_Ref *ref;
  Dwg_Object_BLOCK_CONTROL *block_control;
  double dx, dy;

  model_xmin = dwg_model_x_min (dwg);
  model_ymin = dwg_model_y_min (dwg);
  model_xmax = dwg_model_x_max (dwg);
  model_ymax = dwg_model_y_max (dwg);

  dx = model_xmax - model_xmin;
  dy = model_ymax - model_ymin;
  // double scale_x = dx / (dwg_page_x_max(dwg) - dwg_page_x_min(dwg));
  // double scale_y = dy / (dwg_page_y_max(dwg) - dwg_page_y_min(dwg));
  scale = 25.4 / 72; // pt:mm
  if (isnan (dx))
    dx = 100.0;
  if (isnan (dy))
    dy = 100.0;
  page_width = dx;
  page_height = dy;
  // scale *= (scale_x > scale_y ? scale_x : scale_y);

  // optional, for xmllint
  // <!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
  //   "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
  // But we use jing with relaxng, which is better. Just LaTeXML shipped a
  // broken rng
  printf ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
          "<svg\n"
          "   xmlns:svg=\"http://www.w3.org/2000/svg\"\n"
          "   xmlns=\"http://www.w3.org/2000/svg\"\n"
          "   xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
          "   version=\"1.1\" baseProfile=\"basic\"\n"
          "   width=\"100%%\" height=\"100%%\"\n"
          "   viewBox=\"%f %f %f %f\">\n",
          model_xmin, model_ymin, page_width, page_height);

  if (!mspace && (ref = dwg_paper_space_ref (dwg)))
    num = output_BLOCK_HEADER (
        ref); // how many paper-space entities we did print
  if (!num && (ref = dwg_model_space_ref (dwg)))
    output_BLOCK_HEADER (ref);
  printf ("\t<defs>\n");
  for (i = 0; i < dwg->block_control.num_entries; i++)
    {
      if (dwg->block_control.entries && (ref = dwg->block_control.entries[i]))
        output_BLOCK_HEADER (ref);
    }
  printf ("\t</defs>\n");
  printf ("</svg>\n");
  fflush (stdout);
}

int
main (int argc, char *argv[])
{
  int error;
  int force_free = 0;
  int i = 1;
  int c;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "mspace", 0, 0, 0 },      { "force-free", 0, 0, 0 },
          { "help", 0, 0, 0 },        { "version", 0, 0, 0 },
          { NULL, 0, NULL, 0 } };
#endif

  GC_INIT ();
  if (argc < 2)
    return usage ();

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, ":v:m::h", long_options, &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, ":v:m::hi")) != -1)
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
          if (!strcmp (long_options[option_index].name, "force-free"))
            force_free = 1;
          if (!strcmp (long_options[option_index].name, "mspace"))
            mspace = 1;
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

  if (opts)
    fprintf (stderr, "\nSVG\n===\n");
  if (error < DWG_ERR_CRITICAL)
    output_SVG (&g_dwg);

#if defined __SANITIZE_ADDRESS__ || __has_feature(address_sanitizer)
  {
    char *asanenv = getenv ("ASAN_OPTIONS");
    if (!asanenv)
      force_free = 1;
    // detect_leaks is enabled by default. see if it's turned off
    else if (strstr (asanenv, "detect_leaks=0") == NULL) /* not found */
      force_free = 1;
  }
#endif

  // forget about leaks. really huge DWG's need endlessly here.
  if ((g_dwg.header.version && g_dwg.num_objects < 1000) || force_free
#ifdef HAVE_VALGRIND_VALGRIND_H
      || (RUNNING_ON_VALGRIND)
#endif
  )
    {
      dwg_free (&g_dwg);
    }
  return error >= DWG_ERR_CRITICAL ? 1 : 0;
}

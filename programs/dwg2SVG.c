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
 * POLYLINE_PFACE, RAY, XLINE, TABLE, TOLERANCE, VIEWPORT?
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
  // dxf_cvt_lweight gives 100ths of a mm; mm is lw/100, not lw/1000.
  int lw = dxf_cvt_lweight (ent->linewt);
  if (lw < 0)
    return 0.25; // BYLAYER/BYBLOCK: reasonable default
  if (lw == 0)
    return 0.13; // thinnest line
  return (double)lw * 0.01;
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
  printf ("      style=\"fill:none;stroke:%s;stroke-width:%f;"
          "vector-effect:non-scaling-stroke\" />\n",
          color, lweight);
  if (*color == '#')
    FREE (color);
}

// font-family for a TEXT/MTEXT entity, from its STYLE (null-safe).
static const char *
text_fontfamily (Dwg_Data *dwg, BITCODE_H style_ref)
{
  Dwg_Object *o = style_ref ? dwg_ref_object_silent (dwg, style_ref) : NULL;
  Dwg_Object_STYLE *style = o ? o->tio.object->tio.STYLE : NULL;
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
          return "Arial";
        }
      return "Verdana";
    }
  return "Courier";
}

static void
output_TEXT (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_TEXT *text = obj->tio.entity->tio.TEXT;
  char *escaped;
  const char *fontfamily = text_fontfamily (dwg, text->style);
  BITCODE_2DPOINT pt;

  if (!text->text_value || entity_invisible (obj))
    return;
  if (isnan_2BD (text->ins_pt) || isnan_3BD (text->extrusion))
    return;
  if (dwg->header.version >= R_2007)
    escaped = htmlwescape ((BITCODE_TU)text->text_value);
  else
    escaped = htmlescape (text->text_value, dwg->header.codepage);

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
output_MTEXT (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_MTEXT *mtext = obj->tio.entity->tio.MTEXT;
  char *plaintext, *wrapped, *line;
  const char *fontfamily, *anchor;
  BITCODE_2DPOINT pt;
  double angle, line_height, first_offset, num_lines;
  int idx;

  if (!mtext->text || entity_invisible (obj))
    return;
  if (isnan_3BD (mtext->ins_pt) || isnan_3BD (mtext->extrusion))
    return;
  if (dwg->header.version >= R_2007)
    {
      char *utf8 = bit_convert_TU ((BITCODE_TU)mtext->text);
      if (!utf8)
        return;
      plaintext = mtext_plaintext (utf8);
      free (utf8);
    }
  else
    plaintext = mtext_plaintext ((const char *)mtext->text);
  if (!plaintext)
    return;

  fontfamily = text_fontfamily (dwg, mtext->style);
  angle = mtext_svg_angle (mtext->x_axis_dir);
  line_height = mtext_line_height (mtext->text_height, mtext->linespace_style,
                                   mtext->linespace_factor);
  num_lines = 0;
  wrapped = mtext_wrap_text (plaintext, mtext->rect_width, mtext->text_height,
                             &num_lines);
  free (plaintext);
  if (!wrapped)
    return;

  first_offset = mtext_attachment_first_offset (mtext->attachment, line_height,
                                                num_lines);
  anchor = mtext_attachment_anchor (mtext->attachment);
  // the insertion point of an MTEXT is WCS, unlike that of a TEXT
  pt.x = mtext->ins_pt.x;
  pt.y = mtext->ins_pt.y;

  printf ("\t<text id=\"dwg-object-%d\" x=\"%f\" y=\"%f\" font-family=\"%s\" "
          "font-size=\"%f\" fill=\"%s\" text-anchor=\"%s\"",
          obj->index, transform_X (pt.x), transform_Y (pt.y), fontfamily,
          mtext->text_height, entity_color (obj->tio.entity), anchor);
  if (angle != 0.0)
    printf (" transform=\"rotate(%.1f %f %f)\"", angle, transform_X (pt.x),
            transform_Y (pt.y));
  printf (">\n");
  line = wrapped;
  idx = 0;
  while (line)
    {
      char *nl = strchr (line, '\n');
      char *escaped_line;
      double y;
      if (nl)
        *nl = '\0';
      escaped_line = mtext_escape_line (line);
      y = transform_Y (pt.y) + first_offset + idx * line_height;
      printf ("\t  <tspan x=\"%f\" y=\"%f\">%s</tspan>\n", transform_X (pt.x),
              y, escaped_line ? escaped_line : "");
      if (escaped_line)
        free (escaped_line);
      if (!nl)
        break;
      line = nl + 1;
      idx++;
    }
  printf ("\t</text>\n");
  free (wrapped);
}

static void
output_LINE (Dwg_Object *obj)
{
  Dwg_Entity_LINE *line = obj->tio.entity->tio.LINE;

  if (isnan_3BD (line->start) || isnan_3BD (line->end)
      || isnan_3BD (line->extrusion) || entity_invisible (obj))
    return;
  // the points of a LINE are WCS, its extrusion is only for the thickness
  printf ("\t<!-- line-%d -->\n", obj->index);
  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f L %f,%f\"\n\t", obj->index,
          transform_X (line->start.x), transform_Y (line->start.y),
          transform_X (line->end.x), transform_Y (line->end.y));
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
  BITCODE_3DPOINT pt;

  pt.x = point->x;
  pt.y = point->y;
  pt.z = point->z;
  if (isnan_3BD (pt) || isnan_3BD (point->extrusion) || entity_invisible (obj))
    return;
  // the point is WCS, as in a LINE
  printf ("\t<!-- point-%d -->\n", obj->index);
  printf ("\t<circle id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" r=\"0.1\"\n\t",
          obj->index, transform_X (pt.x), transform_Y (pt.y));
  common_entity (obj->tio.entity);
}

// A point of an ARC at angle, from its OCS to WCS
static void
arc_point (BITCODE_3DPOINT *out, const Dwg_Entity_ARC *arc, const double angle)
{
  BITCODE_3DPOINT p;
  p.x = arc->center.x + arc->radius * cos (angle);
  p.y = arc->center.y + arc->radius * sin (angle);
  p.z = arc->center.z;
  transform_OCS (out, p, arc->extrusion);
}

static void
output_ARC (Dwg_Object *obj)
{
  Dwg_Entity_ARC *arc = obj->tio.entity->tio.ARC;
  BITCODE_3DPOINT start, end;
  double span;
  int large_arc, sweep;

  if (isnan_3BD (arc->center) || isnan_3BD (arc->extrusion)
      || isnan (arc->radius) || isnan (arc->start_angle)
      || isnan (arc->end_angle) || entity_invisible (obj))
    return;
  arc_point (&start, arc, arc->start_angle);
  arc_point (&end, arc, arc->end_angle);
  // the span is in (0, 2 pi], also for an arc across angle 0
  span = fmod (arc->end_angle - arc->start_angle, 2 * M_PI);
  if (span <= 0.0)
    span += 2 * M_PI;
  large_arc = span > M_PI;
  // counterclockwise in the OCS, so a negative sweep (0) in SVG, as y is
  // flipped; a mirrored OCS, e.g. the extrusion (0,0,-1), reverses it
  sweep = transform_OCS_2d_is_mirrored (arc->extrusion);

  printf ("\t<!-- arc-%d -->\n", obj->index);
  printf (
      "\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f 0 %d,%d %f,%f\"\n\t",
      obj->index, transform_X (start.x), transform_Y (start.y), arc->radius,
      arc->radius, large_arc, sweep, transform_X (end.x), transform_Y (end.y));
  common_entity (obj->tio.entity);
}

// The point at parameter u of an ELLIPSE: center + major cos u + minor sin u
static void
ellipse_point (BITCODE_2DPOINT *p, const Dwg_Entity_ELLIPSE *ell,
               const BITCODE_2DPOINT minor, const double u)
{
  p->x = ell->center.x + ell->sm_axis.x * cos (u) + minor.x * sin (u);
  p->y = ell->center.y + ell->sm_axis.y * cos (u) + minor.y * sin (u);
}

static void
output_ELLIPSE (Dwg_Object *obj)
{
  Dwg_Entity_ELLIPSE *ell = obj->tio.entity->tio.ELLIPSE;
  const BITCODE_3BD a = ell->sm_axis;
  const BITCODE_3BD n = ell->extrusion;
  BITCODE_2DPOINT radius, minor, p1, p2;
  double angle_dec, span;

  if (isnan_3BD (ell->center) || isnan_3BD (ell->extrusion)
      || isnan_3BD (ell->sm_axis) || isnan (ell->axis_ratio)
      || isnan (ell->start_angle) || isnan (ell->end_angle)
      || entity_invisible (obj))
    return;
  /* The center and the major axis are already WCS, the minor axis is
     axis_ratio * (extrusion x major axis) */
  radius.x = sqrt (a.x * a.x + a.y * a.y);
  radius.y = radius.x * ell->axis_ratio;
  angle_dec = atan2 (a.y, a.x) * 180.0 / M_PI;
  // the span is in (0, 2 pi]: 0 to 0 and 0 to 2 pi are full ellipses
  span = fmod (ell->end_angle - ell->start_angle, 2 * M_PI);
  if (span <= 0.0)
    span += 2 * M_PI;

  printf ("\t<!-- ellipse-%d -->\n", obj->index);
  printf ("\t<!-- sm_axis=(%f,%f,%f) axis_ratio=%f start_angle=%f "
          "end_angle=%f-->\n",
          ell->sm_axis.x, ell->sm_axis.y, ell->sm_axis.z, ell->axis_ratio,
          ell->start_angle, ell->end_angle);
  // also full if the angles are a few ulp off a full turn
  if (span < 1e-9 || span > 2 * M_PI - 1e-9)
    {
      printf ("\t<ellipse id=\"dwg-object-%d\" cx=\"%f\" cy=\"%f\" rx=\"%f\" "
              "ry=\"%f\" transform=\"rotate(%f %f %f)\"\n\t",
              obj->index, transform_X (ell->center.x),
              transform_Y (ell->center.y), radius.x, radius.y,
              transform_ANGLE (angle_dec), transform_X (ell->center.x),
              transform_Y (ell->center.y));
      common_entity (obj->tio.entity);
      return;
    }
  minor.x = ell->axis_ratio * (n.y * a.z - n.z * a.y);
  minor.y = ell->axis_ratio * (n.z * a.x - n.x * a.z);
  ellipse_point (&p1, ell, minor, ell->start_angle);
  ellipse_point (&p2, ell, minor, ell->start_angle + span);
  // y is flipped in SVG: the rotation changes sign, and so does the sweep,
  // which is positive (1) for a clockwise arc, det (major, minor) < 0
  printf ("\t<path id=\"dwg-object-%d\" d=\"M %f,%f A %f,%f %f %d,%d "
          "%f,%f\"\n\t",
          obj->index, transform_X (p1.x), transform_Y (p1.y), radius.x,
          radius.y, -angle_dec, span > M_PI,
          a.x * minor.y - a.y * minor.x < 0.0, transform_X (p2.x),
          transform_Y (p2.y));
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

// Append the path segment from prev to pt, both already OCS-transformed:
// an arc for a bulged segment, else a line.
static void
path_segment (BITCODE_2DPOINT prev, BITCODE_2DPOINT pt, double bulge,
              int mirrored)
{
  BITCODE_2BD ctr;
  double radius, start_angle, sweep;

  if (mirrored)
    bulge = -bulge;
  if (bulge_to_arc (prev, pt, bulge, &ctr, &radius, &start_angle, &sweep))
    // The output flips Y, so a counter-clockwise (positive) bulge has
    // sweep-flag 0, as in output_ARC.  |bulge| > 1 is over a semicircle.
    printf (" A %f,%f 0 %d,%d %f,%f", radius, radius,
            fabs (bulge) > 1.0 ? 1 : 0, bulge > 0.0 ? 0 : 1,
            transform_X (pt.x), transform_Y (pt.y));
  else
    printf (" L %f,%f", transform_X (pt.x), transform_Y (pt.y));
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
      BITCODE_2DPOINT pt, ptin, first, prev;
      dwg_point_2d *pts = dwg_object_polyline_2d_get_points (obj, &error);
      double *bulges;
      BITCODE_RL j, prev_j = 0;
      int mirrored;

      if (error || isnan_2pt (pts[0]) || isnan_3BD (pline->extrusion))
        {
          free (pts);
          return;
        }
      // without bulges, all segments are straight
      bulges = dwg_object_polyline_2d_get_bulges (obj, &error);
      mirrored = transform_OCS_2d_is_mirrored (pline->extrusion);
      ptin.x = pts[0].x;
      ptin.y = pts[0].y;
      transform_OCS_2d (&pt, ptin, pline->extrusion);
      first = prev = pt;
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
          // TODO widths
          path_segment (prev, pt, bulges ? bulges[prev_j] : 0.0, mirrored);
          prev = pt;
          prev_j = j;
        }
      if (pline->flag & 1) // closed
        {
          // the closing segment has the bulge of the last vertex
          if (bulges)
            path_segment (prev, first, bulges[prev_j], mirrored);
          printf (" Z");
        }
      printf ("\"\n\t");
      common_entity (obj->tio.entity);
      FREE (bulges);
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

      BITCODE_2DPOINT first, prev;
      BITCODE_RL prev_j = 0;
      const int has_bulges
          = pline->bulges && pline->num_bulges == pline->num_points;
      const int mirrored = transform_OCS_2d_is_mirrored (pline->extrusion);

      if (error || isnan_2pt (pts[0]) || isnan_3BD (pline->extrusion))
        return;
      ptin.x = pts[0].x;
      ptin.y = pts[0].y;
      transform_OCS_2d (&pt, ptin, pline->extrusion);
      first = prev = pt;
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
          // TODO widths
          path_segment (prev, pt, has_bulges ? pline->bulges[prev_j] : 0.0,
                        mirrored);
          prev = pt;
          prev_j = j;
        }
      if (pline->flag & 512) // closed
        {
          // the closing segment has the bulge of the last vertex
          if (has_bulges)
            path_segment (prev, first, pline->bulges[prev_j], mirrored);
          printf (" Z");
        }
      printf ("\"\n\t");
      common_entity (obj->tio.entity);
      FREE (pts);
    }
}

// TODO: MINSERT
// The chain of Bezier curves of degree 2 or 3 of a SPLINE: exact with Q or C.
// Bezier curves are affine invariant, so transforming the control points
// transforms the curve.
static void
spline_path_bezier (const BITCODE_3DPOINT *pts, const int num_curves,
                    const int degree)
{
  int j, i;
  printf ("M %f,%f", transform_X (pts[0].x), transform_Y (pts[0].y));
  for (j = 0; j < num_curves; j++)
    {
      printf (" %c", degree == 3 ? 'C' : 'Q');
      for (i = 1; i <= degree; i++)
        printf (" %f,%f", transform_X (pts[j * degree + i].x),
                transform_Y (pts[j * degree + i].y));
    }
}

// Other SPLINEs through evaluated points, SPLINE_SVG_STEPS per knot span.
#define SPLINE_SVG_STEPS 16
static void
spline_path_points (const Dwg_Entity_SPLINE *spline)
{
  BITCODE_BL m;
  int first = 1;
  for (m = spline->degree; m < spline->num_ctrl_pts; m++)
    {
      const double u0 = spline->knots[m], u1 = spline->knots[m + 1];
      int i;
      if (u1 <= u0)
        continue;
      // the first point of a span is the last one of the span before
      for (i = first ? 0 : 1; i <= SPLINE_SVG_STEPS; i++)
        {
          BITCODE_3DPOINT pt;
          spline_eval (&pt, spline, u0 + (u1 - u0) * i / SPLINE_SVG_STEPS);
          if (first)
            printf ("M %f,%f", transform_X (pt.x), transform_Y (pt.y));
          else
            printf (" L %f,%f", transform_X (pt.x), transform_Y (pt.y));
          first = 0;
        }
    }
}

// A SPLINE whose fit points cannot be interpolated: the curve passes
// through them, so a polyline through them is a fair approximation.
static int
spline_fit_points_usable (const Dwg_Entity_SPLINE *spline)
{
  BITCODE_BL i;
  if (!spline->fit_pts || spline->num_fit_pts < 2)
    return 0;
  for (i = 0; i < spline->num_fit_pts; i++)
    if (isnan_3BD (spline->fit_pts[i]))
      return 0;
  return 1;
}

static void
spline_path_fit_points (const Dwg_Entity_SPLINE *spline)
{
  BITCODE_BL i;
  for (i = 0; i < spline->num_fit_pts; i++)
    printf ("%s %f,%f", i ? " L" : "M", transform_X (spline->fit_pts[i].x),
            transform_Y (spline->fit_pts[i].y));
}

// The Bezier curves of a SPLINE, from its control points, or else from its
// fit points by AutoCAD's interpolation.  Returns their number, 0 if none.
static int
spline_bezier (const Dwg_Entity_SPLINE *spline, BITCODE_3DPOINT **pts)
{
  Dwg_Entity_SPLINE fit;
  int num_curves = spline_to_bezier (spline, pts);
  if (num_curves || spline_is_valid (spline)
      || !spline_from_fit_points (spline, &fit))
    return num_curves;
  num_curves = spline_to_bezier (&fit, pts);
  free (fit.ctrl_pts);
  free (fit.knots);
  return num_curves;
}

// A SPLINE has no OCS: its points are in WCS, drawn in the XY plane.
static void
output_SPLINE (Dwg_Object *obj)
{
  Dwg_Entity_SPLINE *spline = obj->tio.entity->tio.SPLINE;
  BITCODE_3DPOINT *pts = NULL;
  int num_curves;

  if (entity_invisible (obj))
    return;
  num_curves = spline_bezier (spline, &pts);
  if (num_curves && spline->degree != 2 && spline->degree != 3)
    {
      free (pts); // SVG has no Bezier curves of other degrees
      num_curves = 0;
    }
  if (!num_curves && !spline_is_valid (spline)
      && !spline_fit_points_usable (spline))
    return; // nothing usable to draw
  printf ("\t<!-- spline-%d -->\n", obj->index);
  printf ("\t<path id=\"dwg-object-%d\" d=\"", obj->index);
  if (num_curves)
    spline_path_bezier (pts, num_curves, spline->degree);
  else if (spline_is_valid (spline))
    spline_path_points (spline);
  else
    spline_path_fit_points (spline);
  if (num_curves)
    free (pts);
  if (spline->closed_b)
    printf (" Z");
  printf ("\"\n\t");
  common_entity (obj->tio.entity);
}

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
  // count only entities that actually put coordinates on the page
  if (obj->supertype == DWG_SUPERTYPE_ENTITY && entity_invisible (obj))
    return 0;

  switch (obj->fixedtype)
    {
    case DWG_TYPE_INSERT:
    case DWG_TYPE_ACIDBLOCKREFERENCE:
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
    case DWG_TYPE_MTEXT:
      output_MTEXT (obj);
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
    case DWG_TYPE_SPLINE:
      output_SPLINE (obj);
      break;
    case DWG_TYPE_SEQEND:
    case DWG_TYPE_VIEWPORT:
      num = 0;
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
          0.0, 0.0, page_width, page_height);

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

#if defined __SANITIZE_ADDRESS__ || defined HAVE_ASAN                         \
    || __has_feature(address_sanitizer)
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

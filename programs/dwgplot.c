/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2026 Free Software Foundation, Inc.                   */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwgplot.c: convert a DWG to the formats of GNU plotutils, with libplot.
 * The entities are those of dwg2SVG.c, which this is derived from:
 * written by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 *
 * Coordinates are the DWG ones, y up as in libplot, so there is no y flip.
 * Blocks are drawn at their INSERT with libplot's coordinate transformation.
 *
 * TODO: all entities: 3DSOLID, SHAPE, ARC_DIMENSION, ATTRIB, DIMENSION*,
 *         *SURFACE, HATCH, HELIX, IMAGE/WIPEOUT/UNDERLAY, LEADER, MESH,
 *         MINSERT, MLINE, MULTILEADER, OLE2FRAME, POLYLINE_3D,
 *         POLYLINE_MESH, POLYLINE_PFACE, TABLE, TOLERANCE, VIEWPORT?
 *       common_entity_data: ltype, ltype_scale, BYLAYER/BYBLOCK colors.
 *       PLINE: widths.
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
#include <plot.h> // needs stdio.h
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

#define MAX_BLOCK_DEPTH 32
// Segments of a full ellipse, and the minimum of an elliptical arc
#define ELLIPSE_STEPS 96
// Other SPLINEs through evaluated points, per knot span
#define SPLINE_STEPS 16

static int opts = 0;
static int mspace = 0; // only mspace, even when pspace is defined
Dwg_Data g_dwg;
static double model_xmin, model_ymin, model_xmax, model_ymax;
static plPlotter *plotter;
// the current point of the open libplot path
static double cur_x, cur_y;
// a line of 0.25 mm is this wide, in the DWG units
static double line_unit = 1.0;
static int block_depth = 0;
// The first pass only measures the extents of what is drawn, as the extents
// in the header are often not usable: the points go through the current
// block transformation xf (a, b, c, d, e, f) into the bounding box.
static int measuring = 0;
static double xf[6] = { 1, 0, 0, 1, 0, 0 };
static double bb_xmin, bb_ymin, bb_xmax, bb_ymax;

static void output_plot (Dwg_Data *dwg);
static int output_BLOCK_HEADER (Dwg_Object_Ref *ref);

static int
usage (void)
{
  printf ("\nUsage: dwgplot [-T FORMAT] [-v[0-9]] DWGFILE >PLOTFILE\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwgplot %s\n", dwg_api_version_string ());
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dwgplot [OPTION]... DWGFILE >PLOTFILE\n");
  printf ("Converts some 2D elements of the DWG to a format of GNU "
          "plotutils.\n\n");
#ifdef HAVE_GETOPT_LONG
  printf ("  -T, --output-format FMT   One of: png, pnm, gif (bitmaps), or\n"
          "                            svg, ps, ai, cgm, fig, pcl, hpgl,\n"
          "                            regis, tek (vector formats).\n"
          "                            The default is meta, the plot(5)\n"
          "                            format for plot(1) or graph(1).\n");
  printf ("  -o, --file FILE           output file, else stdout\n");
  printf ("  -p, --pagesize SIZE       e.g. a4, letter, or a4,xsize=10in\n");
  printf (
      "  -b, --bitmapsize WxH      of png, pnm and gif. default 1024x1024\n");
  printf ("  -v[0-9], --verbose [0-9]  verbosity\n");
  printf ("      -m, --mspace          only model-space, no paper-space\n");
  printf ("           --force-free     force free\n");
  printf ("           --help           display this help and exit\n");
  printf ("           --version        output version information and exit\n"
          "\n");
#else
  printf ("  -T FMT      One of: png, pnm, gif (bitmaps), or\n"
          "              svg, ps, ai, cgm, fig, pcl, hpgl, regis, tek.\n"
          "              The default is meta, the plot(5) format.\n");
  printf ("  -o FILE     output file, else stdout\n");
  printf ("  -p SIZE     pagesize, e.g. a4\n");
  printf ("  -b WxH      bitmapsize of png, pnm and gif\n");
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
isnan_3RD (const BITCODE_3RD pt)
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

// The line width in mm
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

// The color of an entity as 0..255 r, g, b. White (7) is black on paper.
static void
entity_rgb (Dwg_Object_Entity *ent, int *r, int *g, int *b)
{
  // TODO: alpha?
  *r = *g = *b = 0;
  if (ent->color.flag & 0x80 && !(ent->color.flag & 0x40))
    {
      *r = (ent->color.rgb >> 16) & 0xff;
      *g = (ent->color.rgb >> 8) & 0xff;
      *b = ent->color.rgb & 0xff;
    }
  else if (ent->color.index >= 1 && ent->color.index < 256
           && ent->color.index != 7)
    {
      const Dwg_RGB_Palette *palette = dwg_rgb_palette ();
      const Dwg_RGB_Palette *rgb = &palette[ent->color.index];
      *r = rgb->r;
      *g = rgb->g;
      *b = rgb->b;
    }
  // 0 ByBlock, 256 ByLayer, 7: black
}

static void
bbox_add (const double x, const double y)
{
  const double tx = xf[0] * x + xf[2] * y + xf[4];
  const double ty = xf[1] * x + xf[3] * y + xf[5];
  if (isnan (tx) || isnan (ty))
    return;
  if (tx < bb_xmin)
    bb_xmin = tx;
  if (tx > bb_xmax)
    bb_xmax = tx;
  if (ty < bb_ymin)
    bb_ymin = ty;
  if (ty > bb_ymax)
    bb_ymax = ty;
}

static void
draw_line (const double x1, const double y1, const double x2, const double y2)
{
  if (measuring)
    {
      bbox_add (x1, y1);
      bbox_add (x2, y2);
    }
  else
    pl_fline_r (plotter, x1, y1, x2, y2);
}

static void
draw_circle (const double x, const double y, const double r)
{
  if (measuring)
    {
      bbox_add (x - r, y - r);
      bbox_add (x + r, y + r);
    }
  else
    pl_fcircle_r (plotter, x, y, r);
}

static void
draw_ellipse (const double x, const double y, const double rx, const double ry,
              const double angle)
{
  if (measuring)
    {
      const double r = rx > ry ? rx : ry;
      bbox_add (x - r, y - r);
      bbox_add (x + r, y + r);
    }
  else
    pl_fellipse_r (plotter, x, y, rx, ry, angle);
}

static void
draw_point (const double x, const double y)
{
  if (measuring)
    bbox_add (x, y);
  else
    pl_fpoint_r (plotter, x, y);
}

// The pen color and width of the entity, no fill
static void
set_pen (Dwg_Object_Entity *ent)
{
  int r, g, b;
  if (measuring)
    return;
  entity_rgb (ent, &r, &g, &b);
  pl_pencolor_r (plotter, r * 257, g * 257, b * 257);
  pl_fillcolor_r (plotter, r * 257, g * 257, b * 257);
  pl_flinewidth_r (plotter, line_unit * entity_lweight (ent) / 0.25);
  pl_filltype_r (plotter, 0);
}

/* The path: arcs, lines and Bezier curves, as in the SVG path syntax. */

static void
path_end (void)
{
  if (!measuring)
    pl_endpath_r (plotter);
}

static void
path_move (const double x, const double y)
{
  path_end ();
  if (measuring)
    bbox_add (x, y);
  else
    pl_fmove_r (plotter, x, y);
  cur_x = x;
  cur_y = y;
}

static void
path_line (const double x, const double y)
{
  if (measuring)
    bbox_add (x, y);
  else
    pl_fcont_r (plotter, x, y);
  cur_x = x;
  cur_y = y;
}

static void
path_close (void)
{
  if (!measuring)
    pl_closepath_r (plotter);
}

// An arc around the center from the current point to x,y. libplot arcs are
// counterclockwise: a clockwise one is the other way around, and has to
// start a new path.
static void
path_arc (const double cx, const double cy, const double x, const double y,
          const int ccw)
{
  if (measuring)
    bbox_add (x, y); // the ends, not the bulge of the arc
  else if (ccw)
    pl_farc_r (plotter, cx, cy, cur_x, cur_y, x, y);
  else
    {
      path_end ();
      pl_farc_r (plotter, cx, cy, x, y, cur_x, cur_y);
      pl_fmove_r (plotter, x, y);
    }
  cur_x = x;
  cur_y = y;
}

// from the current point
static void
path_bezier2 (const double x1, const double y1, const double x2,
              const double y2)
{
  if (measuring)
    {
      bbox_add (x1, y1);
      bbox_add (x2, y2);
    }
  else
    pl_fbezier2_r (plotter, cur_x, cur_y, x1, y1, x2, y2);
  cur_x = x2;
  cur_y = y2;
}

static void
path_bezier3 (const double x1, const double y1, const double x2,
              const double y2, const double x3, const double y3)
{
  if (measuring)
    {
      bbox_add (x1, y1);
      bbox_add (x2, y2);
      bbox_add (x3, y3);
    }
  else
    pl_fbezier3_r (plotter, cur_x, cur_y, x1, y1, x2, y2, x3, y3);
  cur_x = x3;
  cur_y = y3;
}

// The chain of Bezier curves of degree 2 or 3 of a SPLINE: exact.
// Bezier curves are affine invariant, so transforming the control points
// transforms the curve.
static void
path_bezier_chain (const BITCODE_3DPOINT *pts, const int num_curves,
                   const int degree)
{
  int j;
  path_move (pts[0].x, pts[0].y);
  for (j = 0; j < num_curves; j++)
    {
      const BITCODE_3DPOINT *p = &pts[j * degree];
      if (degree == 3)
        path_bezier3 (p[1].x, p[1].y, p[2].x, p[2].y, p[3].x, p[3].y);
      else
        path_bezier2 (p[1].x, p[1].y, p[2].x, p[2].y);
    }
}

// The 2D point of the OCS point pt at elevation: with a tilted extrusion
// the elevation moves it, as with ARC and CIRCLE
static void
transform_OCS_elevation (BITCODE_2DPOINT *out, const BITCODE_2DPOINT pt,
                         const double elevation, const BITCODE_BE ext)
{
  BITCODE_3DPOINT p, w;
  p.x = pt.x;
  p.y = pt.y;
  p.z = isnan (elevation) ? 0.0 : elevation;
  transform_OCS (&w, p, ext);
  out->x = w.x;
  out->y = w.y;
}

/* Text */

// the PostScript font of libplot for a TEXT/MTEXT entity, from its STYLE
static const char *
text_fontname (Dwg_Data *dwg, BITCODE_H style_ref)
{
  Dwg_Object *o = style_ref ? dwg_ref_object_silent (dwg, style_ref) : NULL;
  Dwg_Object_STYLE *style = o ? o->tio.object->tio.STYLE : NULL;
  // the font file, or the typeface of its EED
  char *font = style && o->fixedtype == DWG_TYPE_STYLE
                   ? dwg_style_font_file (style, NULL)
                   : NULL;
  if (font)
    {
      const char *name = "Helvetica";
      if (strstr (font, "Courier") || strstr (font, "courier")
          || strstr (font, "cour"))
        name = "Courier";
      else if (strstr (font, "Times") || strstr (font, "times")
               || strstr (font, "Roman") || strstr (font, "roman"))
        name = "Times-Roman";
      free (font);
      return name;
    }
  return "Courier";
}

// UTF-8 to latin1, which the PostScript fonts of libplot have. Characters
// beyond are '?'. Returns a new string.
static char *
utf8_to_latin1 (const char *restrict utf8)
{
  const unsigned char *s = (const unsigned char *)utf8;
  char *dest = (char *)malloc (strlen (utf8) + 1);
  char *d = dest;
  if (!dest)
    return NULL;
  while (*s)
    {
      if (*s < 0x80)
        *d++ = (char)*s++;
      else if ((*s & 0xe0) == 0xc0 && (s[1] & 0xc0) == 0x80)
        {
          const unsigned cp = ((*s & 0x1f) << 6) | (s[1] & 0x3f);
          *d++ = cp < 0x100 ? (char)cp : '?';
          s += 2;
        }
      else
        {
          *d++ = '?'; // other multibyte sequence: skip all its bytes
          s++;
          while ((*s & 0xc0) == 0x80)
            s++;
        }
    }
  *d = '\0';
  return dest;
}

// A label at x,y, rotated by angle degrees. hjust is l, c or r, vjust the
// baseline x.
static void
put_label (const double x, const double y, const double height,
           const double angle, const char *fontname, const int hjust,
           const char *restrict utf8)
{
  char *label;
  if (measuring)
    {
      bbox_add (x, y);
      return;
    }
  label = utf8 ? utf8_to_latin1 (utf8) : NULL;
  if (!label)
    return;
  pl_fontname_r (plotter, fontname);
  pl_ffontsize_r (plotter, height);
  pl_ftextangle_r (plotter, angle);
  pl_fmove_r (plotter, x, y);
  pl_alabel_r (plotter, hjust, 'x', label);
  pl_ftextangle_r (plotter, 0.0);
  free (label);
}

// The text as new UTF-8 string. r2007+ it is UCS-2.
static char *
text_utf8 (Dwg_Data *dwg, BITCODE_T text)
{
  if (dwg->header.version >= R_2007)
    return bit_convert_TU ((BITCODE_TU)text);
  else
    {
      char *u8 = bit_TV_to_utf8 ((const char *)text, dwg->header.codepage);
      // may return the same string
      return u8 == (char *)text ? strdup ((const char *)text) : u8;
    }
}

static void
output_TEXT (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_TEXT *text = obj->tio.entity->tio.TEXT;
  char *utf8;
  BITCODE_2DPOINT pt;

  if (!text->text_value || entity_invisible (obj))
    return;
  if (isnan_2BD (text->ins_pt) || isnan_3BD (text->extrusion))
    return;
  utf8 = text_utf8 (dwg, text->text_value);
  if (!utf8)
    return;
  transform_OCS_elevation (&pt, text->ins_pt, text->elevation,
                           text->extrusion);
  set_pen (obj->tio.entity);
  put_label (pt.x, pt.y, text->height /* fontsize */,
             isnan (text->rotation) ? 0.0 : text->rotation * 180.0 / M_PI,
             text_fontname (dwg, text->style), 'l', utf8);
  free (utf8);
}

static int
mtext_hjust (const int attachment)
{
  const char *anchor = mtext_attachment_anchor (attachment);
  return !strcmp (anchor, "middle") ? 'c'
         : !strcmp (anchor, "end")  ? 'r'
                                    : 'l';
}

static void
output_MTEXT (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_MTEXT *mtext = obj->tio.entity->tio.MTEXT;
  char *plaintext, *wrapped, *line;
  const char *fontname;
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

  fontname = text_fontname (dwg, mtext->style);
  angle = -mtext_svg_angle (mtext->x_axis_dir); // ccw
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
  set_pen (obj->tio.entity);
  line = wrapped;
  idx = 0;
  while (line)
    {
      char *nl = strchr (line, '\n');
      // the lines go down along the rotated y axis
      const double d = first_offset + idx * line_height;
      const double a = angle * M_PI / 180.0;
      if (nl)
        *nl = '\0';
      // the insertion point of an MTEXT is WCS, unlike that of a TEXT
      put_label (mtext->ins_pt.x + d * sin (a), mtext->ins_pt.y - d * cos (a),
                 mtext->text_height, angle, fontname,
                 mtext_hjust (mtext->attachment), line);
      if (!nl)
        break;
      line = nl + 1;
      idx++;
    }
  free (wrapped);
}

/* Entities */

static void
output_LINE (Dwg_Object *obj)
{
  Dwg_Entity_LINE *line = obj->tio.entity->tio.LINE;

  if (isnan_3BD (line->start) || isnan_3BD (line->end)
      || isnan_3BD (line->extrusion) || entity_invisible (obj))
    return;
  // the points of a LINE are WCS, its extrusion is only for the thickness
  set_pen (obj->tio.entity);
  draw_line (line->start.x, line->start.y, line->end.x, line->end.y);
}

// Clip the line p + t v, t0 <= t, to the model bounds, and draw it.
// t0 is -inf for an XLINE, 0 for a RAY.
static void
output_clipped_line (Dwg_Object *obj, const BITCODE_3DPOINT p,
                     const BITCODE_3DPOINT v, const int is_ray)
{
  double tmin = is_ray ? 0.0 : -1e300, tmax = 1e300;
  const double lo[2] = { model_xmin, model_ymin };
  const double hi[2] = { model_xmax, model_ymax };
  const double pp[2] = { p.x, p.y }, vv[2] = { v.x, v.y };
  int k;

  if (measuring || (vv[0] == 0.0 && vv[1] == 0.0) || isnan (lo[0])
      || isnan (hi[0]) || isnan (lo[1]) || isnan (hi[1]))
    return;
  for (k = 0; k < 2; k++) // slab method
    {
      if (vv[k] == 0.0)
        {
          if (pp[k] < lo[k] || pp[k] > hi[k])
            return;
        }
      else
        {
          double t1 = (lo[k] - pp[k]) / vv[k];
          double t2 = (hi[k] - pp[k]) / vv[k];
          if (t1 > t2)
            {
              const double t = t1;
              t1 = t2;
              t2 = t;
            }
          if (t1 > tmin)
            tmin = t1;
          if (t2 < tmax)
            tmax = t2;
        }
    }
  if (tmin > tmax)
    return;
  set_pen (obj->tio.entity);
  draw_line (pp[0] + tmin * vv[0], pp[1] + tmin * vv[1], pp[0] + tmax * vv[0],
             pp[1] + tmax * vv[1]);
}

static void
output_XLINE (Dwg_Object *obj)
{
  Dwg_Entity_XLINE *xline = obj->tio.entity->tio.XLINE;
  if (isnan_3BD (xline->point) || isnan_3BD (xline->vector)
      || entity_invisible (obj))
    return;
  output_clipped_line (obj, xline->point, xline->vector, 0);
}

static void
output_RAY (Dwg_Object *obj)
{
  Dwg_Entity_XLINE *xline = obj->tio.entity->tio.RAY;
  if (isnan_3BD (xline->point) || isnan_3BD (xline->vector)
      || entity_invisible (obj))
    return;
  output_clipped_line (obj, xline->point, xline->vector, 1);
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
  set_pen (obj->tio.entity);
  draw_circle (center.x, center.y, circle->radius);
}

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
  set_pen (obj->tio.entity);
  draw_point (pt.x, pt.y);
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
  BITCODE_3DPOINT center, start, end;

  if (isnan_3BD (arc->center) || isnan_3BD (arc->extrusion)
      || isnan (arc->radius) || isnan (arc->start_angle)
      || isnan (arc->end_angle) || entity_invisible (obj))
    return;
  transform_OCS (&center, arc->center, arc->extrusion);
  arc_point (&start, arc, arc->start_angle);
  arc_point (&end, arc, arc->end_angle);
  set_pen (obj->tio.entity);
  path_move (start.x, start.y);
  // counterclockwise in the OCS; a mirrored OCS, e.g. the extrusion
  // (0,0,-1), reverses it
  path_arc (center.x, center.y, end.x, end.y,
            !transform_OCS_2d_is_mirrored (arc->extrusion));
  path_end ();
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
  BITCODE_2DPOINT minor, p;
  double rx, span;
  int j, steps;

  if (isnan_3BD (ell->center) || isnan_3BD (ell->extrusion)
      || isnan_3BD (ell->sm_axis) || isnan (ell->axis_ratio)
      || isnan (ell->start_angle) || isnan (ell->end_angle)
      || entity_invisible (obj))
    return;
  /* The center and the major axis are already WCS, the minor axis is
     axis_ratio * (extrusion x major axis) */
  rx = sqrt (a.x * a.x + a.y * a.y);
  // the span is in (0, 2 pi]: 0 to 0 and 0 to 2 pi are full ellipses
  span = fmod (ell->end_angle - ell->start_angle, 2 * M_PI);
  if (span <= 0.0)
    span += 2 * M_PI;

  set_pen (obj->tio.entity);
  // also full if the angles are a few ulp off a full turn
  if (span < 1e-9 || span > 2 * M_PI - 1e-9)
    {
      draw_ellipse (ell->center.x, ell->center.y, rx, rx * ell->axis_ratio,
                    atan2 (a.y, a.x) * 180.0 / M_PI);
      return;
    }
  minor.x = ell->axis_ratio * (n.y * a.z - n.z * a.y);
  minor.y = ell->axis_ratio * (n.z * a.x - n.x * a.z);
  // an elliptical arc as a polyline, libplot has only the full ellipse
  steps = (int)ceil (ELLIPSE_STEPS * span / (2 * M_PI));
  if (steps < 8)
    steps = 8;
  ellipse_point (&p, ell, minor, ell->start_angle);
  path_move (p.x, p.y);
  for (j = 1; j <= steps; j++)
    {
      ellipse_point (&p, ell, minor, ell->start_angle + span * j / steps);
      path_line (p.x, p.y);
    }
  path_end ();
}

// The outline of the 4 points: the area is not filled, as in dwg2SVG.
static void
output_outline4 (const BITCODE_2DPOINT c1, const BITCODE_2DPOINT c2,
                 const BITCODE_2DPOINT c3, const BITCODE_2DPOINT c4)
{
  path_move (c1.x, c1.y);
  path_line (c2.x, c2.y);
  path_line (c3.x, c3.y);
  path_line (c4.x, c4.y);
  path_close ();
  path_end ();
}

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
  transform_OCS_elevation (&c1, s1, sol->elevation, sol->extrusion);
  transform_OCS_elevation (&c2, s2, sol->elevation, sol->extrusion);
  transform_OCS_elevation (&c3, s3, sol->elevation, sol->extrusion);
  transform_OCS_elevation (&c4, s4, sol->elevation, sol->extrusion);
  set_pen (obj->tio.entity);
  output_outline4 (c1, c2, c3, c4);
}

static void
output_3DFACE (Dwg_Object *obj)
{
  Dwg_Entity__3DFACE *ent = obj->tio.entity->tio._3DFACE;
  const BITCODE_3BD *c[4];
  int k;

  if (isnan_3BD (ent->corner1) || isnan_3BD (ent->corner2)
      || isnan_3BD (ent->corner3) || isnan_3BD (ent->corner4)
      || entity_invisible (obj))
    return;
  c[0] = &ent->corner1;
  c[1] = &ent->corner2;
  c[2] = &ent->corner3;
  c[3] = &ent->corner4;
  set_pen (obj->tio.entity);
  // the edge k is c[k] to c[k+1], invisible with the bit k of invis_flags
  for (k = 0; k < 4; k++)
    if (!(ent->invis_flags & (1 << k)))
      draw_line (c[k]->x, c[k]->y, c[(k + 1) % 4]->x, c[(k + 1) % 4]->y);
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
  // a counter-clockwise bulge is positive
  if (bulge_to_arc (prev, pt, bulge, &ctr, &radius, &start_angle, &sweep))
    path_arc (ctr.x, ctr.y, pt.x, pt.y, bulge > 0.0);
  else
    path_line (pt.x, pt.y);
}

// A spline-fit POLYLINE_2D, flag 4: the exact curve of its frame vertices,
// flag 16, as Bezier curves of degree 2 or 3.  Bezier curves are affine
// invariant, so the frame is transformed first.  Returns 0 if not usable.
static int
polyline_2d_spline (const Dwg_Object *obj, const Dwg_Entity_POLYLINE_2D *pline,
                    const dwg_point_2d *pts, const BITCODE_RC *flags,
                    const BITCODE_RL numpts)
{
  const int degree = pline->curve_type == 5   ? 2
                     : pline->curve_type == 6 ? 3
                                              : 0;
  BITCODE_3DPOINT *frame, *bez;
  BITCODE_RL j, n = 0;
  int num_curves;

  if (!degree)
    return 0;
  frame = (BITCODE_3DPOINT *)malloc (numpts * sizeof (BITCODE_3DPOINT));
  if (!frame)
    return 0;
  for (j = 0; j < numpts; j++)
    if (flags[j] & 16)
      {
        BITCODE_2DPOINT pt, ptin;
        ptin.x = pts[j].x;
        ptin.y = pts[j].y;
        transform_OCS_elevation (&pt, ptin, pline->elevation,
                                 pline->extrusion);
        frame[n].x = pt.x;
        frame[n].y = pt.y;
        frame[n].z = 0.0;
        n++;
      }
  num_curves
      = polyline_spline_to_bezier (frame, n, degree, pline->flag & 1, &bez);
  free (frame);
  if (!num_curves)
    return 0;
  set_pen (obj->tio.entity);
  path_bezier_chain (bez, num_curves, degree);
  if (pline->flag & 1)
    path_close ();
  path_end ();
  free (bez);
  return 1;
}

// Without a usable frame: only the fit vertices, straight as AutoCAD draws
// them.  Returns their number, 0 if there are none and pts is unchanged.
static BITCODE_RL
polyline_2d_drop_frame (dwg_point_2d *pts, const BITCODE_RC *flags,
                        const BITCODE_RL numpts)
{
  BITCODE_RL j, k = 0;
  for (j = 0; j < numpts; j++)
    if (!(flags[j] & 16))
      pts[k++] = pts[j];
  return k;
}

// A spline-fit POLYLINE_2D, flag 4: draws its exact curve and returns 1.
// Else, without a usable frame, keeps only its fit vertices in pts, if
// any, and returns 0 with *straight set: AutoCAD joins them straight.
static int
polyline_2d_spline_fit (const Dwg_Object *obj,
                        const Dwg_Entity_POLYLINE_2D *pline, dwg_point_2d *pts,
                        BITCODE_RL *numpts, int *straight)
{
  int error, done;
  BITCODE_RC *flags = dwg_object_polyline_2d_get_flags (obj, &error);

  *straight = 0;
  if (!flags || error)
    {
      free (flags);
      return 0;
    }
  done = polyline_2d_spline (obj, pline, pts, flags, *numpts);
  if (!done)
    {
      const BITCODE_RL k = polyline_2d_drop_frame (pts, flags, *numpts);
      if (k)
        {
          *numpts = k;
          *straight = 1;
        }
    }
  free (flags);
  return done;
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
      int mirrored, straight = 0;

      if (error || isnan_3BD (pline->extrusion)
          || ((pline->flag & 4) // spline-fit, drawn as a curve
              && polyline_2d_spline_fit (obj, pline, pts, &numpts, &straight))
          || isnan_2pt (pts[0]))
        {
          free (pts);
          return;
        }
      // without bulges, all segments are straight, also the fit vertices
      bulges
          = straight ? NULL : dwg_object_polyline_2d_get_bulges (obj, &error);
      mirrored = transform_OCS_2d_is_mirrored (pline->extrusion);
      ptin.x = pts[0].x;
      ptin.y = pts[0].y;
      transform_OCS_elevation (&pt, ptin, pline->elevation, pline->extrusion);
      first = prev = pt;
      set_pen (obj->tio.entity);
      path_move (pt.x, pt.y);
      for (j = 1; j < numpts; j++)
        {
          ptin.x = pts[j].x;
          ptin.y = pts[j].y;
          if (isnan_2BD (ptin))
            continue;
          transform_OCS_elevation (&pt, ptin, pline->elevation,
                                   pline->extrusion);
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
          path_close ();
        }
      path_end ();
      free (bulges);
      free (pts);
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
        {
          free (pts);
          return;
        }
      ptin.x = pts[0].x;
      ptin.y = pts[0].y;
      transform_OCS_elevation (&pt, ptin, pline->elevation, pline->extrusion);
      first = prev = pt;
      set_pen (obj->tio.entity);
      path_move (pt.x, pt.y);
      for (j = 1; j < numpts; j++)
        {
          ptin.x = pts[j].x;
          ptin.y = pts[j].y;
          if (isnan_2BD (ptin))
            continue;
          transform_OCS_elevation (&pt, ptin, pline->elevation,
                                   pline->extrusion);
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
          path_close ();
        }
      path_end ();
      free (pts);
    }
}

// Other SPLINEs through evaluated points, SPLINE_STEPS per knot span.
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
      for (i = first ? 0 : 1; i <= SPLINE_STEPS; i++)
        {
          BITCODE_3DPOINT pt;
          spline_eval (&pt, spline, u0 + (u1 - u0) * i / SPLINE_STEPS);
          if (first)
            path_move (pt.x, pt.y);
          else
            path_line (pt.x, pt.y);
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
    {
      if (i)
        path_line (spline->fit_pts[i].x, spline->fit_pts[i].y);
      else
        path_move (spline->fit_pts[i].x, spline->fit_pts[i].y);
    }
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
      free (pts); // libplot has no Bezier curves of other degrees
      num_curves = 0;
    }
  if (!num_curves && !spline_is_valid (spline)
      && !spline_fit_points_usable (spline))
    return; // nothing usable to draw
  set_pen (obj->tio.entity);
  if (num_curves)
    path_bezier_chain (pts, num_curves, spline->degree);
  else if (spline_is_valid (spline))
    spline_path_points (spline);
  else
    spline_path_fit_points (spline);
  if (num_curves)
    free (pts);
  if (spline->closed_b)
    path_close ();
  path_end ();
}

// The decoded proxy graphics primitives of an entity, i.e. of a PROXY_ENTITY.
// Only the 2D geometry in the WCS (normal 0,0,1). The colors and layers of the
// sub entities, model transformations and clips are not applied.
// Returns the number of primitives.
static int
output_GRAPHICS (Dwg_Object *obj)
{
  Dwg_Proxy_Graphic *gfx = NULL;
  BITCODE_BL num = 0, i;

  if (entity_invisible (obj)
      || dwg_proxy_graphics (obj->tio.entity, &gfx, &num) || !num)
    {
      dwg_free_proxy_graphics (gfx, num);
      return 0;
    }
  set_pen (obj->tio.entity);
  for (i = 0; i < num; i++)
    {
      const Dwg_Proxy_Graphic *g = &gfx[i];
      switch (g->type)
        {
        case DWG_PROXY_GFX_CIRCLE:
          if (isnan_3RD (g->pt1) || isnan (g->radius))
            break;
          draw_circle (g->pt1.x, g->pt1.y, g->radius);
          break;
        case DWG_PROXY_GFX_CIRCULARARC:
          {
            const double a0 = atan2 (g->direction.y, g->direction.x);
            const double a1 = a0 + g->sweep;
            if (isnan_3RD (g->pt1) || isnan (g->radius) || isnan (g->sweep)
                || isnan (a0))
              break;
            path_move (g->pt1.x + g->radius * cos (a0),
                       g->pt1.y + g->radius * sin (a0));
            path_arc (g->pt1.x, g->pt1.y, g->pt1.x + g->radius * cos (a1),
                      g->pt1.y + g->radius * sin (a1), g->sweep >= 0.0);
            path_end ();
          }
          break;
        case DWG_PROXY_GFX_POLYLINE:
        case DWG_PROXY_GFX_POLYGON:
        case DWG_PROXY_GFX_POLYLINE_NORMAL:
          if (!g->num_points || !g->points)
            break;
          for (BITCODE_RL j = 0; j < g->num_points; j++)
            {
              if (j)
                path_line (g->points[j].x, g->points[j].y);
              else
                path_move (g->points[j].x, g->points[j].y);
            }
          if (g->type == DWG_PROXY_GFX_POLYGON)
            path_close ();
          path_end ();
          break;
        case DWG_PROXY_GFX_TEXT:
        case DWG_PROXY_GFX_TEXT2:
        case DWG_PROXY_GFX_UTEXT:
        case DWG_PROXY_GFX_UTEXT2:
          if (g->text && !isnan_3RD (g->pt1))
            put_label (g->pt1.x, g->pt1.y, g->height, 0.0, "Helvetica", 'l',
                       g->text);
          break;
        default:
          break;
        }
    }
  dwg_free_proxy_graphics (gfx, num);
  return 1;
}

// A block at its INSERT: the block coordinates are relative to its base
// point. The transformation of libplot nests with savestate.
static void
output_INSERT (Dwg_Object *obj)
{
  Dwg_Entity_INSERT *insert = obj->tio.entity->tio.INSERT;
  const Dwg_Object *blk;
  BITCODE_3DPOINT ins_pt;
  double base_x = 0.0, base_y = 0.0, a, b, c, d, k, sn, cs;

  if (entity_invisible (obj))
    return;
  if (!insert->block_header || !insert->block_header->handleref.value
      || !(blk = insert->block_header->obj))
    return;
  if (isnan_3BD (insert->ins_pt) || isnan_3BD (insert->extrusion)
      || isnan (insert->rotation) || isnan_3BD (insert->scale))
    return;
  if (block_depth >= MAX_BLOCK_DEPTH)
    {
      fprintf (stderr, "INSERT nested too deep, ignored\n");
      return;
    }
  if (blk->type == DWG_TYPE_BLOCK_HEADER && blk->tio.object
      && blk->tio.object->tio.BLOCK_HEADER)
    {
      base_x = blk->tio.object->tio.BLOCK_HEADER->base_pt.x;
      base_y = blk->tio.object->tio.BLOCK_HEADER->base_pt.y;
    }
  transform_OCS (&ins_pt, insert->ins_pt, insert->extrusion);
  // a mirrored OCS, e.g. the extrusion (0,0,-1), mirrors the x of the block
  k = 1 - 2 * transform_OCS_2d_is_mirrored (insert->extrusion);
  sn = sin (insert->rotation);
  cs = cos (insert->rotation);
  // p' = ins_pt + mirror (rotate (scale (p - base)))
  a = k * cs * insert->scale.x;
  b = sn * insert->scale.x;
  c = -k * sn * insert->scale.y;
  d = cs * insert->scale.y;
  {
    const double e = ins_pt.x - a * base_x - c * base_y;
    const double f = ins_pt.y - b * base_x - d * base_y;
    double saved[6];
    memcpy (saved, xf, sizeof xf);
    if (measuring) // the new transformation is the local one after the old
      {
        xf[0] = saved[0] * a + saved[2] * b;
        xf[1] = saved[1] * a + saved[3] * b;
        xf[2] = saved[0] * c + saved[2] * d;
        xf[3] = saved[1] * c + saved[3] * d;
        xf[4] = saved[0] * e + saved[2] * f + saved[4];
        xf[5] = saved[1] * e + saved[3] * f + saved[5];
      }
    else
      {
        pl_savestate_r (plotter);
        pl_fconcat_r (plotter, a, b, c, d, e, f);
      }
    block_depth++;
    output_BLOCK_HEADER (insert->block_header);
    block_depth--;
    if (measuring)
      memcpy (xf, saved, sizeof xf);
    else
      pl_restorestate_r (plotter);
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
    case DWG_TYPE_PROXY_ENTITY:
      num = output_GRAPHICS (obj);
      break;
    case DWG_TYPE_SEQEND:
    case DWG_TYPE_VIEWPORT:
      num = 0;
      break;
    default:
      num = 0;
      // the proxy graphics of the entity
      if (obj->supertype == DWG_SUPERTYPE_ENTITY)
        num = output_GRAPHICS (obj);
      if (!num && obj->supertype == DWG_SUPERTYPE_ENTITY)
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
    {
      fprintf (stderr, "Found null obj->tio.object\n");
      return 0;
    }
  if (!obj->tio.object->tio.BLOCK_HEADER)
    {
      fprintf (stderr, "Found null obj->tio.object->tio.BLOCK_HEADER\n");
      return 0;
    }

  obj = get_first_owned_entity (ref->obj);
  while (obj)
    {
      num += output_object (obj);
      obj = get_next_owned_entity (ref->obj, obj);
    }
  return num;
}

// The page of both spaces: paper space, if it has anything, else model space
static void
output_spaces (Dwg_Data *dwg)
{
  int num = 0;
  Dwg_Object_Ref *ref;
  if (!mspace && (ref = dwg_paper_space_ref (dwg)))
    num = output_BLOCK_HEADER (
        ref); // how many paper-space entities we did print
  if (!num && (ref = dwg_model_space_ref (dwg)))
    output_BLOCK_HEADER (ref);
}

static void
output_plot (Dwg_Data *dwg)
{
  double dx, dy, side, cx, cy;

  // the first pass measures the extents
  measuring = 1;
  block_depth = 0;
  bb_xmin = bb_ymin = 1e300;
  bb_xmax = bb_ymax = -1e300;
  output_spaces (dwg);
  measuring = 0;
  if (bb_xmin > bb_xmax || bb_ymin > bb_ymax) // nothing to draw
    {
      bb_xmin = bb_ymin = 0.0;
      bb_xmax = bb_ymax = 100.0;
    }
  model_xmin = bb_xmin;
  model_ymin = bb_ymin;
  model_xmax = bb_xmax;
  model_ymax = bb_ymax;

  dx = model_xmax - model_xmin;
  dy = model_ymax - model_ymin;
  // libplot maps the user space to a square viewport. Keep the aspect ratio
  // with a square one, and add a 2% margin.
  side = (dx > dy ? dx : dy) * 1.04;
  if (side <= 0.0)
    side = 1.0;
  cx = (model_xmin + model_xmax) / 2.0;
  cy = (model_ymin + model_ymax) / 2.0;
  line_unit = side / 1000.0;

  if (pl_openpl_r (plotter) < 0)
    {
      fprintf (stderr, "Cannot open the plotter\n");
      return;
    }
  pl_fspace_r (plotter, cx - side / 2, cy - side / 2, cx + side / 2,
               cy + side / 2);
  pl_filltype_r (plotter, 0);
  output_spaces (dwg);
  pl_closepl_r (plotter);
  fflush (stdout);
}

int
main (int argc, char *argv[])
{
  int error;
  int force_free = 0;
  int i = 1;
  int c;
  char *format = NULL;
  char *outfile = NULL;
  const char *pagesize = NULL;
  const char *bitmapsize = "1024x1024";
  FILE *outfh;
  plPlotterParams *params;
#ifdef HAVE_GETOPT_LONG
  int option_index = 0;
  static struct option long_options[]
      = { { "verbose", 1, &opts, 1 }, // optional
          { "output-format", 1, 0, 'T' },
          { "file", 1, 0, 'o' },
          { "pagesize", 1, 0, 'p' },
          { "bitmapsize", 1, 0, 'b' },
          { "mspace", 0, 0, 'm' },
          { "force-free", 0, 0, 0 },
          { "help", 0, 0, 0 },
          { "version", 0, 0, 0 },
          { NULL, 0, NULL, 0 } };
#endif

  if (argc < 2)
    return usage ();

  while
#ifdef HAVE_GETOPT_LONG
      ((c = getopt_long (argc, argv, ":v:mhT:o:p:b:", long_options,
                         &option_index))
       != -1)
#else
      ((c = getopt (argc, argv, ":v:mhiT:o:p:b:")) != -1)
#endif
    {
      if (c == -1)
        break;
      switch (c)
        {
        case ':': // missing arg
          if (optopt == 'v')
            {
              opts = 1;
              break;
            }
          fprintf (stderr, "%s: option '-%c' requires an argument\n", argv[0],
                   optopt);
          return usage ();
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
          break;
#else
        case 'i':
          return opt_version ();
#endif
        case 'T':
          format = optarg;
          break;
        case 'o':
          outfile = optarg;
          break;
        case 'p':
          pagesize = optarg;
          break;
        case 'b':
          bitmapsize = optarg;
          break;
        case 'm':
          mspace = 1;
          break;
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
  if (error >= DWG_ERR_CRITICAL)
    {
      fprintf (stderr, "Error: Could not read DWG file %s, error: 0x%x\n",
               argv[i], error);
      return 1;
    }

  if (!format)
    format = (char *)"meta"; // the plot(5) format
  if (outfile)
    {
      outfh = fopen (outfile, "wb");
      if (!outfh)
        {
          fprintf (stderr, "Cannot write %s\n", outfile);
          return 1;
        }
    }
  else
    outfh = stdout;
  params = pl_newplparams ();
  pl_setplparam (params, "BITMAPSIZE", (void *)bitmapsize);
  if (pagesize)
    pl_setplparam (params, "PAGESIZE", (void *)pagesize);
  plotter = pl_newpl_r (format, NULL, outfh, stderr, params);
  if (!plotter)
    {
      fprintf (stderr,
               "Unknown output format %s. Use one of png, pnm, gif, svg, ps, "
               "ai, cgm, fig, pcl, hpgl, regis, tek or meta\n",
               format);
      pl_deleteplparams (params);
      if (outfile)
        fclose (outfh);
      dwg_free (&g_dwg);
      return 1;
    }

  if (opts)
    fprintf (stderr, "\nplot\n====\n");
  output_plot (&g_dwg);
  pl_deletepl_r (plotter);
  pl_deleteplparams (params);
  if (outfile)
    fclose (outfh);

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
  return 0;
}

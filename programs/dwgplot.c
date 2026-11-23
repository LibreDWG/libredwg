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
 * Entities: TEXT, MTEXT, ATTRIB, ATTDEF, TOLERANCE, LINE, XLINE, RAY,
 *   CIRCLE, ARC, ELLIPSE, POINT, SOLID, TRACE, 3DFACE, 3DLINE, LWPOLYLINE,
 *   POLYLINE_2D/3D/MESH/PFACE, SPLINE, HELIX, MESH, HATCH (boundaries, solid
 *   ones filled), LEADER, MULTILEADER, MLINE, TABLE, INSERT, MINSERT,
 *   all DIMENSIONs via their generated block, IMAGE, WIPEOUT, UNDERLAY and
 *   OLE2FRAME as frames, the wireframe cache of 3DSOLID, REGION, BODY and
 *   the *SURFACEs, and the proxy graphics of all others.
 * TODO: SHAPE (needs its shx), HATCH patterns, VIEWPORT contents, block
 *       MULTILEADERs, merged TABLE cells, rich text, ltype, ltype_scale,
 *       BYLAYER and BYBLOCK colors, PLINE widths.
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

// The ASCII letters for the Latin Extended-A characters U+0100 to U+017F.
static const char latin_ext_a[]
    = "AaAaAaCcCcCcCcDdDdEeEeEeEeEeGgGgGgGgHhHhIiIiIiIiIiIiJjKkkLlLlLlLlLlNnNn"
      "N"
      "nnNnOoOoOoOoRrRrRrSsSsSsSsTtTtTtUuUuUuUuUuUuWwYyYZzZzZzs";

// UTF-8 to latin1, which the PostScript fonts of libplot have. The Latin
// Extended-A letters are their ASCII base, the others beyond latin1 are '?'.
// Returns a new string.
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
          if (cp < 0x100)
            *d++ = (char)cp;
          else if (cp < 0x180)
            *d++ = latin_ext_a[cp - 0x100];
          else
            *d++ = '?';
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

// The %%-codes of TEXT in place: %%d degree, %%p plus-minus, %%c diameter,
// %%nnn the character, and the toggles %%u underline and %%o overline are
// dropped. The result is UTF-8.
static char *
text_codes (char *restrict utf8)
{
  const char *s = utf8;
  char *dest = (char *)malloc (strlen (utf8) * 2 + 1);
  char *d = dest;
  if (!dest)
    return utf8;
  while (*s)
    {
      if (s[0] == '%' && s[1] == '%' && s[2])
        {
          const char c = s[2];
          if (c == 'd' || c == 'D')
            {
              *d++ = (char)0xc2; // U+00B0
              *d++ = (char)0xb0;
              s += 3;
            }
          else if (c == 'p' || c == 'P')
            {
              *d++ = (char)0xc2; // U+00B1
              *d++ = (char)0xb1;
              s += 3;
            }
          else if (c == 'c' || c == 'C')
            {
              *d++ = (char)0xc3; // U+00D8, the nearest to the diameter
              *d++ = (char)0x98;
              s += 3;
            }
          else if (c >= '0' && c <= '9' && s[3] >= '0' && s[3] <= '9'
                   && s[4] >= '0' && s[4] <= '9')
            {
              const int v = (c - '0') * 100 + (s[3] - '0') * 10 + (s[4] - '0');
              if (v >= 32 && v < 127)
                *d++ = (char)v;
              s += 5;
            }
          else if (c == 'u' || c == 'U' || c == 'o' || c == 'O' || c == '%')
            s += 3;
          else
            *d++ = *s++;
        }
      else
        *d++ = *s++;
    }
  *d = '\0';
  free (utf8);
  return dest;
}

// A label at x,y, rotated by angle degrees. hjust is l, c or r, vjust b, x
// (baseline), c or t.
static void
put_label (const double x, const double y, const double height,
           const double angle, const char *fontname, const int hjust,
           const int vjust, const char *restrict utf8)
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
  pl_alabel_r (plotter, hjust, vjust, label);
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

// A TEXT, ATTRIB or ATTDEF: the point is the alignment_pt for all
// alignments but left baseline, aligned and fit.
static void
draw_text_entity (Dwg_Object *obj, const BITCODE_2DPOINT ins_pt,
                  const BITCODE_2DPOINT alignment_pt, const BITCODE_BS horiz,
                  const BITCODE_BS vert, const double elevation,
                  const BITCODE_BE extrusion, const double height,
                  const double rotation, BITCODE_T text_value, BITCODE_H style)
{
  Dwg_Data *dwg = obj->parent;
  char *utf8;
  BITCODE_2DPOINT pt, at;
  int hjust = 'l', vjust = 'x';

  if (!text_value || entity_invisible (obj))
    return;
  if (isnan_2BD (ins_pt) || isnan_3BD (extrusion))
    return;
  utf8 = text_utf8 (dwg, text_value);
  if (!utf8)
    return;
  utf8 = text_codes (utf8);
  at = ins_pt;
  if ((horiz == 1 || horiz == 2 || horiz == 4 || vert) && horiz != 3
      && horiz != 5 && !isnan_2BD (alignment_pt))
    at = alignment_pt;
  hjust = horiz == 1 || horiz == 4 ? 'c' : horiz == 2 ? 'r' : 'l';
  vjust = horiz == 4  ? 'c'
          : vert == 1 ? 'b'
          : vert == 2 ? 'c'
          : vert == 3 ? 't'
                      : 'x';
  transform_OCS_elevation (&pt, at, elevation, extrusion);
  set_pen (obj->tio.entity);
  put_label (pt.x, pt.y, height /* fontsize */,
             isnan (rotation) ? 0.0 : rotation * 180.0 / M_PI,
             text_fontname (dwg, style), hjust, vjust, utf8);
  free (utf8);
}

static void
output_TEXT (Dwg_Object *obj)
{
  Dwg_Entity_TEXT *text = obj->tio.entity->tio.TEXT;
  draw_text_entity (obj, text->ins_pt, text->alignment_pt,
                    text->horiz_alignment, text->vert_alignment,
                    text->elevation, text->extrusion, text->height,
                    text->rotation, text->text_value, text->style);
}

// The visible attribute value of an INSERT. Invisible ones, flag 1, not.
static void
output_ATTRIB (Dwg_Object *obj)
{
  Dwg_Entity_ATTRIB *attr = obj->tio.entity->tio.ATTRIB;
  if (attr->flags & 1)
    return;
  draw_text_entity (obj, attr->ins_pt, attr->alignment_pt,
                    attr->horiz_alignment, attr->vert_alignment,
                    attr->elevation, attr->extrusion, attr->height,
                    attr->rotation, attr->text_value, attr->style);
}

// The template of an attribute is shown by its tag, only outside of blocks:
// an INSERT shows its ATTRIBs instead.
static void
output_ATTDEF (Dwg_Object *obj)
{
  Dwg_Entity_ATTDEF *attdef = obj->tio.entity->tio.ATTDEF;
  if (block_depth > 0 || (attdef->flags & 1))
    return;
  draw_text_entity (obj, attdef->ins_pt, attdef->alignment_pt,
                    attdef->horiz_alignment, attdef->vert_alignment,
                    attdef->elevation, attdef->extrusion, attdef->height,
                    attdef->rotation, attdef->tag, attdef->style);
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
                 mtext_hjust (mtext->attachment), 'x', line);
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
  output_outline4 (c1, c2, c4, c3); // the corners 3 and 4 are swapped
}

static void
output_TRACE (Dwg_Object *obj)
{
  Dwg_Entity_TRACE *trace = obj->tio.entity->tio.TRACE;
  BITCODE_2DPOINT c1, c2, c3, c4;
  BITCODE_2DPOINT s1, s2, s3, s4;

  memcpy (&s1, &trace->corner1, sizeof s1);
  memcpy (&s2, &trace->corner2, sizeof s1);
  memcpy (&s3, &trace->corner3, sizeof s1);
  memcpy (&s4, &trace->corner4, sizeof s1);
  if (isnan_2BD (s1) || isnan_2BD (s2) || isnan_2BD (s3) || isnan_2BD (s4)
      || entity_invisible (obj))
    return;
  transform_OCS_elevation (&c1, s1, trace->elevation, trace->extrusion);
  transform_OCS_elevation (&c2, s2, trace->elevation, trace->extrusion);
  transform_OCS_elevation (&c3, s3, trace->elevation, trace->extrusion);
  transform_OCS_elevation (&c4, s4, trace->elevation, trace->extrusion);
  set_pen (obj->tio.entity);
  output_outline4 (c1, c2, c4, c3); // the corners 3 and 4 are swapped
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
draw_spline (Dwg_Object *obj, const Dwg_Entity_SPLINE *spline)
{
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

static void
output_SPLINE (Dwg_Object *obj)
{
  draw_spline (obj, obj->tio.entity->tio.SPLINE);
}

// A HELIX is a SPLINE with more fields
static void
output_HELIX (Dwg_Object *obj)
{
  const Dwg_Entity_HELIX *helix = obj->tio.entity->tio.HELIX;
  Dwg_Entity_SPLINE spline;
  memset (&spline, 0, sizeof spline);
  spline.parent = helix->parent;
  spline.flag = helix->flag;
  spline.scenario = helix->scenario;
  spline.degree = helix->degree;
  spline.splineflags = helix->splineflags;
  spline.knotparam = helix->knotparam;
  spline.fit_tol = helix->fit_tol;
  spline.closed_b = helix->closed_b;
  spline.periodic = helix->periodic;
  spline.rational = helix->rational;
  spline.weighted = helix->weighted;
  spline.knot_tol = helix->knot_tol;
  spline.ctrl_tol = helix->ctrl_tol;
  spline.num_fit_pts = helix->num_fit_pts;
  spline.fit_pts = helix->fit_pts;
  spline.num_knots = helix->num_knots;
  spline.knots = helix->knots;
  spline.num_ctrl_pts = helix->num_ctrl_pts;
  spline.ctrl_pts = helix->ctrl_pts;
  draw_spline (obj, &spline);
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
                       'x', g->text);
          break;
        default:
          break;
        }
    }
  dwg_free_proxy_graphics (gfx, num);
  return 1;
}

static void output_attribs (Dwg_Object *obj);

// A block at its INSERT, or each of the cols x rows of a MINSERT: the block
// coordinates are relative to its base point. The transformation of libplot
// nests with savestate.
static void
draw_insert (Dwg_Object *obj, const BITCODE_H block_header,
             const BITCODE_3DPOINT ins, const BITCODE_BE extrusion,
             const BITCODE_3DPOINT scale, const double rotation,
             const int cols, const int rows, const double col_spacing,
             const double row_spacing)
{
  const Dwg_Object *blk;
  BITCODE_3DPOINT ins_pt;
  double base_x = 0.0, base_y = 0.0, a, b, c, d, k, sn, cs;
  int row, col;

  if (entity_invisible (obj))
    return;
  if (!block_header || !block_header->handleref.value
      || !(blk = block_header->obj))
    return;
  if (isnan_3BD (ins) || isnan_3BD (extrusion) || isnan (rotation)
      || isnan_3BD (scale))
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
  transform_OCS (&ins_pt, ins, extrusion);
  // a mirrored OCS, e.g. the extrusion (0,0,-1), mirrors the x of the block
  k = 1 - 2 * transform_OCS_2d_is_mirrored (extrusion);
  sn = sin (rotation);
  cs = cos (rotation);
  // p' = ins_pt + mirror (rotate (scale (p - base)))
  a = k * cs * scale.x;
  b = sn * scale.x;
  c = -k * sn * scale.y;
  d = cs * scale.y;
  for (row = 0; row < rows; row++)
    for (col = 0; col < cols; col++)
      {
        // the array is spaced in the rotated, not scaled, frame
        const double ox = col * col_spacing, oy = row * row_spacing;
        const double e
            = ins_pt.x + k * (cs * ox - sn * oy) - a * base_x - c * base_y;
        const double f
            = ins_pt.y + sn * ox + cs * oy - b * base_x - d * base_y;
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
        output_BLOCK_HEADER (block_header);
        block_depth--;
        if (measuring)
          memcpy (xf, saved, sizeof xf);
        else
          pl_restorestate_r (plotter);
      }
  output_attribs (obj);
}

static void
output_INSERT (Dwg_Object *obj)
{
  Dwg_Entity_INSERT *insert = obj->tio.entity->tio.INSERT;
  draw_insert (obj, insert->block_header, insert->ins_pt, insert->extrusion,
               insert->scale, insert->rotation, 1, 1, 0.0, 0.0);
}

static void
output_MINSERT (Dwg_Object *obj)
{
  Dwg_Entity_MINSERT *insert = obj->tio.entity->tio.MINSERT;
  draw_insert (obj, insert->block_header, insert->ins_pt, insert->extrusion,
               insert->scale, insert->rotation,
               insert->num_cols ? insert->num_cols : 1,
               insert->num_rows ? insert->num_rows : 1, insert->col_spacing,
               insert->row_spacing);
}

// The ATTRIBs of an INSERT or MINSERT. Their coordinates are not those of
// the block.
static void
output_attribs (Dwg_Object *obj)
{
  Dwg_Object *attr;
  BITCODE_BL n = 0;
  // only INSERT and MINSERT have attribs
  if ((obj->fixedtype != DWG_TYPE_INSERT && obj->fixedtype != DWG_TYPE_MINSERT
       && obj->fixedtype != DWG_TYPE_ACIDBLOCKREFERENCE)
      || block_depth > 0)
    return;
  for (attr = get_first_owned_subentity (obj); attr && n < 100000;
       attr = get_next_owned_subentity (obj, attr), n++)
    if (attr->fixedtype == DWG_TYPE_ATTRIB)
      output_ATTRIB (attr);
}

/* Dimensions: the generated anonymous block *D has all their graphics */

static void
output_DIMENSION (Dwg_Object *obj)
{
  // all DIMENSION_* have the same common fields first
  Dwg_Entity_DIMENSION_LINEAR *dim = obj->tio.entity->tio.DIMENSION_LINEAR;
  if (entity_invisible (obj) || !dim || !dim->block)
    return;
  if (block_depth >= MAX_BLOCK_DEPTH)
    return;
  block_depth++;
  output_BLOCK_HEADER (dim->block);
  block_depth--;
}

/* Polylines of vertices */

// The vertex entities of a POLYLINE_3D, MESH or PFACE, in order, as new
// array. Returns their number.
static int
polyline_vertices (Dwg_Object *obj, Dwg_Object ***verts)
{
  Dwg_Object *v;
  Dwg_Object **list = NULL;
  int n = 0, cap = 0;
  for (v = get_first_owned_subentity (obj); v && n < 1000000;
       v = get_next_owned_subentity (obj, v))
    {
      if (v->fixedtype == DWG_TYPE_SEQEND)
        break;
      if (n == cap)
        {
          Dwg_Object **tmp;
          cap = cap ? cap * 2 : 64;
          tmp = (Dwg_Object **)realloc (list, cap * sizeof (Dwg_Object *));
          if (!tmp)
            break;
          list = tmp;
        }
      list[n++] = v;
    }
  *verts = list;
  return n;
}

static void
output_POLYLINE_3D (Dwg_Object *obj)
{
  Dwg_Entity_POLYLINE_3D *pline = obj->tio.entity->tio.POLYLINE_3D;
  Dwg_Object **verts;
  int j, n, first = 1;

  if (entity_invisible (obj))
    return;
  n = polyline_vertices (obj, &verts);
  set_pen (obj->tio.entity);
  // the points are WCS, drawn in the XY plane. The frame of a spline-fit
  // polyline, vertices with flag 16, is not drawn.
  for (j = 0; j < n; j++)
    {
      Dwg_Entity_VERTEX_3D *v;
      if (verts[j]->fixedtype != DWG_TYPE_VERTEX_3D || !verts[j]->tio.entity)
        continue;
      v = verts[j]->tio.entity->tio.VERTEX_3D;
      if (v->flag & 16 || isnan_3BD (v->point))
        continue;
      if (first)
        path_move (v->point.x, v->point.y);
      else
        path_line (v->point.x, v->point.y);
      first = 0;
    }
  if (!first && (pline->flag & 1))
    path_close ();
  path_end ();
  free (verts);
}

static void
output_POLYLINE_MESH (Dwg_Object *obj)
{
  Dwg_Entity_POLYLINE_MESH *mesh = obj->tio.entity->tio.POLYLINE_MESH;
  Dwg_Object **verts;
  BITCODE_3BD *pts;
  int j, m = mesh->num_m_verts, n = mesh->num_n_verts, i, num, k = 0;

  if (entity_invisible (obj) || m < 1 || n < 1)
    return;
  num = polyline_vertices (obj, &verts);
  pts = (BITCODE_3BD *)malloc ((size_t)m * n * sizeof (BITCODE_3BD));
  if (!pts)
    {
      free (verts);
      return;
    }
  for (j = 0; j < num && k < m * n; j++)
    if (verts[j]->fixedtype == DWG_TYPE_VERTEX_MESH && verts[j]->tio.entity)
      pts[k++] = verts[j]->tio.entity->tio.VERTEX_MESH->point;
  free (verts);
  if (k == m * n)
    {
      set_pen (obj->tio.entity);
      // m rows of n vertices: the lines along m and along n, closed with
      // flag 1 for m and 32 for n
      for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
          {
            const BITCODE_3BD *p = &pts[i * n + j];
            if (isnan_3BD (*p))
              continue;
            if (j + 1 < n || (mesh->flag & 32))
              {
                const BITCODE_3BD *q = &pts[i * n + (j + 1) % n];
                if (!isnan_3BD (*q))
                  draw_line (p->x, p->y, q->x, q->y);
              }
            if (i + 1 < m || (mesh->flag & 1))
              {
                const BITCODE_3BD *q = &pts[((i + 1) % m) * n + j];
                if (!isnan_3BD (*q))
                  draw_line (p->x, p->y, q->x, q->y);
              }
          }
    }
  free (pts);
}

// The vertices come first, then the faces with the 1-based vertex indices.
// A negative index starts an invisible edge.
static void
output_POLYLINE_PFACE (Dwg_Object *obj)
{
  Dwg_Entity_POLYLINE_PFACE *pf = obj->tio.entity->tio.POLYLINE_PFACE;
  Dwg_Object **verts;
  BITCODE_3BD *pts;
  int j, n, num, k = 0;

  if (entity_invisible (obj))
    return;
  n = pf->numverts;
  num = polyline_vertices (obj, &verts);
  pts = (BITCODE_3BD *)calloc (n > 0 ? (size_t)n : 1, sizeof (BITCODE_3BD));
  if (!pts)
    {
      free (verts);
      return;
    }
  set_pen (obj->tio.entity);
  for (j = 0; j < num; j++)
    {
      Dwg_Object *v = verts[j];
      if (!v->tio.entity)
        continue;
      if (v->fixedtype == DWG_TYPE_VERTEX_PFACE && k < n)
        pts[k++] = v->tio.entity->tio.VERTEX_PFACE->point;
      else if (v->fixedtype == DWG_TYPE_VERTEX_PFACE_FACE)
        {
          const Dwg_Entity_VERTEX_PFACE_FACE *face
              = v->tio.entity->tio.VERTEX_PFACE_FACE;
          int e, len = 0;
          for (e = 0; e < 4 && face->vertind[e]; e++)
            len++;
          for (e = 0; e < len; e++)
            {
              const int ia = face->vertind[e];
              const int ib = face->vertind[(e + 1) % len];
              const int a = abs (ia) - 1, b = abs (ib) - 1;
              // the sign of the first index of an edge hides it
              if (ia < 0 || a < 0 || b < 0 || a >= k || b >= k
                  || isnan_3BD (pts[a]) || isnan_3BD (pts[b]))
                continue;
              draw_line (pts[a].x, pts[a].y, pts[b].x, pts[b].y);
            }
        }
    }
  free (pts);
  free (verts);
}

/* LEADER, MLINE, TOLERANCE */

// The leader line through the points, with its arrow head. The spline
// leader path_type is drawn straight.
static void
output_LEADER (Dwg_Object *obj)
{
  Dwg_Entity_LEADER *leader = obj->tio.entity->tio.LEADER;
  BITCODE_BL j;

  if (entity_invisible (obj) || !leader->points || leader->num_points < 2
      || isnan_3BD (leader->extrusion))
    return;
  for (j = 0; j < leader->num_points; j++)
    if (isnan_3BD (leader->points[j]))
      return;
  set_pen (obj->tio.entity);
  for (j = 0; j < leader->num_points; j++)
    {
      if (j)
        path_line (leader->points[j].x, leader->points[j].y);
      else
        path_move (leader->points[j].x, leader->points[j].y);
    }
  path_end ();
  if (leader->arrowhead_on && leader->dimasz > 0.0)
    {
      const double dx = leader->points[1].x - leader->points[0].x;
      const double dy = leader->points[1].y - leader->points[0].y;
      const double len = sqrt (dx * dx + dy * dy);
      if (len > 0.0)
        {
          // a closed arrow, 1/3 as wide as long
          const double ux = dx / len, uy = dy / len;
          const double sz = leader->dimasz;
          const double bx = leader->points[0].x + ux * sz;
          const double by = leader->points[0].y + uy * sz;
          path_move (leader->points[0].x, leader->points[0].y);
          path_line (bx - uy * sz / 6.0, by + ux * sz / 6.0);
          path_line (bx + uy * sz / 6.0, by - ux * sz / 6.0);
          path_close ();
          path_end ();
        }
    }
}

// The elements of the style are offset along the miter direction at each
// vertex, scaled by the MLINE scale. The justification moves the lines:
// 0 top, 1 zero, 2 bottom.
static void
output_MLINE (Dwg_Object *obj)
{
  Dwg_Entity_MLINE *mline = obj->tio.entity->tio.MLINE;
  const Dwg_Object_MLINESTYLE *style = NULL;
  double shift = 0.0, maxo = 0.0, mino = 0.0;
  int e, num_lines = mline->num_lines;
  BITCODE_BS v;

  if (entity_invisible (obj) || !mline->verts || mline->num_verts < 2)
    return;
  if (mline->mlinestyle && mline->mlinestyle->obj
      && mline->mlinestyle->obj->fixedtype == DWG_TYPE_MLINESTYLE
      && mline->mlinestyle->obj->tio.object)
    style = mline->mlinestyle->obj->tio.object->tio.MLINESTYLE;
  if (style && style->lines && style->num_lines > 0)
    {
      for (e = 0; e < style->num_lines; e++)
        {
          if (!e || style->lines[e].offset > maxo)
            maxo = style->lines[e].offset;
          if (!e || style->lines[e].offset < mino)
            mino = style->lines[e].offset;
        }
      if (num_lines > style->num_lines)
        num_lines = style->num_lines;
    }
  else
    return;
  shift = mline->justification == 0   ? -maxo
          : mline->justification == 2 ? -mino
                                      : 0.0;
  set_pen (obj->tio.entity);
  for (e = 0; e < num_lines; e++)
    {
      const double off = (style->lines[e].offset + shift) * mline->scale;
      int first = 1;
      for (v = 0; v < mline->num_verts; v++)
        {
          const Dwg_MLINE_vertex *vx = &mline->verts[v];
          const double x = vx->vertex.x + vx->miter_direction.x * off;
          const double y = vx->vertex.y + vx->miter_direction.y * off;
          if (isnan (x) || isnan (y))
            continue;
          if (first)
            path_move (x, y);
          else
            path_line (x, y);
          first = 0;
        }
      if (!first && (mline->flags & 2)) // closed
        path_close ();
      path_end ();
    }
}

// The text with its ^J line breaks of the tolerance frame. The symbols are
// font characters {\Fgdt;j}, kept as their letter.
static void
output_TOLERANCE (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_TOLERANCE *tol = obj->tio.entity->tio.TOLERANCE;
  char *utf8, *plain, *line;
  double angle = 0.0;
  int idx = 0;

  if (entity_invisible (obj) || !tol->text_value || isnan_3BD (tol->ins_pt)
      || isnan (tol->height))
    return;
  utf8 = text_utf8 (dwg, tol->text_value);
  if (!utf8)
    return;
  plain = mtext_plaintext (utf8);
  free (utf8);
  if (!plain)
    return;
  if (!isnan_3BD (tol->x_direction)
      && (tol->x_direction.x != 0.0 || tol->x_direction.y != 0.0))
    angle = atan2 (tol->x_direction.y, tol->x_direction.x);
  set_pen (obj->tio.entity);
  for (line = plain; line; idx++)
    {
      char *nl = strstr (line, "^J");
      const double d = idx * tol->height * 1.6;
      if (nl)
        *nl = '\0';
      put_label (tol->ins_pt.x + d * sin (angle),
                 tol->ins_pt.y - d * cos (angle), tol->height,
                 angle * 180.0 / M_PI, "Helvetica", 'l', 'x', line);
      if (!nl)
        break;
      line = nl + 2;
    }
  free (plain);
}

/* IMAGE, WIPEOUT, UNDERLAYs: their frame */

static void
output_polygon (Dwg_Object_Entity *ent, const BITCODE_2DPOINT *pts,
                const int num)
{
  int j;
  if (num < 2)
    return;
  set_pen (ent);
  for (j = 0; j < num; j++)
    {
      if (j)
        path_line (pts[j].x, pts[j].y);
      else
        path_move (pts[j].x, pts[j].y);
    }
  path_close ();
  path_end ();
}

// The frame of the pixel size, or the clip boundary in pixel coordinates.
static void
output_IMAGE_frame (Dwg_Object *obj, const BITCODE_3BD pt0,
                    const BITCODE_3BD uvec, const BITCODE_3BD vvec,
                    const BITCODE_2RD image_size,
                    const BITCODE_BS clip_boundary_type,
                    const BITCODE_BL num_clip_verts,
                    const BITCODE_2RD *clip_verts)
{
  BITCODE_2DPOINT pts[4];
  BITCODE_2DPOINT *poly = NULL;
  BITCODE_BL j;

  if (entity_invisible (obj) || isnan_3BD (pt0) || isnan_3BD (uvec)
      || isnan_3BD (vvec) || isnan (image_size.x) || isnan (image_size.y))
    return;
  if (clip_boundary_type == 2 && clip_verts && num_clip_verts > 2)
    {
      poly = (BITCODE_2DPOINT *)malloc (num_clip_verts
                                        * sizeof (BITCODE_2DPOINT));
      if (poly)
        {
          // the clip vertices are in pixels from the corner pt0
          for (j = 0; j < num_clip_verts; j++)
            {
              poly[j].x = pt0.x + uvec.x * (clip_verts[j].x + 0.5)
                          + vvec.x * (clip_verts[j].y + 0.5);
              poly[j].y = pt0.y + uvec.y * (clip_verts[j].x + 0.5)
                          + vvec.y * (clip_verts[j].y + 0.5);
            }
          output_polygon (obj->tio.entity, poly, (int)num_clip_verts);
          free (poly);
          return;
        }
    }
  pts[0].x = pt0.x;
  pts[0].y = pt0.y;
  pts[1].x = pt0.x + uvec.x * image_size.x;
  pts[1].y = pt0.y + uvec.y * image_size.x;
  pts[2].x = pts[1].x + vvec.x * image_size.y;
  pts[2].y = pts[1].y + vvec.y * image_size.y;
  pts[3].x = pt0.x + vvec.x * image_size.y;
  pts[3].y = pt0.y + vvec.y * image_size.y;
  output_polygon (obj->tio.entity, pts, 4);
}

static void
output_IMAGE (Dwg_Object *obj)
{
  Dwg_Entity_IMAGE *img = obj->tio.entity->tio.IMAGE;
  output_IMAGE_frame (obj, img->pt0, img->uvec, img->vvec, img->image_size,
                      img->clip_boundary_type, img->num_clip_verts,
                      img->clip_verts);
}

static void
output_WIPEOUT (Dwg_Object *obj)
{
  Dwg_Entity_WIPEOUT *img = obj->tio.entity->tio.WIPEOUT;
  output_IMAGE_frame (obj, img->pt0, img->uvec, img->vvec, img->image_size,
                      img->clip_boundary_type, img->num_clip_verts,
                      img->clip_verts);
}

// The clip boundary of an underlay in its own units, scaled, rotated and
// moved to ins_pt. Without a clip the size is not known here.
static void
output_UNDERLAY (Dwg_Object *obj)
{
  Dwg_Entity_UNDERLAY *u = obj->tio.entity->tio.PDFUNDERLAY;
  BITCODE_2DPOINT *poly;
  BITCODE_BL j, n;
  const double sn = isnan (u->angle) ? 0.0 : sin (u->angle);
  const double cs = isnan (u->angle) ? 1.0 : cos (u->angle);

  if (entity_invisible (obj) || !u->clip_verts || u->num_clip_verts < 2
      || isnan_3BD (u->ins_pt) || isnan_3BD (u->scale))
    return;
  n = u->num_clip_verts == 2 ? 4 : u->num_clip_verts;
  poly = (BITCODE_2DPOINT *)malloc (n * sizeof (BITCODE_2DPOINT));
  if (!poly)
    return;
  for (j = 0; j < n; j++)
    {
      double x, y;
      if (u->num_clip_verts == 2) // a rectangle of two corners
        {
          x = (j == 1 || j == 2) ? u->clip_verts[1].x : u->clip_verts[0].x;
          y = (j >= 2) ? u->clip_verts[1].y : u->clip_verts[0].y;
        }
      else
        {
          x = u->clip_verts[j].x;
          y = u->clip_verts[j].y;
        }
      x *= u->scale.x;
      y *= u->scale.y;
      poly[j].x = u->ins_pt.x + x * cs - y * sn;
      poly[j].y = u->ins_pt.y + x * sn + y * cs;
    }
  output_polygon (obj->tio.entity, poly, (int)n);
  free (poly);
}

/* HATCH: the boundary loops, solid ones filled */

// The loop of a HATCH path as 2D points in the WCS. Circular and elliptical
// arcs are sampled, a spline is its control polygon.
static void
hatch_seg_points (const Dwg_Entity_HATCH *hatch, const Dwg_HATCH_PathSeg *seg,
                  const int first)
{
  BITCODE_2DPOINT p, q;
  BITCODE_BL j;
  int k, steps;
  double a0, a1, span;
  const BITCODE_2DPOINT zero = { 0.0, 0.0 };
  (void)zero;

#define HATCH_PT(pt)                                                          \
  p.x = (pt).x;                                                               \
  p.y = (pt).y;                                                               \
  transform_OCS_elevation (&q, p, hatch->elevation, hatch->extrusion)
  switch (seg->curve_type)
    {
    case 1: // LINE
      HATCH_PT (seg->first_endpoint);
      if (first)
        path_move (q.x, q.y);
      else
        path_line (q.x, q.y);
      HATCH_PT (seg->second_endpoint);
      path_line (q.x, q.y);
      break;
    case 2: // CIRCULAR ARC, angles in radians
    case 3: // ELLIPTICAL ARC, endpoint is the major axis, relative
      a0 = seg->start_angle;
      a1 = seg->end_angle;
      if (isnan (a0) || isnan (a1))
        break;
      if (seg->is_ccw) // counterclockwise from a0 to a1
        {
          span = fmod (a1 - a0, 2 * M_PI);
          if (span <= 0.0)
            span += 2 * M_PI;
        }
      else
        {
          span = -fmod (a0 - a1, 2 * M_PI);
          if (span >= 0.0)
            span -= 2 * M_PI;
        }
      steps = (int)ceil (ELLIPSE_STEPS * fabs (span) / (2 * M_PI));
      if (steps < 8)
        steps = 8;
      for (k = 0; k <= steps; k++)
        {
          const double u = a0 + span * k / steps;
          double x, y;
          if (seg->curve_type == 2)
            {
              x = seg->center.x + seg->radius * cos (u);
              y = seg->center.y + seg->radius * sin (u);
            }
          else // the major axis, and the minor axis rotated 90 degrees
            {
              const double mx = seg->endpoint.x, my = seg->endpoint.y;
              x = seg->center.x + mx * cos (u)
                  - my * seg->minor_major_ratio * sin (u);
              y = seg->center.y + my * cos (u)
                  + mx * seg->minor_major_ratio * sin (u);
            }
          p.x = x;
          p.y = y;
          transform_OCS_elevation (&q, p, hatch->elevation, hatch->extrusion);
          if (first && !k)
            path_move (q.x, q.y);
          else
            path_line (q.x, q.y);
        }
      break;
    case 4: // SPLINE
      for (j = 0; j < seg->num_control_points && seg->control_points; j++)
        {
          HATCH_PT (seg->control_points[j].point);
          if (first && !j)
            path_move (q.x, q.y);
          else
            path_line (q.x, q.y);
        }
      break;
    default:
      break;
    }
#undef HATCH_PT
}

static void
output_HATCH (Dwg_Object *obj)
{
  Dwg_Entity_HATCH *hatch = obj->tio.entity->tio.HATCH;
  BITCODE_BL i;
  const int solid = hatch->is_solid_fill && !measuring;

  if (entity_invisible (obj) || !hatch->paths || isnan_3BD (hatch->extrusion))
    return;
  set_pen (obj->tio.entity);
  if (solid)
    {
      pl_filltype_r (plotter, 1);
      pl_fillmod_r (plotter, "even-odd");
    }
  for (i = 0; i < hatch->num_paths; i++)
    {
      const Dwg_HATCH_Path *path = &hatch->paths[i];
      BITCODE_BL j;
      int first = 1;
      if (path->flag & 2) // polyline path, with bulges
        {
          BITCODE_2DPOINT prev, pt, ptin, start;
          BITCODE_BL prev_j = 0;
          if (!path->polyline_paths || !path->num_segs_or_paths)
            continue;
          ptin.x = path->polyline_paths[0].point.x;
          ptin.y = path->polyline_paths[0].point.y;
          transform_OCS_elevation (&pt, ptin, hatch->elevation,
                                   hatch->extrusion);
          start = prev = pt;
          path_move (pt.x, pt.y);
          for (j = 1; j < path->num_segs_or_paths; j++)
            {
              ptin.x = path->polyline_paths[j].point.x;
              ptin.y = path->polyline_paths[j].point.y;
              transform_OCS_elevation (&pt, ptin, hatch->elevation,
                                       hatch->extrusion);
              path_segment (prev, pt,
                            path->bulges_present
                                ? path->polyline_paths[prev_j].bulge
                                : 0.0,
                            transform_OCS_2d_is_mirrored (hatch->extrusion));
              prev = pt;
              prev_j = j;
            }
          if (path->closed)
            path_segment (prev, start,
                          path->bulges_present
                              ? path->polyline_paths[prev_j].bulge
                              : 0.0,
                          transform_OCS_2d_is_mirrored (hatch->extrusion));
        }
      else
        {
          if (!path->segs)
            continue;
          for (j = 0; j < path->num_segs_or_paths; j++)
            {
              hatch_seg_points (hatch, &path->segs[j], first);
              first = 0;
            }
        }
      if (solid)
        pl_endsubpath_r (plotter); // a compound path: the loops are holes
      else
        {
          path_close ();
          path_end ();
        }
    }
  if (solid)
    {
      pl_endpath_r (plotter);
      pl_filltype_r (plotter, 0);
    }
}

/* 3D entities: the wireframe cache, projected to the XY plane */

static void
output_wires (Dwg_Object *obj)
{
  // all of 3DSOLID, REGION, BODY and the *SURFACEs start with these fields
  const Dwg_Entity__3DSOLID *solid = obj->tio.entity->tio._3DSOLID;
  BITCODE_BL i, j;

  if (entity_invisible (obj) || !solid->wires || !solid->num_wires)
    return;
  set_pen (obj->tio.entity);
  for (i = 0; i < solid->num_wires; i++)
    {
      const Dwg_3DSOLID_wire *wire = &solid->wires[i];
      int first = 1;
      if (!wire->points)
        continue;
      for (j = 0; j < wire->num_points; j++)
        {
          if (isnan_3BD (wire->points[j]))
            continue;
          if (first)
            path_move (wire->points[j].x, wire->points[j].y);
          else
            path_line (wire->points[j].x, wire->points[j].y);
          first = 0;
        }
      path_end ();
    }
}

static void
output_MESH (Dwg_Object *obj)
{
  const Dwg_Entity_MESH *mesh = obj->tio.entity->tio.MESH;
  BITCODE_BL i;

  if (entity_invisible (obj) || !mesh->vertex || !mesh->num_vertex)
    return;
  set_pen (obj->tio.entity);
  if (mesh->edges && mesh->num_edges)
    {
      for (i = 0; i < mesh->num_edges; i++)
        {
          const Dwg_MESH_edge *e = &mesh->edges[i];
          if (e->idxfrom >= mesh->num_vertex || e->idxto >= mesh->num_vertex
              || isnan_3BD (mesh->vertex[e->idxfrom])
              || isnan_3BD (mesh->vertex[e->idxto]))
            continue;
          draw_line (mesh->vertex[e->idxfrom].x, mesh->vertex[e->idxfrom].y,
                     mesh->vertex[e->idxto].x, mesh->vertex[e->idxto].y);
        }
    }
  else if (mesh->faces) // n, then n vertex indices, for each face
    {
      BITCODE_BL pos = 0;
      while (pos < mesh->num_faces)
        {
          const BITCODE_BL n = mesh->faces[pos++];
          BITCODE_BL k;
          if (n < 2 || pos + n > mesh->num_faces)
            break;
          for (k = 0; k < n; k++)
            {
              const BITCODE_BL a = mesh->faces[pos + k];
              const BITCODE_BL b = mesh->faces[pos + (k + 1) % n];
              if (a >= mesh->num_vertex || b >= mesh->num_vertex
                  || isnan_3BD (mesh->vertex[a])
                  || isnan_3BD (mesh->vertex[b]))
                continue;
              draw_line (mesh->vertex[a].x, mesh->vertex[a].y,
                         mesh->vertex[b].x, mesh->vertex[b].y);
            }
          pos += n;
        }
    }
}

// The frame of the OLE object: upper left, upper right, lower right, lower
// left
static void
output_OLE2FRAME (Dwg_Object *obj)
{
  const Dwg_Entity_OLE2FRAME *ole = obj->tio.entity->tio.OLE2FRAME;
  BITCODE_2DPOINT pts[4];

  if (entity_invisible (obj) || isnan_3RD (ole->pt1) || isnan_3RD (ole->pt_ur)
      || isnan_3RD (ole->pt2) || isnan_3RD (ole->pt_ll))
    return;
  pts[0].x = ole->pt1.x;
  pts[0].y = ole->pt1.y;
  pts[1].x = ole->pt_ur.x;
  pts[1].y = ole->pt_ur.y;
  pts[2].x = ole->pt2.x;
  pts[2].y = ole->pt2.y;
  pts[3].x = ole->pt_ll.x;
  pts[3].y = ole->pt_ll.y;
  output_polygon (obj->tio.entity, pts, 4);
}

// The old r10 3DLINE
static void
output_3DLINE (Dwg_Object *obj)
{
  const Dwg_Entity__3DLINE *line = obj->tio.entity->tio._3DLINE;
  if (entity_invisible (obj) || isnan_3RD (line->start)
      || isnan_3RD (line->end))
    return;
  set_pen (obj->tio.entity);
  draw_line (line->start.x, line->start.y, line->end.x, line->end.y);
}

/* MULTILEADER and TABLE */

// The leader lines and doglegs of its nodes, the arrow heads, and the text.
// Block content is not drawn.
static void
output_MULTILEADER (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_MULTILEADER *ml = obj->tio.entity->tio.MULTILEADER;
  const Dwg_MLEADER_AnnotContext *ctx = &ml->ctx;
  BITCODE_BL i, j, k;

  if (entity_invisible (obj))
    return;
  set_pen (obj->tio.entity);
  for (i = 0; ctx->leaders && i < ctx->num_leaders; i++)
    {
      const Dwg_LEADER_Node *node = &ctx->leaders[i];
      for (j = 0; node->lines && j < node->num_lines; j++)
        {
          const Dwg_LEADER_Line *line = &node->lines[j];
          int first = 1;
          for (k = 0; line->points && k < line->num_points; k++)
            {
              if (isnan_3BD (line->points[k]))
                continue;
              if (first)
                path_move (line->points[k].x, line->points[k].y);
              else
                path_line (line->points[k].x, line->points[k].y);
              first = 0;
            }
          // the dogleg continues the last line
          if (!first && node->has_dogleg && !isnan_3BD (node->dogleg_vector)
              && !isnan (node->dogleg_length))
            path_line (cur_x + node->dogleg_vector.x * node->dogleg_length,
                       cur_y + node->dogleg_vector.y * node->dogleg_length);
          path_end ();
          if (line->num_points > 1 && ctx->arrow_size > 0.0
              && !isnan_3BD (line->points[0]) && !isnan_3BD (line->points[1]))
            {
              const double dx = line->points[1].x - line->points[0].x;
              const double dy = line->points[1].y - line->points[0].y;
              const double len = sqrt (dx * dx + dy * dy);
              if (len > 0.0)
                {
                  const double ux = dx / len, uy = dy / len;
                  const double sz = ctx->arrow_size;
                  const double bx = line->points[0].x + ux * sz;
                  const double by = line->points[0].y + uy * sz;
                  path_move (line->points[0].x, line->points[0].y);
                  path_line (bx - uy * sz / 6.0, by + ux * sz / 6.0);
                  path_line (bx + uy * sz / 6.0, by - ux * sz / 6.0);
                  path_close ();
                  path_end ();
                }
            }
        }
    }
  if (ctx->has_content_txt && ctx->content.txt.default_text
      && !isnan_3BD (ctx->content.txt.location))
    {
      char *utf8 = text_utf8 (dwg, ctx->content.txt.default_text);
      char *plain = utf8 ? mtext_plaintext (utf8) : NULL;
      free (utf8);
      if (plain)
        {
          const double h = ctx->text_height > 0.0 ? ctx->text_height
                                                  : ctx->content.txt.height;
          put_label (ctx->content.txt.location.x, ctx->content.txt.location.y,
                     h, 0.0, text_fontname (dwg, ctx->content.txt.style), 'l',
                     'x', plain);
          free (plain);
        }
    }
}

// The grid of the rows and columns, from the insertion point down and along
// the horizontal direction, and the text of its cells, without merging.
static void
output_TABLE (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_TABLE *tbl = obj->tio.entity->tio.TABLE;
  double ux = 1.0, uy = 0.0, vx, vy, width = 0.0, height = 0.0, x0, y0;
  BITCODE_BL r, c;

  if (entity_invisible (obj) || !tbl->col_widths || !tbl->row_heights
      || !tbl->num_cols || !tbl->num_rows || isnan_3BD (tbl->ins_pt))
    return;
  if (!isnan_3BD (tbl->horiz_direction)
      && (tbl->horiz_direction.x != 0.0 || tbl->horiz_direction.y != 0.0))
    {
      const double len
          = sqrt (tbl->horiz_direction.x * tbl->horiz_direction.x
                  + tbl->horiz_direction.y * tbl->horiz_direction.y);
      ux = tbl->horiz_direction.x / len;
      uy = tbl->horiz_direction.y / len;
    }
  vx = uy; // down is the horizontal direction turned clockwise
  vy = -ux;
  for (c = 0; c < tbl->num_cols; c++)
    width += tbl->col_widths[c];
  for (r = 0; r < tbl->num_rows; r++)
    height += tbl->row_heights[r];
  set_pen (obj->tio.entity);
  y0 = 0.0;
  for (r = 0; r <= tbl->num_rows; r++)
    {
      draw_line (tbl->ins_pt.x + vx * y0, tbl->ins_pt.y + vy * y0,
                 tbl->ins_pt.x + ux * width + vx * y0,
                 tbl->ins_pt.y + uy * width + vy * y0);
      if (r < tbl->num_rows)
        y0 += tbl->row_heights[r];
    }
  x0 = 0.0;
  for (c = 0; c <= tbl->num_cols; c++)
    {
      draw_line (tbl->ins_pt.x + ux * x0, tbl->ins_pt.y + uy * x0,
                 tbl->ins_pt.x + ux * x0 + vx * height,
                 tbl->ins_pt.y + uy * x0 + vy * height);
      if (c < tbl->num_cols)
        x0 += tbl->col_widths[c];
    }
  y0 = 0.0;
  for (r = 0; r < tbl->num_rows; r++)
    {
      x0 = 0.0;
      for (c = 0; c < tbl->num_cols; c++)
        {
          const unsigned long idx = (unsigned long)r * tbl->num_cols + c;
          if (tbl->cells && idx < tbl->num_cells && tbl->cells[idx].text_value)
            {
              const Dwg_TABLE_Cell *cell = &tbl->cells[idx];
              char *utf8 = text_utf8 (dwg, cell->text_value);
              char *plain = utf8 ? mtext_plaintext (utf8) : NULL;
              free (utf8);
              if (plain)
                {
                  const double margin = isnan (tbl->horiz_cell_margin)
                                            ? 0.0
                                            : tbl->horiz_cell_margin;
                  const double h = cell->text_height > 0.0
                                       ? cell->text_height
                                       : tbl->row_heights[r] * 0.4;
                  const double cx = x0 + margin,
                               cy = y0 + tbl->row_heights[r] / 2;
                  put_label (tbl->ins_pt.x + ux * cx + vx * cy,
                             tbl->ins_pt.y + uy * cx + vy * cy, h,
                             atan2 (uy, ux) * 180.0 / M_PI, "Helvetica", 'l',
                             'c', plain);
                  free (plain);
                }
            }
          x0 += tbl->col_widths[c];
        }
      y0 += tbl->row_heights[r];
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
    case DWG_TYPE_MINSERT:
      output_MINSERT (obj);
      break;
    case DWG_TYPE_ATTRIB:
      // only with their INSERT, see output_attribs
      num = 0;
      break;
    case DWG_TYPE_ATTDEF:
      output_ATTDEF (obj);
      break;
    case DWG_TYPE_TRACE:
      output_TRACE (obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
    case DWG_TYPE_DIMENSION_LINEAR:
    case DWG_TYPE_DIMENSION_ALIGNED:
    case DWG_TYPE_DIMENSION_ANG3PT:
    case DWG_TYPE_DIMENSION_ANG2LN:
    case DWG_TYPE_DIMENSION_RADIUS:
    case DWG_TYPE_DIMENSION_DIAMETER:
    case DWG_TYPE_ARC_DIMENSION:
      output_DIMENSION (obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      output_POLYLINE_3D (obj);
      break;
    case DWG_TYPE_LEADER:
      output_LEADER (obj);
      break;
    case DWG_TYPE_MLINE:
      output_MLINE (obj);
      break;
    case DWG_TYPE_TOLERANCE:
      output_TOLERANCE (obj);
      break;
    case DWG_TYPE_IMAGE:
      output_IMAGE (obj);
      break;
    case DWG_TYPE_WIPEOUT:
      output_WIPEOUT (obj);
      break;
    case DWG_TYPE_PDFUNDERLAY:
    case DWG_TYPE_DGNUNDERLAY:
    case DWG_TYPE_DWFUNDERLAY:
      output_UNDERLAY (obj);
      break;
    case DWG_TYPE_HATCH:
      output_HATCH (obj);
      break;
    case DWG_TYPE_HELIX:
      output_HELIX (obj);
      break;
    case DWG_TYPE_MULTILEADER:
      output_MULTILEADER (obj);
      break;
    case DWG_TYPE_TABLE:
      output_TABLE (obj);
      break;
    case DWG_TYPE__3DSOLID:
    case DWG_TYPE_REGION:
    case DWG_TYPE_BODY:
    case DWG_TYPE_EXTRUDEDSURFACE:
    case DWG_TYPE_LOFTEDSURFACE:
    case DWG_TYPE_NURBSURFACE:
    case DWG_TYPE_PLANESURFACE:
    case DWG_TYPE_REVOLVEDSURFACE:
    case DWG_TYPE_SWEPTSURFACE:
      output_wires (obj);
      break;
    case DWG_TYPE_MESH:
      output_MESH (obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      output_OLE2FRAME (obj);
      break;
    case DWG_TYPE__3DLINE:
      output_3DLINE (obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      output_POLYLINE_MESH (obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      output_POLYLINE_PFACE (obj);
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
    case DWG_TYPE_VIEWPORT: // only a window, the model-space is the fallback
    case DWG_TYPE_BLOCK:    // the preR13 markers, lights and unknown entities
    case DWG_TYPE_ENDBLK:
    case DWG_TYPE_REPEAT:
    case DWG_TYPE_ENDREP:
    case DWG_TYPE_LIGHT:
    case DWG_TYPE_UNKNOWN_ENT:
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

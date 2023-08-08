/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2023 Free Software Foundation, Inc.                   */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg2ps.c: create a PostScript file of lines from a DWG
 * TODO: more 2D elements, see dwg2SVG
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Reini Urban
 */

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libps/pslib.h>
#include "my_getopt.h"

#include <dwg.h>
#include <dwg_api.h>
#include "common.h"
#include "../src/bits.h" //bit_convert_TU
#include "suffix.inc"
#include "geom.h"

static int opts = 0;

static int
usage (void)
{
  printf ("\nUsage: dwg2ps [-v[0-9]] DWGFILE [PSFILE]\n");
  return 1;
}
static int
opt_version (void)
{
  printf ("dwg2ps %s\n", dwg_api_version_string ());
  return 0;
}
static int
help (void)
{
  printf ("\nUsage: dwg2ps [OPTION]... DWGFILE [PSFILE]\n");
  printf ("Converts some 2D elements of the DWG to a Postscript file.\n"
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

/* T16 is always UCS-2 (=> utf8) */
static int
set_info (PSDoc *restrict ps, Dwg_Data *restrict dwg, const char *restrict key,
          BITCODE_T16 restrict text)
{
  int ret = 0;
  char *u8;
  (void)dwg;
  if (!text)
    return 0;
  u8 = bit_convert_TU ((BITCODE_TU)text);
  if (u8 && strlen (u8))
    {
      PS_set_info (ps, key, u8);
      ret = 1;
    }
  free (u8);
  return ret;
}

// Draw the segment from prev to pt, both already OCS-transformed: an arc
// for a bulged segment, else a line.
static void
ps_segment (PSDoc *ps, BITCODE_2DPOINT prev, BITCODE_2DPOINT pt, double bulge,
            int mirrored)
{
  BITCODE_2BD ctr;
  double radius, start_angle, sweep;

  if (mirrored)
    bulge = -bulge;
  if (bulge_to_arc (prev, pt, bulge, &ctr, &radius, &start_angle, &sweep))
    {
      const double a = start_angle * 180.0 / M_PI;
      const double b = (start_angle + sweep) * 180.0 / M_PI;
      if (sweep > 0.0)
        PS_arc (ps, (float)ctr.x, (float)ctr.y, (float)radius, (float)a,
                (float)b);
      else
        PS_arcn (ps, (float)ctr.x, (float)ctr.y, (float)radius, (float)a,
                 (float)b);
    }
  else
    PS_lineto (ps, (float)pt.x, (float)pt.y);
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

// The 2D WCS point, in place, of the OCS point p of a polyline at elevation
static void
ps_ocs_point (BITCODE_3DPOINT *p, const double elevation, const BITCODE_BE ext)
{
  BITCODE_2DPOINT in, out;
  in.x = p->x;
  in.y = p->y;
  transform_OCS_elevation (&out, in, elevation, ext);
  p->x = out.x;
  p->y = out.y;
}

// The Bezier curve b of degree 1, 2 or 3 from the current point b[0].
// PostScript has only cubic curves: a quadratic one is the cubic one with
// its inner control points 2/3 of the way to its middle one.
static void
ps_bezier (PSDoc *ps, const BITCODE_3DPOINT *b, const int degree)
{
  if (degree == 1)
    PS_lineto (ps, (float)b[1].x, (float)b[1].y);
  else if (degree == 2)
    PS_curveto (ps, (float)(b[0].x + 2.0 / 3 * (b[1].x - b[0].x)),
                (float)(b[0].y + 2.0 / 3 * (b[1].y - b[0].y)),
                (float)(b[2].x + 2.0 / 3 * (b[1].x - b[2].x)),
                (float)(b[2].y + 2.0 / 3 * (b[1].y - b[2].y)), (float)b[2].x,
                (float)b[2].y);
  else
    PS_curveto (ps, (float)b[1].x, (float)b[1].y, (float)b[2].x, (float)b[2].y,
                (float)b[3].x, (float)b[3].y);
}

// A spline-fit POLYLINE_2D as its curve.  Returns 0 if it has none.
static int
ps_polyline_spline (PSDoc *ps, const Dwg_Object *obj)
{
  const Dwg_Entity_POLYLINE_2D *pline = obj->tio.entity->tio.POLYLINE_2D;
  BITCODE_3DPOINT *b;
  int error, degree, j;
  const int k = dwg_object_polyline_2d_get_spline (obj, &b, &degree, &error);

  if (!k)
    return 0;
  for (j = 0; j <= k * degree; j++)
    ps_ocs_point (&b[j], pline->elevation, pline->extrusion);
  PS_moveto (ps, (float)b[0].x, (float)b[0].y);
  for (j = 0; j < k; j++)
    ps_bezier (ps, &b[j * degree], degree);
  PS_stroke (ps);
  free (b);
  return 1;
}

// Draw the numpts OCS points of a polyline at elevation, with optional
// bulges, as one path.  A stroke ends the path, so stroke only once at the
// end.
static void
ps_polyline (PSDoc *ps, const dwg_point_2d *pts, const double *bulges,
             const BITCODE_RL numpts, const double elevation,
             const BITCODE_BE extrusion, const int closed)
{
  BITCODE_2DPOINT pt0, prev, ptin;
  BITCODE_RL j;
  const int mirrored = transform_OCS_2d_is_mirrored (extrusion);

  ptin.x = pts[0].x;
  ptin.y = pts[0].y;
  transform_OCS_elevation (&pt0, ptin, elevation, extrusion);
  prev = pt0;
  PS_moveto (ps, (float)pt0.x, (float)pt0.y);
  for (j = 1; j < numpts; j++)
    {
      BITCODE_2DPOINT pt;
      ptin.x = pts[j].x;
      ptin.y = pts[j].y;
      transform_OCS_elevation (&pt, ptin, elevation, extrusion);
      ps_segment (ps, prev, pt, bulges ? bulges[j - 1] : 0.0, mirrored);
      prev = pt;
    }
  if (closed)
    ps_segment (ps, prev, pt0, bulges ? bulges[numpts - 1] : 0.0, mirrored);
  PS_stroke (ps);
}

// The angle in degrees of the point of an ARC at angle, from its OCS to WCS
static double
ps_arc_angle (const Dwg_Entity_ARC *arc, const BITCODE_3DPOINT center,
              const double angle)
{
  BITCODE_3DPOINT p, w;
  p.x = arc->center.x + arc->radius * cos (angle);
  p.y = arc->center.y + arc->radius * sin (angle);
  p.z = arc->center.z;
  transform_OCS (&w, p, arc->extrusion);
  return atan2 (w.y - center.y, w.x - center.x) * 180.0 / M_PI;
}

// Draw an ARC.  PS_arc takes degrees and turns counterclockwise, so a
// mirrored OCS, e.g. the extrusion (0,0,-1), swaps the ends.
static void
ps_arc (PSDoc *ps, const Dwg_Entity_ARC *arc)
{
  BITCODE_3DPOINT center;
  double a, b;
  transform_OCS (&center, arc->center, arc->extrusion);
  a = ps_arc_angle (arc, center, arc->start_angle);
  b = ps_arc_angle (arc, center, arc->end_angle);
  if (transform_OCS_2d_is_mirrored (arc->extrusion))
    {
      const double t = a;
      a = b;
      b = t;
    }
  PS_arc (ps, (float)center.x, (float)center.y, (float)arc->radius, (float)a,
          (float)b);
  PS_stroke (ps);
}

static void
create_postscript (Dwg_Data *dwg, char *output)
{
  double dx;
  double dy;
  double scale_x;
  double scale_y;
  double scale;
  BITCODE_BL i;
  // FILE *fh;
  PSDoc *ps;

  /* Initialization
   */
  PS_boot ();
  ps = PS_new ();
  if (PS_open_file (ps, output) < 0)
    {
      puts ("Cannot write PostScript file");
      return;
    }

  {
    char creator[80];
    snprintf (creator, sizeof (creator), "dwg2ps %s",
              dwg_api_version_string ());
    PS_set_info (ps, "Creator", creator);
  }
  (void)set_info (ps, dwg, "Subject", dwg->summaryinfo.SUBJECT);
  (void)set_info (ps, dwg, "Author", dwg->summaryinfo.LASTSAVEDBY);
  if (set_info (ps, dwg, "Title", dwg->summaryinfo.TITLE))
    (void)set_info (ps, dwg, "Keywords", dwg->summaryinfo.KEYWORDS);
  else
    {
      PS_set_info (ps, "Title", output);
      PS_set_info (ps, "Keywords", "dwg, postscript, conversion, CAD, plot");
    }

  /* First page: Model Space (?)
   */
  dx = (dwg_model_x_max (dwg) - dwg_model_x_min (dwg));
  dy = (dwg_model_y_max (dwg) - dwg_model_y_min (dwg));
  scale_x = dx / (dwg_model_x_max (dwg) - dwg_model_x_min (dwg));
  scale_y = dy / (dwg_model_y_max (dwg) - dwg_model_y_min (dwg));
  scale = 25.4 / 72; // pt:mm
  PS_begin_page (ps, dx / scale, dy / scale);
  scale *= (scale_x > scale_y ? scale_x : scale_y);
  PS_scale (ps, (float)scale, (float)scale);
  PS_translate (ps, (float)-dwg_model_x_min (dwg),
                (float)-dwg_model_y_min (dwg));
  if (dwg->opts & DWG_OPTS_LOGLEVEL)
    {
      fprintf (stderr, "Limits: %f, %f\n", dx, dy);
      fprintf (stderr, "Scale: %f (%f, %f)\n", scale, scale_x, scale_y);
    }

    /* Mark the origin with a crossed circle
     */
#define H 1
  PS_circle (ps, 0, 0, H);
  PS_moveto (ps, 0, H);
  PS_lineto (ps, 0, -H);
  PS_moveto (ps, -H, 0);
  PS_lineto (ps, H, 0);
  PS_stroke (ps);

  /* Iterate all entities
   */
  for (i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->supertype == DWG_SUPERTYPE_OBJECT) // no entity
        continue;
      // if (obj->tio.entity->entity_mode == 0) // belongs to block
      //  continue;
      if (obj->fixedtype == DWG_TYPE_LINE)
        {
          Dwg_Entity_LINE *line = obj->tio.entity->tio.LINE;

          // the points of a LINE are WCS
          PS_moveto (ps, (float)line->start.x, (float)line->start.y);
          PS_lineto (ps, (float)line->end.x, (float)line->end.y);
          PS_stroke (ps);
        }
      else if (obj->fixedtype == DWG_TYPE_POLYLINE_2D)
        {
          int error;
          BITCODE_RL numpts
              = dwg_object_polyline_2d_get_numpoints (obj, &error);
          dwg_point_2d *pts = dwg_object_polyline_2d_get_points (obj, &error);
          Dwg_Entity_POLYLINE_2D *pline = obj->tio.entity->tio.POLYLINE_2D;
          if (numpts && pts && !error && !ps_polyline_spline (ps, obj))
            {
              double *bulges = dwg_object_polyline_2d_get_bulges (obj, &error);
              ps_polyline (ps, pts, bulges, numpts, pline->elevation,
                           pline->extrusion, pline->flag & 1);
              free (bulges);
            }
          free (pts);
        }
      else if (obj->fixedtype == DWG_TYPE_LWPOLYLINE)
        {
          int error;
          Dwg_Entity_LWPOLYLINE *pline = obj->tio.entity->tio.LWPOLYLINE;
          BITCODE_RL numpts = dwg_ent_lwpline_get_numpoints (pline, &error);
          if (numpts && !error)
            {
              dwg_point_2d *pts = dwg_ent_lwpline_get_points (pline, &error);
              if (pts && !error)
                ps_polyline (
                    ps, pts,
                    pline->num_bulges == numpts ? pline->bulges : NULL, numpts,
                    pline->elevation, pline->extrusion, pline->flag & 512);
              free (pts);
            }
        }
      else if (obj->fixedtype == DWG_TYPE_ARC)
        {
          ps_arc (ps, obj->tio.entity->tio.ARC);
        }
      else if (obj->fixedtype == DWG_TYPE_CIRCLE)
        {
          Dwg_Entity_CIRCLE *circle = obj->tio.entity->tio.CIRCLE;
          BITCODE_3DPOINT center;
          transform_OCS (&center, circle->center, circle->extrusion);
          PS_circle (ps, (float)center.x, (float)center.y,
                     (float)circle->radius);
          PS_stroke (ps);
        }
    }

  /* End Model Space */
  PS_end_page (ps);

  PS_close (ps);
  PS_delete (ps);
  PS_shutdown ();
}

int
main (int argc, char *argv[])
{
  int error;
  char *outfile;
  int i = 1;
  Dwg_Data dwg;

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

  memset (&dwg, 0, sizeof (Dwg_Data));
  dwg.opts = opts;
  error = dwg_read_file (argv[i], &dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fprintf (stderr, "READ ERROR 0x%x\n", error);
      dwg_free (&dwg);
      return 1;
    }

  outfile = suffix (argv[i], "ps");
  create_postscript (&dwg, outfile);

  printf ("Success! See the file '%s'\n", outfile);
  free (outfile);
  dwg_free (&dwg);
  return 0;
}

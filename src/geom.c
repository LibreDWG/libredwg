/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019,2022-2026 Free Software Foundation, Inc.              */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * geom.c: geometric projections from OCS
 * Note: There are certainly bugs lurking here. Not thoroughly tested yet.
 * For the properly exported variants see dwg_api instead, with the
 * dwg_geom_ prefix.
 * written by Reini Urban
 * v4_transform_3d, bulge_to_arc, bulge_split by Garcia Leal
 */

#include "config.h"
#include <stdlib.h>
#include <string.h>
// #include <stdio.h>
#include <math.h>
#include <dwg.h>
#include "common.h"
#include "geom.h"

static void
normalize (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt)
{
  double l = sqrt ((pt.x * pt.x) + (pt.y * pt.y) + (pt.z * pt.z));
  *out = pt;
  if (l != 1.0 && l != 0.0)
    {
      out->x = pt.x / l;
      out->y = pt.y / l;
      out->z = pt.z / l;
    }
}

static void
cross (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt1, BITCODE_3DPOINT pt2)
{
  out->x = pt1.y * pt2.z - pt1.z * pt2.y;
  out->y = pt1.z * pt2.x - pt1.x * pt2.z;
  out->z = pt1.x * pt2.y - pt1.y * pt2.x;
}

// transform a 2D point via its OCS (extrusion or normal) to 2D
void
transform_OCS_2d (BITCODE_2DPOINT *out, BITCODE_2DPOINT pt, BITCODE_BE ext)
{
  // [0,0,0] with preR13
  if (ext.x == 0.0 && ext.y == 0.0 && (ext.z == 1.0 || ext.z == 0.0))
    {
      *out = pt;
    }
  else if (ext.x == 0.0 && ext.y == 0.0 && ext.z == -1.0)
    {
      *out = pt;
      out->x = -out->x;
    }
  else
    {
      /* This is called the "Arbitrary Axis Algorithm" to calculate
         the OCS x-axis from the extrusion z-vector (the "normal") */
      BITCODE_3DPOINT ax, ay, az, be;
      memcpy (&be, &ext, sizeof (BITCODE_3DPOINT));
      normalize (&az, be);
      if ((fabs (az.x) < 1 / 64.0) && (fabs (az.y) < 1 / 64.0))
        {
          BITCODE_3DPOINT tmp = { 0.0, 1.0, 0.0 };
          cross (&tmp, tmp, az);
          normalize (&ax, tmp);
        }
      else
        {
          BITCODE_3DPOINT tmp = { 0.0, 0.0, 1.0 };
          cross (&tmp, tmp, az);
          normalize (&ax, tmp);
        }
      cross (&ay, az, ax);
      normalize (&ay, ay);
      // OCS to WCS: the point in the OCS axes ax, ay
      out->x = pt.x * ax.x + pt.y * ay.x;
      out->y = pt.x * ax.y + pt.y * ay.y;
    }
  return;
}

// transform a 3D point via its OCS (extrusion or normal) to 3D
void
transform_OCS (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt, BITCODE_BE ext)
{
  // [0,0,0] with preR13, as in transform_OCS_2d
  if (ext.x == 0.0 && ext.y == 0.0 && (ext.z == 1.0 || ext.z == 0.0))
    {
      *out = pt;
    }
  else if (ext.x == 0.0 && ext.y == 0.0 && ext.z == -1.0)
    {
      // the OCS axes are (-1,0,0), (0,1,0), (0,0,-1)
      *out = pt;
      out->x = -out->x;
      out->z = -out->z;
    }
  else
    {
      /* This is called the "Arbitrary Axis Algorithm" to calculate
         the OCS x-axis from the extrusion z-vector */
      BITCODE_3DPOINT ax, ay, az, be;
      memcpy (&be, &ext, sizeof (BITCODE_3DPOINT));
      normalize (&az, be);
      if ((fabs (az.x) < 1 / 64.0) && (fabs (az.y) < 1 / 64.0))
        {
          BITCODE_3DPOINT tmp = { 0.0, 1.0, 0.0 };
          cross (&tmp, tmp, az);
          normalize (&ax, tmp);
        }
      else
        {
          BITCODE_3DPOINT tmp = { 0.0, 0.0, 1.0 };
          cross (&tmp, tmp, az);
          normalize (&ax, tmp);
        }
      cross (&ay, az, ax);
      normalize (&ay, ay);
      // OCS to WCS: the point in the OCS axes ax, ay, az
      out->x = pt.x * ax.x + pt.y * ay.x + pt.z * az.x;
      out->y = pt.x * ax.y + pt.y * ay.y + pt.z * az.y;
      out->z = pt.x * ax.z + pt.y * ay.z + pt.z * az.z;
    }
  return;
}

// True if transform_OCS_2d maps the OCS of ext to the 2D plane with a
// reflection, e.g. for the extrusion (0,0,-1).  That reverses the sense
// of arcs and bulges.
int
transform_OCS_2d_is_mirrored (BITCODE_BE ext)
{
  BITCODE_2DPOINT ex = { 1.0, 0.0 }, ey = { 0.0, 1.0 }, tx, ty;
  transform_OCS_2d (&tx, ex, ext);
  transform_OCS_2d (&ty, ey, ext);
  return tx.x * ty.y - tx.y * ty.x < 0.0;
}

// endpoint of the angular vector from ctr, to angle (radian), for radius len
void
angle_vector_2d (BITCODE_2BD *out, BITCODE_2BD ctr, BITCODE_BD angle,
                 BITCODE_BD len)
{
  out->x = ctr.x + (len * cos (angle));
  out->y = ctr.y + (len * sin (angle));
}
// Normalize an angle to [0, 2*PI).  Handles inf, nan and denormals.
double
angle_normalize (double angle)
{
  if (!isfinite (angle))
    return 0.0;
  angle = fmod (angle, 2 * M_PI);
  if (angle < 0)
    angle += 2 * M_PI;
  return angle;
}


// Segmentation of arc,curves into plines for geojson.
void
arc_split (BITCODE_2BD *pts, const int num_pts, const BITCODE_2BD ctr,
           BITCODE_BD start_angle, BITCODE_BD end_angle, const BITCODE_BD len)
{
  double ang, angd;
#ifndef HAVE_NONNULL
  if (!pts)
    return;
#endif
  // A span of a full turn or more (e.g. a CIRCLE as 0 .. 2*PI) is a full
  // circle; normalizing both ends first would collapse it to zero.
  if (isfinite (start_angle) && isfinite (end_angle)
      && fabs (end_angle - start_angle) >= 2 * M_PI)
    {
      start_angle = angle_normalize (start_angle);
      end_angle = start_angle + 2 * M_PI;
    }
  else
    {
      start_angle = angle_normalize (start_angle);
      end_angle = angle_normalize (end_angle);
      if (end_angle < start_angle)
        end_angle += 2 * M_PI;
    }
  // shoot vectors from ctr to ang
  ang = start_angle;
  angd = (end_angle - start_angle) / num_pts;
  // fprintf (stderr, "ctr (%g,%g) ang: %g - %g\n", ctr.x, ctr.y, ang,
  // end_angle);
  for (int i = 0; i < num_pts; i++, ang += angd)
    {
      BITCODE_2BD pt;
      angle_vector_2d (&pt, ctr, ang, len);
      // fprintf (stderr, "ang[%d] %g\n", i, ang);
      pts[i].x = pt.x;
      pts[i].y = pt.y;
    }
}

/* The arc of the polyline segment p1 -> p2 with the given bulge, the
   tangent of a quarter of its included angle.  A positive bulge runs
   counter-clockwise, a bulge of 1 is a semicircle.
   Returns 0 for a straight segment (zero or non-finite bulge, coincident
   points).  Else returns 1 and sets the center, the radius, the angle of
   p1 seen from the center and the signed sweep angle (4 * atan (bulge)),
   so the arc ends at start_angle + sweep. */
int
bulge_to_arc (const BITCODE_2BD p1, const BITCODE_2BD p2, const double bulge,
              BITCODE_2BD *center, double *radius, double *start_angle,
              double *sweep)
{
  const double dx = p2.x - p1.x;
  const double dy = p2.y - p1.y;
  const double chord = hypot (dx, dy);
  double h;

  if (!isfinite (bulge) || fabs (bulge) < 1e-12 || !isfinite (chord)
      || chord < 1e-12)
    return 0;
  // The center lies on the left normal (-dy, dx) / chord of the chord
  // midpoint, at the distance chord * (1 - bulge^2) / (4 * bulge).
  h = (1.0 - bulge * bulge) / (4.0 * bulge);
  center->x = (p1.x + p2.x) / 2.0 - h * dy;
  center->y = (p1.y + p2.y) / 2.0 + h * dx;
  *radius = chord * (1.0 + bulge * bulge) / (4.0 * fabs (bulge));
  *start_angle = atan2 (p1.y - center->y, p1.x - center->x);
  *sweep = 4.0 * atan (bulge);
  return 1;
}

/* Split the polyline segment p1 -> p2 with the given bulge into num_pts
   points, from p1 up to but excluding p2, as arc_split does, so that
   consecutive segments can be appended.  Returns the number of points
   written: num_pts for an arc, 1 (just p1) for a straight segment, and 0
   if num_pts < 1. */
int
bulge_split (BITCODE_2BD *pts, const int num_pts, const BITCODE_2BD p1,
             const BITCODE_2BD p2, const double bulge)
{
  BITCODE_2BD ctr;
  double radius, start_angle, sweep;
  int i;
#ifndef HAVE_NONNULL
  if (!pts)
    return 0;
#endif
  if (num_pts < 1)
    return 0;
  pts[0] = p1;
  if (num_pts == 1
      || !bulge_to_arc (p1, p2, bulge, &ctr, &radius, &start_angle, &sweep))
    return 1;
  for (i = 1; i < num_pts; i++)
    angle_vector_2d (&pts[i], ctr, start_angle + (sweep * i) / num_pts,
                     radius);
  return num_pts;
}

/*----------------------------------------------------------------------
 * V4 Residue Topology Spatial Transformations (Klein Four-Group V4)
 *
 * MATHEMATICAL NOTICE
 * The V4 map is invertible but NOT conformal. Its linear part P has
 * det(P) = 4 and singular values 2, 2, 1, so a right angle becomes 70.53
 * degrees, a circle becomes an ellipse, and arc sweep angles are not
 * preserved. P is NOT an involution (P*P = [[3,1,1],[1,3,-1],[1,-1,3]]),
 * so its inverse is NOT P/4. The "H*H = 4I" relation belongs to the
 * 4-point Hadamard butterfly network, not to this spatial map.
 *
 * Entities inside a block definition are left alone: only the INSERT that
 * references them is transformed, otherwise the block content would get
 * the map twice.
 *----------------------------------------------------------------------*/

static void
v4_transform_3d (double *x, double *y, double *z)
{
  double s0, d0, s1, d1;

  if (!x || !y || !z)
    return;

  s0 = *x + *y;
  d0 = *x - *y;
  s1 = *z + 1.0;
  d1 = *z - 1.0;

  *x = s0 + s1;
  *y = d0 + d1;
  *z = s0 - s1;
}

static void
v4_transform_2d (double *x, double *y)
{
  double s0, d0;

  if (!x || !y)
    return;

  s0 = *x + *y;
  d0 = *x - *y;

  *x = s0 + 1.0;
  *y = d0 - 1.0;
}

/* Entities owned by a block definition are skipped: only the INSERT that
 * references them is transformed.
 *
 * Model space and paper space are themselves BLOCK_HEADERs, so the test is
 * not "is the owner a block header" but "is the owner the mspace or pspace
 * block". Compare by handle: the refs come from different places
 * (header_vars vs block_control) and need not be the same object.
 *
 * Subentities (ATTRIB, VERTEX_*) are owned by a parent entity rather than
 * by a space, so they are skipped too: their parent already moves them. */
static int
v4_is_block_member (const Dwg_Object *obj, Dwg_Data *dwg)
{
  Dwg_Object_Ref *ref;
  Dwg_Object_Ref *space;
  Dwg_Object *owner;

  if (dwg_obj_is_subentity (obj))
    return 1;

  if (!obj->tio.entity || !obj->tio.entity->ownerhandle)
    return 0; /* preR13 mspace: no owner handle */

  ref = obj->tio.entity->ownerhandle;
  owner = ref->obj;
  if (!owner || owner->fixedtype != DWG_TYPE_BLOCK_HEADER)
    return 0;

  space = dwg_model_space_ref (dwg);
  if (space && space->absolute_ref == ref->absolute_ref)
    return 0;

  space = dwg_paper_space_ref (dwg);
  if (space && space->absolute_ref == ref->absolute_ref)
    return 0;

  /* Any other block header is a block definition. */
  return 1;
}

EXPORT void
dwg_geom_v4_transform_2d (double *x, double *y)
{
  v4_transform_2d (x, y);
}

EXPORT uint32_t
dwg_transform_v4 (Dwg_Data *dwg)
{
  uint32_t count = 0;
  uint32_t i;

  if (!dwg || !dwg->object)
    return 0;

  for (i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];

      /* supertype first: tio is a union, and reading it on a non-entity
       * object yields garbage. */
      if (obj->supertype != DWG_SUPERTYPE_ENTITY || !obj->tio.entity)
        continue;

      if (v4_is_block_member (obj, dwg))
        continue;

      /* obj->fixedtype, not obj->type: from R2000 onwards obj->type holds
       * a variable class index. */
      switch (obj->fixedtype)
        {
        case DWG_TYPE_POINT:
          {
            Dwg_Entity_POINT *e = obj->tio.entity->tio.POINT;
            if (!e)
              break;
            v4_transform_3d (&e->x, &e->y, &e->z);
            count++;
            break;
          }
        case DWG_TYPE_LINE:
          {
            Dwg_Entity_LINE *e = obj->tio.entity->tio.LINE;
            if (!e)
              break;
            v4_transform_3d (&e->start.x, &e->start.y, &e->start.z);
            v4_transform_3d (&e->end.x, &e->end.y, &e->end.z);
            count++;
            break;
          }
        case DWG_TYPE_CIRCLE:
          {
            Dwg_Entity_CIRCLE *e = obj->tio.entity->tio.CIRCLE;
            if (!e)
              break;
            /* The centre maps cleanly. The radius cannot: the singular
             * values in the entity plane are 2.00 and 1.41, so the image is
             * an ellipse and no single scale factor is correct. */
            v4_transform_3d (&e->center.x, &e->center.y, &e->center.z);
            count++;
            break;
          }
        case DWG_TYPE_ARC:
          {
            Dwg_Entity_ARC *e = obj->tio.entity->tio.ARC;
            if (!e)
              break;
            v4_transform_3d (&e->center.x, &e->center.y, &e->center.z);
            count++;
            break;
          }
        case DWG_TYPE_ELLIPSE:
          {
            Dwg_Entity_ELLIPSE *e = obj->tio.entity->tio.ELLIPSE;
            if (!e)
              break;
            v4_transform_3d (&e->center.x, &e->center.y, &e->center.z);
            count++;
            break;
          }
        case DWG_TYPE__3DFACE:
          {
            Dwg_Entity__3DFACE *e = obj->tio.entity->tio._3DFACE;
            if (!e)
              break;
            v4_transform_3d (&e->corner1.x, &e->corner1.y, &e->corner1.z);
            v4_transform_3d (&e->corner2.x, &e->corner2.y, &e->corner2.z);
            v4_transform_3d (&e->corner3.x, &e->corner3.y, &e->corner3.z);
            v4_transform_3d (&e->corner4.x, &e->corner4.y, &e->corner4.z);
            count++;
            break;
          }
        case DWG_TYPE_SOLID:
          {
            Dwg_Entity_SOLID *e = obj->tio.entity->tio.SOLID;
            if (!e)
              break;
            /* SOLID stores ONE global elevation for all four corners. Each
             * corner is mapped in the local plane; the shared elevation is
             * mapped exactly once, not four times. */
            v4_transform_2d (&e->corner1.x, &e->corner1.y);
            v4_transform_2d (&e->corner2.x, &e->corner2.y);
            v4_transform_2d (&e->corner3.x, &e->corner3.y);
            v4_transform_2d (&e->corner4.x, &e->corner4.y);
            e->elevation = -e->elevation - 1.0;
            count++;
            break;
          }
        case DWG_TYPE_TRACE:
          {
            Dwg_Entity_TRACE *e = obj->tio.entity->tio.TRACE;
            if (!e)
              break;
            v4_transform_2d (&e->corner1.x, &e->corner1.y);
            v4_transform_2d (&e->corner2.x, &e->corner2.y);
            v4_transform_2d (&e->corner3.x, &e->corner3.y);
            v4_transform_2d (&e->corner4.x, &e->corner4.y);
            e->elevation = -e->elevation - 1.0;
            count++;
            break;
          }
        case DWG_TYPE_LWPOLYLINE:
          {
            Dwg_Entity_LWPOLYLINE *e = obj->tio.entity->tio.LWPOLYLINE;
            uint32_t j;

            if (!e || !e->points)
              break;

            /* Vertices live in the local plane. The global elevation is mapped
             * once via the anchor (0, 0, e), as for TEXT, SOLID and TRACE. */
            for (j = 0; j < e->num_points; j++)
              v4_transform_2d (&e->points[j].x, &e->points[j].y);
            e->elevation = -e->elevation - 1.0;
            count++;
            break;
          }
        case DWG_TYPE_SPLINE:
          {
            Dwg_Entity_SPLINE *e = obj->tio.entity->tio.SPLINE;
            uint32_t j;

            if (!e)
              break;

            if (e->ctrl_pts)
              for (j = 0; j < e->num_ctrl_pts; j++)
                v4_transform_3d (&e->ctrl_pts[j].x, &e->ctrl_pts[j].y,
                                 &e->ctrl_pts[j].z);

            if (e->fit_pts)
              for (j = 0; j < e->num_fit_pts; j++)
                v4_transform_3d (&e->fit_pts[j].x, &e->fit_pts[j].y,
                                 &e->fit_pts[j].z);
            count++;
            break;
          }
        case DWG_TYPE_INSERT:
        case DWG_TYPE_ACIDBLOCKREFERENCE:
          {
            Dwg_Entity_INSERT *e = obj->tio.entity->tio.INSERT;
            if (!e)
              break;
            v4_transform_3d (&e->ins_pt.x, &e->ins_pt.y, &e->ins_pt.z);
            count++;
            break;
          }
        case DWG_TYPE_TEXT:
          {
            Dwg_Entity_TEXT *e = obj->tio.entity->tio.TEXT;
            if (!e)
              break;
            /* ins_pt is 2D: the z lives in the separate elevation field.
             * Map the pair in the local plane and the global elevation
             * once, as with LWPOLYLINE. */
            v4_transform_2d (&e->ins_pt.x, &e->ins_pt.y);
            e->elevation = -e->elevation - 1.0;

            /* Optional second point (DXF 11). Same local plane: without it
             * an aligned text would move to a different place than its
             * insertion point. */
            if (e->dataflags & 2)
              v4_transform_2d (&e->alignment_pt.x, &e->alignment_pt.y);

            count++;
            break;
          }

        case DWG_TYPE_MTEXT:
          {
            Dwg_Entity_MTEXT *e = obj->tio.entity->tio.MTEXT;
            if (!e)
              break;
            v4_transform_3d (&e->ins_pt.x, &e->ins_pt.y, &e->ins_pt.z);
            count++;
            break;
          }
        default:
          break;
        }
    }

  return count;
}

// finite and non-decreasing knots
static int
spline_knots_valid (const double *knots, const BITCODE_BL num_knots)
{
  BITCODE_BL i;
  for (i = 0; i < num_knots; i++)
    if (!isfinite (knots[i]) || (i > 0 && knots[i] < knots[i - 1]))
      return 0;
  return 1;
}

// finite control points, and positive weights if rational
static int
spline_ctrl_pts_valid (const Dwg_Entity_SPLINE *spline)
{
  BITCODE_BL i;
  for (i = 0; i < spline->num_ctrl_pts; i++)
    {
      const Dwg_SPLINE_control_point *c = &spline->ctrl_pts[i];
      if (!isfinite (c->x) || !isfinite (c->y) || !isfinite (c->z))
        return 0;
      if (spline->rational && !(c->w > 0.0 && isfinite (c->w)))
        return 0;
    }
  return 1;
}

/* True if the SPLINE has usable control points and knots: degree 1 to
   SPLINE_MAX_DEGREE, num_knots = num_ctrl_pts + degree + 1, finite
   non-decreasing knots with a non-empty parameter range, finite control
   points, and positive weights if rational.  SPLINEs with only fit points
   are not usable. */
int
spline_is_valid (const Dwg_Entity_SPLINE *spline)
{
  BITCODE_BL p, n;

  if (!spline || !spline->ctrl_pts || !spline->knots)
    return 0;
  p = spline->degree;
  n = spline->num_ctrl_pts;
  if (p < 1 || p > SPLINE_MAX_DEGREE || n <= p
      || spline->num_knots != n + p + 1)
    return 0;
  return spline_knots_valid (spline->knots, spline->num_knots)
         && spline_ctrl_pts_valid (spline)
         && spline->knots[n] > spline->knots[p];
}

/* De Boor: the point at parameter t of a SPLINE with control points.
   Rational splines use the weights, else all weights are 1 (the DWG
   stores w = 0 then).  t is clamped to the valid range [u_p, u_n].
   Returns 0 if the control points or knots are invalid. */
int
spline_eval (BITCODE_3DPOINT *out, const Dwg_Entity_SPLINE *spline, double t)
{
  double d[SPLINE_MAX_DEGREE + 1][4]; // x*w, y*w, z*w, w
  BITCODE_BL p, n, k, i, r;
  const double *u;

  if (!spline_is_valid (spline))
    return 0;
  p = spline->degree;
  n = spline->num_ctrl_pts;
  u = spline->knots;
  if (t < u[p])
    t = u[p];
  if (t > u[n])
    t = u[n];
  // the knot span: the largest k in [p, n-1] with u[k] <= t
  k = n - 1;
  while (k > p && u[k] > t)
    k--;
  // the p+1 control points of that span, in homogeneous coordinates
  for (i = 0; i <= p; i++)
    {
      const Dwg_SPLINE_control_point *c = &spline->ctrl_pts[k - p + i];
      const double w = spline->rational ? c->w : 1.0;
      d[i][0] = c->x * w;
      d[i][1] = c->y * w;
      d[i][2] = c->z * w;
      d[i][3] = w;
    }
  // p rounds of interpolation, each fraction a given by the knots
  for (r = 1; r <= p; r++)
    for (i = p; i >= r; i--)
      {
        const BITCODE_BL j = k - p + i;
        const double den = u[j + p + 1 - r] - u[j];
        const double a = den > 0.0 ? (t - u[j]) / den : 0.0;
        int c;
        for (c = 0; c < 4; c++)
          d[i][c] = (1.0 - a) * d[i - 1][c] + a * d[i][c];
      }
  if (d[p][3] == 0.0)
    return 0;
  out->x = d[p][0] / d[p][3];
  out->y = d[p][1] / d[p][3];
  out->z = d[p][2] / d[p][3];
  return 1;
}

// True if the first and the last degree+1 knots are equal, so that the
// curve starts at the first and ends at the last control point.
static int
spline_is_clamped (const Dwg_Entity_SPLINE *spline)
{
  const BITCODE_BL p = spline->degree, n = spline->num_ctrl_pts;
  const double *u = spline->knots;
  BITCODE_BL i;
  for (i = 0; i < p; i++)
    if (u[i] != u[p] || u[n + 1 + i] != u[n])
      return 0;
  return 1;
}

/* Boehm's algorithm: insert the knot t once into the B-spline of degree p
   with *n control points P and *n + p + 1 knots U, both with room for one
   more.  The curve does not change. */
static void
spline_insert_knot (BITCODE_3DPOINT *P, BITCODE_BL *n, double *U,
                    const BITCODE_BL p, const double t)
{
  BITCODE_BL k = *n - 1, i;
  // the knot span: the largest k in [p, n-1] with U[k] <= t
  while (k > p && U[k] > t)
    k--;
  // the points after k move one place up
  for (i = *n; i > k; i--)
    P[i] = P[i - 1];
  // p new points on the old control polygon, from the end so that
  // P[i-1] is still the old point
  for (i = k; i > k - p; i--)
    {
      const double a = (t - U[i]) / (U[i + p] - U[i]);
      P[i].x = P[i - 1].x + a * (P[i].x - P[i - 1].x);
      P[i].y = P[i - 1].y + a * (P[i].y - P[i - 1].y);
      P[i].z = P[i - 1].z + a * (P[i].z - P[i - 1].z);
    }
  for (i = *n + p + 1; i > k + 1; i--)
    U[i] = U[i - 1];
  U[k + 1] = t;
  (*n)++;
}

/* The SPLINE as a chain of Bezier curves of its degree, by inserting every
   interior knot until it is repeated degree times.  Returns the number of
   Bezier curves k and sets *pts to a malloc'ed array of k * degree + 1
   points: curve j has the points j * degree .. (j + 1) * degree, each curve
   starting at the end of the one before.  Only for valid, clamped and
   non-rational SPLINEs, else returns 0. */
int
spline_to_bezier (const Dwg_Entity_SPLINE *spline, BITCODE_3DPOINT **pts)
{
  BITCODE_BL p, n, i, cap;
  BITCODE_3DPOINT *P;
  double *U;

  *pts = NULL;
  if (!spline_is_valid (spline) || spline->rational
      || !spline_is_clamped (spline))
    return 0;
  p = spline->degree;
  n = spline->num_ctrl_pts;
  // each of the n - p - 1 interior knots needs at most p - 1 insertions
  cap = n + (n - p - 1) * (p - 1);
  P = (BITCODE_3DPOINT *)malloc (cap * sizeof (BITCODE_3DPOINT));
  U = (double *)malloc ((cap + p + 1) * sizeof (double));
  if (!P || !U)
    {
      free (P);
      free (U);
      return 0;
    }
  for (i = 0; i < n; i++)
    {
      P[i].x = spline->ctrl_pts[i].x;
      P[i].y = spline->ctrl_pts[i].y;
      P[i].z = spline->ctrl_pts[i].z;
    }
  memcpy (U, spline->knots, spline->num_knots * sizeof (double));
  // the interior knots are U[p+1] .. U[n-1], n growing with the insertions
  for (i = p + 1; i < n;)
    {
      const double t = U[i];
      BITCODE_BL s = 1;
      while (i + s < n && U[i + s] == t)
        s++;
      if (s > p) // a break in the curve: not one chain of Bezier curves
        {
          free (P);
          free (U);
          return 0;
        }
      for (; s < p; s++)
        spline_insert_knot (P, &n, U, p, t);
      i += p;
    }
  free (U);
  *pts = P;
  return (int)((n - 1) / p);
}

// One row of a tridiagonal system: a * x[r-1] + b * x[r] + c * x[r+1] = d
typedef struct _fit_row
{
  double a, b, c;
  BITCODE_3DPOINT d;
} fit_row;

// out = s * p + q
static void
point_axpy (BITCODE_3DPOINT *out, const double s, const BITCODE_3DPOINT *p,
            const BITCODE_3DPOINT *q)
{
  out->x = s * p->x + q->x;
  out->y = s * p->y + q->y;
  out->z = s * p->z + q->z;
}

// out = p / s
static void
point_div (BITCODE_3DPOINT *out, const BITCODE_3DPOINT *p, const double s)
{
  out->x = p->x / s;
  out->y = p->y / s;
  out->z = p->z / s;
}

// The unit vector of tan.  Returns 0 for a zero or non-finite vector,
// which in a DWG means: no tangent given.
static int
unit_tangent (BITCODE_3DPOINT *out, const BITCODE_3BD tan)
{
  const double len = sqrt (tan.x * tan.x + tan.y * tan.y + tan.z * tan.z);
  if (!isfinite (len) || len < 1e-12)
    return 0;
  out->x = tan.x / len;
  out->y = tan.y / len;
  out->z = tan.z / len;
  return 1;
}

// The chord length parameter t[k] of fit point k, clamped to 0 .. m
static double
fit_t (const double *t, const BITCODE_BL m, const long k)
{
  return t[k < 0 ? 0 : (k > (long)m ? (long)m : k)];
}

/* The rows of the system for the m - 1 interior control points and the
   2 next to the ends, x[r] = P[r+1], r = 0..m.  Interior row k: the curve
   passes through fit point k, C(t[k]) = Q[k].  End rows: the unit tangent
   if given, P1 = P0 + h0 / 3 * T0, else the natural end condition,
   C''(0) = 0.  The same at the end. */
static void
fit_rows (fit_row *row, const double *t, const BITCODE_BL m,
          const Dwg_Entity_SPLINE *spline)
{
  const BITCODE_3DPOINT *Q = spline->fit_pts;
  const double h0 = t[1], hm = t[m] - t[m - 1];
  BITCODE_3DPOINT tan;
  long k;

  memset (row, 0, (m + 1) * sizeof (fit_row));
  if (unit_tangent (&tan, spline->beg_tan_vec))
    {
      row[0].b = 1.0;
      point_axpy (&row[0].d, h0 / 3.0, &tan, &Q[0]);
    }
  else
    {
      row[0].b = 1.0 / h0 + 1.0 / fit_t (t, m, 2);
      row[0].c = -1.0 / fit_t (t, m, 2);
      point_div (&row[0].d, &Q[0], h0);
    }
  for (k = 1; k < (long)m; k++)
    {
      const double tp = fit_t (t, m, k + 1), tm = fit_t (t, m, k - 1);
      const double alpha = (tp - t[k]) * (tp - t[k])
                           / ((tp - tm) * (tp - fit_t (t, m, k - 2)));
      const double gamma = (t[k] - tm) * (t[k] - tm)
                           / ((tp - tm) * (fit_t (t, m, k + 2) - tm));
      row[k].a = alpha;
      row[k].b = 1.0 - alpha - gamma;
      row[k].c = gamma;
      row[k].d = Q[k];
    }
  if (unit_tangent (&tan, spline->end_tan_vec))
    {
      row[m].b = 1.0;
      point_axpy (&row[m].d, -hm / 3.0, &tan, &Q[m]);
    }
  else
    {
      const double e = t[m] - fit_t (t, m, (long)m - 2);
      row[m].b = 1.0 / hm + 1.0 / e;
      row[m].a = -1.0 / e;
      point_div (&row[m].d, &Q[m], hm);
    }
}

/* The Thomas algorithm: Gaussian elimination for a tridiagonal system of
   num rows, in place.  Returns 0 on a zero pivot. */
static int
solve_tridiagonal (fit_row *row, const BITCODE_BL num, BITCODE_3DPOINT *x)
{
  BITCODE_BL r;
  // forward: row r becomes x[r] + c'[r] * x[r+1] = d'[r]
  for (r = 0; r < num; r++)
    {
      const double den = row[r].b - (r ? row[r].a * row[r - 1].c : 0.0);
      if (!isfinite (den) || fabs (den) < 1e-300)
        return 0;
      row[r].c /= den;
      if (r)
        point_axpy (&row[r].d, -row[r].a, &row[r - 1].d, &row[r].d);
      point_div (&row[r].d, &row[r].d, den);
    }
  // backward substitution
  x[num - 1] = row[num - 1].d;
  for (r = num - 1; r-- > 0;)
    point_axpy (&x[r], -row[r].c, &x[r + 1], &row[r].d);
  return 1;
}

// At least 2 finite fit points, and an open SPLINE.
static int
spline_fit_points_valid (const Dwg_Entity_SPLINE *spline)
{
  BITCODE_BL i;
  if (!spline || !spline->fit_pts || spline->num_fit_pts < 2
      || spline->closed_b || spline->periodic)
    return 0;
  for (i = 0; i < spline->num_fit_pts; i++)
    if (!isfinite (spline->fit_pts[i].x) || !isfinite (spline->fit_pts[i].y)
        || !isfinite (spline->fit_pts[i].z))
      return 0;
  return 1;
}

// The chord length parameters t[0..m] of the fit points.  Returns 0 for a
// repeated fit point.
static int
fit_chord_params (double *t, const Dwg_Entity_SPLINE *spline,
                  const BITCODE_BL m)
{
  BITCODE_BL k;
  t[0] = 0.0;
  for (k = 1; k <= m; k++)
    {
      const BITCODE_3DPOINT *p = &spline->fit_pts[k - 1],
                            *q = &spline->fit_pts[k];
      const double h
          = sqrt ((q->x - p->x) * (q->x - p->x) + (q->y - p->y) * (q->y - p->y)
                  + (q->z - p->z) * (q->z - p->z));
      if (!(h > 0.0))
        return 0;
      t[k] = t[k - 1] + h;
    }
  return 1;
}

// The n = m + 3 control points: the first fit point, x[0..m], the last
// fit point; and the clamped knots with the chord length parameters inside.
static void
fit_result (Dwg_SPLINE_control_point *cp, double *u, const double *t,
            const BITCODE_3DPOINT *x, const Dwg_Entity_SPLINE *spline,
            const BITCODE_BL m)
{
  const BITCODE_BL n = m + 3;
  BITCODE_BL k;
  for (k = 0; k < n; k++)
    {
      const BITCODE_3DPOINT *p = k == 0       ? &spline->fit_pts[0]
                                 : k == n - 1 ? &spline->fit_pts[m]
                                              : &x[k - 1];
      cp[k].x = p->x;
      cp[k].y = p->y;
      cp[k].z = p->z;
      cp[k].w = 1.0;
    }
  for (k = 0; k < 4; k++)
    {
      u[k] = 0.0;
      u[n + k] = t[m];
    }
  for (k = 1; k < m; k++)
    u[k + 3] = t[k];
}

/* AutoCAD's cubic interpolation of the fit points of a SPLINE: chord length
   knots, the curve through every fit point, and at each end the unit
   tangent if given, else the natural end condition.  This reproduces the
   control points AutoCAD writes to DXF.  Sets *out to a copy of the SPLINE
   with these control points and knots; the caller frees out->ctrl_pts and
   out->knots.  Returns 0 for fewer than 2 fit points, repeated points, or
   a closed SPLINE. */
int
spline_from_fit_points (const Dwg_Entity_SPLINE *spline,
                        Dwg_Entity_SPLINE *out)
{
  BITCODE_BL m, n;
  double *t = NULL, *u = NULL;
  fit_row *row = NULL;
  BITCODE_3DPOINT *x = NULL;
  Dwg_SPLINE_control_point *cp = NULL;
  int ok = 0;

  if (!spline_fit_points_valid (spline))
    return 0;
  m = spline->num_fit_pts - 1;
  n = m + 3;
  t = (double *)malloc ((m + 1) * sizeof (double));
  row = (fit_row *)malloc ((m + 1) * sizeof (fit_row));
  x = (BITCODE_3DPOINT *)malloc ((m + 1) * sizeof (BITCODE_3DPOINT));
  u = (double *)malloc ((n + 4) * sizeof (double));
  cp = (Dwg_SPLINE_control_point *)calloc (n, sizeof (*cp));
  if (t && row && x && u && cp && fit_chord_params (t, spline, m))
    {
      fit_rows (row, t, m, spline);
      ok = solve_tridiagonal (row, m + 1, x);
    }
  if (ok)
    {
      fit_result (cp, u, t, x, spline, m);
      *out = *spline;
      out->degree = 3;
      out->rational = 0;
      out->num_ctrl_pts = n;
      out->ctrl_pts = cp;
      out->num_knots = n + 4;
      out->knots = u;
    }
  else
    {
      free (cp);
      free (u);
    }
  free (t);
  free (row);
  free (x);
  return ok;
}

// A growing array of points.  ok is 0 after a failed allocation.
typedef struct _point_list
{
  BITCODE_3DPOINT *pts;
  BITCODE_BL num, cap;
  int ok;
} point_list;

static void
point_list_add (point_list *l, const BITCODE_3DPOINT *p)
{
  if (!l->ok)
    return;
  if (l->num == l->cap)
    {
      const BITCODE_BL cap = l->cap ? 2 * l->cap : 64;
      BITCODE_3DPOINT *pts
          = (BITCODE_3DPOINT *)realloc (l->pts, cap * sizeof (*pts));
      if (!pts || cap > SPLINE_MAX_POINTS)
        {
          free (pts ? pts : l->pts);
          l->pts = NULL;
          l->ok = 0;
          return;
        }
      l->pts = pts;
      l->cap = cap;
    }
  l->pts[l->num++] = *p;
}

// The distance from p to the segment a-b.
static double
segment_distance (const BITCODE_3DPOINT *p, const BITCODE_3DPOINT *a,
                  const BITCODE_3DPOINT *b)
{
  const double dx = b->x - a->x, dy = b->y - a->y, dz = b->z - a->z;
  const double l2 = dx * dx + dy * dy + dz * dz;
  double s = 0.0, ex, ey, ez;
  if (l2 > 0.0)
    {
      s = ((p->x - a->x) * dx + (p->y - a->y) * dy + (p->z - a->z) * dz) / l2;
      s = s < 0.0 ? 0.0 : (s > 1.0 ? 1.0 : s);
    }
  ex = p->x - a->x - s * dx;
  ey = p->y - a->y - s * dy;
  ez = p->z - a->z - s * dz;
  return sqrt (ex * ex + ey * ey + ez * ez);
}

// A Bezier curve lies in the convex hull of its control points, so if all
// of them are within tol of the chord b[0]-b[p], so is the curve.
static int
bezier_is_flat (const BITCODE_3DPOINT *b, const int p, const double tol)
{
  int i;
  for (i = 1; i < p; i++)
    if (segment_distance (&b[i], &b[0], &b[p]) > tol)
      return 0;
  return 1;
}

// De Casteljau at t = 1/2: only midpoints.  The left half has the first
// point of each round, the right half the last one.
static void
bezier_halves (const BITCODE_3DPOINT *b, const int p, BITCODE_3DPOINT *left,
               BITCODE_3DPOINT *right)
{
  BITCODE_3DPOINT w[SPLINE_MAX_DEGREE + 1];
  int r, i;
  for (i = 0; i <= p; i++)
    w[i] = b[i];
  left[0] = w[0];
  right[p] = w[p];
  for (r = 1; r <= p; r++)
    {
      for (i = 0; i <= p - r; i++)
        {
          w[i].x = (w[i].x + w[i + 1].x) / 2.0;
          w[i].y = (w[i].y + w[i + 1].y) / 2.0;
          w[i].z = (w[i].z + w[i + 1].z) / 2.0;
        }
      left[r] = w[0];
      right[p - r] = w[p - r];
    }
}

// Adds the points of the Bezier curve b of degree p, after its first one,
// halving it until each piece is flat within tol.
static void
bezier_flatten (point_list *l, const BITCODE_3DPOINT *b, const int p,
                const double tol, const int depth)
{
  BITCODE_3DPOINT left[SPLINE_MAX_DEGREE + 1], right[SPLINE_MAX_DEGREE + 1];
  if (depth >= BEZIER_MAX_DEPTH || bezier_is_flat (b, p, tol))
    {
      point_list_add (l, &b[p]);
      return;
    }
  bezier_halves (b, p, left, right);
  bezier_flatten (l, left, p, tol, depth + 1);
  bezier_flatten (l, right, p, tol, depth + 1);
}

// The size of a SPLINE: the diagonal of the box of its points.
static double
points_size (const BITCODE_3DPOINT *pts, const BITCODE_BL num)
{
  BITCODE_3DPOINT lo = pts[0], hi = pts[0];
  BITCODE_BL i;
  for (i = 1; i < num; i++)
    {
      lo.x = fmin (lo.x, pts[i].x);
      lo.y = fmin (lo.y, pts[i].y);
      lo.z = fmin (lo.z, pts[i].z);
      hi.x = fmax (hi.x, pts[i].x);
      hi.y = fmax (hi.y, pts[i].y);
      hi.z = fmax (hi.z, pts[i].z);
    }
  return sqrt ((hi.x - lo.x) * (hi.x - lo.x) + (hi.y - lo.y) * (hi.y - lo.y)
               + (hi.z - lo.z) * (hi.z - lo.z));
}

// For SPLINEs without Bezier curves (rational, not clamped):
// SPLINE_EVAL_STEPS evaluated points per knot span.
static void
spline_eval_points (point_list *l, const Dwg_Entity_SPLINE *spline)
{
  BITCODE_BL m;
  int first = 1;
  for (m = spline->degree; m < spline->num_ctrl_pts; m++)
    {
      const double u0 = spline->knots[m], u1 = spline->knots[m + 1];
      int i;
      if (u1 <= u0)
        continue;
      for (i = first ? 0 : 1; i <= SPLINE_EVAL_STEPS; i++)
        {
          BITCODE_3DPOINT pt;
          if (spline_eval (&pt, spline,
                           u0 + (u1 - u0) * i / SPLINE_EVAL_STEPS))
            point_list_add (l, &pt);
          first = 0;
        }
    }
}

/* Points on the curve of a SPLINE, as a malloc'ed array, for outputs
   without curves.  Bezier curves are halved by de Casteljau until the
   polyline is within rel_tol times the size of the SPLINE of the curve.
   A SPLINE with only fit points is interpolated first.  Returns the number
   of points, or 0. */
int
spline_flatten (const Dwg_Entity_SPLINE *spline, const double rel_tol,
                BITCODE_3DPOINT **pts)
{
  Dwg_Entity_SPLINE fit;
  const Dwg_Entity_SPLINE *s = spline;
  BITCODE_3DPOINT *bez = NULL;
  point_list l = { NULL, 0, 0, 1 };
  int k, j;

  *pts = NULL;
  if (!spline_is_valid (spline))
    {
      if (!spline_from_fit_points (spline, &fit))
        return 0;
      s = &fit;
    }
  k = spline_to_bezier (s, &bez);
  if (k)
    {
      const int p = s->degree;
      const double tol = rel_tol * points_size (bez, k * p + 1);
      point_list_add (&l, &bez[0]);
      for (j = 0; j < k; j++)
        bezier_flatten (&l, &bez[j * p], p, tol, 0);
      free (bez);
    }
  else
    spline_eval_points (&l, s);
  if (s == &fit)
    {
      free (fit.ctrl_pts);
      free (fit.knots);
    }
  if (!l.ok || l.num < 2)
    {
      free (l.pts);
      return 0;
    }
  *pts = l.pts;
  return (int)l.num;
}

/* The signed area of the polygon pts[0..num-1], closed or not, by the
   shoelace formula: positive if counter-clockwise. */
double
polygon_area (const BITCODE_2BD *pts, const BITCODE_BL num)
{
  double a = 0.0;
  BITCODE_BL i;
  for (i = 0; i < num; i++)
    {
      const BITCODE_2BD *p = &pts[i], *q = &pts[(i + 1) % num];
      a += p->x * q->y - q->x * p->y;
    }
  return a / 2.0;
}

/* True if (x,y) is inside the polygon pts[0..num-1], by the even-odd rule:
   a ray to the right crosses its edges an odd number of times. */
int
point_in_polygon (const double x, const double y, const BITCODE_2BD *pts,
                  const BITCODE_BL num)
{
  int inside = 0;
  BITCODE_BL i, j;
  for (i = 0, j = num - 1; i < num; j = i++)
    if ((pts[i].y > y) != (pts[j].y > y)
        && x < pts[j].x
                   + (pts[i].x - pts[j].x) * (y - pts[j].y)
                         / (pts[i].y - pts[j].y))
      inside = !inside;
  return inside;
}

// True if (x,y) is within eps of an edge of the polygon pts[0..num-1].
static int
point_on_polygon (const double x, const double y, const BITCODE_2BD *pts,
                  const BITCODE_BL num, const double eps)
{
  BITCODE_BL i;
  for (i = 0; i < num; i++)
    {
      const BITCODE_2BD *a = &pts[i], *b = &pts[(i + 1) % num];
      const double dx = b->x - a->x, dy = b->y - a->y;
      const double l2 = dx * dx + dy * dy;
      double s = 0.0;
      if (l2 > 0.0)
        {
          s = ((x - a->x) * dx + (y - a->y) * dy) / l2;
          s = s < 0.0 ? 0.0 : (s > 1.0 ? 1.0 : s);
        }
      if (hypot (x - a->x - s * dx, y - a->y - s * dy) <= eps)
        return 1;
    }
  return 0;
}

/* True if polygon a lies inside polygon b, for polygons that do not cross:
   the first vertex of a that is not on the boundary of b decides, as a
   vertex where a touches b says nothing.  If all of them are on it, as for
   equal polygons, a is not inside. */
int
polygon_in_polygon (const BITCODE_2BD *a, const BITCODE_BL na,
                    const BITCODE_2BD *b, const BITCODE_BL nb)
{
  double xmin = b[0].x, xmax = b[0].x, ymin = b[0].y, ymax = b[0].y, eps;
  BITCODE_BL i;
  for (i = 1; i < nb; i++)
    {
      xmin = fmin (xmin, b[i].x);
      xmax = fmax (xmax, b[i].x);
      ymin = fmin (ymin, b[i].y);
      ymax = fmax (ymax, b[i].y);
    }
  eps = 1e-9 * hypot (xmax - xmin, ymax - ymin);
  for (i = 0; i < na; i++)
    if (!point_on_polygon (a[i].x, a[i].y, b, nb, eps))
      return point_in_polygon (a[i].x, a[i].y, b, nb);
  return 0;
}

// Adds (x,y) unless it repeats the last point within eps.
static void
ring_add (point_list *l, const double x, const double y, const double eps)
{
  BITCODE_3DPOINT p;
  if (l->num && fabs (l->pts[l->num - 1].x - x) <= eps
      && fabs (l->pts[l->num - 1].y - y) <= eps)
    return;
  p.x = x;
  p.y = y;
  p.z = 0.0;
  point_list_add (l, &p);
}

/* Adds the points of an elliptical arc, start and end included: center c,
   major axis vector major, minor = ratio * major turned by 90 degrees,
   from parameter t0 by the signed sweep.  A circular arc is an ellipse
   with ratio 1.  Enough points so that the chords are within tol of the
   curve: a step dt of the parameter has its largest sagitta a * dt^2 / 8
   at the end of the major axis, as for a circle of radius a. */
static void
ring_add_ellipse_sweep (point_list *l, const BITCODE_2RD c,
                        const BITCODE_2RD major, const double ratio,
                        const double t0, const double sweep, const double tol)
{
  const double a = hypot (major.x, major.y);
  const double step = tol < a ? 2.0 * acos (1.0 - tol / a) : M_PI;
  double steps;
  int n, i;
  // corrupt data: no ring rather than a ring without this arc
  if (!isfinite (t0) || !isfinite (sweep) || !isfinite (step) || step <= 0.0)
    {
      l->ok = 0;
      return;
    }
  steps = ceil (fabs (sweep) / step);
  n = steps < 1.0 ? 1 : (steps > 4096.0 ? 4096 : (int)steps);
  for (i = 0; i <= n; i++)
    {
      const double t = t0 + sweep * i / n;
      ring_add (l, c.x + major.x * cos (t) - ratio * major.y * sin (t),
                c.y + major.y * cos (t) + ratio * major.x * sin (t),
                tol * 1e-6);
    }
}

/* Adds the points of an elliptical or circular arc edge of a HATCH, from
   parameter start to end.  As in DXF, a clockwise arc is defined in a
   mirrored system, which turns it into the counter-clockwise arc from
   -end to -start.  Equal parameters give a full ellipse. */
static void
ring_add_ellipse_arc (point_list *l, const BITCODE_2RD c,
                      const BITCODE_2RD major, const double ratio,
                      const double start, const double end, const int ccw,
                      const double tol)
{
  double sweep = fmod (end - start, 2.0 * M_PI);
  if (sweep < 0.0)
    sweep += 2.0 * M_PI;
  if (sweep < 1e-12)
    sweep = 2.0 * M_PI;
  ring_add_ellipse_sweep (l, c, major, ratio, ccw ? start : -end, sweep, tol);
}

// Adds the points of a SPLINE edge, by spline_flatten.
static void
ring_add_spline (point_list *l, const Dwg_HATCH_PathSeg *seg,
                 const double rel_tol)
{
  Dwg_Entity_SPLINE s;
  Dwg_SPLINE_control_point *cp = NULL;
  BITCODE_3DPOINT *fit = NULL, *pts = NULL;
  BITCODE_BL i;
  int n;
  memset (&s, 0, sizeof (s));
  s.degree = seg->degree;
  s.rational = seg->is_rational;
  s.periodic = seg->is_periodic;
  s.num_knots = seg->num_knots;
  s.knots = seg->knots;
  if (seg->num_control_points && seg->control_points
      && (cp = (Dwg_SPLINE_control_point *)calloc (seg->num_control_points,
                                                   sizeof (*cp))))
    for (i = 0; i < seg->num_control_points; i++)
      {
        cp[i].x = seg->control_points[i].point.x;
        cp[i].y = seg->control_points[i].point.y;
        cp[i].w = seg->is_rational ? seg->control_points[i].weight : 1.0;
      }
  s.num_ctrl_pts = cp ? seg->num_control_points : 0;
  s.ctrl_pts = cp;
  if (seg->num_fitpts && seg->fitpts
      && (fit = (BITCODE_3DPOINT *)calloc (seg->num_fitpts, sizeof (*fit))))
    for (i = 0; i < seg->num_fitpts; i++)
      {
        fit[i].x = seg->fitpts[i].x;
        fit[i].y = seg->fitpts[i].y;
      }
  s.num_fit_pts = fit ? seg->num_fitpts : 0;
  s.fit_pts = fit;
  s.beg_tan_vec.x = seg->start_tangent.x;
  s.beg_tan_vec.y = seg->start_tangent.y;
  s.end_tan_vec.x = seg->end_tangent.x;
  s.end_tan_vec.y = seg->end_tangent.y;
  n = spline_flatten (&s, rel_tol, &pts);
  for (i = 0; i < (BITCODE_BL)n; i++)
    ring_add (l, pts[i].x, pts[i].y, 0.0);
  free (pts);
  free (cp);
  free (fit);
}

// Adds (x,y) to a list of defining points.
static void
box_add (point_list *l, const double x, const double y)
{
  BITCODE_3DPOINT p;
  p.x = x;
  p.y = y;
  p.z = 0.0;
  point_list_add (l, &p);
}

// Adds the control points and the fit points of a SPLINE edge.
static void
box_add_spline (point_list *l, const Dwg_HATCH_PathSeg *s)
{
  BITCODE_BL i;
  for (i = 0; s->control_points && i < s->num_control_points; i++)
    box_add (l, s->control_points[i].point.x, s->control_points[i].point.y);
  for (i = 0; s->fitpts && i < s->num_fitpts; i++)
    box_add (l, s->fitpts[i].x, s->fitpts[i].y);
}

// Adds the defining points of the edge s: its endpoints, the box of its
// circle or ellipse, or the points of its SPLINE.
static void
box_add_edge (point_list *l, const Dwg_HATCH_PathSeg *s)
{
  const double r = s->curve_type == 2   ? s->radius
                   : s->curve_type == 3 ? hypot (s->endpoint.x, s->endpoint.y)
                                        : 0.0;
  if (s->curve_type == 1)
    {
      box_add (l, s->first_endpoint.x, s->first_endpoint.y);
      box_add (l, s->second_endpoint.x, s->second_endpoint.y);
    }
  else if (s->curve_type == 2 || s->curve_type == 3)
    {
      box_add (l, s->center.x - r, s->center.y - r);
      box_add (l, s->center.x + r, s->center.y + r);
    }
  else if (s->curve_type == 4)
    box_add_spline (l, s);
}

// The size of a HATCH path: the diagonal of the box of its defining points.
static double
hatch_path_size (const Dwg_HATCH_Path *path)
{
  point_list l = { NULL, 0, 0, 1 };
  BITCODE_BL i;
  double size;
  if (path->flag & 2)
    for (i = 0; path->polyline_paths && i < path->num_segs_or_paths; i++)
      box_add (&l, path->polyline_paths[i].point.x,
               path->polyline_paths[i].point.y);
  else
    for (i = 0; path->segs && i < path->num_segs_or_paths; i++)
      box_add_edge (&l, &path->segs[i]);
  size = l.ok && l.num ? points_size (l.pts, l.num) : 0.0;
  free (l.pts);
  return size;
}

// Adds the points of the edge seg of a HATCH path.
static void
ring_add_edge (point_list *l, const Dwg_HATCH_PathSeg *seg,
               const double rel_tol, const double tol)
{
  BITCODE_2RD x_axis;
  switch (seg->curve_type)
    {
    case 1: // line
      ring_add (l, seg->first_endpoint.x, seg->first_endpoint.y, 0.0);
      ring_add (l, seg->second_endpoint.x, seg->second_endpoint.y, 0.0);
      break;
    case 2: // circular arc: an ellipse with ratio 1
      x_axis.x = seg->radius;
      x_axis.y = 0.0;
      ring_add_ellipse_arc (l, seg->center, x_axis, 1.0, seg->start_angle,
                            seg->end_angle, seg->is_ccw, tol);
      break;
    case 3:
      ring_add_ellipse_arc (l, seg->center, seg->endpoint,
                            seg->minor_major_ratio, seg->start_angle,
                            seg->end_angle, seg->is_ccw, tol);
      break;
    case 4:
      ring_add_spline (l, seg, rel_tol);
      break;
    default:
      break;
    }
}

// Adds the points of a polyline path, with its bulges.
static void
ring_add_polyline (point_list *l, const Dwg_HATCH_Path *path, const double tol)
{
  const BITCODE_BL n = path->num_segs_or_paths;
  BITCODE_BL i;
  for (i = 0; i < n; i++)
    {
      const Dwg_HATCH_PolylinePath *v = &path->polyline_paths[i],
                                   *w = &path->polyline_paths[(i + 1) % n];
      BITCODE_2BD p1, p2, ctr;
      double radius, start, sweep;
      p1.x = v->point.x;
      p1.y = v->point.y;
      p2.x = w->point.x;
      p2.y = w->point.y;
      if (path->bulges_present
          && bulge_to_arc (p1, p2, v->bulge, &ctr, &radius, &start, &sweep))
        {
          BITCODE_2RD c, x_axis;
          c.x = ctr.x;
          c.y = ctr.y;
          x_axis.x = radius;
          x_axis.y = 0.0;
          ring_add_ellipse_sweep (l, c, x_axis, 1.0, start, sweep, tol);
        }
      else
        ring_add (l, p1.x, p1.y, 0.0);
    }
}

// A closed ring is usable with at least 3 distinct points, finite
// coordinates, and an area that is not zero relative to its size.
static int
ring_is_usable (const point_list *l)
{
  double area = 0.0, size;
  BITCODE_BL i;
  if (!l->ok || l->num < 4)
    return 0;
  for (i = 0; i < l->num; i++)
    if (!isfinite (l->pts[i].x) || !isfinite (l->pts[i].y))
      return 0;
  for (i = 0; i + 1 < l->num; i++)
    area += l->pts[i].x * l->pts[i + 1].y - l->pts[i + 1].x * l->pts[i].y;
  size = points_size (l->pts, l->num);
  return fabs (area) / 2.0 > 1e-12 * size * size;
}

// Closes the ring and copies it to a malloc'ed 2D array.  Returns the
// number of points, or 0 if the ring is not usable.
static int
ring_finish (point_list *l, BITCODE_2BD **pts)
{
  BITCODE_BL i;
  if (l->ok && l->num
      && (l->pts[l->num - 1].x != l->pts[0].x
          || l->pts[l->num - 1].y != l->pts[0].y))
    {
      const BITCODE_3DPOINT first = l->pts[0];
      point_list_add (l, &first);
    }
  if (!ring_is_usable (l)
      || !(*pts = (BITCODE_2BD *)malloc (l->num * sizeof (BITCODE_2BD))))
    return 0;
  for (i = 0; i < l->num; i++)
    {
      (*pts)[i].x = l->pts[i].x;
      (*pts)[i].y = l->pts[i].y;
    }
  return (int)l->num;
}

/* The boundary of a HATCH path as a closed ring of points: polylines with
   their bulges, and edges of lines, circular and elliptical arcs and
   SPLINEs.  Curves are within rel_tol times the size of the path.  Returns
   the number of points, the last one repeating the first, as a malloc'ed
   array in *pts; or 0 for fewer than 3 distinct points, non-finite
   coordinates or a zero area. */
int
hatch_path_points (const Dwg_HATCH_Path *path, const double rel_tol,
                   BITCODE_2BD **pts)
{
  point_list l = { NULL, 0, 0, 1 };
  const double tol = rel_tol * hatch_path_size (path);
  BITCODE_BL i;
  int num;

  *pts = NULL;
  if (path->flag & 2)
    {
      if (path->polyline_paths && path->num_segs_or_paths)
        ring_add_polyline (&l, path, tol);
    }
  else
    for (i = 0; path->segs && i < path->num_segs_or_paths; i++)
      ring_add_edge (&l, &path->segs[i], rel_tol, tol);
  num = ring_finish (&l, pts);
  free (l.pts);
  return num;
}

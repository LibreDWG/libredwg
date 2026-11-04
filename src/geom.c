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
      out->x = pt.x * ax.x + pt.y * ax.y;
      out->y = pt.x * ay.x + pt.y * ay.y;
    }
  return;
}

// transform a 3D point via its OCS (extrusion or normal) to 3D
void
transform_OCS (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt, BITCODE_BE ext)
{
  if (ext.x == 0.0 && ext.y == 0.0 && ext.z == 1.0)
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
      out->x = pt.x * ax.x + pt.y * ax.y + pt.z * ax.z;
      out->y = pt.x * ay.x + pt.y * ay.y + pt.z * ay.z;
      out->z = pt.x * az.x + pt.y * az.y + pt.z * az.z;
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

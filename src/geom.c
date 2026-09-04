/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019,2022-2023 Free Software Foundation, Inc.              */
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
 */

#include "config.h"
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

// TODO: bulge -> arc for svg and ps.

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
  start_angle = angle_normalize (start_angle);
  end_angle = angle_normalize (end_angle);
  if (end_angle < start_angle)
    end_angle += 2 * M_PI;
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


/*----------------------------------------------------------------------
 * V4 Residue Topology Spatial Transformations
 *----------------------------------------------------------------------*/

static inline void v4_transform_3d(double *x, double *y, double *z) {
    double s0, d0, s1, d1;

    if (!x || !y || !z) return;

    s0 = *x + *y;
    d0 = *x - *y;
    s1 = *z + 1.0;
    d1 = *z - 1.0;

    *x = s0 + s1;
    *y = d0 + d1;
    *z = s0 - s1;
}

static inline void v4_transform_2d(double *x, double *y) {
    double s0, d0;
    if (!x || !y) return;

    s0 = *x + *y;
    d0 = *x - *y;

    *x = s0 + 1.0;
    *y = d0 - 1.0;
}

EXPORT void dwg_geom_v4_transform_2d(double *x, double *y) {
    v4_transform_2d(x, y);
}

EXPORT uint32_t dwg_transform_v4(Dwg_Data *dwg) {
    uint32_t count = 0;
    uint32_t i;

    if (!dwg || !dwg->object) return 0;

    for (i = 0; i < dwg->num_objects; i++) {
        Dwg_Object *obj = &dwg->object[i];
        if (!obj || !obj->tio.entity) continue;

        switch (obj->type) {
            case DWG_TYPE_POINT: {
                Dwg_Entity_POINT *e = obj->tio.entity->tio.POINT;
                if (e) {
                    v4_transform_3d(&e->x, &e->y, &e->z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_LINE: {
                Dwg_Entity_LINE *e = obj->tio.entity->tio.LINE;
                if (e) {
                    v4_transform_3d(&e->start.x, &e->start.y, &e->start.z);
                    v4_transform_3d(&e->end.x, &e->end.y, &e->end.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_CIRCLE: {
                Dwg_Entity_CIRCLE *e = obj->tio.entity->tio.CIRCLE;
                if (e) {
                    v4_transform_3d(&e->center.x, &e->center.y, &e->center.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_ARC: {
                Dwg_Entity_ARC *e = obj->tio.entity->tio.ARC;
                if (e) {
                    v4_transform_3d(&e->center.x, &e->center.y, &e->center.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_ELLIPSE: {
                Dwg_Entity_ELLIPSE *e = obj->tio.entity->tio.ELLIPSE;
                if (e) {
                    v4_transform_3d(&e->center.x, &e->center.y, &e->center.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE__3DFACE: {
                Dwg_Entity__3DFACE *e = obj->tio.entity->tio._3DFACE;
                if (e) {
                    v4_transform_3d(&e->corner1.x, &e->corner1.y, &e->corner1.z);
                    v4_transform_3d(&e->corner2.x, &e->corner2.y, &e->corner2.z);
                    v4_transform_3d(&e->corner3.x, &e->corner3.y, &e->corner3.z);
                    v4_transform_3d(&e->corner4.x, &e->corner4.y, &e->corner4.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_SOLID: {
                Dwg_Entity_SOLID *e = obj->tio.entity->tio.SOLID;
                if (e) {
                    v4_transform_3d(&e->corner1.x, &e->corner1.y, &e->elevation);
                    v4_transform_3d(&e->corner2.x, &e->corner2.y, &e->elevation);
                    v4_transform_3d(&e->corner3.x, &e->corner3.y, &e->elevation);
                    v4_transform_3d(&e->corner4.x, &e->corner4.y, &e->elevation);
                    count++;
                }
                break;
            }
            case DWG_TYPE_TRACE: {
                Dwg_Entity_TRACE *e = obj->tio.entity->tio.TRACE;
                if (e) {
                    v4_transform_3d(&e->corner1.x, &e->corner1.y, &e->elevation);
                    v4_transform_3d(&e->corner2.x, &e->corner2.y, &e->elevation);
                    v4_transform_3d(&e->corner3.x, &e->corner3.y, &e->elevation);
                    v4_transform_3d(&e->corner4.x, &e->corner4.y, &e->elevation);
                    count++;
                }
                break;
            }
            case DWG_TYPE_LWPOLYLINE: {
                Dwg_Entity_LWPOLYLINE *e = obj->tio.entity->tio.LWPOLYLINE;
                uint32_t j;
                double z;

                if (e && e->points) {
                    for (j = 0; j < e->num_points; j++) {
                        z = e->elevation;
                        v4_transform_3d(&e->points[j].x, &e->points[j].y, &z);
                        e->elevation = z;
                    }
                    count++;
                }
                break;
            }
            case DWG_TYPE_SPLINE: {
                Dwg_Entity_SPLINE *e = obj->tio.entity->tio.SPLINE;
                uint32_t j;

                if (e) {
                    if (e->ctrl_pts) {
                        for (j = 0; j < e->num_ctrl_pts; j++) {
                            v4_transform_3d(&e->ctrl_pts[j].x, &e->ctrl_pts[j].y, &e->ctrl_pts[j].z);
                        }
                    }
                    if (e->fit_pts) {
                        for (j = 0; j < e->num_fit_pts; j++) {
                            v4_transform_3d(&e->fit_pts[j].x, &e->fit_pts[j].y, &e->fit_pts[j].z);
                        }
                    }
                    count++;
                }
                break;
            }
            case DWG_TYPE_INSERT: {
                Dwg_Entity_INSERT *e = obj->tio.entity->tio.INSERT;
                if (e) {
                    v4_transform_3d(&e->ins_pt.x, &e->ins_pt.y, &e->ins_pt.z);
                    count++;
                }
                break;
            }
            case DWG_TYPE_TEXT: {
                Dwg_Entity_TEXT *e = obj->tio.entity->tio.TEXT;
                double z;

                if (e) {
                    z = e->elevation;
                    v4_transform_3d(&e->ins_pt.x, &e->ins_pt.y, &z);
                    e->elevation = z;
                    count++;
                }
                break;
            }
            case DWG_TYPE_MTEXT: {
                Dwg_Entity_MTEXT *e = obj->tio.entity->tio.MTEXT;
                if (e) {
                    v4_transform_3d(&e->ins_pt.x, &e->ins_pt.y, &e->ins_pt.z);
                    count++;
                }
                break;
            }
            default:
                break;
        }
    }

    return count;
}
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
  while (start_angle > end_angle)
    end_angle += M_PI;
  // shoot vectors from ctr to ang
  ang = start_angle;
  angd = (end_angle - start_angle) / num_pts;
  while ((angd = (end_angle - start_angle) / (num_pts - 1)) < 0)
    start_angle += M_PI;
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

/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019 Free Software Foundation, Inc.                        */
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
 * written by Reini Urban
*/

#include "../src/config.h"
#include <math.h>
#include <dwg.h>
#include "geom.h"

void
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

void
cross (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt1, BITCODE_3DPOINT pt2)
{
  out->x = pt1.y * pt2.z - pt1.z * pt2.y;
  out->y = pt1.z * pt2.x - pt1.x * pt2.z;
  out->z = pt1.x * pt2.y - pt1.y * pt2.x;
}

// transform a 2D point via its OCS (extrusion) to 2D
void
transform_OCS_2d (BITCODE_2DPOINT *out, BITCODE_2DPOINT pt, BITCODE_BE ext)
{
  if (ext.x == 0.0 && ext.y == 0.0 && ext.z == 1.0)
    {
      *out = pt;
    }
  else if (ext.x == 0.0 && ext.y == 0.0 && ext.z == -1.0)
    {
      *out = pt;
      out->x = - out->x;
    }
  else
    {
      /* This is called the "Arbitrary Axis Algorithm" to calculate
         the OCS x-axis from the extrusion z-vector */
      BITCODE_3DPOINT ax, ay, az, be;
      be = (BITCODE_3DPOINT)ext;
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

// transform a 3D point via its OCS (extrusion) to 2D
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
      out->x = - out->x;
    }
  else
    {
      /* This is called the "Arbitrary Axis Algorithm" to calculate
         the OCS x-axis from the extrusion z-vector */
      BITCODE_3DPOINT ax, ay, az, be;
      be = (BITCODE_3DPOINT)ext;
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

// TODO: bulge -> arc for svg and ps

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
 * geom.h: geometric projections from OCS
 * written by Reini Urban
 */

#ifndef _GEOM_H_
#define _GEOM_H_

#include "config.h"
#include <dwg.h>

#ifndef M_PI
#  define M_PI 3.14159265358979323846264338327950288
#endif

// void normalize (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt);
// void cross (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt1, BITCODE_3DPOINT pt2);
//  transform a 2D point via its OCS (extrusion) to 2D
EXPORT void transform_OCS_2d (BITCODE_2DPOINT *out, BITCODE_2DPOINT pt,
                              BITCODE_BE ext)
  __nonnull ((1));
// transform a 3D point via its OCS (extrusion) to 2D
EXPORT void transform_OCS (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt,
                           BITCODE_BE ext)
  __nonnull ((1));

// TODO: bulge -> arc for svg and ps
void angle_vector_2d (BITCODE_2DPOINT *out, BITCODE_2DPOINT ctr,
                      BITCODE_BD angle, BITCODE_BD len)
  __nonnull ((1));
EXPORT void arc_split (BITCODE_2BD *pts, const int num_pts,
                       const BITCODE_2BD center_2d,
                       BITCODE_BD start_angle,
                       BITCODE_BD end_angle, const BITCODE_BD radius)
  __nonnull ((1));

#endif /* _GEOM_H_ */

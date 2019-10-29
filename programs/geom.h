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

#include "../src/config.h"
#include <dwg.h>

#ifndef M_PI
#  define M_PI 3.14159265358979323846264338327950288
#endif

void normalize (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt);
void cross (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt1, BITCODE_3DPOINT pt2);
// transform a 2D point via its OCS (extrusion) to 2D
void transform_OCS_2d (BITCODE_2DPOINT *out, BITCODE_2DPOINT pt, BITCODE_BE ext);
// transform a 3D point via its OCS (extrusion) to 2D
void transform_OCS (BITCODE_3DPOINT *out, BITCODE_3DPOINT pt, BITCODE_BE ext);
// TODO: bulge -> arc for svg and ps

#endif  /* _GEOM_H_ */

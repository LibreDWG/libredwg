/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2013, 2018 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * dwg_api.c: external C API
 *
 * written by Gaganjyot Singh
 * modified by Reini Urban
 */

#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>

#include "config.h"
#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif
#include "dwg.h"
#include "common.h"
#include "logging.h"
#include "bits.h"
#include "dwg_api.h"

/** We don't pass in Dwg_Object*'s, so we don't know if the object
   is >= r2007 or <r13 or what. Default is r2000.
   So we need some dwg_api_init_version(&dwg) to store the version.
*/
static Dwg_Version_Type dwg_version = R_INVALID;

/*******************************************************************
 *        Functions created from macro to extract entities           *
 ********************************************************************/

/// extract all text entities from a block
GET_DWG_ENTITY(TEXT)
/// extract all attrib entities from a block
GET_DWG_ENTITY(ATTRIB)
/// extract all attdef entities from a block
GET_DWG_ENTITY(ATTDEF)
/// extract all block entities from a block
GET_DWG_ENTITY(BLOCK)
/// extract endblk entity from a block
GET_DWG_ENTITY(ENDBLK)
/// extract all seqend entities from a block
GET_DWG_ENTITY(SEQEND)
/// extract all insert entities from a block
GET_DWG_ENTITY(INSERT)
/// extract all minsert entities from a block
GET_DWG_ENTITY(MINSERT)
/// extract all vertex_2d entities from a block
GET_DWG_ENTITY(VERTEX_2D)
/// extract all vertex_3d entities from a block
GET_DWG_ENTITY(VERTEX_3D)
/// extract all vertex_mesh entities from a block
GET_DWG_ENTITY(VERTEX_MESH)
/// extract all vertex_pface entities from a block
GET_DWG_ENTITY(VERTEX_PFACE)
/// extract all vertex_pface_face entities from a block
GET_DWG_ENTITY(VERTEX_PFACE_FACE)
/// extract all polyline_2d entities from a block
GET_DWG_ENTITY(POLYLINE_2D)
/// extract all polyline_3d entities from a block
GET_DWG_ENTITY(POLYLINE_3D)
/// extract all arc entities from a block
GET_DWG_ENTITY(ARC)
/// extract all circle entities from a block
GET_DWG_ENTITY(CIRCLE)
/// extract all line entities from a block
GET_DWG_ENTITY(LINE)
/// extract all dimension ordinate entities from a block
GET_DWG_ENTITY(DIMENSION_ORDINATE)
/// extract all dimension linear entities from a block
GET_DWG_ENTITY(DIMENSION_LINEAR)
/// extract all dimension aligned entities from a block
GET_DWG_ENTITY(DIMENSION_ALIGNED)
/// extract all dimension ang3pt entities from a block
GET_DWG_ENTITY(DIMENSION_ANG3PT)
/// extract all dimension ang2ln entities from a block
GET_DWG_ENTITY(DIMENSION_ANG2LN)
/// extract all dimension radius entities from a block
GET_DWG_ENTITY(DIMENSION_RADIUS)
/// extract all dimension diameter entities from a block
GET_DWG_ENTITY(DIMENSION_DIAMETER)
/// extract all points entities from a block
GET_DWG_ENTITY(POINT)
/// extract all polyline_pface entities from a block
GET_DWG_ENTITY(POLYLINE_PFACE)
/// extract all polyline_mesh entities from a block
GET_DWG_ENTITY(POLYLINE_MESH)
/// extract all solid entities from a block
GET_DWG_ENTITY(SOLID)
/// extract all trace entities from a block
GET_DWG_ENTITY(TRACE)
/// extract all shape entities from a block
GET_DWG_ENTITY(SHAPE)
/// extract all viewport entities from a block
GET_DWG_ENTITY(VIEWPORT)
/// extract all ellipse entities from a block
GET_DWG_ENTITY(ELLIPSE)
/// extract all spline entities from a block
GET_DWG_ENTITY(SPLINE)
/// extract all region entities from a block
GET_DWG_ENTITY(REGION)
/// extract all body entities from a block
GET_DWG_ENTITY(BODY)
/// extract all ray entities from a block
GET_DWG_ENTITY(RAY)
/// extract all xline entities from a block
GET_DWG_ENTITY(XLINE)
/// extract all OLEFRAME entities from a block
GET_DWG_ENTITY(OLEFRAME)
/// extract all mtext entities from a block
GET_DWG_ENTITY(MTEXT)
/// extract all leader entities from a block
GET_DWG_ENTITY(LEADER)
/// extract all tolerance entities from a block
GET_DWG_ENTITY(TOLERANCE)
/// extract all mline entities from a block
GET_DWG_ENTITY(MLINE)
/// cast dwg object to ole2frame
GET_DWG_ENTITY(OLE2FRAME)
/// extract all DUMMY entities from a block
GET_DWG_ENTITY(DUMMY)
/// extract all LONG_TRANSACTION entities from a block
GET_DWG_ENTITY(LONG_TRANSACTION)
/// extract all lwpline entities from a block
GET_DWG_ENTITY(LWPOLYLINE)
/// extract all PROXY_ENTITY entities from a block
GET_DWG_ENTITY(PROXY_ENTITY)
/// extract all hatch entities from a block
GET_DWG_ENTITY(HATCH)

//untyped >1000:
/// extract all image entities from a block
//GET_DWG_ENTITY(IMAGE)

/*******************************************************************
 *     Functions created from macro to cast dwg_object to entity     *
 *                 Usage :- dwg_object_to_ENTITY(),                  *
 *                where ENTITY can be LINE or CIRCLE                 *
 ********************************************************************/

/// cast dwg object to text
CAST_DWG_OBJECT_TO_ENTITY(TEXT)
/// cast dwg object to attrib
CAST_DWG_OBJECT_TO_ENTITY(ATTRIB)
/// cast dwg object to attdef
CAST_DWG_OBJECT_TO_ENTITY(ATTDEF)
/// cast dwg object to block
CAST_DWG_OBJECT_TO_ENTITY(BLOCK)
/// extract endblk entity from a block
CAST_DWG_OBJECT_TO_ENTITY(ENDBLK)
/// cast dwg object to seqend
CAST_DWG_OBJECT_TO_ENTITY(SEQEND)
/// cast dwg object to insert
CAST_DWG_OBJECT_TO_ENTITY(INSERT)
/// cast dwg object to minsert
CAST_DWG_OBJECT_TO_ENTITY(MINSERT)
/// cast dwg object to vertex_2d
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_2D)
/// cast dwg object to vertex_3d
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_3D)
/// cast dwg object to vertex_mesh
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_MESH)
/// cast dwg object to vertex_pface
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_PFACE)
/// cast dwg object to vertex_pface_face
CAST_DWG_OBJECT_TO_ENTITY(VERTEX_PFACE_FACE)
/// cast dwg object to polyline_2d
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_2D)
/// cast dwg object to polyline_3d
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_3D)
/// cast dwg object to arc
CAST_DWG_OBJECT_TO_ENTITY(ARC)
/// cast dwg object to circle
CAST_DWG_OBJECT_TO_ENTITY(CIRCLE)
/// cast dwg object to line
CAST_DWG_OBJECT_TO_ENTITY(LINE)
/// cast dwg object to dimension ordinate
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ORDINATE)
/// cast dwg object to dimension linear
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_LINEAR)
/// cast dwg object to dimension aligned
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ALIGNED)
/// cast dwg object to dimension ang3pt
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ANG3PT)
/// cast dwg object to dimension ang2ln
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_ANG2LN)
/// cast dwg object to dimension radius
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_RADIUS)
/// cast dwg object to dimension diameter
CAST_DWG_OBJECT_TO_ENTITY(DIMENSION_DIAMETER)
/// cast dwg object to points
CAST_DWG_OBJECT_TO_ENTITY(POINT)
/// cast dwg object to polyline_pface
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_PFACE)
/// cast dwg object to polyline_mesh
CAST_DWG_OBJECT_TO_ENTITY(POLYLINE_MESH)
/// cast dwg object to solid
CAST_DWG_OBJECT_TO_ENTITY(SOLID)
/// cast dwg object to trace
CAST_DWG_OBJECT_TO_ENTITY(TRACE)
/// cast dwg object to shape
CAST_DWG_OBJECT_TO_ENTITY(SHAPE)
/// cast dwg object to viewport
CAST_DWG_OBJECT_TO_ENTITY(VIEWPORT)
/// cast dwg object to ellipse
CAST_DWG_OBJECT_TO_ENTITY(ELLIPSE)
/// cast dwg object to spline
CAST_DWG_OBJECT_TO_ENTITY(SPLINE)
/// cast dwg object to region
CAST_DWG_OBJECT_TO_ENTITY(REGION)
/// cast dwg object to body
CAST_DWG_OBJECT_TO_ENTITY(BODY)
/// cast dwg object to ray
CAST_DWG_OBJECT_TO_ENTITY(RAY)
/// cast dwg object to xline
CAST_DWG_OBJECT_TO_ENTITY(XLINE)
/// cast dwg object to oleframe
CAST_DWG_OBJECT_TO_ENTITY(OLEFRAME)
/// cast dwg object to mtext
CAST_DWG_OBJECT_TO_ENTITY(MTEXT)
/// cast dwg object to leader
CAST_DWG_OBJECT_TO_ENTITY(LEADER)
/// cast dwg object to tolerance
CAST_DWG_OBJECT_TO_ENTITY(TOLERANCE)
/// cast dwg object to mline
CAST_DWG_OBJECT_TO_ENTITY(MLINE)
/// cast dwg object to ole2frame
CAST_DWG_OBJECT_TO_ENTITY(OLE2FRAME)
/// cast dwg object to dummy
CAST_DWG_OBJECT_TO_ENTITY(DUMMY)
/// cast dwg object to long_transaction
CAST_DWG_OBJECT_TO_ENTITY(LONG_TRANSACTION)
/// cast dwg object to lwpline
CAST_DWG_OBJECT_TO_ENTITY(LWPOLYLINE)
/// cast dwg object to hatch
CAST_DWG_OBJECT_TO_ENTITY(HATCH)

//untyped >500:
/// cast dwg object to image
CAST_DWG_OBJECT_TO_ENTITY_BYNAME(IMAGE)


/*******************************************************************
 *     Functions created from macro to cast dwg object to object     *
 *                 Usage :- dwg_object_to_OBJECT(),                  *
 *            where OBJECT can be LAYER or BLOCK_HEADER              *
 ********************************************************************/
/// cast dwg object to block header
CAST_DWG_OBJECT_TO_OBJECT(BLOCK_HEADER)
/// cast dwg object to block control
CAST_DWG_OBJECT_TO_OBJECT(BLOCK_CONTROL)
/// cast dwg object to layer
CAST_DWG_OBJECT_TO_OBJECT(LAYER)
/// cast dwg object to xrecord
CAST_DWG_OBJECT_TO_OBJECT(XRECORD)

/*******************************************************************
 *                FUNCTIONS START HERE ENTITY SPECIFIC               *
 ********************************************************************/

/* not thread safe */
void dwg_api_init_version(Dwg_Data *dwg)
{
  dwg_version = (Dwg_Version_Type)dwg->header.version;
}

/** To access the common DIMENSION fields (only).
    There is no generic call dwg_get_DIMENSION, for this you have to
    specify the exact DIMENSION_* type.
*/
dwg_ent_dim *
dwg_object_to_DIMENSION(dwg_object *obj)
{
    dwg_ent_dim *ret_obj = NULL;
    if(obj != 0 &&
       (obj->type == DWG_TYPE_DIMENSION_ORDINATE ||
        obj->type == DWG_TYPE_DIMENSION_LINEAR ||
        obj->type == DWG_TYPE_DIMENSION_ALIGNED ||
        obj->type == DWG_TYPE_DIMENSION_ANG3PT ||
        obj->type == DWG_TYPE_DIMENSION_ANG2LN ||
        obj->type == DWG_TYPE_DIMENSION_RADIUS ||
        obj->type == DWG_TYPE_DIMENSION_DIAMETER))
      {
        ret_obj = obj->tio.entity->tio.DIMENSION_common;
      }
    else
      {
        LOG_ERROR("invalid %s type: got 0x%x", "DIMENSION", obj ? obj->type : 0);
      }
    return (dwg_ent_dim *)ret_obj;  
}

/*******************************************************************
 *                    FUNCTIONS FOR CIRCLE ENTITY                    *
 ********************************************************************/

/** Returns the _dwg_entity_CIRCLE::center, DXF 10.
\code Usage: dwg_ent_circle_get_center(circle, &point, &error);
\encode
\param[in]  circle  dwg_ent_circle*
\param[out] point   dwg_point_3d*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_circle_get_center(const dwg_ent_circle *restrict circle,
                          dwg_point_3d *restrict point,
                          int *restrict error)
{
  if (point && circle)
    {
      *error = 0;
      point->x = circle->center.x;
      point->y = circle->center.y;
      point->z = circle->center.z;
    }
  else
    {
      LOG_ERROR("%s: empty point or circle", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_CIRCLE::center, DXF 10.
\code Usage: dwg_ent_circle_set_center(circle, &point, &error);
\endcode
\param[in,out] circle dwg_ent_circle*
\param[in]     point  dwg_point_3d*
\param[out]    error  set to 0 for ok, 1 on error
*/
void
dwg_ent_circle_set_center(dwg_ent_circle *restrict circle,
                          const dwg_point_3d *restrict point,
                          int *restrict error)
{
  if (point && circle)
    {
      *error = 0;
      circle->center.x = point->x;
      circle->center.y = point->y;
      circle->center.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty point or circle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_CIRCLE::radius, DXF 40.
\code Usage: double radius = dwg_ent_circle_get_radius(circle, &error);
\endcode
\param[in]  circle  dwg_ent_circle*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_circle_get_radius(const dwg_ent_circle *restrict circle, int *restrict error)
{
  if (circle)
    {
      *error = 0;
      return circle->radius;
    }
  else
    {
      LOG_ERROR("%s: empty circle", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the _dwg_entity_CIRCLE::radius, DXF 40.
\code Usage: dwg_ent_circle_set_radius(circle, radius, &error);
\endcode
\param[in,out] circle  dwg_ent_circle*
\param[in]     radius  double
\param[out]    error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_circle_set_radius(dwg_ent_circle *restrict circle, const BITCODE_BD radius,
                          int *restrict error)
{
  if (circle)
    {
      *error = 0;
      circle->radius = radius;
    }
  else
    {
      LOG_ERROR("%s: empty circle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the_dwg_entity_CIRCLE::thickness, DXF 39 (the cylinder height)
\code Usage: double thickness = dwg_ent_circle_get_thickness(circle, &error);
\endcode
\param[in]  circle  dwg_ent_circle*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_circle_get_thickness(const dwg_ent_circle *restrict circle,
                             int *restrict error)
{
  if (circle)
    {
      *error = 0;
      return circle->thickness;
    }
  else
    {
      LOG_ERROR("%s: empty circle", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the_dwg_entity_CIRCLE::thickness, DXF 39 (the cylinder height)
\code Usage: dwg_ent_circle_set_thickness(circle, thickness, &error);
\endcode
\param[in,out] circle dwg_ent_circle*
\param[in]  thickness double
\param[out] error     int*, set to 0 for ok, 1 on error
*/
void
dwg_ent_circle_set_thickness(dwg_ent_circle *restrict circle, const BITCODE_BD thickness,
                             int *restrict error)
{
  if (circle)
    {
      *error = 0;
      circle->thickness = thickness;
    }
  else
    {
      LOG_ERROR("%s: empty circle", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the_dwg_entity_CIRCLE::extrusion vector, DXF 210
\code Usage: dwg_ent_circle_set_extrusion(circle, &vector, &error);
\endcode
\param[in,out] circle dwg_ent_circle*
\param[out] vector  dwg_point_3d*
\param[out] error   int*, set to 0 for ok, 1 on error
*/
void
dwg_ent_circle_set_extrusion(dwg_ent_circle *restrict circle,
                             const dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (circle && vector)
    {
      *error = 0;
      circle->extrusion.x = vector->x;
      circle->extrusion.y = vector->y;
      circle->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR("%s: empty circle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_CIRCLE::extrusion vector, DXF 210
\code Usage: dwg_ent_circle_get_extrusion(circle, &point, &error);
\endcode
\param[in]  circle dwg_ent_circle*
\param[out] vector dwg_point_3d*
\param[out] error  int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_circle_get_extrusion(const dwg_ent_circle *restrict circle,
                             dwg_point_3d *restrict vector,
                             int *restrict error)
{
  if (circle && vector)
    {
      *error = 0;
      vector->x = circle->extrusion.x;
      vector->y = circle->extrusion.y;
      vector->z = circle->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty circle", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR LINE ENTITY                      *
 ********************************************************************/

/** Returns the _dwg_entity_LINE::start point, DXF 10.
\code Usage: dwg_ent_line_get_start_point(line, &point, &error);
\endcode
\param[in]  line    dwg_ent_line*
\param[out] point   dwg_point_2d*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_line_get_start_point(const dwg_ent_line *restrict line,
                             dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (line)
    {
      *error = 0;
      point->x = line->start.x;
      point->y = line->start.y;
      point->z = line->start.z;
    }
  else
    {
      LOG_ERROR("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_LINE::start point, DXF 10.
\code Usage: dwg_ent_line_set_start_point(line, point, &error);
\endcode
\param[in,out] line dwg_ent_line*
\param[out] point   dwg_point_2d*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_line_set_start_point(dwg_ent_line *restrict line,
                             const dwg_point_3d *restrict point,
                             int *restrict error)
{
  if (line)
    {
      *error = 0;
      line->start.x = point->x;
      line->start.y = point->y;
      line->start.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_LINE::end point, DXF 11.
\code Usage: dwg_ent_line_get_end_point(line, &point, &error);
\endcode
\param[in]  line    dwg_ent_line*
\param[out] point   dwg_point_2d*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_line_get_end_point(const dwg_ent_line *restrict line,
                           dwg_point_3d *restrict point,
                           int *restrict error)
{
  if (line)
    {
      *error = 0;
      point->x = line->end.x;
      point->y = line->end.y;
      point->z = line->end.z;
    }
  else
    {
      LOG_ERROR("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_LINE::end point, DXF 11.
\code Usage: dwg_ent_line_set_start_end(line, &end_points, &error);
\endcode
\param[in,out] line dwg_ent_line*
\param[out] point   dwg_point_2d*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_line_set_end_point(dwg_ent_line *restrict line, const dwg_point_3d *restrict point,
                           int *restrict error)
{
  if (line)
    {
      *error = 0;
      line->end.x = point->x;
      line->end.y = point->y;
      line->end.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_LINE::thickness, DXF 39.
\code Usage: double thickness = dwg_ent_line_get_thickness(line, &error);
\endcode
\param[in]  line    dwg_ent_line*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_line_get_thickness(const dwg_ent_line *restrict line, int *restrict error)
{
  if (line)
    {
      *error = 0;
      return line->thickness;
    }
  else
    {
      LOG_ERROR("%s: empty line", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the _dwg_entity_LINE::thickness, DXF 39.
\code Usage: dwg_ent_line_get_thickness(line, thickness, &error);
\endcode
\param[in,out] line   dwg_ent_line*
\param[out] thickness double
\param[out] error     int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_line_set_thickness(dwg_ent_line *restrict line, const BITCODE_BD thickness,
                           int *restrict error)
{
  if (line)
    {
      *error = 0;
      line->thickness = thickness;
    }
  else
    {
      LOG_ERROR("%s: empty line", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_LINE::extrusion vector, DXF 210.
\code Usage: dwg_ent_line_get_extrusion(line, &vector, &error);
\endcode
\param[in]  line    dwg_ent_line*
\param[out] vector  dwg_point_3d*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_line_get_extrusion(const dwg_ent_line *restrict line, dwg_point_3d *restrict vector,
                           int *restrict error)
{
  if (line && vector)
    {
      *error = 0;
      vector->x = line->extrusion.x;
      vector->y = line->extrusion.y;
      vector->z = line->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty vector or line", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_LINE::extrusion vector, DXF 210.
\code Usage: dwg_ent_line_set_extrusion(line, vector, &error);
\endcode
\param[in,out] line  dwg_ent_line*
\param[out] vector   dwg_point_3d*
\param[out] error    int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_line_set_extrusion(dwg_ent_line *restrict line, const dwg_point_3d *restrict vector,
                           int *restrict error)
{
  if (line && vector)
    {
      *error = 0;
      line->extrusion.x = vector->x;
      line->extrusion.y = vector->y;
      line->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR("%s: empty vector or line", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR ARC ENTITY                       *
 ********************************************************************/

/** Returns the _dwg_entity_ARC::center, DXF 10.
\code Usage: dwg_ent_arc_get_center(arc, &point, &error);
\endcode
\param[in]   arc    dwg_ent_arc*
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_get_center(const dwg_ent_arc *restrict arc, dwg_point_3d *point,
                       int *restrict error)
{
  if (arc && point)
    {
      *error = 0;
      point->x = arc->center.x;
      point->y = arc->center.y;
      point->z = arc->center.z;
    }
  else
    {
      LOG_ERROR("%s: empty arc or point", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ARC::center, DXF 10.
\code Usage: dwg_ent_arc_set_center(arc, &point, &error);
\endcode
\param[in,out] arc     dwg_ent_arc*
\param[out]    center  dwg_point_3d*
\param[out]    error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_set_center(dwg_ent_arc *restrict arc, const dwg_point_3d *restrict point,
                       int *restrict error)
{
  if (arc && point)
    {
      *error = 0;
      arc->center.x = point->x;
      arc->center.y = point->y;
      arc->center.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty arc or point", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::radius, DXF 40.
\code Usage: double radius = dwg_ent_arc_get_radius(arc, &error);
\endcode
\param[in]   arc    dwg_ent_arc*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_arc_get_radius(const dwg_ent_arc *restrict arc, int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->radius;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the _dwg_entity_ARC::radius, DXF 40.
\code Usage: dwg_ent_arc_set_radius(arc, radius, &error);
\endcode
\param[in,out] arc     dwg_ent_arc*
\param[out]    radius  double
\param[out]    error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_set_radius(dwg_ent_arc *restrict arc, const BITCODE_BD radius,
                       int *restrict error)
{
  if (arc)
    {
      *error = 0;
      arc->radius = radius;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::thickness, DXF 39.
\code Usage: double thickness = dwg_ent_arc_get_thickness(arc, &error);
\endcode
\param[in]   arc    dwg_ent_arc*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_arc_get_thickness(const dwg_ent_arc *restrict arc, int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->thickness;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the _dwg_entity_ARC::thickness, DXF 39.
\code Usage: dwg_ent_arc_get_thickness(arc, thickness, &error);
\endcode
\param[in,out] arc       dwg_ent_arc*
\param[out]    thickness double
\param[out]    error     int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_set_thickness(dwg_ent_arc *restrict arc, const BITCODE_BD thickness,
                          int *restrict error)
{
  if (arc)
    {
      *error = 0;
      arc->thickness = thickness;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::extrusion vector, DXF 210.
\code Usage: dwg_ent_arc_get_extrusion(arc, &vector, &error);
\endcode
\param[in]     arc      dwg_ent_arc*
\param[out]    vector   dwg_point_3d*
\param[out]    error    int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_get_extrusion(const dwg_ent_arc *restrict arc, dwg_point_3d *restrict vector,
                          int *restrict error)
{
  if (arc && vector)
    {
      *error = 0;
      vector->x = arc->extrusion.x;
      vector->y = arc->extrusion.y;
      vector->z = arc->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty arc or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the _dwg_entity_ARC::extrusion vector, DXF 210.
\code Usage: dwg_ent_arc_set_extrusion(arc, vector, &error);
\endcode
\param[in,out] arc      dwg_ent_arc*
\param[in]     vector   dwg_point_3d*
\param[out]    error    int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_set_extrusion(dwg_ent_arc *restrict arc, const dwg_point_3d *restrict vector,
                          int *restrict error)
{
  if (arc && vector)
    {
      *error = 0;
      arc->extrusion.x = vector->x;
      arc->extrusion.y = vector->y;
      arc->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR("%s: empty arc or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::start_angle, DXF 50, in radians.
\code Usage: double start_angle = dwg_ent_arc_get_start_angle(line, &error);
\endcode
\param[in]  arc     dwg_ent_arc*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_arc_get_start_angle(const dwg_ent_arc *restrict arc, int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->start_angle;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the _dwg_entity_ARC::start_angle, DXF 50.
\code Usage: dwg_ent_arc_set_start_angle(arc, angle, &error);
\endcode
\param[in,out] arc      dwg_ent_arc*
\param[in]     angle    double (in radians)
\param[out]    error    int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_set_start_angle(dwg_ent_arc *restrict arc, const BITCODE_BD angle,
                            int *restrict error)
{
  if (arc)
    {
      *error = 0;
      //TODO: normalize to PI? There are some DWG's with 2*PI or 4.623770
      arc->start_angle = angle;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the _dwg_entity_ARC::end_angle, DXF 51, in radians.
\code Usage: double end_angle = dwg_ent_arc_get_end_angle(arc, &error);
\endcode
\param[in]  arc     dwg_ent_arc*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_arc_get_end_angle(const dwg_ent_arc *restrict arc, int *restrict error)
{
  if (arc)
    {
      *error = 0;
      return arc->end_angle;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the _dwg_entity_ARC::end_angle, DXF 51.
\code Usage: dwg_ent_arc_set_end_angle(arc, angle, &error);
\endcode
\param[in,out] arc      dwg_ent_arc*
\param[in]     angle    double (in radians)
\param[out]    error    int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_arc_set_end_angle(dwg_ent_arc *restrict arc, const BITCODE_BD angle,
                          int *restrict error)
{
  if (arc)
    {
      *error = 0;
      //TODO: normalize to PI?
      arc->end_angle = angle;
    }
  else
    {
      LOG_ERROR("%s: empty arc", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR ELLIPSE ENTITY                    *
 ********************************************************************/

/** returns ellipse center.
\code Usage: dwg_ent_ellipse_get_center(ellipse, &center, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_get_center(const dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error)
{
  if (ellipse && point)
    {
      *error = 0;
      point->x = ellipse->center.x;
      point->y = ellipse->center.y;
      point->z = ellipse->center.z;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/** sets ellipse center
\code Usage: dwg_ent_ellipse_set_center(ellipse, &center &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error)
{
  if (ellipse && point)
    {
      *error = 0;
      ellipse->center.x = point->x;
      ellipse->center.y = point->y;
      ellipse->center.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/** returns ellipse sm axis.
\code Usage: dwg_ent_ellipse_get_sm_axis(ellipse, &point, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_get_sm_axis(const dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error)
{
  if (ellipse && point)
    {
      *error = 0;
      point->x = ellipse->sm_axis.x;
      point->y = ellipse->sm_axis.y;
      point->z = ellipse->sm_axis.z;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/** sets ellipse sm axis.
\code Usage: dwg_ent_ellipse_set_sm_axis(ellipse, &point, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error)
{
  if (ellipse && point)
    {
      *error = 0;
      ellipse->sm_axis.x = point->x;
      ellipse->sm_axis.y = point->y;
      ellipse->sm_axis.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse or point", __FUNCTION__)
      *error = 1;
    }
}

/** returns ellipse extrusion.
\code Usage: dwg_ent_ellipse_get_extrusion(ellipse, &ext_points, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_get_extrusion(const dwg_ent_ellipse *ellipse, dwg_point_3d *restrict vector,
                              int *error)
{
  if (ellipse && vector)
    {
      *error = 0;
      vector->x = ellipse->extrusion.x;
      vector->y = ellipse->extrusion.y;
      vector->z = ellipse->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse or vector", __FUNCTION__)
      *error = 1;
    }
}

/** sets ellipse extrusion.
\code Usage: dwg_ent_ellipse_set_extrusion(ellipse, &ext_points, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *restrict vector,
                              int *error)
{
  if (ellipse && vector)
    {
      *error = 0;
      ellipse->extrusion.x = vector->x;
      ellipse->extrusion.y = vector->y;
      ellipse->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns ellipse axis ratio
\code Usage: double axis_ratio = dwg_ent_ellipse_get_axis_ratio(ellipse, &error);
\endcode
    \param 1 dwg_ent_ellipse
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_ellipse_get_axis_ratio(const dwg_ent_ellipse *ellipse, int *error)
{
  if (ellipse)
    {
      *error = 0;
      return ellipse->axis_ratio;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty ellipse", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets ellipse axis ratio.
\code Usage: dwg_ent_ellipse_set_axis_ratio(ellipse, axis_ratio, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, BITCODE_BD ratio,
                               int *error)
{
  if (ellipse)
    {
      *error = 0;
      ellipse->axis_ratio = ratio;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse", __FUNCTION__)
      *error = 1;
    }
}

/** Returns ellipse start angle.
\code Usage: double start_angle = dwg_ent_ellipse_get_start_angle(ellipse, &error);
\endcode
    \param 1 dwg_ent_ellipse
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_ellipse_get_start_angle(const dwg_ent_ellipse *ellipse, int *error)
{
  if (ellipse)
    {
      *error = 0;
      return ellipse->start_angle;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets ellipse start angle.
\code Usage: dwg_ent_ellipse_set_start_angle(ellipse, start_angle, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, BITCODE_BD start_angle,
                                int *error)
{
  if (ellipse)
    {
      *error = 0;
      ellipse->start_angle = start_angle;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse", __FUNCTION__)
      *error = 1;
    }
}

/** Returns ellipse end angle.
\code Usage: double end_angle = dwg_ent_ellipse_get_end_angle(ellipse, &error);
\endcode
    \param 1 dwg_ent_ellipse
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_ellipse_get_end_angle(const dwg_ent_ellipse *ellipse, int *error)
{
  if (ellipse)
    {
      *error = 0;
      return ellipse->end_angle;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets ellipse end angle.
\code Usage: dwg_ent_ellipse_set_end_angle(ellipse, end_angle, &error);
\endcode
    \param 1 dwg_ent_ellipse
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, BITCODE_BD end_angle,
                              int *error)
{
  if (ellipse)
    {
      *error = 0;
      ellipse->end_angle = end_angle;
    }
  else
    {
      LOG_ERROR("%s: empty ellipse", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                    FUNCTIONS FOR TEXT ENTITY                      *
 ********************************************************************/

/** Sets text value of a text entity (utf-8 encoded).
\code Usage: dwg_ent_text_set_text(text, "Hello world", &error);
\endcode
    \param 1 dwg_ent_text
    \param 2 utf-8 string ( char * )
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_set_text(dwg_ent_text *ent, char * text_value, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        ent->text_value = (char*)bit_utf8_to_TU(text_value);
      else
        ent->text_value = text_value;
    }
  else
    {
      LOG_ERROR("%s: empty ent", __FUNCTION__)
      *error = 1;
    }
}

/** This returns the text value of a text entity (utf-8 encoded).
\code Usage: dwg_ent_text_get_text(text, &error);
\endcode
    \param 1 dwg_ent_text
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_ent_text_get_text(const dwg_ent_text *ent, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)ent->text_value);
      else
        return ent->text_value;
    }
  else
    {
      LOG_ERROR("%s: empty ent", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** This returns the insertion point of a text entity into second argument.
\code Usage: dwg_ent_text_get_insertion_point(text, &point, &error);
\endcode
    \param 1 dwg_ent_text
    \param 2 dwg_2d_point
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_get_insertion_point(const dwg_ent_text *text, dwg_point_2d *point,
                                 int *error)
{
  if (text && point)
    {
      *error = 0;
      point->x = text->insertion_pt.x;
      point->y = text->insertion_pt.y;
    }
  else
    {
      LOG_ERROR("%s: empty text or point", __FUNCTION__)
      *error = 1;
    }
}

/// Sets insertion point
/** Usage :- dwg_ent_text_set_insertion_point(text, &point, &error)
    \param 1 dwg_ent_text
    \param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point,
                                 int *error)
{
  if (text && point)
    {
      *error = 0;
      text->insertion_pt.x = point->x;
      text->insertion_pt.y = point->y;
    }
  else
    {
      LOG_ERROR("%s: empty text or point", __FUNCTION__)
      *error = 1;
    }
}

/// Returns height of a text.
/** Usage :- double height = dwg_ent_text_get_height(text);
    \param 1 dwg_ent_text
\param[out] error   int*, is set to 0 for ok, 1 on error
    DXF 40
*/
BITCODE_BD
dwg_ent_text_get_height(const dwg_ent_text *text, int *error)
{
  if (text)
    {
      *error = 0;
      return text->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty text", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets height of Text
/** Usage :- dwg_ent_text_set_height(text, 100, &error);
    \param 1 dwg_ent_text
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
    DXF 40
*/
void
dwg_ent_text_set_height(dwg_ent_text *text, BITCODE_BD height, int *error)
{
  if (text)
    {
      *error = 0;
      text->height = height;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/**
   \def dwg_ent_text_get_extrusion(text, &point, &error);
   \brief Returns extrusion value
   \param 1 dwg_ent_text
   \param 2 dwg_point_2d
   \param 3 int ptr &error
   DXF 210
*/
void
dwg_ent_text_get_extrusion(const dwg_ent_text *text, dwg_point_3d *vector,
                           int *error)
{
  if (text && vector)
    {
      *error = 0;
      vector->x = text->extrusion.x;
      vector->y = text->extrusion.y;
      vector->z = text->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty text or vector", __FUNCTION__)
      *error = 1;
    }
}

/// Sets the text extrusion value.
/// DXF param 210
/** Usage :- dwg_ent_text_set_extrusion(text, &point, &error);
    \param 1 dwg_ent_text
    \param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_point_3d *vector,
                           int *error)
{
  if (text && vector)
    {
      *error = 0;
      text->extrusion.x = vector->x;
      text->extrusion.y = vector->y;
      text->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR("%s: empty text or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the thickness of a text entity, DXF 39.
\code Usage: dwg_ent_text_get_thickness(text, &error);
\endcode
    \param 1 dwg_ent_text
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_text_get_thickness(const dwg_ent_text *text, int *error)
{
  if (text)
    {
      *error = 0;
      return text->thickness;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets the thickness of text.
/// DXF param 39
/** Usage :- dwg_ent_text_set_thickness(text, 50, &error);
    \param 1 dwg_ent_text
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_set_thickness(dwg_ent_text *text, BITCODE_BD thickness, int *error)
{
  if (text)
    {
      *error = 0;
      text->thickness = thickness;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/// Returns the rotation angle of a text entity.
/// DXF param 50
/** Usage :- double rot_ang = dwg_ent_text_get_rotation(text, &error);
    \param 1 dwg_ent_text
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_text_get_rotation(const dwg_ent_text *text, int *error)
{
  if (text)
    {
      *error = 0;
      return text->rotation;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the rotation angle of a text entity, DXF 50.
\code Usage: dwg_ent_text_set_rotation(text, angle, &error);
\endcode
    \param 1 dwg_ent_text
    \param 2 BITCODE_BD
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_set_rotation(dwg_ent_text *text, BITCODE_BD angle, int *error)
{
  if (text)
    {
      *error = 0;
      text->rotation = angle;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the vertical alignment of a text entity, DXF 73.
\code Usage: short align = dwg_ent_text_get_vert_align(text, &error);
\endcode
    \param 1 dwg_ent_text
\param[out] error   int*, is set to 0 for ok, 1 on error
    \return short(1 = bottom, 2 = middle, 3 = top)
*/
BITCODE_BS
dwg_ent_text_get_vert_align(const dwg_ent_text *text, int *error)
{
  if (text)
    {
      *error = 0;
      return text->vert_alignment;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
      return -1;
    }
}

/** Sets the vertical alignment of a text entity, DXF 73.
\code Usage: dwg_ent_text_set_vert_align(text, angle, &error);
\endcode
    \param 1 dwg_ent_text
    \param 2 short (1 = bottom, 2 = middle, 3 = top)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_set_vert_align(dwg_ent_text *text, BITCODE_BS alignment, int *error)
{
  if (text)
    {
      *error = 0;
      //TODO: validation 1-3
      text->vert_alignment = alignment;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the horizontal alignment of a text entity, DXF 72.
\code Usage: short align = dwg_ent_text_get_horiz_align(text, &error);
\endcode
    \param 1 dwg_ent_text
\param[out] error   int*, is set to 0 for ok, 1 on error
    \return (1 = left, 2 = center, 3 = right)
*/
BITCODE_BS
dwg_ent_text_get_horiz_align(const dwg_ent_text *text, int *error)
{
  if (text)
    {
      *error = 0;
      return text->horiz_alignment;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets the horizontal alignment of a text entity, DXF 72.
\code Usage: dwg_ent_text_set_horiz_align(text, angle, &error);
\endcode
    \param 1 dwg_ent_text
    \param 2 short (1 = left, 2 = center, 3 = right)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_text_set_horiz_align(dwg_ent_text *text, BITCODE_BS alignment, int *error)
{
  if (text)
    {
      *error = 0;
      //TODO: validation 1-3
      text->horiz_alignment = alignment;
    }
  else
    {
      LOG_ERROR("%s: empty text", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR ATTRIB ENTITY                     *
 ********************************************************************/

/** Sets text value of a attrib entity (utf-8 encoded).
\code Usage: dwg_ent_attrib_set_text(attrib, "Hello world", &error);
\endcode
    \param 1 dwg_ent_attrib
    \param 2 utf-8 string (char *)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_text(dwg_ent_attrib *ent, char * text_value, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        ent->text_value = (char*)bit_utf8_to_TU(text_value);
      else
        ent->text_value = text_value;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the text value of a attrib entity (utf-8 encoded).
\code Usage: char * text_val = dwg_ent_attrib_get_text(attrib, &error);
\endcode
    \param 1 dwg_ent_attrib
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_ent_attrib_get_text(const dwg_ent_attrib *ent, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)ent->text_value);
      else
        return ent->text_value;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the insertion point of a attrib entity.
\code Usage: dwg_ent_attrib_get_insertion_point(attrib, &point, &error);
\endcode
    \param 1 dwg_ent_attrib
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_get_insertion_point(const dwg_ent_attrib *attrib,
                                   dwg_point_2d *point, int *error)
{
  if (attrib && point)
    {
      *error = 0;
      point->x = attrib->insertion_pt.x;
      point->y = attrib->insertion_pt.y;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the insertion point of a attrib entity.
\code Usage: dwg_ent_attrib_set_insertion_point(attrib, &point, &error)
\endcode
    \param 1 dwg_ent_attrib
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib,
                                   dwg_point_2d *point, int *error)
{
  if (attrib && point)
    {
      *error = 0;
      attrib->insertion_pt.x = point->x;
      attrib->insertion_pt.y = point->y;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** This returns the height of a attrib entity.
\code Usage: double height = dwg_ent_attrib_get_height(attrib, &error);
\endcode
    \param 1 dwg_ent_attrib
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_attrib_get_height(const dwg_ent_attrib *attrib, int *error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->height;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** This sets height of a attrib entity.
\code Usage: dwg_ent_attrib_set_height(attrib, 100, &error);
\endcode
    \param 1 dwg_ent_attrib
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_height(dwg_ent_attrib *attrib, BITCODE_BD height, int *error)
{
  if (attrib)
    {
      *error = 0;
      attrib->height = height;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the extrusion values of attrib.
\code Usage: dwg_ent_attrib_get_extrusion(attrib, &point, &error);
\endcode
    \param 1 dwg_ent_attrib
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_get_extrusion(const dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error)
{
  if (attrib && vector)
    {
      *error = 0;
      vector->x = attrib->extrusion.x;
      vector->y = attrib->extrusion.y;
      vector->z = attrib->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty attrib or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the attrib extrusion values equal to values of second argument.
\code Usage: dwg_ent_attrib_set_extrusion(attrib, &point, &error);
\endcode
    \param 1 dwg_ent_attrib
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error)
{
  if (attrib && vector)
    {
      *error = 0;
      attrib->extrusion.x = vector->x;
      attrib->extrusion.y = vector->y;
      attrib->extrusion.z = vector->z;
    }
  else
    {
      LOG_ERROR("%s: empty attrib or vector", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the thickness of a attrib entity.
\code Usage: double thick = dwg_ent_attrib_get_thickness(attrib, &error);
\endcode
    \param 1 dwg_ent_attrib
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_attrib_get_thickness(const dwg_ent_attrib *attrib, int *error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      return bit_nan();
    }
}

/** This sets the thickness of a attrib entity.
\code Usage: dwg_ent_attrib_set_thickness(attrib, thick, &error);
\endcode
    \param 1 dwg_ent_attrib
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, BITCODE_BD thickness,
                             int *error)
{
  if (attrib)
    {
      *error = 0;
      attrib->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
    }
}

/** Returns the rotation angle of a attrib entity.
\code Usage: double angle = dwg_ent_attrib_get_rot_angle(attrib, &error);
\endcode
    \param 1 dwg_ent_attrib
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_attrib_get_rot_angle(const dwg_ent_attrib *attrib, int *error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->rotation;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// This sets the rotation angle of a attrib entity.
/** Usage :- dwg_ent_attrib_set_rot_angle(attrib, angle, &error);
    \param 1 dwg_ent_attrib
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *attrib, BITCODE_BD angle, int *error)
{
  if (attrib)
    {
      *error = 0;
      attrib->rotation = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** This returns the vertical alignment of a attrib entity.
\code Usage: short vert_align = dwg_ent_attrib_get_vert_align(attrib, &error);
\endcode
    \param 1 dwg_ent_attrib
\param[out] error   int*, is set to 0 for ok, 1 on error
    \return short(1 = bottom, 2 = middle, 3 = top)
*/
BITCODE_BS
dwg_ent_attrib_get_vert_align(const dwg_ent_attrib *attrib, int *error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->vert_alignment;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
      return -1;
    }
}

/// This sets the vertical alignment of a attrib entity.
/** Usage :- dwg_ent_attrib_set_vert_align(attrib, angle, &error);
    \param 1 dwg_ent_attrib
    \param 2 short (1 = bottom, 2 = middle, 3 = top)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, BITCODE_BS alignment,
                              int *error)
{
  if (attrib)
    {
      *error = 0;
      attrib->vert_alignment = alignment;
    }
  else
    {
      LOG_ERROR("%s: empty attrib", __FUNCTION__)
      *error = 1;
    }
}

/** This returns the horizontal alignment of a attrib entity.
\code Usage: short horiz_align =  dwg_ent_attrib_get_horiz_align(attrib, &error);
\endcode
    \param 1 dwg_ent_attrib
\param[out] error   int*, is set to 0 for ok, 1 on error
    \return (1 = left, 2 = center, 3 = right)
*/
BITCODE_BS
dwg_ent_attrib_get_horiz_align(const dwg_ent_attrib *attrib, int *error)
{
  if (attrib)
    {
      *error = 0;
      return attrib->horiz_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return -1;
    }
}

/// This sets the horizontal alignment of a attrib entity.
/** Usage :- dwg_ent_attrib_set_horiz_align(attrib, angle, &error);
    \param 1 dwg_ent_attrib
    \param 2 short (1 = left, 2 = center, 3 = right)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, BITCODE_BS alignment,
                               int *error)
{
  if (attrib)
    {
      *error = 0;
      attrib->horiz_alignment = alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
 *                   FUNCTIONS FOR ATTDEF ENTITY                     *
 ********************************************************************/

/// This sets the default value of an attdef entity (utf-8 encoded).
/** Usage :- dwg_ent_attdef_set_text(attdef, "Hello world", &error);
    \param 1 dwg_ent_attdef
    \param 2 utf-8 string (char *)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_text(dwg_ent_attdef *ent, char * default_value,
                        int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        ent->default_value = (char*)bit_utf8_to_TU(default_value);
      else
        ent->default_value = default_value;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** This returns the default value of an attdef entity (utf-8 encoded).
\code Usage: char * text = dwg_ent_attdef_get_text(attdef, &error);
\endcode
    \param 1 dwg_ent_attdef
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_ent_attdef_get_text(const dwg_ent_attdef *ent, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)ent->default_value);
      else
        return ent->default_value;
    }
  else
    {
      LOG_ERROR("%s: empty attdef", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/// This returns the insertion point of an attdef entity.
/** Usage :- dwg_ent_attdef_get_insertion_point(attdef, &point, &error);
    \param 1 dwg_ent_attdef
    \param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_get_insertion_point(const dwg_ent_attdef *attdef,
                                   dwg_point_2d *point, int *error)
{
  if (attdef && point)
    {
      *error = 0;
      point->x = attdef->insertion_pt.x;
      point->y = attdef->insertion_pt.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This sets the insertion point of a attdef entity.
/** Usage :- dwg_ent_attdef_set_insertion_point(attdef, &point, &error)
    \param 1 dwg_ent_attdef
    \param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef,
                                   dwg_point_2d *point, int *error)
{
  if (attdef && point)
    {
      *error = 0;
      attdef->insertion_pt.x = point->x;
      attdef->insertion_pt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the height of a attdef entity.
/** Usage :- dwg_ent_attdef_get_height(attdef, &error);
    \param 1 dwg_ent_attdef
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_attdef_get_height(const dwg_ent_attdef *attdef, int *error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets height of a attdef entity.
/** Usage :- dwg_ent_attdef_set_height(attdef, 100, &error);
    \param 1 dwg_ent_attdef
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_height(dwg_ent_attdef *attdef, BITCODE_BD height, int *error)
{
  if (attdef)
    {
      *error = 0;
      attdef->height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the extrusion values of attdef.
\code Usage: dwg_ent_attdef_get_extrusion(attdef, &point, &error);
\endcode
    \param 1 dwg_ent_attdef
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_get_extrusion(const dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error)
{
  if (attdef && vector)
    {
      *error = 0;
      vector->x = attdef->extrusion.x;
      vector->y = attdef->extrusion.y;
      vector->z = attdef->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the attdef extrusion values.
/** Usage :- dwg_ent_attdef_set_extrusion(attdef, &point, &error);
    \param 1 dwg_ent_attdef
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error)
{
  if (attdef && vector)
    {
      *error = 0;
      attdef->extrusion.x = vector->x;
      attdef->extrusion.y = vector->y;
      attdef->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the thickness of a attdef entity.
/** Usage :- double thickness = dwg_ent_attdef_get_thickness(attdef, &error);
    \param 1 dwg_ent_attdef
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_attdef_get_thickness(const dwg_ent_attdef *attdef, int *error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets the thickness of a attdef entity.
/** Usage :- dwg_ent_attdef_set_thickness(attdef, thickness, &error);
    \param 1 dwg_ent_attdef
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, BITCODE_BD thickness,
                             int *error)
{
  if (attdef)
    {
      *error = 0;
      attdef->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the rotation angle of a attdef entity.
/** Usage :- double angle = dwg_ent_attdef_get_rot_angle(attdef, &error);
    \param 1 dwg_ent_attdef
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_attdef_get_rot_angle(const dwg_ent_attdef *attdef, int *error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets the rotation angle of a attdef entity.
/** Usage :- dwg_ent_attdef_set_rot_angle(attdef, angle, &error);
    \param 1 dwg_ent_attdef
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *attdef, BITCODE_BD angle, int *error)
{
  if (attdef)
    {
      *error = 0;
      attdef->rotation = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the vertical alignment of a attdef entity.
/** Usage :- short vert_align = dwg_ent_attdef_get_vert_align(attdef, &error);
    \param 1 dwg_ent_attdef
\param[out] error   int*, is set to 0 for ok, 1 on error
    \return (1 = bottom, 2 = middle, 3 = top)
*/
BITCODE_BS
dwg_ent_attdef_get_vert_align(const dwg_ent_attdef *attdef, int *error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->vert_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return -1;
    }
}

/// This sets the vertical alignment of a attdef entity.
/** Usage :- dwg_ent_attdef_set_vert_align(attdef, angle, &error);
    \param 1 dwg_ent_attdef
    \param 2 short (1 = bottom, 2 = middle, 3 = top)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, BITCODE_BS alignment,
                              int *error)
{
  if (attdef)
    {
      *error = 0;
      attdef->vert_alignment = alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the horizontal alignment of a attdef entity.
/** Usage :- short horiz_align = dwg_ent_attdef_get_horiz_align(attdef, &error);
    \param 1 dwg_ent_attdef
\param[out] error   int*, is set to 0 for ok, 1 on error
    \return (1 = left, 2 = center, 3 = right)
*/
BITCODE_BS
dwg_ent_attdef_get_horiz_align(const dwg_ent_attdef *attdef, int *error)
{
  if (attdef)
    {
      *error = 0;
      return attdef->horiz_alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return -1;
    }
}

/// This sets the horizontal alignment of a attdef entity.
/** Usage :- dwg_ent_attdef_set_horiz_align(attdef, alignment, &error);
    \param 1 dwg_ent_attdef
    \param 2 short (1 = left, 2 = center, 3 = right)
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, BITCODE_BS alignment,
                               int *error)
{
  if (attdef)
    {
      *error = 0;
      attdef->horiz_alignment = alignment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR POINT ENTITY                      *
********************************************************************/

/// Sets the point point values.
/** Usage :- dwg_ent_point_set_point(point, &retpoint, &error);
\param 1 dwg_ent_point
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_point_set_point(dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error)
{
  if (point && retpoint)
    {
      *error = 0;
      point->x = retpoint->x;
      point->y = retpoint->y;
      point->z = retpoint->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the point values of point to second argument.
/** Usage :- dwg_ent_point_get_point(point, &retpoint, &error);
\param 1 dwg_ent_point
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_point_get_point(const dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error)
{
  if (point && retpoint)
    {
      *error = 0;
      retpoint->x = point->x;
      retpoint->y = point->y;
      retpoint->z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the thickness of a point entity.
/** Usage :- double thickness = dwg_ent_point_get_thickness(point, &error);
\param 1 dwg_ent_point
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_point_get_thickness(const dwg_ent_point *point, int *error)
{
  if (point)
    {
      *error = 0;
      return point->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets the thickness of a point entity.
/** Usage :- dwg_ent_point_set_thickness(point, thickness, &error);
\param 1 dwg_ent_point
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_point_set_thickness(dwg_ent_point *point, BITCODE_BD thickness,
                            int *error)
{
  if (point)
    {
      *error = 0;
      point->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the point extrusion values.
/** Usage :- dwg_ent_point_set_extrusion(point, &retpoint, &error);
\param 1 dwg_ent_point
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_point_set_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint,
                            int *error)
{
  if (point && retpoint)
    {
      *error = 0;
      point->extrusion.x = retpoint->x;
      point->extrusion.y = retpoint->y;
      point->extrusion.z = retpoint->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the extrusion values of point entity.
/** Usage :- dwg_ent_point_get_extrusion(point, &retpoint, &error);
\param 1 dwg_ent_point
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_point_get_extrusion(const dwg_ent_point *point, dwg_point_3d *retpoint,
                            int *error)
{
  if (point && retpoint)
    {
      *error = 0;
      retpoint->x = point->extrusion.x;
      retpoint->y = point->extrusion.y;
      retpoint->z = point->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR SOLID ENTITY                      *
********************************************************************/

/// This returns the thickness of a solid entity.
/** Usage :- double thickness = dwg_ent_solid_get_thickness(solid, &error);
\param 1 dwg_ent_solid
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
double
dwg_ent_solid_get_thickness(const dwg_ent_solid *solid, int *error)
{
  if (solid)
    {
      *error = 0;
      return solid->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets the thickness of a solid entity.
/** Usage :- dwg_ent_solid_set_thickness(solid, 2.0, &error);
\param 1 dwg_ent_solid
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_set_thickness(dwg_ent_solid *solid, BITCODE_BD thickness,
                            int *error)
{
  if (solid)
    {
      *error = 0;
      solid->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the elevation of a solid entity.
/** Usage :- double elev = dwg_ent_solid_get_elevation(solid, &error);
\param 1 dwg_ent_solid
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_solid_get_elevation(const dwg_ent_solid *solid, int *error)
{
  if (solid)
    {
      *error = 0;
      return solid->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets the elevation of a solid entity.
/** Usage :- dwg_ent_solid_set_elevation(solid, 20.0, &error);
\param 1 dwg_ent_solid
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_set_elevation(dwg_ent_solid *solid, BITCODE_BD elevation,
                            int *error)
{
  if (solid)
    {
      *error = 0;
      solid->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner1 values of solid.
/** Usage :- dwg_ent_solid_get_corner1(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_get_corner1(const dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      point->x = solid->corner1.x;
      point->y = solid->corner1.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the solid corner1 values.
/** Usage :- dwg_ent_solid_set_corner1(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_set_corner1(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      solid->corner1.x = point->x;
      solid->corner1.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner2 values.
/** Usage :- dwg_ent_solid_get_corner2(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_get_corner2(const dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      point->x = solid->corner2.x;
      point->y = solid->corner2.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the solid corner2.
/** Usage :- dwg_ent_solid_set_corner2(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_set_corner2(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      solid->corner2.x = point->x;
      solid->corner2.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner3 values.
/** Usage :- dwg_ent_solid_get_corner3(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_get_corner3(const dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      point->x = solid->corner3.x;
      point->y = solid->corner3.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the solid corner3 values.
/** Usage :- dwg_ent_solid_set_corner3(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_set_corner3(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      solid->corner3.x = point->x;
      solid->corner3.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner4 values.
/** Usage :- dwg_ent_solid_get_corner4(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_get_corner4(const dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      point->x = solid->corner4.x;
      point->y = solid->corner4.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the solid corner4 values.
/** Usage :- dwg_ent_solid_set_corner4(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_set_corner4(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error)
{
  if (solid && point)
    {
      *error = 0;
      solid->corner4.x = point->x;
      solid->corner4.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the extrusion values.
/** Usage :- dwg_ent_solid_get_extrusion(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_get_extrusion(const dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error)
{
  if (solid && vector)
    {
      *error = 0;
      vector->x = solid->extrusion.x;
      vector->y = solid->extrusion.y;
      vector->z = solid->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the solid extrusion values.
/** Usage :- dwg_ent_solid_set_extrusion(solid, &point, &error);
\param 1 dwg_ent_solid
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_solid_set_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error)
{
  if (solid && vector)
    {
      *error = 0;
      solid->extrusion.x = vector->x;
      solid->extrusion.y = vector->y;
      solid->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR BLOCk ENTITY                      *
********************************************************************/

/// sets name of the block entity (utf-8 encoded)
/** Usage :- dwg_ent_block_set_name(block, "block_name", &error);
\param 1 dwg_ent_block
\param 2 utf-8 char *
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_block_set_name(dwg_ent_block *ent, char * name, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        ent->name = (char*)bit_utf8_to_TU(name);
      else
        ent->name = name;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns name of the block entity.
/** Usage :- char * name = dwg_ent_block_get_name(block, &error);
\param 1 dwg_ent_block
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_ent_block_get_name(const dwg_ent_block *block, int *error)
{
  if (block)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)block->name);
      else
        return block->name;
    }
  else
    {
      LOG_ERROR("%s: empty block", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
*                    FUNCTIONS FOR RAY ENTITY                       *
********************************************************************/

/// Returns points of ray entity.
/** Usage :- dwg_ent_ray_get_point(ray, &point, &error);
\param 1 dwg_ent_ray
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ray_get_point(const dwg_ent_ray *ray, dwg_point_3d *point, int *error)
{
  if (ray && point)
    {
      *error = 0;
      point->x = ray->point.x;
      point->y = ray->point.y;
      point->z = ray->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets point of ray entity.
/** Usage :- dwg_ent_ray_set_point(ray, &point, &error);
\param 1 dwg_ent_ray
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ray_set_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error)
{
  if (ray && point)
    {
      *error = 0;
      ray->point.x = point->x;
      ray->point.y = point->y;
      ray->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns vector of ray entity.
/** Usage :- dwg_ent_ray_get_vector(ray, &point, &error);
\param 1 dwg_ent_ray
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ray_get_vector(const dwg_ent_ray *ray, dwg_point_3d *vector, int *error)
{
  if (ray && vector)
    {
      *error = 0;
      vector->x = ray->vector.x;
      vector->y = ray->vector.y;
      vector->z = ray->vector.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets vector of ray entity.
/** Usage :- dwg_ent_ray_set_vector(ray, &point, &error);
\param 1 dwg_ent_ray
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_ray_set_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error)
{
  if (ray && vector)
    {
      *error = 0;
      ray->vector.x = vector->x;
      ray->vector.y = vector->y;
      ray->vector.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR XLINE ENTITY                      *
********************************************************************/

/// Returns points of xline entity.
/** Usage :- dwg_ent_xline_get_point(xline, &point, &error);
\param 1 dwg_ent_xline
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_xline_get_point(const dwg_ent_xline *xline, dwg_point_3d *point, int *error)
{
  if (xline && point)
    {
      *error = 0;
      point->x = xline->point.x;
      point->y = xline->point.y;
      point->z = xline->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets point of xline entity.
/** Usage :- dwg_ent_xline_set_point(xline, &point, &error);
\param 1 dwg_ent_xline
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_xline_set_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error)
{
  if (xline && point)
    {
      *error = 0;
      xline->point.x = point->x;
      xline->point.y = point->y;
      xline->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns vector of xline entity.
/** Usage :- dwg_ent_xline_get_vector(xline, &point, &error);
\param 1 dwg_ent_xline
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_xline_get_vector(const dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error)
{
  if (xline && vector)
    {
      *error = 0;
      vector->x = xline->vector.x;
      vector->y = xline->vector.y;
      vector->z = xline->vector.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets vector of xline entity.
/** Usage :- dwg_ent_xline_set_vector(xline, &point, &error);
\param 1 dwg_ent_xline
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_xline_set_vector(dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error)
{
  if (xline && vector)
    {
      *error = 0;
      xline->vector.x = vector->x;
      xline->vector.y = vector->y;
      xline->vector.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR TRACE ENTITY                      *
********************************************************************/

/// This returns the thickness of a trace entity.
/** Usage :- double thickness = dwg_ent_trace_get_thickness(trace, &error);
\param 1 dwg_ent_trace
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_trace_get_thickness(const dwg_ent_trace *trace, int *error)
{
  if (trace)
    {
      *error = 0;
      return trace->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets the thickness of a trace entity.
/** Usage :- dwg_ent_trace_set_thickness(trace, 2.0, &error);
\param 1 dwg_ent_trace
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_set_thickness(dwg_ent_trace *trace, BITCODE_BD thickness,
                            int *error)
{
  if (trace)
    {
      *error = 0;
      trace->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// This returns the elevation of a trace entity.
/** Usage :- double elev = dwg_ent_trace_get_elevation(trace, &error);
\param 1 dwg_ent_trace
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_trace_get_elevation(const dwg_ent_trace *trace, int *error)
{
  if (trace)
    {
      *error = 0;
      return trace->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// This sets the elevation of a trace entity.
/** Usage :- dwg_ent_trace_set_elevation(trace, 20, &error);
\param 1 dwg_ent_trace
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_set_elevation(dwg_ent_trace *trace, BITCODE_BD elevation,
                            int *error)
{
  if (trace)
    {
      *error = 0;
      trace->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner1 values of trace.
/** Usage :- dwg_ent_trace_get_corner1(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_get_corner1(const dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      point->x = trace->corner1.x;
      point->y = trace->corner1.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner1 values of trace.
/** Usage :- dwg_ent_trace_set_corner1(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_set_corner1(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      trace->corner1.x = point->x;
      trace->corner1.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner2 values of trace.
/** Usage :- dwg_ent_trace_get_corner2(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_get_corner2(const dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      point->x = trace->corner2.x;
      point->y = trace->corner2.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner2 values of trace.
/** Usage :- dwg_ent_trace_set_corner2(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_set_corner2(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      trace->corner2.x = point->x;
      trace->corner2.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner3 values of trace.
/** Usage :- dwg_ent_trace_get_corner3(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_get_corner3(const dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      point->x = trace->corner3.x;
      point->y = trace->corner3.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner3 values of trace.
/** Usage :- dwg_ent_trace_set_corner3(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_set_corner3(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      trace->corner3.x = point->x;
      trace->corner3.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner4 values of trace.
/** Usage :- dwg_ent_trace_get_corner4(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_get_corner4(const dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      point->x = trace->corner4.x;
      point->y = trace->corner4.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner4 values of trace.
/** Usage :- dwg_ent_trace_set_corner4(trace, &point, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_set_corner4(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error)
{
  if (trace && point)
    {
      *error = 0;
      trace->corner4.x = point->x;
      trace->corner4.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the extrusion values of trace.
/** Usage :- dwg_ent_trace_get_extrusion(trace, &ext, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_get_extrusion(const dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error)
{
  if (trace && vector)
    {
      *error = 0;
      vector->x = trace->extrusion.x;
      vector->y = trace->extrusion.y;
      vector->z = trace->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the extrusion values of trace.
/** Usage :- dwg_ent_trace_set_extrusion(trace, &ext, &error);
\param 1 dwg_ent_trace
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_trace_set_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error)
{
  if (trace && vector)
    {
      *error = 0;
      trace->extrusion.x = vector->x;
      trace->extrusion.y = vector->y;
      trace->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                 FUNCTIONS FOR VERTEX_3D ENTITY                    *
********************************************************************/

/// Returns the flag of vertex_3d.
/** Usage :- char flag = dwg_ent_vertex_3d_get_flag(vert, &error);
\param 1 dwg_ent_vertex_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_vertex_3d_get_flag(const dwg_ent_vertex_3d *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets the flag of vertex_3d.
/** Usage :- dwg_ent_vertex_3d_set_flag(vert, flag, &error);
\param 1 dwg_ent_vertex_3d
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_3d_set_flag(dwg_ent_vertex_3d *vert, char flag, int *error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the point of vertex_3d.
\code Usage: dwg_ent_vertex_3d_get_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_3d
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_3d_get_point(const dwg_ent_vertex_3d *vert, dwg_point_3d *point,
                            int *error)
{
  if (vert && point)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the point of vertex_3d.
\code Usage: dwg_ent_vertex_3d_set_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_3d
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_3d_set_point(dwg_ent_vertex_3d *vert,
                            dwg_point_3d *point, int *error)
{
  if (vert && point)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*               FUNCTIONS FOR VERTEX_MESH ENTITY                    *
********************************************************************/

/// Returns the flag of vertex_mesh.
/** Usage :- char flag = dwg_ent_vertex_mesh_get_flag(vert, &error);
\param 1 dwg_ent_vertex_mesh
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_vertex_mesh_get_flag(const dwg_ent_vertex_mesh *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets the flag of vertex_mesh.
/** Usage :- dwg_ent_vertex_mesh_set_flag(vert, flag, &error);
\param 1 dwg_ent_vertex_mesh
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_mesh_set_flag(dwg_ent_vertex_mesh *vert, char flags,
                              int *error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the point of vertex_mesh.
\code Usage: dwg_ent_vertex_mesh_get_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_mesh
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_mesh_get_point(const dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error)
{
  if (vert && point)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the point of vertex_mesh.
\code Usage: dwg_ent_vertex_mesh_set_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_mesh
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_mesh_set_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error)
{
  if (vert && point)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
********************************************************************/

/// Returns the flag of vertex_pface.
/** Usage :- char flag = dwg_ent_vertex_pface_get_flag(vert, &error);
\param 1 dwg_ent_vertex_pface
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_vertex_pface_get_flag(const dwg_ent_vertex_pface *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets the flag of vertex_pface.
/** Usage :- dwg_ent_vertex_pface_set_flag(vert, flag, &error);
\param 1 dwg_ent_vertex_pface
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_pface_set_flag(dwg_ent_vertex_pface *vert, char flags,
                               int *error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the point of vertex_pface.
\code Usage: dwg_ent_vertex_pface_get_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_pface
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_pface_get_point(const dwg_ent_vertex_pface *vert,
                               dwg_point_3d *point, int *error)
{
  if (vert && point)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the point of vertex_pface.
\code Usage: dwg_ent_vertex_pface_set_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_pface
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_pface_set_point(dwg_ent_vertex_pface *vert,
                               dwg_point_3d *point, int *error)
{
  if (vert && point)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                 FUNCTIONS FOR VERTEX_2D ENTITY                    *
********************************************************************/

/** Returns the flag of vertex_2d
\code Usage: dwg_ent_vertex_2d_get_flag(vert, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_vertex_2d_get_flag(const dwg_ent_vertex_2d *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the flag of vertex_2d.
\code Usage: dwg_ent_vertex_2d_set_flag(vert, flag, &error);
\endcode
\param 1 dwg_ent_vertex_mesh
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_2d_set_flag(dwg_ent_vertex_2d *vert, char flags, int *error)
{
  if (vert)
    {
      *error = 0;
      vert->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the point of vertex_2d.
\code Usage: dwg_ent_vertex_2d_get_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_2d_get_point(const dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error)
{
  if (vert && point)
    {
      *error = 0;
      point->x = vert->point.x;
      point->y = vert->point.y;
      point->z = vert->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the point of vertex_2d.
\code Usage: dwg_ent_vertex_2d_set_point(vert, &point, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_2d_set_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error)
{
  if (vert && point)
    {
      *error = 0;
      vert->point.x = point->x;
      vert->point.y = point->y;
      vert->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the start width of vertex_2d.
\code Usage: double width = dwg_ent_vertex_2d_get_start_width(vert, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_vertex_2d_get_start_width(const dwg_ent_vertex_2d *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->start_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the start width of vertex_2d.
\code Usage: dwg_ent_vertex_2d_set_start_width(vert, 20, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_2d_set_start_width(dwg_ent_vertex_2d *vert, BITCODE_BD start_width,
                                  int *error)
{
  if (vert)
    {
      *error = 0;
      vert->start_width = start_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the end width of vertex_2d.
\code Usage: double width = dwg_ent_vertex_2d_get_end_width(vert, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_vertex_2d_get_end_width(const dwg_ent_vertex_2d *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->end_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the end width of vertex_2d.
\code Usage: dwg_ent_vertex_2d_set_end_width(vert, 20, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_2d_set_end_width(dwg_ent_vertex_2d *vert, BITCODE_BD end_width,
                                int *error)
{
  if (vert)
    {
      *error = 0;
      vert->end_width = end_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the bulge of vertex_2d.
\code Usage: double bulge = dwg_ent_vertex_2d_get_bulge(vert, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_vertex_2d_get_bulge(const dwg_ent_vertex_2d *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->bulge;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the bulge of vertex_2d.
\code Usage: dwg_ent_vertex_2d_set_bulge(vert, 20, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_2d_set_bulge(dwg_ent_vertex_2d *vert, BITCODE_BD bulge, int *error)
{
  if (vert)
    {
      *error = 0;
      vert->bulge = bulge;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the tangent_dir of vertex_2d.
\code Usage: double tangent_dir = dwg_ent_vertex_2d_get_tangent_dir(vert, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_vertex_2d_get_tangent_dir(const dwg_ent_vertex_2d *vert, int *error)
{
  if (vert)
    {
      *error = 0;
      return vert->tangent_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the tangent_dir of vertex_2d.
\code Usage: dwg_ent_vertex_2d_set_tangent_dir(vert, 20, &error);
\endcode
\param 1 dwg_ent_vertex_2d
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_vertex_2d_set_tangent_dir(dwg_ent_vertex_2d *vert, BITCODE_BD tangent_dir,
                                  int *error)
{
  if (vert)
    {
      *error = 0;
      vert->tangent_dir = tangent_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR INSERT ENTITY                     *
********************************************************************/

/** Returns the insertion point of insert.
\code Usage: dwg_ent_insert_get_ins_pt(insert, &point, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_insert_get_ins_pt(const dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error)
{
  if (insert && point)
    {
      *error = 0;
      point->x = insert->ins_pt.x;
      point->y = insert->ins_pt.y;
      point->z = insert->ins_pt.z;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the insertion point of insert.
\code Usage: dwg_ent_insert_set_ins_pt(insert, &point, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 dwg_point_3d
\param error[out]   int*
*/
void
dwg_ent_insert_set_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error)
{
  if (insert && point)
    {
      *error = 0;
      insert->ins_pt.x = point->x;
      insert->ins_pt.y = point->y;
      insert->ins_pt.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the scale flag of insert
\code Usage: double flag = dwg_ent_insert_get_flag(insert, &error);
\endcode
\param 1 dwg_ent_insert
\param error[out]   int*
*/
char
dwg_ent_insert_get_scale_flag(const dwg_ent_insert *insert, int *error)
{
  if (insert)
    {
      *error = 0;
      return insert->scale_flag;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the scale flag of insert.
\code Usage: dwg_ent_insert_set_scale_flag(insert, flag, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 char
\param error[out]   int*
*/
void
dwg_ent_insert_set_scale_flag(dwg_ent_insert *insert, char flags, int *error)
{
  if (insert)
    {
      *error = 0;
      insert->scale_flag = flags;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the scale of insert.
\code Usage: dwg_ent_insert_get_scale(insert, &point, &error);
\endcode
\param insert[in]   dwg_ent_insert*
\param scale3d[out] dwg_point_3d*
\param error[out]   int*
*/
void
dwg_ent_insert_get_scale(const dwg_ent_insert *insert, dwg_point_3d *scale3d,
                         int *error)
{
  if (insert && scale3d)
    {
      *error = 0;
      scale3d->x = insert->scale.x;
      scale3d->y = insert->scale.y;
      scale3d->z = insert->scale.z;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the scale of insert.
\code Usage: dwg_ent_insert_set_scale(insert, &point, &error);
\endcode
\param insert[in]   dwg_ent_insert*
\param scale3d[in]  dwg_point_3d*
\param error[out]   int*
*/
void
dwg_ent_insert_set_scale(dwg_ent_insert *insert, dwg_point_3d *scale3d,
                         int *error)
{
  if (insert && scale3d)
    {
      *error = 0;
      insert->scale.x = scale3d->x;
      insert->scale.y = scale3d->y;
      insert->scale.z = scale3d->z;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the rotation angle of insert.
\code Usage: double angle = dwg_ent_insert_get_rotation(insert, &error);
\endcode
\param 1 dwg_ent_insert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_insert_get_rotation(const dwg_ent_insert *insert, int *error)
{
  if (insert)
    {
      *error = 0;
      return insert->rotation;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the rotation angle of insert.
\code Usage: dwg_ent_insert_set_rotation(insert, angle, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_insert_set_rotation(dwg_ent_insert *insert, BITCODE_BD rot_ang,
                                  int *error)
{
  if (insert)
    {
      *error = 0;
      insert->rotation = rot_ang;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the extrusion of insert.
\code Usage: dwg_ent_insert_get_extrusion(insert, &point, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_insert_get_extrusion(const dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error)
{
  if (insert && point)
    {
      *error = 0;
      point->x = insert->extrusion.x;
      point->y = insert->extrusion.y;
      point->z = insert->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the extrusion of insert.
\code Usage: dwg_ent_insert_set_extrusion(insert, &point, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_insert_set_extrusion(dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error)
{
  if (insert && point)
    {
      *error = 0;
      insert->extrusion.x = point->x;
      insert->extrusion.y = point->y;
      insert->extrusion.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the has attribs of insert.
\code Usage: double attribs = dwg_ent_insert_has_attribs(intrest, &error);
\endcode
\param 1 dwg_ent_insert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_insert_has_attribs(dwg_ent_insert *insert, int *error)
{
  if (insert)
    {
      *error = 0;
      return insert->has_attribs;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Returns the owned object count of insert
\code Usage: BITCODE_BL count = dwg_ent_insert_get_num_owned(insert, &error);
\endcode
\param 1 dwg_ent_insert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_insert_get_num_owned(const dwg_ent_insert *insert, int *error)
{
  if (insert)
    {
      *error = 0;
      return insert->num_owned;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

/// FIXME needs to adjust handle array instead: add/delete
// TODO dwg_ent_insert_add_owned, dwg_ent_insert_delete_owned

/** Returns the ref handle.
\code Usage: dwg_obj_ref* handle = dwg_ent_insert_get_ref_handle(insert, &error);
\endcode
\param 1 dwg_ent_insert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_handle *
dwg_ent_insert_get_ref_handle(const dwg_ent_insert *insert, int *error)
{
  if (insert)
    {
      *error = 0;
      return &insert->block_header->handleref;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the abs reference.
\code Usage: BITCODE_BL ref = dwg_ent_insert_get_abs_ref(insert, &error);
\endcode
\param 1 dwg_ent_insert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_insert_get_abs_ref(const dwg_ent_insert *insert, int *error)
{
  if (insert)
    {
      *error = 0;
      return insert->block_header->absolute_ref;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
      return (BITCODE_BL)-1L;
    }
}

/*******************************************************************
*                  FUNCTIONS FOR MINSERT ENTITY                     *
********************************************************************/

/** Returns the insertion point of minsert.
\code Usage: dwg_ent_minsert_get_ins_pt(minsert, &point, &error);
\endcode
\param 1 dwg_ent_minsert
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_get_ins_pt(const dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error)
{
  if (minsert && point)
    {
      *error = 0;
      point->x = minsert->ins_pt.x;
      point->y = minsert->ins_pt.y;
      point->z = minsert->ins_pt.z;
    }
  else
    {
      LOG_ERROR("%s: empty insert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the insertion point of minsert.
\code Usage: dwg_ent_minsert_set_ins_pt(minsert, &point, &error);
\endcode
\param 1 dwg_ent_minsert
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_set_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error)
{
  if (minsert && point)
    {
      *error = 0;
      minsert->ins_pt.x = point->x;
      minsert->ins_pt.y = point->y;
      minsert->ins_pt.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the scale flag of minsert
\code Usage: double flag = dwg_ent_minsert_get_flag(minsert, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_minsert_get_scale_flag(const dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->scale_flag;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the scale flag of minsert.
\code Usage: dwg_ent_minsert_set_scale_flag(minsert, flag, &error);
\endcode
\param 1 dwg_ent_minsert
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_set_scale_flag(dwg_ent_minsert *minsert, char flags,
                               int *error)
{
  if (minsert)
    {
      *error = 0;
      minsert->scale_flag = flags;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the scale of minsert.
\code Usage: dwg_ent_minsert_get_scale(minsert, &scale3d, &error);
\endcode
\param minsert[in]   dwg_ent_insert*
\param scale3d[out]  dwg_point_3d*
\param error[out]    int*
*/
void
dwg_ent_minsert_get_scale(const dwg_ent_minsert *minsert, dwg_point_3d *scale3d,
                          int *error)
{
  if (minsert && scale3d)
    {
      *error = 0;
      scale3d->x = minsert->scale.x;
      scale3d->y = minsert->scale.y;
      scale3d->z = minsert->scale.z;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the scale of minsert.
\code Usage: dwg_ent_minsert_set_scale(minsert, &scale3d, &error);
\endcode
\param minsert[in]   dwg_ent_insert*
\param scale3d[in]   dwg_point_3d*
\param error[out]    int*
*/
void
dwg_ent_minsert_set_scale(dwg_ent_minsert *minsert, dwg_point_3d *scale3d,
                          int *error)
{
  if (minsert && scale3d)
    {
      *error = 0;
      minsert->scale.x = scale3d->x;
      minsert->scale.y = scale3d->y;
      minsert->scale.z = scale3d->z;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the rotation angle of minsert.
\code Usage: double angle = dwg_ent_minsert_get_rotation(minsert, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_minsert_get_rotation(const dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->rotation;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the rotation angle of minsert.
\code Usage: dwg_ent_minsert_set_rotation(minsert, angle, &error);
\endcode
\param 1 dwg_ent_minsert
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_set_rotation(dwg_ent_minsert *minsert, BITCODE_BD rot_ang,
                             int *error)
{
  if (minsert)
    {
      *error = 0;
      minsert->rotation = rot_ang;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the extrusion of minsert.
\code Usage: dwg_ent_minsert_get_extrusion(minsert, &point, &error);
\endcode
\param 1 dwg_ent_minsert
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_get_extrusion(const dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error)
{
  if (minsert && point)
    {
      *error = 0;
      point->x = minsert->extrusion.x;
      point->y = minsert->extrusion.y;
      point->z = minsert->extrusion.z;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Sets the extrusion of minsert.
\code Usage: dwg_ent_minsert_set_extrusion(minsert, &point, &error);
\endcode
\param 1 dwg_ent_minsert
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_set_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error)
{
  if (minsert && point)
    {
      *error = 0;
      minsert->extrusion.x = point->x;
      minsert->extrusion.y = point->y;
      minsert->extrusion.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the has attribs of minsert.
\code Usage: double attribs = dwg_ent_minsert_has_attribs(mintrest, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_minsert_has_attribs(dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->has_attribs;
    }
  else
    {
      LOG_ERROR("%s: empty minsert", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}


/** Returns the owned object count of minsert
\code Usage: BITCODE_BL count = dwg_ent_minsert_get_num_owned(minsert, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_minsert_get_num_owned(const dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->num_owned;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Sets the owned object count of minsert.
/// FIXME needs to adjust attrib_handles array: add/delete

// TODO dwg_ent_minsert_add_owned, dwg_ent_minsert_delete_owned
// TODO dwg_ent_minsert_add_row, dwg_ent_insert_delete_row
// TODO dwg_ent_minsert_add_col, dwg_ent_insert_delete_col

/** Returns the num cols of minsert
\code Usage: BITCODE_BL num_cols = dwg_ent_minsert_get_numcols(minsert, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_minsert_get_numcols(const dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->numcols;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns the number of rows of minsert
\code Usage: BITCODE_BL num_rows = dwg_ent_minsert_get_numrows(minsert, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_minsert_get_numrows(const dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->numrows;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns the column spacing of minsert
\code Usage: double spacing = dwg_ent_minsert_get_col_spacing(minsert, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_minsert_get_col_spacing(const dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->col_spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the column spacing of minsert.
\code Usage: dwg_ent_minsert_set_col_spacing(minsert, 20, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_set_col_spacing(dwg_ent_minsert *minsert, BITCODE_BD spacing,
                                int *error)
{
  if (minsert)
    {
      *error = 0;
      minsert->col_spacing = spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the row spacing of minsert
\code Usage: double spacing = dwg_ent_minsert_get_row_spacing(minsert, &error);
\endcode
\param 1 dwg_ent_minsert
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_minsert_get_row_spacing(const dwg_ent_minsert *minsert, int *error)
{
  if (minsert)
    {
      *error = 0;
      return minsert->row_spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the row spacing of minsert.
\code Usage: dwg_ent_minsert_set_row_spacing(minsert, 20, &error);
\endcode
\param 1 dwg_ent_insert
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_minsert_set_row_spacing(dwg_ent_minsert *minsert, BITCODE_BD spacing,
                                int *error)
{
  if (minsert)
    {
      *error = 0;
      minsert->row_spacing = spacing;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                FUNCTIONS FOR MLINESTYLE OBJECT                    *
********************************************************************/

/** Returns the name of mlinestyle (utf-8 encoded)
\code Usage: char * name = dwg_obj_mlinestyle_get_name(mlinestyle, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_obj_mlinestyle_get_name(const dwg_obj_mlinestyle *mlinestyle, int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)mlinestyle->entry_name);
      else
        return mlinestyle->entry_name;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Sets the name of mlinestyle object
\code Usage: dwg_obj_mlinestyle_set_name(minsert, "mstylename", &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param 2 char *
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_obj_mlinestyle_set_name(dwg_obj_mlinestyle *mlinestyle, char * name,
                            int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        mlinestyle->entry_name = (char*)bit_utf8_to_TU(name);
      else
        mlinestyle->entry_name = name;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the desc of mlinestyle (utf-8 encoded)
\code Usage: char * desc = dwg_obj_mlinestyle_get_desc(mlinestyle, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_obj_mlinestyle_get_desc(const dwg_obj_mlinestyle *mlinestyle, int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)mlinestyle->desc);
      else
        return mlinestyle->desc;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Sets the desc of mlinestyle. (utf-8 encoded)
\code Usage: dwg_obj_mlinestyle_set_desc(minsert, desc, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param 2 utf-8 char *
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_obj_mlinestyle_set_desc(dwg_obj_mlinestyle *mlinestyle, char * desc,
                            int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        mlinestyle->desc = (char*)bit_utf8_to_TU(desc);
      else
        mlinestyle->desc = desc;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the flags of mlinestyle
\code Usage: int flag = dwg_obj_mlinestyle_get_flag(minsert, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
int
dwg_obj_mlinestyle_get_flag(const dwg_obj_mlinestyle *mlinestyle, int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->flag;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the flag of mlinestyle
\code Usage: dwg_obj_mlinestyle_set_flag(mlinestyle, 20, &error);
\endcode
\param 1 dwg_ent_insert
\param[out] error   int*, is set to 0 for ok, 1 on error
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_obj_mlinestyle_set_flag(dwg_obj_mlinestyle *mlinestyle, int flags,
                             int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      mlinestyle->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the start angle of mlinestyle
\code Usage: double start_angle = dwg_obj_mlinestyle_get_start_angle(mlinestyle, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_obj_mlinestyle_get_start_angle(const dwg_obj_mlinestyle *mlinestyle, int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->start_angle;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the start angle of mlinestyle
\code Usage: dwg_obj_mlinestyle_set_start_angle(mlinestyle, 20.00, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_obj_mlinestyle_set_start_angle(dwg_obj_mlinestyle *mlinestyle,
                                   double start_angle, int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      mlinestyle->start_angle = start_angle;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the end angle of mlinestyle
\code Usage: double angle = dwg_obj_mlinestyle_get_end_angle(mlinestyle, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_obj_mlinestyle_get_end_angle(const dwg_obj_mlinestyle *mlinestyle, int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->end_angle;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the end angle of mlinestyle
\code Usage: dwg_obj_mlinestyle_set_end_angle(mlinestyle, 20.00, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_obj_mlinestyle_set_end_angle(dwg_obj_mlinestyle *mlinestyle, BITCODE_BD end_angle,
                                 int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      mlinestyle->end_angle = end_angle;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the lines in style of mlinestyle
\code Usage: char lines = dwg_obj_mlinestyle_get_num_lines(mlinestyle, &error);
\endcode
\param 1 dwg_obj_mlinestyle
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_obj_mlinestyle_get_num_lines(const dwg_obj_mlinestyle *mlinestyle,
                                 int *error)
{
  if (mlinestyle)
    {
      *error = 0;
      return mlinestyle->num_lines;
    }
  else
    {
      LOG_ERROR("%s: empty mlinestyle", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/*******************************************************************
*               FUNCTIONS FOR APPID_CONTROL OBJECT                  *
********************************************************************/

/** Returns the number of all registered apps of appid_control
\code Usage: int num = dwg_obj_appid_control_get_num_entries(appcontrol, &error);
\endcode
\param 1 dwg_obj_appid_control
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_obj_appid_control_get_num_entries(const dwg_obj_appid_control *appid, int *error)
{
  if (appid)
    {
      *error = 0;
      return appid->num_entries;
    }
  else
    {
      LOG_ERROR("%s: empty appid", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

dwg_object_ref *
dwg_obj_appid_control_get_appid(const dwg_obj_appid_control *appid,
                                BITCODE_BS index, int *error)
{
  if (appid && index < appid->num_entries)
    {
      *error = 0;
      return appid->apps[index];
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                    FUNCTIONS FOR APPID OBJECT                     *
********************************************************************/

/** Returns the name of appid (utf-8 encoded)
\code Usage: char * name = dwg_obj_appid_get_entry_name(mlinestyle, &error);
\endcode
\param 1 dwg_obj_appid
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_obj_appid_get_entry_name(const dwg_obj_appid *appid, int *error)
{
  if (appid)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)appid->entry_name);
      else
        return appid->entry_name;
    }
  else
    {
      LOG_ERROR("%s: empty appid", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Sets the name of appid (utf-8 encoded)
\code Usage: dwg_obj_appid_set_name(appid, "appid1", &error);
\endcode
\param 1 dwg_obj_appid
\param 2 utf-8 char *
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_obj_appid_set_entry_name(dwg_obj_appid *appid, char * entry_name,
                             int *error)
{
  if (appid)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        appid->entry_name = (char*)bit_utf8_to_TU(entry_name);
      else
        appid->entry_name = entry_name;
    }
  else
    {
      LOG_ERROR("%s: empty appid", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the flag of appid
\code Usage: char flag = dwg_obj_appid_get_flag(appid, &error);
\endcode
\param 1 dwg_obj_appid
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_obj_appid_get_flag(const dwg_obj_appid *appid, int *error)
{
  if (appid)
    {
      *error = 0;
      return appid->flag & 1 ||
             appid->xrefref >> 6 ||
             appid->xrefdep >> 4;
    }
  else
    {
      LOG_ERROR("%s: empty appid", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/** Sets the flag of appid
\code Usage: dwg_obj_appid_set_flag(appid, flag, &error);
\endcode
\param 1 dwg_obj_appid
\param[out] error   int*, is set to 0 for ok, 1 on error
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_obj_appid_set_flag(dwg_obj_appid *appid, char flag, int *error)
{
  if (appid)
    {
      *error = 0;
      appid->flag = flag;
      appid->xrefdep = flag & 16;
      appid->xrefref = flag & 64;
    }
  else
    {
      LOG_ERROR("%s: empty appid", __FUNCTION__)
      *error = 1;
    }
}

/** Returns the appid control handler
\code Usage: dwg_obj_appid_control* appid = dwg_obj_appid_get_appid_control(appid, &error);
\endcode
\param 1 dwg_obj_appid*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_obj_appid_control*
dwg_obj_appid_get_appid_control(const dwg_obj_appid *appid, int *error)
{
  if (appid)
    {
      *error = 0;
      return appid->app_control->obj->tio.object->tio.APPID_CONTROL;
    }
  else
    {
      LOG_ERROR("%s: empty appid", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
*            FUNCTIONS FOR ALL DIMENSION ENTITIES                *
********************************************************************/

// (utf-8 encoded)
char *
dwg_ent_dim_get_block_name(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      char *name = ((dwg_ent_dim_linear *)dim)->
                     block->obj->tio.object->tio.BLOCK_HEADER->entry_name;
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)name);
      else
        return name;
    }
  else
    {
      LOG_ERROR("%s: empty dim", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Returns the elevation, the z-coord for all 11,12, 16 ECS points
\code Usage: double elevation = dwg_ent_dim_get_elevation(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_get_elevation(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->elevation;
    }
  else
    {
      LOG_ERROR("%s: empty dim", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/** Sets the elevation for the 11, 12, 16 ECS points
\code Usage: dwg_ent_dim_set_elevation(dim, z, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_elevation(dwg_ent_dim *dim, double elevation, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the flag1
\code Usage: char flags1 = dwg_ent_dim_get_flag1(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_dim_get_flag1(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->flags_1;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the flag1
\code Usage: dwg_ent_dim_set_flag1(dim, flag1, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_flag1(dwg_ent_dim *dim, char flag,
                      int *error)
{
  if (dim)
    {
      *error = 0;
      dim->flags_1 = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the act measurement
\code Usage: double measure = dwg_ent_dim_get_act_measurement(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_get_act_measurement(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->act_measurement;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the act measurement
\code Usage: dwg_ent_dim_set_act_measurement(dim, measure, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_act_measurement(dwg_ent_dim *dim,
                                double act_measurement, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->act_measurement = act_measurement;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the horiz dir
\code Usage: double horiz_dir = dwg_ent_dim_get_horiz_dir(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_get_horiz_dir(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->horiz_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the horiz dir
\code Usage: dwg_ent_dim_set_horiz_dir(dim, horiz_dir, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_horiz_dir(dwg_ent_dim *dim, BITCODE_BD horiz_dir,
                          int *error)
{
  if (dim)
    {
      *error = 0;
      dim->horiz_dir = horiz_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the lspace_factor
\code Usage: double lspace_factor = dwg_ent_dim_get_lspace_factor(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_get_lspace_factor(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->lspace_factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the lspace factor
\code Usage: dwg_ent_dim_set_lspace_factor(dim, factor, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_lspace_factor(dwg_ent_dim *dim,
                              double factor, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->lspace_factor = factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the lspace_style
\code Usage: BITCODE_BS lspace_style = dwg_ent_dim_get_lspace_style(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_dim_get_lspace_style(const dwg_ent_dim *dim,
                             int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->lspace_style;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the lspace style
\code Usage: dwg_ent_dim_set_lspace_style(dim, style, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 BITCODE_BS
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_lspace_style(dwg_ent_dim *dim,
                             BITCODE_BS style, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->lspace_style = style;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the attachment index
\code Usage: BITCODE_BS attachment = dwg_ent_dim_get_attachment_pt(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_dim_get_attachment(const dwg_ent_dim *dim,
                           int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets the attachment index
\code Usage: dwg_ent_dim_set_attachment(dim, attachment, &error);
\endcode
    \param 1 dwg_ent_dim
    \param 2 BITCODE_BS point index
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_attachment(dwg_ent_dim *dim,
                           BITCODE_BS attachment, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->attachment = attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the extrusion
\code Usage: dwg_ent_dim_get_extrusion(dim, &point, &error);
\endcode
    \param 1 dwg_ent_dim
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_extrusion(dwg_ent_dim *dim,
                          dwg_point_3d *point, int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->extrusion.x = point->x;
      dim->extrusion.y = point->y;
      dim->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the extrusion
\code Usage: dwg_ent_dim_set_extrusion(dim, &point, &error);
\endcode
    \param 1 dwg_ent_dim
    \param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_get_extrusion(const dwg_ent_dim *dim,
                          dwg_point_3d *point, int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->extrusion.x;
      point->y = dim->extrusion.y;
      point->z = dim->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the user text (utf-8 encoded)
\code Usage: char * text  = dwg_ent_dim_get_user_text(dim, &error);
\endcode
    \param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char *
dwg_ent_dim_get_user_text(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)dim->user_text);
      else
        return dim->user_text;
    }
  else
    {
      LOG_ERROR("%s: empty dim", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/** Sets the user text (utf-8 encoded)
\code Usage: dwg_ent_dim_set_user_text(dim, "dimension text", &error);
\endcode
    \param 1 dwg_ent_dim
    \param 2 char *
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_user_text(dwg_ent_dim *dim, char * text,
                          int *error)
{
  if (dim)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        dim->user_text = (char*)bit_utf8_to_TU(text);
      else
        dim->user_text = text;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns text rotation
\code Usage: double text_rot  = dwg_ent_dim_get_text_rot(dim, &error);
\endcode
    \param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_get_text_rot(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->text_rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the text rotation
\code Usage: dwg_ent_dim_set_text_rot(dim, 10.10, &error);
\endcode
    \param 1 dwg_ent_dim
    \param 2 double rot
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_text_rot(dwg_ent_dim *dim, BITCODE_BD rot,
                         int *error)
{
  if (dim)
    {
      *error = 0;
      dim->text_rot = rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns ins rotation
\code Usage: double ins_rot  = dwg_ent_dim_get_ins_rotation(dim, &error);
\endcode
    \param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_get_ins_rotation(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->ins_rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the ins rotation
\code Usage: dwg_ent_dim_set_ins_rotation(dim, 10.10, &error);
\endcode
    \param 1 dwg_ent_dim
    \param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_ins_rotation(dwg_ent_dim *dim, BITCODE_BD rot,
                             int *error)
{
  if (dim)
    {
      *error = 0;
      dim->ins_rotation = rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns flip arrow1
\code Usage: char arrow1 = dwg_ent_dim_get_flip_arrow1(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_dim_get_flip_arrow1(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->flip_arrow1;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the flip arrow1
\code Usage: dwg_ent_dim_set_flip_arrow1(dim, arrow1, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_flip_arrow1(dwg_ent_dim *dim,
                            char flip_arrow, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->flip_arrow1 = flip_arrow;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns flip arrow2
\code Usage: char arrow2 = dwg_ent_dim_get_flip_arrow2(dim, &error);
\endcode
\param 1 dwg_ent_dim
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_dim_get_flip_arrow2(const dwg_ent_dim *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->flip_arrow2;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets the flip arrow2
\code Usage: dwg_ent_dim_set_flip_arrow2(dim, arrow2, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_flip_arrow2(dwg_ent_dim *dim,
                            char flip_arrow, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->flip_arrow2 = flip_arrow;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the text mid point
\code Usage: dwg_ent_dim_get_text_mid_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_text_mid_pt(dwg_ent_dim *dim,
                            dwg_point_2d *point, int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->text_midpt.x = point->x;
      dim->text_midpt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the text mid point
\code Usage: dwg_ent_dim_set_text_mid_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_get_text_mid_pt(const dwg_ent_dim *dim,
                            dwg_point_2d *point, int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->text_midpt.x;
      point->y = dim->text_midpt.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the ins scale
\code Usage: dwg_ent_dim_set_ins_scale(dim, &point, &error);
\endcode
\param dim[in]     dwg_ent_dim*
\param scale3d[in] dwg_point_3d*
\param error[out]  int*
*/
void
dwg_ent_dim_set_ins_scale(dwg_ent_dim *dim,
                          dwg_point_3d *scale3d, int *error)
{
  if (dim && scale3d)
    {
      *error = 0;
      dim->ins_scale.x = scale3d->x;
      dim->ins_scale.y = scale3d->y;
      dim->ins_scale.z = scale3d->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the ins scale
\code Usage: dwg_ent_dim_get_ins_scale(dim, &point, &error);
\endcode
\param dim[in]      dwg_ent_dim*
\param scale3d[out] dwg_point_3d*
\param error[out]   int*
*/
void
dwg_ent_dim_get_ins_scale(const dwg_ent_dim *dim,
                          dwg_point_3d *scale3d, int *error)
{
  if (dim && scale3d)
    {
      *error = 0;
      scale3d->x = dim->ins_scale.x;
      scale3d->y = dim->ins_scale.y;
      scale3d->z = dim->ins_scale.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the clone_ins point 12
\code Usage: dwg_ent_dim_set_clone_ins_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_set_clone_ins_pt(dwg_ent_dim *dim, dwg_point_2d *point,
                             int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->clone_ins_pt.x = point->x;
      dim->clone_ins_pt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the clone_ins point
\code Usage: dwg_ent_dim_get_clone_ins_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_get_clone_ins_pt(const dwg_ent_dim *dim, dwg_point_2d *point,
                               int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->clone_ins_pt.x;
      point->y = dim->clone_ins_pt.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*              FUNCTIONS FOR ORDINATE DIMENSION ENTITY             *
********************************************************************/

/** Returns flags2
\code Usage: char flag2 = dwg_ent_dim_ordinate_get_flag2(dim, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_dim_ordinate_get_flag2(const dwg_ent_dim_ordinate *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->flags_2;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets flags2
\code Usage: dwg_ent_dim_ordinate_set_flag2(dim, flag2, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ordinate_set_flag2(dwg_ent_dim_ordinate *dim, char flag,
                                int *error)
{
  if (dim)
    {
      *error = 0;
      dim->flags_2 = flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 10 ucsorigin point
\code Usage: dwg_ent_dim_ordinate_set_ucsorigin_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ordinate_set_ucsorigin_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->ucsorigin_pt.x = point->x;
      dim->ucsorigin_pt.y = point->y;
      dim->ucsorigin_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 10 ucsorigin point
\code Usage: dwg_ent_dim_ordinate_get_ucsorigin_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ordinate_get_ucsorigin_pt(const dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->ucsorigin_pt.x;
      point->y = dim->ucsorigin_pt.y;
      point->z = dim->ucsorigin_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 13 feature_location point
\code Usage: dwg_ent_dim_ordinate_set_feature_location_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ordinate_set_feature_location_pt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->feature_location_pt.x = point->x;
      dim->feature_location_pt.y = point->y;
      dim->feature_location_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 13 feature_location point
\code Usage: dwg_ent_dim_ordinate_get_feature_location_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ordinate_get_feature_location_pt(const dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->feature_location_pt.x;
      point->y = dim->feature_location_pt.y;
      point->z = dim->feature_location_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 14 leader_endpt
\code Usage: dwg_ent_dim_ordinate_set_leader_endpt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ordinate_set_leader_endpt(dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->leader_endpt.x = point->x;
      dim->leader_endpt.y = point->y;
      dim->leader_endpt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 14 leader_endpoint point
\code Usage: dwg_ent_dim_ordinate_get_leader_endpt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ordinate
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ordinate_get_leader_endpt(const dwg_ent_dim_ordinate *dim, dwg_point_3d *point,
                               int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->leader_endpt.x;
      point->y = dim->leader_endpt.y;
      point->z = dim->leader_endpt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
********************************************************************/

/** Sets the 10 def point
\code Usage: dwg_ent_dim_linear_set_def_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_set_def_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->def_pt.x = point->x;
      dim->def_pt.y = point->y;
      dim->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 10 def point
\code Usage: dwg_ent_dim_linear_set_def_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_get_def_pt(const dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->def_pt.x;
      point->y = dim->def_pt.y;
      point->z = dim->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 13 point
\code Usage: dwg_ent_dim_linear_set_13_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_set_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->_13_pt.x = point->x;
      dim->_13_pt.y = point->y;
      dim->_13_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 13 point
\code Usage: dwg_ent_dim_linear_set_13_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_get_13_pt(const dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->_13_pt.x;
      point->y = dim->_13_pt.y;
      point->z = dim->_13_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 14 point
\code Usage: dwg_ent_dim_linear_set_14_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_set_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->_14_pt.x = point->x;
      dim->_14_pt.y = point->y;
      dim->_14_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 14 point
\code Usage: dwg_ent_dim_linear_get_14_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_get_14_pt(const dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->_14_pt.x;
      point->y = dim->_14_pt.y;
      point->z = dim->_14_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the ext line rotation
\code Usage: double rot = dwg_ent_dim_linear_get_ext_line_rotation(dim, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_linear_get_ext_line_rotation(const dwg_ent_dim_linear *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->ext_line_rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the ext line rotation
\code Usage: dwg_ent_dim_linear_set_ext_line_rotation(dim, rot, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_set_ext_line_rotation(dwg_ent_dim_linear *dim, BITCODE_BD rot,
                                         int *error)
{
  if (dim)
    {
      *error = 0;
      dim->ext_line_rot = rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the dim rotation
\code Usage: double rot = dwg_ent_dim_linear_get_dim_rot(dim, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_linear_get_dim_rot(const dwg_ent_dim_linear *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->dim_rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the dim rotation
\code Usage: dwg_ent_dim_linear_set_dim_rot(dim, rot, &error);
\endcode
\param 1 dwg_ent_dim_linear
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_linear_set_dim_rot(dwg_ent_dim_linear *dim, BITCODE_BD rot, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->dim_rot = rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*             FUNCTIONS FOR ALIGNED DIMENSION ENTITY                *
********************************************************************/

/** Sets the 10 def point
\code Usage: dwg_ent_dim_aligned_set_def_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_aligned_set_def_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->def_pt.x = point->x;
      dim->def_pt.y = point->y;
      dim->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 10 def point
\code Usage: dwg_ent_dim_aligned_get_def_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_aligned_get_def_pt(const dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->def_pt.x;
      point->y = dim->def_pt.y;
      point->z = dim->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 13 point
\code Usage: dwg_ent_dim_aligned_set_13_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_aligned_set_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->_13_pt.x = point->x;
      dim->_13_pt.y = point->y;
      dim->_13_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 13 point
\code Usage: dwg_ent_dim_aligned_get_13_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_aligned_get_13_pt(const dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim && point)
    {
      *error = 0;
      point->x = dim->_13_pt.x;
      point->y = dim->_13_pt.y;
      point->z = dim->_13_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 14 point
\code Usage: dwg_ent_dim_aligned_set_14_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_aligned_set_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim && point)
    {
      *error = 0;
      dim->_14_pt.x = point->x;
      dim->_14_pt.y = point->y;
      dim->_14_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 14 point
\code Usage: dwg_ent_dim_aligned_get_14_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_aligned_get_14_pt(const dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error)
{
  if (dim)
    {
      *error = 0;
      point->x = dim->_14_pt.x;
      point->y = dim->_14_pt.y;
      point->z = dim->_14_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the ext line rotation
\code Usage: double rot = dwg_ent_dim_aligned_get_ext_line_rotation(dim, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_dim_aligned_get_ext_line_rotation(const dwg_ent_dim_aligned *dim, int *error)
{
  if (dim)
    {
      *error = 0;
      return dim->ext_line_rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets the ext line rotation
\code Usage: dwg_ent_dim_aligned_set_ext_line_rotation(dim, rot, &error);
\endcode
\param 1 dwg_ent_dim_aligned
\param 2 double
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_aligned_set_ext_line_rotation(dwg_ent_dim_aligned *dim,
                                          double rot, int *error)
{
  if (dim)
    {
      *error = 0;
      dim->ext_line_rot = rot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*              FUNCTIONS FOR ANG3PT DIMENSION ENTITY                *
********************************************************************/

/** Sets the 10 point
\code Usage: dwg_ent_dim_ang3pt_set_10_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_set_def_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->def_pt.x = point->x;
      ang->def_pt.y = point->y;
      ang->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 10 def pt
\code Usage: dwg_ent_dim_ang3pt_get_def_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_get_def_pt(const dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang)
    {
      *error = 0;
      point->x = ang->def_pt.x;
      point->y = ang->def_pt.y;
      point->z = ang->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 13 point
\code Usage: dwg_ent_dim_ang3pt_set_13_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_set_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                            int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->_13_pt.x = point->x;
      ang->_13_pt.y = point->y;
      ang->_13_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 13 pt
\code Usage: dwg_ent_dim_ang3pt_get_13_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_get_13_pt(const dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->_13_pt.x;
      point->y = ang->_13_pt.y;
      point->z = ang->_13_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets the 14 point
\code Usage: dwg_ent_dim_ang3pt_set_14_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_set_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->_14_pt.x = point->x;
      ang->_14_pt.y = point->y;
      ang->_14_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the 14 pt
\code Usage: dwg_ent_dim_ang3pt_get_14_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_get_14_pt(const dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->_14_pt.x;
      point->y = ang->_14_pt.y;
      point->z = ang->_14_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}


/** Sets the 15 first_arc point
\code Usage: dwg_ent_dim_ang3pt_set_first_arc_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_3d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_set_first_arc_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->first_arc_pt.x = point->x;
      ang->first_arc_pt.y = point->y;
      ang->first_arc_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns the text 15 first_arc pt
\code Usage: dwg_ent_dim_ang3pt_get_text_first_arc_pt(dim, &point, &error);
\endcode
\param 1 dwg_ent_dim_ang3pt
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_dim_ang3pt_get_first_arc_pt(const dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->first_arc_pt.x;
      point->y = ang->first_arc_pt.y;
      point->z = ang->first_arc_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*              FUNCTIONS FOR ANG2LN DIMENSION ENTITY                *
********************************************************************/


/// Sets dim ang2ln 10 def point
void
dwg_ent_dim_ang2ln_set_def_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->def_pt.x = point->x;
      ang->def_pt.y = point->y;
      ang->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim ang2ln 10 def point
void
dwg_ent_dim_ang2ln_get_def_pt(const dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->def_pt.x;
      point->y = ang->def_pt.y;
      point->z = ang->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets dim ang2ln 13 point
void
dwg_ent_dim_ang2ln_set_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->_13_pt.x = point->x;
      ang->_13_pt.y = point->y;
      ang->_13_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim ang2ln 13 point
void
dwg_ent_dim_ang2ln_get_13_pt(const dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->_13_pt.x;
      point->y = ang->_13_pt.y;
      point->z = ang->_13_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets dim ang2ln 14 point
void
dwg_ent_dim_ang2ln_set_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->_14_pt.x = point->x;
      ang->_14_pt.y = point->y;
      ang->_14_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim ang2ln 14 point
void
dwg_ent_dim_ang2ln_get_14_pt(const dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->_14_pt.x;
      point->y = ang->_14_pt.y;
      point->z = ang->_14_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}


/// Sets dim ang2ln first_arc 15 point
void
dwg_ent_dim_ang2ln_set_first_arc_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->first_arc_pt.x = point->x;
      ang->first_arc_pt.y = point->y;
      ang->first_arc_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim ang2ln first_arc 15 point
void
dwg_ent_dim_ang2ln_get_first_arc_pt(const dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->first_arc_pt.x;
      point->y = ang->first_arc_pt.y;
      point->z = ang->first_arc_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}


/// Sets dim ang2ln 16 point
void
dwg_ent_dim_ang2ln_set_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      ang->_16_pt.x = point->x;
      ang->_16_pt.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim ang2ln 16 point
void
dwg_ent_dim_ang2ln_get_16_pt(const dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error)
{
  if (ang && point)
    {
      *error = 0;
      point->x = ang->_16_pt.x;
      point->y = ang->_16_pt.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
********************************************************************/


/// Sets dim radius def 10 point
void
dwg_ent_dim_radius_set_def_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius && point)
    {
      *error = 0;
      radius->def_pt.x = point->x;
      radius->def_pt.y = point->y;
      radius->def_pt.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/// Returns dim radius def 10 point
void
dwg_ent_dim_radius_get_def_pt(const dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius && point)
    {
      *error = 0;
      point->x = radius->def_pt.x;
      point->y = radius->def_pt.y;
      point->z = radius->def_pt.z;
    }
  else
    {
      LOG_ERROR("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}


/// Sets dim radius first_arc 15 point
void
dwg_ent_dim_radius_set_first_arc_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius && point)
    {
      *error = 0;
      radius->first_arc_pt.x = point->x;
      radius->first_arc_pt.y = point->y;
      radius->first_arc_pt.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/// Returns dim radius first_arc 15 point
void
dwg_ent_dim_radius_get_first_arc_pt(const dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error)
{
  if (radius && point)
    {
      *error = 0;
      point->x = radius->first_arc_pt.x;
      point->y = radius->first_arc_pt.y;
      point->z = radius->first_arc_pt.z;
    }
  else
    {
      LOG_ERROR("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/// Returns dim radius leader length
BITCODE_BD
dwg_ent_dim_radius_get_leader_length(const dwg_ent_dim_radius *radius, int *error)
{
  if (radius)
    {
      *error = 0;
      return radius->leader_len;
    }
  else
    {
      LOG_ERROR("%s: empty radius", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets dim radius leader length
void
dwg_ent_dim_radius_set_leader_length(dwg_ent_dim_radius *radius, BITCODE_BD length,
                                     int *error)
{
  if (radius)
    {
      *error = 0;
      radius->leader_len = length;
    }
  else
    {
      LOG_ERROR("%s: empty radius", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
*             FUNCTIONS FOR DIAMETER DIMENSION ENTITY               *
********************************************************************/


/// Sets dim diameter def 10 point
void
dwg_ent_dim_diameter_set_def_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia && point)
    {
      *error = 0;
      dia->def_pt.x = point->x;
      dia->def_pt.y = point->y;
      dia->def_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim diameter def 10 point
void
dwg_ent_dim_diameter_get_def_pt(const dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia && point)
    {
      *error = 0;
      point->x = dia->def_pt.x;
      point->y = dia->def_pt.y;
      point->z = dia->def_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}


/// Sets dim diameter first_arc 15 point
void
dwg_ent_dim_diameter_set_first_arc_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia && point)
    {
      *error = 0;
      dia->first_arc_pt.x = point->x;
      dia->first_arc_pt.y = point->y;
      dia->first_arc_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim diameter first_arc 15 point
void
dwg_ent_dim_diameter_get_first_arc_pt(const dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error)
{
  if (dia && point)
    {
      *error = 0;
      point->x = dia->first_arc_pt.x;
      point->y = dia->first_arc_pt.y;
      point->z = dia->first_arc_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns dim diameter leader length
BITCODE_BD
dwg_ent_dim_diameter_get_leader_length(const dwg_ent_dim_diameter *dia, int *error)
{
  if (dia)
    {
      *error = 0;
      return dia->leader_len;
    }
  else
    {
      LOG_ERROR("%s: empty dia", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets dim diameter leader length
void
dwg_ent_dim_diameter_set_leader_length(dwg_ent_dim_diameter *dia,
                                       double length, int *error)
{
  if (dia)
    {
      *error = 0;
      dia->leader_len = length;
    }
  else
    {
      LOG_ERROR("%s: empty dia", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
*                   FUNCTIONS FOR ENDBLK ENTITY                     *
********************************************************************/

/** Returns the dummy
\code Usage: char dummy = dwg_ent_endblk_get_dummy(endblk, &error);
\endcode
\param 1 dwg_ent_endblk
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_endblk_get_dummy(const dwg_ent_endblk *endblk, int *error)
{
  if (endblk)
    {
      *error = 0;
      return endblk->dummy;
    }
  else
    {
      LOG_ERROR("%s: empty endblk", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the dummy
\code Usage: dwg_ent_endblk_set_dummy(endblk, dummy, &error);
\endcode
\param 1 dwg_ent_endblk
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_endblk_set_dummy(dwg_ent_endblk *endblk, char dummy, int *error)
{
  if (endblk)
    {
      *error = 0;
      endblk->dummy = dummy;
    }
  else
    {
      LOG_ERROR("%s: empty endblk", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
*                   FUNCTIONS FOR SEQEND ENTITY                     *
********************************************************************/

/** Returns the dummy
\code Usage: char dummy = dwg_ent_seqend_get_dummy(seqend, &error);
\endcode
\param 1 dwg_ent_seqend
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
char
dwg_ent_seqend_get_dummy(const dwg_ent_seqend *seqend, int *error)
{
  if (seqend)
    {
      *error = 0;
      return seqend->dummy;
    }
  else
    {
      LOG_ERROR("%s: empty seqend", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/** Sets the dummy
\code Usage: dwg_ent_seqend_set_dummy(seqend, dummy, &error);
\endcode
\param 1 dwg_ent_seqend
\param 2 char
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_seqend_set_dummy(dwg_ent_seqend *seqend, char dummy, int *error)
{
  if (seqend)
    {
      *error = 0;
      seqend->dummy = dummy;
    }
  else
    {
      LOG_ERROR("%s: empty seqend", __FUNCTION__)
      *error = 1;
    }
}

/*******************************************************************
*                    FUNCTIONS FOR SHAPE ENTITY                     *
********************************************************************/

/// Returns shape ins point
void
dwg_ent_shape_get_ins_pt(const dwg_ent_shape *shape, dwg_point_3d *point, int *error)
{
  if (shape && point)
    {
      *error = 0;
      point->x = shape->ins_pt.x;
      point->y = shape->ins_pt.y;
      point->z = shape->ins_pt.z;
    }
  else
    {
      LOG_ERROR("%s: empty shape or point", __FUNCTION__)
      *error = 1;
    }
}

/// Sets shape ins point
void
dwg_ent_shape_set_ins_pt(dwg_ent_shape *shape, dwg_point_3d *point, int *error)
{
  if (shape && point)
    {
      *error = 0;
      shape->ins_pt.x = point->x;
      shape->ins_pt.y = point->y;
      shape->ins_pt.z = point->z;
    }
  else
    {
      LOG_ERROR("%s: empty shape or point", __FUNCTION__)
      *error = 1;
    }
}

/// Returns shape scale
BITCODE_BD
dwg_ent_shape_get_scale(const dwg_ent_shape *shape, int *error)
{
  if (shape)
    {
      *error = 0;
      return shape->scale;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets shape scale
void
dwg_ent_shape_set_scale(dwg_ent_shape *shape, BITCODE_BD scale, int *error)
{
  if (shape)
    {
      *error = 0;
      shape->scale = scale;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/// Returns shape rotation
BITCODE_BD
dwg_ent_shape_get_rotation(const dwg_ent_shape *shape, int *error)
{
  if (shape)
    {
      *error = 0;
      return shape->rotation;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets shape rotation
void
dwg_ent_shape_set_rotation(dwg_ent_shape *shape, BITCODE_BD rotation, int *error)
{
  if (shape)
    {
      *error = 0;
      shape->rotation = rotation;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/// Returns shape width factor
BITCODE_BD
dwg_ent_shape_get_width_factor(const dwg_ent_shape *shape, int *error)
{
  if (shape)
    {
      *error = 0;
      return shape->width_factor;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets shape width factor
void
dwg_ent_shape_set_width_factor(dwg_ent_shape *shape, BITCODE_BD width_factor,
                               int *error)
{
  if (shape)
    {
      *error = 0;
      shape->width_factor = width_factor;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/// Returns shape oblique
BITCODE_BD
dwg_ent_shape_get_oblique(const dwg_ent_shape *shape, int *error)
{
  if (shape)
    {
      *error = 0;
      return shape->oblique;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets shape oblique
void
dwg_ent_shape_set_oblique(dwg_ent_shape *shape, BITCODE_BD oblique, int *error)
{
  if (shape)
    {
      *error = 0;
      shape->oblique = oblique;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/// Returns shape thickness
BITCODE_BD
dwg_ent_shape_get_thickness(const dwg_ent_shape *shape, int *error)
{
  if (shape)
    {
      *error = 0;
      return shape->thickness;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
      return bit_nan();
    }
}

/// Sets shape thickness
void
dwg_ent_shape_set_thickness(dwg_ent_shape *shape, BITCODE_BD thickness,
                            int *error)
{
  if (shape)
    {
      *error = 0;
      shape->thickness = thickness;
    }
  else
    {
      LOG_ERROR("%s: empty shape", __FUNCTION__)
      *error = 1;
    }
}

/// Returns shape shape no
BITCODE_BD
dwg_ent_shape_get_shape_no(const dwg_ent_shape *shape, int *error)
{
  if (shape)
    {
      *error = 0;
      return shape->shape_no;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets shape shape no
void
dwg_ent_shape_set_shape_no(dwg_ent_shape *shape, BITCODE_BD no, int *error)
{
  if (shape)
    {
      *error = 0;
      shape->shape_no = no;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns shape extrusion
void
dwg_ent_shape_get_extrusion(const dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error)
{
  if (shape && point)
    {
      *error = 0;
      point->x = shape->extrusion.x;
      point->y = shape->extrusion.y;
      point->z = shape->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets shape extrusion
void
dwg_ent_shape_set_extrusion(dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error)
{
  if (shape && point)
    {
      *error = 0;
      shape->extrusion.x = point->x;
      shape->extrusion.y = point->y;
      shape->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}


/*******************************************************************
*                    FUNCTIONS FOR MTEXT ENTITY                     *
********************************************************************/

/// Sets mtext insertion point
void
dwg_ent_mtext_set_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error)
{
  if (mtext && point)
    {
      *error = 0;
      mtext->insertion_pt.x = point->x;
      mtext->insertion_pt.y = point->y;
      mtext->insertion_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns mtext insertion point
void
dwg_ent_mtext_get_insertion_pt(const dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error)
{
  if (mtext && point)
    {
      *error = 0;
      point->x = mtext->insertion_pt.x;
      point->y = mtext->insertion_pt.y;
      point->z = mtext->insertion_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets mtext extrusion
void
dwg_ent_mtext_set_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error)
{
  if (mtext && point)
    {
      *error = 0;
      mtext->extrusion.x = point->x;
      mtext->extrusion.y = point->y;
      mtext->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext extrusion
void
dwg_ent_mtext_get_extrusion(const dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error)
{
  if (mtext && point)
    {
      *error = 0;
      point->x = mtext->extrusion.x;
      point->y = mtext->extrusion.y;
      point->z = mtext->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets mtext x axis dir
void
dwg_ent_mtext_set_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error)
{
  if (mtext && point)
    {
      *error = 0;
      mtext->x_axis_dir.x = point->x;
      mtext->x_axis_dir.y = point->y;
      mtext->x_axis_dir.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext x axis dir
void
dwg_ent_mtext_get_x_axis_dir(const dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error)
{
  if (mtext && point)
    {
      *error = 0;
      point->x = mtext->x_axis_dir.x;
      point->y = mtext->x_axis_dir.y;
      point->z = mtext->x_axis_dir.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets mtext rect height
void
dwg_ent_mtext_set_rect_height(dwg_ent_mtext *mtext, BITCODE_BD rect_height,
                              int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->rect_height = rect_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns mtext rect height
BITCODE_BD
dwg_ent_mtext_get_rect_height(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->rect_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets mtext rect width
void
dwg_ent_mtext_set_rect_width(dwg_ent_mtext *mtext, BITCODE_BD rect_width,
                             int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->rect_width = rect_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext rect width
BITCODE_BD
dwg_ent_mtext_get_rect_width(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->rect_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets mtext text height
void
dwg_ent_mtext_set_text_height(dwg_ent_mtext *mtext, BITCODE_BD text_height,
                              int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->text_height = text_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext text height
BITCODE_BD
dwg_ent_mtext_get_text_height(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->text_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Returns mtext attachment
BITCODE_BS
dwg_ent_mtext_get_attachment(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets mtext attachment
void
dwg_ent_mtext_set_attachment(dwg_ent_mtext *mtext, BITCODE_BS attachment,
                             int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->attachment = attachment;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext drawing dir
BITCODE_BS
dwg_ent_mtext_get_drawing_dir(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->drawing_dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets mtext drawing dir
void
dwg_ent_mtext_set_drawing_dir(dwg_ent_mtext *mtext, BITCODE_BS dir,
                              int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->drawing_dir = dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext extents_height
BITCODE_BD
dwg_ent_mtext_get_extents_height(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->extents_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets mtext 
void
dwg_ent_mtext_set_extents_height(dwg_ent_mtext *mtext, BITCODE_BD ht, int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->extents_height = ht;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext extents width
BITCODE_BD
dwg_ent_mtext_get_extents_width(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->extents_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets mtext extents_width
void
dwg_ent_mtext_set_extents_width(dwg_ent_mtext *mtext, BITCODE_BD wid, int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->extents_width = wid;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext text value (utf-8 encoded)
char *
dwg_ent_mtext_get_text(const dwg_ent_mtext *ent, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)ent->text);
      else
        return ent->text;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Sets mtext text value (utf-8 encoded)
void
dwg_ent_mtext_set_text(dwg_ent_mtext *ent, char *text, int *error)
{
  if (ent)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        ent->text = (char*)bit_utf8_to_TU(text);
      else
        ent->text = text;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext linespace style
BITCODE_BS
dwg_ent_mtext_get_linespace_style(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->linespace_style;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets mtext linespace style
void
dwg_ent_mtext_set_linespace_style(dwg_ent_mtext *mtext, BITCODE_BS style,
                                  int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->linespace_style = style;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns mtext linespace factor
BITCODE_BD
dwg_ent_mtext_get_linespace_factor(const dwg_ent_mtext *mtext, int *error)
{
  if (mtext)
    {
      *error = 0;
      return mtext->linespace_factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets mtext linespace factor
void
dwg_ent_mtext_set_linespace_factor(dwg_ent_mtext *mtext, BITCODE_BD factor,
                                   int *error)
{
  if (mtext)
    {
      *error = 0;
      mtext->linespace_factor = factor;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR LEADER ENTITY                     *
********************************************************************/

/// Sets leader annot type
void
dwg_ent_leader_set_annot_type(dwg_ent_leader *leader, BITCODE_BS type,
                              int *error)
{
  if (leader)
    {
      *error = 0;
      leader->annot_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns leader annot type
BITCODE_BS
dwg_ent_leader_get_annot_type(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->annot_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets leader path type
void
dwg_ent_leader_set_path_type(dwg_ent_leader *leader, BITCODE_BS type,
                             int *error)
{
  if (leader)
    {
      *error = 0;
      leader->path_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns leader path type
BITCODE_BS
dwg_ent_leader_get_path_type(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->path_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Returns leader numpts
BITCODE_BL
dwg_ent_leader_get_numpts(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->numpts;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

//TODO: dwg_ent_leader_add_point, dwg_ent_leader_delete_pts

/// Sets leader end point proj
void
dwg_ent_leader_set_origin(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error)
{
  if (leader && point)
    {
      *error = 0;
      leader->origin.x = point->x;
      leader->origin.y = point->y;
      leader->origin.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader origin
void
dwg_ent_leader_get_origin(const dwg_ent_leader *leader, dwg_point_3d *point,
                          int *error)
{
  if (leader && point)
    {
      *error = 0;
      point->x = leader->origin.x;
      point->y = leader->origin.y;
      point->z = leader->origin.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets leader extrusion value
void
dwg_ent_leader_set_extrusion(dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error)
{
  if (leader && point)
    {
      *error = 0;
      leader->extrusion.x = point->x;
      leader->extrusion.y = point->y;
      leader->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader extrusion value
void
dwg_ent_leader_get_extrusion(const dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error)
{
  if (leader && point)
    {
      *error = 0;
      point->x = leader->extrusion.x;
      point->y = leader->extrusion.y;
      point->z = leader->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets leader x direction value
void
dwg_ent_leader_set_x_direction(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error)
{
  if (leader && point)
    {
      *error = 0;
      leader->x_direction.x = point->x;
      leader->x_direction.y = point->y;
      leader->x_direction.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader x direction value
void
dwg_ent_leader_get_x_direction(const dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error)
{
  if (leader && point)
    {
      *error = 0;
      point->x = leader->x_direction.x;
      point->y = leader->x_direction.y;
      point->z = leader->x_direction.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets leader offset to block insert point
void
dwg_ent_leader_set_offset_to_block_ins_pt(dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error)
{
  if (leader && point)
    {
      *error = 0;
      leader->offset_to_block_ins_pt.x = point->x;
      leader->offset_to_block_ins_pt.y = point->y;
      leader->offset_to_block_ins_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader offset to block ins point
void
dwg_ent_leader_get_offset_to_block_ins_pt(const dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error)
{
  if (leader && point)
    {
      *error = 0;
      point->x = leader->offset_to_block_ins_pt.x;
      point->y = leader->offset_to_block_ins_pt.y;
      point->z = leader->offset_to_block_ins_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets leader dimgap
void
dwg_ent_leader_set_dimgap(dwg_ent_leader *leader, BITCODE_BD dimgap, int *error)
{
  if (leader)
    {
      *error = 0;
      leader->dimgap = dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader dimgap
BITCODE_BD
dwg_ent_leader_get_dimgap(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets leader box height
void
dwg_ent_leader_set_box_height(dwg_ent_leader *leader, BITCODE_BD height,
                              int *error)
{
  if (leader)
    {
      *error = 0;
      leader->box_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader box height
BITCODE_BD
dwg_ent_leader_get_box_height(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->box_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets leader box width
void
dwg_ent_leader_set_box_width(dwg_ent_leader *leader, BITCODE_BD width, int *error)
{
  if (leader)
    {
      *error = 0;
      leader->box_width = width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader box width
BITCODE_BD
dwg_ent_leader_get_box_width(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->box_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets leader hook line on x dir value
void
dwg_ent_leader_set_hook_line_on_x_dir(dwg_ent_leader *leader, char hook,
                                      int *error)
{
  if (leader)
    {
      *error = 0;
      leader->hooklineonxdir = hook;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader hook line on x dir value
char
dwg_ent_leader_get_hook_line_on_x_dir(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->hooklineonxdir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets leader arrowhead on
void
dwg_ent_leader_set_arrowhead_on(dwg_ent_leader *leader, char arrow, int *error)
{
  if (leader)
    {
      *error = 0;
      leader->arrowhead_on = arrow;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader arrow head on
char
dwg_ent_leader_get_arrowhead_on(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->arrowhead_on;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets leader arrow head type
void
dwg_ent_leader_set_arrowhead_type(dwg_ent_leader *leader, BITCODE_BS type,
                                  int *error)
{
  if (leader)
    {
      leader->arrowhead_type = type;
      *error = 0;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns leader arrowhead type
BITCODE_BS
dwg_ent_leader_get_arrowhead_type(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->arrowhead_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets leader dimasz
void
dwg_ent_leader_set_dimasz(dwg_ent_leader *leader, BITCODE_BD dimasz, int *error)
{
  if (leader)
    {
      *error = 0;
      leader->dimasz = dimasz;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns leader dimasz
BITCODE_BD
dwg_ent_leader_get_dimasz(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->dimasz;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets leader byblock color
void
dwg_ent_leader_set_byblock_color(dwg_ent_leader *leader, BITCODE_BS color,
                                 int *error)
{
  if (leader)
    {
      *error = 0;
      leader->byblock_color = color;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns leader byblock color
BITCODE_BS
dwg_ent_leader_get_byblock_color(const dwg_ent_leader *leader, int *error)
{
  if (leader)
    {
      *error = 0;
      return leader->byblock_color;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/*******************************************************************
*                  FUNCTIONS FOR TOLERANCE ENTITY                   *
********************************************************************/

/// Sets tolerance height
void
dwg_ent_tolerance_set_height(dwg_ent_tolerance *tol, BITCODE_BD height,
                             int *error)
{
  if (tol)
    {
      *error = 0;
      tol->height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns tolerance height
BITCODE_BD
dwg_ent_tolerance_get_height(const dwg_ent_tolerance *tol, int *error)
{
  if (tol)
    {
      *error = 0;
      return tol->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets tolerance dimgap
void
dwg_ent_tolerance_set_dimgap(dwg_ent_tolerance *tol, BITCODE_BD dimgap,
                             int *error)
{
  if (tol)
    {
      *error = 0;
      tol->dimgap = dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns tolerance dimgap
BITCODE_BD
dwg_ent_tolerance_get_dimgap(const dwg_ent_tolerance *tol, int *error)
{
  if (tol)
    {
      *error = 0;
      return tol->dimgap;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets tolerance insertion point
void
dwg_ent_tolerance_set_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error)
{
  if (tol && point)
    {
      *error = 0;
      tol->ins_pt.x = point->x;
      tol->ins_pt.y = point->y;
      tol->ins_pt.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns tolerance insertion point
void
dwg_ent_tolerance_get_ins_pt(const dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error)
{
  if (tol && point)
    {
      *error = 0;
      point->x = tol->ins_pt.x;
      point->y = tol->ins_pt.y;
      point->z = tol->ins_pt.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets tolerance x direction
void
dwg_ent_tolerance_set_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error)
{
  if (tol && point)
    {
      *error = 0;
      tol->x_direction.x = point->x;
      tol->x_direction.y = point->y;
      tol->x_direction.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns tolerance x direction
void
dwg_ent_tolerance_get_x_direction(const dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error)
{
  if (tol && point)
    {
      *error = 0;
      point->x = tol->x_direction.x;
      point->y = tol->x_direction.y;
      point->z = tol->x_direction.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets tolerance extrusion
void
dwg_ent_tolerance_set_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error)
{
  if (tol && point)
    {
      *error = 0;
      tol->extrusion.x = point->x;
      tol->extrusion.y = point->y;
      tol->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns tolerance extrusion
void
dwg_ent_tolerance_get_extrusion(const dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error)
{
  if (tol && point)
    {
      *error = 0;
      point->x = tol->extrusion.x;
      point->y = tol->extrusion.y;
      point->z = tol->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Sets tolerance text string (utf-8 encoded)
void
dwg_ent_tolerance_set_text_string(dwg_ent_tolerance *tol, char * string,
                                  int *error)
{
  if (tol)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        tol->text_string = (char*)bit_utf8_to_TU(string);
      else
        tol->text_string = string;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }

}

/// Returns tolerance text string (utf-8 encoded)
char *
dwg_ent_tolerance_get_text_string(const dwg_ent_tolerance *tol, int *error)
{
  if (tol)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)tol->text_string);
      else
        return tol->text_string;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                   FUNCTIONS FOR LWPOLYLINE ENTITY                    *
********************************************************************/
/// Returns lwpline flags
BITCODE_BS
dwg_ent_lwpline_get_flag(const dwg_ent_lwpline *lwpline, int *error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets lwpline flags
void
dwg_ent_lwpline_set_flag(dwg_ent_lwpline *lwpline, char flags, int *error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns lwpline const width
BITCODE_BD
dwg_ent_lwpline_get_const_width(const dwg_ent_lwpline *lwpline, int *error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->const_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets lwpline const width
void
dwg_ent_lwpline_set_const_width(dwg_ent_lwpline *lwpline, BITCODE_BD const_width,
                                int *error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->const_width = const_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}
/// Returns lwpline elevation
BITCODE_BD
dwg_ent_lwpline_get_elevation(const dwg_ent_lwpline *lwpline, int *error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets lwpline elevation
void
dwg_ent_lwpline_set_elevation(dwg_ent_lwpline *lwpline, BITCODE_BD elevation,
                              int *error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}
/// Returns lwpline thickness
BITCODE_BD
dwg_ent_lwpline_get_thickness(const dwg_ent_lwpline *lwpline, int *error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets lwpline thickness
void
dwg_ent_lwpline_set_thickness(dwg_ent_lwpline *lwpline, BITCODE_BD thickness,
                              int *error)
{
  if (lwpline)
    {
      *error = 0;
      lwpline->thickness = thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}
/// Returns lwpline point count
BITCODE_BL
dwg_ent_lwpline_get_numpoints(const dwg_ent_lwpline *lwpline, int *error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->num_points;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Returns lwpline bulges count
BITCODE_BL
dwg_ent_lwpline_get_numbulges(const dwg_ent_lwpline *lwpline, int *error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->num_bulges;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}


/// Returns lwpline width count
BITCODE_BL
dwg_ent_lwpline_get_numwidths(const dwg_ent_lwpline *lwpline, int *error)
{
  if (lwpline)
    {
      *error = 0;
      return lwpline->num_widths;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}


/// Returns lwpline normal
void
dwg_ent_lwpline_get_normal(const dwg_ent_lwpline *lwpline, dwg_point_3d *point,
                           int *error)
{
  if (lwpline && point)
    {
      *error = 0;
      point->x = lwpline->normal.x;
      point->y = lwpline->normal.y;
      point->z = lwpline->normal.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets lwpline normal
void
dwg_ent_lwpline_set_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *point,
                           int *error)
{
  if (lwpline && point)
    {
      *error = 0;
      lwpline->normal.x = point->x;
      lwpline->normal.y = point->y;
      lwpline->normal.z = point->z;
  }
    else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}
/// Returns lwpline bulges
BITCODE_BD *
dwg_ent_lwpline_get_bulges(const dwg_ent_lwpline *lwpline, int *error)
{
  BITCODE_BD *ptx = (BITCODE_BD*) malloc(sizeof(BITCODE_BD)* lwpline->num_bulges);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < lwpline->num_bulges ; i++)
        {
          ptx[i] = lwpline->bulges[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns lwpline points
dwg_point_2d *
dwg_ent_lwpline_get_points(const dwg_ent_lwpline *lwpline, int *error)
{
  dwg_point_2d *ptx = (dwg_point_2d*) malloc(sizeof(dwg_point_2d)* lwpline->num_points);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < lwpline->num_points ; i++)
        {
          ptx[i].x = lwpline->points[i].x;
          ptx[i].y = lwpline->points[i].y;
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns lwpline widths
dwg_lwpline_widths *
dwg_ent_lwpline_get_widths(const dwg_ent_lwpline *lwpline, int *error)
{
  dwg_lwpline_widths *ptx = (dwg_lwpline_widths*)
    malloc(sizeof(dwg_lwpline_widths)* lwpline->num_widths);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < lwpline->num_widths ; i++)
        {
          ptx[i].start = lwpline->widths[i].start;
          ptx[i].end = lwpline->widths[i].end;
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
********************************************************************/

/// Returns ole2frame flags
BITCODE_BS
dwg_ent_ole2frame_get_flag(const dwg_ent_ole2frame *frame, int *error)
{
  if (frame)
    {
      *error = 0;
      return frame->flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return (BITCODE_BS)-1;
    }
}

/// Sets ole2frame flags
void
dwg_ent_ole2frame_set_flag(dwg_ent_ole2frame *frame, BITCODE_BS flags,
                            int *error)
{
  if (frame)
    {
      *error = 0;
      frame->flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns ole2frame mode
BITCODE_BS
dwg_ent_ole2frame_get_mode(const dwg_ent_ole2frame *frame, int *error)
{
  if (frame)
    {
      *error = 0;
      return frame->mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return (BITCODE_BS)-1;
    }
}

/// set ole2frame mode
void
dwg_ent_ole2frame_set_mode(dwg_ent_ole2frame *frame, BITCODE_BS mode,
                           int *error)
{
  if (frame)
    {
      *error = 0;
      frame->mode = mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns ole2frame data length
BITCODE_BL
dwg_ent_ole2frame_get_data_length(const dwg_ent_ole2frame *frame, int *error)
{
  if (frame)
    {
      *error = 0;
      return frame->data_length;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Sets ole2frame data length
void
dwg_ent_ole2frame_set_data_length(dwg_ent_ole2frame *frame, BITCODE_BL data_length,
                                  int *error)
{
  if (frame)
    {
      *error = 0;
      frame->data_length = data_length;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns ole2frame data (binary)
char *
dwg_ent_ole2frame_get_data(const dwg_ent_ole2frame *frame, int *error)
{
  if (frame)
    {
      *error = 0;
      return frame->data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Sets ole2frame data
void
dwg_ent_ole2frame_set_data(dwg_ent_ole2frame *frame, char * data, int *error)
{
  if (frame)
    {
      *error = 0;
      frame->data = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/********************************************************************
*                  FUNCTIONS FOR PROXY OBJECT                       *
********************************************************************/


BITCODE_BL
dwg_obj_proxy_get_class_id(const dwg_obj_proxy *proxy, int *error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->class_id;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

void
dwg_obj_proxy_set_class_id(dwg_obj_proxy *proxy, BITCODE_BL class_id,
                        int *error)
{
  if (proxy)
    {
      *error = 0;
      proxy->class_id = class_id;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

BITCODE_BL
dwg_obj_proxy_get_version(const dwg_obj_proxy *proxy, int *error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

void
dwg_obj_proxy_set_version(dwg_obj_proxy *proxy,
                          BITCODE_BL version,
                          int *error)
{
  if (proxy)
    {
      *error = 0;
      proxy->version = version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

BITCODE_B
dwg_obj_proxy_get_from_dxf(const dwg_obj_proxy *proxy, int *error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->from_dxf;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

void
dwg_obj_proxy_set_from_dxf(dwg_obj_proxy *proxy,
                           BITCODE_B from_dxf,
                           int *error)
{
  if (proxy)
    {
      *error = 0;
      proxy->from_dxf = from_dxf;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

char*
dwg_obj_proxy_get_data(const dwg_obj_proxy *proxy, int *error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

void
dwg_obj_proxy_set_data(dwg_obj_proxy *proxy, char* data,
                       int *error)
{
  if (proxy)
    {
      *error = 0;
      proxy->data = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

dwg_object_ref*
dwg_obj_proxy_get_parenthandle(const dwg_obj_proxy *proxy, int *error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->parenthandle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

dwg_object_ref**
dwg_obj_proxy_get_reactors(const dwg_obj_proxy *proxy, int *error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->reactors;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

dwg_object_ref**
dwg_obj_proxy_get_objid_object_handles(const dwg_obj_proxy *proxy, int *error)
{
  if (proxy)
    {
      *error = 0;
      return proxy->objid_object_handles;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/********************************************************************
*                  FUNCTIONS FOR XRECORD OBJECT                       *
********************************************************************/


BITCODE_BL
dwg_obj_xrecord_get_num_databytes(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->num_databytes;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

BITCODE_BS
dwg_obj_xrecord_get_cloning_flags(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->cloning_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

void
dwg_obj_xrecord_set_cloning_flags(dwg_obj_xrecord *xrecord, BITCODE_BS cloning_flags,
                                  int *error)
{
  if (xrecord)
    {
      *error = 0;
      xrecord->cloning_flags = cloning_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

BITCODE_BL
dwg_obj_xrecord_get_num_eed(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->num_eed;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0U;
    }
}

Dwg_Resbuf*
dwg_obj_xrecord_get_xdata(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->xdata;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

void
dwg_obj_xrecord_set_xdata(dwg_obj_xrecord *xrecord, Dwg_Resbuf* xdata,
                       int *error)
{
  if (xrecord)
    {
      *error = 0;
      xrecord->xdata = xdata;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

dwg_object_ref*
dwg_obj_xrecord_get_parenthandle(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->parenthandle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

dwg_object_ref**
dwg_obj_xrecord_get_reactors(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->reactors;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

BITCODE_BL
dwg_obj_xrecord_get_num_objid_handles(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->num_objid_handles;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

dwg_object_ref**
dwg_obj_xrecord_get_objid_handles(const dwg_obj_xrecord *xrecord, int *error)
{
  if (xrecord)
    {
      *error = 0;
      return xrecord->objid_handles;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                   FUNCTIONS FOR SPLINE ENTITY                     *
********************************************************************/

/// Returns spline scenario
BITCODE_BS
dwg_ent_spline_get_scenario(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->scenario;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets spline scenario
void
dwg_ent_spline_set_scenario(dwg_ent_spline *spline, BITCODE_BS scenario,
                               int *error)
{
  if (spline)
    {
      *error = 0;
      spline->scenario = scenario;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline degree
BITCODE_BS
dwg_ent_spline_get_degree(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->degree;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets spline degree
void
dwg_ent_spline_set_degree(dwg_ent_spline *spline, BITCODE_BS degree,
                             int *error)
{
  if (spline)
    {
      *error = 0;
      spline->degree = degree;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline fit tol
BITCODE_BD
dwg_ent_spline_get_fit_tol(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->fit_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets spline fit tol
void
dwg_ent_spline_set_fit_tol(dwg_ent_spline *spline, int fit_tol, int *error)
{
  if (spline)
    {
      *error = 0;
      spline->fit_tol = fit_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline begin tan vector
void
dwg_ent_spline_get_begin_tan_vector(const dwg_ent_spline *spline,
                                    dwg_point_3d *point, int *error)
{
  if (spline && point)
    {
      *error = 0;
      point->x = spline->beg_tan_vec.x;
      point->y = spline->beg_tan_vec.y;
      point->z = spline->beg_tan_vec.z;
  }
    else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets spline begin tan vector
void
dwg_ent_spline_set_begin_tan_vector(dwg_ent_spline *spline,
                                    dwg_point_3d *point, int *error)
{
  if (spline && point)
    {
      *error = 0;
      spline->beg_tan_vec.x = point->x;
      spline->beg_tan_vec.y = point->y;
      spline->beg_tan_vec.z = point->z;
  }
    else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline end tan vector points
void
dwg_ent_spline_get_end_tan_vector(const dwg_ent_spline *spline,
                                  dwg_point_3d *point, int *error)
{
  if (spline && point)
    {
      *error = 0;
      point->x = spline->end_tan_vec.x;
      point->y = spline->end_tan_vec.y;
      point->z = spline->end_tan_vec.z;
  }
    else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets spline end tan vector
void
dwg_ent_spline_set_end_tan_vector(dwg_ent_spline *spline,
                                     dwg_point_3d *point, int *error)
{
  if (spline && point)
    {
      *error = 0;
      spline->end_tan_vec.x = point->x;
      spline->end_tan_vec.y = point->y;
      spline->end_tan_vec.z = point->z;
  }
    else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline knot tol value
BITCODE_BD
dwg_ent_spline_get_knot_tol(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->knot_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets spline knot tol value
void
dwg_ent_spline_set_knot_tol(dwg_ent_spline *spline, BITCODE_BD knot_tol,
                            int *error)
{
  if (spline)
    {
      *error = 0;
      spline->knot_tol = knot_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline control tol value
BITCODE_BD
dwg_ent_spline_get_ctrl_tol(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->ctrl_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets spline control tol
void
dwg_ent_spline_set_ctrl_tol(dwg_ent_spline *spline, BITCODE_BD ctrl_tol,
                            int *error)
{
  if (spline)
    {
      *error = 0;
      spline->ctrl_tol = ctrl_tol;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline  number of fit points
BITCODE_BS
dwg_ent_spline_get_num_fit_pts(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->num_fit_pts;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Returns spline rational
char
dwg_ent_spline_get_rational(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->rational;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets rational value
void
dwg_ent_spline_set_rational(dwg_ent_spline *spline, char rational,
                            int *error)
{
  if (spline)
    {
      *error = 0;
      spline->rational = rational;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline closed_b
char
dwg_ent_spline_get_closed_b(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->closed_b;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets spline closed_b
void
dwg_ent_spline_set_closed_b(dwg_ent_spline *spline, char closed_b,
                            int *error)
{
  if (spline)
    {
      *error = 0;
      spline->closed_b = closed_b;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline weighted value
char
dwg_ent_spline_get_weighted(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->weighted;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets spline weighted
void
dwg_ent_spline_set_weighted(dwg_ent_spline *spline, char weighted,
                            int *error)
{
  if (spline)
    {
      *error = 0;
      spline->weighted = weighted;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline periodic
char
dwg_ent_spline_get_periodic(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->periodic;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets spline periodic
void
dwg_ent_spline_set_periodic(dwg_ent_spline *spline, char periodic,
                            int *error)
{
  if (spline)
    {
      *error = 0;
      spline->periodic = periodic;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns spline knots number
BITCODE_BL
dwg_ent_spline_get_num_knots(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->num_knots;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

//TODO: dwg_ent_spline_add_fit_pts, dwg_ent_spline_delete_fit_pts
//TODO: dwg_ent_spline_add_knots, dwg_ent_spline_delete_knots
//TODO: dwg_ent_spline_add_ctrl_pts, dwg_ent_spline_delete_ctrl_pts

/// Returns spline control points number
BITCODE_BL
dwg_ent_spline_get_num_ctrl_pts(const dwg_ent_spline *spline, int *error)
{
  if (spline)
    {
      *error = 0;
      return spline->num_ctrl_pts;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Return all spline fit points
dwg_ent_spline_point *
dwg_ent_spline_get_fit_pts(const dwg_ent_spline *spline, int *error)
{
  dwg_ent_spline_point *ptx = (dwg_ent_spline_point*)
    malloc(sizeof(dwg_ent_spline_point)* spline->num_fit_pts);
  if (ptx)
    {
      BITCODE_BS i;
      *error = 0;
      for (i = 0; i < spline->num_fit_pts ; i++)
        {
          ptx[i] = spline->fit_pts[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns spline control points
dwg_ent_spline_control_point *
dwg_ent_spline_get_ctrl_pts(const dwg_ent_spline *spline, int *error)
{
  dwg_ent_spline_control_point *ptx = (dwg_ent_spline_control_point*)
    malloc(sizeof(dwg_ent_spline_control_point)* spline->num_ctrl_pts);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < spline->num_ctrl_pts ; i++)
        {
          ptx[i] = spline->ctrl_pts[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns spline knots
double *
dwg_ent_spline_get_knots(const dwg_ent_spline *spline, int *error)
{
  double *ptx = (double*) malloc(sizeof(double)* spline->num_knots);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < spline->num_knots ; i++)
        {
          ptx[i] = spline->knots[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                   FUNCTIONS FOR VIEWPORT ENTITY                   *
********************************************************************/

/// Returns viewport center points
void
dwg_ent_viewport_get_center(const dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->center.x;
      point->y = vp->center.y;
      point->z = vp->center.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport center
void
dwg_ent_viewport_set_center(dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->center.x = point->x;
      vp->center.y = point->y;
      vp->center.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport width
BITCODE_BD
dwg_ent_viewport_get_width(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets viewport width
void
dwg_ent_viewport_set_width(dwg_ent_viewport *vp, BITCODE_BD width, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->width = width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport height
BITCODE_BD
dwg_ent_viewport_get_height(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets viewport height
void
dwg_ent_viewport_set_height(dwg_ent_viewport *vp, BITCODE_BD height, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport grid major
BITCODE_BS
dwg_ent_viewport_get_grid_major(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->grid_major;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets viewport grid major
void
dwg_ent_viewport_set_grid_major(dwg_ent_viewport *vp, BITCODE_BS major,
                                int *error)
{
  if (vp)
    {
      *error = 0;
      vp->grid_major = major;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport frozen layer count
BITCODE_BL
dwg_ent_viewport_get_num_frozen_layers(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->num_frozen_layers;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Sets viewport frozen layer count (apparently safe to set)
void
dwg_ent_viewport_set_num_frozen_layers(dwg_ent_viewport *vp, BITCODE_BL count,
                                        int *error)
{
  if (vp)
    {
      *error = 0;
      vp->num_frozen_layers = count;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport style sheet name (utf-8 encoded)
char *
dwg_ent_viewport_get_style_sheet(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)vp->style_sheet);
      else
        return vp->style_sheet;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Sets viewport style sheet name (utf-8 encoded)
void
dwg_ent_viewport_set_style_sheet(dwg_ent_viewport *vp, char * sheet,
                                 int *error)
{
  if (vp)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        vp->style_sheet = (char*)bit_utf8_to_TU(sheet);
      else
        vp->style_sheet = sheet;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets circle zoom value
void
dwg_ent_viewport_set_circle_zoom(dwg_ent_viewport *vp, BITCODE_BS zoom,
                                 int *error)
{
  if (vp)
    {
      *error = 0;
      vp->circle_zoom = zoom;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns circle zoom value
BITCODE_BS
dwg_ent_viewport_get_circle_zoom(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->circle_zoom;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets viewport status flags
void
dwg_ent_viewport_set_status_flag(dwg_ent_viewport *vp, BITCODE_BL flags, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->status_flag = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport status flag
BITCODE_BL
dwg_ent_viewport_get_status_flag(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->status_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Returns viewport render mode
char
dwg_ent_viewport_get_render_mode(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->render_mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets viewport render mode
void
dwg_ent_viewport_set_render_mode(dwg_ent_viewport *vp, char mode, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->render_mode = mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport ucs at origin
void
dwg_ent_viewport_set_ucs_at_origin(dwg_ent_viewport *vp, unsigned char origin,
                                   int *error)
{
  if (vp)
    {
      *error = 0;
      vp->ucs_at_origin = origin;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport ucs at origini value
unsigned char
dwg_ent_viewport_get_ucs_at_origin(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->ucs_at_origin;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets viewport ucs per viewport
void
dwg_ent_viewport_set_ucs_per_viewport(dwg_ent_viewport *vp,
                                      unsigned char viewport, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->ucs_per_viewport = viewport;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport ucs per viewport
unsigned char
dwg_ent_viewport_get_ucs_per_viewport(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->ucs_per_viewport;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets viewport view target
void
dwg_ent_viewport_set_view_target(dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->view_target.x = point->x;
      vp->view_target.y = point->y;
      vp->view_target.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport view target
void
dwg_ent_viewport_get_view_target(const dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->view_target.x;
      point->y = vp->view_target.y;
      point->z = vp->view_target.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport view direction
void
dwg_ent_viewport_set_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->view_direction.x = point->x;
      vp->view_direction.y = point->y;
      vp->view_direction.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport view direction
void
dwg_ent_viewport_get_view_direction(const dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->view_direction.x;
      point->y = vp->view_direction.y;
      point->z = vp->view_direction.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets view twist angle
void
dwg_ent_viewport_set_view_twist(dwg_ent_viewport *vp, BITCODE_BD angle,
                                      int *error)
{
  if (vp)
    {
      *error = 0;
      vp->view_twist = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns view twist angle
BITCODE_BD
dwg_ent_viewport_get_view_twist(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->view_twist;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets viewport view height
void
dwg_ent_viewport_set_view_height(dwg_ent_viewport *vp, BITCODE_BD height,
                                 int *error)
{
  if (vp)
    {
      *error = 0;
      vp->view_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport view height
BITCODE_BD
dwg_ent_viewport_get_view_height(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->view_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets viewport lens length
void
dwg_ent_viewport_set_lens_length(dwg_ent_viewport *vp, BITCODE_BD length,
                                 int *error)
{
  if (vp)
    {
      *error = 0;
      vp->lens_length = length;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns lens length
BITCODE_BD
dwg_ent_viewport_get_lens_length(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->lens_length;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets viewport front clip z value
void
dwg_ent_viewport_set_front_clip_z(dwg_ent_viewport *vp, BITCODE_BD front_z,
                                  int *error)
{
  if (vp)
    {
      *error = 0;
      vp->front_clip_z = front_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport front clip z value
BITCODE_BD
dwg_ent_viewport_get_front_clip_z(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->front_clip_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets viewport back clip z value
void
dwg_ent_viewport_set_back_clip_z(dwg_ent_viewport *vp, BITCODE_BD back_z,
                                 int *error)
{
  if (vp)
    {
      *error = 0;
      vp->back_clip_z = back_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport back clip z value
BITCODE_BD
dwg_ent_viewport_get_back_clip_z(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->back_clip_z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets viewport snap angle
void
dwg_ent_viewport_set_snap_angle(dwg_ent_viewport *vp, BITCODE_BD angle, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->snap_angle = angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport snap angle
BITCODE_BD
dwg_ent_viewport_get_snap_angle(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->snap_angle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Returns viewport view center
void
dwg_ent_viewport_get_view_center(const dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->view_center.x;
      point->y = vp->view_center.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport view center
void
dwg_ent_viewport_set_view_center(dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error)
{
  if (vp)
    {
      *error = 0;
      vp->view_center.x = point->x;
      vp->view_center.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns grid spacing
void
dwg_ent_viewport_get_grid_spacing(const dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->grid_spacing.x;
      point->y = vp->grid_spacing.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets grid spacing
void
dwg_ent_viewport_set_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->grid_spacing.x = point->x;
      vp->grid_spacing.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport snap base
void
dwg_ent_viewport_get_snap_base(const dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->snap_base.x;
      point->y = vp->snap_base.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport snap base
void
dwg_ent_viewport_set_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->snap_base.x = point->x;
      vp->snap_base.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport snap spacing
void
dwg_ent_viewport_get_snap_spacing(const dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->snap_spacing.x;
      point->y = vp->snap_spacing.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport snap spacing
void
dwg_ent_viewport_set_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->snap_spacing.x = point->x;
      vp->snap_spacing.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport ucs origin
void
dwg_ent_viewport_set_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->ucs_origin.x = point->x;
      vp->ucs_origin.y = point->y;
      vp->ucs_origin.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport ucs origin
void
dwg_ent_viewport_get_ucs_origin(const dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->ucs_origin.x;
      point->y = vp->ucs_origin.y;
      point->z = vp->ucs_origin.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport ucs X axis
void
dwg_ent_viewport_set_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->ucs_x_axis.x = point->x;
      vp->ucs_x_axis.y = point->y;
      vp->ucs_x_axis.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport ucs X axis
void
dwg_ent_viewport_get_ucs_x_axis(const dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->ucs_x_axis.x;
      point->y = vp->ucs_x_axis.y;
      point->z = vp->ucs_x_axis.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport ucs y axis
void
dwg_ent_viewport_set_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp && point)
    {
      *error = 0;
      vp->ucs_y_axis.x = point->x;
      vp->ucs_y_axis.y = point->y;
      vp->ucs_y_axis.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport ucs y axis
void
dwg_ent_viewport_get_ucs_y_axis(const dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error)
{
  if (vp && point)
    {
      *error = 0;
      point->x = vp->ucs_y_axis.x;
      point->y = vp->ucs_y_axis.y;
      point->z = vp->ucs_y_axis.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets viewport ucs elevation
void
dwg_ent_viewport_set_ucs_elevation(dwg_ent_viewport *vp, BITCODE_BD elevation,
                                   int *error)
{
  if (vp)
    {
      *error = 0;
      vp->ucs_elevation = elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns ucs elevation
BITCODE_BD
dwg_ent_viewport_get_ucs_elevation(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->ucs_elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets UCS ortho view type
void
dwg_ent_viewport_set_ucs_ortho_view_type(dwg_ent_viewport *vp,
                                         BITCODE_BS type, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->ucs_ortho_view_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns UCS ortho view type
BITCODE_BS
dwg_ent_viewport_get_ucs_ortho_view_type(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->ucs_ortho_view_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets shade plot mode value
void
dwg_ent_viewport_set_shadeplot_mode(dwg_ent_viewport *vp,
                                    BITCODE_BS shadeplot, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->shadeplot_mode = shadeplot;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns shade plot mode value
BITCODE_BS
dwg_ent_viewport_get_shadeplot_mode(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->shadeplot_mode;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets viewport default lightning usage
void
dwg_ent_viewport_set_use_default_lights(dwg_ent_viewport *vp,
                                    unsigned char lights, int *error)
{
  if (vp)
    {
      *error = 0;
      vp->use_default_lights = lights;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport default lightning usage
unsigned char
dwg_ent_viewport_get_use_default_lights(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->use_default_lights;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets viewport default lightning type
void
dwg_ent_viewport_set_default_lighting_type(dwg_ent_viewport *vp, char type,
                                       int *error)
{
  if (vp)
    {
      *error = 0;
      vp->default_lighting_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport default lightning type
char
dwg_ent_viewport_get_default_lighting_type(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->default_lighting_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets viewport brightness
void
dwg_ent_viewport_set_brightness(dwg_ent_viewport *vp, BITCODE_BD brightness,
                                int *error)
{
  if (vp)
    {
      *error = 0;
      vp->brightness = brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport brightness
BITCODE_BD
dwg_ent_viewport_get_brightness(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// sets viewport contrast
void
dwg_ent_viewport_set_contrast(dwg_ent_viewport *vp, BITCODE_BD contrast,
                              int *error)
{
  if (vp)
    {
      *error = 0;
      vp->contrast = contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns viewport contrast
BITCODE_BD
dwg_ent_viewport_get_contrast(const dwg_ent_viewport *vp, int *error)
{
  if (vp)
    {
      *error = 0;
      return vp->contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/*******************************************************************
*                FUNCTIONS FOR POLYLINE PFACE ENTITY                *
********************************************************************/

/// Returns polyline pface num verts
BITCODE_BS
dwg_ent_polyline_pface_get_numpoints(const dwg_ent_polyline_pface *pface, int *error)
{
  if (pface)
    {
      *error = 0;
      return pface->numverts;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Returns polyline pface numfaces
BITCODE_BS
dwg_ent_polyline_pface_get_numfaces(const dwg_ent_polyline_pface *pface, int *error)
{
  if (pface)
    {
      *error = 0;
      return pface->numfaces;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

dwg_point_3d *
dwg_obj_polyline_pface_get_points(const dwg_object *obj, int *error)
{
  if (obj && obj->type == DWG_TYPE_POLYLINE_PFACE)
    {
      *error = 0;
      LOG_ERROR("%s: nyi", __FUNCTION__);
      return NULL; //TODO
    }
  else
    {
      LOG_ERROR("%s: empty or wrong arg", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
*                FUNCTIONS FOR POLYLINE_MESH ENTITY                 *
********************************************************************/

/// Returns polyline mesh flags
BITCODE_BS
dwg_ent_polyline_mesh_get_flag(const dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->flag;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Sets polyline mesh flags
void
dwg_ent_polyline_mesh_set_flag(dwg_ent_polyline_mesh *mesh,
                                BITCODE_BS flags, int *error)
{
  if (mesh)
    {
      *error = 0;
      mesh->flag = flags;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline mesh curve type
BITCODE_BS
dwg_ent_polyline_mesh_get_curve_type(const dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->curve_type;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Sets polyline mesh curve type
void
dwg_ent_polyline_mesh_set_curve_type(dwg_ent_polyline_mesh *mesh,
                                     BITCODE_BS curve_type, int *error)
{
  if (mesh)
    {
      *error = 0;
      mesh->curve_type = curve_type;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline mesh n vert count
BITCODE_BS
dwg_ent_polyline_mesh_get_num_m_verts(const dwg_ent_polyline_mesh *mesh,
                                       int *error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->num_m_verts;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// TODO: dwg_ent_polyline_mesh_add_{m,n}_vert, dwg_ent_polyline_mesh_delete_{m,n}_vert

/// Returns polyline mesh n vert count
BITCODE_BS
dwg_ent_polyline_mesh_get_num_n_verts(const dwg_ent_polyline_mesh *mesh,
                                       int *error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->num_n_verts;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Returns polyline mesh n density
BITCODE_BS
dwg_ent_polyline_mesh_get_m_density(const dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->m_density;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Sets polyline mesh M density
void
dwg_ent_polyline_mesh_set_m_density(dwg_ent_polyline_mesh *mesh,
                                    BITCODE_BS m_density, int *error)
{
  if (mesh)
    {
      *error = 0;
      mesh->m_density = m_density;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline mesh n density
BITCODE_BS
dwg_ent_polyline_mesh_get_n_density(const dwg_ent_polyline_mesh *mesh, int *error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->n_density;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Sets polyline mesh n density
void
dwg_ent_polyline_mesh_set_n_density(dwg_ent_polyline_mesh *mesh,
                                    BITCODE_BS n_density, int *error)
{
  if (mesh)
    {
      *error = 0;
      mesh->n_density = n_density;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline mesh owned object count
BITCODE_BL
dwg_ent_polyline_mesh_get_num_owned(const dwg_ent_polyline_mesh *mesh,
                                          int *error)
{
  if (mesh)
    {
      *error = 0;
      return mesh->num_owned;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

/*******************************************************************
*                 FUNCTIONS FOR POLYLINE_2D ENTITY                  *
********************************************************************/

/// Returns polyline 2d extrusion
void
dwg_ent_polyline_2d_get_extrusion(const dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error)
{
  if (line2d && point)
    {
      *error = 0;
      point->x = line2d->extrusion.x;
      point->y = line2d->extrusion.y;
      point->z = line2d->extrusion.z;
  }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Sets polyline extrusion
void
dwg_ent_polyline_2d_set_extrusion(dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error)
{
  if (line2d && point)
    {
      *error = 0;
      line2d->extrusion.x = point->x;
      line2d->extrusion.y = point->y;
      line2d->extrusion.z = point->z;
  }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline 2d start width
BITCODE_BD
dwg_ent_polyline_2d_get_start_width(const dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d)
    {
      *error = 0;
      return line2d->start_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets polyline 2d start width
void
dwg_ent_polyline_2d_set_start_width(dwg_ent_polyline_2d *line2d,
                                    double start_width, int *error)
{
  if (line2d)
    {
      *error = 0;
      line2d->start_width = start_width;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline 2d end width
BITCODE_BD
dwg_ent_polyline_2d_get_end_width(const dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d)
    {
      *error = 0;
      return line2d->end_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets polyline 2d end width
void
dwg_ent_polyline_2d_set_end_width(dwg_ent_polyline_2d *line2d,
                                  double end_width, int *error)
{
  if (line2d)
    {
      *error = 0;
      line2d->end_width = end_width;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline 2d thickness
BITCODE_BD
dwg_ent_polyline_2d_get_thickness(const dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d)
    {
      *error = 0;
      return line2d->thickness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets polyline 2d thickness
void
dwg_ent_polyline_2d_set_thickness(dwg_ent_polyline_2d *line2d,
                                  double thickness, int *error)
{
  if (line2d)
    {
      *error = 0;
      line2d->thickness = thickness;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline 2d elevation
BITCODE_BD
dwg_ent_polyline_2d_get_elevation(const dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d)
    {
      *error = 0;
      return line2d->elevation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets polyline 2d elevation
void
dwg_ent_polyline_2d_set_elevation(dwg_ent_polyline_2d *line2d,
                                  double elevation, int *error)
{
  if (line2d)
    {
      *error = 0;
      line2d->elevation = elevation;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline 2d flag
BITCODE_BS
dwg_ent_polyline_2d_get_flag(const dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d)
    {
      *error = 0;
      return line2d->flag;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Sets polyline 2d flags
void
dwg_ent_polyline_2d_set_flag(dwg_ent_polyline_2d *line2d, BITCODE_BS flags,
                              int *error)
{
  if (line2d)
    {
      *error = 0;
      line2d->flag = flags;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns curve type value
BITCODE_BS
dwg_ent_polyline_2d_get_curve_type(const dwg_ent_polyline_2d *line2d, int *error)
{
  if (line2d)
    {
      *error = 0;
      return line2d->curve_type;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return 0;
    }
}

/// Sets curve type value
void
dwg_ent_polyline_2d_set_curve_type(dwg_ent_polyline_2d *line2d,
                                   BITCODE_BS curve_type, int *error)
{
  if (line2d)
    {
      *error = 0;
      line2d->curve_type = curve_type;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns number of vertices
BITCODE_BL
dwg_obj_polyline_2d_get_numpoints(const dwg_object *obj, int *error)
{
  if (obj && obj->type == DWG_TYPE_POLYLINE_2D)
    {
      BITCODE_BL num_points = 0;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_2D *_obj = obj->tio.entity->tio.POLYLINE_2D;
      Dwg_Entity_VERTEX_2D *vertex;
      *error = 0;

      if (dwg->header.version >= R_2004)
        return obj->tio.entity->tio.POLYLINE_2D->num_owned;
      else if (dwg->header.version >= R_13) // iterate over first_vertex - last_vertex
        {
          Dwg_Object *vobj = dwg_ref_get_object(dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_get_object(dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else {
            do {
              if ((vertex = dwg_object_to_VERTEX_2D(vobj))) {
                num_points++;
              } else {
                *error = 1; // return not all vertexes, but some
              }
            } while ((vobj = dwg_next_object(vobj)) && vobj != vlast);
          }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj;
          while ((vobj = dwg_next_object(obj)) && vobj->type != DWG_TYPE_SEQEND)
            {
              if ((vertex = dwg_object_to_VERTEX_2D(vobj)))
                num_points++;
              else
                *error = 1; // return not all vertexes, but some
            }
        }
      return num_points;
    }
  else
    {
      LOG_ERROR("%s: empty or wrong arg", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

dwg_point_2d *
dwg_obj_polyline_2d_get_points(const dwg_object *obj, int *error)
{
  *error = 0;
  if (obj && obj->type == DWG_TYPE_POLYLINE_2D)
    {
      BITCODE_BL i;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_2D *_obj = obj->tio.entity->tio.POLYLINE_2D;
      BITCODE_BL num_points = dwg_obj_polyline_2d_get_numpoints(obj, error);
      dwg_point_2d *ptx = calloc(num_points, sizeof(dwg_point_2d));
      Dwg_Entity_VERTEX_2D *vertex;

      if (*error)
        return NULL;
      if (!ptx)
        {
          LOG_ERROR("%s: Out of memory", __FUNCTION__);
          *error = 1; return NULL;
        }
      vertex = NULL;
      if (dwg->header.version >= R_2004)
        for (i = 0; i < num_points; i++)
          {
            Dwg_Object *vobj = dwg_ref_get_object(dwg, _obj->vertex[i]);
            if (vobj && (vertex = dwg_object_to_VERTEX_2D(vobj))) {
              ptx[i].x = vertex->point.x;
              ptx[i].y = vertex->point.y;
            } else {
              *error = 1; // return not all vertexes, but some
            }
          }
      else if (dwg->header.version >= R_13) // iterate over first_vertex - last_vertex
        {
          Dwg_Object *vobj = dwg_ref_get_object(dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_get_object(dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else {
            i = 0;
            do {
              if ((vertex = dwg_object_to_VERTEX_2D(vobj))) {
                ptx[i].x = vertex->point.x;
                ptx[i].y = vertex->point.y;
                i++;
                if (i > num_points)
                  {
                    *error = 1;
                    break;
                  }
              } else {
                *error = 1; // return not all vertexes, but some
              }
            } while ((vobj = dwg_next_object(vobj)) && vobj != vlast);
          }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj;
          i = 0;
          while ((vobj = dwg_next_object(obj)) && vobj->type != DWG_TYPE_SEQEND)
            {
              if ((vertex = dwg_object_to_VERTEX_2D(vobj)))
                {
                  ptx[i].x = vertex->point.x;
                  ptx[i].y = vertex->point.y;
                  i++;
                  if (i > num_points)
                    {
                      *error = 1;
                      break;
                    }
                }
              else
                {
                  *error = 1; // return not all vertexes, but some
                }
            }
        }
      return ptx;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
*                 FUNCTIONS FOR POLYLINE_3D ENTITY                  *
********************************************************************/

/// Returns polyline 3d flag
BITCODE_RC
dwg_ent_polyline_3d_get_flag(const dwg_ent_polyline_3d *line3d, int *error)
{
  if (line3d)
    {
      *error = 0;
      return line3d->flag;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/// Sets polyline 3d flag
void
dwg_ent_polyline_3d_set_flag(dwg_ent_polyline_3d *line3d, BITCODE_RC flag,
                                int *error)
{
  if (line3d)
    {
      *error = 0;
      line3d->flag = flag;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns polyline 3d flag2
BITCODE_RC
dwg_ent_polyline_3d_get_flag2(const dwg_ent_polyline_3d *line3d, int *error)
{
  if (line3d)
    {
      *error = 0;
      return line3d->flag2;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return '\0';
    }
}

/// Sets polyline 3d flag2
void
dwg_ent_polyline_3d_set_flag2(dwg_ent_polyline_3d *line3d, BITCODE_RC flag2,
                                int *error)
{
  if (line3d)
    {
      *error = 0;
      line3d->flag2 = flag2;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
    }
}

/// Returns number of vertices
BITCODE_BL
dwg_obj_polyline_3d_get_numpoints(const dwg_object *obj, int *error)
{
  if (obj && obj->type == DWG_TYPE_POLYLINE_3D)
    {
      BITCODE_BL num_points = 0;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_3D *_obj = obj->tio.entity->tio.POLYLINE_3D;
      Dwg_Entity_VERTEX_3D *vertex;
      *error = 0;

      if (dwg->header.version >= R_2004)
        return obj->tio.entity->tio.POLYLINE_3D->num_owned;
      else if (dwg->header.version >= R_13) // iterate over first_vertex - last_vertex
        {
          Dwg_Object *vobj = dwg_ref_get_object(dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_get_object(dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else {
            do {
              if ((vertex = dwg_object_to_VERTEX_3D(vobj))) {
                num_points++;
              } else {
                *error = 1; // return not all vertexes, but some
              }
            } while ((vobj = dwg_next_object(vobj)) && vobj != vlast);
          }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj;
          while ((vobj = dwg_next_object(obj)) && vobj->type != DWG_TYPE_SEQEND)
            {
              if ((vertex = dwg_object_to_VERTEX_3D(vobj)))
                num_points++;
              else
                *error = 1; // return not all vertexes, but some
            }
        }
      return num_points;
    }
  else
    {
      LOG_ERROR("%s: empty or wrong arg", __FUNCTION__)
      *error = 1;
      return 0L;
    }
}

/// Returns vertices
dwg_point_3d *
dwg_obj_polyline_3d_get_points(const dwg_object *obj, int *error)
{
  *error = 0;
  if (obj && obj->type == DWG_TYPE_POLYLINE_3D)
    {
      BITCODE_BL i;
      Dwg_Data *dwg = obj->parent;
      Dwg_Entity_POLYLINE_3D *_obj = obj->tio.entity->tio.POLYLINE_3D;
      BITCODE_BL num_points = dwg_obj_polyline_3d_get_numpoints(obj, error);
      dwg_point_3d *ptx = calloc(num_points, sizeof(dwg_point_3d));
      Dwg_Entity_VERTEX_3D *vertex;

      if (*error)
        return NULL;
      if (!ptx)
        {
          LOG_ERROR("%s: Out of memory", __FUNCTION__);
          *error = 1; return NULL;
        }
      vertex = NULL;
      if (dwg->header.version >= R_2004)
        for (i = 0; i < num_points; i++)
          {
            Dwg_Object *vobj = dwg_ref_get_object(dwg, _obj->vertex[i]);
            if (vobj && (vertex = dwg_object_to_VERTEX_3D(vobj))) {
              ptx[i].x = vertex->point.x;
              ptx[i].y = vertex->point.y;
              ptx[i].z = vertex->point.z;
            } else {
              *error = 1; // return not all vertexes, but some
            }
          }
      else if (dwg->header.version >= R_13) // iterate over first_vertex - last_vertex
        {
          Dwg_Object *vobj = dwg_ref_get_object(dwg, _obj->first_vertex);
          Dwg_Object *vlast = dwg_ref_get_object(dwg, _obj->last_vertex);
          if (!vobj)
            *error = 1;
          else {
            i = 0;
            do {
              if ((vertex = dwg_object_to_VERTEX_3D(vobj))) {
                ptx[i].x = vertex->point.x;
                ptx[i].y = vertex->point.y;
                ptx[i].z = vertex->point.z;
                i++;
                if (i > num_points)
                  {
                    *error = 1;
                    break;
                  }
              } else {
                *error = 1; // return not all vertexes, but some
              }
            } while ((vobj = dwg_next_object(vobj)) && vobj != vlast);
          }
        }
      else // <r13: iterate over vertices until seqend
        {
          Dwg_Object *vobj;
          i = 0;
          while ((vobj = dwg_next_object(obj)) && vobj->type != DWG_TYPE_SEQEND)
            {
              if ((vertex = dwg_object_to_VERTEX_3D(vobj)))
                {
                  ptx[i].x = vertex->point.x;
                  ptx[i].y = vertex->point.y;
                  ptx[i].z = vertex->point.z;
                  i++;
                  if (i > num_points)
                    {
                      *error = 1;
                      break;
                    }
                }
              else
                {
                  *error = 1; // return not all vertexes, but some
                }
            }
        }
      return ptx;
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      *error = 1;
      return NULL;
    }
}

/*******************************************************************
*                   FUNCTIONS FOR 3DFACE ENTITY                     *
********************************************************************/

/// Returns the invis flags of a _3dface entity.
/** Usage :- BITCODE_BS flag = dwg_ent_3dface_get_invis_flags(_3dface, &error);
\param[in] _3dface  dwg_ent_3dface*
\param[out] error   set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_3dface_get_invis_flags(const dwg_ent_3dface *_3dface, int *error)
{
  if (_3dface)
    {
      *error = 0;
      return _3dface->invis_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// This sets the invis_flags of a _3dface entity.
/** Usage :- dwg_ent_3dface_set_invis_flagsx(_3dface, flags, &error);
\param 1 dwg_ent_3dface
\param 2 BITCODE_BS
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_set_invis_flags(dwg_ent_3dface *_3dface,
                               BITCODE_BS invis_flags, int *error)
{
  if (_3dface)
    {
      *error = 0;
      _3dface->invis_flags = invis_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner1 values of 3dface.
/** Usage :- dwg_ent_3dface_get_corner1(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_get_corner1(const dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      point->x = _3dface->corner1.x;
      point->y = _3dface->corner1.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner1 values of 3dface.
/** Usage :- dwg_ent_3dface_set_corner1(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_set_corner1(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      _3dface->corner1.x = point->x;
      _3dface->corner1.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner2 values of 3dface.
/** Usage :- dwg_ent_3dface_get_corner2(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_get_corner2(const dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      point->x = _3dface->corner2.x;
      point->y = _3dface->corner2.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner2 values of 3dface.
/** Usage :- dwg_ent_3dface_set_corner2(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_set_corner2(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      _3dface->corner2.x = point->x;
      _3dface->corner2.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner3 values of 3dface.
/** Usage :- dwg_ent_3dface_get_corner3(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_get_corner3(const dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      point->x = _3dface->corner3.x;
      point->y = _3dface->corner3.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner3 values of 3dface.
/** Usage :- dwg_ent_3dface_set_corner3(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_set_corner3(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      _3dface->corner3.x = point->x;
      _3dface->corner3.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns the corner4 values of 3dface.
/** Usage :- dwg_ent_3dface_get_corner4(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_get_corner4(const dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      point->x = _3dface->corner4.x;
      point->y = _3dface->corner4.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets the corner4 values of 3dface.
/** Usage :- dwg_ent_3dface_set_corner4(face, &point, &error);
\param 1 dwg_ent_3dface
\param 2 dwg_point_2d
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
void
dwg_ent_3dface_set_corner4(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error)
{
  if (_3dface && point)
    {
      *error = 0;
      _3dface->corner4.x = point->x;
      _3dface->corner4.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                    FUNCTIONS FOR IMAGE ENTITY                     *
********************************************************************/

/// Returns image class version
BITCODE_BL
dwg_ent_image_get_class_version(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->class_version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Sets image class version
void
dwg_ent_image_set_class_version(dwg_ent_image *image, BITCODE_BL class_version,
                                int *error)
{
  if (image)
    {
      *error = 0;
      image->class_version = class_version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image point 0 points
void
dwg_ent_image_get_pt0(const dwg_ent_image *image, dwg_point_3d *point, int *error)
{
  if (image && point)
    {
      *error = 0;
      point->x = image->pt0.x;
      point->y = image->pt0.y;
      point->z = image->pt0.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets image point 0 points
void
dwg_ent_image_set_pt0(dwg_ent_image *image, dwg_point_3d *point, int *error)
{
  if (image && point)
    {
      *error = 0;
      image->pt0.x = point->x;
      image->pt0.y = point->y;
      image->pt0.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image U_vector points
void
dwg_ent_image_get_u_vector(const dwg_ent_image *image, dwg_point_3d *point,
                           int *error)
{
  if (image && point)
    {
      *error = 0;
      point->x = image->uvec.x;
      point->y = image->uvec.y;
      point->z = image->uvec.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets image U_vector points
void
dwg_ent_image_set_u_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error)
{
  if (image && point)
    {
      *error = 0;
      image->uvec.x = point->x ;
      image->uvec.y = point->y ;
      image->uvec.z = point->z ;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image v_vector points
void
dwg_ent_image_get_v_vector(const dwg_ent_image *image, dwg_point_3d *point,
                           int *error)
{
  if (image && point)
    {
      *error = 0;
      point->x = image->vvec.x;
      point->y = image->vvec.y;
      point->z = image->vvec.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets v vector points
void
dwg_ent_image_set_v_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error)
{
  if (image && point)
    {
      *error = 0;
      image->vvec.x = point->x ;
      image->vvec.y = point->y ;
      image->vvec.z = point->z ;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image size height
BITCODE_BD
dwg_ent_image_get_size_height(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->size.height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets image size height
void
dwg_ent_image_set_size_height(dwg_ent_image *image, BITCODE_BD size_height,
                              int *error)
{
  if (image)
    {
      *error = 0;
      image->size.height = size_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image size width
BITCODE_BD
dwg_ent_image_get_size_width(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->size.width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Sets image size width
void
dwg_ent_image_set_size_width(dwg_ent_image *image, BITCODE_BD size_width,
                             int *error)
{
  if (image)
    {
      *error = 0;
      image->size.width = size_width;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image display props
BITCODE_BS
dwg_ent_image_get_display_props(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->display_props;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets image display props
void
dwg_ent_image_set_display_props(dwg_ent_image *image,
                                BITCODE_BS display_props, int *error)
{
  if (image)
    {
      *error = 0;
      image->display_props = display_props;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image clipping
unsigned char
dwg_ent_image_get_clipping(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->clipping;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets image clipping
void
dwg_ent_image_set_clipping(dwg_ent_image *image, unsigned char clipping,
                           int *error)
{
  if (image)
    {
      *error = 0;
      image->clipping = clipping;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image brightness
char
dwg_ent_image_get_brightness(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets image brightness
void
dwg_ent_image_set_brightness(dwg_ent_image *image, char brightness, int *error)
{
  if (image)
    {
      *error = 0;
      image->brightness = brightness;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Return image contrast
char
dwg_ent_image_get_contrast(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets image contrast
void
dwg_ent_image_set_contrast(dwg_ent_image *image, char contrast, int *error)
{
  if (image)
    {
      *error = 0;
      image->contrast = contrast;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image fade
char
dwg_ent_image_get_fade(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->fade;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets image fade
void
dwg_ent_image_set_fade(dwg_ent_image *image, char fade, int *error)
{
  if (image)
    {
      *error = 0;
      image->fade = fade;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image clip boundary type
BITCODE_BS
dwg_ent_image_get_clip_boundary_type(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->clip_boundary_type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets image clip boundary type
void
dwg_ent_image_set_clip_boundary_type(dwg_ent_image *image, BITCODE_BS type,
                                     int *error)
{
  if (image)
    {
      *error = 0;
      image->clip_boundary_type = type;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns image boundary point 0
void
dwg_ent_image_get_boundary_pt0(const dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image && point)
    {
      *error = 0;
      point->x = image->boundary_pt0.x;
      point->y = image->boundary_pt0.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets image boundary point 0
void
dwg_ent_image_set_boundary_pt0(dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image && point)
    {
      *error = 0;
      image->boundary_pt0.x = point->x;
      image->boundary_pt0.y = point->y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns boundary point1
void
dwg_ent_image_get_boundary_pt1(const dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image && point)
    {
      *error = 0;
      point->x = image->boundary_pt1.x;
      point->y = image->boundary_pt1.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets boundary point1
void
dwg_ent_image_set_boundary_pt1(dwg_ent_image *image, dwg_point_2d *point,
                               int *error)
{
  if (image && point)
    {
      *error = 0;
      point->x = image->boundary_pt1.x;
      point->y = image->boundary_pt1.y;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Retuns number of clip verts
BITCODE_BD
dwg_ent_image_get_num_clip_verts(const dwg_ent_image *image, int *error)
{
  if (image)
    {
      *error = 0;
      return image->num_clip_verts;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// Returns image clip verts
BITCODE_2RD *
dwg_ent_image_get_clip_verts(const dwg_ent_image *image, int *error)
{
  BITCODE_2RD *ptx = (BITCODE_2RD*)
    malloc(sizeof(BITCODE_2RD) * image->num_clip_verts);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < image->num_clip_verts ; i++)
        {
          ptx[i] = image->clip_verts[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                    FUNCTIONS FOR MLINE ENTITY                     *
********************************************************************/

/// Sets mline scale value
void
dwg_ent_mline_set_scale(dwg_ent_mline *mline, BITCODE_BD scale, int *error)
{
  if (mline)
    {
      *error = 0;
      mline->scale = scale;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns scale value
BITCODE_BD
dwg_ent_mline_get_scale(const dwg_ent_mline *mline, int *error)
{
  if (mline)
    {
      *error = 0;
      return mline->scale;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/// sets justification value
void
dwg_ent_mline_set_justification(dwg_ent_mline *mline, char justification, int *error)
{
  if (mline)
    {
      *error = 0;
      mline->justification = justification;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns justification value
char
dwg_ent_mline_get_justification(const dwg_ent_mline *mline, int *error)
{
  if (mline)
    {
      *error = 0;
      return mline->justification;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets base point value
void
dwg_ent_mline_set_base_point(dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error)
{
  if (mline && point)
    {
      *error = 0;
      mline->base_point.x = point->x;
      mline->base_point.y = point->y;
      mline->base_point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns base point value
void
dwg_ent_mline_get_base_point(const dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error)
{
  if (mline && point)
    {
      *error = 0;
      point->x = mline->base_point.x;
      point->y = mline->base_point.y;
      point->z = mline->base_point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets extrusion points
void
dwg_ent_mline_set_extrusion(dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error)
{
  if (mline && point)
    {
      *error = 0;
      mline->extrusion.x = point->x;
      mline->extrusion.y = point->y;
      mline->extrusion.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns extrusion points
void
dwg_ent_mline_get_extrusion(const dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error)
{
  if (mline && point)
    {
      *error = 0;
      point->x = mline->extrusion.x;
      point->y = mline->extrusion.y;
      point->z = mline->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets open closed value
void
dwg_ent_mline_set_flags(dwg_ent_mline *mline, BITCODE_BS oc,
                              int *error)
{
  if (mline)
    {
      *error = 0;
      mline->flags = oc;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns flags value
BITCODE_BS
dwg_ent_mline_get_flags(const dwg_ent_mline *mline, int *error)
{
  if (mline)
    {
      *error = 0;
      return mline->flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Returns number of lines
BITCODE_RC
dwg_ent_mline_get_num_lines(const dwg_ent_mline *mline, int *error)
{
  if (mline)
    {
      *error = 0;
      return mline->num_lines;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Returns number of vertices
BITCODE_BS
dwg_ent_mline_get_num_verts(const dwg_ent_mline *mline, int *error)
{
  if (mline)
    {
      *error = 0;
      return mline->num_verts;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Returns mline vertices
dwg_ent_mline_vertex *
dwg_ent_mline_get_verts(const dwg_ent_mline *mline, int *error)
{
  dwg_ent_mline_vertex *ptx = (dwg_ent_mline_vertex*)
    malloc(sizeof(dwg_ent_mline_vertex)* mline->num_verts);
  if (ptx)
    {
      BITCODE_BS i;
      *error = 0;
      for (i = 0; i < mline->num_verts ; i++)
        {
          ptx[i] = mline->verts[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*              FUNCTIONS FOR VERTEX_PFACE_FACE ENTITY               *
********************************************************************/

/// Returns vertex_pface vertind
BITCODE_BS
dwg_ent_vertex_pface_face_get_vertind(const dwg_ent_vert_pface_face *face)
{
  if (face)
    {
      return face->vertind[3];
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return (BITCODE_BS)-1;
    }
}

/// Sets vertex_pface vertind
void
dwg_ent_vertex_pface_face_set_vertind(dwg_ent_vert_pface_face *face,
                                      BITCODE_BS vertind[4])
{
  if (face && vertind)
    {
      face->vertind[0] = vertind[0];
      face->vertind[1] = vertind[1];
      face->vertind[2] = vertind[2];
      face->vertind[3] = vertind[3];
    }
  else
    {
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                  FUNCTIONS FOR 3DSOLID ENTITY                     *
********************************************************************/

/// Returns acis empty value
unsigned char
dwg_ent_3dsolid_get_acis_empty(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->acis_empty;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets acis empty value
void
dwg_ent_3dsolid_set_acis_empty(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                               int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->acis_empty = acis;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns version value
BITCODE_BS
dwg_ent_3dsolid_get_version(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets version value
void
dwg_ent_3dsolid_set_version(dwg_ent_3dsolid *_3dsolid, BITCODE_BS version,
                            int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->version = version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns pointer to block size
BITCODE_BL*
dwg_ent_3dsolid_get_block_size(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->block_size;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Returns acis data
char *
dwg_ent_3dsolid_get_acis_data(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->acis_data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Sets acis data
void
dwg_ent_3dsolid_set_acis_data(dwg_ent_3dsolid *_3dsolid,
                              char * data, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->acis_data = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns wireframe data present value
char
dwg_ent_3dsolid_get_wireframe_data_present(const dwg_ent_3dsolid *_3dsolid,
                                           int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->wireframe_data_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets wireframe data present value
void
dwg_ent_3dsolid_set_wireframe_data_present(dwg_ent_3dsolid *_3dsolid,
                                           char data, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->wireframe_data_present = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns point present value
char
dwg_ent_3dsolid_get_point_present(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->point_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets point present value
void
dwg_ent_3dsolid_set_point_present(dwg_ent_3dsolid *_3dsolid, char point,
                                  int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->point_present = point;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns point value
void
dwg_ent_3dsolid_get_point(const dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point,
                          int *error)
{
  if (_3dsolid && point)
    {
      *error = 0;
      point->x = _3dsolid->point.x;
      point->y = _3dsolid->point.y;
      point->z = _3dsolid->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets point values
void
dwg_ent_3dsolid_set_point(dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point,
                          int *error)
{
  if (_3dsolid && point)
    {
      *error = 0;
      _3dsolid->point.x = point->x;
      _3dsolid->point.y = point->y;
      _3dsolid->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns number of isolines
BITCODE_BL
dwg_ent_3dsolid_get_num_isolines(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->num_isolines;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Sets number of isolines (apparently safe to set)
void
dwg_ent_3dsolid_set_num_isolines(dwg_ent_3dsolid *_3dsolid, BITCODE_BL num,
                                 int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->num_isolines = num;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns isoline present value
char
dwg_ent_3dsolid_get_isoline_present(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->isoline_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets isoline present value
void
dwg_ent_3dsolid_set_isoline_present(dwg_ent_3dsolid *_3dsolid, char iso,
                                    int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->isoline_present = iso;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns number of wires
BITCODE_BL
dwg_ent_3dsolid_get_num_wires(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->num_wires;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}


/// Returns all wires
dwg_ent_solid_wire *
dwg_ent_3dsolid_get_wires(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  dwg_ent_solid_wire *ptx = (dwg_ent_solid_wire*)
    malloc(sizeof(dwg_ent_solid_wire)* _3dsolid->num_wires);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < _3dsolid->num_wires ; i++)
        {
          ptx[i] = _3dsolid->wires[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns number of silhouettes
BITCODE_BL
dwg_ent_3dsolid_get_num_silhouettes(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->num_silhouettes;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}


/// Returns all silhouettes
dwg_ent_solid_silhouette *
dwg_ent_3dsolid_get_silhouettes(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  dwg_ent_solid_silhouette *ptx = (dwg_ent_solid_silhouette*)
    malloc(sizeof(dwg_ent_solid_silhouette)* _3dsolid->num_silhouettes);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < _3dsolid->num_silhouettes ; i++)
        {
          ptx[i] = _3dsolid->silhouettes[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns acis empty 2 value
unsigned char
dwg_ent_3dsolid_get_acis_empty2(const dwg_ent_3dsolid *_3dsolid, int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      return _3dsolid->acis_empty2;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets acis empty 2 value
void
dwg_ent_3dsolid_set_acis_empty2(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                                int *error)
{
  if (_3dsolid)
    {
      *error = 0;
      _3dsolid->acis_empty2 = acis;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                   FUNCTIONS FOR REGION ENTITY                     *
********************************************************************/

/// Returns acis empty value
unsigned char
dwg_ent_region_get_acis_empty(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->acis_empty;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets acis empty value
void
dwg_ent_region_set_acis_empty(dwg_ent_region *region, unsigned char acis,
                              int *error)
{
  if (region)
    {
      *error = 0;
      region->acis_empty = acis;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns version value
BITCODE_BS
dwg_ent_region_get_version(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets version value
void
dwg_ent_region_set_version(dwg_ent_region *region, BITCODE_BS version,
                           int *error)
{
  if (region)
    {
      *error = 0;
      region->version = version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns pointer
BITCODE_BL*
dwg_ent_region_get_block_size(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->block_size;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Returns acis data
char *
dwg_ent_region_get_acis_data(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->acis_data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Sets acis data
void
dwg_ent_region_set_acis_data(dwg_ent_region *region, char * data,
                             int *error)
{
  if (region)
    {
      *error = 0;
      region->acis_data = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns wireframe data present value
char
dwg_ent_region_get_wireframe_data_present(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->wireframe_data_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets wireframe data present value
void
dwg_ent_region_set_wireframe_data_present(dwg_ent_region *region, char data,
                                          int *error)
{
  if (region)
    {
      *error = 0;
      region->wireframe_data_present = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns point present value
char
dwg_ent_region_get_point_present(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->point_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets point present value
void
dwg_ent_region_set_point_present(dwg_ent_region *region, char point,
                                 int *error)
{
  if (region)
    {
      *error = 0;
      region->point_present = point;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns point value
void
dwg_ent_region_get_point(const dwg_ent_region *region, dwg_point_3d *point,
                         int *error)
{
  if (region && point)
    {
      *error = 0;
      point->x = region->point.x;
      point->y = region->point.y;
      point->z = region->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets point values
void
dwg_ent_region_set_point(dwg_ent_region *region, dwg_point_3d *point,
                         int *error)
{
  if (region && point)
    {
      *error = 0;
      region->point.x = point->x;
      region->point.y = point->y;
      region->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns number of isolines
BITCODE_BL
dwg_ent_region_get_num_isolines(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->num_isolines;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Sets number of isolines (apparently safe to set)
void
dwg_ent_region_set_num_isolines(dwg_ent_region *region, BITCODE_BL num, int *error)
{
  if (region)
    {
      *error = 0;
      region->num_isolines = num;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns isoline present value
char
dwg_ent_region_get_isoline_present(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->isoline_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets isoline present value
void
dwg_ent_region_set_isoline_present(dwg_ent_region *region, char iso,
                                   int *error)
{
  if (region)
    {
      *error = 0;
      region->isoline_present = iso;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns number of wires
BITCODE_BL
dwg_ent_region_get_num_wires(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->num_wires;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

//TODO dwg_ent_region_add_wire, dwg_ent_region_delete_wire

/// Returns all wires
dwg_ent_solid_wire *
dwg_ent_region_get_wires(const dwg_ent_region *region, int *error)
{
  dwg_ent_solid_wire *ptx = (dwg_ent_solid_wire*)
    malloc(sizeof(dwg_ent_solid_wire)* region->num_wires);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < region->num_wires ; i++)
        {
          ptx[i] = region->wires[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns number of silhouettes
BITCODE_BL
dwg_ent_region_get_num_silhouettes(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->num_silhouettes;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

//TODO dwg_ent_region_add_silhouette, dwg_ent_region_delete_silhouette

/// Returns all silhouettes
dwg_ent_solid_silhouette *
dwg_ent_region_get_silhouettes(const dwg_ent_region *region, int *error)
{
  dwg_ent_solid_silhouette *ptx = (dwg_ent_solid_silhouette*)
    malloc(sizeof(dwg_ent_solid_silhouette)* region->num_silhouettes);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < region->num_silhouettes ; i++)
        {
          ptx[i] = region->silhouettes[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns acis empty 2 value
unsigned char
dwg_ent_region_get_acis_empty2(const dwg_ent_region *region, int *error)
{
  if (region)
    {
      *error = 0;
      return region->acis_empty2;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets acis empty 2 value
void
dwg_ent_region_set_acis_empty2(dwg_ent_region *region, unsigned char acis,
                               int *error)
{
  if (region)
    {
      *error = 0;
      region->acis_empty2 = acis;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                    FUNCTIONS FOR BODY ENTITY                      *
********************************************************************/
/// Returns acis empty value
unsigned char
dwg_ent_body_get_acis_empty(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->acis_empty;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets acis empty value
void
dwg_ent_body_set_acis_empty(dwg_ent_body *body, unsigned char acis,
                            int *error)
{
  if (body)
    {
      *error = 0;
      body->acis_empty = acis;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns version value
BITCODE_BS
dwg_ent_body_get_version(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/// Sets version value
void
dwg_ent_body_set_version(dwg_ent_body *body, BITCODE_BS version, int *error)
{
  if (body)
    {
      *error = 0;
      body->version = version;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns pointer to body block size
BITCODE_BL *
dwg_ent_body_get_block_size(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->block_size;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Returns body acis data value
char *
dwg_ent_body_get_acis_data(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->acis_data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/// Sets acis data of body
void
dwg_ent_body_set_acis_data(dwg_ent_body *body, char * data, int *error)
{
  if (body)
    {
      *error = 0;
      body->acis_data = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns wireframe data present value
char
dwg_ent_body_get_wireframe_data_present(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->wireframe_data_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets wireframe data present value
void
dwg_ent_body_set_wireframe_data_present(dwg_ent_body *body, char data,
                                        int *error)
{
  if (body)
    {
      *error = 0;
      body->wireframe_data_present = data;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns point present value
char
dwg_ent_body_get_point_present(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->point_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets point present value
void
dwg_ent_body_set_point_present(dwg_ent_body *body, char point, int *error)
{
  if (body)
    {
      *error = 0;
      body->point_present = point;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns point values
void
dwg_ent_body_get_point(const dwg_ent_body *body, dwg_point_3d *point, int *error)
{
  if (body)
    {
      *error = 0;
      point->x = body->point.x;
      point->y = body->point.y;
      point->z = body->point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Sets point of body entity
void
dwg_ent_body_set_point(dwg_ent_body *body, dwg_point_3d *point, int *error)
{
  if (body)
    {
      *error = 0;
      body->point.x = point->x;
      body->point.y = point->y;
      body->point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns number of isolines
BITCODE_BL
dwg_ent_body_get_num_isolines(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->num_isolines;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Sets number of isolines (apparently safe to set)
void
dwg_ent_body_set_num_isolines(dwg_ent_body *body, BITCODE_BL num, int *error)
{
  if (body)
    {
      *error = 0;
      body->num_isolines = num;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// returns isoline present value
char
dwg_ent_body_get_isoline_present(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->isoline_present;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets isoline present value
void
dwg_ent_body_set_isoline_present(dwg_ent_body *body, char iso, int *error)
{
  if (body)
    {
      *error = 0;
      body->isoline_present = iso;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/// Returns number of wires
BITCODE_BL
dwg_ent_body_get_num_wires(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->num_wires;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Returns all wires of body
dwg_ent_solid_wire *
dwg_ent_body_get_wires(const dwg_ent_body *body, int *error)
{
  dwg_ent_solid_wire *ptx = (dwg_ent_solid_wire*)
    malloc(sizeof(dwg_ent_solid_wire)* body->num_wires);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < body->num_wires ; i++)
        {
          ptx[i] = body->wires[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns number of silhouettes value
BITCODE_BL
dwg_ent_body_get_num_silhouettes(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->num_silhouettes;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/// Returns all silhouettes for body entity
dwg_ent_solid_silhouette *
dwg_ent_body_get_silhouettes(const dwg_ent_body *body,
                            int *error)
{
  dwg_ent_solid_silhouette *ptx = (dwg_ent_solid_silhouette*)
    malloc(sizeof(dwg_ent_solid_silhouette)* body->num_silhouettes);
  if (ptx)
    {
      BITCODE_BL i;
      *error = 0;
      for (i = 0; i < body->num_silhouettes ; i++)
        {
          ptx[i] = body->silhouettes[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/// Returns acis empty2 value
unsigned char
dwg_ent_body_get_acis_empty2(const dwg_ent_body *body, int *error)
{
  if (body)
    {
      *error = 0;
      return body->acis_empty2;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/// Sets body acis empty2 value
void
dwg_ent_body_set_acis_empty2(dwg_ent_body *body, unsigned char acis,
                             int *error)
{
  if (body)
    {
      *error = 0;
      body->acis_empty2 = acis;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/*******************************************************************
*                    FUNCTIONS FOR TABLE ENTITY                     *
********************************************************************/

/** Sets _dwg_entity_TABLE::insertion_point, DXF 10.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_insertion_point(dwg_ent_table *restrict table,
                                  const dwg_point_3d *restrict point,
                                  int *restrict error)
{
  if (table && point)
    {
      *error = 0;
      table->insertion_point.x = point->x;
      table->insertion_point.y = point->y;
      table->insertion_point.z = point->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::insertion point, DXF 10.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_get_insertion_point(const dwg_ent_table *restrict table,
                                  dwg_point_3d *restrict point,
                                  int *restrict error)
{
  if (table && point)
    {
      *error = 0;
      point->x = table->insertion_point.x;
      point->y = table->insertion_point.y;
      point->z = table->insertion_point.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets _dwg_entity_TABLE::scale, DXF 41. if r13+
\param[in]  table      dwg_ent_table *
\param[in]  scale3d    dwg_point_3d *, scale in x, y, z
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_scale(dwg_ent_table *restrict table,
                        const dwg_point_3d *restrict scale3d,
                        int *restrict error)
{
  if (table && scale3d)
    {
      *error = 0;
      // set data_flags (for r2000+)
      if (scale3d->x == 1.0) {
        if (scale3d->y == 1.0 && scale3d->z == 1.0)
          table->data_flags = 3;
        else
          table->data_flags = 1;
      } else if (scale3d->x == scale3d->y && scale3d->x == scale3d->z)
        table->data_flags = 2;
      else
        table->data_flags = 0;

      table->scale.x = scale3d->x;
      table->scale.y = scale3d->y;
      table->scale.z = scale3d->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::scale, DXF 41. if r13+
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_get_scale(const dwg_ent_table *restrict table,
                        dwg_point_3d *restrict scale3d,
                        int *restrict error)
{
  if (table && scale3d)
    {
      *error = 0;
      scale3d->x = table->scale.x;
      scale3d->y = table->scale.y;
      scale3d->z = table->scale.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Sets _dwg_entity_TABLE::data_flags, if r2000+.
\param[in]  table      dwg_ent_table *
\param[in]  flags      0 - 3
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_flags(dwg_ent_table *restrict table, const unsigned char flags,
                             int *restrict error)
{
  if (table && flags <= 3)
    {
      *error = 0;
      table->data_flags = flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_flags, no DXF.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_get_data_flags(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_flags;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::rotation, DXF 50.
\param[in]  table      dwg_ent_table *
\param[in]  rotation   double
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_rotation(dwg_ent_table *restrict table, const BITCODE_BD rotation, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->rotation = rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::rotation, DXF 50.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_table_get_rotation(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->rotation;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets _dwg_entity_TABLE::extrusion, DXF 210.
\param[in]  table      dwg_ent_table *
\param[in]  vector      dwg_point_3d *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_extrusion(dwg_ent_table *restrict table, const dwg_point_3d *vector,
                            int *restrict error)
{
  if (table && vector)
    {
      *error = 0;
      table->extrusion.x = vector->x;
      table->extrusion.y = vector->y;
      table->extrusion.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::extrusion, DXF 210.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_get_extrusion(const dwg_ent_table *restrict table, dwg_point_3d *vector,
                            int *restrict error)
{
  if (table && vector)
    {
      *error = 0;
      vector->x = table->extrusion.x;
      vector->y = table->extrusion.y;
      vector->z = table->extrusion.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}


/** Returns _dwg_entity_TABLE::has_attribs boolean, DXF 66.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_has_attribs(dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_attribs;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

// needs to adjust handle array: add/delete
// TODO dwg_ent_table_add_owned, dwg_ent_table_delete_owned

/** Returns _dwg_entity_TABLE::num_owned, no DXF.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_table_get_num_owned(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->num_owned;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::flag_for_table_value, DXF 90.
\param[in]  table      dwg_ent_table *
\param[in]  value      short

    Bit flags, 0x06 (0x02 + 0x04): has block, 0x10: table direction, 0
    = up, 1 = down, 0x20: title suppressed. Normally 0x06 is always
    set.

\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_flag_for_table_value(dwg_ent_table *restrict table,
                                       const BITCODE_BS value, int *restrict error)
{
  if (table && value < 0x30)
    {
      *error = 0;
      table->flag_for_table_value = value;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::flag_for_table_value, DXF 90.
    \sa dwg_ent_table_set_flag_for_table_value
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_flag_for_table_value(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->flag_for_table_value;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::horiz_direction, DXF 11.
\param[in]  table      dwg_ent_table *
\param[in]  vector      dwg_point_3d *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_horiz_direction(dwg_ent_table *restrict table, const dwg_point_3d *vector,
                                  int *restrict error)
{
  if (table && vector)
    {
      *error = 0;
      table->horiz_direction.x = vector->x;
      table->horiz_direction.y = vector->y;
      table->horiz_direction.z = vector->z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::horiz_direction, DXF 11.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_get_horiz_direction(const dwg_ent_table *restrict table, dwg_point_3d *vector,
                                  int *restrict error)
{
  if (table && vector)
    {
      *error = 0;
      vector->x = table->horiz_direction.x;
      vector->y = table->horiz_direction.y;
      vector->z = table->horiz_direction.z;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::num_cols number of columns, DXF 91.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_table_get_num_cols(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->num_cols;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns _dwg_entity_TABLE::num_cols number of rows, DXF 92.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_table_get_num_rows(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->num_rows;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

//  TODO dwg_ent_table_add_col, dwg_ent_table_delete_col
//  TODO dwg_ent_table_add_row, dwg_ent_table_delete_row


/** Returns array of _dwg_entity_TABLE::col_widths, DXF 142
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
double *
dwg_ent_table_get_col_widths(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->col_widths;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Returns array of _dwg_entity_TABLE::row_heights, DXF 141
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
double *
dwg_ent_table_get_row_heights(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->row_heights;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Returns _dwg_entity_TABLE::has_table_overrides boolean, no DXF.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_B
dwg_ent_table_has_table_overrides(dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_table_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::table_flag_override, DXF 90.
\param[in]  table      dwg_ent_table *
\param[in]  override   0 - 0x7fffff
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_table_flag_override(dwg_ent_table *restrict table, const BITCODE_BL override,
                                      int *restrict error)
{
  if (table != NULL && override < 0x800000)
    {
      *error = 0;
      table->table_flag_override = override;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::table_flag_override, DXF 93.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_table_get_table_flag_override(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->table_flag_override;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::title_suppressed, DXF 280.
\param[in]  table      dwg_ent_table *
\param[in]  yesno      0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_suppressed(dwg_ent_table *restrict table, const unsigned char yesno,
                                   int *restrict error)
{
  if (table != NULL && yesno <= 1)
    {
      *error = 0;
      if (yesno) table->table_flag_override |= 0x1;
      table->title_suppressed = yesno;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_suppressed, DXF 280.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_get_title_suppressed(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_suppressed;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::header_suppressed, DXF 281.
\param[in]  table      dwg_ent_table *
\param[in]  header     0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_suppressed(dwg_ent_table *restrict table, const unsigned char header,
                                    int *restrict error)
{
  if (table && header <= 1)
    {
      *error = 0;
      table->header_suppressed = header;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_suppressed, DXF 281.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_get_header_suppressed(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_suppressed;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::flow_direction, DXF 70.
\param[in]  table      dwg_ent_table *
\param[in]  dir        short?
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_flow_direction(dwg_ent_table *restrict table, const BITCODE_BS dir,
                                 int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (dir) table->table_flag_override |= 0x4;
      table->flow_direction = dir;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::flow_direction, DXF 70.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_flow_direction(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->flow_direction;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::horiz_cell_margin, DXF 41.
\param[in]  table      dwg_ent_table *
\param[in]  margin     double
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_horiz_cell_margin(dwg_ent_table *restrict table, const BITCODE_BD margin,
                                    int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (margin > 0.0) table->table_flag_override |= 0x8;
      else table->table_flag_override &= ~0x8;
      table->horiz_cell_margin = margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::horiz_cell_margin, DXF 41.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_table_get_horiz_cell_margin(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->horiz_cell_margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets _dwg_entity_TABLE::vert_cell_margin, DXF 41.
\param[in]  table      dwg_ent_table *
\param[in]  margin     double
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_vert_cell_margin(dwg_ent_table *restrict table, const BITCODE_BD margin,
                                   int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (margin > 0.0) table->table_flag_override |= 0x10;
      else table->table_flag_override &= ~0x10;
      table->vert_cell_margin = margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::vert_cell_margin, DXF 41.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_table_get_vert_cell_margin(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->vert_cell_margin;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets _dwg_entity_TABLE::title_row_fill_none, DXF 283.
\param[in]  table      dwg_ent_table *
\param[in]  fill       ?
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_row_fill_none(dwg_ent_table *restrict table, const unsigned char fill,
                                      int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (fill) table->table_flag_override |= 0x100;
      table->title_row_fill_none = fill;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_row_fill_none, DXF 283.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_get_title_row_fill_none(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_row_fill_none;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::header_row_fill_none, DXF 283.
\param[in]  table      dwg_ent_table *
\param[in]  fill       ?
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_row_fill_none(dwg_ent_table *restrict table,
                                       const unsigned char fill, int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (fill) table->table_flag_override |= 0x200;
      table->header_row_fill_none = fill;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_row_fill_none, DXF 283.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_get_header_row_fill_none(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_row_fill_none;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::data_row_fill_none, DXF 283.
\param[in]  table      dwg_ent_table *
\param[in]  fill       ?
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_row_fill_none(dwg_ent_table *restrict table, const unsigned char fill,
                                     int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (fill) table->table_flag_override |= 0x400;
      table->data_row_fill_none = fill;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_row_fill_none, DXF 283.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_get_data_row_fill_none(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_row_fill_none;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::title_row_align, DXF 170.
    and possibly enables bitmask 0x4000 of
    _dwg_entity_TABLE::table_flag_override, DXF 93 .
\param[in]  table      dwg_ent_table *
\param[in]  align      short?
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_row_align(dwg_ent_table *restrict table, const unsigned char align,
                                  int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (align) table->table_flag_override |= 0x4000;
      table->title_row_align = align;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_row_align, DXF 170.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_row_align(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_row_align;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_row_align, DXF 170.
    and possibly enables bitmask 0x8000 of _dwg_entity_TABLE::table_flag_override, DXF 93 .
\param[in]  table      dwg_ent_table *
\param[in]  align      short?
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_row_align(dwg_ent_table *restrict table, const BITCODE_BS align,
                                   int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (align) table->table_flag_override |= 0x8000;
      table->header_row_align = align;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_row_align, DXF 170.
    Might be ignored if bit of 0x8000 of table_flag_override DXF 93 is not set.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_row_align(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_row_align;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_row_align, DXF 170,
    and possibly table_flag_override 93.

    TODO: possible values?
\param[in]  table      dwg_ent_table *
\param[in]  align      short?
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_row_align(dwg_ent_table *restrict table, const BITCODE_BS align,
                                 int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (align) table->table_flag_override |= 0x10000;
      table->data_row_align = align;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_row_align, DXF 170.
    Might be ignored if bit of 0x10000 of table_flag_override, DXF 93 is not set.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_row_align(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_row_align;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_row_height, DXF 140.
    and en/disables the _dwg_entity_TABLE::table_flag_override
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_row_height(dwg_ent_table *restrict table, const BITCODE_BD height,
                                   int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (height > 0.0) table->table_flag_override |= 0x100000;
      else table->table_flag_override &= ~0x100000;
      table->title_row_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_row_height, DXF 140.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_table_get_title_row_height(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_row_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets _dwg_entity_TABLE::header_row_height, DXF 140.
    and en/disables the _dwg_entity_TABLE::table_flag_override.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_row_height(dwg_ent_table *restrict table, const BITCODE_BD height,
                                    int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (height > 0.0) table->table_flag_override |= 0x200000;
      else table->table_flag_override &= ~0x200000;
      table->header_row_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_row_height, DXF 140.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_table_get_header_row_height(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_row_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Sets _dwg_entity_TABLE::data_row_height, DXF 140.
    and en/disables the _dwg_entity_TABLE::table_flag_override.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_row_height(dwg_ent_table *restrict table, BITCODE_BD height,
                                  int *restrict error)
{
  if (table)
    {
      *error = 0;
      if (height > 0.0) table->table_flag_override |= 0x400000;
      else table->table_flag_override &= ~0x400000;
      table->data_row_height = height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_row_height, DXF 140.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BD
dwg_ent_table_get_data_row_height(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_row_height;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return bit_nan();
    }
}

/** Returns _dwg_entity_TABLE::has_border_color_overrides, if DXF 94 > 0.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_has_border_color_overrides(dwg_ent_table *restrict table,
                                         int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_border_color_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::border_color_overrides_flag, DXF 94.
    \sa dwg_ent_table_get_border_color_overrides_flag
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_border_color_overrides_flag(dwg_ent_table *restrict table,
                                              const BITCODE_BL overrides,
                                              int *restrict error)
{
  if (table && overrides <= 1)
    {
      *error = 0;
      table->border_color_overrides_flag = overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::border_color_overrides_flag, DXF 94.

    Bitmask 1: has title_horiz_top_color
            2: has title_horiz_ins_color
            4: has title_horiz_bottom_color
            8: has title_vert_left_color
           10: has title_vert_ins_color
           20: has title_vert_right_color
           ...
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
 */
BITCODE_BL
dwg_ent_table_get_border_color_overrides_flag(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->border_color_overrides_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}


/** Returns _dwg_entity_TABLE::has_border_lineweight_overrides, if DXF 95 > 0
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_has_border_lineweight_overrides(dwg_ent_table *restrict table,
                                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_border_lineweight_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::border_lineweight_overrides_flag, DXF 95.
\param[in]  table      dwg_ent_table *
\param[in]  overrides  0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_border_lineweight_overrides_flag(dwg_ent_table *restrict table,
                                                   const BITCODE_BL overrides,
                                                   int *restrict error)
{
  if (table && overrides <= 1)
    {
      *error = 0;
      table->border_lineweight_overrides_flag = overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::border_lineweight_overrides_flag, DXF 95.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_table_get_border_lineweight_overrides_flag(const dwg_ent_table *restrict table,
                                                   int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->border_lineweight_overrides_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_top_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_horiz_top_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_horiz_top_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_top_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_horiz_top_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_top_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_horiz_ins_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_horiz_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_horiz_ins_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_bottom_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_horiz_bottom_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_horiz_bottom_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_bottom_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_linewt(const dwg_ent_table *restrict table,
                                            int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_bottom_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_left_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_vert_left_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_vert_left_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_left_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_vert_left_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_left_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_vert_ins_linewt(dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_vert_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_vert_ins_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_right_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_vert_right_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->title_vert_right_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_right_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_vert_right_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_right_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_top_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_horiz_top_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_horiz_top_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_top_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_horiz_top_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_top_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_horiz_ins_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_horiz_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_horiz_ins_linewt(const dwg_ent_table *restrict table,
                                          int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_bottom_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_horiz_bottom_linewt(dwg_ent_table *restrict table,
                                             const BITCODE_BS linewt,
                                             int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_horiz_bottom_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_bottom_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_linewt(const dwg_ent_table *restrict table,
                                             int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_bottom_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_left_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_vert_left_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_vert_left_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_left_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_vert_left_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_left_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_vert_ins_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_vert_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_ins_linewt
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_vert_ins_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_right_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_vert_right_linewt(dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->header_vert_right_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_right_linewt
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_vert_right_linewt(const dwg_ent_table *restrict table,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_right_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_top_linewt, DXF ?.
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_horiz_top_linewt(dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_horiz_top_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_top_linewt, DXF ?.
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_horiz_top_linewt(const dwg_ent_table *restrict table,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_top_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_ins_linewt, DXF ?.
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_horiz_ins_linewt(dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_horiz_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_ins_linewt, DXF ?.
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_horiz_ins_linewt(const dwg_ent_table *restrict table,
                                        int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_bottom_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_horiz_bottom_linewt(dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_horiz_bottom_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_bottom_linewt
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_linewt(const dwg_ent_table *restrict table,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_bottom_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_ins_linewt
\param[in]  table      dwg_ent_table *
\param[in]  linewt     short
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_vert_ins_linewt(dwg_ent_table *restrict table,
                                       BITCODE_BS linewt, int *restrict error)
{
  if (table)
    {
      *error = 0;
      table->data_horiz_ins_linewt = linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_ins_linewt
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_vert_ins_linewt(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_ins_linewt;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}


/** Returns _dwg_entity_TABLE::has_border_visibility_overrides
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
unsigned char
dwg_ent_table_has_border_visibility_overrides(dwg_ent_table *restrict table,
                                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->has_border_visibility_overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return '\0';
    }
}

/** Sets _dwg_entity_TABLE::border_visibility_overrides_flag, DXF 96.
\param[in]  table      dwg_ent_table *
\param[in]  overrides  0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_border_visibility_overrides_flag(dwg_ent_table *restrict table,
                                                   BITCODE_BL overrides,
                                                   int *restrict error)
{
  if (table && overrides <= 1)
    {
      *error = 0;
      table->border_visibility_overrides_flag = overrides;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::border_visibility_overrides_flag, DXF 96.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_ent_table_get_border_visibility_overrides_flag(const dwg_ent_table *restrict table,
                                                   int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->border_visibility_overrides_flag;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_top_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_horiz_top_visibility(dwg_ent_table *restrict table,
                                             BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table && visibility >=0 && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x1;
      table->title_horiz_top_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_top_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_horiz_top_visibility(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_top_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_horiz_ins_visibility(dwg_ent_table *restrict table,
                                             BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x2;
      table->title_horiz_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_horiz_ins_visibility(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_horiz_bottom_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_horiz_bottom_visibility(dwg_ent_table *restrict table,
                                                BITCODE_BS visibility,
                                                int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x4;
      table->title_horiz_bottom_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_horiz_bottom_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_visibility(const dwg_ent_table *restrict table,
                                                int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_horiz_bottom_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_left_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_vert_left_visibility(dwg_ent_table *restrict table,
                                             BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x8;
      table->title_vert_left_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_left_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_vert_left_visibility(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_left_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_vert_ins_visibility(dwg_ent_table *restrict table,
                                            BITCODE_BS visibility,
                                            int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x10;
      table->title_vert_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_vert_ins_visibility(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::title_vert_right_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_title_vert_right_visibility(dwg_ent_table *restrict table,
                                              BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x20;
      table->title_vert_right_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::title_vert_right_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_title_vert_right_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->title_vert_right_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_top_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_horiz_top_visibility(dwg_ent_table *restrict table,
                                              BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x40;
      table->header_horiz_top_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_top_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_horiz_top_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_top_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_horiz_ins_visibility(dwg_ent_table *restrict table,
                                              BITCODE_BS visibility,
                                              int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x80;
      table->header_horiz_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_horiz_ins_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_horiz_bottom_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_horiz_bottom_visibility(dwg_ent_table *restrict table,
                                                 BITCODE_BS visibility,
                                                 int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x100;
      table->header_horiz_bottom_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_horiz_bottom_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_visibility(const dwg_ent_table *restrict table,
                                                 int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_horiz_bottom_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_left_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_vert_left_visibility(dwg_ent_table *restrict table,
                                               BITCODE_BS visibility,
                                               int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x200;
      table->header_vert_left_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_left_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_vert_left_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_left_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::header_vert_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_vert_ins_visibility(dwg_ent_table *restrict table,
                                             BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x400;
      table->header_vert_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_ins_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_vert_ins_visibility(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets data header vert right visibility
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_header_vert_right_visibility(dwg_ent_table *restrict table,
                                               BITCODE_BS visibility,
                                               int *restrict error)
{
  if (table && visibility >=0 && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x800;
      table->header_vert_right_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::header_vert_right_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_header_vert_right_visibility(const dwg_ent_table *restrict table,
                                               int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->header_vert_right_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_top_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_horiz_top_visibility(dwg_ent_table *restrict table,
                                            BITCODE_BS visibility,
                                            int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x1000;
      table->data_horiz_top_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_top_visibility, DXF ??.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_horiz_top_visibility(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_top_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_ins_visibility, DXF ?
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_horiz_ins_visibility(dwg_ent_table *restrict table,
                                            BITCODE_BS visibility,
                                            int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x2000;
      table->data_horiz_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_ins_visibility, DXF ?
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_horiz_ins_visibility(const dwg_ent_table *restrict table, int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_horiz_bottom_visibility, DXF ?.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_horiz_bottom_visibility(dwg_ent_table *restrict table,
                                               BITCODE_BS visibility,
                                               int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x4000;
      table->data_horiz_bottom_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_horiz_bottom_visibility, DXF ?.
\param[in]  table      dwg_ent_table *
\param[out] error      set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_visibility(const dwg_ent_table *restrict table,
                                               int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_horiz_bottom_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_vert_left_visibility, DXF ?.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_vert_left_visibility(dwg_ent_table *restrict table,
                                           const BITCODE_BS visibility,
                                           int *restrict error)
{
  if (table && visibility >=0 && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x8000;
      table->data_vert_left_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_vert_left_visibility, DXF ?.
\param[in]  table  dwg_ent_table *
\param[out] error  set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_vert_left_visibility(const dwg_ent_table *restrict table,
                                            int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_left_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets _dwg_entity_TABLE::data_vert_ins_visibility, DXF ?.
\param[in]  table      dwg_ent_table *
\param[in]  visibility 0 or 1
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_vert_ins_visibility(dwg_ent_table *restrict table,
                                           const BITCODE_BS visibility,
                                           int *restrict error)
{
  if (table && visibility >=0 && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x10000;
      table->data_vert_ins_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data_vert_ins_visibility, DXF ?.
\param[in]  table  dwg_ent_table *
\param[out] error  set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_vert_ins_visibility(const dwg_ent_table *restrict table,
                                           int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_ins_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/** Sets the table data vert right visibility.
    Bit 0x20000 of border_visibility override flag, DXF 96
\param[in]  table      dwg_ent_table *
\param[in]  visibility short: 0 = visible, 1 = invisible
\param[out] error      set to 0 for ok, 1 on error
*/
void
dwg_ent_table_set_data_vert_right_visibility(dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error)
{
  if (table && visibility <= 1)
    {
      *error = 0;
      if (visibility) table->border_visibility_overrides_flag |= 0x20000;
      table->data_vert_right_visibility = visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
    }
}

/** Returns _dwg_entity_TABLE::data vert right visibility, DXF ?.
    Bit 0x20000 of border_visibility override flag, DXF 96
\param[in]  table  dwg_ent_table *
\param[out] error  set to 0 for ok, 1 on error
*/
BITCODE_BS
dwg_ent_table_get_data_vert_right_visibility(const dwg_ent_table *restrict table,
                                             int *restrict error)
{
  if (table)
    {
      *error = 0;
      return table->data_vert_right_visibility;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0;
    }
}

/*******************************************************************
*                    FUNCTIONS FOR TABLES                          *
*        First the special tables: BLOCKS and LAYER                *
********************************************************************/

/*******************************************************************
*               FUNCTIONS FOR BLOCK_CONTROL OBJECT                  *
********************************************************************/

/** Returns the block control object from the block header
\code Usage: dwg_obj_block_control *blk = dwg_block_header_get_block_control(hdr, &error);
\endcode
\param[in]  block_header
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_obj_block_control *
dwg_block_header_get_block_control(const dwg_obj_block_header* block_header,
                                   int *error)
{
  if (block_header &&
      block_header->block_control &&
      block_header->block_control->obj &&
      block_header->block_control->obj->type == DWG_TYPE_BLOCK_CONTROL &&
      block_header->block_control->obj->tio.object)
    {
      *error = 0;
      return block_header->block_control->obj->tio.object->tio.BLOCK_CONTROL;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid arg", __FUNCTION__)
      return NULL;
    }
}

/** Extracts and returns all block headers as references
\param[in]  ctrl
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref **
dwg_obj_block_control_get_block_headers(const dwg_obj_block_control *ctrl,
                                        int *error)
{
  dwg_object_ref **ptx = (dwg_object_ref**)
    malloc(ctrl->num_entries * sizeof(Dwg_Object_Ref *));
  if (ptx)
    {
      BITCODE_BS i;
      *error = 0;
      for (i=0; i < ctrl->num_entries; i++)
        {
          ptx[i] = ctrl->block_headers[i];
        }
      return ptx;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: null malloc", __FUNCTION__)
      return NULL;
    }
}

/** Returns number of blocks
\param[in]  ctrl
\param[out] error  set to 0 for ok, >0 if not found.
*/
BITCODE_BL
dwg_obj_block_control_get_num_entries(const dwg_obj_block_control *ctrl, int *error)
{
  if (ctrl)
    {
      *error = 0;
      return ctrl->num_entries;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return 0L;
    }
}

/** Returns reference to model space block
\param[in]  ctrl
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref *
dwg_obj_block_control_get_model_space(const dwg_obj_block_control *ctrl, int *error)
{
  if (ctrl)
    {
      *error = 0;
      return ctrl->model_space;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Returns reference to paper space block
\param[in]  ctrl
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref *
dwg_obj_block_control_get_paper_space(const dwg_obj_block_control *ctrl, int *error)
{
  if (ctrl)
    {
      *error = 0;
      return ctrl->paper_space;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                FUNCTIONS FOR BLOCK_HEADER OBJECT                  *
********************************************************************/

/** Get name of the block header (utf-8 encoded)
\code Usage: char* block_name = dwg_obj_block_header_get_name(hdr, &error);
\endcode
\param[in]  hdr
\param[out] error  set to 0 for ok, >0 if not found.
*/
char *
dwg_obj_block_header_get_name(const dwg_obj_block_header *hdr, int *error)
{
  if (hdr)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)hdr->entry_name);
      else
        return hdr->entry_name;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/** Returns 1st block header present in the dwg file.
    Usually the model space block.
\code Usage: dwg_obj_block_header = dwg_get_block_header(dwg, &error);
\endcode
\param[in]  dwg
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_obj_block_header *
dwg_get_block_header(dwg_data *dwg, int *error)
{
  Dwg_Object *obj;
  Dwg_Object_BLOCK_HEADER *blk;

  *error = 0;
  if (!dwg || dwg->num_classes > 1000 || dwg->num_objects > 0xfffffff)
    {
      *error = 1;
      return NULL;
    }
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;

  obj = &dwg->object[0];
  while (obj && obj->type != DWG_TYPE_BLOCK_HEADER)
    {
      if (obj->size > 0xffff)
        {
          *error = 2;
          return NULL;
        }
      obj = dwg_next_object(obj);
    }
  if (obj && DWG_TYPE_BLOCK_HEADER == obj->type)
    {
      if (obj->size > 0xffff)
        {
          *error = 2;
          return NULL;
        }
      blk = obj->tio.object->tio.BLOCK_HEADER;
      if (!strcmp(blk->entry_name, "*Paper_Space"))
        dwg->pspace_block = obj;
      else if (!strcmp(blk->entry_name, "*Model_Space"))
        dwg->mspace_block = obj;
      return blk;
    }
  else
    {
      *error = 3;
      LOG_ERROR("%s: BLOCK_HEADER not found", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*                    FUNCTIONS FOR LAYER OBJECT                     *
********************************************************************/

/** Get name of the layer (utf-8 encoded)
\code Usage: char* layer_name = dwg_obj_layer_get_name(layer, &error);
\endcode
\param[in]  layer
\param[out] error  set to 0 for ok, >0 if not found.
*/
char *
dwg_obj_layer_get_name(const dwg_obj_layer *restrict layer, int *restrict error)
{
  if (layer)
    {
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)layer->entry_name);
      else
        return layer->entry_name;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty arg", __FUNCTION__)
      return NULL;
    }
}

/*******************************************************************
*           GENERIC FUNCTIONS FOR OTHER TABLE OBJECTS              *
********************************************************************/

/** Get number of table entries from the generic table control object.
\code Usage: char* name = dwg_obj_tablectrl_get_num_entries(obj, &error);
\endcode
\param[in]  obj    a TABLE_CONTROL dwg_object*
\param[out] error  set to 0 for ok, >0 if not found.
*/
BITCODE_BL
dwg_obj_tablectrl_get_num_entries(const dwg_object *restrict obj, int *restrict error)
{
  if (obj &&
      obj->supertype == DWG_SUPERTYPE_OBJECT &&
      dwg_obj_is_control(obj))
    {
      // HACK: we can guarantee that num_entries is always the first field.
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      *error = 0;
      return ctrl->num_entries;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid table control arg %p, type: 0x%x",
                __FUNCTION__, obj, obj ? obj->type : 0)
      return 0;
    }
}

/** Get all table entries from the generic table control object.
\code Usage: dwg_object_ref **refs = dwg_obj_tablectrl_get_entries(obj, &error);
\endcode
\param[in]  obj    a TABLE_CONTROL dwg_object*
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref **
dwg_obj_tablectrl_get_entries(const dwg_object *restrict obj, int *restrict error)
{
  if (obj &&
      obj->supertype == DWG_SUPERTYPE_OBJECT &&
      dwg_obj_is_control(obj))
    {
      // HACK: we can guarantee a common layout of the common fields
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      return ctrl->styles;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid table control arg %p, type: 0x%x",
                __FUNCTION__, obj, obj ? obj->type : 0)
      return NULL;
    }
}

/** Get the nth table entry from the generic table control object.
\code Usage: dwg_object_ref *ref = dwg_obj_tablectrl_get_entry(obj, 0, &error);
\endcode
\param[in]  obj    a TABLE_CONTROL dwg_object*
\param[in]  index  BITCODE_BS
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref *
dwg_obj_tablectrl_get_entry(const dwg_object *restrict obj, const BITCODE_BS index,
                            int *restrict error)
{
  if (obj &&
      obj->supertype == DWG_SUPERTYPE_OBJECT &&
      dwg_obj_is_control(obj))
    {
      // HACK: we can guarantee a common layout of the common fields
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      BITCODE_BS count = ctrl->num_entries;
      if (index < count)
        {
          *error = 0;
          return ctrl->styles[index];
        }
      else
        {
          *error = 2;
          LOG_ERROR("%s: index %d out of bounds %d",
                    __FUNCTION__, index, count);
          return NULL;
        }
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid table control arg %p, type: 0x%x",
                __FUNCTION__, obj, obj ? obj->type : 0)
      return NULL;
    }
}

/** Get the null_handle from the generic table control object.
\code Usage: dwg_object_ref *ref = dwg_obj_tablectrl_get_null_handle(obj, &error);
\endcode
\param[in]  obj    a TABLE_CONTROL dwg_object*
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref *
dwg_obj_tablectrl_get_null_handle(const dwg_object *restrict obj,
                                  int *restrict error)
{
  if (obj &&
      obj->supertype == DWG_SUPERTYPE_OBJECT &&
      dwg_obj_is_control(obj))
    {
      // HACK: we can guarantee a common layout of the common fields
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      return ctrl->null_handle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid table control arg %p, type: 0x%x",
                __FUNCTION__, obj, obj ? obj->type : 0)
      return NULL;
    }
}

/** Get the xdicobjhandle from the generic table control object.
\code Usage: dwg_object_ref *ref = dwg_obj_tablectrl_get_xdicobjhandle(obj, &error);
\endcode
\param[in]  obj    a TABLE_CONTROL dwg_object*
\param[out] error  set to 0 for ok, >0 if not found.
*/
dwg_object_ref *
dwg_obj_tablectrl_get_xdicobjhandle(const dwg_object *restrict obj,
                                    int *restrict error)
{
  if (obj &&
      obj->supertype == DWG_SUPERTYPE_OBJECT &&
      dwg_obj_is_control(obj))
    {
      // HACK: we can guarantee a common layout of the common fields
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      return ctrl->xdicobjhandle;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid table control arg %p, type: 0x%x",
                __FUNCTION__, obj, obj ? obj->type : 0)
      return NULL;
    }
}

/** Get the objid from the generic table control object.
\code Usage: objid = dwg_obj_tablectrl_get_objid(obj, &error);
\endcode
\param[in]  obj    a TABLE_CONTROL dwg_object*
\param[out] error  set to 0 for ok, >0 if not found.
*/
long unsigned int
dwg_obj_tablectrl_get_objid(const dwg_object *restrict obj,
                            int *restrict error)
{
  if (obj &&
      obj->supertype == DWG_SUPERTYPE_OBJECT &&
      dwg_obj_is_control(obj))
    {
      // HACK: we can guarantee a common layout of the common fields
      Dwg_Object_STYLE_CONTROL *ctrl = obj->tio.object->tio.STYLE_CONTROL;
      return ctrl->objid;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid table control arg %p, type: 0x%x",
                __FUNCTION__, obj, obj ? obj->type : 0)
      return 0;
    }
}

/** Get name of the generic table entry (utf-8 encoded)
\code Usage: char* name = dwg_obj_table_get_name(obj, &error);
\endcode
\param[in]  obj    a TABLE dwg_object*
\param[out] error  set to 0 for ok, >0 if not found.
*/
char *
dwg_obj_table_get_name(const dwg_object *restrict obj, int *restrict error)
{
  if (obj &&
      obj->supertype == DWG_SUPERTYPE_OBJECT &&
      dwg_obj_is_table(obj))
    {
      // HACK: we can guarantee that the entry_name is always the first field,
      // by using COMMON_TABLE_FLAGS.
      Dwg_Object_STYLE *table = obj->tio.object->tio.STYLE;
      *error = 0;
      if (dwg_version >= R_2007)
        return bit_convert_TU((BITCODE_TU)table->entry_name);
      else
        return table->entry_name;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty or invalid table arg %p, type: 0x%x",
                __FUNCTION__, obj, obj ? obj->type : 0)
      return NULL;
    }
}

/*******************************************************************
*                    FUNCTIONS FOR DWG OBJECT                       *
********************************************************************/

/** Returns the number of classes or 0
\code Usage: unsigned num_classes = dwg_get_num_classes(dwg);
\endcode
\param[in]  dwg   dwg_data*
*/
unsigned int
dwg_get_num_classes(const dwg_data *dwg)
{
  if (!dwg)
    return 0;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  return dwg->num_classes;
}

/** Returns the nth class or NULL
\code Usage: dwg_object* obj = dwg_get_object(dwg, 0);
\endcode
\param[in]  dwg   dwg_data*
\param[in]  index
*/
dwg_class *
dwg_get_class(const dwg_data *dwg, unsigned int index)
{
  if (!dwg)
    return NULL;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  return (index < dwg->num_classes) ? &dwg->dwg_class[index] : NULL;
}

/** Returns the nth object or NULL
\code Usage: dwg_object* obj = dwg_get_object(dwg, 0);
\endcode
\param[in]  dwg   dwgdata*
\param[in]  index
*/
dwg_object *
dwg_get_object(dwg_data *dwg, long unsigned int index)
{
  if (!dwg)
    return NULL;
  if (dwg_version == R_INVALID)
    dwg_version = (Dwg_Version_Type)dwg->header.version;
  return (index < dwg->num_objects) ? &dwg->object[index] : NULL;
}

/** Returns the object bitsize or 0
\code Usage: long bitsize = dwg_obj_get_bitsize(obj);
\endcode
\param[in]  obj   dwg_object*
*/
BITCODE_RL
dwg_obj_get_bitsize(const dwg_object *obj)
{
  return obj ? obj->bitsize : 0;
}

/** Returns the entity bitsize
\code Usage: long bitsize = dwg_ent_get_bitsize(ent, &error);
\endcode
\param[in]  ent   dwg_obj_ent*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_RL
dwg_ent_get_bitsize(const dwg_obj_ent *ent, int *error)
{
  if (!ent || !ent->object || ent->object->supertype != DWG_SUPERTYPE_ENTITY) {
    *error = 1;
    return 0;
  } else {
    *error = 0;
    return ent->object->bitsize;
  }
}
/** Returns the number of object EED structures.
See dwg_object_to_object how to get the obj.
\code Usage: int num_eed = dwg_obj_get_num_eed(ent,&error);
\endcode
\param[in]  obj    dwg_obj_obj*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
unsigned int
dwg_obj_get_num_eed(const dwg_obj_obj *obj, int *error)
{
  if (!obj || !obj->object || obj->object->supertype != DWG_SUPERTYPE_OBJECT) {
    *error = 1;
    LOG_ERROR("%s: empty or invalid obj", __FUNCTION__)
    return 0;
  } else {
    *error = 0;
    return obj->num_eed;
  }
}
/** Returns the number of entity EED structures
See dwg_object_to_entity how to get the ent.
\code Usage: int num_eed = dwg_ent_get_num_eed(ent,&error);
\endcode
\param[in]  ent   dwg_obj_ent*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
unsigned int
dwg_ent_get_num_eed(const dwg_obj_ent *ent, int *error)
{
  if (!ent || !ent->object || ent->object->supertype != DWG_SUPERTYPE_ENTITY) {
    *error = 1;
    LOG_ERROR("%s: empty or invalid ent", __FUNCTION__)
    return 0;
  } else {
    *error = 0;
    return ent->num_eed;
  }
}
/** Returns the nth EED structure.
\code Usage: dwg_entity_eed *eed = dwg_ent_get_eed(ent,0,&error);
\endcode
\param[in]  ent    dwg_obj_ent*
\param[in]  index  [0 - num_eed-1]
\param[out] error  set to 0 for ok, 1 if ent == NULL or 2 if index out of bounds.
*/
dwg_entity_eed *
dwg_ent_get_eed(const dwg_obj_ent *ent, const unsigned int index, int *error)
{
  if (!ent || !ent->object || ent->object->supertype != DWG_SUPERTYPE_ENTITY) {
    *error = 1;
    LOG_ERROR("%s: empty or invalid ent", __FUNCTION__)
    return NULL;
  }
  else if (index >= ent->num_eed) {
    *error = 2;
    return NULL;
  }
  else {
    *error = 0;
    return &ent->eed[index];
  }
}

/** Returns the data union of the nth EED structure.
\code Usage: dwg_entity_eed_data *eed = dwg_ent_get_eed_data(ent,0,&error);
\endcode
\param[in]  ent    dwg_obj_ent*
\param[in]  index  [0 - num_eed-1]
\param[out] error  set to 0 for ok, 1 if ent == NULL or 2 if index out of bounds.
*/
dwg_entity_eed_data *
dwg_ent_get_eed_data(const dwg_obj_ent *ent, const unsigned int index, int *error)
{
  if (!ent || !ent->object || ent->object->supertype != DWG_SUPERTYPE_ENTITY) {
    *error = 1;
    LOG_ERROR("%s: empty or invalid ent", __FUNCTION__)
    return NULL;
  }
  else if (index >= ent->num_eed) {
    *error = 2;
    return NULL;
  }
  else {
    *error = 0;
    return ent->eed[index].data;
  }
}

/** Returns the global index/objid in the list of all objects.
    This is the same as a dwg_handle absolute_ref value.
\code Usage: int index = dwg_obj_object_get_index(obj, &error);
\endcode
\param[in]  obj   dwg_object*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
unsigned int
dwg_obj_object_get_index(const dwg_object *obj, int *error)
{
  if (obj)
    {
      *error = 0;
      if (dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return obj->index;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty obj", __FUNCTION__)
      return 0;
    }
}

/** Returns dwg_handle* from dwg_object*
\code Usage: dwg_handle* handle = dwg_object_get_handle(obj, &error);
\endcode
\param[in]  obj   dwg_object*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_handle *
dwg_obj_get_handle(dwg_object *obj, int *error)
{
  if (obj)
    {
      *error = 0;
      if (dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return &(obj->handle);
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty obj", __FUNCTION__)
      return NULL;
    }
}

/** Returns dwg_obj_obj* from dwg_object*
\code Usage: dwg_obj_obj ent = dwg_object_to_object(obj, &error);
\endcode
\param[in]  obj   dwg_object*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_obj_obj *
dwg_object_to_object(dwg_object *obj, int *error)
{
  if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT)
    {
      *error = 0;
      if (dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return obj->tio.object;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: Empty or invalid obj", __FUNCTION__)
      return NULL;
    }
}

/** Returns dwg_obj_ent* from dwg object
\code Usage : dwg_obj_ent* ent = dwg_object_to_entity(obj, &error);
\endcode
\param obj   dwg_object*
\param error
*/
dwg_obj_ent *
dwg_object_to_entity(dwg_object *obj, int *error)
{
  if (obj && obj->supertype == DWG_SUPERTYPE_ENTITY)
    {
      *error = 0;
      if (dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return obj->tio.entity;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: Empty or invalid obj", __FUNCTION__)
      return NULL;
    }
}

/** Returns object from reference or NULL
\code Usage: dwg_object obj = dwg_obj_reference_get_object(obj, &error);
\endcode
\param[in]  ref   dwg_object_ref*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
dwg_object *
dwg_obj_reference_get_object(const dwg_object_ref *ref, int *error)
{
  if (ref)
    {
      *error = 0;
      return ref->obj;
    }
  else
    {
      *error = 1;
      LOG_ERROR("%s: empty ref", __FUNCTION__)
      return NULL;
    }
}

/* Returns absolute reference
\code Usage: BITCODE_BL ref = dwg_obj_ref_get_abs_ref(obj, &error);
\endcode
\param[in]  ref   dwg_object_ref*
\param[out] error   int*, is set to 0 for ok, 1 on error
*/
BITCODE_BL
dwg_obj_ref_get_abs_ref(const dwg_object_ref *ref, int *error)
{
  if (ref)
    {
      *error = 0;
      return ref->absolute_ref;
    }
  else
    {
      LOG_ERROR("%s: empty ref", __FUNCTION__)
      *error = 1;
      return (BITCODE_BL)-1;
    }
}

/** Returns Dwg object type
\code Usage: int type = dwg_get_type(obj);
\endcode
\param[in]  obj   dwg_object*
*/
int
dwg_get_type(const dwg_object *obj)
{
  if (obj)
    {
      return obj->type;
    }
  else
    {
      LOG_ERROR("%s: empty ref", __FUNCTION__)
      return -1;
    }
}

/** Returns the object dxfname as ASCII string
\code Usage : int type = dwg_get_dxfname(obj);
\endcode
\param obj dwg_object*
*/
char*
dwg_get_dxfname(const dwg_object *obj)
{
  if (obj)
    {
      if (dwg_version == R_INVALID)
        dwg_version = (Dwg_Version_Type)obj->parent->header.version;
      return obj->dxfname;
    }
  else
    {
      LOG_ERROR("%s: empty ref", __FUNCTION__)
      return NULL;
    }
}

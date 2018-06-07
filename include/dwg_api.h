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
 * dwg_api.h: external C API
 * This is the only file an user of the API needs to include.
 *
 * written by Gaganjyot Singh
 * modified by Reini Urban
 */

#ifndef _DWG_API_H_
#define _DWG_API_H_

#include <stdio.h>
#include "dwg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_ERROR
# define LOG_ERROR(msg,name,type) \
   fprintf(stderr, msg, name, (type))
#endif

typedef struct dwg_point_3d
{
  double x;
  double y;
  double z;
} dwg_point_3d;

typedef struct dwg_point_2d
{
  double x;
  double y;
} dwg_point_2d;

typedef struct _dwg_LWPLINE_widths
{
  double start;
  double end;
} dwg_lwpline_widths;

// Extract All Entities of a specific type from a BLOCK
#define GET_DWG_ENTITY_DECL(token) \
EXPORT \
Dwg_Entity_##token **dwg_get_##token (Dwg_Object_Ref * ref);

#define GET_DWG_ENTITY(token) \
EXPORT \
Dwg_Entity_##token **dwg_get_##token (Dwg_Object_Ref * ref) \
{ \
  int x=0, counts=0; \
  Dwg_Entity_##token ** ret_##token; \
  Dwg_Object_BLOCK_HEADER *hdr = ref->obj->tio.object->tio.BLOCK_HEADER; \
  Dwg_Object * obj = get_first_owned_object(ref->obj, hdr); \
  while (obj) \
    { \
      if (obj->type == DWG_TYPE_##token) \
        counts++; \
      obj = get_next_owned_object(ref->obj, obj, hdr); \
    } \
  if (!counts) \
    return NULL; \
  ret_##token = (Dwg_Entity_##token **)malloc (counts * sizeof(Dwg_Entity_##token *));\
  obj = get_first_owned_object(ref->obj, hdr); \
  while (obj) \
    { \
      if(obj->type == DWG_TYPE_##token) \
        { \
          ret_##token[x] = obj->tio.entity->tio.token; \
          x++; \
        } \
        obj = get_next_owned_object(ref->obj, obj, hdr); \
    } \
  return ret_##token; \
}

// Cast a Dwg_Object to Entity
#define CAST_DWG_OBJECT_TO_ENTITY_DECL(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token(Dwg_Object *obj);
#define CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token(Dwg_Object *obj);

// Only for fixed typed entities, < 500
#define CAST_DWG_OBJECT_TO_ENTITY(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token(Dwg_Object *obj) \
{ \
    Dwg_Entity_##token *ret_obj = NULL; \
    if (obj != 0 && obj->type == DWG_TYPE_##token) \
      { \
        ret_obj = obj->tio.entity->tio.token; \
      } \
    else \
      { \
        LOG_ERROR("invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
      } \
  return ret_obj; \
}

/// for all classes, types > 500. There's only IMAGE.
#define CAST_DWG_OBJECT_TO_ENTITY_BYNAME(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token(Dwg_Object *obj) \
{ \
    Dwg_Entity_##token *ret_obj = NULL; \
    if (obj && (obj->type == DWG_TYPE_##token || \
               (obj->dxfname && !strcmp(obj->dxfname, #token)))) \
      { \
        ret_obj = obj->tio.entity->tio.token; \
      } \
    else \
      { \
        LOG_ERROR("invalid %s type: got %s, 0x%x", #token, \
                  obj ? obj->dxfname : "<no obj>", \
                  obj ? obj->type : 0); \
      } \
  return ret_obj; \
}

#define CAST_DWG_OBJECT_TO_OBJECT_DECL(token) \
EXPORT \
Dwg_Object_##token *dwg_object_to_##token(Dwg_Object *obj);

#define CAST_DWG_OBJECT_TO_OBJECT(token) \
EXPORT \
Dwg_Object_##token *dwg_object_to_##token(Dwg_Object *obj) \
{ \
    Dwg_Object_##token *ret_obj = NULL; \
    if(obj != 0 && obj->type == DWG_TYPE_##token) \
      { \
        ret_obj = obj->tio.object->tio.token; \
      } \
    else \
      { \
        LOG_ERROR("invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
      } \
  return ret_obj; \
}

#define CAST_DWG_OBJECT_TO_OBJECT_BYNAME(token, dxfname) \
EXPORT \
Dwg_Object_##token *dwg_object_to_##token(Dwg_Object *obj) \
{ \
    Dwg_Object_##token *ret_obj = NULL; \
    if (obj && obj->dxfname && !strcmp(obj->dxfname, #dxfname)) \
      { \
        ret_obj = obj->tio.object->tio.token; \
      } \
    else \
      { \
        LOG_ERROR("invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
      } \
  return ret_obj; \
}

///////////////////////////////////////////////////////////////////////////

typedef struct _dwg_entity_CIRCLE                 dwg_ent_circle;
typedef struct _dwg_entity_LINE                   dwg_ent_line;
typedef struct _dwg_entity_POLYLINE_3D            dwg_ent_polyline_3d;
typedef struct _dwg_entity_POLYLINE_2D            dwg_ent_polyline_2d;
typedef struct _dwg_entity_POLYLINE_MESH          dwg_ent_polyline_mesh;
typedef struct _dwg_entity_POLYLINE_PFACE         dwg_ent_polyline_pface;
typedef struct _dwg_entity_LWPOLYLINE             dwg_ent_lwpline;
typedef struct _dwg_entity_ARC                    dwg_ent_arc;
typedef struct _dwg_entity_ELLIPSE                dwg_ent_ellipse;
typedef struct _dwg_entity_TEXT                   dwg_ent_text;
typedef struct _dwg_entity_POINT                  dwg_ent_point;
typedef struct _dwg_entity_ATTRIB                 dwg_ent_attrib;
typedef struct _dwg_entity_ATTDEF                 dwg_ent_attdef;
typedef struct _dwg_entity_SOLID                  dwg_ent_solid;
typedef struct _dwg_entity_TRACE                  dwg_ent_trace;
typedef struct _dwg_entity_3DFACE                 dwg_ent_3dface;
typedef struct _dwg_entity_INSERT                 dwg_ent_insert;
typedef struct _dwg_entity_MINSERT                dwg_ent_minsert;
typedef struct _dwg_entity_BLOCK                  dwg_ent_block;
typedef struct _dwg_entity_IMAGE                  dwg_ent_image;
typedef struct _dwg_MLINE_line                    dwg_mline_line;
typedef struct _dwg_MLINE_vertex                  dwg_mline_vertex;
typedef struct _dwg_entity_MLINE                  dwg_ent_mline;
typedef struct _dwg_entity_RAY                    dwg_ent_ray, 
                                                  dwg_ent_xline;
typedef struct _dwg_entity_VERTEX_3D              dwg_ent_vertex_3d, 
                                                  dwg_ent_vertex_mesh,
                                                  dwg_ent_vertex_pface;
typedef struct _dwg_entity_VERTEX_2D              dwg_ent_vertex_2d;
typedef struct _dwg_entity_DIMENSION_common       dwg_ent_dim;
typedef struct _dwg_entity_DIMENSION_ORDINATE     dwg_ent_dim_ordinate;
typedef struct _dwg_entity_DIMENSION_LINEAR       dwg_ent_dim_linear;
typedef struct _dwg_entity_DIMENSION_ALIGNED      dwg_ent_dim_aligned;
typedef struct _dwg_entity_DIMENSION_ANG3PT       dwg_ent_dim_ang3pt;
typedef struct _dwg_entity_DIMENSION_ANG2LN       dwg_ent_dim_ang2ln;
typedef struct _dwg_entity_DIMENSION_RADIUS       dwg_ent_dim_radius;
typedef struct _dwg_entity_DIMENSION_DIAMETER     dwg_ent_dim_diameter;
typedef struct _dwg_entity_LEADER                 dwg_ent_leader;
typedef struct _dwg_entity_SHAPE                  dwg_ent_shape;
typedef struct _dwg_entity_MTEXT                  dwg_ent_mtext;
typedef struct _dwg_entity_TOLERANCE              dwg_ent_tolerance;
typedef struct _dwg_entity_ENDBLK                 dwg_ent_endblk;
typedef struct _dwg_entity_SEQEND                 dwg_ent_seqend;
typedef struct _dwg_entity_SPLINE                 dwg_ent_spline;
typedef struct _dwg_SPLINE_point                  dwg_spline_point;
typedef struct _dwg_SPLINE_control_point          dwg_spline_control_point;
typedef struct _dwg_entity_OLEFRAME               dwg_ent_oleframe;
typedef struct _dwg_entity_OLE2FRAME              dwg_ent_ole2frame;
typedef struct _dwg_entity_VIEWPORT               dwg_ent_viewport;
typedef struct _dwg_entity_3DSOLID                dwg_ent_3dsolid,
                                                  dwg_ent_region,
                                                  dwg_ent_body;
typedef struct _dwg_3DSOLID_wire                  dwg_3dsolid_wire;
typedef struct _dwg_3DSOLID_silhouette            dwg_3dsolid_silhouette;
typedef struct _dwg_entity_TABLE                  dwg_ent_table;
typedef struct _dwg_entity_HATCH                  dwg_ent_hatch;
typedef struct _dwg_entity_VERTEX_PFACE_FACE      dwg_ent_vert_pface_face;
typedef struct _dwg_entity_DUMMY                  dwg_ent_generic;

///////////////////////////////////////////////////////////////////////////

typedef struct _dwg_object_LAYER                  dwg_obj_layer;
typedef struct _dwg_object_BLOCK_HEADER           dwg_obj_block_header;
typedef struct _dwg_object_BLOCK_CONTROL          dwg_obj_block_control;
typedef struct _dwg_object_MLINESTYLE             dwg_obj_mlinestyle;
typedef struct _dwg_object_APPID                  dwg_obj_appid;
typedef struct _dwg_object_APPID_CONTROL          dwg_obj_appid_control;
typedef struct _dwg_object_PROXY_OBJECT           dwg_obj_proxy;
typedef struct _dwg_object_XRECORD                dwg_obj_xrecord;
typedef struct _dwg_object_UNKNOWN_OBJ            dwg_obj_generic;
typedef struct _dwg_object_STYLE_CONTROL          dwg_tbl_generic;

typedef struct _dwg_object                        dwg_object;
typedef struct _dwg_object_ref                    dwg_object_ref;
typedef struct _dwg_handle                        dwg_handle;
typedef struct _dwg_object_entity                 dwg_obj_ent;
typedef struct _dwg_object_object                 dwg_obj_obj;
typedef struct _dwg_class                         dwg_class;
typedef struct _dwg_struct                        dwg_data;
typedef struct _dwg_entity_eed_data		  dwg_entity_eed_data;
typedef struct _dwg_entity_eed			  dwg_entity_eed;

///////////////////////////////////////////////////////////////////////////

/*******************************************************************
*        Functions created from macro to extract entities           *
********************************************************************/

/// extract all text entities from a block
GET_DWG_ENTITY_DECL(TEXT)
/// extract all attrib entities from a block
GET_DWG_ENTITY_DECL(ATTRIB)
/// extract all attdef entities from a block
GET_DWG_ENTITY_DECL(ATTDEF)
/// extract all block entities from a block
GET_DWG_ENTITY_DECL(BLOCK)
/// extract endblk entity from a block
GET_DWG_ENTITY_DECL(ENDBLK)
/// extract all seqend entities from a block
GET_DWG_ENTITY_DECL(SEQEND)
/// extract all insert entities from a block
GET_DWG_ENTITY_DECL(INSERT)
/// extract all minsert entities from a block
GET_DWG_ENTITY_DECL(MINSERT)
/// extract all vertex_2d entities from a block
GET_DWG_ENTITY_DECL(VERTEX_2D)
/// extract all vertex_3d entities from a block
GET_DWG_ENTITY_DECL(VERTEX_3D)
/// extract all vertex_mesh entities from a block
GET_DWG_ENTITY_DECL(VERTEX_MESH)
/// extract all vertex_pface entities from a block
GET_DWG_ENTITY_DECL(VERTEX_PFACE)
/// extract all vertex_pface_face entities from a block
GET_DWG_ENTITY_DECL(VERTEX_PFACE_FACE)
/// extract all polyline_2d entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_2D)
/// extract all polyline_3d entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_3D)
/// extract all arc entities from a block
GET_DWG_ENTITY_DECL(ARC)
/// extract all circle entities from a block
GET_DWG_ENTITY_DECL(CIRCLE)
/// extract all line entities from a block
GET_DWG_ENTITY_DECL(LINE)
/// extract all dimension ordinate entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ORDINATE)
/// extract all dimension linear entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_LINEAR)
/// extract all dimension aligned entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ALIGNED)
/// extract all dimension ang3pt entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ANG3PT)
/// extract all dimension ang2ln entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ANG2LN)
/// extract all dimension radius entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_RADIUS)
/// extract all dimension diameter entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_DIAMETER)
/// extract all points entities from a block
GET_DWG_ENTITY_DECL(POINT)
/// extract all polyline_pface entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_PFACE)
/// extract all polyline_mesh entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_MESH)
/// extract all solid entities from a block
GET_DWG_ENTITY_DECL(SOLID)
/// extract all trace entities from a block
GET_DWG_ENTITY_DECL(TRACE)
/// extract all shape entities from a block
GET_DWG_ENTITY_DECL(SHAPE)
/// extract all viewport entities from a block
GET_DWG_ENTITY_DECL(VIEWPORT)
/// extract all ellipse entities from a block
GET_DWG_ENTITY_DECL(ELLIPSE)
/// extract all spline entities from a block
GET_DWG_ENTITY_DECL(SPLINE)
/// extract all region entities from a block
GET_DWG_ENTITY_DECL(REGION)
/// extract all body entities from a block
GET_DWG_ENTITY_DECL(BODY)
/// extract all ray entities from a block
GET_DWG_ENTITY_DECL(RAY)
/// extract all xline entities from a block
GET_DWG_ENTITY_DECL(XLINE)
/// extract all OLEFRAME entities from a block
GET_DWG_ENTITY_DECL(OLEFRAME)
/// extract all mtext entities from a block
GET_DWG_ENTITY_DECL(MTEXT)
/// extract all leader entities from a block
GET_DWG_ENTITY_DECL(LEADER)
/// extract all tolerance entities from a block
GET_DWG_ENTITY_DECL(TOLERANCE)
/// extract all mline entities from a block
GET_DWG_ENTITY_DECL(MLINE)
/// extract all OLE2FRAME entities from a block
GET_DWG_ENTITY_DECL(OLE2FRAME)
/// extract all DUMMY entities from a block
GET_DWG_ENTITY_DECL(DUMMY)
/// extract all LONG_TRANSACTION entities from a block
GET_DWG_ENTITY_DECL(LONG_TRANSACTION)
/// extract all lwpline entities from a block
GET_DWG_ENTITY_DECL(LWPOLYLINE)
/// extract all PROXY_ENTITY entities from a block
GET_DWG_ENTITY_DECL(PROXY_ENTITY)
/// extract all hatch entities from a block
GET_DWG_ENTITY_DECL(HATCH)
/// extract all image entities from a block
GET_DWG_ENTITY_DECL(IMAGE)

/*******************************************************************
*     Functions created from macro to cast dwg_object to entity     *
*                 Usage :- dwg_object_to_ENTITY(),                  *
*                where ENTITY can be LINE or CIRCLE                 *
********************************************************************/

/// dwg object to text
CAST_DWG_OBJECT_TO_ENTITY_DECL(TEXT)
/// dwg object to attrib
CAST_DWG_OBJECT_TO_ENTITY_DECL(ATTRIB)
/// dwg object to attdef
CAST_DWG_OBJECT_TO_ENTITY_DECL(ATTDEF)
/// dwg object to block
CAST_DWG_OBJECT_TO_ENTITY_DECL(BLOCK)
/// extract endblk entity from a block
CAST_DWG_OBJECT_TO_ENTITY_DECL(ENDBLK)
/// dwg object to seqend
CAST_DWG_OBJECT_TO_ENTITY_DECL(SEQEND)
/// dwg object to insert
CAST_DWG_OBJECT_TO_ENTITY_DECL(INSERT)
/// dwg object to minsert
CAST_DWG_OBJECT_TO_ENTITY_DECL(MINSERT)
/// dwg object to vertex_2d
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_2D)
/// dwg object to vertex_3d
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_3D)
/// dwg object to vertex_mesh
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_MESH)
/// dwg object to vertex_pface
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_PFACE)
/// dwg object to vertex_pface_face
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_PFACE_FACE)
/// dwg object to polyline_2d
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_2D)
/// dwg object to polyline_3d
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_3D)
/// dwg object to arc
CAST_DWG_OBJECT_TO_ENTITY_DECL(ARC)
/// dwg object to circle
CAST_DWG_OBJECT_TO_ENTITY_DECL(CIRCLE)
/// dwg object to line
CAST_DWG_OBJECT_TO_ENTITY_DECL(LINE)

/// dwg object to common dimension
EXPORT dwg_ent_dim * dwg_object_to_DIMENSION(dwg_object *obj);
/// dwg object to dimension ordinate
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ORDINATE)
/// dwg object to dimension linear
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_LINEAR)
/// dwg object to dimension aligned
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ALIGNED)
/// dwg object to dimension ang3pt
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ANG3PT)
/// dwg object to dimension ang2ln
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ANG2LN)
/// dwg object to dimension radius
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_RADIUS)
/// dwg object to dimension diameter
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_DIAMETER)
/// dwg object to points
CAST_DWG_OBJECT_TO_ENTITY_DECL(POINT)
/// dwg object to polyline_pface
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_PFACE)
/// dwg object to polyline_mesh
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_MESH)
/// dwg object to solid
CAST_DWG_OBJECT_TO_ENTITY_DECL(SOLID)
/// dwg object to trace
CAST_DWG_OBJECT_TO_ENTITY_DECL(TRACE)
/// dwg object to shape
CAST_DWG_OBJECT_TO_ENTITY_DECL(SHAPE)
/// dwg object to viewport
CAST_DWG_OBJECT_TO_ENTITY_DECL(VIEWPORT)
/// dwg object to ellipse
CAST_DWG_OBJECT_TO_ENTITY_DECL(ELLIPSE)
/// dwg object to spline
CAST_DWG_OBJECT_TO_ENTITY_DECL(SPLINE)
/// dwg object to region
CAST_DWG_OBJECT_TO_ENTITY_DECL(REGION)
/// dwg object to body
CAST_DWG_OBJECT_TO_ENTITY_DECL(BODY)
/// dwg object to ray
CAST_DWG_OBJECT_TO_ENTITY_DECL(RAY)
/// dwg object to xline
CAST_DWG_OBJECT_TO_ENTITY_DECL(XLINE)
/// dwg object to oleframe
CAST_DWG_OBJECT_TO_ENTITY_DECL(OLEFRAME)
/// dwg object to mtext
CAST_DWG_OBJECT_TO_ENTITY_DECL(MTEXT)
/// dwg object to leader
CAST_DWG_OBJECT_TO_ENTITY_DECL(LEADER)
/// dwg object to tolerance
CAST_DWG_OBJECT_TO_ENTITY_DECL(TOLERANCE)
/// dwg object to mline
CAST_DWG_OBJECT_TO_ENTITY_DECL(MLINE)
/// dwg object to ole2frame
CAST_DWG_OBJECT_TO_ENTITY_DECL(OLE2FRAME)
/// dwg object to dummy
CAST_DWG_OBJECT_TO_ENTITY_DECL(DUMMY)
/// dwg object to long_transaction
CAST_DWG_OBJECT_TO_ENTITY_DECL(LONG_TRANSACTION)
/// dwg object to lwpline
CAST_DWG_OBJECT_TO_ENTITY_DECL(LWPOLYLINE)
/// dwg object to proxy_entity
CAST_DWG_OBJECT_TO_ENTITY_DECL(PROXY_ENTITY)
/// dwg object to hatch
CAST_DWG_OBJECT_TO_ENTITY_DECL(HATCH)
/// dwg object to image
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL(IMAGE)

/*******************************************************************
*     Functions created from macro to dwg object to object     *
*                 Usage :- dwg_object_to_OBJECT(),                  *
*            where OBJECT can be LAYER or BLOCK_HEADER              *
********************************************************************/
/// dwg object to block header
CAST_DWG_OBJECT_TO_OBJECT_DECL(BLOCK_HEADER)
/// dwg object to block control
CAST_DWG_OBJECT_TO_OBJECT_DECL(BLOCK_CONTROL)
/// dwg object to layer
CAST_DWG_OBJECT_TO_OBJECT_DECL(LAYER)
/// dwg object to xrecord
CAST_DWG_OBJECT_TO_OBJECT_DECL(XRECORD)

/// initialize the DWG version, needed for r2007+ unicode strings
/// and pre-R13 field variants. unneeded for r13-r2004
EXPORT void dwg_api_init_version(Dwg_Data *dwg);

/********************************************************************
*                FUNCTIONS START HERE ENTITY SPECIFIC               *
********************************************************************/


/********************************************************************
*                    FUNCTIONS FOR CIRCLE ENTITY                    *
********************************************************************/

// Get/Set the center point of a _dwg_entity_CIRCLE::
EXPORT void
dwg_ent_circle_get_center(const dwg_ent_circle *restrict circle,
                          dwg_point_3d *restrict point,
                          int *restrict error);

void
dwg_ent_circle_set_center(dwg_ent_circle *restrict circle,
                          const dwg_point_3d *restrict point,
                          int *restrict error);

// Get/Set the radius of a circle
EXPORT double
dwg_ent_circle_get_radius(const dwg_ent_circle *restrict circle,
                          int *restrict error);

EXPORT void
dwg_ent_circle_set_radius(dwg_ent_circle *restrict circle,
                          const BITCODE_BD radius,
                          int *restrict error);

// Get/Set the thickness of a circle
EXPORT double
dwg_ent_circle_get_thickness(const dwg_ent_circle *restrict circle,
                          int *restrict error);

EXPORT void
dwg_ent_circle_set_thickness(dwg_ent_circle *restrict circle,
                             const double  thickness,
                             int *restrict error);

// Get/Set the extrusion of a circle
EXPORT void
dwg_ent_circle_get_extrusion(const dwg_ent_circle *restrict circle,
                             dwg_point_3d *restrict vector,
                          int *restrict error);

EXPORT void
dwg_ent_circle_set_extrusion(dwg_ent_circle *restrict circle,
                             const dwg_point_3d *restrict vector,
                          int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR LINE ENTITY                      *
********************************************************************/

// Get/Set the start point of a line
EXPORT void
dwg_ent_line_get_start_point(const dwg_ent_line *restrict line,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_line_set_start_point(dwg_ent_line *restrict line,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

// Get/Set the end point of a line
EXPORT void
dwg_ent_line_get_end_point(const dwg_ent_line *restrict line,
                           dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_line_set_end_point(dwg_ent_line *restrict line,
                           const dwg_point_3d *restrict point,
                           int *restrict error);

// Get/Set the thickness of a line
EXPORT double
dwg_ent_line_get_thickness(const dwg_ent_line *restrict line,
                          int *restrict error);

EXPORT void
dwg_ent_line_set_thickness(dwg_ent_line *restrict line,
                           const BITCODE_BD thickness,
                          int *restrict error);

// Get/Set the extrusion of a line
EXPORT void
dwg_ent_line_set_extrusion(dwg_ent_line *restrict line,
                           const dwg_point_3d *restrict vector,
                           int *restrict error);

EXPORT void
dwg_ent_line_get_extrusion(const dwg_ent_line *restrict line,
                           dwg_point_3d *restrict vector,
                           int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR ARC ENTITY                       *
********************************************************************/

// Get/Set the center point of a arc
EXPORT void
dwg_ent_arc_get_center(const dwg_ent_arc *restrict arc,
                       dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_arc_set_center(dwg_ent_arc *restrict arc,
                       const dwg_point_3d *restrict point,
                          int *restrict error);

// Get/Set the radius of a arc
EXPORT double
dwg_ent_arc_get_radius(const dwg_ent_arc *restrict arc,
                          int *restrict error);

EXPORT void
dwg_ent_arc_set_radius(dwg_ent_arc *restrict arc,
                       const BITCODE_BD radius,
                          int *restrict error);

// Get/Set the thickness of arc
EXPORT double
dwg_ent_arc_get_thickness(const dwg_ent_arc *restrict arc,
                          int *restrict error);

EXPORT void
dwg_ent_arc_set_thickness(dwg_ent_arc *restrict arc,
                          const BITCODE_BD thickness,
                          int *restrict error);

//Get/Set the extrusion of arc
EXPORT void
dwg_ent_arc_get_extrusion(const dwg_ent_arc *restrict arc,
                          dwg_point_3d *restrict vector,
                          int *restrict error);

EXPORT void
dwg_ent_arc_set_extrusion(dwg_ent_arc *restrict arc,
                          const dwg_point_3d *restrict vector,
                          int *restrict error);

// Get/set start angle arc
EXPORT double
dwg_ent_arc_get_start_angle(const dwg_ent_arc *restrict arc,
                          int *restrict error);

EXPORT void
dwg_ent_arc_set_start_angle(dwg_ent_arc *restrict arc,
                            const BITCODE_BD start_angle,
                          int *restrict error);

// Get/set end angle of arc
EXPORT double
dwg_ent_arc_get_end_angle(const dwg_ent_arc *restrict arc,
                          int *restrict error);

EXPORT void
dwg_ent_arc_set_end_angle(dwg_ent_arc *restrict arc,
                          const BITCODE_BD end_angle,
                          int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR ELLIPSE ENTITY                    *
********************************************************************/

// Get/Set center of ellipse
EXPORT void
dwg_ent_ellipse_get_center(const dwg_ent_ellipse *restrict ellipse,
                           dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_ellipse_set_center(dwg_ent_ellipse *restrict ellipse,
                           const dwg_point_3d *restrict point,
                           int *restrict error);

// Get/Set sm axis of ellipse
EXPORT void
dwg_ent_ellipse_get_sm_axis(const dwg_ent_ellipse *restrict ellipse,
                            dwg_point_3d *restrict point,
                            int *restrict error);

EXPORT void
dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *restrict ellipse,
                            const dwg_point_3d *restrict point,
                            int *restrict error);

// Get/Set extrusion of ellipse
EXPORT void
dwg_ent_ellipse_get_extrusion(const dwg_ent_ellipse *restrict ellipse,
                              dwg_point_3d *restrict vector,
                              int *restrict error);

EXPORT void
dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *restrict ellipse,
                              const dwg_point_3d *restrict vector,
                              int *restrict error);

// Get/Set axis ratio of ellipse
EXPORT double
dwg_ent_ellipse_get_axis_ratio(const dwg_ent_ellipse *restrict ellipse,
                          int *restrict error);

EXPORT void
dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *restrict ellipse, const double  ratio,
                               int *restrict error);

// Get/Set start angle of ellipse
EXPORT double
dwg_ent_ellipse_get_start_angle(const dwg_ent_ellipse *restrict ellipse,
                          int *restrict error);

EXPORT void
dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *restrict ellipse,
                                const double  start_angle,
                                int *restrict error);

// Get/Set end angle of ellipse
EXPORT double
dwg_ent_ellipse_get_end_angle(const dwg_ent_ellipse *restrict ellipse,
                          int *restrict error);

EXPORT void
dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *restrict ellipse,
                              const double end_angle,
                              int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR TEXT ENTITY                      *
********************************************************************/


// Get/Set text of text (utf-8 encoded)
EXPORT void
dwg_ent_text_set_text(dwg_ent_text *restrict text, const char *restrict text_value,
                      int *restrict error);

EXPORT char *
dwg_ent_text_get_text(const dwg_ent_text *restrict text,
                          int *restrict error);

// Get/Set insertion points of text
EXPORT void
dwg_ent_text_get_insertion_point(const dwg_ent_text *restrict text,
                                 dwg_point_2d *restrict point,
                                 int *restrict error);

EXPORT void
dwg_ent_text_set_insertion_point(dwg_ent_text *restrict text,
                                 const dwg_point_2d *restrict point,
                                 int *restrict error);

//Get/Set height of text
EXPORT double
dwg_ent_text_get_height(const dwg_ent_text *restrict text,
                          int *restrict error);

EXPORT void
dwg_ent_text_set_height(dwg_ent_text *restrict text,
                        const double height,
                          int *restrict error);

//Get/Set extrusion of text
EXPORT void
dwg_ent_text_get_extrusion(const dwg_ent_text *restrict text,
                           dwg_point_3d *restrict vector,
                           int *restrict error);

EXPORT void
dwg_ent_text_set_extrusion(dwg_ent_text *restrict text,
                           const dwg_point_3d *restrict vector,
                           int *restrict error);

//Get/Set thickness of text
EXPORT double
dwg_ent_text_get_thickness(const dwg_ent_text *restrict text,
                          int *restrict error);

EXPORT void
dwg_ent_text_set_thickness(dwg_ent_text *restrict text,
                           const double thickness,
                          int *restrict error);

//Get/Set rotation angle of text
EXPORT double
dwg_ent_text_get_rotation(const dwg_ent_text *restrict text,
                          int *restrict error);

EXPORT void
dwg_ent_text_set_rotation(dwg_ent_text *restrict text,
                          const double angle,
                          int *restrict error);

//Get/Set horizontal alignment angle of text
EXPORT BITCODE_BS
dwg_ent_text_get_vert_align(const dwg_ent_text *restrict text,
                          int *restrict error);

EXPORT void
dwg_ent_text_set_vert_align(dwg_ent_text *restrict text,
                            const BITCODE_BS alignment,
                          int *restrict error);

//Get/Set vertical alignment of text
EXPORT BITCODE_BS
dwg_ent_text_get_horiz_align(const dwg_ent_text *restrict text,
                          int *restrict error);

EXPORT void
dwg_ent_text_set_horiz_align(dwg_ent_text *restrict text,
                             const BITCODE_BS alignment,
                          int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR ATTRIB ENTITY                     *
********************************************************************/


// Get/Set text of attrib (utf-8 encoded)
EXPORT void
dwg_ent_attrib_set_text(dwg_ent_attrib *restrict attrib,
                        const char *restrict text_value,
                          int *restrict error);

EXPORT char *
dwg_ent_attrib_get_text(const dwg_ent_attrib *restrict attrib,
                          int *restrict error);

// Get/Set insertion points of attrib
EXPORT void
dwg_ent_attrib_get_insertion_point(const dwg_ent_attrib *restrict attrib,
                                   dwg_point_2d *restrict point,
                                   int *restrict error);

EXPORT void
dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *restrict attrib,
                                   const dwg_point_2d *restrict point,
                                   int *restrict error);

//Get/Set height of attrib
EXPORT double
dwg_ent_attrib_get_height(const dwg_ent_attrib *restrict attrib,
                          int *restrict error);

EXPORT void
dwg_ent_attrib_set_height(dwg_ent_attrib *restrict attrib,
                          const double  height,
                          int *restrict error);

//Get/Set extrusion of attrib
EXPORT void
dwg_ent_attrib_get_extrusion(const dwg_ent_attrib *restrict attrib,
                             dwg_point_3d *restrict vector,
                             int *restrict error);

EXPORT void
dwg_ent_attrib_set_extrusion(dwg_ent_attrib *restrict attrib,
                             const dwg_point_3d *restrict vector,
                             int *restrict error);

//Get/Set thickness of attrib
EXPORT double
dwg_ent_attrib_get_thickness(const dwg_ent_attrib *restrict attrib,
                          int *restrict error);

EXPORT void
dwg_ent_attrib_set_thickness(dwg_ent_attrib *restrict attrib,
                             const double  thickness,
                             int *restrict error);

//Get/Set rotation angle of attrib
EXPORT double
dwg_ent_attrib_get_rot_angle(const dwg_ent_attrib *restrict attrib,
                          int *restrict error);

EXPORT void
dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *restrict attrib,
                             const double  angle,
                             int *restrict error);

//Get/Set horizontal alignment angle of attrib
EXPORT BITCODE_BS
dwg_ent_attrib_get_vert_align(const dwg_ent_attrib *restrict attrib,
                          int *restrict error);

EXPORT void
dwg_ent_attrib_set_vert_align(dwg_ent_attrib *restrict attrib,
                              const BITCODE_BS alignment,
                              int *restrict error);

//Get/Set vertical alignment of attrib
EXPORT BITCODE_BS
dwg_ent_attrib_get_horiz_align(const dwg_ent_attrib *restrict attrib,
                          int *restrict error);

EXPORT void
dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *restrict attrib,
                               const BITCODE_BS alignment,
                               int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR ATTDEF ENTITY                     *
********************************************************************/


// Get/Set default text of attdef (utf-8 encoded)
EXPORT void
dwg_ent_attdef_set_text(dwg_ent_attdef *restrict attdef,
                        const char *restrict default_value,
                        int *restrict error);

EXPORT char *
dwg_ent_attdef_get_text(const dwg_ent_attdef *restrict attdef,
                          int *restrict error);

// Get/Set insertion points of attdef
EXPORT void
dwg_ent_attdef_get_insertion_point(const dwg_ent_attdef *restrict attdef,
                                   dwg_point_2d *restrict point,
                                   int *restrict error);

EXPORT void
dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *restrict attdef,
                                   const dwg_point_2d *restrict point,
                                   int *restrict error);

//Get/Set height of attdef
EXPORT double
dwg_ent_attdef_get_height(const dwg_ent_attdef *restrict attdef,
                          int *restrict error);

EXPORT void
dwg_ent_attdef_set_height(dwg_ent_attdef *restrict attdef,
                          const double  height,
                          int *restrict error);

//Get/Set extrusion of attdef
EXPORT void
dwg_ent_attdef_get_extrusion(const dwg_ent_attdef *restrict attdef,
                             dwg_point_3d *restrict vector,
                             int *restrict error);

EXPORT void
dwg_ent_attdef_set_extrusion(dwg_ent_attdef *restrict attdef,
                             const dwg_point_3d *restrict vector,
                             int *restrict error);

//Get/Set thickness of attdef
EXPORT double
dwg_ent_attdef_get_thickness(const dwg_ent_attdef *restrict attdef,
                          int *restrict error);

EXPORT void
dwg_ent_attdef_set_thickness(dwg_ent_attdef *restrict attdef, const double  thickness,
                             int *restrict error);

//Get/Set rotation angle of attdef
EXPORT double
dwg_ent_attdef_get_rot_angle(const dwg_ent_attdef *restrict attdef,
                          int *restrict error);

EXPORT void
dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *restrict attdef, const double  angle,
                             int *restrict error);

//Get/Set horizontal alignment angle of attdef
EXPORT BITCODE_BS
dwg_ent_attdef_get_vert_align(const dwg_ent_attdef *restrict attdef,
                          int *restrict error);

EXPORT void
dwg_ent_attdef_set_vert_align(dwg_ent_attdef *restrict attdef, const BITCODE_BS alignment,
                              int *restrict error);

//Get/Set vertical alignment of attdef
EXPORT BITCODE_BS
dwg_ent_attdef_get_horiz_align(const dwg_ent_attdef *restrict attdef,
                          int *restrict error);

EXPORT void
dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *restrict attdef, const BITCODE_BS alignment,
                               int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR POINT ENTITY                      *
********************************************************************/

// Get/Set point of point entity
EXPORT void
dwg_ent_point_set_point(dwg_ent_point *restrict point,
                        const dwg_point_3d *restrict retpoint,
                        int *restrict error);

EXPORT void
dwg_ent_point_get_point(const dwg_ent_point *restrict point,
                        dwg_point_3d *restrict retpoint,
                        int *restrict error);

// Get/Set thickness of point entity
EXPORT double
dwg_ent_point_get_thickness(const dwg_ent_point *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_point_set_thickness(dwg_ent_point *restrict point, const double thickness,
                            int *restrict error);

// Get/Set extrusion of point entity
EXPORT void
dwg_ent_point_set_extrusion(dwg_ent_point *restrict point,
                            const dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_point_get_extrusion(const dwg_ent_point *restrict point,
                            dwg_point_3d *restrict vector,
                            int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR SOLID ENTITY                      *
********************************************************************/


// Get/Set thickness of solid entity
EXPORT double
dwg_ent_solid_get_thickness(const dwg_ent_solid *restrict solid,
                          int *restrict error);

EXPORT void
dwg_ent_solid_set_thickness(dwg_ent_solid *restrict solid, const double  thickness,
                            int *restrict error);

// Get/Set elevation of solid entity
EXPORT double
dwg_ent_solid_get_elevation(const dwg_ent_solid *restrict solid,
                          int *restrict error);

EXPORT void
dwg_ent_solid_set_elevation(dwg_ent_solid *restrict solid, const double  elevation,
                            int *restrict error);

// Get/Set corner1 of solid entity
EXPORT void
dwg_ent_solid_get_corner1(const dwg_ent_solid *restrict solid, dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_solid_set_corner1(dwg_ent_solid *restrict solid,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set corner2 of solid entity
EXPORT void
dwg_ent_solid_get_corner2(const dwg_ent_solid *restrict solid,
                          dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_solid_set_corner2(dwg_ent_solid *restrict solid,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set corner3 of solid entity
EXPORT void
dwg_ent_solid_get_corner3(const dwg_ent_solid *restrict solid,
                          dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_solid_set_corner3(dwg_ent_solid *restrict solid,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set corner4 of solid entity
EXPORT void
dwg_ent_solid_get_corner4(const dwg_ent_solid *restrict solid,
                          dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_solid_set_corner4(dwg_ent_solid *restrict solid,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set extrusion of solid entity
EXPORT void
dwg_ent_solid_get_extrusion(const dwg_ent_solid *restrict solid,
                            dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_solid_set_extrusion(dwg_ent_solid *restrict solid,
                            const dwg_point_3d *restrict vector,
                            int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR BLOCk ENTITY                      *
********************************************************************/


// Get/Set text of block entity
EXPORT void
dwg_ent_block_set_name(dwg_ent_block *restrict block,
                       const char *restrict name,
                          int *restrict error);

EXPORT char *
dwg_ent_block_get_name(const dwg_ent_block *restrict block,
                          int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR RAY ENTITY                       *
********************************************************************/


// Get/Set point of ray entity
EXPORT void
dwg_ent_ray_get_point(const dwg_ent_ray *restrict ray,
                      dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_ray_set_point(dwg_ent_ray *restrict ray,
                      const dwg_point_3d *restrict point,
                          int *restrict error);

// Get/Set vector of ray entity
EXPORT void
dwg_ent_ray_get_vector(const dwg_ent_ray *restrict ray,
                       dwg_point_3d *restrict vector,
                          int *restrict error);

EXPORT void
dwg_ent_ray_set_vector(dwg_ent_ray *restrict ray,
                       const dwg_point_3d *restrict vector,
                          int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR XLINE ENTITY                      *
********************************************************************/


// Get/Set point of xline entity
EXPORT void
dwg_ent_xline_get_point(const dwg_ent_xline *restrict xline,
                        dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_xline_set_point(dwg_ent_xline *restrict xline,
                        const dwg_point_3d *restrict point,
                          int *restrict error);

// Get/Set vector of xline entity
EXPORT void
dwg_ent_xline_get_vector(const dwg_ent_xline *restrict xline,
                         dwg_point_3d *restrict vector,
                         int *restrict error);

EXPORT void
dwg_ent_xline_set_vector(dwg_ent_xline *restrict xline,
                         const dwg_point_3d *restrict vector,
                         int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR TRACE ENTITY                      *
********************************************************************/


// Get/Set thickness of trace entity
EXPORT double
dwg_ent_trace_get_thickness(const dwg_ent_trace *restrict trace,
                          int *restrict error);

EXPORT void
dwg_ent_trace_set_thickness(dwg_ent_trace *restrict trace,
                            const double  thickness,
                            int *restrict error);

// Get/Set elevation of trace entity
EXPORT double
dwg_ent_trace_get_elevation(const dwg_ent_trace *restrict trace,
                          int *restrict error);

EXPORT void
dwg_ent_trace_set_elevation(dwg_ent_trace *restrict trace,
                            const double  elevation,
                            int *restrict error);

// Get/Set corner1 of trace entity
EXPORT void
dwg_ent_trace_get_corner1(const dwg_ent_trace *restrict trace,
                          dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_trace_set_corner1(dwg_ent_trace *restrict trace,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set corner2 of trace entity
EXPORT void
dwg_ent_trace_get_corner2(const dwg_ent_trace *restrict trace,
                          dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_trace_set_corner2(dwg_ent_trace *restrict trace,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set corner3 of trace entity
EXPORT void
dwg_ent_trace_get_corner3(const dwg_ent_trace *restrict trace,
                          dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_trace_set_corner3(dwg_ent_trace *restrict trace,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set corner4 of trace entity
EXPORT void
dwg_ent_trace_get_corner4(const dwg_ent_trace *restrict trace,
                          dwg_point_2d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_trace_set_corner4(dwg_ent_trace *restrict trace,
                          const dwg_point_2d *restrict point,
                          int *restrict error);

// Get/Set extrusion of trace entity
EXPORT void
dwg_ent_trace_get_extrusion(const dwg_ent_trace *restrict trace,
                            dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_trace_set_extrusion(dwg_ent_trace *restrict trace,
                            const dwg_point_3d *restrict vector,
                            int *restrict error);


/********************************************************************
*                 FUNCTIONS FOR VERTEX_3D ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_3d entity
EXPORT char
dwg_ent_vertex_3d_get_flag(const dwg_ent_vertex_3d *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_3d_set_flag(dwg_ent_vertex_3d *restrict vert,
                           const char flags,
                          int *restrict error);

// Get/Set point of vertex_3d entity
EXPORT void
dwg_ent_vertex_3d_get_point(const dwg_ent_vertex_3d *restrict vert,
                            dwg_point_3d *restrict point,
                            int *restrict error);

EXPORT void
dwg_ent_vertex_3d_set_point(dwg_ent_vertex_3d *restrict vert,
                            const dwg_point_3d *restrict point,
                            int *restrict error);


/********************************************************************
*               FUNCTIONS FOR VERTEX_MESH ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_mesh entity
EXPORT char
dwg_ent_vertex_mesh_get_flag(const dwg_ent_vertex_mesh *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_mesh_set_flag(dwg_ent_vertex_mesh *restrict vert,
                             const char flags,
                             int *restrict error);

// Get/Set point of vertex_mesh entity
EXPORT void
dwg_ent_vertex_mesh_get_point(const dwg_ent_vertex_mesh *restrict vert,
                              dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_vertex_mesh_set_point(dwg_ent_vertex_mesh *restrict vert,
                              const dwg_point_3d *restrict point,
                              int *restrict error);


/********************************************************************
*               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
********************************************************************/


// Get/Set flags of vertex_pface entity
EXPORT char
dwg_ent_vertex_pface_get_flag(const dwg_ent_vertex_pface *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_pface_set_flag(dwg_ent_vertex_pface *restrict vert,
                              const char flags,
                              int *restrict error);

// Get/Set point of vertex_pface entity
EXPORT void
dwg_ent_vertex_pface_get_point(const dwg_ent_vertex_pface *restrict vert,
                               dwg_point_3d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_vertex_pface_set_point(dwg_ent_vertex_pface *restrict vert,
                               const dwg_point_3d *restrict point,
                               int *restrict error);


/********************************************************************
*                 FUNCTIONS FOR VERTEX_2D ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_2d entity
EXPORT char
dwg_ent_vertex_2d_get_flag(const dwg_ent_vertex_2d *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_2d_set_flag(dwg_ent_vertex_2d *restrict vert,
                           const char flags,
                          int *restrict error);

// Get/Set point of vertex_2d entity
EXPORT void
dwg_ent_vertex_2d_get_point(const dwg_ent_vertex_2d *restrict vert,
                            dwg_point_3d *restrict point,
                            int *restrict error);

EXPORT void
dwg_ent_vertex_2d_set_point(dwg_ent_vertex_2d *restrict vert,
                            const dwg_point_3d *restrict point,
                            int *restrict error);

// Get/Set start_width of vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_start_width(const dwg_ent_vertex_2d *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_2d_set_start_width(dwg_ent_vertex_2d *restrict vert,
                                  const double  start_width,
                                  int *restrict error);

// Get/Set end width of a vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_end_width(const dwg_ent_vertex_2d *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_2d_set_end_width(dwg_ent_vertex_2d *restrict vert,
                                const double  end_width,
                                int *restrict error);

// Get/Set bulge of a vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_bulge(const dwg_ent_vertex_2d *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_2d_set_bulge(dwg_ent_vertex_2d *restrict vert,
                            const double  bulge,
                          int *restrict error);

// Get/Set tanget_direction of a vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_tangent_dir(const dwg_ent_vertex_2d *restrict vert,
                          int *restrict error);

EXPORT void
dwg_ent_vertex_2d_set_tangent_dir(dwg_ent_vertex_2d *restrict vert,
                                  const double  tangent_dir,
                                  int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR INSERT ENTITY                     *
********************************************************************/


// Get/Set insertion point of insert entity
EXPORT void
dwg_ent_insert_get_ins_pt(const dwg_ent_insert *restrict insert,
                          dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_insert_set_ins_pt(dwg_ent_insert *restrict insert,
                          const dwg_point_3d *restrict point,
                          int *restrict error);

// Get/Set scale flag of an insert entity
EXPORT char
dwg_ent_insert_get_scale_flag(const dwg_ent_insert *restrict insert,
                          int *restrict error);

EXPORT void
dwg_ent_insert_set_scale_flag(dwg_ent_insert *restrict insert,
                              const char flags,
                          int *restrict error);

EXPORT void
dwg_ent_insert_get_scale(const dwg_ent_insert *restrict insert,
                         dwg_point_3d *restrict point,
                         int *restrict error);

EXPORT void
dwg_ent_insert_set_scale(dwg_ent_insert *restrict insert,
                         const dwg_point_3d *restrict point,
                         int *restrict error);

EXPORT double
dwg_ent_insert_get_rotation(const dwg_ent_insert *restrict insert,
                          int *restrict error);

EXPORT void
dwg_ent_insert_set_rotation(dwg_ent_insert *restrict insert,
                            const double  rot_ang,
                            int *restrict error);

EXPORT void
dwg_ent_insert_get_extrusion(const dwg_ent_insert *restrict insert,
                             dwg_point_3d *restrict vector,
                             int *restrict error);

EXPORT void
dwg_ent_insert_set_extrusion(dwg_ent_insert *restrict insert,
                             const dwg_point_3d *restrict vector,
                             int *restrict error);

EXPORT char
dwg_ent_insert_has_attribs(dwg_ent_insert *restrict insert,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_insert_get_num_owned(const dwg_ent_insert *restrict insert,
                          int *restrict error);

//TODO dwg_ent_insert_add_owned, dwg_ent_insert_delete_owned

EXPORT dwg_object *
dwg_ent_insert_get_block_header(const dwg_ent_insert *restrict insert,
                          int *restrict error);

/********************************************************************
*                  FUNCTIONS FOR MINSERT ENTITY                     *
********************************************************************/

EXPORT void
dwg_ent_minsert_get_ins_pt(const dwg_ent_minsert *restrict minsert,
                           dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_minsert_set_ins_pt(dwg_ent_minsert *restrict minsert,
                           const dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT char
dwg_ent_minsert_get_scale_flag(const dwg_ent_minsert *restrict minsert,
                          int *restrict error);

EXPORT void
dwg_ent_minsert_set_scale_flag(dwg_ent_minsert *restrict minsert,
                               const char flags,
                               int *restrict error);

EXPORT void
dwg_ent_minsert_get_scale(const dwg_ent_minsert *restrict minsert,
                          dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_minsert_set_scale(dwg_ent_minsert *restrict minsert,
                          const dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT double
dwg_ent_minsert_get_rotation(const dwg_ent_minsert *restrict minsert,
                          int *restrict error);

EXPORT void
dwg_ent_minsert_set_rotation(dwg_ent_minsert *restrict minsert,
                             const double  rot_ang,
                             int *restrict error);

EXPORT void
dwg_ent_minsert_get_extrusion(const dwg_ent_minsert *restrict minsert,
                              dwg_point_3d *restrict vector,
                              int *restrict error);

EXPORT void
dwg_ent_minsert_set_extrusion(dwg_ent_minsert *restrict minsert,
                              const dwg_point_3d *restrict vector,
                              int *restrict error);

EXPORT char
dwg_ent_minsert_has_attribs(dwg_ent_minsert *restrict minsert,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_minsert_get_num_owned(const dwg_ent_minsert *restrict minsert,
                          int *restrict error);

//TODO dwg_ent_minsert_add_owned, dwg_ent_insert_delete_owned

EXPORT BITCODE_BL
dwg_ent_minsert_get_numcols(const dwg_ent_minsert *restrict minsert,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_minsert_get_numrows(const dwg_ent_minsert *restrict minsert,
                          int *restrict error);

//TODO dwg_ent_minsert_add_row, dwg_ent_insert_delete_row
//TODO dwg_ent_minsert_add_col, dwg_ent_insert_delete_col

EXPORT double
dwg_ent_minsert_get_col_spacing(const dwg_ent_minsert *restrict minsert,
                          int *restrict error);

EXPORT void
dwg_ent_minsert_set_col_spacing(dwg_ent_minsert *restrict minsert,
                                const double  spacing,
                                int *restrict error);

EXPORT double
dwg_ent_minsert_get_row_spacing(const dwg_ent_minsert *restrict minsert,
                          int *restrict error);

EXPORT void
dwg_ent_minsert_set_row_spacing(dwg_ent_minsert *restrict minsert,
                                const double  spacing,
                                int *restrict error);

EXPORT dwg_object *
dwg_ent_minsert_get_block_header(const dwg_ent_minsert *restrict minsert,
                                 int *restrict error);

/********************************************************************
*                FUNCTIONS FOR MLINESTYLE OBJECT                    *
********************************************************************/

EXPORT char *
dwg_obj_mlinestyle_get_name(const dwg_obj_mlinestyle *restrict mlinestyle,
                            int *restrict error);

EXPORT void
dwg_obj_mlinestyle_set_name(dwg_obj_mlinestyle *restrict mlinestyle,
                            const char *restrict name,
                            int *restrict error);

EXPORT char *
dwg_obj_mlinestyle_get_desc(const dwg_obj_mlinestyle *restrict mlinestyle,
                            int *restrict error);

EXPORT void
dwg_obj_mlinestyle_set_desc(dwg_obj_mlinestyle *restrict mlinestyle,
                            const char *restrict desc,
                            int *restrict error);

int
dwg_obj_mlinestyle_get_flag(const dwg_obj_mlinestyle *restrict mlinestyle,
                            int *restrict error);

EXPORT void
dwg_obj_mlinestyle_set_flag(dwg_obj_mlinestyle *restrict mlinestyle,
                            const int flags,
                            int *restrict error);

EXPORT double
dwg_obj_mlinestyle_get_start_angle(const dwg_obj_mlinestyle *restrict mlinestyle,
                                   int *restrict error);

EXPORT void
dwg_obj_mlinestyle_set_start_angle(dwg_obj_mlinestyle *restrict mlinestyle,
                                   const double startang,
                          int *restrict error);

EXPORT double
dwg_obj_mlinestyle_get_end_angle(const dwg_obj_mlinestyle *restrict mlinestyle,
                          int *restrict error);

EXPORT void
dwg_obj_mlinestyle_set_end_angle(dwg_obj_mlinestyle *restrict mlinestyle,
                                 const double endang,
                          int *restrict error);

EXPORT char
dwg_obj_mlinestyle_get_num_lines(const dwg_obj_mlinestyle *restrict mlinestyle,
                                 int *restrict error);

/********************************************************************
*               FUNCTIONS FOR APPID_CONTROL OBJECT                  *
*                       The APPID table                             *
********************************************************************/


EXPORT BITCODE_BS
dwg_obj_appid_control_get_num_entries(const dwg_obj_appid_control *restrict appid,
                                      int *restrict error);

EXPORT dwg_object_ref *
dwg_obj_appid_control_get_appid(const dwg_obj_appid_control *restrict appid,
                                const BITCODE_BS index,
                                int *restrict error);

/********************************************************************
*                    FUNCTIONS FOR APPID OBJECT                     *
*                       An APPID table entry                        *
********************************************************************/


EXPORT char *
dwg_obj_appid_get_entry_name(const dwg_obj_appid *restrict appid,
                          int *restrict error);

EXPORT void
dwg_obj_appid_set_entry_name(dwg_obj_appid *restrict appid,
                             const char *restrict entry_name,
                             int *restrict error);

EXPORT BITCODE_RC
dwg_obj_appid_get_flag(const dwg_obj_appid *restrict appid,
                          int *restrict error);

EXPORT void
dwg_obj_appid_set_flag(dwg_obj_appid *restrict appid,
                       const BITCODE_RC flag,
                          int *restrict error);

EXPORT dwg_obj_appid_control *
dwg_obj_appid_get_appid_control(const dwg_obj_appid *restrict appid,
                                int *restrict error);

/*******************************************************************
*            FUNCTIONS FOR ALL DIMENSION ENTITIES                  *
********************************************************************/

EXPORT char *
dwg_ent_dim_get_block_name(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT double
dwg_ent_dim_get_elevation(const dwg_ent_dim *restrict dim,
                          int *restrict error);
EXPORT void
dwg_ent_dim_set_elevation(dwg_ent_dim *restrict dim,
                          const double elevation,
                          int *restrict error);
EXPORT char
dwg_ent_dim_get_flag1(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_flag1(dwg_ent_dim *restrict dim, char flag,
                      int *restrict error);
EXPORT double
dwg_ent_dim_get_act_measurement(const dwg_ent_dim *restrict dim,
                                int *restrict error);
EXPORT void
dwg_ent_dim_set_act_measurement(dwg_ent_dim *restrict dim,
                                const double act_measurement,
                          int *restrict error);
EXPORT double
dwg_ent_dim_get_horiz_dir(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_horiz_dir(dwg_ent_dim *restrict dim, const double  horiz_dir,
                          int *restrict error);
EXPORT double
dwg_ent_dim_get_lspace_factor(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_lspace_factor(dwg_ent_dim *restrict dim,
                              const double factor,
                          int *restrict error);
EXPORT BITCODE_BS
dwg_ent_dim_get_lspace_style(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_lspace_style(dwg_ent_dim *restrict dim,
                             BITCODE_BS style,
                          int *restrict error);
EXPORT BITCODE_BS
dwg_ent_dim_get_attachment(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_attachment(dwg_ent_dim *restrict dim,
                           const BITCODE_BS attachment,
                          int *restrict error);
EXPORT void
dwg_ent_dim_set_extrusion(dwg_ent_dim *restrict dim,
                          dwg_point_3d *restrict vector,
                          int *restrict error);
EXPORT void
dwg_ent_dim_get_extrusion(const dwg_ent_dim *restrict dim,
                          dwg_point_3d *restrict vector,
                          int *restrict error);
/* (utf-8 encoded) */
EXPORT char *
dwg_ent_dim_get_user_text(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_user_text(dwg_ent_dim *restrict dim, const char *text,
                          int *restrict error);
EXPORT double
dwg_ent_dim_get_text_rot(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_text_rot(dwg_ent_dim *restrict dim,
                         const double rot,
                         int *restrict error);
EXPORT double
dwg_ent_dim_get_ins_rotation(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_ins_rotation(dwg_ent_dim *restrict dim,
                             const double rot,
                             int *restrict error);
EXPORT char
dwg_ent_dim_get_flip_arrow1(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_flip_arrow1(dwg_ent_dim *restrict dim,
                            const char flip_arrow,
                          int *restrict error);
EXPORT char
dwg_ent_dim_get_flip_arrow2(const dwg_ent_dim *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_set_flip_arrow2(dwg_ent_dim *restrict dim,
                            const char flip_arrow,
                          int *restrict error);
EXPORT void
dwg_ent_dim_set_text_mid_pt(dwg_ent_dim *restrict dim,
                            const dwg_point_2d *restrict point,
                          int *restrict error);
EXPORT void
dwg_ent_dim_get_text_mid_pt(const dwg_ent_dim *restrict dim,
                            dwg_point_2d *restrict point,
                          int *restrict error);
EXPORT void
dwg_ent_dim_set_ins_scale(dwg_ent_dim *restrict dim,
                          const dwg_point_3d *restrict point,
                          int *restrict error);
EXPORT void
dwg_ent_dim_get_ins_scale(const dwg_ent_dim *restrict dim,
                          dwg_point_3d *restrict point,
                          int *restrict error);
EXPORT void
dwg_ent_dim_set_clone_ins_pt(dwg_ent_dim *restrict dim,
                             const dwg_point_2d *restrict point,
                             int *restrict error);
EXPORT void
dwg_ent_dim_get_clone_ins_pt(const dwg_ent_dim *restrict dim,
                             dwg_point_2d *restrict point,
                             int *restrict error);

/********************************************************************
*            FUNCTIONS FOR ORDINATE DIMENSION ENTITY                *
********************************************************************/


EXPORT void
dwg_ent_dim_ordinate_set_ucsorigin_pt(dwg_ent_dim_ordinate *ord,
                                      const dwg_point_3d *restrict point,
                                      int *restrict error);

EXPORT void
dwg_ent_dim_ordinate_get_ucsorigin_pt(const dwg_ent_dim_ordinate *restrict ord,
                                      dwg_point_3d *restrict point,
                                      int *restrict error);

EXPORT void
dwg_ent_dim_ordinate_set_feature_location_pt(dwg_ent_dim_ordinate *restrict ord,
                                             const dwg_point_3d *restrict point,
                                             int *restrict error);

EXPORT void
dwg_ent_dim_ordinate_get_feature_location_pt(const dwg_ent_dim_ordinate *restrict ord,
                                             dwg_point_3d *restrict point,
                                             int *restrict error);

EXPORT void
dwg_ent_dim_ordinate_set_leader_endpt(dwg_ent_dim_ordinate *restrict ord,
                                      const dwg_point_3d *restrict point,
                                      int *restrict error);

EXPORT void
dwg_ent_dim_ordinate_get_leader_endpt(const dwg_ent_dim_ordinate *restrict ord,
                                      dwg_point_3d *restrict point,
                                      int *restrict error);

EXPORT void
dwg_ent_dim_ordinate_set_flag2(dwg_ent_dim_ordinate *restrict ord,
                               const char flag,
                               int *restrict error);

EXPORT char
dwg_ent_dim_ordinate_get_flag2(const dwg_ent_dim_ordinate *restrict ord,
                          int *restrict error);


/********************************************************************
*              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
********************************************************************/

EXPORT void
dwg_ent_dim_linear_set_def_pt(dwg_ent_dim_linear *restrict dim,
                              const dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_linear_get_def_pt(const dwg_ent_dim_linear *restrict dim,
                              dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_linear_set_13_pt(dwg_ent_dim_linear *restrict dim,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_linear_get_13_pt(const dwg_ent_dim_linear *restrict dim,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_linear_set_14_pt(dwg_ent_dim_linear *restrict dim,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_linear_get_14_pt(const dwg_ent_dim_linear *restrict dim,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT double
dwg_ent_dim_linear_get_dim_rot(const dwg_ent_dim_linear *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_linear_set_dim_rot(dwg_ent_dim_linear *restrict dim,
                               const double  rot,
                               int *restrict error);

EXPORT double
dwg_ent_dim_linear_get_ext_line_rotation(const dwg_ent_dim_linear *restrict dim,
                          int *restrict error);

EXPORT void
dwg_ent_dim_linear_set_ext_line_rotation(dwg_ent_dim_linear *restrict dim,
                                         const double  rot,
                                         int *restrict error);


/********************************************************************
*             FUNCTIONS FOR ALIGNED DIMENSION ENTITY                *
********************************************************************/

EXPORT void
dwg_ent_dim_aligned_set_def_pt(dwg_ent_dim_aligned *restrict dim,
                               const dwg_point_3d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_dim_aligned_get_def_pt(const dwg_ent_dim_aligned *restrict dim,
                               dwg_point_3d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_dim_aligned_set_13_pt(dwg_ent_dim_aligned *restrict dim,
                              const dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_aligned_get_13_pt(const dwg_ent_dim_aligned *restrict dim,
                              dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_aligned_set_14_pt(dwg_ent_dim_aligned *restrict dim,
                              const dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_aligned_get_14_pt(const dwg_ent_dim_aligned *restrict dim,
                              dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT double
dwg_ent_dim_aligned_get_ext_line_rotation(const dwg_ent_dim_aligned *restrict dim,
                                          int *restrict error);

EXPORT void
dwg_ent_dim_aligned_set_ext_line_rotation(dwg_ent_dim_aligned *restrict dim,
                                          const double  rot,
                                          int *restrict error);


/********************************************************************
*              FUNCTIONS FOR ANG3PT DIMENSION ENTITY                *
********************************************************************/

EXPORT void
dwg_ent_dim_ang3pt_set_def_pt(dwg_ent_dim_ang3pt *restrict ang,
                              const dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_ang3pt_get_def_pt(const dwg_ent_dim_ang3pt *restrict ang,
                              dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_ang3pt_set_13_pt(dwg_ent_dim_ang3pt *restrict ang,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang3pt_get_13_pt(const dwg_ent_dim_ang3pt *restrict ang,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang3pt_set_14_pt(dwg_ent_dim_ang3pt *restrict ang,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang3pt_get_14_pt(const dwg_ent_dim_ang3pt *restrict ang,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang3pt_set_first_arc_pt(dwg_ent_dim_ang3pt *restrict ang,
                                    const dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_dim_ang3pt_get_first_arc_pt(const dwg_ent_dim_ang3pt *restrict ang,
                                    dwg_point_3d *restrict point,
                                    int *restrict error);


/********************************************************************
*              FUNCTIONS FOR ANG2LN DIMENSION ENTITY                *
********************************************************************/

EXPORT void
dwg_ent_dim_ang2ln_set_def_pt(dwg_ent_dim_ang2ln *restrict ang,
                              const dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_get_def_pt(const dwg_ent_dim_ang2ln *restrict ang,
                              dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_set_13_pt(dwg_ent_dim_ang2ln *restrict ang,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_get_13_pt(const dwg_ent_dim_ang2ln *restrict ang,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_set_14_pt(dwg_ent_dim_ang2ln *restrict ang,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_get_14_pt(const dwg_ent_dim_ang2ln *restrict ang,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_set_first_arc_pt(dwg_ent_dim_ang2ln *restrict ang,
                                    const dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_get_first_arc_pt(const dwg_ent_dim_ang2ln *restrict ang,
                                    dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_set_16_pt(dwg_ent_dim_ang2ln *restrict ang,
                             const dwg_point_2d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_dim_ang2ln_get_16_pt(const dwg_ent_dim_ang2ln *restrict ang,
                             dwg_point_2d *restrict point,
                             int *restrict error);


/********************************************************************
*              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
********************************************************************/

EXPORT void
dwg_ent_dim_radius_set_def_pt(dwg_ent_dim_radius *restrict radius,
                              const dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_radius_get_def_pt(const dwg_ent_dim_radius *restrict radius,
                              dwg_point_3d *restrict point,
                              int *restrict error);

EXPORT void
dwg_ent_dim_radius_set_first_arc_pt(dwg_ent_dim_radius *restrict radius,
                                    const dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_dim_radius_get_first_arc_pt(const dwg_ent_dim_radius *restrict radius,
                                    dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT double
dwg_ent_dim_radius_get_leader_length(const dwg_ent_dim_radius *restrict radius,
                          int *restrict error);

EXPORT void
dwg_ent_dim_radius_set_leader_length(dwg_ent_dim_radius *restrict radius,
                                     const double  length,
                                     int *restrict error);


/********************************************************************
*             FUNCTIONS FOR DIAMETER DIMENSION ENTITY               *
********************************************************************/

EXPORT void
dwg_ent_dim_diameter_set_def_pt(dwg_ent_dim_diameter *restrict dia,
                                const dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_dim_diameter_get_def_pt(const dwg_ent_dim_diameter *restrict dia,
                                dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_dim_diameter_set_first_arc_pt(dwg_ent_dim_diameter *restrict dia,
                                      const dwg_point_3d *restrict point,
                                      int *restrict error);

EXPORT void
dwg_ent_dim_diameter_get_first_arc_pt(const dwg_ent_dim_diameter *restrict dia,
                                      dwg_point_3d *restrict point,
                                      int *restrict error);

EXPORT double
dwg_ent_dim_diameter_get_leader_length(const dwg_ent_dim_diameter *restrict dia,
                          int *restrict error);

EXPORT void
dwg_ent_dim_diameter_set_leader_length(dwg_ent_dim_diameter *restrict dia,
                                       const double leader_len,
                          int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR ENDBLK ENTITY                     *
********************************************************************/


/********************************************************************
*                   FUNCTIONS FOR SEQEND ENTITY                     *
********************************************************************/


/********************************************************************
*                    FUNCTIONS FOR SHAPE ENTITY                     *
********************************************************************/


EXPORT void
dwg_ent_shape_get_ins_pt(const dwg_ent_shape *restrict shape,
                         dwg_point_3d *restrict point,
                         int *restrict error);

EXPORT void
dwg_ent_shape_set_ins_pt(dwg_ent_shape *restrict shape,
                         const dwg_point_3d *restrict point,
                         int *restrict error);

EXPORT double
dwg_ent_shape_get_scale(const dwg_ent_shape *restrict shape,
                          int *restrict error);

EXPORT void
dwg_ent_shape_set_scale(dwg_ent_shape *restrict shape,
                        const double  scale,
                          int *restrict error);

EXPORT double
dwg_ent_shape_get_rotation(const dwg_ent_shape *restrict shape,
                          int *restrict error);

EXPORT void
dwg_ent_shape_set_rotation(dwg_ent_shape *restrict shape,
                           const double  rotation,
                          int *restrict error);

EXPORT double
dwg_ent_shape_get_width_factor(const dwg_ent_shape *restrict shape,
                          int *restrict error);

EXPORT void
dwg_ent_shape_set_width_factor(dwg_ent_shape *restrict shape,
                               const double  width_factor,
                               int *restrict error);

EXPORT double
dwg_ent_shape_get_oblique(const dwg_ent_shape *restrict shape,
                          int *restrict error);

EXPORT void
dwg_ent_shape_set_oblique(dwg_ent_shape *restrict shape,
                          const double  oblique,
                          int *restrict error);

EXPORT double
dwg_ent_shape_get_thickness(const dwg_ent_shape *restrict shape,
                          int *restrict error);

EXPORT void
dwg_ent_shape_set_thickness(dwg_ent_shape *restrict shape, const double  thickness,
                            int *restrict error);

EXPORT double
dwg_ent_shape_get_shape_no(const dwg_ent_shape *restrict shape,
                          int *restrict error);

EXPORT void
dwg_ent_shape_set_shape_no(dwg_ent_shape *restrict shape,
                           const double  no,
                          int *restrict error);

EXPORT void
dwg_ent_shape_get_extrusion(const dwg_ent_shape *restrict shape,
                            dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_shape_set_extrusion(dwg_ent_shape *restrict shape,
                            const dwg_point_3d *restrict vector,
                            int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR MTEXT ENTITY                     *
********************************************************************/


EXPORT void
dwg_ent_mtext_set_insertion_pt(dwg_ent_mtext *restrict mtext,
                               const dwg_point_3d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_mtext_get_insertion_pt(const dwg_ent_mtext *restrict mtext,
                               dwg_point_3d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_mtext_set_extrusion(dwg_ent_mtext *restrict mtext,
                            const dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_mtext_get_extrusion(const dwg_ent_mtext *restrict mtext,
                            dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_mtext_set_x_axis_dir(dwg_ent_mtext *restrict mtext,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_mtext_get_x_axis_dir(const dwg_ent_mtext *restrict mtext,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_mtext_set_rect_height(dwg_ent_mtext *restrict mtext,
                              const double  rect_height,
                              int *restrict error);

EXPORT double
dwg_ent_mtext_get_rect_height(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT void
dwg_ent_mtext_set_rect_width(dwg_ent_mtext *restrict mtext,
                             const double  rect_width,
                             int *restrict error);

EXPORT double
dwg_ent_mtext_get_rect_width(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT void
dwg_ent_mtext_set_text_height(dwg_ent_mtext *restrict mtext,
                              const double  text_height,
                              int *restrict error);

EXPORT double
dwg_ent_mtext_get_text_height(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_mtext_get_attachment(const dwg_ent_mtext *restrict mtext,
                             int *restrict error);

EXPORT void
dwg_ent_mtext_set_attachment(dwg_ent_mtext *restrict mtext,
                             const BITCODE_BS attachment,
                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_mtext_get_drawing_dir(const dwg_ent_mtext *restrict mtext,
                              int *restrict error);

EXPORT void
dwg_ent_mtext_set_drawing_dir(dwg_ent_mtext *restrict mtext,
                              const BITCODE_BS dir,
                              int *restrict error);

EXPORT double
dwg_ent_mtext_get_extents_height(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT void
dwg_ent_mtext_set_extents_height(dwg_ent_mtext *restrict mtext,
                                 const double  ht,
                          int *restrict error);

EXPORT double
dwg_ent_mtext_get_extents_width(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT void
dwg_ent_mtext_set_extents_width(dwg_ent_mtext *restrict mtext,
                                const double  wid,
                          int *restrict error);

/* (utf-8 encoded) */
EXPORT char* dwg_ent_mtext_get_text(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT void
dwg_ent_mtext_set_text(dwg_ent_mtext *restrict mtext, char *text,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_mtext_get_linespace_style(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT void
dwg_ent_mtext_set_linespace_style(dwg_ent_mtext *restrict mtext,
                                  const BITCODE_BS style,
                                  int *restrict error);

EXPORT double
dwg_ent_mtext_get_linespace_factor(const dwg_ent_mtext *restrict mtext,
                          int *restrict error);

EXPORT void
dwg_ent_mtext_set_linespace_factor(dwg_ent_mtext *restrict mtext,
                                   const double  factor,
                                   int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR LEADER ENTITY                     *
********************************************************************/


EXPORT void
dwg_ent_leader_set_annot_type(dwg_ent_leader *restrict leader,
                              const BITCODE_BS type,
                              int *restrict error);

EXPORT BITCODE_BS
dwg_ent_leader_get_annot_type(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_path_type(dwg_ent_leader *restrict leader,
                             const BITCODE_BS type,
                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_leader_get_path_type(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_leader_get_numpts(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_origin(dwg_ent_leader *restrict leader,
                          const dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_leader_get_origin(const dwg_ent_leader *restrict leader,
                          dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_extrusion(dwg_ent_leader *restrict leader,
                             const dwg_point_3d *restrict vector,
                             int *restrict error);

EXPORT void
dwg_ent_leader_get_extrusion(const dwg_ent_leader *restrict leader,
                             dwg_point_3d *restrict vector,
                             int *restrict error);

EXPORT void
dwg_ent_leader_set_x_direction(dwg_ent_leader *restrict leader,
                               const dwg_point_3d *restrict vector,
                               int *restrict error);

EXPORT void
dwg_ent_leader_get_x_direction(const dwg_ent_leader *restrict leader,
                               dwg_point_3d *restrict vector,
                               int *restrict error);

EXPORT void
dwg_ent_leader_set_offset_to_block_ins_pt(dwg_ent_leader *restrict leader,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error);

EXPORT void
dwg_ent_leader_get_offset_to_block_ins_pt(const dwg_ent_leader *restrict leader,
                                          dwg_point_3d *restrict point,
                                          int *restrict error);

EXPORT void
dwg_ent_leader_set_dimgap(dwg_ent_leader *restrict leader,
                          const double dimgap,
                          int *restrict error);

EXPORT double
dwg_ent_leader_get_dimgap(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_box_height(dwg_ent_leader *restrict leader,
                              const double  height,
                              int *restrict error);

EXPORT double
dwg_ent_leader_get_box_height(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_box_width(dwg_ent_leader *restrict leader,
                             const double  width,
                             int *restrict error);

EXPORT double
dwg_ent_leader_get_box_width(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_hook_line_on_x_dir(dwg_ent_leader *restrict leader, const char hook,
                                      int *restrict error);

EXPORT char
dwg_ent_leader_get_hook_line_on_x_dir(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_arrowhead_on(dwg_ent_leader *restrict leader, const char arrow,
                                int *restrict error);

EXPORT char
dwg_ent_leader_get_arrowhead_on(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_arrowhead_type(dwg_ent_leader *restrict leader,
                                  const BITCODE_BS type,
                                  int *restrict error);

EXPORT BITCODE_BS
dwg_ent_leader_get_arrowhead_type(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_dimasz(dwg_ent_leader *restrict leader,
                          const double dimasz,
                          int *restrict error);

EXPORT double
dwg_ent_leader_get_dimasz(const dwg_ent_leader *restrict leader,
                          int *restrict error);

EXPORT void
dwg_ent_leader_set_byblock_color(dwg_ent_leader *restrict leader,
                                 const BITCODE_BS color,
                                 int *restrict error);

EXPORT BITCODE_BS
dwg_ent_leader_get_byblock_color(const dwg_ent_leader *restrict leader,
                                 int *restrict error);


/********************************************************************
*                  FUNCTIONS FOR TOLERANCE ENTITY                   *
********************************************************************/


EXPORT void
dwg_ent_tolerance_set_height(dwg_ent_tolerance *restrict tol,
                             const double  height,
                             int *restrict error);

EXPORT double
dwg_ent_tolerance_get_height(const dwg_ent_tolerance *restrict tol,
                             int *restrict error);

EXPORT void
dwg_ent_tolerance_set_dimgap(dwg_ent_tolerance *restrict tol,
                             const double  dimgap,
                             int *restrict error);

EXPORT double
dwg_ent_tolerance_get_dimgap(const dwg_ent_tolerance *restrict tol,
                             int *restrict error);

EXPORT void
dwg_ent_tolerance_set_ins_pt(dwg_ent_tolerance *restrict tol,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_tolerance_get_ins_pt(const dwg_ent_tolerance *restrict tol,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_tolerance_set_x_direction(dwg_ent_tolerance *restrict tol,
                                  const dwg_point_3d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_tolerance_get_x_direction(const dwg_ent_tolerance *restrict tol,
                                  dwg_point_3d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_tolerance_set_extrusion(dwg_ent_tolerance *restrict tol,
                                const dwg_point_3d *restrict vector,
                                int *restrict error);

EXPORT void
dwg_ent_tolerance_get_extrusion(const dwg_ent_tolerance *restrict tol,
                                dwg_point_3d *restrict vector,
                                int *restrict error);

/* (utf-8 encoded) */
EXPORT void
dwg_ent_tolerance_set_text_string(dwg_ent_tolerance *restrict tol,
                                  const char *string,
                                  int *restrict error);

EXPORT char *
dwg_ent_tolerance_get_text_string(const dwg_ent_tolerance *restrict tol,
                                  int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR LWPOLYLINE ENTITY                    *
********************************************************************/


EXPORT BITCODE_BS
dwg_ent_lwpline_get_flag(const dwg_ent_lwpline *restrict lwpline,
                         int *restrict error);

EXPORT void
dwg_ent_lwpline_set_flag(dwg_ent_lwpline *restrict lwpline,
                         const char flags,
                          int *restrict error);

EXPORT double
dwg_ent_lwpline_get_const_width(const dwg_ent_lwpline *restrict lwpline,
                                int *restrict error);

EXPORT void
dwg_ent_lwpline_set_const_width(dwg_ent_lwpline *restrict lwpline,
                                const double const_width,
                                int *restrict error);

EXPORT double
dwg_ent_lwpline_get_elevation(const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error);

EXPORT void
dwg_ent_lwpline_set_elevation(dwg_ent_lwpline *restrict lwpline,
                              const double elevation,
                              int *restrict error);

EXPORT double
dwg_ent_lwpline_get_thickness(const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error);

EXPORT void
dwg_ent_lwpline_set_thickness(dwg_ent_lwpline *restrict lwpline,
                              const double thickness,
                              int *restrict error);

EXPORT void
dwg_ent_lwpline_get_normal(const dwg_ent_lwpline *restrict lwpline,
                           dwg_point_3d *points,
                           int *restrict error);
EXPORT void
dwg_ent_lwpline_set_normal(dwg_ent_lwpline *restrict lwpline,
                           const dwg_point_3d *points,
                           int *restrict error);

EXPORT BITCODE_BL
dwg_ent_lwpline_get_numpoints(const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error);

EXPORT BITCODE_BL
dwg_ent_lwpline_get_numbulges(const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error);

EXPORT BITCODE_BL
dwg_ent_lwpline_get_numwidths(const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error);

EXPORT double *
dwg_ent_lwpline_get_bulges(const dwg_ent_lwpline *restrict lwpline,
                           int *restrict error);

EXPORT dwg_point_2d *
dwg_ent_lwpline_get_points(const dwg_ent_lwpline *restrict lwpline,
                           int *restrict error);

EXPORT dwg_lwpline_widths *
dwg_ent_lwpline_get_widths(const dwg_ent_lwpline *restrict lwpline,
                           int *restrict error);


/********************************************************************
*                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
********************************************************************/


EXPORT BITCODE_BS
dwg_ent_ole2frame_get_flag(const dwg_ent_ole2frame *restrict frame,
                           int *restrict error);

EXPORT void
dwg_ent_ole2frame_set_flag(dwg_ent_ole2frame *restrict frame,
                           const BITCODE_BS flags,
                           int *restrict error);

EXPORT BITCODE_BS
dwg_ent_ole2frame_get_mode(const dwg_ent_ole2frame *restrict frame,
                           int *restrict error);

EXPORT void
dwg_ent_ole2frame_set_mode(dwg_ent_ole2frame *restrict frame,

                           const BITCODE_BS mode,
                           int *restrict error);

EXPORT BITCODE_BL
dwg_ent_ole2frame_get_data_length(const dwg_ent_ole2frame *restrict frame,
                                  int *restrict error);

EXPORT void
dwg_ent_ole2frame_set_data_length(dwg_ent_ole2frame *restrict frame,
                                  const BITCODE_BL data_length,
                                  int *restrict error);

EXPORT char *
dwg_ent_ole2frame_get_data(const dwg_ent_ole2frame *restrict frame,
                           int *restrict error);

EXPORT void
dwg_ent_ole2frame_set_data(dwg_ent_ole2frame *restrict frame,
                           const char *data,
                          int *restrict error);


/********************************************************************
*                  FUNCTIONS FOR PROXY OBJECT                       *
********************************************************************/


EXPORT BITCODE_BL
dwg_obj_proxy_get_class_id(const dwg_obj_proxy *restrict proxy,
                           int *restrict error);

EXPORT void
dwg_obj_proxy_set_class_id(dwg_obj_proxy *restrict proxy,
                           const BITCODE_BL class_id,
                           int *restrict error);

EXPORT BITCODE_BL
dwg_obj_proxy_get_version(const dwg_obj_proxy *restrict proxy,
                          int *restrict error);

EXPORT void
dwg_obj_proxy_set_version(dwg_obj_proxy *restrict proxy,
                          const BITCODE_BL version,
                          int *restrict error);

EXPORT BITCODE_B
dwg_obj_proxy_get_from_dxf(const dwg_obj_proxy *restrict proxy,
                           int *restrict error);

EXPORT void
dwg_obj_proxy_set_from_dxf(dwg_obj_proxy *restrict proxy,
                           const BITCODE_B from_dxf,
                           int *restrict error);

EXPORT char*
dwg_obj_proxy_get_data(const dwg_obj_proxy *restrict proxy,
                       int *restrict error);

EXPORT void
dwg_obj_proxy_set_data(dwg_obj_proxy *restrict proxy,
                       const char* data,
                       int *restrict error);

EXPORT dwg_object_ref*
dwg_obj_proxy_get_parenthandle(const dwg_obj_proxy *restrict proxy,
                               int *restrict error);

Dwg_Object_Ref**
dwg_obj_proxy_get_reactors(const dwg_obj_proxy *restrict proxy,
                           int *restrict error);

EXPORT dwg_object_ref**
dwg_obj_proxy_get_objid_object_handles(const dwg_obj_proxy *restrict proxy,
                                       int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR SPLINE ENTITY                     *
********************************************************************/


EXPORT BITCODE_BS
dwg_ent_spline_get_scenario(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT void
dwg_ent_spline_set_scenario(dwg_ent_spline *restrict spline,
                            const BITCODE_BS scenario,
                            int *restrict error);

EXPORT BITCODE_BS
dwg_ent_spline_get_degree(const dwg_ent_spline *restrict spline,
                          int *restrict error);

EXPORT void
dwg_ent_spline_set_degree(dwg_ent_spline *restrict spline,
                          const BITCODE_BS degree,
                          int *restrict error);

EXPORT double
dwg_ent_spline_get_fit_tol(const dwg_ent_spline *restrict spline,
                           int *restrict error);

EXPORT void
dwg_ent_spline_set_fit_tol(dwg_ent_spline *restrict spline,
                           const int fit_tol,
                           int *restrict error);

EXPORT void
dwg_ent_spline_get_begin_tan_vector(const dwg_ent_spline *restrict spline,
                                    dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_spline_set_begin_tan_vector(dwg_ent_spline *restrict spline,
                                    const dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_spline_get_end_tan_vector(const dwg_ent_spline *restrict spline,
                                  dwg_point_3d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_spline_set_end_tan_vector(dwg_ent_spline *restrict spline,
                                  const dwg_point_3d *restrict point,
                                  int *restrict error);

EXPORT double
dwg_ent_spline_get_knot_tol(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT void
dwg_ent_spline_set_knot_tol(dwg_ent_spline *restrict spline,
                            const double  knot_tol,
                            int *restrict error);

EXPORT double
dwg_ent_spline_get_ctrl_tol(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT void
dwg_ent_spline_set_ctrl_tol(dwg_ent_spline *restrict spline,
                            const double  ctrl_tol,
                            int *restrict error);

EXPORT BITCODE_BS
dwg_ent_spline_get_num_fit_pts(const dwg_ent_spline *restrict spline,
                               int *restrict error);

//TODO: dwg_ent_spline_add_fit_pts, dwg_ent_spline_delete_fit_pts

EXPORT char
dwg_ent_spline_get_rational(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT void
dwg_ent_spline_set_rational(dwg_ent_spline *restrict spline,
                            const char rational,
                            int *restrict error);

EXPORT char
dwg_ent_spline_get_closed_b(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT void
dwg_ent_spline_set_closed_b(dwg_ent_spline *restrict spline,
                            const char closed_b,
                            int *restrict error);

EXPORT char
dwg_ent_spline_get_weighted(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT void
dwg_ent_spline_set_weighted(dwg_ent_spline *restrict spline,
                            char weighted,
                            int *restrict error);

EXPORT char
dwg_ent_spline_get_periodic(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT void
dwg_ent_spline_set_periodic(dwg_ent_spline *restrict spline,
                            char periodic,
                            int *restrict error);

EXPORT BITCODE_BL
dwg_ent_spline_get_num_knots(const dwg_ent_spline *restrict spline,
                             int *restrict error);

//TODO: dwg_ent_spline_add_knots, dwg_ent_spline_delete_knots

EXPORT BITCODE_BL
dwg_ent_spline_get_num_ctrl_pts(const dwg_ent_spline *restrict spline,
                                int *restrict error);

//TODO: dwg_ent_spline_add_ctrl_pts, dwg_ent_spline_delete_ctrl_pts

EXPORT dwg_spline_point *
dwg_ent_spline_get_fit_pts(const dwg_ent_spline *restrict spline,
                           int *restrict error);

EXPORT dwg_spline_control_point *
dwg_ent_spline_get_ctrl_pts(const dwg_ent_spline *restrict spline,
                            int *restrict error);

EXPORT double *
dwg_ent_spline_get_knots(const dwg_ent_spline *restrict spline,
                         int *restrict error);


/********************************************************************
*                   FUNCTIONS FOR VIEWPORT ENTITY                   *
********************************************************************/


EXPORT void
dwg_ent_viewport_get_center(const dwg_ent_viewport *restrict vp,
                            dwg_point_3d *restrict point,
                            int *restrict error);

EXPORT void
dwg_ent_viewport_set_center(dwg_ent_viewport *restrict vp,
                            const dwg_point_3d *restrict point,
                            int *restrict error);

EXPORT double
dwg_ent_viewport_get_width(const dwg_ent_viewport *restrict vp,
                           int *restrict error);

EXPORT void
dwg_ent_viewport_set_width(dwg_ent_viewport *restrict vp,
                           const double  width,
                          int *restrict error);

EXPORT double
dwg_ent_viewport_get_height(const dwg_ent_viewport *restrict vp,
                            int *restrict error);

EXPORT void
dwg_ent_viewport_set_height(dwg_ent_viewport *restrict vp,
                            const double  height,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_viewport_get_grid_major(const dwg_ent_viewport *restrict vp,
                                int *restrict error);

EXPORT void
dwg_ent_viewport_set_grid_major(dwg_ent_viewport *restrict vp,
                                const BITCODE_BS major,
                                int *restrict error);

EXPORT BITCODE_BL
dwg_ent_viewport_get_num_frozen_layers(const dwg_ent_viewport *restrict vp,
                                       int *restrict error);

EXPORT void
dwg_ent_viewport_set_num_frozen_layers(dwg_ent_viewport *restrict vp,
                                       const BITCODE_BL count,
                                       int *restrict error);

EXPORT char *
dwg_ent_viewport_get_style_sheet(const dwg_ent_viewport *restrict vp,
                                 int *restrict error);

EXPORT void
dwg_ent_viewport_set_style_sheet(dwg_ent_viewport *restrict vp, char *sheet,
                                 int *restrict error);

EXPORT void
dwg_ent_viewport_set_circle_zoom(dwg_ent_viewport *restrict vp,
                                 const BITCODE_BS zoom,
                                 int *restrict error);

EXPORT BITCODE_BS
dwg_ent_viewport_get_circle_zoom(const dwg_ent_viewport *restrict vp,
                                 int *restrict error);

EXPORT void
dwg_ent_viewport_set_status_flag(dwg_ent_viewport *restrict vp,
                                 const BITCODE_BL flags,
                                 int *restrict error);

EXPORT BITCODE_BL
dwg_ent_viewport_get_status_flag(const dwg_ent_viewport *restrict vp,
                                 int *restrict error);

EXPORT char
dwg_ent_viewport_get_render_mode(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_render_mode(dwg_ent_viewport *restrict vp,
                                 char mode,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_ucs_at_origin(dwg_ent_viewport *restrict vp,
                                   unsigned char origin,
                                   int *restrict error);

EXPORT unsigned char
dwg_ent_viewport_get_ucs_at_origin(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_ucs_per_viewport(dwg_ent_viewport *restrict vp,
                                      unsigned char viewport,
                          int *restrict error);

EXPORT unsigned char
dwg_ent_viewport_get_ucs_per_viewport(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_view_target(dwg_ent_viewport *restrict vp,
                                 const dwg_point_3d *restrict point,
                                 int *restrict error);

EXPORT void
dwg_ent_viewport_get_view_target(const dwg_ent_viewport *restrict vp,
                                 dwg_point_3d *restrict point,
                                 int *restrict error);

EXPORT void
dwg_ent_viewport_set_view_direction(dwg_ent_viewport *restrict vp,
                                    const dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_viewport_get_view_direction(const dwg_ent_viewport *restrict vp,
                                    dwg_point_3d *restrict point,
                                    int *restrict error);

EXPORT void
dwg_ent_viewport_set_view_twist(dwg_ent_viewport *restrict vp,
                                const double  angle,
                                int *restrict error);

EXPORT double
dwg_ent_viewport_get_view_twist(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_view_height(dwg_ent_viewport *restrict vp,
                                 const double  height,
                                 int *restrict error);

EXPORT double
dwg_ent_viewport_get_view_height(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_lens_length(dwg_ent_viewport *restrict vp,
                                 const double  length,
                                 int *restrict error);

EXPORT double
dwg_ent_viewport_get_lens_length(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_front_clip_z(dwg_ent_viewport *restrict vp,
                                  const double  front_z,
                                  int *restrict error);

EXPORT double
dwg_ent_viewport_get_front_clip_z(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_back_clip_z(dwg_ent_viewport *restrict vp,
                                 const double  back_z,
                                 int *restrict error);

EXPORT double
dwg_ent_viewport_get_back_clip_z(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_snap_angle(dwg_ent_viewport *restrict vp,
                                const double  angle,
                                int *restrict error);

EXPORT double
dwg_ent_viewport_get_snap_angle(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_get_view_center(const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
                                 int *restrict error);

EXPORT void
dwg_ent_viewport_set_view_center(dwg_ent_viewport *restrict vp,
                                 const dwg_point_2d *restrict point,
                                 int *restrict error);

EXPORT void
dwg_ent_viewport_get_grid_spacing(const dwg_ent_viewport *restrict vp,
                                  dwg_point_2d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_viewport_set_grid_spacing(dwg_ent_viewport *restrict vp,
                                  const dwg_point_2d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_viewport_get_snap_base(const dwg_ent_viewport *restrict vp,
                               dwg_point_2d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_viewport_set_snap_base(dwg_ent_viewport *restrict vp,
                               const dwg_point_2d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_viewport_get_snap_spacing(const dwg_ent_viewport *restrict vp,
                                  dwg_point_2d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_viewport_set_snap_spacing(dwg_ent_viewport *restrict vp,
                                  const dwg_point_2d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_viewport_set_ucs_origin(dwg_ent_viewport *restrict vp,
                                const dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_viewport_get_ucs_origin(const dwg_ent_viewport *restrict vp,
                                dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_viewport_set_ucs_x_axis(dwg_ent_viewport *restrict vp,
                                const dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_viewport_get_ucs_x_axis(const dwg_ent_viewport *restrict vp,
                                dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_viewport_set_ucs_y_axis(dwg_ent_viewport *restrict vp,
                                const dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_viewport_get_ucs_y_axis(const dwg_ent_viewport *restrict vp,
                                dwg_point_3d *restrict point,
                                int *restrict error);

EXPORT void
dwg_ent_viewport_set_ucs_elevation(dwg_ent_viewport *restrict vp,
                                   const double  elevation,
                                   int *restrict error);

EXPORT double
dwg_ent_viewport_get_ucs_elevation(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_ucs_ortho_view_type(dwg_ent_viewport *restrict vp, 
                                         const BITCODE_BS type,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_viewport_get_ucs_ortho_view_type(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_shadeplot_mode(dwg_ent_viewport *restrict vp,
                                    const BITCODE_BS shadeplot,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_viewport_get_shadeplot_mode(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_use_default_lights(dwg_ent_viewport *restrict vp,
                                        const unsigned char lights,
                          int *restrict error);

EXPORT unsigned char
dwg_ent_viewport_get_use_default_lights(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_default_lighting_type(dwg_ent_viewport *restrict vp,
                                           const char type,
                                           int *restrict error);

EXPORT char
dwg_ent_viewport_get_default_lighting_type(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_brightness(dwg_ent_viewport *restrict vp,
                                const double brightness,
                                int *restrict error);

EXPORT double
dwg_ent_viewport_get_brightness(const dwg_ent_viewport *restrict vp,
                          int *restrict error);

EXPORT void
dwg_ent_viewport_set_contrast(dwg_ent_viewport *restrict vp,
                              const double  contrast,
                              int *restrict error);

EXPORT double
dwg_ent_viewport_get_contrast(const dwg_ent_viewport *restrict vp,
                          int *restrict error);


/********************************************************************
*                FUNCTIONS FOR POLYLINE PFACE ENTITY                *
********************************************************************/


EXPORT BITCODE_BS
dwg_ent_polyline_pface_get_numpoints(const dwg_ent_polyline_pface *restrict pface,
                                     int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_pface_get_numfaces(const dwg_ent_polyline_pface *restrict pface,
                                    int *restrict error);

EXPORT dwg_point_3d *
dwg_obj_polyline_pface_get_points(const dwg_object *restrict obj,
                                  int *restrict error);

/********************************************************************
*                FUNCTIONS FOR POLYLINE_MESH ENTITY                 *
********************************************************************/


EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_flag(const dwg_ent_polyline_mesh *restrict mesh,
                               int *restrict error);

EXPORT void
dwg_ent_polyline_mesh_set_flag(dwg_ent_polyline_mesh *restrict mesh,
                               const BITCODE_BS flags,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_curve_type(const dwg_ent_polyline_mesh *restrict mesh,
                                     int *restrict error);

EXPORT void
dwg_ent_polyline_mesh_set_curve_type(dwg_ent_polyline_mesh *restrict mesh,
                                     const BITCODE_BS curve_type,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_num_m_verts(const dwg_ent_polyline_mesh *restrict mesh,
                                      int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_num_n_verts(const dwg_ent_polyline_mesh *restrict mesh,
                                      int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_m_density(const dwg_ent_polyline_mesh *restrict mesh,
                                    int *restrict error);

EXPORT void
dwg_ent_polyline_mesh_set_m_density(dwg_ent_polyline_mesh *restrict mesh,
                                    const BITCODE_BS m_density,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_mesh_get_n_density(const dwg_ent_polyline_mesh *restrict mesh,
                                    int *restrict error);

EXPORT void
dwg_ent_polyline_mesh_set_n_density(dwg_ent_polyline_mesh *restrict mesh,
                                    const BITCODE_BS n_density,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_polyline_mesh_get_num_owned(const dwg_ent_polyline_mesh *restrict mesh,
                                    int *restrict error);

//TODO dwg_ent_*_add_owned, dwg_ent_insert_delete_owned


/********************************************************************
*                 FUNCTIONS FOR POLYLINE_2D ENTITY                  *
********************************************************************/


EXPORT void
dwg_ent_polyline_2d_get_extrusion(const dwg_ent_polyline_2d *restrict pline2d,
                                  dwg_point_3d *restrict vector,
                                  int *restrict error);

EXPORT void
dwg_ent_polyline_2d_set_extrusion(dwg_ent_polyline_2d *restrict pline2d,
                                  const dwg_point_3d *restrict vector,
                                  int *restrict error);

EXPORT double
dwg_ent_polyline_2d_get_start_width(const dwg_ent_polyline_2d *restrict pline2d,
                                    int *restrict error);

EXPORT void
dwg_ent_polyline_2d_set_start_width(dwg_ent_polyline_2d *restrict pline2d,
                                    const double start_width,
                          int *restrict error);

EXPORT double
dwg_ent_polyline_2d_get_end_width(const dwg_ent_polyline_2d *restrict pline2d,
                                  int *restrict error);

EXPORT void
dwg_ent_polyline_2d_set_end_width(dwg_ent_polyline_2d *restrict pline2d,
                                  const double end_width,
                          int *restrict error);

EXPORT double
dwg_ent_polyline_2d_get_thickness(const dwg_ent_polyline_2d *restrict pline2d,
                                  int *restrict error);

EXPORT void
dwg_ent_polyline_2d_set_thickness(dwg_ent_polyline_2d *restrict pline2d,
                                  const double thickness,
                          int *restrict error);

EXPORT double
dwg_ent_polyline_2d_get_elevation(const dwg_ent_polyline_2d *restrict pline2d,
                                  int *restrict error);

EXPORT void
dwg_ent_polyline_2d_set_elevation(dwg_ent_polyline_2d *restrict pline2d,
                                  const double elevation,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_2d_get_flag(const dwg_ent_polyline_2d *restrict pline2d,
                             int *restrict error);

EXPORT void
dwg_ent_polyline_2d_set_flag(dwg_ent_polyline_2d *restrict pline2d,
                             const BITCODE_BS flags,
                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_polyline_2d_get_curve_type(const dwg_ent_polyline_2d *restrict pline2d,
                                   int *restrict error);

EXPORT void
dwg_ent_polyline_2d_set_curve_type(dwg_ent_polyline_2d *restrict pline2d,
                                   const BITCODE_BS curve_type,
                                   int *restrict error);

EXPORT BITCODE_BL
dwg_obj_polyline_2d_get_numpoints(const dwg_object *restrict obj,
                                  int *restrict error);

EXPORT dwg_point_2d *
dwg_obj_polyline_2d_get_points(const dwg_object *restrict obj,
                               int *restrict error);


/********************************************************************
*                 FUNCTIONS FOR POLYLINE_3D ENTITY                  *
********************************************************************/


EXPORT char
dwg_ent_polyline_3d_get_flag(const dwg_ent_polyline_3d *restrict pline3d,
                             int *restrict error);

EXPORT void
dwg_ent_polyline_3d_set_flag(dwg_ent_polyline_3d *restrict pline3d,
                             const char flag,
                             int *restrict error);

EXPORT char
dwg_ent_polyline_3d_get_flag2(const dwg_ent_polyline_3d *restrict pline3d,
                              int *restrict error);

EXPORT void
dwg_ent_polyline_3d_set_flag2(dwg_ent_polyline_3d *restrict pline3d,
                              const char flag2,
                              int *restrict error);

EXPORT BITCODE_BL
dwg_obj_polyline_3d_get_numpoints(const dwg_object *restrict obj,
                                  int *restrict error);

EXPORT dwg_point_3d *
dwg_obj_polyline_3d_get_points(const dwg_object *restrict obj,
                               int *restrict error);

/********************************************************************
*                   FUNCTIONS FOR 3DFACE ENTITY                     *
********************************************************************/


EXPORT BITCODE_BS
dwg_ent_3dface_get_invis_flags(const dwg_ent_3dface *restrict _3dface,
                               int *restrict error);

EXPORT void
dwg_ent_3dface_set_invis_flags(dwg_ent_3dface *restrict _3dface,
                               const BITCODE_BS invis_flags,
                               int *restrict error);

EXPORT void
dwg_ent_3dface_get_corner1(const dwg_ent_3dface *restrict _3dface,
                           dwg_point_2d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_3dface_set_corner1(dwg_ent_3dface *restrict _3dface,
                           const dwg_point_2d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_3dface_get_corner2(const dwg_ent_3dface *restrict _3dface,
                           dwg_point_2d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_3dface_set_corner2(dwg_ent_3dface *restrict _3dface,
                           const dwg_point_2d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_3dface_get_corner3(const dwg_ent_3dface *restrict _3dface,
                           dwg_point_2d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_3dface_set_corner3(dwg_ent_3dface *restrict _3dface,
                           const dwg_point_2d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_3dface_get_corner4(const dwg_ent_3dface *restrict _3dface,
                           dwg_point_2d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_3dface_set_corner4(dwg_ent_3dface *restrict _3dface,
                           const dwg_point_2d *restrict point,
                           int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR IMAGE ENTITY                     *
********************************************************************/


EXPORT BITCODE_BL
dwg_ent_image_get_class_version(const dwg_ent_image *restrict image,
                                int *restrict error);

EXPORT void
dwg_ent_image_set_class_version(dwg_ent_image *restrict image,
                                const BITCODE_BL class_version,
                                int *restrict error);

EXPORT void
dwg_ent_image_get_pt0(const dwg_ent_image *restrict image,
                      dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_image_set_pt0(dwg_ent_image *restrict image,
                      const dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_image_get_u_vector(const dwg_ent_image *restrict image,
                           dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_image_set_u_vector(dwg_ent_image *restrict image,
                           const dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_image_get_v_vector(const dwg_ent_image *restrict image,
                           dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT void
dwg_ent_image_set_v_vector(dwg_ent_image *restrict image,
                           const dwg_point_3d *restrict point,
                           int *restrict error);

EXPORT double
dwg_ent_image_get_size_height(const dwg_ent_image *restrict image,
                              int *restrict error);

EXPORT void
dwg_ent_image_set_size_height(dwg_ent_image *restrict image,
                              const double  size_height,
                              int *restrict error);

EXPORT double
dwg_ent_image_get_size_width(const dwg_ent_image *restrict image,
                             int *restrict error);

EXPORT void
dwg_ent_image_set_size_width(dwg_ent_image *restrict image,
                             const double  size_width,
                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_image_get_display_props(const dwg_ent_image *restrict image,
                                int *restrict error);

EXPORT void
dwg_ent_image_set_display_props(dwg_ent_image *restrict image,
                                const BITCODE_BS display_props,
                                int *restrict error);

EXPORT unsigned char
dwg_ent_image_get_clipping(const dwg_ent_image *restrict image,
                           int *restrict error);

EXPORT void
dwg_ent_image_set_clipping(dwg_ent_image *restrict image,
                           const unsigned char clipping,
                           int *restrict error);

EXPORT char
dwg_ent_image_get_brightness(const dwg_ent_image *restrict image,
                             int *restrict error);

EXPORT void
dwg_ent_image_set_brightness(dwg_ent_image *restrict image,
                             const char brightness,
                             int *restrict error);

EXPORT char
dwg_ent_image_get_contrast(const dwg_ent_image *restrict image,
                           int *restrict error);

EXPORT void
dwg_ent_image_set_contrast(dwg_ent_image *restrict image,
                           const char contrast,
                          int *restrict error);

EXPORT char
dwg_ent_image_get_fade(const dwg_ent_image *restrict image,
                       int *restrict error);

EXPORT void
dwg_ent_image_set_fade(dwg_ent_image *restrict image,
                       const char fade,
                          int *restrict error);

EXPORT BITCODE_BS
dwg_ent_image_get_clip_boundary_type(const dwg_ent_image *restrict image,
                                     int *restrict error);

EXPORT void
dwg_ent_image_set_clip_boundary_type(dwg_ent_image *restrict image,
                                     const BITCODE_BS type,
                                    int *restrict error);

EXPORT void
dwg_ent_image_get_boundary_pt0(const dwg_ent_image *restrict image,
                               dwg_point_2d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_image_set_boundary_pt0(dwg_ent_image *restrict image,
                               const dwg_point_2d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_image_get_boundary_pt1(const dwg_ent_image *restrict image,
                               dwg_point_2d *restrict point,
                               int *restrict error);

EXPORT void
dwg_ent_image_set_boundary_pt1(dwg_ent_image *restrict image,
                               const dwg_point_2d *restrict point,
                               int *restrict error);

EXPORT double
dwg_ent_image_get_num_clip_verts(const dwg_ent_image *restrict image,
                                 int *restrict error);

EXPORT BITCODE_2RD *
dwg_ent_image_get_clip_verts(const dwg_ent_image *restrict image,
                             int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR MLINE ENTITY                     *
********************************************************************/


EXPORT void
dwg_ent_mline_set_scale(dwg_ent_mline *restrict mline,
                        const double scale,
                          int *restrict error);

EXPORT double
dwg_ent_mline_get_scale(const dwg_ent_mline *restrict mline,
                          int *restrict error);

EXPORT void
dwg_ent_mline_set_justification(dwg_ent_mline *restrict mline,
                                const BITCODE_RC just,
                          int *restrict error);

EXPORT BITCODE_RC
dwg_ent_mline_get_justification(const dwg_ent_mline *restrict mline,
                          int *restrict error);

EXPORT void
dwg_ent_mline_set_base_point(dwg_ent_mline *restrict mline,
                             const dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_mline_get_base_point(const dwg_ent_mline *restrict mline,
                             dwg_point_3d *restrict point,
                             int *restrict error);

EXPORT void
dwg_ent_mline_set_extrusion(dwg_ent_mline *restrict mline,
                            const dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_mline_get_extrusion(const dwg_ent_mline *restrict mline,
                            dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_mline_set_flags(dwg_ent_mline *restrict mline,
                        const BITCODE_BS oc,
                        int *restrict error);

EXPORT BITCODE_BS
dwg_ent_mline_get_flags(const dwg_ent_mline *restrict mline,
                        int *restrict error);

EXPORT BITCODE_RC
dwg_ent_mline_get_num_lines(const dwg_ent_mline *restrict mline,
                            int *restrict error);

EXPORT BITCODE_BS
dwg_ent_mline_get_num_verts(const dwg_ent_mline *restrict mline,
                            int *restrict error);

EXPORT dwg_mline_vertex *
dwg_ent_mline_get_verts(const dwg_ent_mline *restrict mline,
                        int *restrict error);

EXPORT dwg_mline_line *
dwg_ent_mline_get_lines(const dwg_ent_mline *restrict mline,
                        int *restrict error);

/********************************************************************
*                  FUNCTIONS FOR 3DSOLID ENTITY                     *
********************************************************************/


EXPORT unsigned char
dwg_ent_3dsolid_get_acis_empty(const dwg_ent_3dsolid *restrict _3dsolid,
                          int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_acis_empty(dwg_ent_3dsolid *restrict _3dsolid,
                               const unsigned char acis,
                               int *restrict error);

EXPORT BITCODE_BS
dwg_ent_3dsolid_get_version(const dwg_ent_3dsolid *restrict _3dsolid,
                            int *restrict error);

EXPORT unsigned char*
dwg_ent_3dsolid_get_acis_data(const dwg_ent_3dsolid *restrict _3dsolid,
                              int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_acis_data(dwg_ent_3dsolid *restrict _3dsolid,
                              const unsigned char *restrict sat_data,
                              int *restrict error);

EXPORT char
dwg_ent_3dsolid_get_wireframe_data_present(const dwg_ent_3dsolid *restrict _3dsolid,
                                           int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_wireframe_data_present(dwg_ent_3dsolid *restrict _3dsolid,
                                           const char present,
                          int *restrict error);

EXPORT char
dwg_ent_3dsolid_get_point_present(const dwg_ent_3dsolid *restrict _3dsolid,
                                  int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_point_present(dwg_ent_3dsolid *restrict _3dsolid,
                                  const char present,
                                  int *restrict error);

EXPORT void
dwg_ent_3dsolid_get_point(const dwg_ent_3dsolid *restrict _3dsolid,
                          dwg_point_3d *restrict point, 
                          int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_point(dwg_ent_3dsolid *restrict _3dsolid,
                          const dwg_point_3d *restrict point, 
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_3dsolid_get_num_isolines(const dwg_ent_3dsolid *restrict _3dsolid,
                                 int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_num_isolines(dwg_ent_3dsolid *restrict _3dsolid,
                                 const BITCODE_BL num,
                                 int *restrict error);

EXPORT char
dwg_ent_3dsolid_get_isoline_present(const dwg_ent_3dsolid *restrict _3dsolid,
                                    int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_isoline_present(dwg_ent_3dsolid *restrict _3dsolid,
                                    const char present,
                                    int *restrict error);

EXPORT BITCODE_BL
dwg_ent_3dsolid_get_num_wires(const dwg_ent_3dsolid *restrict _3dsolid,
                              int *restrict error);

EXPORT dwg_3dsolid_wire *
dwg_ent_3dsolid_get_wires(const dwg_ent_3dsolid *restrict _3dsolid,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_3dsolid_get_num_silhouettes(const dwg_ent_3dsolid *restrict _3dsolid,
                                    int *restrict error);

EXPORT dwg_3dsolid_silhouette *
dwg_ent_3dsolid_get_silhouettes(const dwg_ent_3dsolid *restrict _3dsolid,
                                int *restrict error);

EXPORT unsigned char
dwg_ent_3dsolid_get_acis_empty2(const dwg_ent_3dsolid *restrict _3dsolid,
                                int *restrict error);

EXPORT void
dwg_ent_3dsolid_set_acis_empty2(dwg_ent_3dsolid *restrict _3dsolid,
                                const unsigned char emtpy2,
                                int *restrict error); 


/********************************************************************
*                   FUNCTIONS FOR REGION ENTITY                     *
********************************************************************/


EXPORT unsigned char
dwg_ent_region_get_acis_empty(const dwg_ent_region *restrict region,
                              int *restrict error);

EXPORT void
dwg_ent_region_set_acis_empty(dwg_ent_region *restrict region,
                              const unsigned char empty,
                              int *restrict error);

EXPORT BITCODE_BS
dwg_ent_region_get_version(const dwg_ent_region *restrict region,
                          int *restrict error);

EXPORT unsigned char *
dwg_ent_region_get_acis_data(const dwg_ent_region *restrict region,
                             int *restrict error);

EXPORT void
dwg_ent_region_set_acis_data(dwg_ent_region *restrict region,
                             const unsigned char *restrict data,
                             int *restrict error);

EXPORT char
dwg_ent_region_get_wireframe_data_present(const dwg_ent_region *restrict region,
                                          int *restrict error);

EXPORT void
dwg_ent_region_set_wireframe_data_present(dwg_ent_region *restrict region,
                                          const char present,
                                          int *restrict error);

EXPORT char
dwg_ent_region_get_point_present(const dwg_ent_region *restrict region,
                                 int *restrict error);

EXPORT void
dwg_ent_region_set_point_present(dwg_ent_region *restrict region,
                                 const char present,
                                 int *restrict error);

EXPORT void
dwg_ent_region_get_point(const dwg_ent_region *restrict region,
                         dwg_point_3d *restrict point,
                         int *restrict error);

EXPORT void
dwg_ent_region_set_point(dwg_ent_region *restrict region,
                         const dwg_point_3d *restrict point,
                         int *restrict error);

EXPORT BITCODE_BL
dwg_ent_region_get_num_isolines(const dwg_ent_region *restrict region,
                                int *restrict error);

EXPORT void
dwg_ent_region_set_num_isolines(dwg_ent_region *restrict region,
                                const BITCODE_BL num,
                                int *restrict error);

EXPORT char
dwg_ent_region_get_isoline_present(const dwg_ent_region *restrict region,
                                   int *restrict error);

EXPORT void
dwg_ent_region_set_isoline_present(dwg_ent_region *restrict region,
                                   const char present,
                                   int *restrict error);

EXPORT BITCODE_BL
dwg_ent_region_get_num_wires(const dwg_ent_region *restrict region,
                             int *restrict error);

EXPORT dwg_3dsolid_wire *
dwg_ent_region_get_wires(const dwg_ent_region *restrict region,
                         int *restrict error);

EXPORT BITCODE_BL
dwg_ent_region_get_num_silhouettes(const dwg_ent_region *restrict region,
                                   int *restrict error);

EXPORT dwg_3dsolid_silhouette *
dwg_ent_region_get_silhouettes(const dwg_ent_region *restrict region,
                               int *restrict error);

EXPORT unsigned char
dwg_ent_region_get_acis_empty2(const dwg_ent_region *restrict region,
                               int *restrict error);

EXPORT void
dwg_ent_region_set_acis_empty2(dwg_ent_region *restrict region,
                               const unsigned char empty2,
                               int *restrict error); 


/********************************************************************
*                    FUNCTIONS FOR BODY ENTITY                      *
********************************************************************/

EXPORT unsigned char
dwg_ent_body_get_acis_empty(const dwg_ent_body *restrict body,
                          int *restrict error);

EXPORT void
dwg_ent_body_set_acis_empty(dwg_ent_body *restrict body,
                            const unsigned char empty,
                            int *restrict error);

EXPORT BITCODE_BS
dwg_ent_body_get_version(const dwg_ent_body *restrict body,
                          int *restrict error);

EXPORT unsigned char *
dwg_ent_body_get_acis_data(const dwg_ent_body *restrict body,
                           int *restrict error);

EXPORT void
dwg_ent_body_set_acis_data(dwg_ent_body *restrict body,
                           const unsigned char *data,
                           int *restrict error);

EXPORT char
dwg_ent_body_get_wireframe_data_present(const dwg_ent_body *restrict body,
                                        int *restrict error);

EXPORT void
dwg_ent_body_set_wireframe_data_present(dwg_ent_body *restrict body,
                                        const char present,
                                        int *restrict error);

EXPORT char
dwg_ent_body_get_point_present(const dwg_ent_body *restrict body,
                               int *restrict error);

EXPORT void
dwg_ent_body_set_point_present(dwg_ent_body *restrict body,
                               const char present,
                               int *restrict error);

EXPORT void
dwg_ent_body_get_point(const dwg_ent_body *restrict body,
                       dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT void
dwg_ent_body_set_point(dwg_ent_body *restrict body,
                       const dwg_point_3d *restrict point,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_body_get_num_isolines(const dwg_ent_body *restrict body,
                              int *restrict error);

EXPORT void
dwg_ent_body_set_num_isolines(dwg_ent_body *restrict body,
                              const BITCODE_BL num,
                          int *restrict error);

EXPORT char
dwg_ent_body_get_isoline_present(const dwg_ent_body *restrict body,
                                 int *restrict error);

EXPORT void
dwg_ent_body_set_isoline_present(dwg_ent_body *restrict body,
                                 const char present,
                                 int *restrict error);

EXPORT BITCODE_BL
dwg_ent_body_get_num_wires(const dwg_ent_body *restrict body,
                          int *restrict error);

EXPORT dwg_3dsolid_wire *
dwg_ent_body_get_wires(const dwg_ent_body *restrict body,
                       int *restrict error);

EXPORT BITCODE_BL
dwg_ent_body_get_num_silhouettes(const dwg_ent_body *restrict body,
                                 int *restrict error);

EXPORT dwg_3dsolid_silhouette *
dwg_ent_body_get_silhouettes(const dwg_ent_body *restrict body,
                          int *restrict error);

EXPORT unsigned char
dwg_ent_body_get_acis_empty2(const dwg_ent_body *restrict body,
                          int *restrict error);

EXPORT void
dwg_ent_body_set_acis_empty2(dwg_ent_body *restrict body,
                             unsigned char empty2,
                             int *restrict error); 

/********************************************************************
*                    FUNCTIONS FOR TABLE ENTITY                     *
********************************************************************/

EXPORT void
dwg_ent_table_set_insertion_point(dwg_ent_table *restrict table,
                                  const dwg_point_3d *restrict point,
                                  int *restrict error);
EXPORT void
dwg_ent_table_get_insertion_point(const dwg_ent_table *restrict table,
                                  dwg_point_3d *restrict point,
                                  int *restrict error);

EXPORT void
dwg_ent_table_set_scale(dwg_ent_table *restrict table,
                        const dwg_point_3d *restrict point,
                        int *restrict error);
EXPORT void
dwg_ent_table_get_scale(const dwg_ent_table *restrict table,
                        dwg_point_3d *restrict point,
                        int *restrict error);

EXPORT void
dwg_ent_table_set_data_flags(dwg_ent_table *restrict table,
                             const unsigned char flags,
                             int *restrict error);
EXPORT unsigned char
dwg_ent_table_get_data_flags(const dwg_ent_table *restrict table,
                             int *restrict error);

EXPORT void
dwg_ent_table_set_rotation(dwg_ent_table *restrict table,
                           const BITCODE_BD rotation,
                           int *restrict error);
EXPORT double
dwg_ent_table_get_rotation(const dwg_ent_table *restrict table,
                           int *restrict error);

EXPORT void
dwg_ent_table_set_extrusion(dwg_ent_table *restrict table,
                            const dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT void
dwg_ent_table_get_extrusion(const dwg_ent_table *restrict table,
                            dwg_point_3d *restrict vector,
                            int *restrict error);

EXPORT unsigned char
dwg_ent_table_has_attribs(dwg_ent_table *restrict table,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_table_get_num_owned(const dwg_ent_table *restrict table,
                            int *restrict error);

//TODO dwg_ent_*_add_owned, dwg_ent_insert_delete_owned

EXPORT void
dwg_ent_table_set_flag_for_table_value(dwg_ent_table *restrict table,
                                       const BITCODE_BS value,
                                       int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_flag_for_table_value(const dwg_ent_table *restrict table,
                                       int *restrict error);

EXPORT void
dwg_ent_table_set_horiz_direction(dwg_ent_table *restrict table,
                                  const dwg_point_3d *restrict point,
                                  int *restrict error);
EXPORT void
dwg_ent_table_get_horiz_direction(const dwg_ent_table *restrict table,
                                  dwg_point_3d *restrict point,
                                  int *restrict error);

EXPORT BITCODE_BL
dwg_ent_table_get_num_cols(const dwg_ent_table *restrict table,
                           int *restrict error);

EXPORT BITCODE_BL
dwg_ent_table_get_num_rows(const dwg_ent_table *restrict table,
                           int *restrict error);

EXPORT double *
dwg_ent_table_get_col_widths(const dwg_ent_table *restrict table,
                             int *restrict error);

EXPORT double *
dwg_ent_table_get_row_heights(const dwg_ent_table *restrict table,
                              int *restrict error);

EXPORT BITCODE_B
dwg_ent_table_has_table_overrides(dwg_ent_table *restrict table,
                                  int *restrict error);

EXPORT void
dwg_ent_table_set_table_flag_override(dwg_ent_table *restrict table,
                                      const BITCODE_BL override,
                                      int *restrict error);
EXPORT BITCODE_BL
dwg_ent_table_get_table_flag_override(const dwg_ent_table *restrict table,
                                      int *restrict error);

EXPORT void
dwg_ent_table_set_title_suppressed(dwg_ent_table *restrict table,
                                   const unsigned char title,
                                   int *restrict error);
EXPORT unsigned char
dwg_ent_table_get_title_suppressed(const dwg_ent_table *restrict table,
                                   int *restrict error);

EXPORT void
dwg_ent_table_set_header_suppressed(dwg_ent_table *restrict table,
                                    const unsigned char header,
                                    int *restrict error);
EXPORT unsigned char
dwg_ent_table_get_header_suppressed(const dwg_ent_table *restrict table,
                                    int *restrict error);

EXPORT void
dwg_ent_table_set_flow_direction(dwg_ent_table *restrict table,
                                 const BITCODE_BS dir,
                                 int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_flow_direction(const dwg_ent_table *restrict table,
                                 int *restrict error);

EXPORT void
dwg_ent_table_set_horiz_cell_margin(dwg_ent_table *restrict table,
                                    const BITCODE_BD margin,
                                    int *restrict error);
EXPORT double
dwg_ent_table_get_horiz_cell_margin(const dwg_ent_table *restrict table,
                                    int *restrict error);

EXPORT void
dwg_ent_table_set_vert_cell_margin(dwg_ent_table *restrict table,
                                   const BITCODE_BD margin,
                                   int *restrict error);
EXPORT double
dwg_ent_table_get_vert_cell_margin(const dwg_ent_table *restrict table,
                                   int *restrict error);

EXPORT void
dwg_ent_table_set_title_row_fill_none(dwg_ent_table *restrict table,
                                      const unsigned char fill,
                                      int *restrict error);
EXPORT unsigned char
dwg_ent_table_get_title_row_fill_none(const dwg_ent_table *restrict table,
                                      int *restrict error);

EXPORT void
dwg_ent_table_set_header_row_fill_none(dwg_ent_table *restrict table, 
                                       unsigned char fill,
                                       int *restrict error);
EXPORT unsigned char
dwg_ent_table_get_header_row_fill_none(const dwg_ent_table *restrict table,
                                       int *restrict error);

EXPORT void
dwg_ent_table_set_data_row_fill_none(dwg_ent_table *restrict table,
                                     const unsigned char fill,
                                     int *restrict error);
EXPORT unsigned char
dwg_ent_table_get_data_row_fill_none(const dwg_ent_table *restrict table,
                                     int *restrict error);

EXPORT void
dwg_ent_table_set_title_row_align(dwg_ent_table *restrict table,
                                  const unsigned char fill,
                                  int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_title_row_align(const dwg_ent_table *restrict table,
                                  int *restrict error);

EXPORT void
dwg_ent_table_set_header_row_align(dwg_ent_table *restrict table,
                                   const BITCODE_BS align,
                                   int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_row_align(const dwg_ent_table *restrict table,
                                   int *restrict error);

EXPORT void
dwg_ent_table_set_data_row_align(dwg_ent_table *restrict table,
                                 const BITCODE_BS align,
                                 int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_row_align(const dwg_ent_table *restrict table,
                                 int *restrict error);

EXPORT void
dwg_ent_table_set_title_row_height(dwg_ent_table *restrict table,
                                   const BITCODE_BD height,
                                   int *restrict error);
EXPORT double
dwg_ent_table_get_title_row_height(const dwg_ent_table *restrict table,
                                   int *restrict error);

EXPORT void
dwg_ent_table_set_header_row_height(dwg_ent_table *restrict table,
                                    const BITCODE_BD height,
                                    int *restrict error);
EXPORT double
dwg_ent_table_get_header_row_height(const dwg_ent_table *restrict table,
                                    int *restrict error);

EXPORT void
dwg_ent_table_set_data_row_height(dwg_ent_table *restrict table,
                                  const BITCODE_BD height,
                                  int *restrict error);
EXPORT double
dwg_ent_table_get_data_row_height(const dwg_ent_table *restrict table,
                                  int *restrict error);

EXPORT unsigned char
dwg_ent_table_has_border_color_overrides(dwg_ent_table *restrict table,
                                         int *restrict error);

EXPORT void
dwg_ent_table_set_border_color_overrides_flag(dwg_ent_table *restrict table,
                                              const BITCODE_BL overrides,
                                              int *restrict error);
EXPORT BITCODE_BL
dwg_ent_table_get_border_color_overrides_flag(const dwg_ent_table *restrict table,
                                              int *restrict error);

EXPORT unsigned char
dwg_ent_table_has_border_lineweight_overrides(dwg_ent_table *restrict table,
                                              int *restrict error);

EXPORT void
dwg_ent_table_set_border_lineweight_overrides_flag(dwg_ent_table *restrict table,
                                                   const BITCODE_BL overrides,
                                                   int *restrict error);
EXPORT BITCODE_BL
dwg_ent_table_get_border_lineweight_overrides_flag(const dwg_ent_table *restrict table,
                                                   int *restrict error);

EXPORT void
dwg_ent_table_set_title_horiz_top_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_top_linewt(const dwg_ent_table *restrict table,
                                         int *restrict error);

EXPORT void
dwg_ent_table_set_title_horiz_ins_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_ins_linewt(const dwg_ent_table *restrict table,
                                         int *restrict error);

EXPORT void
dwg_ent_table_set_title_horiz_bottom_linewt(dwg_ent_table *restrict table,
                                            const BITCODE_BS linewt,
                                            int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_linewt(const dwg_ent_table *restrict table,
                                            int *restrict error);

EXPORT void
dwg_ent_table_set_title_vert_left_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_left_linewt(const dwg_ent_table *restrict table,
                                         int *restrict error);

EXPORT void
dwg_ent_table_set_title_vert_ins_linewt(dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt,
                                        int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_ins_linewt(const dwg_ent_table *restrict table,
                                        int *restrict error);

EXPORT void
dwg_ent_table_set_title_vert_right_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_right_linewt(const dwg_ent_table *restrict table,
                                          int *restrict error);

EXPORT void
dwg_ent_table_set_header_horiz_top_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_top_linewt(const dwg_ent_table *restrict table,
                                          int *restrict error);

EXPORT void
dwg_ent_table_set_header_horiz_ins_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_ins_linewt(const dwg_ent_table *restrict table,
                                          int *restrict error);

EXPORT void
dwg_ent_table_set_header_horiz_bottom_linewt(dwg_ent_table *restrict table,
                                             const BITCODE_BS linewt,
                                             int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_linewt(const dwg_ent_table *restrict table,
                                             int *restrict error);

EXPORT void
dwg_ent_table_set_header_vert_left_linewt(dwg_ent_table *restrict table,
                                          const BITCODE_BS linewt,
                                          int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_left_linewt(const dwg_ent_table *restrict table,
                                          int *restrict error);

EXPORT void
dwg_ent_table_set_header_vert_ins_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_ins_linewt(const dwg_ent_table *restrict table,
                                         int *restrict error);

EXPORT void
dwg_ent_table_set_header_vert_right_linewt(dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt,
                                           int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_right_linewt(const dwg_ent_table *restrict table,
                                           int *restrict error);

EXPORT void
dwg_ent_table_set_data_horiz_top_linewt(dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt,
                                        int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_top_linewt(const dwg_ent_table *restrict table,
                                        int *restrict error);

EXPORT void
dwg_ent_table_set_data_horiz_ins_linewt(dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt,
                                        int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_ins_linewt(const dwg_ent_table *restrict table,
                                        int *restrict error);

EXPORT void
dwg_ent_table_set_data_horiz_bottom_linewt(dwg_ent_table *restrict table,
                                           const BITCODE_BS linewt,
                                           int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_linewt(const dwg_ent_table *restrict table,
                                           int *restrict error);

EXPORT void
dwg_ent_table_set_data_vert_left_linewt(dwg_ent_table *restrict table,
                                        const BITCODE_BS linewt,
                                        int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_left_linewt(const dwg_ent_table *restrict table,
                                        int *restrict error);

EXPORT void
dwg_ent_table_set_data_vert_ins_linewt(dwg_ent_table *restrict table,
                                       const BITCODE_BS linewt,
                                       int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_ins_linewt(const dwg_ent_table *restrict table,
                                       int *restrict error);

EXPORT void
dwg_ent_table_set_data_vert_right_linewt(dwg_ent_table *restrict table,
                                         const BITCODE_BS linewt,
                                         int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_right_linewt(const dwg_ent_table *restrict table,
                                         int *restrict error);

EXPORT unsigned char
dwg_ent_table_has_border_visibility_overrides(dwg_ent_table *restrict table,
                                              int *restrict error);

EXPORT void
dwg_ent_table_set_border_visibility_overrides_flag(dwg_ent_table *restrict table,
                                                   const BITCODE_BL overrides,
                                                   int *restrict error);

EXPORT BITCODE_BL
dwg_ent_table_get_border_visibility_overrides_flag(const dwg_ent_table *restrict table,
                                                   int *restrict error);

EXPORT void
dwg_ent_table_set_title_horiz_top_visibility(dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_top_visibility(const dwg_ent_table *restrict table,
                                             int *restrict error);

EXPORT void
dwg_ent_table_set_title_horiz_ins_visibility(dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_ins_visibility(const dwg_ent_table *restrict table,
                                             int *restrict error);

EXPORT void
dwg_ent_table_set_title_horiz_bottom_visibility(dwg_ent_table *restrict table,
                                                const BITCODE_BS visibility,
                                                int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_visibility(const dwg_ent_table *restrict table,
                                                int *restrict error);

EXPORT void
dwg_ent_table_set_title_vert_left_visibility(dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_left_visibility(const dwg_ent_table *restrict table,
                                             int *restrict error);

EXPORT void
dwg_ent_table_set_title_vert_ins_visibility(dwg_ent_table *restrict table,
                                            const BITCODE_BS visibility,
                                            int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_ins_visibility(const dwg_ent_table *restrict table,
                                            int *restrict error);

EXPORT void
dwg_ent_table_set_title_vert_right_visibility(dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_title_vert_right_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error);

EXPORT void
dwg_ent_table_set_header_horiz_top_visibility(dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_top_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error);

EXPORT void
dwg_ent_table_set_header_vert_left_visibility(dwg_ent_table *restrict table,
                                               BITCODE_BS visibility,
                                              int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_left_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error);
EXPORT void
dwg_ent_table_set_header_horiz_ins_visibility(dwg_ent_table *restrict table,
                                              const BITCODE_BS visibility,
                                              int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_ins_visibility(const dwg_ent_table *restrict table,
                                              int *restrict error);

EXPORT void
dwg_ent_table_set_header_horiz_bottom_visibility(dwg_ent_table *restrict table,
                                                 const BITCODE_BS visibility,
                                                 int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_visibility(const dwg_ent_table *restrict table,
                                                 int *restrict error);

EXPORT void
dwg_ent_table_set_header_vert_ins_visibility(dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_ins_visibility(const dwg_ent_table *restrict table,
                                             int *restrict error);

EXPORT void
dwg_ent_table_set_header_vert_right_visibility(dwg_ent_table *restrict table,
                                               const BITCODE_BS visibility,
                                               int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_header_vert_right_visibility(const dwg_ent_table *restrict table,
                                               int *restrict error);


EXPORT void
dwg_ent_table_set_data_horiz_top_visibility(dwg_ent_table *restrict table,
                                            const BITCODE_BS visibility,
                                            int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_top_visibility(const dwg_ent_table *restrict table,
                                            int *restrict error);

EXPORT void
dwg_ent_table_set_data_horiz_ins_visibility(dwg_ent_table *restrict table,
                                            const BITCODE_BS visibility,
                                            int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_ins_visibility(const dwg_ent_table *restrict table,
                                            int *restrict error);

EXPORT void
dwg_ent_table_set_data_horiz_bottom_visibility(dwg_ent_table *restrict table,
                                               const BITCODE_BS visibility,
                                               int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_visibility(const dwg_ent_table *restrict table,
                                               int *restrict error);

EXPORT void
dwg_ent_table_set_data_vert_left_visibility(dwg_ent_table *restrict table,
                                            const BITCODE_BS visibility,
                                            int *restrict error);
EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_left_visibility(const dwg_ent_table *restrict table,
                                            int *restrict error);

EXPORT void
dwg_ent_table_set_data_vert_ins_visibility(dwg_ent_table *restrict table,
                                           const BITCODE_BS visibility,
                                           int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_ins_visibility(const dwg_ent_table *restrict table,
                                           int *restrict error);

EXPORT void
dwg_ent_table_set_data_vert_right_visibility(dwg_ent_table *restrict table,
                                             const BITCODE_BS visibility,
                                             int *restrict error);

EXPORT BITCODE_BS
dwg_ent_table_get_data_vert_right_visibility(const dwg_ent_table *restrict table,
                                             int *restrict error);


/********************************************************************
*              FUNCTIONS FOR VERTEX_PFACE_FACE ENTITY               *
********************************************************************/


// Get/Set vertind of a vertex_pface_face entity
EXPORT BITCODE_BS
dwg_ent_vertex_pface_face_get_vertind(const dwg_ent_vert_pface_face *face);

EXPORT void
dwg_ent_vertex_pface_face_set_vertind(dwg_ent_vert_pface_face *restrict face,
                                      BITCODE_BS vertind[4]);


/*******************************************************************
*                    FUNCTIONS FOR TABLES                          *
*        First the special tables: BLOCKS and LAYER                *
********************************************************************/


/********************************************************************
*                FUNCTIONS FOR BLOCK_HEADER OBJECT                  *
********************************************************************/

/* Get Block Name of the block header
Usage :- char *block_name = dwg_obj_block_header_get_name(hdr);
*/
EXPORT char *
dwg_obj_block_header_get_name(const dwg_obj_block_header *restrict hdr,
                              int *restrict error);

EXPORT dwg_obj_block_header *
dwg_get_block_header(dwg_data *restrict dwg,
                          int *restrict error);


/********************************************************************
*               FUNCTIONS FOR BLOCK_CONTROL OBJECT                  *
********************************************************************/

EXPORT BITCODE_BL
dwg_obj_block_control_get_num_entries(const dwg_obj_block_control *restrict ctrl,
                                      int *restrict error);

EXPORT dwg_object_ref **
dwg_obj_block_control_get_block_headers(const dwg_obj_block_control *restrict ctrl,
                                        int *restrict error);

EXPORT dwg_obj_block_control *
dwg_block_header_get_block_control(const dwg_obj_block_header* block_header,
                                   int *restrict error);

EXPORT dwg_object_ref *
dwg_obj_block_control_get_model_space(const dwg_obj_block_control *restrict ctrl,
                                      int *restrict error);

EXPORT dwg_object_ref *
dwg_obj_block_control_get_paper_space(const dwg_obj_block_control *restrict ctrl,
                                      int *restrict error);


/********************************************************************
*                    FUNCTIONS FOR LAYER OBJECT                     *
********************************************************************/

// Get Layer Name
EXPORT char *
dwg_obj_layer_get_name(const dwg_obj_layer *restrict layer,
                          int *restrict error);

/*******************************************************************
*                    FUNCTIONS FOR TABLES                          *
*             All other tables and table entries                   *
********************************************************************/

/// Get name of any table entry. Defaults to ByLayer.
/// \sa dwg_ent_get_layer_name which defaults to "0"
EXPORT char *
dwg_obj_table_get_name(const dwg_object *restrict obj,
                          int *restrict error);

// Get name of the referenced table entry. Defaults to ByLayer
EXPORT char *
dwg_ref_get_table_name(const dwg_object_ref *restrict ref,
                          int *restrict error);

// Get number of table entries from the table.
EXPORT BITCODE_BL
dwg_obj_tablectrl_get_num_entries(const dwg_object *restrict obj,
                          int *restrict error);

// Get the nth table entry from the table.
EXPORT dwg_object_ref *
dwg_obj_tablectrl_get_entry(const dwg_object *restrict obj, const BITCODE_BS index,
                            int *restrict error);

// Get all table entries from the table.
EXPORT dwg_object_ref **
dwg_obj_tablectrl_get_entries(const dwg_object *restrict obj, int * error);

EXPORT dwg_object_ref *
dwg_obj_tablectrl_get_null_handle(const dwg_object *restrict obj,
                                  int *restrict error);
EXPORT dwg_object_ref *
dwg_obj_tablectrl_get_xdicobjhandle(const dwg_object *restrict obj,
                                    int *restrict error);
long unsigned int
dwg_obj_tablectrl_get_objid(const dwg_object *restrict obj,
                            int *restrict error);

/********************************************************************
*                    FUNCTIONS FOR XRECORD OBJECT                     *
********************************************************************/

EXPORT BITCODE_BL
dwg_obj_xrecord_get_num_databytes(const dwg_obj_xrecord *restrict xrecord,
                                  int *restrict error);

EXPORT BITCODE_BS
dwg_obj_xrecord_get_cloning_flags(const dwg_obj_xrecord *restrict xrecord,
                                  int *restrict error);
EXPORT void
dwg_obj_xrecord_set_cloning_flags(dwg_obj_xrecord *restrict xrecord,
                                  const BITCODE_BS cloning_flags,
                                  int *restrict error);
EXPORT BITCODE_BL
dwg_obj_xrecord_get_num_eed(const dwg_obj_xrecord *restrict xrecord,
                            int *restrict error);

//EXPORT Dwg_Eed*
//dwg_obj_xrecord_get_eed(const dwg_obj_xrecord *restrict xrecord,
//                        const BITCODE_BL index,
//                        int *restrict error);

EXPORT Dwg_Resbuf*
dwg_obj_xrecord_get_xdata(const dwg_obj_xrecord *restrict xrecord,
                          int *restrict error);

EXPORT void
dwg_obj_xrecord_set_xdata(dwg_obj_xrecord *restrict xrecord,
                          const Dwg_Resbuf* xdata,
                          int *restrict error);

EXPORT dwg_object_ref*
dwg_obj_xrecord_get_parenthandle(const dwg_obj_xrecord *restrict xrecord,
                                 int *restrict error);

EXPORT dwg_object_ref**
dwg_obj_xrecord_get_reactors(const dwg_obj_xrecord *restrict xrecord,
                             int *restrict error);

EXPORT BITCODE_BL
dwg_obj_xrecord_get_num_objid_handles(const dwg_obj_xrecord *restrict xrecord,
                                      int *restrict error);

EXPORT dwg_object_ref**
dwg_obj_xrecord_get_objid_handles(const dwg_obj_xrecord *restrict xrecord,
                                  int *restrict error);


/********************************************************************
*                    COMMON FUNCTIONS FOR DWG ENTITY                *
********************************************************************/

EXPORT dwg_object *
dwg_ent_to_object(const dwg_obj_ent *restrict obj,
                          int *restrict error);

EXPORT dwg_object *
dwg_ent_generic_to_object(const dwg_ent_generic *restrict obj,
                          int *restrict error);

EXPORT dwg_obj_ent *
dwg_ent_generic_parent(const dwg_ent_generic *restrict ent,
                          int *restrict error);

EXPORT BITCODE_RL
dwg_ent_get_bitsize(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT unsigned int
dwg_ent_get_num_eed(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT dwg_entity_eed *
dwg_ent_get_eed(const dwg_obj_ent *restrict ent, unsigned int index,
                          int *restrict error);

EXPORT dwg_entity_eed_data *
dwg_ent_get_eed_data(const dwg_obj_ent *restrict ent, unsigned int index,
                          int *restrict error);

EXPORT BITCODE_B
dwg_ent_get_picture_exists(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_BLL
dwg_ent_get_picture_size(const dwg_obj_ent *restrict ent,
                          int *restrict error); // before r2007 only RL

EXPORT BITCODE_RC *
dwg_ent_get_picture(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_BB
dwg_ent_get_entity_mode(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ent_get_num_reactors(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_B
dwg_ent_get_xdic_missing_flag(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2004+

EXPORT char *
dwg_ent_get_layer_name(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_B
dwg_ent_get_isbylayerlt(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r13-r14 only

EXPORT BITCODE_B
dwg_ent_get_nolinks(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_CMC
dwg_ent_get_color(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT double
dwg_ent_get_linetype_scale(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_BB
dwg_ent_get_linetype_flag(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2000+

EXPORT BITCODE_BB
dwg_ent_get_plotstyle_flag(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2000+

EXPORT BITCODE_BB
dwg_ent_get_material_flag(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2007+

EXPORT BITCODE_RC
dwg_ent_get_shadow_flag(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2007+

EXPORT BITCODE_B
dwg_ent_has_full_visualstyle(dwg_obj_ent *restrict ent,
                          int *restrict error); //r2010+

EXPORT BITCODE_B
dwg_ent_has_face_visualstyle(dwg_obj_ent *restrict ent,
                          int *restrict error); //r2010+

EXPORT BITCODE_B
dwg_ent_has_edge_visualstyle(dwg_obj_ent *restrict ent,
                          int *restrict error); //r2010+

EXPORT BITCODE_BS
dwg_ent_get_invisible(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_RC
dwg_ent_get_lineweight(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2000+

/*EXPORT unsigned int
dwg_ent_get_num_handles(const dwg_obj_ent *restrict ent,
                          int *restrict error);*/

//TODO: dwg_object_ref* or dwg_handle*, not handle
EXPORT BITCODE_H
dwg_ent_get_subentity(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_H*
dwg_ent_get_reactors(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_H
dwg_ent_get_xdicobjhandle(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_H
dwg_ent_get_prev_entity(const dwg_obj_ent *restrict ent,
                          int *restrict error);  //r13-r2000

EXPORT BITCODE_H
dwg_ent_get_next_entity(const dwg_obj_ent *restrict ent,
                          int *restrict error);  //r13-r2000

EXPORT BITCODE_H
dwg_ent_get_color_handle(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2004+

EXPORT BITCODE_H
dwg_ent_get_layer(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_H
dwg_ent_get_ltype(const dwg_obj_ent *restrict ent,
                          int *restrict error);

EXPORT BITCODE_H
dwg_ent_get_material(const dwg_obj_ent *restrict ent,
                          int *restrict error);     //r2007+

EXPORT BITCODE_H
dwg_ent_get_plotstyle(const dwg_obj_ent *restrict ent,
                          int *restrict error);    //r2000+

EXPORT BITCODE_H
dwg_ent_get_full_visualstyle(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2010+

EXPORT BITCODE_H
dwg_ent_get_face_visualstyle(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2010+

EXPORT BITCODE_H
dwg_ent_get_edge_visualstyle(const dwg_obj_ent *restrict ent,
                          int *restrict error); //r2010+

/********************************************************************
*                    FUNCTIONS FOR DWG OBJECT                       *
********************************************************************/

EXPORT dwg_object *
dwg_obj_obj_to_object(const dwg_obj_obj *restrict obj,
                          int *restrict error);

EXPORT dwg_object *
dwg_obj_generic_to_object(const dwg_obj_generic *restrict obj,
                          int *restrict error);

EXPORT dwg_obj_obj *
dwg_obj_generic_parent(const dwg_obj_generic *restrict obj,
                          int *restrict error);

EXPORT dwg_object *
dwg_get_object(dwg_data *dwg, long unsigned int index);

EXPORT BITCODE_RL
dwg_obj_get_bitsize(const dwg_object *obj);

EXPORT unsigned int
dwg_obj_get_num_eed(const dwg_obj_obj *restrict obj,
                          int *restrict error);

EXPORT dwg_entity_eed *
dwg_obj_get_eed(const dwg_obj_obj *restrict obj, const int index,
                          int *restrict error);

EXPORT dwg_entity_eed_data *
dwg_obj_get_eed_data(const dwg_obj_obj *restrict obj, const int index,
                          int *restrict error);

EXPORT BITCODE_B
dwg_obj_get_picture_exists(const dwg_object *restrict obj,
                          int *restrict error);

EXPORT BITCODE_BLL
dwg_obj_get_picture_size(const dwg_object *restrict obj,
                          int *restrict error); // before r2007 only RL

EXPORT BITCODE_RC *
dwg_obj_get_picture(const dwg_object *restrict obj,
                          int *restrict error);

EXPORT BITCODE_BB
dwg_obj_get_entity_mode(const dwg_object *restrict obj,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_obj_get_num_reactors(const dwg_object *restrict obj,
                          int *restrict error);

EXPORT BITCODE_B
dwg_obj_get_xdic_missing_flag(const dwg_object *restrict obj,
                          int *restrict error); //r2004+

EXPORT unsigned int
dwg_obj_object_get_index(const dwg_object *restrict obj,
                          int *restrict error);

EXPORT dwg_handle *
dwg_obj_get_handle(dwg_object *restrict obj,
                          int *restrict error);

EXPORT BITCODE_BL
dwg_ref_get_absref(const dwg_object_ref *restrict ref,
                          int *restrict error);

EXPORT dwg_object *
dwg_obj_ref_get_object(const dwg_object_ref *restrict ref,
                          int *restrict error);

dwg_object *
dwg_absref_get_object(const dwg_data* dwg, const long unsigned int absref);

EXPORT dwg_obj_obj *
dwg_object_to_object(dwg_object *restrict obj,
                     int *restrict error);

EXPORT dwg_obj_ent *
dwg_object_to_entity(dwg_object *restrict obj,
                     int *restrict error);

EXPORT int
dwg_get_type(const dwg_object *obj);

EXPORT char*
dwg_get_dxfname(const dwg_object *obj);

EXPORT unsigned int
dwg_get_num_classes(const dwg_data *dwg);

EXPORT dwg_class *
dwg_get_class(const dwg_data *dwg, unsigned int index);

#ifdef __cplusplus
}
#endif

#endif

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

typedef struct dwg_lwpline_widths
{
  double start;
  double end;
} dwg_lwpline_widths;

// Extract All Entities of a specific type from a BLOCK
#define GET_DWG_ENTITY_DECL(token) \
Dwg_Entity_##token **dwg_get_##token (Dwg_Object_Ref * ref);

#define GET_DWG_ENTITY(token) \
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
Dwg_Entity_##token *dwg_object_to_##token(Dwg_Object *obj);

#define CAST_DWG_OBJECT_TO_ENTITY(token) \
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

#define CAST_DWG_OBJECT_TO_OBJECT_DECL(token) \
Dwg_Object_##token *dwg_object_to_##token(Dwg_Object *obj);

#define CAST_DWG_OBJECT_TO_OBJECT(token) \
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
Dwg_Object_##token *dwg_object_to_##token(Dwg_Object *obj) \
{ \
    Dwg_Object_##token *ret_obj = NULL; \
    if (obj && obj->dxfname && !strcmp(obj->dxfname, dxfname)) \
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
typedef struct _dwg_entity_LWPLINE                dwg_ent_lwpline;
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
typedef struct _dwg_entity_MLINE_line             dwg_ent_mline_line;
typedef struct _dwg_entity_MLINE_vertex           dwg_ent_mline_vertex;
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
typedef struct _dwg_entity_SPLINE_point           dwg_ent_spline_point;
typedef struct _dwg_entity_SPLINE                 dwg_ent_spline;
typedef struct _dwg_entity_SPLINE_control_point   dwg_ent_spline_control_point;
typedef struct _dwg_entity_OLEFRAME               dwg_ent_oleframe;
typedef struct _dwg_entity_OLE2FRAME              dwg_ent_ole2frame;
typedef struct _dwg_entity_VIEWPORT               dwg_ent_viewport;
typedef struct _dwg_entity_3DSOLID                dwg_ent_3dsolid,
                                                  dwg_ent_region,
                                                  dwg_ent_body;
typedef struct _dwg_entity_3DSOLID_wire           dwg_ent_solid_wire;
typedef struct _dwg_entity_3DSOLID_silhouette     dwg_ent_solid_silhouette;
typedef struct _dwg_entity_TABLE                  dwg_ent_table;
typedef struct _dwg_entity_HATCH                  dwg_ent_hatch;
typedef struct _dwg_entity_VERTEX_PFACE_FACE      dwg_ent_vert_pface_face;

///////////////////////////////////////////////////////////////////////////

typedef struct _dwg_object_LAYER                  dwg_obj_layer;
typedef struct _dwg_object_BLOCK_HEADER           dwg_obj_block_header;
typedef struct _dwg_object_BLOCK_CONTROL          dwg_obj_block_control;
typedef struct _dwg_object_MLINESTYLE             dwg_obj_mlinestyle;
typedef struct _dwg_object_APPID                  dwg_obj_appid;
typedef struct _dwg_object_APPID_CONTROL          dwg_obj_appid_control;
typedef struct _dwg_object_PROXY                  dwg_obj_proxy;
typedef struct _dwg_object_XRECORD                dwg_obj_xrecord;
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

/// Macro call to extract all text entities from a block
GET_DWG_ENTITY_DECL(TEXT)
/// Macro call to extract all attrib entities from a block
GET_DWG_ENTITY_DECL(ATTRIB)
/// Macro call to extract all attdef entities from a block
GET_DWG_ENTITY_DECL(ATTDEF)
/// Macro call to extract all block entities from a block
GET_DWG_ENTITY_DECL(BLOCK)
/// Macro call to extract endblk entity from a block
GET_DWG_ENTITY_DECL(ENDBLK)
/// Macro call to extract all seqend entities from a block
GET_DWG_ENTITY_DECL(SEQEND)
/// Macro call to extract all insert entities from a block
GET_DWG_ENTITY_DECL(INSERT)
/// Macro call to extract all minsert entities from a block
GET_DWG_ENTITY_DECL(MINSERT)
/// Macro call to extract all vertex_2d entities from a block
GET_DWG_ENTITY_DECL(VERTEX_2D)
/// Macro call to extract all vertex_3d entities from a block
GET_DWG_ENTITY_DECL(VERTEX_3D)
/// Macro call to extract all vertex_mesh entities from a block
GET_DWG_ENTITY_DECL(VERTEX_MESH)
/// Macro call to extract all vertex_pface entities from a block
GET_DWG_ENTITY_DECL(VERTEX_PFACE)
/// Macro call to extract all vertex_pface_face entities from a block
GET_DWG_ENTITY_DECL(VERTEX_PFACE_FACE)
/// Macro call to extract all polyline_2d entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_2D)
/// Macro call to extract all polyline_3d entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_3D)
/// Macro call to extract all arc entities from a block
GET_DWG_ENTITY_DECL(ARC)
/// Macro call to extract all circle entities from a block
GET_DWG_ENTITY_DECL(CIRCLE)
/// Macro call to extract all line entities from a block
GET_DWG_ENTITY_DECL(LINE)
/// Macro call to extract all dimension ordinate entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ORDINATE)
/// Macro call to extract all dimension linear entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_LINEAR)
/// Macro call to extract all dimension aligned entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ALIGNED)
/// Macro call to extract all dimension ang3pt entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ANG3PT)
/// Macro call to extract all dimension ang2ln entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_ANG2LN)
/// Macro call to extract all dimension radius entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_RADIUS)
/// Macro call to extract all dimension diameter entities from a block
GET_DWG_ENTITY_DECL(DIMENSION_DIAMETER)
/// Macro call to extract all points entities from a block
GET_DWG_ENTITY_DECL(POINT)
/// Macro call to extract all polyline_pface entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_PFACE)
/// Macro call to extract all polyline_mesh entities from a block
GET_DWG_ENTITY_DECL(POLYLINE_MESH)
/// Macro call to extract all solid entities from a block
GET_DWG_ENTITY_DECL(SOLID)
/// Macro call to extract all trace entities from a block
GET_DWG_ENTITY_DECL(TRACE)
/// Macro call to extract all shape entities from a block
GET_DWG_ENTITY_DECL(SHAPE)
/// Macro call to extract all viewport entities from a block
GET_DWG_ENTITY_DECL(VIEWPORT)
/// Macro call to extract all ellipse entities from a block
GET_DWG_ENTITY_DECL(ELLIPSE)
/// Macro call to extract all spline entities from a block
GET_DWG_ENTITY_DECL(SPLINE)
/// Macro call to extract all region entities from a block
GET_DWG_ENTITY_DECL(REGION)
/// Macro call to extract all body entities from a block
GET_DWG_ENTITY_DECL(BODY)
/// Macro call to extract all ray entities from a block
GET_DWG_ENTITY_DECL(RAY)
/// Macro call to extract all xline entities from a block
GET_DWG_ENTITY_DECL(XLINE)
/// Macro call to extract all OLEFRAME entities from a block
GET_DWG_ENTITY_DECL(OLEFRAME)
/// Macro call to extract all mtext entities from a block
GET_DWG_ENTITY_DECL(MTEXT)
/// Macro call to extract all leader entities from a block
GET_DWG_ENTITY_DECL(LEADER)
/// Macro call to extract all tolerance entities from a block
GET_DWG_ENTITY_DECL(TOLERANCE)
/// Macro call to extract all mline entities from a block
GET_DWG_ENTITY_DECL(MLINE)
/// Macro call to extract all OLE2FRAME entities from a block
GET_DWG_ENTITY_DECL(OLE2FRAME)
/// Macro call to extract all DUMMY entities from a block
GET_DWG_ENTITY_DECL(DUMMY)
/// Macro call to extract all LONG_TRANSACTION entities from a block
GET_DWG_ENTITY_DECL(LONG_TRANSACTION)
/// Macro call to extract all lwpline entities from a block
GET_DWG_ENTITY_DECL(LWPLINE)
/// Macro call to extract all PROXY_ENTITY entities from a block
GET_DWG_ENTITY_DECL(PROXY_ENTITY)
/// Macro call to extract all hatch entities from a block
GET_DWG_ENTITY_DECL(HATCH)
/// Macro call to extract all image entities from a block
GET_DWG_ENTITY_DECL(IMAGE)

/*******************************************************************
*     Functions created from macro to cast dwg_object to entity     *
*                 Usage :- dwg_object_to_ENTITY(),                  *
*                where ENTITY can be LINE or CIRCLE                 *
********************************************************************/

/// Macro call to cast dwg object to text
CAST_DWG_OBJECT_TO_ENTITY_DECL(TEXT)
/// Macro call to cast dwg object to attrib
CAST_DWG_OBJECT_TO_ENTITY_DECL(ATTRIB)
/// Macro call to cast dwg object to attdef
CAST_DWG_OBJECT_TO_ENTITY_DECL(ATTDEF)
/// Macro call to cast dwg object to block
CAST_DWG_OBJECT_TO_ENTITY_DECL(BLOCK)
/// Macro call to extract endblk entity from a block
CAST_DWG_OBJECT_TO_ENTITY_DECL(ENDBLK)
/// Macro call to cast dwg object to seqend
CAST_DWG_OBJECT_TO_ENTITY_DECL(SEQEND)
/// Macro call to cast dwg object to insert
CAST_DWG_OBJECT_TO_ENTITY_DECL(INSERT)
/// Macro call to cast dwg object to minsert
CAST_DWG_OBJECT_TO_ENTITY_DECL(MINSERT)
/// Macro call to cast dwg object to vertex_2d
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_2D)
/// Macro call to cast dwg object to vertex_3d
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_3D)
/// Macro call to cast dwg object to vertex_mesh
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_MESH)
/// Macro call to cast dwg object to vertex_pface
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_PFACE)
/// Macro call to cast dwg object to vertex_pface_face
CAST_DWG_OBJECT_TO_ENTITY_DECL(VERTEX_PFACE_FACE)
/// Macro call to cast dwg object to polyline_2d
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_2D)
/// Macro call to cast dwg object to polyline_3d
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_3D)
/// Macro call to cast dwg object to arc
CAST_DWG_OBJECT_TO_ENTITY_DECL(ARC)
/// Macro call to cast dwg object to circle
CAST_DWG_OBJECT_TO_ENTITY_DECL(CIRCLE)
/// Macro call to cast dwg object to line
CAST_DWG_OBJECT_TO_ENTITY_DECL(LINE)

/// Macro call to cast dwg object to common dimension
dwg_ent_dim * dwg_object_to_DIMENSION(dwg_object *obj);
/// Macro call to cast dwg object to dimension ordinate
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ORDINATE)
/// Macro call to cast dwg object to dimension linear
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_LINEAR)
/// Macro call to cast dwg object to dimension aligned
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ALIGNED)
/// Macro call to cast dwg object to dimension ang3pt
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ANG3PT)
/// Macro call to cast dwg object to dimension ang2ln
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_ANG2LN)
/// Macro call to cast dwg object to dimension radius
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_RADIUS)
/// Macro call to cast dwg object to dimension diameter
CAST_DWG_OBJECT_TO_ENTITY_DECL(DIMENSION_DIAMETER)
/// Macro call to cast dwg object to points
CAST_DWG_OBJECT_TO_ENTITY_DECL(POINT)
/// Macro call to cast dwg object to polyline_pface
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_PFACE)
/// Macro call to cast dwg object to polyline_mesh
CAST_DWG_OBJECT_TO_ENTITY_DECL(POLYLINE_MESH)
/// Macro call to cast dwg object to solid
CAST_DWG_OBJECT_TO_ENTITY_DECL(SOLID)
/// Macro call to cast dwg object to trace
CAST_DWG_OBJECT_TO_ENTITY_DECL(TRACE)
/// Macro call to cast dwg object to shape
CAST_DWG_OBJECT_TO_ENTITY_DECL(SHAPE)
/// Macro call to cast dwg object to viewport
CAST_DWG_OBJECT_TO_ENTITY_DECL(VIEWPORT)
/// Macro call to cast dwg object to ellipse
CAST_DWG_OBJECT_TO_ENTITY_DECL(ELLIPSE)
/// Macro call to cast dwg object to spline
CAST_DWG_OBJECT_TO_ENTITY_DECL(SPLINE)
/// Macro call to cast dwg object to region
CAST_DWG_OBJECT_TO_ENTITY_DECL(REGION)
/// Macro call to cast dwg object to body
CAST_DWG_OBJECT_TO_ENTITY_DECL(BODY)
/// Macro call to cast dwg object to ray
CAST_DWG_OBJECT_TO_ENTITY_DECL(RAY)
/// Macro call to cast dwg object to xline
CAST_DWG_OBJECT_TO_ENTITY_DECL(XLINE)
/// Macro call to cast dwg object to oleframe
CAST_DWG_OBJECT_TO_ENTITY_DECL(OLEFRAME)
/// Macro call to cast dwg object to mtext
CAST_DWG_OBJECT_TO_ENTITY_DECL(MTEXT)
/// Macro call to cast dwg object to leader
CAST_DWG_OBJECT_TO_ENTITY_DECL(LEADER)
/// Macro call to cast dwg object to tolerance
CAST_DWG_OBJECT_TO_ENTITY_DECL(TOLERANCE)
/// Macro call to cast dwg object to mline
CAST_DWG_OBJECT_TO_ENTITY_DECL(MLINE)
/// Macro call to cast dwg object to ole2frame
CAST_DWG_OBJECT_TO_ENTITY_DECL(OLE2FRAME)
/// Macro call to cast dwg object to dummy
CAST_DWG_OBJECT_TO_ENTITY_DECL(DUMMY)
/// Macro call to cast dwg object to long_transaction
CAST_DWG_OBJECT_TO_ENTITY_DECL(LONG_TRANSACTION)
/// Macro call to cast dwg object to lwpline
CAST_DWG_OBJECT_TO_ENTITY_DECL(LWPLINE)
/// Macro call to cast dwg object to proxy_entity
CAST_DWG_OBJECT_TO_ENTITY_DECL(PROXY_ENTITY)
/// Macro call to cast dwg object to hatch
CAST_DWG_OBJECT_TO_ENTITY_DECL(HATCH)
/// Macro call to cast dwg object to image
CAST_DWG_OBJECT_TO_ENTITY_DECL(IMAGE)

/*******************************************************************
*     Functions created from macro to cast dwg object to object     *
*                 Usage :- dwg_object_to_OBJECT(),                  *
*            where OBJECT can be LAYER or BLOCK_HEADER              *
********************************************************************/
/// Macro call to cast dwg object to block header
CAST_DWG_OBJECT_TO_OBJECT_DECL(BLOCK_HEADER)
/// Macro call to cast dwg object to block control
CAST_DWG_OBJECT_TO_OBJECT_DECL(BLOCK_CONTROL)
/// Macro call to cast dwg object to layer
CAST_DWG_OBJECT_TO_OBJECT_DECL(LAYER)
/// Macro call to cast dwg object to xrecord
CAST_DWG_OBJECT_TO_OBJECT_DECL(XRECORD)

// initialize the DWG version, for r2007+ unicode strings and older field variants
void dwg_api_init_version(Dwg_Data *dwg);

/********************************************************************
*                FUNCTIONS START HERE ENTITY SPECIFIC               *
********************************************************************/


/********************************************************************
*                    FUNCTIONS FOR CIRCLE ENTITY                    *
********************************************************************/


// Creates a new circle entity
dwg_ent_circle*
dwg_ent_circle_new(int *error);

// Deletes a circle entity
void
dwg_ent_circle_delete(dwg_ent_circle *circle, int *error);

// Initializes a circle with its default values
dwg_ent_circle *
dwg_ent_circle_init(dwg_ent_circle *circle);

// Get/Set the center point of a circle
void
dwg_ent_circle_get_center(dwg_ent_circle *circle, dwg_point_3d *point,
                          int *error);

void
dwg_ent_circle_set_center(dwg_ent_circle *circle, dwg_point_3d *point,
                          int *error);

// Get/Set the radius of a circle
BITCODE_BD
dwg_ent_circle_get_radius(dwg_ent_circle *circle, int *error);

void
dwg_ent_circle_set_radius(dwg_ent_circle *circle, BITCODE_BD radius,
                          int *error);

// Get/Set the thickness of a circle
BITCODE_BD
dwg_ent_circle_get_thickness(dwg_ent_circle *circle, int *error);

void
dwg_ent_circle_set_thickness(dwg_ent_circle *circle, BITCODE_BD thickness,
                             int *error);

// Get/Set the extrusion of a circle
void
dwg_ent_circle_set_extrusion(dwg_ent_circle *circle,
                             dwg_point_3d *vector, int *error);

void
dwg_ent_circle_get_extrusion(dwg_ent_circle *circle,
                             dwg_point_3d *vector, int *error);


/********************************************************************
*                    FUNCTIONS FOR LINE ENTITY                      *
********************************************************************/


// Creates a new line entity
dwg_ent_line *
dwg_ent_line_new(int *error);

// Deletes a line entity
void
dwg_ent_line_delete(dwg_ent_line *line, int *error);

// Initializes a line with its default values
dwg_ent_line *
dwg_ent_line_init(dwg_ent_line *line);

// Get/Set the start point of a line
void
dwg_ent_line_get_start_point(dwg_ent_line *line, dwg_point_3d *point,
                             int *error);

void
dwg_ent_line_set_start_point(dwg_ent_line *line, dwg_point_3d *point,
                             int *error);

// Get/Set the end point of a line
void
dwg_ent_line_get_end_point(dwg_ent_line *line, dwg_point_3d *point,
                           int *error);

void
dwg_ent_line_set_end_point(dwg_ent_line *line, dwg_point_3d *point,
                           int *error);

// Get/Set the thickness of a line
BITCODE_BD
dwg_ent_line_get_thickness(dwg_ent_line *line, int *error);

void
dwg_ent_line_set_thickness(dwg_ent_line *line, BITCODE_BD thickness, int *error);

// Get/Set the extrusion of a line
void
dwg_ent_line_set_extrusion(dwg_ent_line *line, dwg_point_3d *vector,
                           int *error);

void
dwg_ent_line_get_extrusion(dwg_ent_line *line, dwg_point_3d *vector,
                           int *error);


/********************************************************************
*                    FUNCTIONS FOR ARC ENTITY                       *
********************************************************************/


// Creates a new arc entity
dwg_ent_arc* dwg_ent_arc_new(int *error);

// Deletes a arc entity
void
dwg_ent_arc_delete(dwg_ent_arc *arc, int *error);

// Initializes a arc with its default values
dwg_ent_arc* dwg_ent_arc_init(dwg_ent_arc *arc);

// Get/Set the center point of a arc
void
dwg_ent_arc_get_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error);

void
dwg_ent_arc_set_center(dwg_ent_arc *arc, dwg_point_3d *point, int *error);

// Get/Set the radius of a arc
BITCODE_BD
dwg_ent_arc_get_radius(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_radius(dwg_ent_arc *arc, BITCODE_BD radius, int *error);

// Get/Set the thickness of arc
BITCODE_BD
dwg_ent_arc_get_thickness(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_thickness(dwg_ent_arc *arc, BITCODE_BD thickness, int *error);

//Get/Set the extrusion of arc
void
dwg_ent_arc_get_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error);

void
dwg_ent_arc_set_extrusion(dwg_ent_arc *arc, dwg_point_3d *vector, int *error);

// Get/set start angle arc
BITCODE_BD
dwg_ent_arc_get_start_angle(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_start_angle(dwg_ent_arc *arc, BITCODE_BD start_angle, int *error);

// Get/set end angle of arc
BITCODE_BD
dwg_ent_arc_get_end_angle(dwg_ent_arc *arc, int *error);

void
dwg_ent_arc_set_end_angle(dwg_ent_arc *arc, BITCODE_BD end_angle, int *error);


/********************************************************************
*                   FUNCTIONS FOR ELLIPSE ENTITY                    *
********************************************************************/


// Creates a new ellipse entity
dwg_ent_ellipse *
dwg_ent_ellipse_new(int *error);

// Deletes a ellipse entity
void
dwg_ent_ellipse_delete(dwg_ent_ellipse *ellipse, int *error);

// Initializes a ellipse with its default values
dwg_ent_ellipse *
dwg_ent_ellipse_init(dwg_ent_ellipse *ellipse);

// Get/Set center of ellipse
void
dwg_ent_ellipse_get_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error);

void
dwg_ent_ellipse_set_center(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                           int *error);

// Get/Set sm axis of ellipse
void
dwg_ent_ellipse_get_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error);

void
dwg_ent_ellipse_set_sm_axis(dwg_ent_ellipse *ellipse, dwg_point_3d *point,
                            int *error);

// Get/Set extrusion of ellipse
void
dwg_ent_ellipse_get_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector,
                              int *error);

void
dwg_ent_ellipse_set_extrusion(dwg_ent_ellipse *ellipse, dwg_point_3d *vector,
                              int *error);

// Get/Set axis ratio of ellipse
BITCODE_BD
dwg_ent_ellipse_get_axis_ratio(dwg_ent_ellipse *ellipse, int *error);

void
dwg_ent_ellipse_set_axis_ratio(dwg_ent_ellipse *ellipse, BITCODE_BD ratio,
                               int *error);

// Get/Set start angle of ellipse
BITCODE_BD
dwg_ent_ellipse_get_start_angle(dwg_ent_ellipse *ellipse, int *error);

void
dwg_ent_ellipse_set_start_angle(dwg_ent_ellipse *ellipse, BITCODE_BD start_angle,
                                int *error);

// Get/Set end angle of ellipse
BITCODE_BD
dwg_ent_ellipse_get_end_angle(dwg_ent_ellipse *ellipse, int *error);

void
dwg_ent_ellipse_set_end_angle(dwg_ent_ellipse *ellipse, BITCODE_BD end_angle,
                              int *error);


/********************************************************************
*                    FUNCTIONS FOR TEXT ENTITY                      *
********************************************************************/


// Get/Set text of text (utf-8 encoded)
void
dwg_ent_text_set_text(dwg_ent_text *text, char *text_value, int *error);

char *
dwg_ent_text_get_text(dwg_ent_text *text, int *error);

// Get/Set insertion points of text
void
dwg_ent_text_get_insertion_point(dwg_ent_text *text, dwg_point_2d *point,
                                 int *error);

void
dwg_ent_text_set_insertion_point(dwg_ent_text *text, dwg_point_2d *point,
                                 int *error);

//Get/Set height of text
BITCODE_BD
dwg_ent_text_get_height(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_height(dwg_ent_text *text, BITCODE_BD height, int *error);

//Get/Set extrusion of text
void
dwg_ent_text_get_extrusion(dwg_ent_text *text, dwg_point_3d *vector,
                           int *error);

void
dwg_ent_text_set_extrusion(dwg_ent_text *text, dwg_point_3d *vector,
                           int *error);

//Get/Set thickness of text
BITCODE_BD
dwg_ent_text_get_thickness(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_thickness(dwg_ent_text *text, BITCODE_BD thickness, int *error);

//Get/Set rotation angle of text
BITCODE_BD
dwg_ent_text_get_rotation(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_rotation(dwg_ent_text *text, BITCODE_BD angle, int *error);

//Get/Set horizontal alignment angle of text
BITCODE_BS
dwg_ent_text_get_vert_align(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_vert_align(dwg_ent_text *text, BITCODE_BS alignment, int *error);

//Get/Set vertical alignment of text
BITCODE_BS
dwg_ent_text_get_horiz_align(dwg_ent_text *text, int *error);

void
dwg_ent_text_set_horiz_align(dwg_ent_text *text, BITCODE_BS alignment, int *error);


/********************************************************************
*                   FUNCTIONS FOR ATTRIB ENTITY                     *
********************************************************************/


// Get/Set text of attrib (utf-8 encoded)
void
dwg_ent_attrib_set_text(dwg_ent_attrib *attrib, char *text_value, int *error);

char *
dwg_ent_attrib_get_text(dwg_ent_attrib *attrib, int *error);

// Get/Set insertion points of attrib
void
dwg_ent_attrib_get_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point,
                                   int *error);

void
dwg_ent_attrib_set_insertion_point(dwg_ent_attrib *attrib, dwg_point_2d *point,
                                   int *error);

//Get/Set height of attrib
BITCODE_BD
dwg_ent_attrib_get_height(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_height(dwg_ent_attrib *attrib, BITCODE_BD height, int *error);

//Get/Set extrusion of attrib
void
dwg_ent_attrib_get_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error);

void
dwg_ent_attrib_set_extrusion(dwg_ent_attrib *attrib, dwg_point_3d *vector,
                             int *error);

//Get/Set thickness of attrib
BITCODE_BD
dwg_ent_attrib_get_thickness(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_thickness(dwg_ent_attrib *attrib, BITCODE_BD thickness,
                             int *error);

//Get/Set rotation angle of attrib
BITCODE_BD
dwg_ent_attrib_get_rot_angle(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_rot_angle(dwg_ent_attrib *attrib, BITCODE_BD angle,
                             int *error);

//Get/Set horizontal alignment angle of attrib
BITCODE_BS
dwg_ent_attrib_get_vert_align(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_vert_align(dwg_ent_attrib *attrib, BITCODE_BS alignment,
                              int *error);

//Get/Set vertical alignment of attrib
BITCODE_BS
dwg_ent_attrib_get_horiz_align(dwg_ent_attrib *attrib, int *error);

void
dwg_ent_attrib_set_horiz_align(dwg_ent_attrib *attrib, BITCODE_BS alignment,
                               int *error);


/********************************************************************
*                   FUNCTIONS FOR ATTDEF ENTITY                     *
********************************************************************/


// Get/Set default text of attdef (utf-8 encoded)
void
dwg_ent_attdef_set_text(dwg_ent_attdef *attdef, char *default_value,
                        int *error);

char *
dwg_ent_attdef_get_text(dwg_ent_attdef *attdef, int *error);

// Get/Set insertion points of attdef
void
dwg_ent_attdef_get_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point,
                                   int *error);

void
dwg_ent_attdef_set_insertion_point(dwg_ent_attdef *attdef, dwg_point_2d *point,
                                   int *error);

//Get/Set height of attdef
BITCODE_BD
dwg_ent_attdef_get_height(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_height(dwg_ent_attdef *attdef, BITCODE_BD height, int *error);

//Get/Set extrusion of attdef
void
dwg_ent_attdef_get_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error);

void
dwg_ent_attdef_set_extrusion(dwg_ent_attdef *attdef, dwg_point_3d *vector,
                             int *error);

//Get/Set thickness of attdef
BITCODE_BD
dwg_ent_attdef_get_thickness(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_thickness(dwg_ent_attdef *attdef, BITCODE_BD thickness,
                             int *error);

//Get/Set rotation angle of attdef
BITCODE_BD
dwg_ent_attdef_get_rot_angle(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_rot_angle(dwg_ent_attdef *attdef, BITCODE_BD angle,
                             int *error);

//Get/Set horizontal alignment angle of attdef
BITCODE_BS
dwg_ent_attdef_get_vert_align(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_vert_align(dwg_ent_attdef *attdef, BITCODE_BS alignment,
                              int *error);

//Get/Set vertical alignment of attdef
BITCODE_BS
dwg_ent_attdef_get_horiz_align(dwg_ent_attdef *attdef, int *error);

void
dwg_ent_attdef_set_horiz_align(dwg_ent_attdef *attdef, BITCODE_BS alignment,
                               int *error);


/********************************************************************
*                   FUNCTIONS FOR POINT ENTITY                      *
********************************************************************/

// Get/Set point of point entity
void
dwg_ent_point_set_point(dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error);

void
dwg_ent_point_get_point(dwg_ent_point *point, dwg_point_3d *retpoint,
                        int *error);

// Get/Set thickness of point entity
BITCODE_BD
dwg_ent_point_get_thickness(dwg_ent_point *point, int *error);

void
dwg_ent_point_set_thickness(dwg_ent_point *point, BITCODE_BD thickness,
                            int *error);

// Get/Set extrusion of point entity
void
dwg_ent_point_set_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint,
                            int *error);

void
dwg_ent_point_get_extrusion(dwg_ent_point *point, dwg_point_3d *retpoint,
                            int *error);


/********************************************************************
*                   FUNCTIONS FOR SOLID ENTITY                      *
********************************************************************/


// Get/Set thickness of solid entity
BITCODE_BD
dwg_ent_solid_get_thickness(dwg_ent_solid *solid, int *error);

void
dwg_ent_solid_set_thickness(dwg_ent_solid *solid, BITCODE_BD thickness,
                            int *error);

// Get/Set elevation of solid entity
BITCODE_BD
dwg_ent_solid_get_elevation(dwg_ent_solid *solid, int *error);

void
dwg_ent_solid_set_elevation(dwg_ent_solid *solid, BITCODE_BD elevation,
                            int *error);

// Get/Set corner1 of solid entity
void
dwg_ent_solid_get_corner1(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner1(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set corner2 of solid entity
void
dwg_ent_solid_get_corner2(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner2(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set corner3 of solid entity
void
dwg_ent_solid_get_corner3(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner3(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set corner4 of solid entity
void
dwg_ent_solid_get_corner4(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

void
dwg_ent_solid_set_corner4(dwg_ent_solid *solid, dwg_point_2d *point,
                          int *error);

// Get/Set extrusion of solid entity
void
dwg_ent_solid_get_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error);

void
dwg_ent_solid_set_extrusion(dwg_ent_solid *solid, dwg_point_3d *vector,
                            int *error);


/********************************************************************
*                   FUNCTIONS FOR BLOCk ENTITY                      *
********************************************************************/


// Get/Set text of block entity
void
dwg_ent_block_set_name(dwg_ent_block *block, char *name, int *error);

char *
dwg_ent_block_get_name(dwg_ent_block *block, int *error);


/********************************************************************
*                    FUNCTIONS FOR RAY ENTITY                       *
********************************************************************/


// Get/Set point of ray entity
void
dwg_ent_ray_get_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error);

void
dwg_ent_ray_set_point(dwg_ent_ray *ray, dwg_point_3d *point, int *error);

// Get/Set vector of ray entity
void
dwg_ent_ray_get_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error);

void
dwg_ent_ray_set_vector(dwg_ent_ray *ray, dwg_point_3d *vector, int *error);


/********************************************************************
*                   FUNCTIONS FOR XLINE ENTITY                      *
********************************************************************/


// Get/Set point of xline entity
void
dwg_ent_xline_get_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error);

void
dwg_ent_xline_set_point(dwg_ent_xline *xline, dwg_point_3d *point, int *error);

// Get/Set vector of xline entity
void
dwg_ent_xline_get_vector(dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error);

void
dwg_ent_xline_set_vector(dwg_ent_xline *xline, dwg_point_3d *vector,
                         int *error);


/********************************************************************
*                   FUNCTIONS FOR TRACE ENTITY                      *
********************************************************************/


// Get/Set thickness of trace entity
BITCODE_BD
dwg_ent_trace_get_thickness(dwg_ent_trace *trace, int *error);

void
dwg_ent_trace_set_thickness(dwg_ent_trace *trace, BITCODE_BD thickness,
                            int *error);

// Get/Set elevation of trace entity
BITCODE_BD
dwg_ent_trace_get_elevation(dwg_ent_trace *trace, int *error);

void
dwg_ent_trace_set_elevation(dwg_ent_trace *trace, BITCODE_BD elevation,
                            int *error);

// Get/Set corner1 of trace entity
void
dwg_ent_trace_get_corner1(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner1(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set corner2 of trace entity
void
dwg_ent_trace_get_corner2(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner2(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set corner3 of trace entity
void
dwg_ent_trace_get_corner3(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner3(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set corner4 of trace entity
void
dwg_ent_trace_get_corner4(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

void
dwg_ent_trace_set_corner4(dwg_ent_trace *trace, dwg_point_2d *point,
                          int *error);

// Get/Set extrusion of trace entity
void
dwg_ent_trace_get_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error);

void
dwg_ent_trace_set_extrusion(dwg_ent_trace *trace, dwg_point_3d *vector,
                            int *error);


/********************************************************************
*                 FUNCTIONS FOR VERTEX_3D ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_3d entity
char
dwg_ent_vertex_3d_get_flag(dwg_ent_vertex_3d *vert, int *error);

void
dwg_ent_vertex_3d_set_flag(dwg_ent_vertex_3d *vert, char flags, int *error);

// Get/Set point of vertex_3d entity
void
dwg_ent_vertex_3d_get_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point,
                            int *error);

void
dwg_ent_vertex_3d_set_point(dwg_ent_vertex_3d *vert, dwg_point_3d *point,
                            int *error);


/********************************************************************
*               FUNCTIONS FOR VERTEX_MESH ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_mesh entity
char
dwg_ent_vertex_mesh_get_flag(dwg_ent_vertex_mesh *vert, int *error);

void
dwg_ent_vertex_mesh_set_flag(dwg_ent_vertex_mesh *vert, char flags,
                              int *error);

// Get/Set point of vertex_mesh entity
void
dwg_ent_vertex_mesh_get_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error);

void
dwg_ent_vertex_mesh_set_point(dwg_ent_vertex_mesh *vert, dwg_point_3d *point,
                              int *error);


/********************************************************************
*               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
********************************************************************/


// Get/Set flags of vertex_pface entity
char
dwg_ent_vertex_pface_get_flag(dwg_ent_vertex_pface *vert, int *error);

void
dwg_ent_vertex_pface_set_flag(dwg_ent_vertex_pface *vert, char flags,
                               int *error);

// Get/Set point of vertex_pface entity
void
dwg_ent_vertex_pface_get_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point,
                               int *error);

void
dwg_ent_vertex_pface_set_point(dwg_ent_vertex_pface *vert, dwg_point_3d *point,
                               int *error);


/********************************************************************
*                 FUNCTIONS FOR VERTEX_2D ENTITY                    *
********************************************************************/


// Get/Set flags of vertex_2d entity
char
dwg_ent_vertex_2d_get_flag(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_flag(dwg_ent_vertex_2d *vert, char flags, int *error);

// Get/Set point of vertex_2d entity
void
dwg_ent_vertex_2d_get_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error);

void
dwg_ent_vertex_2d_set_point(dwg_ent_vertex_2d *vert, dwg_point_3d *point,
                            int *error);

// Get/Set start_width of vertex_2d entity
BITCODE_BD
dwg_ent_vertex_2d_get_start_width(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_start_width(dwg_ent_vertex_2d *vert, BITCODE_BD start_width,
                                  int *error);

// Get/Set end width of a vertex_2d entity
BITCODE_BD
dwg_ent_vertex_2d_get_end_width(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_end_width(dwg_ent_vertex_2d *vert, BITCODE_BD end_width,
                                int *error);

// Get/Set bulge of a vertex_2d entity
BITCODE_BD
dwg_ent_vertex_2d_get_bulge(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_bulge(dwg_ent_vertex_2d *vert, BITCODE_BD bulge, int *error);

// Get/Set tanget_direction of a vertex_2d entity
BITCODE_BD
dwg_ent_vertex_2d_get_tangent_dir(dwg_ent_vertex_2d *vert, int *error);

void
dwg_ent_vertex_2d_set_tangent_dir(dwg_ent_vertex_2d *vert, BITCODE_BD tangent_dir,
                                  int *error);


/********************************************************************
*                   FUNCTIONS FOR INSERT ENTITY                     *
********************************************************************/


// Get/Set insertion point of insert entity
void
dwg_ent_insert_get_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error);

void
dwg_ent_insert_set_ins_pt(dwg_ent_insert *insert, dwg_point_3d *point,
                          int *error);

// Get/Set scale flag of an insert entity
char
dwg_ent_insert_get_scale_flag(dwg_ent_insert *insert, int *error);

void
dwg_ent_insert_set_scale_flag(dwg_ent_insert *insert, char flags, int *error);

void
dwg_ent_insert_get_scale(dwg_ent_insert *insert, dwg_point_3d *point,
                         int *error);

void
dwg_ent_insert_set_scale(dwg_ent_insert *insert, dwg_point_3d *point,
                         int *error);

BITCODE_BD
dwg_ent_insert_get_rotation(dwg_ent_insert *insert, int *error);

void
dwg_ent_insert_set_rotation(dwg_ent_insert *insert, BITCODE_BD rot_ang,
                                  int *error);

void
dwg_ent_insert_get_extrusion(dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error);

void
dwg_ent_insert_set_extrusion(dwg_ent_insert *insert, dwg_point_3d *point,
                             int *error);

char
dwg_ent_insert_has_attribs(dwg_ent_insert *insert, int *error);

BITCODE_BL
dwg_ent_insert_get_owned_obj_count(dwg_ent_insert *insert, int *error);

void
dwg_ent_insert_set_owned_obj_count(dwg_ent_insert *insert, BITCODE_BL count,
                                   int *error);


/********************************************************************
*                  FUNCTIONS FOR MINSERT ENTITY                     *
********************************************************************/


void
dwg_ent_minsert_get_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error);

void
dwg_ent_minsert_set_ins_pt(dwg_ent_minsert *minsert, dwg_point_3d *point,
                           int *error);

char
dwg_ent_minsert_get_scale_flag(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_scale_flag(dwg_ent_minsert *minsert, char flags,
                               int *error);

void
dwg_ent_minsert_get_scale(dwg_ent_minsert *minsert, dwg_point_3d *point,
                          int *error);

void
dwg_ent_minsert_set_scale(dwg_ent_minsert *minsert, dwg_point_3d *point,
                          int *error);

BITCODE_BD
dwg_ent_minsert_get_rotation(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_rotation(dwg_ent_minsert *minsert, BITCODE_BD rot_ang,
                                   int *error);

void
dwg_ent_minsert_get_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error);

void
dwg_ent_minsert_set_extrusion(dwg_ent_minsert *minsert, dwg_point_3d *point,
                              int *error);

char
dwg_ent_minsert_has_attribs(dwg_ent_minsert *minsert, int *error);

BITCODE_BL
dwg_ent_minsert_get_owned_obj_count(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_owned_obj_count(dwg_ent_minsert *minsert, BITCODE_BL count,
                                    int *error);

BITCODE_BL
dwg_ent_minsert_get_numcols(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_numcols(dwg_ent_minsert *minsert, BITCODE_BL cols,
                            int *error);

BITCODE_BL
dwg_ent_minsert_get_numrows(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_numrows(dwg_ent_minsert *minsert, BITCODE_BL cols,
                            int *error);

BITCODE_BD
dwg_ent_minsert_get_col_spacing(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_col_spacing(dwg_ent_minsert *minsert, BITCODE_BD spacing,
                                int *error);

BITCODE_BD
dwg_ent_minsert_get_row_spacing(dwg_ent_minsert *minsert, int *error);

void
dwg_ent_minsert_set_row_spacing(dwg_ent_minsert *minsert, BITCODE_BD spacing,
                                int *error);


/********************************************************************
*                FUNCTIONS FOR MLINESTYLE OBJECT                    *
********************************************************************/

char *
dwg_obj_mlinestyle_get_name(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_name(dwg_obj_mlinestyle *mlinestyle, char *name,
                            int *error);

char *
dwg_obj_mlinestyle_get_desc(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_desc(dwg_obj_mlinestyle *mlinestyle, char *desc,
                            int *error);

int
dwg_obj_mlinestyle_get_flag(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_flag(dwg_obj_mlinestyle *mlinestyle, int flags,
                             int *error);

BITCODE_BD
dwg_obj_mlinestyle_get_start_angle(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_start_angle(dwg_obj_mlinestyle *mlinestyle,
                                   double startang, int *error);

BITCODE_BD
dwg_obj_mlinestyle_get_end_angle(dwg_obj_mlinestyle *mlinestyle, int *error);

void
dwg_obj_mlinestyle_set_end_angle(dwg_obj_mlinestyle *mlinestyle,
                                 double endang, int *error);

char
dwg_obj_mlinestyle_get_num_lines(dwg_obj_mlinestyle *mlinestyle,
                                    int *error);

void
dwg_obj_mlinestyle_set_num_lines(dwg_obj_mlinestyle *mlinestyle,
                                 char num_lines, int *error);


/********************************************************************
*               FUNCTIONS FOR APPID_CONTROL OBJECT                  *
********************************************************************/


BITCODE_BS
dwg_obj_appid_control_get_num_entries(dwg_obj_appid_control *appid,
                                      int *error);

dwg_object_ref *
dwg_obj_appid_control_get_appid(dwg_obj_appid_control *appid, BITCODE_BS index,
                                int *error);

/********************************************************************
*                    FUNCTIONS FOR APPID OBJECT                     *
********************************************************************/


char *
dwg_obj_appid_get_entry_name(dwg_obj_appid *appid, int *error);

void
dwg_obj_appid_set_entry_name(dwg_obj_appid *appid, char *entry_name,
                             int *error);

char
dwg_obj_appid_get_flag(dwg_obj_appid *appid, int *error);

void
dwg_obj_appid_set_flag(dwg_obj_appid *appid, char flag, int *error);

dwg_obj_appid_control *
dwg_obj_appid_get_appid_control(dwg_obj_appid *appid,
                                int *error);

/*******************************************************************
*            FUNCTIONS FOR ALL DIMENSION ENTITIES                *
********************************************************************/

char *
dwg_ent_dim_get_block_name(dwg_ent_dim *dim, int *error);

BITCODE_BD
dwg_ent_dim_get_elevation(dwg_ent_dim *dim,
                          int *error);
void
dwg_ent_dim_set_elevation(dwg_ent_dim *dim,
                          double elevation, int *error);
char
dwg_ent_dim_get_flag1(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_flag1(dwg_ent_dim *dim, char flag,
                      int *error);
BITCODE_BD
dwg_ent_dim_get_act_measurement(dwg_ent_dim *dim,
                                int *error);
void
dwg_ent_dim_set_act_measurement(dwg_ent_dim *dim,
                                double act_measurement, int *error);
BITCODE_BD
dwg_ent_dim_get_horiz_dir(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_horiz_dir(dwg_ent_dim *dim, BITCODE_BD horiz_dir,
                          int *error);
BITCODE_BD
dwg_ent_dim_get_lspace_factor(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_lspace_factor(dwg_ent_dim *dim,
                              double factor, int *error);
BITCODE_BS
dwg_ent_dim_get_lspace_style(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_lspace_style(dwg_ent_dim *dim,
                             BITCODE_BS style, int *error);
BITCODE_BS
dwg_ent_dim_get_attachment(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_attachment(dwg_ent_dim *dim,
                           BITCODE_BS attachment, int *error);
void
dwg_ent_dim_set_extrusion(dwg_ent_dim *dim,
                          dwg_point_3d *point, int *error);
void
dwg_ent_dim_get_extrusion(dwg_ent_dim *dim,
                          dwg_point_3d *point, int *error);
/* (utf-8 encoded) */
char *
dwg_ent_dim_get_user_text(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_user_text(dwg_ent_dim *dim, char *text,
                          int *error);
BITCODE_BD
dwg_ent_dim_get_text_rot(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_text_rot(dwg_ent_dim *dim, BITCODE_BD rot,
                         int *error);
BITCODE_BD
dwg_ent_dim_get_ins_rotation(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_ins_rotation(dwg_ent_dim *dim, BITCODE_BD rot,
                             int *error);
char
dwg_ent_dim_get_flip_arrow1(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_flip_arrow1(dwg_ent_dim *dim,
                            char flip_arrow, int *error);
char
dwg_ent_dim_get_flip_arrow2(dwg_ent_dim *dim, int *error);

void
dwg_ent_dim_set_flip_arrow2(dwg_ent_dim *dim,
                            char flip_arrow, int *error);
void
dwg_ent_dim_set_text_mid_pt(dwg_ent_dim *dim,
                            dwg_point_2d *point, int *error);
void
dwg_ent_dim_get_text_mid_pt(dwg_ent_dim *dim,
                            dwg_point_2d *point, int *error);
void
dwg_ent_dim_set_ins_scale(dwg_ent_dim *dim,
                          dwg_point_3d *point, int *error);
void
dwg_ent_dim_get_ins_scale(dwg_ent_dim *dim,
                          dwg_point_3d *point, int *error);
void
dwg_ent_dim_set_clone_ins_pt(dwg_ent_dim *dim, dwg_point_2d *point,
                             int *error);
void
dwg_ent_dim_get_clone_ins_pt(dwg_ent_dim *dim, dwg_point_2d *point,
                             int *error);

/********************************************************************
*            FUNCTIONS FOR ORDINATE DIMENSION ENTITY                *
********************************************************************/


void
dwg_ent_dim_ordinate_set_ucsorigin_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                                      int *error);

void
dwg_ent_dim_ordinate_get_ucsorigin_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                                      int *error);

void
dwg_ent_dim_ordinate_set_feature_location_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_get_feature_location_pt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_set_leader_endpt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_get_leader_endpt(dwg_ent_dim_ordinate *ord, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_ordinate_set_flag2(dwg_ent_dim_ordinate *ord, char flag,
                                int *error);

char
dwg_ent_dim_ordinate_get_flag2(dwg_ent_dim_ordinate *ord, int *error);


/********************************************************************
*              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
********************************************************************/

void
dwg_ent_dim_linear_set_def_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_get_def_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_set_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_get_13_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_set_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_linear_get_14_pt(dwg_ent_dim_linear *dim, dwg_point_3d *point,
                             int *error);

BITCODE_BD
dwg_ent_dim_linear_get_dim_rot(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_dim_rot(dwg_ent_dim_linear *dim, BITCODE_BD rot,
                               int *error);

BITCODE_BD
dwg_ent_dim_linear_get_ext_line_rotation(dwg_ent_dim_linear *dim, int *error);

void
dwg_ent_dim_linear_set_ext_line_rotation(dwg_ent_dim_linear *dim, BITCODE_BD rot,
                                         int *error);


/********************************************************************
*             FUNCTIONS FOR ALIGNED DIMENSION ENTITY                *
********************************************************************/

void
dwg_ent_dim_aligned_set_def_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_get_def_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_set_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_get_13_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_set_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

void
dwg_ent_dim_aligned_get_14_pt(dwg_ent_dim_aligned *dim, dwg_point_3d *point,
                              int *error);

BITCODE_BD
dwg_ent_dim_aligned_get_ext_line_rotation(dwg_ent_dim_aligned *dim,
                                          int *error);

void
dwg_ent_dim_aligned_set_ext_line_rotation(dwg_ent_dim_aligned *dim, BITCODE_BD rot,
                                          int *error);


/********************************************************************
*              FUNCTIONS FOR ANG3PT DIMENSION ENTITY                *
********************************************************************/

void
dwg_ent_dim_ang3pt_set_def_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_def_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_set_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_13_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_set_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_14_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_set_first_arc_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang3pt_get_first_arc_pt(dwg_ent_dim_ang3pt *ang, dwg_point_3d *point,
                             int *error);


/********************************************************************
*              FUNCTIONS FOR ANG2LN DIMENSION ENTITY                *
********************************************************************/

void
dwg_ent_dim_ang2ln_set_def_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_def_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_13_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_14_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_first_arc_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_first_arc_pt(dwg_ent_dim_ang2ln *ang, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_set_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error);

void
dwg_ent_dim_ang2ln_get_16_pt(dwg_ent_dim_ang2ln *ang, dwg_point_2d *point,
                             int *error);


/********************************************************************
*              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
********************************************************************/

void
dwg_ent_dim_radius_set_def_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_radius_get_def_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_radius_set_first_arc_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

void
dwg_ent_dim_radius_get_first_arc_pt(dwg_ent_dim_radius *radius, dwg_point_3d *point,
                             int *error);

BITCODE_BD
dwg_ent_dim_radius_get_leader_length(dwg_ent_dim_radius *radius, int *error);

void
dwg_ent_dim_radius_set_leader_length(dwg_ent_dim_radius *radius, BITCODE_BD length,
                                  int *error);


/********************************************************************
*             FUNCTIONS FOR DIAMETER DIMENSION ENTITY               *
********************************************************************/

void
dwg_ent_dim_diameter_set_def_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_diameter_get_def_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_diameter_set_first_arc_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

void
dwg_ent_dim_diameter_get_first_arc_pt(dwg_ent_dim_diameter *dia, dwg_point_3d *point,
                               int *error);

BITCODE_BD
dwg_ent_dim_diameter_get_leader_length(dwg_ent_dim_diameter *dia, int *error);

void
dwg_ent_dim_diameter_set_leader_length(dwg_ent_dim_diameter *dia,
                                    double leader_len, int *error);


/********************************************************************
*                   FUNCTIONS FOR ENDBLK ENTITY                     *
********************************************************************/


char
dwg_ent_endblk_get_dummy(dwg_ent_endblk *endblk, int *error);

void
dwg_ent_endblk_set_dummy(dwg_ent_endblk *endblk, char dummy, int *error);


/********************************************************************
*                   FUNCTIONS FOR SEQEND ENTITY                     *
********************************************************************/


char
dwg_ent_seqend_get_dummy(dwg_ent_seqend *seqend, int *error);

void
dwg_ent_seqend_set_dummy(dwg_ent_seqend *seqend, char dummy, int *error);


/********************************************************************
*                    FUNCTIONS FOR SHAPE ENTITY                     *
********************************************************************/


void
dwg_ent_shape_get_ins_pt(dwg_ent_shape *shape, dwg_point_3d *point,
                         int *error);

void
dwg_ent_shape_set_ins_pt(dwg_ent_shape *shape, dwg_point_3d *point,
                         int *error);

BITCODE_BD
dwg_ent_shape_get_scale(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_scale(dwg_ent_shape *shape, BITCODE_BD scale, int *error);

BITCODE_BD
dwg_ent_shape_get_rotation(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_rotation(dwg_ent_shape *shape, BITCODE_BD rotation, int *error);

BITCODE_BD
dwg_ent_shape_get_width_factor(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_width_factor(dwg_ent_shape *shape, BITCODE_BD width_factor,
                               int *error);

BITCODE_BD
dwg_ent_shape_get_oblique(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_oblique(dwg_ent_shape *shape, BITCODE_BD oblique, int *error);

BITCODE_BD
dwg_ent_shape_get_thickness(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_thickness(dwg_ent_shape *shape, BITCODE_BD thickness,
                            int *error);

BITCODE_BD
dwg_ent_shape_get_shape_no(dwg_ent_shape *shape, int *error);

void
dwg_ent_shape_set_shape_no(dwg_ent_shape *shape, BITCODE_BD no, int *error);

void
dwg_ent_shape_get_extrusion(dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error);

void
dwg_ent_shape_set_extrusion(dwg_ent_shape *shape, dwg_point_3d *point,
                            int *error);


/********************************************************************
*                    FUNCTIONS FOR MTEXT ENTITY                     *
********************************************************************/


void
dwg_ent_mtext_set_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error);

void
dwg_ent_mtext_get_insertion_pt(dwg_ent_mtext *mtext, dwg_point_3d *point,
                               int *error);

void
dwg_ent_mtext_set_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mtext_get_extrusion(dwg_ent_mtext *mtext, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mtext_set_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mtext_get_x_axis_dir(dwg_ent_mtext *mtext, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mtext_set_rect_height(dwg_ent_mtext *mtext, BITCODE_BD rect_height,
                              int *error);

BITCODE_BD
dwg_ent_mtext_get_rect_height(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_rect_width(dwg_ent_mtext *mtext, BITCODE_BD rect_width,
                             int *error);

BITCODE_BD
dwg_ent_mtext_get_rect_width(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_text_height(dwg_ent_mtext *mtext, BITCODE_BD text_height,
                              int *error);

BITCODE_BD
dwg_ent_mtext_get_text_height(dwg_ent_mtext *mtext, int *error);

BITCODE_BS
dwg_ent_mtext_get_attachment(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_attachment(dwg_ent_mtext *mtext, BITCODE_BS attachment,
                             int *error);

BITCODE_BS
dwg_ent_mtext_get_drawing_dir(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_drawing_dir(dwg_ent_mtext *mtext, BITCODE_BS dir,
                              int *error);

BITCODE_BD
dwg_ent_mtext_get_extents_height(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_extents_height(dwg_ent_mtext *mtext, BITCODE_BD ht, int *error);

BITCODE_BD
dwg_ent_mtext_get_extents_width(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_extents_width(dwg_ent_mtext *mtext, BITCODE_BD wid, int *error);

/* (utf-8 encoded) */
char* dwg_ent_mtext_get_text(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_text(dwg_ent_mtext *mtext, char *text, int *error);

BITCODE_BS
dwg_ent_mtext_get_linespace_style(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_linespace_style(dwg_ent_mtext *mtext, BITCODE_BS style,
                                  int *error);

BITCODE_BD
dwg_ent_mtext_get_linespace_factor(dwg_ent_mtext *mtext, int *error);

void
dwg_ent_mtext_set_linespace_factor(dwg_ent_mtext *mtext, BITCODE_BD factor,
                                   int *error);


/********************************************************************
*                   FUNCTIONS FOR LEADER ENTITY                     *
********************************************************************/


void
dwg_ent_leader_set_annot_type(dwg_ent_leader *leader, BITCODE_BS type,
                              int *error);

BITCODE_BS
dwg_ent_leader_get_annot_type(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_path_type(dwg_ent_leader *leader, BITCODE_BS type,
                             int *error);

BITCODE_BS
dwg_ent_leader_get_path_type(dwg_ent_leader *leader, int *error);

BITCODE_BL
dwg_ent_leader_get_numpts(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_numpts(dwg_ent_leader *leader, BITCODE_BL numpts, int *error);

void
dwg_ent_leader_set_origin(dwg_ent_leader *leader, dwg_point_3d *point,
                          int *error);

void
dwg_ent_leader_get_origin(dwg_ent_leader *leader, dwg_point_3d *point,
                          int *error);

void
dwg_ent_leader_set_extrusion(dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error);

void
dwg_ent_leader_get_extrusion(dwg_ent_leader *leader, dwg_point_3d *point,
                             int *error);

void
dwg_ent_leader_set_x_direction(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error);

void
dwg_ent_leader_get_x_direction(dwg_ent_leader *leader, dwg_point_3d *point,
                               int *error);

void
dwg_ent_leader_set_offset_to_block_ins_pt(dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error);

void
dwg_ent_leader_get_offset_to_block_ins_pt(dwg_ent_leader *leader,
                                          dwg_point_3d *point, int *error);

void
dwg_ent_leader_set_dimgap(dwg_ent_leader *leader, BITCODE_BD dimgap, int *error);

BITCODE_BD
dwg_ent_leader_get_dimgap(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_box_height(dwg_ent_leader *leader, BITCODE_BD height,
                              int *error);

BITCODE_BD
dwg_ent_leader_get_box_height(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_box_width(dwg_ent_leader *leader, BITCODE_BD width,
                             int *error);

BITCODE_BD
dwg_ent_leader_get_box_width(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_hook_line_on_x_dir(dwg_ent_leader *leader, char hook,
                                      int *error);

char
dwg_ent_leader_get_hook_line_on_x_dir(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_arrowhead_on(dwg_ent_leader *leader, char arrow,
                                int *error);

char
dwg_ent_leader_get_arrowhead_on(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_arrowhead_type(dwg_ent_leader *leader, BITCODE_BS type,
                                  int *error);

BITCODE_BS
dwg_ent_leader_get_arrowhead_type(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_dimasz(dwg_ent_leader *leader, BITCODE_BD dimasz, int *error);

BITCODE_BD
dwg_ent_leader_get_dimasz(dwg_ent_leader *leader, int *error);

void
dwg_ent_leader_set_byblock_color(dwg_ent_leader *leader, BITCODE_BS color,
                                 int *error);

BITCODE_BS
dwg_ent_leader_get_byblock_color(dwg_ent_leader *leader, int *error);


/********************************************************************
*                  FUNCTIONS FOR TOLERANCE ENTITY                   *
********************************************************************/


void
dwg_ent_tolerance_set_height(dwg_ent_tolerance *tol, BITCODE_BD height,
                             int *error);

BITCODE_BD
dwg_ent_tolerance_get_height(dwg_ent_tolerance *tol, int *error);

void
dwg_ent_tolerance_set_dimgap(dwg_ent_tolerance *tol, BITCODE_BD dimgap,
                             int *error);

BITCODE_BD
dwg_ent_tolerance_get_dimgap(dwg_ent_tolerance *tol, int *error);

void
dwg_ent_tolerance_set_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error);

void
dwg_ent_tolerance_get_ins_pt(dwg_ent_tolerance *tol, dwg_point_3d *point,
                             int *error);

void
dwg_ent_tolerance_set_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_tolerance_get_x_direction(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_tolerance_set_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error);

void
dwg_ent_tolerance_get_extrusion(dwg_ent_tolerance *tol, dwg_point_3d *point,
                                int *error);

/* (utf-8 encoded) */
void
dwg_ent_tolerance_set_text_string(dwg_ent_tolerance *tol, char *string,
                                  int *error);

char *
dwg_ent_tolerance_get_text_string(dwg_ent_tolerance *tol, int *error);


/********************************************************************
*                   FUNCTIONS FOR LWPLINE ENTITY                    *
********************************************************************/


BITCODE_BS
dwg_ent_lwpline_get_flag(dwg_ent_lwpline *lwpline, int *error);
void
dwg_ent_lwpline_set_flag(dwg_ent_lwpline *lwpline, char flags, int *error);

BITCODE_BD
dwg_ent_lwpline_get_const_width(dwg_ent_lwpline *lwpline, int *error);
void
dwg_ent_lwpline_set_const_width(dwg_ent_lwpline *lwpline, BITCODE_BD const_width,
                                int *error);

BITCODE_BD
dwg_ent_lwpline_get_elevation(dwg_ent_lwpline *lwpline, int *error);
void
dwg_ent_lwpline_set_elevation(dwg_ent_lwpline *lwpline, BITCODE_BD elevation,
                              int *error);

BITCODE_BD
dwg_ent_lwpline_get_thickness(dwg_ent_lwpline *lwpline, int *error);
void
dwg_ent_lwpline_set_thickness(dwg_ent_lwpline *lwpline, BITCODE_BD thickness,
                              int *error);

void
dwg_ent_lwpline_get_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points,
                           int *error);
void
dwg_ent_lwpline_set_normal(dwg_ent_lwpline *lwpline, dwg_point_3d *points,
                           int *error);

BITCODE_BL
dwg_ent_lwpline_get_numpoints(dwg_ent_lwpline *lwpline, int *error);

BITCODE_BL
dwg_ent_lwpline_get_numbulges(dwg_ent_lwpline *lwpline, int *error);

BITCODE_BL
dwg_ent_lwpline_get_numwidths(dwg_ent_lwpline *lwpline, int *error);

double *
dwg_ent_lwpline_get_bulges(dwg_ent_lwpline *lwpline, int *error);

dwg_point_2d *
dwg_ent_lwpline_get_points(dwg_ent_lwpline *lwpline, int *error);

dwg_lwpline_widths *
dwg_ent_lwpline_get_widths(dwg_ent_lwpline *lwpline, int *error);


/********************************************************************
*                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
********************************************************************/


BITCODE_BS
dwg_ent_ole2frame_get_flag(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_flag(dwg_ent_ole2frame *frame, BITCODE_BS flags,
                            int *error);

BITCODE_BS
dwg_ent_ole2frame_get_mode(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_mode(dwg_ent_ole2frame *frame, BITCODE_BS mode,
                           int *error);

BITCODE_BL
dwg_ent_ole2frame_get_data_length(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_data_length(dwg_ent_ole2frame *frame, BITCODE_BL data_length,
                                  int *error);

char *
dwg_ent_ole2frame_get_data(dwg_ent_ole2frame *frame, int *error);

void
dwg_ent_ole2frame_set_data(dwg_ent_ole2frame *frame, char *data, int *error);


/********************************************************************
*                  FUNCTIONS FOR PROXY OBJECT                       *
********************************************************************/


BITCODE_BL
dwg_obj_proxy_get_class_id(dwg_obj_proxy *proxy, int *error);

void
dwg_obj_proxy_set_class_id(dwg_obj_proxy *proxy, BITCODE_BL class_id,
                        int *error);

BITCODE_BL
dwg_obj_proxy_get_version(dwg_obj_proxy *proxy, int *error);

void
dwg_obj_proxy_set_version(dwg_obj_proxy *proxy, BITCODE_BL version,
                          int *error);

BITCODE_B
dwg_obj_proxy_get_from_dxf(dwg_obj_proxy *proxy, int *error);

void
dwg_obj_proxy_set_from_dxf(dwg_obj_proxy *proxy, BITCODE_B from_dxf,
                           int *error);

char*
dwg_obj_proxy_get_data(dwg_obj_proxy *proxy, int *error);

void
dwg_obj_proxy_set_data(dwg_obj_proxy *proxy, char* data,
                       int *error);

dwg_object_ref*
dwg_obj_proxy_get_parenthandle(dwg_obj_proxy *proxy, int *error);

Dwg_Object_Ref**
dwg_obj_proxy_get_reactors(dwg_obj_proxy *proxy, int *error);

dwg_object_ref**
dwg_obj_proxy_get_objid_object_handles(dwg_obj_proxy *proxy, int *error);


/********************************************************************
*                   FUNCTIONS FOR SPLINE ENTITY                     *
********************************************************************/


BITCODE_BS
dwg_ent_spline_get_scenario(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_scenario(dwg_ent_spline *spline, BITCODE_BS scenario,
                               int *error);

BITCODE_BS
dwg_ent_spline_get_degree(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_degree(dwg_ent_spline *spline, BITCODE_BS degree,
                             int *error);

BITCODE_BD
dwg_ent_spline_get_fit_tol(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_fit_tol(dwg_ent_spline *spline, int fit_tol,
                              int *error);

void
dwg_ent_spline_get_begin_tan_vector(dwg_ent_spline *spline,
                                       dwg_point_3d *point, int *error);

void
dwg_ent_spline_set_begin_tan_vector(dwg_ent_spline *spline,
                                       dwg_point_3d *point, int *error);

void
dwg_ent_spline_get_end_tan_vector(dwg_ent_spline *spline,
                                     dwg_point_3d *point, int *error);

void
dwg_ent_spline_set_end_tan_vector(dwg_ent_spline *spline,
                                     dwg_point_3d *point, int *error);

BITCODE_BD
dwg_ent_spline_get_knot_tol(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_knot_tol(dwg_ent_spline *spline, BITCODE_BD knot_tol,
                               int *error);

BITCODE_BD
dwg_ent_spline_get_ctrl_tol(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_ctrl_tol(dwg_ent_spline *spline, BITCODE_BD ctrl_tol,
                               int *error);

BITCODE_BS
dwg_ent_spline_get_num_fit_pts(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_num_fit_pts(dwg_ent_spline *spline, BITCODE_BS num_fit_pts,
                                  int *error);

char
dwg_ent_spline_get_rational(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_rational(dwg_ent_spline *spline, char rational,
                               int *error);

char
dwg_ent_spline_get_closed_b(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_closed_b(dwg_ent_spline *spline, char closed_b,
                               int *error);

char
dwg_ent_spline_get_weighted(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_weighted(dwg_ent_spline *spline, char weighted,
                               int *error);

char
dwg_ent_spline_get_periodic(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_periodic(dwg_ent_spline *spline, char periodic,
                               int *error);

BITCODE_BL
dwg_ent_spline_get_num_knots(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_num_knots(dwg_ent_spline *spline, BITCODE_BL nums, int *error);

BITCODE_BL
dwg_ent_spline_get_num_ctrl_pts(dwg_ent_spline *spline, int *error);

void
dwg_ent_spline_set_num_ctrl_pts(dwg_ent_spline *spline, BITCODE_BL nums,
                                   int *error);

dwg_ent_spline_point *
dwg_ent_spline_get_fit_points(dwg_ent_spline *spline, int *error);

dwg_ent_spline_control_point *
dwg_ent_spline_get_ctrl_pts(dwg_ent_spline *spline, int *error);

double *
dwg_ent_spline_get_knots(dwg_ent_spline *spline, int *error);


/********************************************************************
*                   FUNCTIONS FOR VIEWPORT ENTITY                   *
********************************************************************/


void
dwg_ent_viewport_get_center(dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error);

void
dwg_ent_viewport_set_center(dwg_ent_viewport *vp, dwg_point_3d *point,
                            int *error);

BITCODE_BD
dwg_ent_viewport_get_width(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_width(dwg_ent_viewport *vp, BITCODE_BD width, int *error);

BITCODE_BD
dwg_ent_viewport_get_height(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_height(dwg_ent_viewport *vp, BITCODE_BD height, int *error);

BITCODE_BS
dwg_ent_viewport_get_grid_major(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_grid_major(dwg_ent_viewport *vp, BITCODE_BS major,
                                int *error);

BITCODE_BL
dwg_ent_viewport_get_frozen_layer_count(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_frozen_layer_count(dwg_ent_viewport *vp, BITCODE_BL count,
                                        int *error);

char *
dwg_ent_viewport_get_style_sheet(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_style_sheet(dwg_ent_viewport *vp, char *sheet,
                                 int *error);

void
dwg_ent_viewport_set_circle_zoom(dwg_ent_viewport *vp, BITCODE_BS zoom,
                                 int *error);

BITCODE_BS
dwg_ent_viewport_get_circle_zoom(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_status_flag(dwg_ent_viewport *vp, BITCODE_BL flags,
                                  int *error);

BITCODE_BL
dwg_ent_viewport_get_status_flag(dwg_ent_viewport *vp, int *error);

char
dwg_ent_viewport_get_render_mode(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_render_mode(dwg_ent_viewport *vp, char mode, int *error);

void
dwg_ent_viewport_set_ucs_at_origin(dwg_ent_viewport *vp, unsigned char origin,
                                   int *error);

unsigned char
dwg_ent_viewport_get_ucs_at_origin(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_ucs_per_viewport(dwg_ent_viewport *vp,
                                      unsigned char viewport, int *error);

unsigned char
dwg_ent_viewport_get_ucs_per_viewport(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_view_target(dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_viewport_get_view_target(dwg_ent_viewport *vp, dwg_point_3d *point,
                                 int *error);

void
dwg_ent_viewport_set_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error);

void
dwg_ent_viewport_get_view_direction(dwg_ent_viewport *vp, dwg_point_3d *point,
                                    int *error);

void
dwg_ent_viewport_set_view_twist_angle(dwg_ent_viewport *vp, BITCODE_BD angle,
                                      int *error);

BITCODE_BD
dwg_ent_viewport_get_view_twist_angle(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_view_height(dwg_ent_viewport *vp, BITCODE_BD height,
                                 int *error);

BITCODE_BD
dwg_ent_viewport_get_view_height(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_lens_length(dwg_ent_viewport *vp, BITCODE_BD length,
                                 int *error);

BITCODE_BD
dwg_ent_viewport_get_lens_length(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_front_clip_z(dwg_ent_viewport *vp, BITCODE_BD front_z,
                                  int *error);

BITCODE_BD
dwg_ent_viewport_get_front_clip_z(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_back_clip_z(dwg_ent_viewport *vp, BITCODE_BD back_z,
                                 int *error);

BITCODE_BD
dwg_ent_viewport_get_back_clip_z(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_snap_angle(dwg_ent_viewport *vp, BITCODE_BD angle,
                                int *error);

BITCODE_BD
dwg_ent_viewport_get_snap_angle(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_get_view_center(dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error);

void
dwg_ent_viewport_set_view_center(dwg_ent_viewport *vp, dwg_point_2d *point,
                                 int *error);

void
dwg_ent_viewport_get_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_set_grid_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_get_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error);

void
dwg_ent_viewport_set_snap_base(dwg_ent_viewport *vp, dwg_point_2d *point,
                               int *error);

void
dwg_ent_viewport_get_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_set_snap_spacing(dwg_ent_viewport *vp, dwg_point_2d *point,
                                  int *error);

void
dwg_ent_viewport_set_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_get_ucs_origin(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_set_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_get_ucs_x_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_set_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_get_ucs_y_axis(dwg_ent_viewport *vp, dwg_point_3d *point,
                                int *error);

void
dwg_ent_viewport_set_ucs_elevation(dwg_ent_viewport *vp, BITCODE_BD elevation,
                                   int *error);

BITCODE_BD
dwg_ent_viewport_get_ucs_elevation(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_ucs_ortho_view_type(dwg_ent_viewport *vp, 
                                         BITCODE_BS type, int *error);

BITCODE_BS
dwg_ent_viewport_get_ucs_ortho_view_type(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_shadeplot_mode(dwg_ent_viewport *vp,
                                    BITCODE_BS shadeplot, int *error);

BITCODE_BS
dwg_ent_viewport_get_shadeplot_mode(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_use_default_lights(dwg_ent_viewport *vp,
                                    unsigned char lights, int *error);

unsigned char
dwg_ent_viewport_get_use_default_lights(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_default_lighting_type(dwg_ent_viewport *vp, char type,
                                       int *error);

char
dwg_ent_viewport_get_default_lighting_type(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_brightness(dwg_ent_viewport *vp, BITCODE_BD brightness,
                                int *error);

BITCODE_BD
dwg_ent_viewport_get_brightness(dwg_ent_viewport *vp, int *error);

void
dwg_ent_viewport_set_contrast(dwg_ent_viewport *vp, BITCODE_BD contrast,
                              int *error);

BITCODE_BD
dwg_ent_viewport_get_contrast(dwg_ent_viewport *vp, int *error);


/********************************************************************
*                FUNCTIONS FOR POLYLINE PFACE ENTITY                *
********************************************************************/


BITCODE_BS
dwg_ent_polyline_pface_get_numpoints(dwg_ent_polyline_pface *pface, int *error);

BITCODE_BS
dwg_ent_polyline_pface_get_numfaces(dwg_ent_polyline_pface *pface, int *error);

dwg_point_3d *
dwg_obj_polyline_pface_get_points(dwg_object *obj, int *error);

/********************************************************************
*                FUNCTIONS FOR POLYLINE_MESH ENTITY                 *
********************************************************************/


BITCODE_BS
dwg_ent_polyline_mesh_get_flag(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_flag(dwg_ent_polyline_mesh *mesh,
                                BITCODE_BS flags, int *error);

BITCODE_BS
dwg_ent_polyline_mesh_get_curve_type(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_curve_type(dwg_ent_polyline_mesh *mesh,
                                     BITCODE_BS curve_type, int *error);

BITCODE_BS
dwg_ent_polyline_mesh_get_m_vert_count(dwg_ent_polyline_mesh *mesh,
                                       int *error);

void
dwg_ent_polyline_mesh_set_m_vert_count(dwg_ent_polyline_mesh *mesh,
                                       BITCODE_BS m_vert_count, int *error);

BITCODE_BS
dwg_ent_polyline_mesh_get_n_vert_count(dwg_ent_polyline_mesh *mesh,
                                       int *error);

void
dwg_ent_polyline_mesh_set_n_vert_count(dwg_ent_polyline_mesh *mesh,
                                       BITCODE_BS n_vert_count, int *error);

BITCODE_BS
dwg_ent_polyline_mesh_get_m_density(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_m_density(dwg_ent_polyline_mesh *mesh,
                                    BITCODE_BS m_density, int *error);

BITCODE_BS
dwg_ent_polyline_mesh_get_n_density(dwg_ent_polyline_mesh *mesh, int *error);

void
dwg_ent_polyline_mesh_set_n_density(dwg_ent_polyline_mesh *mesh,
                                    BITCODE_BS n_density, int *error);

BITCODE_BL
dwg_ent_polyline_mesh_get_owned_obj_count(dwg_ent_polyline_mesh *mesh,
                                          int *error);

void
dwg_ent_polyline_mesh_set_owned_obj_count(dwg_ent_polyline_mesh *mesh,
                                          BITCODE_BL owned_obj_count, int *error);


/********************************************************************
*                 FUNCTIONS FOR POLYLINE_2D ENTITY                  *
********************************************************************/


void
dwg_ent_polyline_2d_get_extrusion(dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error);

void
dwg_ent_polyline_2d_set_extrusion(dwg_ent_polyline_2d *line2d,
                                  dwg_point_3d *point, int *error);

BITCODE_BD
dwg_ent_polyline_2d_get_start_width(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_start_width(dwg_ent_polyline_2d *line2d,
                                    double start_width, int *error);

BITCODE_BD
dwg_ent_polyline_2d_get_end_width(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_end_width(dwg_ent_polyline_2d *line2d,
                                  double end_width, int *error);

BITCODE_BD
dwg_ent_polyline_2d_get_thickness(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_thickness(dwg_ent_polyline_2d *line2d,
                                  double thickness, int *error);

BITCODE_BD
dwg_ent_polyline_2d_get_elevation(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_elevation(dwg_ent_polyline_2d *line2d,
                                  double elevation, int *error);

BITCODE_BS
dwg_ent_polyline_2d_get_flag(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_flag(dwg_ent_polyline_2d *line2d, BITCODE_BS flags,
                              int *error);

BITCODE_BS
dwg_ent_polyline_2d_get_curve_type(dwg_ent_polyline_2d *line2d, int *error);

void
dwg_ent_polyline_2d_set_curve_type(dwg_ent_polyline_2d *line2d,
                                   BITCODE_BS curve_type, int *error);

BITCODE_BL
dwg_obj_polyline_2d_get_numpoints(dwg_object *obj, int *error);

dwg_point_2d *
dwg_obj_polyline_2d_get_points(dwg_object *obj, int *error);


/********************************************************************
*                 FUNCTIONS FOR POLYLINE_3D ENTITY                  *
********************************************************************/


char
dwg_ent_polyline_3d_get_flag(dwg_ent_polyline_3d *line3d, int *error);

void
dwg_ent_polyline_3d_set_flag(dwg_ent_polyline_3d *line3d, char flag,
                             int *error);

char
dwg_ent_polyline_3d_get_flag2(dwg_ent_polyline_3d *line3d, int *error);

void
dwg_ent_polyline_3d_set_flag2(dwg_ent_polyline_3d *line3d, char flag2,
                                int *error);

BITCODE_BL
dwg_ent_polyline_3d_get_owned_obj_count(dwg_ent_polyline_3d *line3d,
                                        int *error);

void
dwg_ent_polyline_3d_set_owned_obj_count(dwg_ent_polyline_3d *line3d,
                                        BITCODE_BL owned_obj_count, int *error);


/********************************************************************
*                   FUNCTIONS FOR 3DFACE ENTITY                     *
********************************************************************/


BITCODE_BS
dwg_ent_3dface_get_invis_flags(dwg_ent_3dface *_3dface, int *error);

void
dwg_ent_3dface_set_invis_flags(dwg_ent_3dface *_3dface,
                               BITCODE_BS invis_flags, int *error);

void
dwg_ent_3dface_get_corner1(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner1(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_get_corner2(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner2(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_get_corner3(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner3(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_get_corner4(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);

void
dwg_ent_3dface_set_corner4(dwg_ent_3dface *_3dface, dwg_point_2d *point,
                           int *error);


/********************************************************************
*                    FUNCTIONS FOR IMAGE ENTITY                     *
********************************************************************/


BITCODE_BL
dwg_ent_image_get_class_version(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_class_version(dwg_ent_image *image, BITCODE_BL class_version,
                                int *error);

void
dwg_ent_image_get_pt0(dwg_ent_image *image, dwg_point_3d *point, int *error);

void
dwg_ent_image_set_pt0(dwg_ent_image *image, dwg_point_3d *point, int *error);

void
dwg_ent_image_get_u_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

void
dwg_ent_image_set_u_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

void
dwg_ent_image_get_v_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

void
dwg_ent_image_set_v_vector(dwg_ent_image *image, dwg_point_3d *point,
                           int *error);

BITCODE_BD
dwg_ent_image_get_size_height(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_size_height(dwg_ent_image *image, BITCODE_BD size_height,
                              int *error);

BITCODE_BD
dwg_ent_image_get_size_width(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_size_width(dwg_ent_image *image, BITCODE_BD size_width,
                             int *error);

BITCODE_BS
dwg_ent_image_get_display_props(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_display_props(dwg_ent_image *image,
                                BITCODE_BS display_props, int *error);

unsigned char
dwg_ent_image_get_clipping(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_clipping(dwg_ent_image *image, unsigned char clipping,
                           int *error);

char
dwg_ent_image_get_brightness(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_brightness(dwg_ent_image *image, char brightness,
                             int *error);

char
dwg_ent_image_get_contrast(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_contrast(dwg_ent_image *image, char contrast, int *error);

char
dwg_ent_image_get_fade(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_fade(dwg_ent_image *image, char fade, int *error);

BITCODE_BS
dwg_ent_image_get_clip_boundary_type(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_clip_boundary_type(dwg_ent_image *image, BITCODE_BS type,
                                    int *error);

void
dwg_ent_image_get_boundary_pt0(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

void
dwg_ent_image_set_boundary_pt0(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

void
dwg_ent_image_get_boundary_pt1(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

void
dwg_ent_image_set_boundary_pt1(dwg_ent_image *image, dwg_point_2d *point,
                               int *error);

BITCODE_BD
dwg_ent_image_get_num_clip_verts(dwg_ent_image *image, int *error);

void
dwg_ent_image_set_num_clip_verts(dwg_ent_image *image, BITCODE_BD num, int *error);

BITCODE_2RD *
dwg_ent_image_get_clip_verts(dwg_ent_image *image, int *error);


/********************************************************************
*                    FUNCTIONS FOR MLINE ENTITY                     *
********************************************************************/


void
dwg_ent_mline_set_scale(dwg_ent_mline *mline, BITCODE_BD scale, int *error);

BITCODE_BD
dwg_ent_mline_get_scale(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_justification(dwg_ent_mline *mline, BITCODE_RC just, int *error);

BITCODE_RC
dwg_ent_mline_get_justification(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_base_point(dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mline_get_base_point(dwg_ent_mline *mline, dwg_point_3d *point,
                             int *error);

void
dwg_ent_mline_set_extrusion(dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mline_get_extrusion(dwg_ent_mline *mline, dwg_point_3d *point,
                            int *error);

void
dwg_ent_mline_set_flags(dwg_ent_mline *mline, BITCODE_BS oc,
                              int *error);

BITCODE_BS
dwg_ent_mline_get_flags(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_num_lines(dwg_ent_mline *mline, BITCODE_RC num, int *error);

BITCODE_RC
dwg_ent_mline_get_num_lines(dwg_ent_mline *mline, int *error);

void
dwg_ent_mline_set_num_verts(dwg_ent_mline *mline, BITCODE_BS num,
                            int *error);

BITCODE_BS
dwg_ent_mline_get_num_verts(dwg_ent_mline *mline, int *error);

dwg_ent_mline_vertex *
dwg_ent_mline_get_verts(dwg_ent_mline *mline, int *error);

dwg_ent_mline_line *
dwg_ent_mline_get_lines(dwg_ent_mline *mline, int *error);

/********************************************************************
*                  FUNCTIONS FOR 3DSOLID ENTITY                     *
********************************************************************/


unsigned char
dwg_ent_3dsolid_get_acis_empty(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_acis_empty(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                               int *error);

BITCODE_BS
dwg_ent_3dsolid_get_version(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_version(dwg_ent_3dsolid *_3dsolid, BITCODE_BS version,
                            int *error);

BITCODE_BL*
dwg_ent_3dsolid_get_block_size(dwg_ent_3dsolid *_3dsolid, int *error);

char *
dwg_ent_3dsolid_get_acis_data(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_acis_data(dwg_ent_3dsolid *_3dsolid, char *data,
                              int *error);

char
dwg_ent_3dsolid_get_wireframe_data_present(dwg_ent_3dsolid *_3dsolid,
                                           int *error);

void
dwg_ent_3dsolid_set_wireframe_data_present(dwg_ent_3dsolid *_3dsolid,
                                           char data, int *error);

char
dwg_ent_3dsolid_get_point_present(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_point_present(dwg_ent_3dsolid *_3dsolid, char point,
                                  int *error);

void
dwg_ent_3dsolid_get_point(dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point, 
                          int *error);

void
dwg_ent_3dsolid_set_point(dwg_ent_3dsolid *_3dsolid, dwg_point_3d *point, 
                          int *error);

BITCODE_BL
dwg_ent_3dsolid_get_num_isolines(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_num_isolines(dwg_ent_3dsolid *_3dsolid, BITCODE_BL num,
                                 int *error);

char
dwg_ent_3dsolid_get_isoline_present(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_isoline_present(dwg_ent_3dsolid *_3dsolid, char iso,
                                    int *error);

BITCODE_BL
dwg_ent_3dsolid_get_num_wires(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_num_wires(dwg_ent_3dsolid *_3dsolid, BITCODE_BL num, int *error);

dwg_ent_solid_wire *
dwg_ent_3dsolid_get_wire(dwg_ent_3dsolid *_3dsolid, int *error);

BITCODE_BL
dwg_ent_3dsolid_get_num_silhouettes(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_num_silhouettes(dwg_ent_3dsolid *_3dsolid,
                                    BITCODE_BL silhouettes, int *error);

dwg_ent_solid_silhouette *
dwg_ent_3dsolid_get_silhouette(dwg_ent_3dsolid *_3dsolid, int *error);

unsigned char
dwg_ent_3dsolid_get_acis_empty2(dwg_ent_3dsolid *_3dsolid, int *error);

void
dwg_ent_3dsolid_set_acis_empty2(dwg_ent_3dsolid *_3dsolid, unsigned char acis,
                                int *error); 


/********************************************************************
*                   FUNCTIONS FOR REGION ENTITY                     *
********************************************************************/


unsigned char
dwg_ent_region_get_acis_empty(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_acis_empty(dwg_ent_region *region, unsigned char acis,
                              int *error);

BITCODE_BS
dwg_ent_region_get_version(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_version(dwg_ent_region *region, BITCODE_BS version,
                           int *error);

BITCODE_BL*
dwg_ent_region_get_block_size(dwg_ent_region *region, int *error);

char *
dwg_ent_region_get_acis_data(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_acis_data(dwg_ent_region *region, char *data, int *error);

char
dwg_ent_region_get_wireframe_data_present(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_wireframe_data_present(dwg_ent_region *region, char data,
                                          int *error);

char
dwg_ent_region_get_point_present(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_point_present(dwg_ent_region *region, char point,
                                 int *error);

void
dwg_ent_region_get_point(dwg_ent_region *region, dwg_point_3d *point,
                         int *error);

void
dwg_ent_region_set_point(dwg_ent_region *region, dwg_point_3d *point,
                         int *error);

BITCODE_BL
dwg_ent_region_get_num_isolines(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_num_isolines(dwg_ent_region *region, BITCODE_BL num, int *error);

char
dwg_ent_region_get_isoline_present(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_isoline_present(dwg_ent_region *region, char iso,
                                   int *error);

BITCODE_BL
dwg_ent_region_get_num_wires(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_num_wires(dwg_ent_region *region, BITCODE_BL num, int *error);

dwg_ent_solid_wire *
dwg_ent_region_get_wire(dwg_ent_region *region, int *error);

BITCODE_BL
dwg_ent_region_get_num_silhouettes(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_num_silhouettes(dwg_ent_region *region, BITCODE_BL silhouettes,
                                   int *error);

dwg_ent_solid_silhouette *
dwg_ent_region_get_silhouette(dwg_ent_region *region, int *error);

unsigned char
dwg_ent_region_get_acis_empty2(dwg_ent_region *region, int *error);

void
dwg_ent_region_set_acis_empty2(dwg_ent_region *region, unsigned char acis,
                               int *error); 


/********************************************************************
*                    FUNCTIONS FOR BODY ENTITY                      *
********************************************************************/


unsigned char
dwg_ent_body_get_acis_empty(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_acis_empty(dwg_ent_body *body, unsigned char acis,
                            int *error);

BITCODE_BS
dwg_ent_body_get_version(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_version(dwg_ent_body *body, BITCODE_BS version, int *error);

BITCODE_BL*
dwg_ent_body_get_block_size(dwg_ent_body *body, int *error);

char *
dwg_ent_body_get_acis_data(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_acis_data(dwg_ent_body *body, char *data, int *error);

char
dwg_ent_body_get_wireframe_data_present(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_wireframe_data_present(dwg_ent_body *body, char data,
                                        int *error);

char
dwg_ent_body_get_point_present(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_point_present(dwg_ent_body *body, char point, int *error);

void
dwg_ent_body_get_point(dwg_ent_body *body, dwg_point_3d *point, int *error);

void
dwg_ent_body_set_point(dwg_ent_body *body, dwg_point_3d *point, int *error);

BITCODE_BL
dwg_ent_body_get_num_isolines(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_num_isolines(dwg_ent_body *body, BITCODE_BL num, int *error);

char
dwg_ent_body_get_isoline_present(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_isoline_present(dwg_ent_body *body, char iso, int *error);

BITCODE_BL
dwg_ent_body_get_num_wires(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_num_wires(dwg_ent_body *body, BITCODE_BL num, int *error);

dwg_ent_solid_wire *
dwg_ent_body_get_wire(dwg_ent_body *body, int *error);

BITCODE_BL
dwg_ent_body_get_num_silhouettes(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_num_silhouettes(dwg_ent_body *body, BITCODE_BL silhouettes,
                                 int *error);

dwg_ent_solid_silhouette *
dwg_ent_body_get_silhouette(dwg_ent_body *body, int *error);

unsigned char
dwg_ent_body_get_acis_empty2(dwg_ent_body *body, int *error);

void
dwg_ent_body_set_acis_empty2(dwg_ent_body *body, unsigned char acis,
                             int *error); 


/********************************************************************
*                    FUNCTIONS FOR TABLE ENTITY                     *
********************************************************************/


void
dwg_ent_table_set_insertion_point(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_get_insertion_point(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_set_scale(dwg_ent_table *table, dwg_point_3d *point,
                        int *error);

void
dwg_ent_table_get_scale(dwg_ent_table *table, dwg_point_3d *point,
                        int *error);

void
dwg_ent_table_set_data_flags(dwg_ent_table *table, unsigned char flags,
                             int *error);

unsigned char
dwg_ent_table_get_data_flags(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_rotation(dwg_ent_table *table, BITCODE_BD rotation, int *error);

BITCODE_BD
dwg_ent_table_get_rotation(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_extrusion(dwg_ent_table *table, dwg_point_3d *point,
                            int *error);

void
dwg_ent_table_get_extrusion(dwg_ent_table *table, dwg_point_3d *point,
                            int *error);

unsigned char
dwg_ent_table_has_attribs(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_owned_object_count(dwg_ent_table *table, BITCODE_BL count,
                                     int *error);

BITCODE_BL
dwg_ent_table_get_owned_object_count(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_flag_for_table_value(dwg_ent_table *table,
                                       BITCODE_BS value, int *error);

BITCODE_BS
dwg_ent_table_get_flag_for_table_value(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_horiz_direction(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_get_horiz_direction(dwg_ent_table *table, dwg_point_3d *point,
                                  int *error);

void
dwg_ent_table_set_num_cols(dwg_ent_table *table, BITCODE_BL num, int *error);

BITCODE_BL
dwg_ent_table_get_num_cols(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_num_rows(dwg_ent_table *table, BITCODE_BL num, int *error);

BITCODE_BL
dwg_ent_table_get_num_rows(dwg_ent_table *table, int *error);

double *
dwg_ent_table_get_col_widths(dwg_ent_table *table, int *error);

double *
dwg_ent_table_get_row_heights(dwg_ent_table *table, int *error);

BITCODE_B
dwg_ent_table_has_table_overrides(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_table_flag_override(dwg_ent_table *table, BITCODE_BL override,
                                      int *error);

BITCODE_BL
dwg_ent_table_get_table_flag_override(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_suppressed(dwg_ent_table *table, unsigned char title,
                                   int *error);

unsigned char
dwg_ent_table_get_title_suppressed(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_suppressed(dwg_ent_table *table, unsigned char header,
                                    int *error);

unsigned char
dwg_ent_table_get_header_suppressed(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_flow_direction(dwg_ent_table *table, BITCODE_BS dir,
                                 int *error);

BITCODE_BS
dwg_ent_table_get_flow_direction(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_horiz_cell_margin(dwg_ent_table *table, BITCODE_BD margin,
                                    int *error);

BITCODE_BD
dwg_ent_table_get_horiz_cell_margin(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_vert_cell_margin(dwg_ent_table *table, BITCODE_BD margin,
                                   int *error);

BITCODE_BD
dwg_ent_table_get_vert_cell_margin(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_row_fill_none(dwg_ent_table *table, unsigned char fill,
                                      int *error);

unsigned char
dwg_ent_table_get_title_row_fill_none(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_row_fill_none(dwg_ent_table *table, 
                                       unsigned char fill, int *error);

unsigned char
dwg_ent_table_get_header_row_fill_none(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_row_fill_none(dwg_ent_table *table, unsigned char fill,
                                     int *error);

unsigned char
dwg_ent_table_get_data_row_fill_none(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_row_align(dwg_ent_table *table, unsigned char fill,
                                  int *error);

BITCODE_BS
dwg_ent_table_get_title_row_align(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_row_align(dwg_ent_table *table, BITCODE_BS align,
                                   int *error);

BITCODE_BS
dwg_ent_table_get_header_row_align(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_row_align(dwg_ent_table *table, BITCODE_BS align,
                                 int *error);

BITCODE_BS
dwg_ent_table_get_data_row_align(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_row_height(dwg_ent_table *table, BITCODE_BD height,
                                   int *error);

BITCODE_BD
dwg_ent_table_get_title_row_height(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_row_height(dwg_ent_table *table, BITCODE_BD height,
                                    int *error);

BITCODE_BD
dwg_ent_table_get_header_row_height(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_row_height(dwg_ent_table *table, BITCODE_BD height,
                                  int *error);

BITCODE_BD
dwg_ent_table_get_data_row_height(dwg_ent_table *table, int *error);

unsigned char
dwg_ent_table_has_border_color_overrides(dwg_ent_table *table,
                                         int *error);

void
dwg_ent_table_set_border_color_overrides_flag(dwg_ent_table *table,
                                              BITCODE_BL overrides, int *error);

BITCODE_BL
dwg_ent_table_get_border_color_overrides_flag(dwg_ent_table *table,
                                              int *error);

unsigned char
dwg_ent_table_has_border_lineweight_overrides(dwg_ent_table *table,
                                                      int *error);

void
dwg_ent_table_set_border_lineweight_overrides_flag(dwg_ent_table *table,
                                                   BITCODE_BL overrides, int *error);

BITCODE_BL
dwg_ent_table_get_border_lineweight_overrides_flag(dwg_ent_table *table,
                                                   int *error);

void
dwg_ent_table_set_title_horiz_top_linewt(dwg_ent_table *table,
                                             BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_title_horiz_top_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_ins_linewt(dwg_ent_table *table,
                                             BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_title_horiz_ins_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_bottom_linewt(dwg_ent_table *table,
                                                BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_linewt(dwg_ent_table *table,
                                                int *error);

void
dwg_ent_table_set_title_vert_left_linewt(dwg_ent_table *table,
                                             BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_title_vert_left_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_ins_linewt(dwg_ent_table *table,
                                            BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_title_vert_ins_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_right_linewt(dwg_ent_table *table,
                                              BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_title_vert_right_linewt(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_horiz_top_linewt(dwg_ent_table *table,
                                              BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_header_horiz_top_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_horiz_ins_linewt(dwg_ent_table *table,
                                              BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_header_horiz_ins_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_horiz_bottom_linewt(dwg_ent_table *table,
                                                 BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_linewt(dwg_ent_table *table,
                                                 int *error);

void
dwg_ent_table_set_header_vert_left_linewt(dwg_ent_table *table,
                                              BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_header_vert_left_linewt(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_vert_ins_linewt(dwg_ent_table *table,
                                             BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_header_vert_ins_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_vert_right_linewt(dwg_ent_table *table,
                                               BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_header_vert_right_linewt(dwg_ent_table *table,
                                               int *error);

void
dwg_ent_table_set_data_horiz_top_linewt(dwg_ent_table *table,
                                            BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_data_horiz_top_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_ins_linewt(dwg_ent_table *table,
                                            BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_data_horiz_ins_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_bottom_linewt(dwg_ent_table *table,
                                               BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_linewt(dwg_ent_table *table,
                                               int *error);

void
dwg_ent_table_set_data_vert_left_linewt(dwg_ent_table *table,
                                            BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_data_vert_left_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_vert_ins_linewt(dwg_ent_table *table,
                                           BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_data_vert_ins_linewt(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_vert_right_linewt(dwg_ent_table *table,
                                             BITCODE_BS lw, int *error);

BITCODE_BS
dwg_ent_table_get_data_vert_right_linewt(dwg_ent_table *table, int *error);

unsigned char
dwg_ent_table_has_border_visibility_overrides(dwg_ent_table *table,
                                                      int *error);

void
dwg_ent_table_set_border_visibility_overrides_flag(dwg_ent_table *table,
                                                   BITCODE_BL overrides, int *error);

BITCODE_BL
dwg_ent_table_get_border_visibility_overrides_flag(dwg_ent_table *table,
                                                   int *error);

void
dwg_ent_table_set_title_horiz_top_visibility(dwg_ent_table *table,
                                             BITCODE_BS visibility,
                                             int *error);

BITCODE_BS
dwg_ent_table_get_title_horiz_top_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_ins_visibility(dwg_ent_table *table,
                                             BITCODE_BS visibility,
                                             int *error);

BITCODE_BS
dwg_ent_table_get_title_horiz_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_horiz_bottom_visibility(dwg_ent_table *table,
                                                BITCODE_BS visibility,
                                                int *error);

BITCODE_BS
dwg_ent_table_get_title_horiz_bottom_visibility(dwg_ent_table *table,
                                                int *error);

void
dwg_ent_table_set_title_vert_left_visibility(dwg_ent_table *table,
                                             BITCODE_BS visibility,
                                             int *error);

BITCODE_BS
dwg_ent_table_get_title_vert_left_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_ins_visibility(dwg_ent_table *table,
                                            BITCODE_BS visibility,
                                            int *error);

BITCODE_BS
dwg_ent_table_get_title_vert_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_title_vert_right_visibility(dwg_ent_table *table,
                                              BITCODE_BS visibility,
                                              int *error);

BITCODE_BS
dwg_ent_table_get_title_vert_right_visibility(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_horiz_top_visibility(dwg_ent_table *table,
                                              BITCODE_BS visibility,
                                              int *error);

BITCODE_BS
dwg_ent_table_get_header_horiz_top_visibility(dwg_ent_table *table,
                                              int *error);

BITCODE_BS
dwg_ent_table_get_header_vert_left_visibility(dwg_ent_table *table,
                                              int *error);
void
dwg_ent_table_set_header_horiz_ins_visibility(dwg_ent_table *table,
                                              BITCODE_BS visibility,
                                              int *error);

BITCODE_BS
dwg_ent_table_get_header_horiz_ins_visibility(dwg_ent_table *table,
                                              int *error);

void
dwg_ent_table_set_header_horiz_bottom_visibility(dwg_ent_table *table,
                                                 BITCODE_BS visibility,
                                                 int *error);

BITCODE_BS
dwg_ent_table_get_header_horiz_bottom_visibility(dwg_ent_table *table,
                                                 int *error);

void
dwg_ent_table_set_header_vert_ins_visibility(dwg_ent_table *table,
                                             BITCODE_BS visibility,
                                             int *error);

BITCODE_BS
dwg_ent_table_get_header_vert_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_header_vert_right_visibility(dwg_ent_table *table,
                                               BITCODE_BS visibility,
                                               int *error);

BITCODE_BS
dwg_ent_table_get_header_vert_right_visibility(dwg_ent_table *table,
                                               int *error);


void
dwg_ent_table_set_data_horiz_top_visibility(dwg_ent_table *table,
                                            BITCODE_BS visibility,
                                            int *error);

BITCODE_BS
dwg_ent_table_get_data_horiz_top_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_ins_visibility(dwg_ent_table *table,
                                            BITCODE_BS visibility,
                                            int *error);

BITCODE_BS
dwg_ent_table_get_data_horiz_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_horiz_bottom_visibility(dwg_ent_table *table,
                                               BITCODE_BS visibility,
                                               int *error);

BITCODE_BS
dwg_ent_table_get_data_horiz_bottom_visibility(dwg_ent_table *table,
                                               int *error);

void
dwg_ent_table_set_data_vert_ins_visibility(dwg_ent_table *table,
                                           BITCODE_BS visibility,
                                           int *error);

BITCODE_BS
dwg_ent_table_get_data_vert_ins_visibility(dwg_ent_table *table, int *error);

void
dwg_ent_table_set_data_vert_right_visibility(dwg_ent_table *table,
                                             BITCODE_BS visibility,
                                             int *error);

BITCODE_BS
dwg_ent_table_get_data_vert_right_visibility(dwg_ent_table *table, int *error);


/********************************************************************
*                    FUNCTIONS FOR LAYER OBJECT                     *
********************************************************************/


// Get Layer Name
char *
dwg_obj_layer_get_name(dwg_obj_layer *layer, int *error);


/********************************************************************
*              FUNCTIONS FOR VERTEX_PFACE_FACE ENTITY               *
********************************************************************/


// Get/Set vertind of a vertex_pface_face entity
BITCODE_BS
dwg_ent_vertex_pface_face_get_vertind(dwg_ent_vert_pface_face *face);

void
dwg_ent_vertex_pface_face_set_vertind(dwg_ent_vert_pface_face *face,
                                      BITCODE_BS vertind[4]);


/********************************************************************
*                FUNCTIONS FOR BLOCK_HEADER OBJECT                  *
********************************************************************/


/* Get Block Name of the block header type argument passed in function
Usage :- char *block_name = dwg_obj_block_header_get_name(hdr);
*/
char *
dwg_obj_block_header_get_name(dwg_obj_block_header *hdr, int *error);

dwg_obj_block_header *
dwg_get_block_header(dwg_data *dwg, int *error);


/********************************************************************
*               FUNCTIONS FOR BLOCK_CONTROL OBJECT                  *
********************************************************************/


BITCODE_BL
dwg_obj_block_control_get_num_entries(dwg_obj_block_control *ctrl, int *error);

dwg_object_ref **
dwg_obj_block_control_get_block_headers(dwg_obj_block_control *ctrl,
                                        int *error);

dwg_obj_block_control *
dwg_block_header_get_block_control(dwg_obj_block_header* block_header,
                                   int *error);

dwg_object_ref *
dwg_obj_block_control_get_model_space(dwg_obj_block_control *ctrl, int *error);

dwg_object_ref *
dwg_obj_block_control_get_paper_space(dwg_obj_block_control *ctrl, int *error);


/********************************************************************
*                    FUNCTIONS FOR XRECORD OBJECT                     *
********************************************************************/

BITCODE_BL
dwg_obj_xrecord_get_num_databytes(dwg_obj_xrecord *xrecord, int *error);
void
dwg_obj_xrecord_set_num_databytes(dwg_obj_xrecord *xrecord, BITCODE_BL num_databytes,
                                  int *error);

BITCODE_BS
dwg_obj_xrecord_get_cloning_flags(dwg_obj_xrecord *xrecord, int *error);
void
dwg_obj_xrecord_set_cloning_flags(dwg_obj_xrecord *xrecord, BITCODE_BS cloning_flags,
                                  int *error);
BITCODE_BL
dwg_obj_xrecord_get_num_eed(dwg_obj_xrecord *xrecord, int *error);
void
dwg_obj_xrecord_set_num_eed(dwg_obj_xrecord *xrecord, BITCODE_BL num_eed,
                            int *error);

//Dwg_Eed*
//dwg_obj_xrecord_get_eed(dwg_obj_xrecord *xrecord, BITCODE_BL index, int *error);

Dwg_Resbuf*
dwg_obj_xrecord_get_xdata(dwg_obj_xrecord *xrecord, int *error);
void
dwg_obj_xrecord_set_xdata(dwg_obj_xrecord *xrecord, Dwg_Resbuf* xdata, int *error);

dwg_object_ref*
dwg_obj_xrecord_get_parenthandle(dwg_obj_xrecord *xrecord, int *error);
dwg_object_ref**
dwg_obj_xrecord_get_reactors(dwg_obj_xrecord *xrecord, int *error);

BITCODE_BL
dwg_obj_xrecord_get_num_objid_handles(dwg_obj_xrecord *xrecord, int *error);

dwg_object_ref**
dwg_obj_xrecord_get_objid_handles(dwg_obj_xrecord *xrecord, int *error);


/********************************************************************
*                    COMMON FUNCTIONS FOR DWG ENTITY                *
********************************************************************/

BITCODE_RL
dwg_ent_get_bitsize(dwg_obj_ent *ent, int *error);

unsigned int
dwg_ent_get_num_eed(dwg_obj_ent *ent, int *error);

dwg_entity_eed *
dwg_ent_get_eed(dwg_obj_ent *ent, unsigned int index, int *error);

dwg_entity_eed_data *
dwg_ent_get_eed_data(dwg_obj_ent *ent, unsigned int index, int *error);

BITCODE_B
dwg_ent_get_picture_exists(dwg_obj_ent *ent, int *error);

BITCODE_BLL
dwg_ent_get_picture_size(dwg_obj_ent *ent, int *error); // before r2007 only RL

BITCODE_RC *
dwg_ent_get_picture(dwg_obj_ent *ent, int *error);

BITCODE_BB
dwg_ent_get_entity_mode(dwg_obj_ent *ent, int *error);

BITCODE_BL
dwg_ent_get_num_reactors(dwg_obj_ent *ent, int *error);

BITCODE_B
dwg_ent_get_xdic_missing_flag(dwg_obj_ent *ent, int *error); //r2004+

BITCODE_B
dwg_ent_get_isbylayerlt(dwg_obj_ent *ent, int *error); //r13-r14 only

BITCODE_B
dwg_ent_get_nolinks(dwg_obj_ent *ent, int *error);

BITCODE_CMC
dwg_ent_get_color(dwg_obj_ent *ent, int *error);

BITCODE_BD
dwg_ent_get_linetype_scale(dwg_obj_ent *ent, int *error);

BITCODE_BB
dwg_ent_get_linetype_flag(dwg_obj_ent *ent, int *error); //r2000+

BITCODE_BB
dwg_ent_get_plotstyle_flag(dwg_obj_ent *ent, int *error); //r2000+

BITCODE_BB
dwg_ent_get_material_flag(dwg_obj_ent *ent, int *error); //r2007+

BITCODE_RC
dwg_ent_get_shadow_flag(dwg_obj_ent *ent, int *error); //r2007+

BITCODE_B
dwg_ent_has_full_visualstyle(dwg_obj_ent *ent, int *error); //r2010+

BITCODE_B
dwg_ent_has_face_visualstyle(dwg_obj_ent *ent, int *error); //r2010+

BITCODE_B
dwg_ent_has_edge_visualstyle(dwg_obj_ent *ent, int *error); //r2010+

BITCODE_BS
dwg_ent_get_invisible(dwg_obj_ent *ent, int *error);

BITCODE_RC
dwg_ent_get_lineweight(dwg_obj_ent *ent, int *error); //r2000+

/*unsigned int
dwg_ent_get_num_handles(dwg_obj_ent *ent, int *error);*/

//TODO: dwg_object_ref* or dwg_handle*, not handle
BITCODE_H
dwg_ent_get_subentity(dwg_obj_ent *ent, int *error);

BITCODE_H*
dwg_ent_get_reactors(dwg_obj_ent *ent, int *error);

BITCODE_H
dwg_ent_get_xdicobjhandle(dwg_obj_ent *ent, int *error);

BITCODE_H
dwg_ent_get_prev_entity(dwg_obj_ent *ent, int *error);  //r13-r2000

BITCODE_H
dwg_ent_get_next_entity(dwg_obj_ent *ent, int *error);  //r13-r2000

BITCODE_H
dwg_ent_get_color_handle(dwg_obj_ent *ent, int *error); //r2004+

BITCODE_H
dwg_ent_get_layer(dwg_obj_ent *ent, int *error);

BITCODE_H
dwg_ent_get_ltype(dwg_obj_ent *ent, int *error);

BITCODE_H
dwg_ent_get_material(dwg_obj_ent *ent, int *error);     //r2007+

BITCODE_H
dwg_ent_get_plotstyle(dwg_obj_ent *ent, int *error);    //r2000+

BITCODE_H
dwg_ent_get_full_visualstyle(dwg_obj_ent *ent, int *error); //r2010+

BITCODE_H
dwg_ent_get_face_visualstyle(dwg_obj_ent *ent, int *error); //r2010+

BITCODE_H
dwg_ent_get_edge_visualstyle(dwg_obj_ent *ent, int *error); //r2010+

/********************************************************************
*                    FUNCTIONS FOR DWG OBJECT                       *
********************************************************************/

unsigned int
dwg_get_num_classes(dwg_data *dwg);

dwg_class *
dwg_get_class(dwg_data *dwg, unsigned int index);

long unsigned int
dwg_get_num_objects(dwg_data *dwg);
long unsigned int
dwg_get_num_entities(dwg_data *dwg);

dwg_object *
dwg_get_object(dwg_data *dwg, long unsigned int index);

BITCODE_RL
dwg_obj_get_bitsize(dwg_object *obj);

unsigned int
dwg_obj_get_num_eed(dwg_obj_obj *obj, int *error);

dwg_entity_eed *
dwg_obj_get_eed(dwg_obj_obj *obj, int index, int *error);

dwg_entity_eed_data *
dwg_obj_get_eed_data(dwg_obj_obj *obj, int index, int *error);

BITCODE_B
dwg_obj_get_picture_exists(dwg_object *obj, int *error);

BITCODE_BLL
dwg_obj_get_picture_size(dwg_object *obj, int *error); // before r2007 only RL

BITCODE_RC *
dwg_obj_get_picture(dwg_object *obj, int *error);

BITCODE_BB
dwg_obj_get_entity_mode(dwg_object *obj, int *error);

BITCODE_BL
dwg_obj_get_num_reactors(dwg_object *obj, int *error);

BITCODE_B
dwg_obj_get_xdic_missing_flag(dwg_object *obj, int *error); //r2004+

int 
dwg_obj_object_get_index(dwg_object *obj, int *error);

dwg_handle *
dwg_obj_get_handle(dwg_object *obj, int *error);

BITCODE_BL
dwg_obj_ref_get_abs_ref(dwg_object_ref *ref, int *error);

dwg_handle *
dwg_ent_insert_get_ref_handle(dwg_ent_insert *insert, int *error);

BITCODE_BL
dwg_ent_insert_get_abs_ref(dwg_ent_insert *insert, int *error);

dwg_object *
dwg_obj_reference_get_object(dwg_object_ref *ref, int *error);

dwg_obj_obj *
dwg_object_to_object(dwg_object *obj, int *error);

dwg_obj_ent *
dwg_object_to_entity(dwg_object *obj, int *error);

int
dwg_get_type(dwg_object *obj);

char*
dwg_get_dxfname(dwg_object *obj);

#ifdef __cplusplus
}
#endif

#endif

/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2013,2018-2020 Free Software Foundation, Inc.              */
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
#include <stdbool.h>
#include "dwg.h"

/* Since version 3.2, gcc allows marking deprecated functions.  */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 302)
#  define __attribute_deprecated__ __attribute__ ((__deprecated__))
#else
#  define __attribute_deprecated__
#endif

#if defined(__clang__) && defined(__has_extension)
#  define _clang_has_extension(ext) __has_extension (ext)
#else
#  define _clang_has_extension(ext) 0
#endif

/* Since version 4.5, gcc also allows one to specify the message printed
   when a deprecated function is used.
   clang claims to be gcc 4.2, but may also support this feature.
   icc (at least 12) not.
   glibc 2.28 /usr/include/sys/cdefs.h is wrong about icc compat. */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 405          \
     && !defined(__ICC))                                                      \
    || _clang_has_extension(attribute_deprecated_with_message)
#  undef __attribute_deprecated_msg
#  define __attribute_deprecated_msg__(msg)                                   \
    __attribute__ ((__deprecated__ (msg)))
#else
#  define __attribute_deprecated_msg__(msg) __attribute_deprecated__
#endif

/* The __nonnull function attribute marks pointer arguments which
   must not be NULL.  */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 303)
#  undef __nonnull
#  define __nonnull(params) __attribute__ ((__nonnull__ params))
#  define HAVE_NONNULL
#else
#  undef HAVE_NONNULL
#  define __nonnull(params)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOG_ERROR
# define LOG_ERROR(msg,name,type) \
   fprintf(stderr, msg, name, (type))
#endif

/** dynapi */
typedef struct dwg_field_name_type_offset
{
  const char *const name; /* field name */
  const char *const type; /* e.g "RS" for BITCODE_RS */
  const unsigned short size;        /* e.g. 2 for RS, 4 for BL */
  const unsigned short offset;
  const unsigned short is_indirect:1;  // for pointers, references, like 3BD, CMC, H, TV
  const unsigned short is_malloc:1;    // for strings and dynamic arrays only, H*, TV, unknown size
  const unsigned short is_string:1;    // for null-terminated strings, use strcpy/wcscpy. not memcpy
  const short dxf;
} Dwg_DYNAPI_field;

/** Check if the name is a valid ENTITY name, not an OBJECT.
 */
EXPORT bool is_dwg_entity (const char *name) __nonnull ((1));

/** Check if the name is a valid OBJECT name, not an ENTITY.
 */
EXPORT bool is_dwg_object (const char *name) __nonnull ((1));

/** Returns the HEADER.fieldname value in out.
    The optional Dwg_DYNAPI_field *fp is filled with the field types from
   dynapi.c
 */
EXPORT bool dwg_dynapi_header_value (const Dwg_Data *restrict dwg,
                                     const char *restrict fieldname,
                                     void *restrict out,
                                     Dwg_DYNAPI_field *restrict fp)
  __nonnull ((1, 2, 3));

/** Returns the ENTITY|OBJECT.fieldname value in out.
   entity is the Dwg_Entity_ENTITY or Dwg_Object_OBJECT struct with the
   specific fields. The optional Dwg_DYNAPI_field *fp is filled with the field
   types from dynapi.c.
 */
EXPORT bool dwg_dynapi_entity_value (void *restrict entity,
                                     const char *restrict dxfname,
                                     const char *restrict fieldname,
                                     void *restrict out,
                                     Dwg_DYNAPI_field *restrict fp)
  __nonnull ((1, 2, 3, 4));

/** Returns the common ENTITY|OBJECT.fieldname value in out.
   _obj is the Dwg_Entity_ENTITY or Dwg_Object_OBJECT struct with the
   specific fields. The optional Dwg_DYNAPI_field *fp is filled with the field
   types from dynapi.c
 */
EXPORT bool dwg_dynapi_common_value (void *restrict _obj,
                                     const char *restrict fieldname,
                                     void *restrict out,
                                     Dwg_DYNAPI_field *restrict fp)
  __nonnull ((1, 2, 3));

/** Returns the common OBJECT.subclass.fieldname value in out.
    ptr points to the subclass field. The optional Dwg_DYNAPI_field *fp is filled with the field
    types from dynapi.c
 */
EXPORT bool
dwg_dynapi_subclass_value (const void *restrict ptr, const char *restrict subclass,
                           const char *restrict fieldname, void *restrict out,
                           Dwg_DYNAPI_field *restrict fp)
    __nonnull ((1, 2, 3, 4));

// Converts T or TU wide-strings to utf-8. Only for text values
// isnew is set to 1 if textp is freshly malloced (r2007+), otherwise 0
EXPORT bool dwg_dynapi_header_utf8text (const Dwg_Data *restrict dwg,
                                        const char *restrict fieldname,
                                        char **restrict textp, int *isnewp,
                                        Dwg_DYNAPI_field *restrict fp)
  __nonnull ((1, 2, 3));

/** Returns the ENTITY|OBJECT.fieldname text value in textp as utf-8.
   entity is the Dwg_Entity_ENTITY or Dwg_Object_OBJECT struct with the
   specific fields. The optional Dwg_DYNAPI_field *fp is filled with the field
   types from dynapi.c
   With DWG's since r2007+ creates a fresh UTF-8 conversion from the UTF-16
   wchar value (which needs to be free'd), with older DWG's or with TV, TF or TFF
   returns the unconverted text value.
   Only valid for text fields.
   isnew is set to 1 if textp is freshly malloced (r2007+), otherwise 0
*/
EXPORT bool dwg_dynapi_entity_utf8text (void *restrict entity,
                                        const char *restrict name,
                                        const char *restrict fieldname,
                                        char **restrict textp, int *isnewp,
                                        Dwg_DYNAPI_field *restrict fp)
  __nonnull ((1, 2, 3, 4));
EXPORT bool dwg_dynapi_common_utf8text (void *restrict _obj,
                                        const char *restrict fieldname,
                                        char **restrict textp, int *isnewp,
                                        Dwg_DYNAPI_field *restrict fp)
  __nonnull ((1, 2, 3));

/** Sets the HEADER.fieldname to a value.
    A malloc'ed struct or string is passed by ptr, not by the content.
    A non-malloc'ed struct is set by content.
    If is_utf8 is set, the given value is a UTF-8 string, and will be
    converted to TV or TU.
 */
EXPORT bool dwg_dynapi_header_set_value (const Dwg_Data *restrict dwg,
                                         const char *restrict fieldname,
                                         const void *restrict value,
                                         const bool is_utf8)
  __nonnull ((1, 2, 3));

/** Sets the ENTITY.fieldname to a value.
    A malloc'ed struct is passed by ptr, not by the content.
    A non-malloc'ed struct is set by content.
    Arrays or strings must be malloced before. We just set the new pointer,
    the old value will be freed.
    If is_utf8 is set, the given value is a UTF-8 string, and will be
    converted to TV or TU.
 */
EXPORT bool dwg_dynapi_entity_set_value (void *restrict entity,
                                         const char *restrict dxfname,
                                         const char *restrict fieldname,
                                         const void *restrict value,
                                         const bool is_utf8)
  __nonnull ((1, 2, 3, 4));

/** Sets the common ENTITY or OBJECT.fieldname to a value.
    A malloc'ed struct is passed by ptr, not by the content.
    A non-malloc'ed struct is set by content.
    Arrays or strings must be malloced before. We just set the new pointer,
    the old value will be freed.
    If is_utf8 is set, the given value is a UTF-8 string, and will be
    converted to TV or TU.
 */
EXPORT bool dwg_dynapi_common_set_value (void *restrict _obj,
                                         const char *restrict fieldname,
                                         const void *restrict value,
                                         const bool is_utf8)
  __nonnull ((1, 2, 3));

/** Checks if the handle hdl points an object or entity with a name field,
    and returns it if so. Converted to UTF8 for r2007+ wide-strings. */
EXPORT char *dwg_dynapi_handle_name (const Dwg_Data *restrict dwg,
                                     Dwg_Object_Ref *restrict hdl)
    __nonnull ((1, 2));

/** Return the field for custom type checks. */
EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_header_field (const char *restrict fieldname) __nonnull ((1));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_entity_field (const char *restrict name,
                         const char *restrict fieldname) __nonnull ((1, 2));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_subclass_field (const char *restrict name,
                           const char *restrict fieldname) __nonnull ((1, 2));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_common_entity_field (const char *restrict fieldname)
    __nonnull ((1));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_common_object_field (const char *restrict fieldname)
    __nonnull ((1));

/** Find the fields for this entity or object. */
EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_entity_fields (const char *restrict name) __nonnull ((1));

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_common_entity_fields (void);

EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_common_object_fields (void);

/** Find the fields for this subclass. See dwg.h */
EXPORT const Dwg_DYNAPI_field *
dwg_dynapi_subclass_fields (const char *restrict name) __nonnull ((1));

/** The sum of the size of all fields, by entity or subclass name */
EXPORT int dwg_dynapi_fields_size (const char *restrict name) __nonnull ((1));

/* static api */
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

/* Returns a NULL-terminated array of all entities of a specific type from a BLOCK */
#define DWG_GETALL_ENTITY_DECL(token) \
EXPORT \
Dwg_Entity_##token **dwg_getall_##token (Dwg_Object_Ref * hdr);

/* Checks now also variable classes */
#define DWG_GETALL_ENTITY(token) \
EXPORT \
Dwg_Entity_##token **dwg_getall_##token (Dwg_Object_Ref * hdr) \
{ \
  int i=0, counts=0; \
  Dwg_Entity_##token ** ret_##token; \
  Dwg_Object * obj; \
  if (!hdr || !hdr->obj) \
    return NULL; \
  obj = get_first_owned_entity (hdr->obj); \
  while (obj) \
    { \
      if (obj->type == DWG_TYPE_##token || obj->fixedtype == DWG_TYPE_##token) \
        counts++; \
      obj = get_next_owned_entity (hdr->obj, obj); \
    } \
  if (!counts) \
    return NULL; \
  ret_##token = (Dwg_Entity_##token **)malloc ((counts + 1) * sizeof(Dwg_Entity_##token *)); \
  obj = get_first_owned_entity (hdr->obj); \
  while (obj) \
    { \
      if (obj->type == DWG_TYPE_##token || obj->fixedtype == DWG_TYPE_##token) \
        { \
          ret_##token[i] = obj->tio.entity->tio.token; \
          i++; \
        } \
        obj = get_next_owned_entity (hdr->obj, obj); \
    } \
  ret_##token[i] = NULL; \
  return ret_##token; \
}

/* Returns a NULL-terminated array of all objects of a specific type */
#define DWG_GETALL_OBJECT_DECL(token) \
EXPORT \
Dwg_Object_##token **dwg_getall_##token (Dwg_Data *dwg);

#define DWG_GETALL_OBJECT(token) \
EXPORT \
Dwg_Object_##token **dwg_getall_##token (Dwg_Data *dwg) \
{ \
  BITCODE_BL i, counts=0; \
  Dwg_Object_##token ** ret_##token; \
  for (i=0; i < dwg->num_objects; i++) \
    { \
      if (dwg->object[i].supertype == DWG_SUPERTYPE_OBJECT \
          && (dwg->object[i].type == DWG_TYPE_##token \
              || dwg->object[i].fixedtype == DWG_TYPE_##token)) \
        { \
          counts++; \
        } \
    } \
  if (!counts) \
    return NULL; \
  ret_##token = (Dwg_Object_##token **)malloc ((counts + 1) * sizeof(Dwg_Object_##token *)); \
  for (i=0; i < dwg->num_objects; i++) \
    { \
      const Dwg_Object *const obj = &dwg->object[i]; \
      if (obj->supertype == DWG_SUPERTYPE_OBJECT \
          && (obj->type == DWG_TYPE_##token || obj->fixedtype == DWG_TYPE_##token)) \
        { \
          ret_##token[i] = obj->tio.object->tio.token; \
          i++; \
        } \
    } \
  ret_##token[i] = NULL; \
  return ret_##token; \
}

// Cast a Dwg_Object to Entity
#define CAST_DWG_OBJECT_TO_ENTITY_DECL(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj);
#define CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj);

// Only for fixed typed entities, < 500
// Dwg_Entity* -> Dwg_Object_TYPE*
#define CAST_DWG_OBJECT_TO_ENTITY(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj) \
{                                                      \
  Dwg_Entity_##token *ret_obj = NULL;                  \
  if (obj &&                                           \
      obj->tio.entity &&                               \
      (obj->type == DWG_TYPE_##token ||                \
       obj->fixedtype == DWG_TYPE_##token))            \
    {                                                  \
      ret_obj = obj->tio.entity->tio.token;            \
    }                                                  \
  else                                                 \
    {                                                  \
      loglevel = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
      LOG_ERROR("Invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
    }                                                  \
  return ret_obj;                                      \
}

/// for all classes, types > 500. IMAGE, OLE2FRAME, WIPEOUT
#define CAST_DWG_OBJECT_TO_ENTITY_BYNAME(token) \
EXPORT \
Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj) \
{ \
    Dwg_Entity_##token *ret_obj = NULL; \
    if (obj && obj->tio.entity && \
        (obj->type == DWG_TYPE_##token || obj->fixedtype == DWG_TYPE_##token)) \
      { \
        ret_obj = obj->tio.entity->tio.token; \
      } \
    else \
      { \
        loglevel = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
        LOG_ERROR("Invalid %s type: got %s, 0x%x", #token, \
                  obj ? obj->dxfname : "<no obj>", \
                  obj ? obj->type : 0); \
      } \
  return ret_obj; \
}

#define CAST_DWG_OBJECT_TO_OBJECT_DECL(token) \
EXPORT \
Dwg_Object_##token *dwg_object_to_##token (Dwg_Object *obj);

// Dwg_Object* -> Dwg_Object_TYPE*
#define CAST_DWG_OBJECT_TO_OBJECT(token) \
EXPORT \
Dwg_Object_##token *dwg_object_to_##token (Dwg_Object *obj) \
{ \
    Dwg_Object_##token *ret_obj = NULL; \
    if (obj && obj->tio.object && \
        (obj->type == DWG_TYPE_##token || obj->fixedtype == DWG_TYPE_##token)) \
      { \
        ret_obj = obj->tio.object->tio.token; \
      } \
    else \
      { \
        loglevel = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
        LOG_ERROR("Invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
      } \
  return ret_obj; \
}

// unused, we have now fixedtype.
#define CAST_DWG_OBJECT_TO_OBJECT_BYNAME(token, dxfname) \
EXPORT \
Dwg_Object_##token *dwg_object_to_##token (Dwg_Object *obj) \
{ \
    Dwg_Object_##token *ret_obj = NULL; \
    if (obj && obj->dxfname && !strcmp(obj->dxfname, #dxfname)) \
      { \
        ret_obj = obj->tio.object->tio.token; \
      } \
    else \
      { \
        loglevel = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
        LOG_ERROR("Invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
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
typedef struct _dwg_entity_VERTEX_PFACE_FACE      dwg_ent_vertex_pface_face;
typedef struct _dwg_entity_VERTEX_2D              dwg_ent_vertex_2d;
typedef struct _dwg_DIMENSION_common              dwg_ent_dim;
typedef struct _dwg_entity_DIMENSION_ORDINATE     dwg_ent_dim_ordinate;
typedef struct _dwg_entity_DIMENSION_LINEAR       dwg_ent_dim_linear;
typedef struct _dwg_entity_DIMENSION_ALIGNED      dwg_ent_dim_aligned;
typedef struct _dwg_entity_DIMENSION_ANG3PT       dwg_ent_dim_ang3pt;
typedef struct _dwg_entity_DIMENSION_ANG2LN       dwg_ent_dim_ang2ln;
typedef struct _dwg_entity_DIMENSION_RADIUS       dwg_ent_dim_radius;
typedef struct _dwg_entity_DIMENSION_DIAMETER     dwg_ent_dim_diameter;
typedef struct _dwg_entity_LEADER                 dwg_ent_leader;
typedef struct _dwg_entity_MULTILEADER            dwg_ent_mleader;
typedef struct _dwg_entity_SHAPE                  dwg_ent_shape;
typedef struct _dwg_entity_MTEXT                  dwg_ent_mtext;
typedef struct _dwg_entity_TOLERANCE              dwg_ent_tolerance;
typedef struct _dwg_entity_ENDBLK                 dwg_ent_endblk;
typedef struct _dwg_entity_SEQEND                 dwg_ent_seqend;
typedef struct _dwg_entity_SPLINE                 dwg_ent_spline;
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
typedef struct _dwg_entity_POINT                  dwg_ent_generic;
typedef struct _dwg_entity_CAMERA                 dwg_ent_camera;
typedef struct _dwg_entity_HELIX                  dwg_ent_helix;
typedef struct _dwg_entity_LIGHT                  dwg_ent_light;
typedef struct _dwg_entity_UNDERLAY               dwg_ent_underlay;
typedef struct _dwg_entity_WIPEOUT                dwg_ent_wipeout;
typedef struct _dwg_entity_ARC_DIMENSION          dwg_ent_arc_dimension;
typedef struct _dwg_entity_PLANESURFACE           dwg_ent_planesurface;
typedef struct _dwg_entity_EXTRUDEDSURFACE        dwg_ent_extrudedsurface;
typedef struct _dwg_entity_LOFTEDSURFACE          dwg_ent_loftedsurface;
typedef struct _dwg_entity_REVOLVEDSURFACE        dwg_ent_revolvedsurface;
typedef struct _dwg_entity_SWEPTSURFACE           dwg_ent_sweptsurface;
typedef struct _dwg_entity_GEOPOSITIONMARKER      dwg_ent_geopositionmarker;
typedef struct _dwg_entity_MESH                   dwg_ent_mesh;
typedef struct _dwg_entity_PROXY_ENTITY           dwg_ent_proxy;
typedef struct _dwg_entity_UNKNOWN_ENT            dwg_ent_unknown;

///////////////////////////////////////////////////////////////////////////

typedef struct _dwg_object_STYLE_CONTROL          dwg_tbl_generic;
typedef struct _dwg_object_UNKNOWN_OBJ            dwg_obj_generic;
typedef struct _dwg_object_UNKNOWN_OBJ            dwg_obj_unknown;
typedef struct _dwg_object_LAYER_CONTROL          dwg_obj_layer_control;
typedef struct _dwg_object_LAYER                  dwg_obj_layer;
typedef struct _dwg_object_BLOCK_HEADER           dwg_obj_block_header;
typedef struct _dwg_object_BLOCK_CONTROL          dwg_obj_block_control;
typedef struct _dwg_object_APPID_CONTROL          dwg_obj_appid_control;
typedef struct _dwg_object_APPID                  dwg_obj_appid;
typedef struct _dwg_object_STYLE_CONTROL          dwg_obj_style_control;
typedef struct _dwg_object_STYLE                  dwg_obj_style;
typedef struct _dwg_object_LTYPE_CONTROL          dwg_obj_ltype_control;
typedef struct _dwg_object_LTYPE                  dwg_obj_ltype;
typedef struct _dwg_object_VIEW_CONTROL           dwg_obj_view_control;
typedef struct _dwg_object_VIEW                   dwg_obj_view;
typedef struct _dwg_object_UCS_CONTROL            dwg_obj_ucs_control;
typedef struct _dwg_object_UCS                    dwg_obj_ucs;
typedef struct _dwg_object_VPORT_CONTROL          dwg_obj_vport_control;
typedef struct _dwg_object_VPORT                  dwg_obj_vport;
typedef struct _dwg_object_DIMSTYLE_CONTROL       dwg_obj_dimstyle_control;
typedef struct _dwg_object_DIMSTYLE               dwg_obj_dimstyle;
typedef struct _dwg_object_VPORT_ENTITY_CONTROL   dwg_obj_vport_entity_control;
typedef struct _dwg_object_VPORT_ENTITY_HEADER    dwg_obj_vport_entity_header;
typedef struct _dwg_object_DICTIONARY             dwg_obj_dictionary;
typedef struct _dwg_object_MLINESTYLE             dwg_obj_mlinestyle;
typedef struct _dwg_object_PROXY_OBJECT           dwg_obj_proxy;
typedef struct _dwg_object_PROXY_OBJECT           dwg_obj_proxy_object;
// stable:
typedef struct _dwg_object_DICTIONARYVAR          dwg_obj_dictionaryvar;
typedef struct _dwg_object_DICTIONARYWDFLT        dwg_obj_dictionarywdflt;
typedef struct _dwg_object_DUMMY                  dwg_obj_dummy;
typedef struct _dwg_object_FIELD                  dwg_obj_field;
typedef struct _dwg_object_FIELDLIST              dwg_obj_fieldlist;
typedef struct _dwg_object_GROUP                  dwg_obj_group;
typedef struct _dwg_object_IDBUFFER               dwg_obj_idbuffer;
typedef struct _dwg_object_IMAGEDEF               dwg_obj_imagedef;
typedef struct _dwg_object_IMAGEDEF_REACTOR       dwg_obj_imagedef_reactor;
typedef struct _dwg_object_LAYER_INDEX            dwg_obj_layer_index;
typedef struct _dwg_object_LAYOUT                 dwg_obj_layout;
typedef struct _dwg_object_MLEADERSTYLE           dwg_obj_mleaderstyle;
typedef struct _dwg_object_OBJECTCONTEXTDATA      dwg_obj_objectcontextdata;
typedef struct _dwg_object_PLACEHOLDER            dwg_obj_placeholder;
typedef struct _dwg_object_RASTERVARIABLES        dwg_obj_rastervariables;
typedef struct _dwg_object_SCALE                  dwg_obj_scale;
typedef struct _dwg_object_SORTENTSTABLE          dwg_obj_sortentstable;
typedef struct _dwg_object_SPATIAL_FILTER         dwg_obj_spatial_filter;
typedef struct _dwg_object_SPATIAL_INDEX          dwg_obj_spatial_index;
typedef struct _dwg_object_WIPEOUTVARIABLES       dwg_obj_wipeoutvariables;
typedef struct _dwg_object_XRECORD                dwg_obj_xrecord;
// unstable:
typedef struct _dwg_object_ASSOCDEPENDENCY        dwg_obj_assocdependency;
typedef struct _dwg_object_ASSOCALIGNEDDIMACTIONBODY dwg_obj_assocaligneddimactionbody;
typedef struct _dwg_object_ASSOCPLANESURFACEACTIONBODY dwg_obj_assocplanesurfaceactionbody;
typedef struct _dwg_object_DIMASSOC               dwg_obj_dimassoc;
typedef struct _dwg_object_DBCOLOR                dwg_obj_dbcolor;
typedef struct _dwg_object_DYNAMICBLOCKPURGEPREVENTER dwg_obj_dynamicblockpurgepreventer;
typedef struct _dwg_object_GEODATA                dwg_obj_geodata;
typedef struct _dwg_object_LONG_TRANSACTION       dwg_obj_long_transaction;
typedef struct _dwg_object_OBJECT_PTR             dwg_obj_object_ptr;
typedef struct _dwg_object_PERSSUBENTMANAGER      dwg_obj_perssubentmanager;
typedef struct _dwg_object_UNDERLAYDEFINITION     dwg_obj_underlaydefinition;
typedef struct _dwg_object_TABLESTYLE             dwg_obj_tablestyle;
typedef struct _dwg_object_VISUALSTYLE            dwg_obj_visualstyle;
#ifdef DEBUG_CLASSES
typedef struct _dwg_object_TABLECONTENT           dwg_obj_tablecontent;
typedef struct _dwg_object_TABLEGEOMETRY          dwg_obj_tablegeometry;
typedef struct _dwg_object_CELLSTYLEMAP           dwg_obj_cellstylemap;
typedef struct _dwg_object_MATERIAL               dwg_obj_material;
typedef struct _dwg_object_PLOTSETTINGS           dwg_obj_plotsettings;
typedef struct _dwg_object_SUN                    dwg_obj_sun;
typedef struct _dwg_object_SUNSTUDY               dwg_obj_sunstudy;
typedef struct _dwg_object_VBA_PROJECT            dwg_obj_vba_project;
typedef struct _dwg_object_ACSH_SWEEP_CLASS       dwg_obj_acsh_sweep_class;
typedef struct _dwg_object_ACSH_BOX_CLASS         dwg_obj_acsh_box_class;
typedef struct _dwg_object_ACSH_EXTRUSION_CLASS   dwg_obj_acsh_extrusion_class;
typedef struct _dwg_object_ALDIMOBJECTCONTEXTDATA dwg_obj_aldimobjectcontextdata;
typedef struct _dwg_object_BLKREFOBJECTCONTEXTDATA  dwg_obj_blkrefobjectcontextdata;
typedef struct _dwg_object_LEADEROBJECTCONTEXTDATA  dwg_obj_leaderobjectcontextdata;
typedef struct _dwg_object_MLEADEROBJECTCONTEXTDATA  dwg_obj_mleaderobjectcontextdata;
typedef struct _dwg_object_MTEXTATTRIBUTEOBJECTCONTEXTDATA  dwg_obj_mtextattributeobjectcontextdata;
typedef struct _dwg_object_MTEXTOBJECTCONTEXTDATA  dwg_obj_mtextobjectcontextdata;
typedef struct _dwg_object_TEXTOBJECTCONTEXTDATA  dwg_obj_textobjectcontextdata;
typedef struct _dwg_object_ASSOC2DCONSTRAINTGROUP dwg_obj_assoc2dconstraintgroup;
typedef struct _dwg_object_ASSOCACTION            dwg_obj_assocaction;
typedef struct _dwg_object_ASSOCEXTRUDEDSURFACEACTIONBODY dwg_obj_assocextrudedsurfaceactionbody;
typedef struct _dwg_object_ASSOCLOFTEDSURFACEACTIONBODY dwg_obj_assocloftedsurfaceactionbody;
typedef struct _dwg_object_ASSOCNETWORK           dwg_obj_assocnetwork;
typedef struct _dwg_object_ASSOCOSNAPPOINTREFACTIONPARAM dwg_obj_assocosnappointrefactionparam;
typedef struct _dwg_object_ASSOCPERSSUBENTMANAGER dwg_obj_assocperssubentmanager;
typedef struct _dwg_object_ASSOCREVOLVEDSURFACEACTIONBODY dwg_obj_assocrevolvedsurfaceactionbody;
typedef struct _dwg_object_ASSOCSWEPTSURFACEACTIONBODY dwg_obj_assocsweptsurfaceactionbody;
typedef struct _dwg_object_EVALUATION_GRAPH       dwg_obj_evaluation_graph;
typedef struct _dwg_object_NAVISWORKSMODELDEF     dwg_obj_navisworksmodeldef;
typedef struct _dwg_object_GEOMAPIMAGE            dwg_obj_geomapimage;
typedef struct _dwg_object_LIGHTLIST              dwg_obj_lightlist;
typedef struct _dwg_object_RENDERENVIRONMENT      dwg_obj_renderenvironment;
typedef struct _dwg_object_RENDERGLOBAL           dwg_obj_renderglobal;
typedef struct _dwg_object_DATALINK               dwg_obj_datalink;
typedef struct _dwg_object_DATATABLE              dwg_obj_datatable;
#endif
// unhandled:
//typedef struct _dwg_object_ACDSRECORD           dwg_obj_acdsrecord;
//typedef struct _dwg_object_ACDSSCHEMA           dwg_obj_acdsschema;
//typedef struct _dwg_object_ACDSSCOPE            dwg_obj_acdsscope;
//typedef struct _dwg_object_ACMECOMMANDHISTORY   dwg_obj_acmecommandhistory;
//typedef struct _dwg_object_ACMESTATEMGR         dwg_obj_acmestatemgr;
//typedef struct _dwg_object_ACSH_HISTORY_CLASS   dwg_obj_acsh_history_class;
//typedef struct _dwg_object_ACSH_REVOLVE_CLASS   dwg_obj_acsh_revolve_class;
//typedef struct _dwg_object_ACSH_SPHERE_CLASS    dwg_obj_acsh_sphere_class;
//typedef struct _dwg_object_ARCALIGNEDTEXT       dwg_obj_arcalignedtext;
//typedef struct _dwg_object_ASSOCGEOMDEPENDENCY  dwg_obj_assocgeomdependency;
//typedef struct _dwg_object_ASSOCOSNAPPOINTREFACTIONPARAM  dwg_obj_assocosnappointrefactionparam;
//typedef struct _dwg_object_ASSOCVERTEXACTIONPARAM         dwg_obj_assocvertexactionparam;
//typedef struct _dwg_object_DETAILVIEWSTYLE      dwg_obj_detailviewstyle;
//typedef struct _dwg_object_DOCUMENTOPTIONS      dwg_obj_documentoptions;
//typedef struct _dwg_object_LAYERFILTER         dwg_obj_layerfilter;
//typedef struct _dwg_object_LAYOUTPRINTCONFIG    dwg_obj_layoutprintconfig;
//typedef struct _dwg_object_LEADEROBJECTCONTEXTDATA dwg_obj_leaderobjectcontextdata;
//typedef struct _dwg_object_NPOCOLLECTION        dwg_obj_npocollection;
//typedef struct _dwg_object_POINTCLOUD           dwg_obj_pointcloud;
//typedef struct _dwg_object_MENTALRAYRENDERSETTINGS   dwg_obj_mentalrayrendersettings;
//typedef struct _dwg_object_RAPIDRTRENDERENVIRONMENT  dwg_obj_rapidrtrenderenvironment;
//typedef struct _dwg_object_RAPIDRTRENDERSETTINGS     dwg_obj_rapidrtrendersettings;
//typedef struct _dwg_object_RTEXT                dwg_obj_rtext;
//typedef struct _dwg_object_SECTIONVIEWSTYLE     dwg_obj_sectionviewstyle;
//typedef struct _dwg_object_XREFPANELOBJECT      dwg_obj_xrefpanelobject;

typedef struct _dwg_object                        dwg_object;
typedef struct _dwg_object_ref                    dwg_object_ref;
typedef struct _dwg_handle                        dwg_handle;
typedef struct _dwg_object_entity                 dwg_obj_ent;
typedef struct _dwg_object_object                 dwg_obj_obj;
typedef struct _dwg_class                         dwg_class;
typedef struct _dwg_struct                        dwg_data;
typedef struct _dwg_entity_eed_data               dwg_entity_eed_data;
typedef struct _dwg_entity_eed                    dwg_entity_eed;

///////////////////////////////////////////////////////////////////////////

/********************************************************************
 * Functions to return NULL-terminated array of all owned entities  *
 ********************************************************************/

/// extract all owned entities from a block header (mspace or pspace)
DWG_GETALL_ENTITY_DECL (TEXT)
DWG_GETALL_ENTITY_DECL (ATTRIB)
DWG_GETALL_ENTITY_DECL (ATTDEF)
DWG_GETALL_ENTITY_DECL (BLOCK)
DWG_GETALL_ENTITY_DECL (ENDBLK)
DWG_GETALL_ENTITY_DECL (SEQEND)
DWG_GETALL_ENTITY_DECL (INSERT)
DWG_GETALL_ENTITY_DECL (MINSERT)
DWG_GETALL_ENTITY_DECL (VERTEX_2D)
DWG_GETALL_ENTITY_DECL (VERTEX_3D)
DWG_GETALL_ENTITY_DECL (VERTEX_MESH)
DWG_GETALL_ENTITY_DECL (VERTEX_PFACE)
DWG_GETALL_ENTITY_DECL (VERTEX_PFACE_FACE)
DWG_GETALL_ENTITY_DECL (POLYLINE_2D)
DWG_GETALL_ENTITY_DECL (POLYLINE_3D)
DWG_GETALL_ENTITY_DECL (ARC)
DWG_GETALL_ENTITY_DECL (CIRCLE)
DWG_GETALL_ENTITY_DECL (LINE)
DWG_GETALL_ENTITY_DECL (DIMENSION_ORDINATE)
DWG_GETALL_ENTITY_DECL (DIMENSION_LINEAR)
DWG_GETALL_ENTITY_DECL (DIMENSION_ALIGNED)
DWG_GETALL_ENTITY_DECL (DIMENSION_ANG3PT)
DWG_GETALL_ENTITY_DECL (DIMENSION_ANG2LN)
DWG_GETALL_ENTITY_DECL (DIMENSION_RADIUS)
DWG_GETALL_ENTITY_DECL (DIMENSION_DIAMETER)
DWG_GETALL_ENTITY_DECL (POINT)
DWG_GETALL_ENTITY_DECL (POLYLINE_PFACE)
DWG_GETALL_ENTITY_DECL (POLYLINE_MESH)
DWG_GETALL_ENTITY_DECL (SOLID)
DWG_GETALL_ENTITY_DECL (TRACE)
DWG_GETALL_ENTITY_DECL (SHAPE)
DWG_GETALL_ENTITY_DECL (VIEWPORT)
DWG_GETALL_ENTITY_DECL (ELLIPSE)
DWG_GETALL_ENTITY_DECL (SPLINE)
DWG_GETALL_ENTITY_DECL (REGION)
DWG_GETALL_ENTITY_DECL (BODY)
DWG_GETALL_ENTITY_DECL (RAY)
DWG_GETALL_ENTITY_DECL (XLINE)
DWG_GETALL_ENTITY_DECL (OLEFRAME)
DWG_GETALL_ENTITY_DECL (MTEXT)
DWG_GETALL_ENTITY_DECL (LEADER)
DWG_GETALL_ENTITY_DECL (TOLERANCE)
DWG_GETALL_ENTITY_DECL (MLINE)
DWG_GETALL_ENTITY_DECL (OLE2FRAME)
DWG_GETALL_ENTITY_DECL (LWPOLYLINE)
DWG_GETALL_ENTITY_DECL (PROXY_ENTITY)
DWG_GETALL_ENTITY_DECL (HATCH)
DWG_GETALL_ENTITY_DECL (UNKNOWN_ENT)

/// extract all varying entities from a block
DWG_GETALL_ENTITY_DECL (IMAGE)
DWG_GETALL_ENTITY_DECL (CAMERA)
DWG_GETALL_ENTITY_DECL (HELIX)
DWG_GETALL_ENTITY_DECL (LIGHT)
DWG_GETALL_ENTITY_DECL (MULTILEADER)
DWG_GETALL_ENTITY_DECL (UNDERLAY)
DWG_GETALL_ENTITY_DECL (WIPEOUT)
// DEBUG_CLASSES
DWG_GETALL_ENTITY_DECL (ARC_DIMENSION)
DWG_GETALL_ENTITY_DECL (EXTRUDEDSURFACE)
DWG_GETALL_ENTITY_DECL (GEOPOSITIONMARKER)
DWG_GETALL_ENTITY_DECL (LOFTEDSURFACE)
DWG_GETALL_ENTITY_DECL (PLANESURFACE)
DWG_GETALL_ENTITY_DECL (REVOLVEDSURFACE)
DWG_GETALL_ENTITY_DECL (SWEPTSURFACE)
DWG_GETALL_ENTITY_DECL (TABLE)
DWG_GETALL_ENTITY_DECL (MESH)

/********************************************************************
 *     Functions to return NULL-terminated array of all objects     *
 ********************************************************************/

/// extract all objects from a dwg

// fixed (incl. tables)
DWG_GETALL_OBJECT_DECL (BLOCK_CONTROL)
DWG_GETALL_OBJECT_DECL (BLOCK_HEADER)
DWG_GETALL_OBJECT_DECL (LAYER_CONTROL)
DWG_GETALL_OBJECT_DECL (LAYER)
DWG_GETALL_OBJECT_DECL (STYLE_CONTROL)
DWG_GETALL_OBJECT_DECL (STYLE)
DWG_GETALL_OBJECT_DECL (LTYPE_CONTROL)
DWG_GETALL_OBJECT_DECL (LTYPE)
DWG_GETALL_OBJECT_DECL (VIEW_CONTROL)
DWG_GETALL_OBJECT_DECL (VIEW)
DWG_GETALL_OBJECT_DECL (UCS_CONTROL)
DWG_GETALL_OBJECT_DECL (UCS)
DWG_GETALL_OBJECT_DECL (VPORT_CONTROL)
DWG_GETALL_OBJECT_DECL (VPORT)
DWG_GETALL_OBJECT_DECL (APPID_CONTROL)
DWG_GETALL_OBJECT_DECL (APPID)
DWG_GETALL_OBJECT_DECL (DIMSTYLE_CONTROL)
DWG_GETALL_OBJECT_DECL (DIMSTYLE)
DWG_GETALL_OBJECT_DECL (VPORT_ENTITY_CONTROL)
DWG_GETALL_OBJECT_DECL (VPORT_ENTITY_HEADER)

DWG_GETALL_OBJECT_DECL (DICTIONARY)
DWG_GETALL_OBJECT_DECL (MLINESTYLE)
DWG_GETALL_OBJECT_DECL (UNKNOWN_OBJ)
// stable:
DWG_GETALL_OBJECT_DECL (DICTIONARYVAR)
DWG_GETALL_OBJECT_DECL (DICTIONARYWDFLT)
DWG_GETALL_OBJECT_DECL (DUMMY)
DWG_GETALL_OBJECT_DECL (FIELD)
DWG_GETALL_OBJECT_DECL (FIELDLIST)
DWG_GETALL_OBJECT_DECL (GROUP)
DWG_GETALL_OBJECT_DECL (IDBUFFER)
DWG_GETALL_OBJECT_DECL (IMAGEDEF)
DWG_GETALL_OBJECT_DECL (IMAGEDEF_REACTOR)
DWG_GETALL_OBJECT_DECL (LAYER_INDEX)
DWG_GETALL_OBJECT_DECL (LAYOUT)
DWG_GETALL_OBJECT_DECL (MLEADERSTYLE)
DWG_GETALL_OBJECT_DECL (OBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (PLACEHOLDER)
DWG_GETALL_OBJECT_DECL (RASTERVARIABLES)
DWG_GETALL_OBJECT_DECL (SCALE)
DWG_GETALL_OBJECT_DECL (SORTENTSTABLE)
DWG_GETALL_OBJECT_DECL (SPATIAL_FILTER)
DWG_GETALL_OBJECT_DECL (SPATIAL_INDEX)
DWG_GETALL_OBJECT_DECL (WIPEOUTVARIABLES)
DWG_GETALL_OBJECT_DECL (XRECORD)
// unstable:
DWG_GETALL_OBJECT_DECL (ASSOCDEPENDENCY)
DWG_GETALL_OBJECT_DECL (ASSOCALIGNEDDIMACTIONBODY)
DWG_GETALL_OBJECT_DECL (ASSOCPLANESURFACEACTIONBODY)
DWG_GETALL_OBJECT_DECL (DIMASSOC)
DWG_GETALL_OBJECT_DECL (DBCOLOR)
DWG_GETALL_OBJECT_DECL (DYNAMICBLOCKPURGEPREVENTER)
DWG_GETALL_OBJECT_DECL (GEODATA)
DWG_GETALL_OBJECT_DECL (LONG_TRANSACTION)
DWG_GETALL_OBJECT_DECL (OBJECT_PTR)
DWG_GETALL_OBJECT_DECL (PERSSUBENTMANAGER)
DWG_GETALL_OBJECT_DECL (PROXY_OBJECT)
DWG_GETALL_OBJECT_DECL (UNDERLAYDEFINITION)
DWG_GETALL_OBJECT_DECL (TABLESTYLE)
DWG_GETALL_OBJECT_DECL (VISUALSTYLE)

#ifdef DEBUG_CLASSES
DWG_GETALL_OBJECT_DECL (TABLECONTENT)
DWG_GETALL_OBJECT_DECL (TABLEGEOMETRY)
DWG_GETALL_OBJECT_DECL (CELLSTYLEMAP)
DWG_GETALL_OBJECT_DECL (MATERIAL)
DWG_GETALL_OBJECT_DECL (PLOTSETTINGS)
DWG_GETALL_OBJECT_DECL (SUN)
DWG_GETALL_OBJECT_DECL (SUNSTUDY)
DWG_GETALL_OBJECT_DECL (VBA_PROJECT)
DWG_GETALL_OBJECT_DECL (ACSH_SWEEP_CLASS)
DWG_GETALL_OBJECT_DECL (ACSH_BOX_CLASS)
DWG_GETALL_OBJECT_DECL (ACSH_EXTRUSION_CLASS)
DWG_GETALL_OBJECT_DECL (ALDIMOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (BLKREFOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (LEADEROBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (MLEADEROBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (MTEXTATTRIBUTEOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (MTEXTOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (TEXTOBJECTCONTEXTDATA)
DWG_GETALL_OBJECT_DECL (ASSOCACTION)
DWG_GETALL_OBJECT_DECL (ASSOCNETWORK)
DWG_GETALL_OBJECT_DECL (ASSOCEXTRUDEDSURFACEACTIONBODY)
DWG_GETALL_OBJECT_DECL (ASSOCLOFTEDSURFACEACTIONBODY)
DWG_GETALL_OBJECT_DECL (ASSOCREVOLVEDSURFACEACTIONBODY)
DWG_GETALL_OBJECT_DECL (ASSOCSWEPTSURFACEACTIONBODY)
DWG_GETALL_OBJECT_DECL (ASSOCOSNAPPOINTREFACTIONPARAM)
DWG_GETALL_OBJECT_DECL (ASSOCPERSSUBENTMANAGER)
DWG_GETALL_OBJECT_DECL (ASSOC2DCONSTRAINTGROUP)
DWG_GETALL_OBJECT_DECL (EVALUATION_GRAPH)
DWG_GETALL_OBJECT_DECL (NAVISWORKSMODELDEF)
DWG_GETALL_OBJECT_DECL (GEOMAPIMAGE)
DWG_GETALL_OBJECT_DECL (LIGHTLIST)
DWG_GETALL_OBJECT_DECL (RENDERENVIRONMENT)
DWG_GETALL_OBJECT_DECL (RENDERGLOBAL)
DWG_GETALL_OBJECT_DECL (DATALINK)
DWG_GETALL_OBJECT_DECL (DATATABLE)
#endif
// unhandled:
// DWG_GETALL_OBJECT_DECL (ACDSRECORD)
// DWG_GETALL_OBJECT_DECL (ACDSSCHEMA)
// DWG_GETALL_OBJECT_DECL (ACDSSCOPE)
// DWG_GETALL_OBJECT_DECL (ACMECOMMANDHISTORY)
// DWG_GETALL_OBJECT_DECL (ACMESTATEMGR)
// DWG_GETALL_OBJECT_DECL (ACSH_HISTORY_CLASS)
// DWG_GETALL_OBJECT_DECL (ACSH_REVOLVE_CLASS)
// DWG_GETALL_OBJECT_DECL (ACSH_SPHERE_CLASS)
// DWG_GETALL_OBJECT_DECL (ARCALIGNEDTEXT)
// DWG_GETALL_OBJECT_DECL (ASSOCGEOMDEPENDENCY)
// DWG_GETALL_OBJECT_DECL (ASSOCOSNAPPOINTREFACTIONPARAM)
// DWG_GETALL_OBJECT_DECL (ASSOCVERTEXACTIONPARAM)
// DWG_GETALL_OBJECT_DECL (DETAILVIEWSTYLE)
// DWG_GETALL_OBJECT_DECL (DOCUMENTOPTIONS)
// DWG_GETALL_OBJECT_DECL (LAYERFILTER)
// DWG_GETALL_OBJECT_DECL (LAYOUTPRINTCONFIG)
// DWG_GETALL_OBJECT_DECL (LEADEROBJECTCONTEXTDATA)
// DWG_GETALL_OBJECT_DECL (MENTALRAYRENDERSETTINGS)
// DWG_GETALL_OBJECT_DECL (RAPIDRTRENDERENVIRONMENT)
// DWG_GETALL_OBJECT_DECL (RAPIDRTRENDERSETTINGS)
// DWG_GETALL_OBJECT_DECL (NPOCOLLECTION)
// DWG_GETALL_OBJECT_DECL (POINTCLOUD)
// DWG_GETALL_OBJECT_DECL (RTEXT)
// DWG_GETALL_OBJECT_DECL (SECTIONVIEWSTYLE)
// DWG_GETALL_OBJECT_DECL (XREFPANELOBJECT)

/*******************************************************************
*     Functions created from macro to cast dwg_object to entity     *
*                 Usage :- dwg_object_to_ENTITY(),                  *
*                where ENTITY can be LINE or CIRCLE                 *
********************************************************************/

CAST_DWG_OBJECT_TO_ENTITY_DECL (TEXT)
CAST_DWG_OBJECT_TO_ENTITY_DECL (ATTRIB)
CAST_DWG_OBJECT_TO_ENTITY_DECL (ATTDEF)
CAST_DWG_OBJECT_TO_ENTITY_DECL (BLOCK)
CAST_DWG_OBJECT_TO_ENTITY_DECL (ENDBLK)
CAST_DWG_OBJECT_TO_ENTITY_DECL (SEQEND)
CAST_DWG_OBJECT_TO_ENTITY_DECL (INSERT)
CAST_DWG_OBJECT_TO_ENTITY_DECL (MINSERT)
CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_2D)
CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_3D)
CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_MESH)
CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_PFACE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_PFACE_FACE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_2D)
CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_3D)
CAST_DWG_OBJECT_TO_ENTITY_DECL (ARC)
CAST_DWG_OBJECT_TO_ENTITY_DECL (CIRCLE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (LINE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (_3DFACE)
EXPORT dwg_ent_dim * dwg_object_to_DIMENSION(dwg_object *obj);
CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ORDINATE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_LINEAR)
CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ALIGNED)
CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ANG3PT)
CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ANG2LN)
CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_RADIUS)
CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_DIAMETER)
//CAST_DWG_OBJECT_TO_ENTITY_DECL (ARC_DIMENSION)
CAST_DWG_OBJECT_TO_ENTITY_DECL (POINT)
CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_PFACE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_MESH)
CAST_DWG_OBJECT_TO_ENTITY_DECL (SOLID)
CAST_DWG_OBJECT_TO_ENTITY_DECL (TRACE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (SHAPE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (VIEWPORT)
CAST_DWG_OBJECT_TO_ENTITY_DECL (ELLIPSE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (SPLINE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (REGION)
CAST_DWG_OBJECT_TO_ENTITY_DECL (BODY)
CAST_DWG_OBJECT_TO_ENTITY_DECL (RAY)
CAST_DWG_OBJECT_TO_ENTITY_DECL (XLINE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (OLEFRAME)
CAST_DWG_OBJECT_TO_ENTITY_DECL (MTEXT)
CAST_DWG_OBJECT_TO_ENTITY_DECL (LEADER)
CAST_DWG_OBJECT_TO_ENTITY_DECL (TOLERANCE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (MLINE)
CAST_DWG_OBJECT_TO_ENTITY_DECL (OLE2FRAME)
CAST_DWG_OBJECT_TO_ENTITY_DECL (PROXY_ENTITY)
CAST_DWG_OBJECT_TO_ENTITY_DECL (HATCH)
CAST_DWG_OBJECT_TO_ENTITY_DECL (UNKNOWN_ENT)

/// dwg object to variable types
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LWPOLYLINE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (CAMERA)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (HELIX)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (IMAGE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LIGHT)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (MULTILEADER)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (UNDERLAY)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (WIPEOUT)
// DEBUG_CLASSES
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ARC_DIMENSION)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (GEOPOSITIONMARKER)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (EXTRUDEDSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LOFTEDSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (PLANESURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (REVOLVEDSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (SWEPTSURFACE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (TABLE)
CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (MESH)

/*******************************************************************
*     Functions created from macro to dwg object to object         *
*                 Usage :- dwg_object_to_OBJECT(),                 *
*            where OBJECT can be any object                        *
********************************************************************/
CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCK_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCK_HEADER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYER_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (STYLE_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (STYLE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LTYPE_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LTYPE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VIEW_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VIEW)
CAST_DWG_OBJECT_TO_OBJECT_DECL (UCS_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (UCS)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VPORT_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VPORT)
CAST_DWG_OBJECT_TO_OBJECT_DECL (APPID_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (APPID)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DIMSTYLE_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DIMSTYLE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VPORT_ENTITY_CONTROL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VPORT_ENTITY_HEADER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (GROUP)
CAST_DWG_OBJECT_TO_OBJECT_DECL (MLINESTYLE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (UNKNOWN_OBJ)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DICTIONARY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (PLACEHOLDER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (PROXY_OBJECT)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYOUT)
CAST_DWG_OBJECT_TO_OBJECT_DECL (XRECORD)

CAST_DWG_OBJECT_TO_OBJECT_DECL (DICTIONARYVAR)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DICTIONARYWDFLT)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DUMMY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (FIELD)
CAST_DWG_OBJECT_TO_OBJECT_DECL (FIELDLIST)
CAST_DWG_OBJECT_TO_OBJECT_DECL (IDBUFFER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (IMAGEDEF)
CAST_DWG_OBJECT_TO_OBJECT_DECL (IMAGEDEF_REACTOR)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYER_INDEX)
CAST_DWG_OBJECT_TO_OBJECT_DECL (MLEADERSTYLE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (OBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (RASTERVARIABLES)
CAST_DWG_OBJECT_TO_OBJECT_DECL (SCALE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (SORTENTSTABLE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (SPATIAL_FILTER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (SPATIAL_INDEX)
CAST_DWG_OBJECT_TO_OBJECT_DECL (TABLESTYLE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VISUALSTYLE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (WIPEOUTVARIABLES)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCDEPENDENCY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCALIGNEDDIMACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCPLANESURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DBCOLOR)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DYNAMICBLOCKPURGEPREVENTER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DIMASSOC)
CAST_DWG_OBJECT_TO_OBJECT_DECL (GEODATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LONG_TRANSACTION)
CAST_DWG_OBJECT_TO_OBJECT_DECL (OBJECT_PTR)
CAST_DWG_OBJECT_TO_OBJECT_DECL (PERSSUBENTMANAGER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (UNDERLAYDEFINITION)

#ifdef DEBUG_CLASSES
CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_SWEEP_CLASS)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_BOX_CLASS)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_EXTRUSION_CLASS)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ALDIMOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (BLKREFOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LEADEROBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (MLEADEROBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (MTEXTATTRIBUTEOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (MTEXTOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (TEXTOBJECTCONTEXTDATA)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCACTION)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCNETWORK)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCEXTRUDEDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCLOFTEDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCREVOLVEDSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCSWEPTSURFACEACTIONBODY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCOSNAPPOINTREFACTIONPARAM)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCPERSSUBENTMANAGER)
CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOC2DCONSTRAINTGROUP)
CAST_DWG_OBJECT_TO_OBJECT_DECL (EVALUATION_GRAPH)
CAST_DWG_OBJECT_TO_OBJECT_DECL (NAVISWORKSMODELDEF)
CAST_DWG_OBJECT_TO_OBJECT_DECL (GEOMAPIMAGE)
CAST_DWG_OBJECT_TO_OBJECT_DECL (MATERIAL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (TABLECONTENT)
CAST_DWG_OBJECT_TO_OBJECT_DECL (TABLEGEOMETRY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (CELLSTYLEMAP)
CAST_DWG_OBJECT_TO_OBJECT_DECL (PLOTSETTINGS)
CAST_DWG_OBJECT_TO_OBJECT_DECL (SUN)
CAST_DWG_OBJECT_TO_OBJECT_DECL (SUNSTUDY)
CAST_DWG_OBJECT_TO_OBJECT_DECL (VBA_PROJECT)
CAST_DWG_OBJECT_TO_OBJECT_DECL (LIGHTLIST)
CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERENVIRONMENT)
CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERGLOBAL)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DATALINK)
CAST_DWG_OBJECT_TO_OBJECT_DECL (DATATABLE)
#endif


/// initialize the DWG version, needed for r2007+ unicode strings
/// and pre-R13 field variants. unneeded for r13-r2004
EXPORT void dwg_api_init_version (Dwg_Data *dwg);

#define _deprecated_dynapi_getter \
  __attribute_deprecated_msg__("use ‘dwg_dynapi_entity_value‘ instead")
#define _deprecated_dynapi_setter \
  __attribute_deprecated_msg__("use ‘dwg_dynapi_entity_set_value‘ instead")
#define _deprecated_dynapi_common_getter \
  __attribute_deprecated_msg__("use ‘dwg_dynapi_common_value‘ instead")
#define _deprecated_dynapi_common_setter \
  __attribute_deprecated_msg__("use ‘dwg_dynapi_common_set_value‘ instead")

/********************************************************************
*                FUNCTIONS START HERE ENTITY SPECIFIC               *
*********************************************************************/

EXPORT bool dwg_get_HEADER (const Dwg_Data *restrict dwg,
                            const char *restrict fieldname, void *restrict out)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_get_HEADER_utf8text (const Dwg_Data *restrict dwg,
                                     const char *restrict fieldname,
                                     char **restrict textp, int *isnewp)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_set_HEADER (Dwg_Data *restrict dwg,
                            const char *restrict fieldname,
                            const void *restrict value) __nonnull ((1, 2, 3));
EXPORT bool dwg_set_HEADER_utf8text (Dwg_Data *restrict dwg,
                                     const char *restrict fieldname,
                                     const char *restrict utf8)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_get_ENTITY_common (Dwg_Object_Entity *restrict obj,
                                   const char *restrict fieldname,
                                   void *restrict value) __nonnull ((1, 2, 3));
EXPORT bool dwg_get_ENTITY_common_utf8text (Dwg_Object_Entity *restrict obj,
                                     const char *restrict fieldname,
                                     char **restrict textp, int *isnewp)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_get_OBJECT_common (Dwg_Object_Object *restrict obj,
                                   const char *restrict fieldname,
                                   void *restrict value)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_get_OBJECT_common_utf8text (Dwg_Object_Object *restrict obj,
                                            const char *restrict fieldname,
                                            char **restrict textp, int *isnewp)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_set_ENTITY_common (Dwg_Object_Entity *restrict obj,
                            const char *restrict fieldname,
                            const void *restrict value) __nonnull ((1, 2, 3));
EXPORT bool dwg_set_ENTITY_common_utf8text (Dwg_Object_Entity *restrict obj,
                                     const char *restrict fieldname,
                                     const char *restrict utf8)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_set_OBJECT_common (Dwg_Object_Object *restrict obj,
                                   const char *restrict fieldname,
                                   const void *restrict value)
    __nonnull ((1, 2, 3));
EXPORT bool dwg_set_OBJECT_common_utf8text (Dwg_Object_Object *restrict obj,
                                            const char *restrict fieldname,
                                            const char *restrict utf8)
    __nonnull ((1, 2, 3));

#define dwg_get_OBJECT_DECL(name, OBJECT)                               \
  EXPORT bool                                                           \
  dwg_get_##OBJECT (const dwg_##name *restrict name,                    \
                    const char *restrict fieldname, void *restrict out); \
  EXPORT bool                                                           \
  dwg_set_##OBJECT (const dwg_##name *restrict name,                    \
                    const char *restrict fieldname, void *restrict value); \

dwg_get_OBJECT_DECL (ent_text, TEXT);
dwg_get_OBJECT_DECL (ent_attrib, ATTRIB);
dwg_get_OBJECT_DECL (ent_attdef, ATTDEF);
dwg_get_OBJECT_DECL (ent_block, BLOCK);
dwg_get_OBJECT_DECL (ent_endblk, ENDBLK);
dwg_get_OBJECT_DECL (ent_seqend, SEQEND);
dwg_get_OBJECT_DECL (ent_insert, INSERT);
dwg_get_OBJECT_DECL (ent_minsert, MINSERT);
dwg_get_OBJECT_DECL (ent_vertex_2d, VERTEX_2D);
dwg_get_OBJECT_DECL (ent_vertex_3d, VERTEX_3D);
dwg_get_OBJECT_DECL (ent_vertex_mesh, VERTEX_MESH);
dwg_get_OBJECT_DECL (ent_vertex_pface, VERTEX_PFACE);
dwg_get_OBJECT_DECL (ent_vertex_pface_face, VERTEX_PFACE_FACE);
dwg_get_OBJECT_DECL (ent_polyline_2d, POLYLINE_2D);
dwg_get_OBJECT_DECL (ent_polyline_3d, POLYLINE_3D);
dwg_get_OBJECT_DECL (ent_arc, ARC);
dwg_get_OBJECT_DECL (ent_circle, CIRCLE);
dwg_get_OBJECT_DECL (ent_line, LINE);
dwg_get_OBJECT_DECL (ent_dim_ordinate, DIMENSION_ORDINATE);
dwg_get_OBJECT_DECL (ent_dim_linear, DIMENSION_LINEAR);
dwg_get_OBJECT_DECL (ent_dim_aligned, DIMENSION_ALIGNED);
dwg_get_OBJECT_DECL (ent_dim_ang3pt, DIMENSION_ANG3PT);
dwg_get_OBJECT_DECL (ent_dim_ang2ln, DIMENSION_ANG2LN);
dwg_get_OBJECT_DECL (ent_dim_radius, DIMENSION_RADIUS);
dwg_get_OBJECT_DECL (ent_dim_diameter, DIMENSION_DIAMETER);
dwg_get_OBJECT_DECL (ent_point, POINT);
dwg_get_OBJECT_DECL (ent_polyline_pface, POLYLINE_PFACE);
dwg_get_OBJECT_DECL (ent_polyline_mesh, POLYLINE_MESH);
dwg_get_OBJECT_DECL (ent_solid, SOLID);
dwg_get_OBJECT_DECL (ent_trace, TRACE);
dwg_get_OBJECT_DECL (ent_shape, SHAPE);
dwg_get_OBJECT_DECL (ent_viewport, VIEWPORT);
dwg_get_OBJECT_DECL (ent_ellipse, ELLIPSE);
dwg_get_OBJECT_DECL (ent_spline, SPLINE);
dwg_get_OBJECT_DECL (ent_region, REGION);
dwg_get_OBJECT_DECL (ent_body, BODY);
dwg_get_OBJECT_DECL (ent_ray, RAY);
dwg_get_OBJECT_DECL (ent_xline, XLINE);
dwg_get_OBJECT_DECL (ent_oleframe, OLEFRAME);
dwg_get_OBJECT_DECL (ent_mtext, MTEXT);
dwg_get_OBJECT_DECL (ent_leader, LEADER);
dwg_get_OBJECT_DECL (ent_tolerance, TOLERANCE);
dwg_get_OBJECT_DECL (ent_mline, MLINE);
dwg_get_OBJECT_DECL (ent_ole2frame, OLE2FRAME);
dwg_get_OBJECT_DECL (ent_lwpline, LWPOLYLINE);
dwg_get_OBJECT_DECL (ent_proxy, PROXY_ENTITY);
dwg_get_OBJECT_DECL (ent_hatch, HATCH);
dwg_get_OBJECT_DECL (ent_unknown, UNKNOWN_ENT);
dwg_get_OBJECT_DECL (ent_image, IMAGE);
dwg_get_OBJECT_DECL (ent_camera, CAMERA);
dwg_get_OBJECT_DECL (ent_helix, HELIX);
dwg_get_OBJECT_DECL (ent_light, LIGHT);
dwg_get_OBJECT_DECL (ent_mleader, MULTILEADER);
dwg_get_OBJECT_DECL (ent_underlay, UNDERLAY);
dwg_get_OBJECT_DECL (ent_wipeout, WIPEOUT);
#ifdef DEBUG_CLASSES
dwg_get_OBJECT_DECL (ent_arc_dimension, ARC_DIMENSION);
dwg_get_OBJECT_DECL (ent_planesurface, PLANESURFACE);
dwg_get_OBJECT_DECL (ent_extrudedsurface, EXTRUDEDSURFACE);
dwg_get_OBJECT_DECL (ent_loftedsurface, LOFTEDSURFACE);
dwg_get_OBJECT_DECL (ent_revolvedsurface, REVOLVEDSURFACE);
dwg_get_OBJECT_DECL (ent_sweptsurface, SWEPTSURFACE);
dwg_get_OBJECT_DECL (ent_geopositionmarker, GEOPOSITIONMARKER);
dwg_get_OBJECT_DECL (ent_table, TABLE);
dwg_get_OBJECT_DECL (ent_mesh, MESH);
#endif

dwg_get_OBJECT_DECL (obj_block_control, BLOCK_CONTROL);
dwg_get_OBJECT_DECL (obj_block_header, BLOCK_HEADER);
dwg_get_OBJECT_DECL (obj_layer_control, LAYER_CONTROL);
dwg_get_OBJECT_DECL (obj_layer, LAYER);
dwg_get_OBJECT_DECL (obj_style_control, STYLE_CONTROL);
dwg_get_OBJECT_DECL (obj_style, STYLE);
dwg_get_OBJECT_DECL (obj_ltype_control, LTYPE_CONTROL);
dwg_get_OBJECT_DECL (obj_ltype, LTYPE);
dwg_get_OBJECT_DECL (obj_view_control, VIEW_CONTROL);
dwg_get_OBJECT_DECL (obj_view, VIEW);
dwg_get_OBJECT_DECL (obj_ucs_control, UCS_CONTROL);
dwg_get_OBJECT_DECL (obj_ucs, UCS);
dwg_get_OBJECT_DECL (obj_vport_control, VPORT_CONTROL);
dwg_get_OBJECT_DECL (obj_vport, VPORT);
dwg_get_OBJECT_DECL (obj_appid_control, APPID_CONTROL);
dwg_get_OBJECT_DECL (obj_appid, APPID);
dwg_get_OBJECT_DECL (obj_dimstyle_control, DIMSTYLE_CONTROL);
dwg_get_OBJECT_DECL (obj_dimstyle, DIMSTYLE);
dwg_get_OBJECT_DECL (obj_vport_entity_control, VPORT_ENTITY_CONTROL);
dwg_get_OBJECT_DECL (obj_vport_entity_header, VPORT_ENTITY_HEADER);
dwg_get_OBJECT_DECL (obj_dictionary, DICTIONARY);
dwg_get_OBJECT_DECL (obj_mlinestyle, MLINESTYLE);
dwg_get_OBJECT_DECL (obj_unknown, UNKNOWN_OBJ);
// stable:
dwg_get_OBJECT_DECL (obj_dictionaryvar, DICTIONARYVAR);
dwg_get_OBJECT_DECL (obj_dictionarywdflt, DICTIONARYWDFLT);
dwg_get_OBJECT_DECL (obj_dummy, DUMMY);
dwg_get_OBJECT_DECL (obj_field, FIELD);
dwg_get_OBJECT_DECL (obj_fieldlist, FIELDLIST);
dwg_get_OBJECT_DECL (obj_group, GROUP);
dwg_get_OBJECT_DECL (obj_idbuffer, IDBUFFER);
dwg_get_OBJECT_DECL (obj_imagedef, IMAGEDEF);
dwg_get_OBJECT_DECL (obj_imagedef_reactor, REACTOR);
dwg_get_OBJECT_DECL (obj_layer_index, LAYER_INDEX);
dwg_get_OBJECT_DECL (obj_layout, LAYOUT);
dwg_get_OBJECT_DECL (obj_mleaderstyle, MLEADERSTYLE);
dwg_get_OBJECT_DECL (obj_objectcontextdata, OBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_placeholder, PLACEHOLDER);
dwg_get_OBJECT_DECL (obj_rastervariables, RASTERVARIABLES);
dwg_get_OBJECT_DECL (obj_scale, SCALE);
dwg_get_OBJECT_DECL (obj_sortentstable, SORTENTSTABLE);
dwg_get_OBJECT_DECL (obj_spatial_filter, SPATIAL_FILTER);
dwg_get_OBJECT_DECL (obj_spatial_index, SPATIAL_INDEX);
dwg_get_OBJECT_DECL (obj_wipeoutvariables, WIPEOUTVARIABLES);
dwg_get_OBJECT_DECL (obj_xrecord, XRECORD);
// unstable:
dwg_get_OBJECT_DECL (obj_assocdependency, ASSOCDEPENDENCY);
dwg_get_OBJECT_DECL (obj_assocaligneddimactionbody, ASSOCALIGNEDDIMACTIONBODY);
dwg_get_OBJECT_DECL (obj_assocplanesurfaceactionbody, ASSOCPLANESURFACEACTIONBODY);
dwg_get_OBJECT_DECL (obj_dimassoc, DIMASSOC);
dwg_get_OBJECT_DECL (obj_dbcolor, DBCOLOR);
dwg_get_OBJECT_DECL (obj_dynamicblockpurgepreventer, DYNAMICBLOCKPURGEPREVENTER);
dwg_get_OBJECT_DECL (obj_geodata, GEODATA);
dwg_get_OBJECT_DECL (obj_long_transaction, LONG_TRANSACTION);
dwg_get_OBJECT_DECL (obj_object_ptr, OBJECT_PTR);
dwg_get_OBJECT_DECL (obj_proxy_object, PROXY_OBJECT);
dwg_get_OBJECT_DECL (obj_perssubentmanager, PERSSUBENTMANAGER);
dwg_get_OBJECT_DECL (obj_underlaydefinition, UNDERLAYDEFINITION);
dwg_get_OBJECT_DECL (obj_tablestyle, TABLESTYLE);
dwg_get_OBJECT_DECL (obj_visualstyle, VISUALSTYLE);
#ifdef DEBUG_CLASSES
dwg_get_OBJECT_DECL (obj_tablecontent, TABLECONTENT);
dwg_get_OBJECT_DECL (obj_tablegeometry, TABLEGEOMETRY);
dwg_get_OBJECT_DECL (obj_cellstylemap, CELLSTYLEMAP);
dwg_get_OBJECT_DECL (obj_material, MATERIAL);
dwg_get_OBJECT_DECL (obj_plotsettings, PLOTSETTINGS);
dwg_get_OBJECT_DECL (obj_sun, SUN);
dwg_get_OBJECT_DECL (obj_sunstudy, SUNSTUDY);
dwg_get_OBJECT_DECL (obj_vba_project, VBA_PROJECT);
dwg_get_OBJECT_DECL (obj_acsh_sweep_class, ACSH_SWEEP_CLASS);
dwg_get_OBJECT_DECL (obj_acsh_box_class, ACSH_BOX_CLASS);
dwg_get_OBJECT_DECL (obj_acsh_extrusion_class, ACSH_EXTRUSION_CLASS);
dwg_get_OBJECT_DECL (obj_aldimobjectcontextdata, ALDIMOBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_blkrefobjectcontextdata, BLKREFOBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_leaderobjectcontextdata, LEADEROBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_mleaderobjectcontextdata, MLEADEROBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_mtextattributeobjectcontextdata, MTEXTATRIBUTEOBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_mtextobjectcontextdata, MTEXTOBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_textobjectcontextdata, TEXTOBJECTCONTEXTDATA);
dwg_get_OBJECT_DECL (obj_assocaction, ASSOCACTION);
dwg_get_OBJECT_DECL (obj_assocnetwork, ASSOCNETWORK);
dwg_get_OBJECT_DECL (obj_assocosnappointrefactionparam, ASSOCOSNAPPOINTREFACTIONPARAM);
dwg_get_OBJECT_DECL (obj_assocperssubentmanager, ASSOCPERSSUBENTMANAGER);
dwg_get_OBJECT_DECL (obj_assoc2dconstraintgroup, ASSOC2DCONSTRAINTGROUP);
dwg_get_OBJECT_DECL (obj_evaluation_graph, EVALUATION_GRAPH);
dwg_get_OBJECT_DECL (obj_navisworksmodeldef, NAVISWORKSMODELDEF);
dwg_get_OBJECT_DECL (obj_geomapimage, GEOMAPIMAGE)
dwg_get_OBJECT_DECL (obj_lightlist, LIGHTLIST);
dwg_get_OBJECT_DECL (obj_renderenvironment, RENDERENVIRONMENT);
dwg_get_OBJECT_DECL (obj_renderglobal, RENDERGLOBAL);
dwg_get_OBJECT_DECL (obj_datalink, DATALINK);
dwg_get_OBJECT_DECL (obj_datatable, DATATABLE);
#endif
// unhandled:
// dwg_get_OBJECT_DECL (obj_acdsrecord, ACDSRECORD)
// dwg_get_OBJECT_DECL (obj_acdsschema, ACDSSCHEMA)
// dwg_get_OBJECT_DECL (obj_acdsscope, ACDSSCOPE)
// dwg_get_OBJECT_DECL (obj_acmecommandhistory, ACMECOMMANDHISTORY)
// dwg_get_OBJECT_DECL (obj_acmestatemgr, ACMESTATEMGR)
// dwg_get_OBJECT_DECL (obj_acsh_history_class, ACSH_HISTORY_CLASS)
// dwg_get_OBJECT_DECL (obj_acsh_revolve_class, ACSH_REVOLVE_CLASS)
// dwg_get_OBJECT_DECL (obj_acsh_sphere_class, ACSH_SPHERE_CLASS)
// dwg_get_OBJECT_DECL (obj_arcalignedtext, ARCALIGNEDTEXT)
// dwg_get_OBJECT_DECL (obj_assocgeomdependency, ASSOCGEOMDEPENDENCY)
// dwg_get_OBJECT_DECL (obj_assocosnappointrefactionparam, ASSOCOSNAPPOINTREFACTIONPARAM)
// dwg_get_OBJECT_DECL (obj_assocvertexactionparam, ASSOCVERTEXACTIONPARAM)
// dwg_get_OBJECT_DECL (obj_detailviewstyle, DETAILVIEWSTYLE)
// dwg_get_OBJECT_DECL (obj_documentoptions, DOCUMENTOPTIONS)
// dwg_get_OBJECT_DECL (obj_layerfilter, LAYERFILTER)
// dwg_get_OBJECT_DECL (obj_layoutprintconfig, LAYOUTPRINTCONFIG)
// dwg_get_OBJECT_DECL (obj_leaderobjectcontextdata, LEADEROBJECTCONTEXTDATA)
// dwg_get_OBJECT_DECL (obj_mentalrayrendersettings, MENTALRAYRENDERSETTINGS)
// dwg_get_OBJECT_DECL (obj_npocollection, NPOCOLLECTION)
// dwg_get_OBJECT_DECL (obj_pointcloud, POINTCLOUD)
// dwg_get_OBJECT_DECL (obj_rapidrtrenderenvironment, RAPIDRTRENDERENVIRONMENT)
// dwg_get_OBJECT_DECL (obj_rapidrtrendersettings, RAPIDRTRENDERSETTINGS)
// dwg_get_OBJECT_DECL (obj_rtext, RTEXT)
// dwg_get_OBJECT_DECL (obj_sectionviewstyle, SECTIONVIEWSTYLE)
// dwg_get_OBJECT_DECL (obj_xrefpanelobject, XREFPANELOBJECT)

/********************************************************************
 *                FUNCTIONS TYPE SPECIFIC                            *
 *********************************************************************/

/* Should we accept dwg and entities? or add dwg_header_get_TYPE */
EXPORT dwg_point_2d *dwg_ent_get_POINT2D (const void *restrict _obj,
                                          const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_POINT2D (void *restrict _obj,
                                 const char *restrict fieldname,
                                 const dwg_point_2d *point)
    __nonnull ((1, 2, 3));

EXPORT dwg_point_3d *dwg_ent_get_POINT3D (const void *restrict _obj,
                                          const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_POINT3D (void *restrict _obj,
                                 const char *restrict fieldname,
                                 const dwg_point_3d *point)
    __nonnull ((1, 2, 3));

EXPORT char *dwg_ent_get_STRING (const void *restrict _obj,
                                 const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_STRING (void *restrict _obj,
                                const char *restrict fieldname,
                                const char *restrict string)
    __nonnull ((1, 2, 3));

EXPORT char *dwg_ent_get_UTF8 (const void *restrict _obj,
                               const char *restrict fieldname, int *isnewp)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_UTF8 (void *restrict _obj,
                              const char *restrict fieldname,
                              const char *restrict string)
    __nonnull ((1, 2, 3));

EXPORT BITCODE_BD dwg_ent_get_REAL (const void *restrict _obj,
                                    const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_REAL (void *restrict _obj,
                              const char *restrict fieldname,
                              const BITCODE_BD num) __nonnull ((1, 2));

EXPORT BITCODE_BS dwg_ent_get_INT16 (const void *restrict _obj,
                                     const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_INT16 (void *restrict _obj,
                               const char *restrict fieldname,
                               const BITCODE_BS num) __nonnull ((1, 2));

EXPORT BITCODE_BL dwg_ent_get_INT32 (const void *restrict _obj,
                                     const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_INT32 (void *restrict _obj,
                               const char *restrict fieldname,
                               const BITCODE_BL num) __nonnull ((1, 2));

EXPORT char *dwg_ent_get_BINARY (const void *restrict _obj,
                                 const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_BINARY (void *restrict _obj,
                                const char *restrict fieldname,
                                const char *string) __nonnull ((1, 2, 3));

EXPORT BITCODE_H dwg_ent_get_HANDLE (const void *restrict _obj,
                                     const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_HANDLE (void *restrict _obj,
                                const char *restrict fieldname,
                                const BITCODE_H handle) __nonnull ((1, 2, 3));

EXPORT BITCODE_B dwg_ent_get_BOOL (const void *restrict _obj,
                                   const char *restrict fieldname)
    __nonnull ((1, 2));
EXPORT bool dwg_ent_set_BOOL (void *restrict _obj,
                              const char *restrict fieldname,
                              const BITCODE_B num) __nonnull ((1, 2));

#ifndef SWIGIMPORTED

/********************************************************************
*                    FUNCTIONS FOR CIRCLE ENTITY                    *
*********************************************************************/

// Get/Set the center point of a _dwg_entity_CIRCLE::
EXPORT void dwg_ent_circle_get_center (const dwg_ent_circle *restrict circle,
                                       dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

void dwg_ent_circle_set_center (dwg_ent_circle *restrict circle,
                                const dwg_point_3d *restrict point,
                                int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set the radius of a circle
EXPORT double dwg_ent_circle_get_radius (const dwg_ent_circle *restrict circle,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_circle_set_radius (dwg_ent_circle *restrict circle,
                                       const BITCODE_BD radius,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set the thickness of a circle
EXPORT double
dwg_ent_circle_get_thickness (const dwg_ent_circle *restrict circle,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_circle_set_thickness (dwg_ent_circle *restrict circle,
                                          const double thickness,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set the extrusion of a circle
EXPORT void dwg_ent_circle_get_extrusion (
    const dwg_ent_circle *restrict circle, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_circle_set_extrusion (dwg_ent_circle *restrict circle,
                                          const dwg_point_3d *restrict vector,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
*                    FUNCTIONS FOR LINE ENTITY                      *
********************************************************************/

// Get/Set the start point of a line
EXPORT void dwg_ent_line_get_start_point (const dwg_ent_line *restrict line,
                                          dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_line_set_start_point (dwg_ent_line *restrict line,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set the end point of a line
EXPORT void dwg_ent_line_get_end_point (const dwg_ent_line *restrict line,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_line_set_end_point (dwg_ent_line *restrict line,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set the thickness of a line
EXPORT double dwg_ent_line_get_thickness (const dwg_ent_line *restrict line,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_line_set_thickness (dwg_ent_line *restrict line,
                                        const BITCODE_BD thickness,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set the extrusion of a line
EXPORT void dwg_ent_line_set_extrusion (dwg_ent_line *restrict line,
                                        const dwg_point_3d *restrict vector,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_line_get_extrusion (const dwg_ent_line *restrict line,
                                        dwg_point_3d *restrict vector,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

/********************************************************************
*                    FUNCTIONS FOR ARC ENTITY                       *
********************************************************************/

// Get/Set the center point of a arc
EXPORT void dwg_ent_arc_get_center (const dwg_ent_arc *restrict arc,
                                    dwg_point_3d *restrict point,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_arc_set_center (dwg_ent_arc *restrict arc,
                                    const dwg_point_3d *restrict point,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set the radius of a arc
EXPORT double dwg_ent_arc_get_radius (const dwg_ent_arc *restrict arc,
                                      int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_arc_set_radius (dwg_ent_arc *restrict arc,
                                    const BITCODE_BD radius,
                                    int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set the thickness of arc
EXPORT double dwg_ent_arc_get_thickness (const dwg_ent_arc *restrict arc,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_arc_set_thickness (dwg_ent_arc *restrict arc,
                                       const BITCODE_BD thickness,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set the extrusion of arc
EXPORT void dwg_ent_arc_get_extrusion (const dwg_ent_arc *restrict arc,
                                       dwg_point_3d *restrict vector,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_arc_set_extrusion (dwg_ent_arc *restrict arc,
                                       const dwg_point_3d *restrict vector,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/set start angle arc
EXPORT double dwg_ent_arc_get_start_angle (const dwg_ent_arc *restrict arc,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_arc_set_start_angle (dwg_ent_arc *restrict arc,
                                         const BITCODE_BD start_angle,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/set end angle of arc
EXPORT double dwg_ent_arc_get_end_angle (const dwg_ent_arc *restrict arc,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_arc_set_end_angle (dwg_ent_arc *restrict arc,
                                       const BITCODE_BD end_angle,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
*                   FUNCTIONS FOR ELLIPSE ENTITY                    *
********************************************************************/

// Get/Set center of ellipse
EXPORT void
dwg_ent_ellipse_get_center (const dwg_ent_ellipse *restrict ellipse,
                            dwg_point_3d *restrict point, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ellipse_set_center (dwg_ent_ellipse *restrict ellipse,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set sm axis of ellipse
EXPORT void
dwg_ent_ellipse_get_sm_axis (const dwg_ent_ellipse *restrict ellipse,
                             dwg_point_3d *restrict point, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ellipse_set_sm_axis (dwg_ent_ellipse *restrict ellipse,
                                         const dwg_point_3d *restrict point,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set extrusion of ellipse
EXPORT void dwg_ent_ellipse_get_extrusion (
    const dwg_ent_ellipse *restrict ellipse, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ellipse_set_extrusion (dwg_ent_ellipse *restrict ellipse,
                                           const dwg_point_3d *restrict vector,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set axis ratio of ellipse
EXPORT double
dwg_ent_ellipse_get_axis_ratio (const dwg_ent_ellipse *restrict ellipse,
                                int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ellipse_set_axis_ratio (dwg_ent_ellipse *restrict ellipse,
                                            const double ratio,
                                            int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set start angle of ellipse
EXPORT double
dwg_ent_ellipse_get_start_angle (const dwg_ent_ellipse *restrict ellipse,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ellipse_set_start_angle (dwg_ent_ellipse *restrict ellipse,
                                             const double start_angle,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set end angle of ellipse
EXPORT double
dwg_ent_ellipse_get_end_angle (const dwg_ent_ellipse *restrict ellipse,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ellipse_set_end_angle (dwg_ent_ellipse *restrict ellipse,
                                           const double end_angle,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
*                    FUNCTIONS FOR TEXT ENTITY                      *
********************************************************************/

// Get/Set text of text (utf-8 encoded)
EXPORT void dwg_ent_text_set_text (dwg_ent_text *restrict text,
                                   const char *restrict text_value,
                                   int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char *dwg_ent_text_get_text (const dwg_ent_text *restrict text,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// Get/Set insertion points of text
EXPORT void dwg_ent_text_get_insertion_point (
    const dwg_ent_text *restrict text, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_text_set_insertion_point (
    dwg_ent_text *restrict text, const dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set height of text
EXPORT double dwg_ent_text_get_height (const dwg_ent_text *restrict text,
                                       int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_text_set_height (dwg_ent_text *restrict text,
                                     const double height, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set extrusion of text
EXPORT void dwg_ent_text_get_extrusion (const dwg_ent_text *restrict text,
                                        dwg_point_3d *restrict vector,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_text_set_extrusion (dwg_ent_text *restrict text,
                                        const dwg_point_3d *restrict vector,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set thickness of text
EXPORT double dwg_ent_text_get_thickness (const dwg_ent_text *restrict text,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_text_set_thickness (dwg_ent_text *restrict text,
                                        const double thickness,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set rotation angle of text
EXPORT double dwg_ent_text_get_rotation (const dwg_ent_text *restrict text,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_text_set_rotation (dwg_ent_text *restrict text,
                                       const double angle, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set horizontal alignment angle of text
EXPORT BITCODE_BS dwg_ent_text_get_vert_alignment (
    const dwg_ent_text *restrict text, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_text_set_vert_alignment (dwg_ent_text *restrict text,
                                             const BITCODE_BS alignment,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set vertical alignment of text
EXPORT BITCODE_BS dwg_ent_text_get_horiz_alignment (
    const dwg_ent_text *restrict text, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_text_set_horiz_alignment (dwg_ent_text *restrict text,
                                              const BITCODE_BS alignment,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
*                   FUNCTIONS FOR ATTRIB ENTITY                     *
********************************************************************/

// Get/Set text of attrib (utf-8 encoded)
EXPORT void dwg_ent_attrib_set_text (dwg_ent_attrib *restrict attrib,
                                     const char *restrict text_value,
                                     int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT char *dwg_ent_attrib_get_text (const dwg_ent_attrib *restrict attrib,
                                      int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// Get/Set insertion points of attrib
EXPORT void dwg_ent_attrib_get_insertion_point (
    const dwg_ent_attrib *restrict attrib, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attrib_set_insertion_point (
    dwg_ent_attrib *restrict attrib, const dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set height of attrib
EXPORT double dwg_ent_attrib_get_height (const dwg_ent_attrib *restrict attrib,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attrib_set_height (dwg_ent_attrib *restrict attrib,
                                       const double height,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set extrusion of attrib
EXPORT void dwg_ent_attrib_get_extrusion (
    const dwg_ent_attrib *restrict attrib, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attrib_set_extrusion (dwg_ent_attrib *restrict attrib,
                                          const dwg_point_3d *restrict vector,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set thickness of attrib
EXPORT double
dwg_ent_attrib_get_thickness (const dwg_ent_attrib *restrict attrib,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attrib_set_thickness (dwg_ent_attrib *restrict attrib,
                                          const double thickness,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set rotation angle of attrib
EXPORT double
dwg_ent_attrib_get_rotation (const dwg_ent_attrib *restrict attrib,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attrib_set_rotation (dwg_ent_attrib *restrict attrib,
                                         const double angle,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set horizontal alignment angle of attrib
EXPORT BITCODE_BS dwg_ent_attrib_get_vert_alignment (
    const dwg_ent_attrib *restrict attrib, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attrib_set_vert_alignment (dwg_ent_attrib *restrict attrib,
                                               const BITCODE_BS alignment,
                                               int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set vertical alignment of attrib
EXPORT BITCODE_BS dwg_ent_attrib_get_horiz_alignment (
    const dwg_ent_attrib *restrict attrib, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attrib_set_horiz_alignment (
    dwg_ent_attrib *restrict attrib, const BITCODE_BS alignment,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
*                   FUNCTIONS FOR ATTDEF ENTITY                     *
********************************************************************/

// Get/Set default text of attdef (utf-8 encoded)
EXPORT void dwg_ent_attdef_set_default_value (
    dwg_ent_attdef *restrict attdef, const char *restrict default_value,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char *
dwg_ent_attdef_get_default_value (const dwg_ent_attdef *restrict attdef,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// Get/Set insertion points of attdef
EXPORT void dwg_ent_attdef_get_insertion_point (
    const dwg_ent_attdef *restrict attdef, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attdef_set_insertion_point (
    dwg_ent_attdef *restrict attdef, const dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set height of attdef
EXPORT double dwg_ent_attdef_get_height (const dwg_ent_attdef *restrict attdef,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attdef_set_height (dwg_ent_attdef *restrict attdef,
                                       const double height,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set extrusion of attdef
EXPORT void dwg_ent_attdef_get_extrusion (
    const dwg_ent_attdef *restrict attdef, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attdef_set_extrusion (dwg_ent_attdef *restrict attdef,
                                          const dwg_point_3d *restrict vector,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set thickness of attdef
EXPORT double
dwg_ent_attdef_get_thickness (const dwg_ent_attdef *restrict attdef,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attdef_set_thickness (dwg_ent_attdef *restrict attdef,
                                          const double thickness,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set rotation angle of attdef
EXPORT double
dwg_ent_attdef_get_rotation (const dwg_ent_attdef *restrict attdef,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attdef_set_rotation (dwg_ent_attdef *restrict attdef,
                                         const double angle,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set horizontal alignment angle of attdef
EXPORT BITCODE_BS dwg_ent_attdef_get_vert_alignment (
    const dwg_ent_attdef *restrict attdef, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attdef_set_vert_alignment (dwg_ent_attdef *restrict attdef,
                                               const BITCODE_BS alignment,
                                               int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set vertical alignment of attdef
EXPORT BITCODE_BS dwg_ent_attdef_get_horiz_alignment (
    const dwg_ent_attdef *restrict attdef, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_attdef_set_horiz_alignment (
    dwg_ent_attdef *restrict attdef, const BITCODE_BS alignment,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *                   FUNCTIONS FOR POINT ENTITY                      *
 ********************************************************************/

// Get/Set point of point entity
EXPORT void dwg_ent_point_set_point (dwg_ent_point *restrict point,
                                     const dwg_point_3d *restrict retpoint,
                                     int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_point_get_point (const dwg_ent_point *restrict point,
                                     dwg_point_3d *restrict retpoint,
                                     int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

// Get/Set thickness of point entity
EXPORT double dwg_ent_point_get_thickness (const dwg_ent_point *restrict point,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_point_set_thickness (dwg_ent_point *restrict point,
                                         const double thickness,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set extrusion of point entity
EXPORT void dwg_ent_point_set_extrusion (dwg_ent_point *restrict point,
                                         const dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_point_get_extrusion (const dwg_ent_point *restrict point,
                                         dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

/********************************************************************
 *                   FUNCTIONS FOR SOLID ENTITY                      *
 ********************************************************************/

// Get/Set thickness of solid entity
EXPORT double dwg_ent_solid_get_thickness (const dwg_ent_solid *restrict solid,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_solid_set_thickness (dwg_ent_solid *restrict solid,
                                         const double thickness,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set elevation of solid entity
EXPORT double dwg_ent_solid_get_elevation (const dwg_ent_solid *restrict solid,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_solid_set_elevation (dwg_ent_solid *restrict solid,
                                         const double elevation,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set corner1 of solid entity
EXPORT void dwg_ent_solid_get_corner1 (const dwg_ent_solid *restrict solid,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_solid_set_corner1 (dwg_ent_solid *restrict solid,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set corner2 of solid entity
EXPORT void dwg_ent_solid_get_corner2 (const dwg_ent_solid *restrict solid,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_solid_set_corner2 (dwg_ent_solid *restrict solid,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set corner3 of solid entity
EXPORT void dwg_ent_solid_get_corner3 (const dwg_ent_solid *restrict solid,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_solid_set_corner3 (dwg_ent_solid *restrict solid,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set corner4 of solid entity
EXPORT void dwg_ent_solid_get_corner4 (const dwg_ent_solid *restrict solid,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_solid_set_corner4 (dwg_ent_solid *restrict solid,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set extrusion of solid entity
EXPORT void dwg_ent_solid_get_extrusion (const dwg_ent_solid *restrict solid,
                                         dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_solid_set_extrusion (dwg_ent_solid *restrict solid,
                                         const dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *                   FUNCTIONS FOR BLOCk ENTITY                      *
 ********************************************************************/

// Get/Set text of block entity
EXPORT void dwg_ent_block_set_name (dwg_ent_block *restrict block,
                                    const char *restrict name,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char *dwg_ent_block_get_name (const dwg_ent_block *restrict block,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                    FUNCTIONS FOR RAY ENTITY                       *
 ********************************************************************/

// Get/Set point of ray entity
EXPORT void dwg_ent_ray_get_point (const dwg_ent_ray *restrict ray,
                                   dwg_point_3d *restrict point,
                                   int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ray_set_point (dwg_ent_ray *restrict ray,
                                   const dwg_point_3d *restrict point,
                                   int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set vector of ray entity
EXPORT void dwg_ent_ray_get_vector (const dwg_ent_ray *restrict ray,
                                    dwg_point_3d *restrict vector,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ray_set_vector (dwg_ent_ray *restrict ray,
                                    const dwg_point_3d *restrict vector,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *                   FUNCTIONS FOR XLINE ENTITY                      *
 ********************************************************************/

// Get/Set point of xline entity
EXPORT void dwg_ent_xline_get_point (const dwg_ent_xline *restrict xline,
                                     dwg_point_3d *restrict point,
                                     int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_xline_set_point (dwg_ent_xline *restrict xline,
                                     const dwg_point_3d *restrict point,
                                     int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set vector of xline entity
EXPORT void dwg_ent_xline_get_vector (const dwg_ent_xline *restrict xline,
                                      dwg_point_3d *restrict vector,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_xline_set_vector (dwg_ent_xline *restrict xline,
                                      const dwg_point_3d *restrict vector,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *                   FUNCTIONS FOR TRACE ENTITY                      *
 ********************************************************************/

// Get/Set thickness of trace entity
EXPORT double dwg_ent_trace_get_thickness (const dwg_ent_trace *restrict trace,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_trace_set_thickness (dwg_ent_trace *restrict trace,
                                         const double thickness,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set elevation of trace entity
EXPORT double dwg_ent_trace_get_elevation (const dwg_ent_trace *restrict trace,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_trace_set_elevation (dwg_ent_trace *restrict trace,
                                         const double elevation,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set corner1 of trace entity
EXPORT void dwg_ent_trace_get_corner1 (const dwg_ent_trace *restrict trace,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_trace_set_corner1 (dwg_ent_trace *restrict trace,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set corner2 of trace entity
EXPORT void dwg_ent_trace_get_corner2 (const dwg_ent_trace *restrict trace,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_trace_set_corner2 (dwg_ent_trace *restrict trace,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set corner3 of trace entity
EXPORT void dwg_ent_trace_get_corner3 (const dwg_ent_trace *restrict trace,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_trace_set_corner3 (dwg_ent_trace *restrict trace,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set corner4 of trace entity
EXPORT void dwg_ent_trace_get_corner4 (const dwg_ent_trace *restrict trace,
                                       dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_trace_set_corner4 (dwg_ent_trace *restrict trace,
                                       const dwg_point_2d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set extrusion of trace entity
EXPORT void dwg_ent_trace_get_extrusion (const dwg_ent_trace *restrict trace,
                                         dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_trace_set_extrusion (dwg_ent_trace *restrict trace,
                                         const dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *                 FUNCTIONS FOR VERTEX_3D ENTITY                    *
 ********************************************************************/

// Get/Set flags of vertex_3d entity
EXPORT char dwg_ent_vertex_3d_get_flag (const dwg_ent_vertex_3d *restrict vert,
                                        int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_3d_set_flag (dwg_ent_vertex_3d *restrict vert,
                                        const char flags, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set point of vertex_3d entity
EXPORT void
dwg_ent_vertex_3d_get_point (const dwg_ent_vertex_3d *restrict vert,
                             dwg_point_3d *restrict point, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_3d_set_point (dwg_ent_vertex_3d *restrict vert,
                                         const dwg_point_3d *restrict point,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *               FUNCTIONS FOR VERTEX_MESH ENTITY                    *
 ********************************************************************/

// Get/Set flags of vertex_mesh entity
EXPORT char
dwg_ent_vertex_mesh_get_flag (const dwg_ent_vertex_mesh *restrict vert,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_mesh_set_flag (dwg_ent_vertex_mesh *restrict vert,
                                          const char flags,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set point of vertex_mesh entity
EXPORT void dwg_ent_vertex_mesh_get_point (
    const dwg_ent_vertex_mesh *restrict vert, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_mesh_set_point (dwg_ent_vertex_mesh *restrict vert,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
 ********************************************************************/

// Get/Set flags of vertex_pface entity
EXPORT char
dwg_ent_vertex_pface_get_flag (const dwg_ent_vertex_pface *restrict vert,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_pface_set_flag (dwg_ent_vertex_pface *restrict vert,
                                           const char flags,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set point of vertex_pface entity
EXPORT void dwg_ent_vertex_pface_get_point (
    const dwg_ent_vertex_pface *restrict vert, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_pface_set_point (
    dwg_ent_vertex_pface *restrict vert, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *                 FUNCTIONS FOR VERTEX_2D ENTITY                    *
 ********************************************************************/

// Get/Set flags of vertex_2d entity
EXPORT char dwg_ent_vertex_2d_get_flag (const dwg_ent_vertex_2d *restrict vert,
                                        int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_2d_set_flag (dwg_ent_vertex_2d *restrict vert,
                                        const char flags, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set point of vertex_2d entity
EXPORT void
dwg_ent_vertex_2d_get_point (const dwg_ent_vertex_2d *restrict vert,
                             dwg_point_3d *restrict point, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_2d_set_point (dwg_ent_vertex_2d *restrict vert,
                                         const dwg_point_3d *restrict point,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

// Get/Set start_width of vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_start_width (const dwg_ent_vertex_2d *restrict vert,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_2d_set_start_width (
    dwg_ent_vertex_2d *restrict vert, const double start_width,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set end width of a vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_end_width (const dwg_ent_vertex_2d *restrict vert,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_2d_set_end_width (dwg_ent_vertex_2d *restrict vert,
                                             const double end_width,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set bulge of a vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_bulge (const dwg_ent_vertex_2d *restrict vert,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_2d_set_bulge (dwg_ent_vertex_2d *restrict vert,
                                         const double bulge,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

// Get/Set tanget_direction of a vertex_2d entity
EXPORT double
dwg_ent_vertex_2d_get_tangent_dir (const dwg_ent_vertex_2d *restrict vert,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_vertex_2d_set_tangent_dir (
    dwg_ent_vertex_2d *restrict vert, const double tangent_dir,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *                   FUNCTIONS FOR INSERT ENTITY                     *
 ********************************************************************/

// Get/Set insertion point of insert entity
EXPORT void dwg_ent_insert_get_ins_pt (const dwg_ent_insert *restrict insert,
                                       dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_insert_set_ins_pt (dwg_ent_insert *restrict insert,
                                       const dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_insert_get_scale (const dwg_ent_insert *restrict insert,
                                      dwg_point_3d *restrict point,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_insert_set_scale (dwg_ent_insert *restrict insert,
                                      const dwg_point_3d *restrict point,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_insert_get_rotation (const dwg_ent_insert *restrict insert,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_insert_set_rotation (dwg_ent_insert *restrict insert,
                                         const double rot_ang,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_insert_get_extrusion (
    const dwg_ent_insert *restrict insert, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_insert_set_extrusion (dwg_ent_insert *restrict insert,
                                          const dwg_point_3d *restrict vector,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_insert_has_attribs (dwg_ent_insert *restrict insert,
                                        int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_insert_get_num_owned (
    const dwg_ent_insert *restrict insert, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// TODO dwg_ent_insert_add_owned, dwg_ent_insert_delete_owned

EXPORT dwg_object *
dwg_ent_insert_get_block_header (const dwg_ent_insert *restrict insert,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                  FUNCTIONS FOR MINSERT ENTITY                     *
 ********************************************************************/

EXPORT void
dwg_ent_minsert_get_ins_pt (const dwg_ent_minsert *restrict minsert,
                            dwg_point_3d *restrict point, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_minsert_set_ins_pt (dwg_ent_minsert *restrict minsert,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_minsert_get_scale (const dwg_ent_minsert *restrict minsert,
                                       dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_minsert_set_scale (dwg_ent_minsert *restrict minsert,
                                       const dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_minsert_get_rotation (const dwg_ent_minsert *restrict minsert,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_minsert_set_rotation (dwg_ent_minsert *restrict minsert,
                                          const double rot_ang,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_minsert_get_extrusion (
    const dwg_ent_minsert *restrict minsert, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_minsert_set_extrusion (dwg_ent_minsert *restrict minsert,
                                           const dwg_point_3d *restrict vector,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_minsert_has_attribs (dwg_ent_minsert *restrict minsert,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_minsert_get_num_owned (
    const dwg_ent_minsert *restrict minsert, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// TODO dwg_ent_minsert_add_owned, dwg_ent_insert_delete_owned

EXPORT BITCODE_BL dwg_ent_minsert_get_num_cols (
    const dwg_ent_minsert *restrict minsert, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_minsert_set_num_cols (dwg_ent_minsert *restrict minsert,
                                          const BITCODE_BL num_cols,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BL dwg_ent_minsert_get_num_rows (
    const dwg_ent_minsert *restrict minsert, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_minsert_set_num_rows (dwg_ent_minsert *restrict minsert,
                                          const BITCODE_BL num_rows,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_minsert_get_col_spacing (const dwg_ent_minsert *restrict minsert,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_minsert_set_col_spacing (dwg_ent_minsert *restrict minsert,
                                             const double spacing,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_minsert_get_row_spacing (const dwg_ent_minsert *restrict minsert,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_minsert_set_row_spacing (dwg_ent_minsert *restrict minsert,
                                             const double spacing,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT dwg_object *
dwg_ent_minsert_get_block_header (const dwg_ent_minsert *restrict minsert,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                FUNCTIONS FOR MLINESTYLE OBJECT                    *
 ********************************************************************/

EXPORT char *
dwg_obj_mlinestyle_get_name (const dwg_obj_mlinestyle *restrict mlinestyle,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_obj_mlinestyle_set_name (dwg_obj_mlinestyle *restrict mlinestyle,
                             const char *restrict name, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char *
dwg_obj_mlinestyle_get_desc (const dwg_obj_mlinestyle *restrict mlinestyle,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_obj_mlinestyle_set_desc (dwg_obj_mlinestyle *restrict mlinestyle,
                             const char *restrict desc, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT int
dwg_obj_mlinestyle_get_flag (const dwg_obj_mlinestyle *restrict mlinestyle,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_obj_mlinestyle_set_flag (dwg_obj_mlinestyle *restrict mlinestyle,
                             const int flags, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_obj_mlinestyle_get_start_angle (
    const dwg_obj_mlinestyle *restrict mlinestyle, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_obj_mlinestyle_set_start_angle (dwg_obj_mlinestyle *restrict mlinestyle,
                                    const double startang, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_obj_mlinestyle_get_end_angle (
    const dwg_obj_mlinestyle *restrict mlinestyle, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_obj_mlinestyle_set_end_angle (dwg_obj_mlinestyle *restrict mlinestyle,
                                  const double endang, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_obj_mlinestyle_get_num_lines (
    const dwg_obj_mlinestyle *restrict mlinestyle, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *               FUNCTIONS FOR APPID_CONTROL OBJECT                  *
 *                       The APPID table                             *
 ********************************************************************/

EXPORT BITCODE_BS dwg_obj_appid_control_get_num_entries (
    const dwg_obj_appid_control *restrict appid, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_object_ref *
dwg_obj_appid_control_get_appid (const dwg_obj_appid_control *restrict appid,
                                 const BITCODE_BS index, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *                    FUNCTIONS FOR APPID OBJECT                     *
 *                       An APPID table entry                        *
 ********************************************************************/

EXPORT char *dwg_obj_appid_get_name (const dwg_obj_appid *restrict appid,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_RC dwg_obj_appid_get_flag (const dwg_obj_appid *restrict appid,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_obj_appid_set_flag (dwg_obj_appid *restrict appid,
                                    const BITCODE_RC flag, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT dwg_obj_appid_control *
dwg_obj_appid_get_appid_control (const dwg_obj_appid *restrict appid,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/*******************************************************************
 *            FUNCTIONS FOR ALL DIMENSION ENTITIES                  *
 ********************************************************************/

EXPORT char *dwg_ent_dim_get_block_name (const dwg_ent_dim *restrict dim,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BD dwg_ent_dim_get_elevation (const dwg_ent_dim *restrict dim,
                                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_dim_set_elevation (dwg_ent_dim *restrict dim,
                                       const BITCODE_BD elevation,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT char dwg_ent_dim_get_flag1 (const dwg_ent_dim *restrict dim,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_flag1 (dwg_ent_dim *restrict dim, char flag,
                                   int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BD dwg_ent_dim_get_act_measurement (
    const dwg_ent_dim *restrict dim, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_dim_set_act_measurement (dwg_ent_dim *restrict dim,
                                             const BITCODE_BD act_measurement,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BD dwg_ent_dim_get_horiz_dir (const dwg_ent_dim *restrict dim,
                                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_horiz_dir (dwg_ent_dim *restrict dim,
                                       const BITCODE_BD horiz_dir,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double dwg_ent_dim_get_lspace_factor (const dwg_ent_dim *restrict dim,
                                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_lspace_factor (dwg_ent_dim *restrict dim,
                                           const BITCODE_BD factor,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_dim_get_lspace_style (
    const dwg_ent_dim *restrict dim, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_lspace_style (dwg_ent_dim *restrict dim,
                                          const BITCODE_BS style,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_dim_get_attachment (const dwg_ent_dim *restrict dim,
                                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_attachment (dwg_ent_dim *restrict dim,
                                        const BITCODE_BS attachment,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_dim_set_extrusion (dwg_ent_dim *restrict dim,
                                       const dwg_point_3d *restrict vector,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_dim_get_extrusion (const dwg_ent_dim *restrict dim,
                                       dwg_point_3d *restrict vector,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;
/* (utf-8 encoded) */
EXPORT char *dwg_ent_dim_get_user_text (const dwg_ent_dim *restrict dim,
                                        int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_user_text (dwg_ent_dim *restrict dim,
                                       const char *text, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT double dwg_ent_dim_get_text_rotation (const dwg_ent_dim *restrict dim,
                                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_text_rotation (dwg_ent_dim *restrict dim,
                                           const double rot,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double dwg_ent_dim_get_ins_rotation (const dwg_ent_dim *restrict dim,
                                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_ins_rotation (dwg_ent_dim *restrict dim,
                                          const double rot,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT char dwg_ent_dim_get_flip_arrow1 (const dwg_ent_dim *restrict dim,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_flip_arrow1 (dwg_ent_dim *restrict dim,
                                         const char flip_arrow,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT char dwg_ent_dim_get_flip_arrow2 (const dwg_ent_dim *restrict dim,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_set_flip_arrow2 (dwg_ent_dim *restrict dim,
                                         const char flip_arrow,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_dim_set_text_midpt (dwg_ent_dim *restrict dim,
                                        const dwg_point_2d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_dim_get_text_midpt (const dwg_ent_dim *restrict dim,
                                        dwg_point_2d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_dim_set_ins_scale (dwg_ent_dim *restrict dim,
                                       const dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_dim_get_ins_scale (const dwg_ent_dim *restrict dim,
                                       dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_dim_set_clone_ins_pt (dwg_ent_dim *restrict dim,
                                          const dwg_point_2d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_dim_get_clone_ins_pt (const dwg_ent_dim *restrict dim,
                                          dwg_point_2d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

/********************************************************************
 *            FUNCTIONS FOR ORDINATE DIMENSION ENTITY                *
 ********************************************************************/

EXPORT void dwg_ent_dim_ordinate_set_def_pt (
    dwg_ent_dim_ordinate *ord, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ordinate_get_def_pt (
    const dwg_ent_dim_ordinate *restrict ord, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ordinate_set_feature_location_pt (
    dwg_ent_dim_ordinate *restrict ord, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ordinate_get_feature_location_pt (
    const dwg_ent_dim_ordinate *restrict ord, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ordinate_set_leader_endpt (
    dwg_ent_dim_ordinate *restrict ord, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ordinate_get_leader_endpt (
    const dwg_ent_dim_ordinate *restrict ord, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ordinate_set_flag2 (dwg_ent_dim_ordinate *restrict ord,
                                            const char flag,
                                            int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char
dwg_ent_dim_ordinate_get_flag2 (const dwg_ent_dim_ordinate *restrict ord,
                                int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
 ********************************************************************/

EXPORT void dwg_ent_dim_linear_set_def_pt (dwg_ent_dim_linear *restrict dim,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_linear_get_def_pt (
    const dwg_ent_dim_linear *restrict dim, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_linear_set_13_pt (dwg_ent_dim_linear *restrict dim,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_linear_get_13_pt (
    const dwg_ent_dim_linear *restrict dim, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_linear_set_14_pt (dwg_ent_dim_linear *restrict dim,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_linear_get_14_pt (
    const dwg_ent_dim_linear *restrict dim, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT double
dwg_ent_dim_linear_get_dim_rotation (const dwg_ent_dim_linear *restrict dim,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_dim_linear_set_dim_rotation (dwg_ent_dim_linear *restrict dim,
                                     const double rot, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_dim_linear_get_ext_line_rotation (
    const dwg_ent_dim_linear *restrict dim, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_linear_set_ext_line_rotation (
    dwg_ent_dim_linear *restrict dim, const double rot, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *             FUNCTIONS FOR ALIGNED DIMENSION ENTITY                *
 ********************************************************************/

EXPORT void dwg_ent_dim_aligned_set_def_pt (dwg_ent_dim_aligned *restrict dim,
                                            const dwg_point_3d *restrict point,
                                            int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_aligned_get_def_pt (
    const dwg_ent_dim_aligned *restrict dim, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_aligned_set_13_pt (dwg_ent_dim_aligned *restrict dim,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_aligned_get_13_pt (
    const dwg_ent_dim_aligned *restrict dim, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_aligned_set_14_pt (dwg_ent_dim_aligned *restrict dim,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_aligned_get_14_pt (
    const dwg_ent_dim_aligned *restrict dim, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT double dwg_ent_dim_aligned_get_ext_line_rotation (
    const dwg_ent_dim_aligned *restrict dim, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_aligned_set_ext_line_rotation (
    dwg_ent_dim_aligned *restrict dim, const double rot, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *              FUNCTIONS FOR ANG3PT DIMENSION ENTITY                *
 ********************************************************************/

EXPORT void dwg_ent_dim_ang3pt_set_def_pt (dwg_ent_dim_ang3pt *restrict ang,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang3pt_get_def_pt (
    const dwg_ent_dim_ang3pt *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ang3pt_set_13_pt (dwg_ent_dim_ang3pt *restrict ang,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang3pt_get_13_pt (
    const dwg_ent_dim_ang3pt *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ang3pt_set_14_pt (dwg_ent_dim_ang3pt *restrict ang,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang3pt_get_14_pt (
    const dwg_ent_dim_ang3pt *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ang3pt_set_first_arc_pt (
    dwg_ent_dim_ang3pt *restrict ang, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang3pt_get_first_arc_pt (
    const dwg_ent_dim_ang3pt *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

/********************************************************************
 *              FUNCTIONS FOR ANG2LN DIMENSION ENTITY                *
 ********************************************************************/

EXPORT void dwg_ent_dim_ang2ln_set_def_pt (dwg_ent_dim_ang2ln *restrict ang,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang2ln_get_def_pt (
    const dwg_ent_dim_ang2ln *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ang2ln_set_13_pt (dwg_ent_dim_ang2ln *restrict ang,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang2ln_get_13_pt (
    const dwg_ent_dim_ang2ln *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ang2ln_set_14_pt (dwg_ent_dim_ang2ln *restrict ang,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang2ln_get_14_pt (
    const dwg_ent_dim_ang2ln *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ang2ln_set_first_arc_pt (
    dwg_ent_dim_ang2ln *restrict ang, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang2ln_get_first_arc_pt (
    const dwg_ent_dim_ang2ln *restrict ang, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_ang2ln_set_16_pt (dwg_ent_dim_ang2ln *restrict ang,
                                          const dwg_point_2d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_ang2ln_get_16_pt (
    const dwg_ent_dim_ang2ln *restrict ang, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

/********************************************************************
 *              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
 ********************************************************************/

EXPORT void dwg_ent_dim_radius_set_def_pt (dwg_ent_dim_radius *restrict radius,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_radius_get_def_pt (
    const dwg_ent_dim_radius *restrict radius, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_radius_set_first_arc_pt (
    dwg_ent_dim_radius *restrict radius, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_radius_get_first_arc_pt (
    const dwg_ent_dim_radius *restrict radius, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT double dwg_ent_dim_radius_get_leader_length (
    const dwg_ent_dim_radius *restrict radius, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_dim_radius_set_leader_length (dwg_ent_dim_radius *restrict radius,
                                      const double length, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *             FUNCTIONS FOR DIAMETER DIMENSION ENTITY               *
 ********************************************************************/

EXPORT void dwg_ent_dim_diameter_set_def_pt (
    dwg_ent_dim_diameter *restrict dia, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_diameter_get_def_pt (
    const dwg_ent_dim_diameter *restrict dia, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_diameter_set_first_arc_pt (
    dwg_ent_dim_diameter *restrict dia, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_dim_diameter_get_first_arc_pt (
    const dwg_ent_dim_diameter *restrict dia, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT double dwg_ent_dim_diameter_get_leader_length (
    const dwg_ent_dim_diameter *restrict dia, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_dim_diameter_set_leader_length (
    dwg_ent_dim_diameter *restrict dia, const double leader_len,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *                   FUNCTIONS FOR ENDBLK ENTITY                     *
 ********************************************************************/

/********************************************************************
 *                   FUNCTIONS FOR SEQEND ENTITY                     *
 ********************************************************************/

/********************************************************************
 *                    FUNCTIONS FOR SHAPE ENTITY                     *
 ********************************************************************/

EXPORT void dwg_ent_shape_get_ins_pt (const dwg_ent_shape *restrict shape,
                                      dwg_point_3d *restrict point,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_ins_pt (dwg_ent_shape *restrict shape,
                                      const dwg_point_3d *restrict point,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_shape_get_scale (const dwg_ent_shape *restrict shape,
                                       int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_scale (dwg_ent_shape *restrict shape,
                                     const double scale, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_shape_get_rotation (const dwg_ent_shape *restrict shape,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_rotation (dwg_ent_shape *restrict shape,
                                        const double rotation,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_shape_get_width_factor (const dwg_ent_shape *restrict shape,
                                int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_width_factor (dwg_ent_shape *restrict shape,
                                            const double width_factor,
                                            int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_shape_get_oblique (const dwg_ent_shape *restrict shape,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_oblique (dwg_ent_shape *restrict shape,
                                       const double oblique,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_shape_get_thickness (const dwg_ent_shape *restrict shape,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_thickness (dwg_ent_shape *restrict shape,
                                         const double thickness,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_shape_get_shape_no (const dwg_ent_shape *restrict shape,
                                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_shape_no (dwg_ent_shape *restrict shape,
                                        const BITCODE_BS id, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_shape_get_style_id (const dwg_ent_shape *restrict shape,
                                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_style_id (dwg_ent_shape *restrict shape,
                                        const BITCODE_BS id, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_shape_get_extrusion (const dwg_ent_shape *restrict shape,
                                         dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_shape_set_extrusion (dwg_ent_shape *restrict shape,
                                         const dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *                    FUNCTIONS FOR MTEXT ENTITY                     *
 ********************************************************************/

EXPORT void dwg_ent_mtext_set_insertion_pt (dwg_ent_mtext *restrict mtext,
                                            const dwg_point_3d *restrict point,
                                            int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_mtext_get_insertion_pt (
    const dwg_ent_mtext *restrict mtext, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_extrusion (dwg_ent_mtext *restrict mtext,
                                         const dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_mtext_get_extrusion (const dwg_ent_mtext *restrict mtext,
                                         dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_x_axis_dir (dwg_ent_mtext *restrict mtext,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_mtext_get_x_axis_dir (const dwg_ent_mtext *restrict mtext,
                                          dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_rect_height (dwg_ent_mtext *restrict mtext,
                                           const double rect_height,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_mtext_get_rect_height (const dwg_ent_mtext *restrict mtext,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_rect_width (dwg_ent_mtext *restrict mtext,
                                          const double rect_width,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_mtext_get_rect_width (const dwg_ent_mtext *restrict mtext,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_text_height (dwg_ent_mtext *restrict mtext,
                                           const double text_height,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_mtext_get_text_height (const dwg_ent_mtext *restrict mtext,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BS dwg_ent_mtext_get_attachment (
    const dwg_ent_mtext *restrict mtext, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_attachment (dwg_ent_mtext *restrict mtext,
                                          const BITCODE_BS attachment,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_mtext_get_drawing_dir (
    const dwg_ent_mtext *restrict mtext, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_drawing_dir (dwg_ent_mtext *restrict mtext,
                                           const BITCODE_BS dir,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_mtext_get_extents_height (const dwg_ent_mtext *restrict mtext,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_extents_height (dwg_ent_mtext *restrict mtext,
                                              const double ht,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_mtext_get_extents_width (const dwg_ent_mtext *restrict mtext,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_mtext_set_extents_width (dwg_ent_mtext *restrict mtext,
                                             const double wid,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/* (utf-8 encoded) */
EXPORT char *dwg_ent_mtext_get_text (const dwg_ent_mtext *restrict mtext,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_text (dwg_ent_mtext *restrict mtext, char *text,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_mtext_get_linespace_style (
    const dwg_ent_mtext *restrict mtext, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_linespace_style (dwg_ent_mtext *restrict mtext,
                                               const BITCODE_BS style,
                                               int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_mtext_get_linespace_factor (const dwg_ent_mtext *restrict mtext,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mtext_set_linespace_factor (dwg_ent_mtext *restrict mtext,
                                                const double factor,
                                                int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
 *                   FUNCTIONS FOR LEADER ENTITY                     *
 ********************************************************************/

EXPORT void dwg_ent_leader_set_annot_type (dwg_ent_leader *restrict leader,
                                           const BITCODE_BS type,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_leader_get_annot_type (
    const dwg_ent_leader *restrict leader, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_path_type (dwg_ent_leader *restrict leader,
                                          const BITCODE_BS type,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_leader_get_path_type (
    const dwg_ent_leader *restrict leader, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_leader_get_num_points (
    const dwg_ent_leader *restrict leader, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_origin (dwg_ent_leader *restrict leader,
                                       const dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_leader_get_origin (const dwg_ent_leader *restrict leader,
                                       dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_extrusion (dwg_ent_leader *restrict leader,
                                          const dwg_point_3d *restrict vector,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_leader_get_extrusion (
    const dwg_ent_leader *restrict leader, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_x_direction (
    dwg_ent_leader *restrict leader, const dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_leader_get_x_direction (
    const dwg_ent_leader *restrict leader, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_offset_to_block_ins_pt (
    dwg_ent_leader *restrict leader, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_leader_get_offset_to_block_ins_pt (
    const dwg_ent_leader *restrict leader, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_dimgap (dwg_ent_leader *restrict leader,
                                       const double dimgap,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_leader_get_dimgap (const dwg_ent_leader *restrict leader,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_box_height (dwg_ent_leader *restrict leader,
                                           const double height,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_leader_get_box_height (const dwg_ent_leader *restrict leader,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_box_width (dwg_ent_leader *restrict leader,
                                          const double width,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_leader_get_box_width (const dwg_ent_leader *restrict leader,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_hookline_dir (dwg_ent_leader *restrict leader,
                                             const char dir,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char
dwg_ent_leader_get_hookline_dir (const dwg_ent_leader *restrict leader,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_arrowhead_on (dwg_ent_leader *restrict leader,
                                             const char arrow,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char
dwg_ent_leader_get_arrowhead_on (const dwg_ent_leader *restrict leader,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_arrowhead_type (dwg_ent_leader *restrict leader,
                                               const BITCODE_BS type,
                                               int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_leader_get_arrowhead_type (
    const dwg_ent_leader *restrict leader, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_dimasz (dwg_ent_leader *restrict leader,
                                       const double dimasz,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_leader_get_dimasz (const dwg_ent_leader *restrict leader,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_leader_set_byblock_color (dwg_ent_leader *restrict leader,
                                              const BITCODE_BS color,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_leader_get_byblock_color (
    const dwg_ent_leader *restrict leader, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                  FUNCTIONS FOR TOLERANCE ENTITY                   *
 ********************************************************************/

EXPORT void dwg_ent_tolerance_set_height (dwg_ent_tolerance *restrict tol,
                                          const double height,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_tolerance_get_height (const dwg_ent_tolerance *restrict tol,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_tolerance_set_dimgap (dwg_ent_tolerance *restrict tol,
                                          const double dimgap,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_tolerance_get_dimgap (const dwg_ent_tolerance *restrict tol,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_tolerance_set_ins_pt (dwg_ent_tolerance *restrict tol,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_tolerance_get_ins_pt (
    const dwg_ent_tolerance *restrict tol, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_tolerance_set_x_direction (
    dwg_ent_tolerance *restrict tol, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_tolerance_get_x_direction (
    const dwg_ent_tolerance *restrict tol, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_tolerance_set_extrusion (
    dwg_ent_tolerance *restrict tol, const dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_tolerance_get_extrusion (
    const dwg_ent_tolerance *restrict tol, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

/* (utf-8 encoded) */
EXPORT void dwg_ent_tolerance_set_text_string (dwg_ent_tolerance *restrict tol,
                                               const char *string,
                                               int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char *
dwg_ent_tolerance_get_text_string (const dwg_ent_tolerance *restrict tol,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                   FUNCTIONS FOR LWPOLYLINE ENTITY                    *
 ********************************************************************/

EXPORT BITCODE_BS dwg_ent_lwpline_get_flag (
    const dwg_ent_lwpline *restrict lwpline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_lwpline_set_flag (dwg_ent_lwpline *restrict lwpline,
                                      const char flags, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_lwpline_get_const_width (const dwg_ent_lwpline *restrict lwpline,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_lwpline_set_const_width (dwg_ent_lwpline *restrict lwpline,
                                             const double const_width,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_lwpline_get_elevation (const dwg_ent_lwpline *restrict lwpline,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_lwpline_set_elevation (dwg_ent_lwpline *restrict lwpline,
                                           const double elevation,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_lwpline_get_thickness (const dwg_ent_lwpline *restrict lwpline,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_lwpline_set_thickness (dwg_ent_lwpline *restrict lwpline,
                                           const double thickness,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void
dwg_ent_lwpline_get_extrusion (const dwg_ent_lwpline *restrict lwpline,
                               dwg_point_3d *points, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_lwpline_set_extrusion (dwg_ent_lwpline *restrict lwpline,
                                           const dwg_point_3d *points,
                                           int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_lwpline_get_numpoints (
    const dwg_ent_lwpline *restrict lwpline, int *restrict error)
    __nonnull ((2));

EXPORT BITCODE_BL dwg_ent_lwpline_get_numbulges (
    const dwg_ent_lwpline *restrict lwpline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_lwpline_get_numwidths (
    const dwg_ent_lwpline *restrict lwpline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT double *
dwg_ent_lwpline_get_bulges (const dwg_ent_lwpline *restrict lwpline,
                            int *restrict error) __nonnull ((2));

EXPORT dwg_point_2d *
dwg_ent_lwpline_get_points (const dwg_ent_lwpline *restrict lwpline,
                            int *restrict error) __nonnull ((2));

EXPORT dwg_lwpline_widths *
dwg_ent_lwpline_get_widths (const dwg_ent_lwpline *restrict lwpline,
                            int *restrict error) __nonnull ((2));

/********************************************************************
 *                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
 ********************************************************************/

EXPORT BITCODE_BS dwg_ent_ole2frame_get_type (
    const dwg_ent_ole2frame *restrict frame, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_ole2frame_set_type (dwg_ent_ole2frame *restrict frame,
                            const BITCODE_BS type, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_ole2frame_get_mode (
    const dwg_ent_ole2frame *restrict frame, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ole2frame_set_mode (dwg_ent_ole2frame *restrict frame,
                                        const BITCODE_BS mode,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_ole2frame_get_data_size (
    const dwg_ent_ole2frame *restrict frame, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// backcompat API
#define dwg_ent_ole2frame_get_data_length(a, b) dwg_ent_ole2frame_get_data_size (a, b)

EXPORT BITCODE_TF
dwg_ent_ole2frame_get_data (const dwg_ent_ole2frame *restrict frame,
                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_ole2frame_set_data (dwg_ent_ole2frame *restrict frame,
                                        const BITCODE_TF restrict data,
                                        const BITCODE_BL data_length,
                                        int *restrict error)
    __nonnull ((4)) _deprecated_dynapi_setter;

/********************************************************************
 *                  FUNCTIONS FOR PROXY OBJECT                       *
 ********************************************************************/

EXPORT BITCODE_BL dwg_obj_proxy_get_class_id (
    const dwg_obj_proxy *restrict proxy, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_obj_proxy_set_class_id (dwg_obj_proxy *restrict proxy,
                                        const BITCODE_BL class_id,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_obj_proxy_get_version (
    const dwg_obj_proxy *restrict proxy, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_obj_proxy_set_version (dwg_obj_proxy *restrict proxy,
                                       const BITCODE_BL version,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_B dwg_obj_proxy_get_from_dxf (
    const dwg_obj_proxy *restrict proxy, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_obj_proxy_set_from_dxf (dwg_obj_proxy *restrict proxy,
                                        const BITCODE_B from_dxf,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char *dwg_obj_proxy_get_data (const dwg_obj_proxy *restrict proxy,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_obj_proxy_set_data (dwg_obj_proxy *restrict proxy,
                                    const char *data, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT dwg_object_ref **
dwg_obj_proxy_get_objid_object_handles (const dwg_obj_proxy *restrict proxy,
                                        int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                   FUNCTIONS FOR SPLINE ENTITY                     *
 ********************************************************************/

EXPORT BITCODE_BS dwg_ent_spline_get_scenario (
    const dwg_ent_spline *restrict spline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_scenario (dwg_ent_spline *restrict spline,
                                         const BITCODE_BS scenario,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_spline_get_degree (
    const dwg_ent_spline *restrict spline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_degree (dwg_ent_spline *restrict spline,
                                       const BITCODE_BS degree,
                                       int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_spline_get_fit_tol (const dwg_ent_spline *restrict spline,
                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_fit_tol (dwg_ent_spline *restrict spline,
                                        const int fit_tol, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_spline_get_begin_tan_vector (
    const dwg_ent_spline *restrict spline, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_begin_tan_vector (
    dwg_ent_spline *restrict spline, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_spline_get_end_tan_vector (
    const dwg_ent_spline *restrict spline, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_end_tan_vector (
    dwg_ent_spline *restrict spline, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_spline_get_knot_tol (const dwg_ent_spline *restrict spline,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_knot_tol (dwg_ent_spline *restrict spline,
                                         const double knot_tol,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_spline_get_ctrl_tol (const dwg_ent_spline *restrict spline,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_ctrl_tol (dwg_ent_spline *restrict spline,
                                         const double ctrl_tol,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_spline_get_num_fit_pts (
    const dwg_ent_spline *restrict spline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// TODO: dwg_ent_spline_add_fit_pts, dwg_ent_spline_delete_fit_pts

EXPORT char dwg_ent_spline_get_rational (const dwg_ent_spline *restrict spline,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_rational (dwg_ent_spline *restrict spline,
                                         const char rational,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_spline_get_closed_b (const dwg_ent_spline *restrict spline,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_closed_b (dwg_ent_spline *restrict spline,
                                         const char closed_b,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_spline_get_weighted (const dwg_ent_spline *restrict spline,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_weighted (dwg_ent_spline *restrict spline,
                                         char weighted, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_spline_get_periodic (const dwg_ent_spline *restrict spline,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_spline_set_periodic (dwg_ent_spline *restrict spline,
                                         char periodic, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_spline_get_num_knots (
    const dwg_ent_spline *restrict spline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// TODO: dwg_ent_spline_add_knots, dwg_ent_spline_delete_knots

EXPORT BITCODE_BL dwg_ent_spline_get_num_ctrl_pts (
    const dwg_ent_spline *restrict spline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// TODO: dwg_ent_spline_add_ctrl_pts, dwg_ent_spline_delete_ctrl_pts

EXPORT dwg_point_3d *
dwg_ent_spline_get_fit_pts (const dwg_ent_spline *restrict spline,
                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_spline_control_point *
dwg_ent_spline_get_ctrl_pts (const dwg_ent_spline *restrict spline,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT double *dwg_ent_spline_get_knots (const dwg_ent_spline *restrict spline,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                   FUNCTIONS FOR VIEWPORT ENTITY                   *
 ********************************************************************/

EXPORT void dwg_ent_viewport_get_center (const dwg_ent_viewport *restrict vp,
                                         dwg_point_3d *restrict point,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_center (dwg_ent_viewport *restrict vp,
                                         const dwg_point_3d *restrict point,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_viewport_get_width (const dwg_ent_viewport *restrict vp,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_width (dwg_ent_viewport *restrict vp,
                                        const double width,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_viewport_get_height (const dwg_ent_viewport *restrict vp,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_height (dwg_ent_viewport *restrict vp,
                                         const double height,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_viewport_get_grid_major (
    const dwg_ent_viewport *restrict vp, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_grid_major (dwg_ent_viewport *restrict vp,
                                             const BITCODE_BS major,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_viewport_get_num_frozen_layers (
    const dwg_ent_viewport *restrict vp, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_num_frozen_layers (
    dwg_ent_viewport *restrict vp, const BITCODE_BL count, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char *
dwg_ent_viewport_get_style_sheet (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_style_sheet (dwg_ent_viewport *restrict vp,
                                              char *sheet, int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_set_circle_zoom (dwg_ent_viewport *restrict vp,
                                              const BITCODE_BS zoom,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_viewport_get_circle_zoom (
    const dwg_ent_viewport *restrict vp, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_status_flag (dwg_ent_viewport *restrict vp,
                                              const BITCODE_BL flags,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_viewport_get_status_flag (
    const dwg_ent_viewport *restrict vp, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT char
dwg_ent_viewport_get_render_mode (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_render_mode (dwg_ent_viewport *restrict vp,
                                              char mode, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_set_ucs_at_origin (dwg_ent_viewport *restrict vp,
                                                unsigned char origin,
                                                int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT unsigned char
dwg_ent_viewport_get_ucs_at_origin (const dwg_ent_viewport *restrict vp,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_ucs_per_viewport (
    dwg_ent_viewport *restrict vp, unsigned char viewport, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT unsigned char
dwg_ent_viewport_get_ucs_per_viewport (const dwg_ent_viewport *restrict vp,
                                       int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_view_target (
    dwg_ent_viewport *restrict vp, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_viewport_get_view_target (
    const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_view_direction (
    dwg_ent_viewport *restrict vp, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_viewport_get_view_direction (
    const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_view_twist (dwg_ent_viewport *restrict vp,
                                             const double angle,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_viewport_get_view_twist (const dwg_ent_viewport *restrict vp,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_view_height (dwg_ent_viewport *restrict vp,
                                              const double height,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_viewport_get_view_height (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_lens_length (dwg_ent_viewport *restrict vp,
                                              const double length,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_viewport_get_lens_length (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_front_clip_z (dwg_ent_viewport *restrict vp,
                                               const double front_z,
                                               int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_viewport_get_front_clip_z (const dwg_ent_viewport *restrict vp,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_back_clip_z (dwg_ent_viewport *restrict vp,
                                              const double back_z,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_viewport_get_back_clip_z (const dwg_ent_viewport *restrict vp,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_snap_angle (dwg_ent_viewport *restrict vp,
                                             const double angle,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_viewport_get_snap_angle (const dwg_ent_viewport *restrict vp,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_get_view_center (
    const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_view_center (
    dwg_ent_viewport *restrict vp, const dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_get_grid_spacing (
    const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_grid_spacing (
    dwg_ent_viewport *restrict vp, const dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_get_snap_base (
    const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_snap_base (dwg_ent_viewport *restrict vp,
                                            const dwg_point_2d *restrict point,
                                            int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_get_snap_spacing (
    const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_viewport_set_snap_spacing (
    dwg_ent_viewport *restrict vp, const dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_set_ucs_origin (
    dwg_ent_viewport *restrict vp, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_get_ucs_origin (
    const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_ucs_x_axis (
    dwg_ent_viewport *restrict vp, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_viewport_get_ucs_x_axis (
    const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_ucs_y_axis (
    dwg_ent_viewport *restrict vp, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_viewport_get_ucs_y_axis (
    const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_ucs_elevation (dwg_ent_viewport *restrict vp,
                                                const double elevation,
                                                int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_viewport_get_ucs_elevation (const dwg_ent_viewport *restrict vp,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_ucs_ortho_view_type (
    dwg_ent_viewport *restrict vp, const BITCODE_BS type, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_viewport_get_ucs_ortho_view_type (
    const dwg_ent_viewport *restrict vp, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_shadeplot_mode (dwg_ent_viewport *restrict vp,
                                                 const BITCODE_BS shadeplot,
                                                 int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_viewport_get_shadeplot_mode (
    const dwg_ent_viewport *restrict vp, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_use_default_lights (
    dwg_ent_viewport *restrict vp, const unsigned char lights,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT unsigned char
dwg_ent_viewport_get_use_default_lights (const dwg_ent_viewport *restrict vp,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_default_lighting_type (
    dwg_ent_viewport *restrict vp, const char type, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_viewport_get_default_lighting_type (
    const dwg_ent_viewport *restrict vp, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_brightness (dwg_ent_viewport *restrict vp,
                                             const double brightness,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_viewport_get_brightness (const dwg_ent_viewport *restrict vp,
                                 int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_viewport_set_contrast (dwg_ent_viewport *restrict vp,
                                           const double contrast,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_viewport_get_contrast (const dwg_ent_viewport *restrict vp,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                FUNCTIONS FOR POLYLINE PFACE ENTITY                *
 ********************************************************************/

EXPORT BITCODE_BS dwg_ent_polyline_pface_get_numpoints (
    const dwg_ent_polyline_pface *restrict pface, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BS dwg_ent_polyline_pface_get_numfaces (
    const dwg_ent_polyline_pface *restrict pface, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/* not implemented. use the dynapi instead */
EXPORT dwg_point_3d *
dwg_ent_polyline_pface_get_points (const dwg_object *restrict obj,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                FUNCTIONS FOR POLYLINE_MESH ENTITY                 *
 ********************************************************************/

EXPORT BITCODE_BS dwg_ent_polyline_mesh_get_flag (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_polyline_mesh_set_flag (dwg_ent_polyline_mesh *restrict mesh,
                                const BITCODE_BS flags, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_polyline_mesh_get_curve_type (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_polyline_mesh_set_curve_type (
    dwg_ent_polyline_mesh *restrict mesh, const BITCODE_BS curve_type,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_polyline_mesh_get_num_m_verts (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BS dwg_ent_polyline_mesh_get_num_n_verts (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BS dwg_ent_polyline_mesh_get_m_density (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_polyline_mesh_set_m_density (
    dwg_ent_polyline_mesh *restrict mesh, const BITCODE_BS m_density,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_polyline_mesh_get_n_density (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_polyline_mesh_set_n_density (
    dwg_ent_polyline_mesh *restrict mesh, const BITCODE_BS n_density,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_polyline_mesh_get_num_owned (
    const dwg_ent_polyline_mesh *restrict mesh, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// TODO dwg_ent_*_add_owned, dwg_ent_insert_delete_owned

/********************************************************************
 *                 FUNCTIONS FOR POLYLINE_2D ENTITY                  *
 ********************************************************************/

EXPORT void dwg_ent_polyline_2d_get_extrusion (
    const dwg_ent_polyline_2d *restrict pline2d, dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_polyline_2d_set_extrusion (
    dwg_ent_polyline_2d *restrict pline2d, const dwg_point_3d *restrict vector,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_polyline_2d_get_start_width (
    const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_polyline_2d_set_start_width (
    dwg_ent_polyline_2d *restrict pline2d, const double start_width,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_polyline_2d_get_end_width (const dwg_ent_polyline_2d *restrict pline2d,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_polyline_2d_set_end_width (dwg_ent_polyline_2d *restrict pline2d,
                                   const double end_width, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_polyline_2d_get_thickness (const dwg_ent_polyline_2d *restrict pline2d,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_polyline_2d_set_thickness (dwg_ent_polyline_2d *restrict pline2d,
                                   const double thickness, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_polyline_2d_get_elevation (const dwg_ent_polyline_2d *restrict pline2d,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_polyline_2d_set_elevation (dwg_ent_polyline_2d *restrict pline2d,
                                   const double elevation, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_polyline_2d_get_flag (
    const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_polyline_2d_set_flag (dwg_ent_polyline_2d *restrict pline2d,
                              const BITCODE_BS flags, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_polyline_2d_get_curve_type (
    const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_polyline_2d_set_curve_type (
    dwg_ent_polyline_2d *restrict pline2d, const BITCODE_BS curve_type,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_object_polyline_2d_get_numpoints (
    const dwg_object *restrict obj, int *restrict error) __nonnull ((2));

EXPORT dwg_point_2d *
dwg_object_polyline_2d_get_points (const dwg_object *restrict obj,
                                   int *restrict error) __nonnull ((2));

/********************************************************************
 *                 FUNCTIONS FOR POLYLINE_3D ENTITY                  *
 ********************************************************************/

EXPORT BITCODE_RC dwg_ent_polyline_3d_get_flag (
    const dwg_ent_polyline_3d *restrict pline3d, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_polyline_3d_set_flag (dwg_ent_polyline_3d *restrict pline3d,
                              const BITCODE_RC flag, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_RC dwg_ent_polyline_3d_get_curve_type (
    const dwg_ent_polyline_3d *restrict pline3d, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_polyline_3d_set_curve_type (
    dwg_ent_polyline_3d *restrict pline3d, const BITCODE_RC curve_type,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_object_polyline_3d_get_numpoints (
    const dwg_object *restrict obj, int *restrict error) __nonnull ((2));

EXPORT dwg_point_3d *
dwg_object_polyline_3d_get_points (const dwg_object *restrict obj,
                                   int *restrict error) __nonnull ((2));

/********************************************************************
 *                   FUNCTIONS FOR 3DFACE ENTITY                     *
 ********************************************************************/

EXPORT BITCODE_BS dwg_ent_3dface_get_invis_flags (
    const dwg_ent_3dface *restrict _3dface, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_3dface_set_invis_flags (dwg_ent_3dface *restrict _3dface,
                                            const BITCODE_BS invis_flags,
                                            int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_3dface_get_corner1 (const dwg_ent_3dface *restrict _3dface,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_3dface_set_corner1 (dwg_ent_3dface *restrict _3dface,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_3dface_get_corner2 (const dwg_ent_3dface *restrict _3dface,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_3dface_set_corner2 (dwg_ent_3dface *restrict _3dface,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_3dface_get_corner3 (const dwg_ent_3dface *restrict _3dface,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_3dface_set_corner3 (dwg_ent_3dface *restrict _3dface,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_3dface_get_corner4 (const dwg_ent_3dface *restrict _3dface,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_3dface_set_corner4 (dwg_ent_3dface *restrict _3dface,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

/********************************************************************
 *                    FUNCTIONS FOR IMAGE ENTITY                     *
 ********************************************************************/

EXPORT BITCODE_BL dwg_ent_image_get_class_version (
    const dwg_ent_image *restrict image, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_class_version (dwg_ent_image *restrict image,
                                             const BITCODE_BL class_version,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_image_get_pt0 (const dwg_ent_image *restrict image,
                                   dwg_point_3d *restrict point,
                                   int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_pt0 (dwg_ent_image *restrict image,
                                   const dwg_point_3d *restrict point,
                                   int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_image_get_u_vector (const dwg_ent_image *restrict image,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_u_vector (dwg_ent_image *restrict image,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_image_get_v_vector (const dwg_ent_image *restrict image,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_v_vector (dwg_ent_image *restrict image,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_image_get_size_height (const dwg_ent_image *restrict image,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_size_height (dwg_ent_image *restrict image,
                                           const double size_height,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_image_get_size_width (const dwg_ent_image *restrict image,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_size_width (dwg_ent_image *restrict image,
                                          const double size_width,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_image_get_display_props (
    const dwg_ent_image *restrict image, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_display_props (dwg_ent_image *restrict image,
                                             const BITCODE_BS display_props,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_B dwg_ent_image_get_clipping (
    const dwg_ent_image *restrict image, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_clipping (dwg_ent_image *restrict image,
                                        const BITCODE_B clipping,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_image_get_brightness (const dwg_ent_image *restrict image,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_brightness (dwg_ent_image *restrict image,
                                          const char brightness,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_image_get_contrast (const dwg_ent_image *restrict image,
                                        int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_contrast (dwg_ent_image *restrict image,
                                        const char contrast,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_image_get_fade (const dwg_ent_image *restrict image,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_fade (dwg_ent_image *restrict image,
                                    const char fade, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_image_get_clip_boundary_type (
    const dwg_ent_image *restrict image, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_clip_boundary_type (
    dwg_ent_image *restrict image, const BITCODE_BS type, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_image_get_boundary_pt0 (
    const dwg_ent_image *restrict image, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_boundary_pt0 (dwg_ent_image *restrict image,
                                            const dwg_point_2d *restrict point,
                                            int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_image_get_boundary_pt1 (
    const dwg_ent_image *restrict image, dwg_point_2d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_image_set_boundary_pt1 (dwg_ent_image *restrict image,
                                            const dwg_point_2d *restrict point,
                                            int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT double
dwg_ent_image_get_num_clip_verts (const dwg_ent_image *restrict image,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_2RD *
dwg_ent_image_get_clip_verts (const dwg_ent_image *restrict image,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                    FUNCTIONS FOR MLINE ENTITY                     *
 ********************************************************************/

EXPORT void dwg_ent_mline_set_scale (dwg_ent_mline *restrict mline,
                                     const double scale, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT double dwg_ent_mline_get_scale (const dwg_ent_mline *restrict mline,
                                       int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mline_set_justification (dwg_ent_mline *restrict mline,
                                             const BITCODE_RC just,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_RC dwg_ent_mline_get_justification (
    const dwg_ent_mline *restrict mline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mline_set_base_point (dwg_ent_mline *restrict mline,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_mline_get_base_point (const dwg_ent_mline *restrict mline,
                                          dwg_point_3d *restrict point,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mline_set_extrusion (dwg_ent_mline *restrict mline,
                                         const dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_mline_get_extrusion (const dwg_ent_mline *restrict mline,
                                         dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_mline_set_flags (dwg_ent_mline *restrict mline,
                                     const BITCODE_BS oc, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_mline_get_flags (const dwg_ent_mline *restrict mline,
                                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_RC dwg_ent_mline_get_num_lines (
    const dwg_ent_mline *restrict mline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_mline_line *
dwg_mline_vertex_get_lines (const dwg_mline_vertex *restrict vertex,
                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BS dwg_ent_mline_get_num_verts (
    const dwg_ent_mline *restrict mline, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_mline_vertex *
dwg_ent_mline_get_verts (const dwg_ent_mline *restrict mline,
                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
 *                  FUNCTIONS FOR 3DSOLID ENTITY                     *
 ********************************************************************/

EXPORT unsigned char
dwg_ent_3dsolid_get_acis_empty (const dwg_ent_3dsolid *restrict _3dsolid,
                                int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT void dwg_ent_3dsolid_set_acis_empty (dwg_ent_3dsolid *restrict _3dsolid,
                                            const unsigned char acis,
                                            int *restrict error)
    __nonnull ((3)) /*_deprecated_dynapi_setter*/;

EXPORT BITCODE_BS dwg_ent_3dsolid_get_version (
    const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT unsigned char *
dwg_ent_3dsolid_get_acis_data (const dwg_ent_3dsolid *restrict _3dsolid,
                               int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT void dwg_ent_3dsolid_set_acis_data (
    dwg_ent_3dsolid *restrict _3dsolid, const unsigned char *restrict sat_data,
    int *restrict error) __nonnull ((2, 3)) /*_deprecated_dynapi_setter*/;

EXPORT char dwg_ent_3dsolid_get_wireframe_data_present (
    const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT void dwg_ent_3dsolid_set_wireframe_data_present (
    dwg_ent_3dsolid *restrict _3dsolid, const char present,
    int *restrict error) __nonnull ((3)) /*_deprecated_dynapi_setter*/;

EXPORT char
dwg_ent_3dsolid_get_point_present (const dwg_ent_3dsolid *restrict _3dsolid,
                                   int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT void
dwg_ent_3dsolid_set_point_present (dwg_ent_3dsolid *restrict _3dsolid,
                                   const char present, int *restrict error)
    __nonnull ((3)) /*_deprecated_dynapi_setter*/;

EXPORT void
dwg_ent_3dsolid_get_point (const dwg_ent_3dsolid *restrict _3dsolid,
                           dwg_point_3d *restrict point, int *restrict error)
    __nonnull ((2, 3)) /*_deprecated_dynapi_getter*/;

EXPORT void dwg_ent_3dsolid_set_point (dwg_ent_3dsolid *restrict _3dsolid,
                                       const dwg_point_3d *restrict point,
                                       int *restrict error)
    __nonnull ((2, 3)) /*_deprecated_dynapi_setter*/;

EXPORT BITCODE_BL dwg_ent_3dsolid_get_num_isolines (
    const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT void
dwg_ent_3dsolid_set_num_isolines (dwg_ent_3dsolid *restrict _3dsolid,
                                  const BITCODE_BL num, int *restrict error)
    __nonnull ((3)) /*_deprecated_dynapi_setter*/;

EXPORT char
dwg_ent_3dsolid_get_isoline_present (const dwg_ent_3dsolid *restrict _3dsolid,
                                     int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT void
dwg_ent_3dsolid_set_isoline_present (dwg_ent_3dsolid *restrict _3dsolid,
                                     const char present, int *restrict error)
    __nonnull ((3)) /*_deprecated_dynapi_setter*/;

EXPORT BITCODE_BL dwg_ent_3dsolid_get_num_wires (
    const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT dwg_3dsolid_wire *
dwg_ent_3dsolid_get_wires (const dwg_ent_3dsolid *restrict _3dsolid,
                           int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT BITCODE_BL dwg_ent_3dsolid_get_num_silhouettes (
    const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT dwg_3dsolid_silhouette *
dwg_ent_3dsolid_get_silhouettes (const dwg_ent_3dsolid *restrict _3dsolid,
                                 int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT unsigned char
dwg_ent_3dsolid_get_acis_empty2 (const dwg_ent_3dsolid *restrict _3dsolid,
                                 int *restrict error)
    __nonnull ((2)) /*_deprecated_dynapi_getter*/;

EXPORT void dwg_ent_3dsolid_set_acis_empty2 (
    dwg_ent_3dsolid *restrict _3dsolid, const unsigned char empty2,
    int *restrict error) __nonnull ((3)) /*_deprecated_dynapi_setter*/;

/********************************************************************
 *                   FUNCTIONS FOR REGION ENTITY                     *
 ********************************************************************/

EXPORT unsigned char
dwg_ent_region_get_acis_empty (const dwg_ent_region *restrict region,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_region_set_acis_empty (dwg_ent_region *restrict region,
                                           const unsigned char empty,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_region_get_version (
    const dwg_ent_region *restrict region, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT unsigned char *
dwg_ent_region_get_acis_data (const dwg_ent_region *restrict region,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_region_set_acis_data (dwg_ent_region *restrict region,
                                          const unsigned char *restrict data,
                                          int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_region_get_wireframe_data_present (
    const dwg_ent_region *restrict region, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_region_set_wireframe_data_present (
    dwg_ent_region *restrict region, const char present, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char
dwg_ent_region_get_point_present (const dwg_ent_region *restrict region,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_region_set_point_present (dwg_ent_region *restrict region,
                                              const char present,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_region_get_point (const dwg_ent_region *restrict region,
                                      dwg_point_3d *restrict point,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_region_set_point (dwg_ent_region *restrict region,
                                      const dwg_point_3d *restrict point,
                                      int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_region_get_num_isolines (
    const dwg_ent_region *restrict region, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_region_set_num_isolines (dwg_ent_region *restrict region,
                                             const BITCODE_BL num,
                                             int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char
dwg_ent_region_get_isoline_present (const dwg_ent_region *restrict region,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_region_set_isoline_present (dwg_ent_region *restrict region,
                                    const char present, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_region_get_num_wires (
    const dwg_ent_region *restrict region, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_3dsolid_wire *
dwg_ent_region_get_wires (const dwg_ent_region *restrict region,
                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_region_get_num_silhouettes (
    const dwg_ent_region *restrict region, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_3dsolid_silhouette *
dwg_ent_region_get_silhouettes (const dwg_ent_region *restrict region,
                                int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT unsigned char
dwg_ent_region_get_acis_empty2 (const dwg_ent_region *restrict region,
                                int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_region_set_acis_empty2 (dwg_ent_region *restrict region,
                                            const unsigned char empty2,
                                            int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
*                    FUNCTIONS FOR BODY ENTITY                      *
********************************************************************/

EXPORT unsigned char
dwg_ent_body_get_acis_empty (const dwg_ent_body *restrict body,
                             int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_acis_empty (dwg_ent_body *restrict body,
                                         const unsigned char empty,
                                         int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_body_get_version (const dwg_ent_body *restrict body,
                                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT unsigned char *
dwg_ent_body_get_acis_data (const dwg_ent_body *restrict body,
                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_acis_data (dwg_ent_body *restrict body,
                                        const unsigned char *data,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char
dwg_ent_body_get_wireframe_data_present (const dwg_ent_body *restrict body,
                                         int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_wireframe_data_present (
    dwg_ent_body *restrict body, const char present, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char dwg_ent_body_get_point_present (const dwg_ent_body *restrict body,
                                            int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_point_present (dwg_ent_body *restrict body,
                                            const char present,
                                            int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_body_get_point (const dwg_ent_body *restrict body,
                                    dwg_point_3d *restrict point,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_point (dwg_ent_body *restrict body,
                                    const dwg_point_3d *restrict point,
                                    int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_body_get_num_isolines (
    const dwg_ent_body *restrict body, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_num_isolines (dwg_ent_body *restrict body,
                                           const BITCODE_BL num,
                                           int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT char
dwg_ent_body_get_isoline_present (const dwg_ent_body *restrict body,
                                  int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_isoline_present (dwg_ent_body *restrict body,
                                              const char present,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_body_get_num_wires (
    const dwg_ent_body *restrict body, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_3dsolid_wire *
dwg_ent_body_get_wires (const dwg_ent_body *restrict body, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_body_get_num_silhouettes (
    const dwg_ent_body *restrict body, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_3dsolid_silhouette *
dwg_ent_body_get_silhouettes (const dwg_ent_body *restrict body,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT unsigned char
dwg_ent_body_get_acis_empty2 (const dwg_ent_body *restrict body,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_body_set_acis_empty2 (dwg_ent_body *restrict body,
                                          unsigned char empty2,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

/********************************************************************
*                    FUNCTIONS FOR TABLE ENTITY                     *
********************************************************************/

EXPORT void dwg_ent_table_set_insertion_point (
    dwg_ent_table *restrict table, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_table_get_insertion_point (
    const dwg_ent_table *restrict table, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_scale (dwg_ent_table *restrict table,
                                     const dwg_point_3d *restrict point,
                                     int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_table_get_scale (const dwg_ent_table *restrict table,
                                     dwg_point_3d *restrict point,
                                     int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_flags (dwg_ent_table *restrict table,
                                          const unsigned char flags,
                                          int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT unsigned char
dwg_ent_table_get_data_flags (const dwg_ent_table *restrict table,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_rotation (dwg_ent_table *restrict table,
                                        const BITCODE_BD rotation,
                                        int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double dwg_ent_table_get_rotation (const dwg_ent_table *restrict table,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_extrusion (dwg_ent_table *restrict table,
                                         const dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT void dwg_ent_table_get_extrusion (const dwg_ent_table *restrict table,
                                         dwg_point_3d *restrict vector,
                                         int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT unsigned char dwg_ent_table_has_attribs (dwg_ent_table *restrict table,
                                                int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_table_get_num_owned (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// TODO dwg_ent_*_add_owned, dwg_ent_insert_delete_owned

EXPORT void dwg_ent_table_set_flag_for_table_value (
    dwg_ent_table *restrict table, const BITCODE_BS value, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_flag_for_table_value (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_horiz_direction (
    dwg_ent_table *restrict table, const dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;
EXPORT void dwg_ent_table_get_horiz_direction (
    const dwg_ent_table *restrict table, dwg_point_3d *restrict point,
    int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_table_get_num_cols (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BL dwg_ent_table_get_num_rows (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT double *
dwg_ent_table_get_col_widths (const dwg_ent_table *restrict table,
                              int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT double *
dwg_ent_table_get_row_heights (const dwg_ent_table *restrict table,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_B dwg_ent_table_has_table_overrides (
    dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_table_flag_override (
    dwg_ent_table *restrict table, const BITCODE_BL override,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BL dwg_ent_table_get_table_flag_override (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_suppressed (dwg_ent_table *restrict table,
                                                const unsigned char title,
                                                int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT unsigned char
dwg_ent_table_get_title_suppressed (const dwg_ent_table *restrict table,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_suppressed (dwg_ent_table *restrict table,
                                                 const unsigned char header,
                                                 int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT unsigned char
dwg_ent_table_get_header_suppressed (const dwg_ent_table *restrict table,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_flow_direction (dwg_ent_table *restrict table,
                                              const BITCODE_BS dir,
                                              int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_flow_direction (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_horiz_cell_margin (dwg_ent_table *restrict table,
                                                 const BITCODE_BD margin,
                                                 int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_table_get_horiz_cell_margin (const dwg_ent_table *restrict table,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_vert_cell_margin (dwg_ent_table *restrict table,
                                                const BITCODE_BD margin,
                                                int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_table_get_vert_cell_margin (const dwg_ent_table *restrict table,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_row_fill_none (
    dwg_ent_table *restrict table, const unsigned char fill,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT unsigned char
dwg_ent_table_get_title_row_fill_none (const dwg_ent_table *restrict table,
                                       int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_row_fill_none (
    dwg_ent_table *restrict table, unsigned char fill, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT unsigned char
dwg_ent_table_get_header_row_fill_none (const dwg_ent_table *restrict table,
                                        int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_row_fill_none (
    dwg_ent_table *restrict table, const unsigned char fill,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT unsigned char
dwg_ent_table_get_data_row_fill_none (const dwg_ent_table *restrict table,
                                      int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_row_alignment (
    dwg_ent_table *restrict table, const unsigned char fill,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_title_row_alignment (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_row_alignment (
    dwg_ent_table *restrict table, const BITCODE_BS align, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_row_alignment (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_row_alignment (
    dwg_ent_table *restrict table, const BITCODE_BS align, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_row_alignment (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_row_height (dwg_ent_table *restrict table,
                                                const BITCODE_BD height,
                                                int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_table_get_title_row_height (const dwg_ent_table *restrict table,
                                    int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_row_height (dwg_ent_table *restrict table,
                                                 const BITCODE_BD height,
                                                 int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_table_get_header_row_height (const dwg_ent_table *restrict table,
                                     int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_row_height (dwg_ent_table *restrict table,
                                               const BITCODE_BD height,
                                               int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT double
dwg_ent_table_get_data_row_height (const dwg_ent_table *restrict table,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT unsigned char
dwg_ent_table_has_border_color_overrides (dwg_ent_table *restrict table,
                                          int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_border_color_overrides_flag (
    dwg_ent_table *restrict table, const BITCODE_BL overrides,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BL dwg_ent_table_get_border_color_overrides_flag (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT unsigned char
dwg_ent_table_has_border_lineweight_overrides (dwg_ent_table *restrict table,
                                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_border_lineweight_overrides_flag (
    dwg_ent_table *restrict table, const BITCODE_BL overrides,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BL dwg_ent_table_get_border_lineweight_overrides_flag (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_horiz_top_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_title_horiz_top_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_horiz_ins_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_title_horiz_ins_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_horiz_bottom_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_title_horiz_bottom_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_vert_left_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_title_vert_left_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_vert_ins_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_title_vert_ins_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_vert_right_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_title_vert_right_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_horiz_top_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_horiz_top_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_horiz_ins_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_horiz_ins_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_horiz_bottom_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_horiz_bottom_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_vert_left_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_vert_left_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_vert_ins_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_vert_ins_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_vert_right_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_vert_right_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_horiz_top_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_horiz_top_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_horiz_ins_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_horiz_ins_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_horiz_bottom_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_horiz_bottom_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_vert_left_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_vert_left_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_vert_ins_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_vert_ins_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_vert_right_linewt (
    dwg_ent_table *restrict table, const BITCODE_BS linewt,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_vert_right_linewt (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT unsigned char
dwg_ent_table_has_border_visibility_overrides (dwg_ent_table *restrict table,
                                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_border_visibility_overrides_flag (
    dwg_ent_table *restrict table, const BITCODE_BL overrides,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_ent_table_get_border_visibility_overrides_flag (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_horiz_top_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_title_horiz_top_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_horiz_ins_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_title_horiz_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_horiz_bottom_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_title_horiz_bottom_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_vert_left_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_title_vert_left_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_vert_ins_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_title_vert_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_title_vert_right_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_title_vert_right_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_horiz_top_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_header_horiz_top_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_vert_left_visibility (
    dwg_ent_table *restrict table, BITCODE_BS visibility, int *restrict error)
    __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_header_vert_left_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_ent_table_set_header_horiz_ins_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_header_horiz_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_horiz_bottom_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_header_horiz_bottom_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_vert_ins_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_header_vert_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_header_vert_right_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_header_vert_right_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_horiz_top_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_data_horiz_top_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_horiz_ins_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_data_horiz_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_horiz_bottom_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_data_horiz_bottom_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_vert_left_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BS dwg_ent_table_get_data_vert_left_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_vert_ins_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_data_vert_ins_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_ent_table_set_data_vert_right_visibility (
    dwg_ent_table *restrict table, const BITCODE_BS visibility,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BS dwg_ent_table_get_data_vert_right_visibility (
    const dwg_ent_table *restrict table, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

/********************************************************************
*              FUNCTIONS FOR VERTEX_PFACE_FACE ENTITY               *
********************************************************************/

// Get/Set vertind of a vertex_pface_face entity
EXPORT BITCODE_BS dwg_ent_vertex_pface_face_get_vertind (
    const dwg_ent_vert_pface_face *face) _deprecated_dynapi_getter;

EXPORT void
dwg_ent_vertex_pface_face_set_vertind (dwg_ent_vert_pface_face *restrict face,
                                       const BITCODE_BS vertind[4])
    __nonnull ((2)) _deprecated_dynapi_setter;

/********************************************************************
 *                    FUNCTIONS FOR XRECORD OBJECT                     *
 ********************************************************************/

#define dwg_obj_xrecord_get_num_databytes(a,b) dwg_obj_xrecord_get_xdata_size (a,b)
EXPORT BITCODE_BL dwg_obj_xrecord_get_xdata_size (
    const dwg_obj_xrecord *restrict xrecord, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT BITCODE_BS dwg_obj_xrecord_get_cloning_flags (
    const dwg_obj_xrecord *restrict xrecord, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;
EXPORT void dwg_obj_xrecord_set_cloning_flags (
    dwg_obj_xrecord *restrict xrecord, const BITCODE_BS cloning_flags,
    int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
EXPORT BITCODE_BL dwg_obj_xrecord_get_num_xdata (
    const dwg_obj_xrecord *restrict xrecord, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

// EXPORT Dwg_Eed*
// dwg_obj_xrecord_get_eed(const dwg_obj_xrecord *restrict xrecord,
//                        const BITCODE_BL index,
//                        int *restrict error)
//  __nonnull ((3)) _deprecated_dynapi_getter;

EXPORT Dwg_Resbuf *
dwg_obj_xrecord_get_xdata (const dwg_obj_xrecord *restrict xrecord,
                           int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT void dwg_obj_xrecord_set_xdata (dwg_obj_xrecord *restrict xrecord,
                                       const Dwg_Resbuf *xdata,
                                       int *restrict error)
    __nonnull ((2, 3)) _deprecated_dynapi_setter;

EXPORT BITCODE_BL dwg_obj_xrecord_get_num_objid_handles (
    const dwg_obj_xrecord *restrict xrecord, int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_object_ref **
dwg_obj_xrecord_get_objid_handles (const dwg_obj_xrecord *restrict xrecord,
                                   int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

#endif /* SWIGIMPORTED */

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
dwg_obj_block_header_get_name (const dwg_obj_block_header *restrict hdr,
                               int *restrict error)
    __nonnull ((2)) _deprecated_dynapi_getter;

EXPORT dwg_obj_block_header *dwg_get_block_header (dwg_data *restrict dwg,
                                                   int *restrict error)
    __nonnull ((2));

/********************************************************************
*               FUNCTIONS FOR BLOCK_CONTROL OBJECT                  *
********************************************************************/

EXPORT BITCODE_BL dwg_obj_block_control_get_num_entries (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
    __nonnull ((2));

EXPORT dwg_object_ref **dwg_obj_block_control_get_block_headers (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
    __nonnull ((2));

EXPORT dwg_obj_block_control *
dwg_block_header_get_block_control (const dwg_obj_block_header *block_header,
                                    int *restrict error) __nonnull ((2));

EXPORT dwg_object_ref *dwg_obj_block_control_get_model_space (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
    __nonnull ((2));

EXPORT dwg_object_ref *dwg_obj_block_control_get_paper_space (
    const dwg_obj_block_control *restrict ctrl, int *restrict error)
    __nonnull ((2));

/********************************************************************
*                    FUNCTIONS FOR LAYER OBJECT                     *
********************************************************************/

// Get/Set name (utf-8) of the layer object
EXPORT char *dwg_obj_layer_get_name (const dwg_obj_layer *restrict layer,
                                     int *restrict error) __nonnull ((2));
EXPORT void dwg_obj_layer_set_name (dwg_obj_layer *restrict layer,
                                    const char *restrict name,
                                    int *restrict error) __nonnull ((2, 3));

/*******************************************************************
*                    FUNCTIONS FOR TABLES                          *
*             All other tables and table entries                   *
********************************************************************/

/// Get name of any table entry. Defaults to ByLayer.
/// \sa dwg_ent_get_layer_name which defaults to "0"
EXPORT char *dwg_obj_table_get_name (const dwg_object *restrict obj,
                                     int *restrict error) __nonnull ((2));

// Get name of the referenced table entry. Defaults to ByLayer
EXPORT char *dwg_ref_get_table_name (const dwg_object_ref *restrict ref,
                                     int *restrict error) __nonnull ((2));

// Get number of table entries from the table.
EXPORT BITCODE_BL dwg_object_tablectrl_get_num_entries (
    const dwg_object *restrict obj, int *restrict error) __nonnull ((2));

// Get the nth table entry from the table.
EXPORT dwg_object_ref *
dwg_object_tablectrl_get_entry (const dwg_object *restrict obj,
                                const BITCODE_BS index, int *restrict error)
    __nonnull ((3));

// Get all table entries from the table.
EXPORT dwg_object_ref **
dwg_object_tablectrl_get_entries (const dwg_object *restrict obj,
                                  int *restrict error) __nonnull ((2));

EXPORT dwg_object_ref *
dwg_object_tablectrl_get_ownerhandle (const dwg_object *restrict obj,
                                      int *restrict error) __nonnull ((2));
EXPORT dwg_object_ref *
dwg_object_tablectrl_get_xdicobjhandle (const dwg_object *restrict obj,
                                        int *restrict error) __nonnull ((2));
EXPORT BITCODE_BL dwg_object_tablectrl_get_objid (
    const dwg_object *restrict obj, int *restrict error) __nonnull ((2));

/********************************************************************
*                    COMMON FUNCTIONS FOR DWG ENTITY                *
********************************************************************/

EXPORT dwg_object *dwg_ent_to_object (const dwg_obj_ent *restrict obj,
                                      int *restrict error) __nonnull ((2));

EXPORT dwg_object *
dwg_ent_generic_to_object (const dwg_ent_generic *restrict obj,
                           int *restrict error) __nonnull ((2));

EXPORT dwg_obj_ent *
dwg_ent_generic_parent (const dwg_ent_generic *restrict ent,
                        int *restrict error) __nonnull ((2));

EXPORT BITCODE_RL dwg_ent_get_bitsize (const dwg_obj_ent *restrict ent,
                                       int *restrict error) __nonnull ((2));

EXPORT BITCODE_BL dwg_ent_get_num_eed (const dwg_obj_ent *restrict ent,
                                       int *restrict error) __nonnull ((2));

EXPORT dwg_entity_eed *dwg_ent_get_eed (const dwg_obj_ent *restrict ent,
                                        BITCODE_BL index, int *restrict error)
    __nonnull ((3));

EXPORT dwg_entity_eed_data *
dwg_ent_get_eed_data (const dwg_obj_ent *restrict ent, BITCODE_BL index,
                      int *restrict error) __nonnull ((3));

EXPORT BITCODE_B dwg_ent_get_picture_exists (const dwg_obj_ent *restrict ent,
                                             int *restrict error)
    __nonnull ((2));

EXPORT BITCODE_BLL
dwg_ent_get_picture_size (const dwg_obj_ent *restrict ent,
                          int *restrict error) // before r2007 only RL
    __nonnull ((2));

EXPORT BITCODE_TF
dwg_ent_get_picture (const dwg_obj_ent *restrict ent,
                     int *restrict error) __nonnull ((2));

EXPORT BITCODE_BB dwg_ent_get_entmode (const dwg_obj_ent *restrict ent,
                                       int *restrict error) __nonnull ((2));

EXPORT BITCODE_BL dwg_ent_get_num_reactors (const dwg_obj_ent *restrict ent,
                                            int *restrict error)
    __nonnull ((2));

EXPORT BITCODE_B
dwg_ent_get_xdic_missing_flag (const dwg_obj_ent *restrict ent,
                               int *restrict error) // r2004+
    __nonnull ((2));

EXPORT char *dwg_ent_get_layer_name (const dwg_obj_ent *restrict ent,
                                     int *restrict error) __nonnull ((2));

EXPORT char *dwg_ent_get_ltype_name (const dwg_obj_ent *restrict ent,
                                     int *restrict error) __nonnull ((2));

EXPORT BITCODE_B dwg_ent_get_isbylayerlt (const dwg_obj_ent *restrict ent,
                                          int *restrict error) // r13-r14 only
    __nonnull ((2));

EXPORT BITCODE_B dwg_ent_get_nolinks (const dwg_obj_ent *restrict ent,
                                      int *restrict error) __nonnull ((2));

EXPORT const Dwg_Color *dwg_ent_get_color (const dwg_obj_ent *restrict ent,
                                           int *restrict error)
    __nonnull ((2));

EXPORT double dwg_ent_get_linetype_scale (const dwg_obj_ent *restrict ent,
                                          int *restrict error) __nonnull ((2));

EXPORT BITCODE_BB dwg_ent_get_linetype_flags (const dwg_obj_ent *restrict ent,
                                              int *restrict error) // r2000+
    __nonnull ((2));

EXPORT BITCODE_BB dwg_ent_get_plotstyle_flags (const dwg_obj_ent *restrict ent,
                                               int *restrict error) // r2000+
    __nonnull ((2));

EXPORT BITCODE_BB dwg_ent_get_material_flags (const dwg_obj_ent *restrict ent,
                                              int *restrict error) // r2007+
    __nonnull ((2));

EXPORT BITCODE_RC dwg_ent_get_shadow_flags (const dwg_obj_ent *restrict ent,
                                            int *restrict error) // r2007+
    __nonnull ((2));

EXPORT BITCODE_B dwg_ent_has_full_visualstyle (dwg_obj_ent *restrict ent,
                                               int *restrict error) // r2010+
    __nonnull ((2));

EXPORT BITCODE_B dwg_ent_has_face_visualstyle (dwg_obj_ent *restrict ent,
                                               int *restrict error) // r2010+
    __nonnull ((2));

EXPORT BITCODE_B dwg_ent_has_edge_visualstyle (dwg_obj_ent *restrict ent,
                                               int *restrict error) // r2010+
    __nonnull ((2));

EXPORT BITCODE_BS dwg_ent_get_invisible (const dwg_obj_ent *restrict ent,
                                         int *restrict error) __nonnull ((2));

/* See dxf_cvt_lweight() for the mm value */
EXPORT BITCODE_RC dwg_ent_get_linewt (const dwg_obj_ent *restrict ent,
                                      int *restrict error) // r2000+
    __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_ownerhandle (const dwg_obj_ent *restrict ent, int *restrict error)
    __nonnull ((2));

EXPORT dwg_object_ref **dwg_ent_get_reactors (const dwg_obj_ent *restrict ent,
                                              int *restrict error)
    __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_xdicobjhandle (const dwg_obj_ent *restrict ent,
                           int *restrict error) __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_prev_entity (const dwg_obj_ent *restrict ent,
                         int *restrict error) // r13-r2000
    __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_next_entity (const dwg_obj_ent *restrict ent,
                         int *restrict error) // r13-r2000
    __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_color_handle (const dwg_obj_ent *restrict ent,
                          int *restrict error) // r2004+
    __nonnull ((2));

EXPORT dwg_object_ref *dwg_ent_get_layer (const dwg_obj_ent *restrict ent,
                                          int *restrict error) __nonnull ((2));

EXPORT dwg_object_ref *dwg_ent_get_ltype (const dwg_obj_ent *restrict ent,
                                          int *restrict error) __nonnull ((2));

EXPORT dwg_object_ref *dwg_ent_get_material (const dwg_obj_ent *restrict ent,
                                             int *restrict error) // r2007+
    __nonnull ((2));

EXPORT dwg_object_ref *dwg_ent_get_plotstyle (const dwg_obj_ent *restrict ent,
                                              int *restrict error) // r2000+
    __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_full_visualstyle (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
    __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_face_visualstyle (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
    __nonnull ((2));

EXPORT dwg_object_ref *
dwg_ent_get_edge_visualstyle (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2010+
    __nonnull ((2));

/********************************************************************
*                    FUNCTIONS FOR DWG OBJECT                       *
********************************************************************/

EXPORT dwg_object *dwg_obj_obj_to_object (const dwg_obj_obj *restrict obj,
                                          int *restrict error) __nonnull ((2));

EXPORT BITCODE_BL dwg_obj_get_objid (const dwg_obj_obj *restrict obj,
                                     int *restrict error) __nonnull ((2));

EXPORT BITCODE_BL dwg_obj_get_num_eed (const dwg_obj_obj *restrict obj,
                                       int *restrict error) __nonnull ((2));
EXPORT dwg_entity_eed *dwg_obj_get_eed (const dwg_obj_obj *restrict obj,
                                        const BITCODE_BL index,
                                        int *restrict error) __nonnull ((3));
EXPORT dwg_entity_eed_data *
dwg_obj_get_eed_data (const dwg_obj_obj *restrict obj, const BITCODE_BL index,
                      int *restrict error) __nonnull ((3));

EXPORT BITCODE_H dwg_obj_get_ownerhandle (const dwg_obj_obj *restrict obj,
                                           int *restrict error)
    __nonnull ((2));
EXPORT BITCODE_BL dwg_obj_get_num_reactors (const dwg_obj_obj *restrict obj,
                                            int *restrict error)
    __nonnull ((2));
EXPORT BITCODE_H *dwg_obj_get_reactors (const dwg_obj_obj *restrict obj,
                                        int *restrict error) __nonnull ((2));
EXPORT BITCODE_H dwg_obj_get_xdicobjhandle (const dwg_obj_obj *restrict obj,
                                            int *restrict error)
    __nonnull ((2));
/* r2004+ */
EXPORT BITCODE_B dwg_obj_get_xdic_missing_flag (
    const dwg_obj_obj *restrict obj, int *restrict error) __nonnull ((2));
/* r2013+ */
EXPORT BITCODE_B dwg_obj_get_has_ds_binary_data (
    const dwg_obj_obj *restrict obj, int *restrict error) __nonnull ((2));
EXPORT Dwg_Handle *dwg_obj_get_handleref (const dwg_obj_obj *restrict obj,
                                          int *restrict error) __nonnull ((2));

EXPORT dwg_object *
dwg_obj_generic_to_object (const dwg_obj_generic *restrict obj,
                           int *restrict error) __nonnull ((2));

EXPORT dwg_obj_obj *
dwg_obj_generic_parent (const dwg_obj_generic *restrict obj,
                        int *restrict error) __nonnull ((2));

EXPORT dwg_object *dwg_get_object (dwg_data *dwg, BITCODE_BL index);

EXPORT BITCODE_RL dwg_object_get_bitsize (const dwg_object *obj);

EXPORT BITCODE_BL dwg_object_get_index (const dwg_object *restrict obj,
                                        int *restrict error) __nonnull ((2));

EXPORT dwg_handle *dwg_object_get_handle (dwg_object *restrict obj,
                                          int *restrict error) __nonnull ((2));

EXPORT dwg_obj_obj *dwg_object_to_object (dwg_object *restrict obj,
                                          int *restrict error) __nonnull ((2));

EXPORT dwg_obj_ent *dwg_object_to_entity (dwg_object *restrict obj,
                                          int *restrict error) __nonnull ((2));

EXPORT int dwg_object_get_type (const dwg_object *obj);

EXPORT int dwg_object_get_fixedtype (const dwg_object *obj);

EXPORT char *dwg_object_get_dxfname (const dwg_object *obj);

EXPORT BITCODE_BL dwg_ref_get_absref (const dwg_object_ref *restrict ref,
                                      int *restrict error) __nonnull ((2));

EXPORT dwg_object *dwg_ref_get_object (const dwg_object_ref *restrict ref,
                                       int *restrict error) __nonnull ((2));

EXPORT dwg_object *dwg_absref_get_object (const dwg_data *dwg,
                                          const BITCODE_BL absref);

EXPORT unsigned int dwg_get_num_classes (const dwg_data *dwg);

EXPORT dwg_class *dwg_get_class (const dwg_data *dwg, unsigned int index);

#ifdef __cplusplus
}
#endif

#endif

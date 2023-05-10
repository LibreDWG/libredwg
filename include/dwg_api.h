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

#ifndef __attribute_deprecated__
/* Since version 3.2, gcc allows marking deprecated functions.  */
#  if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 302)
#    define __attribute_deprecated__ __attribute__ ((__deprecated__))
#  else
#    define __attribute_deprecated__
#  endif
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
#ifndef __attribute_deprecated_msg__
#  if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 405        \
       && !defined(__ICC))                                                    \
      || _clang_has_extension(attribute_deprecated_with_message)
#    undef __attribute_deprecated_msg
#    define __attribute_deprecated_msg__(msg)                                 \
      __attribute__ ((__deprecated__ (msg)))
#  else
#    define __attribute_deprecated_msg__(msg) __attribute_deprecated__
#  endif
#endif

/* The __nonnull function attribute marks pointer arguments which
   must not be NULL.  */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 303)         \
    && !defined(__cplusplus)
#  undef __nonnull
#  define __nonnull(params) __attribute__ ((__nonnull__ params))
// cygwin/newlib has this
#  ifndef __nonnull_all
#    define __nonnull_all __attribute__ ((__nonnull__))
#  endif
#  define HAVE_NONNULL
#else
#  undef __nonnull
#  undef HAVE_NONNULL
#  define __nonnull(params)
#  define __nonnull_all
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef LOG_ERROR
#  define LOG_ERROR(msg, name, type) fprintf (stderr, msg, name, (type))
#endif

  // static bool g_add_to_mspace = true;

  /** dynapi */
  typedef struct dwg_field_name_type_offset
  {
    const char *const name;    /* field name */
    const char *const type;    /* e.g "RS" for BITCODE_RS */
    const unsigned short size; /* e.g. 2 for RS, 4 for BL */
    const unsigned short offset;
    const unsigned short
        is_indirect : 1; // for pointers, references, like 3BD, CMC, H, TV
    const unsigned short is_malloc : 1; // for strings and dynamic arrays only,
                                        // H*, TV, unknown size
    const unsigned short is_string : 1; // for null-terminated strings, use
                                        // strcpy/wcscpy. not memcpy
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
     specific fields. The optional Dwg_DYNAPI_field *fp is filled with the
     field types from dynapi.c.
   */
  EXPORT bool
  dwg_dynapi_entity_value (void *restrict entity, const char *restrict dxfname,
                           const char *restrict fieldname, void *restrict out,
                           Dwg_DYNAPI_field *restrict fp)
      __nonnull ((1, 2, 3, 4));

  /** Returns the common ENTITY|OBJECT.fieldname value in out.
     _obj is the Dwg_Entity_ENTITY or Dwg_Object_OBJECT struct with the
     specific fields. The optional Dwg_DYNAPI_field *fp is filled with the
     field types from dynapi.c
   */
  EXPORT bool
  dwg_dynapi_common_value (void *restrict _obj, const char *restrict fieldname,
                           void *restrict out, Dwg_DYNAPI_field *restrict fp)
      __nonnull ((1, 2, 3));

  /** Returns the common OBJECT.subclass.fieldname value in out.
      ptr points to the subclass field. The optional Dwg_DYNAPI_field *fp is
     filled with the field types from dynapi.c
   */
  EXPORT bool dwg_dynapi_subclass_value (const void *restrict ptr,
                                         const char *restrict subclass,
                                         const char *restrict fieldname,
                                         void *restrict out,
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
     specific fields. The optional Dwg_DYNAPI_field *fp is filled with the
     field types from dynapi.c With DWG's since r2007+ creates a fresh UTF-8
     conversion from the UTF-16 wchar value (which needs to be free'd), with
     older DWG's or with TV, TF or TFF returns the unconverted text value. Only
     valid for text fields. isnew is set to 1 if textp is freshly malloced
     (r2007+), otherwise 0
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
  EXPORT bool dwg_dynapi_header_set_value (Dwg_Data *restrict dwg,
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
      and returns it if so. Converted to UTF8 for r2007+ wide-strings. Sets
     alloced if the returned string is freshly malloc'd.
  */
  EXPORT char *dwg_dynapi_handle_name (const Dwg_Data *restrict dwg,
                                       Dwg_Object_Ref *restrict hdl,
                                       int *alloced) __nonnull ((1, 2, 3));

  /** Return the field for custom type checks. */
  EXPORT const Dwg_DYNAPI_field *
  dwg_dynapi_header_field (const char *restrict fieldname) __nonnull ((1));

  EXPORT const Dwg_DYNAPI_field *
  dwg_dynapi_entity_field (const char *restrict name,
                           const char *restrict fieldname) __nonnull ((1, 2));

  EXPORT const Dwg_DYNAPI_field *
  dwg_dynapi_subclass_field (const char *restrict name,
                             const char *restrict fieldname)
      __nonnull ((1, 2));

  EXPORT const Dwg_DYNAPI_field *
  dwg_dynapi_common_entity_field (const char *restrict fieldname)
      __nonnull ((1));

  EXPORT const Dwg_DYNAPI_field *
  dwg_dynapi_common_object_field (const char *restrict fieldname)
      __nonnull ((1));

  /** Find the fields for this entity or object. */
  EXPORT const Dwg_DYNAPI_field *
  dwg_dynapi_entity_fields (const char *restrict name) __nonnull ((1));

  EXPORT const Dwg_DYNAPI_field *dwg_dynapi_common_entity_fields (void);

  EXPORT const Dwg_DYNAPI_field *dwg_dynapi_common_object_fields (void);

  /** Find the fields for this subclass. See dwg.h */
  EXPORT const Dwg_DYNAPI_field *
  dwg_dynapi_subclass_fields (const char *restrict name) __nonnull ((1));

  /** The sum of the size of all fields, by entity or subclass name */
  EXPORT int dwg_dynapi_fields_size (const char *restrict name)
      __nonnull ((1));

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

  /* invisible face edge if negative.
     no 4th edge (ie a triangle) if the last face has index 0 (starts with 1)
  */
  typedef BITCODE_BSd dwg_face[4];

/* Returns a NULL-terminated array of all entities of a specific type from a
 * BLOCK */
#define DWG_GETALL_ENTITY_DECL(token)                                         \
  EXPORT Dwg_Entity_##token **dwg_getall_##token (Dwg_Object_Ref *hdr)

/* Checks now also variable classes */
#define DWG_GETALL_ENTITY(token)                                              \
  EXPORT                                                                      \
  Dwg_Entity_##token **dwg_getall_##token (Dwg_Object_Ref *hdr)               \
  {                                                                           \
    int i = 0, counts = 0;                                                    \
    Dwg_Entity_##token **ret_##token;                                         \
    Dwg_Object *obj;                                                          \
    if (!hdr || !hdr->obj)                                                    \
      return NULL;                                                            \
    obj = get_first_owned_entity (hdr->obj);                                  \
    while (obj)                                                               \
      {                                                                       \
        if (obj->fixedtype == DWG_TYPE_##token)                               \
          counts++;                                                           \
        obj = get_next_owned_entity (hdr->obj, obj);                          \
      }                                                                       \
    if (!counts)                                                              \
      return NULL;                                                            \
    ret_##token = (Dwg_Entity_##token **)malloc (                             \
        (counts + 1) * sizeof (Dwg_Entity_##token *));                        \
    obj = get_first_owned_entity (hdr->obj);                                  \
    while (obj)                                                               \
      {                                                                       \
        if (obj->fixedtype == DWG_TYPE_##token)                               \
          {                                                                   \
            ret_##token[i] = obj->tio.entity->tio.token;                      \
            i++;                                                              \
            if (i >= counts)                                                  \
              break;                                                          \
          }                                                                   \
        obj = get_next_owned_entity (hdr->obj, obj);                          \
      }                                                                       \
    ret_##token[i] = NULL;                                                    \
    return ret_##token;                                                       \
  }

/* Returns a NULL-terminated array of all objects of a specific type */
#define DWG_GETALL_OBJECT_DECL(token)                                         \
  EXPORT                                                                      \
  Dwg_Object_##token **dwg_getall_##token (Dwg_Data *dwg)

#define DWG_GET_FIRST_OBJECT_DECL(token)                                      \
  EXPORT Dwg_Object_##token *dwg_get_first_##token (Dwg_Data *dwg)

#define DWG_GETALL_OBJECT(token)                                              \
  EXPORT                                                                      \
  Dwg_Object_##token **dwg_getall_##token (Dwg_Data *dwg)                     \
  {                                                                           \
    BITCODE_BL i, c, counts = 0;                                              \
    Dwg_Object_##token **ret_##token;                                         \
    for (i = 0; i < dwg->num_objects; i++)                                    \
      {                                                                       \
        const Dwg_Object *const obj = &dwg->object[i];                        \
        if (obj->supertype == DWG_SUPERTYPE_OBJECT                            \
            && obj->fixedtype == DWG_TYPE_##token)                            \
          {                                                                   \
            counts++;                                                         \
          }                                                                   \
      }                                                                       \
    if (!counts)                                                              \
      return NULL;                                                            \
    ret_##token = (Dwg_Object_##token **)malloc (                             \
        (counts + 1) * sizeof (Dwg_Object_##token *));                        \
    for (c = 0, i = 0; i < dwg->num_objects; i++)                             \
      {                                                                       \
        const Dwg_Object *const obj = &dwg->object[i];                        \
        if (obj->supertype == DWG_SUPERTYPE_OBJECT                            \
            && obj->fixedtype == DWG_TYPE_##token)                            \
          {                                                                   \
            ret_##token[c] = obj->tio.object->tio.token;                      \
            c++;                                                              \
            if (c >= counts)                                                  \
              break;                                                          \
          }                                                                   \
      }                                                                       \
    ret_##token[c] = NULL;                                                    \
    return ret_##token;                                                       \
  }

#define DWG_GET_FIRST_OBJECT(token)                                           \
  EXPORT Dwg_Object_##token *dwg_get_first_##token (Dwg_Data *dwg)            \
  {                                                                           \
    for (i = 0; i < dwg->num_objects; i++)                                    \
      {                                                                       \
        const Dwg_Object *const obj = &dwg->object[i];                        \
        if (obj->supertype == DWG_SUPERTYPE_OBJECT                            \
            && obj->fixedtype == DWG_TYPE_##token)                            \
          {                                                                   \
            return obj->tio.object->tio.token;                                \
          }                                                                   \
      }                                                                       \
    return NULL;                                                              \
  }

// Cast a Dwg_Object to Entity
#define CAST_DWG_OBJECT_TO_ENTITY_DECL(token)                                 \
  EXPORT                                                                      \
  Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj)

#define CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL(token)                          \
  EXPORT                                                                      \
  Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj);

// Only for fixed typed entities, < 500
// Dwg_Entity* -> Dwg_Object_TYPE*
#define CAST_DWG_OBJECT_TO_ENTITY(token)                                      \
  EXPORT                                                                      \
  Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj)                 \
  {                                                                           \
    Dwg_Entity_##token *ret_obj = NULL;                                       \
    if (obj && obj->tio.entity                                                \
        && (obj->type == DWG_TYPE_##token                                     \
            || obj->fixedtype == DWG_TYPE_##token))                           \
      {                                                                       \
        ret_obj = obj->tio.entity->tio.token;                                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        loglevel                                                              \
            = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
        LOG_ERROR ("Invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
      }                                                                       \
    return ret_obj;                                                           \
  }

/// for all classes, types > 500. IMAGE, OLE2FRAME, WIPEOUT
#define CAST_DWG_OBJECT_TO_ENTITY_BYNAME(token)                               \
  EXPORT                                                                      \
  Dwg_Entity_##token *dwg_object_to_##token (Dwg_Object *obj)                 \
  {                                                                           \
    Dwg_Entity_##token *ret_obj = NULL;                                       \
    if (obj && obj->tio.entity                                                \
        && (obj->type == DWG_TYPE_##token                                     \
            || obj->fixedtype == DWG_TYPE_##token))                           \
      {                                                                       \
        ret_obj = obj->tio.entity->tio.token;                                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        loglevel                                                              \
            = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
        LOG_ERROR ("Invalid %s type: got %s, 0x%x", #token,                   \
                   obj ? obj->dxfname : "<no obj>", obj ? obj->type : 0);     \
      }                                                                       \
    return ret_obj;                                                           \
  }

#define CAST_DWG_OBJECT_TO_OBJECT_DECL(token)                                 \
  EXPORT                                                                      \
  Dwg_Object_##token *dwg_object_to_##token (Dwg_Object *obj)

// Dwg_Object* -> Dwg_Object_TYPE*
#define CAST_DWG_OBJECT_TO_OBJECT(token)                                      \
  EXPORT                                                                      \
  Dwg_Object_##token *dwg_object_to_##token (Dwg_Object *obj)                 \
  {                                                                           \
    Dwg_Object_##token *ret_obj = NULL;                                       \
    if (obj && obj->tio.object                                                \
        && (obj->type == DWG_TYPE_##token                                     \
            || obj->fixedtype == DWG_TYPE_##token))                           \
      {                                                                       \
        ret_obj = obj->tio.object->tio.token;                                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        loglevel                                                              \
            = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
        LOG_ERROR ("Invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
      }                                                                       \
    return ret_obj;                                                           \
  }

// unused, we have now fixedtype.
#define CAST_DWG_OBJECT_TO_OBJECT_BYNAME(token, dxfname)                      \
  EXPORT                                                                      \
  Dwg_Object_##token *dwg_object_to_##token (Dwg_Object *obj)                 \
  {                                                                           \
    Dwg_Object_##token *ret_obj = NULL;                                       \
    if (obj && obj->dxfname && !strcmp (obj->dxfname, #dxfname))              \
      {                                                                       \
        ret_obj = obj->tio.object->tio.token;                                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        loglevel                                                              \
            = obj && obj->parent ? obj->parent->opts & DWG_OPTS_LOGLEVEL : 0; \
        LOG_ERROR ("Invalid %s type: got 0x%x", #token, obj ? obj->type : 0); \
      }                                                                       \
    return ret_obj;                                                           \
  }

  ///////////////////////////////////////////////////////////////////////////

  typedef struct _dwg_object dwg_object;
  typedef struct _dwg_object_ref dwg_object_ref;
  typedef struct _dwg_handle dwg_handle;
  typedef struct _dwg_object_entity dwg_obj_ent;
  typedef struct _dwg_object_object dwg_obj_obj;
  typedef struct _dwg_class dwg_class;
  typedef struct _dwg_struct dwg_data;
  typedef struct _dwg_entity_eed_data dwg_entity_eed_data;
  typedef struct _dwg_entity_eed dwg_entity_eed;

  typedef struct _dwg_object_STYLE_CONTROL dwg_tbl_generic;
  typedef struct _dwg_object_UNKNOWN_OBJ dwg_obj_generic;
  typedef struct _dwg_object_UNKNOWN_OBJ dwg_obj_unknown;
  typedef struct _dwg_entity_UNKNOWN_ENT dwg_ent_unknown;
  typedef struct _dwg_entity_POINT dwg_ent_generic;
  typedef struct _dwg_entity_PROXY_ENTITY dwg_ent_proxy;
  typedef struct _dwg_object_PROXY_OBJECT dwg_obj_proxy;
  typedef struct _dwg_DIMENSION_common dwg_ent_dim;
  typedef struct _dwg_entity_3DSOLID dwg_ent_3dsolid;
  typedef struct _dwg_entity_3DFACE dwg_ent_3dface;

  typedef struct _dwg_MLINE_line dwg_mline_line;
  typedef struct _dwg_MLINE_vertex dwg_mline_vertex;
  typedef struct _dwg_SPLINE_control_point dwg_spline_control_point;
  typedef struct _dwg_3DSOLID_wire dwg_3dsolid_wire;
  typedef struct _dwg_3DSOLID_silhouette dwg_3dsolid_silhouette;
  typedef struct _dwg_entity_RAY dwg_ent_xline;
  typedef struct _dwg_entity_VERTEX_3D dwg_ent_vertex_mesh,
      dwg_ent_vertex_pface;
  typedef struct _dwg_entity_3DSOLID dwg_ent_region, dwg_ent_body;

  //////////////////////////////////////////////////////////////////////

#define dwg_get_OBJECT_DECL(name, OBJECT)                                     \
  EXPORT bool dwg_get_##OBJECT (const dwg_##name *restrict name,              \
                                const char *restrict fieldname,               \
                                void *restrict out);                          \
  EXPORT bool dwg_set_##OBJECT (const dwg_##name *restrict name,              \
                                const char *restrict fieldname,               \
                                void *restrict value)

  dwg_get_OBJECT_DECL (ent_vertex_mesh, VERTEX_MESH);
  dwg_get_OBJECT_DECL (ent_vertex_pface, VERTEX_PFACE);
  dwg_get_OBJECT_DECL (ent_region, REGION);
  dwg_get_OBJECT_DECL (ent_body, BODY);
  dwg_get_OBJECT_DECL (ent_3dsolid, 3DSOLID);
  dwg_get_OBJECT_DECL (ent_xline, XLINE);

  /********************************************************************/
  typedef struct _dwg_abstractentity_UNDERLAY
      dwg_ent_underlay; /* same layout for all 3 */
  typedef struct _dwg_abstractobject_UNDERLAYDEFINITION
      dwg_obj_underlaydefinition;
  // typedef struct _dwg_entity_3DLINE		dwg_ent_3dline;
  /* Start auto-generated content. Do not touch. */
  // clang-format: off
  typedef struct _dwg_entity__3DFACE		dwg_ent__3dface;
  typedef struct _dwg_entity__3DSOLID		dwg_ent__3dsolid;
  typedef struct _dwg_entity_ARC		dwg_ent_arc;
  typedef struct _dwg_entity_ATTDEF		dwg_ent_attdef;
  typedef struct _dwg_entity_ATTRIB		dwg_ent_attrib;
  typedef struct _dwg_entity_BLOCK		dwg_ent_block;
  typedef struct _dwg_entity_CIRCLE		dwg_ent_circle;
  typedef struct _dwg_entity_DIMENSION_ALIGNED		dwg_ent_dim_aligned;
  typedef struct _dwg_entity_DIMENSION_ANG2LN		dwg_ent_dim_ang2ln;
  typedef struct _dwg_entity_DIMENSION_ANG3PT		dwg_ent_dim_ang3pt;
  typedef struct _dwg_entity_DIMENSION_DIAMETER		dwg_ent_dim_diameter;
  typedef struct _dwg_entity_DIMENSION_LINEAR		dwg_ent_dim_linear;
  typedef struct _dwg_entity_DIMENSION_ORDINATE		dwg_ent_dim_ordinate;
  typedef struct _dwg_entity_DIMENSION_RADIUS		dwg_ent_dim_radius;
  typedef struct _dwg_entity_ELLIPSE		dwg_ent_ellipse;
  typedef struct _dwg_entity_ENDBLK		dwg_ent_endblk;
  typedef struct _dwg_entity_INSERT		dwg_ent_insert;
  typedef struct _dwg_entity_LEADER		dwg_ent_leader;
  typedef struct _dwg_entity_LINE		dwg_ent_line;
  typedef struct _dwg_entity_LOAD		dwg_ent_load;
  typedef struct _dwg_entity_MINSERT		dwg_ent_minsert;
  typedef struct _dwg_entity_MLINE		dwg_ent_mline;
  typedef struct _dwg_entity_MTEXT		dwg_ent_mtext;
  typedef struct _dwg_entity_OLEFRAME		dwg_ent_oleframe;
  typedef struct _dwg_entity_POINT		dwg_ent_point;
  typedef struct _dwg_entity_POLYLINE		dwg_ent_polyline;
  typedef struct _dwg_entity_POLYLINE_2D		dwg_ent_polyline_2d;
  typedef struct _dwg_entity_POLYLINE_3D		dwg_ent_polyline_3d;
  typedef struct _dwg_entity_POLYLINE_MESH		dwg_ent_polyline_mesh;
  typedef struct _dwg_entity_POLYLINE_PFACE		dwg_ent_polyline_pface;
  typedef struct _dwg_entity_PROXY_ENTITY		dwg_ent_proxy_entity;
  typedef struct _dwg_entity_RAY		dwg_ent_ray;
  typedef struct _dwg_entity_SEQEND		dwg_ent_seqend;
  typedef struct _dwg_entity_SHAPE		dwg_ent_shape;
  typedef struct _dwg_entity_SOLID		dwg_ent_solid;
  typedef struct _dwg_entity_SPLINE		dwg_ent_spline;
  typedef struct _dwg_entity_TEXT		dwg_ent_text;
  typedef struct _dwg_entity_TOLERANCE		dwg_ent_tolerance;
  typedef struct _dwg_entity_TRACE		dwg_ent_trace;
  typedef struct _dwg_entity_UNKNOWN_ENT		dwg_ent_unknown_ent;
  typedef struct _dwg_entity_VERTEX		dwg_ent_vertex;
  typedef struct _dwg_entity_VERTEX_2D		dwg_ent_vertex_2d;
  typedef struct _dwg_entity_VERTEX_3D		dwg_ent_vertex_3d;
  typedef struct _dwg_entity_VERTEX_PFACE_FACE		dwg_ent_vert_pface_face;
  typedef struct _dwg_entity_VIEWPORT		dwg_ent_viewport;
  /* untyped > 500 */
  typedef struct _dwg_entity_3DLINE		dwg_ent__3dline;
  typedef struct _dwg_entity_CAMERA		dwg_ent_camera;
  typedef struct _dwg_entity_DGNUNDERLAY		dwg_ent_dgnunderlay;
  typedef struct _dwg_entity_DWFUNDERLAY		dwg_ent_dwfunderlay;
  typedef struct _dwg_entity_ENDREP		dwg_ent_endrep;
  typedef struct _dwg_entity_HATCH		dwg_ent_hatch;
  typedef struct _dwg_entity_IMAGE		dwg_ent_image;
  typedef struct _dwg_entity_JUMP		dwg_ent_jump;
  typedef struct _dwg_entity_LIGHT		dwg_ent_light;
  typedef struct _dwg_entity_LWPOLYLINE		dwg_ent_lwpline;
  typedef struct _dwg_entity_MESH		dwg_ent_mesh;
  typedef struct _dwg_entity_MULTILEADER		dwg_ent_mleader;
  typedef struct _dwg_entity_OLE2FRAME		dwg_ent_ole2frame;
  typedef struct _dwg_entity_PDFUNDERLAY		dwg_ent_pdfunderlay;
  typedef struct _dwg_entity_REPEAT		dwg_ent_repeat;
  typedef struct _dwg_entity_SECTIONOBJECT		dwg_ent_sectionobject;
  typedef struct _dwg_entity_WIPEOUT		dwg_ent_wipeout;
  /* unstable */
  typedef struct _dwg_entity_ARC_DIMENSION		dwg_ent_arc_dimension;
  typedef struct _dwg_entity_HELIX		dwg_ent_helix;
  typedef struct _dwg_entity_LARGE_RADIAL_DIMENSION		dwg_ent_large_radial_dimension;
  typedef struct _dwg_entity_LAYOUTPRINTCONFIG		dwg_ent_layoutprintconfig;
  typedef struct _dwg_entity_PLANESURFACE		dwg_ent_planesurface;
  typedef struct _dwg_entity_POINTCLOUD		dwg_ent_pointcloud;
  typedef struct _dwg_entity_POINTCLOUDEX		dwg_ent_pointcloudex;
  /* debugging */
  typedef struct _dwg_entity_ALIGNMENTPARAMETERENTITY		dwg_ent_alignmentparameterentity;
  typedef struct _dwg_entity_ARCALIGNEDTEXT		dwg_ent_arcalignedtext;
  typedef struct _dwg_entity_BASEPOINTPARAMETERENTITY		dwg_ent_basepointparameterentity;
  typedef struct _dwg_entity_EXTRUDEDSURFACE		dwg_ent_extrudedsurface;
  typedef struct _dwg_entity_FLIPGRIPENTITY		dwg_ent_flipgripentity;
  typedef struct _dwg_entity_FLIPPARAMETERENTITY		dwg_ent_flipparameterentity;
  typedef struct _dwg_entity_GEOPOSITIONMARKER		dwg_ent_geopositionmarker;
  typedef struct _dwg_entity_LINEARGRIPENTITY		dwg_ent_lineargripentity;
  typedef struct _dwg_entity_LINEARPARAMETERENTITY		dwg_ent_linearparameterentity;
  typedef struct _dwg_entity_LOFTEDSURFACE		dwg_ent_loftedsurface;
  typedef struct _dwg_entity_MPOLYGON		dwg_ent_mpolygon;
  typedef struct _dwg_entity_NAVISWORKSMODEL		dwg_ent_navisworksmodel;
  typedef struct _dwg_entity_NURBSURFACE		dwg_ent_nurbsurface;
  typedef struct _dwg_entity_POINTPARAMETERENTITY		dwg_ent_pointparameterentity;
  typedef struct _dwg_entity_POLARGRIPENTITY		dwg_ent_polargripentity;
  typedef struct _dwg_entity_REVOLVEDSURFACE		dwg_ent_revolvedsurface;
  typedef struct _dwg_entity_ROTATIONGRIPENTITY		dwg_ent_rotationgripentity;
  typedef struct _dwg_entity_ROTATIONPARAMETERENTITY		dwg_ent_rotationparameterentity;
  typedef struct _dwg_entity_RTEXT		dwg_ent_rtext;
  typedef struct _dwg_entity_SWEPTSURFACE		dwg_ent_sweptsurface;
  typedef struct _dwg_entity_TABLE		dwg_ent_table;
  typedef struct _dwg_entity_VISIBILITYGRIPENTITY		dwg_ent_visibilitygripentity;
  typedef struct _dwg_entity_VISIBILITYPARAMETERENTITY		dwg_ent_visibilityparameterentity;
  typedef struct _dwg_entity_XYGRIPENTITY		dwg_ent_xygripentity;
  typedef struct _dwg_entity_XYPARAMETERENTITY		dwg_ent_xyparameterentity;
  typedef struct _dwg_object_APPID		dwg_obj_appid;
  typedef struct _dwg_object_APPID_CONTROL		dwg_obj_appid_control;
  typedef struct _dwg_object_BLOCK_CONTROL		dwg_obj_block_control;
  typedef struct _dwg_object_BLOCK_HEADER		dwg_obj_block_header;
  typedef struct _dwg_object_DICTIONARY		dwg_obj_dictionary;
  typedef struct _dwg_object_DIMSTYLE		dwg_obj_dimstyle;
  typedef struct _dwg_object_DIMSTYLE_CONTROL		dwg_obj_dimstyle_control;
  typedef struct _dwg_object_DUMMY		dwg_obj_dummy;
  typedef struct _dwg_object_LAYER		dwg_obj_layer;
  typedef struct _dwg_object_LAYER_CONTROL		dwg_obj_layer_control;
  typedef struct _dwg_object_LONG_TRANSACTION		dwg_obj_long_transaction;
  typedef struct _dwg_object_LTYPE		dwg_obj_ltype;
  typedef struct _dwg_object_LTYPE_CONTROL		dwg_obj_ltype_control;
  typedef struct _dwg_object_MLINESTYLE		dwg_obj_mlinestyle;
  typedef struct _dwg_object_STYLE		dwg_obj_style;
  typedef struct _dwg_object_STYLE_CONTROL		dwg_obj_style_control;
  typedef struct _dwg_object_UCS		dwg_obj_ucs;
  typedef struct _dwg_object_UCS_CONTROL		dwg_obj_ucs_control;
  typedef struct _dwg_object_UNKNOWN_OBJ		dwg_obj_unknown_obj;
  typedef struct _dwg_object_VIEW		dwg_obj_view;
  typedef struct _dwg_object_VIEW_CONTROL		dwg_obj_view_control;
  typedef struct _dwg_object_VPORT		dwg_obj_vport;
  typedef struct _dwg_object_VPORT_CONTROL		dwg_obj_vport_control;
  typedef struct _dwg_object_VX_CONTROL		dwg_obj_vx_control;
  typedef struct _dwg_object_VX_TABLE_RECORD		dwg_obj_vx_table_record;
  /* untyped > 500 */
  typedef struct _dwg_object_ACSH_BOOLEAN_CLASS		dwg_obj_acsh_boolean_class;
  typedef struct _dwg_object_ACSH_BOX_CLASS		dwg_obj_acsh_box_class;
  typedef struct _dwg_object_ACSH_CONE_CLASS		dwg_obj_acsh_cone_class;
  typedef struct _dwg_object_ACSH_CYLINDER_CLASS		dwg_obj_acsh_cylinder_class;
  typedef struct _dwg_object_ACSH_FILLET_CLASS		dwg_obj_acsh_fillet_class;
  typedef struct _dwg_object_ACSH_HISTORY_CLASS		dwg_obj_acsh_history_class;
  typedef struct _dwg_object_ACSH_SPHERE_CLASS		dwg_obj_acsh_sphere_class;
  typedef struct _dwg_object_ACSH_TORUS_CLASS		dwg_obj_acsh_torus_class;
  typedef struct _dwg_object_ACSH_WEDGE_CLASS		dwg_obj_acsh_wedge_class;
  typedef struct _dwg_object_ASSOCGEOMDEPENDENCY		dwg_obj_assocgeomdependency;
  typedef struct _dwg_object_ASSOCNETWORK		dwg_obj_assocnetwork;
  typedef struct _dwg_object_BLOCKALIGNMENTGRIP		dwg_obj_blockalignmentgrip;
  typedef struct _dwg_object_BLOCKALIGNMENTPARAMETER		dwg_obj_blockalignmentparameter;
  typedef struct _dwg_object_BLOCKBASEPOINTPARAMETER		dwg_obj_blockbasepointparameter;
  typedef struct _dwg_object_BLOCKFLIPACTION		dwg_obj_blockflipaction;
  typedef struct _dwg_object_BLOCKFLIPGRIP		dwg_obj_blockflipgrip;
  typedef struct _dwg_object_BLOCKFLIPPARAMETER		dwg_obj_blockflipparameter;
  typedef struct _dwg_object_BLOCKGRIPLOCATIONCOMPONENT		dwg_obj_blockgriplocationcomponent;
  typedef struct _dwg_object_BLOCKLINEARGRIP		dwg_obj_blocklineargrip;
  typedef struct _dwg_object_BLOCKLOOKUPGRIP		dwg_obj_blocklookupgrip;
  typedef struct _dwg_object_BLOCKMOVEACTION		dwg_obj_blockmoveaction;
  typedef struct _dwg_object_BLOCKROTATEACTION		dwg_obj_blockrotateaction;
  typedef struct _dwg_object_BLOCKROTATIONGRIP		dwg_obj_blockrotationgrip;
  typedef struct _dwg_object_BLOCKSCALEACTION		dwg_obj_blockscaleaction;
  typedef struct _dwg_object_BLOCKVISIBILITYGRIP		dwg_obj_blockvisibilitygrip;
  typedef struct _dwg_object_CELLSTYLEMAP		dwg_obj_cellstylemap;
  typedef struct _dwg_object_DETAILVIEWSTYLE		dwg_obj_detailviewstyle;
  typedef struct _dwg_object_DICTIONARYVAR		dwg_obj_dictionaryvar;
  typedef struct _dwg_object_DICTIONARYWDFLT		dwg_obj_dictionarywdflt;
  typedef struct _dwg_object_DYNAMICBLOCKPURGEPREVENTER		dwg_obj_dynamicblockpurgepreventer;
  typedef struct _dwg_object_FIELD		dwg_obj_field;
  typedef struct _dwg_object_FIELDLIST		dwg_obj_fieldlist;
  typedef struct _dwg_object_GEODATA		dwg_obj_geodata;
  typedef struct _dwg_object_GROUP		dwg_obj_group;
  typedef struct _dwg_object_IDBUFFER		dwg_obj_idbuffer;
  typedef struct _dwg_object_IMAGEDEF		dwg_obj_imagedef;
  typedef struct _dwg_object_IMAGEDEF_REACTOR		dwg_obj_imagedef_reactor;
  typedef struct _dwg_object_INDEX		dwg_obj_index;
  typedef struct _dwg_object_LAYERFILTER		dwg_obj_layerfilter;
  typedef struct _dwg_object_LAYER_INDEX		dwg_obj_layer_index;
  typedef struct _dwg_object_LAYOUT		dwg_obj_layout;
  typedef struct _dwg_object_PLACEHOLDER		dwg_obj_placeholder;
  typedef struct _dwg_object_PLOTSETTINGS		dwg_obj_plotsettings;
  typedef struct _dwg_object_RASTERVARIABLES		dwg_obj_rastervariables;
  typedef struct _dwg_object_RENDERENVIRONMENT		dwg_obj_renderenvironment;
  typedef struct _dwg_object_SCALE		dwg_obj_scale;
  typedef struct _dwg_object_SECTIONVIEWSTYLE		dwg_obj_sectionviewstyle;
  typedef struct _dwg_object_SECTION_MANAGER		dwg_obj_section_manager;
  typedef struct _dwg_object_SORTENTSTABLE		dwg_obj_sortentstable;
  typedef struct _dwg_object_SPATIAL_FILTER		dwg_obj_spatial_filter;
  typedef struct _dwg_object_SUN		dwg_obj_sun;
  typedef struct _dwg_object_TABLEGEOMETRY		dwg_obj_tablegeometry;
  typedef struct _dwg_object_VBA_PROJECT		dwg_obj_vba_project;
  typedef struct _dwg_object_VISUALSTYLE		dwg_obj_visualstyle;
  typedef struct _dwg_object_WIPEOUTVARIABLES		dwg_obj_wipeoutvariables;
  typedef struct _dwg_object_XRECORD		dwg_obj_xrecord;
  typedef struct _dwg_abstractobject_UNDERLAYDEFINITION		dwg_obj_pdfdefinition;
  typedef struct _dwg_abstractobject_UNDERLAYDEFINITION		dwg_obj_dgndefinition;
  typedef struct _dwg_abstractobject_UNDERLAYDEFINITION		dwg_obj_dwfdefinition;
  /* unstable */
  typedef struct _dwg_object_ACSH_BREP_CLASS		dwg_obj_acsh_brep_class;
  typedef struct _dwg_object_ACSH_CHAMFER_CLASS		dwg_obj_acsh_chamfer_class;
  typedef struct _dwg_object_ACSH_PYRAMID_CLASS		dwg_obj_acsh_pyramid_class;
  typedef struct _dwg_object_ALDIMOBJECTCONTEXTDATA		dwg_obj_aldimobjectcontextdata;
  typedef struct _dwg_object_ASSOC2DCONSTRAINTGROUP		dwg_obj_assoc2dconstraintgroup;
  typedef struct _dwg_object_ASSOCACTION		dwg_obj_assocaction;
  typedef struct _dwg_object_ASSOCACTIONPARAM		dwg_obj_assocactionparam;
  typedef struct _dwg_object_ASSOCARRAYACTIONBODY		dwg_obj_assocarrayactionbody;
  typedef struct _dwg_object_ASSOCASMBODYACTIONPARAM		dwg_obj_assocasmbodyactionparam;
  typedef struct _dwg_object_ASSOCBLENDSURFACEACTIONBODY		dwg_obj_assocblendsurfaceactionbody;
  typedef struct _dwg_object_ASSOCCOMPOUNDACTIONPARAM		dwg_obj_assoccompoundactionparam;
  typedef struct _dwg_object_ASSOCDEPENDENCY		dwg_obj_assocdependency;
  typedef struct _dwg_object_ASSOCDIMDEPENDENCYBODY		dwg_obj_assocdimdependencybody;
  typedef struct _dwg_object_ASSOCEXTENDSURFACEACTIONBODY		dwg_obj_assocextendsurfaceactionbody;
  typedef struct _dwg_object_ASSOCEXTRUDEDSURFACEACTIONBODY		dwg_obj_assocextrudedsurfaceactionbody;
  typedef struct _dwg_object_ASSOCFACEACTIONPARAM		dwg_obj_assocfaceactionparam;
  typedef struct _dwg_object_ASSOCFILLETSURFACEACTIONBODY		dwg_obj_assocfilletsurfaceactionbody;
  typedef struct _dwg_object_ASSOCLOFTEDSURFACEACTIONBODY		dwg_obj_assocloftedsurfaceactionbody;
  typedef struct _dwg_object_ASSOCNETWORKSURFACEACTIONBODY		dwg_obj_assocnetworksurfaceactionbody;
  typedef struct _dwg_object_ASSOCOBJECTACTIONPARAM		dwg_obj_assocobjectactionparam;
  typedef struct _dwg_object_ASSOCOFFSETSURFACEACTIONBODY		dwg_obj_assocoffsetsurfaceactionbody;
  typedef struct _dwg_object_ASSOCOSNAPPOINTREFACTIONPARAM		dwg_obj_assocosnappointrefactionparam;
  typedef struct _dwg_object_ASSOCPATCHSURFACEACTIONBODY		dwg_obj_assocpatchsurfaceactionbody;
  typedef struct _dwg_object_ASSOCPATHACTIONPARAM		dwg_obj_assocpathactionparam;
  typedef struct _dwg_object_ASSOCPLANESURFACEACTIONBODY		dwg_obj_assocplanesurfaceactionbody;
  typedef struct _dwg_object_ASSOCPOINTREFACTIONPARAM		dwg_obj_assocpointrefactionparam;
  typedef struct _dwg_object_ASSOCREVOLVEDSURFACEACTIONBODY		dwg_obj_assocrevolvedsurfaceactionbody;
  typedef struct _dwg_object_ASSOCTRIMSURFACEACTIONBODY		dwg_obj_assoctrimsurfaceactionbody;
  typedef struct _dwg_object_ASSOCVALUEDEPENDENCY		dwg_obj_assocvaluedependency;
  typedef struct _dwg_object_ASSOCVARIABLE		dwg_obj_assocvariable;
  typedef struct _dwg_object_ASSOCVERTEXACTIONPARAM		dwg_obj_assocvertexactionparam;
  typedef struct _dwg_object_BLKREFOBJECTCONTEXTDATA		dwg_obj_blkrefobjectcontextdata;
  typedef struct _dwg_object_BLOCKALIGNEDCONSTRAINTPARAMETER		dwg_obj_blockalignedconstraintparameter;
  typedef struct _dwg_object_BLOCKANGULARCONSTRAINTPARAMETER		dwg_obj_blockangularconstraintparameter;
  typedef struct _dwg_object_BLOCKARRAYACTION		dwg_obj_blockarrayaction;
  typedef struct _dwg_object_BLOCKDIAMETRICCONSTRAINTPARAMETER		dwg_obj_blockdiametricconstraintparameter;
  typedef struct _dwg_object_BLOCKHORIZONTALCONSTRAINTPARAMETER		dwg_obj_blockhorizontalconstraintparameter;
  typedef struct _dwg_object_BLOCKLINEARCONSTRAINTPARAMETER		dwg_obj_blocklinearconstraintparameter;
  typedef struct _dwg_object_BLOCKLINEARPARAMETER		dwg_obj_blocklinearparameter;
  typedef struct _dwg_object_BLOCKLOOKUPACTION		dwg_obj_blocklookupaction;
  typedef struct _dwg_object_BLOCKLOOKUPPARAMETER		dwg_obj_blocklookupparameter;
  typedef struct _dwg_object_BLOCKPARAMDEPENDENCYBODY		dwg_obj_blockparamdependencybody;
  typedef struct _dwg_object_BLOCKPOINTPARAMETER		dwg_obj_blockpointparameter;
  typedef struct _dwg_object_BLOCKPOLARGRIP		dwg_obj_blockpolargrip;
  typedef struct _dwg_object_BLOCKPOLARPARAMETER		dwg_obj_blockpolarparameter;
  typedef struct _dwg_object_BLOCKPOLARSTRETCHACTION		dwg_obj_blockpolarstretchaction;
  typedef struct _dwg_object_BLOCKRADIALCONSTRAINTPARAMETER		dwg_obj_blockradialconstraintparameter;
  typedef struct _dwg_object_BLOCKREPRESENTATION		dwg_obj_blockrepresentation;
  typedef struct _dwg_object_BLOCKROTATIONPARAMETER		dwg_obj_blockrotationparameter;
  typedef struct _dwg_object_BLOCKSTRETCHACTION		dwg_obj_blockstretchaction;
  typedef struct _dwg_object_BLOCKUSERPARAMETER		dwg_obj_blockuserparameter;
  typedef struct _dwg_object_BLOCKVERTICALCONSTRAINTPARAMETER		dwg_obj_blockverticalconstraintparameter;
  typedef struct _dwg_object_BLOCKVISIBILITYPARAMETER		dwg_obj_blockvisibilityparameter;
  typedef struct _dwg_object_BLOCKXYGRIP		dwg_obj_blockxygrip;
  typedef struct _dwg_object_BLOCKXYPARAMETER		dwg_obj_blockxyparameter;
  typedef struct _dwg_object_DATALINK		dwg_obj_datalink;
  typedef struct _dwg_object_DBCOLOR		dwg_obj_dbcolor;
  typedef struct _dwg_object_EVALUATION_GRAPH		dwg_obj_evaluation_graph;
  typedef struct _dwg_object_FCFOBJECTCONTEXTDATA		dwg_obj_fcfobjectcontextdata;
  typedef struct _dwg_object_GRADIENT_BACKGROUND		dwg_obj_gradient_background;
  typedef struct _dwg_object_GROUND_PLANE_BACKGROUND		dwg_obj_ground_plane_background;
  typedef struct _dwg_object_IBL_BACKGROUND		dwg_obj_ibl_background;
  typedef struct _dwg_object_IMAGE_BACKGROUND		dwg_obj_image_background;
  typedef struct _dwg_object_LEADEROBJECTCONTEXTDATA		dwg_obj_leaderobjectcontextdata;
  typedef struct _dwg_object_LIGHTLIST		dwg_obj_lightlist;
  typedef struct _dwg_object_MATERIAL		dwg_obj_material;
  typedef struct _dwg_object_MENTALRAYRENDERSETTINGS		dwg_obj_mentalrayrendersettings;
  typedef struct _dwg_object_MLEADERSTYLE		dwg_obj_mleaderstyle;
  typedef struct _dwg_object_MTEXTOBJECTCONTEXTDATA		dwg_obj_mtextobjectcontextdata;
  typedef struct _dwg_object_OBJECT_PTR		dwg_obj_object_ptr;
  typedef struct _dwg_object_PARTIAL_VIEWING_INDEX		dwg_obj_partial_viewing_index;
  typedef struct _dwg_object_POINTCLOUDCOLORMAP		dwg_obj_pointcloudcolormap;
  typedef struct _dwg_object_POINTCLOUDDEF		dwg_obj_pointclouddef;
  typedef struct _dwg_object_POINTCLOUDDEFEX		dwg_obj_pointclouddefex;
  typedef struct _dwg_object_POINTCLOUDDEF_REACTOR		dwg_obj_pointclouddef_reactor;
  typedef struct _dwg_object_POINTCLOUDDEF_REACTOR_EX		dwg_obj_pointclouddef_reactor_ex;
  typedef struct _dwg_object_PROXY_OBJECT		dwg_obj_proxy_object;
  typedef struct _dwg_object_RAPIDRTRENDERSETTINGS		dwg_obj_rapidrtrendersettings;
  typedef struct _dwg_object_RENDERENTRY		dwg_obj_renderentry;
  typedef struct _dwg_object_RENDERGLOBAL		dwg_obj_renderglobal;
  typedef struct _dwg_object_RENDERSETTINGS		dwg_obj_rendersettings;
  typedef struct _dwg_object_SECTION_SETTINGS		dwg_obj_section_settings;
  typedef struct _dwg_object_SKYLIGHT_BACKGROUND		dwg_obj_skylight_background;
  typedef struct _dwg_object_SOLID_BACKGROUND		dwg_obj_solid_background;
  typedef struct _dwg_object_SPATIAL_INDEX		dwg_obj_spatial_index;
  typedef struct _dwg_object_TABLESTYLE		dwg_obj_tablestyle;
  typedef struct _dwg_object_TEXTOBJECTCONTEXTDATA		dwg_obj_textobjectcontextdata;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS		dwg_obj_assocarraymodifyparameters;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS		dwg_obj_assocarraypathparameters;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS		dwg_obj_assocarraypolarparameters;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS		dwg_obj_assocarrayrectangularparameters;
  /* debugging */
  typedef struct _dwg_object_ACMECOMMANDHISTORY		dwg_obj_acmecommandhistory;
  typedef struct _dwg_object_ACMESCOPE		dwg_obj_acmescope;
  typedef struct _dwg_object_ACMESTATEMGR		dwg_obj_acmestatemgr;
  typedef struct _dwg_object_ACSH_EXTRUSION_CLASS		dwg_obj_acsh_extrusion_class;
  typedef struct _dwg_object_ACSH_LOFT_CLASS		dwg_obj_acsh_loft_class;
  typedef struct _dwg_object_ACSH_REVOLVE_CLASS		dwg_obj_acsh_revolve_class;
  typedef struct _dwg_object_ACSH_SWEEP_CLASS		dwg_obj_acsh_sweep_class;
  typedef struct _dwg_object_ANGDIMOBJECTCONTEXTDATA		dwg_obj_angdimobjectcontextdata;
  typedef struct _dwg_object_ANNOTSCALEOBJECTCONTEXTDATA		dwg_obj_annotscaleobjectcontextdata;
  typedef struct _dwg_object_ASSOC3POINTANGULARDIMACTIONBODY		dwg_obj_assoc3pointangulardimactionbody;
  typedef struct _dwg_object_ASSOCALIGNEDDIMACTIONBODY		dwg_obj_assocaligneddimactionbody;
  typedef struct _dwg_object_ASSOCARRAYMODIFYACTIONBODY		dwg_obj_assocarraymodifyactionbody;
  typedef struct _dwg_object_ASSOCEDGEACTIONPARAM		dwg_obj_assocedgeactionparam;
  typedef struct _dwg_object_ASSOCEDGECHAMFERACTIONBODY		dwg_obj_assocedgechamferactionbody;
  typedef struct _dwg_object_ASSOCEDGEFILLETACTIONBODY		dwg_obj_assocedgefilletactionbody;
  typedef struct _dwg_object_ASSOCMLEADERACTIONBODY		dwg_obj_assocmleaderactionbody;
  typedef struct _dwg_object_ASSOCORDINATEDIMACTIONBODY		dwg_obj_assocordinatedimactionbody;
  typedef struct _dwg_object_ASSOCPERSSUBENTMANAGER		dwg_obj_assocperssubentmanager;
  typedef struct _dwg_object_ASSOCRESTOREENTITYSTATEACTIONBODY		dwg_obj_assocrestoreentitystateactionbody;
  typedef struct _dwg_object_ASSOCROTATEDDIMACTIONBODY		dwg_obj_assocrotateddimactionbody;
  typedef struct _dwg_object_ASSOCSWEPTSURFACEACTIONBODY		dwg_obj_assocsweptsurfaceactionbody;
  typedef struct _dwg_object_BLOCKPROPERTIESTABLE		dwg_obj_blockpropertiestable;
  typedef struct _dwg_object_BLOCKPROPERTIESTABLEGRIP		dwg_obj_blockpropertiestablegrip;
  typedef struct _dwg_object_BREAKDATA		dwg_obj_breakdata;
  typedef struct _dwg_object_BREAKPOINTREF		dwg_obj_breakpointref;
  typedef struct _dwg_object_CONTEXTDATAMANAGER		dwg_obj_contextdatamanager;
  typedef struct _dwg_object_CSACDOCUMENTOPTIONS		dwg_obj_csacdocumentoptions;
  typedef struct _dwg_object_CURVEPATH		dwg_obj_curvepath;
  typedef struct _dwg_object_DATATABLE		dwg_obj_datatable;
  typedef struct _dwg_object_DIMASSOC		dwg_obj_dimassoc;
  typedef struct _dwg_object_DMDIMOBJECTCONTEXTDATA		dwg_obj_dmdimobjectcontextdata;
  typedef struct _dwg_object_DYNAMICBLOCKPROXYNODE		dwg_obj_dynamicblockproxynode;
  typedef struct _dwg_object_GEOMAPIMAGE		dwg_obj_geomapimage;
  typedef struct _dwg_object_MLEADEROBJECTCONTEXTDATA		dwg_obj_mleaderobjectcontextdata;
  typedef struct _dwg_object_MOTIONPATH		dwg_obj_motionpath;
  typedef struct _dwg_object_MTEXTATTRIBUTEOBJECTCONTEXTDATA		dwg_obj_mtextattributeobjectcontextdata;
  typedef struct _dwg_object_NAVISWORKSMODELDEF		dwg_obj_navisworksmodeldef;
  typedef struct _dwg_object_ORDDIMOBJECTCONTEXTDATA		dwg_obj_orddimobjectcontextdata;
  typedef struct _dwg_object_PERSUBENTMGR		dwg_obj_persubentmgr;
  typedef struct _dwg_object_POINTPATH		dwg_obj_pointpath;
  typedef struct _dwg_object_RADIMLGOBJECTCONTEXTDATA		dwg_obj_radimlgobjectcontextdata;
  typedef struct _dwg_object_RADIMOBJECTCONTEXTDATA		dwg_obj_radimobjectcontextdata;
  typedef struct _dwg_object_SUNSTUDY		dwg_obj_sunstudy;
  typedef struct _dwg_object_TABLECONTENT		dwg_obj_tablecontent;
  typedef struct _dwg_object_TVDEVICEPROPERTIES		dwg_obj_tvdeviceproperties;
//  typedef struct _dwg_object_ABSHDRAWINGSETTINGS		dwg_obj_abshdrawingsettings;
//  typedef struct _dwg_object_ACAECUSTOBJ		dwg_obj_acaecustobj;
//  typedef struct _dwg_object_ACAEEEMGROBJ		dwg_obj_acaeeemgrobj;
//  typedef struct _dwg_object_ACAMCOMP		dwg_obj_acamcomp;
//  typedef struct _dwg_object_ACAMCOMPDEF		dwg_obj_acamcompdef;
//  typedef struct _dwg_object_ACAMCOMPDEFMGR		dwg_obj_acamcompdefmgr;
//  typedef struct _dwg_object_ACAMCONTEXTMODELER		dwg_obj_acamcontextmodeler;
//  typedef struct _dwg_object_ACAMGDIMSTD		dwg_obj_acamgdimstd;
//  typedef struct _dwg_object_ACAMGFILTERDAT		dwg_obj_acamgfilterdat;
//  typedef struct _dwg_object_ACAMGHOLECHARTSTDCSN		dwg_obj_acamgholechartstdcsn;
//  typedef struct _dwg_object_ACAMGHOLECHARTSTDDIN		dwg_obj_acamgholechartstddin;
//  typedef struct _dwg_object_ACAMGHOLECHARTSTDISO		dwg_obj_acamgholechartstdiso;
//  typedef struct _dwg_object_ACAMGLAYSTD		dwg_obj_acamglaystd;
//  typedef struct _dwg_object_ACAMGRCOMPDEF		dwg_obj_acamgrcompdef;
//  typedef struct _dwg_object_ACAMGRCOMPDEFSET		dwg_obj_acamgrcompdefset;
//  typedef struct _dwg_object_ACAMGTITLESTD		dwg_obj_acamgtitlestd;
//  typedef struct _dwg_object_ACAMMVDBACKUPOBJECT		dwg_obj_acammvdbackupobject;
//  typedef struct _dwg_object_ACAMPROJECT		dwg_obj_acamproject;
//  typedef struct _dwg_object_ACAMSHAFTCOMPDEF		dwg_obj_acamshaftcompdef;
//  typedef struct _dwg_object_ACAMSTDPCOMPDEF		dwg_obj_acamstdpcompdef;
//  typedef struct _dwg_object_ACAMWBLOCKTEMPENTS		dwg_obj_acamwblocktempents;
//  typedef struct _dwg_object_ACARRAYJIGENTITY		dwg_obj_acarrayjigentity;
//  typedef struct _dwg_object_ACCMCONTEXT		dwg_obj_accmcontext;
//  typedef struct _dwg_object_ACDBCIRCARCRES		dwg_obj_acdbcircarcres;
//  typedef struct _dwg_object_ACDBDIMENSIONRES		dwg_obj_acdbdimensionres;
//  typedef struct _dwg_object_ACDBENTITYCACHE		dwg_obj_acdbentitycache;
//  typedef struct _dwg_object_ACDBLINERES		dwg_obj_acdblineres;
//  typedef struct _dwg_object_ACDBSTDPARTRES_ARC		dwg_obj_acdbstdpartres_arc;
//  typedef struct _dwg_object_ACDBSTDPARTRES_LINE		dwg_obj_acdbstdpartres_line;
//  typedef struct _dwg_object_ACDB_HATCHSCALECONTEXTDATA_CLASS		dwg_obj_acdb_hatchscalecontextdata_class;
//  typedef struct _dwg_object_ACDB_HATCHVIEWCONTEXTDATA_CLASS		dwg_obj_acdb_hatchviewcontextdata_class;
//  typedef struct _dwg_object_ACDB_PROXY_ENTITY_DATA		dwg_obj_acdb_proxy_entity_data;
//  typedef struct _dwg_object_ACDSRECORD		dwg_obj_acdsrecord;
//  typedef struct _dwg_object_ACDSSCHEMA		dwg_obj_acdsschema;
//  typedef struct _dwg_object_ACGREFACADMASTER		dwg_obj_acgrefacadmaster;
//  typedef struct _dwg_object_ACGREFMASTER		dwg_obj_acgrefmaster;
//  typedef struct _dwg_object_ACIMINTSYSVAR		dwg_obj_acimintsysvar;
//  typedef struct _dwg_object_ACIMREALSYSVAR		dwg_obj_acimrealsysvar;
//  typedef struct _dwg_object_ACIMSTRSYSVAR		dwg_obj_acimstrsysvar;
//  typedef struct _dwg_object_ACIMSYSVARMAN		dwg_obj_acimsysvarman;
//  typedef struct _dwg_object_ACMANOOTATIONVIEWSTANDARDANSI		dwg_obj_acmanootationviewstandardansi;
//  typedef struct _dwg_object_ACMANOOTATIONVIEWSTANDARDCSN		dwg_obj_acmanootationviewstandardcsn;
//  typedef struct _dwg_object_ACMANOOTATIONVIEWSTANDARDDIN		dwg_obj_acmanootationviewstandarddin;
//  typedef struct _dwg_object_ACMANOOTATIONVIEWSTANDARDISO		dwg_obj_acmanootationviewstandardiso;
//  typedef struct _dwg_object_ACMAPLEGENDDBOBJECT		dwg_obj_acmaplegenddbobject;
//  typedef struct _dwg_object_ACMAPLEGENDITEMDBOBJECT		dwg_obj_acmaplegenditemdbobject;
//  typedef struct _dwg_object_ACMAPMAPVIEWPORTDBOBJECT		dwg_obj_acmapmapviewportdbobject;
//  typedef struct _dwg_object_ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER		dwg_obj_acmapprintlayoutelementdbobjectcontainer;
//  typedef struct _dwg_object_ACMBALLOON		dwg_obj_acmballoon;
//  typedef struct _dwg_object_ACMBOM		dwg_obj_acmbom;
//  typedef struct _dwg_object_ACMBOMROW		dwg_obj_acmbomrow;
//  typedef struct _dwg_object_ACMBOMROWSTRUCT		dwg_obj_acmbomrowstruct;
//  typedef struct _dwg_object_ACMBOMSTANDARDANSI		dwg_obj_acmbomstandardansi;
//  typedef struct _dwg_object_ACMBOMSTANDARDCSN		dwg_obj_acmbomstandardcsn;
//  typedef struct _dwg_object_ACMBOMSTANDARDDIN		dwg_obj_acmbomstandarddin;
//  typedef struct _dwg_object_ACMBOMSTANDARDISO		dwg_obj_acmbomstandardiso;
//  typedef struct _dwg_object_ACMCENTERLINESTANDARDANSI		dwg_obj_acmcenterlinestandardansi;
//  typedef struct _dwg_object_ACMCENTERLINESTANDARDCSN		dwg_obj_acmcenterlinestandardcsn;
//  typedef struct _dwg_object_ACMCENTERLINESTANDARDDIN		dwg_obj_acmcenterlinestandarddin;
//  typedef struct _dwg_object_ACMCENTERLINESTANDARDISO		dwg_obj_acmcenterlinestandardiso;
//  typedef struct _dwg_object_ACMDATADICTIONARY		dwg_obj_acmdatadictionary;
//  typedef struct _dwg_object_ACMDATAENTRY		dwg_obj_acmdataentry;
//  typedef struct _dwg_object_ACMDATAENTRYBLOCK		dwg_obj_acmdataentryblock;
//  typedef struct _dwg_object_ACMDATUMID		dwg_obj_acmdatumid;
//  typedef struct _dwg_object_ACMDATUMSTANDARDANSI		dwg_obj_acmdatumstandardansi;
//  typedef struct _dwg_object_ACMDATUMSTANDARDCSN		dwg_obj_acmdatumstandardcsn;
//  typedef struct _dwg_object_ACMDATUMSTANDARDDIN		dwg_obj_acmdatumstandarddin;
//  typedef struct _dwg_object_ACMDATUMSTANDARDISO		dwg_obj_acmdatumstandardiso;
//  typedef struct _dwg_object_ACMDATUMSTANDARDISO2012		dwg_obj_acmdatumstandardiso2012;
//  typedef struct _dwg_object_ACMDETAILSTANDARDANSI		dwg_obj_acmdetailstandardansi;
//  typedef struct _dwg_object_ACMDETAILSTANDARDCSN		dwg_obj_acmdetailstandardcsn;
//  typedef struct _dwg_object_ACMDETAILSTANDARDDIN		dwg_obj_acmdetailstandarddin;
//  typedef struct _dwg_object_ACMDETAILSTANDARDISO		dwg_obj_acmdetailstandardiso;
//  typedef struct _dwg_object_ACMDETAILTANDARDCUSTOM		dwg_obj_acmdetailtandardcustom;
//  typedef struct _dwg_object_ACMDIMBREAKPERSREACTOR		dwg_obj_acmdimbreakpersreactor;
//  typedef struct _dwg_object_ACMEDRAWINGMAN		dwg_obj_acmedrawingman;
//  typedef struct _dwg_object_ACMEVIEW		dwg_obj_acmeview;
//  typedef struct _dwg_object_ACME_DATABASE		dwg_obj_acme_database;
//  typedef struct _dwg_object_ACME_DOCUMENT		dwg_obj_acme_document;
//  typedef struct _dwg_object_ACMFCFRAME		dwg_obj_acmfcframe;
//  typedef struct _dwg_object_ACMFCFSTANDARDANSI		dwg_obj_acmfcfstandardansi;
//  typedef struct _dwg_object_ACMFCFSTANDARDCSN		dwg_obj_acmfcfstandardcsn;
//  typedef struct _dwg_object_ACMFCFSTANDARDDIN		dwg_obj_acmfcfstandarddin;
//  typedef struct _dwg_object_ACMFCFSTANDARDISO		dwg_obj_acmfcfstandardiso;
//  typedef struct _dwg_object_ACMFCFSTANDARDISO2004		dwg_obj_acmfcfstandardiso2004;
//  typedef struct _dwg_object_ACMFCFSTANDARDISO2012		dwg_obj_acmfcfstandardiso2012;
//  typedef struct _dwg_object_ACMIDSTANDARDANSI		dwg_obj_acmidstandardansi;
//  typedef struct _dwg_object_ACMIDSTANDARDCSN		dwg_obj_acmidstandardcsn;
//  typedef struct _dwg_object_ACMIDSTANDARDDIN		dwg_obj_acmidstandarddin;
//  typedef struct _dwg_object_ACMIDSTANDARDISO		dwg_obj_acmidstandardiso;
//  typedef struct _dwg_object_ACMIDSTANDARDISO2004		dwg_obj_acmidstandardiso2004;
//  typedef struct _dwg_object_ACMIDSTANDARDISO2012		dwg_obj_acmidstandardiso2012;
//  typedef struct _dwg_object_ACMNOTESTANDARDANSI		dwg_obj_acmnotestandardansi;
//  typedef struct _dwg_object_ACMNOTESTANDARDCSN		dwg_obj_acmnotestandardcsn;
//  typedef struct _dwg_object_ACMNOTESTANDARDDIN		dwg_obj_acmnotestandarddin;
//  typedef struct _dwg_object_ACMNOTESTANDARDISO		dwg_obj_acmnotestandardiso;
//  typedef struct _dwg_object_ACMPARTLIST		dwg_obj_acmpartlist;
//  typedef struct _dwg_object_ACMPICKOBJ		dwg_obj_acmpickobj;
//  typedef struct _dwg_object_ACMSECTIONSTANDARDANSI		dwg_obj_acmsectionstandardansi;
//  typedef struct _dwg_object_ACMSECTIONSTANDARDCSN2002		dwg_obj_acmsectionstandardcsn2002;
//  typedef struct _dwg_object_ACMSECTIONSTANDARDCUSTOM		dwg_obj_acmsectionstandardcustom;
//  typedef struct _dwg_object_ACMSECTIONSTANDARDDIN		dwg_obj_acmsectionstandarddin;
//  typedef struct _dwg_object_ACMSECTIONSTANDARDISO		dwg_obj_acmsectionstandardiso;
//  typedef struct _dwg_object_ACMSECTIONSTANDARDISO2001		dwg_obj_acmsectionstandardiso2001;
//  typedef struct _dwg_object_ACMSTANDARDANSI		dwg_obj_acmstandardansi;
//  typedef struct _dwg_object_ACMSTANDARDCSN		dwg_obj_acmstandardcsn;
//  typedef struct _dwg_object_ACMSTANDARDDIN		dwg_obj_acmstandarddin;
//  typedef struct _dwg_object_ACMSTANDARDISO		dwg_obj_acmstandardiso;
//  typedef struct _dwg_object_ACMSURFSTANDARDANSI		dwg_obj_acmsurfstandardansi;
//  typedef struct _dwg_object_ACMSURFSTANDARDCSN		dwg_obj_acmsurfstandardcsn;
//  typedef struct _dwg_object_ACMSURFSTANDARDDIN		dwg_obj_acmsurfstandarddin;
//  typedef struct _dwg_object_ACMSURFSTANDARDISO		dwg_obj_acmsurfstandardiso;
//  typedef struct _dwg_object_ACMSURFSTANDARDISO2002		dwg_obj_acmsurfstandardiso2002;
//  typedef struct _dwg_object_ACMSURFSYM		dwg_obj_acmsurfsym;
//  typedef struct _dwg_object_ACMTAPERSTANDARDANSI		dwg_obj_acmtaperstandardansi;
//  typedef struct _dwg_object_ACMTAPERSTANDARDCSN		dwg_obj_acmtaperstandardcsn;
//  typedef struct _dwg_object_ACMTAPERSTANDARDDIN		dwg_obj_acmtaperstandarddin;
//  typedef struct _dwg_object_ACMTAPERSTANDARDISO		dwg_obj_acmtaperstandardiso;
//  typedef struct _dwg_object_ACMTHREADLINESTANDARDANSI		dwg_obj_acmthreadlinestandardansi;
//  typedef struct _dwg_object_ACMTHREADLINESTANDARDCSN		dwg_obj_acmthreadlinestandardcsn;
//  typedef struct _dwg_object_ACMTHREADLINESTANDARDDIN		dwg_obj_acmthreadlinestandarddin;
//  typedef struct _dwg_object_ACMTHREADLINESTANDARDISO		dwg_obj_acmthreadlinestandardiso;
//  typedef struct _dwg_object_ACMWELDSTANDARDANSI		dwg_obj_acmweldstandardansi;
//  typedef struct _dwg_object_ACMWELDSTANDARDCSN		dwg_obj_acmweldstandardcsn;
//  typedef struct _dwg_object_ACMWELDSTANDARDDIN		dwg_obj_acmweldstandarddin;
//  typedef struct _dwg_object_ACMWELDSTANDARDISO		dwg_obj_acmweldstandardiso;
//  typedef struct _dwg_object_ACMWELDSYM		dwg_obj_acmweldsym;
//  typedef struct _dwg_object_ACRFATTGENMGR		dwg_obj_acrfattgenmgr;
//  typedef struct _dwg_object_ACRFINSADJ		dwg_obj_acrfinsadj;
//  typedef struct _dwg_object_ACRFINSADJUSTERMGR		dwg_obj_acrfinsadjustermgr;
//  typedef struct _dwg_object_ACRFMCADAPIATTHOLDER		dwg_obj_acrfmcadapiattholder;
//  typedef struct _dwg_object_ACRFOBJATTMGR		dwg_obj_acrfobjattmgr;
//  typedef struct _dwg_object_ACSH_SUBENT_MATERIAL_CLASS		dwg_obj_acsh_subent_material_class;
//  typedef struct _dwg_object_AC_AM_2D_XREF_MGR		dwg_obj_ac_am_2d_xref_mgr;
//  typedef struct _dwg_object_AC_AM_BASIC_VIEW		dwg_obj_ac_am_basic_view;
//  typedef struct _dwg_object_AC_AM_BASIC_VIEW_DEF		dwg_obj_ac_am_basic_view_def;
//  typedef struct _dwg_object_AC_AM_COMPLEX_HIDE_SITUATION		dwg_obj_ac_am_complex_hide_situation;
//  typedef struct _dwg_object_AC_AM_COMP_VIEW_DEF		dwg_obj_ac_am_comp_view_def;
//  typedef struct _dwg_object_AC_AM_COMP_VIEW_INST		dwg_obj_ac_am_comp_view_inst;
//  typedef struct _dwg_object_AC_AM_DIRTY_NODES		dwg_obj_ac_am_dirty_nodes;
//  typedef struct _dwg_object_AC_AM_HIDE_SITUATION		dwg_obj_ac_am_hide_situation;
//  typedef struct _dwg_object_AC_AM_MAPPER_CACHE		dwg_obj_ac_am_mapper_cache;
//  typedef struct _dwg_object_AC_AM_MASTER_VIEW_DEF		dwg_obj_ac_am_master_view_def;
//  typedef struct _dwg_object_AC_AM_MVD_DEP_MGR		dwg_obj_ac_am_mvd_dep_mgr;
//  typedef struct _dwg_object_AC_AM_OVERRIDE_FILTER		dwg_obj_ac_am_override_filter;
//  typedef struct _dwg_object_AC_AM_PROPS_OVERRIDE		dwg_obj_ac_am_props_override;
//  typedef struct _dwg_object_AC_AM_SHAFT_HIDE_SITUATION		dwg_obj_ac_am_shaft_hide_situation;
//  typedef struct _dwg_object_AC_AM_STDP_VIEW_DEF		dwg_obj_ac_am_stdp_view_def;
//  typedef struct _dwg_object_AC_AM_TRANSFORM_GHOST		dwg_obj_ac_am_transform_ghost;
//  typedef struct _dwg_object_ADAPPL		dwg_obj_adappl;
//  typedef struct _dwg_object_AECC_ALIGNMENT_DESIGN_CHECK_SET		dwg_obj_aecc_alignment_design_check_set;
//  typedef struct _dwg_object_AECC_ALIGNMENT_LABEL_SET		dwg_obj_aecc_alignment_label_set;
//  typedef struct _dwg_object_AECC_ALIGNMENT_LABEL_SET_EXT		dwg_obj_aecc_alignment_label_set_ext;
//  typedef struct _dwg_object_AECC_ALIGNMENT_PARCEL_NODE		dwg_obj_aecc_alignment_parcel_node;
//  typedef struct _dwg_object_AECC_ALIGNMENT_STYLE		dwg_obj_aecc_alignment_style;
//  typedef struct _dwg_object_AECC_APPURTENANCE_STYLE		dwg_obj_aecc_appurtenance_style;
//  typedef struct _dwg_object_AECC_ASSEMBLY_STYLE		dwg_obj_aecc_assembly_style;
//  typedef struct _dwg_object_AECC_BUILDING_SITE_STYLE		dwg_obj_aecc_building_site_style;
//  typedef struct _dwg_object_AECC_CANT_DIAGRAM_VIEW_STYLE		dwg_obj_aecc_cant_diagram_view_style;
//  typedef struct _dwg_object_AECC_CATCHMENT_STYLE		dwg_obj_aecc_catchment_style;
//  typedef struct _dwg_object_AECC_CLASS_NODE		dwg_obj_aecc_class_node;
//  typedef struct _dwg_object_AECC_CONTOURVIEW		dwg_obj_aecc_contourview;
//  typedef struct _dwg_object_AECC_CORRIDOR_STYLE		dwg_obj_aecc_corridor_style;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT		dwg_obj_aecc_disp_rep_alignment;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP		dwg_obj_aecc_disp_rep_alignment_cant_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_CSV		dwg_obj_aecc_disp_rep_alignment_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_CURVE_LABEL		dwg_obj_aecc_disp_rep_alignment_curve_label;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP		dwg_obj_aecc_disp_rep_alignment_designspeed_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP		dwg_obj_aecc_disp_rep_alignment_geompt_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL		dwg_obj_aecc_disp_rep_alignment_indexed_pi_label;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP		dwg_obj_aecc_disp_rep_alignment_minor_station_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_PI_LABEL		dwg_obj_aecc_disp_rep_alignment_pi_label;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL		dwg_obj_aecc_disp_rep_alignment_spiral_label;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP		dwg_obj_aecc_disp_rep_alignment_staequ_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP		dwg_obj_aecc_disp_rep_alignment_station_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL		dwg_obj_aecc_disp_rep_alignment_station_offset_label;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP		dwg_obj_aecc_disp_rep_alignment_superelevation_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_TABLE		dwg_obj_aecc_disp_rep_alignment_table;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL		dwg_obj_aecc_disp_rep_alignment_tangent_label;
//  typedef struct _dwg_object_AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING		dwg_obj_aecc_disp_rep_alignment_vertical_geompt_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_APPURTENANCE		dwg_obj_aecc_disp_rep_appurtenance;
//  typedef struct _dwg_object_AECC_DISP_REP_APPURTENANCE_CSV		dwg_obj_aecc_disp_rep_appurtenance_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_APPURTENANCE_LABELING		dwg_obj_aecc_disp_rep_appurtenance_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING		dwg_obj_aecc_disp_rep_appurtenance_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_ASSEMBLY		dwg_obj_aecc_disp_rep_assembly;
//  typedef struct _dwg_object_AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE		dwg_obj_aecc_disp_rep_auto_corridor_feature_line;
//  typedef struct _dwg_object_AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE		dwg_obj_aecc_disp_rep_auto_corridor_feature_line_profile;
//  typedef struct _dwg_object_AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION		dwg_obj_aecc_disp_rep_auto_corridor_feature_line_section;
//  typedef struct _dwg_object_AECC_DISP_REP_AUTO_FEATURE_LINE		dwg_obj_aecc_disp_rep_auto_feature_line;
//  typedef struct _dwg_object_AECC_DISP_REP_AUTO_FEATURE_LINE_CSV		dwg_obj_aecc_disp_rep_auto_feature_line_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE		dwg_obj_aecc_disp_rep_auto_feature_line_profile;
//  typedef struct _dwg_object_AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION		dwg_obj_aecc_disp_rep_auto_feature_line_section;
//  typedef struct _dwg_object_AECC_DISP_REP_BUILDINGSITE		dwg_obj_aecc_disp_rep_buildingsite;
//  typedef struct _dwg_object_AECC_DISP_REP_BUILDINGUTIL_CONNECTOR		dwg_obj_aecc_disp_rep_buildingutil_connector;
//  typedef struct _dwg_object_AECC_DISP_REP_CANT_DIAGRAM_VIEW		dwg_obj_aecc_disp_rep_cant_diagram_view;
//  typedef struct _dwg_object_AECC_DISP_REP_CATCHMENT_AREA		dwg_obj_aecc_disp_rep_catchment_area;
//  typedef struct _dwg_object_AECC_DISP_REP_CATCHMENT_AREA_LABEL		dwg_obj_aecc_disp_rep_catchment_area_label;
//  typedef struct _dwg_object_AECC_DISP_REP_CORRIDOR		dwg_obj_aecc_disp_rep_corridor;
//  typedef struct _dwg_object_AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING		dwg_obj_aecc_disp_rep_crossing_pipe_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING		dwg_obj_aecc_disp_rep_crossing_pressure_pipe_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_CSVSTATIONSLIDER		dwg_obj_aecc_disp_rep_csvstationslider;
//  typedef struct _dwg_object_AECC_DISP_REP_FACE		dwg_obj_aecc_disp_rep_face;
//  typedef struct _dwg_object_AECC_DISP_REP_FEATURE		dwg_obj_aecc_disp_rep_feature;
//  typedef struct _dwg_object_AECC_DISP_REP_FEATURE_LABEL		dwg_obj_aecc_disp_rep_feature_label;
//  typedef struct _dwg_object_AECC_DISP_REP_FEATURE_LINE		dwg_obj_aecc_disp_rep_feature_line;
//  typedef struct _dwg_object_AECC_DISP_REP_FEATURE_LINE_CSV		dwg_obj_aecc_disp_rep_feature_line_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_FEATURE_LINE_PROFILE		dwg_obj_aecc_disp_rep_feature_line_profile;
//  typedef struct _dwg_object_AECC_DISP_REP_FEATURE_LINE_SECTION		dwg_obj_aecc_disp_rep_feature_line_section;
//  typedef struct _dwg_object_AECC_DISP_REP_FITTING		dwg_obj_aecc_disp_rep_fitting;
//  typedef struct _dwg_object_AECC_DISP_REP_FITTING_CSV		dwg_obj_aecc_disp_rep_fitting_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_FITTING_LABELING		dwg_obj_aecc_disp_rep_fitting_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_FITTING_PROFILE_LABELING		dwg_obj_aecc_disp_rep_fitting_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_FLOW_SEGMENT_LABEL		dwg_obj_aecc_disp_rep_flow_segment_label;
//  typedef struct _dwg_object_AECC_DISP_REP_GENERAL_SEGMENT_LABEL		dwg_obj_aecc_disp_rep_general_segment_label;
//  typedef struct _dwg_object_AECC_DISP_REP_GRADING		dwg_obj_aecc_disp_rep_grading;
//  typedef struct _dwg_object_AECC_DISP_REP_GRAPH		dwg_obj_aecc_disp_rep_graph;
//  typedef struct _dwg_object_AECC_DISP_REP_GRAPHPROFILE_NETWORKPART		dwg_obj_aecc_disp_rep_graphprofile_networkpart;
//  typedef struct _dwg_object_AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART		dwg_obj_aecc_disp_rep_graphprofile_pressurepart;
//  typedef struct _dwg_object_AECC_DISP_REP_GRID_SURFACE		dwg_obj_aecc_disp_rep_grid_surface;
//  typedef struct _dwg_object_AECC_DISP_REP_GRID_SURFACE_CSV		dwg_obj_aecc_disp_rep_grid_surface_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_horgeometry_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_HYDRO_REGION		dwg_obj_aecc_disp_rep_hydro_region;
//  typedef struct _dwg_object_AECC_DISP_REP_INTERFERENCE_CHECK		dwg_obj_aecc_disp_rep_interference_check;
//  typedef struct _dwg_object_AECC_DISP_REP_INTERFERENCE_PART		dwg_obj_aecc_disp_rep_interference_part;
//  typedef struct _dwg_object_AECC_DISP_REP_INTERFERENCE_PART_SECTION		dwg_obj_aecc_disp_rep_interference_part_section;
//  typedef struct _dwg_object_AECC_DISP_REP_INTERSECTION		dwg_obj_aecc_disp_rep_intersection;
//  typedef struct _dwg_object_AECC_DISP_REP_INTERSECTION_LOCATION_LABELING		dwg_obj_aecc_disp_rep_intersection_location_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_LABELING		dwg_obj_aecc_disp_rep_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_LEGEND_TABLE		dwg_obj_aecc_disp_rep_legend_table;
//  typedef struct _dwg_object_AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL		dwg_obj_aecc_disp_rep_line_between_points_label;
//  typedef struct _dwg_object_AECC_DISP_REP_LOTLINE_CSV		dwg_obj_aecc_disp_rep_lotline_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_MASSHAULLINE		dwg_obj_aecc_disp_rep_masshaulline;
//  typedef struct _dwg_object_AECC_DISP_REP_MASS_HAUL_VIEW		dwg_obj_aecc_disp_rep_mass_haul_view;
//  typedef struct _dwg_object_AECC_DISP_REP_MATCHLINE_LABELING		dwg_obj_aecc_disp_rep_matchline_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_MATCH_LINE		dwg_obj_aecc_disp_rep_match_line;
//  typedef struct _dwg_object_AECC_DISP_REP_MATERIAL_SECTION		dwg_obj_aecc_disp_rep_material_section;
//  typedef struct _dwg_object_AECC_DISP_REP_NETWORK		dwg_obj_aecc_disp_rep_network;
//  typedef struct _dwg_object_AECC_DISP_REP_NOTE_LABEL		dwg_obj_aecc_disp_rep_note_label;
//  typedef struct _dwg_object_AECC_DISP_REP_OFFSET_ELEV_LABEL		dwg_obj_aecc_disp_rep_offset_elev_label;
//  typedef struct _dwg_object_AECC_DISP_REP_PARCEL_BOUNDARY		dwg_obj_aecc_disp_rep_parcel_boundary;
//  typedef struct _dwg_object_AECC_DISP_REP_PARCEL_FACE_LABEL		dwg_obj_aecc_disp_rep_parcel_face_label;
//  typedef struct _dwg_object_AECC_DISP_REP_PARCEL_SEGMENT		dwg_obj_aecc_disp_rep_parcel_segment;
//  typedef struct _dwg_object_AECC_DISP_REP_PARCEL_SEGMENT_LABEL		dwg_obj_aecc_disp_rep_parcel_segment_label;
//  typedef struct _dwg_object_AECC_DISP_REP_PARCEL_SEGMENT_TABLE		dwg_obj_aecc_disp_rep_parcel_segment_table;
//  typedef struct _dwg_object_AECC_DISP_REP_PARCEL_TABLE		dwg_obj_aecc_disp_rep_parcel_table;
//  typedef struct _dwg_object_AECC_DISP_REP_PIPE		dwg_obj_aecc_disp_rep_pipe;
//  typedef struct _dwg_object_AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_pipenetwork_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_PIPE_CSV		dwg_obj_aecc_disp_rep_pipe_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_PIPE_LABELING		dwg_obj_aecc_disp_rep_pipe_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_PIPE_PROFILE_LABELING		dwg_obj_aecc_disp_rep_pipe_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_PIPE_SECTION_LABELING		dwg_obj_aecc_disp_rep_pipe_section_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_PIPE_TABLE		dwg_obj_aecc_disp_rep_pipe_table;
//  typedef struct _dwg_object_AECC_DISP_REP_POINT_ENT		dwg_obj_aecc_disp_rep_point_ent;
//  typedef struct _dwg_object_AECC_DISP_REP_POINT_GROUP		dwg_obj_aecc_disp_rep_point_group;
//  typedef struct _dwg_object_AECC_DISP_REP_POINT_TABLE		dwg_obj_aecc_disp_rep_point_table;
//  typedef struct _dwg_object_AECC_DISP_REP_PRESSUREPIPENETWORK		dwg_obj_aecc_disp_rep_pressurepipenetwork;
//  typedef struct _dwg_object_AECC_DISP_REP_PRESSURE_PART_TABLE		dwg_obj_aecc_disp_rep_pressure_part_table;
//  typedef struct _dwg_object_AECC_DISP_REP_PRESSURE_PIPE		dwg_obj_aecc_disp_rep_pressure_pipe;
//  typedef struct _dwg_object_AECC_DISP_REP_PRESSURE_PIPE_CSV		dwg_obj_aecc_disp_rep_pressure_pipe_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_PRESSURE_PIPE_LABELING		dwg_obj_aecc_disp_rep_pressure_pipe_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING		dwg_obj_aecc_disp_rep_pressure_pipe_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING		dwg_obj_aecc_disp_rep_pressure_pipe_section_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_PROFILE		dwg_obj_aecc_disp_rep_profile;
//  typedef struct _dwg_object_AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_profiledata_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_PROFILE_PROJECTION		dwg_obj_aecc_disp_rep_profile_projection;
//  typedef struct _dwg_object_AECC_DISP_REP_PROFILE_PROJECTION_LABEL		dwg_obj_aecc_disp_rep_profile_projection_label;
//  typedef struct _dwg_object_AECC_DISP_REP_PROFILE_VIEW		dwg_obj_aecc_disp_rep_profile_view;
//  typedef struct _dwg_object_AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL		dwg_obj_aecc_disp_rep_profile_view_depth_label;
//  typedef struct _dwg_object_AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE		dwg_obj_aecc_disp_rep_quantity_takeoff_aggregate_earthwork_table;
//  typedef struct _dwg_object_AECC_DISP_REP_RIGHT_OF_WAY		dwg_obj_aecc_disp_rep_right_of_way;
//  typedef struct _dwg_object_AECC_DISP_REP_SAMPLELINE_LABELING		dwg_obj_aecc_disp_rep_sampleline_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_SAMPLE_LINE		dwg_obj_aecc_disp_rep_sample_line;
//  typedef struct _dwg_object_AECC_DISP_REP_SAMPLE_LINE_GROUP		dwg_obj_aecc_disp_rep_sample_line_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION		dwg_obj_aecc_disp_rep_section;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_sectionaldata_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_sectiondata_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_sectionsegment_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_CORRIDOR		dwg_obj_aecc_disp_rep_section_corridor;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP		dwg_obj_aecc_disp_rep_section_corridor_point_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP		dwg_obj_aecc_disp_rep_section_gradebreak_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP		dwg_obj_aecc_disp_rep_section_minor_offset_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP		dwg_obj_aecc_disp_rep_section_offset_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_PIPENETWORK		dwg_obj_aecc_disp_rep_section_pipenetwork;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK		dwg_obj_aecc_disp_rep_section_pressurepipenetwork;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_PROJECTION		dwg_obj_aecc_disp_rep_section_projection;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_PROJECTION_LABEL		dwg_obj_aecc_disp_rep_section_projection_label;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP		dwg_obj_aecc_disp_rep_section_segment_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_VIEW		dwg_obj_aecc_disp_rep_section_view;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL		dwg_obj_aecc_disp_rep_section_view_depth_label;
//  typedef struct _dwg_object_AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE		dwg_obj_aecc_disp_rep_section_view_quantity_takeoff_table;
//  typedef struct _dwg_object_AECC_DISP_REP_SHEET		dwg_obj_aecc_disp_rep_sheet;
//  typedef struct _dwg_object_AECC_DISP_REP_SPANNING_PIPE_LABELING		dwg_obj_aecc_disp_rep_spanning_pipe_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING		dwg_obj_aecc_disp_rep_spanning_pipe_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_STATION_ELEV_LABEL		dwg_obj_aecc_disp_rep_station_elev_label;
//  typedef struct _dwg_object_AECC_DISP_REP_STRUCTURE		dwg_obj_aecc_disp_rep_structure;
//  typedef struct _dwg_object_AECC_DISP_REP_STRUCTURE_CSV		dwg_obj_aecc_disp_rep_structure_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_STRUCTURE_LABELING		dwg_obj_aecc_disp_rep_structure_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_STRUCTURE_PROFILE_LABELING		dwg_obj_aecc_disp_rep_structure_profile_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_STRUCTURE_SECTION_LABELING		dwg_obj_aecc_disp_rep_structure_section_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_SUBASSEMBLY		dwg_obj_aecc_disp_rep_subassembly;
//  typedef struct _dwg_object_AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_superelevation_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW		dwg_obj_aecc_disp_rep_superelevation_diagram_view;
//  typedef struct _dwg_object_AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP		dwg_obj_aecc_disp_rep_surface_contour_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SURFACE_ELEVATION_LABEL		dwg_obj_aecc_disp_rep_surface_elevation_label;
//  typedef struct _dwg_object_AECC_DISP_REP_SURFACE_SLOPE_LABEL		dwg_obj_aecc_disp_rep_surface_slope_label;
//  typedef struct _dwg_object_AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP		dwg_obj_aecc_disp_rep_survey_figure_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_SVFIGURE		dwg_obj_aecc_disp_rep_svfigure;
//  typedef struct _dwg_object_AECC_DISP_REP_SVFIGURE_CSV		dwg_obj_aecc_disp_rep_svfigure_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_SVFIGURE_PROFILE		dwg_obj_aecc_disp_rep_svfigure_profile;
//  typedef struct _dwg_object_AECC_DISP_REP_SVFIGURE_SECTION		dwg_obj_aecc_disp_rep_svfigure_section;
//  typedef struct _dwg_object_AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL		dwg_obj_aecc_disp_rep_svfigure_segment_label;
//  typedef struct _dwg_object_AECC_DISP_REP_SVNETWORK		dwg_obj_aecc_disp_rep_svnetwork;
//  typedef struct _dwg_object_AECC_DISP_REP_TANGENT_INTERSECTION_TABLE		dwg_obj_aecc_disp_rep_tangent_intersection_table;
//  typedef struct _dwg_object_AECC_DISP_REP_TIN_SURFACE		dwg_obj_aecc_disp_rep_tin_surface;
//  typedef struct _dwg_object_AECC_DISP_REP_TIN_SURFACE_CSV		dwg_obj_aecc_disp_rep_tin_surface_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP		dwg_obj_aecc_disp_rep_valignment_crestcurve_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_CSV		dwg_obj_aecc_disp_rep_valignment_csv;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP		dwg_obj_aecc_disp_rep_valignment_hageompt_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP		dwg_obj_aecc_disp_rep_valignment_line_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP		dwg_obj_aecc_disp_rep_valignment_minor_station_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP		dwg_obj_aecc_disp_rep_valignment_pvi_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP		dwg_obj_aecc_disp_rep_valignment_sagcurve_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP		dwg_obj_aecc_disp_rep_valignment_station_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP		dwg_obj_aecc_disp_rep_verticalgeometry_band_label_group;
//  typedef struct _dwg_object_AECC_DISP_REP_VIEWFRAME_LABELING		dwg_obj_aecc_disp_rep_viewframe_labeling;
//  typedef struct _dwg_object_AECC_DISP_REP_VIEW_FRAME		dwg_obj_aecc_disp_rep_view_frame;
//  typedef struct _dwg_object_AECC_FEATURELINE_STYLE		dwg_obj_aecc_featureline_style;
//  typedef struct _dwg_object_AECC_FEATURE_STYLE		dwg_obj_aecc_feature_style;
//  typedef struct _dwg_object_AECC_FITTING_STYLE		dwg_obj_aecc_fitting_style;
//  typedef struct _dwg_object_AECC_FORMAT_MANAGER_OBJECT		dwg_obj_aecc_format_manager_object;
//  typedef struct _dwg_object_AECC_GRADEVIEW		dwg_obj_aecc_gradeview;
//  typedef struct _dwg_object_AECC_GRADING_CRITERIA		dwg_obj_aecc_grading_criteria;
//  typedef struct _dwg_object_AECC_GRADING_CRITERIA_SET		dwg_obj_aecc_grading_criteria_set;
//  typedef struct _dwg_object_AECC_GRADING_GROUP		dwg_obj_aecc_grading_group;
//  typedef struct _dwg_object_AECC_GRADING_STYLE		dwg_obj_aecc_grading_style;
//  typedef struct _dwg_object_AECC_IMPORT_STORM_SEWER_DEFAULTS		dwg_obj_aecc_import_storm_sewer_defaults;
//  typedef struct _dwg_object_AECC_INTERFERENCE_STYLE		dwg_obj_aecc_interference_style;
//  typedef struct _dwg_object_AECC_INTERSECTION_STYLE		dwg_obj_aecc_intersection_style;
//  typedef struct _dwg_object_AECC_LABEL_COLLECTOR_STYLE		dwg_obj_aecc_label_collector_style;
//  typedef struct _dwg_object_AECC_LABEL_NODE		dwg_obj_aecc_label_node;
//  typedef struct _dwg_object_AECC_LABEL_RADIAL_LINE_STYLE		dwg_obj_aecc_label_radial_line_style;
//  typedef struct _dwg_object_AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE		dwg_obj_aecc_label_text_iterator_curve_or_spiral_style;
//  typedef struct _dwg_object_AECC_LABEL_TEXT_ITERATOR_STYLE		dwg_obj_aecc_label_text_iterator_style;
//  typedef struct _dwg_object_AECC_LABEL_TEXT_STYLE		dwg_obj_aecc_label_text_style;
//  typedef struct _dwg_object_AECC_LABEL_VECTOR_ARROW_STYLE		dwg_obj_aecc_label_vector_arrow_style;
//  typedef struct _dwg_object_AECC_LEGEND_TABLE_STYLE		dwg_obj_aecc_legend_table_style;
//  typedef struct _dwg_object_AECC_MASS_HAUL_LINE_STYLE		dwg_obj_aecc_mass_haul_line_style;
//  typedef struct _dwg_object_AECC_MASS_HAUL_VIEW_STYLE		dwg_obj_aecc_mass_haul_view_style;
//  typedef struct _dwg_object_AECC_MATCHLINE_STYLE		dwg_obj_aecc_matchline_style;
//  typedef struct _dwg_object_AECC_MATERIAL_STYLE		dwg_obj_aecc_material_style;
//  typedef struct _dwg_object_AECC_NETWORK_PART_CATALOG_DEF_NODE		dwg_obj_aecc_network_part_catalog_def_node;
//  typedef struct _dwg_object_AECC_NETWORK_PART_FAMILY_ITEM		dwg_obj_aecc_network_part_family_item;
//  typedef struct _dwg_object_AECC_NETWORK_PART_LIST		dwg_obj_aecc_network_part_list;
//  typedef struct _dwg_object_AECC_NETWORK_RULE		dwg_obj_aecc_network_rule;
//  typedef struct _dwg_object_AECC_PARCEL_NODE		dwg_obj_aecc_parcel_node;
//  typedef struct _dwg_object_AECC_PARCEL_STYLE		dwg_obj_aecc_parcel_style;
//  typedef struct _dwg_object_AECC_PART_SIZE_FILTER		dwg_obj_aecc_part_size_filter;
//  typedef struct _dwg_object_AECC_PIPE_RULES		dwg_obj_aecc_pipe_rules;
//  typedef struct _dwg_object_AECC_PIPE_STYLE		dwg_obj_aecc_pipe_style;
//  typedef struct _dwg_object_AECC_PIPE_STYLE_EXTENSION		dwg_obj_aecc_pipe_style_extension;
//  typedef struct _dwg_object_AECC_POINTCLOUD_STYLE		dwg_obj_aecc_pointcloud_style;
//  typedef struct _dwg_object_AECC_POINTVIEW		dwg_obj_aecc_pointview;
//  typedef struct _dwg_object_AECC_POINT_STYLE		dwg_obj_aecc_point_style;
//  typedef struct _dwg_object_AECC_PRESSURE_PART_LIST		dwg_obj_aecc_pressure_part_list;
//  typedef struct _dwg_object_AECC_PRESSURE_PIPE_STYLE		dwg_obj_aecc_pressure_pipe_style;
//  typedef struct _dwg_object_AECC_PROFILESECTIONENTITY_STYLE		dwg_obj_aecc_profilesectionentity_style;
//  typedef struct _dwg_object_AECC_PROFILE_DESIGN_CHECK_SET		dwg_obj_aecc_profile_design_check_set;
//  typedef struct _dwg_object_AECC_PROFILE_LABEL_SET		dwg_obj_aecc_profile_label_set;
//  typedef struct _dwg_object_AECC_PROFILE_STYLE		dwg_obj_aecc_profile_style;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_BAND_STYLE_SET		dwg_obj_aecc_profile_view_band_style_set;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_DATA_BAND_STYLE		dwg_obj_aecc_profile_view_data_band_style;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE		dwg_obj_aecc_profile_view_horizontal_geometry_band_style;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE		dwg_obj_aecc_profile_view_pipe_network_band_style;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE		dwg_obj_aecc_profile_view_sectional_data_band_style;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_STYLE		dwg_obj_aecc_profile_view_style;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE		dwg_obj_aecc_profile_view_superelevation_diagram_band_style;
//  typedef struct _dwg_object_AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE		dwg_obj_aecc_profile_view_vertical_geometry_band_style;
//  typedef struct _dwg_object_AECC_QUANTITY_TAKEOFF_CRITERIA		dwg_obj_aecc_quantity_takeoff_criteria;
//  typedef struct _dwg_object_AECC_ROADWAYLINK_STYLE		dwg_obj_aecc_roadwaylink_style;
//  typedef struct _dwg_object_AECC_ROADWAYMARKER_STYLE		dwg_obj_aecc_roadwaymarker_style;
//  typedef struct _dwg_object_AECC_ROADWAYSHAPE_STYLE		dwg_obj_aecc_roadwayshape_style;
//  typedef struct _dwg_object_AECC_ROADWAY_STYLE_SET		dwg_obj_aecc_roadway_style_set;
//  typedef struct _dwg_object_AECC_ROOT_SETTINGS_NODE		dwg_obj_aecc_root_settings_node;
//  typedef struct _dwg_object_AECC_SAMPLE_LINE_GROUP_STYLE		dwg_obj_aecc_sample_line_group_style;
//  typedef struct _dwg_object_AECC_SAMPLE_LINE_STYLE		dwg_obj_aecc_sample_line_style;
//  typedef struct _dwg_object_AECC_SECTION_LABEL_SET		dwg_obj_aecc_section_label_set;
//  typedef struct _dwg_object_AECC_SECTION_STYLE		dwg_obj_aecc_section_style;
//  typedef struct _dwg_object_AECC_SECTION_VIEW_BAND_STYLE_SET		dwg_obj_aecc_section_view_band_style_set;
//  typedef struct _dwg_object_AECC_SECTION_VIEW_DATA_BAND_STYLE		dwg_obj_aecc_section_view_data_band_style;
//  typedef struct _dwg_object_AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE		dwg_obj_aecc_section_view_road_surface_band_style;
//  typedef struct _dwg_object_AECC_SECTION_VIEW_STYLE		dwg_obj_aecc_section_view_style;
//  typedef struct _dwg_object_AECC_SETTINGS_NODE		dwg_obj_aecc_settings_node;
//  typedef struct _dwg_object_AECC_SHEET_STYLE		dwg_obj_aecc_sheet_style;
//  typedef struct _dwg_object_AECC_SLOPE_PATTERN_STYLE		dwg_obj_aecc_slope_pattern_style;
//  typedef struct _dwg_object_AECC_STATION_FORMAT_STYLE		dwg_obj_aecc_station_format_style;
//  typedef struct _dwg_object_AECC_STRUCTURE_RULES		dwg_obj_aecc_structure_rules;
//  typedef struct _dwg_object_AECC_STUCTURE_STYLE		dwg_obj_aecc_stucture_style;
//  typedef struct _dwg_object_AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE		dwg_obj_aecc_superelevation_diagram_view_style;
//  typedef struct _dwg_object_AECC_SURFACE_STYLE		dwg_obj_aecc_surface_style;
//  typedef struct _dwg_object_AECC_SVFIGURE_STYLE		dwg_obj_aecc_svfigure_style;
//  typedef struct _dwg_object_AECC_SVNETWORK_STYLE		dwg_obj_aecc_svnetwork_style;
//  typedef struct _dwg_object_AECC_TABLE_STYLE		dwg_obj_aecc_table_style;
//  typedef struct _dwg_object_AECC_TAG_MANAGER		dwg_obj_aecc_tag_manager;
//  typedef struct _dwg_object_AECC_TREE_NODE		dwg_obj_aecc_tree_node;
//  typedef struct _dwg_object_AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION		dwg_obj_aecc_user_defined_attribute_classification;
//  typedef struct _dwg_object_AECC_VALIGNMENT_STYLE_EXTENSION		dwg_obj_aecc_valignment_style_extension;
//  typedef struct _dwg_object_AECC_VIEW_FRAME_STYLE		dwg_obj_aecc_view_frame_style;
//  typedef struct _dwg_object_AECS_DISP_PROPS_MEMBER		dwg_obj_aecs_disp_props_member;
//  typedef struct _dwg_object_AECS_DISP_PROPS_MEMBER_LOGICAL		dwg_obj_aecs_disp_props_member_logical;
//  typedef struct _dwg_object_AECS_DISP_PROPS_MEMBER_PLAN		dwg_obj_aecs_disp_props_member_plan;
//  typedef struct _dwg_object_AECS_DISP_PROPS_MEMBER_PLAN_SKETCH		dwg_obj_aecs_disp_props_member_plan_sketch;
//  typedef struct _dwg_object_AECS_DISP_PROPS_MEMBER_PROJECTED		dwg_obj_aecs_disp_props_member_projected;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_ELEVATION_DESIGN		dwg_obj_aecs_disp_rep_member_elevation_design;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_ELEVATION_DETAIL		dwg_obj_aecs_disp_rep_member_elevation_detail;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_LOGICAL		dwg_obj_aecs_disp_rep_member_logical;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_MODEL_DESIGN		dwg_obj_aecs_disp_rep_member_model_design;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_MODEL_DETAIL		dwg_obj_aecs_disp_rep_member_model_detail;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_PLAN_DESIGN		dwg_obj_aecs_disp_rep_member_plan_design;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_PLAN_DETAIL		dwg_obj_aecs_disp_rep_member_plan_detail;
//  typedef struct _dwg_object_AECS_DISP_REP_MEMBER_PLAN_SKETCH		dwg_obj_aecs_disp_rep_member_plan_sketch;
//  typedef struct _dwg_object_AECS_MEMBER_NODE_SHAPE		dwg_obj_aecs_member_node_shape;
//  typedef struct _dwg_object_AECS_MEMBER_STYLE		dwg_obj_aecs_member_style;
//  typedef struct _dwg_object_AEC_2DSECTION_STYLE		dwg_obj_aec_2dsection_style;
//  typedef struct _dwg_object_AEC_AECDBDISPREPBDGELEVLINEPLAN100		dwg_obj_aec_aecdbdisprepbdgelevlineplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPBDGELEVLINEPLAN50		dwg_obj_aec_aecdbdisprepbdgelevlineplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPBDGSECTIONLINEPLAN100		dwg_obj_aec_aecdbdisprepbdgsectionlineplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPBDGSECTIONLINEPLAN50		dwg_obj_aec_aecdbdisprepbdgsectionlineplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCEILINGGRIDPLAN100		dwg_obj_aec_aecdbdisprepceilinggridplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCEILINGGRIDPLAN50		dwg_obj_aec_aecdbdisprepceilinggridplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCOLUMNGRIDPLAN100		dwg_obj_aec_aecdbdisprepcolumngridplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCOLUMNGRIDPLAN50		dwg_obj_aec_aecdbdisprepcolumngridplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100		dwg_obj_aec_aecdbdisprepcurtainwalllayoutplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50		dwg_obj_aec_aecdbdisprepcurtainwalllayoutplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCURTAINWALLUNITPLAN100		dwg_obj_aec_aecdbdisprepcurtainwallunitplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPCURTAINWALLUNITPLAN50		dwg_obj_aec_aecdbdisprepcurtainwallunitplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPMVBLOCKREFPLAN100		dwg_obj_aec_aecdbdisprepmvblockrefplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPMVBLOCKREFPLAN50		dwg_obj_aec_aecdbdisprepmvblockrefplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPROOFPLAN100		dwg_obj_aec_aecdbdispreproofplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPROOFPLAN50		dwg_obj_aec_aecdbdispreproofplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPROOFSLABPLAN100		dwg_obj_aec_aecdbdispreproofslabplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPROOFSLABPLAN50		dwg_obj_aec_aecdbdispreproofslabplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPSLABPLAN100		dwg_obj_aec_aecdbdisprepslabplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPSLABPLAN50		dwg_obj_aec_aecdbdisprepslabplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPSPACEPLAN100		dwg_obj_aec_aecdbdisprepspaceplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPSPACEPLAN50		dwg_obj_aec_aecdbdisprepspaceplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPWALLPLAN100		dwg_obj_aec_aecdbdisprepwallplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPWALLPLAN50		dwg_obj_aec_aecdbdisprepwallplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100		dwg_obj_aec_aecdbdisprepwindowassemblyplan100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50		dwg_obj_aec_aecdbdisprepwindowassemblyplan50;
//  typedef struct _dwg_object_AEC_AECDBDISPREPZONE100		dwg_obj_aec_aecdbdisprepzone100;
//  typedef struct _dwg_object_AEC_AECDBDISPREPZONE50		dwg_obj_aec_aecdbdisprepzone50;
//  typedef struct _dwg_object_AEC_AECDBZONEDEF		dwg_obj_aec_aecdbzonedef;
//  typedef struct _dwg_object_AEC_AECDBZONESTYLE		dwg_obj_aec_aecdbzonestyle;
//  typedef struct _dwg_object_AEC_ANCHOR_OPENINGBASE_TO_WALL		dwg_obj_aec_anchor_openingbase_to_wall;
//  typedef struct _dwg_object_AEC_CLASSIFICATION_DEF		dwg_obj_aec_classification_def;
//  typedef struct _dwg_object_AEC_CLASSIFICATION_SYSTEM_DEF		dwg_obj_aec_classification_system_def;
//  typedef struct _dwg_object_AEC_CLEANUP_GROUP_DEF		dwg_obj_aec_cleanup_group_def;
//  typedef struct _dwg_object_AEC_CURTAIN_WALL_LAYOUT_STYLE		dwg_obj_aec_curtain_wall_layout_style;
//  typedef struct _dwg_object_AEC_CURTAIN_WALL_UNIT_STYLE		dwg_obj_aec_curtain_wall_unit_style;
//  typedef struct _dwg_object_AEC_CVSECTIONVIEW		dwg_obj_aec_cvsectionview;
//  typedef struct _dwg_object_AEC_DB_DISP_REP_DIM_GROUP_PLAN		dwg_obj_aec_db_disp_rep_dim_group_plan;
//  typedef struct _dwg_object_AEC_DB_DISP_REP_DIM_GROUP_PLAN100		dwg_obj_aec_db_disp_rep_dim_group_plan100;
//  typedef struct _dwg_object_AEC_DB_DISP_REP_DIM_GROUP_PLAN50		dwg_obj_aec_db_disp_rep_dim_group_plan50;
//  typedef struct _dwg_object_AEC_DIM_STYLE		dwg_obj_aec_dim_style;
//  typedef struct _dwg_object_AEC_DISPLAYTHEME_STYLE		dwg_obj_aec_displaytheme_style;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPMASSELEMPLAN100		dwg_obj_aec_disprepaecdbdisprepmasselemplan100;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPMASSELEMPLAN50		dwg_obj_aec_disprepaecdbdisprepmasselemplan50;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100		dwg_obj_aec_disprepaecdbdisprepmassgroupplan100;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50		dwg_obj_aec_disprepaecdbdisprepmassgroupplan50;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPOPENINGPLAN100		dwg_obj_aec_disprepaecdbdisprepopeningplan100;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPOPENINGPLAN50		dwg_obj_aec_disprepaecdbdisprepopeningplan50;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED		dwg_obj_aec_disprepaecdbdisprepopeningplanreflected;
//  typedef struct _dwg_object_AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN		dwg_obj_aec_disprepaecdbdisprepopeningsillplan;
//  typedef struct _dwg_object_AEC_DISPROPSMASSELEMPLANCOMMON		dwg_obj_aec_dispropsmasselemplancommon;
//  typedef struct _dwg_object_AEC_DISPROPSMASSGROUPPLANCOMMON		dwg_obj_aec_dispropsmassgroupplancommon;
//  typedef struct _dwg_object_AEC_DISPROPSOPENINGPLANCOMMON		dwg_obj_aec_dispropsopeningplancommon;
//  typedef struct _dwg_object_AEC_DISPROPSOPENINGPLANCOMMONHATCHED		dwg_obj_aec_dispropsopeningplancommonhatched;
//  typedef struct _dwg_object_AEC_DISPROPSOPENINGSILLPLAN		dwg_obj_aec_dispropsopeningsillplan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_2D_SECTION		dwg_obj_aec_disp_props_2d_section;
//  typedef struct _dwg_object_AEC_DISP_PROPS_CLIP_VOLUME		dwg_obj_aec_disp_props_clip_volume;
//  typedef struct _dwg_object_AEC_DISP_PROPS_CLIP_VOLUME_RESULT		dwg_obj_aec_disp_props_clip_volume_result;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DIM		dwg_obj_aec_disp_props_dim;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DISPLAYTHEME		dwg_obj_aec_disp_props_displaytheme;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DOOR		dwg_obj_aec_disp_props_door;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DOOR_NOMINAL		dwg_obj_aec_disp_props_door_nominal;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DOOR_PLAN_100		dwg_obj_aec_disp_props_door_plan_100;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DOOR_PLAN_50		dwg_obj_aec_disp_props_door_plan_50;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN		dwg_obj_aec_disp_props_door_threshold_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN		dwg_obj_aec_disp_props_door_threshold_symbol_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL		dwg_obj_aec_disp_props_editinplaceprofile_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_ENT		dwg_obj_aec_disp_props_ent;
//  typedef struct _dwg_object_AEC_DISP_PROPS_ENT_REF		dwg_obj_aec_disp_props_ent_ref;
//  typedef struct _dwg_object_AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL		dwg_obj_aec_disp_props_grid_assembly_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN		dwg_obj_aec_disp_props_grid_assembly_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_LAYOUT_CURVE		dwg_obj_aec_disp_props_layout_curve;
//  typedef struct _dwg_object_AEC_DISP_PROPS_LAYOUT_GRID2D		dwg_obj_aec_disp_props_layout_grid2d;
//  typedef struct _dwg_object_AEC_DISP_PROPS_LAYOUT_GRID3D		dwg_obj_aec_disp_props_layout_grid3d;
//  typedef struct _dwg_object_AEC_DISP_PROPS_MASKBLOCK		dwg_obj_aec_disp_props_maskblock;
//  typedef struct _dwg_object_AEC_DISP_PROPS_MASS_ELEM_MODEL		dwg_obj_aec_disp_props_mass_elem_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_MASS_GROUP		dwg_obj_aec_disp_props_mass_group;
//  typedef struct _dwg_object_AEC_DISP_PROPS_MATERIAL		dwg_obj_aec_disp_props_material;
//  typedef struct _dwg_object_AEC_DISP_PROPS_OPENING		dwg_obj_aec_disp_props_opening;
//  typedef struct _dwg_object_AEC_DISP_PROPS_POLYGON_MODEL		dwg_obj_aec_disp_props_polygon_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_POLYGON_TRUECOLOUR		dwg_obj_aec_disp_props_polygon_truecolour;
//  typedef struct _dwg_object_AEC_DISP_PROPS_RAILING_MODEL		dwg_obj_aec_disp_props_railing_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_RAILING_PLAN		dwg_obj_aec_disp_props_railing_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_ROOF		dwg_obj_aec_disp_props_roof;
//  typedef struct _dwg_object_AEC_DISP_PROPS_ROOFSLAB		dwg_obj_aec_disp_props_roofslab;
//  typedef struct _dwg_object_AEC_DISP_PROPS_ROOFSLAB_PLAN		dwg_obj_aec_disp_props_roofslab_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_SCHEDULE_TABLE		dwg_obj_aec_disp_props_schedule_table;
//  typedef struct _dwg_object_AEC_DISP_PROPS_SLAB		dwg_obj_aec_disp_props_slab;
//  typedef struct _dwg_object_AEC_DISP_PROPS_SLAB_PLAN		dwg_obj_aec_disp_props_slab_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_SLICE		dwg_obj_aec_disp_props_slice;
//  typedef struct _dwg_object_AEC_DISP_PROPS_SPACE_DECOMPOSED		dwg_obj_aec_disp_props_space_decomposed;
//  typedef struct _dwg_object_AEC_DISP_PROPS_SPACE_MODEL		dwg_obj_aec_disp_props_space_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_SPACE_PLAN		dwg_obj_aec_disp_props_space_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_STAIR_MODEL		dwg_obj_aec_disp_props_stair_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_STAIR_PLAN		dwg_obj_aec_disp_props_stair_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING		dwg_obj_aec_disp_props_stair_plan_overlapping;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WALL_GRAPH		dwg_obj_aec_disp_props_wall_graph;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WALL_MODEL		dwg_obj_aec_disp_props_wall_model;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WALL_PLAN		dwg_obj_aec_disp_props_wall_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WALL_SCHEM		dwg_obj_aec_disp_props_wall_schem;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WINDOW		dwg_obj_aec_disp_props_window;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN		dwg_obj_aec_disp_props_window_assembly_sill_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WINDOW_NOMINAL		dwg_obj_aec_disp_props_window_nominal;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WINDOW_PLAN_100		dwg_obj_aec_disp_props_window_plan_100;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WINDOW_PLAN_50		dwg_obj_aec_disp_props_window_plan_50;
//  typedef struct _dwg_object_AEC_DISP_PROPS_WINDOW_SILL_PLAN		dwg_obj_aec_disp_props_window_sill_plan;
//  typedef struct _dwg_object_AEC_DISP_PROPS_ZONE		dwg_obj_aec_disp_props_zone;
//  typedef struct _dwg_object_AEC_DISP_REP_2D_SECTION		dwg_obj_aec_disp_rep_2d_section;
//  typedef struct _dwg_object_AEC_DISP_REP_ANCHOR		dwg_obj_aec_disp_rep_anchor;
//  typedef struct _dwg_object_AEC_DISP_REP_ANCHOR_BUB_TO_GRID		dwg_obj_aec_disp_rep_anchor_bub_to_grid;
//  typedef struct _dwg_object_AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL		dwg_obj_aec_disp_rep_anchor_bub_to_grid_model;
//  typedef struct _dwg_object_AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP		dwg_obj_aec_disp_rep_anchor_bub_to_grid_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_ANCHOR_ENT_TO_NODE		dwg_obj_aec_disp_rep_anchor_ent_to_node;
//  typedef struct _dwg_object_AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT		dwg_obj_aec_disp_rep_anchor_ext_tag_to_ent;
//  typedef struct _dwg_object_AEC_DISP_REP_ANCHOR_TAG_TO_ENT		dwg_obj_aec_disp_rep_anchor_tag_to_ent;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_ELEVLINE_MODEL		dwg_obj_aec_disp_rep_bdg_elevline_model;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_ELEVLINE_PLAN		dwg_obj_aec_disp_rep_bdg_elevline_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_ELEVLINE_RCP		dwg_obj_aec_disp_rep_bdg_elevline_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_SECTIONLINE_MODEL		dwg_obj_aec_disp_rep_bdg_sectionline_model;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_SECTIONLINE_PLAN		dwg_obj_aec_disp_rep_bdg_sectionline_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_SECTIONLINE_RCP		dwg_obj_aec_disp_rep_bdg_sectionline_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_SECTION_MODEL		dwg_obj_aec_disp_rep_bdg_section_model;
//  typedef struct _dwg_object_AEC_DISP_REP_BDG_SECTION_SUBDIV		dwg_obj_aec_disp_rep_bdg_section_subdiv;
//  typedef struct _dwg_object_AEC_DISP_REP_CEILING_GRID		dwg_obj_aec_disp_rep_ceiling_grid;
//  typedef struct _dwg_object_AEC_DISP_REP_CEILING_GRID_MODEL		dwg_obj_aec_disp_rep_ceiling_grid_model;
//  typedef struct _dwg_object_AEC_DISP_REP_CEILING_GRID_RCP		dwg_obj_aec_disp_rep_ceiling_grid_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_CLIP_VOLUME_MODEL		dwg_obj_aec_disp_rep_clip_volume_model;
//  typedef struct _dwg_object_AEC_DISP_REP_CLIP_VOLUME_PLAN		dwg_obj_aec_disp_rep_clip_volume_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_CLIP_VOLUME_RESULT		dwg_obj_aec_disp_rep_clip_volume_result;
//  typedef struct _dwg_object_AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV		dwg_obj_aec_disp_rep_clip_volume_result_subdiv;
//  typedef struct _dwg_object_AEC_DISP_REP_COLUMN_GRID		dwg_obj_aec_disp_rep_column_grid;
//  typedef struct _dwg_object_AEC_DISP_REP_COLUMN_GRID_MODEL		dwg_obj_aec_disp_rep_column_grid_model;
//  typedef struct _dwg_object_AEC_DISP_REP_COLUMN_GRID_RCP		dwg_obj_aec_disp_rep_column_grid_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_BLOCK		dwg_obj_aec_disp_rep_col_block;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CIRCARC2D		dwg_obj_aec_disp_rep_col_circarc2d;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONCOINCIDENT		dwg_obj_aec_disp_rep_col_concoincident;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONCONCENTRIC		dwg_obj_aec_disp_rep_col_conconcentric;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONEQUALDISTANCE		dwg_obj_aec_disp_rep_col_conequaldistance;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONMIDPOINT		dwg_obj_aec_disp_rep_col_conmidpoint;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONNECTOR		dwg_obj_aec_disp_rep_col_connector;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONNORMAL		dwg_obj_aec_disp_rep_col_connormal;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONPARALLEL		dwg_obj_aec_disp_rep_col_conparallel;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONPERPENDICULAR		dwg_obj_aec_disp_rep_col_conperpendicular;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONSYMMETRIC		dwg_obj_aec_disp_rep_col_consymmetric;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_CONTANGENT		dwg_obj_aec_disp_rep_col_contangent;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_DIMANGLE		dwg_obj_aec_disp_rep_col_dimangle;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_DIMDIAMETER		dwg_obj_aec_disp_rep_col_dimdiameter;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_DIMDISTANCE		dwg_obj_aec_disp_rep_col_dimdistance;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_DIMLENGTH		dwg_obj_aec_disp_rep_col_dimlength;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_DIMMAJORRADIUS		dwg_obj_aec_disp_rep_col_dimmajorradius;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_DIMMINORRADIUS		dwg_obj_aec_disp_rep_col_dimminorradius;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_ELLIPARC2D		dwg_obj_aec_disp_rep_col_elliparc2d;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_LAYOUTDATA		dwg_obj_aec_disp_rep_col_layoutdata;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_LINE2D		dwg_obj_aec_disp_rep_col_line2d;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_ADD		dwg_obj_aec_disp_rep_col_modifier_add;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_CUTPLANE		dwg_obj_aec_disp_rep_col_modifier_cutplane;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_EXTRUSION		dwg_obj_aec_disp_rep_col_modifier_extrusion;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_GROUP		dwg_obj_aec_disp_rep_col_modifier_group;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_LOFT		dwg_obj_aec_disp_rep_col_modifier_loft;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_PATH		dwg_obj_aec_disp_rep_col_modifier_path;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_REVOLVE		dwg_obj_aec_disp_rep_col_modifier_revolve;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_SUBTRACT		dwg_obj_aec_disp_rep_col_modifier_subtract;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_MODIFIER_TRANSITION		dwg_obj_aec_disp_rep_col_modifier_transition;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_POINT2D		dwg_obj_aec_disp_rep_col_point2d;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_PROFILE		dwg_obj_aec_disp_rep_col_profile;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_WORKPLANE		dwg_obj_aec_disp_rep_col_workplane;
//  typedef struct _dwg_object_AEC_DISP_REP_COL_WORKPLANE_REF		dwg_obj_aec_disp_rep_col_workplane_ref;
//  typedef struct _dwg_object_AEC_DISP_REP_CONFIG		dwg_obj_aec_disp_rep_config;
//  typedef struct _dwg_object_AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL		dwg_obj_aec_disp_rep_curtain_wall_layout_model;
//  typedef struct _dwg_object_AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN		dwg_obj_aec_disp_rep_curtain_wall_layout_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL		dwg_obj_aec_disp_rep_curtain_wall_unit_model;
//  typedef struct _dwg_object_AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN		dwg_obj_aec_disp_rep_curtain_wall_unit_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_DCM_DIMRADIUS		dwg_obj_aec_disp_rep_dcm_dimradius;
//  typedef struct _dwg_object_AEC_DISP_REP_DISPLAYTHEME		dwg_obj_aec_disp_rep_displaytheme;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_ELEV		dwg_obj_aec_disp_rep_door_elev;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_MODEL		dwg_obj_aec_disp_rep_door_model;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_NOMINAL		dwg_obj_aec_disp_rep_door_nominal;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_PLAN		dwg_obj_aec_disp_rep_door_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_PLAN_50		dwg_obj_aec_disp_rep_door_plan_50;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_PLAN_HEKTO		dwg_obj_aec_disp_rep_door_plan_hekto;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_RCP		dwg_obj_aec_disp_rep_door_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_THRESHOLD_PLAN		dwg_obj_aec_disp_rep_door_threshold_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN		dwg_obj_aec_disp_rep_door_threshold_symbol_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_EDITINPLACEPROFILE		dwg_obj_aec_disp_rep_editinplaceprofile;
//  typedef struct _dwg_object_AEC_DISP_REP_ENT_REF		dwg_obj_aec_disp_rep_ent_ref;
//  typedef struct _dwg_object_AEC_DISP_REP_LAYOUT_CURVE		dwg_obj_aec_disp_rep_layout_curve;
//  typedef struct _dwg_object_AEC_DISP_REP_LAYOUT_GRID2D		dwg_obj_aec_disp_rep_layout_grid2d;
//  typedef struct _dwg_object_AEC_DISP_REP_LAYOUT_GRID3D		dwg_obj_aec_disp_rep_layout_grid3d;
//  typedef struct _dwg_object_AEC_DISP_REP_MASKBLOCK_REF		dwg_obj_aec_disp_rep_maskblock_ref;
//  typedef struct _dwg_object_AEC_DISP_REP_MASKBLOCK_REF_RCP		dwg_obj_aec_disp_rep_maskblock_ref_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_MASS_ELEM_MODEL		dwg_obj_aec_disp_rep_mass_elem_model;
//  typedef struct _dwg_object_AEC_DISP_REP_MASS_ELEM_RCP		dwg_obj_aec_disp_rep_mass_elem_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_MASS_ELEM_SCHEM		dwg_obj_aec_disp_rep_mass_elem_schem;
//  typedef struct _dwg_object_AEC_DISP_REP_MASS_GROUP_MODEL		dwg_obj_aec_disp_rep_mass_group_model;
//  typedef struct _dwg_object_AEC_DISP_REP_MASS_GROUP_PLAN		dwg_obj_aec_disp_rep_mass_group_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_MASS_GROUP_RCP		dwg_obj_aec_disp_rep_mass_group_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_MATERIAL		dwg_obj_aec_disp_rep_material;
//  typedef struct _dwg_object_AEC_DISP_REP_MVBLOCK_REF		dwg_obj_aec_disp_rep_mvblock_ref;
//  typedef struct _dwg_object_AEC_DISP_REP_MVBLOCK_REF_MODEL		dwg_obj_aec_disp_rep_mvblock_ref_model;
//  typedef struct _dwg_object_AEC_DISP_REP_MVBLOCK_REF_RCP		dwg_obj_aec_disp_rep_mvblock_ref_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_OPENING		dwg_obj_aec_disp_rep_opening;
//  typedef struct _dwg_object_AEC_DISP_REP_OPENING_MODEL		dwg_obj_aec_disp_rep_opening_model;
//  typedef struct _dwg_object_AEC_DISP_REP_POLYGON_MODEL		dwg_obj_aec_disp_rep_polygon_model;
//  typedef struct _dwg_object_AEC_DISP_REP_POLYGON_TRUECOLOUR		dwg_obj_aec_disp_rep_polygon_truecolour;
//  typedef struct _dwg_object_AEC_DISP_REP_RAILING_MODEL		dwg_obj_aec_disp_rep_railing_model;
//  typedef struct _dwg_object_AEC_DISP_REP_RAILING_PLAN		dwg_obj_aec_disp_rep_railing_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_RAILING_PLAN_100		dwg_obj_aec_disp_rep_railing_plan_100;
//  typedef struct _dwg_object_AEC_DISP_REP_RAILING_PLAN_50		dwg_obj_aec_disp_rep_railing_plan_50;
//  typedef struct _dwg_object_AEC_DISP_REP_ROOFSLAB_MODEL		dwg_obj_aec_disp_rep_roofslab_model;
//  typedef struct _dwg_object_AEC_DISP_REP_ROOFSLAB_PLAN		dwg_obj_aec_disp_rep_roofslab_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_ROOF_MODEL		dwg_obj_aec_disp_rep_roof_model;
//  typedef struct _dwg_object_AEC_DISP_REP_ROOF_PLAN		dwg_obj_aec_disp_rep_roof_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_ROOF_RCP		dwg_obj_aec_disp_rep_roof_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_SCHEDULE_TABLE		dwg_obj_aec_disp_rep_schedule_table;
//  typedef struct _dwg_object_AEC_DISP_REP_SET		dwg_obj_aec_disp_rep_set;
//  typedef struct _dwg_object_AEC_DISP_REP_SLAB_MODEL		dwg_obj_aec_disp_rep_slab_model;
//  typedef struct _dwg_object_AEC_DISP_REP_SLAB_PLAN		dwg_obj_aec_disp_rep_slab_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_SLICE		dwg_obj_aec_disp_rep_slice;
//  typedef struct _dwg_object_AEC_DISP_REP_SPACE_DECOMPOSED		dwg_obj_aec_disp_rep_space_decomposed;
//  typedef struct _dwg_object_AEC_DISP_REP_SPACE_MODEL		dwg_obj_aec_disp_rep_space_model;
//  typedef struct _dwg_object_AEC_DISP_REP_SPACE_PLAN		dwg_obj_aec_disp_rep_space_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_SPACE_RCP		dwg_obj_aec_disp_rep_space_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_SPACE_VOLUME		dwg_obj_aec_disp_rep_space_volume;
//  typedef struct _dwg_object_AEC_DISP_REP_STAIR_MODEL		dwg_obj_aec_disp_rep_stair_model;
//  typedef struct _dwg_object_AEC_DISP_REP_STAIR_PLAN		dwg_obj_aec_disp_rep_stair_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_STAIR_PLAN_100		dwg_obj_aec_disp_rep_stair_plan_100;
//  typedef struct _dwg_object_AEC_DISP_REP_STAIR_PLAN_50		dwg_obj_aec_disp_rep_stair_plan_50;
//  typedef struct _dwg_object_AEC_DISP_REP_STAIR_PLAN_OVERLAPPING		dwg_obj_aec_disp_rep_stair_plan_overlapping;
//  typedef struct _dwg_object_AEC_DISP_REP_STAIR_RCP		dwg_obj_aec_disp_rep_stair_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_WALL_GRAPH		dwg_obj_aec_disp_rep_wall_graph;
//  typedef struct _dwg_object_AEC_DISP_REP_WALL_MODEL		dwg_obj_aec_disp_rep_wall_model;
//  typedef struct _dwg_object_AEC_DISP_REP_WALL_PLAN		dwg_obj_aec_disp_rep_wall_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_WALL_RCP		dwg_obj_aec_disp_rep_wall_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_WALL_SCHEM		dwg_obj_aec_disp_rep_wall_schem;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN		dwg_obj_aec_disp_rep_windowassembly_sill_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL		dwg_obj_aec_disp_rep_window_assembly_model;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN		dwg_obj_aec_disp_rep_window_assembly_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_ELEV		dwg_obj_aec_disp_rep_window_elev;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_MODEL		dwg_obj_aec_disp_rep_window_model;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_NOMINAL		dwg_obj_aec_disp_rep_window_nominal;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_PLAN		dwg_obj_aec_disp_rep_window_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_PLAN_100		dwg_obj_aec_disp_rep_window_plan_100;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_PLAN_50		dwg_obj_aec_disp_rep_window_plan_50;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_RCP		dwg_obj_aec_disp_rep_window_rcp;
//  typedef struct _dwg_object_AEC_DISP_REP_WINDOW_SILL_PLAN		dwg_obj_aec_disp_rep_window_sill_plan;
//  typedef struct _dwg_object_AEC_DISP_REP_ZONE		dwg_obj_aec_disp_rep_zone;
//  typedef struct _dwg_object_AEC_DISP_ROPS_RAILING_PLAN_100		dwg_obj_aec_disp_rops_railing_plan_100;
//  typedef struct _dwg_object_AEC_DISP_ROPS_RAILING_PLAN_50		dwg_obj_aec_disp_rops_railing_plan_50;
//  typedef struct _dwg_object_AEC_DISP_ROPS_STAIR_PLAN_100		dwg_obj_aec_disp_rops_stair_plan_100;
//  typedef struct _dwg_object_AEC_DISP_ROPS_STAIR_PLAN_50		dwg_obj_aec_disp_rops_stair_plan_50;
//  typedef struct _dwg_object_AEC_DOOR_STYLE		dwg_obj_aec_door_style;
//  typedef struct _dwg_object_AEC_ENDCAP_STYLE		dwg_obj_aec_endcap_style;
//  typedef struct _dwg_object_AEC_FRAME_DEF		dwg_obj_aec_frame_def;
//  typedef struct _dwg_object_AEC_LAYERKEY_STYLE		dwg_obj_aec_layerkey_style;
//  typedef struct _dwg_object_AEC_LIST_DEF		dwg_obj_aec_list_def;
//  typedef struct _dwg_object_AEC_MASKBLOCK_DEF		dwg_obj_aec_maskblock_def;
//  typedef struct _dwg_object_AEC_MASS_ELEM_STYLE		dwg_obj_aec_mass_elem_style;
//  typedef struct _dwg_object_AEC_MATERIAL_DEF		dwg_obj_aec_material_def;
//  typedef struct _dwg_object_AEC_MVBLOCK_DEF		dwg_obj_aec_mvblock_def;
//  typedef struct _dwg_object_AEC_MVBLOCK_REF		dwg_obj_aec_mvblock_ref;
//  typedef struct _dwg_object_AEC_NOTIFICATION_TRACKER		dwg_obj_aec_notification_tracker;
//  typedef struct _dwg_object_AEC_POLYGON		dwg_obj_aec_polygon;
//  typedef struct _dwg_object_AEC_POLYGON_STYLE		dwg_obj_aec_polygon_style;
//  typedef struct _dwg_object_AEC_PROPERTY_SET_DEF		dwg_obj_aec_property_set_def;
//  typedef struct _dwg_object_AEC_RAILING_STYLE		dwg_obj_aec_railing_style;
//  typedef struct _dwg_object_AEC_REFEDIT_STATUS_TRACKER		dwg_obj_aec_refedit_status_tracker;
//  typedef struct _dwg_object_AEC_ROOFSLABEDGE_STYLE		dwg_obj_aec_roofslabedge_style;
//  typedef struct _dwg_object_AEC_ROOFSLAB_STYLE		dwg_obj_aec_roofslab_style;
//  typedef struct _dwg_object_AEC_SCHEDULE_DATA_FORMAT		dwg_obj_aec_schedule_data_format;
//  typedef struct _dwg_object_AEC_SLABEDGE_STYLE		dwg_obj_aec_slabedge_style;
//  typedef struct _dwg_object_AEC_SLAB_STYLE		dwg_obj_aec_slab_style;
//  typedef struct _dwg_object_AEC_SPACE_STYLES		dwg_obj_aec_space_styles;
//  typedef struct _dwg_object_AEC_STAIR_STYLE		dwg_obj_aec_stair_style;
//  typedef struct _dwg_object_AEC_STAIR_WINDER_STYLE		dwg_obj_aec_stair_winder_style;
//  typedef struct _dwg_object_AEC_STAIR_WINDER_TYPE_BALANCED		dwg_obj_aec_stair_winder_type_balanced;
//  typedef struct _dwg_object_AEC_STAIR_WINDER_TYPE_MANUAL		dwg_obj_aec_stair_winder_type_manual;
//  typedef struct _dwg_object_AEC_STAIR_WINDER_TYPE_SINGLE_POINT		dwg_obj_aec_stair_winder_type_single_point;
//  typedef struct _dwg_object_AEC_VARS_AECBBLDSRV		dwg_obj_aec_vars_aecbbldsrv;
//  typedef struct _dwg_object_AEC_VARS_ARCHBASE		dwg_obj_aec_vars_archbase;
//  typedef struct _dwg_object_AEC_VARS_DWG_SETUP		dwg_obj_aec_vars_dwg_setup;
//  typedef struct _dwg_object_AEC_VARS_MUNICH		dwg_obj_aec_vars_munich;
//  typedef struct _dwg_object_AEC_VARS_STRUCTUREBASE		dwg_obj_aec_vars_structurebase;
//  typedef struct _dwg_object_AEC_WALLMOD_STYLE		dwg_obj_aec_wallmod_style;
//  typedef struct _dwg_object_AEC_WALL_STYLE		dwg_obj_aec_wall_style;
//  typedef struct _dwg_object_AEC_WINDOW_ASSEMBLY_STYLE		dwg_obj_aec_window_assembly_style;
//  typedef struct _dwg_object_AEC_WINDOW_STYLE		dwg_obj_aec_window_style;
//  typedef struct _dwg_object_ALIGNMENTGRIPENTITY		dwg_obj_alignmentgripentity;
//  typedef struct _dwg_object_AMCONTEXTMGR		dwg_obj_amcontextmgr;
//  typedef struct _dwg_object_AMDTADMENUSTATE		dwg_obj_amdtadmenustate;
//  typedef struct _dwg_object_AMDTAMMENUSTATE		dwg_obj_amdtammenustate;
//  typedef struct _dwg_object_AMDTBROWSERDBTAB		dwg_obj_amdtbrowserdbtab;
//  typedef struct _dwg_object_AMDTDMMENUSTATE		dwg_obj_amdtdmmenustate;
//  typedef struct _dwg_object_AMDTEDGESTANDARDDIN		dwg_obj_amdtedgestandarddin;
//  typedef struct _dwg_object_AMDTEDGESTANDARDDIN13715		dwg_obj_amdtedgestandarddin13715;
//  typedef struct _dwg_object_AMDTEDGESTANDARDISO		dwg_obj_amdtedgestandardiso;
//  typedef struct _dwg_object_AMDTEDGESTANDARDISO13715		dwg_obj_amdtedgestandardiso13715;
//  typedef struct _dwg_object_AMDTFORMULAUPDATEDISPATCHER		dwg_obj_amdtformulaupdatedispatcher;
//  typedef struct _dwg_object_AMDTINTERNALREACTOR		dwg_obj_amdtinternalreactor;
//  typedef struct _dwg_object_AMDTMCOMMENUSTATE		dwg_obj_amdtmcommenustate;
//  typedef struct _dwg_object_AMDTMENUSTATEMGR		dwg_obj_amdtmenustatemgr;
//  typedef struct _dwg_object_AMDTNOTE		dwg_obj_amdtnote;
//  typedef struct _dwg_object_AMDTNOTETEMPLATEDB		dwg_obj_amdtnotetemplatedb;
//  typedef struct _dwg_object_AMDTSECTIONSYM		dwg_obj_amdtsectionsym;
//  typedef struct _dwg_object_AMDTSECTIONSYMLABEL		dwg_obj_amdtsectionsymlabel;
//  typedef struct _dwg_object_AMDTSYSATTR		dwg_obj_amdtsysattr;
//  typedef struct _dwg_object_AMGOBJPROPCFG		dwg_obj_amgobjpropcfg;
//  typedef struct _dwg_object_AMGSETTINGSOBJ		dwg_obj_amgsettingsobj;
//  typedef struct _dwg_object_AMIMASTER		dwg_obj_amimaster;
//  typedef struct _dwg_object_AM_DRAWING_MGR		dwg_obj_am_drawing_mgr;
//  typedef struct _dwg_object_AM_DWGMGR_NAME		dwg_obj_am_dwgmgr_name;
//  typedef struct _dwg_object_AM_DWG_DOCUMENT		dwg_obj_am_dwg_document;
//  typedef struct _dwg_object_AM_DWG_SHEET		dwg_obj_am_dwg_sheet;
//  typedef struct _dwg_object_AM_VIEWDIMPARMAP		dwg_obj_am_viewdimparmap;
//  typedef struct _dwg_object_BINRECORD		dwg_obj_binrecord;
//  typedef struct _dwg_object_CAMSCATALOGAPPOBJECT		dwg_obj_camscatalogappobject;
//  typedef struct _dwg_object_CAMSSTRUCTBTNSTATE		dwg_obj_camsstructbtnstate;
//  typedef struct _dwg_object_CATALOGSTATE		dwg_obj_catalogstate;
//  typedef struct _dwg_object_CBROWSERAPPOBJECT		dwg_obj_cbrowserappobject;
//  typedef struct _dwg_object_DEPMGR		dwg_obj_depmgr;
//  typedef struct _dwg_object_DMBASEELEMENT		dwg_obj_dmbaseelement;
//  typedef struct _dwg_object_DMDEFAULTSTYLE		dwg_obj_dmdefaultstyle;
//  typedef struct _dwg_object_DMLEGEND		dwg_obj_dmlegend;
//  typedef struct _dwg_object_DMMAP		dwg_obj_dmmap;
//  typedef struct _dwg_object_DMMAPMANAGER		dwg_obj_dmmapmanager;
//  typedef struct _dwg_object_DMSTYLECATEGORY		dwg_obj_dmstylecategory;
//  typedef struct _dwg_object_DMSTYLELIBRARY		dwg_obj_dmstylelibrary;
//  typedef struct _dwg_object_DMSTYLEREFERENCE		dwg_obj_dmstylereference;
//  typedef struct _dwg_object_DMSTYLIZEDENTITIESTABLE		dwg_obj_dmstylizedentitiestable;
//  typedef struct _dwg_object_DMSURROGATESTYLESETS		dwg_obj_dmsurrogatestylesets;
//  typedef struct _dwg_object_DM_PLACEHOLDER		dwg_obj_dm_placeholder;
//  typedef struct _dwg_object_EXACTERMXREFMAP		dwg_obj_exactermxrefmap;
//  typedef struct _dwg_object_EXACXREFPANELOBJECT		dwg_obj_exacxrefpanelobject;
//  typedef struct _dwg_object_EXPO_NOTIFYBLOCK		dwg_obj_expo_notifyblock;
//  typedef struct _dwg_object_EXPO_NOTIFYHALL		dwg_obj_expo_notifyhall;
//  typedef struct _dwg_object_EXPO_NOTIFYPILLAR		dwg_obj_expo_notifypillar;
//  typedef struct _dwg_object_EXPO_NOTIFYSTAND		dwg_obj_expo_notifystand;
//  typedef struct _dwg_object_EXPO_NOTIFYSTANDNOPOLY		dwg_obj_expo_notifystandnopoly;
//  typedef struct _dwg_object_FLIPACTIONENTITY		dwg_obj_flipactionentity;
//  typedef struct _dwg_object_GSMANAGER		dwg_obj_gsmanager;
//  typedef struct _dwg_object_IRD_DSC_DICT		dwg_obj_ird_dsc_dict;
//  typedef struct _dwg_object_IRD_DSC_RECORD		dwg_obj_ird_dsc_record;
//  typedef struct _dwg_object_IRD_OBJ_RECORD		dwg_obj_ird_obj_record;
//  typedef struct _dwg_object_MAPFSMRVOBJECT		dwg_obj_mapfsmrvobject;
//  typedef struct _dwg_object_MAPGWSUNDOOBJECT		dwg_obj_mapgwsundoobject;
//  typedef struct _dwg_object_MAPIAMMOUDLE		dwg_obj_mapiammoudle;
//  typedef struct _dwg_object_MAPMETADATAOBJECT		dwg_obj_mapmetadataobject;
//  typedef struct _dwg_object_MAPRESOURCEMANAGEROBJECT		dwg_obj_mapresourcemanagerobject;
//  typedef struct _dwg_object_MOVEACTIONENTITY		dwg_obj_moveactionentity;
//  typedef struct _dwg_object_McDbContainer2		dwg_obj_mcdbcontainer2;
//  typedef struct _dwg_object_McDbMarker		dwg_obj_mcdbmarker;
//  typedef struct _dwg_object_NAMEDAPPL		dwg_obj_namedappl;
//  typedef struct _dwg_object_NEWSTDPARTPARLIST		dwg_obj_newstdpartparlist;
//  typedef struct _dwg_object_NPOCOLLECTION		dwg_obj_npocollection;
//  typedef struct _dwg_object_OBJCLONER		dwg_obj_objcloner;
//  typedef struct _dwg_object_PARAMMGR		dwg_obj_parammgr;
//  typedef struct _dwg_object_PARAMSCOPE		dwg_obj_paramscope;
//  typedef struct _dwg_object_PILLAR		dwg_obj_pillar;
//  typedef struct _dwg_object_RAPIDRTRENDERENVIRONMENT		dwg_obj_rapidrtrenderenvironment;
//  typedef struct _dwg_object_ROTATEACTIONENTITY		dwg_obj_rotateactionentity;
//  typedef struct _dwg_object_SCALEACTIONENTITY		dwg_obj_scaleactionentity;
//  typedef struct _dwg_object_STDPART2D		dwg_obj_stdpart2d;
//  typedef struct _dwg_object_STRETCHACTIONENTITY		dwg_obj_stretchactionentity;
//  typedef struct _dwg_object_TCH_ARROW		dwg_obj_tch_arrow;
//  typedef struct _dwg_object_TCH_AXIS_LABEL		dwg_obj_tch_axis_label;
//  typedef struct _dwg_object_TCH_BLOCK_INSERT		dwg_obj_tch_block_insert;
//  typedef struct _dwg_object_TCH_COLUMN		dwg_obj_tch_column;
//  typedef struct _dwg_object_TCH_DBCONFIG		dwg_obj_tch_dbconfig;
//  typedef struct _dwg_object_TCH_DIMENSION2		dwg_obj_tch_dimension2;
//  typedef struct _dwg_object_TCH_DRAWINGINDEX		dwg_obj_tch_drawingindex;
//  typedef struct _dwg_object_TCH_HANDRAIL		dwg_obj_tch_handrail;
//  typedef struct _dwg_object_TCH_LINESTAIR		dwg_obj_tch_linestair;
//  typedef struct _dwg_object_TCH_OPENING		dwg_obj_tch_opening;
//  typedef struct _dwg_object_TCH_RECTSTAIR		dwg_obj_tch_rectstair;
//  typedef struct _dwg_object_TCH_SLAB		dwg_obj_tch_slab;
//  typedef struct _dwg_object_TCH_SPACE		dwg_obj_tch_space;
//  typedef struct _dwg_object_TCH_TEXT		dwg_obj_tch_text;
//  typedef struct _dwg_object_TCH_WALL		dwg_obj_tch_wall;
//  typedef struct _dwg_object_TGrupoPuntos		dwg_obj_tgrupopuntos;
//  typedef struct _dwg_object_VAACIMAGEINVENTORY		dwg_obj_vaacimageinventory;
//  typedef struct _dwg_object_VAACXREFPANELOBJECT		dwg_obj_vaacxrefpanelobject;
//  typedef struct _dwg_object_XREFPANELOBJECT		dwg_obj_xrefpanelobject;


  dwg_get_OBJECT_DECL (ent__3dface, _3DFACE);
  dwg_get_OBJECT_DECL (ent__3dsolid, _3DSOLID);
  dwg_get_OBJECT_DECL (ent_arc, ARC);
  dwg_get_OBJECT_DECL (ent_attdef, ATTDEF);
  dwg_get_OBJECT_DECL (ent_attrib, ATTRIB);
  dwg_get_OBJECT_DECL (ent_block, BLOCK);
  dwg_get_OBJECT_DECL (ent_circle, CIRCLE);
  dwg_get_OBJECT_DECL (ent_dim_aligned, DIMENSION_ALIGNED);
  dwg_get_OBJECT_DECL (ent_dim_ang2ln, DIMENSION_ANG2LN);
  dwg_get_OBJECT_DECL (ent_dim_ang3pt, DIMENSION_ANG3PT);
  dwg_get_OBJECT_DECL (ent_dim_diameter, DIMENSION_DIAMETER);
  dwg_get_OBJECT_DECL (ent_dim_linear, DIMENSION_LINEAR);
  dwg_get_OBJECT_DECL (ent_dim_ordinate, DIMENSION_ORDINATE);
  dwg_get_OBJECT_DECL (ent_dim_radius, DIMENSION_RADIUS);
  dwg_get_OBJECT_DECL (ent_ellipse, ELLIPSE);
  dwg_get_OBJECT_DECL (ent_endblk, ENDBLK);
  dwg_get_OBJECT_DECL (ent_insert, INSERT);
  dwg_get_OBJECT_DECL (ent_leader, LEADER);
  dwg_get_OBJECT_DECL (ent_line, LINE);
  dwg_get_OBJECT_DECL (ent_load, LOAD);
  dwg_get_OBJECT_DECL (ent_minsert, MINSERT);
  dwg_get_OBJECT_DECL (ent_mline, MLINE);
  dwg_get_OBJECT_DECL (ent_mtext, MTEXT);
  dwg_get_OBJECT_DECL (ent_oleframe, OLEFRAME);
  dwg_get_OBJECT_DECL (ent_point, POINT);
  dwg_get_OBJECT_DECL (ent_polyline, POLYLINE);
  dwg_get_OBJECT_DECL (ent_polyline_2d, POLYLINE_2D);
  dwg_get_OBJECT_DECL (ent_polyline_3d, POLYLINE_3D);
  dwg_get_OBJECT_DECL (ent_polyline_mesh, POLYLINE_MESH);
  dwg_get_OBJECT_DECL (ent_polyline_pface, POLYLINE_PFACE);
  dwg_get_OBJECT_DECL (ent_proxy_entity, PROXY_ENTITY);
  dwg_get_OBJECT_DECL (ent_ray, RAY);
  dwg_get_OBJECT_DECL (ent_seqend, SEQEND);
  dwg_get_OBJECT_DECL (ent_shape, SHAPE);
  dwg_get_OBJECT_DECL (ent_solid, SOLID);
  dwg_get_OBJECT_DECL (ent_spline, SPLINE);
  dwg_get_OBJECT_DECL (ent_text, TEXT);
  dwg_get_OBJECT_DECL (ent_tolerance, TOLERANCE);
  dwg_get_OBJECT_DECL (ent_trace, TRACE);
  dwg_get_OBJECT_DECL (ent_unknown_ent, UNKNOWN_ENT);
  dwg_get_OBJECT_DECL (ent_vertex, VERTEX);
  dwg_get_OBJECT_DECL (ent_vertex_2d, VERTEX_2D);
  dwg_get_OBJECT_DECL (ent_vertex_3d, VERTEX_3D);
  dwg_get_OBJECT_DECL (ent_vert_pface_face, VERTEX_PFACE_FACE);
  dwg_get_OBJECT_DECL (ent_viewport, VIEWPORT);
  /* untyped > 500 */
  dwg_get_OBJECT_DECL (ent__3dline, _3DLINE);
  dwg_get_OBJECT_DECL (ent_camera, CAMERA);
  dwg_get_OBJECT_DECL (ent_dgnunderlay, DGNUNDERLAY);
  dwg_get_OBJECT_DECL (ent_dwfunderlay, DWFUNDERLAY);
  dwg_get_OBJECT_DECL (ent_endrep, ENDREP);
  dwg_get_OBJECT_DECL (ent_hatch, HATCH);
  dwg_get_OBJECT_DECL (ent_image, IMAGE);
  dwg_get_OBJECT_DECL (ent_jump, JUMP);
  dwg_get_OBJECT_DECL (ent_light, LIGHT);
  dwg_get_OBJECT_DECL (ent_lwpline, LWPOLYLINE);
  dwg_get_OBJECT_DECL (ent_mesh, MESH);
  dwg_get_OBJECT_DECL (ent_mleader, MULTILEADER);
  dwg_get_OBJECT_DECL (ent_ole2frame, OLE2FRAME);
  dwg_get_OBJECT_DECL (ent_pdfunderlay, PDFUNDERLAY);
  dwg_get_OBJECT_DECL (ent_repeat, REPEAT);
  dwg_get_OBJECT_DECL (ent_sectionobject, SECTIONOBJECT);
  dwg_get_OBJECT_DECL (ent_wipeout, WIPEOUT);
  /* unstable */
  dwg_get_OBJECT_DECL (ent_arc_dimension, ARC_DIMENSION);
  dwg_get_OBJECT_DECL (ent_helix, HELIX);
  dwg_get_OBJECT_DECL (ent_large_radial_dimension, LARGE_RADIAL_DIMENSION);
  dwg_get_OBJECT_DECL (ent_layoutprintconfig, LAYOUTPRINTCONFIG);
  dwg_get_OBJECT_DECL (ent_planesurface, PLANESURFACE);
  dwg_get_OBJECT_DECL (ent_pointcloud, POINTCLOUD);
  dwg_get_OBJECT_DECL (ent_pointcloudex, POINTCLOUDEX);
#ifdef DEBUG_CLASSES
    dwg_get_OBJECT_DECL (ent_alignmentparameterentity, ALIGNMENTPARAMETERENTITY);
    dwg_get_OBJECT_DECL (ent_arcalignedtext, ARCALIGNEDTEXT);
    dwg_get_OBJECT_DECL (ent_basepointparameterentity, BASEPOINTPARAMETERENTITY);
    dwg_get_OBJECT_DECL (ent_extrudedsurface, EXTRUDEDSURFACE);
    dwg_get_OBJECT_DECL (ent_flipgripentity, FLIPGRIPENTITY);
    dwg_get_OBJECT_DECL (ent_flipparameterentity, FLIPPARAMETERENTITY);
    dwg_get_OBJECT_DECL (ent_geopositionmarker, GEOPOSITIONMARKER);
    dwg_get_OBJECT_DECL (ent_lineargripentity, LINEARGRIPENTITY);
    dwg_get_OBJECT_DECL (ent_linearparameterentity, LINEARPARAMETERENTITY);
    dwg_get_OBJECT_DECL (ent_loftedsurface, LOFTEDSURFACE);
    dwg_get_OBJECT_DECL (ent_mpolygon, MPOLYGON);
    dwg_get_OBJECT_DECL (ent_navisworksmodel, NAVISWORKSMODEL);
    dwg_get_OBJECT_DECL (ent_nurbsurface, NURBSURFACE);
    dwg_get_OBJECT_DECL (ent_pointparameterentity, POINTPARAMETERENTITY);
    dwg_get_OBJECT_DECL (ent_polargripentity, POLARGRIPENTITY);
    dwg_get_OBJECT_DECL (ent_revolvedsurface, REVOLVEDSURFACE);
    dwg_get_OBJECT_DECL (ent_rotationgripentity, ROTATIONGRIPENTITY);
    dwg_get_OBJECT_DECL (ent_rotationparameterentity, ROTATIONPARAMETERENTITY);
    dwg_get_OBJECT_DECL (ent_rtext, RTEXT);
    dwg_get_OBJECT_DECL (ent_sweptsurface, SWEPTSURFACE);
    dwg_get_OBJECT_DECL (ent_table, TABLE);
    dwg_get_OBJECT_DECL (ent_visibilitygripentity, VISIBILITYGRIPENTITY);
    dwg_get_OBJECT_DECL (ent_visibilityparameterentity, VISIBILITYPARAMETERENTITY);
    dwg_get_OBJECT_DECL (ent_xygripentity, XYGRIPENTITY);
    dwg_get_OBJECT_DECL (ent_xyparameterentity, XYPARAMETERENTITY);
#endif

  dwg_get_OBJECT_DECL (obj_appid, APPID);
  dwg_get_OBJECT_DECL (obj_appid_control, APPID_CONTROL);
  dwg_get_OBJECT_DECL (obj_block_control, BLOCK_CONTROL);
  dwg_get_OBJECT_DECL (obj_block_header, BLOCK_HEADER);
  dwg_get_OBJECT_DECL (obj_dictionary, DICTIONARY);
  dwg_get_OBJECT_DECL (obj_dimstyle, DIMSTYLE);
  dwg_get_OBJECT_DECL (obj_dimstyle_control, DIMSTYLE_CONTROL);
  dwg_get_OBJECT_DECL (obj_dummy, DUMMY);
  dwg_get_OBJECT_DECL (obj_layer, LAYER);
  dwg_get_OBJECT_DECL (obj_layer_control, LAYER_CONTROL);
  dwg_get_OBJECT_DECL (obj_long_transaction, LONG_TRANSACTION);
  dwg_get_OBJECT_DECL (obj_ltype, LTYPE);
  dwg_get_OBJECT_DECL (obj_ltype_control, LTYPE_CONTROL);
  dwg_get_OBJECT_DECL (obj_mlinestyle, MLINESTYLE);
  dwg_get_OBJECT_DECL (obj_style, STYLE);
  dwg_get_OBJECT_DECL (obj_style_control, STYLE_CONTROL);
  dwg_get_OBJECT_DECL (obj_ucs, UCS);
  dwg_get_OBJECT_DECL (obj_ucs_control, UCS_CONTROL);
  dwg_get_OBJECT_DECL (obj_unknown_obj, UNKNOWN_OBJ);
  dwg_get_OBJECT_DECL (obj_view, VIEW);
  dwg_get_OBJECT_DECL (obj_view_control, VIEW_CONTROL);
  dwg_get_OBJECT_DECL (obj_vport, VPORT);
  dwg_get_OBJECT_DECL (obj_vport_control, VPORT_CONTROL);
  dwg_get_OBJECT_DECL (obj_vx_control, VX_CONTROL);
  dwg_get_OBJECT_DECL (obj_vx_table_record, VX_TABLE_RECORD);
  /* untyped > 500 */
  dwg_get_OBJECT_DECL (obj_acsh_boolean_class, ACSH_BOOLEAN_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_box_class, ACSH_BOX_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_cone_class, ACSH_CONE_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_cylinder_class, ACSH_CYLINDER_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_fillet_class, ACSH_FILLET_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_history_class, ACSH_HISTORY_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_sphere_class, ACSH_SPHERE_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_torus_class, ACSH_TORUS_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_wedge_class, ACSH_WEDGE_CLASS);
  dwg_get_OBJECT_DECL (obj_assocgeomdependency, ASSOCGEOMDEPENDENCY);
  dwg_get_OBJECT_DECL (obj_assocnetwork, ASSOCNETWORK);
  dwg_get_OBJECT_DECL (obj_blockalignmentgrip, BLOCKALIGNMENTGRIP);
  dwg_get_OBJECT_DECL (obj_blockalignmentparameter, BLOCKALIGNMENTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockbasepointparameter, BLOCKBASEPOINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockflipaction, BLOCKFLIPACTION);
  dwg_get_OBJECT_DECL (obj_blockflipgrip, BLOCKFLIPGRIP);
  dwg_get_OBJECT_DECL (obj_blockflipparameter, BLOCKFLIPPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockgriplocationcomponent, BLOCKGRIPLOCATIONCOMPONENT);
  dwg_get_OBJECT_DECL (obj_blocklineargrip, BLOCKLINEARGRIP);
  dwg_get_OBJECT_DECL (obj_blocklookupgrip, BLOCKLOOKUPGRIP);
  dwg_get_OBJECT_DECL (obj_blockmoveaction, BLOCKMOVEACTION);
  dwg_get_OBJECT_DECL (obj_blockrotateaction, BLOCKROTATEACTION);
  dwg_get_OBJECT_DECL (obj_blockrotationgrip, BLOCKROTATIONGRIP);
  dwg_get_OBJECT_DECL (obj_blockscaleaction, BLOCKSCALEACTION);
  dwg_get_OBJECT_DECL (obj_blockvisibilitygrip, BLOCKVISIBILITYGRIP);
  dwg_get_OBJECT_DECL (obj_cellstylemap, CELLSTYLEMAP);
  dwg_get_OBJECT_DECL (obj_detailviewstyle, DETAILVIEWSTYLE);
  dwg_get_OBJECT_DECL (obj_dictionaryvar, DICTIONARYVAR);
  dwg_get_OBJECT_DECL (obj_dictionarywdflt, DICTIONARYWDFLT);
  dwg_get_OBJECT_DECL (obj_dynamicblockpurgepreventer, DYNAMICBLOCKPURGEPREVENTER);
  dwg_get_OBJECT_DECL (obj_field, FIELD);
  dwg_get_OBJECT_DECL (obj_fieldlist, FIELDLIST);
  dwg_get_OBJECT_DECL (obj_geodata, GEODATA);
  dwg_get_OBJECT_DECL (obj_group, GROUP);
  dwg_get_OBJECT_DECL (obj_idbuffer, IDBUFFER);
  dwg_get_OBJECT_DECL (obj_imagedef, IMAGEDEF);
  dwg_get_OBJECT_DECL (obj_imagedef_reactor, IMAGEDEF_REACTOR);
  dwg_get_OBJECT_DECL (obj_index, INDEX);
  dwg_get_OBJECT_DECL (obj_layerfilter, LAYERFILTER);
  dwg_get_OBJECT_DECL (obj_layer_index, LAYER_INDEX);
  dwg_get_OBJECT_DECL (obj_layout, LAYOUT);
  dwg_get_OBJECT_DECL (obj_placeholder, PLACEHOLDER);
  dwg_get_OBJECT_DECL (obj_plotsettings, PLOTSETTINGS);
  dwg_get_OBJECT_DECL (obj_rastervariables, RASTERVARIABLES);
  dwg_get_OBJECT_DECL (obj_renderenvironment, RENDERENVIRONMENT);
  dwg_get_OBJECT_DECL (obj_scale, SCALE);
  dwg_get_OBJECT_DECL (obj_sectionviewstyle, SECTIONVIEWSTYLE);
  dwg_get_OBJECT_DECL (obj_section_manager, SECTION_MANAGER);
  dwg_get_OBJECT_DECL (obj_sortentstable, SORTENTSTABLE);
  dwg_get_OBJECT_DECL (obj_spatial_filter, SPATIAL_FILTER);
  dwg_get_OBJECT_DECL (obj_sun, SUN);
  dwg_get_OBJECT_DECL (obj_tablegeometry, TABLEGEOMETRY);
  dwg_get_OBJECT_DECL (obj_vba_project, VBA_PROJECT);
  dwg_get_OBJECT_DECL (obj_visualstyle, VISUALSTYLE);
  dwg_get_OBJECT_DECL (obj_wipeoutvariables, WIPEOUTVARIABLES);
  dwg_get_OBJECT_DECL (obj_xrecord, XRECORD);
  dwg_get_OBJECT_DECL (obj_pdfdefinition, PDFDEFINITION);
  dwg_get_OBJECT_DECL (obj_dgndefinition, DGNDEFINITION);
  dwg_get_OBJECT_DECL (obj_dwfdefinition, DWFDEFINITION);
  /* unstable */
  dwg_get_OBJECT_DECL (obj_acsh_brep_class, ACSH_BREP_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_chamfer_class, ACSH_CHAMFER_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_pyramid_class, ACSH_PYRAMID_CLASS);
  dwg_get_OBJECT_DECL (obj_aldimobjectcontextdata, ALDIMOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_assoc2dconstraintgroup, ASSOC2DCONSTRAINTGROUP);
  dwg_get_OBJECT_DECL (obj_assocaction, ASSOCACTION);
  dwg_get_OBJECT_DECL (obj_assocactionparam, ASSOCACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocarrayactionbody, ASSOCARRAYACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocasmbodyactionparam, ASSOCASMBODYACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocblendsurfaceactionbody, ASSOCBLENDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assoccompoundactionparam, ASSOCCOMPOUNDACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocdependency, ASSOCDEPENDENCY);
  dwg_get_OBJECT_DECL (obj_assocdimdependencybody, ASSOCDIMDEPENDENCYBODY);
  dwg_get_OBJECT_DECL (obj_assocextendsurfaceactionbody, ASSOCEXTENDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocextrudedsurfaceactionbody, ASSOCEXTRUDEDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocfaceactionparam, ASSOCFACEACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocfilletsurfaceactionbody, ASSOCFILLETSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocloftedsurfaceactionbody, ASSOCLOFTEDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocnetworksurfaceactionbody, ASSOCNETWORKSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocobjectactionparam, ASSOCOBJECTACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocoffsetsurfaceactionbody, ASSOCOFFSETSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocosnappointrefactionparam, ASSOCOSNAPPOINTREFACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocpatchsurfaceactionbody, ASSOCPATCHSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocpathactionparam, ASSOCPATHACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocplanesurfaceactionbody, ASSOCPLANESURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocpointrefactionparam, ASSOCPOINTREFACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocrevolvedsurfaceactionbody, ASSOCREVOLVEDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assoctrimsurfaceactionbody, ASSOCTRIMSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocvaluedependency, ASSOCVALUEDEPENDENCY);
  dwg_get_OBJECT_DECL (obj_assocvariable, ASSOCVARIABLE);
  dwg_get_OBJECT_DECL (obj_assocvertexactionparam, ASSOCVERTEXACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_blkrefobjectcontextdata, BLKREFOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_blockalignedconstraintparameter, BLOCKALIGNEDCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockangularconstraintparameter, BLOCKANGULARCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockarrayaction, BLOCKARRAYACTION);
  dwg_get_OBJECT_DECL (obj_blockdiametricconstraintparameter, BLOCKDIAMETRICCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockhorizontalconstraintparameter, BLOCKHORIZONTALCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blocklinearconstraintparameter, BLOCKLINEARCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blocklinearparameter, BLOCKLINEARPARAMETER);
  dwg_get_OBJECT_DECL (obj_blocklookupaction, BLOCKLOOKUPACTION);
  dwg_get_OBJECT_DECL (obj_blocklookupparameter, BLOCKLOOKUPPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockparamdependencybody, BLOCKPARAMDEPENDENCYBODY);
  dwg_get_OBJECT_DECL (obj_blockpointparameter, BLOCKPOINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockpolargrip, BLOCKPOLARGRIP);
  dwg_get_OBJECT_DECL (obj_blockpolarparameter, BLOCKPOLARPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockpolarstretchaction, BLOCKPOLARSTRETCHACTION);
  dwg_get_OBJECT_DECL (obj_blockradialconstraintparameter, BLOCKRADIALCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockrepresentation, BLOCKREPRESENTATION);
  dwg_get_OBJECT_DECL (obj_blockrotationparameter, BLOCKROTATIONPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockstretchaction, BLOCKSTRETCHACTION);
  dwg_get_OBJECT_DECL (obj_blockuserparameter, BLOCKUSERPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockverticalconstraintparameter, BLOCKVERTICALCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockvisibilityparameter, BLOCKVISIBILITYPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockxygrip, BLOCKXYGRIP);
  dwg_get_OBJECT_DECL (obj_blockxyparameter, BLOCKXYPARAMETER);
  dwg_get_OBJECT_DECL (obj_datalink, DATALINK);
  dwg_get_OBJECT_DECL (obj_dbcolor, DBCOLOR);
  dwg_get_OBJECT_DECL (obj_evaluation_graph, EVALUATION_GRAPH);
  dwg_get_OBJECT_DECL (obj_fcfobjectcontextdata, FCFOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_gradient_background, GRADIENT_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_ground_plane_background, GROUND_PLANE_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_ibl_background, IBL_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_image_background, IMAGE_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_leaderobjectcontextdata, LEADEROBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_lightlist, LIGHTLIST);
  dwg_get_OBJECT_DECL (obj_material, MATERIAL);
  dwg_get_OBJECT_DECL (obj_mentalrayrendersettings, MENTALRAYRENDERSETTINGS);
  dwg_get_OBJECT_DECL (obj_mleaderstyle, MLEADERSTYLE);
  dwg_get_OBJECT_DECL (obj_mtextobjectcontextdata, MTEXTOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_object_ptr, OBJECT_PTR);
  dwg_get_OBJECT_DECL (obj_partial_viewing_index, PARTIAL_VIEWING_INDEX);
  dwg_get_OBJECT_DECL (obj_pointcloudcolormap, POINTCLOUDCOLORMAP);
  dwg_get_OBJECT_DECL (obj_pointclouddef, POINTCLOUDDEF);
  dwg_get_OBJECT_DECL (obj_pointclouddefex, POINTCLOUDDEFEX);
  dwg_get_OBJECT_DECL (obj_pointclouddef_reactor, POINTCLOUDDEF_REACTOR);
  dwg_get_OBJECT_DECL (obj_pointclouddef_reactor_ex, POINTCLOUDDEF_REACTOR_EX);
  dwg_get_OBJECT_DECL (obj_proxy_object, PROXY_OBJECT);
  dwg_get_OBJECT_DECL (obj_rapidrtrendersettings, RAPIDRTRENDERSETTINGS);
  dwg_get_OBJECT_DECL (obj_renderentry, RENDERENTRY);
  dwg_get_OBJECT_DECL (obj_renderglobal, RENDERGLOBAL);
  dwg_get_OBJECT_DECL (obj_rendersettings, RENDERSETTINGS);
  dwg_get_OBJECT_DECL (obj_section_settings, SECTION_SETTINGS);
  dwg_get_OBJECT_DECL (obj_skylight_background, SKYLIGHT_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_solid_background, SOLID_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_spatial_index, SPATIAL_INDEX);
  dwg_get_OBJECT_DECL (obj_tablestyle, TABLESTYLE);
  dwg_get_OBJECT_DECL (obj_textobjectcontextdata, TEXTOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_assocarraymodifyparameters, ASSOCARRAYMODIFYPARAMETERS);
  dwg_get_OBJECT_DECL (obj_assocarraypathparameters, ASSOCARRAYPATHPARAMETERS);
  dwg_get_OBJECT_DECL (obj_assocarraypolarparameters, ASSOCARRAYPOLARPARAMETERS);
  dwg_get_OBJECT_DECL (obj_assocarrayrectangularparameters, ASSOCARRAYRECTANGULARPARAMETERS);
#ifdef DEBUG_CLASSES
    dwg_get_OBJECT_DECL (obj_acmecommandhistory, ACMECOMMANDHISTORY);
    dwg_get_OBJECT_DECL (obj_acmescope, ACMESCOPE);
    dwg_get_OBJECT_DECL (obj_acmestatemgr, ACMESTATEMGR);
    dwg_get_OBJECT_DECL (obj_acsh_extrusion_class, ACSH_EXTRUSION_CLASS);
    dwg_get_OBJECT_DECL (obj_acsh_loft_class, ACSH_LOFT_CLASS);
    dwg_get_OBJECT_DECL (obj_acsh_revolve_class, ACSH_REVOLVE_CLASS);
    dwg_get_OBJECT_DECL (obj_acsh_sweep_class, ACSH_SWEEP_CLASS);
    dwg_get_OBJECT_DECL (obj_angdimobjectcontextdata, ANGDIMOBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_annotscaleobjectcontextdata, ANNOTSCALEOBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_assoc3pointangulardimactionbody, ASSOC3POINTANGULARDIMACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocaligneddimactionbody, ASSOCALIGNEDDIMACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocarraymodifyactionbody, ASSOCARRAYMODIFYACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocedgeactionparam, ASSOCEDGEACTIONPARAM);
    dwg_get_OBJECT_DECL (obj_assocedgechamferactionbody, ASSOCEDGECHAMFERACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocedgefilletactionbody, ASSOCEDGEFILLETACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocmleaderactionbody, ASSOCMLEADERACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocordinatedimactionbody, ASSOCORDINATEDIMACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocperssubentmanager, ASSOCPERSSUBENTMANAGER);
    dwg_get_OBJECT_DECL (obj_assocrestoreentitystateactionbody, ASSOCRESTOREENTITYSTATEACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocrotateddimactionbody, ASSOCROTATEDDIMACTIONBODY);
    dwg_get_OBJECT_DECL (obj_assocsweptsurfaceactionbody, ASSOCSWEPTSURFACEACTIONBODY);
    dwg_get_OBJECT_DECL (obj_blockpropertiestable, BLOCKPROPERTIESTABLE);
    dwg_get_OBJECT_DECL (obj_blockpropertiestablegrip, BLOCKPROPERTIESTABLEGRIP);
    dwg_get_OBJECT_DECL (obj_breakdata, BREAKDATA);
    dwg_get_OBJECT_DECL (obj_breakpointref, BREAKPOINTREF);
    dwg_get_OBJECT_DECL (obj_contextdatamanager, CONTEXTDATAMANAGER);
    dwg_get_OBJECT_DECL (obj_csacdocumentoptions, CSACDOCUMENTOPTIONS);
    dwg_get_OBJECT_DECL (obj_curvepath, CURVEPATH);
    dwg_get_OBJECT_DECL (obj_datatable, DATATABLE);
    dwg_get_OBJECT_DECL (obj_dimassoc, DIMASSOC);
    dwg_get_OBJECT_DECL (obj_dmdimobjectcontextdata, DMDIMOBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_dynamicblockproxynode, DYNAMICBLOCKPROXYNODE);
    dwg_get_OBJECT_DECL (obj_geomapimage, GEOMAPIMAGE);
    dwg_get_OBJECT_DECL (obj_mleaderobjectcontextdata, MLEADEROBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_motionpath, MOTIONPATH);
    dwg_get_OBJECT_DECL (obj_mtextattributeobjectcontextdata, MTEXTATTRIBUTEOBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_navisworksmodeldef, NAVISWORKSMODELDEF);
    dwg_get_OBJECT_DECL (obj_orddimobjectcontextdata, ORDDIMOBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_persubentmgr, PERSUBENTMGR);
    dwg_get_OBJECT_DECL (obj_pointpath, POINTPATH);
    dwg_get_OBJECT_DECL (obj_radimlgobjectcontextdata, RADIMLGOBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_radimobjectcontextdata, RADIMOBJECTCONTEXTDATA);
    dwg_get_OBJECT_DECL (obj_sunstudy, SUNSTUDY);
    dwg_get_OBJECT_DECL (obj_tablecontent, TABLECONTENT);
    dwg_get_OBJECT_DECL (obj_tvdeviceproperties, TVDEVICEPROPERTIES);
//     dwg_get_OBJECT_DECL (obj_abshdrawingsettings, ABSHDRAWINGSETTINGS);
//     dwg_get_OBJECT_DECL (obj_acaecustobj, ACAECUSTOBJ);
//     dwg_get_OBJECT_DECL (obj_acaeeemgrobj, ACAEEEMGROBJ);
//     dwg_get_OBJECT_DECL (obj_acamcomp, ACAMCOMP);
//     dwg_get_OBJECT_DECL (obj_acamcompdef, ACAMCOMPDEF);
//     dwg_get_OBJECT_DECL (obj_acamcompdefmgr, ACAMCOMPDEFMGR);
//     dwg_get_OBJECT_DECL (obj_acamcontextmodeler, ACAMCONTEXTMODELER);
//     dwg_get_OBJECT_DECL (obj_acamgdimstd, ACAMGDIMSTD);
//     dwg_get_OBJECT_DECL (obj_acamgfilterdat, ACAMGFILTERDAT);
//     dwg_get_OBJECT_DECL (obj_acamgholechartstdcsn, ACAMGHOLECHARTSTDCSN);
//     dwg_get_OBJECT_DECL (obj_acamgholechartstddin, ACAMGHOLECHARTSTDDIN);
//     dwg_get_OBJECT_DECL (obj_acamgholechartstdiso, ACAMGHOLECHARTSTDISO);
//     dwg_get_OBJECT_DECL (obj_acamglaystd, ACAMGLAYSTD);
//     dwg_get_OBJECT_DECL (obj_acamgrcompdef, ACAMGRCOMPDEF);
//     dwg_get_OBJECT_DECL (obj_acamgrcompdefset, ACAMGRCOMPDEFSET);
//     dwg_get_OBJECT_DECL (obj_acamgtitlestd, ACAMGTITLESTD);
//     dwg_get_OBJECT_DECL (obj_acammvdbackupobject, ACAMMVDBACKUPOBJECT);
//     dwg_get_OBJECT_DECL (obj_acamproject, ACAMPROJECT);
//     dwg_get_OBJECT_DECL (obj_acamshaftcompdef, ACAMSHAFTCOMPDEF);
//     dwg_get_OBJECT_DECL (obj_acamstdpcompdef, ACAMSTDPCOMPDEF);
//     dwg_get_OBJECT_DECL (obj_acamwblocktempents, ACAMWBLOCKTEMPENTS);
//     dwg_get_OBJECT_DECL (obj_acarrayjigentity, ACARRAYJIGENTITY);
//     dwg_get_OBJECT_DECL (obj_accmcontext, ACCMCONTEXT);
//     dwg_get_OBJECT_DECL (obj_acdbcircarcres, ACDBCIRCARCRES);
//     dwg_get_OBJECT_DECL (obj_acdbdimensionres, ACDBDIMENSIONRES);
//     dwg_get_OBJECT_DECL (obj_acdbentitycache, ACDBENTITYCACHE);
//     dwg_get_OBJECT_DECL (obj_acdblineres, ACDBLINERES);
//     dwg_get_OBJECT_DECL (obj_acdbstdpartres_arc, ACDBSTDPARTRES_ARC);
//     dwg_get_OBJECT_DECL (obj_acdbstdpartres_line, ACDBSTDPARTRES_LINE);
//     dwg_get_OBJECT_DECL (obj_acdb_hatchscalecontextdata_class, ACDB_HATCHSCALECONTEXTDATA_CLASS);
//     dwg_get_OBJECT_DECL (obj_acdb_hatchviewcontextdata_class, ACDB_HATCHVIEWCONTEXTDATA_CLASS);
//     dwg_get_OBJECT_DECL (obj_acdb_proxy_entity_data, ACDB_PROXY_ENTITY_DATA);
//     dwg_get_OBJECT_DECL (obj_acdsrecord, ACDSRECORD);
//     dwg_get_OBJECT_DECL (obj_acdsschema, ACDSSCHEMA);
//     dwg_get_OBJECT_DECL (obj_acgrefacadmaster, ACGREFACADMASTER);
//     dwg_get_OBJECT_DECL (obj_acgrefmaster, ACGREFMASTER);
//     dwg_get_OBJECT_DECL (obj_acimintsysvar, ACIMINTSYSVAR);
//     dwg_get_OBJECT_DECL (obj_acimrealsysvar, ACIMREALSYSVAR);
//     dwg_get_OBJECT_DECL (obj_acimstrsysvar, ACIMSTRSYSVAR);
//     dwg_get_OBJECT_DECL (obj_acimsysvarman, ACIMSYSVARMAN);
//     dwg_get_OBJECT_DECL (obj_acmanootationviewstandardansi, ACMANOOTATIONVIEWSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmanootationviewstandardcsn, ACMANOOTATIONVIEWSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmanootationviewstandarddin, ACMANOOTATIONVIEWSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmanootationviewstandardiso, ACMANOOTATIONVIEWSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmaplegenddbobject, ACMAPLEGENDDBOBJECT);
//     dwg_get_OBJECT_DECL (obj_acmaplegenditemdbobject, ACMAPLEGENDITEMDBOBJECT);
//     dwg_get_OBJECT_DECL (obj_acmapmapviewportdbobject, ACMAPMAPVIEWPORTDBOBJECT);
//     dwg_get_OBJECT_DECL (obj_acmapprintlayoutelementdbobjectcontainer, ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER);
//     dwg_get_OBJECT_DECL (obj_acmballoon, ACMBALLOON);
//     dwg_get_OBJECT_DECL (obj_acmbom, ACMBOM);
//     dwg_get_OBJECT_DECL (obj_acmbomrow, ACMBOMROW);
//     dwg_get_OBJECT_DECL (obj_acmbomrowstruct, ACMBOMROWSTRUCT);
//     dwg_get_OBJECT_DECL (obj_acmbomstandardansi, ACMBOMSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmbomstandardcsn, ACMBOMSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmbomstandarddin, ACMBOMSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmbomstandardiso, ACMBOMSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmcenterlinestandardansi, ACMCENTERLINESTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmcenterlinestandardcsn, ACMCENTERLINESTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmcenterlinestandarddin, ACMCENTERLINESTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmcenterlinestandardiso, ACMCENTERLINESTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmdatadictionary, ACMDATADICTIONARY);
//     dwg_get_OBJECT_DECL (obj_acmdataentry, ACMDATAENTRY);
//     dwg_get_OBJECT_DECL (obj_acmdataentryblock, ACMDATAENTRYBLOCK);
//     dwg_get_OBJECT_DECL (obj_acmdatumid, ACMDATUMID);
//     dwg_get_OBJECT_DECL (obj_acmdatumstandardansi, ACMDATUMSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmdatumstandardcsn, ACMDATUMSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmdatumstandarddin, ACMDATUMSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmdatumstandardiso, ACMDATUMSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmdatumstandardiso2012, ACMDATUMSTANDARDISO2012);
//     dwg_get_OBJECT_DECL (obj_acmdetailstandardansi, ACMDETAILSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmdetailstandardcsn, ACMDETAILSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmdetailstandarddin, ACMDETAILSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmdetailstandardiso, ACMDETAILSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmdetailtandardcustom, ACMDETAILTANDARDCUSTOM);
//     dwg_get_OBJECT_DECL (obj_acmdimbreakpersreactor, ACMDIMBREAKPERSREACTOR);
//     dwg_get_OBJECT_DECL (obj_acmedrawingman, ACMEDRAWINGMAN);
//     dwg_get_OBJECT_DECL (obj_acmeview, ACMEVIEW);
//     dwg_get_OBJECT_DECL (obj_acme_database, ACME_DATABASE);
//     dwg_get_OBJECT_DECL (obj_acme_document, ACME_DOCUMENT);
//     dwg_get_OBJECT_DECL (obj_acmfcframe, ACMFCFRAME);
//     dwg_get_OBJECT_DECL (obj_acmfcfstandardansi, ACMFCFSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmfcfstandardcsn, ACMFCFSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmfcfstandarddin, ACMFCFSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmfcfstandardiso, ACMFCFSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmfcfstandardiso2004, ACMFCFSTANDARDISO2004);
//     dwg_get_OBJECT_DECL (obj_acmfcfstandardiso2012, ACMFCFSTANDARDISO2012);
//     dwg_get_OBJECT_DECL (obj_acmidstandardansi, ACMIDSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmidstandardcsn, ACMIDSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmidstandarddin, ACMIDSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmidstandardiso, ACMIDSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmidstandardiso2004, ACMIDSTANDARDISO2004);
//     dwg_get_OBJECT_DECL (obj_acmidstandardiso2012, ACMIDSTANDARDISO2012);
//     dwg_get_OBJECT_DECL (obj_acmnotestandardansi, ACMNOTESTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmnotestandardcsn, ACMNOTESTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmnotestandarddin, ACMNOTESTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmnotestandardiso, ACMNOTESTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmpartlist, ACMPARTLIST);
//     dwg_get_OBJECT_DECL (obj_acmpickobj, ACMPICKOBJ);
//     dwg_get_OBJECT_DECL (obj_acmsectionstandardansi, ACMSECTIONSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmsectionstandardcsn2002, ACMSECTIONSTANDARDCSN2002);
//     dwg_get_OBJECT_DECL (obj_acmsectionstandardcustom, ACMSECTIONSTANDARDCUSTOM);
//     dwg_get_OBJECT_DECL (obj_acmsectionstandarddin, ACMSECTIONSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmsectionstandardiso, ACMSECTIONSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmsectionstandardiso2001, ACMSECTIONSTANDARDISO2001);
//     dwg_get_OBJECT_DECL (obj_acmstandardansi, ACMSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmstandardcsn, ACMSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmstandarddin, ACMSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmstandardiso, ACMSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmsurfstandardansi, ACMSURFSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmsurfstandardcsn, ACMSURFSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmsurfstandarddin, ACMSURFSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmsurfstandardiso, ACMSURFSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmsurfstandardiso2002, ACMSURFSTANDARDISO2002);
//     dwg_get_OBJECT_DECL (obj_acmsurfsym, ACMSURFSYM);
//     dwg_get_OBJECT_DECL (obj_acmtaperstandardansi, ACMTAPERSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmtaperstandardcsn, ACMTAPERSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmtaperstandarddin, ACMTAPERSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmtaperstandardiso, ACMTAPERSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmthreadlinestandardansi, ACMTHREADLINESTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmthreadlinestandardcsn, ACMTHREADLINESTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmthreadlinestandarddin, ACMTHREADLINESTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmthreadlinestandardiso, ACMTHREADLINESTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmweldstandardansi, ACMWELDSTANDARDANSI);
//     dwg_get_OBJECT_DECL (obj_acmweldstandardcsn, ACMWELDSTANDARDCSN);
//     dwg_get_OBJECT_DECL (obj_acmweldstandarddin, ACMWELDSTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_acmweldstandardiso, ACMWELDSTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_acmweldsym, ACMWELDSYM);
//     dwg_get_OBJECT_DECL (obj_acrfattgenmgr, ACRFATTGENMGR);
//     dwg_get_OBJECT_DECL (obj_acrfinsadj, ACRFINSADJ);
//     dwg_get_OBJECT_DECL (obj_acrfinsadjustermgr, ACRFINSADJUSTERMGR);
//     dwg_get_OBJECT_DECL (obj_acrfmcadapiattholder, ACRFMCADAPIATTHOLDER);
//     dwg_get_OBJECT_DECL (obj_acrfobjattmgr, ACRFOBJATTMGR);
//     dwg_get_OBJECT_DECL (obj_acsh_subent_material_class, ACSH_SUBENT_MATERIAL_CLASS);
//     dwg_get_OBJECT_DECL (obj_ac_am_2d_xref_mgr, AC_AM_2D_XREF_MGR);
//     dwg_get_OBJECT_DECL (obj_ac_am_basic_view, AC_AM_BASIC_VIEW);
//     dwg_get_OBJECT_DECL (obj_ac_am_basic_view_def, AC_AM_BASIC_VIEW_DEF);
//     dwg_get_OBJECT_DECL (obj_ac_am_complex_hide_situation, AC_AM_COMPLEX_HIDE_SITUATION);
//     dwg_get_OBJECT_DECL (obj_ac_am_comp_view_def, AC_AM_COMP_VIEW_DEF);
//     dwg_get_OBJECT_DECL (obj_ac_am_comp_view_inst, AC_AM_COMP_VIEW_INST);
//     dwg_get_OBJECT_DECL (obj_ac_am_dirty_nodes, AC_AM_DIRTY_NODES);
//     dwg_get_OBJECT_DECL (obj_ac_am_hide_situation, AC_AM_HIDE_SITUATION);
//     dwg_get_OBJECT_DECL (obj_ac_am_mapper_cache, AC_AM_MAPPER_CACHE);
//     dwg_get_OBJECT_DECL (obj_ac_am_master_view_def, AC_AM_MASTER_VIEW_DEF);
//     dwg_get_OBJECT_DECL (obj_ac_am_mvd_dep_mgr, AC_AM_MVD_DEP_MGR);
//     dwg_get_OBJECT_DECL (obj_ac_am_override_filter, AC_AM_OVERRIDE_FILTER);
//     dwg_get_OBJECT_DECL (obj_ac_am_props_override, AC_AM_PROPS_OVERRIDE);
//     dwg_get_OBJECT_DECL (obj_ac_am_shaft_hide_situation, AC_AM_SHAFT_HIDE_SITUATION);
//     dwg_get_OBJECT_DECL (obj_ac_am_stdp_view_def, AC_AM_STDP_VIEW_DEF);
//     dwg_get_OBJECT_DECL (obj_ac_am_transform_ghost, AC_AM_TRANSFORM_GHOST);
//     dwg_get_OBJECT_DECL (obj_adappl, ADAPPL);
//     dwg_get_OBJECT_DECL (obj_aecc_alignment_design_check_set, AECC_ALIGNMENT_DESIGN_CHECK_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_alignment_label_set, AECC_ALIGNMENT_LABEL_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_alignment_label_set_ext, AECC_ALIGNMENT_LABEL_SET_EXT);
//     dwg_get_OBJECT_DECL (obj_aecc_alignment_parcel_node, AECC_ALIGNMENT_PARCEL_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_alignment_style, AECC_ALIGNMENT_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_appurtenance_style, AECC_APPURTENANCE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_assembly_style, AECC_ASSEMBLY_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_building_site_style, AECC_BUILDING_SITE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_cant_diagram_view_style, AECC_CANT_DIAGRAM_VIEW_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_catchment_style, AECC_CATCHMENT_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_class_node, AECC_CLASS_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_contourview, AECC_CONTOURVIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_corridor_style, AECC_CORRIDOR_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment, AECC_DISP_REP_ALIGNMENT);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_cant_label_group, AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_csv, AECC_DISP_REP_ALIGNMENT_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_curve_label, AECC_DISP_REP_ALIGNMENT_CURVE_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_designspeed_label_group, AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_geompt_label_group, AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_indexed_pi_label, AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_minor_station_label_group, AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_pi_label, AECC_DISP_REP_ALIGNMENT_PI_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_spiral_label, AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_staequ_label_group, AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_station_label_group, AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_station_offset_label, AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_superelevation_label_group, AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_table, AECC_DISP_REP_ALIGNMENT_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_tangent_label, AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_alignment_vertical_geompt_labeling, AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_appurtenance, AECC_DISP_REP_APPURTENANCE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_appurtenance_csv, AECC_DISP_REP_APPURTENANCE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_appurtenance_labeling, AECC_DISP_REP_APPURTENANCE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_appurtenance_profile_labeling, AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_assembly, AECC_DISP_REP_ASSEMBLY);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_auto_corridor_feature_line, AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_auto_corridor_feature_line_profile, AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_auto_corridor_feature_line_section, AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_auto_feature_line, AECC_DISP_REP_AUTO_FEATURE_LINE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_auto_feature_line_csv, AECC_DISP_REP_AUTO_FEATURE_LINE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_auto_feature_line_profile, AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_auto_feature_line_section, AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_buildingsite, AECC_DISP_REP_BUILDINGSITE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_buildingutil_connector, AECC_DISP_REP_BUILDINGUTIL_CONNECTOR);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_cant_diagram_view, AECC_DISP_REP_CANT_DIAGRAM_VIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_catchment_area, AECC_DISP_REP_CATCHMENT_AREA);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_catchment_area_label, AECC_DISP_REP_CATCHMENT_AREA_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_corridor, AECC_DISP_REP_CORRIDOR);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_crossing_pipe_profile_labeling, AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_crossing_pressure_pipe_profile_labeling, AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_csvstationslider, AECC_DISP_REP_CSVSTATIONSLIDER);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_face, AECC_DISP_REP_FACE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_feature, AECC_DISP_REP_FEATURE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_feature_label, AECC_DISP_REP_FEATURE_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_feature_line, AECC_DISP_REP_FEATURE_LINE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_feature_line_csv, AECC_DISP_REP_FEATURE_LINE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_feature_line_profile, AECC_DISP_REP_FEATURE_LINE_PROFILE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_feature_line_section, AECC_DISP_REP_FEATURE_LINE_SECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_fitting, AECC_DISP_REP_FITTING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_fitting_csv, AECC_DISP_REP_FITTING_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_fitting_labeling, AECC_DISP_REP_FITTING_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_fitting_profile_labeling, AECC_DISP_REP_FITTING_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_flow_segment_label, AECC_DISP_REP_FLOW_SEGMENT_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_general_segment_label, AECC_DISP_REP_GENERAL_SEGMENT_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_grading, AECC_DISP_REP_GRADING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_graph, AECC_DISP_REP_GRAPH);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_graphprofile_networkpart, AECC_DISP_REP_GRAPHPROFILE_NETWORKPART);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_graphprofile_pressurepart, AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_grid_surface, AECC_DISP_REP_GRID_SURFACE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_grid_surface_csv, AECC_DISP_REP_GRID_SURFACE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_horgeometry_band_label_group, AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_hydro_region, AECC_DISP_REP_HYDRO_REGION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_interference_check, AECC_DISP_REP_INTERFERENCE_CHECK);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_interference_part, AECC_DISP_REP_INTERFERENCE_PART);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_interference_part_section, AECC_DISP_REP_INTERFERENCE_PART_SECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_intersection, AECC_DISP_REP_INTERSECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_intersection_location_labeling, AECC_DISP_REP_INTERSECTION_LOCATION_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_labeling, AECC_DISP_REP_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_legend_table, AECC_DISP_REP_LEGEND_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_line_between_points_label, AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_lotline_csv, AECC_DISP_REP_LOTLINE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_masshaulline, AECC_DISP_REP_MASSHAULLINE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_mass_haul_view, AECC_DISP_REP_MASS_HAUL_VIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_matchline_labeling, AECC_DISP_REP_MATCHLINE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_match_line, AECC_DISP_REP_MATCH_LINE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_material_section, AECC_DISP_REP_MATERIAL_SECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_network, AECC_DISP_REP_NETWORK);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_note_label, AECC_DISP_REP_NOTE_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_offset_elev_label, AECC_DISP_REP_OFFSET_ELEV_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_parcel_boundary, AECC_DISP_REP_PARCEL_BOUNDARY);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_parcel_face_label, AECC_DISP_REP_PARCEL_FACE_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_parcel_segment, AECC_DISP_REP_PARCEL_SEGMENT);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_parcel_segment_label, AECC_DISP_REP_PARCEL_SEGMENT_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_parcel_segment_table, AECC_DISP_REP_PARCEL_SEGMENT_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_parcel_table, AECC_DISP_REP_PARCEL_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pipe, AECC_DISP_REP_PIPE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pipenetwork_band_label_group, AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pipe_csv, AECC_DISP_REP_PIPE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pipe_labeling, AECC_DISP_REP_PIPE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pipe_profile_labeling, AECC_DISP_REP_PIPE_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pipe_section_labeling, AECC_DISP_REP_PIPE_SECTION_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pipe_table, AECC_DISP_REP_PIPE_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_point_ent, AECC_DISP_REP_POINT_ENT);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_point_group, AECC_DISP_REP_POINT_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_point_table, AECC_DISP_REP_POINT_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pressurepipenetwork, AECC_DISP_REP_PRESSUREPIPENETWORK);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pressure_part_table, AECC_DISP_REP_PRESSURE_PART_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pressure_pipe, AECC_DISP_REP_PRESSURE_PIPE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pressure_pipe_csv, AECC_DISP_REP_PRESSURE_PIPE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pressure_pipe_labeling, AECC_DISP_REP_PRESSURE_PIPE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pressure_pipe_profile_labeling, AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_pressure_pipe_section_labeling, AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_profile, AECC_DISP_REP_PROFILE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_profiledata_band_label_group, AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_profile_projection, AECC_DISP_REP_PROFILE_PROJECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_profile_projection_label, AECC_DISP_REP_PROFILE_PROJECTION_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_profile_view, AECC_DISP_REP_PROFILE_VIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_profile_view_depth_label, AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_quantity_takeoff_aggregate_earthwork_table, AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_right_of_way, AECC_DISP_REP_RIGHT_OF_WAY);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_sampleline_labeling, AECC_DISP_REP_SAMPLELINE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_sample_line, AECC_DISP_REP_SAMPLE_LINE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_sample_line_group, AECC_DISP_REP_SAMPLE_LINE_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section, AECC_DISP_REP_SECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_sectionaldata_band_label_group, AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_sectiondata_band_label_group, AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_sectionsegment_band_label_group, AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_corridor, AECC_DISP_REP_SECTION_CORRIDOR);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_corridor_point_label_group, AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_gradebreak_label_group, AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_minor_offset_label_group, AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_offset_label_group, AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_pipenetwork, AECC_DISP_REP_SECTION_PIPENETWORK);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_pressurepipenetwork, AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_projection, AECC_DISP_REP_SECTION_PROJECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_projection_label, AECC_DISP_REP_SECTION_PROJECTION_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_segment_label_group, AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_view, AECC_DISP_REP_SECTION_VIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_view_depth_label, AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_section_view_quantity_takeoff_table, AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_sheet, AECC_DISP_REP_SHEET);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_spanning_pipe_labeling, AECC_DISP_REP_SPANNING_PIPE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_spanning_pipe_profile_labeling, AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_station_elev_label, AECC_DISP_REP_STATION_ELEV_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_structure, AECC_DISP_REP_STRUCTURE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_structure_csv, AECC_DISP_REP_STRUCTURE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_structure_labeling, AECC_DISP_REP_STRUCTURE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_structure_profile_labeling, AECC_DISP_REP_STRUCTURE_PROFILE_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_structure_section_labeling, AECC_DISP_REP_STRUCTURE_SECTION_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_subassembly, AECC_DISP_REP_SUBASSEMBLY);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_superelevation_band_label_group, AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_superelevation_diagram_view, AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_surface_contour_label_group, AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_surface_elevation_label, AECC_DISP_REP_SURFACE_ELEVATION_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_surface_slope_label, AECC_DISP_REP_SURFACE_SLOPE_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_survey_figure_label_group, AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_svfigure, AECC_DISP_REP_SVFIGURE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_svfigure_csv, AECC_DISP_REP_SVFIGURE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_svfigure_profile, AECC_DISP_REP_SVFIGURE_PROFILE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_svfigure_section, AECC_DISP_REP_SVFIGURE_SECTION);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_svfigure_segment_label, AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_svnetwork, AECC_DISP_REP_SVNETWORK);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_tangent_intersection_table, AECC_DISP_REP_TANGENT_INTERSECTION_TABLE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_tin_surface, AECC_DISP_REP_TIN_SURFACE);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_tin_surface_csv, AECC_DISP_REP_TIN_SURFACE_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_crestcurve_label_group, AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_csv, AECC_DISP_REP_VALIGNMENT_CSV);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_hageompt_label_group, AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_line_label_group, AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_minor_station_label_group, AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_pvi_label_group, AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_sagcurve_label_group, AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_valignment_station_label_group, AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_verticalgeometry_band_label_group, AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_viewframe_labeling, AECC_DISP_REP_VIEWFRAME_LABELING);
//     dwg_get_OBJECT_DECL (obj_aecc_disp_rep_view_frame, AECC_DISP_REP_VIEW_FRAME);
//     dwg_get_OBJECT_DECL (obj_aecc_featureline_style, AECC_FEATURELINE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_feature_style, AECC_FEATURE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_fitting_style, AECC_FITTING_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_format_manager_object, AECC_FORMAT_MANAGER_OBJECT);
//     dwg_get_OBJECT_DECL (obj_aecc_gradeview, AECC_GRADEVIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_grading_criteria, AECC_GRADING_CRITERIA);
//     dwg_get_OBJECT_DECL (obj_aecc_grading_criteria_set, AECC_GRADING_CRITERIA_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_grading_group, AECC_GRADING_GROUP);
//     dwg_get_OBJECT_DECL (obj_aecc_grading_style, AECC_GRADING_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_import_storm_sewer_defaults, AECC_IMPORT_STORM_SEWER_DEFAULTS);
//     dwg_get_OBJECT_DECL (obj_aecc_interference_style, AECC_INTERFERENCE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_intersection_style, AECC_INTERSECTION_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_label_collector_style, AECC_LABEL_COLLECTOR_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_label_node, AECC_LABEL_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_label_radial_line_style, AECC_LABEL_RADIAL_LINE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_label_text_iterator_curve_or_spiral_style, AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_label_text_iterator_style, AECC_LABEL_TEXT_ITERATOR_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_label_text_style, AECC_LABEL_TEXT_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_label_vector_arrow_style, AECC_LABEL_VECTOR_ARROW_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_legend_table_style, AECC_LEGEND_TABLE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_mass_haul_line_style, AECC_MASS_HAUL_LINE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_mass_haul_view_style, AECC_MASS_HAUL_VIEW_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_matchline_style, AECC_MATCHLINE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_material_style, AECC_MATERIAL_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_network_part_catalog_def_node, AECC_NETWORK_PART_CATALOG_DEF_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_network_part_family_item, AECC_NETWORK_PART_FAMILY_ITEM);
//     dwg_get_OBJECT_DECL (obj_aecc_network_part_list, AECC_NETWORK_PART_LIST);
//     dwg_get_OBJECT_DECL (obj_aecc_network_rule, AECC_NETWORK_RULE);
//     dwg_get_OBJECT_DECL (obj_aecc_parcel_node, AECC_PARCEL_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_parcel_style, AECC_PARCEL_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_part_size_filter, AECC_PART_SIZE_FILTER);
//     dwg_get_OBJECT_DECL (obj_aecc_pipe_rules, AECC_PIPE_RULES);
//     dwg_get_OBJECT_DECL (obj_aecc_pipe_style, AECC_PIPE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_pipe_style_extension, AECC_PIPE_STYLE_EXTENSION);
//     dwg_get_OBJECT_DECL (obj_aecc_pointcloud_style, AECC_POINTCLOUD_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_pointview, AECC_POINTVIEW);
//     dwg_get_OBJECT_DECL (obj_aecc_point_style, AECC_POINT_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_pressure_part_list, AECC_PRESSURE_PART_LIST);
//     dwg_get_OBJECT_DECL (obj_aecc_pressure_pipe_style, AECC_PRESSURE_PIPE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profilesectionentity_style, AECC_PROFILESECTIONENTITY_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_design_check_set, AECC_PROFILE_DESIGN_CHECK_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_label_set, AECC_PROFILE_LABEL_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_style, AECC_PROFILE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_band_style_set, AECC_PROFILE_VIEW_BAND_STYLE_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_data_band_style, AECC_PROFILE_VIEW_DATA_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_horizontal_geometry_band_style, AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_pipe_network_band_style, AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_sectional_data_band_style, AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_style, AECC_PROFILE_VIEW_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_superelevation_diagram_band_style, AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_profile_view_vertical_geometry_band_style, AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_quantity_takeoff_criteria, AECC_QUANTITY_TAKEOFF_CRITERIA);
//     dwg_get_OBJECT_DECL (obj_aecc_roadwaylink_style, AECC_ROADWAYLINK_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_roadwaymarker_style, AECC_ROADWAYMARKER_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_roadwayshape_style, AECC_ROADWAYSHAPE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_roadway_style_set, AECC_ROADWAY_STYLE_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_root_settings_node, AECC_ROOT_SETTINGS_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_sample_line_group_style, AECC_SAMPLE_LINE_GROUP_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_sample_line_style, AECC_SAMPLE_LINE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_section_label_set, AECC_SECTION_LABEL_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_section_style, AECC_SECTION_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_section_view_band_style_set, AECC_SECTION_VIEW_BAND_STYLE_SET);
//     dwg_get_OBJECT_DECL (obj_aecc_section_view_data_band_style, AECC_SECTION_VIEW_DATA_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_section_view_road_surface_band_style, AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_section_view_style, AECC_SECTION_VIEW_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_settings_node, AECC_SETTINGS_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_sheet_style, AECC_SHEET_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_slope_pattern_style, AECC_SLOPE_PATTERN_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_station_format_style, AECC_STATION_FORMAT_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_structure_rules, AECC_STRUCTURE_RULES);
//     dwg_get_OBJECT_DECL (obj_aecc_stucture_style, AECC_STUCTURE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_superelevation_diagram_view_style, AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_surface_style, AECC_SURFACE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_svfigure_style, AECC_SVFIGURE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_svnetwork_style, AECC_SVNETWORK_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_table_style, AECC_TABLE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecc_tag_manager, AECC_TAG_MANAGER);
//     dwg_get_OBJECT_DECL (obj_aecc_tree_node, AECC_TREE_NODE);
//     dwg_get_OBJECT_DECL (obj_aecc_user_defined_attribute_classification, AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION);
//     dwg_get_OBJECT_DECL (obj_aecc_valignment_style_extension, AECC_VALIGNMENT_STYLE_EXTENSION);
//     dwg_get_OBJECT_DECL (obj_aecc_view_frame_style, AECC_VIEW_FRAME_STYLE);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_props_member, AECS_DISP_PROPS_MEMBER);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_props_member_logical, AECS_DISP_PROPS_MEMBER_LOGICAL);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_props_member_plan, AECS_DISP_PROPS_MEMBER_PLAN);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_props_member_plan_sketch, AECS_DISP_PROPS_MEMBER_PLAN_SKETCH);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_props_member_projected, AECS_DISP_PROPS_MEMBER_PROJECTED);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_elevation_design, AECS_DISP_REP_MEMBER_ELEVATION_DESIGN);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_elevation_detail, AECS_DISP_REP_MEMBER_ELEVATION_DETAIL);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_logical, AECS_DISP_REP_MEMBER_LOGICAL);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_model_design, AECS_DISP_REP_MEMBER_MODEL_DESIGN);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_model_detail, AECS_DISP_REP_MEMBER_MODEL_DETAIL);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_plan_design, AECS_DISP_REP_MEMBER_PLAN_DESIGN);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_plan_detail, AECS_DISP_REP_MEMBER_PLAN_DETAIL);
//     dwg_get_OBJECT_DECL (obj_aecs_disp_rep_member_plan_sketch, AECS_DISP_REP_MEMBER_PLAN_SKETCH);
//     dwg_get_OBJECT_DECL (obj_aecs_member_node_shape, AECS_MEMBER_NODE_SHAPE);
//     dwg_get_OBJECT_DECL (obj_aecs_member_style, AECS_MEMBER_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_2dsection_style, AEC_2DSECTION_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepbdgelevlineplan100, AEC_AECDBDISPREPBDGELEVLINEPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepbdgelevlineplan50, AEC_AECDBDISPREPBDGELEVLINEPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepbdgsectionlineplan100, AEC_AECDBDISPREPBDGSECTIONLINEPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepbdgsectionlineplan50, AEC_AECDBDISPREPBDGSECTIONLINEPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepceilinggridplan100, AEC_AECDBDISPREPCEILINGGRIDPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepceilinggridplan50, AEC_AECDBDISPREPCEILINGGRIDPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepcolumngridplan100, AEC_AECDBDISPREPCOLUMNGRIDPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepcolumngridplan50, AEC_AECDBDISPREPCOLUMNGRIDPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepcurtainwalllayoutplan100, AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepcurtainwalllayoutplan50, AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepcurtainwallunitplan100, AEC_AECDBDISPREPCURTAINWALLUNITPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepcurtainwallunitplan50, AEC_AECDBDISPREPCURTAINWALLUNITPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepmvblockrefplan100, AEC_AECDBDISPREPMVBLOCKREFPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepmvblockrefplan50, AEC_AECDBDISPREPMVBLOCKREFPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdispreproofplan100, AEC_AECDBDISPREPROOFPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdispreproofplan50, AEC_AECDBDISPREPROOFPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdispreproofslabplan100, AEC_AECDBDISPREPROOFSLABPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdispreproofslabplan50, AEC_AECDBDISPREPROOFSLABPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepslabplan100, AEC_AECDBDISPREPSLABPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepslabplan50, AEC_AECDBDISPREPSLABPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepspaceplan100, AEC_AECDBDISPREPSPACEPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepspaceplan50, AEC_AECDBDISPREPSPACEPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepwallplan100, AEC_AECDBDISPREPWALLPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepwallplan50, AEC_AECDBDISPREPWALLPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepwindowassemblyplan100, AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepwindowassemblyplan50, AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepzone100, AEC_AECDBDISPREPZONE100);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbdisprepzone50, AEC_AECDBDISPREPZONE50);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbzonedef, AEC_AECDBZONEDEF);
//     dwg_get_OBJECT_DECL (obj_aec_aecdbzonestyle, AEC_AECDBZONESTYLE);
//     dwg_get_OBJECT_DECL (obj_aec_anchor_openingbase_to_wall, AEC_ANCHOR_OPENINGBASE_TO_WALL);
//     dwg_get_OBJECT_DECL (obj_aec_classification_def, AEC_CLASSIFICATION_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_classification_system_def, AEC_CLASSIFICATION_SYSTEM_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_cleanup_group_def, AEC_CLEANUP_GROUP_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_curtain_wall_layout_style, AEC_CURTAIN_WALL_LAYOUT_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_curtain_wall_unit_style, AEC_CURTAIN_WALL_UNIT_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_cvsectionview, AEC_CVSECTIONVIEW);
//     dwg_get_OBJECT_DECL (obj_aec_db_disp_rep_dim_group_plan, AEC_DB_DISP_REP_DIM_GROUP_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_db_disp_rep_dim_group_plan100, AEC_DB_DISP_REP_DIM_GROUP_PLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_db_disp_rep_dim_group_plan50, AEC_DB_DISP_REP_DIM_GROUP_PLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_dim_style, AEC_DIM_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_displaytheme_style, AEC_DISPLAYTHEME_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepmasselemplan100, AEC_DISPREPAECDBDISPREPMASSELEMPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepmasselemplan50, AEC_DISPREPAECDBDISPREPMASSELEMPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepmassgroupplan100, AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepmassgroupplan50, AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepopeningplan100, AEC_DISPREPAECDBDISPREPOPENINGPLAN100);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepopeningplan50, AEC_DISPREPAECDBDISPREPOPENINGPLAN50);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepopeningplanreflected, AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED);
//     dwg_get_OBJECT_DECL (obj_aec_disprepaecdbdisprepopeningsillplan, AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN);
//     dwg_get_OBJECT_DECL (obj_aec_dispropsmasselemplancommon, AEC_DISPROPSMASSELEMPLANCOMMON);
//     dwg_get_OBJECT_DECL (obj_aec_dispropsmassgroupplancommon, AEC_DISPROPSMASSGROUPPLANCOMMON);
//     dwg_get_OBJECT_DECL (obj_aec_dispropsopeningplancommon, AEC_DISPROPSOPENINGPLANCOMMON);
//     dwg_get_OBJECT_DECL (obj_aec_dispropsopeningplancommonhatched, AEC_DISPROPSOPENINGPLANCOMMONHATCHED);
//     dwg_get_OBJECT_DECL (obj_aec_dispropsopeningsillplan, AEC_DISPROPSOPENINGSILLPLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_2d_section, AEC_DISP_PROPS_2D_SECTION);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_clip_volume, AEC_DISP_PROPS_CLIP_VOLUME);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_clip_volume_result, AEC_DISP_PROPS_CLIP_VOLUME_RESULT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_dim, AEC_DISP_PROPS_DIM);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_displaytheme, AEC_DISP_PROPS_DISPLAYTHEME);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_door, AEC_DISP_PROPS_DOOR);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_door_nominal, AEC_DISP_PROPS_DOOR_NOMINAL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_door_plan_100, AEC_DISP_PROPS_DOOR_PLAN_100);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_door_plan_50, AEC_DISP_PROPS_DOOR_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_door_threshold_plan, AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_door_threshold_symbol_plan, AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_editinplaceprofile_model, AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_ent, AEC_DISP_PROPS_ENT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_ent_ref, AEC_DISP_PROPS_ENT_REF);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_grid_assembly_model, AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_grid_assembly_plan, AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_layout_curve, AEC_DISP_PROPS_LAYOUT_CURVE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_layout_grid2d, AEC_DISP_PROPS_LAYOUT_GRID2D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_layout_grid3d, AEC_DISP_PROPS_LAYOUT_GRID3D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_maskblock, AEC_DISP_PROPS_MASKBLOCK);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_mass_elem_model, AEC_DISP_PROPS_MASS_ELEM_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_mass_group, AEC_DISP_PROPS_MASS_GROUP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_material, AEC_DISP_PROPS_MATERIAL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_opening, AEC_DISP_PROPS_OPENING);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_polygon_model, AEC_DISP_PROPS_POLYGON_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_polygon_truecolour, AEC_DISP_PROPS_POLYGON_TRUECOLOUR);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_railing_model, AEC_DISP_PROPS_RAILING_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_railing_plan, AEC_DISP_PROPS_RAILING_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_roof, AEC_DISP_PROPS_ROOF);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_roofslab, AEC_DISP_PROPS_ROOFSLAB);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_roofslab_plan, AEC_DISP_PROPS_ROOFSLAB_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_schedule_table, AEC_DISP_PROPS_SCHEDULE_TABLE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_slab, AEC_DISP_PROPS_SLAB);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_slab_plan, AEC_DISP_PROPS_SLAB_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_slice, AEC_DISP_PROPS_SLICE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_space_decomposed, AEC_DISP_PROPS_SPACE_DECOMPOSED);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_space_model, AEC_DISP_PROPS_SPACE_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_space_plan, AEC_DISP_PROPS_SPACE_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_stair_model, AEC_DISP_PROPS_STAIR_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_stair_plan, AEC_DISP_PROPS_STAIR_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_stair_plan_overlapping, AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_wall_graph, AEC_DISP_PROPS_WALL_GRAPH);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_wall_model, AEC_DISP_PROPS_WALL_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_wall_plan, AEC_DISP_PROPS_WALL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_wall_schem, AEC_DISP_PROPS_WALL_SCHEM);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_window, AEC_DISP_PROPS_WINDOW);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_window_assembly_sill_plan, AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_window_nominal, AEC_DISP_PROPS_WINDOW_NOMINAL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_window_plan_100, AEC_DISP_PROPS_WINDOW_PLAN_100);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_window_plan_50, AEC_DISP_PROPS_WINDOW_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_window_sill_plan, AEC_DISP_PROPS_WINDOW_SILL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_props_zone, AEC_DISP_PROPS_ZONE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_2d_section, AEC_DISP_REP_2D_SECTION);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_anchor, AEC_DISP_REP_ANCHOR);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_anchor_bub_to_grid, AEC_DISP_REP_ANCHOR_BUB_TO_GRID);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_anchor_bub_to_grid_model, AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_anchor_bub_to_grid_rcp, AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_anchor_ent_to_node, AEC_DISP_REP_ANCHOR_ENT_TO_NODE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_anchor_ext_tag_to_ent, AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_anchor_tag_to_ent, AEC_DISP_REP_ANCHOR_TAG_TO_ENT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_elevline_model, AEC_DISP_REP_BDG_ELEVLINE_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_elevline_plan, AEC_DISP_REP_BDG_ELEVLINE_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_elevline_rcp, AEC_DISP_REP_BDG_ELEVLINE_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_sectionline_model, AEC_DISP_REP_BDG_SECTIONLINE_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_sectionline_plan, AEC_DISP_REP_BDG_SECTIONLINE_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_sectionline_rcp, AEC_DISP_REP_BDG_SECTIONLINE_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_section_model, AEC_DISP_REP_BDG_SECTION_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_bdg_section_subdiv, AEC_DISP_REP_BDG_SECTION_SUBDIV);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_ceiling_grid, AEC_DISP_REP_CEILING_GRID);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_ceiling_grid_model, AEC_DISP_REP_CEILING_GRID_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_ceiling_grid_rcp, AEC_DISP_REP_CEILING_GRID_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_clip_volume_model, AEC_DISP_REP_CLIP_VOLUME_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_clip_volume_plan, AEC_DISP_REP_CLIP_VOLUME_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_clip_volume_result, AEC_DISP_REP_CLIP_VOLUME_RESULT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_clip_volume_result_subdiv, AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_column_grid, AEC_DISP_REP_COLUMN_GRID);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_column_grid_model, AEC_DISP_REP_COLUMN_GRID_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_column_grid_rcp, AEC_DISP_REP_COLUMN_GRID_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_block, AEC_DISP_REP_COL_BLOCK);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_circarc2d, AEC_DISP_REP_COL_CIRCARC2D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_concoincident, AEC_DISP_REP_COL_CONCOINCIDENT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_conconcentric, AEC_DISP_REP_COL_CONCONCENTRIC);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_conequaldistance, AEC_DISP_REP_COL_CONEQUALDISTANCE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_conmidpoint, AEC_DISP_REP_COL_CONMIDPOINT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_connector, AEC_DISP_REP_COL_CONNECTOR);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_connormal, AEC_DISP_REP_COL_CONNORMAL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_conparallel, AEC_DISP_REP_COL_CONPARALLEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_conperpendicular, AEC_DISP_REP_COL_CONPERPENDICULAR);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_consymmetric, AEC_DISP_REP_COL_CONSYMMETRIC);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_contangent, AEC_DISP_REP_COL_CONTANGENT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_dimangle, AEC_DISP_REP_COL_DIMANGLE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_dimdiameter, AEC_DISP_REP_COL_DIMDIAMETER);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_dimdistance, AEC_DISP_REP_COL_DIMDISTANCE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_dimlength, AEC_DISP_REP_COL_DIMLENGTH);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_dimmajorradius, AEC_DISP_REP_COL_DIMMAJORRADIUS);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_dimminorradius, AEC_DISP_REP_COL_DIMMINORRADIUS);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_elliparc2d, AEC_DISP_REP_COL_ELLIPARC2D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_layoutdata, AEC_DISP_REP_COL_LAYOUTDATA);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_line2d, AEC_DISP_REP_COL_LINE2D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_add, AEC_DISP_REP_COL_MODIFIER_ADD);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_cutplane, AEC_DISP_REP_COL_MODIFIER_CUTPLANE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_extrusion, AEC_DISP_REP_COL_MODIFIER_EXTRUSION);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_group, AEC_DISP_REP_COL_MODIFIER_GROUP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_loft, AEC_DISP_REP_COL_MODIFIER_LOFT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_path, AEC_DISP_REP_COL_MODIFIER_PATH);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_revolve, AEC_DISP_REP_COL_MODIFIER_REVOLVE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_subtract, AEC_DISP_REP_COL_MODIFIER_SUBTRACT);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_modifier_transition, AEC_DISP_REP_COL_MODIFIER_TRANSITION);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_point2d, AEC_DISP_REP_COL_POINT2D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_profile, AEC_DISP_REP_COL_PROFILE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_workplane, AEC_DISP_REP_COL_WORKPLANE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_col_workplane_ref, AEC_DISP_REP_COL_WORKPLANE_REF);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_config, AEC_DISP_REP_CONFIG);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_curtain_wall_layout_model, AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_curtain_wall_layout_plan, AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_curtain_wall_unit_model, AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_curtain_wall_unit_plan, AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_dcm_dimradius, AEC_DISP_REP_DCM_DIMRADIUS);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_displaytheme, AEC_DISP_REP_DISPLAYTHEME);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_elev, AEC_DISP_REP_DOOR_ELEV);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_model, AEC_DISP_REP_DOOR_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_nominal, AEC_DISP_REP_DOOR_NOMINAL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_plan, AEC_DISP_REP_DOOR_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_plan_50, AEC_DISP_REP_DOOR_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_plan_hekto, AEC_DISP_REP_DOOR_PLAN_HEKTO);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_rcp, AEC_DISP_REP_DOOR_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_threshold_plan, AEC_DISP_REP_DOOR_THRESHOLD_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_door_threshold_symbol_plan, AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_editinplaceprofile, AEC_DISP_REP_EDITINPLACEPROFILE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_ent_ref, AEC_DISP_REP_ENT_REF);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_layout_curve, AEC_DISP_REP_LAYOUT_CURVE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_layout_grid2d, AEC_DISP_REP_LAYOUT_GRID2D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_layout_grid3d, AEC_DISP_REP_LAYOUT_GRID3D);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_maskblock_ref, AEC_DISP_REP_MASKBLOCK_REF);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_maskblock_ref_rcp, AEC_DISP_REP_MASKBLOCK_REF_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mass_elem_model, AEC_DISP_REP_MASS_ELEM_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mass_elem_rcp, AEC_DISP_REP_MASS_ELEM_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mass_elem_schem, AEC_DISP_REP_MASS_ELEM_SCHEM);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mass_group_model, AEC_DISP_REP_MASS_GROUP_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mass_group_plan, AEC_DISP_REP_MASS_GROUP_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mass_group_rcp, AEC_DISP_REP_MASS_GROUP_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_material, AEC_DISP_REP_MATERIAL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mvblock_ref, AEC_DISP_REP_MVBLOCK_REF);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mvblock_ref_model, AEC_DISP_REP_MVBLOCK_REF_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_mvblock_ref_rcp, AEC_DISP_REP_MVBLOCK_REF_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_opening, AEC_DISP_REP_OPENING);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_opening_model, AEC_DISP_REP_OPENING_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_polygon_model, AEC_DISP_REP_POLYGON_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_polygon_truecolour, AEC_DISP_REP_POLYGON_TRUECOLOUR);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_railing_model, AEC_DISP_REP_RAILING_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_railing_plan, AEC_DISP_REP_RAILING_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_railing_plan_100, AEC_DISP_REP_RAILING_PLAN_100);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_railing_plan_50, AEC_DISP_REP_RAILING_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_roofslab_model, AEC_DISP_REP_ROOFSLAB_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_roofslab_plan, AEC_DISP_REP_ROOFSLAB_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_roof_model, AEC_DISP_REP_ROOF_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_roof_plan, AEC_DISP_REP_ROOF_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_roof_rcp, AEC_DISP_REP_ROOF_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_schedule_table, AEC_DISP_REP_SCHEDULE_TABLE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_set, AEC_DISP_REP_SET);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_slab_model, AEC_DISP_REP_SLAB_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_slab_plan, AEC_DISP_REP_SLAB_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_slice, AEC_DISP_REP_SLICE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_space_decomposed, AEC_DISP_REP_SPACE_DECOMPOSED);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_space_model, AEC_DISP_REP_SPACE_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_space_plan, AEC_DISP_REP_SPACE_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_space_rcp, AEC_DISP_REP_SPACE_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_space_volume, AEC_DISP_REP_SPACE_VOLUME);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_stair_model, AEC_DISP_REP_STAIR_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_stair_plan, AEC_DISP_REP_STAIR_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_stair_plan_100, AEC_DISP_REP_STAIR_PLAN_100);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_stair_plan_50, AEC_DISP_REP_STAIR_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_stair_plan_overlapping, AEC_DISP_REP_STAIR_PLAN_OVERLAPPING);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_stair_rcp, AEC_DISP_REP_STAIR_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_wall_graph, AEC_DISP_REP_WALL_GRAPH);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_wall_model, AEC_DISP_REP_WALL_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_wall_plan, AEC_DISP_REP_WALL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_wall_rcp, AEC_DISP_REP_WALL_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_wall_schem, AEC_DISP_REP_WALL_SCHEM);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_windowassembly_sill_plan, AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_assembly_model, AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_assembly_plan, AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_elev, AEC_DISP_REP_WINDOW_ELEV);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_model, AEC_DISP_REP_WINDOW_MODEL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_nominal, AEC_DISP_REP_WINDOW_NOMINAL);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_plan, AEC_DISP_REP_WINDOW_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_plan_100, AEC_DISP_REP_WINDOW_PLAN_100);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_plan_50, AEC_DISP_REP_WINDOW_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_rcp, AEC_DISP_REP_WINDOW_RCP);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_window_sill_plan, AEC_DISP_REP_WINDOW_SILL_PLAN);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rep_zone, AEC_DISP_REP_ZONE);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rops_railing_plan_100, AEC_DISP_ROPS_RAILING_PLAN_100);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rops_railing_plan_50, AEC_DISP_ROPS_RAILING_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rops_stair_plan_100, AEC_DISP_ROPS_STAIR_PLAN_100);
//     dwg_get_OBJECT_DECL (obj_aec_disp_rops_stair_plan_50, AEC_DISP_ROPS_STAIR_PLAN_50);
//     dwg_get_OBJECT_DECL (obj_aec_door_style, AEC_DOOR_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_endcap_style, AEC_ENDCAP_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_frame_def, AEC_FRAME_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_layerkey_style, AEC_LAYERKEY_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_list_def, AEC_LIST_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_maskblock_def, AEC_MASKBLOCK_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_mass_elem_style, AEC_MASS_ELEM_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_material_def, AEC_MATERIAL_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_mvblock_def, AEC_MVBLOCK_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_mvblock_ref, AEC_MVBLOCK_REF);
//     dwg_get_OBJECT_DECL (obj_aec_notification_tracker, AEC_NOTIFICATION_TRACKER);
//     dwg_get_OBJECT_DECL (obj_aec_polygon, AEC_POLYGON);
//     dwg_get_OBJECT_DECL (obj_aec_polygon_style, AEC_POLYGON_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_property_set_def, AEC_PROPERTY_SET_DEF);
//     dwg_get_OBJECT_DECL (obj_aec_railing_style, AEC_RAILING_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_refedit_status_tracker, AEC_REFEDIT_STATUS_TRACKER);
//     dwg_get_OBJECT_DECL (obj_aec_roofslabedge_style, AEC_ROOFSLABEDGE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_roofslab_style, AEC_ROOFSLAB_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_schedule_data_format, AEC_SCHEDULE_DATA_FORMAT);
//     dwg_get_OBJECT_DECL (obj_aec_slabedge_style, AEC_SLABEDGE_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_slab_style, AEC_SLAB_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_space_styles, AEC_SPACE_STYLES);
//     dwg_get_OBJECT_DECL (obj_aec_stair_style, AEC_STAIR_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_stair_winder_style, AEC_STAIR_WINDER_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_stair_winder_type_balanced, AEC_STAIR_WINDER_TYPE_BALANCED);
//     dwg_get_OBJECT_DECL (obj_aec_stair_winder_type_manual, AEC_STAIR_WINDER_TYPE_MANUAL);
//     dwg_get_OBJECT_DECL (obj_aec_stair_winder_type_single_point, AEC_STAIR_WINDER_TYPE_SINGLE_POINT);
//     dwg_get_OBJECT_DECL (obj_aec_vars_aecbbldsrv, AEC_VARS_AECBBLDSRV);
//     dwg_get_OBJECT_DECL (obj_aec_vars_archbase, AEC_VARS_ARCHBASE);
//     dwg_get_OBJECT_DECL (obj_aec_vars_dwg_setup, AEC_VARS_DWG_SETUP);
//     dwg_get_OBJECT_DECL (obj_aec_vars_munich, AEC_VARS_MUNICH);
//     dwg_get_OBJECT_DECL (obj_aec_vars_structurebase, AEC_VARS_STRUCTUREBASE);
//     dwg_get_OBJECT_DECL (obj_aec_wallmod_style, AEC_WALLMOD_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_wall_style, AEC_WALL_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_window_assembly_style, AEC_WINDOW_ASSEMBLY_STYLE);
//     dwg_get_OBJECT_DECL (obj_aec_window_style, AEC_WINDOW_STYLE);
//     dwg_get_OBJECT_DECL (obj_alignmentgripentity, ALIGNMENTGRIPENTITY);
//     dwg_get_OBJECT_DECL (obj_amcontextmgr, AMCONTEXTMGR);
//     dwg_get_OBJECT_DECL (obj_amdtadmenustate, AMDTADMENUSTATE);
//     dwg_get_OBJECT_DECL (obj_amdtammenustate, AMDTAMMENUSTATE);
//     dwg_get_OBJECT_DECL (obj_amdtbrowserdbtab, AMDTBROWSERDBTAB);
//     dwg_get_OBJECT_DECL (obj_amdtdmmenustate, AMDTDMMENUSTATE);
//     dwg_get_OBJECT_DECL (obj_amdtedgestandarddin, AMDTEDGESTANDARDDIN);
//     dwg_get_OBJECT_DECL (obj_amdtedgestandarddin13715, AMDTEDGESTANDARDDIN13715);
//     dwg_get_OBJECT_DECL (obj_amdtedgestandardiso, AMDTEDGESTANDARDISO);
//     dwg_get_OBJECT_DECL (obj_amdtedgestandardiso13715, AMDTEDGESTANDARDISO13715);
//     dwg_get_OBJECT_DECL (obj_amdtformulaupdatedispatcher, AMDTFORMULAUPDATEDISPATCHER);
//     dwg_get_OBJECT_DECL (obj_amdtinternalreactor, AMDTINTERNALREACTOR);
//     dwg_get_OBJECT_DECL (obj_amdtmcommenustate, AMDTMCOMMENUSTATE);
//     dwg_get_OBJECT_DECL (obj_amdtmenustatemgr, AMDTMENUSTATEMGR);
//     dwg_get_OBJECT_DECL (obj_amdtnote, AMDTNOTE);
//     dwg_get_OBJECT_DECL (obj_amdtnotetemplatedb, AMDTNOTETEMPLATEDB);
//     dwg_get_OBJECT_DECL (obj_amdtsectionsym, AMDTSECTIONSYM);
//     dwg_get_OBJECT_DECL (obj_amdtsectionsymlabel, AMDTSECTIONSYMLABEL);
//     dwg_get_OBJECT_DECL (obj_amdtsysattr, AMDTSYSATTR);
//     dwg_get_OBJECT_DECL (obj_amgobjpropcfg, AMGOBJPROPCFG);
//     dwg_get_OBJECT_DECL (obj_amgsettingsobj, AMGSETTINGSOBJ);
//     dwg_get_OBJECT_DECL (obj_amimaster, AMIMASTER);
//     dwg_get_OBJECT_DECL (obj_am_drawing_mgr, AM_DRAWING_MGR);
//     dwg_get_OBJECT_DECL (obj_am_dwgmgr_name, AM_DWGMGR_NAME);
//     dwg_get_OBJECT_DECL (obj_am_dwg_document, AM_DWG_DOCUMENT);
//     dwg_get_OBJECT_DECL (obj_am_dwg_sheet, AM_DWG_SHEET);
//     dwg_get_OBJECT_DECL (obj_am_viewdimparmap, AM_VIEWDIMPARMAP);
//     dwg_get_OBJECT_DECL (obj_binrecord, BINRECORD);
//     dwg_get_OBJECT_DECL (obj_camscatalogappobject, CAMSCATALOGAPPOBJECT);
//     dwg_get_OBJECT_DECL (obj_camsstructbtnstate, CAMSSTRUCTBTNSTATE);
//     dwg_get_OBJECT_DECL (obj_catalogstate, CATALOGSTATE);
//     dwg_get_OBJECT_DECL (obj_cbrowserappobject, CBROWSERAPPOBJECT);
//     dwg_get_OBJECT_DECL (obj_depmgr, DEPMGR);
//     dwg_get_OBJECT_DECL (obj_dmbaseelement, DMBASEELEMENT);
//     dwg_get_OBJECT_DECL (obj_dmdefaultstyle, DMDEFAULTSTYLE);
//     dwg_get_OBJECT_DECL (obj_dmlegend, DMLEGEND);
//     dwg_get_OBJECT_DECL (obj_dmmap, DMMAP);
//     dwg_get_OBJECT_DECL (obj_dmmapmanager, DMMAPMANAGER);
//     dwg_get_OBJECT_DECL (obj_dmstylecategory, DMSTYLECATEGORY);
//     dwg_get_OBJECT_DECL (obj_dmstylelibrary, DMSTYLELIBRARY);
//     dwg_get_OBJECT_DECL (obj_dmstylereference, DMSTYLEREFERENCE);
//     dwg_get_OBJECT_DECL (obj_dmstylizedentitiestable, DMSTYLIZEDENTITIESTABLE);
//     dwg_get_OBJECT_DECL (obj_dmsurrogatestylesets, DMSURROGATESTYLESETS);
//     dwg_get_OBJECT_DECL (obj_dm_placeholder, DM_PLACEHOLDER);
//     dwg_get_OBJECT_DECL (obj_exactermxrefmap, EXACTERMXREFMAP);
//     dwg_get_OBJECT_DECL (obj_exacxrefpanelobject, EXACXREFPANELOBJECT);
//     dwg_get_OBJECT_DECL (obj_expo_notifyblock, EXPO_NOTIFYBLOCK);
//     dwg_get_OBJECT_DECL (obj_expo_notifyhall, EXPO_NOTIFYHALL);
//     dwg_get_OBJECT_DECL (obj_expo_notifypillar, EXPO_NOTIFYPILLAR);
//     dwg_get_OBJECT_DECL (obj_expo_notifystand, EXPO_NOTIFYSTAND);
//     dwg_get_OBJECT_DECL (obj_expo_notifystandnopoly, EXPO_NOTIFYSTANDNOPOLY);
//     dwg_get_OBJECT_DECL (obj_flipactionentity, FLIPACTIONENTITY);
//     dwg_get_OBJECT_DECL (obj_gsmanager, GSMANAGER);
//     dwg_get_OBJECT_DECL (obj_ird_dsc_dict, IRD_DSC_DICT);
//     dwg_get_OBJECT_DECL (obj_ird_dsc_record, IRD_DSC_RECORD);
//     dwg_get_OBJECT_DECL (obj_ird_obj_record, IRD_OBJ_RECORD);
//     dwg_get_OBJECT_DECL (obj_mapfsmrvobject, MAPFSMRVOBJECT);
//     dwg_get_OBJECT_DECL (obj_mapgwsundoobject, MAPGWSUNDOOBJECT);
//     dwg_get_OBJECT_DECL (obj_mapiammoudle, MAPIAMMOUDLE);
//     dwg_get_OBJECT_DECL (obj_mapmetadataobject, MAPMETADATAOBJECT);
//     dwg_get_OBJECT_DECL (obj_mapresourcemanagerobject, MAPRESOURCEMANAGEROBJECT);
//     dwg_get_OBJECT_DECL (obj_moveactionentity, MOVEACTIONENTITY);
//     dwg_get_OBJECT_DECL (obj_mcdbcontainer2, McDbContainer2);
//     dwg_get_OBJECT_DECL (obj_mcdbmarker, McDbMarker);
//     dwg_get_OBJECT_DECL (obj_namedappl, NAMEDAPPL);
//     dwg_get_OBJECT_DECL (obj_newstdpartparlist, NEWSTDPARTPARLIST);
//     dwg_get_OBJECT_DECL (obj_npocollection, NPOCOLLECTION);
//     dwg_get_OBJECT_DECL (obj_objcloner, OBJCLONER);
//     dwg_get_OBJECT_DECL (obj_parammgr, PARAMMGR);
//     dwg_get_OBJECT_DECL (obj_paramscope, PARAMSCOPE);
//     dwg_get_OBJECT_DECL (obj_pillar, PILLAR);
//     dwg_get_OBJECT_DECL (obj_rapidrtrenderenvironment, RAPIDRTRENDERENVIRONMENT);
//     dwg_get_OBJECT_DECL (obj_rotateactionentity, ROTATEACTIONENTITY);
//     dwg_get_OBJECT_DECL (obj_scaleactionentity, SCALEACTIONENTITY);
//     dwg_get_OBJECT_DECL (obj_stdpart2d, STDPART2D);
//     dwg_get_OBJECT_DECL (obj_stretchactionentity, STRETCHACTIONENTITY);
//     dwg_get_OBJECT_DECL (obj_tch_arrow, TCH_ARROW);
//     dwg_get_OBJECT_DECL (obj_tch_axis_label, TCH_AXIS_LABEL);
//     dwg_get_OBJECT_DECL (obj_tch_block_insert, TCH_BLOCK_INSERT);
//     dwg_get_OBJECT_DECL (obj_tch_column, TCH_COLUMN);
//     dwg_get_OBJECT_DECL (obj_tch_dbconfig, TCH_DBCONFIG);
//     dwg_get_OBJECT_DECL (obj_tch_dimension2, TCH_DIMENSION2);
//     dwg_get_OBJECT_DECL (obj_tch_drawingindex, TCH_DRAWINGINDEX);
//     dwg_get_OBJECT_DECL (obj_tch_handrail, TCH_HANDRAIL);
//     dwg_get_OBJECT_DECL (obj_tch_linestair, TCH_LINESTAIR);
//     dwg_get_OBJECT_DECL (obj_tch_opening, TCH_OPENING);
//     dwg_get_OBJECT_DECL (obj_tch_rectstair, TCH_RECTSTAIR);
//     dwg_get_OBJECT_DECL (obj_tch_slab, TCH_SLAB);
//     dwg_get_OBJECT_DECL (obj_tch_space, TCH_SPACE);
//     dwg_get_OBJECT_DECL (obj_tch_text, TCH_TEXT);
//     dwg_get_OBJECT_DECL (obj_tch_wall, TCH_WALL);
//     dwg_get_OBJECT_DECL (obj_tgrupopuntos, TGrupoPuntos);
//     dwg_get_OBJECT_DECL (obj_vaacimageinventory, VAACIMAGEINVENTORY);
//     dwg_get_OBJECT_DECL (obj_vaacxrefpanelobject, VAACXREFPANELOBJECT);
//     dwg_get_OBJECT_DECL (obj_xrefpanelobject, XREFPANELOBJECT);
#endif

/********************************************************************
 * Functions to return NULL-terminated array of all owned entities  *
 ********************************************************************/

/// extract all owned entities from a block header (mspace or pspace)
  DWG_GETALL_ENTITY_DECL (_3DFACE);
  DWG_GETALL_ENTITY_DECL (_3DSOLID);
  DWG_GETALL_ENTITY_DECL (ARC);
  DWG_GETALL_ENTITY_DECL (ATTDEF);
  DWG_GETALL_ENTITY_DECL (ATTRIB);
  DWG_GETALL_ENTITY_DECL (BLOCK);
  DWG_GETALL_ENTITY_DECL (BODY);
  DWG_GETALL_ENTITY_DECL (CIRCLE);
  DWG_GETALL_ENTITY_DECL (DIMENSION_ALIGNED);
  DWG_GETALL_ENTITY_DECL (DIMENSION_ANG2LN);
  DWG_GETALL_ENTITY_DECL (DIMENSION_ANG3PT);
  DWG_GETALL_ENTITY_DECL (DIMENSION_DIAMETER);
  DWG_GETALL_ENTITY_DECL (DIMENSION_LINEAR);
  DWG_GETALL_ENTITY_DECL (DIMENSION_ORDINATE);
  DWG_GETALL_ENTITY_DECL (DIMENSION_RADIUS);
  DWG_GETALL_ENTITY_DECL (ELLIPSE);
  DWG_GETALL_ENTITY_DECL (ENDBLK);
  DWG_GETALL_ENTITY_DECL (INSERT);
  DWG_GETALL_ENTITY_DECL (LEADER);
  DWG_GETALL_ENTITY_DECL (LINE);
  DWG_GETALL_ENTITY_DECL (LOAD);
  DWG_GETALL_ENTITY_DECL (MINSERT);
  DWG_GETALL_ENTITY_DECL (MLINE);
  DWG_GETALL_ENTITY_DECL (MTEXT);
  DWG_GETALL_ENTITY_DECL (OLEFRAME);
  DWG_GETALL_ENTITY_DECL (POINT);
  DWG_GETALL_ENTITY_DECL (POLYLINE);
  DWG_GETALL_ENTITY_DECL (POLYLINE_2D);
  DWG_GETALL_ENTITY_DECL (POLYLINE_3D);
  DWG_GETALL_ENTITY_DECL (POLYLINE_MESH);
  DWG_GETALL_ENTITY_DECL (POLYLINE_PFACE);
  DWG_GETALL_ENTITY_DECL (PROXY_ENTITY);
  DWG_GETALL_ENTITY_DECL (RAY);
  DWG_GETALL_ENTITY_DECL (REGION);
  DWG_GETALL_ENTITY_DECL (SEQEND);
  DWG_GETALL_ENTITY_DECL (SHAPE);
  DWG_GETALL_ENTITY_DECL (SOLID);
  DWG_GETALL_ENTITY_DECL (SPLINE);
  DWG_GETALL_ENTITY_DECL (TEXT);
  DWG_GETALL_ENTITY_DECL (TOLERANCE);
  DWG_GETALL_ENTITY_DECL (TRACE);
  DWG_GETALL_ENTITY_DECL (UNKNOWN_ENT);
  DWG_GETALL_ENTITY_DECL (VERTEX);
  DWG_GETALL_ENTITY_DECL (VERTEX_2D);
  DWG_GETALL_ENTITY_DECL (VERTEX_3D);
  DWG_GETALL_ENTITY_DECL (VERTEX_MESH);
  DWG_GETALL_ENTITY_DECL (VERTEX_PFACE);
  DWG_GETALL_ENTITY_DECL (VERTEX_PFACE_FACE);
  DWG_GETALL_ENTITY_DECL (VIEWPORT);
  DWG_GETALL_ENTITY_DECL (XLINE);
  /* untyped > 500 */
  DWG_GETALL_ENTITY_DECL (_3DLINE);
  DWG_GETALL_ENTITY_DECL (CAMERA);
  DWG_GETALL_ENTITY_DECL (DGNUNDERLAY);
  DWG_GETALL_ENTITY_DECL (DWFUNDERLAY);
  DWG_GETALL_ENTITY_DECL (ENDREP);
  DWG_GETALL_ENTITY_DECL (HATCH);
  DWG_GETALL_ENTITY_DECL (IMAGE);
  DWG_GETALL_ENTITY_DECL (JUMP);
  DWG_GETALL_ENTITY_DECL (LIGHT);
  DWG_GETALL_ENTITY_DECL (LWPOLYLINE);
  DWG_GETALL_ENTITY_DECL (MESH);
  DWG_GETALL_ENTITY_DECL (MULTILEADER);
  DWG_GETALL_ENTITY_DECL (OLE2FRAME);
  DWG_GETALL_ENTITY_DECL (PDFUNDERLAY);
  DWG_GETALL_ENTITY_DECL (REPEAT);
  DWG_GETALL_ENTITY_DECL (SECTIONOBJECT);
  DWG_GETALL_ENTITY_DECL (WIPEOUT);
  /* unstable */
  DWG_GETALL_ENTITY_DECL (ARC_DIMENSION);
  DWG_GETALL_ENTITY_DECL (HELIX);
  DWG_GETALL_ENTITY_DECL (LARGE_RADIAL_DIMENSION);
  DWG_GETALL_ENTITY_DECL (LAYOUTPRINTCONFIG);
  DWG_GETALL_ENTITY_DECL (PLANESURFACE);
  DWG_GETALL_ENTITY_DECL (POINTCLOUD);
  DWG_GETALL_ENTITY_DECL (POINTCLOUDEX);
  /* debugging */
  DWG_GETALL_ENTITY_DECL (ALIGNMENTPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (ARCALIGNEDTEXT);
  DWG_GETALL_ENTITY_DECL (BASEPOINTPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (EXTRUDEDSURFACE);
  DWG_GETALL_ENTITY_DECL (FLIPGRIPENTITY);
  DWG_GETALL_ENTITY_DECL (FLIPPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (GEOPOSITIONMARKER);
  DWG_GETALL_ENTITY_DECL (LINEARGRIPENTITY);
  DWG_GETALL_ENTITY_DECL (LINEARPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (LOFTEDSURFACE);
  DWG_GETALL_ENTITY_DECL (MPOLYGON);
  DWG_GETALL_ENTITY_DECL (NAVISWORKSMODEL);
  DWG_GETALL_ENTITY_DECL (NURBSURFACE);
  DWG_GETALL_ENTITY_DECL (POINTPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (POLARGRIPENTITY);
  DWG_GETALL_ENTITY_DECL (REVOLVEDSURFACE);
  DWG_GETALL_ENTITY_DECL (ROTATIONGRIPENTITY);
  DWG_GETALL_ENTITY_DECL (ROTATIONPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (RTEXT);
  DWG_GETALL_ENTITY_DECL (SWEPTSURFACE);
  DWG_GETALL_ENTITY_DECL (TABLE);
  DWG_GETALL_ENTITY_DECL (VISIBILITYGRIPENTITY);
  DWG_GETALL_ENTITY_DECL (VISIBILITYPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (XYGRIPENTITY);
  DWG_GETALL_ENTITY_DECL (XYPARAMETERENTITY);

/********************************************************************
 *     Functions to return NULL-terminated array of all objects     *
 ********************************************************************/

/**
 * \fn Dwg_Object_OBJECT dwg_getall_OBJECT(Dwg_Data *dwg)
 * Extracts all objects of this type from a dwg, and returns a malloced
 * NULL-terminated array.
 */

  DWG_GETALL_OBJECT_DECL (APPID);
  DWG_GETALL_OBJECT_DECL (APPID_CONTROL);
  DWG_GETALL_OBJECT_DECL (BLOCK_CONTROL);
  DWG_GETALL_OBJECT_DECL (BLOCK_HEADER);
  DWG_GETALL_OBJECT_DECL (DICTIONARY);
  DWG_GETALL_OBJECT_DECL (DIMSTYLE);
  DWG_GETALL_OBJECT_DECL (DIMSTYLE_CONTROL);
  DWG_GETALL_OBJECT_DECL (DUMMY);
  DWG_GETALL_OBJECT_DECL (LAYER);
  DWG_GETALL_OBJECT_DECL (LAYER_CONTROL);
  DWG_GETALL_OBJECT_DECL (LONG_TRANSACTION);
  DWG_GETALL_OBJECT_DECL (LTYPE);
  DWG_GETALL_OBJECT_DECL (LTYPE_CONTROL);
  DWG_GETALL_OBJECT_DECL (MLINESTYLE);
  DWG_GETALL_OBJECT_DECL (STYLE);
  DWG_GETALL_OBJECT_DECL (STYLE_CONTROL);
  DWG_GETALL_OBJECT_DECL (UCS);
  DWG_GETALL_OBJECT_DECL (UCS_CONTROL);
  DWG_GETALL_OBJECT_DECL (UNKNOWN_OBJ);
  DWG_GETALL_OBJECT_DECL (VIEW);
  DWG_GETALL_OBJECT_DECL (VIEW_CONTROL);
  DWG_GETALL_OBJECT_DECL (VPORT);
  DWG_GETALL_OBJECT_DECL (VPORT_CONTROL);
  DWG_GETALL_OBJECT_DECL (VX_CONTROL);
  DWG_GETALL_OBJECT_DECL (VX_TABLE_RECORD);
  /* untyped > 500 */
  DWG_GETALL_OBJECT_DECL (ACSH_BOOLEAN_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_BOX_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_CONE_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_CYLINDER_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_FILLET_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_HISTORY_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_SPHERE_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_TORUS_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_WEDGE_CLASS);
  DWG_GETALL_OBJECT_DECL (ASSOCGEOMDEPENDENCY);
  DWG_GETALL_OBJECT_DECL (ASSOCNETWORK);
  DWG_GETALL_OBJECT_DECL (BLOCKALIGNMENTGRIP);
  DWG_GETALL_OBJECT_DECL (BLOCKALIGNMENTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKBASEPOINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKFLIPACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKFLIPGRIP);
  DWG_GETALL_OBJECT_DECL (BLOCKFLIPPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKGRIPLOCATIONCOMPONENT);
  DWG_GETALL_OBJECT_DECL (BLOCKLINEARGRIP);
  DWG_GETALL_OBJECT_DECL (BLOCKLOOKUPGRIP);
  DWG_GETALL_OBJECT_DECL (BLOCKMOVEACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKROTATEACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKROTATIONGRIP);
  DWG_GETALL_OBJECT_DECL (BLOCKSCALEACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKVISIBILITYGRIP);
  DWG_GETALL_OBJECT_DECL (CELLSTYLEMAP);
  DWG_GETALL_OBJECT_DECL (DETAILVIEWSTYLE);
  DWG_GETALL_OBJECT_DECL (DICTIONARYVAR);
  DWG_GETALL_OBJECT_DECL (DICTIONARYWDFLT);
  DWG_GETALL_OBJECT_DECL (DYNAMICBLOCKPURGEPREVENTER);
  DWG_GETALL_OBJECT_DECL (FIELD);
  DWG_GETALL_OBJECT_DECL (FIELDLIST);
  DWG_GETALL_OBJECT_DECL (GEODATA);
  DWG_GETALL_OBJECT_DECL (GROUP);
  DWG_GETALL_OBJECT_DECL (IDBUFFER);
  DWG_GETALL_OBJECT_DECL (IMAGEDEF);
  DWG_GETALL_OBJECT_DECL (IMAGEDEF_REACTOR);
  DWG_GETALL_OBJECT_DECL (INDEX);
  DWG_GETALL_OBJECT_DECL (LAYERFILTER);
  DWG_GETALL_OBJECT_DECL (LAYER_INDEX);
  DWG_GETALL_OBJECT_DECL (LAYOUT);
  DWG_GETALL_OBJECT_DECL (PLACEHOLDER);
  DWG_GETALL_OBJECT_DECL (PLOTSETTINGS);
  DWG_GETALL_OBJECT_DECL (RASTERVARIABLES);
  DWG_GETALL_OBJECT_DECL (RENDERENVIRONMENT);
  DWG_GETALL_OBJECT_DECL (SCALE);
  DWG_GETALL_OBJECT_DECL (SECTIONVIEWSTYLE);
  DWG_GETALL_OBJECT_DECL (SECTION_MANAGER);
  DWG_GETALL_OBJECT_DECL (SORTENTSTABLE);
  DWG_GETALL_OBJECT_DECL (SPATIAL_FILTER);
  DWG_GETALL_OBJECT_DECL (SUN);
  DWG_GETALL_OBJECT_DECL (TABLEGEOMETRY);
  DWG_GETALL_OBJECT_DECL (VBA_PROJECT);
  DWG_GETALL_OBJECT_DECL (VISUALSTYLE);
  DWG_GETALL_OBJECT_DECL (WIPEOUTVARIABLES);
  DWG_GETALL_OBJECT_DECL (XRECORD);
  DWG_GETALL_OBJECT_DECL (PDFDEFINITION);
  DWG_GETALL_OBJECT_DECL (DGNDEFINITION);
  DWG_GETALL_OBJECT_DECL (DWFDEFINITION);
  /* unstable */
  DWG_GETALL_OBJECT_DECL (ACSH_BREP_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_CHAMFER_CLASS);
  DWG_GETALL_OBJECT_DECL (ACSH_PYRAMID_CLASS);
  DWG_GETALL_OBJECT_DECL (ALDIMOBJECTCONTEXTDATA);
  DWG_GETALL_OBJECT_DECL (ASSOC2DCONSTRAINTGROUP);
  DWG_GETALL_OBJECT_DECL (ASSOCACTION);
  DWG_GETALL_OBJECT_DECL (ASSOCACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCASMBODYACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCBLENDSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCCOMPOUNDACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCDEPENDENCY);
  DWG_GETALL_OBJECT_DECL (ASSOCDIMDEPENDENCYBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCEXTENDSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCEXTRUDEDSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCFACEACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCFILLETSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCLOFTEDSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCNETWORKSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCOBJECTACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCOFFSETSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCOSNAPPOINTREFACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCPATCHSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCPATHACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCPLANESURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCPOINTREFACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (ASSOCREVOLVEDSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCTRIMSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCVALUEDEPENDENCY);
  DWG_GETALL_OBJECT_DECL (ASSOCVARIABLE);
  DWG_GETALL_OBJECT_DECL (ASSOCVERTEXACTIONPARAM);
  DWG_GETALL_OBJECT_DECL (BLKREFOBJECTCONTEXTDATA);
  DWG_GETALL_OBJECT_DECL (BLOCKALIGNEDCONSTRAINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKANGULARCONSTRAINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKARRAYACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKDIAMETRICCONSTRAINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKHORIZONTALCONSTRAINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKLINEARCONSTRAINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKLINEARPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKLOOKUPACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKLOOKUPPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKPARAMDEPENDENCYBODY);
  DWG_GETALL_OBJECT_DECL (BLOCKPOINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKPOLARGRIP);
  DWG_GETALL_OBJECT_DECL (BLOCKPOLARPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKPOLARSTRETCHACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKRADIALCONSTRAINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKREPRESENTATION);
  DWG_GETALL_OBJECT_DECL (BLOCKROTATIONPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKSTRETCHACTION);
  DWG_GETALL_OBJECT_DECL (BLOCKUSERPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKVERTICALCONSTRAINTPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKVISIBILITYPARAMETER);
  DWG_GETALL_OBJECT_DECL (BLOCKXYGRIP);
  DWG_GETALL_OBJECT_DECL (BLOCKXYPARAMETER);
  DWG_GETALL_OBJECT_DECL (DATALINK);
  DWG_GETALL_OBJECT_DECL (DBCOLOR);
  DWG_GETALL_OBJECT_DECL (EVALUATION_GRAPH);
  DWG_GETALL_OBJECT_DECL (FCFOBJECTCONTEXTDATA);
  DWG_GETALL_OBJECT_DECL (GRADIENT_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (GROUND_PLANE_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (IBL_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (IMAGE_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (LEADEROBJECTCONTEXTDATA);
  DWG_GETALL_OBJECT_DECL (LIGHTLIST);
  DWG_GETALL_OBJECT_DECL (MATERIAL);
  DWG_GETALL_OBJECT_DECL (MENTALRAYRENDERSETTINGS);
  DWG_GETALL_OBJECT_DECL (MLEADERSTYLE);
  DWG_GETALL_OBJECT_DECL (MTEXTOBJECTCONTEXTDATA);
  DWG_GETALL_OBJECT_DECL (OBJECT_PTR);
  DWG_GETALL_OBJECT_DECL (PARTIAL_VIEWING_INDEX);
  DWG_GETALL_OBJECT_DECL (POINTCLOUDCOLORMAP);
  DWG_GETALL_OBJECT_DECL (POINTCLOUDDEF);
  DWG_GETALL_OBJECT_DECL (POINTCLOUDDEFEX);
  DWG_GETALL_OBJECT_DECL (POINTCLOUDDEF_REACTOR);
  DWG_GETALL_OBJECT_DECL (POINTCLOUDDEF_REACTOR_EX);
  DWG_GETALL_OBJECT_DECL (PROXY_OBJECT);
  DWG_GETALL_OBJECT_DECL (RAPIDRTRENDERSETTINGS);
  DWG_GETALL_OBJECT_DECL (RENDERENTRY);
  DWG_GETALL_OBJECT_DECL (RENDERGLOBAL);
  DWG_GETALL_OBJECT_DECL (RENDERSETTINGS);
  DWG_GETALL_OBJECT_DECL (SECTION_SETTINGS);
  DWG_GETALL_OBJECT_DECL (SKYLIGHT_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (SOLID_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (SPATIAL_INDEX);
  DWG_GETALL_OBJECT_DECL (TABLESTYLE);
  DWG_GETALL_OBJECT_DECL (TEXTOBJECTCONTEXTDATA);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYMODIFYPARAMETERS);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYPATHPARAMETERS);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYPOLARPARAMETERS);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYRECTANGULARPARAMETERS);
#  ifdef DEBUG_CLASSES
    DWG_GETALL_OBJECT_DECL (ACMECOMMANDHISTORY);
    DWG_GETALL_OBJECT_DECL (ACMESCOPE);
    DWG_GETALL_OBJECT_DECL (ACMESTATEMGR);
    DWG_GETALL_OBJECT_DECL (ACSH_EXTRUSION_CLASS);
    DWG_GETALL_OBJECT_DECL (ACSH_LOFT_CLASS);
    DWG_GETALL_OBJECT_DECL (ACSH_REVOLVE_CLASS);
    DWG_GETALL_OBJECT_DECL (ACSH_SWEEP_CLASS);
    DWG_GETALL_OBJECT_DECL (ANGDIMOBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (ANNOTSCALEOBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (ASSOC3POINTANGULARDIMACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCALIGNEDDIMACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCARRAYMODIFYACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCEDGEACTIONPARAM);
    DWG_GETALL_OBJECT_DECL (ASSOCEDGECHAMFERACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCEDGEFILLETACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCMLEADERACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCORDINATEDIMACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCPERSSUBENTMANAGER);
    DWG_GETALL_OBJECT_DECL (ASSOCRESTOREENTITYSTATEACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCROTATEDDIMACTIONBODY);
    DWG_GETALL_OBJECT_DECL (ASSOCSWEPTSURFACEACTIONBODY);
    DWG_GETALL_OBJECT_DECL (BLOCKPROPERTIESTABLE);
    DWG_GETALL_OBJECT_DECL (BLOCKPROPERTIESTABLEGRIP);
    DWG_GETALL_OBJECT_DECL (BREAKDATA);
    DWG_GETALL_OBJECT_DECL (BREAKPOINTREF);
    DWG_GETALL_OBJECT_DECL (CONTEXTDATAMANAGER);
    DWG_GETALL_OBJECT_DECL (CSACDOCUMENTOPTIONS);
    DWG_GETALL_OBJECT_DECL (CURVEPATH);
    DWG_GETALL_OBJECT_DECL (DATATABLE);
    DWG_GETALL_OBJECT_DECL (DIMASSOC);
    DWG_GETALL_OBJECT_DECL (DMDIMOBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (DYNAMICBLOCKPROXYNODE);
    DWG_GETALL_OBJECT_DECL (GEOMAPIMAGE);
    DWG_GETALL_OBJECT_DECL (MLEADEROBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (MOTIONPATH);
    DWG_GETALL_OBJECT_DECL (MTEXTATTRIBUTEOBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (NAVISWORKSMODELDEF);
    DWG_GETALL_OBJECT_DECL (ORDDIMOBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (PERSUBENTMGR);
    DWG_GETALL_OBJECT_DECL (POINTPATH);
    DWG_GETALL_OBJECT_DECL (RADIMLGOBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (RADIMOBJECTCONTEXTDATA);
    DWG_GETALL_OBJECT_DECL (SUNSTUDY);
    DWG_GETALL_OBJECT_DECL (TABLECONTENT);
    DWG_GETALL_OBJECT_DECL (TVDEVICEPROPERTIES);
//  DWG_GETALL_OBJECT_DECL (ABSHDRAWINGSETTINGS);
//  DWG_GETALL_OBJECT_DECL (ACAECUSTOBJ);
//  DWG_GETALL_OBJECT_DECL (ACAEEEMGROBJ);
//  DWG_GETALL_OBJECT_DECL (ACAMCOMP);
//  DWG_GETALL_OBJECT_DECL (ACAMCOMPDEF);
//  DWG_GETALL_OBJECT_DECL (ACAMCOMPDEFMGR);
//  DWG_GETALL_OBJECT_DECL (ACAMCONTEXTMODELER);
//  DWG_GETALL_OBJECT_DECL (ACAMGDIMSTD);
//  DWG_GETALL_OBJECT_DECL (ACAMGFILTERDAT);
//  DWG_GETALL_OBJECT_DECL (ACAMGHOLECHARTSTDCSN);
//  DWG_GETALL_OBJECT_DECL (ACAMGHOLECHARTSTDDIN);
//  DWG_GETALL_OBJECT_DECL (ACAMGHOLECHARTSTDISO);
//  DWG_GETALL_OBJECT_DECL (ACAMGLAYSTD);
//  DWG_GETALL_OBJECT_DECL (ACAMGRCOMPDEF);
//  DWG_GETALL_OBJECT_DECL (ACAMGRCOMPDEFSET);
//  DWG_GETALL_OBJECT_DECL (ACAMGTITLESTD);
//  DWG_GETALL_OBJECT_DECL (ACAMMVDBACKUPOBJECT);
//  DWG_GETALL_OBJECT_DECL (ACAMPROJECT);
//  DWG_GETALL_OBJECT_DECL (ACAMSHAFTCOMPDEF);
//  DWG_GETALL_OBJECT_DECL (ACAMSTDPCOMPDEF);
//  DWG_GETALL_OBJECT_DECL (ACAMWBLOCKTEMPENTS);
//  DWG_GETALL_OBJECT_DECL (ACARRAYJIGENTITY);
//  DWG_GETALL_OBJECT_DECL (ACCMCONTEXT);
//  DWG_GETALL_OBJECT_DECL (ACDBCIRCARCRES);
//  DWG_GETALL_OBJECT_DECL (ACDBDIMENSIONRES);
//  DWG_GETALL_OBJECT_DECL (ACDBENTITYCACHE);
//  DWG_GETALL_OBJECT_DECL (ACDBLINERES);
//  DWG_GETALL_OBJECT_DECL (ACDBSTDPARTRES_ARC);
//  DWG_GETALL_OBJECT_DECL (ACDBSTDPARTRES_LINE);
//  DWG_GETALL_OBJECT_DECL (ACDB_HATCHSCALECONTEXTDATA_CLASS);
//  DWG_GETALL_OBJECT_DECL (ACDB_HATCHVIEWCONTEXTDATA_CLASS);
//  DWG_GETALL_OBJECT_DECL (ACDB_PROXY_ENTITY_DATA);
//  DWG_GETALL_OBJECT_DECL (ACDSRECORD);
//  DWG_GETALL_OBJECT_DECL (ACDSSCHEMA);
//  DWG_GETALL_OBJECT_DECL (ACGREFACADMASTER);
//  DWG_GETALL_OBJECT_DECL (ACGREFMASTER);
//  DWG_GETALL_OBJECT_DECL (ACIMINTSYSVAR);
//  DWG_GETALL_OBJECT_DECL (ACIMREALSYSVAR);
//  DWG_GETALL_OBJECT_DECL (ACIMSTRSYSVAR);
//  DWG_GETALL_OBJECT_DECL (ACIMSYSVARMAN);
//  DWG_GETALL_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMAPLEGENDDBOBJECT);
//  DWG_GETALL_OBJECT_DECL (ACMAPLEGENDITEMDBOBJECT);
//  DWG_GETALL_OBJECT_DECL (ACMAPMAPVIEWPORTDBOBJECT);
//  DWG_GETALL_OBJECT_DECL (ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER);
//  DWG_GETALL_OBJECT_DECL (ACMBALLOON);
//  DWG_GETALL_OBJECT_DECL (ACMBOM);
//  DWG_GETALL_OBJECT_DECL (ACMBOMROW);
//  DWG_GETALL_OBJECT_DECL (ACMBOMROWSTRUCT);
//  DWG_GETALL_OBJECT_DECL (ACMBOMSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMBOMSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMBOMSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMBOMSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMCENTERLINESTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMCENTERLINESTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMCENTERLINESTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMCENTERLINESTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMDATADICTIONARY);
//  DWG_GETALL_OBJECT_DECL (ACMDATAENTRY);
//  DWG_GETALL_OBJECT_DECL (ACMDATAENTRYBLOCK);
//  DWG_GETALL_OBJECT_DECL (ACMDATUMID);
//  DWG_GETALL_OBJECT_DECL (ACMDATUMSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMDATUMSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMDATUMSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMDATUMSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMDATUMSTANDARDISO2012);
//  DWG_GETALL_OBJECT_DECL (ACMDETAILSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMDETAILSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMDETAILSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMDETAILSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMDETAILTANDARDCUSTOM);
//  DWG_GETALL_OBJECT_DECL (ACMDIMBREAKPERSREACTOR);
//  DWG_GETALL_OBJECT_DECL (ACMEDRAWINGMAN);
//  DWG_GETALL_OBJECT_DECL (ACMEVIEW);
//  DWG_GETALL_OBJECT_DECL (ACME_DATABASE);
//  DWG_GETALL_OBJECT_DECL (ACME_DOCUMENT);
//  DWG_GETALL_OBJECT_DECL (ACMFCFRAME);
//  DWG_GETALL_OBJECT_DECL (ACMFCFSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMFCFSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMFCFSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMFCFSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMFCFSTANDARDISO2004);
//  DWG_GETALL_OBJECT_DECL (ACMFCFSTANDARDISO2012);
//  DWG_GETALL_OBJECT_DECL (ACMIDSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMIDSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMIDSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMIDSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMIDSTANDARDISO2004);
//  DWG_GETALL_OBJECT_DECL (ACMIDSTANDARDISO2012);
//  DWG_GETALL_OBJECT_DECL (ACMNOTESTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMNOTESTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMNOTESTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMNOTESTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMPARTLIST);
//  DWG_GETALL_OBJECT_DECL (ACMPICKOBJ);
//  DWG_GETALL_OBJECT_DECL (ACMSECTIONSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMSECTIONSTANDARDCSN2002);
//  DWG_GETALL_OBJECT_DECL (ACMSECTIONSTANDARDCUSTOM);
//  DWG_GETALL_OBJECT_DECL (ACMSECTIONSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMSECTIONSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMSECTIONSTANDARDISO2001);
//  DWG_GETALL_OBJECT_DECL (ACMSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMSURFSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMSURFSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMSURFSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMSURFSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMSURFSTANDARDISO2002);
//  DWG_GETALL_OBJECT_DECL (ACMSURFSYM);
//  DWG_GETALL_OBJECT_DECL (ACMTAPERSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMTAPERSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMTAPERSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMTAPERSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMTHREADLINESTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMTHREADLINESTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMTHREADLINESTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMTHREADLINESTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMWELDSTANDARDANSI);
//  DWG_GETALL_OBJECT_DECL (ACMWELDSTANDARDCSN);
//  DWG_GETALL_OBJECT_DECL (ACMWELDSTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (ACMWELDSTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (ACMWELDSYM);
//  DWG_GETALL_OBJECT_DECL (ACRFATTGENMGR);
//  DWG_GETALL_OBJECT_DECL (ACRFINSADJ);
//  DWG_GETALL_OBJECT_DECL (ACRFINSADJUSTERMGR);
//  DWG_GETALL_OBJECT_DECL (ACRFMCADAPIATTHOLDER);
//  DWG_GETALL_OBJECT_DECL (ACRFOBJATTMGR);
//  DWG_GETALL_OBJECT_DECL (ACSH_SUBENT_MATERIAL_CLASS);
//  DWG_GETALL_OBJECT_DECL (AC_AM_2D_XREF_MGR);
//  DWG_GETALL_OBJECT_DECL (AC_AM_BASIC_VIEW);
//  DWG_GETALL_OBJECT_DECL (AC_AM_BASIC_VIEW_DEF);
//  DWG_GETALL_OBJECT_DECL (AC_AM_COMPLEX_HIDE_SITUATION);
//  DWG_GETALL_OBJECT_DECL (AC_AM_COMP_VIEW_DEF);
//  DWG_GETALL_OBJECT_DECL (AC_AM_COMP_VIEW_INST);
//  DWG_GETALL_OBJECT_DECL (AC_AM_DIRTY_NODES);
//  DWG_GETALL_OBJECT_DECL (AC_AM_HIDE_SITUATION);
//  DWG_GETALL_OBJECT_DECL (AC_AM_MAPPER_CACHE);
//  DWG_GETALL_OBJECT_DECL (AC_AM_MASTER_VIEW_DEF);
//  DWG_GETALL_OBJECT_DECL (AC_AM_MVD_DEP_MGR);
//  DWG_GETALL_OBJECT_DECL (AC_AM_OVERRIDE_FILTER);
//  DWG_GETALL_OBJECT_DECL (AC_AM_PROPS_OVERRIDE);
//  DWG_GETALL_OBJECT_DECL (AC_AM_SHAFT_HIDE_SITUATION);
//  DWG_GETALL_OBJECT_DECL (AC_AM_STDP_VIEW_DEF);
//  DWG_GETALL_OBJECT_DECL (AC_AM_TRANSFORM_GHOST);
//  DWG_GETALL_OBJECT_DECL (ADAPPL);
//  DWG_GETALL_OBJECT_DECL (AECC_ALIGNMENT_DESIGN_CHECK_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_ALIGNMENT_LABEL_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_ALIGNMENT_LABEL_SET_EXT);
//  DWG_GETALL_OBJECT_DECL (AECC_ALIGNMENT_PARCEL_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_ALIGNMENT_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_APPURTENANCE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_ASSEMBLY_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_BUILDING_SITE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_CANT_DIAGRAM_VIEW_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_CATCHMENT_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_CLASS_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_CONTOURVIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_CORRIDOR_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_CURVE_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_PI_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_ASSEMBLY);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_BUILDINGSITE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_BUILDINGUTIL_CONNECTOR);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_CANT_DIAGRAM_VIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_CATCHMENT_AREA);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_CATCHMENT_AREA_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_CORRIDOR);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_CSVSTATIONSLIDER);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FACE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FEATURE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FEATURE_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE_PROFILE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE_SECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FITTING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FITTING_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FITTING_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FITTING_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_FLOW_SEGMENT_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_GENERAL_SEGMENT_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_GRADING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_GRAPH);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_GRAPHPROFILE_NETWORKPART);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_GRID_SURFACE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_GRID_SURFACE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_HYDRO_REGION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_INTERFERENCE_CHECK);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_INTERFERENCE_PART);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_INTERFERENCE_PART_SECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_INTERSECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_INTERSECTION_LOCATION_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_LEGEND_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_LOTLINE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_MASSHAULLINE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_MASS_HAUL_VIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_MATCHLINE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_MATCH_LINE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_MATERIAL_SECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_NETWORK);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_NOTE_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_OFFSET_ELEV_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PARCEL_BOUNDARY);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PARCEL_FACE_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PARCEL_SEGMENT);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PARCEL_SEGMENT_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PARCEL_SEGMENT_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PARCEL_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PIPE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PIPE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PIPE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PIPE_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PIPE_SECTION_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PIPE_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_POINT_ENT);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_POINT_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_POINT_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PRESSUREPIPENETWORK);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PART_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PROFILE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PROFILE_PROJECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PROFILE_PROJECTION_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PROFILE_VIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_RIGHT_OF_WAY);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SAMPLELINE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SAMPLE_LINE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SAMPLE_LINE_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_CORRIDOR);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_PIPENETWORK);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_PROJECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_PROJECTION_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_VIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SHEET);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SPANNING_PIPE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_STATION_ELEV_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_STRUCTURE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_PROFILE_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_SECTION_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SUBASSEMBLY);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SURFACE_ELEVATION_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SURFACE_SLOPE_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SVFIGURE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_PROFILE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_SECTION);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_SVNETWORK);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_TANGENT_INTERSECTION_TABLE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_TIN_SURFACE);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_TIN_SURFACE_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_CSV);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VIEWFRAME_LABELING);
//  DWG_GETALL_OBJECT_DECL (AECC_DISP_REP_VIEW_FRAME);
//  DWG_GETALL_OBJECT_DECL (AECC_FEATURELINE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_FEATURE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_FITTING_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_FORMAT_MANAGER_OBJECT);
//  DWG_GETALL_OBJECT_DECL (AECC_GRADEVIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_GRADING_CRITERIA);
//  DWG_GETALL_OBJECT_DECL (AECC_GRADING_CRITERIA_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_GRADING_GROUP);
//  DWG_GETALL_OBJECT_DECL (AECC_GRADING_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_IMPORT_STORM_SEWER_DEFAULTS);
//  DWG_GETALL_OBJECT_DECL (AECC_INTERFERENCE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_INTERSECTION_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_LABEL_COLLECTOR_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_LABEL_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_LABEL_RADIAL_LINE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_LABEL_TEXT_ITERATOR_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_LABEL_TEXT_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_LABEL_VECTOR_ARROW_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_LEGEND_TABLE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_MASS_HAUL_LINE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_MASS_HAUL_VIEW_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_MATCHLINE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_MATERIAL_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_NETWORK_PART_CATALOG_DEF_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_NETWORK_PART_FAMILY_ITEM);
//  DWG_GETALL_OBJECT_DECL (AECC_NETWORK_PART_LIST);
//  DWG_GETALL_OBJECT_DECL (AECC_NETWORK_RULE);
//  DWG_GETALL_OBJECT_DECL (AECC_PARCEL_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_PARCEL_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PART_SIZE_FILTER);
//  DWG_GETALL_OBJECT_DECL (AECC_PIPE_RULES);
//  DWG_GETALL_OBJECT_DECL (AECC_PIPE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PIPE_STYLE_EXTENSION);
//  DWG_GETALL_OBJECT_DECL (AECC_POINTCLOUD_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_POINTVIEW);
//  DWG_GETALL_OBJECT_DECL (AECC_POINT_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PRESSURE_PART_LIST);
//  DWG_GETALL_OBJECT_DECL (AECC_PRESSURE_PIPE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILESECTIONENTITY_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_DESIGN_CHECK_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_LABEL_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_BAND_STYLE_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_DATA_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_QUANTITY_TAKEOFF_CRITERIA);
//  DWG_GETALL_OBJECT_DECL (AECC_ROADWAYLINK_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_ROADWAYMARKER_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_ROADWAYSHAPE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_ROADWAY_STYLE_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_ROOT_SETTINGS_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_SAMPLE_LINE_GROUP_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SAMPLE_LINE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SECTION_LABEL_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_SECTION_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SECTION_VIEW_BAND_STYLE_SET);
//  DWG_GETALL_OBJECT_DECL (AECC_SECTION_VIEW_DATA_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SECTION_VIEW_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SETTINGS_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_SHEET_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SLOPE_PATTERN_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_STATION_FORMAT_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_STRUCTURE_RULES);
//  DWG_GETALL_OBJECT_DECL (AECC_STUCTURE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SURFACE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SVFIGURE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_SVNETWORK_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_TABLE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECC_TAG_MANAGER);
//  DWG_GETALL_OBJECT_DECL (AECC_TREE_NODE);
//  DWG_GETALL_OBJECT_DECL (AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION);
//  DWG_GETALL_OBJECT_DECL (AECC_VALIGNMENT_STYLE_EXTENSION);
//  DWG_GETALL_OBJECT_DECL (AECC_VIEW_FRAME_STYLE);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_PROPS_MEMBER);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_LOGICAL);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_PLAN);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_PLAN_SKETCH);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_PROJECTED);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_ELEVATION_DESIGN);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_ELEVATION_DETAIL);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_LOGICAL);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_MODEL_DESIGN);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_MODEL_DETAIL);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_PLAN_DESIGN);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_PLAN_DETAIL);
//  DWG_GETALL_OBJECT_DECL (AECS_DISP_REP_MEMBER_PLAN_SKETCH);
//  DWG_GETALL_OBJECT_DECL (AECS_MEMBER_NODE_SHAPE);
//  DWG_GETALL_OBJECT_DECL (AECS_MEMBER_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_2DSECTION_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPBDGELEVLINEPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPBDGELEVLINEPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPBDGSECTIONLINEPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPBDGSECTIONLINEPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCEILINGGRIDPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCEILINGGRIDPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCOLUMNGRIDPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCOLUMNGRIDPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLUNITPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLUNITPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPMVBLOCKREFPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPMVBLOCKREFPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPROOFPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPROOFPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPROOFSLABPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPROOFSLABPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPSLABPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPSLABPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPSPACEPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPSPACEPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPWALLPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPWALLPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPZONE100);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBDISPREPZONE50);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBZONEDEF);
//  DWG_GETALL_OBJECT_DECL (AEC_AECDBZONESTYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_ANCHOR_OPENINGBASE_TO_WALL);
//  DWG_GETALL_OBJECT_DECL (AEC_CLASSIFICATION_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_CLASSIFICATION_SYSTEM_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_CLEANUP_GROUP_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_CURTAIN_WALL_LAYOUT_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_CURTAIN_WALL_UNIT_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_CVSECTIONVIEW);
//  DWG_GETALL_OBJECT_DECL (AEC_DB_DISP_REP_DIM_GROUP_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DB_DISP_REP_DIM_GROUP_PLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_DB_DISP_REP_DIM_GROUP_PLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_DIM_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPLAYTHEME_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSELEMPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSELEMPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGPLAN100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGPLAN50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPROPSMASSELEMPLANCOMMON);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPROPSMASSGROUPPLANCOMMON);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPROPSOPENINGPLANCOMMON);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPROPSOPENINGPLANCOMMONHATCHED);
//  DWG_GETALL_OBJECT_DECL (AEC_DISPROPSOPENINGSILLPLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_2D_SECTION);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_CLIP_VOLUME);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_CLIP_VOLUME_RESULT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DIM);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DISPLAYTHEME);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DOOR);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DOOR_NOMINAL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DOOR_PLAN_100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DOOR_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_ENT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_ENT_REF);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_LAYOUT_CURVE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_LAYOUT_GRID2D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_LAYOUT_GRID3D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_MASKBLOCK);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_MASS_ELEM_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_MASS_GROUP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_MATERIAL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_OPENING);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_POLYGON_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_POLYGON_TRUECOLOUR);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_RAILING_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_RAILING_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_ROOF);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_ROOFSLAB);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_ROOFSLAB_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_SCHEDULE_TABLE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_SLAB);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_SLAB_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_SLICE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_SPACE_DECOMPOSED);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_SPACE_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_SPACE_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_STAIR_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_STAIR_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WALL_GRAPH);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WALL_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WALL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WALL_SCHEM);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WINDOW);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_NOMINAL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_PLAN_100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_SILL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_PROPS_ZONE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_2D_SECTION);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ANCHOR);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ANCHOR_BUB_TO_GRID);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ANCHOR_ENT_TO_NODE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ANCHOR_TAG_TO_ENT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_ELEVLINE_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_ELEVLINE_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_ELEVLINE_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_SECTIONLINE_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_SECTIONLINE_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_SECTIONLINE_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_SECTION_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_BDG_SECTION_SUBDIV);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CEILING_GRID);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CEILING_GRID_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CEILING_GRID_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_RESULT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COLUMN_GRID);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COLUMN_GRID_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COLUMN_GRID_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_BLOCK);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CIRCARC2D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONCOINCIDENT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONCONCENTRIC);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONEQUALDISTANCE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONMIDPOINT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONNECTOR);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONNORMAL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONPARALLEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONPERPENDICULAR);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONSYMMETRIC);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_CONTANGENT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_DIMANGLE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_DIMDIAMETER);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_DIMDISTANCE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_DIMLENGTH);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_DIMMAJORRADIUS);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_DIMMINORRADIUS);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_ELLIPARC2D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_LAYOUTDATA);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_LINE2D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_ADD);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_CUTPLANE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_EXTRUSION);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_GROUP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_LOFT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_PATH);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_REVOLVE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_SUBTRACT);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_TRANSITION);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_POINT2D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_PROFILE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_WORKPLANE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_COL_WORKPLANE_REF);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CONFIG);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DCM_DIMRADIUS);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DISPLAYTHEME);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_ELEV);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_NOMINAL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_PLAN_HEKTO);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_THRESHOLD_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_EDITINPLACEPROFILE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ENT_REF);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_LAYOUT_CURVE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_LAYOUT_GRID2D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_LAYOUT_GRID3D);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASKBLOCK_REF);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASKBLOCK_REF_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASS_ELEM_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASS_ELEM_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASS_ELEM_SCHEM);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASS_GROUP_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASS_GROUP_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MASS_GROUP_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MATERIAL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MVBLOCK_REF);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MVBLOCK_REF_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_MVBLOCK_REF_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_OPENING);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_OPENING_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_POLYGON_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_POLYGON_TRUECOLOUR);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_RAILING_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_RAILING_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_RAILING_PLAN_100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_RAILING_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ROOFSLAB_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ROOFSLAB_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ROOF_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ROOF_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ROOF_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SCHEDULE_TABLE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SET);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SLAB_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SLAB_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SLICE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SPACE_DECOMPOSED);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SPACE_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SPACE_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SPACE_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_SPACE_VOLUME);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_STAIR_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN_100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN_OVERLAPPING);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_STAIR_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WALL_GRAPH);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WALL_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WALL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WALL_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WALL_SCHEM);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_ELEV);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_MODEL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_NOMINAL);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_PLAN_100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_RCP);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_WINDOW_SILL_PLAN);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_REP_ZONE);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_ROPS_RAILING_PLAN_100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_ROPS_RAILING_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_ROPS_STAIR_PLAN_100);
//  DWG_GETALL_OBJECT_DECL (AEC_DISP_ROPS_STAIR_PLAN_50);
//  DWG_GETALL_OBJECT_DECL (AEC_DOOR_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_ENDCAP_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_FRAME_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_LAYERKEY_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_LIST_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_MASKBLOCK_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_MASS_ELEM_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_MATERIAL_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_MVBLOCK_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_MVBLOCK_REF);
//  DWG_GETALL_OBJECT_DECL (AEC_NOTIFICATION_TRACKER);
//  DWG_GETALL_OBJECT_DECL (AEC_POLYGON);
//  DWG_GETALL_OBJECT_DECL (AEC_POLYGON_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_PROPERTY_SET_DEF);
//  DWG_GETALL_OBJECT_DECL (AEC_RAILING_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_REFEDIT_STATUS_TRACKER);
//  DWG_GETALL_OBJECT_DECL (AEC_ROOFSLABEDGE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_ROOFSLAB_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_SCHEDULE_DATA_FORMAT);
//  DWG_GETALL_OBJECT_DECL (AEC_SLABEDGE_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_SLAB_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_SPACE_STYLES);
//  DWG_GETALL_OBJECT_DECL (AEC_STAIR_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_STAIR_WINDER_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_STAIR_WINDER_TYPE_BALANCED);
//  DWG_GETALL_OBJECT_DECL (AEC_STAIR_WINDER_TYPE_MANUAL);
//  DWG_GETALL_OBJECT_DECL (AEC_STAIR_WINDER_TYPE_SINGLE_POINT);
//  DWG_GETALL_OBJECT_DECL (AEC_VARS_AECBBLDSRV);
//  DWG_GETALL_OBJECT_DECL (AEC_VARS_ARCHBASE);
//  DWG_GETALL_OBJECT_DECL (AEC_VARS_DWG_SETUP);
//  DWG_GETALL_OBJECT_DECL (AEC_VARS_MUNICH);
//  DWG_GETALL_OBJECT_DECL (AEC_VARS_STRUCTUREBASE);
//  DWG_GETALL_OBJECT_DECL (AEC_WALLMOD_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_WALL_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_WINDOW_ASSEMBLY_STYLE);
//  DWG_GETALL_OBJECT_DECL (AEC_WINDOW_STYLE);
//  DWG_GETALL_OBJECT_DECL (ALIGNMENTGRIPENTITY);
//  DWG_GETALL_OBJECT_DECL (AMCONTEXTMGR);
//  DWG_GETALL_OBJECT_DECL (AMDTADMENUSTATE);
//  DWG_GETALL_OBJECT_DECL (AMDTAMMENUSTATE);
//  DWG_GETALL_OBJECT_DECL (AMDTBROWSERDBTAB);
//  DWG_GETALL_OBJECT_DECL (AMDTDMMENUSTATE);
//  DWG_GETALL_OBJECT_DECL (AMDTEDGESTANDARDDIN);
//  DWG_GETALL_OBJECT_DECL (AMDTEDGESTANDARDDIN13715);
//  DWG_GETALL_OBJECT_DECL (AMDTEDGESTANDARDISO);
//  DWG_GETALL_OBJECT_DECL (AMDTEDGESTANDARDISO13715);
//  DWG_GETALL_OBJECT_DECL (AMDTFORMULAUPDATEDISPATCHER);
//  DWG_GETALL_OBJECT_DECL (AMDTINTERNALREACTOR);
//  DWG_GETALL_OBJECT_DECL (AMDTMCOMMENUSTATE);
//  DWG_GETALL_OBJECT_DECL (AMDTMENUSTATEMGR);
//  DWG_GETALL_OBJECT_DECL (AMDTNOTE);
//  DWG_GETALL_OBJECT_DECL (AMDTNOTETEMPLATEDB);
//  DWG_GETALL_OBJECT_DECL (AMDTSECTIONSYM);
//  DWG_GETALL_OBJECT_DECL (AMDTSECTIONSYMLABEL);
//  DWG_GETALL_OBJECT_DECL (AMDTSYSATTR);
//  DWG_GETALL_OBJECT_DECL (AMGOBJPROPCFG);
//  DWG_GETALL_OBJECT_DECL (AMGSETTINGSOBJ);
//  DWG_GETALL_OBJECT_DECL (AMIMASTER);
//  DWG_GETALL_OBJECT_DECL (AM_DRAWING_MGR);
//  DWG_GETALL_OBJECT_DECL (AM_DWGMGR_NAME);
//  DWG_GETALL_OBJECT_DECL (AM_DWG_DOCUMENT);
//  DWG_GETALL_OBJECT_DECL (AM_DWG_SHEET);
//  DWG_GETALL_OBJECT_DECL (AM_VIEWDIMPARMAP);
//  DWG_GETALL_OBJECT_DECL (BINRECORD);
//  DWG_GETALL_OBJECT_DECL (CAMSCATALOGAPPOBJECT);
//  DWG_GETALL_OBJECT_DECL (CAMSSTRUCTBTNSTATE);
//  DWG_GETALL_OBJECT_DECL (CATALOGSTATE);
//  DWG_GETALL_OBJECT_DECL (CBROWSERAPPOBJECT);
//  DWG_GETALL_OBJECT_DECL (DEPMGR);
//  DWG_GETALL_OBJECT_DECL (DMBASEELEMENT);
//  DWG_GETALL_OBJECT_DECL (DMDEFAULTSTYLE);
//  DWG_GETALL_OBJECT_DECL (DMLEGEND);
//  DWG_GETALL_OBJECT_DECL (DMMAP);
//  DWG_GETALL_OBJECT_DECL (DMMAPMANAGER);
//  DWG_GETALL_OBJECT_DECL (DMSTYLECATEGORY);
//  DWG_GETALL_OBJECT_DECL (DMSTYLELIBRARY);
//  DWG_GETALL_OBJECT_DECL (DMSTYLEREFERENCE);
//  DWG_GETALL_OBJECT_DECL (DMSTYLIZEDENTITIESTABLE);
//  DWG_GETALL_OBJECT_DECL (DMSURROGATESTYLESETS);
//  DWG_GETALL_OBJECT_DECL (DM_PLACEHOLDER);
//  DWG_GETALL_OBJECT_DECL (EXACTERMXREFMAP);
//  DWG_GETALL_OBJECT_DECL (EXACXREFPANELOBJECT);
//  DWG_GETALL_OBJECT_DECL (EXPO_NOTIFYBLOCK);
//  DWG_GETALL_OBJECT_DECL (EXPO_NOTIFYHALL);
//  DWG_GETALL_OBJECT_DECL (EXPO_NOTIFYPILLAR);
//  DWG_GETALL_OBJECT_DECL (EXPO_NOTIFYSTAND);
//  DWG_GETALL_OBJECT_DECL (EXPO_NOTIFYSTANDNOPOLY);
//  DWG_GETALL_OBJECT_DECL (FLIPACTIONENTITY);
//  DWG_GETALL_OBJECT_DECL (GSMANAGER);
//  DWG_GETALL_OBJECT_DECL (IRD_DSC_DICT);
//  DWG_GETALL_OBJECT_DECL (IRD_DSC_RECORD);
//  DWG_GETALL_OBJECT_DECL (IRD_OBJ_RECORD);
//  DWG_GETALL_OBJECT_DECL (MAPFSMRVOBJECT);
//  DWG_GETALL_OBJECT_DECL (MAPGWSUNDOOBJECT);
//  DWG_GETALL_OBJECT_DECL (MAPIAMMOUDLE);
//  DWG_GETALL_OBJECT_DECL (MAPMETADATAOBJECT);
//  DWG_GETALL_OBJECT_DECL (MAPRESOURCEMANAGEROBJECT);
//  DWG_GETALL_OBJECT_DECL (MOVEACTIONENTITY);
//  DWG_GETALL_OBJECT_DECL (McDbContainer2);
//  DWG_GETALL_OBJECT_DECL (McDbMarker);
//  DWG_GETALL_OBJECT_DECL (NAMEDAPPL);
//  DWG_GETALL_OBJECT_DECL (NEWSTDPARTPARLIST);
//  DWG_GETALL_OBJECT_DECL (NPOCOLLECTION);
//  DWG_GETALL_OBJECT_DECL (OBJCLONER);
//  DWG_GETALL_OBJECT_DECL (PARAMMGR);
//  DWG_GETALL_OBJECT_DECL (PARAMSCOPE);
//  DWG_GETALL_OBJECT_DECL (PILLAR);
//  DWG_GETALL_OBJECT_DECL (RAPIDRTRENDERENVIRONMENT);
//  DWG_GETALL_OBJECT_DECL (ROTATEACTIONENTITY);
//  DWG_GETALL_OBJECT_DECL (SCALEACTIONENTITY);
//  DWG_GETALL_OBJECT_DECL (STDPART2D);
//  DWG_GETALL_OBJECT_DECL (STRETCHACTIONENTITY);
//  DWG_GETALL_OBJECT_DECL (TCH_ARROW);
//  DWG_GETALL_OBJECT_DECL (TCH_AXIS_LABEL);
//  DWG_GETALL_OBJECT_DECL (TCH_BLOCK_INSERT);
//  DWG_GETALL_OBJECT_DECL (TCH_COLUMN);
//  DWG_GETALL_OBJECT_DECL (TCH_DBCONFIG);
//  DWG_GETALL_OBJECT_DECL (TCH_DIMENSION2);
//  DWG_GETALL_OBJECT_DECL (TCH_DRAWINGINDEX);
//  DWG_GETALL_OBJECT_DECL (TCH_HANDRAIL);
//  DWG_GETALL_OBJECT_DECL (TCH_LINESTAIR);
//  DWG_GETALL_OBJECT_DECL (TCH_OPENING);
//  DWG_GETALL_OBJECT_DECL (TCH_RECTSTAIR);
//  DWG_GETALL_OBJECT_DECL (TCH_SLAB);
//  DWG_GETALL_OBJECT_DECL (TCH_SPACE);
//  DWG_GETALL_OBJECT_DECL (TCH_TEXT);
//  DWG_GETALL_OBJECT_DECL (TCH_WALL);
//  DWG_GETALL_OBJECT_DECL (TGrupoPuntos);
//  DWG_GETALL_OBJECT_DECL (VAACIMAGEINVENTORY);
//  DWG_GETALL_OBJECT_DECL (VAACXREFPANELOBJECT);
//  DWG_GETALL_OBJECT_DECL (XREFPANELOBJECT);
#  endif

/*******************************************************************
 *     Functions created from macro to cast dwg_object to entity     *
 *                 Usage :- dwg_object_to_ENTITY(),                  *
 *                where ENTITY can be LINE or CIRCLE                 *
 ********************************************************************/

/**
 * \fn Dwg_Entity_ENTITY *dwg_object_to_ENTITY(Dwg_Object *obj)
 * cast a Dwg_Object to Entity
 */
/* fixed <500 */
  CAST_DWG_OBJECT_TO_ENTITY_DECL (_3DFACE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (_3DSOLID);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (ARC);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (ATTDEF);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (ATTRIB);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (BLOCK);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (BODY);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (CIRCLE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ALIGNED);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ANG2LN);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ANG3PT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_DIAMETER);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_LINEAR);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_ORDINATE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (DIMENSION_RADIUS);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (ELLIPSE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (ENDBLK);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (INSERT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (LEADER);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (LINE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (LOAD);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (MINSERT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (MLINE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (MTEXT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (OLEFRAME);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (POINT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_2D);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_3D);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_MESH);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (POLYLINE_PFACE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (PROXY_ENTITY);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (RAY);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (REGION);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (SEQEND);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (SHAPE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (SOLID);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (SPLINE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (TEXT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (TOLERANCE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (TRACE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (UNKNOWN_ENT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_2D);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_3D);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_MESH);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_PFACE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (VERTEX_PFACE_FACE);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (VIEWPORT);
  CAST_DWG_OBJECT_TO_ENTITY_DECL (XLINE);
  /* untyped > 500 */
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (_3DLINE);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (CAMERA);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (DGNUNDERLAY);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (DWFUNDERLAY);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ENDREP);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (HATCH);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (IMAGE);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (JUMP);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LIGHT);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LWPOLYLINE);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (MESH);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (MULTILEADER);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (OLE2FRAME);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (PDFUNDERLAY);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (REPEAT);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (SECTIONOBJECT);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (WIPEOUT);
  /* unstable */
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ARC_DIMENSION);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (HELIX);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LARGE_RADIAL_DIMENSION);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LAYOUTPRINTCONFIG);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (PLANESURFACE);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (POINTCLOUD);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (POINTCLOUDEX);
#  ifdef DEBUG_CLASSES
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ALIGNMENTPARAMETERENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ARCALIGNEDTEXT);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (BASEPOINTPARAMETERENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (EXTRUDEDSURFACE);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (FLIPGRIPENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (FLIPPARAMETERENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (GEOPOSITIONMARKER);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LINEARGRIPENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LINEARPARAMETERENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LOFTEDSURFACE);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (MPOLYGON);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (NAVISWORKSMODEL);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (NURBSURFACE);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (POINTPARAMETERENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (POLARGRIPENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (REVOLVEDSURFACE);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ROTATIONGRIPENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ROTATIONPARAMETERENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (RTEXT);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (SWEPTSURFACE);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (TABLE);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (VISIBILITYGRIPENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (VISIBILITYPARAMETERENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (XYGRIPENTITY);
    CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (XYPARAMETERENTITY);
#  endif

/*******************************************************************
 *     Functions created from macro to cast dwg object to object     *
 *                 Usage :- dwg_object_to_OBJECT(),                  *
 *            where OBJECT can be LAYER or BLOCK_HEADER              *
 ********************************************************************/
/**
 * \fn Dwg_Object_OBJECT *dwg_object_to_OBJECT(Dwg_Object *obj)
 * cast a Dwg_Object to Object
 */
  CAST_DWG_OBJECT_TO_OBJECT_DECL (APPID);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (APPID_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCK_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCK_HEADER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DICTIONARY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DIMSTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DIMSTYLE_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DUMMY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYER_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LONG_TRANSACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LTYPE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LTYPE_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (MLINESTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (STYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (STYLE_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (UCS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (UCS_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (UNKNOWN_OBJ);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VIEW);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VIEW_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VPORT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VPORT_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VX_CONTROL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VX_TABLE_RECORD);
  /* untyped > 500 */
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_BOOLEAN_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_BOX_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_CONE_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_CYLINDER_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_FILLET_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_HISTORY_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_SPHERE_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_TORUS_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_WEDGE_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCGEOMDEPENDENCY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCNETWORK);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKALIGNMENTGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKALIGNMENTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKBASEPOINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKFLIPACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKFLIPGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKFLIPPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKGRIPLOCATIONCOMPONENT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKLINEARGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKLOOKUPGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKMOVEACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKROTATEACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKROTATIONGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKSCALEACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKVISIBILITYGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (CELLSTYLEMAP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DETAILVIEWSTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DICTIONARYVAR);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DICTIONARYWDFLT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DYNAMICBLOCKPURGEPREVENTER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (FIELD);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (FIELDLIST);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (GEODATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (GROUP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (IDBUFFER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (IMAGEDEF);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (IMAGEDEF_REACTOR);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (INDEX);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYERFILTER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYER_INDEX);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LAYOUT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (PLACEHOLDER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (PLOTSETTINGS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RASTERVARIABLES);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERENVIRONMENT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SCALE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SECTIONVIEWSTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SECTION_MANAGER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SORTENTSTABLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SPATIAL_FILTER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SUN);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (TABLEGEOMETRY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VBA_PROJECT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (VISUALSTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (WIPEOUTVARIABLES);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (XRECORD);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (PDFDEFINITION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DGNDEFINITION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DWFDEFINITION);
  /* unstable */
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_BREP_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_CHAMFER_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_PYRAMID_CLASS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ALDIMOBJECTCONTEXTDATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOC2DCONSTRAINTGROUP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCASMBODYACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCBLENDSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCCOMPOUNDACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCDEPENDENCY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCDIMDEPENDENCYBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCEXTENDSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCEXTRUDEDSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCFACEACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCFILLETSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCLOFTEDSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCNETWORKSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCOBJECTACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCOFFSETSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCOSNAPPOINTREFACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCPATCHSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCPATHACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCPLANESURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCPOINTREFACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCREVOLVEDSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCTRIMSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCVALUEDEPENDENCY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCVARIABLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCVERTEXACTIONPARAM);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLKREFOBJECTCONTEXTDATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKALIGNEDCONSTRAINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKANGULARCONSTRAINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKARRAYACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKDIAMETRICCONSTRAINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKHORIZONTALCONSTRAINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKLINEARCONSTRAINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKLINEARPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKLOOKUPACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKLOOKUPPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKPARAMDEPENDENCYBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKPOINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKPOLARGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKPOLARPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKPOLARSTRETCHACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKRADIALCONSTRAINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKREPRESENTATION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKROTATIONPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKSTRETCHACTION);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKUSERPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKVERTICALCONSTRAINTPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKVISIBILITYPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKXYGRIP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKXYPARAMETER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DATALINK);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DBCOLOR);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (EVALUATION_GRAPH);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (FCFOBJECTCONTEXTDATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (GRADIENT_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (GROUND_PLANE_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (IBL_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (IMAGE_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LEADEROBJECTCONTEXTDATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (LIGHTLIST);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (MATERIAL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (MENTALRAYRENDERSETTINGS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (MLEADERSTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (MTEXTOBJECTCONTEXTDATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (OBJECT_PTR);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (PARTIAL_VIEWING_INDEX);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (POINTCLOUDCOLORMAP);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (POINTCLOUDDEF);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (POINTCLOUDDEFEX);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (POINTCLOUDDEF_REACTOR);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (POINTCLOUDDEF_REACTOR_EX);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (PROXY_OBJECT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RAPIDRTRENDERSETTINGS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERENTRY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERGLOBAL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERSETTINGS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SECTION_SETTINGS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SKYLIGHT_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SOLID_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SPATIAL_INDEX);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (TABLESTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (TEXTOBJECTCONTEXTDATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYMODIFYPARAMETERS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYPATHPARAMETERS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYPOLARPARAMETERS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYRECTANGULARPARAMETERS);
#  ifdef DEBUG_CLASSES
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMECOMMANDHISTORY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMESCOPE);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMESTATEMGR);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_EXTRUSION_CLASS);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_LOFT_CLASS);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_REVOLVE_CLASS);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_SWEEP_CLASS);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ANGDIMOBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ANNOTSCALEOBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOC3POINTANGULARDIMACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCALIGNEDDIMACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYMODIFYACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCEDGEACTIONPARAM);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCEDGECHAMFERACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCEDGEFILLETACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCMLEADERACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCORDINATEDIMACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCPERSSUBENTMANAGER);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCRESTOREENTITYSTATEACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCROTATEDDIMACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCSWEPTSURFACEACTIONBODY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKPROPERTIESTABLE);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (BLOCKPROPERTIESTABLEGRIP);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (BREAKDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (BREAKPOINTREF);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (CONTEXTDATAMANAGER);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (CSACDOCUMENTOPTIONS);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (CURVEPATH);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (DATATABLE);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (DIMASSOC);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (DMDIMOBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (DYNAMICBLOCKPROXYNODE);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (GEOMAPIMAGE);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (MLEADEROBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (MOTIONPATH);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (MTEXTATTRIBUTEOBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (NAVISWORKSMODELDEF);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (ORDDIMOBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (PERSUBENTMGR);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (POINTPATH);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (RADIMLGOBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (RADIMOBJECTCONTEXTDATA);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (SUNSTUDY);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (TABLECONTENT);
    CAST_DWG_OBJECT_TO_OBJECT_DECL (TVDEVICEPROPERTIES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ABSHDRAWINGSETTINGS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAECUSTOBJ);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAEEEMGROBJ);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMCOMP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMCOMPDEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMCOMPDEFMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMCONTEXTMODELER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGDIMSTD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGFILTERDAT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGHOLECHARTSTDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGHOLECHARTSTDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGHOLECHARTSTDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGLAYSTD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGRCOMPDEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGRCOMPDEFSET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMGTITLESTD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMMVDBACKUPOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMPROJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMSHAFTCOMPDEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMSTDPCOMPDEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACAMWBLOCKTEMPENTS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACARRAYJIGENTITY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACCMCONTEXT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDBCIRCARCRES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDBDIMENSIONRES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDBENTITYCACHE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDBLINERES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDBSTDPARTRES_ARC);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDBSTDPARTRES_LINE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDB_HATCHSCALECONTEXTDATA_CLASS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDB_HATCHVIEWCONTEXTDATA_CLASS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDB_PROXY_ENTITY_DATA);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDSRECORD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDSSCHEMA);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACGREFACADMASTER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACGREFMASTER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACIMINTSYSVAR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACIMREALSYSVAR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACIMSTRSYSVAR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACIMSYSVARMAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMANOOTATIONVIEWSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMAPLEGENDDBOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMAPLEGENDITEMDBOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMAPMAPVIEWPORTDBOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMAPPRINTLAYOUTELEMENTDBOBJECTCONTAINER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBALLOON);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBOM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBOMROW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBOMROWSTRUCT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBOMSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBOMSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBOMSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMBOMSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMCENTERLINESTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMCENTERLINESTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMCENTERLINESTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMCENTERLINESTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATADICTIONARY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATAENTRY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATAENTRYBLOCK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATUMID);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATUMSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATUMSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATUMSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATUMSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDATUMSTANDARDISO2012);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDETAILSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDETAILSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDETAILSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDETAILSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDETAILTANDARDCUSTOM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMDIMBREAKPERSREACTOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMEDRAWINGMAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMEVIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACME_DATABASE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACME_DOCUMENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMFCFRAME);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMFCFSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMFCFSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMFCFSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMFCFSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMFCFSTANDARDISO2004);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMFCFSTANDARDISO2012);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMIDSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMIDSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMIDSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMIDSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMIDSTANDARDISO2004);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMIDSTANDARDISO2012);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMNOTESTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMNOTESTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMNOTESTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMNOTESTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMPARTLIST);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMPICKOBJ);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSECTIONSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSECTIONSTANDARDCSN2002);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSECTIONSTANDARDCUSTOM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSECTIONSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSECTIONSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSECTIONSTANDARDISO2001);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSURFSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSURFSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSURFSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSURFSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSURFSTANDARDISO2002);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMSURFSYM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTAPERSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTAPERSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTAPERSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTAPERSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTHREADLINESTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTHREADLINESTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTHREADLINESTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMTHREADLINESTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMWELDSTANDARDANSI);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMWELDSTANDARDCSN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMWELDSTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMWELDSTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACMWELDSYM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACRFATTGENMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACRFINSADJ);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACRFINSADJUSTERMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACRFMCADAPIATTHOLDER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACRFOBJATTMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ACSH_SUBENT_MATERIAL_CLASS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_2D_XREF_MGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_BASIC_VIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_BASIC_VIEW_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_COMPLEX_HIDE_SITUATION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_COMP_VIEW_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_COMP_VIEW_INST);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_DIRTY_NODES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_HIDE_SITUATION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_MAPPER_CACHE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_MASTER_VIEW_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_MVD_DEP_MGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_OVERRIDE_FILTER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_PROPS_OVERRIDE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_SHAFT_HIDE_SITUATION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_STDP_VIEW_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AC_AM_TRANSFORM_GHOST);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ADAPPL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ALIGNMENT_DESIGN_CHECK_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ALIGNMENT_LABEL_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ALIGNMENT_LABEL_SET_EXT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ALIGNMENT_PARCEL_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ALIGNMENT_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_APPURTENANCE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ASSEMBLY_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_BUILDING_SITE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_CANT_DIAGRAM_VIEW_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_CATCHMENT_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_CLASS_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_CONTOURVIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_CORRIDOR_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_CANT_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_CURVE_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_DESIGNSPEED_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_GEOMPT_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_INDEXED_PI_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_MINOR_STATION_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_PI_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_SPIRAL_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_STAEQU_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_STATION_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_STATION_OFFSET_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_SUPERELEVATION_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_TANGENT_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ALIGNMENT_VERTICAL_GEOMPT_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_APPURTENANCE_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_ASSEMBLY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_PROFILE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_AUTO_CORRIDOR_FEATURE_LINE_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE_PROFILE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_AUTO_FEATURE_LINE_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_BUILDINGSITE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_BUILDINGUTIL_CONNECTOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_CANT_DIAGRAM_VIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_CATCHMENT_AREA);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_CATCHMENT_AREA_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_CORRIDOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_CROSSING_PIPE_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_CROSSING_PRESSURE_PIPE_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_CSVSTATIONSLIDER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FACE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FEATURE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FEATURE_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE_PROFILE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FEATURE_LINE_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FITTING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FITTING_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FITTING_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FITTING_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_FLOW_SEGMENT_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_GENERAL_SEGMENT_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_GRADING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_GRAPH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_GRAPHPROFILE_NETWORKPART);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_GRAPHPROFILE_PRESSUREPART);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_GRID_SURFACE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_GRID_SURFACE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_HORGEOMETRY_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_HYDRO_REGION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_INTERFERENCE_CHECK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_INTERFERENCE_PART);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_INTERFERENCE_PART_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_INTERSECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_INTERSECTION_LOCATION_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_LEGEND_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_LINE_BETWEEN_POINTS_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_LOTLINE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_MASSHAULLINE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_MASS_HAUL_VIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_MATCHLINE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_MATCH_LINE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_MATERIAL_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_NETWORK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_NOTE_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_OFFSET_ELEV_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PARCEL_BOUNDARY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PARCEL_FACE_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PARCEL_SEGMENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PARCEL_SEGMENT_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PARCEL_SEGMENT_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PARCEL_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PIPE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PIPENETWORK_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PIPE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PIPE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PIPE_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PIPE_SECTION_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PIPE_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_POINT_ENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_POINT_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_POINT_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PRESSUREPIPENETWORK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PART_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PRESSURE_PIPE_SECTION_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PROFILE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PROFILEDATA_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PROFILE_PROJECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PROFILE_PROJECTION_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PROFILE_VIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_PROFILE_VIEW_DEPTH_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_QUANTITY_TAKEOFF_AGGREGATE_EARTHWORK_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_RIGHT_OF_WAY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SAMPLELINE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SAMPLE_LINE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SAMPLE_LINE_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTIONALDATA_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTIONDATA_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTIONSEGMENT_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_CORRIDOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_CORRIDOR_POINT_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_GRADEBREAK_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_MINOR_OFFSET_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_OFFSET_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_PIPENETWORK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_PRESSUREPIPENETWORK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_PROJECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_PROJECTION_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_SEGMENT_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_VIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_VIEW_DEPTH_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SECTION_VIEW_QUANTITY_TAKEOFF_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SHEET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SPANNING_PIPE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SPANNING_PIPE_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_STATION_ELEV_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_STRUCTURE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_PROFILE_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_STRUCTURE_SECTION_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SUBASSEMBLY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SUPERELEVATION_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SUPERELEVATION_DIAGRAM_VIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SURFACE_CONTOUR_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SURFACE_ELEVATION_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SURFACE_SLOPE_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SURVEY_FIGURE_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SVFIGURE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_PROFILE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SVFIGURE_SEGMENT_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_SVNETWORK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_TANGENT_INTERSECTION_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_TIN_SURFACE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_TIN_SURFACE_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_CRESTCURVE_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_CSV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_HAGEOMPT_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_LINE_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_MINOR_STATION_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_PVI_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_SAGCURVE_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VALIGNMENT_STATION_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VERTICALGEOMETRY_BAND_LABEL_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VIEWFRAME_LABELING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_DISP_REP_VIEW_FRAME);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_FEATURELINE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_FEATURE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_FITTING_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_FORMAT_MANAGER_OBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_GRADEVIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_GRADING_CRITERIA);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_GRADING_CRITERIA_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_GRADING_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_GRADING_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_IMPORT_STORM_SEWER_DEFAULTS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_INTERFERENCE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_INTERSECTION_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LABEL_COLLECTOR_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LABEL_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LABEL_RADIAL_LINE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LABEL_TEXT_ITERATOR_CURVE_OR_SPIRAL_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LABEL_TEXT_ITERATOR_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LABEL_TEXT_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LABEL_VECTOR_ARROW_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_LEGEND_TABLE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_MASS_HAUL_LINE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_MASS_HAUL_VIEW_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_MATCHLINE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_MATERIAL_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_NETWORK_PART_CATALOG_DEF_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_NETWORK_PART_FAMILY_ITEM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_NETWORK_PART_LIST);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_NETWORK_RULE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PARCEL_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PARCEL_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PART_SIZE_FILTER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PIPE_RULES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PIPE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PIPE_STYLE_EXTENSION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_POINTCLOUD_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_POINTVIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_POINT_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PRESSURE_PART_LIST);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PRESSURE_PIPE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILESECTIONENTITY_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_DESIGN_CHECK_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_LABEL_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_BAND_STYLE_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_DATA_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_HORIZONTAL_GEOMETRY_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_PIPE_NETWORK_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_SECTIONAL_DATA_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_SUPERELEVATION_DIAGRAM_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_PROFILE_VIEW_VERTICAL_GEOMETRY_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_QUANTITY_TAKEOFF_CRITERIA);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ROADWAYLINK_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ROADWAYMARKER_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ROADWAYSHAPE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ROADWAY_STYLE_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_ROOT_SETTINGS_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SAMPLE_LINE_GROUP_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SAMPLE_LINE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SECTION_LABEL_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SECTION_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SECTION_VIEW_BAND_STYLE_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SECTION_VIEW_DATA_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SECTION_VIEW_ROAD_SURFACE_BAND_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SECTION_VIEW_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SETTINGS_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SHEET_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SLOPE_PATTERN_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_STATION_FORMAT_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_STRUCTURE_RULES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_STUCTURE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SUPERELEVATION_DIAGRAM_VIEW_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SURFACE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SVFIGURE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_SVNETWORK_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_TABLE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_TAG_MANAGER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_TREE_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_USER_DEFINED_ATTRIBUTE_CLASSIFICATION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_VALIGNMENT_STYLE_EXTENSION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECC_VIEW_FRAME_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_PROPS_MEMBER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_LOGICAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_PLAN_SKETCH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_PROPS_MEMBER_PROJECTED);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_ELEVATION_DESIGN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_ELEVATION_DETAIL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_LOGICAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_MODEL_DESIGN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_MODEL_DETAIL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_PLAN_DESIGN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_PLAN_DETAIL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_DISP_REP_MEMBER_PLAN_SKETCH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_MEMBER_NODE_SHAPE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AECS_MEMBER_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_2DSECTION_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPBDGELEVLINEPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPBDGELEVLINEPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPBDGSECTIONLINEPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPBDGSECTIONLINEPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCEILINGGRIDPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCEILINGGRIDPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCOLUMNGRIDPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCOLUMNGRIDPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLLAYOUTPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLUNITPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPCURTAINWALLUNITPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPMVBLOCKREFPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPMVBLOCKREFPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPROOFPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPROOFPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPROOFSLABPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPROOFSLABPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPSLABPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPSLABPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPSPACEPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPSPACEPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPWALLPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPWALLPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPWINDOWASSEMBLYPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPZONE100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBDISPREPZONE50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBZONEDEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_AECDBZONESTYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_ANCHOR_OPENINGBASE_TO_WALL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_CLASSIFICATION_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_CLASSIFICATION_SYSTEM_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_CLEANUP_GROUP_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_CURTAIN_WALL_LAYOUT_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_CURTAIN_WALL_UNIT_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_CVSECTIONVIEW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DB_DISP_REP_DIM_GROUP_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DB_DISP_REP_DIM_GROUP_PLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DB_DISP_REP_DIM_GROUP_PLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DIM_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPLAYTHEME_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSELEMPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSELEMPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPMASSGROUPPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGPLAN100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGPLAN50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGPLANREFLECTED);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPREPAECDBDISPREPOPENINGSILLPLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPROPSMASSELEMPLANCOMMON);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPROPSMASSGROUPPLANCOMMON);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPROPSOPENINGPLANCOMMON);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPROPSOPENINGPLANCOMMONHATCHED);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISPROPSOPENINGSILLPLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_2D_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_CLIP_VOLUME);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_CLIP_VOLUME_RESULT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DIM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DISPLAYTHEME);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DOOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DOOR_NOMINAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DOOR_PLAN_100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DOOR_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DOOR_THRESHOLD_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_DOOR_THRESHOLD_SYMBOL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_EDITINPLACEPROFILE_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_ENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_ENT_REF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_GRID_ASSEMBLY_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_GRID_ASSEMBLY_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_LAYOUT_CURVE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_LAYOUT_GRID2D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_LAYOUT_GRID3D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_MASKBLOCK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_MASS_ELEM_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_MASS_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_MATERIAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_OPENING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_POLYGON_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_POLYGON_TRUECOLOUR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_RAILING_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_RAILING_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_ROOF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_ROOFSLAB);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_ROOFSLAB_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_SCHEDULE_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_SLAB);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_SLAB_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_SLICE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_SPACE_DECOMPOSED);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_SPACE_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_SPACE_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_STAIR_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_STAIR_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_STAIR_PLAN_OVERLAPPING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WALL_GRAPH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WALL_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WALL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WALL_SCHEM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WINDOW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_ASSEMBLY_SILL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_NOMINAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_PLAN_100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_WINDOW_SILL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_PROPS_ZONE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_2D_SECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ANCHOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ANCHOR_BUB_TO_GRID);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ANCHOR_BUB_TO_GRID_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ANCHOR_ENT_TO_NODE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ANCHOR_EXT_TAG_TO_ENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ANCHOR_TAG_TO_ENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_ELEVLINE_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_ELEVLINE_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_ELEVLINE_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_SECTIONLINE_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_SECTIONLINE_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_SECTIONLINE_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_SECTION_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_BDG_SECTION_SUBDIV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CEILING_GRID);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CEILING_GRID_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CEILING_GRID_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_RESULT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CLIP_VOLUME_RESULT_SUBDIV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COLUMN_GRID);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COLUMN_GRID_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COLUMN_GRID_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_BLOCK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CIRCARC2D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONCOINCIDENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONCONCENTRIC);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONEQUALDISTANCE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONMIDPOINT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONNECTOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONNORMAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONPARALLEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONPERPENDICULAR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONSYMMETRIC);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_CONTANGENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_DIMANGLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_DIMDIAMETER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_DIMDISTANCE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_DIMLENGTH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_DIMMAJORRADIUS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_DIMMINORRADIUS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_ELLIPARC2D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_LAYOUTDATA);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_LINE2D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_ADD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_CUTPLANE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_EXTRUSION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_GROUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_LOFT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_PATH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_REVOLVE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_SUBTRACT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_MODIFIER_TRANSITION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_POINT2D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_PROFILE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_WORKPLANE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_COL_WORKPLANE_REF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CONFIG);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_LAYOUT_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_UNIT_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_CURTAIN_WALL_UNIT_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DCM_DIMRADIUS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DISPLAYTHEME);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_ELEV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_NOMINAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_PLAN_HEKTO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_THRESHOLD_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_DOOR_THRESHOLD_SYMBOL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_EDITINPLACEPROFILE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ENT_REF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_LAYOUT_CURVE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_LAYOUT_GRID2D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_LAYOUT_GRID3D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASKBLOCK_REF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASKBLOCK_REF_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASS_ELEM_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASS_ELEM_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASS_ELEM_SCHEM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASS_GROUP_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASS_GROUP_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MASS_GROUP_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MATERIAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MVBLOCK_REF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MVBLOCK_REF_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_MVBLOCK_REF_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_OPENING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_OPENING_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_POLYGON_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_POLYGON_TRUECOLOUR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_RAILING_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_RAILING_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_RAILING_PLAN_100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_RAILING_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ROOFSLAB_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ROOFSLAB_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ROOF_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ROOF_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ROOF_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SCHEDULE_TABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SLAB_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SLAB_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SLICE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SPACE_DECOMPOSED);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SPACE_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SPACE_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SPACE_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_SPACE_VOLUME);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_STAIR_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN_100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_STAIR_PLAN_OVERLAPPING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_STAIR_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WALL_GRAPH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WALL_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WALL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WALL_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WALL_SCHEM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOWASSEMBLY_SILL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_ASSEMBLY_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_ASSEMBLY_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_ELEV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_MODEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_NOMINAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_PLAN_100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_RCP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_WINDOW_SILL_PLAN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_REP_ZONE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_ROPS_RAILING_PLAN_100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_ROPS_RAILING_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_ROPS_STAIR_PLAN_100);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DISP_ROPS_STAIR_PLAN_50);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_DOOR_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_ENDCAP_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_FRAME_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_LAYERKEY_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_LIST_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_MASKBLOCK_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_MASS_ELEM_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_MATERIAL_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_MVBLOCK_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_MVBLOCK_REF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_NOTIFICATION_TRACKER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_POLYGON);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_POLYGON_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_PROPERTY_SET_DEF);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_RAILING_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_REFEDIT_STATUS_TRACKER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_ROOFSLABEDGE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_ROOFSLAB_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_SCHEDULE_DATA_FORMAT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_SLABEDGE_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_SLAB_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_SPACE_STYLES);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_STAIR_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_STAIR_WINDER_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_STAIR_WINDER_TYPE_BALANCED);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_STAIR_WINDER_TYPE_MANUAL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_STAIR_WINDER_TYPE_SINGLE_POINT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_VARS_AECBBLDSRV);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_VARS_ARCHBASE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_VARS_DWG_SETUP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_VARS_MUNICH);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_VARS_STRUCTUREBASE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_WALLMOD_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_WALL_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_WINDOW_ASSEMBLY_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AEC_WINDOW_STYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ALIGNMENTGRIPENTITY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMCONTEXTMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTADMENUSTATE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTAMMENUSTATE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTBROWSERDBTAB);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTDMMENUSTATE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTEDGESTANDARDDIN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTEDGESTANDARDDIN13715);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTEDGESTANDARDISO);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTEDGESTANDARDISO13715);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTFORMULAUPDATEDISPATCHER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTINTERNALREACTOR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTMCOMMENUSTATE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTMENUSTATEMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTNOTE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTNOTETEMPLATEDB);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTSECTIONSYM);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTSECTIONSYMLABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMDTSYSATTR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMGOBJPROPCFG);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMGSETTINGSOBJ);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AMIMASTER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AM_DRAWING_MGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AM_DWGMGR_NAME);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AM_DWG_DOCUMENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AM_DWG_SHEET);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (AM_VIEWDIMPARMAP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (BINRECORD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (CAMSCATALOGAPPOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (CAMSSTRUCTBTNSTATE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (CATALOGSTATE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (CBROWSERAPPOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DEPMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMBASEELEMENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMDEFAULTSTYLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMLEGEND);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMMAP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMMAPMANAGER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMSTYLECATEGORY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMSTYLELIBRARY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMSTYLEREFERENCE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMSTYLIZEDENTITIESTABLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DMSURROGATESTYLESETS);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (DM_PLACEHOLDER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (EXACTERMXREFMAP);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (EXACXREFPANELOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (EXPO_NOTIFYBLOCK);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (EXPO_NOTIFYHALL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (EXPO_NOTIFYPILLAR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (EXPO_NOTIFYSTAND);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (EXPO_NOTIFYSTANDNOPOLY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (FLIPACTIONENTITY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (GSMANAGER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (IRD_DSC_DICT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (IRD_DSC_RECORD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (IRD_OBJ_RECORD);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (MAPFSMRVOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (MAPGWSUNDOOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (MAPIAMMOUDLE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (MAPMETADATAOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (MAPRESOURCEMANAGEROBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (MOVEACTIONENTITY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (McDbContainer2);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (McDbMarker);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (NAMEDAPPL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (NEWSTDPARTPARLIST);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (NPOCOLLECTION);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (OBJCLONER);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (PARAMMGR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (PARAMSCOPE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (PILLAR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (RAPIDRTRENDERENVIRONMENT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (ROTATEACTIONENTITY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (SCALEACTIONENTITY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (STDPART2D);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (STRETCHACTIONENTITY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_ARROW);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_AXIS_LABEL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_BLOCK_INSERT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_COLUMN);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_DBCONFIG);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_DIMENSION2);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_DRAWINGINDEX);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_HANDRAIL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_LINESTAIR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_OPENING);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_RECTSTAIR);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_SLAB);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_SPACE);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_TEXT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TCH_WALL);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (TGrupoPuntos);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (VAACIMAGEINVENTORY);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (VAACXREFPANELOBJECT);
  //  CAST_DWG_OBJECT_TO_OBJECT_DECL (XREFPANELOBJECT);
#  endif
  // clang-format: on
  /* End auto-generated content */

  EXPORT dwg_ent_dim *dwg_object_to_DIMENSION (dwg_object *obj);

  /********************************************************************/

  /// initialize the DWG version, needed for r2007+ unicode strings
  /// and pre-R13 field variants. unneeded for r13-r2004
  EXPORT void dwg_api_init_version (Dwg_Data *dwg);

#define _deprecated_dynapi_getter                                             \
  __attribute_deprecated_msg__ ("use ‘dwg_dynapi_entity_value‘ instead")
#define _deprecated_dynapi_setter                                             \
  __attribute_deprecated_msg__ ("use ‘dwg_dynapi_entity_set_value‘ instead")
#define _deprecated_dynapi_common_getter                                      \
  __attribute_deprecated_msg__ ("use ‘dwg_dynapi_common_value‘ instead")
#define _deprecated_dynapi_common_setter                                      \
  __attribute_deprecated_msg__ ("use ‘dwg_dynapi_common_set_value‘ instead")

  /********************************************************************
   *                FUNCTIONS START HERE ENTITY SPECIFIC               *
   *********************************************************************/

  EXPORT bool dwg_get_HEADER (const Dwg_Data *restrict dwg,
                              const char *restrict fieldname,
                              void *restrict out) __nonnull ((1, 2, 3));
  EXPORT bool dwg_get_HEADER_utf8text (const Dwg_Data *restrict dwg,
                                       const char *restrict fieldname,
                                       char **restrict textp, int *isnewp)
      __nonnull ((1, 2, 3));
  EXPORT bool dwg_set_HEADER (Dwg_Data *restrict dwg,
                              const char *restrict fieldname,
                              const void *restrict value)
      __nonnull ((1, 2, 3));
  EXPORT bool dwg_set_HEADER_utf8text (Dwg_Data *restrict dwg,
                                       const char *restrict fieldname,
                                       const char *restrict utf8)
      __nonnull ((1, 2, 3));
  EXPORT bool dwg_get_ENTITY_common (Dwg_Object_Entity *restrict obj,
                                     const char *restrict fieldname,
                                     void *restrict value)
      __nonnull ((1, 2, 3));
  EXPORT bool dwg_get_ENTITY_common_utf8text (Dwg_Object_Entity *restrict obj,
                                              const char *restrict fieldname,
                                              char **restrict textp,
                                              int *isnewp)
      __nonnull ((1, 2, 3));
  EXPORT bool dwg_get_OBJECT_common (Dwg_Object_Object *restrict obj,
                                     const char *restrict fieldname,
                                     void *restrict value)
      __nonnull ((1, 2, 3));
  EXPORT bool dwg_get_OBJECT_common_utf8text (Dwg_Object_Object *restrict obj,
                                              const char *restrict fieldname,
                                              char **restrict textp,
                                              int *isnewp)
      __nonnull ((1, 2, 3));
  EXPORT bool dwg_set_ENTITY_common (Dwg_Object_Entity *restrict obj,
                                     const char *restrict fieldname,
                                     const void *restrict value)
      __nonnull ((1, 2, 3));
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
                                  const BITCODE_H handle)
      __nonnull ((1, 2, 3));

  EXPORT BITCODE_B dwg_ent_get_BOOL (const void *restrict _obj,
                                     const char *restrict fieldname)
      __nonnull ((1, 2));
  EXPORT bool dwg_ent_set_BOOL (void *restrict _obj,
                                const char *restrict fieldname,
                                const BITCODE_B num) __nonnull ((1, 2));

#ifndef SWIGIMPORTED

#  ifdef USE_DEPRECATED_API

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
  EXPORT double
  dwg_ent_circle_get_radius (const dwg_ent_circle *restrict circle,
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

  EXPORT void dwg_ent_circle_set_extrusion (
      dwg_ent_circle *restrict circle, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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
  EXPORT void dwg_ent_ellipse_get_center (
      const dwg_ent_ellipse *restrict ellipse, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_ellipse_set_center (dwg_ent_ellipse *restrict ellipse,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  // Get/Set sm axis of ellipse
  EXPORT void dwg_ent_ellipse_get_sm_axis (
      const dwg_ent_ellipse *restrict ellipse, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_ellipse_set_sm_axis (dwg_ent_ellipse *restrict ellipse,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  // Get/Set extrusion of ellipse
  EXPORT void dwg_ent_ellipse_get_extrusion (
      const dwg_ent_ellipse *restrict ellipse, dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_ellipse_set_extrusion (
      dwg_ent_ellipse *restrict ellipse, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  // Get/Set axis ratio of ellipse
  EXPORT double
  dwg_ent_ellipse_get_axis_ratio (const dwg_ent_ellipse *restrict ellipse,
                                  int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void
  dwg_ent_ellipse_set_axis_ratio (dwg_ent_ellipse *restrict ellipse,
                                  const double ratio, int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  // Get/Set start angle of ellipse
  EXPORT double
  dwg_ent_ellipse_get_start_angle (const dwg_ent_ellipse *restrict ellipse,
                                   int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_ellipse_set_start_angle (
      dwg_ent_ellipse *restrict ellipse, const double start_angle,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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
  EXPORT void dwg_ent_text_get_insertion_pt (const dwg_ent_text *restrict text,
                                             dwg_point_2d *restrict point,
                                             int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_text_set_insertion_pt (
      dwg_ent_text *restrict text, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  // Get/Set height of text
  EXPORT double dwg_ent_text_get_height (const dwg_ent_text *restrict text,
                                         int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_text_set_height (dwg_ent_text *restrict text,
                                       const double height,
                                       int *restrict error)
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
                                         const double angle,
                                         int *restrict error)
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
  EXPORT void dwg_ent_attrib_get_insertion_pt (
      const dwg_ent_attrib *restrict attrib, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_attrib_set_insertion_pt (
      dwg_ent_attrib *restrict attrib, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  // Get/Set height of attrib
  EXPORT double
  dwg_ent_attrib_get_height (const dwg_ent_attrib *restrict attrib,
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

  EXPORT void dwg_ent_attrib_set_extrusion (
      dwg_ent_attrib *restrict attrib, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_attrib_set_vert_alignment (
      dwg_ent_attrib *restrict attrib, const BITCODE_BS alignment,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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
  EXPORT void dwg_ent_attdef_get_insertion_pt (
      const dwg_ent_attdef *restrict attdef, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_attdef_set_insertion_pt (
      dwg_ent_attdef *restrict attdef, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  // Get/Set height of attdef
  EXPORT double
  dwg_ent_attdef_get_height (const dwg_ent_attdef *restrict attdef,
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

  EXPORT void dwg_ent_attdef_set_extrusion (
      dwg_ent_attdef *restrict attdef, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_attdef_set_vert_alignment (
      dwg_ent_attdef *restrict attdef, const BITCODE_BS alignment,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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
  EXPORT double
  dwg_ent_point_get_thickness (const dwg_ent_point *restrict point,
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
  EXPORT double
  dwg_ent_solid_get_thickness (const dwg_ent_solid *restrict solid,
                               int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_solid_set_thickness (dwg_ent_solid *restrict solid,
                                           const double thickness,
                                           int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  // Get/Set elevation of solid entity
  EXPORT double
  dwg_ent_solid_get_elevation (const dwg_ent_solid *restrict solid,
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
  EXPORT double
  dwg_ent_trace_get_thickness (const dwg_ent_trace *restrict trace,
                               int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_trace_set_thickness (dwg_ent_trace *restrict trace,
                                           const double thickness,
                                           int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  // Get/Set elevation of trace entity
  EXPORT double
  dwg_ent_trace_get_elevation (const dwg_ent_trace *restrict trace,
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
  EXPORT char
  dwg_ent_vertex_3d_get_flag (const dwg_ent_vertex_3d *restrict vert,
                              int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_vertex_3d_set_flag (dwg_ent_vertex_3d *restrict vert,
                                          const char flags,
                                          int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  // Get/Set point of vertex_3d entity
  EXPORT void dwg_ent_vertex_3d_get_point (
      const dwg_ent_vertex_3d *restrict vert, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

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

  EXPORT void dwg_ent_vertex_mesh_set_point (
      dwg_ent_vertex_mesh *restrict vert, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  /********************************************************************
   *               FUNCTIONS FOR VERTEX_PFACE ENTITY                   *
   ********************************************************************/

  // Get/Set flags of vertex_pface entity
  EXPORT char
  dwg_ent_vertex_pface_get_flag (const dwg_ent_vertex_pface *restrict vert,
                                 int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void
  dwg_ent_vertex_pface_set_flag (dwg_ent_vertex_pface *restrict vert,
                                 const char flags, int *restrict error)
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
  EXPORT char
  dwg_ent_vertex_2d_get_flag (const dwg_ent_vertex_2d *restrict vert,
                              int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_vertex_2d_set_flag (dwg_ent_vertex_2d *restrict vert,
                                          const char flags,
                                          int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  // Get/Set point of vertex_2d entity
  EXPORT void dwg_ent_vertex_2d_get_point (
      const dwg_ent_vertex_2d *restrict vert, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

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

  EXPORT void
  dwg_ent_vertex_2d_set_end_width (dwg_ent_vertex_2d *restrict vert,
                                   const double end_width, int *restrict error)
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

  EXPORT void dwg_ent_insert_set_extrusion (
      dwg_ent_insert *restrict insert, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_minsert_get_ins_pt (
      const dwg_ent_minsert *restrict minsert, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_minsert_set_ins_pt (dwg_ent_minsert *restrict minsert,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void
  dwg_ent_minsert_get_scale (const dwg_ent_minsert *restrict minsert,
                             dwg_point_3d *restrict point, int *restrict error)
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

  EXPORT void dwg_ent_minsert_set_extrusion (
      dwg_ent_minsert *restrict minsert, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void
  dwg_ent_minsert_set_col_spacing (dwg_ent_minsert *restrict minsert,
                                   const double spacing, int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT double
  dwg_ent_minsert_get_row_spacing (const dwg_ent_minsert *restrict minsert,
                                   int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void
  dwg_ent_minsert_set_row_spacing (dwg_ent_minsert *restrict minsert,
                                   const double spacing, int *restrict error)
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

  EXPORT void dwg_obj_mlinestyle_set_start_angle (
      dwg_obj_mlinestyle *restrict mlinestyle, const double startang,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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

  EXPORT BITCODE_RC dwg_obj_appid_get_flag (
      const dwg_obj_appid *restrict appid, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_obj_appid_set_flag (dwg_obj_appid *restrict appid,
                                      const BITCODE_RC flag,
                                      int *restrict error)
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
  EXPORT void dwg_ent_dim_set_act_measurement (
      dwg_ent_dim *restrict dim, const BITCODE_BD act_measurement,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
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
  EXPORT BITCODE_BS dwg_ent_dim_get_attachment (
      const dwg_ent_dim *restrict dim, int *restrict error)
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

  EXPORT void
  dwg_ent_dim_ordinate_set_flag2 (dwg_ent_dim_ordinate *restrict ord,
                                  const char flag, int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT char
  dwg_ent_dim_ordinate_get_flag2 (const dwg_ent_dim_ordinate *restrict ord,
                                  int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  /********************************************************************
   *              FUNCTIONS FOR LINEAR DIMENSION ENTITY                *
   ********************************************************************/

  EXPORT void dwg_ent_dim_linear_set_def_pt (
      dwg_ent_dim_linear *restrict dim, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_dim_aligned_set_def_pt (
      dwg_ent_dim_aligned *restrict dim, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_dim_aligned_get_def_pt (
      const dwg_ent_dim_aligned *restrict dim, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_dim_aligned_set_13_pt (
      dwg_ent_dim_aligned *restrict dim, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_dim_aligned_get_13_pt (
      const dwg_ent_dim_aligned *restrict dim, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_dim_aligned_set_14_pt (
      dwg_ent_dim_aligned *restrict dim, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_dim_ang3pt_set_def_pt (
      dwg_ent_dim_ang3pt *restrict ang, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_dim_ang2ln_set_def_pt (
      dwg_ent_dim_ang2ln *restrict ang, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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
                                            const dwg_point_3d *restrict point,
                                            int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_dim_ang2ln_get_16_pt (
      const dwg_ent_dim_ang2ln *restrict ang, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  /********************************************************************
   *              FUNCTIONS FOR RADIUS DIMENSION ENTITY                *
   ********************************************************************/

  EXPORT void dwg_ent_dim_radius_set_def_pt (
      dwg_ent_dim_radius *restrict radius, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_dim_radius_set_leader_length (
      dwg_ent_dim_radius *restrict radius, const double length,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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

  EXPORT double
  dwg_ent_shape_get_rotation (const dwg_ent_shape *restrict shape,
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

  EXPORT double
  dwg_ent_shape_get_thickness (const dwg_ent_shape *restrict shape,
                               int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_shape_set_thickness (dwg_ent_shape *restrict shape,
                                           const double thickness,
                                           int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT BITCODE_BS dwg_ent_shape_get_shape_no (
      const dwg_ent_shape *restrict shape, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_shape_set_shape_no (dwg_ent_shape *restrict shape,
                                          const BITCODE_BS id,
                                          int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT BITCODE_BS dwg_ent_shape_get_style_id (
      const dwg_ent_shape *restrict shape, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_shape_set_style_id (dwg_ent_shape *restrict shape,
                                          const BITCODE_BS id,
                                          int *restrict error)
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

  EXPORT void dwg_ent_mtext_set_insertion_pt (
      dwg_ent_mtext *restrict mtext, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_mtext_get_x_axis_dir (
      const dwg_ent_mtext *restrict mtext, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

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

  EXPORT void dwg_ent_mtext_set_text (dwg_ent_mtext *restrict mtext,
                                      char *text, int *restrict error)
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

  EXPORT void
  dwg_ent_mtext_set_linespace_factor (dwg_ent_mtext *restrict mtext,
                                      const double factor, int *restrict error)
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

  EXPORT void dwg_ent_leader_set_extrusion (
      dwg_ent_leader *restrict leader, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_leader_get_extrusion (
      const dwg_ent_leader *restrict leader, dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_leader_set_x_direction (
      dwg_ent_leader *restrict leader, const dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_leader_get_x_direction (
      const dwg_ent_leader *restrict leader, dwg_point_3d *restrict vector,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_leader_set_inspt_offset (
      dwg_ent_leader *restrict leader, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_leader_get_inspt_offset (
      const dwg_ent_leader *restrict leader, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_leader_set_dimgap (dwg_ent_leader *restrict leader,
                                         const double dimgap,
                                         int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT double
  dwg_ent_leader_get_dimgap (const dwg_ent_leader *restrict leader,
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

  EXPORT void dwg_ent_leader_set_arrowhead_type (
      dwg_ent_leader *restrict leader, const BITCODE_BS type,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT BITCODE_BS dwg_ent_leader_get_arrowhead_type (
      const dwg_ent_leader *restrict leader, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_leader_set_dimasz (dwg_ent_leader *restrict leader,
                                         const double dimasz,
                                         int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT double
  dwg_ent_leader_get_dimasz (const dwg_ent_leader *restrict leader,
                             int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_leader_set_byblock_color (
      dwg_ent_leader *restrict leader, const BITCODE_BS color,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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
  EXPORT void
  dwg_ent_tolerance_set_text_string (dwg_ent_tolerance *restrict tol,
                                     const char *string, int *restrict error)
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

  EXPORT void dwg_ent_lwpline_set_const_width (
      dwg_ent_lwpline *restrict lwpline, const double const_width,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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

  EXPORT BITCODE_BL dwg_ent_lwpline_get_numbulges (
      const dwg_ent_lwpline *restrict lwpline, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT BITCODE_BL dwg_ent_lwpline_get_numwidths (
      const dwg_ent_lwpline *restrict lwpline, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  /********************************************************************
   *                  FUNCTIONS FOR OLE2FRAME ENTITY                   *
   ********************************************************************/

  EXPORT BITCODE_BS dwg_ent_ole2frame_get_type (
      const dwg_ent_ole2frame *restrict frame, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_ole2frame_set_type (dwg_ent_ole2frame *restrict frame,
                                          const BITCODE_BS type,
                                          int *restrict error)
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
#    define dwg_ent_ole2frame_get_data_length(a, b)                           \
      dwg_ent_ole2frame_get_data_size (a, b)

  EXPORT BITCODE_TF dwg_ent_ole2frame_get_data (
      const dwg_ent_ole2frame *restrict frame, int *restrict error)
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

  EXPORT BITCODE_RC *
  dwg_obj_proxy_get_data (const dwg_obj_proxy *restrict proxy,
                          int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_obj_proxy_set_data (dwg_obj_proxy *restrict proxy,
                                      const BITCODE_RC *data,
                                      int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT dwg_object_ref **
  dwg_obj_proxy_get_objids (const dwg_obj_proxy *restrict proxy,
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
                                          const int fit_tol,
                                          int *restrict error)
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

  EXPORT char
  dwg_ent_spline_get_rational (const dwg_ent_spline *restrict spline,
                               int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_spline_set_rational (dwg_ent_spline *restrict spline,
                                           const char rational,
                                           int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT char
  dwg_ent_spline_get_closed_b (const dwg_ent_spline *restrict spline,
                               int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_spline_set_closed_b (dwg_ent_spline *restrict spline,
                                           const char closed_b,
                                           int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT char
  dwg_ent_spline_get_weighted (const dwg_ent_spline *restrict spline,
                               int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_spline_set_weighted (dwg_ent_spline *restrict spline,
                                           char weighted, int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT char
  dwg_ent_spline_get_periodic (const dwg_ent_spline *restrict spline,
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

  EXPORT double *
  dwg_ent_spline_get_knots (const dwg_ent_spline *restrict spline,
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

  EXPORT double
  dwg_ent_viewport_get_width (const dwg_ent_viewport *restrict vp,
                              int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_width (dwg_ent_viewport *restrict vp,
                                          const double width,
                                          int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT double
  dwg_ent_viewport_get_height (const dwg_ent_viewport *restrict vp,
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
      dwg_ent_viewport *restrict vp, const BITCODE_BL count,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT char *
  dwg_ent_viewport_get_style_sheet (const dwg_ent_viewport *restrict vp,
                                    int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_style_sheet (dwg_ent_viewport *restrict vp,
                                                char *sheet,
                                                int *restrict error)
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

  EXPORT void dwg_ent_viewport_set_ucs_at_origin (
      dwg_ent_viewport *restrict vp, unsigned char origin, int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT unsigned char
  dwg_ent_viewport_get_ucs_at_origin (const dwg_ent_viewport *restrict vp,
                                      int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_UCSVP (dwg_ent_viewport *restrict vp,
                                          unsigned char viewport,
                                          int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT unsigned char
  dwg_ent_viewport_get_UCSVP (const dwg_ent_viewport *restrict vp,
                              int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_view_target (
      dwg_ent_viewport *restrict vp, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;
  EXPORT void dwg_ent_viewport_get_view_target (
      const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_VIEWDIR (dwg_ent_viewport *restrict vp,
                                            const dwg_point_3d *restrict point,
                                            int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;
  EXPORT void dwg_ent_viewport_get_VIEWDIR (
      const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_twist_angle (dwg_ent_viewport *restrict vp,
                                                const double angle,
                                                int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT double
  dwg_ent_viewport_get_twist_angle (const dwg_ent_viewport *restrict vp,
                                    int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_VIEWSIZE (dwg_ent_viewport *restrict vp,
                                             const double height,
                                             int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT double
  dwg_ent_viewport_get_VIEWSIZE (const dwg_ent_viewport *restrict vp,
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

  EXPORT void dwg_ent_viewport_set_SNAPANG (dwg_ent_viewport *restrict vp,
                                            const double angle,
                                            int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT double
  dwg_ent_viewport_get_SNAPANG (const dwg_ent_viewport *restrict vp,
                                int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_get_VIEWCTR (
      const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_VIEWCTR (dwg_ent_viewport *restrict vp,
                                            const dwg_point_2d *restrict point,
                                            int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_viewport_get_GRIDUNIT (
      const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_GRIDUNIT (
      dwg_ent_viewport *restrict vp, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_viewport_get_SNAPBASE (
      const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_SNAPBASE (
      dwg_ent_viewport *restrict vp, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_viewport_get_SNAPUNIT (
      const dwg_ent_viewport *restrict vp, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;
  EXPORT void dwg_ent_viewport_set_SNAPUNIT (
      dwg_ent_viewport *restrict vp, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_viewport_set_ucsorg (dwg_ent_viewport *restrict vp,
                                           const dwg_point_3d *restrict point,
                                           int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_viewport_get_ucsorg (const dwg_ent_viewport *restrict vp,
                                           dwg_point_3d *restrict point,
                                           int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_ucsxdir (dwg_ent_viewport *restrict vp,
                                            const dwg_point_3d *restrict point,
                                            int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_viewport_get_ucsxdir (
      const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_ucsydir (dwg_ent_viewport *restrict vp,
                                            const dwg_point_3d *restrict point,
                                            int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;
  EXPORT void dwg_ent_viewport_get_ucsydir (
      const dwg_ent_viewport *restrict vp, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_ucs_elevation (
      dwg_ent_viewport *restrict vp, const double elevation,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT double
  dwg_ent_viewport_get_ucs_elevation (const dwg_ent_viewport *restrict vp,
                                      int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_UCSORTHOVIEW (dwg_ent_viewport *restrict vp,
                                                 const BITCODE_BS type,
                                                 int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT BITCODE_BS dwg_ent_viewport_get_UCSORTHOVIEW (
      const dwg_ent_viewport *restrict vp, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_viewport_set_shadeplot_mode (
      dwg_ent_viewport *restrict vp, const BITCODE_BS shadeplot,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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
      const dwg_ent_polyline_2d *restrict pline2d,
      dwg_point_3d *restrict vector, int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void
  dwg_ent_polyline_2d_set_extrusion (dwg_ent_polyline_2d *restrict pline2d,
                                     const dwg_point_3d *restrict vector,
                                     int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT double dwg_ent_polyline_2d_get_start_width (
      const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;
  EXPORT void dwg_ent_polyline_2d_set_start_width (
      dwg_ent_polyline_2d *restrict pline2d, const double start_width,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT double dwg_ent_polyline_2d_get_end_width (
      const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_polyline_2d_set_end_width (
      dwg_ent_polyline_2d *restrict pline2d, const double end_width,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT double dwg_ent_polyline_2d_get_thickness (
      const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_polyline_2d_set_thickness (
      dwg_ent_polyline_2d *restrict pline2d, const double thickness,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT double dwg_ent_polyline_2d_get_elevation (
      const dwg_ent_polyline_2d *restrict pline2d, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_polyline_2d_set_elevation (
      dwg_ent_polyline_2d *restrict pline2d, const double elevation,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_3dface_get_corner1 (
      const dwg_ent_3dface *restrict _3dface, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_3dface_set_corner1 (dwg_ent_3dface *restrict _3dface,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_3dface_get_corner2 (
      const dwg_ent_3dface *restrict _3dface, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_3dface_set_corner2 (dwg_ent_3dface *restrict _3dface,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_3dface_get_corner3 (
      const dwg_ent_3dface *restrict _3dface, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_3dface_set_corner3 (dwg_ent_3dface *restrict _3dface,
                                          const dwg_point_3d *restrict point,
                                          int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_3dface_get_corner4 (
      const dwg_ent_3dface *restrict _3dface, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

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

  EXPORT char
  dwg_ent_image_get_brightness (const dwg_ent_image *restrict image,
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
      dwg_ent_image *restrict image, const BITCODE_BS type,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_image_get_boundary_pt0 (
      const dwg_ent_image *restrict image, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_image_set_boundary_pt0 (
      dwg_ent_image *restrict image, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_image_get_boundary_pt1 (
      const dwg_ent_image *restrict image, dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_image_set_boundary_pt1 (
      dwg_ent_image *restrict image, const dwg_point_2d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_mline_get_base_point (
      const dwg_ent_mline *restrict mline, dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_mline_set_extrusion (dwg_ent_mline *restrict mline,
                                           const dwg_point_3d *restrict vector,
                                           int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_mline_get_extrusion (const dwg_ent_mline *restrict mline,
                                           dwg_point_3d *restrict vector,
                                           int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_mline_set_flags (dwg_ent_mline *restrict mline,
                                       const BITCODE_BS oc,
                                       int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT BITCODE_BS dwg_ent_mline_get_flags (
      const dwg_ent_mline *restrict mline, int *restrict error)
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

  EXPORT void dwg_ent_3dsolid_set_acis_empty (
      dwg_ent_3dsolid *restrict _3dsolid, const unsigned char acis,
      int *restrict error) __nonnull ((3)) /*_deprecated_dynapi_setter*/;

  EXPORT BITCODE_BS dwg_ent_3dsolid_get_version (
      const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
      __nonnull ((2)) /*_deprecated_dynapi_getter*/;

  EXPORT unsigned char *
  dwg_ent_3dsolid_get_acis_data (const dwg_ent_3dsolid *restrict _3dsolid,
                                 int *restrict error)
      __nonnull ((2)) /*_deprecated_dynapi_getter*/;

  EXPORT void
  dwg_ent_3dsolid_set_acis_data (dwg_ent_3dsolid *restrict _3dsolid,
                                 const unsigned char *restrict sat_data,
                                 int *restrict error)
      __nonnull ((2, 3)) /*_deprecated_dynapi_setter*/;

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

  EXPORT BITCODE_BL dwg_ent_3dsolid_get_isolines (
      const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
      __nonnull ((2)) /*_deprecated_dynapi_getter*/;

  EXPORT void dwg_ent_3dsolid_set_isolines (dwg_ent_3dsolid *restrict _3dsolid,
                                            const BITCODE_BL num,
                                            int *restrict error)
      __nonnull ((3)) /*_deprecated_dynapi_setter*/;

  EXPORT char dwg_ent_3dsolid_get_isoline_present (
      const dwg_ent_3dsolid *restrict _3dsolid, int *restrict error)
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

  EXPORT void
  dwg_ent_region_set_point_present (dwg_ent_region *restrict region,
                                    const char present, int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;

  EXPORT void dwg_ent_region_get_point (const dwg_ent_region *restrict region,
                                        dwg_point_3d *restrict point,
                                        int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_region_set_point (dwg_ent_region *restrict region,
                                        const dwg_point_3d *restrict point,
                                        int *restrict error)
      __nonnull ((2, 3)) _deprecated_dynapi_setter;

  EXPORT BITCODE_BL dwg_ent_region_get_isolines (
      const dwg_ent_region *restrict region, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_region_set_isolines (dwg_ent_region *restrict region,
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

  EXPORT BITCODE_BS dwg_ent_body_get_version (
      const dwg_ent_body *restrict body, int *restrict error)
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

  EXPORT char
  dwg_ent_body_get_point_present (const dwg_ent_body *restrict body,
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

  EXPORT BITCODE_BL dwg_ent_body_get_isolines (
      const dwg_ent_body *restrict body, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_body_set_isolines (dwg_ent_body *restrict body,
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
  dwg_ent_body_get_wires (const dwg_ent_body *restrict body,
                          int *restrict error)
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

  EXPORT void dwg_ent_table_set_insertion_pt (
      dwg_ent_table *restrict table, const dwg_point_3d *restrict point,
      int *restrict error) __nonnull ((2, 3)) _deprecated_dynapi_setter;
  EXPORT void dwg_ent_table_get_insertion_pt (
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

  EXPORT void dwg_ent_table_set_rotation (dwg_ent_table *restrict table,
                                          const BITCODE_BD rotation,
                                          int *restrict error)
      __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT double
  dwg_ent_table_get_rotation (const dwg_ent_table *restrict table,
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

  EXPORT unsigned char
  dwg_ent_table_has_attribs (dwg_ent_table *restrict table,
                             int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT BITCODE_BL dwg_ent_table_get_num_owned (
      const dwg_ent_table *restrict table, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  // TODO dwg_ent_*_add_owned, dwg_ent_insert_delete_owned

  EXPORT void dwg_ent_table_set_flag_for_table_value (
      dwg_ent_table *restrict table, const BITCODE_BS value,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;

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

  EXPORT void dwg_ent_table_set_title_suppressed (
      dwg_ent_table *restrict table, const unsigned char title,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT unsigned char
  dwg_ent_table_get_title_suppressed (const dwg_ent_table *restrict table,
                                      int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_table_set_header_suppressed (
      dwg_ent_table *restrict table, const unsigned char header,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
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

  EXPORT void dwg_ent_table_set_horiz_cell_margin (
      dwg_ent_table *restrict table, const BITCODE_BD margin,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT double
  dwg_ent_table_get_horiz_cell_margin (const dwg_ent_table *restrict table,
                                       int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_table_set_vert_cell_margin (
      dwg_ent_table *restrict table, const BITCODE_BD margin,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
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
      dwg_ent_table *restrict table, const BITCODE_BS align,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT BITCODE_BS dwg_ent_table_get_header_row_alignment (
      const dwg_ent_table *restrict table, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_table_set_data_row_alignment (
      dwg_ent_table *restrict table, const BITCODE_BS align,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT BITCODE_BS dwg_ent_table_get_data_row_alignment (
      const dwg_ent_table *restrict table, int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_table_set_title_row_height (
      dwg_ent_table *restrict table, const BITCODE_BD height,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
  EXPORT double
  dwg_ent_table_get_title_row_height (const dwg_ent_table *restrict table,
                                      int *restrict error)
      __nonnull ((2)) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_table_set_header_row_height (
      dwg_ent_table *restrict table, const BITCODE_BD height,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
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
      dwg_ent_table *restrict table, BITCODE_BS visibility,
      int *restrict error) __nonnull ((3)) _deprecated_dynapi_setter;
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
  EXPORT BITCODE_BSd dwg_ent_vertex_pface_face_get_vertind (
      const dwg_ent_vert_pface_face *face) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_vertex_pface_face_set_vertind (
      dwg_ent_vert_pface_face *restrict face, const BITCODE_BSd vertind[4])
      __nonnull ((2)) _deprecated_dynapi_setter;

  /********************************************************************
   *                    FUNCTIONS FOR XRECORD OBJECT                     *
   ********************************************************************/

#    define dwg_obj_xrecord_get_num_databytes(a, b)                           \
      dwg_obj_xrecord_get_xdata_size (a, b)
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

#  endif /* USE_DEPRECATED_API */

  /* ************************************************************** */

  EXPORT BITCODE_BL dwg_object_polyline_2d_get_numpoints (
      const dwg_object *restrict obj, int *restrict error) __nonnull ((2));

  EXPORT dwg_point_2d *
  dwg_object_polyline_2d_get_points (const dwg_object *restrict obj,
                                     int *restrict error) __nonnull ((2));

  EXPORT BITCODE_BL dwg_object_polyline_3d_get_numpoints (
      const dwg_object *restrict obj, int *restrict error) __nonnull ((2));

  EXPORT dwg_point_3d *
  dwg_object_polyline_3d_get_points (const dwg_object *restrict obj,
                                     int *restrict error) __nonnull ((2));

  EXPORT double *
  dwg_ent_lwpline_get_bulges (const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error) __nonnull ((2));

  EXPORT BITCODE_BL dwg_ent_lwpline_get_numpoints (
      const dwg_ent_lwpline *restrict lwpline, int *restrict error)
      __nonnull ((2));

  EXPORT dwg_point_2d *
  dwg_ent_lwpline_get_points (const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error) __nonnull ((2));

  EXPORT int dwg_ent_lwpline_set_points (
      dwg_ent_lwpline *restrict lwpline, const BITCODE_BL num_pts2d,
      const dwg_point_2d *restrict pts2d) __nonnull_all;

  EXPORT dwg_lwpline_widths *
  dwg_ent_lwpline_get_widths (const dwg_ent_lwpline *restrict lwpline,
                              int *restrict error) __nonnull_all;

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

  EXPORT dwg_obj_block_header *
  dwg_get_block_header (dwg_data *restrict dwg,
                        int *restrict error) __nonnull_all;

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
                                      int *restrict error) __nonnull_all;

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
                                       int *restrict error) __nonnull_all;
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
                                       int *restrict error) __nonnull_all;

  // Get name of the referenced table entry. Defaults to ByLayer
  EXPORT char *dwg_ref_get_table_name (const dwg_object_ref *restrict ref,
                                       int *restrict error) __nonnull_all;

  // Get number of table entries from the table.
  EXPORT BITCODE_BL dwg_object_tablectrl_get_num_entries (
      const dwg_object *restrict obj, int *restrict error) __nonnull_all;

  // Get the nth table entry from the table.
  EXPORT dwg_object_ref *
  dwg_object_tablectrl_get_entry (const dwg_object *restrict obj,
                                  const BITCODE_BS index, int *restrict error)
      __nonnull ((3));

  // Get all table entries from the table.
  EXPORT dwg_object_ref **
  dwg_object_tablectrl_get_entries (const dwg_object *restrict obj,
                                    int *restrict error) __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_object_tablectrl_get_ownerhandle (const dwg_object *restrict obj,
                                        int *restrict error) __nonnull_all;
  EXPORT dwg_object_ref *
  dwg_object_tablectrl_get_xdicobjhandle (const dwg_object *restrict obj,
                                          int *restrict error) __nonnull_all;
  EXPORT BITCODE_BL dwg_object_tablectrl_get_objid (
      const dwg_object *restrict obj, int *restrict error) __nonnull_all;

  /********************************************************************
   *                    COMMON FUNCTIONS FOR DWG ENTITY                *
   ********************************************************************/

  EXPORT dwg_object *dwg_ent_to_object (const dwg_obj_ent *restrict obj,
                                        int *restrict error) __nonnull_all;

  EXPORT dwg_object *
  dwg_ent_generic_to_object (const void *restrict obj,
                             int *restrict error) __nonnull_all;
  EXPORT dwg_obj_ent *
  dwg_ent_generic_parent (const void *restrict ent,
                          int *restrict error) __nonnull_all;

  EXPORT BITCODE_RL dwg_ent_get_bitsize (const dwg_obj_ent *restrict ent,
                                         int *restrict error) __nonnull_all;

  EXPORT BITCODE_BL dwg_ent_get_num_eed (const dwg_obj_ent *restrict ent,
                                         int *restrict error) __nonnull ((2));

  EXPORT dwg_entity_eed *dwg_ent_get_eed (const dwg_obj_ent *restrict ent,
                                          BITCODE_BL index,
                                          int *restrict error) __nonnull ((3));

  EXPORT dwg_entity_eed_data *
  dwg_ent_get_eed_data (const dwg_obj_ent *restrict ent, BITCODE_BL index,
                        int *restrict error) __nonnull ((3));

  EXPORT BITCODE_B dwg_ent_get_picture_exists (
      const dwg_obj_ent *restrict ent, int *restrict error) __nonnull_all;

  EXPORT BITCODE_BLL
  dwg_ent_get_picture_size (const dwg_obj_ent *restrict ent,
                            int *restrict error) // before r2007 only RL
      __nonnull_all;

  EXPORT BITCODE_TF dwg_ent_get_picture (const dwg_obj_ent *restrict ent,
                                         int *restrict error) __nonnull_all;

  EXPORT BITCODE_BB dwg_ent_get_entmode (const dwg_obj_ent *restrict ent,
                                         int *restrict error) __nonnull_all;

  EXPORT BITCODE_BL dwg_ent_get_num_reactors (
      const dwg_obj_ent *restrict ent, int *restrict error) __nonnull_all;

  EXPORT BITCODE_B
  dwg_ent_get_is_xdic_missing (const dwg_obj_ent *restrict ent,
                               int *restrict error) // r2004+
      __nonnull_all;

  EXPORT char *dwg_ent_get_layer_name (const dwg_obj_ent *restrict ent,
                                       int *restrict error) __nonnull_all;

  EXPORT char *dwg_ent_get_ltype_name (const dwg_obj_ent *restrict ent,
                                       int *restrict error) __nonnull_all;

  EXPORT BITCODE_B
  dwg_ent_get_isbylayerlt (const dwg_obj_ent *restrict ent,
                           int *restrict error) // r13-r14 only
      __nonnull_all;

  EXPORT BITCODE_B dwg_ent_get_nolinks (const dwg_obj_ent *restrict ent,
                                        int *restrict error) __nonnull_all;

  EXPORT const Dwg_Color *dwg_ent_get_color (const dwg_obj_ent *restrict ent,
                                             int *restrict error)
      __nonnull ((2));

  EXPORT double dwg_ent_get_linetype_scale (const dwg_obj_ent *restrict ent,
                                            int *restrict error) __nonnull_all;

  EXPORT BITCODE_BB
  dwg_ent_get_linetype_flags (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2000+
      __nonnull_all;

  EXPORT BITCODE_BB
  dwg_ent_get_plotstyle_flags (const dwg_obj_ent *restrict ent,
                               int *restrict error) // r2000+
      __nonnull_all;

  EXPORT BITCODE_BB
  dwg_ent_get_material_flags (const dwg_obj_ent *restrict ent,
                              int *restrict error) // r2007+
      __nonnull_all;

  EXPORT BITCODE_RC dwg_ent_get_shadow_flags (const dwg_obj_ent *restrict ent,
                                              int *restrict error) // r2007+
      __nonnull_all;

  EXPORT BITCODE_B dwg_ent_has_full_visualstyle (dwg_obj_ent *restrict ent,
                                                 int *restrict error) // r2010+
      __nonnull_all;

  EXPORT BITCODE_B dwg_ent_has_face_visualstyle (dwg_obj_ent *restrict ent,
                                                 int *restrict error) // r2010+
      __nonnull_all;

  EXPORT BITCODE_B dwg_ent_has_edge_visualstyle (dwg_obj_ent *restrict ent,
                                                 int *restrict error) // r2010+
      __nonnull_all;

  EXPORT BITCODE_BS dwg_ent_get_invisible (const dwg_obj_ent *restrict ent,
                                           int *restrict error) __nonnull_all;

  /* See dxf_cvt_lweight() for the mm value */
  EXPORT BITCODE_RC dwg_ent_get_linewt (const dwg_obj_ent *restrict ent,
                                        int *restrict error) // r2000+
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_ownerhandle (const dwg_obj_ent *restrict ent,
                           int *restrict error) __nonnull_all;

  EXPORT dwg_object_ref **
  dwg_ent_get_reactors (const dwg_obj_ent *restrict ent,
                        int *restrict error) __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_xdicobjhandle (const dwg_obj_ent *restrict ent,
                             int *restrict error) __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_prev_entity (const dwg_obj_ent *restrict ent,
                           int *restrict error) // r13-r2000
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_next_entity (const dwg_obj_ent *restrict ent,
                           int *restrict error) // r13-r2000
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_color_handle (const dwg_obj_ent *restrict ent,
                            int *restrict error) // r2004+
      __nonnull_all;

  EXPORT dwg_object_ref *dwg_ent_get_layer (const dwg_obj_ent *restrict ent,
                                            int *restrict error) __nonnull_all;

  EXPORT dwg_object_ref *dwg_ent_get_ltype (const dwg_obj_ent *restrict ent,
                                            int *restrict error) __nonnull_all;

  EXPORT dwg_object_ref *dwg_ent_get_material (const dwg_obj_ent *restrict ent,
                                               int *restrict error) // r2007+
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_plotstyle (const dwg_obj_ent *restrict ent,
                         int *restrict error) // r2000+
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_full_visualstyle (const dwg_obj_ent *restrict ent,
                                int *restrict error) // r2010+
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_face_visualstyle (const dwg_obj_ent *restrict ent,
                                int *restrict error) // r2010+
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_edge_visualstyle (const dwg_obj_ent *restrict ent,
                                int *restrict error) // r2010+
      __nonnull_all;

  EXPORT int dwg_ent_set_ltype (dwg_obj_ent *restrict ent,
                                const char *restrict name) __nonnull_all;

  /********************************************************************
   *                    FUNCTIONS FOR DWG OBJECT                       *
   ********************************************************************/

  EXPORT dwg_object *dwg_obj_obj_to_object (const dwg_obj_obj *restrict obj,
                                            int *restrict error) __nonnull_all;

  EXPORT BITCODE_BL dwg_obj_get_objid (const dwg_obj_obj *restrict obj,
                                       int *restrict error) __nonnull_all;

  EXPORT BITCODE_BL dwg_obj_get_num_eed (const dwg_obj_obj *restrict obj,
                                         int *restrict error) __nonnull_all;
  EXPORT dwg_entity_eed *dwg_obj_get_eed (const dwg_obj_obj *restrict obj,
                                          const BITCODE_BL index,
                                          int *restrict error) __nonnull ((3));
  EXPORT dwg_entity_eed_data *
  dwg_obj_get_eed_data (const dwg_obj_obj *restrict obj,
                        const BITCODE_BL index, int *restrict error)
      __nonnull ((3));

  EXPORT BITCODE_H dwg_obj_get_ownerhandle (const dwg_obj_obj *restrict obj,
                                            int *restrict error) __nonnull_all;
  EXPORT BITCODE_BL dwg_obj_get_num_reactors (
      const dwg_obj_obj *restrict obj, int *restrict error) __nonnull_all;
  EXPORT BITCODE_H *dwg_obj_get_reactors (const dwg_obj_obj *restrict obj,
                                          int *restrict error) __nonnull_all;
  EXPORT BITCODE_H dwg_obj_get_xdicobjhandle (
      const dwg_obj_obj *restrict obj, int *restrict error) __nonnull_all;
  /* r2004+ */
  EXPORT BITCODE_B dwg_obj_get_is_xdic_missing (
      const dwg_obj_obj *restrict obj, int *restrict error) __nonnull_all;
  /* r2013+ */
  EXPORT BITCODE_B dwg_obj_get_has_ds_binary_data (
      const dwg_obj_obj *restrict obj, int *restrict error) __nonnull_all;
  EXPORT Dwg_Handle *dwg_obj_get_handleref (const dwg_obj_obj *restrict obj,
                                            int *restrict error) __nonnull_all;

  EXPORT dwg_object *
  dwg_obj_generic_to_object (const void *restrict obj,
                             int *restrict error) __nonnull_all;
  EXPORT BITCODE_RLL dwg_obj_generic_handlevalue (void *_obj) __nonnull_all;
  Dwg_Data *dwg_obj_generic_dwg (const void *restrict obj,
                                 int *restrict error) __nonnull_all;
  EXPORT dwg_obj_obj *
  dwg_obj_generic_parent (const void *restrict obj,
                          int *restrict error) __nonnull_all;

  EXPORT dwg_object *dwg_get_object (dwg_data *dwg, BITCODE_BL index);

  EXPORT BITCODE_RL dwg_object_get_bitsize (const dwg_object *obj);

  EXPORT BITCODE_BL dwg_object_get_index (const dwg_object *restrict obj,
                                          int *restrict error) __nonnull_all;

  EXPORT dwg_handle *dwg_object_get_handle (dwg_object *restrict obj,
                                            int *restrict error) __nonnull_all;

  EXPORT dwg_obj_obj *dwg_object_to_object (dwg_object *restrict obj,
                                            int *restrict error) __nonnull_all;

  EXPORT dwg_obj_ent *dwg_object_to_entity (dwg_object *restrict obj,
                                            int *restrict error) __nonnull_all;

  EXPORT int dwg_object_get_type (const dwg_object *obj);

  EXPORT int dwg_object_get_fixedtype (const dwg_object *obj);

  EXPORT char *dwg_object_get_dxfname (const dwg_object *obj);

  EXPORT BITCODE_BL dwg_ref_get_absref (const dwg_object_ref *restrict ref,
                                        int *restrict error) __nonnull_all;

  EXPORT dwg_object *dwg_ref_get_object (const dwg_object_ref *restrict ref,
                                         int *restrict error) __nonnull ((2));

  EXPORT dwg_object *dwg_absref_get_object (const dwg_data *dwg,
                                            const BITCODE_BL absref);

  EXPORT unsigned int dwg_get_num_classes (const dwg_data *dwg);

  EXPORT dwg_class *dwg_get_class (const dwg_data *dwg, unsigned int index);

  /********************************************************************
   *                    FUNCTIONS FOR ADDING OBJECTS                  *
   ********************************************************************/
  /* This is only useful for DXF exports or dwg USE_WRITE support,
     but also needed to read preR13 DWG's. */

  /* All BITCODE_T/char* input strings are encoded as UTF-8, as with the
     dynapi. Most names are copied, since most names are considered to be
     constant. If not, you need to free them by yourself.

     Exceptions are dxfname (there exists a separate dxfname_u variant),
     the VX name, which does not exists anymore since r2000.

     When writing DWG, a version of R_2000 is recommended, only R_2 - R-2000
     are supported yet. For DXF you can try all versions >= R_13.
   */

  EXPORT Dwg_Data *dwg_new_Document (const Dwg_Version_Type version,
                                     const int imperial, const int loglevel);
  EXPORT int dwg_add_Document (Dwg_Data *restrict dwg, const int imperial);

  /* Convert UTF-8 strings to BITCODE_T fields. Returns a copy of the string.
   */
  EXPORT BITCODE_T dwg_add_u8_input (Dwg_Data *restrict dwg,
                                     const char *restrict u8str) __nonnull_all;

  /* no proxy, no is_zombie */
  /* returns -1 on error, 0 on success */
  EXPORT int dwg_add_class (Dwg_Data *restrict dwg,
                            const char *const restrict dxfname,
                            const char *const restrict cppname,
                            const char *const restrict appname,
                            const bool is_entity) __nonnull ((1, 2, 3));
  /* check if already exists, and if not add dxfname-specific defaults */
  EXPORT int dwg_require_class (Dwg_Data *restrict dwg,
                                const char *const restrict dxfname,
                                const size_t len) __nonnull_all;
  /* insert entity into mspace, pspace or other block */
  EXPORT int dwg_insert_entity (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                                Dwg_Object *restrict obj) __nonnull_all;

  /* Set defaults from HEADER: CLAYER, linewt, ltype_scale, color, ... */
  EXPORT int
  dwg_add_entity_defaults (Dwg_Data *restrict dwg,
                           Dwg_Object_Entity *restrict ent) __nonnull_all;

  /* returns BLOCK_HEADER owner for generic entity from ent->ownerhandle. */
  EXPORT Dwg_Object_BLOCK_HEADER *
  dwg_entity_owner (const void *_ent) __nonnull_all;

  /* check for valid symbol table record name, and if it fits the codepage.
     names can be up to 255 characters long and can contain letters,
     digits, and the following special characters:
     dollar sign ($), hyphen (-), and underscore (_).
     input is a TV or TU string
  */
  EXPORT bool dwg_is_valid_name (Dwg_Data *restrict dwg,
                                 const char *restrict name) __nonnull_all;
  // variant where input is a UTF-8 string.
  EXPORT bool dwg_is_valid_name_u8 (Dwg_Data *restrict dwg,
                                    const char *restrict name) __nonnull_all;

  /* utf-8 string without lowercase letters, space or !. maxlen 256 */
  EXPORT bool dwg_is_valid_tag (const char *tag) __nonnull_all;

  EXPORT Dwg_Entity_TEXT *
  dwg_add_TEXT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const char *restrict text_value,
                const dwg_point_3d *restrict ins_pt,
                const double height) __nonnull_all;

  /* Experimental. Does not work yet properly */
  /* This should add the ATTRIB and ENDBLK to the insert,
     and the ATTDEF and ENDBLK to the block, if missing.
     flags, bitmask of:
     0 none
     1 invisible, overridden by ATTDISP
     2 constant, no prompt
     4 verify on insert
     8 preset, inserted only with its default values, not editable.
  */
  EXPORT Dwg_Entity_ATTRIB *dwg_add_Attribute (
      Dwg_Entity_INSERT *restrict insert, const double height, const int flags,
      const char *restrict prompt, const dwg_point_3d *restrict ins_pt,
      const char *restrict tag, const char *restrict text_value) __nonnull_all;
  EXPORT Dwg_Entity_BLOCK *
  dwg_add_BLOCK (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const char *restrict name) __nonnull_all;
  EXPORT Dwg_Entity_ENDBLK *
  dwg_add_ENDBLK (Dwg_Object_BLOCK_HEADER *restrict blkhdr) __nonnull_all;
  EXPORT Dwg_Entity_SEQEND *
  dwg_add_SEQEND (dwg_ent_generic *restrict owner) __nonnull_all;

  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_INSERT *
  dwg_add_INSERT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const dwg_point_3d *restrict ins_pt,
                  const char *restrict name, const double xscale,
                  const double yscale, const double zscale,
                  const double rotation) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_MINSERT *dwg_add_MINSERT (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr,
      const dwg_point_3d *restrict ins_pt, const char *restrict name,
      const double xscale, const double yscale, const double zscale,
      const double rotation, const int num_rows, const int num_cols,
      const double row_spacing, const double col_spacing) __nonnull_all;
  EXPORT Dwg_Entity_POLYLINE_2D *
  dwg_add_POLYLINE_2D (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                       const int num_pts,
                       const dwg_point_2d *restrict pts) __nonnull_all;
  EXPORT Dwg_Entity_POLYLINE_3D *
  dwg_add_POLYLINE_3D (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                       const int num_pts,
                       const dwg_point_3d *restrict pts) __nonnull_all;
  /* invisible face edge if negative
     no 4th edge (ie a triangle) if the last face has index 0 (starts with 1)
  */
  EXPORT Dwg_Entity_POLYLINE_PFACE *
  dwg_add_POLYLINE_PFACE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                          const unsigned numverts, const unsigned numfaces,
                          const dwg_point_3d *restrict verts,
                          const dwg_face *restrict faces) __nonnull_all;
  EXPORT Dwg_Entity_POLYLINE_MESH *
  dwg_add_POLYLINE_MESH (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                         const unsigned num_m_verts,
                         const unsigned num_n_verts,
                         const dwg_point_3d *restrict verts) __nonnull_all;
  EXPORT Dwg_Entity_ARC *dwg_add_ARC (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                                      const dwg_point_3d *restrict center,
                                      const double radius,
                                      const double start_angle,
                                      const double end_angle) __nonnull_all;
  EXPORT Dwg_Entity_ATTDEF *dwg_add_ATTDEF (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr, const double height,
      const int mode, const char *restrict prompt,
      const dwg_point_3d *restrict ins_pt, const char *restrict tag,
      const char *restrict default_value) __nonnull_all;
  EXPORT Dwg_Entity_ATTRIB *
  dwg_add_ATTRIB (Dwg_Entity_INSERT *restrict insert, const double height,
                  const int flags, const dwg_point_3d *restrict ins_pt,
                  const char *restrict tag,
                  const char *restrict text_value) __nonnull_all;
  EXPORT Dwg_Entity_CIRCLE *
  dwg_add_CIRCLE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const dwg_point_3d *restrict center,
                  const double radius) __nonnull_all;
  EXPORT Dwg_Entity_LINE *
  dwg_add_LINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const dwg_point_3d *restrict start_pt,
                const dwg_point_3d *restrict end_pt) __nonnull_all;
  EXPORT Dwg_Entity_DIMENSION_ALIGNED *dwg_add_DIMENSION_ALIGNED (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr,
      const dwg_point_3d *restrict xline1_pt,
      const dwg_point_3d *restrict xline2_pt,
      const dwg_point_3d *restrict text_pt) __nonnull_all;
  EXPORT Dwg_Entity_DIMENSION_ANG2LN * /* DimAngular */
  dwg_add_DIMENSION_ANG2LN (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                            const dwg_point_3d *restrict center_pt,
                            const dwg_point_3d *restrict xline1end_pt,
                            const dwg_point_3d *restrict xline2end_pt,
                            const dwg_point_3d *restrict text_pt)
      __nonnull_all;
  EXPORT Dwg_Entity_DIMENSION_ANG3PT *dwg_add_DIMENSION_ANG3PT (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr,
      const dwg_point_3d *restrict center_pt,
      const dwg_point_3d *restrict xline1_pt,
      const dwg_point_3d *restrict xline2_pt,
      const dwg_point_3d *restrict text_pt) __nonnull_all;
  EXPORT Dwg_Entity_DIMENSION_DIAMETER *
  dwg_add_DIMENSION_DIAMETER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                              const dwg_point_3d *restrict chord_pt,
                              const dwg_point_3d *restrict far_chord_pt,
                              const double leader_len) __nonnull_all;
  EXPORT Dwg_Entity_DIMENSION_ORDINATE *dwg_add_DIMENSION_ORDINATE (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr,
      const dwg_point_3d *restrict def_pt, /* = feature_location_pt */
      const dwg_point_3d *restrict leader_endpt,
      const bool use_x_axis) __nonnull_all;
  EXPORT Dwg_Entity_DIMENSION_RADIUS *
  dwg_add_DIMENSION_RADIUS (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                            const dwg_point_3d *restrict center_pt,
                            const dwg_point_3d *restrict chord_pt,
                            const double leader_len) __nonnull_all;
  EXPORT Dwg_Entity_DIMENSION_LINEAR * /* Rotated */
  dwg_add_DIMENSION_LINEAR (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                            const dwg_point_3d *restrict xline1_pt,
                            const dwg_point_3d *restrict xline2_pt,
                            const dwg_point_3d *restrict def_pt,
                            const double rotation_angle) __nonnull_all;
  EXPORT Dwg_Entity_POINT *
  dwg_add_POINT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict pt) __nonnull_all;
  EXPORT Dwg_Entity__3DFACE *
  dwg_add_3DFACE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const dwg_point_3d *restrict pt1,
                  const dwg_point_3d *restrict pt2,
                  const dwg_point_3d *restrict pt3,
                  const dwg_point_3d *restrict pt4 /* may be NULL */)
      __nonnull ((1, 2, 3, 4));
  EXPORT Dwg_Entity_SOLID *
  dwg_add_SOLID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict pt1,
                 const dwg_point_2d *restrict pt2,
                 const dwg_point_2d *restrict pt3,
                 const dwg_point_2d *restrict pt4) __nonnull_all;
  EXPORT Dwg_Entity_TRACE *
  dwg_add_TRACE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict pt1,
                 const dwg_point_2d *restrict pt2,
                 const dwg_point_2d *restrict pt3,
                 const dwg_point_2d *restrict pt4) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_SHAPE *
  dwg_add_SHAPE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const char *restrict name,
                 const dwg_point_3d *restrict ins_pt, const double scale,
                 const double oblique_angle) __nonnull_all;
  EXPORT Dwg_Entity_VIEWPORT *
  dwg_add_VIEWPORT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                    const char *restrict name) __nonnull_all;
  EXPORT Dwg_Entity_ELLIPSE *
  dwg_add_ELLIPSE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                   const dwg_point_3d *restrict center,
                   const double major_axis,
                   const double axis_ratio) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_SPLINE *
  dwg_add_SPLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const int num_fit_pts, const dwg_point_3d *restrict fit_pts,
                  const dwg_point_3d *restrict beg_tan_vec,
                  const dwg_point_3d *restrict end_tan_vec) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_REGION *
  dwg_add_REGION (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const char *acis_data) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_3DSOLID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                   const char *acis_data) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_BODY *
  dwg_add_BODY (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const char *acis_data) __nonnull_all;
  /* TODO VBA has two points, not a vector */
  EXPORT Dwg_Entity_RAY *
  dwg_add_RAY (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict point,
               const dwg_point_3d *restrict vector) __nonnull_all;
  EXPORT Dwg_Entity_XLINE *
  dwg_add_XLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict point,
                 const dwg_point_3d *restrict vector) __nonnull_all;

  EXPORT Dwg_Object_DICTIONARY *
  dwg_add_DICTIONARY (Dwg_Data *restrict dwg,
                      const char *restrict name, /* the NOD entry */
                      const char *restrict text, /* maybe NULL */
                      const BITCODE_RLL absolute_ref) __nonnull ((1));
  EXPORT Dwg_Object_DICTIONARY *
  dwg_add_DICTIONARY_item (Dwg_Object_DICTIONARY *_obj,
                           const char *restrict text,
                           const BITCODE_RLL absolute_ref) __nonnull_all;
  EXPORT Dwg_Object_DICTIONARYWDFLT *
  dwg_add_DICTIONARYWDFLT (Dwg_Data *restrict dwg,
                           const char *restrict name, /* the NOD entry */
                           const char *restrict text, /* maybe NULL */
                           const BITCODE_RLL absolute_ref) __nonnull ((1));
  EXPORT Dwg_Entity_OLE2FRAME *
  dwg_add_OLE2FRAME (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                     const dwg_point_3d *restrict pt1,
                     const dwg_point_3d *restrict pt2) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_MTEXT *
  dwg_add_MTEXT (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict ins_pt, const double rect_width,
                 const char *restrict text_value /*UTF-8*/) __nonnull_all;
  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_LEADER *dwg_add_LEADER (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr, const unsigned num_points,
      const dwg_point_3d *restrict points,
      const Dwg_Entity_MTEXT *restrict associated_annotation, /* maybe NULL */
      const unsigned type) __nonnull ((1, 3));
  EXPORT Dwg_Entity_TOLERANCE *
  dwg_add_TOLERANCE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                     const char *restrict text_value,
                     const dwg_point_3d *restrict ins_pt,
                     const dwg_point_3d *restrict x_direction /* maybe NULL */)
      __nonnull ((1, 2, 3));
  EXPORT Dwg_Entity_MLINE *
  dwg_add_MLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const unsigned num_verts,
                 const dwg_point_3d *restrict verts) __nonnull_all;

  EXPORT Dwg_Entity_LWPOLYLINE *
  dwg_add_LWPOLYLINE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                      const int num_pts2d,
                      const dwg_point_2d *restrict pts2d) __nonnull_all;

  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Entity_HATCH *
  dwg_add_HATCH (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const int pattern_type, const char *restrict name,
                 const bool is_associative, const unsigned num_paths,
                 // Line, Polyline, Circle, Ellipse, Spline or Region objs as
                 // boundary_handles
                 const Dwg_Object **pathobjs) __nonnull_all;

  /* Add to DICTIONARY */
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD (Dwg_Object_DICTIONARY *restrict dict,
                   const char *restrict keyword) __nonnull_all;
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_bool (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                        const BITCODE_B value) __nonnull ((1));
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_int8 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                        const BITCODE_RC value) __nonnull ((1));
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_int16 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                         const BITCODE_BS value) __nonnull ((1));
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_int32 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                         const BITCODE_BL value) __nonnull ((1));
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_int64 (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                         const BITCODE_BLL value) __nonnull ((1));
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_real (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                        const BITCODE_BD value) __nonnull ((1));
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_pointd3d (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                            const BITCODE_3DPOINT *pt) __nonnull ((1));
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_binary (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                          const int size,
                          const BITCODE_RC *data) __nonnull_all;
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_string (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                          const BITCODE_BS len,
                          const char *restrict str) __nonnull_all;
  EXPORT Dwg_Object_XRECORD *
  dwg_add_XRECORD_handle (Dwg_Object_XRECORD *restrict _obj, const short dxf,
                          const Dwg_Handle hdl) __nonnull_all;

  EXPORT Dwg_Object_PLACEHOLDER *
  dwg_add_PLACEHOLDER (Dwg_Data *restrict dwg) __nonnull_all;
  EXPORT Dwg_Object_VBA_PROJECT *
  dwg_add_VBA_PROJECT (Dwg_Data *restrict dwg, const BITCODE_BL size,
                       const BITCODE_RC *data) __nonnull_all;
  /* Either added to mspace, pspace, or VIEWPORT entity in pspace, or VPORT
   * object in mspace. */
  EXPORT Dwg_Object_LAYOUT *
  dwg_add_LAYOUT (Dwg_Object *restrict vp, const char *restrict name,
                  const char *restrict canonical_media_name) __nonnull_all;

  /* Experimental. Does not work yet properly. */
  // Called Raster in VBA
  EXPORT Dwg_Entity_IMAGE *dwg_add_IMAGE (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr, const char *restrict file_path,
      const dwg_point_3d *restrict ins_pt, const double scale_factor,
      const double rotation_angle) __nonnull_all;

  EXPORT Dwg_Entity_LARGE_RADIAL_DIMENSION *
  dwg_add_LARGE_RADIAL_DIMENSION (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                                  const dwg_point_3d *restrict center_pt,
                                  const dwg_point_3d *restrict first_arc_pt,
                                  const dwg_point_3d *restrict ovr_center,
                                  const dwg_point_3d *restrict jog_point,
                                  const double leader_len) __nonnull_all;

  /* Experimental. Does not work yet properly. */
  EXPORT Dwg_Entity_PDFUNDERLAY *dwg_add_PDFUNDERLAY (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr, const char *restrict filename,
      const dwg_point_3d *restrict ins_pt, const double scale_factor,
      const double rotation_angle) __nonnull_all;

  /* All the ACSH methods and 3d primitives are still experimental.
     They do not work yet properly */
  EXPORT Dwg_Object_ACSH_BOX_CLASS *
  dwg_add_ACSH_BOX_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                          const dwg_point_3d *restrict origin_pt,
                          const dwg_point_3d *restrict normal,
                          const double length, const double width,
                          const double height) __nonnull_all;
  EXPORT Dwg_Object_ACSH_CHAMFER_CLASS *dwg_add_ACSH_CHAMFER_CLASS (
      Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
      const dwg_point_3d *restrict origin_pt,
      const dwg_point_3d *restrict normal, const int bl92,
      const double base_dist, const double other_dist, const int num_edges,
      const int32_t *edges, const int bl95) __nonnull_all;
  EXPORT Dwg_Object_ACSH_CONE_CLASS *
  dwg_add_ACSH_CONE_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                           const dwg_point_3d *restrict origin_pt,
                           const dwg_point_3d *restrict normal,
                           const double height, const double major_radius,
                           const double minor_radius,
                           const double x_radius) __nonnull_all;
  EXPORT Dwg_Object_ACSH_CYLINDER_CLASS *
  dwg_add_ACSH_CYLINDER_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                               const dwg_point_3d *restrict origin_pt,
                               const dwg_point_3d *restrict normal,
                               const double height, const double major_radius,
                               const double minor_radius,
                               const double x_radius) __nonnull_all;
  EXPORT Dwg_Object_ACSH_PYRAMID_CLASS *
  dwg_add_ACSH_PYRAMID_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                              const dwg_point_3d *restrict origin_pt,
                              const dwg_point_3d *restrict normal,
                              const double height, const int sides,
                              const double radius,
                              const double topradius) __nonnull_all;
  EXPORT Dwg_Object_ACSH_SPHERE_CLASS *
  dwg_add_ACSH_SPHERE_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                             const dwg_point_3d *restrict origin_pt,
                             const dwg_point_3d *restrict normal,
                             const double radius) __nonnull_all;
  EXPORT Dwg_Object_ACSH_TORUS_CLASS *
  dwg_add_ACSH_TORUS_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                            const dwg_point_3d *restrict origin_pt,
                            const dwg_point_3d *restrict normal,
                            const double major_radius,
                            const double minor_radius) __nonnull_all;
  EXPORT Dwg_Object_ACSH_WEDGE_CLASS *
  dwg_add_ACSH_WEDGE_CLASS (Dwg_Object_EVALUATION_GRAPH *restrict evalgraph,
                            const dwg_point_3d *restrict origin_pt,
                            const dwg_point_3d *restrict normal,
                            const double length, const double width,
                            const double height) __nonnull_all;
  EXPORT Dwg_Object_EVALUATION_GRAPH *
  dwg_add_EVALUATION_GRAPH (Dwg_Data *restrict dwg, const int has_graph,
                            const int nodeid, const unsigned num_evalexpr,
                            const BITCODE_H *restrict evalexpr)
      __nonnull ((1));
  EXPORT Dwg_Object_ACSH_HISTORY_CLASS *
  dwg_add_ACSH_HISTORY_CLASS (Dwg_Entity_3DSOLID *restrict region,
                              const int h_nodeid) __nonnull_all;

  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_BOX (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
               const dwg_point_3d *restrict origin_pt,
               const dwg_point_3d *restrict normal, /* maybe NULL */
               const double length, const double width, const double height)
      __nonnull ((1, 2));

  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_CONE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                const dwg_point_3d *restrict origin_pt,
                const dwg_point_3d *restrict normal, /* maybe NULL */
                const double height, const double major_radius,
                const double minor_radius, const double x_radius)
      __nonnull ((1, 2));
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_CYLINDER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                    const dwg_point_3d *restrict origin_pt,
                    const dwg_point_3d *restrict normal, /* maybe NULL */
                    const double height, const double major_radius,
                    const double minor_radius, const double x_radius)
      __nonnull ((1, 2));
  // EXPORT Dwg_Entity_3DSOLID*
  // dwg_add_CHAMFER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
  //                  const dwg_point_3d *restrict origin_pt,
  //                  const dwg_point_3d *restrict normal, /* maybe NULL */
  //                  const int bl92, const double base_dist,
  //                  const double other_dist, const int num_edges,
  //                  const int32_t* edges, const int bl95)  __nonnull ((1,2));
  // EXPORT Dwg_Entity_3DSOLID*
  // dwg_add_ELLIPTICAL_CONE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
  //                          const dwg_point_3d *restrict origin_pt,
  //                          const dwg_point_3d *restrict normal, /* maybe
  //                          NULL */ const double major_radius, const double
  //                          minor_radius, const double height) __nonnull
  //                          ((1,2));
  // EXPORT Dwg_Entity_3DSOLID*
  // dwg_add_ELLIPTICAL_CYLINDER (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
  //                              const dwg_point_3d *restrict origin_pt,
  //                              const dwg_point_3d *restrict normal, /* maybe
  //                              NULL */ const double major_radius, const
  //                              double minor_radius, const double height)
  //                              __nonnull ((1,2));
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_EXTRUDED_SOLID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                          const Dwg_Object *restrict profile,
                          const double height,
                          const double taper_angle) __nonnull_all;
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_EXTRUDED_PATH (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                         const Dwg_Object *restrict profile,
                         const double height,
                         const double taper_angle) __nonnull_all;
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_PYRAMID (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                   const dwg_point_3d *restrict origin_pt,
                   const dwg_point_3d *restrict normal, /* maybe NULL */
                   const double height, const int sides, const double radius,
                   const double topradius) __nonnull ((1, 2));
  EXPORT Dwg_Entity_3DSOLID *dwg_add_REVOLVED_SOLID (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr,
      const Dwg_Object *restrict profile, const dwg_point_3d *restrict axis_pt,
      const dwg_point_3d *restrict axis_dir, const double angle) __nonnull_all;
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_SPHERE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                  const dwg_point_3d *restrict origin_pt,
                  const dwg_point_3d *restrict normal, /* maybe NULL */
                  const double radius) __nonnull ((1, 2));
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_TORUS (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict origin_pt,
                 const dwg_point_3d *restrict normal, /* maybe NULL */
                 const double torus_radius, const double tube_radius)
      __nonnull ((1, 2));
  EXPORT Dwg_Entity_3DSOLID *
  dwg_add_WEDGE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict origin_pt,
                 const dwg_point_3d *restrict normal, /* maybe NULL */
                 const double length, const double width, const double height)
      __nonnull ((1, 2));
  /*
  EXPORT Dwg_Entity_TABLE*
  dwg_add_TABLE (Dwg_Object_BLOCK_HEADER *restrict blkhdr,
                 const dwg_point_3d *restrict ins_pt,
                 const int num_rows,
                 const int num_cols,
                 const double row_height,
                 const double col_width) __nonnull_all;
  */

  /* Tables:
     The names are all accepted as UTF-8 only.
     On NULL names just create the CONTROL object/resp. NOD entry.
   */
  EXPORT Dwg_Object_BLOCK_CONTROL *
  dwg_add_BLOCK_CONTROL (Dwg_Data *restrict dwg, const unsigned ms,
                         const unsigned ps) __nonnull_all;

  EXPORT Dwg_Object_BLOCK_HEADER *
  dwg_add_BLOCK_HEADER (Dwg_Data *restrict dwg, const char *restrict name)
      __nonnull ((1));
  EXPORT Dwg_Object_UCS *
  dwg_add_UCS (Dwg_Data *restrict dwg, const dwg_point_3d *restrict origin,
               const dwg_point_3d *restrict x_axis,
               const dwg_point_3d *restrict y_axis, const char *restrict name)
      __nonnull ((1));
  EXPORT Dwg_Object_LAYER *
  dwg_add_LAYER (Dwg_Data *restrict dwg,
                 const char *restrict name /* maybe NULL */) __nonnull ((1));
  EXPORT Dwg_Object_STYLE *
  dwg_add_STYLE (Dwg_Data *restrict dwg,
                 const char *restrict name /* maybe NULL */) __nonnull ((1));
  EXPORT Dwg_Object_LTYPE *
  dwg_add_LTYPE (Dwg_Data *restrict dwg,
                 const char *restrict name /* maybe NULL */) __nonnull ((1));
  EXPORT Dwg_Object_VIEW *
  dwg_add_VIEW (Dwg_Data *restrict dwg,
                const char *restrict name /* maybe NULL */) __nonnull ((1));
  EXPORT Dwg_Object_DIMSTYLE *
  dwg_add_DIMSTYLE (Dwg_Data *restrict dwg,
                    const char *restrict name /* maybe NULL */)
      __nonnull ((1));
  EXPORT Dwg_Object_VPORT *
  dwg_add_VPORT (Dwg_Data *restrict dwg,
                 const char *restrict name /* maybe NULL */) __nonnull ((1));
  EXPORT Dwg_Object_VX_TABLE_RECORD *
  dwg_add_VX (Dwg_Data *restrict dwg,
              const char *restrict name /* maybe NULL */) __nonnull ((1));
  EXPORT Dwg_Object_APPID *
  dwg_add_APPID (Dwg_Data *restrict dwg,
                 const char *restrict name /* maybe NULL */) __nonnull ((1));
  EXPORT Dwg_Object_GROUP *
  dwg_add_GROUP (Dwg_Data *restrict dwg,
                 const char *restrict name /* maybe NULL */) __nonnull ((1));

  /* Experimental. Does not work yet properly */
  EXPORT Dwg_Object_MLINESTYLE *
  dwg_add_MLINESTYLE (Dwg_Data *restrict dwg,
                      const char *restrict name) __nonnull_all;

  /* Experimental API's. Will change. */
  EXPORT Dwg_Object_PROXY_OBJECT *
  dwg_add_PROXY_OBJECT (Dwg_Data *restrict dwg, char *name, char *key
                        /*, size, data */) __nonnull_all;
  EXPORT Dwg_Entity_PROXY_ENTITY *dwg_add_PROXY_ENTITY (
      Dwg_Object_BLOCK_HEADER *restrict blkhdr /* ... */) __nonnull_all;
  EXPORT Dwg_Object_LAYERFILTER *
  dwg_add_LAYERFILTER (Dwg_Data *restrict dwg /* ... */) __nonnull_all;
  EXPORT Dwg_Object_LAYER_INDEX *
  dwg_add_LAYER_INDEX (Dwg_Data *restrict dwg /* ... */) __nonnull_all;
  EXPORT Dwg_Object_SPATIAL_FILTER *dwg_add_SPATIAL_FILTER (
      Dwg_Entity_INSERT *restrict insert /*, clip_verts... */) __nonnull_all;
  EXPORT Dwg_Object_SPATIAL_INDEX *
  dwg_add_SPATIAL_INDEX (Dwg_Data *restrict dwg /* ... */) __nonnull_all;

  EXPORT Dwg_Object_WIPEOUTVARIABLES *
  dwg_add_WIPEOUTVARIABLES (Dwg_Data *dwg /* ... */) __nonnull_all;

/* List of yet supported and unsupported add entity and object API from
   objects.inc, so we can auto-generate API's, i.e. for gambas.

   _3D* are defined without underscore.
   add_<TABLE>_CONTROL are not needed, they are added on dwg_add_Document()
   and via add_<TABLE>.
 */
#define HAVE_NO_DWG_ADD_ACMECOMMANDHISTORY
#define HAVE_NO_DWG_ADD_ACMESCOPE
#define HAVE_NO_DWG_ADD_ACMESTATEMGR
#define HAVE_NO_DWG_ADD_ACSH_BOOLEAN_CLASS
#define HAVE_NO_DWG_ADD_ACSH_BREP_CLASS
#define HAVE_NO_DWG_ADD_ACSH_EXTRUSION_CLASS
#define HAVE_NO_DWG_ADD_ACSH_FILLET_CLASS
#define HAVE_NO_DWG_ADD_ACSH_LOFT_CLASS
#define HAVE_NO_DWG_ADD_ACSH_REVOLVE_CLASS
#define HAVE_NO_DWG_ADD_ACSH_SWEEP_CLASS
#define HAVE_NO_DWG_ADD_ALDIMOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_ALIGNMENTPARAMETERENTITY
#define HAVE_NO_DWG_ADD_ANGDIMOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_ANNOTSCALEOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_APPID_CONTROL
#define HAVE_NO_DWG_ADD_ARCALIGNEDTEXT
#define HAVE_NO_DWG_ADD_ARC_DIMENSION
#define HAVE_NO_DWG_ADD_ASSOC2DCONSTRAINTGROUP
#define HAVE_NO_DWG_ADD_ASSOC3POINTANGULARDIMACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCACTION
#define HAVE_NO_DWG_ADD_ASSOCACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCALIGNEDDIMACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCARRAYACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCARRAYMODIFYACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCARRAYMODIFYPARAMETERS
#define HAVE_NO_DWG_ADD_ASSOCARRAYPATHPARAMETERS
#define HAVE_NO_DWG_ADD_ASSOCARRAYPOLARPARAMETERS
#define HAVE_NO_DWG_ADD_ASSOCARRAYRECTANGULARPARAMETERS
#define HAVE_NO_DWG_ADD_ASSOCASMBODYACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCBLENDSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCCOMPOUNDACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCDEPENDENCY
#define HAVE_NO_DWG_ADD_ASSOCDIMDEPENDENCYBODY
#define HAVE_NO_DWG_ADD_ASSOCEDGEACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCEDGECHAMFERACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCEDGEFILLETACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCEXTENDSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCEXTRUDEDSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCFACEACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCFILLETSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCGEOMDEPENDENCY
#define HAVE_NO_DWG_ADD_ASSOCLOFTEDSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCMLEADERACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCNETWORK
#define HAVE_NO_DWG_ADD_ASSOCNETWORKSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCOBJECTACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCOFFSETSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCORDINATEDIMACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCOSNAPPOINTREFACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCPATCHSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCPATHACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCPERSSUBENTMANAGER
#define HAVE_NO_DWG_ADD_ASSOCPLANESURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCPOINTREFACTIONPARAM
#define HAVE_NO_DWG_ADD_ASSOCRESTOREENTITYSTATEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCREVOLVEDSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCROTATEDDIMACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCSWEPTSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCTRIMSURFACEACTIONBODY
#define HAVE_NO_DWG_ADD_ASSOCVALUEDEPENDENCY
#define HAVE_NO_DWG_ADD_ASSOCVARIABLE
#define HAVE_NO_DWG_ADD_ASSOCVERTEXACTIONPARAM
#define HAVE_NO_DWG_ADD_ATTDEF
#define HAVE_NO_DWG_ADD_ATTRIB
#define HAVE_NO_DWG_ADD_BACKGROUND
#define HAVE_NO_DWG_ADD_BASEPOINTPARAMETERENTITY
#define HAVE_NO_DWG_ADD_BLKREFOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_BLOCKALIGNEDCONSTRAINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKALIGNMENTGRIP
#define HAVE_NO_DWG_ADD_BLOCKALIGNMENTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKANGULARCONSTRAINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKARRAYACTION
#define HAVE_NO_DWG_ADD_BLOCKBASEPOINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKDIAMETRICCONSTRAINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKFLIPACTION
#define HAVE_NO_DWG_ADD_BLOCKFLIPGRIP
#define HAVE_NO_DWG_ADD_BLOCKFLIPPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKGRIPLOCATIONCOMPONENT
#define HAVE_NO_DWG_ADD_BLOCKHORIZONTALCONSTRAINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKLINEARCONSTRAINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKLINEARGRIP
#define HAVE_NO_DWG_ADD_BLOCKLINEARPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKLOOKUPACTION
#define HAVE_NO_DWG_ADD_BLOCKLOOKUPGRIP
#define HAVE_NO_DWG_ADD_BLOCKLOOKUPPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKMOVEACTION
#define HAVE_NO_DWG_ADD_BLOCKPARAMDEPENDENCYBODY
#define HAVE_NO_DWG_ADD_BLOCKPOINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKPOLARGRIP
#define HAVE_NO_DWG_ADD_BLOCKPOLARPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKPOLARSTRETCHACTION
#define HAVE_NO_DWG_ADD_BLOCKPROPERTIESTABLE
#define HAVE_NO_DWG_ADD_BLOCKPROPERTIESTABLEGRIP
#define HAVE_NO_DWG_ADD_BLOCKRADIALCONSTRAINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKREPRESENTATION
#define HAVE_NO_DWG_ADD_BLOCKROTATEACTION
#define HAVE_NO_DWG_ADD_BLOCKROTATIONGRIP
#define HAVE_NO_DWG_ADD_BLOCKROTATIONPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKSCALEACTION
#define HAVE_NO_DWG_ADD_BLOCKSTRETCHACTION
#define HAVE_NO_DWG_ADD_BLOCKUSERPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKVERTICALCONSTRAINTPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKVISIBILITYGRIP
#define HAVE_NO_DWG_ADD_BLOCKVISIBILITYPARAMETER
#define HAVE_NO_DWG_ADD_BLOCKXYGRIP
#define HAVE_NO_DWG_ADD_BLOCKXYPARAMETER
#define HAVE_NO_DWG_ADD_CAMERA
#define HAVE_NO_DWG_ADD_CELLSTYLEMAP
#define HAVE_NO_DWG_ADD_CONTEXTDATAMANAGER
#define HAVE_NO_DWG_ADD_CSACDOCUMENTOPTIONS
#define HAVE_NO_DWG_ADD_CURVEPATH
#define HAVE_NO_DWG_ADD_DATALINK
#define HAVE_NO_DWG_ADD_DATATABLE
#define HAVE_NO_DWG_ADD_DBCOLOR
#define HAVE_NO_DWG_ADD_DETAILVIEWSTYLE
#define HAVE_NO_DWG_ADD_DICTIONARYVAR
#define HAVE_NO_DWG_ADD_DIMASSOC
#define HAVE_NO_DWG_ADD_DIMSTYLE_CONTROL
#define HAVE_NO_DWG_ADD_DMDIMOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_DUMMY
#define HAVE_NO_DWG_ADD_DYNAMICBLOCKPROXYNODE
#define HAVE_NO_DWG_ADD_DYNAMICBLOCKPURGEPREVENTER
#define HAVE_NO_DWG_ADD_EXTRUDEDSURFACE
#define HAVE_NO_DWG_ADD_FCFOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_FIELD
#define HAVE_NO_DWG_ADD_FIELDLIST
#define HAVE_NO_DWG_ADD_FLIPPARAMETERENTITY
#define HAVE_NO_DWG_ADD_GEODATA
#define HAVE_NO_DWG_ADD_GEOMAPIMAGE
#define HAVE_NO_DWG_ADD_GEOPOSITIONMARKER
#define HAVE_NO_DWG_ADD_HELIX
#define HAVE_NO_DWG_ADD_IDBUFFER
#define HAVE_NO_DWG_ADD_IMAGEDEF
#define HAVE_NO_DWG_ADD_IMAGEDEF_REACTOR
#define HAVE_NO_DWG_ADD_INDEX
#define HAVE_NO_DWG_ADD_LAYER_CONTROL
#define HAVE_NO_DWG_ADD_LAYOUTPRINTCONFIG
#define HAVE_NO_DWG_ADD_LEADEROBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_LIGHT
#define HAVE_NO_DWG_ADD_LIGHTLIST
#define HAVE_NO_DWG_ADD_LINEARPARAMETERENTITY
#define HAVE_NO_DWG_ADD_LOFTEDSURFACE
#define HAVE_NO_DWG_ADD_LONG_TRANSACTION
#define HAVE_NO_DWG_ADD_LTYPE_CONTROL
#define HAVE_NO_DWG_ADD_MATERIAL
#define HAVE_NO_DWG_ADD_MENTALRAYRENDERSETTINGS
#define HAVE_NO_DWG_ADD_MESH
#define HAVE_NO_DWG_ADD_MLEADEROBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_MLEADERSTYLE
#define HAVE_NO_DWG_ADD_MOTIONPATH
#define HAVE_NO_DWG_ADD_MPOLYGON
#define HAVE_NO_DWG_ADD_MTEXTATTRIBUTEOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_MTEXTOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_MULTILEADER
#define HAVE_NO_DWG_ADD_NAVISWORKSMODEL
#define HAVE_NO_DWG_ADD_NAVISWORKSMODELDEF
#define HAVE_NO_DWG_ADD_NURBSURFACE
#define HAVE_NO_DWG_ADD_OBJECT_PTR
#define HAVE_NO_DWG_ADD_OLEFRAME
#define HAVE_NO_DWG_ADD_ORDDIMOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_PERSUBENTMGR
#define HAVE_NO_DWG_ADD_PLANESURFACE
#define HAVE_NO_DWG_ADD_PLOTSETTINGS
#define HAVE_NO_DWG_ADD_POINTCLOUD
#define HAVE_NO_DWG_ADD_POINTCLOUDCOLORMAP
#define HAVE_NO_DWG_ADD_POINTCLOUDDEF
#define HAVE_NO_DWG_ADD_POINTCLOUDDEFEX
#define HAVE_NO_DWG_ADD_POINTCLOUDDEF_REACTOR
#define HAVE_NO_DWG_ADD_POINTCLOUDDEF_REACTOR_EX
#define HAVE_NO_DWG_ADD_POINTCLOUDEX
#define HAVE_NO_DWG_ADD_POINTPARAMETERENTITY
#define HAVE_NO_DWG_ADD_POINTPATH
#define HAVE_NO_DWG_ADD_RADIMLGOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_RADIMOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_RAPIDRTRENDERSETTINGS
#define HAVE_NO_DWG_ADD_RASTERVARIABLES
#define HAVE_NO_DWG_ADD_RENDERENTRY
#define HAVE_NO_DWG_ADD_RENDERENVIRONMENT
#define HAVE_NO_DWG_ADD_RENDERGLOBAL
#define HAVE_NO_DWG_ADD_RENDERSETTINGS
#define HAVE_NO_DWG_ADD_REVOLVEDSURFACE
#define HAVE_NO_DWG_ADD_ROTATIONPARAMETERENTITY
#define HAVE_NO_DWG_ADD_RTEXT
#define HAVE_NO_DWG_ADD_SCALE
#define HAVE_NO_DWG_ADD_SECTIONOBJECT
#define HAVE_NO_DWG_ADD_SECTIONVIEWSTYLE
#define HAVE_NO_DWG_ADD_SECTION_MANAGER
#define HAVE_NO_DWG_ADD_SECTION_SETTINGS
#define HAVE_NO_DWG_ADD_SEQEND
#define HAVE_NO_DWG_ADD_SORTENTSTABLE
#define HAVE_NO_DWG_ADD_STYLE_CONTROL
#define HAVE_NO_DWG_ADD_SUN
#define HAVE_NO_DWG_ADD_SUNSTUDY
#define HAVE_NO_DWG_ADD_SWEPTSURFACE
#define HAVE_NO_DWG_ADD_TABLE
#define HAVE_NO_DWG_ADD_TABLECONTENT
#define HAVE_NO_DWG_ADD_TABLEGEOMETRY
#define HAVE_NO_DWG_ADD_TABLESTYLE
#define HAVE_NO_DWG_ADD_TEXTOBJECTCONTEXTDATA
#define HAVE_NO_DWG_ADD_TVDEVICEPROPERTIES
#define HAVE_NO_DWG_ADD_UCS_CONTROL
#define HAVE_NO_DWG_ADD_DGNUNDERLAY
#define HAVE_NO_DWG_ADD_DWFPDFUNDERLAY
#define HAVE_NO_DWG_ADD_PDFDEFINITION
#define HAVE_NO_DWG_ADD_DGNDEFINITION
#define HAVE_NO_DWG_ADD_DWFDEFINITION
#define HAVE_NO_DWG_ADD_UNKNOWN_ENT
#define HAVE_NO_DWG_ADD_UNKNOWN_OBJ
#define HAVE_NO_DWG_ADD_VERTEX_2D
#define HAVE_NO_DWG_ADD_VERTEX_3D
#define HAVE_NO_DWG_ADD_VERTEX_MESH
#define HAVE_NO_DWG_ADD_VERTEX_PFACE
#define HAVE_NO_DWG_ADD_VERTEX_PFACE_FACE
#define HAVE_NO_DWG_ADD_VIEW_CONTROL
#define HAVE_NO_DWG_ADD_VISIBILITYGRIPENTITY
#define HAVE_NO_DWG_ADD_VISIBILITYPARAMETERENTITY
#define HAVE_NO_DWG_ADD_VISUALSTYLE
#define HAVE_NO_DWG_ADD_VPORT_CONTROL
#define HAVE_NO_DWG_ADD_VX_CONTROL
#define HAVE_NO_DWG_ADD_VX_TABLE_RECORD
#define HAVE_NO_DWG_ADD_WIPEOUT
#define HAVE_NO_DWG_ADD_WIPEOUTVARIABLES
#define HAVE_NO_DWG_ADD_XYPARAMETERENTITY

#define HAVE_NO_DWG_ADD_LAYERFILTER
#define HAVE_NO_DWG_ADD_LAYER_INDEX
#define HAVE_NO_DWG_ADD_SPATIAL_FILTER
#define HAVE_NO_DWG_ADD_SPATIAL_INDEX

#define HAVE_DWG_ADD_BOX
#define HAVE_DWG_ADD_CONE
#define HAVE_DWG_ADD_CYLINDER
#define HAVE_DWG_ADD_SPHERE
#define HAVE_DWG_ADD_TORUS
#define HAVE_DWG_ADD_WEDGE
#define HAVE_NO_DWG_ADD_CHAMFER
#define HAVE_NO_DWG_ADD_ELLIPTICAL_CONE
#define HAVE_NO_DWG_ADD_ELLIPTICAL_CYLINDER
#define HAVE_NO_DWG_ADD_EXTRUDED_SOLID
#define HAVE_NO_DWG_ADD_EXTRUDED_PATH
#define HAVE_NO_DWG_ADD_REVOLVED_SOLID
#define HAVE_NO_DWG_ADD_CHAMFER
#define HAVE_NO_DWG_ADD_PYRAMID

#define HAVE_DWG_ADD_3DFACE
#define HAVE_DWG_ADD_3DSOLID
#define HAVE_DWG_ADD_ACSH_BOX_CLASS
#define HAVE_DWG_ADD_ACSH_CHAMFER_CLASS
#define HAVE_DWG_ADD_ACSH_CONE_CLASS
#define HAVE_DWG_ADD_ACSH_CYLINDER_CLASS
#define HAVE_DWG_ADD_ACSH_HISTORY_CLASS
#define HAVE_DWG_ADD_ACSH_PYRAMID_CLASS
#define HAVE_DWG_ADD_ACSH_SPHERE_CLASS
#define HAVE_DWG_ADD_ACSH_TORUS_CLASS
#define HAVE_DWG_ADD_ACSH_WEDGE_CLASS
#define HAVE_DWG_ADD_APPID
#define HAVE_DWG_ADD_ARC
#define HAVE_DWG_ADD_BLOCK
#define HAVE_DWG_ADD_BLOCK_CONTROL
#define HAVE_DWG_ADD_BLOCK_HEADER
#define HAVE_DWG_ADD_BODY
#define HAVE_DWG_ADD_CIRCLE
#define HAVE_DWG_ADD_DICTIONARY
#define HAVE_DWG_ADD_DICTIONARYWDFLT
#define HAVE_DWG_ADD_DIMENSION_ALIGNED
#define HAVE_DWG_ADD_DIMENSION_ANG2LN
#define HAVE_DWG_ADD_DIMENSION_ANG3PT
#define HAVE_DWG_ADD_DIMENSION_DIAMETER
#define HAVE_DWG_ADD_DIMENSION_LINEAR
#define HAVE_DWG_ADD_DIMENSION_ORDINATE
#define HAVE_DWG_ADD_DIMENSION_RADIUS
#define HAVE_DWG_ADD_DIMSTYLE
#define HAVE_DWG_ADD_ELLIPSE
#define HAVE_DWG_ADD_ENDBLK
#define HAVE_DWG_ADD_EVALUATION_GRAPH
#define HAVE_DWG_ADD_GROUP
#define HAVE_DWG_ADD_HATCH
#define HAVE_DWG_ADD_IMAGE
#define HAVE_DWG_ADD_INSERT
#define HAVE_DWG_ADD_LARGE_RADIAL_DIMENSION
#define HAVE_DWG_ADD_LAYER
#define HAVE_DWG_ADD_LAYOUT
#define HAVE_DWG_ADD_LEADER
#define HAVE_DWG_ADD_LINE
#define HAVE_DWG_ADD_LTYPE
#define HAVE_DWG_ADD_LWPOLYLINE
#define HAVE_DWG_ADD_MINSERT
#define HAVE_DWG_ADD_MLINE
#define HAVE_DWG_ADD_MLINESTYLE
#define HAVE_DWG_ADD_MTEXT
#define HAVE_DWG_ADD_OLE2FRAME
#define HAVE_DWG_ADD_PDFUNDERLAY
#define HAVE_DWG_ADD_PLACEHOLDER
#define HAVE_DWG_ADD_POINT
#define HAVE_DWG_ADD_POLYLINE_2D
#define HAVE_DWG_ADD_POLYLINE_3D
#define HAVE_DWG_ADD_POLYLINE_MESH
#define HAVE_DWG_ADD_POLYLINE_PFACE
#define HAVE_DWG_ADD_PROXY_ENTITY
#define HAVE_DWG_ADD_PROXY_OBJECT
#define HAVE_DWG_ADD_RAY
#define HAVE_DWG_ADD_REGION
#define HAVE_DWG_ADD_SHAPE
#define HAVE_DWG_ADD_SOLID
#define HAVE_DWG_ADD_SPLINE
#define HAVE_DWG_ADD_STYLE
#define HAVE_DWG_ADD_TEXT
#define HAVE_DWG_ADD_TOLERANCE
#define HAVE_DWG_ADD_TRACE
#define HAVE_DWG_ADD_UCS
#define HAVE_DWG_ADD_VBA_PROJECT
#define HAVE_DWG_ADD_VIEW
#define HAVE_DWG_ADD_VIEWPORT
#define HAVE_DWG_ADD_VPORT
#define HAVE_DWG_ADD_VX
#define HAVE_DWG_ADD_XLINE
#define HAVE_DWG_ADD_XRECORD

  /* Some geometric helpers */

  /* in_radians */
  EXPORT double dwg_geom_angle_normalize (double angle);

  EXPORT dwg_point_3d *
  dwg_geom_normalize (dwg_point_3d *out, const dwg_point_3d pt) __nonnull_all;

  EXPORT dwg_point_3d *dwg_geom_cross (dwg_point_3d *out,
                                       const dwg_point_3d pt1,
                                       const dwg_point_3d pt2) __nonnull_all;

  /* Transform a 3D point via its OCS (extrusion or normal) */
  EXPORT dwg_point_3d *
  dwg_geom_transform_OCS (dwg_point_3d *out, const dwg_point_3d pt,
                          const dwg_point_3d ext) __nonnull_all;

  /* Generic version api */
  EXPORT const char *dwg_api_version_string (void); // ie PACKAGE_VERSION
  EXPORT int dwg_api_version (void);                // ie 100*major + minor
  EXPORT int dwg_api_version_major (void); // ie LIBREDWG_VERSION_MAJOR
  EXPORT int dwg_api_version_minor (void); // ie LIBREDWG_VERSION_MINOR
  /* if PACKAGE_VERSION == release tag only, and configured with
   * --enable-release */
  EXPORT bool dwg_api_version_is_release (void);
  EXPORT const char *dwg_api_so_version (void);

#ifdef __cplusplus
}
#endif

#endif

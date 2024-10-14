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
     && !defined(__ICC))                                                      \
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
  typedef struct _dwg_entity__3DFACE dwg_ent__3dface;
  typedef struct _dwg_entity__3DSOLID dwg_ent__3dsolid;
  typedef struct _dwg_entity_ARC dwg_ent_arc;
  typedef struct _dwg_entity_ATTDEF dwg_ent_attdef;
  typedef struct _dwg_entity_ATTRIB dwg_ent_attrib;
  typedef struct _dwg_entity_BLOCK dwg_ent_block;
  typedef struct _dwg_entity_CIRCLE dwg_ent_circle;
  typedef struct _dwg_entity_DIMENSION_ALIGNED dwg_ent_dim_aligned;
  typedef struct _dwg_entity_DIMENSION_ANG2LN dwg_ent_dim_ang2ln;
  typedef struct _dwg_entity_DIMENSION_ANG3PT dwg_ent_dim_ang3pt;
  typedef struct _dwg_entity_DIMENSION_DIAMETER dwg_ent_dim_diameter;
  typedef struct _dwg_entity_DIMENSION_LINEAR dwg_ent_dim_linear;
  typedef struct _dwg_entity_DIMENSION_ORDINATE dwg_ent_dim_ordinate;
  typedef struct _dwg_entity_DIMENSION_RADIUS dwg_ent_dim_radius;
  typedef struct _dwg_entity_ELLIPSE dwg_ent_ellipse;
  typedef struct _dwg_entity_ENDBLK dwg_ent_endblk;
  typedef struct _dwg_entity_INSERT dwg_ent_insert;
  typedef struct _dwg_entity_LEADER dwg_ent_leader;
  typedef struct _dwg_entity_LINE dwg_ent_line;
  typedef struct _dwg_entity_LOAD dwg_ent_load;
  typedef struct _dwg_entity_MINSERT dwg_ent_minsert;
  typedef struct _dwg_entity_MLINE dwg_ent_mline;
  typedef struct _dwg_entity_MTEXT dwg_ent_mtext;
  typedef struct _dwg_entity_OLEFRAME dwg_ent_oleframe;
  typedef struct _dwg_entity_POINT dwg_ent_point;
  typedef struct _dwg_entity_POLYLINE_2D dwg_ent_polyline_2d;
  typedef struct _dwg_entity_POLYLINE_3D dwg_ent_polyline_3d;
  typedef struct _dwg_entity_POLYLINE_MESH dwg_ent_polyline_mesh;
  typedef struct _dwg_entity_POLYLINE_PFACE dwg_ent_polyline_pface;
  typedef struct _dwg_entity_PROXY_ENTITY dwg_ent_proxy_entity;
  typedef struct _dwg_entity_RAY dwg_ent_ray;
  typedef struct _dwg_entity_SEQEND dwg_ent_seqend;
  typedef struct _dwg_entity_SHAPE dwg_ent_shape;
  typedef struct _dwg_entity_SOLID dwg_ent_solid;
  typedef struct _dwg_entity_SPLINE dwg_ent_spline;
  typedef struct _dwg_entity_TEXT dwg_ent_text;
  typedef struct _dwg_entity_TOLERANCE dwg_ent_tolerance;
  typedef struct _dwg_entity_TRACE dwg_ent_trace;
  typedef struct _dwg_entity_UNKNOWN_ENT dwg_ent_unknown_ent;
  typedef struct _dwg_entity_VERTEX_2D dwg_ent_vertex_2d;
  typedef struct _dwg_entity_VERTEX_3D dwg_ent_vertex_3d;
  typedef struct _dwg_entity_VERTEX_PFACE_FACE dwg_ent_vert_pface_face;
  typedef struct _dwg_entity_VIEWPORT dwg_ent_viewport;
  /* untyped > 500 */
  typedef struct _dwg_entity_3DLINE dwg_ent__3dline;
  typedef struct _dwg_entity_CAMERA dwg_ent_camera;
  typedef struct _dwg_entity_DGNUNDERLAY dwg_ent_dgnunderlay;
  typedef struct _dwg_entity_DWFUNDERLAY dwg_ent_dwfunderlay;
  typedef struct _dwg_entity_ENDREP dwg_ent_endrep;
  typedef struct _dwg_entity_HATCH dwg_ent_hatch;
  typedef struct _dwg_entity_IMAGE dwg_ent_image;
  typedef struct _dwg_entity_JUMP dwg_ent_jump;
  typedef struct _dwg_entity_LIGHT dwg_ent_light;
  typedef struct _dwg_entity_LWPOLYLINE dwg_ent_lwpline;
  typedef struct _dwg_entity_MESH dwg_ent_mesh;
  typedef struct _dwg_entity_MULTILEADER dwg_ent_mleader;
  typedef struct _dwg_entity_OLE2FRAME dwg_ent_ole2frame;
  typedef struct _dwg_entity_PDFUNDERLAY dwg_ent_pdfunderlay;
  typedef struct _dwg_entity_REPEAT dwg_ent_repeat;
  typedef struct _dwg_entity_SECTIONOBJECT dwg_ent_sectionobject;
  /* unstable */
  typedef struct _dwg_entity_ARC_DIMENSION dwg_ent_arc_dimension;
  typedef struct _dwg_entity_HELIX dwg_ent_helix;
  typedef struct _dwg_entity_LARGE_RADIAL_DIMENSION
      dwg_ent_large_radial_dimension;
  typedef struct _dwg_entity_PLANESURFACE dwg_ent_planesurface;
  typedef struct _dwg_entity_POINTCLOUD dwg_ent_pointcloud;
  typedef struct _dwg_entity_POINTCLOUDEX dwg_ent_pointcloudex;
  typedef struct _dwg_entity_WIPEOUT dwg_ent_wipeout;
  /* debugging */
  typedef struct _dwg_entity_ALIGNMENTPARAMETERENTITY
      dwg_ent_alignmentparameterentity;
  typedef struct _dwg_entity_ARCALIGNEDTEXT dwg_ent_arcalignedtext;
  typedef struct _dwg_entity_BASEPOINTPARAMETERENTITY
      dwg_ent_basepointparameterentity;
  typedef struct _dwg_entity_EXTRUDEDSURFACE dwg_ent_extrudedsurface;
  typedef struct _dwg_entity_FLIPGRIPENTITY dwg_ent_flipgripentity;
  typedef struct _dwg_entity_FLIPPARAMETERENTITY dwg_ent_flipparameterentity;
  typedef struct _dwg_entity_GEOPOSITIONMARKER dwg_ent_geopositionmarker;
  typedef struct _dwg_entity_LAYOUTPRINTCONFIG dwg_ent_layoutprintconfig;
  typedef struct _dwg_entity_LINEARGRIPENTITY dwg_ent_lineargripentity;
  typedef struct _dwg_entity_LINEARPARAMETERENTITY
      dwg_ent_linearparameterentity;
  typedef struct _dwg_entity_LOFTEDSURFACE dwg_ent_loftedsurface;
  typedef struct _dwg_entity_MPOLYGON dwg_ent_mpolygon;
  typedef struct _dwg_entity_NAVISWORKSMODEL dwg_ent_navisworksmodel;
  typedef struct _dwg_entity_NURBSURFACE dwg_ent_nurbsurface;
  typedef struct _dwg_entity_POINTPARAMETERENTITY dwg_ent_pointparameterentity;
  typedef struct _dwg_entity_POLARGRIPENTITY dwg_ent_polargripentity;
  typedef struct _dwg_entity_REVOLVEDSURFACE dwg_ent_revolvedsurface;
  typedef struct _dwg_entity_ROTATIONGRIPENTITY dwg_ent_rotationgripentity;
  typedef struct _dwg_entity_ROTATIONPARAMETERENTITY
      dwg_ent_rotationparameterentity;
  typedef struct _dwg_entity_RTEXT dwg_ent_rtext;
  typedef struct _dwg_entity_SWEPTSURFACE dwg_ent_sweptsurface;
  typedef struct _dwg_entity_TABLE dwg_ent_table;
  typedef struct _dwg_entity_VISIBILITYGRIPENTITY dwg_ent_visibilitygripentity;
  typedef struct _dwg_entity_VISIBILITYPARAMETERENTITY
      dwg_ent_visibilityparameterentity;
  typedef struct _dwg_entity_XYGRIPENTITY dwg_ent_xygripentity;
  typedef struct _dwg_entity_XYPARAMETERENTITY dwg_ent_xyparameterentity;
  typedef struct _dwg_object_APPID dwg_obj_appid;
  typedef struct _dwg_object_APPID_CONTROL dwg_obj_appid_control;
  typedef struct _dwg_object_BLOCK_CONTROL dwg_obj_block_control;
  typedef struct _dwg_object_BLOCK_HEADER dwg_obj_block_header;
  typedef struct _dwg_object_DICTIONARY dwg_obj_dictionary;
  typedef struct _dwg_object_DIMSTYLE dwg_obj_dimstyle;
  typedef struct _dwg_object_DIMSTYLE_CONTROL dwg_obj_dimstyle_control;
  typedef struct _dwg_object_DUMMY dwg_obj_dummy;
  typedef struct _dwg_object_LAYER dwg_obj_layer;
  typedef struct _dwg_object_LAYER_CONTROL dwg_obj_layer_control;
  typedef struct _dwg_object_LONG_TRANSACTION dwg_obj_long_transaction;
  typedef struct _dwg_object_LTYPE dwg_obj_ltype;
  typedef struct _dwg_object_LTYPE_CONTROL dwg_obj_ltype_control;
  typedef struct _dwg_object_MLINESTYLE dwg_obj_mlinestyle;
  typedef struct _dwg_object_STYLE dwg_obj_style;
  typedef struct _dwg_object_STYLE_CONTROL dwg_obj_style_control;
  typedef struct _dwg_object_UCS dwg_obj_ucs;
  typedef struct _dwg_object_UCS_CONTROL dwg_obj_ucs_control;
  typedef struct _dwg_object_UNKNOWN_OBJ dwg_obj_unknown_obj;
  typedef struct _dwg_object_VIEW dwg_obj_view;
  typedef struct _dwg_object_VIEW_CONTROL dwg_obj_view_control;
  typedef struct _dwg_object_VPORT dwg_obj_vport;
  typedef struct _dwg_object_VPORT_CONTROL dwg_obj_vport_control;
  typedef struct _dwg_object_VX_CONTROL dwg_obj_vx_control;
  typedef struct _dwg_object_VX_TABLE_RECORD dwg_obj_vx_table_record;
  /* untyped > 500 */
  typedef struct _dwg_object_ACSH_BOOLEAN_CLASS dwg_obj_acsh_boolean_class;
  typedef struct _dwg_object_ACSH_BOX_CLASS dwg_obj_acsh_box_class;
  typedef struct _dwg_object_ACSH_CONE_CLASS dwg_obj_acsh_cone_class;
  typedef struct _dwg_object_ACSH_CYLINDER_CLASS dwg_obj_acsh_cylinder_class;
  typedef struct _dwg_object_ACSH_FILLET_CLASS dwg_obj_acsh_fillet_class;
  typedef struct _dwg_object_ACSH_HISTORY_CLASS dwg_obj_acsh_history_class;
  typedef struct _dwg_object_ACSH_SPHERE_CLASS dwg_obj_acsh_sphere_class;
  typedef struct _dwg_object_ACSH_TORUS_CLASS dwg_obj_acsh_torus_class;
  typedef struct _dwg_object_ACSH_WEDGE_CLASS dwg_obj_acsh_wedge_class;
  typedef struct _dwg_object_BLOCKALIGNMENTGRIP dwg_obj_blockalignmentgrip;
  typedef struct _dwg_object_BLOCKALIGNMENTPARAMETER
      dwg_obj_blockalignmentparameter;
  typedef struct _dwg_object_BLOCKBASEPOINTPARAMETER
      dwg_obj_blockbasepointparameter;
  typedef struct _dwg_object_BLOCKFLIPACTION dwg_obj_blockflipaction;
  typedef struct _dwg_object_BLOCKFLIPGRIP dwg_obj_blockflipgrip;
  typedef struct _dwg_object_BLOCKFLIPPARAMETER dwg_obj_blockflipparameter;
  typedef struct _dwg_object_BLOCKGRIPLOCATIONCOMPONENT
      dwg_obj_blockgriplocationcomponent;
  typedef struct _dwg_object_BLOCKLINEARGRIP dwg_obj_blocklineargrip;
  typedef struct _dwg_object_BLOCKLOOKUPGRIP dwg_obj_blocklookupgrip;
  typedef struct _dwg_object_BLOCKMOVEACTION dwg_obj_blockmoveaction;
  typedef struct _dwg_object_BLOCKROTATEACTION dwg_obj_blockrotateaction;
  typedef struct _dwg_object_BLOCKROTATIONGRIP dwg_obj_blockrotationgrip;
  typedef struct _dwg_object_BLOCKSCALEACTION dwg_obj_blockscaleaction;
  typedef struct _dwg_object_BLOCKVISIBILITYGRIP dwg_obj_blockvisibilitygrip;
  typedef struct _dwg_object_CELLSTYLEMAP dwg_obj_cellstylemap;
  typedef struct _dwg_object_DETAILVIEWSTYLE dwg_obj_detailviewstyle;
  typedef struct _dwg_object_DICTIONARYVAR dwg_obj_dictionaryvar;
  typedef struct _dwg_object_DICTIONARYWDFLT dwg_obj_dictionarywdflt;
  typedef struct _dwg_object_DYNAMICBLOCKPURGEPREVENTER
      dwg_obj_dynamicblockpurgepreventer;
  typedef struct _dwg_object_FIELD dwg_obj_field;
  typedef struct _dwg_object_FIELDLIST dwg_obj_fieldlist;
  typedef struct _dwg_object_GEODATA dwg_obj_geodata;
  typedef struct _dwg_object_GROUP dwg_obj_group;
  typedef struct _dwg_object_IDBUFFER dwg_obj_idbuffer;
  typedef struct _dwg_object_IMAGEDEF dwg_obj_imagedef;
  typedef struct _dwg_object_IMAGEDEF_REACTOR dwg_obj_imagedef_reactor;
  typedef struct _dwg_object_INDEX dwg_obj_index;
  typedef struct _dwg_object_LAYERFILTER dwg_obj_layerfilter;
  typedef struct _dwg_object_LAYER_INDEX dwg_obj_layer_index;
  typedef struct _dwg_object_LAYOUT dwg_obj_layout;
  typedef struct _dwg_object_PLACEHOLDER dwg_obj_placeholder;
  typedef struct _dwg_object_PLOTSETTINGS dwg_obj_plotsettings;
  typedef struct _dwg_object_RASTERVARIABLES dwg_obj_rastervariables;
  typedef struct _dwg_object_SCALE dwg_obj_scale;
  typedef struct _dwg_object_SECTIONVIEWSTYLE dwg_obj_sectionviewstyle;
  typedef struct _dwg_object_SECTION_MANAGER dwg_obj_section_manager;
  typedef struct _dwg_object_SORTENTSTABLE dwg_obj_sortentstable;
  typedef struct _dwg_object_SPATIAL_FILTER dwg_obj_spatial_filter;
  typedef struct _dwg_object_TABLEGEOMETRY dwg_obj_tablegeometry;
  typedef struct _dwg_object_VBA_PROJECT dwg_obj_vba_project;
  typedef struct _dwg_object_VISUALSTYLE dwg_obj_visualstyle;
  typedef struct _dwg_object_WIPEOUTVARIABLES dwg_obj_wipeoutvariables;
  typedef struct _dwg_object_XRECORD dwg_obj_xrecord;
  typedef struct _dwg_abstractobject_UNDERLAYDEFINITION dwg_obj_pdfdefinition;
  typedef struct _dwg_abstractobject_UNDERLAYDEFINITION dwg_obj_dgndefinition;
  typedef struct _dwg_abstractobject_UNDERLAYDEFINITION dwg_obj_dwfdefinition;
  /* unstable */
  typedef struct _dwg_object_ACSH_BREP_CLASS dwg_obj_acsh_brep_class;
  typedef struct _dwg_object_ACSH_CHAMFER_CLASS dwg_obj_acsh_chamfer_class;
  typedef struct _dwg_object_ACSH_PYRAMID_CLASS dwg_obj_acsh_pyramid_class;
  typedef struct _dwg_object_ALDIMOBJECTCONTEXTDATA
      dwg_obj_aldimobjectcontextdata;
  typedef struct _dwg_object_ASSOC2DCONSTRAINTGROUP
      dwg_obj_assoc2dconstraintgroup;
  typedef struct _dwg_object_ASSOCACTION dwg_obj_assocaction;
  typedef struct _dwg_object_ASSOCACTIONPARAM dwg_obj_assocactionparam;
  typedef struct _dwg_object_ASSOCARRAYACTIONBODY dwg_obj_assocarrayactionbody;
  typedef struct _dwg_object_ASSOCASMBODYACTIONPARAM
      dwg_obj_assocasmbodyactionparam;
  typedef struct _dwg_object_ASSOCBLENDSURFACEACTIONBODY
      dwg_obj_assocblendsurfaceactionbody;
  typedef struct _dwg_object_ASSOCCOMPOUNDACTIONPARAM
      dwg_obj_assoccompoundactionparam;
  typedef struct _dwg_object_ASSOCDEPENDENCY dwg_obj_assocdependency;
  typedef struct _dwg_object_ASSOCDIMDEPENDENCYBODY
      dwg_obj_assocdimdependencybody;
  typedef struct _dwg_object_ASSOCEXTENDSURFACEACTIONBODY
      dwg_obj_assocextendsurfaceactionbody;
  typedef struct _dwg_object_ASSOCEXTRUDEDSURFACEACTIONBODY
      dwg_obj_assocextrudedsurfaceactionbody;
  typedef struct _dwg_object_ASSOCFACEACTIONPARAM dwg_obj_assocfaceactionparam;
  typedef struct _dwg_object_ASSOCFILLETSURFACEACTIONBODY
      dwg_obj_assocfilletsurfaceactionbody;
  typedef struct _dwg_object_ASSOCGEOMDEPENDENCY dwg_obj_assocgeomdependency;
  typedef struct _dwg_object_ASSOCLOFTEDSURFACEACTIONBODY
      dwg_obj_assocloftedsurfaceactionbody;
  typedef struct _dwg_object_ASSOCNETWORK dwg_obj_assocnetwork;
  typedef struct _dwg_object_ASSOCNETWORKSURFACEACTIONBODY
      dwg_obj_assocnetworksurfaceactionbody;
  typedef struct _dwg_object_ASSOCOBJECTACTIONPARAM
      dwg_obj_assocobjectactionparam;
  typedef struct _dwg_object_ASSOCOFFSETSURFACEACTIONBODY
      dwg_obj_assocoffsetsurfaceactionbody;
  typedef struct _dwg_object_ASSOCOSNAPPOINTREFACTIONPARAM
      dwg_obj_assocosnappointrefactionparam;
  typedef struct _dwg_object_ASSOCPATCHSURFACEACTIONBODY
      dwg_obj_assocpatchsurfaceactionbody;
  typedef struct _dwg_object_ASSOCPATHACTIONPARAM dwg_obj_assocpathactionparam;
  typedef struct _dwg_object_ASSOCPLANESURFACEACTIONBODY
      dwg_obj_assocplanesurfaceactionbody;
  typedef struct _dwg_object_ASSOCPOINTREFACTIONPARAM
      dwg_obj_assocpointrefactionparam;
  typedef struct _dwg_object_ASSOCREVOLVEDSURFACEACTIONBODY
      dwg_obj_assocrevolvedsurfaceactionbody;
  typedef struct _dwg_object_ASSOCTRIMSURFACEACTIONBODY
      dwg_obj_assoctrimsurfaceactionbody;
  typedef struct _dwg_object_ASSOCVALUEDEPENDENCY dwg_obj_assocvaluedependency;
  typedef struct _dwg_object_ASSOCVARIABLE dwg_obj_assocvariable;
  typedef struct _dwg_object_ASSOCVERTEXACTIONPARAM
      dwg_obj_assocvertexactionparam;
  typedef struct _dwg_object_BLKREFOBJECTCONTEXTDATA
      dwg_obj_blkrefobjectcontextdata;
  typedef struct _dwg_object_BLOCKALIGNEDCONSTRAINTPARAMETER
      dwg_obj_blockalignedconstraintparameter;
  typedef struct _dwg_object_BLOCKANGULARCONSTRAINTPARAMETER
      dwg_obj_blockangularconstraintparameter;
  typedef struct _dwg_object_BLOCKARRAYACTION dwg_obj_blockarrayaction;
  typedef struct _dwg_object_BLOCKDIAMETRICCONSTRAINTPARAMETER
      dwg_obj_blockdiametricconstraintparameter;
  typedef struct _dwg_object_BLOCKHORIZONTALCONSTRAINTPARAMETER
      dwg_obj_blockhorizontalconstraintparameter;
  typedef struct _dwg_object_BLOCKLINEARCONSTRAINTPARAMETER
      dwg_obj_blocklinearconstraintparameter;
  typedef struct _dwg_object_BLOCKLINEARPARAMETER dwg_obj_blocklinearparameter;
  typedef struct _dwg_object_BLOCKLOOKUPACTION dwg_obj_blocklookupaction;
  typedef struct _dwg_object_BLOCKLOOKUPPARAMETER dwg_obj_blocklookupparameter;
  typedef struct _dwg_object_BLOCKPARAMDEPENDENCYBODY
      dwg_obj_blockparamdependencybody;
  typedef struct _dwg_object_BLOCKPOINTPARAMETER dwg_obj_blockpointparameter;
  typedef struct _dwg_object_BLOCKPOLARGRIP dwg_obj_blockpolargrip;
  typedef struct _dwg_object_BLOCKPOLARPARAMETER dwg_obj_blockpolarparameter;
  typedef struct _dwg_object_BLOCKPOLARSTRETCHACTION
      dwg_obj_blockpolarstretchaction;
  typedef struct _dwg_object_BLOCKRADIALCONSTRAINTPARAMETER
      dwg_obj_blockradialconstraintparameter;
  typedef struct _dwg_object_BLOCKREPRESENTATION dwg_obj_blockrepresentation;
  typedef struct _dwg_object_BLOCKROTATIONPARAMETER
      dwg_obj_blockrotationparameter;
  typedef struct _dwg_object_BLOCKSTRETCHACTION dwg_obj_blockstretchaction;
  typedef struct _dwg_object_BLOCKUSERPARAMETER dwg_obj_blockuserparameter;
  typedef struct _dwg_object_BLOCKVERTICALCONSTRAINTPARAMETER
      dwg_obj_blockverticalconstraintparameter;
  typedef struct _dwg_object_BLOCKVISIBILITYPARAMETER
      dwg_obj_blockvisibilityparameter;
  typedef struct _dwg_object_BLOCKXYGRIP dwg_obj_blockxygrip;
  typedef struct _dwg_object_BLOCKXYPARAMETER dwg_obj_blockxyparameter;
  typedef struct _dwg_object_DATALINK dwg_obj_datalink;
  typedef struct _dwg_object_DBCOLOR dwg_obj_dbcolor;
  typedef struct _dwg_object_EVALUATION_GRAPH dwg_obj_evaluation_graph;
  typedef struct _dwg_object_FCFOBJECTCONTEXTDATA dwg_obj_fcfobjectcontextdata;
  typedef struct _dwg_object_GRADIENT_BACKGROUND dwg_obj_gradient_background;
  typedef struct _dwg_object_GROUND_PLANE_BACKGROUND
      dwg_obj_ground_plane_background;
  typedef struct _dwg_object_IBL_BACKGROUND dwg_obj_ibl_background;
  typedef struct _dwg_object_IMAGE_BACKGROUND dwg_obj_image_background;
  typedef struct _dwg_object_LEADEROBJECTCONTEXTDATA
      dwg_obj_leaderobjectcontextdata;
  typedef struct _dwg_object_LIGHTLIST dwg_obj_lightlist;
  typedef struct _dwg_object_MATERIAL dwg_obj_material;
  typedef struct _dwg_object_MENTALRAYRENDERSETTINGS
      dwg_obj_mentalrayrendersettings;
  typedef struct _dwg_object_MLEADERSTYLE dwg_obj_mleaderstyle;
  typedef struct _dwg_object_MTEXTOBJECTCONTEXTDATA
      dwg_obj_mtextobjectcontextdata;
  typedef struct _dwg_object_OBJECT_PTR dwg_obj_object_ptr;
  typedef struct _dwg_object_PARTIAL_VIEWING_INDEX
      dwg_obj_partial_viewing_index;
  typedef struct _dwg_object_POINTCLOUDCOLORMAP dwg_obj_pointcloudcolormap;
  typedef struct _dwg_object_POINTCLOUDDEF dwg_obj_pointclouddef;
  typedef struct _dwg_object_POINTCLOUDDEFEX dwg_obj_pointclouddefex;
  typedef struct _dwg_object_POINTCLOUDDEF_REACTOR
      dwg_obj_pointclouddef_reactor;
  typedef struct _dwg_object_POINTCLOUDDEF_REACTOR_EX
      dwg_obj_pointclouddef_reactor_ex;
  typedef struct _dwg_object_PROXY_OBJECT dwg_obj_proxy_object;
  typedef struct _dwg_object_RAPIDRTRENDERSETTINGS
      dwg_obj_rapidrtrendersettings;
  typedef struct _dwg_object_RENDERENTRY dwg_obj_renderentry;
  typedef struct _dwg_object_RENDERENVIRONMENT dwg_obj_renderenvironment;
  typedef struct _dwg_object_RENDERGLOBAL dwg_obj_renderglobal;
  typedef struct _dwg_object_RENDERSETTINGS dwg_obj_rendersettings;
  typedef struct _dwg_object_SECTION_SETTINGS dwg_obj_section_settings;
  typedef struct _dwg_object_SKYLIGHT_BACKGROUND dwg_obj_skylight_background;
  typedef struct _dwg_object_SOLID_BACKGROUND dwg_obj_solid_background;
  typedef struct _dwg_object_SPATIAL_INDEX dwg_obj_spatial_index;
  typedef struct _dwg_object_SUN dwg_obj_sun;
  typedef struct _dwg_object_TABLESTYLE dwg_obj_tablestyle;
  typedef struct _dwg_object_TEXTOBJECTCONTEXTDATA
      dwg_obj_textobjectcontextdata;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS
      dwg_obj_assocarraymodifyparameters;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS
      dwg_obj_assocarraypathparameters;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS
      dwg_obj_assocarraypolarparameters;
  typedef struct _dwg_abstractobject_ASSOCARRAYPARAMETERS
      dwg_obj_assocarrayrectangularparameters;
  /* debugging */
  typedef struct _dwg_object_ACMECOMMANDHISTORY dwg_obj_acmecommandhistory;
  typedef struct _dwg_object_ACMESCOPE dwg_obj_acmescope;
  typedef struct _dwg_object_ACMESTATEMGR dwg_obj_acmestatemgr;
  typedef struct _dwg_object_ACSH_EXTRUSION_CLASS dwg_obj_acsh_extrusion_class;
  typedef struct _dwg_object_ACSH_LOFT_CLASS dwg_obj_acsh_loft_class;
  typedef struct _dwg_object_ACSH_REVOLVE_CLASS dwg_obj_acsh_revolve_class;
  typedef struct _dwg_object_ACSH_SWEEP_CLASS dwg_obj_acsh_sweep_class;
  typedef struct _dwg_object_ANGDIMOBJECTCONTEXTDATA
      dwg_obj_angdimobjectcontextdata;
  typedef struct _dwg_object_ANNOTSCALEOBJECTCONTEXTDATA
      dwg_obj_annotscaleobjectcontextdata;
  typedef struct _dwg_object_ASSOC3POINTANGULARDIMACTIONBODY
      dwg_obj_assoc3pointangulardimactionbody;
  typedef struct _dwg_object_ASSOCALIGNEDDIMACTIONBODY
      dwg_obj_assocaligneddimactionbody;
  typedef struct _dwg_object_ASSOCARRAYMODIFYACTIONBODY
      dwg_obj_assocarraymodifyactionbody;
  typedef struct _dwg_object_ASSOCEDGEACTIONPARAM dwg_obj_assocedgeactionparam;
  typedef struct _dwg_object_ASSOCEDGECHAMFERACTIONBODY
      dwg_obj_assocedgechamferactionbody;
  typedef struct _dwg_object_ASSOCEDGEFILLETACTIONBODY
      dwg_obj_assocedgefilletactionbody;
  typedef struct _dwg_object_ASSOCMLEADERACTIONBODY
      dwg_obj_assocmleaderactionbody;
  typedef struct _dwg_object_ASSOCORDINATEDIMACTIONBODY
      dwg_obj_assocordinatedimactionbody;
  typedef struct _dwg_object_ASSOCPERSSUBENTMANAGER
      dwg_obj_assocperssubentmanager;
  typedef struct _dwg_object_ASSOCRESTOREENTITYSTATEACTIONBODY
      dwg_obj_assocrestoreentitystateactionbody;
  typedef struct _dwg_object_ASSOCROTATEDDIMACTIONBODY
      dwg_obj_assocrotateddimactionbody;
  typedef struct _dwg_object_ASSOCSWEPTSURFACEACTIONBODY
      dwg_obj_assocsweptsurfaceactionbody;
  typedef struct _dwg_object_BLOCKPROPERTIESTABLE dwg_obj_blockpropertiestable;
  typedef struct _dwg_object_BLOCKPROPERTIESTABLEGRIP
      dwg_obj_blockpropertiestablegrip;
  typedef struct _dwg_object_BREAKDATA dwg_obj_breakdata;
  typedef struct _dwg_object_BREAKPOINTREF dwg_obj_breakpointref;
  typedef struct _dwg_object_CONTEXTDATAMANAGER dwg_obj_contextdatamanager;
  typedef struct _dwg_object_CSACDOCUMENTOPTIONS dwg_obj_csacdocumentoptions;
  typedef struct _dwg_object_CURVEPATH dwg_obj_curvepath;
  typedef struct _dwg_object_DATATABLE dwg_obj_datatable;
  typedef struct _dwg_object_DIMASSOC dwg_obj_dimassoc;
  typedef struct _dwg_object_DMDIMOBJECTCONTEXTDATA
      dwg_obj_dmdimobjectcontextdata;
  typedef struct _dwg_object_DYNAMICBLOCKPROXYNODE
      dwg_obj_dynamicblockproxynode;
  typedef struct _dwg_object_GEOMAPIMAGE dwg_obj_geomapimage;
  typedef struct _dwg_object_MLEADEROBJECTCONTEXTDATA
      dwg_obj_mleaderobjectcontextdata;
  typedef struct _dwg_object_MOTIONPATH dwg_obj_motionpath;
  typedef struct _dwg_object_MTEXTATTRIBUTEOBJECTCONTEXTDATA
      dwg_obj_mtextattributeobjectcontextdata;
  typedef struct _dwg_object_NAVISWORKSMODELDEF dwg_obj_navisworksmodeldef;
  typedef struct _dwg_object_ORDDIMOBJECTCONTEXTDATA
      dwg_obj_orddimobjectcontextdata;
  typedef struct _dwg_object_PERSUBENTMGR dwg_obj_persubentmgr;
  typedef struct _dwg_object_POINTPATH dwg_obj_pointpath;
  typedef struct _dwg_object_RADIMLGOBJECTCONTEXTDATA
      dwg_obj_radimlgobjectcontextdata;
  typedef struct _dwg_object_RADIMOBJECTCONTEXTDATA
      dwg_obj_radimobjectcontextdata;
  typedef struct _dwg_object_SUNSTUDY dwg_obj_sunstudy;
  typedef struct _dwg_object_TABLECONTENT dwg_obj_tablecontent;
  typedef struct _dwg_object_TVDEVICEPROPERTIES dwg_obj_tvdeviceproperties;
  // typedef struct _dwg_object_ACDSRECORD		dwg_obj_acdsrecord;
  // typedef struct _dwg_object_ACDSSCHEMA		dwg_obj_acdsschema;
  // typedef struct _dwg_object_NPOCOLLECTION		dwg_obj_npocollection;
  // typedef struct _dwg_object_RAPIDRTRENDERENVIRONMENT
  // dwg_obj_rapidrtrenderenvironment; typedef struct
  // _dwg_object_XREFPANELOBJECT		dwg_obj_xrefpanelobject;

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
  /* unstable */
  dwg_get_OBJECT_DECL (ent_arc_dimension, ARC_DIMENSION);
  dwg_get_OBJECT_DECL (ent_helix, HELIX);
  dwg_get_OBJECT_DECL (ent_large_radial_dimension, LARGE_RADIAL_DIMENSION);
  dwg_get_OBJECT_DECL (ent_layoutprintconfig, LAYOUTPRINTCONFIG);
  dwg_get_OBJECT_DECL (ent_planesurface, PLANESURFACE);
  dwg_get_OBJECT_DECL (ent_pointcloud, POINTCLOUD);
  dwg_get_OBJECT_DECL (ent_pointcloudex, POINTCLOUDEX);
  dwg_get_OBJECT_DECL (ent_wipeout, WIPEOUT);
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
  dwg_get_OBJECT_DECL (ent_visibilityparameterentity,
                       VISIBILITYPARAMETERENTITY);
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
  dwg_get_OBJECT_DECL (obj_blockalignmentgrip, BLOCKALIGNMENTGRIP);
  dwg_get_OBJECT_DECL (obj_blockalignmentparameter, BLOCKALIGNMENTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockbasepointparameter, BLOCKBASEPOINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockflipaction, BLOCKFLIPACTION);
  dwg_get_OBJECT_DECL (obj_blockflipgrip, BLOCKFLIPGRIP);
  dwg_get_OBJECT_DECL (obj_blockflipparameter, BLOCKFLIPPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockgriplocationcomponent,
                       BLOCKGRIPLOCATIONCOMPONENT);
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
  dwg_get_OBJECT_DECL (obj_dynamicblockpurgepreventer,
                       DYNAMICBLOCKPURGEPREVENTER);
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
  dwg_get_OBJECT_DECL (obj_scale, SCALE);
  dwg_get_OBJECT_DECL (obj_sectionviewstyle, SECTIONVIEWSTYLE);
  dwg_get_OBJECT_DECL (obj_section_manager, SECTION_MANAGER);
  dwg_get_OBJECT_DECL (obj_sortentstable, SORTENTSTABLE);
  dwg_get_OBJECT_DECL (obj_spatial_filter, SPATIAL_FILTER);
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
  dwg_get_OBJECT_DECL (obj_assocblendsurfaceactionbody,
                       ASSOCBLENDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assoccompoundactionparam, ASSOCCOMPOUNDACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocdependency, ASSOCDEPENDENCY);
  dwg_get_OBJECT_DECL (obj_assocdimdependencybody, ASSOCDIMDEPENDENCYBODY);
  dwg_get_OBJECT_DECL (obj_assocextendsurfaceactionbody,
                       ASSOCEXTENDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocextrudedsurfaceactionbody,
                       ASSOCEXTRUDEDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocfaceactionparam, ASSOCFACEACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocfilletsurfaceactionbody,
                       ASSOCFILLETSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocgeomdependency, ASSOCGEOMDEPENDENCY);
  dwg_get_OBJECT_DECL (obj_assocloftedsurfaceactionbody,
                       ASSOCLOFTEDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocnetwork, ASSOCNETWORK);
  dwg_get_OBJECT_DECL (obj_assocnetworksurfaceactionbody,
                       ASSOCNETWORKSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocobjectactionparam, ASSOCOBJECTACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocoffsetsurfaceactionbody,
                       ASSOCOFFSETSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocosnappointrefactionparam,
                       ASSOCOSNAPPOINTREFACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocpatchsurfaceactionbody,
                       ASSOCPATCHSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocpathactionparam, ASSOCPATHACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocplanesurfaceactionbody,
                       ASSOCPLANESURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocpointrefactionparam, ASSOCPOINTREFACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocrevolvedsurfaceactionbody,
                       ASSOCREVOLVEDSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assoctrimsurfaceactionbody,
                       ASSOCTRIMSURFACEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocvaluedependency, ASSOCVALUEDEPENDENCY);
  dwg_get_OBJECT_DECL (obj_assocvariable, ASSOCVARIABLE);
  dwg_get_OBJECT_DECL (obj_assocvertexactionparam, ASSOCVERTEXACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_blkrefobjectcontextdata, BLKREFOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_blockalignedconstraintparameter,
                       BLOCKALIGNEDCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockangularconstraintparameter,
                       BLOCKANGULARCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockarrayaction, BLOCKARRAYACTION);
  dwg_get_OBJECT_DECL (obj_blockdiametricconstraintparameter,
                       BLOCKDIAMETRICCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockhorizontalconstraintparameter,
                       BLOCKHORIZONTALCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blocklinearconstraintparameter,
                       BLOCKLINEARCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blocklinearparameter, BLOCKLINEARPARAMETER);
  dwg_get_OBJECT_DECL (obj_blocklookupaction, BLOCKLOOKUPACTION);
  dwg_get_OBJECT_DECL (obj_blocklookupparameter, BLOCKLOOKUPPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockparamdependencybody, BLOCKPARAMDEPENDENCYBODY);
  dwg_get_OBJECT_DECL (obj_blockpointparameter, BLOCKPOINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockpolargrip, BLOCKPOLARGRIP);
  dwg_get_OBJECT_DECL (obj_blockpolarparameter, BLOCKPOLARPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockpolarstretchaction, BLOCKPOLARSTRETCHACTION);
  dwg_get_OBJECT_DECL (obj_blockradialconstraintparameter,
                       BLOCKRADIALCONSTRAINTPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockrepresentation, BLOCKREPRESENTATION);
  dwg_get_OBJECT_DECL (obj_blockrotationparameter, BLOCKROTATIONPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockstretchaction, BLOCKSTRETCHACTION);
  dwg_get_OBJECT_DECL (obj_blockuserparameter, BLOCKUSERPARAMETER);
  dwg_get_OBJECT_DECL (obj_blockverticalconstraintparameter,
                       BLOCKVERTICALCONSTRAINTPARAMETER);
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
  dwg_get_OBJECT_DECL (obj_renderenvironment, RENDERENVIRONMENT);
  dwg_get_OBJECT_DECL (obj_renderglobal, RENDERGLOBAL);
  dwg_get_OBJECT_DECL (obj_rendersettings, RENDERSETTINGS);
  dwg_get_OBJECT_DECL (obj_section_settings, SECTION_SETTINGS);
  dwg_get_OBJECT_DECL (obj_skylight_background, SKYLIGHT_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_solid_background, SOLID_BACKGROUND);
  dwg_get_OBJECT_DECL (obj_spatial_index, SPATIAL_INDEX);
  dwg_get_OBJECT_DECL (obj_sun, SUN);
  dwg_get_OBJECT_DECL (obj_tablestyle, TABLESTYLE);
  dwg_get_OBJECT_DECL (obj_textobjectcontextdata, TEXTOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_assocarraymodifyparameters,
                       ASSOCARRAYMODIFYPARAMETERS);
  dwg_get_OBJECT_DECL (obj_assocarraypathparameters, ASSOCARRAYPATHPARAMETERS);
  dwg_get_OBJECT_DECL (obj_assocarraypolarparameters,
                       ASSOCARRAYPOLARPARAMETERS);
  dwg_get_OBJECT_DECL (obj_assocarrayrectangularparameters,
                       ASSOCARRAYRECTANGULARPARAMETERS);
#ifdef DEBUG_CLASSES
  dwg_get_OBJECT_DECL (obj_acmecommandhistory, ACMECOMMANDHISTORY);
  dwg_get_OBJECT_DECL (obj_acmescope, ACMESCOPE);
  dwg_get_OBJECT_DECL (obj_acmestatemgr, ACMESTATEMGR);
  dwg_get_OBJECT_DECL (obj_acsh_extrusion_class, ACSH_EXTRUSION_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_loft_class, ACSH_LOFT_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_revolve_class, ACSH_REVOLVE_CLASS);
  dwg_get_OBJECT_DECL (obj_acsh_sweep_class, ACSH_SWEEP_CLASS);
  dwg_get_OBJECT_DECL (obj_angdimobjectcontextdata, ANGDIMOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_annotscaleobjectcontextdata,
                       ANNOTSCALEOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_assoc3pointangulardimactionbody,
                       ASSOC3POINTANGULARDIMACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocaligneddimactionbody,
                       ASSOCALIGNEDDIMACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocarraymodifyactionbody,
                       ASSOCARRAYMODIFYACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocedgeactionparam, ASSOCEDGEACTIONPARAM);
  dwg_get_OBJECT_DECL (obj_assocedgechamferactionbody,
                       ASSOCEDGECHAMFERACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocedgefilletactionbody,
                       ASSOCEDGEFILLETACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocmleaderactionbody, ASSOCMLEADERACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocordinatedimactionbody,
                       ASSOCORDINATEDIMACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocperssubentmanager, ASSOCPERSSUBENTMANAGER);
  dwg_get_OBJECT_DECL (obj_assocrestoreentitystateactionbody,
                       ASSOCRESTOREENTITYSTATEACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocrotateddimactionbody,
                       ASSOCROTATEDDIMACTIONBODY);
  dwg_get_OBJECT_DECL (obj_assocsweptsurfaceactionbody,
                       ASSOCSWEPTSURFACEACTIONBODY);
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
  dwg_get_OBJECT_DECL (obj_mtextattributeobjectcontextdata,
                       MTEXTATTRIBUTEOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_navisworksmodeldef, NAVISWORKSMODELDEF);
  dwg_get_OBJECT_DECL (obj_orddimobjectcontextdata, ORDDIMOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_persubentmgr, PERSUBENTMGR);
  dwg_get_OBJECT_DECL (obj_pointpath, POINTPATH);
  dwg_get_OBJECT_DECL (obj_radimlgobjectcontextdata, RADIMLGOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_radimobjectcontextdata, RADIMOBJECTCONTEXTDATA);
  dwg_get_OBJECT_DECL (obj_sunstudy, SUNSTUDY);
  dwg_get_OBJECT_DECL (obj_tablecontent, TABLECONTENT);
  dwg_get_OBJECT_DECL (obj_tvdeviceproperties, TVDEVICEPROPERTIES);
// dwg_get_OBJECT_DECL (obj_acdsrecord, ACDSRECORD);
// dwg_get_OBJECT_DECL (obj_acdsschema, ACDSSCHEMA);
// dwg_get_OBJECT_DECL (obj_npocollection, NPOCOLLECTION);
// dwg_get_OBJECT_DECL (obj_rapidrtrenderenvironment,
// RAPIDRTRENDERENVIRONMENT); dwg_get_OBJECT_DECL (obj_xrefpanelobject,
// XREFPANELOBJECT);
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
  /* unstable */
  DWG_GETALL_ENTITY_DECL (ARC_DIMENSION);
  DWG_GETALL_ENTITY_DECL (HELIX);
  DWG_GETALL_ENTITY_DECL (LARGE_RADIAL_DIMENSION);
  DWG_GETALL_ENTITY_DECL (PLANESURFACE);
  DWG_GETALL_ENTITY_DECL (POINTCLOUD);
  DWG_GETALL_ENTITY_DECL (POINTCLOUDEX);
  DWG_GETALL_ENTITY_DECL (WIPEOUT);
  /* debugging */
  DWG_GETALL_ENTITY_DECL (ALIGNMENTPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (ARCALIGNEDTEXT);
  DWG_GETALL_ENTITY_DECL (BASEPOINTPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (EXTRUDEDSURFACE);
  DWG_GETALL_ENTITY_DECL (FLIPGRIPENTITY);
  DWG_GETALL_ENTITY_DECL (FLIPPARAMETERENTITY);
  DWG_GETALL_ENTITY_DECL (GEOPOSITIONMARKER);
  DWG_GETALL_ENTITY_DECL (LAYOUTPRINTCONFIG);
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
  DWG_GETALL_OBJECT_DECL (SCALE);
  DWG_GETALL_OBJECT_DECL (SECTIONVIEWSTYLE);
  DWG_GETALL_OBJECT_DECL (SECTION_MANAGER);
  DWG_GETALL_OBJECT_DECL (SORTENTSTABLE);
  DWG_GETALL_OBJECT_DECL (SPATIAL_FILTER);
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
  DWG_GETALL_OBJECT_DECL (ASSOCGEOMDEPENDENCY);
  DWG_GETALL_OBJECT_DECL (ASSOCLOFTEDSURFACEACTIONBODY);
  DWG_GETALL_OBJECT_DECL (ASSOCNETWORK);
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
  DWG_GETALL_OBJECT_DECL (RENDERENVIRONMENT);
  DWG_GETALL_OBJECT_DECL (RENDERGLOBAL);
  DWG_GETALL_OBJECT_DECL (RENDERSETTINGS);
  DWG_GETALL_OBJECT_DECL (SECTION_SETTINGS);
  DWG_GETALL_OBJECT_DECL (SKYLIGHT_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (SOLID_BACKGROUND);
  DWG_GETALL_OBJECT_DECL (SPATIAL_INDEX);
  DWG_GETALL_OBJECT_DECL (SUN);
  DWG_GETALL_OBJECT_DECL (TABLESTYLE);
  DWG_GETALL_OBJECT_DECL (TEXTOBJECTCONTEXTDATA);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYMODIFYPARAMETERS);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYPATHPARAMETERS);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYPOLARPARAMETERS);
  DWG_GETALL_OBJECT_DECL (ASSOCARRAYRECTANGULARPARAMETERS);
#ifdef DEBUG_CLASSES
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
// DWG_GETALL_OBJECT_DECL (ACDSRECORD);
// DWG_GETALL_OBJECT_DECL (ACDSSCHEMA);
// DWG_GETALL_OBJECT_DECL (NPOCOLLECTION);
// DWG_GETALL_OBJECT_DECL (RAPIDRTRENDERENVIRONMENT);
// DWG_GETALL_OBJECT_DECL (XREFPANELOBJECT);
#endif

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
  /* unstable */
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (ARC_DIMENSION);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (HELIX);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LARGE_RADIAL_DIMENSION);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (LAYOUTPRINTCONFIG);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (PLANESURFACE);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (POINTCLOUD);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (POINTCLOUDEX);
  CAST_DWG_OBJECT_TO_ENTITY_BYNAME_DECL (WIPEOUT);
#ifdef DEBUG_CLASSES
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
#endif

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
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SCALE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SECTIONVIEWSTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SECTION_MANAGER);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SORTENTSTABLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SPATIAL_FILTER);
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
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCGEOMDEPENDENCY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCLOFTEDSURFACEACTIONBODY);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCNETWORK);
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
  CAST_DWG_OBJECT_TO_OBJECT_DECL (DIMASSOC);
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
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERENVIRONMENT);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERGLOBAL);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (RENDERSETTINGS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SECTION_SETTINGS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SKYLIGHT_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SOLID_BACKGROUND);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SPATIAL_INDEX);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (SUN);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (TABLESTYLE);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (TEXTOBJECTCONTEXTDATA);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYMODIFYPARAMETERS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYPATHPARAMETERS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYPOLARPARAMETERS);
  CAST_DWG_OBJECT_TO_OBJECT_DECL (ASSOCARRAYRECTANGULARPARAMETERS);
#ifdef DEBUG_CLASSES
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
// CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDSRECORD);
// CAST_DWG_OBJECT_TO_OBJECT_DECL (ACDSSCHEMA);
// CAST_DWG_OBJECT_TO_OBJECT_DECL (NPOCOLLECTION);
// CAST_DWG_OBJECT_TO_OBJECT_DECL (RAPIDRTRENDERENVIRONMENT);
// CAST_DWG_OBJECT_TO_OBJECT_DECL (XREFPANELOBJECT);
#endif
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
  EXPORT BITCODE_BS dwg_ent_vertex_pface_face_get_vertind (
      const dwg_ent_vert_pface_face *face) _deprecated_dynapi_getter;

  EXPORT void dwg_ent_vertex_pface_face_set_vertind (
      dwg_ent_vert_pface_face *restrict face, const BITCODE_BS vertind[4])
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

  EXPORT dwg_obj_block_header *dwg_get_block_header (dwg_data *restrict dwg,
                                                     int *restrict error)
      __nonnull_all;

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

  EXPORT dwg_object *dwg_ent_generic_to_object (const void *restrict obj,
                                                int *restrict error)
      __nonnull_all;
  EXPORT dwg_obj_ent *dwg_ent_generic_parent (const void *restrict ent,
                                              int *restrict error)
      __nonnull_all;

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

  EXPORT BITCODE_B dwg_ent_get_picture_exists (const dwg_obj_ent *restrict ent,
                                               int *restrict error)
    __nonnull_all;

  EXPORT BITCODE_BLL
  dwg_ent_get_picture_size (const dwg_obj_ent *restrict ent,
                            int *restrict error) // before r2007 only RL
      __nonnull_all;

  EXPORT BITCODE_TF dwg_ent_get_picture (const dwg_obj_ent *restrict ent,
                                         int *restrict error) __nonnull_all;

  EXPORT BITCODE_BB dwg_ent_get_entmode (const dwg_obj_ent *restrict ent,
                                         int *restrict error) __nonnull_all;

  EXPORT BITCODE_BL dwg_ent_get_num_reactors (const dwg_obj_ent *restrict ent,
                                              int *restrict error)
      __nonnull_all;

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
                                            int *restrict error)
      __nonnull_all;

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
                                           int *restrict error)
      __nonnull_all;

  /* See dxf_cvt_lweight() for the mm value */
  EXPORT BITCODE_RC dwg_ent_get_linewt (const dwg_obj_ent *restrict ent,
                                        int *restrict error) // r2000+
      __nonnull_all;

  EXPORT dwg_object_ref *
  dwg_ent_get_ownerhandle (const dwg_obj_ent *restrict ent,
                           int *restrict error) __nonnull_all;

  EXPORT dwg_object_ref **
  dwg_ent_get_reactors (const dwg_obj_ent *restrict ent, int *restrict error)
      __nonnull_all;

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
                                            int *restrict error)
      __nonnull_all;

  EXPORT dwg_object_ref *dwg_ent_get_ltype (const dwg_obj_ent *restrict ent,
                                            int *restrict error)
      __nonnull_all;

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

  EXPORT int
  dwg_ent_set_ltype (dwg_obj_ent *restrict ent, const char *restrict name)
    __nonnull_all;

  /********************************************************************
   *                    FUNCTIONS FOR DWG OBJECT                       *
   ********************************************************************/

  EXPORT dwg_object *dwg_obj_obj_to_object (const dwg_obj_obj *restrict obj,
                                            int *restrict error)
      __nonnull_all;

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
                                            int *restrict error)
      __nonnull_all;
  EXPORT BITCODE_BL dwg_obj_get_num_reactors (const dwg_obj_obj *restrict obj,
                                              int *restrict error)
      __nonnull_all;
  EXPORT BITCODE_H *dwg_obj_get_reactors (const dwg_obj_obj *restrict obj,
                                          int *restrict error) __nonnull_all;
  EXPORT BITCODE_H dwg_obj_get_xdicobjhandle (const dwg_obj_obj *restrict obj,
                                              int *restrict error)
      __nonnull_all;
  /* r2004+ */
  EXPORT BITCODE_B dwg_obj_get_is_xdic_missing (
      const dwg_obj_obj *restrict obj, int *restrict error) __nonnull_all;
  /* r2013+ */
  EXPORT BITCODE_B dwg_obj_get_has_ds_binary_data (
      const dwg_obj_obj *restrict obj, int *restrict error) __nonnull_all;
  EXPORT Dwg_Handle *dwg_obj_get_handleref (const dwg_obj_obj *restrict obj,
                                            int *restrict error)
      __nonnull_all;

  EXPORT dwg_object *dwg_obj_generic_to_object (const void *restrict obj,
                                                int *restrict error)
      __nonnull_all;
  EXPORT BITCODE_RLL dwg_obj_generic_handlevalue (void *_obj) __nonnull_all;
  Dwg_Data *dwg_obj_generic_dwg (const void *restrict obj,
                                 int *restrict error) __nonnull_all;
  EXPORT dwg_obj_obj *dwg_obj_generic_parent (const void *restrict obj,
                                              int *restrict error)
      __nonnull_all;

  EXPORT dwg_object *dwg_get_object (dwg_data *dwg, BITCODE_BL index);

  EXPORT BITCODE_RL dwg_object_get_bitsize (const dwg_object *obj);

  EXPORT BITCODE_BL dwg_object_get_index (const dwg_object *restrict obj,
                                          int *restrict error) __nonnull_all;

  EXPORT dwg_handle *dwg_object_get_handle (dwg_object *restrict obj,
                                            int *restrict error)
      __nonnull_all;

  EXPORT dwg_obj_obj *dwg_object_to_object (dwg_object *restrict obj,
                                            int *restrict error)
      __nonnull_all;

  EXPORT dwg_obj_ent *dwg_object_to_entity (dwg_object *restrict obj,
                                            int *restrict error)
      __nonnull_all;

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
  EXPORT bool
  dwg_is_valid_name (Dwg_Data *restrict dwg, const char *restrict name)
    __nonnull_all;
  // variant where input is a UTF-8 string.
  EXPORT bool
  dwg_is_valid_name_u8 (Dwg_Data *restrict dwg, const char *restrict name)
    __nonnull_all;

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

#ifdef __cplusplus
}
#endif

#endif

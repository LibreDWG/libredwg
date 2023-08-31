/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * in_dxf.h: read ASCII DXF to dwg
 * written by Reini Urban
 */

#ifndef IN_DXF_H
#define IN_DXF_H

#include "dwg.h"
#include "bits.h"
#include "decode.h"
#include "dynapi.h"

#ifdef __cplusplus
extern "C"
{
#endif
// from dwg_api
#ifndef _DWG_API_H_
BITCODE_T dwg_add_u8_input (Dwg_Data *restrict dwg,
                            const char *restrict u8str) __nonnull_all;
#endif

EXPORT int dwg_read_dxf (Bit_Chain *restrict dat,
                         Dwg_Data *restrict dwg) __nonnull_all;
EXPORT int dwg_read_dxfb (Bit_Chain *restrict dat,
                          Dwg_Data *restrict dwg) __nonnull_all;
#ifdef __cplusplus
}
#endif

// global array of [obj -> [fields], ...]
typedef struct _dxf_field
{
  char *name;
  char *type;
  int dxf;
} Dxf_Field;

// to search obj ptr in array
/*
typedef struct _dxf_objs
{
  Dwg_Object *obj;
  int num_fields;
  int size_fields;
  Dxf_Field *fields;
} Dxf_Objs;
*/

typedef struct _dxf_pair
{
  short code;
  enum RESBUF_VALUE_TYPE type;
  union // must be big enough for setting BD
  {
    unsigned int u;
    int i;
    char *s;
    long l;
    uint64_t rll;
    double d;
  } value;
} Dxf_Pair;

/* We need to postpone the HEADER handles from names,
   when we didn't read the TABLES yet, which sets the handle values.
   Also for EED appid handles, and some object names => handle.
   Store all handle fieldnames and string values into this array,
   which is prefixed with the number of stored items.
 */
struct array_hdl
{
  char *field;
  char *name;
  int code; // or objid
};
typedef struct _array_hdls
{
  uint32_t nitems;
  uint32_t size;            // in chunks of 16
  struct array_hdl items[]; // Flexible array grows
} array_hdls;

array_hdls *array_push (array_hdls *restrict hdls, const char *restrict field,
                        const char *restrict name, const int code);
array_hdls *new_array_hdls (uint32_t size);
void free_array_hdls (array_hdls *hdls);

void dxf_add_field (Dwg_Object *restrict obj, const char *restrict name,
                    const char *restrict type, int dxf);
Dxf_Field *dxf_search_field (Dwg_Object *restrict obj,
                             const char *restrict name,
                             const char *restrict type, int dxf);
const Dwg_DYNAPI_field *find_numfield (const Dwg_DYNAPI_field *restrict fields,
                                       const char *restrict key);
BITCODE_H find_tablehandle (Dwg_Data *restrict dwg, Dxf_Pair *restrict pair);
int is_table_name (const char *restrict name) __nonnull_all;
int is_textlike (Dwg_Object *restrict obj) __nonnull_all;

BITCODE_RC dxf_find_lweight (const int16_t lw);

// for sscanf with BD we need to use %lf not %g
#undef FORMAT_BD
#define FORMAT_BD "%lf"

#define DWG_OBJECT(token)
#define DWG_ENTITY(token)

#define NEW_OBJECT(dwg, obj)                                                  \
  {                                                                           \
    BITCODE_BL idx = dwg->num_objects;                                        \
    (void)dwg_add_object (dwg);                                               \
    obj = &dwg->object[idx];                                                  \
    obj->supertype = DWG_SUPERTYPE_OBJECT;                                    \
    obj->tio.object                                                           \
        = (Dwg_Object_Object *)calloc (1, sizeof (Dwg_Object_Object));        \
    obj->tio.object->objid = obj->index;                                      \
    obj->tio.object->dwg = dwg;                                               \
  }

#define NEW_ENTITY(dwg, obj)                                                  \
  {                                                                           \
    BITCODE_BL idx = dwg->num_objects;                                        \
    (void)dwg_add_object (dwg);                                               \
    obj = &dwg->object[idx];                                                  \
    obj->supertype = DWG_SUPERTYPE_ENTITY;                                    \
    obj->tio.entity                                                           \
        = (Dwg_Object_Entity *)calloc (1, sizeof (Dwg_Object_Entity));        \
    obj->tio.entity->objid = obj->index;                                      \
    obj->tio.entity->dwg = dwg;                                               \
  }

#ifndef __cplusplus

#  define ADD_OBJECT(token)                                                   \
    obj->type = obj->fixedtype = DWG_TYPE_##token;                            \
    obj->name = (char *)#token;                                               \
    obj->dxfname = dxfname;                                                   \
    if (obj->type >= DWG_TYPE_GROUP)                                          \
      (void)dwg_encode_get_class (obj->parent, obj);                          \
    LOG_TRACE ("  ADD_OBJECT %s [%d]\n", obj->name, obj->index)               \
    _obj = calloc (1, sizeof (Dwg_Object_##token));                           \
    obj->tio.object->tio.token = (Dwg_Object_##token *)_obj;                  \
    obj->tio.object->tio.token->parent = obj->tio.object;                     \
    obj->tio.object->objid = obj->index

#  define ADD_OBJECT1(token, tgt) ADD_OBJECT (token)

#  define ADD_ENTITY(token)                                                   \
    obj->type = obj->fixedtype = DWG_TYPE_##token;                            \
    if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))                    \
      obj->name = (char *)&#token[1];                                         \
    else                                                                      \
      obj->name = (char *)#token;                                             \
    obj->dxfname = dxfname;                                                   \
    if (obj->type >= DWG_TYPE_GROUP)                                          \
      (void)dwg_encode_get_class (obj->parent, obj);                          \
    LOG_TRACE ("  ADD_ENTITY %s [%d]\n", obj->name, obj->index)               \
    GCC14_DIAG_IGNORE (-Wanalyzer-allocation-size)                          \
    _obj = calloc (1, sizeof (Dwg_Entity_##token));                           \
    GCC14_DIAG_RESTORE                                                        \
    obj->tio.entity->tio.token = (Dwg_Entity_##token *)_obj;                  \
    obj->tio.entity->tio.token->parent = obj->tio.entity;                     \
    obj->tio.entity->objid = obj->index

#else // __cplusplus

#  define ADD_OBJECT(token)                                                   \
    obj->type = obj->fixedtype = DWG_TYPE_##token;                            \
    obj->name = (char *)#token;                                               \
    obj->dxfname = dxfname;                                                   \
    if (obj->type >= DWG_TYPE_GROUP)                                          \
      (void)dwg_encode_get_class (obj->parent, obj);                          \
    LOG_TRACE ("  ADD_OBJECT %s [%d]\n", obj->name, obj->index)               \
    _obj = reinterpret_cast<Dwg_Object_APPID *> (                             \
        calloc (1, sizeof (Dwg_Object_##token)));                             \
    obj->tio.object->tio.token = (Dwg_Object_##token *)_obj;                  \
    obj->tio.object->tio.token->parent = obj->tio.object;                     \
    obj->tio.object->objid = obj->index

#  define ADD_OBJECT1(token, tgt)                                             \
    obj->type = obj->fixedtype = DWG_TYPE_##token;                            \
    obj->name = (char *)#token;                                               \
    obj->dxfname = dxfname;                                                   \
    if (obj->type >= DWG_TYPE_GROUP)                                          \
      (void)dwg_encode_get_class (obj->parent, obj);                          \
    LOG_TRACE ("  ADD_OBJECT %s [%d]\n", obj->name, obj->index)               \
    _obj = reinterpret_cast<Dwg_Object_##tgt *> (                             \
        calloc (1, sizeof (Dwg_Object_##token)));                             \
    obj->tio.object->tio.token = (Dwg_Object_##token *)_obj;                  \
    obj->tio.object->tio.token->parent = obj->tio.object;                     \
    obj->tio.object->objid = obj->index

#  define ADD_ENTITY(token)                                                   \
    obj->type = obj->fixedtype = DWG_TYPE_##token;                            \
    if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))                    \
      obj->name = (char *)&#token[1];                                         \
    else                                                                      \
      obj->name = (char *)#token;                                             \
    obj->dxfname = dxfname;                                                   \
    if (obj->type >= DWG_TYPE_GROUP)                                          \
      (void)dwg_encode_get_class (obj->parent, obj);                          \
    LOG_TRACE ("  ADD_ENTITY %s [%d]\n", obj->name, obj->index)               \
    _obj = reinterpret_cast<Dwg_Object_APPID *> (                             \
        (char *)calloc (1, sizeof (Dwg_Entity_##token)));                     \
    obj->tio.entity->tio.token = (Dwg_Entity_##token *)_obj;                  \
    obj->tio.entity->tio.token->parent = obj->tio.entity;                     \
    obj->tio.entity->objid = obj->index

#endif // __cplusplus

#define ADD_TABLE_IF(nam, token)                                              \
  if (strEQc (name, #nam))                                                    \
    {                                                                         \
      ADD_OBJECT (token);                                                     \
    }
#define ADD_TABLE_IF1(nam, token)                                             \
  if (strEQc (name, #nam))                                                    \
    {                                                                         \
      ADD_OBJECT1 (token, LTYPE_CONTROL);                                     \
    }

#define STRADD_TV(field, string)                                              \
  if (string)                                                                 \
    {                                                                         \
      field = (char *)malloc (strlen (string) + 1);                           \
      strcpy (field, string);                                                 \
    }
#define STRADD_T(field, string)                                               \
  if (string)                                                                 \
    {                                                                         \
      field = dwg_add_u8_input (dwg, string);                                 \
    }

#ifndef __cplusplus

#  define UPGRADE_ENTITY(FROM, TO)                                            \
    obj->type = obj->fixedtype = DWG_TYPE_##TO;                               \
    obj->name = (char *)#TO;                                                  \
    free (obj->dxfname);                                                      \
    obj->dxfname = strdup (obj->name);                                        \
    strcpy (name, obj->name);                                                 \
    LOG_TRACE ("change type to %s\n", name);                                  \
    if (sizeof (Dwg_Entity_##TO) > sizeof (Dwg_Entity_##FROM))                \
      {                                                                       \
        LOG_TRACE ("realloc to %s\n", name);                                  \
        _obj = realloc (_obj, sizeof (Dwg_Entity_##TO));                      \
        obj->tio.entity->tio.TO = (Dwg_Entity_##TO *)_obj;                    \
      }

#else

#  define UPGRADE_ENTITY(FROM, TO)                                            \
    obj->type = obj->fixedtype = DWG_TYPE_##TO;                               \
    obj->name = (char *)#TO;                                                  \
    free (obj->dxfname);                                                      \
    obj->dxfname = strdup (obj->name);                                        \
    strcpy (name, obj->name);                                                 \
    LOG_TRACE ("change type to %s\n", name);                                  \
    if (sizeof (Dwg_Entity_##TO) > sizeof (Dwg_Entity_##FROM))                \
      {                                                                       \
        LOG_TRACE ("realloc to %s\n", name);                                  \
        _obj = reinterpret_cast<Dwg_Object_APPID *> (                         \
            (char *)realloc (_obj, sizeof (Dwg_Entity_##TO)));                \
        obj->tio.entity->tio.TO = (Dwg_Entity_##TO *)_obj;                    \
      }

#endif // cplusplus

#endif

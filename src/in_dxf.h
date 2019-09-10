/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
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

// global array of [obj -> [fields], ...]
typedef struct _dxf_field
{
  char *name;
  char *type;
  int dxf;
} Dxf_Field;

// to search obj ptr in array
typedef struct _dxf_objs
{
  Dwg_Object *obj;
  int num_fields;
  int size_fields;
  Dxf_Field *fields;
} Dxf_Objs;

typedef struct _dxf_pair
{
  short code;
  enum RES_BUF_VALUE_TYPE type;
  union // must be big enough for setting BD
  {
    unsigned int u;
    int i;
    char *s;
    long l;
    uint64_t bll;
    double d;
  } value;
} Dxf_Pair;

/* We need to postpone the HEADER handles from names,
   when we didn't read the TABLES yet, which sets the handle values.
   Store all handle fieldnames and string values into this array,
   which is prefixed with the number of stored items.
 */
struct array_hdl {
  char *field;
  char *name;
  short code;
};
typedef struct _array_hdls {
  uint32_t nitems;
  uint32_t size;            // in chunks of 16
  struct array_hdl items[]; // Flexible array grows
} array_hdls;

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof (arr[0]))

array_hdls *array_push (array_hdls *restrict hdls, char *restrict field,
                        char *restrict name, short code);
void free_array_hdls (array_hdls *hdls);
int matches_type (Dxf_Pair *restrict pair, const Dwg_DYNAPI_field *restrict f);

void dxf_add_field (Dwg_Object *restrict obj, const char *restrict name,
                    const char *restrict type, int dxf);
Dxf_Field *dxf_search_field (Dwg_Object *restrict obj,
                             const char *restrict name,
                             const char *restrict type, int dxf);

void add_eed (Dwg_Object *restrict obj, const char *restrict name,
              Dxf_Pair *restrict pair);
Dxf_Pair *add_xdata (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                     Dxf_Pair *restrict pair);
void add_dictionary_itemhandles (Dwg_Object *restrict obj, Dxf_Pair *restrict pair,
                                 char *restrict text);
void resolve_postponed_header_refs (Dwg_Data *restrict dwg);
void resolve_postponed_eed_refs (Dwg_Data *restrict dwg);
void resolve_header_dicts (Dwg_Data *restrict dwg);
BITCODE_H find_tablehandle (Dwg_Data *restrict dwg, Dxf_Pair *restrict pair);
int is_table_name (const char *name);
void entity_alias (char *name);
void object_alias (char *name);

EXPORT int dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
EXPORT int dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);

// for sscanf with BD we need to use %lf not %g
#undef FORMAT_BD
#define FORMAT_BD "%lf"

#define DWG_OBJECT(token)
#define DWG_ENTITY(token)

#define NEW_OBJECT(dwg, obj)                    \
  {                                             \
    BITCODE_BL idx = dwg->num_objects;          \
    (void)dwg_add_object (dwg);                 \
    obj = &dwg->object[idx];                    \
    obj->supertype = DWG_SUPERTYPE_OBJECT;      \
    obj->tio.object = calloc (1, sizeof (Dwg_Object_Object)); \
    obj->tio.object->objid = obj->index;        \
    obj->tio.object->dwg = dwg;                 \
  }

#define NEW_ENTITY(dwg, obj)                    \
  {                                             \
    BITCODE_BL idx = dwg->num_objects;          \
    (void)dwg_add_object (dwg);                 \
    obj = &dwg->object[idx];                    \
    obj->supertype = DWG_SUPERTYPE_ENTITY;      \
    obj->tio.entity = calloc (1, sizeof (Dwg_Object_Entity)); \
    obj->tio.entity->objid = obj->index;        \
    obj->tio.entity->dwg = dwg;                 \
  }

#define ADD_OBJECT(token)                                      \
  obj->type = obj->fixedtype = DWG_TYPE_##token;               \
  obj->name = (char *)#token;                                  \
  obj->dxfname = dxfname;                                      \
  LOG_TRACE ("  ADD_OBJECT %s %d\n", obj->name, obj->index)    \
  _obj = calloc (1, sizeof (Dwg_Object_##token));              \
  obj->tio.object->tio.token = (Dwg_Object_##token *)_obj;     \
  obj->tio.object->tio.token->parent = obj->tio.object;        \
  obj->tio.object->objid = obj->index

#define ADD_ENTITY(token)                                      \
  obj->type = obj->fixedtype = DWG_TYPE_##token;               \
  if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))       \
    obj->name = (char *)&#token[1];                            \
  else                                                         \
    obj->name = (char *)#token;                                \
  obj->dxfname = dxfname;                                      \
  LOG_TRACE ("  ADD_ENTITY %s %d\n", obj->name, obj->index)    \
  _obj = calloc (1, sizeof (Dwg_Entity_##token));              \
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)_obj;     \
  obj->tio.entity->tio.token->parent = obj->tio.entity;        \
  obj->tio.entity->objid = obj->index

#define ADD_TABLE_IF(nam, token)                       \
  if (strEQc (name, #nam))                             \
    {                                                  \
      ADD_OBJECT(token);                               \
    }

#define STRADD(field, string)                          \
  field = malloc (strlen (string) + 1);                \
  strcpy (field, string)

// normally in spec.h, but indxf does not import spec.h
#ifndef M_PI_2
#  define M_PI_2 1.57079632679489661923132169163975144
#endif
#define rad2deg(ang) (ang) * 90.0 / M_PI_2
#define deg2rad(ang) (ang) * M_PI_2 / 90.0

#endif

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
int matches_type (Dxf_Pair *restrict pair, const Dwg_DYNAPI_field *restrict f);

void dxf_add_field (Dwg_Object *restrict obj, const char *restrict name,
                    const char *restrict type, int dxf);
Dxf_Field *dxf_search_field (Dwg_Object *restrict obj,
                             const char *restrict name,
                             const char *restrict type, int dxf);

int add_handle (Dwg_Handle *restrict hdl, BITCODE_RC code,
                BITCODE_RL value, Dwg_Object *restrict obj)
  __nonnull ((1));

Dwg_Object_Ref * ATTRIBUTE_MALLOC
add_handleref (Dwg_Data *restrict dwg, BITCODE_RC code, BITCODE_RL value, Dwg_Object *obj);

void add_eed (Dwg_Object *restrict obj, const char *restrict name,
              Dxf_Pair *restrict pair);
int add_xdata (Bit_Chain *restrict dat,
               Dwg_Object *restrict obj, Dxf_Pair *restrict pair);

int is_table_name (const char *name);

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
  obj->name = obj->dxfname = (char *)#token;                   \
  _obj = calloc (1, sizeof (Dwg_Object_##token));              \
  obj->tio.object->tio.token = (Dwg_Object_##token *)_obj;     \
  obj->tio.object->tio.token->parent = obj->tio.object;        \
  obj->tio.object->objid = obj->index

#define ADD_ENTITY(token)                                      \
  obj->type = obj->fixedtype = DWG_TYPE_##token;               \
  if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))       \
    obj->name = obj->dxfname = (char *)&#token[1];             \
  else                                                         \
    obj->name = obj->dxfname = (char *)#token;                 \
  _obj = calloc (1, sizeof (Dwg_Entity_##token));              \
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)_obj;     \
  obj->tio.entity->tio.token->parent = obj->tio.entity;        \
  obj->tio.entity->objid = obj->index

#define ADD_TABLE_IF(nam, token)                       \
  if (strEQc (name, #nam))                             \
    {                                                  \
      ADD_OBJECT(token);                               \
      if (strEQc (#nam, "VPORT"))                      \
        strcpy (ctrl_hdlv, "vports");                  \
      else if (strEQc (#nam, "UCS"))                   \
        strcpy (ctrl_hdlv, "ucs");                     \
      else if (strEQc (#nam, "VIEW"))                  \
        strcpy (ctrl_hdlv, "views");                   \
      else if (strEQc (#nam, "LTYPE"))                 \
        strcpy (ctrl_hdlv, "linetypes");               \
      else if (strEQc (#nam, "STYLE"))                 \
        strcpy (ctrl_hdlv, "styles");                  \
      else if (strEQc (#nam, "LAYER"))                 \
        strcpy (ctrl_hdlv, "layers");                  \
      else if (strEQc (#nam, "DIMSTYLE"))              \
        strcpy (ctrl_hdlv, "dimstyles");               \
      else if (strEQc (#nam, "APPID"))                 \
        strcpy (ctrl_hdlv, "apps");                    \
      else if (strEQc (#nam, "VPORT_ENTITY"))          \
        strcpy (ctrl_hdlv, "vport_entity_headers");    \
      else if (strEQc (#nam, "BLOCK_RECORD"))          \
        strcpy (ctrl_hdlv, "block_headers");           \
    }

#define STRADD(field, string)                          \
  field = malloc (strlen (string) + 1);                \
  strcpy (field, string)

#endif

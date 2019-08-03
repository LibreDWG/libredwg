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
};
typedef struct _array_hdls {
  uint32_t nitems;
  uint32_t size;            // in chunks of 16
  struct array_hdl items[]; // Flexible array grows
} array_hdls;

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof (arr[0]))

array_hdls *array_push (array_hdls *restrict hdls, char *restrict field,
                        char *restrict name);
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
add_handleref (BITCODE_RC code, BITCODE_RL value, Dwg_Object *obj);

void add_eed (Dwg_Object *restrict obj, const char *restrict name,
              Dxf_Pair *restrict pair);

int is_table_name (const char *name);

EXPORT int dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
EXPORT int dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);

// for sscanf with BD we need to use %lf not %g
#undef FORMAT_BD
#define FORMAT_BD "%lf"

#define MY_DWG_OBJECT(token) \
  if (0) { \
    Dwg_Object_##token *_obj;
#define DWG_OBJECT_END \
  }
#define MY_DWG_ENTITY(token) \
  if (0) { \
    Dwg_Entity_##token *_obj; \
    Dwg_Object_Entity *_ent;
#define DWG_ENTITY_END \
  }

#define DWG_OBJECT(token) MY_DWG_OBJECT(token)
#define DWG_ENTITY(token) MY_DWG_ENTITY(token)

// Disable most of dwg.spec

#define R11OPTS(x) 0
#define R11FLAG(x) 0
#define FIELD(name, type)
#define FIELDG(name, type, dxf)
#define FIELD_CAST(name, type, cast, dxf)
#define FIELD_TRACE(name, type)
#define SUB_FIELD(o, name, type, dxf)
#define VALUE_TV(value, dxf)
#define SUBCLASS(text)
#define VALUE_TU(value, dxf)
#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)
#define FIELD_HANDLE(nam, handle_code, dxf)
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)
#define HEADER_9(name)
#define VALUE_H(value, dxf)
#define HEADER_H(name, dxf)
#define HEADER_VALUE(name, type, dxf, value)
#define HEADER_VAR(name, type, dxf)
#define HEADER_3D(name)
#define HEADER_2D(name)
#define HEADER_BLL(name, dxf)
#define HEADER_TIMEBLL(name, dxf)
#define SECTION(section)
#define ENDSEC()
#define TABLE(table)
#define ENDTAB()
#define RECORD(record)
#define GROUP(dxf)
#define VALUE(value, type, dxf)
#define HEADER_HANDLE_NAME(name, dxf, section)
#define HANDLE_NAME(id, dxf)
#define FIELD_DATAHANDLE(name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)
#define HEADER_RC(name, dxf)
#define HEADER_RS(name, dxf)
#define HEADER_RL(name, dxf)
#define HEADER_RD(name, dxf)
#define HEADER_RLL(name, dxf)
#define HEADER_TV(name, dxf)
#define HEADER_TU(name, dxf)
#define HEADER_T(name, dxf)
#define HEADER_B(name, dxf)
#define HEADER_BS(name, dxf)
#define HEADER_BL(name, dxf)
#define HEADER_BD(name, dxf)
#define VALUE_B(value, dxf)
#define VALUE_BB(value, dxf)
#define VALUE_3B(value, dxf)
#define VALUE_BS(value, dxf)
#define VALUE_BL(value, dxf)
#define VALUE_BLL(value, dxf)
#define VALUE_BD(value, dxf)
#define VALUE_RC(value, dxf)
#define VALUE_RS(value, dxf)
#define VALUE_RD(value, dxf)
#define VALUE_RL(value, dxf)
#define VALUE_RLL(value, dxf)
#define VALUE_MC(value, dxf)
#define VALUE_MS(value, dxf)
#define FIELD_B(name, dxf)
#define FIELD_BB(name, dxf)
#define FIELD_3B(name, dxf)
#define FIELD_BS(name, dxf)
#define FIELD_BL(name, dxf)
#define FIELD_BLL(name, dxf)
#define FIELD_BD(name, dxf)
#define FIELD_RC(name, dxf)
#define FIELD_RS(name, dxf)
#define FIELD_RD(name, dxf)
#define FIELD_RL(name, dxf)
#define FIELD_RLL(name, dxf)
#define FIELD_MC(name, dxf)
#define FIELD_MS(name, dxf)
#define FIELD_TF(name, len, dxf)
#define FIELD_TFF(name, len, dxf)
#define FIELD_TV(name, dxf)
#define FIELD_TU(name, dxf)
#define FIELD_T(name, dxf)
#define VALUE_T(value, dxf)
#define FIELD_BT(name, dxf)
#define FIELD_4BITS(name, dxf)
#define FIELD_BE(name, dxf)
#define FIELD_DD(name, _default, dxf)
#define FIELD_2DD(name, d1, d2, dxf)
#define FIELD_3DD(name, def, dxf)
#define FIELD_2RD(name, dxf) {}
#define FIELD_2BD(name, dxf)
#define FIELD_2BD_1(name, dxf)
#define FIELD_3RD(name, dxf) {}
#define FIELD_3BD(name, dxf)
#define FIELD_3BD_1(name, dxf)
#define FIELD_3DPOINT(name, dxf)
#define FIELD_CMC(color, dxf1, dxf2)
#define SUB_FIELD_CMC(o, color, dxf1, dxf2)
#define FIELD_TIMEBLL(name, dxf)
#define HEADER_CMC(name, dxf)
#define POINT_3D(name, var, c1, c2, c3)
#define POINT_2D(name, var, c1, c2)
#define FIELD_VECTOR_N(name, type, size, dxf)
#define FIELD_VECTOR_T(name, size, dxf)
#define FIELD_VECTOR(name, type, size, dxf)
#define FIELD_2RD_VECTOR(name, size, dxf)
#define FIELD_2DD_VECTOR(name, size, dxf)
#define FIELD_3DPOINT_VECTOR(name, size, dxf)
#define HANDLE_VECTOR_N(name, size, code, dxf)
#define HANDLE_VECTOR(name, sizefield, code, dxf)
#define FIELD_NUM_INSERTS(num_inserts, type, dxf)
#define FIELD_XDATA(name, size)
#define REACTORS(code)
#define ENT_REACTORS(code)
#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code)
#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM
#define _REPEAT(times, name, type, idx)    if (0) for (; 0; )
#define _REPEAT_C(times, name, type, idx)  if (0) for (; 0; )
#define REPEAT(times, name, type) _REPEAT (times, name, type, 1)
#define REPEAT2(times, name, type) _REPEAT (times, name, type, 2)
#define REPEAT3(times, name, type) _REPEAT (times, name, type, 3)
#define REPEAT4(times, name, type) _REPEAT (times, name, type, 4)
#define REPEAT_C(times, name, type) _REPEAT_C (times, name, type, 1)
#define REPEAT2_C(times, name, type) _REPEAT_C (times, name, type, 2)
#define REPEAT3_C(times, name, type) _REPEAT_C (times, name, type, 3)
#define REPEAT4_C(times, name, type) _REPEAT_C (times, name, type, 4)
#define REPEAT_CN(times, name, type) _REPEAT (times, name, type, 1)
#define REPEAT_N(times, name, type) _REPEAT (times, name, type, 1)
#define _REPEAT_N(times, name, type, idx) _REPEAT (times, name, type, 1)
#define DIMENSION_COMMON_DECODE

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
  obj->name = obj->dxfname = (char*)#token;                    \
  _gobj = calloc (1, sizeof (Dwg_Object_##token));              \
  obj->tio.object->tio.token = (Dwg_Object_##token *)_gobj;     \
  obj->tio.object->tio.token->parent = obj->tio.object;        \
  obj->tio.object->objid = obj->index

#define ADD_ENTITY(token)                                      \
  obj->type = obj->fixedtype = DWG_TYPE_##token;               \
  obj->name = obj->dxfname = (char*)#token;                    \
  _gobj = calloc (1, sizeof (Dwg_Entity_##token));              \
  obj->tio.entity->tio.token = (Dwg_Entity_##token *)_gobj;     \
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

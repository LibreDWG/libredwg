/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2025 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * free.c: helper functions to free all spec fields
 * written by Reini Urban
 * modified by Denis Pruchkovsky
 */

#include "config.h"
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "free.h"
#include "classes.h"
#include "hash.h"
#include "free.h"

static unsigned int loglevel;
#ifdef USE_TRACING
static int env_var_checked_p;
#endif
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static Bit_Chain pdat = { 0 };
static BITCODE_BL rcount1, rcount2;

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION free
#define IS_FREE

#define FREE_IF(ptr)                                                          \
  {                                                                           \
    if (ptr)                                                                  \
      free (ptr);                                                             \
    ptr = NULL;                                                               \
  }

#undef UNTIL
#undef SINCE
#undef PRE
#undef VERSIONS
#undef VERSION
#define UNTIL(v) if (dat->from_version <= v)
#define SINCE(v) if (dat->from_version >= v)
#define PRE(v) if (dat->from_version < v)
#define VERSIONS(v1, v2)                                                      \
  if (dat->from_version >= v1 && dat->from_version <= v2)
#define VERSION(v) if (dat->from_version == v)

#define VALUE(value, type, dxf)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RLx(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)
#define VALUE_BD(value, dxf) VALUE (value, BD, dxf)

#define FIELD(name, type)                                                     \
  {                                                                           \
  }
#define FIELD_TRACE(name, type)                                               \
  LOG_TRACE (#name ": " FORMAT_##type "\n", _obj->name)
#define FIELD_G_TRACE(name, type, dxfgroup)                                   \
  LOG_TRACE (#name ": " FORMAT_##type " [" #type " %d]\n", _obj->name,        \
             dxfgroup)
#define FIELD_CAST(name, type, cast, dxf)                                     \
  {                                                                           \
  }
#define SUB_FIELD_CAST(o, name, type, cast, dxf)                              \
  {                                                                           \
  }
#define SUB_FIELD(o, nam, type, dxf) FIELD (_obj->o.nam, type)

#define ANYCODE -1
#define FIELD_HANDLE(nam, code, dxf) VALUE_HANDLE (_obj->nam, nam, code, dxf)
#define SUB_FIELD_HANDLE(o, nam, code, dxf)                                   \
  if (obj->fixedtype == DWG_TYPE_TABLESTYLE)                                  \
    LOG_HANDLE ("free TABLESTYLE.%s.%s\n", #o, #nam);                         \
  VALUE_HANDLE (_obj->o.nam, nam, code, dxf)
// compare to dwg_decode_handleref_with_code: not all refs are stored in the
// object_ref vector, like relative ptrs and NULL.
// But with INDXF skip the NULL HDL, it is global and shared there.
// obj is the relative base object here and there.
#define VALUE_HANDLE(ref, nam, _code, dxf)                                    \
  if (ref && !ref->handleref.is_global)                                       \
    {                                                                         \
      free (ref);                                                             \
      ref = NULL;                                                             \
    } /* else freed globally */
#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE (name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, code, dxf) FIELD_HANDLE (name, code, dxf)
#define FIELD_VECTOR_INL(nam, type, size, dxf)
#define VALUE_H(hdl, dxf)

#define FIELD_B(name, dxf) FIELD (name, B)
#define FIELD_BB(name, dxf) FIELD (name, BB)
#define FIELD_3B(name, dxf) FIELD (name, 3B)
#define FIELD_BS(name, dxf) FIELD (name, BS)
#define FIELD_BL(name, dxf) FIELD (name, BL)
#define FIELD_BLL(name, dxf) FIELD (name, BLL)
#define FIELD_BD(name, dxf) FIELD (name, BD)
#define FIELD_RC(name, dxf) FIELD (name, RC)
#define FIELD_RS(name, dxf) FIELD (name, RS)
#define FIELD_RD(name, dxf) FIELD (name, RD)
#define FIELD_RL(name, dxf) FIELD (name, RL)
#define FIELD_RLL(name, dxf) FIELD (name, RLL)
#define FIELD_MC(name, dxf) FIELD (name, MC)
#define FIELD_MS(name, dxf) FIELD (name, MS)
#define FIELD_TV(name, dxf) FREE_IF (FIELD_VALUE (name))
#define VALUE_TV(value, dxf) FREE_IF (value)
#define VALUE_TF(value, dxf) FREE_IF (value)
#define VALUE_TFF(value, dxf)
#define FIELD_TU(name, dxf) FIELD_TV (name, dxf)
#define FIELD_TF(name, len, dxf) FIELD_TV (name, dxf)
#define FIELD_TFv(name, len, dxf) FIELD_TV (name, dxf)
#define FIELD_TFF(name, len, dxf)                                             \
  {                                                                           \
  }
#define FIELD_T(name, dxf) FIELD_TV (name, dxf)
#define FIELD_BINARY(name, len, dxf) FIELD_TV (name, dxf)
#define FIELD_T16(name, dxf) FIELD_TV (name, dxf)
#define FIELD_TU16(name, dxf) FIELD_TV (name, dxf)
#define FIELD_T32(name, dxf) FIELD_TV (name, dxf)
#define FIELD_BT(name, dxf) FIELD (name, BT);
#define FIELD_4BITS(name, dxf)                                                \
  {                                                                           \
  }
#define FIELD_BE(name, dxf)                                                   \
  {                                                                           \
  }
#define FIELD_DD(name, _default, dxf)                                         \
  {                                                                           \
  }
#define FIELD_2DD(name, def, dxf)                                             \
  {                                                                           \
  }
#define FIELD_3DD(name, def, dxf)                                             \
  {                                                                           \
  }
#define FIELD_2RD(name, dxf)                                                  \
  {                                                                           \
  }
#define FIELD_2BD(name, dxf)                                                  \
  {                                                                           \
  }
#define FIELD_2BD_1(name, dxf)                                                \
  {                                                                           \
  }
#define FIELD_3RD(name, dxf)                                                  \
  {                                                                           \
  }
#define FIELD_3BD(name, dxf)                                                  \
  {                                                                           \
  }
#define FIELD_3BD_1(name, dxf)                                                \
  {                                                                           \
  }
#define FIELD_3DPOINT(name, dxf)                                              \
  {                                                                           \
  }
#define FIELD_2PT_TRACE(name, type, dxf)                                      \
  {                                                                           \
  }
#define FIELD_3PT_TRACE(name, type, dxf)                                      \
  {                                                                           \
  }
#define FIELD_TIMEBLL(name, dxf)
#define FIELD_TIMERLL(name, dxf)
#define FIELD_CMC(color, dxf)                                                 \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    FIELD_T (color.name, 0);                                                  \
    FIELD_T (color.book_name, 0);                                             \
  }
#define SUB_FIELD_CMC(o, color, dxf)                                          \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    VALUE_TV (_obj->o.color.name, 0);                                         \
    VALUE_TV (_obj->o.color.book_name, 0);                                    \
  }

// FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  if ((size) && _obj->nam)                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)(size); vcount++)                 \
        FIELD_##type (nam[vcount], dxf);                                      \
    }                                                                         \
  FIELD_TV (nam, dxf);
#define FIELD_VECTOR_T(name, type, size, dxf)                                 \
  FIELD_VECTOR_N (name, TV, _obj->size, dxf)
#define FIELD_VECTOR(name, type, size, dxf)                                   \
  FIELD_VECTOR_N (name, type, _obj->size, dxf)
#define FIELD_2RD_VECTOR(name, size, dxf) FIELD_TV (name, dxf)
#define FIELD_2DD_VECTOR(name, size, dxf) FIELD_TV (name, dxf)
#define FIELD_3DPOINT_VECTOR(name, size, dxf) FIELD_TV (name, dxf)
#define HANDLE_VECTOR_N(name, size, code, dxf)                                \
  if (_obj->name)                                                             \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIELD_HANDLE_N (name[vcount], vcount, code, dxf);                   \
        }                                                                     \
      FIELD_TV (name, dxf)                                                    \
    }
#define HANDLE_VECTOR(name, sizefield, code, dxf)                             \
  HANDLE_VECTOR_N (name, FIELD_VALUE (sizefield), code, dxf)
#define SUB_HANDLE_VECTOR(o, name, sizefield, code, dxf)                      \
  if (_obj->o.name && _obj->o.sizefield)                                      \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.sizefield; vcount++)      \
        {                                                                     \
          SUB_FIELD_HANDLE (o, name[vcount], code, dxf);                      \
        }                                                                     \
      FREE_IF (_obj->o.name)                                                  \
    }
#define SUB_FIELD_VECTOR(o, nam, type, sizefield, dxf)                        \
  if (_obj->o.sizefield && _obj->o.nam)                                       \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)(_obj->o.sizefield); vcount++)    \
        SUB_FIELD_##type (o, nam[vcount], dxf);                               \
    }                                                                         \
  FREE_IF (_obj->o.nam)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)
#define FIELD_XDATA(name, size) dwg_free_xdata (_obj, _obj->size)

#define REACTORS(code)                                                        \
  if (obj->tio.object->reactors)                                              \
    {                                                                         \
      for (vcount = 0; vcount < obj->tio.object->num_reactors; vcount++)      \
        VALUE_HANDLE (obj->tio.object->reactors[vcount], reactors, code,      \
                      330);                                                   \
      VALUE_TV (obj->tio.object->reactors, 0);                                \
    }
#define ENT_REACTORS(code)                                                    \
  if (_ent->reactors)                                                         \
    {                                                                         \
      for (vcount = 0; vcount < _ent->num_reactors; vcount++)                 \
        VALUE_HANDLE (_ent->reactors[vcount], reactors, code, 330);           \
      VALUE_TV (_ent->reactors, 0);                                           \
    }
#define XDICOBJHANDLE(code)                                                   \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    if (!obj->tio.object->is_xdic_missing)                                    \
      {                                                                       \
        VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,    \
                      0);                                                     \
      }                                                                       \
  }                                                                           \
  PRIOR_VERSIONS                                                              \
  {                                                                           \
    VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code, 0);    \
  }
#define ENT_XDICOBJHANDLE(code)                                               \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    if (!_ent->is_xdic_missing)                                               \
      {                                                                       \
        VALUE_HANDLE (_ent->xdicobjhandle, xdicobjhandle, code, 0);           \
      }                                                                       \
  }                                                                           \
  PRIOR_VERSIONS                                                              \
  {                                                                           \
    VALUE_HANDLE (_ent->xdicobjhandle, xdicobjhandle, code, 0);               \
  }

#define END_REPEAT(field) FIELD_TV (field, 0)

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#include "spec.h"

#define DWG_ENTITY(token)                                                     \
  static int dwg_free_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj);           \
                                                                              \
  static int dwg_free_##token (Bit_Chain *restrict dat,                       \
                               Dwg_Object *restrict obj)                      \
  {                                                                           \
    int error = 0;                                                            \
    if (obj->tio.entity)                                                      \
      {                                                                       \
        LOG_HANDLE ("Free entity " #token " [%d]\n", obj->index)              \
        if (obj->tio.entity->tio.token)                                       \
          error = dwg_free_##token##_private (dat, dat, dat, obj);            \
                                                                              \
        dwg_free_common_entity_data (obj);                                    \
        dwg_free_eed (obj);                                                   \
        if (obj->tio.entity)                                                  \
          {                                                                   \
            FREE_IF (obj->tio.entity->tio.token);                             \
            FREE_IF (obj->tio.entity);                                        \
          }                                                                   \
      }                                                                       \
    obj->parent = NULL;                                                       \
    return error;                                                             \
  }                                                                           \
  static int dwg_free_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj)            \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    _ent = obj->tio.entity;                                                   \
    if (!_ent)                                                                \
      return 0;                                                               \
    _obj = ent = _ent->tio.token;

#define DWG_ENTITY_END                                                        \
  FREE_IF (obj->unknown_rest);                                                \
  return error;                                                               \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_free_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj);           \
                                                                              \
  static int dwg_free_##token (Bit_Chain *restrict dat,                       \
                               Dwg_Object *restrict obj)                      \
  {                                                                           \
    int error = 0;                                                            \
    Dwg_Object_##token *_obj = NULL;                                          \
    if (obj->tio.object)                                                      \
      {                                                                       \
        _obj = obj->tio.object->tio.token;                                    \
        LOG_HANDLE ("Free object " #token " [%d]\n", obj->index)              \
        error = dwg_free_##token##_private (dat, dat, dat, obj);              \
        dwg_free_common_object_data (obj);                                    \
        dwg_free_eed (obj);                                                   \
        FREE_IF (_obj);                                                       \
        FREE_IF (obj->tio.object);                                            \
      }                                                                       \
    obj->parent = NULL;                                                       \
    return error;                                                             \
  }                                                                           \
                                                                              \
  static int dwg_free_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj)            \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Object_##token *_obj;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    if (!obj->tio.object)                                                     \
      return 0;                                                               \
    _obj = obj->tio.object->tio.token;

/* obj itself is allocated via dwg->object[], dxfname is klass->dxfname or
 * static */
#define DWG_OBJECT_END                                                        \
  FREE_IF (obj->unknown_rest);                                                \
  return error;                                                               \
  }

static void
dwg_free_common_entity_data (Dwg_Object *obj)
{

  Dwg_Data *dwg = obj->parent;
  Bit_Chain *dat = &pdat;
  Bit_Chain *hdl_dat = &pdat;
  Dwg_Object_Entity *_obj;
  Dwg_Object_Entity *_ent;
  BITCODE_BL vcount;
  int error = 0;

  _ent = obj->tio.entity;
  if (!_ent)
    return;
  _obj = _ent;

  FREE_IF (_ent->preview);

  // clang-format off
  #include "common_entity_data.spec"
  if (dat->from_version >= R_2007 && _ent->color.flag & 0x40)
    FIELD_HANDLE (color.handle, 0, 430);
  SINCE (R_13b1) {
    #include "common_entity_handle_data.spec"
  }
  // clang-format on
}

static void
dwg_free_common_object_data (Dwg_Object *obj)
{

  Dwg_Data *dwg = obj->parent;
  Bit_Chain *dat = &pdat;
  Bit_Chain *hdl_dat = &pdat;
  Dwg_Object_Object *_obj = obj->tio.object;
  BITCODE_BL vcount;
  int error = 0;

  // clang-format off
  #include "common_object_handle_data.spec"
  // clang-format on
}

static void
dwg_free_xdata (Dwg_Object_XRECORD *obj, int size)
{
  dwg_free_xdata_resbuf (obj->xdata);
  obj->xdata = NULL;
}

EXPORT void
dwg_free_eed (Dwg_Object *obj)
{
  BITCODE_BL i;
  if (obj->supertype == DWG_SUPERTYPE_ENTITY)
    {
      Dwg_Object_Entity *_obj = obj->tio.entity;
      for (i = 0; i < _obj->num_eed; i++)
        {
          FREE_IF (_obj->eed[i].raw);
          FREE_IF (_obj->eed[i].data);
        }
      FREE_IF (_obj->eed);
      _obj->num_eed = 0;
    }
  else
    {
      Dwg_Object_Object *_obj = obj->tio.object;
      if (!_obj || !_obj->eed)
        return;
      for (i = 0; i < _obj->num_eed; i++)
        {
          FREE_IF (_obj->eed[i].raw);
          FREE_IF (_obj->eed[i].data);
        }
      FREE_IF (_obj->eed);
      _obj->num_eed = 0;
    }
}

#include "dwg.spec"

// could be a hash or switch, but there are not that many DEBUGGING classes.
// but a switch is fine, as we get all missing types in objects.inc, generated
// by regen-dynapi
int
dwg_free_variable_no_class (Dwg_Data *restrict dwg, Dwg_Object *restrict obj)
{
  Bit_Chain *dat = &pdat;

#undef DWG_ENTITY
#undef DWG_OBJECT
#define FREE_NOCLASS(name)                                                    \
  case DWG_TYPE_##name:                                                       \
    return dwg_free_##name (dat, obj);
#define DWG_ENTITY(name) FREE_NOCLASS (name)
#define DWG_OBJECT(name) FREE_NOCLASS (name)

  switch (obj->fixedtype)
    {
#include "objects.inc"

    case DWG_TYPE_FREED:
      break; // already freed

    case DWG_TYPE_ACDSRECORD:
    case DWG_TYPE_ACDSSCHEMA:
    case DWG_TYPE_NPOCOLLECTION:
    case DWG_TYPE_XREFPANELOBJECT:
    default:
      LOG_ERROR ("Unhandled class %s, fixedtype %d in objects.inc",
                 dwg_type_name (obj->fixedtype), (int)obj->fixedtype);
    }

#undef DWG_ENTITY
#undef DWG_OBJECT
#undef FREE_NOCLASS

  return DWG_ERR_UNHANDLEDCLASS;
}

/* returns error.
 */
int
dwg_free_variable_type (Dwg_Data *restrict dwg, Dwg_Object *restrict obj)
{
  const int i = obj->type - 500;
  Dwg_Class *klass;
  Bit_Chain *dat = &pdat;

  // no matching class
  if (i < 0 || i >= (int)dwg->num_classes)
    {
      LOG_WARN ("No class for %s type %d found", obj->name, obj->type);
      return dwg_free_variable_no_class (dwg, obj);
    }

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname || !obj->dxfname)
    {
      LOG_WARN ("No class for %s type %d found", obj->name, obj->type);
      return dwg_free_variable_no_class (dwg, obj);
    }

  if (strNE (obj->dxfname, klass->dxfname))
    {
      // But we know how to handle the UNKNOWN_* types
      if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ
          || obj->fixedtype == DWG_TYPE_UNKNOWN_ENT)
        return DWG_ERR_UNHANDLEDCLASS;
      else
        {
          LOG_ERROR ("Wrong %s.type %d for obj [%d]: != %s", obj->dxfname,
                     obj->type, obj->index, klass->dxfname);
          return dwg_free_variable_no_class (dwg, obj);
        }
    }

    // global class dispatcher:
    // with indxf even DEBUGGING objects, such as TABLE are created.
    // usually not written/encoded though.

    // clang-format off
  #include "classes.inc"

  #undef WARN_UNHANDLED_CLASS
  #undef WARN_UNSTABLE_CLASS
  // clang-format on

  LOG_WARN ("No class for %s type %d found", obj->name, obj->type);
  return dwg_free_variable_no_class (dwg, obj);
}

/* returns error */
int
dwg_free_variable_type_private (Dwg_Object *restrict obj)
{
  Dwg_Data *restrict dwg = obj->parent;
  Bit_Chain *dat = &pdat;

#undef DWG_ENTITY
#undef DWG_OBJECT
#define FREE_NOCLASS(name)                                                    \
  case DWG_TYPE_##name:                                                       \
    return dwg_free_##name##_private (dat, dat, dat, obj);
#define DWG_ENTITY(name) FREE_NOCLASS (name)
#define DWG_OBJECT(name) FREE_NOCLASS (name)

  switch (obj->fixedtype)
    {
#include "objects.inc"

    case DWG_TYPE_FREED:
      break; // already freed

    case DWG_TYPE_ACDSRECORD:
    case DWG_TYPE_ACDSSCHEMA:
    case DWG_TYPE_NPOCOLLECTION:
    case DWG_TYPE_XREFPANELOBJECT:
    default:
      LOG_ERROR ("Unhandled class %s, fixedtype %d in objects.inc",
                 dwg_type_name (obj->fixedtype), (int)obj->fixedtype);
    }

#undef DWG_ENTITY
#undef DWG_OBJECT
#undef FREE_NOCLASS

  return DWG_ERR_UNHANDLEDCLASS;
}

// after downconvert_TABLESTYLE()
// we need to pass through the old code, as the new code is handled in the spec
// free() following from_version, not version.
static void
free_TABLESTYLE_r2010 (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  Dwg_Object_TABLESTYLE *_obj
      = obj->tio.object ? obj->tio.object->tio.TABLESTYLE : NULL;
  if (!obj || obj->fixedtype != DWG_TYPE_TABLESTYLE)
    {
      LOG_ERROR ("Invalid fixedtype %u for free_TABLESTYLE_r2010",
                 obj ? obj->fixedtype : 0);
      return;
    }
  LOG_HANDLE ("free_TABLESTYLE_r2010\n");
  if (_obj->rowstyles)
    for (unsigned i = 0; i < 3; i++)
      {
        for (unsigned j = 0; j < 6; j++)
          {
            SUB_FIELD_CMTC (rowstyles[i].borders[j], color, 0);
          }
        FREE_IF (_obj->rowstyles[i].borders);
        SUB_FIELD_HANDLE (rowstyles[i], text_style, 5, 7);
        SUB_FIELD_CMTC (rowstyles[i], text_color, 0);
        SUB_FIELD_CMTC (rowstyles[i], fill_color, 0);
      }
  FREE_IF (_obj->rowstyles);
  _obj->num_rowstyles = 0;
  for (unsigned j = 0; j < _obj->sty.cellstyle.num_borders; j++)
    {
      SUB_FIELD_HANDLE (sty.cellstyle.borders[j], ltype, 3, 340);
      SUB_FIELD_CMTC (sty.cellstyle.borders[j], color, 0);
    }
  FIELD_CMTC (sty.cellstyle.bg_color, 62);
  FIELD_T (sty.cellstyle.content_format.value_format_string, 300);
  if (_obj->numoverrides)
    {
      for (unsigned j = 0; j < _obj->ovr.cellstyle.num_borders; j++)
        {
          SUB_FIELD_HANDLE (ovr.cellstyle.borders[j], ltype, 3, 340);
          SUB_FIELD_CMTC (ovr.cellstyle.borders[j], color, 0);
        }
      FIELD_CMTC (ovr.cellstyle.bg_color, 62);
      FIELD_T (ovr.cellstyle.content_format.value_format_string, 300);
    }
  FIELD_TV (name, 3);
  FIELD_TV (sty.name, 300);
  FIELD_TV (ovr.name, 300);
}

static void
free_preR13_object (Dwg_Object *obj)
{
  int error = 0;
  long unsigned int j;
  Dwg_Data *dwg;
  Bit_Chain *dat = &pdat;

  // if (obj->name)
  //   LOG_HANDLE ("free_preR13_object: %s %d\n", obj->name, obj->index)
  if (obj && obj->parent)
    {
      dwg = obj->parent;
      dat->version = dwg->header.version;
      dat->from_version = dwg->header.from_version;
    }
  else
    return;
  if (obj->type == DWG_TYPE_FREED || obj->tio.object == NULL)
    return;

  if (obj->supertype == DWG_SUPERTYPE_ENTITY)
    {
      Dwg_Object_Entity *_obj = obj->tio.entity;
      FIELD_HANDLE (layer, 2, 8);
      if (_obj->flag_r11 & FLAG_R11_HAS_LTYPE) // 2
        FIELD_HANDLE (ltype, 1, 6);
      if (_obj->flag_r11 & FLAG_R11_HAS_HANDLING)
        {   // 32
          ; // obj->handle is static
        }
    }

  if (obj->fixedtype == DWG_TYPE_UNUSED // deleted
      && dwg->header.version < R_2_0b && obj->type > 64)
    {
      obj->type = -(int8_t)obj->type;
      // handle only entities with extra vectors specially
      switch (obj->type)
        {
        case DWG_TYPE_TEXT_r11:
          dwg_free_TEXT (dat, obj);
          break;
        case DWG_TYPE_ATTRIB_r11:
          dwg_free_ATTRIB (dat, obj);
          break;
        case DWG_TYPE_ATTDEF_r11:
          dwg_free_ATTDEF (dat, obj);
          break;
        case DWG_TYPE_BLOCK_r11:
          dwg_free_BLOCK (dat, obj);
          break;
        case DWG_TYPE_INSERT_r11:
          dwg_free_INSERT (dat, obj);
          break;
        case DWG_TYPE_POLYLINE_r11:
          dwg_free_POLYLINE_R11 (dat, obj);
          break;
        case DWG_TYPE_VERTEX_r11:
          dwg_free_VERTEX_R11 (dat, obj);
          break;
        case DWG_TYPE_DIMENSION_r11:
          switch (obj->tio.entity->flag_r11)
            {
            case 64:
              dwg_free_DIMENSION_LINEAR (dat, obj);
              break;
            case 65:
              dwg_free_DIMENSION_ALIGNED (dat, obj);
              break;
            case 66:
              dwg_free_DIMENSION_ANG2LN (dat, obj);
              break;
            case 68:
              dwg_free_DIMENSION_DIAMETER (dat, obj);
              break;
            case 72:
              dwg_free_DIMENSION_RADIUS (dat, obj);
              break;
            case 80:
              dwg_free_DIMENSION_ANG3PT (dat, obj);
              break;
            case 96:
              dwg_free_DIMENSION_ORDINATE (dat, obj);
              break;
            default:
              LOG_ERROR ("Unknown preR11 %s.flag_r11 %d", obj->name,
                         obj->tio.entity->flag_r11)
            }
          break;
        // now the rest
        default:
          dwg_free_POINT (dat, obj);
          break;
        }
    }

  // we could also use (Dwg_Object_Type_r11)obj->type
  switch (obj->fixedtype)
    {
    case DWG_TYPE_TEXT:
      dwg_free_TEXT (dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_free_ATTRIB (dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_free_ATTDEF (dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_free_BLOCK (dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_free_ENDBLK (dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_free_SEQEND (dat, obj);
      break;
    case DWG_TYPE_REPEAT:
      dwg_free_REPEAT (dat, obj);
      break;
    case DWG_TYPE_ENDREP:
      dwg_free_ENDREP (dat, obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_free_INSERT (dat, obj);
      break;
    case DWG_TYPE_VERTEX_R11:
      dwg_free_VERTEX_R11 (dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_free_VERTEX_2D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_free_VERTEX_3D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_free_VERTEX_MESH (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_free_VERTEX_PFACE (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_free_VERTEX_PFACE_FACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_R11:
      dwg_free_POLYLINE_R11 (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_free_POLYLINE_2D (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_free_POLYLINE_3D (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_free_POLYLINE_PFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_free_POLYLINE_MESH (dat, obj);
      break;
    case DWG_TYPE_ARC:
      dwg_free_ARC (dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_free_CIRCLE (dat, obj);
      break;
    case DWG_TYPE_LINE:
      dwg_free_LINE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_free_DIMENSION_ORDINATE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_free_DIMENSION_LINEAR (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_free_DIMENSION_ALIGNED (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_free_DIMENSION_ANG3PT (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_free_DIMENSION_ANG2LN (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_free_DIMENSION_RADIUS (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_free_DIMENSION_DIAMETER (dat, obj);
      break;
    case DWG_TYPE_POINT:
      dwg_free_POINT (dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_free__3DFACE (dat, obj);
      break;
    case DWG_TYPE__3DLINE:
      dwg_free__3DLINE (dat, obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_free_SOLID (dat, obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_free_TRACE (dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_free_SHAPE (dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_free_VIEWPORT (dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      // _ctrl->entries
      dwg_free_BLOCK_CONTROL (dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_free_BLOCK_HEADER (dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_free_LAYER_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_free_LAYER (dat, obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      dwg_free_STYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_STYLE:
      dwg_free_STYLE (dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_free_LTYPE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_free_LTYPE (dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_free_VIEW_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_free_VIEW (dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_free_UCS_CONTROL (dat, obj);
      break;
    case DWG_TYPE_UCS:
      dwg_free_UCS (dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_free_VPORT_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_free_VPORT (dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_free_APPID_CONTROL (dat, obj);
      break;
    case DWG_TYPE_APPID:
      dwg_free_APPID (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_free_DIMSTYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_free_DIMSTYLE (dat, obj);
      break;
    case DWG_TYPE_VX_CONTROL:
      dwg_free_VX_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VX_TABLE_RECORD:
      dwg_free_VX_TABLE_RECORD (dat, obj);
      break;
    case DWG_TYPE_LOAD:
      dwg_free_LOAD (dat, obj);
      break;
    case DWG_TYPE_JUMP:
      dwg_free_JUMP (dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_free_DICTIONARY (dat, obj);
      break;
    case DWG_TYPE_UNUSED:
      // deleted entity. leak? see above
      break;
    default:
      LOG_ERROR (
          "Unhandled preR13 class %s, fixedtype %d in free_preR13_object()",
          dwg_type_name (obj->fixedtype), (int)obj->fixedtype);
    }

  /* With indxf and injson the dxfname is dynamic, just the name is const */
  if (dwg->opts & DWG_OPTS_IN)
    FREE_IF (obj->dxfname);
  /* With injson even the name is dynamic */
  if (dwg->opts & DWG_OPTS_INJSON)
    FREE_IF (obj->name);
  obj->type = DWG_TYPE_FREED;
}

// using the global dat
EXPORT void
dwg_free_object (Dwg_Object *obj)
{
  int error = 0;
  long unsigned int j;
  Dwg_Data *dwg;
  Bit_Chain *dat = &pdat;

  if (obj && obj->parent)
    {
      dwg = obj->parent;
      dat->version = dwg->header.version;
      dat->from_version = dwg->header.from_version;
    }
  else
    return;
  if (obj->type == DWG_TYPE_FREED || obj->tio.object == NULL)
    return;

  PRE (R_13b1)
  {
    free_preR13_object (obj);
    return;
  }
  if (obj->fixedtype == DWG_TYPE_TABLESTYLE
      && dwg->header.from_version > R_2007)
    {
      free_TABLESTYLE_r2010 (dat, obj);
    }
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      dwg_free_TEXT (dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_free_ATTRIB (dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_free_ATTDEF (dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_free_BLOCK (dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_free_ENDBLK (dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_free_SEQEND (dat, obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_free_INSERT (dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      dwg_free_MINSERT (dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_free_VERTEX_2D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_free_VERTEX_3D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_free_VERTEX_MESH (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_free_VERTEX_PFACE (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_free_VERTEX_PFACE_FACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_free_POLYLINE_2D (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_free_POLYLINE_3D (dat, obj);
      break;
    case DWG_TYPE_ARC:
      dwg_free_ARC (dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_free_CIRCLE (dat, obj);
      break;
    case DWG_TYPE_LINE:
      dwg_free_LINE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_free_DIMENSION_ORDINATE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_free_DIMENSION_LINEAR (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_free_DIMENSION_ALIGNED (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_free_DIMENSION_ANG3PT (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_free_DIMENSION_ANG2LN (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_free_DIMENSION_RADIUS (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_free_DIMENSION_DIAMETER (dat, obj);
      break;
    case DWG_TYPE_POINT:
      dwg_free_POINT (dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_free__3DFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_free_POLYLINE_PFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_free_POLYLINE_MESH (dat, obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_free_SOLID (dat, obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_free_TRACE (dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_free_SHAPE (dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_free_VIEWPORT (dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_free_ELLIPSE (dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      dwg_free_SPLINE (dat, obj);
      break;
    case DWG_TYPE_REGION:
      dwg_free_REGION (dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
      dwg_free__3DSOLID (dat, obj);
      break; /* Check the type of the object */
    case DWG_TYPE_BODY:
      dwg_free_BODY (dat, obj);
      break;
    case DWG_TYPE_RAY:
      dwg_free_RAY (dat, obj);
      break;
    case DWG_TYPE_XLINE:
      dwg_free_XLINE (dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_free_DICTIONARY (dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      dwg_free_MTEXT (dat, obj);
      break;
    case DWG_TYPE_LEADER:
      dwg_free_LEADER (dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_free_TOLERANCE (dat, obj);
      break;
    case DWG_TYPE_MLINE:
      dwg_free_MLINE (dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      dwg_free_BLOCK_CONTROL (dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_free_BLOCK_HEADER (dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_free_LAYER_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_free_LAYER (dat, obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      dwg_free_STYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_STYLE:
      dwg_free_STYLE (dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_free_LTYPE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_free_LTYPE (dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_free_VIEW_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_free_VIEW (dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_free_UCS_CONTROL (dat, obj);
      break;
    case DWG_TYPE_UCS:
      dwg_free_UCS (dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_free_VPORT_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_free_VPORT (dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_free_APPID_CONTROL (dat, obj);
      break;
    case DWG_TYPE_APPID:
      dwg_free_APPID (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_free_DIMSTYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_free_DIMSTYLE (dat, obj);
      break;
    case DWG_TYPE_VX_CONTROL:
      dwg_free_VX_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VX_TABLE_RECORD:
      dwg_free_VX_TABLE_RECORD (dat, obj);
      break;
    case DWG_TYPE_GROUP:
      dwg_free_GROUP (dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      dwg_free_MLINESTYLE (dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_free_OLE2FRAME (dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_free_DUMMY (dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_free_LONG_TRANSACTION (dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      dwg_free_LWPOLYLINE (dat, obj);
      break;
    case DWG_TYPE_HATCH:
      dwg_free_HATCH (dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      dwg_free_XRECORD (dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      dwg_free_PLACEHOLDER (dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_free_OLEFRAME (dat, obj);
      break;
#ifdef DEBUG_VBA_PROJECT
    case DWG_TYPE_VBA_PROJECT:
      dwg_free_VBA_PROJECT (dat, obj);
      break;
#endif
    case DWG_TYPE_LAYOUT:
      dwg_free_LAYOUT (dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      dwg_free_PROXY_ENTITY (dat, obj);
      break;
    case DWG_TYPE_PROXY_OBJECT:
      dwg_free_PROXY_OBJECT (dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_type
          && obj->fixedtype == DWG_TYPE_LAYOUT)
        {
          SINCE (R_13b1)
          {
            dwg_free_LAYOUT (dat, obj); // XXX avoid double-free, esp. in eed
          }
        }
      else if ((error = dwg_free_variable_type (obj->parent, obj))
               & DWG_ERR_UNHANDLEDCLASS)
        {
          if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT)
            dwg_free_UNKNOWN_ENT (dat, obj);
          else if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
            dwg_free_UNKNOWN_OBJ (dat, obj);
        }
    }
  /* With indxf the dxfname is dynamic, just the name is const */
  if ((dwg->opts & DWG_OPTS_INDXF) || (dwg->opts & DWG_OPTS_INJSON))
    FREE_IF (obj->dxfname);
  /* With injson even the name is dynamic */
  if (dwg->opts & DWG_OPTS_INJSON)
    FREE_IF (obj->name);
  obj->type = DWG_TYPE_FREED;
}

/* Needed when we cast types.
   By fixedtype, not dxfname.
 */
EXPORT void
dwg_free_object_private (Dwg_Object *obj)
{
  int error = 0;
  long unsigned int j;
  Dwg_Data *dwg;
  Bit_Chain *dat = &pdat;

  if (obj && obj->parent)
    {
      dwg = obj->parent;
      dat->version = dwg->header.version;
      dat->from_version = dwg->header.from_version;
    }
  else
    return;
  if (obj->type == DWG_TYPE_FREED || obj->tio.object == NULL)
    return;

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      dwg_free_TEXT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      dwg_free_ATTRIB_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      dwg_free_ATTDEF_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      dwg_free_BLOCK_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      dwg_free_ENDBLK_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      dwg_free_SEQEND_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_INSERT:
      dwg_free_INSERT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      dwg_free_MINSERT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      dwg_free_VERTEX_2D_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      dwg_free_VERTEX_3D_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      dwg_free_VERTEX_MESH_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      dwg_free_VERTEX_PFACE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      dwg_free_VERTEX_PFACE_FACE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      dwg_free_POLYLINE_2D_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      dwg_free_POLYLINE_3D_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_ARC:
      dwg_free_ARC_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_free_CIRCLE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LINE:
      dwg_free_LINE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_free_DIMENSION_ORDINATE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      dwg_free_DIMENSION_LINEAR_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      dwg_free_DIMENSION_ALIGNED_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      dwg_free_DIMENSION_ANG3PT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      dwg_free_DIMENSION_ANG2LN_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_free_DIMENSION_RADIUS_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_free_DIMENSION_DIAMETER_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_POINT:
      dwg_free_POINT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      dwg_free__3DFACE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      dwg_free_POLYLINE_PFACE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      dwg_free_POLYLINE_MESH_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_SOLID:
      dwg_free_SOLID_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_TRACE:
      dwg_free_TRACE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      dwg_free_SHAPE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_free_VIEWPORT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_free_ELLIPSE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      dwg_free_SPLINE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_REGION:
      dwg_free_REGION_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
      dwg_free__3DSOLID_private (dat, dat, dat, obj);
      break; /* Check the type of the object */
    case DWG_TYPE_BODY:
      dwg_free_BODY_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_RAY:
      dwg_free_RAY_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_XLINE:
      dwg_free_XLINE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_free_DICTIONARY_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      dwg_free_MTEXT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LEADER:
      dwg_free_LEADER_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_free_TOLERANCE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_MLINE:
      dwg_free_MLINE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      dwg_free_BLOCK_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      dwg_free_BLOCK_HEADER_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      dwg_free_LAYER_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LAYER:
      dwg_free_LAYER_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      dwg_free_STYLE_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_STYLE:
      dwg_free_STYLE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      dwg_free_LTYPE_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      dwg_free_LTYPE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      dwg_free_VIEW_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VIEW:
      dwg_free_VIEW_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      dwg_free_UCS_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_UCS:
      dwg_free_UCS_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      dwg_free_VPORT_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VPORT:
      dwg_free_VPORT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      dwg_free_APPID_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_APPID:
      dwg_free_APPID_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      dwg_free_DIMSTYLE_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      dwg_free_DIMSTYLE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VX_CONTROL:
      dwg_free_VX_CONTROL_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_VX_TABLE_RECORD:
      dwg_free_VX_TABLE_RECORD_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_GROUP:
      dwg_free_GROUP_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      dwg_free_MLINESTYLE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_free_OLE2FRAME_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      dwg_free_DUMMY_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      dwg_free_LONG_TRANSACTION_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      dwg_free_LWPOLYLINE_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_HATCH:
      dwg_free_HATCH_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      dwg_free_XRECORD_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      dwg_free_PLACEHOLDER_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      dwg_free_OLEFRAME_private (dat, dat, dat, obj);
      break;
#ifdef DEBUG_VBA_PROJECT
    case DWG_TYPE_VBA_PROJECT:
      dwg_free_VBA_PROJECT_private (dat, dat, dat, obj);
      break;
#endif
    case DWG_TYPE_LAYOUT:
      dwg_free_LAYOUT_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      dwg_free_PROXY_ENTITY_private (dat, dat, dat, obj);
      break;
    case DWG_TYPE_PROXY_OBJECT:
      dwg_free_PROXY_OBJECT_private (dat, dat, dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_type
          && obj->fixedtype == DWG_TYPE_LAYOUT)
        {
          SINCE (R_13b1)
          {
            dwg_free_LAYOUT_private (
                dat, dat, dat, obj); // XXX avoid double-free, esp. in eed
          }
        }
      else
        dwg_free_variable_type_private (obj);
    }
}

static int
dwg_free_preR13_header_vars (Dwg_Data *dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;
  int error = 0;

  // fields added by dwg_add_Document:
  FIELD_TV (MENU, 0);
  FIELD_TFv (unit1_name, 32, 1);

  // clang-format off
  #include "header_variables_r11.spec"
  // clang-format on

  return 0;
}

static int
dwg_free_header_vars (Dwg_Data *dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;

  // clang-format off
  #include "header_variables.spec"
  // clang-format on

  FIELD_TV (DWGCODEPAGE, 0);
  return 0;
}

static int
dwg_free_summaryinfo (Dwg_Data *dwg)
{
  Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;

  // clang-format off
  #include "summaryinfo.spec"
  // clang-format on
  return 0;
}

static int
dwg_free_appinfo (Dwg_Data *dwg)
{
  Dwg_AppInfo *_obj = &dwg->appinfo;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;

  // clang-format off
  #include "appinfo.spec"
  // clang-format on
  return 0;
}
static int
dwg_free_filedeplist (Dwg_Data *dwg)
{
  Dwg_FileDepList *_obj = &dwg->filedeplist;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;
  BITCODE_RL vcount;

  // clang-format off
  #include "filedeplist.spec"
  // clang-format on
  return 0;
}
static int
dwg_free_security (Dwg_Data *dwg)
{
  Dwg_Security *_obj = &dwg->security;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;

  // clang-format off
  #include "security.spec"
  // clang-format on
  return 0;
}

static int
dwg_free_acds (Dwg_Data *dwg)
{
  Dwg_AcDs *_obj = &dwg->acds;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;
  BITCODE_RL rcount3 = 0, rcount4, vcount;
  int error = 0;

  // clang-format off
  #include "acds.spec"
  // clang-format on
  return 0;
}

void
dwg_free (Dwg_Data *dwg)
{
  BITCODE_BL i;
  if (dwg)
    {
      pdat.version = dwg->header.version;
      pdat.from_version = dwg->header.from_version;
      if (dwg->opts)
        {
          loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
          pdat.opts = dwg->opts;
        }
#ifdef USE_TRACING
      /* Before starting, set the logging level, but only do so once.  */
      if (!env_var_checked_p)
        {
          char *probe = getenv ("LIBREDWG_TRACE");
          if (probe)
            loglevel = atoi (probe);
          env_var_checked_p = 1;
        }
#endif /* USE_TRACING */
      LOG_INFO ("\n============\ndwg_free\n")
      // copied table fields have duplicate pointers, but are freed only once
      for (i = 0; i < dwg->num_objects; ++i)
        {
          if (!dwg_obj_is_control (&dwg->object[i]))
            dwg_free_object (&dwg->object[i]);
        }
      if (dwg->header.version < R_13b1)
        dwg_free_preR13_header_vars (dwg);
      else
        dwg_free_header_vars (dwg);
      dwg_free_summaryinfo (dwg);
      if (dwg->header.section_infohdr.num_desc)
        {
          for (i = 0; i < dwg->header.section_infohdr.num_desc; ++i)
            FREE_IF (dwg->header.section_info[i].sections);
          FREE_IF (dwg->header.section_info);
        }
      dwg_free_appinfo (dwg);
      dwg_free_filedeplist (dwg);
      dwg_free_security (dwg);
      dwg_free_acds (dwg);

      FREE_IF (dwg->thumbnail.chain);
      FREE_IF (dwg->vbaproject.unknown_bits);
      FREE_IF (dwg->revhistory.histories);
      FREE_IF (dwg->appinfohistory.unknown_bits);
      // FREE_IF (dwg->objfreespace...);
      FREE_IF (dwg->Template.description);
      FREE_IF (dwg->header.section);
      // FREE_IF (dwg->auxheader.R11_HANDSEED);

      for (i = 0; i < dwg->num_objects; ++i)
        {
          if (dwg_obj_is_control (&dwg->object[i]))
            dwg_free_object (&dwg->object[i]);
        }
      if (dwg->num_classes && dwg->dwg_class)
        {
          for (i = 0; i < dwg->num_classes; ++i)
            {
              FREE_IF (dwg->dwg_class[i].appname);
              FREE_IF (dwg->dwg_class[i].cppname);
              FREE_IF (dwg->dwg_class[i].dxfname);
              if (dwg->header.from_version >= R_2007)
                FREE_IF (dwg->dwg_class[i].dxfname_u);
            }
        }
      FREE_IF (dwg->dwg_class);
      if (dwg->object_ref)
        {
          LOG_HANDLE ("free %d global refs\n", dwg->num_object_refs)
          for (i = 0; i < dwg->num_object_refs; ++i)
            {
              LOG_INSANE ("free ref %d\n", i)
              FREE_IF (dwg->object_ref[i]);
            }
        }
      FREE_IF (dwg->object_ref);
      for (i = 0; i < dwg->num_acis_sab_hdl; ++i)
        {
          FREE_IF (dwg->acis_sab_hdl[i]);
        }
      FREE_IF (dwg->acis_sab_hdl);
      FREE_IF (dwg->object);
      if (dwg->object_map)
        {
          hash_free (dwg->object_map);
          dwg->object_map = NULL;
        }
      dwg->num_objects = dwg->num_classes = dwg->num_object_refs = 0;
      FREE_IF (dwg->object_ordered_ref);
      dwg->num_object_ordered_refs = 0;
#undef FREE_IF
    }
}

#undef IS_FREE

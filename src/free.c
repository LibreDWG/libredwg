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
#include "hash.h"

static unsigned int loglevel;
#ifdef USE_TRACING
static int env_var_checked_p;
#endif
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static Bit_Chain pdat = { NULL, 0, 0, 0, 0, 0 };

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

#define VALUE(value, type, dxf)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)

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
#define FIELD_VALUE(name) _obj->name
#define SUB_FIELD(o, nam, type, dxf) FIELD (_obj->o.nam, type)

#define ANYCODE -1
#define FIELD_HANDLE(nam, code, dxf) VALUE_HANDLE (_obj->nam, nam, code, dxf)
#define SUB_FIELD_HANDLE(o, nam, code, dxf)                                   \
  VALUE_HANDLE (_obj->o.nam, nam, code, dxf)
#define VALUE_HANDLE(ref, nam, _code, dxf)                                    \
  if (ref)                                                                    \
    {                                                                         \
      if (!(ref->handleref.size || (obj && ref->handleref.code > 5)))         \
        free (ref);                                                           \
      ref = NULL;                                                             \
    } /* else freed globally */
#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE (name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, code, dxf) FIELD_HANDLE (name, code, dxf)

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
#define FIELD_TFF(name, len, dxf)                                             \
  {                                                                           \
  }
#define FIELD_T(name, dxf) FIELD_TV (name, dxf)
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
#define FIELD_2DD(name, d1, d2, dxf)                                          \
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
#define FIELD_TIMEBLL(name, dxf)
#define FIELD_TIMERLL(name, dxf)
#define FIELD_CMC(color, dxf1, dxf2)                                          \
  {                                                                           \
    FIELD_T (color.name, 0);                                                  \
    FIELD_T (color.book_name, 0);                                             \
  }
#define SUB_FIELD_CMC(o, color, dxf1, dxf2)                                   \
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
      for (vcount = 0; vcount < (BITCODE_BL) (size); vcount++)                \
        FIELD_##type (nam[vcount], dxf);                                      \
    }                                                                         \
  FIELD_TV (nam, dxf);
#define FIELD_VECTOR_T(name, size, dxf)                                       \
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
      if (size)                                                               \
        {                                                                     \
          FIELD_TV (name, dxf)                                                \
        }                                                                     \
    }
#define HANDLE_VECTOR(name, sizefield, code, dxf)                             \
  HANDLE_VECTOR_N (name, FIELD_VALUE (sizefield), code, dxf)

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
  if (ent->reactors)                                                          \
    {                                                                         \
      for (vcount = 0; vcount < ent->num_reactors; vcount++)                  \
        VALUE_HANDLE (ent->reactors[vcount], reactors, code, 330);            \
      VALUE_TV (ent->reactors, 0);                                            \
    }
#define XDICOBJHANDLE(code)                                                   \
  SINCE (R_2004)                                                              \
  {                                                                           \
    if (!obj->tio.object->xdic_missing_flag)                                  \
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
  SINCE (R_2004)                                                              \
  {                                                                           \
    if (!ent->xdic_missing_flag)                                              \
      {                                                                       \
        VALUE_HANDLE (ent->xdicobjhandle, xdicobjhandle, code, 0);            \
      }                                                                       \
  }                                                                           \
  PRIOR_VERSIONS { VALUE_HANDLE (ent->xdicobjhandle, xdicobjhandle, code, 0); }

#define END_REPEAT(field) FIELD_TV (field, 0)

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

static int dwg_free_UNKNOWN_ENT (Bit_Chain *restrict dat,
                                 Dwg_Object *restrict obj);
static int dwg_free_UNKNOWN_OBJ (Bit_Chain *restrict dat,
                                 Dwg_Object *restrict obj);

#define DWG_ENTITY(token)                                                     \
  static int dwg_free_##token##_private (Bit_Chain *restrict dat,             \
                                         Dwg_Object *restrict obj);           \
                                                                              \
  static int dwg_free_##token (Bit_Chain *restrict dat,                       \
                               Dwg_Object *restrict obj)                      \
  {                                                                           \
    int error = 0;                                                            \
    if (obj->tio.entity)                                                      \
      {                                                                       \
        LOG_HANDLE ("Free entity " #token "\n")                               \
        if (obj->tio.entity->tio.token)                                       \
          error = dwg_free_##token##_private (dat, obj);                      \
                                                                              \
        dwg_free_common_entity_data (obj);                                    \
        dwg_free_eed (obj);                                                   \
        if (obj->tio.object)                                                  \
          {                                                                   \
            FREE_IF (obj->tio.entity->tio.token);                             \
            FREE_IF (obj->tio.entity);                                        \
          }                                                                   \
      }                                                                       \
    obj->parent = NULL;                                                       \
    return error;                                                             \
  }                                                                           \
  static int dwg_free_##token##_private (Bit_Chain *restrict dat,             \
                                         Dwg_Object *restrict obj)            \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    _ent = obj->tio.entity;                                                   \
    _obj = ent = _ent->tio.token;

#define DWG_ENTITY_END                                                        \
  return error;                                                               \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_free_##token##_private (Bit_Chain *restrict dat,             \
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
        LOG_HANDLE ("Free object " #token " %p\n", obj)                       \
        if (strcmp (#token, "UNKNOWN_OBJ")                                    \
            && obj->supertype == DWG_SUPERTYPE_UNKNOWN)                       \
          {                                                                   \
            _obj = NULL;                                                      \
            error = dwg_free_UNKNOWN_OBJ (dat, obj);                          \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            error = dwg_free_##token##_private (dat, obj);                    \
          }                                                                   \
        dwg_free_eed (obj);                                                   \
        FREE_IF (_obj);                                                       \
        FREE_IF (obj->tio.object);                                            \
      }                                                                       \
    obj->parent = NULL;                                                       \
    return error;                                                             \
  }                                                                           \
                                                                              \
  static int dwg_free_##token##_private (Bit_Chain *restrict dat,             \
                                         Dwg_Object *restrict obj)            \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Dwg_Object_##token *_obj;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    if (!obj->tio.object)                                                     \
      return 0;                                                               \
    _obj = obj->tio.object->tio.token;

/* obj itself is allocated via dwg->object[], dxfname is klass->dxfname or
 * static */
#define DWG_OBJECT_END                                                        \
  return error;                                                               \
  }

static void
dwg_free_common_entity_data (Dwg_Object *obj)
{

  Dwg_Data *dwg = obj->parent;
  Bit_Chain *dat = &pdat;
  Bit_Chain *hdl_dat = &pdat;
  Dwg_Object_Entity *_obj;
  Dwg_Object_Entity *ent;
  BITCODE_BL vcount;
  int error = 0;

  ent = obj->tio.entity;
  if (!ent)
    return;
  _obj = ent;

  // clang-format off
  #include "common_entity_data.spec"
  SINCE (R_13) {
  #include "common_entity_handle_data.spec"
  }
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

/* returns 1 if object could be freed and 0 otherwise
 */
static int
dwg_free_variable_type (Dwg_Data *restrict dwg, Dwg_Object *restrict obj)
{
  int i;
  int is_entity;
  Dwg_Class *klass;
  Bit_Chain *dat = &pdat;

  i = obj->type - 500;
  if (i < 0 || i > (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  // almost always false
  is_entity = dwg_class_is_entity (klass);

  // global class dispatcher
  // clang-format off
  #include "classes.inc"

  #undef WARN_UNHANDLED_CLASS
  #undef WARN_UNSTABLE_CLASS
  // clang-format on

  return DWG_ERR_UNHANDLEDCLASS;
}

// using the global dat
void
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
    }
  else
    return;
  if (obj->type == DWG_TYPE_FREED || obj->tio.object == NULL)
    return;
  dat->from_version = dat->version;
  if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
    goto unhandled;

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
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      dwg_free_VPORT_ENTITY_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      dwg_free_VPORT_ENTITY_HEADER (dat, obj);
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
      if (obj->type == obj->parent->layout_type)
        {
          SINCE (R_13)
          {
            dwg_free_LAYOUT (dat, obj); // XXX avoid double-free, esp. in eed
          }
        }

      else if ((error = dwg_free_variable_type (obj->parent, obj))
               & DWG_ERR_UNHANDLEDCLASS)
        {
          int is_entity;
          int i;
          Dwg_Class *klass;

        unhandled:
          i = obj->type - 500;
          klass = NULL;

          dwg = obj->parent;
          if (dwg->dwg_class && i >= 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = klass ? dwg_class_is_entity (klass) : 0;
            }
          if (klass && !is_entity)
            {
              dwg_free_UNKNOWN_OBJ (dat, obj);
            }
          else if (klass)
            {
              dwg_free_UNKNOWN_ENT (dat, obj);
            }
          else // not a class
            {
              FREE_IF (obj->tio.unknown);
            }
        }
    }
  obj->type = DWG_TYPE_FREED;
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
  return 0;
}

static int
dwg_free_summaryinfo (Dwg_Data *dwg)
{
  struct Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
  Dwg_Object *obj = NULL;
  Bit_Chain *dat = &pdat;
  BITCODE_RL rcount1, rcount2;
  // clang-format off
  #include "summaryinfo.spec"
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
      pdat.from_version = dwg->header.version;
      if (dwg->opts)
        loglevel = dwg->opts & 0xf;
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
      FREE_IF (dwg->header.section);
      dwg_free_header_vars (dwg);
      dwg_free_summaryinfo (dwg);
      FREE_IF (dwg->thumbnail.chain);
      if (dwg->header.section_infohdr.num_desc)
        {
          for (i = 0; i < dwg->header.section_infohdr.num_desc; ++i)
            FREE_IF (dwg->header.section_info[i].sections);
          FREE_IF (dwg->header.section_info);
        }
      for (i = 0; i < dwg->second_header.num_handlers; i++)
        FREE_IF (dwg->second_header.handlers[i].data);
      // auxheader has no strings
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
              if (dwg->header.version >= R_2007)
                FREE_IF (dwg->dwg_class[i].dxfname_u);
            }
          FREE_IF (dwg->dwg_class);
        }
      if (dwg->object_ref)
        {
          //LOG_HANDLE ("free %d refs:\n", dwg->num_object_refs)
          for (i = 0; i < dwg->num_object_refs; ++i)
            {
              //LOG_HANDLE ("free ref %d\n", i)
              FREE_IF (dwg->object_ref[i]);
            }
        }
      FREE_IF (dwg->object_ref);
      FREE_IF (dwg->object);
      if (dwg->object_map)
        hash_free (dwg->object_map);
#undef FREE_IF
    }
}

#undef IS_FREE

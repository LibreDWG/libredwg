/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * print.c: print helper functions
 * written by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "print.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static BITCODE_BL rcount1, rcount2;

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION print
#define IS_PRINT
#undef USE_WRITE

#define FIELD(nam, type) FIELD_TRACE (nam, type)
#define FIELDG(nam, type, dxf) FIELD_G_TRACE (nam, type, dxf)
#define FIELD_TRACE(nam, type)                                                \
  LOG_TRACE (#nam ": " FORMAT_##type " [" #type "]\n", _obj->nam)
#define FIELD_G_TRACE(nam, type, dxf)                                         \
  LOG_TRACE (#nam ": " FORMAT_##type " [" #type " " #dxf "]\n", _obj->nam)
#define FIELD_CAST(nam, type, cast, dxf)                                      \
  LOG_TRACE (#nam ": " FORMAT_##type " [" #type " " #dxf "]\n",               \
             (BITCODE_##type)_obj->nam)
#define SUB_FIELD(o, nam, type, dxf) FIELDG (o.nam, type, dxf)
#define SUB_FIELD_CAST(o, nam, type, cast, dxf)                               \
  FIELD_G_TRACE (o.nam, cast, dxf)

#define LOG_INSANE_TF(var, len)
#define FIELD_2PT_TRACE(name, type, dxf)                                      \
  {                                                                           \
    LOG_TRACE (#name ": (" FORMAT_BD ", " FORMAT_BD ") [" #type " %d]\n",     \
               _obj->name.x, _obj->name.y, dxf);                              \
  }
#define FIELD_3PT_TRACE(name, type, dxf)                                      \
  {                                                                           \
    LOG_TRACE (#name ": (" FORMAT_BD ", " FORMAT_BD ", " FORMAT_BD            \
                     ") [" #type " %d]\n",                                    \
               _obj->name.x, _obj->name.y, _obj->name.z, dxf);                \
  }

#define ANYCODE -1
#define VALUE_HANDLE(handleptr, name, handle_code, dxf)                       \
  if (handleptr)                                                              \
    {                                                                         \
      LOG_TRACE (#name ": HANDLE" FORMAT_REF " [%d]\n", ARGS_REF (handleptr), \
                 dxf);                                                        \
    }
#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  VALUE_HANDLE (_obj->nam, nam, handle_code, dxf)
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  VALUE_HANDLE (_obj->o.nam, nam, handle_code, dxf)
#define FIELD_DATAHANDLE(nam, code, dxf) FIELD_HANDLE (nam, code, dxf)
#define VALUE_HANDLE_N(handleptr, name, vcount, handle_code, dxf)             \
  if (handleptr)                                                              \
    {                                                                         \
      LOG_TRACE (#name "[%d]: HANDLE" FORMAT_REF " [%d]\n", (int)vcount,      \
                 ARGS_REF (handleptr), dxf);                                  \
    }
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)                        \
  VALUE_HANDLE_N (_obj->name, name, vcount, handle_code, dxf)

#define FIELD_B(name, dxf) FIELDG (name, B, dxf);
#define FIELD_BB(name, dxf) FIELDG (name, BB, dxf);
#define FIELD_3B(name, dxf) FIELDG (name, 3B, dxf);
#define FIELD_BS(name, dxf) FIELDG (name, BS, dxf);
#define FIELD_BL(name, dxf) FIELDG (name, BL, dxf);
#define FIELD_BLL(name, dxf) FIELDG (name, BLL, dxf);
#define FIELD_BD(name, dxf)                                                   \
  {                                                                           \
    if (bit_isnan (_obj->name))                                               \
      {                                                                       \
        LOG_ERROR ("Invalid BD " #name);                                      \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELDG (name, BD, dxf);                                                   \
  }
#define FIELD_RC(name, dxf) FIELDG (name, RC, dxf);
#define FIELD_RS(name, dxf) FIELDG (name, RS, dxf);
#define FIELD_RD(name, dxf)                                                   \
  {                                                                           \
    if (bit_isnan (_obj->name))                                               \
      {                                                                       \
        LOG_ERROR ("Invalid BD " #name);                                      \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
    FIELDG (name, RD, dxf);                                                   \
  }
#define FIELD_RL(name, dxf) FIELDG (name, RL, dxf);
#define FIELD_RLL(name, dxf) FIELDG (name, RLL, dxf);
#define FIELD_RLx(name, dxf)                                                  \
  LOG_TRACE (#name ": %x [RL " #dxf "]\n", _obj->name)
#define FIELD_MC(name, dxf) FIELDG (name, MC, dxf);
#define FIELD_MS(name, dxf) FIELDG (name, MS, dxf);
#define FIELD_TF(name, len, dxf)                                              \
  {                                                                           \
    LOG_TRACE (#name ": [%d TF " #dxf "]\n", len);                            \
    LOG_INSANE_TF (FIELD_VALUE (name), (int)len);                             \
  }
#define FIELD_TFF(name, len, dxf)                                             \
  {                                                                           \
    LOG_TRACE (#name ": [%d TFF " #dxf "]\n", len);                           \
    LOG_INSANE_TF (FIELD_VALUE (name), (int)len);                             \
  }

#define FIELD_TV(name, dxf) FIELDG (name, TV, dxf);
#define FIELD_TU(name, dxf) LOG_TRACE_TU (#name, (BITCODE_TU)_obj->name, dxf)
#define FIELD_T FIELD_TV /*TODO: implement version dependent string fields */
#define FIELD_BT(name, dxf) FIELDG (name, BT, dxf);
#define FIELD_4BITS(nam, dxf)                                                 \
  {                                                                           \
    int _b = _obj->nam;                                                       \
    LOG_TRACE (#nam ": b%d%d%d%d [4BITS %d]\n", _b & 8, _b & 4, _b & 2,       \
               _b & 1, dxf);                                                  \
  }
#define FIELD_BE(name, dxf) FIELD_3RD (name, dxf)
#define FIELD_DD(name, _default, dxf)
#define FIELD_2DD(name, def, dxf) FIELD_2PT_TRACE (name, DD, dxf)
#define FIELD_3DD(name, def, dxf) FIELD_3PT_TRACE (name, DD, dxf)
#define FIELD_2RD(name, dxf) FIELD_2PT_TRACE (name, RD, dxf)
#define FIELD_2BD(name, dxf) FIELD_2PT_TRACE (name, BD, dxf)
#define FIELD_2BD_1(name, dxf) FIELD_2PT_TRACE (name, BD, dxf)
#define FIELD_3RD(name, dxf) FIELD_3PT_TRACE (name, RD, dxf)
#define FIELD_3BD(name, dxf) FIELD_3PT_TRACE (name, BD, dxf)
#define FIELD_3BD_1(name, dxf) FIELD_3PT_TRACE (name, BD, dxf)
#define FIELD_3DPOINT(name, dxf) FIELD_3BD (name, dxf)
#define FIELD_CMC(color, dxf)                                                 \
  {                                                                           \
    LOG_TRACE (#color ".index: %d [CMC.BS %d]\n", _obj->color.index, dxf)     \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        LOG_TRACE (#color ".rgb: 0x%06x [CMC.BL %d]\n",                       \
                   (unsigned)_obj->color.rgb, dxf + 420 - 62);                \
        LOG_TRACE (#color ".flag: 0x%x [CMC.RC]\n",                           \
                   (unsigned)_obj->color.flag);                               \
        if (_obj->color.flag & 1)                                             \
          LOG_TRACE (#color ".name: %s [CMC.TV]\n", _obj->color.name);        \
        if (_obj->color.flag & 2)                                             \
          LOG_TRACE (#color ".bookname: %s [CMC.TV]\n",                       \
                     _obj->color.book_name);                                  \
      }                                                                       \
  }
#define SUB_FIELD_CMC(o, color, dxf)                                          \
  {                                                                           \
    LOG_TRACE (#color ".index: %d [CMC.BS %d]\n", _obj->o.color.index, dxf)   \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        LOG_TRACE (#color ".rgb: 0x%06x [CMC.BL %d]\n",                       \
                   (unsigned)_obj->o.color.rgb, dxf + 420 - 62);              \
        LOG_TRACE (#color ".flag: 0x%x [CMC.RC]\n",                           \
                   (unsigned)_obj->o.color.flag);                             \
        if (_obj->o.color.flag & 1)                                           \
          LOG_TRACE (#color ".name: %s [CMC.TV]\n", _obj->o.color.name);      \
        if (_obj->o.color.flag & 2)                                           \
          LOG_TRACE (#color ".bookname: %s [CMC.TV]\n",                       \
                     _obj->o.color.book_name);                                \
      }                                                                       \
  }
#define FIELD_ENC(color, dxf1, dxf2)                                          \
  {                                                                           \
    LOG_TRACE (#color ".index: %d [ENC.BS %d]\n", _obj->color.index, dxf1);   \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        if (_obj->color.flag)                                                 \
          LOG_TRACE (#color ".flag: 0x%x\n", (unsigned)_obj->color.flag);     \
        if (_obj->color.flag & 0x20)                                          \
          LOG_TRACE (#color ".alpha: 0%d [ENC.BL %d]\n",                      \
                     (int)_obj->color.alpha, dxf + 440 - 62);                 \
        if (_obj->color.flag & 0x40)                                          \
          LOG_TRACE (#color ".handle: " FORMAT_REF " [ENC.H %d]\n",           \
                     ARGS_REF (_obj->color.handle), dxf + 430 - 62);          \
        if (_obj->color.flag & 0x80)                                          \
          LOG_TRACE (#color ".rgb: 0x%06x [ENC.BL %d]\n",                     \
                     (unsigned)_obj->color.rgb, dxf + 420 - 62);              \
      }                                                                       \
  }

#define FIELD_TIMEBLL(name, dxf)                                              \
  LOG_TRACE (#name " " #dxf ": " FORMAT_BL "." FORMAT_BL "\n",                \
             _obj->name.days, _obj->name.ms)

#define VALUE(value, type, dxf)                                               \
  LOG_TRACE (FORMAT_##type " [" #type " " #dxf "]\n", value)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE ((BITCODE_RS)value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RLx(value, dxf)                                                 \
  LOG_TRACE (FORMAT_RLx " [RL " #dxf "]\n", (BITCODE_RL)value)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)
#define VALUE_BD(value, dxf) VALUE (value, BD, dxf)

// FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(name, type, size, dxf)                                 \
  if (size > 0 && _obj->name != NULL)                                         \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          LOG_TRACE (#name "[%ld]: " FORMAT_##type "\n", (long)vcount,        \
                     _obj->name[vcount])                                      \
        }                                                                     \
    }
#define FIELD_VECTOR_T(name, type, size, dxf)                                 \
  if (_obj->size > 0 && _obj->name != NULL)                                   \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          PRE (R_2007a)                                                       \
          {                                                                   \
            LOG_TRACE (#name "[%ld]: %s\n", (long)vcount, _obj->name[vcount]) \
          }                                                                   \
          else                                                                \
          {                                                                   \
            LOG_TRACE_TU (#name, _obj->name[vcount], dxf)                     \
          }                                                                   \
        }                                                                     \
    }

#define FIELD_VECTOR(name, type, size, dxf)                                   \
  FIELD_VECTOR_N (name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)                                     \
  if (_obj->name)                                                             \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2RD (name[vcount], dxf);                                      \
        }                                                                     \
    }

#define FIELD_2DD_VECTOR(name, size, dxf)                                     \
  if (_obj->name)                                                             \
    {                                                                         \
      FIELD_2RD (name[0], 0);                                                 \
      for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2DD (name[vcount], name[vcount - 1], dxf);                    \
        }                                                                     \
    }

#define FIELD_3DPOINT_VECTOR(name, size, dxf)                                 \
  if (_obj->name)                                                             \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_3DPOINT (name[vcount], dxf);                                  \
        }                                                                     \
    }

#define HANDLE_VECTOR_N(name, size, code, dxf)                                \
  if (_obj->name)                                                             \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIELD_HANDLE_N (name[vcount], vcount, code, dxf);                   \
        }                                                                     \
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf)                             \
  HANDLE_VECTOR_N (name, FIELD_VALUE (sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  FIELD_G_TRACE (num_inserts, type, dxf)

#define FIELD_XDATA(name, size)

#define REACTORS(code)                                                        \
  if (dat->version >= R_2000 && obj->tio.object->num_reactors > 0x1000)       \
    {                                                                         \
      LOG_ERROR ("Invalid num_reactors: %ld\n",                               \
                 (long)obj->tio.object->num_reactors);                        \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  if (obj->tio.object->reactors)                                              \
    {                                                                         \
      for (vcount = 0; vcount < obj->tio.object->num_reactors; vcount++)      \
        {                                                                     \
          VALUE_HANDLE_N (obj->tio.object->reactors[vcount], reactors,        \
                          vcount, code, -5);                                  \
        }                                                                     \
    }

#define XDICOBJHANDLE(code)                                                   \
  SINCE (R_2004a)                                                             \
  {                                                                           \
    if (!obj->tio.object->is_xdic_missing)                                    \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code, 0);  \
  }                                                                           \
  PRIOR_VERSIONS                                                              \
  {                                                                           \
    VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code, 0);    \
  }

#define COMMON_ENTITY_HANDLE_DATA /*  Empty */
#define SECTION_STRING_STREAM                                                 \
  {                                                                           \
    Bit_Chain sav_dat = *dat;                                                 \
    dat = str_dat;
#define START_STRING_STREAM                                                   \
  obj->has_strings = bit_read_B (dat);                                        \
  if (obj->has_strings)                                                       \
    {                                                                         \
      Bit_Chain sav_dat = *dat;                                               \
      obj_string_stream (dat, obj, dat);
#define END_STRING_STREAM                                                     \
  *dat = sav_dat;                                                             \
  }
#define START_HANDLE_STREAM                                                   \
  *hdl_dat = *dat;                                                            \
  if (dat->version >= R_2007)                                                 \
  bit_set_position (hdl_dat, obj->hdlpos)

#define DWG_ENTITY(token)                                                     \
  static int dwg_print_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat, \
                                          Bit_Chain *str_dat,                 \
                                          const Dwg_Object *restrict obj)     \
  {                                                                           \
    return 0;                                                                 \
  }                                                                           \
  static int dwg_print_##token (Bit_Chain *restrict dat,                      \
                                const Dwg_Object *restrict obj)               \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    LOG_INFO ("Entity " #token ":\n")                                         \
    _ent = obj->tio.entity;                                                   \
    _obj = ent = _ent->tio.token;                                             \
    dwg_print_##token##_private (dat, hdl_dat, str_dat, obj);                 \
    LOG_TRACE ("Entity handle: " FORMAT_H "\n", ARGS_H (obj->handle))

#define DWG_ENTITY_END                                                        \
  return 0;                                                                   \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_print_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat, \
                                          Bit_Chain *str_dat,                 \
                                          const Dwg_Object *restrict obj)     \
  {                                                                           \
    return 0;                                                                 \
  }                                                                           \
  static int dwg_print_##token (Bit_Chain *restrict dat,                      \
                                const Dwg_Object *restrict obj)               \
  {                                                                           \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Object_##token *_obj;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    LOG_INFO ("Object " #token ":\n")                                         \
    dwg_print_##token##_private (dat, hdl_dat, str_dat, obj);                 \
    _obj = obj->tio.object->tio.token;                                        \
    LOG_TRACE ("Object handle: " FORMAT_H "\n", ARGS_H (obj->handle))

#define DWG_OBJECT_END                                                        \
  return 0;                                                                   \
  }

#include "dwg.spec"

/* Returns 0 on success
   Dispatches on the variable types.
 */
static int
dwg_print_variable_type (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                         Dwg_Object *restrict obj)
{
  int i;
  int is_entity;
  Dwg_Class *klass;

  i = obj->type - 500;
  if (i < 0 || i > (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  // almost always false
  is_entity = dwg_class_is_entity (klass);

  // clang-format off
  #include "classes.inc"
  // clang-format on

  return DWG_ERR_UNHANDLEDCLASS;
}

/* prints to logging.h OUTPUT (ie stderr). Returns 0 on success
   Dispatches on the fixed types.
*/
int
dwg_print_object (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error = 0;
  unsigned int type;

  if (!obj || !obj->parent)
    return DWG_ERR_INTERNALERROR;
  type = dat->version < R_13b1 ? (unsigned int)obj->fixedtype : obj->type;
  // Bit_Chain * dat = (Bit_Chain *)obj->parent->bit_chain;
  // Bit_Chain *hdl_dat = dat;
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_print_TEXT (dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_print_ATTRIB (dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_print_ATTDEF (dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_print_BLOCK (dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_print_ENDBLK (dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_print_SEQEND (dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_print_INSERT (dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_print_MINSERT (dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_print_VERTEX_2D (dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_print_VERTEX_3D (dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_print_VERTEX_MESH (dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_print_VERTEX_PFACE (dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_print_VERTEX_PFACE_FACE (dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_print_POLYLINE_2D (dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_print_POLYLINE_3D (dat, obj);
    case DWG_TYPE_ARC:
      return dwg_print_ARC (dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_print_CIRCLE (dat, obj);
    case DWG_TYPE_LINE:
      return dwg_print_LINE (dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_print_DIMENSION_ORDINATE (dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_print_DIMENSION_LINEAR (dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_print_DIMENSION_ALIGNED (dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_print_DIMENSION_ANG3PT (dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_print_DIMENSION_ANG2LN (dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_print_DIMENSION_RADIUS (dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_print_DIMENSION_DIAMETER (dat, obj);
    case DWG_TYPE_POINT:
      return dwg_print_POINT (dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_print__3DFACE (dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_print_POLYLINE_PFACE (dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_print_POLYLINE_MESH (dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_print_SOLID (dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_print_TRACE (dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_print_SHAPE (dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_print_VIEWPORT (dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_print_ELLIPSE (dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_print_SPLINE (dat, obj);
    case DWG_TYPE_REGION:
      return dwg_print_REGION (dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_print__3DSOLID (dat, obj);
      /* Check the type of the object? */
    case DWG_TYPE_BODY:
      return dwg_print_BODY (dat, obj);
    case DWG_TYPE_RAY:
      return dwg_print_RAY (dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_print_XLINE (dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_print_DICTIONARY (dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_print_MTEXT (dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_print_LEADER (dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_print_TOLERANCE (dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_print_MLINE (dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_print_BLOCK_CONTROL (dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      return dwg_print_BLOCK_HEADER (dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_print_LAYER_CONTROL (dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_print_LAYER (dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_print_STYLE_CONTROL (dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_print_STYLE (dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_print_LTYPE_CONTROL (dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_print_LTYPE (dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_print_VIEW_CONTROL (dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_print_VIEW (dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_print_UCS_CONTROL (dat, obj);
    case DWG_TYPE_UCS:
      return dwg_print_UCS (dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_print_VPORT_CONTROL (dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_print_VPORT (dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_print_APPID_CONTROL (dat, obj);
    case DWG_TYPE_APPID:
      return dwg_print_APPID (dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_print_DIMSTYLE_CONTROL (dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_print_DIMSTYLE (dat, obj);
    case DWG_TYPE_VX_CONTROL:
      return dwg_print_VX_CONTROL (dat, obj);
    case DWG_TYPE_VX_TABLE_RECORD:
      return dwg_print_VX_TABLE_RECORD (dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_print_GROUP (dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_print_MLINESTYLE (dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_print_OLE2FRAME (dat, obj);
    case DWG_TYPE_DUMMY:
      return dwg_print_DUMMY (dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_print_LONG_TRANSACTION (dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_print_LWPOLYLINE (dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_print_HATCH (dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_print_XRECORD (dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_print_PLACEHOLDER (dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_print_OLEFRAME (dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own section\n");
      // dwg_print_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      return dwg_print_LAYOUT (dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_print_PROXY_ENTITY (dat, obj);
    case DWG_TYPE_PROXY_OBJECT: // DXF name: PROXY
      return dwg_print_PROXY_OBJECT (dat, obj);
    case DWG_TYPE_REPEAT:
      return dwg_print_REPEAT (dat, obj);
    case DWG_TYPE_ENDREP:
      return dwg_print_ENDREP (dat, obj);
    case DWG_TYPE__3DLINE:
      return dwg_print__3DLINE (dat, obj);
    case DWG_TYPE_LOAD:
      return dwg_print_LOAD (dat, obj);
    case DWG_TYPE_JUMP:
      return dwg_print_JUMP (dat, obj);
    case DWG_TYPE_POLYLINE_R11:
      return dwg_print_POLYLINE_R11 (dat, obj);
    case DWG_TYPE_VERTEX_R11:
      return dwg_print_VERTEX_R11 (dat, obj);
    default:
      if (obj->type == obj->parent->layout_type)
        {
          return dwg_print_LAYOUT (dat, obj);
        }
      /* > 500 */
      else if ((error = dwg_print_variable_type (obj->parent, dat, obj))
               & DWG_ERR_UNHANDLEDCLASS)
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity = 0;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (i > 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = klass ? dwg_class_is_entity (klass) : 0;
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              return dwg_print_UNKNOWN_OBJ (dat, obj);
            }
          else if (klass)
            {
              return dwg_print_UNKNOWN_ENT (dat, obj);
            }
          else // not a class
            {
              LOG_WARN ("Unknown object, skipping eed/reactors/xdic");
              SINCE (R_2000b)
              {
                LOG_INFO ("Object bitsize: %u\n", obj->bitsize);
              }
              LOG_INFO ("Object handle: " FORMAT_H "\n", ARGS_H (obj->handle));
              return error | DWG_ERR_INVALIDTYPE;
            }
        }
    }
  return DWG_ERR_UNHANDLEDCLASS;
}

#undef IS_PRINT

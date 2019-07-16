/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2019 Free Software Foundation, Inc.         */
/*  Copyright (C) 2010 Thien-Thi Nguyen                                      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * encode.c: encoding functions to write a DWG
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Thien-Thi Nguyen
 * modified by Till Heuschmann
 * modified by Anderson Pierre Cardoso
 * modified by Reini Urban
 */

#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#endif
#include <string.h>
#include <assert.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"
#include "decode.h"

/* The logging level for the write (encode) path.  */
static unsigned int loglevel;
/* the current version per spec block */
static unsigned int cur_ver = 0;

#ifdef USE_TRACING
/* This flag means we have checked the environment variable
   LIBREDWG_TRACE and set `loglevel' appropriately.  */
static bool env_var_checked_p;
#endif /* USE_TRACING */
#define DWG_LOGLEVEL loglevel

#include "logging.h"

/*--------------------------------------------------------------------------------
 * spec MACROS
 */

#define ACTION encode
#define IS_ENCODER

#define ANYCODE -1
#define REFS_PER_REALLOC 100

#define VALUE(value, type, dxf)                                               \
  {                                                                           \
    bit_write_##type (dat, value);                                            \
    LOG_TRACE (FORMAT_##type " [" #type " %d]\n", value, dxf);                \
  }
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)

#define FIELD(nam, type)                                                      \
  {                                                                           \
    bit_write_##type (dat, _obj->nam);                                        \
    FIELD_TRACE (nam, type);                                                  \
  }
#define FIELDG(nam, type, dxf)                                                \
  {                                                                           \
    bit_write_##type (dat, _obj->nam);                                        \
    FIELD_G_TRACE (nam, type, dxf);                                           \
  }
#define FIELD_TRACE(nam, type)                                                \
  LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)
#define FIELD_G_TRACE(nam, type, dxfgroup)                                    \
  LOG_TRACE (#nam ": " FORMAT_##type " [" #type " " #dxfgroup "]\n", _obj->nam)
#define FIELD_CAST(nam, type, cast, dxf)                                      \
  {                                                                           \
    bit_write_##type (dat, (BITCODE_##type)_obj->nam);                        \
    FIELD_G_TRACE (nam, cast, dxf);                                           \
  }
#define SUB_FIELD(o, nam, type, dxf) FIELD (o.nam, type)

#define FIELD_VALUE(nam) _obj->nam

#define FIELD_B(nam, dxf) FIELDG (nam, B, dxf)
#define FIELD_BB(nam, dxf) FIELDG (nam, BB, dxf)
#define FIELD_3B(nam, dxf) FIELDG (nam, 3B, dxf)
#define FIELD_BS(nam, dxf) FIELDG (nam, BS, dxf)
#define FIELD_BL(nam, dxf) FIELDG (nam, BL, dxf)
#define FIELD_BLL(nam, dxf) FIELDG (nam, BLL, dxf)
#define FIELD_BD(nam, dxf) FIELDG (nam, BD, dxf)
#define FIELD_RC(nam, dxf) FIELDG (nam, RC, dxf)
#define FIELD_RS(nam, dxf) FIELDG (nam, RS, dxf)
#define FIELD_RD(nam, dxf) FIELDG (nam, RD, dxf)
#define FIELD_RL(nam, dxf) FIELDG (nam, RL, dxf)
#define FIELD_RLL(nam, dxf) FIELDG (nam, RLL, dxf)
#define FIELD_MC(nam, dxf) FIELDG (nam, MC, dxf)
#define FIELD_MS(nam, dxf) FIELDG (nam, MS, dxf)
#define FIELD_TV(nam, dxf)                                                    \
  {                                                                           \
    IF_ENCODE_FROM_EARLIER { _obj->nam = strdup (""); }                       \
    FIELDG (nam, TV, dxf);                                                    \
  }
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (dat->version < R_2007)                                                \
      {                                                                       \
        FIELD_TV (nam, dxf)                                                   \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (obj->has_strings)                                                 \
          {                                                                   \
            FIELD_TU (nam, dxf)                                               \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            LOG_TRACE_TU (#nam, L"", dxf);                                    \
          }                                                                   \
      }                                                                       \
  }
#define FIELD_TF(nam, len, dxf)                                               \
  {                                                                           \
    if (_obj->nam && len > 0)                                                 \
      bit_write_TF (dat, (BITCODE_TF)_obj->nam, len);                         \
    FIELD_G_TRACE (nam, TF, dxf);                                             \
  }
#define FIELD_TFF(nam, len, dxf)                                              \
  {                                                                           \
    if (_obj->nam && len > 0)                                                 \
      bit_write_TF (dat, (BITCODE_TF)_obj->nam, len);                         \
    FIELD_G_TRACE (nam, TF, dxf);                                             \
  }
#define FIELD_TU(nam, dxf)                                                    \
  {                                                                           \
    if (_obj->nam)                                                            \
      bit_write_TU (dat, (BITCODE_TU)_obj->nam);                              \
    LOG_TRACE_TU (#nam, (BITCODE_TU)_obj->nam, dxf);                          \
  }
#define FIELD_BT(nam, dxf) FIELDG (nam, BT, dxf);

#define FIELD_DD(nam, _default, dxf)                                          \
  bit_write_DD (dat, FIELD_VALUE (nam), _default);
#define FIELD_2DD(nam, d1, d2, dxf)                                           \
  {                                                                           \
    FIELD_DD (nam.x, d1, dxf);                                                \
    FIELD_DD (nam.y, d2, dxf + 10);                                           \
  }
#define FIELD_3DD(nam, def, dxf)                                              \
  {                                                                           \
    FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                               \
    FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                          \
    FIELD_DD (nam.z, FIELD_VALUE (def.z), dxf + 20);                          \
  }
#define FIELD_2RD(nam, dxf)                                                   \
  {                                                                           \
    FIELDG (nam.x, RD, dxf);                                                  \
    FIELDG (nam.y, RD, dxf + 10);                                             \
  }
#define FIELD_2BD(nam, dxf)                                                   \
  {                                                                           \
    FIELDG (nam.x, BD, dxf);                                                  \
    FIELDG (nam.y, BD, dxf + 10);                                             \
  }
#define FIELD_2BD_1(nam, dxf)                                                 \
  {                                                                           \
    FIELDG (nam.x, BD, dxf);                                                  \
    FIELDG (nam.y, BD, dxf + 1);                                              \
  }
#define FIELD_3RD(nam, dxf)                                                   \
  {                                                                           \
    FIELDG (nam.x, RD, dxf);                                                  \
    FIELDG (nam.y, RD, dxf + 10);                                             \
    FIELDG (nam.z, RD, dxf + 20);                                             \
  }
#define FIELD_3BD(nam, dxf)                                                   \
  {                                                                           \
    FIELDG (nam.x, BD, dxf);                                                  \
    FIELDG (nam.y, BD, dxf + 10);                                             \
    FIELDG (nam.z, BD, dxf + 20);                                             \
  }
#define FIELD_3BD_1(nam, dxf)                                                 \
  {                                                                           \
    FIELDG (nam.x, BD, dxf);                                                  \
    FIELDG (nam.y, BD, dxf + 1);                                              \
    FIELDG (nam.z, BD, dxf + 2);                                              \
  }
#define FIELD_3DPOINT(nam, dxf) FIELD_3BD (nam, dxf)
#define FIELD_4BITS(nam, dxf) bit_write_4BITS (dat, _obj->nam);
#define FIELD_TIMEBLL(nam, dxf)                                               \
  {                                                                           \
    bit_write_TIMEBLL (dat, (BITCODE_TIMEBLL)_obj->nam);                      \
    LOG_TRACE (#nam ": " FORMAT_BL "." FORMAT_BL "\n", _obj->nam.days,        \
               _obj->nam.ms);                                                 \
  }

#define FIELD_CMC(nam, dxf1, dxf2) bit_write_CMC (dat, &_obj->nam)
#define SUB_FIELD_CMC(o, nam, dxf1, dxf2) bit_write_CMC (dat, &_obj->o.nam)

#define FIELD_BE(nam, dxf)                                                    \
  bit_write_BE (dat, FIELD_VALUE (nam.x), FIELD_VALUE (nam.y),                \
                FIELD_VALUE (nam.z));

// No overflow check with IS_RELEASE
#ifdef IS_RELEASE
#  define OVERFLOW_CHECK(nam, size)
#  define OVERFLOW_CHECK_LV(nam, size)
#else
#  define OVERFLOW_CHECK(nam, size)                                           \
    if ((size) > 0xff00)                                                      \
      {                                                                       \
        LOG_ERROR ("Invalid " #nam " %ld", (long)size);                       \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }
#  define OVERFLOW_CHECK_LV(nam, size)                                        \
    if ((size) > 0xff00)                                                      \
      {                                                                       \
        LOG_ERROR ("Invalid " #nam " %ld, set to 0", (long)size);             \
        size = 0;                                                             \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }
#endif

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  OVERFLOW_CHECK_LV (nam, _obj->size)                                         \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2RD (nam[vcount], dxf);                                           \
    }

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  OVERFLOW_CHECK (nam, _obj->size)                                            \
  FIELD_2RD (nam[0], dxf);                                                    \
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2DD (nam[vcount], FIELD_VALUE (nam[vcount - 1].x),                \
                 FIELD_VALUE (nam[vcount - 1].y), dxf);                       \
    }

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  OVERFLOW_CHECK_LV (nam, _obj->size)                                         \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_3DPOINT (nam[vcount], dxf);                                       \
    }

#define REACTORS(code)                                                        \
  if (obj->tio.object->reactors)                                              \
    {                                                                         \
      OVERFLOW_CHECK_LV (nam, obj->tio.object->num_reactors)                  \
      SINCE (R_13)                                                            \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)obj->tio.object->num_reactors;  \
             vcount++)                                                        \
          {                                                                   \
            VALUE_HANDLE (obj->tio.object->reactors[vcount], reactors, code,  \
                          330);                                               \
          }                                                                   \
      }                                                                       \
    }

#define XDICOBJHANDLE(code)                                                   \
  RESET_VER                                                                   \
  SINCE (R_2004)                                                              \
  {                                                                           \
    if (!obj->tio.object->xdic_missing_flag)                                  \
      {                                                                       \
        VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,    \
                      360);                                                   \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13)                                                              \
    {                                                                         \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
    }                                                                         \
  }                                                                           \
  RESET_VER

#define ENT_XDICOBJHANDLE(code)                                               \
  RESET_VER                                                                   \
  SINCE (R_2004)                                                              \
  {                                                                           \
    if (!obj->tio.entity->xdic_missing_flag)                                  \
      {                                                                       \
        VALUE_HANDLE (obj->tio.entity->xdicobjhandle, xdicobjhandle, code,    \
                      360);                                                   \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    SINCE (R_13)                                                              \
    {                                                                         \
      VALUE_HANDLE (obj->tio.entity->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
    }                                                                         \
  }                                                                           \
  RESET_VER

// FIELD_VECTOR_N(nam, type, size, dxf):
// writes a 'size' elements vector of data of the type indicated by 'type'
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  if (size > 0 && _obj->nam)                                                  \
    {                                                                         \
      OVERFLOW_CHECK (nam, size)                                              \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          bit_write_##type (dat, _obj->nam[vcount]);                          \
          LOG_TRACE (#nam "[%ld]: " FORMAT_##type "\n", (long)vcount,         \
                     _obj->nam[vcount])                                       \
        }                                                                     \
    }
#define FIELD_VECTOR_T(nam, size, dxf)                                        \
  if (_obj->size > 0 && _obj->nam)                                            \
    {                                                                         \
      OVERFLOW_CHECK_LV (nam, _obj->size)                                     \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          PRE (R_2007)                                                        \
          {                                                                   \
            bit_write_TV (dat, _obj->nam[vcount]);                            \
            LOG_TRACE (#nam "[%d]: %s\n", (int)vcount, _obj->nam[vcount])     \
          }                                                                   \
          else                                                                \
          {                                                                   \
            bit_write_TU (dat, (BITCODE_TU)_obj->nam[vcount]);                \
            LOG_TRACE_TU (#nam, _obj->nam[vcount], dxf)                       \
          }                                                                   \
        }                                                                     \
      RESET_VER                                                               \
    }

#define FIELD_VECTOR(nam, type, size, dxf)                                    \
  FIELD_VECTOR_N (nam, type, _obj->size, dxf)

#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  IF_ENCODE_SINCE_R13                                                         \
  {                                                                           \
    RESET_VER                                                                 \
    if (!hdlptr)                                                              \
      {                                                                       \
        Dwg_Handle null_handle = { 0, 0, 0 };                                 \
        bit_write_H (hdl_dat, &null_handle);                                  \
        LOG_TRACE (#nam ": HANDLE(0.0.0) absolute:0 [%d]\n", dxf)             \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (handle_code != ANYCODE && hdlptr->handleref.code != handle_code)  \
          {                                                                   \
            LOG_WARN ("Expected a CODE %d handle, got a %d", handle_code,     \
                      hdlptr->handleref.code);                                \
          }                                                                   \
        bit_write_H (hdl_dat, &hdlptr->handleref);                            \
        LOG_TRACE (#nam ": HANDLE(%x.%d.%lX) absolute:%lu [%d]\n",            \
                   hdlptr->handleref.code, hdlptr->handleref.size,            \
                   hdlptr->handleref.value, hdlptr->absolute_ref, dxf)        \
      }                                                                       \
  }

#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  VALUE_HANDLE (_obj->nam, nam, handle_code, dxf)
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  VALUE_HANDLE (_obj->o.nam, nam, handle_code, dxf)
#define FIELD_DATAHANDLE(nam, handle_code, dxf)                               \
  {                                                                           \
    bit_write_H (dat, _obj->nam ? &_obj->nam->handleref : NULL);              \
  }

#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf)                         \
  IF_ENCODE_SINCE_R13                                                         \
  {                                                                           \
    RESET_VER                                                                 \
    if (!_obj->nam)                                                           \
      bit_write_H (hdl_dat, NULL);                                            \
    else                                                                      \
      {                                                                       \
        if (handle_code != ANYCODE                                            \
            && _obj->nam->handleref.code != handle_code)                      \
          {                                                                   \
            LOG_WARN ("Expected a CODE %x handle, got a %x", handle_code,     \
                      _obj->nam->handleref.code);                             \
          }                                                                   \
        bit_write_H (hdl_dat, &_obj->nam->handleref);                         \
        LOG_TRACE (#nam "[%d]: HANDLE(%x.%d.%lX) absolute:%lu [%d]\n",        \
                   (int)vcount, _obj->nam->handleref.code,                    \
                   _obj->nam->handleref.size, _obj->nam->handleref.value,     \
                   _obj->nam->absolute_ref, dxf)                              \
      }                                                                       \
  }

#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  if (size > 0 && _obj->nam)                                                  \
    {                                                                         \
      OVERFLOW_CHECK (nam, size)                                              \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          if (_obj->nam[vcount])                                              \
            {                                                                 \
              FIELD_HANDLE_N (nam[vcount], vcount, code, dxf);                \
            }                                                                 \
        }                                                                     \
    }

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  for (vcount = 0; vcount < FIELD_VALUE (num_inserts); vcount++)              \
    {                                                                         \
      bit_write_RC (dat, 1);                                                  \
    }                                                                         \
  bit_write_RC (dat, 0)

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

#define FIELD_XDATA(nam, size)                                                \
  error |= dwg_encode_xdata (dat, _obj, _obj->size)

#define COMMON_ENTITY_HANDLE_DATA                                             \
  SINCE (R_13)                                                                \
  {                                                                           \
    error |= dwg_encode_common_entity_handle_data (dat, hdl_dat, obj);        \
  }                                                                           \
  RESET_VER

#define SECTION_STRING_STREAM                                                 \
  {                                                                           \
    Bit_Chain sav_dat = *dat;                                                 \
    dat = str_dat;

/* TODO: dump all TU strings here */
#define START_STRING_STREAM                                                   \
  bit_write_B (dat, obj->has_strings);                                        \
  RESET_VER                                                                   \
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
    bit_set_position (hdl_dat, obj->hdlpos);                                  \
  RESET_VER

#if 0
/** See dec_macro.h instead.
   Returns -1 if not added, else returns the new objid.
   Does a complete handleref rescan to invalidate and resolve
   all internal obj pointers after a object[] realloc.
*/
EXPORT long dwg_add_##token (Dwg_Data * dwg)    \
{                                               \
  Bit_Chain dat = { 0 };                        \
  BITCODE_BL num_objs  = dwg->num_objects;      \
  int error = 0;                                \
  dat.size = sizeof(Dwg_Entity_##token) + 40;   \
  dat.chain = calloc(dat.size, 1);              \
  dat.version = dwg->header.version;            \
  dat.from_version = dwg->header.from_version;  \
  bit_write_MS(&dat, dat.size);                 \
  if (dat.version >= R_2010) {                  \
    /* FIXME: should be UMC handlestream_size */ \
    bit_write_UMC(&dat, 8*sizeof(Dwg_Entity_##token)); \
    bit_write_BOT(&dat, DWG_TYPE_##token);      \
  } else {                                      \
    bit_write_BS(&dat, DWG_TYPE_##token);       \
  }                                             \
  bit_set_position(&dat, 0);                    \
  error = dwg_decode_add_object(dwg, &dat, &dat, 0);\
  if (-1 == error) \
    dwg_resolve_objectrefs_silent(dwg);         \
  if (num_objs == dwg->num_objects)             \
    return -1;                                  \
  else                                          \
    return (long)dwg->num_objects;              \
}

EXPORT long dwg_add_##token (Dwg_Data * dwg)     \
{                                                \
  Bit_Chain dat = { 0 };                         \
  int error = 0; \
  BITCODE_BL num_objs  = dwg->num_objects;       \
  dat.size = sizeof(Dwg_Object_##token) + 40;    \
  dat.chain = calloc(dat.size, 1);               \
  dat.version = dwg->header.version;             \
  dat.from_version = dwg->header.from_version;   \
  bit_write_MS(&dat, dat.size);                  \
  if (dat.version >= R_2010) {                   \
    /* FIXME: should be UMC handlestream_size */ \
    bit_write_UMC(&dat, 8*sizeof(Dwg_Object_##token)); \
    bit_write_BOT(&dat, DWG_TYPE_##token);       \
  } else {                                       \
    bit_write_BS(&dat, DWG_TYPE_##token);        \
  }                                              \
  bit_set_position(&dat, 0);                     \
  error = dwg_decode_add_object(dwg, &dat, &dat, 0);\
  if (-1 ==  error) \
    dwg_resolve_objectrefs_silent(dwg);          \
  if (num_objs == dwg->num_objects)              \
    return -1;                                   \
  else                                           \
    return (long)dwg->num_objects;               \
}

#endif

#define DWG_ENTITY(token)                                                     \
  static int dwg_encode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Dwg_Object_Entity *_ent = obj->tio.entity;                                \
    Dwg_Entity_##token *_obj = _ent->tio.token;                               \
    int error;                                                                \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    LOG_INFO ("Entity " #token ":\n")                                         \
    error = dwg_encode_entity (obj, dat, hdl_dat, str_dat);                   \
    if (error)                                                                \
      return error;

/* patch in the final bitsize */
#define DWG_ENTITY_END                                                        \
  if (obj->bitsize == 0 && dat->version >= R_13 && dat->version <= R_2010)    \
    {                                                                         \
      unsigned long address = bit_position (dat);                             \
      unsigned long bitsize = address - obj->bitsize_pos;                     \
      bit_set_position (dat, obj->bitsize_pos);                               \
      bit_write_RL (dat, bitsize);                                            \
      bit_set_position (dat, address);                                        \
      /* TODO CRC */                                                          \
    }                                                                         \
  return error;                                                               \
  }

/** Returns -1 if not added, else returns the new objid.
   Does a complete handleref rescan to invalidate and resolve
   all internal obj pointers after a object[] realloc.
*/
#define DWG_OBJECT(token)                                                     \
  static int dwg_encode_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    int error;                                                                \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_##token *_obj = obj->tio.object->tio.token;                    \
    error = dwg_encode_object (obj, dat, hdl_dat, str_dat);                   \
    if (error)                                                                \
      return error;                                                           \
    LOG_INFO ("Object " #token " handle: %x.%d.%lX\n", obj->handle.code,      \
              obj->handle.size, obj->handle.value)

#define DWG_OBJECT_END                                                        \
  if (obj->bitsize == 0 && dat->version >= R_13 && dat->version <= R_2007)    \
    {                                                                         \
      unsigned long address = bit_position (dat);                             \
      unsigned long bitsize = address - obj->bitsize_pos;                 \
      bit_set_position (dat, obj->bitsize_pos);                           \
      bit_write_RL (dat, bitsize);                                            \
      bit_set_position (dat, address);                                        \
      /* CRC? */                                                              \
    }                                                                         \
  return error;                                                               \
  }

#define ENT_REACTORS(code)                                                    \
  if (dat->version >= R_2000 && _obj->num_reactors > 0x1000)                  \
    {                                                                         \
      fprintf (stderr, "Invalid num_reactors: %ld\n",                         \
               (long)_obj->num_reactors);                                     \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  SINCE (R_13)                                                                \
  {                                                                           \
    for (vcount = 0; vcount < _obj->num_reactors; vcount++)                   \
      {                                                                       \
        FIELD_HANDLE_N (reactors[vcount], vcount, code, 330);                 \
      }                                                                       \
  }

#undef DEBUG_POS
#define DEBUG_POS                                                             \
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                     \
    {                                                                         \
      LOG_TRACE ("DEBUG_POS @%u.%u / 0x%x (%lu)\n", (unsigned int)dat->byte,  \
                 dat->bit, (unsigned int)dat->byte, bit_position (dat));      \
    }

/*--------------------------------------------------------------------------------*/
typedef struct
{
  long int handle;
  long int address;
  BITCODE_BL index;
} Object_Map;

/*--------------------------------------------------------------------------------
 * Private functions prototypes
 */
static int encode_preR13 (Dwg_Data *restrict dwg, Bit_Chain *restrict dat);

static int dwg_encode_entity (Dwg_Object *restrict obj, Bit_Chain *dat,
                              Bit_Chain *hdl_dat, Bit_Chain *str_dat);
static int dwg_encode_object (Dwg_Object *restrict obj,
                              Bit_Chain *restrict dat, Bit_Chain *hdl_dat,
                              Bit_Chain *str_dat);
static int dwg_encode_common_entity_handle_data (Bit_Chain *dat,
                                                 Bit_Chain *hdl_dat,
                                                 Dwg_Object *restrict obj);
static int dwg_encode_header_variables (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                        Bit_Chain *str_dat, Dwg_Data *dwg);
static int dwg_encode_variable_type (Dwg_Data *restrict dwg,
                                     Bit_Chain *restrict dat,
                                     Dwg_Object *restrict obj);
void dwg_encode_handleref (Bit_Chain *hdl_dat, Dwg_Object *obj,
                           Dwg_Data *restrict dwg,
                           Dwg_Object_Ref *restrict ref);
void dwg_encode_handleref_with_code (Bit_Chain *hdl_dat, Dwg_Object *obj,
                                     Dwg_Data *dwg,
                                     Dwg_Object_Ref *restrict ref,
                                     unsigned int code);
int dwg_encode_add_object (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                           unsigned long address);

static int dwg_encode_xdata (Bit_Chain *restrict dat,
                             Dwg_Object_XRECORD *restrict obj, int size);

/*--------------------------------------------------------------------------------
 * Public functions
 */

/**
 * dwg_encode(): the current generic encoder entry point.
 *
 * TODO: preR13 tables, 2007 maps.
 * 2010+ uses the 2004 format.
 * Returns a summary bitmask of all errors.
 */
int
dwg_encode (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  int ckr_missing = 1;
  int i, error = 0;
  BITCODE_BL j;
  long unsigned int section_address;
  unsigned char pvzbit;
  long unsigned int pvzadr;
  long unsigned int pvzadr_2;
  unsigned int ckr;
  unsigned int sekcisize = 0;
  long unsigned int last_address;
  long unsigned int last_handle;
  Object_Map *omap;
  Object_Map pvzmap;
  Bit_Chain *hdl_dat;

  if (dwg->opts)
    loglevel = dwg->opts & 0xf;
#ifdef USE_TRACING
  /* Before starting, set the logging level, but only do so once.  */
  if (!env_var_checked_p)
    {
      char *probe = getenv ("LIBREDWG_TRACE");
      if (probe)
        loglevel = atoi (probe);
      env_var_checked_p = true;
    }
#endif /* USE_TRACING */

  bit_chain_alloc (dat);
  hdl_dat = dat;

  /*------------------------------------------------------------
   * Header
   */
  strcpy ((char *)dat->chain,
          version_codes[dwg->header.version]); // Chain version
  dat->byte += 6;

  {
    struct Dwg_Header *_obj = &dwg->header;
    Dwg_Object *obj = NULL;

#include "header.spec"
  }
  section_address = dat->byte;

#define WE_CAN                                                                \
  "This version of LibreDWG is only capable of encoding "                     \
  "version R13-R2000 (code: AC1012-AC1015) DWG files.\n"

  PRE (R_13)
  {
    // TODO: tables, entities, block entities
    LOG_ERROR (WE_CAN "We don't encode tables, entities, blocks yet")
#ifndef IS_RELEASE
    return encode_preR13 (dwg, dat);
#endif
  }

  PRE (R_2004)
  {
    if (!dwg->header.num_sections) /* Usually 3-5, max 6 */
      dwg->header.num_sections = 6;
    bit_write_RL (dat, dwg->header.num_sections);
    if (!dwg->header.section)
      dwg->header.section
          = calloc (dwg->header.num_sections, sizeof (Dwg_Section));
    section_address = dat->byte; // Jump to section address
    dat->byte += (dwg->header.num_sections * 9);
    bit_write_CRC (dat, 0, 0xC0C1);

    bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_HEADER_END));

    /*------------------------------------------------------------
     * AuxHeader section 5
     * R2000+, mostly redundant file header information
     */
    dwg->header.section[5].number = 5;
    dwg->header.section[5].address = dat->byte;
    // dwg->header.section[5].size = 0;
    if (dwg->header.num_sections == 6)
      {
        struct Dwg_AuxHeader *_obj = &dwg->auxheader;
        Dwg_Object *obj = NULL;

#include "auxheader.spec"
      }
  }

  VERSION (R_2007)
  {
    LOG_ERROR (WE_CAN "We don't encode R2007 sections yet")
    return DWG_ERR_NOTYETSUPPORTED;
  }

  /* r2004 file header (compressed + encrypted) */
  SINCE (R_2004)
  {
    /* System Section */
    typedef union _system_section
    {
      unsigned char data[0x14]; // 20byte: 5*4
      struct
      {
        uint32_t section_type; /* 0x4163043b */
        uint32_t decomp_data_size;
        uint32_t comp_data_size;
        uint32_t compression_type;
        uint32_t checksum; // see section_page_checksum
      } fields;
    } system_section;

    system_section ss;
    Dwg_Section *section;

    Dwg_Object *obj = NULL;
    struct Dwg_R2004_Header *_obj = &dwg->r2004_header;
    const int size = sizeof (struct Dwg_R2004_Header);
    char encrypted_data[size];
    unsigned int rseed = 1;
    uint32_t checksum;

    LOG_ERROR (WE_CAN "We don't encode the R2004_section_map yet")

    if (dwg->header.section_infohdr.num_desc && !dwg->header.section_info)
      dwg->header.section_info = calloc (dwg->header.section_infohdr.num_desc,
                                         sizeof (Dwg_Section_Info));

    dat->byte = 0x80;
    for (i = 0; i < size; i++)
      {
        rseed *= 0x343fd;
        rseed += 0x269ec3;
        encrypted_data[i] = bit_read_RC (dat) ^ (rseed >> 0x10);
      }
    LOG_TRACE ("\n#### Write 2004 File Header ####\n");
    dat->byte = 0x80;
    if (dat->byte + 0x80 >= dat->size - 1)
      {
        dat->size = dat->byte + 0x80;
        bit_chain_alloc (dat);
      }
    memcpy (&dat->chain[0x80], encrypted_data, size);
    LOG_INFO ("@0x%lx\n", dat->byte);

#include "r2004_file_header.spec"

    dwg->r2004_header.checksum = 0;
    dwg->r2004_header.checksum = dwg_section_page_checksum (0, dat, size);

    /*-------------------------------------------------------------------------
     * Section Page Map
     */
    dat->byte = dwg->r2004_header.section_map_address + 0x100;

    LOG_TRACE ("\n=== Write System Section (Section Page Map) ===\n");
#ifndef HAVE_COMPRESS_R2004_SECTION
    dwg->r2004_header.comp_data_size = dwg->r2004_header.decomp_data_size;
    dwg->r2004_header.compression_type = 0;
#endif
    FIELD_RL (section_type, 0); // should be 0x4163043b
    FIELD_RL (decomp_data_size, 0);
    FIELD_RL (comp_data_size, 0);
    FIELD_RL (compression_type, 0);
    dwg_section_page_checksum (dwg->r2004_header.checksum, dat, size);
    FIELD_RL (checksum, 0);
    LOG_TRACE ("\n")

    LOG_WARN ("TODO write_R2004_section_map(dat, dwg)")
    LOG_TRACE ("\n")

    return DWG_ERR_NOTYETSUPPORTED;
  }

  /*------------------------------------------------------------
   * Picture (Pre-R13C3?)
   */
  if (dwg->header.preview_addr)
    {
      dat->byte = dwg->header.preview_addr;
      // dwg->picture.size = 0; // If one desires not to copy preview pictures,
      // should un-comment this line
      bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_PICTURE_BEGIN));
      bit_write_TF (dat, (char *)dwg->picture.chain, dwg->picture.size);
      if (dwg->picture.size == 0)
        {
          bit_write_RL (dat, 5);
          bit_write_RC (dat, 0);
        }
      bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_PICTURE_END));
    }

  /*------------------------------------------------------------
   * Header Variables
   */
  dwg->header.section[0].number = 0;
  dwg->header.section[0].address = dat->byte;
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_BEGIN));
  pvzadr = dat->byte; // Afterwards one must rewrite the correct values of size
                      // here

  bit_write_RL (dat, 0); // Size of the section

  // encode
  // if (dat->version >= R_2007)
  //  str_dat = dat;
  dwg_encode_header_variables (dat, hdl_dat, dat, dwg);

  /* Write the size of the section at its beginning
   */
  pvzadr_2 = dat->byte;
  pvzbit = dat->bit;
  dat->byte = pvzadr;
  dat->bit = 0;
  bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
  dat->byte = pvzadr_2;
  dat->bit = pvzbit;
  // printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

  /* CRC and sentinel
   */
  bit_write_CRC (dat, pvzadr, 0xC0C1);

  // XXX trying to fix CRC 2-byte overflow. Must find actual reason
  dat->byte -= 2;

  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_END));
  dwg->header.section[0].size = dat->byte - dwg->header.section[0].address;

  /*------------------------------------------------------------
   * Classes
   */
  dwg->header.section[1].number = 1;
  dwg->header.section[1].address = dat->byte;
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_BEGIN));
  pvzadr = dat->byte; // Afterwards one must rewrite the correct values of size
                      // here
  bit_write_RL (dat, 0); // Size of the section

  for (j = 0; j < dwg->num_classes; j++)
    {
      Dwg_Class *klass;
      klass = &dwg->dwg_class[j];
      bit_write_BS (dat, klass->number);
      bit_write_BS (dat, klass->proxyflag);
      bit_write_TV (dat, klass->appname);
      bit_write_TV (dat, klass->cppname);
      bit_write_TV (dat, klass->dxfname);
      bit_write_B (dat, klass->wasazombie);
      bit_write_BS (dat, klass->item_class_id);
      LOG_TRACE ("Class %d 0x%x %s\n"
                 "%s \"%s\" %d 0x%x\n",
                 klass->number, klass->proxyflag, klass->dxfname,
                 klass->cppname, klass->appname, klass->wasazombie,
                 klass->item_class_id)

      SINCE (R_2007)
      {
        bit_write_BL (dat, klass->num_instances);
        bit_write_BL (dat, klass->dwg_version);
        bit_write_BL (dat, klass->maint_version);
        bit_write_BL (dat, klass->unknown_1);
        bit_write_BL (dat, klass->unknown_2);
        LOG_TRACE ("%d %d\n", (int)klass->num_instances,
                   (int)klass->dwg_version);
      }
    }

  /* Write the size of the section at its beginning
   */
  pvzadr_2 = dat->byte;
  pvzbit = dat->bit;
  dat->byte = pvzadr;
  dat->bit = 0;
  bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
  dat->byte = pvzadr_2;
  dat->bit = pvzbit;
  // printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

  /* CRC and sentinel
   */
  bit_write_CRC (dat, pvzadr, 0xC0C1);

  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_END));
  dwg->header.section[1].size = dat->byte - dwg->header.section[1].address;

  bit_write_RL (dat,
                0x00000000); // 0xDCA Unknown bitlong inter class and objects

  /*------------------------------------------------------------
   * Objects
   */

  pvzadr = dat->byte;

  /* Define object-map
   */
  omap = (Object_Map *)malloc (dwg->num_objects * sizeof (Object_Map));
  if (!omap)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  for (j = 0; j < dwg->num_objects; j++)
    {
      /* Define the handle of each object, including unknown */
      omap[j].index = j;
      omap[j].handle = dwg->object[j].handle.value;

      /* Arrange the sequence of handles according to a growing order  */
      if (j > 0)
        {
          BITCODE_BL k = j;
          while (omap[k].handle < omap[k - 1].handle)
            {
              pvzmap.handle = omap[k].handle;
              pvzmap.index = omap[k].index;

              omap[k - 1].handle = pvzmap.handle;
              omap[k - 1].index = pvzmap.index;

              omap[k].handle = omap[k - 1].handle;
              omap[k].index = omap[k - 1].index;

              k--;
              if (k == 0)
                break;
            }
        }
    }
  // for (i = 0; i < dwg->num_objects; i++)
  //  printf ("Handle(%i): %lu / Idc: %u\n", i, omap[i].handle, omap[i].index);

  /* Write the re-sorted objects
   */
  for (j = 0; j < dwg->num_objects; j++)
    {
      Dwg_Object *obj;
      BITCODE_BL index = omap[j].index;
      LOG_TRACE ("\n> Next object: " FORMAT_BL " \tHandle: %lX\tOffset: %lu\n"
                 "==========================================\n",
                 j, omap[j].handle, dat->byte);
      omap[j].address = dat->byte;
      if (index > dwg->num_objects)
        {
          LOG_ERROR ("Invalid object map index " FORMAT_BL ", max " FORMAT_BL
                     ". Skipping",
                     index, dwg->num_objects)
          error |= DWG_ERR_VALUEOUTOFBOUNDS;
          continue;
        }
      obj = &dwg->object[index];
      obj->address
          = dat->byte; // change the address to the linearly sorted one
      error |= dwg_encode_add_object (obj, dat, dat->byte);
      bit_write_CRC (dat, omap[j].address, 0xC0C1);
    }

  /*for (j = 0; j < dwg->num_objects; j++)
      LOG_INFO ("Object(%lu): %6lu / Address: %08lX / Idc: %u\n",
                 j, omap[j].handle, omap[j].address, omap[j].index);
  */

  /* Unknown bitdouble between objects and object map (or short?)
   */
  bit_write_RS (dat, 0);

  /*------------------------------------------------------------
   * Object-map
   */
  dwg->header.section[2].number = 2;
  dwg->header.section[2].address
      = dat->byte; // Value of size should be calculated later
  // printf ("Begin: 0x%08X\n", dat->byte);

  sekcisize = 0;
  pvzadr = dat->byte; // Correct value of section size must be written later
  dat->byte += 2;
  last_address = 0;
  last_handle = 0;
  for (j = 0; j < dwg->num_objects; j++)
    {
      BITCODE_BL index;
      long int pvz;

      index = omap[j].index;

      pvz = omap[index].handle - last_handle;
      bit_write_UMC (dat, pvz);
      // printf ("Handle(%i): %6lu / ", j, pvz);
      last_handle = omap[index].handle;

      pvz = omap[index].address - last_address;
      bit_write_MC (dat, pvz);
      // printf ("Address: %08X\n", pvz);
      last_address = omap[index].address;

      // dwg dwg_encode_add_object(dwg->object[j], dat, last_address);

      ckr_missing = 1;
      if (dat->byte - pvzadr > 2030) // 2029
        {
          ckr_missing = 0;
          sekcisize = dat->byte - pvzadr;
          dat->chain[pvzadr] = sekcisize >> 8;
          dat->chain[pvzadr + 1] = sekcisize & 0xFF;
          bit_write_CRC (dat, pvzadr, 0xC0C1);

          pvzadr = dat->byte;
          dat->byte += 2;
          last_address = 0;
          last_handle = 0;
        }
    }
  // printf ("Obj size: %u\n", i);
  if (ckr_missing)
    {
      sekcisize = dat->byte - pvzadr;
      dat->chain[pvzadr] = sekcisize >> 8;
      dat->chain[pvzadr + 1] = sekcisize & 0xFF;
      bit_write_CRC (dat, pvzadr, 0xC0C1);
    }
  pvzadr = dat->byte;
  bit_write_RC (dat, 0);
  bit_write_RC (dat, 2);
  bit_write_CRC (dat, pvzadr, 0xC0C1);

  /* Calculate and write the size of the object map
   */
  dwg->header.section[2].size = dat->byte - dwg->header.section[2].address;
  free (omap);

  /*------------------------------------------------------------
   * Second header, section 3. R13-R2000 only.
   * But partially also since r2004.
   */
  SINCE (R_13)
  {
    struct _dwg_second_header *_obj = &dwg->second_header;
    Dwg_Object *obj = NULL;
    BITCODE_BL vcount;

    dwg->header.section[3].number = 3;
    dwg->header.section[3].address = dwg->second_header.address;
    dwg->header.section[3].size = dwg->second_header.size;
    bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_BEGIN));

    LOG_INFO ("\n=======> Second Header: %8X\n", (unsigned int)dat->byte - 16);
    pvzadr = dat->byte; // Keep the first address of the section to write its
                        // size later
    LOG_TRACE ("pvzadr: %lx\n", pvzadr)

    FIELD_RL (size, 0);
    if (FIELD_VALUE (address) != (BITCODE_RL) (pvzadr - 16))
      {
        LOG_WARN ("second_header->address %x != %x", FIELD_VALUE (address),
                  (unsigned)(pvzadr - 16));
        FIELD_VALUE (address) = pvzadr - 16;
      }
    FIELD_BL (address, 0);

    // AC1012, AC1014 or AC1015. This is a char[11], zero padded.
    // with \n at 12.
    bit_write_TF (dat, (char *)_obj->version, 12);
    LOG_TRACE ("version: %s\n", _obj->version)

    for (i = 0; i < 4; i++)
      FIELD_B (null_b[i], 0);
    FIELD_RC (unknown_10, 0); // 0x10
    for (i = 0; i < 4; i++)
      FIELD_RC (unknown_rc4[i], 0);

    UNTIL (R_2000)
    {
      FIELD_RC (num_sections, 0); // r14: 5, r2000: 6
      for (i = 0; i < FIELD_VALUE (num_sections); i++)
        {
          FIELD_RC (section[i].nr, 0);
          FIELD_BL (section[i].address, 0);
          FIELD_BL (section[i].size, 0);
        }

      FIELD_BS (num_handlers, 0); // 14, resp. 16 in r14
      if (FIELD_VALUE (num_handlers) > 16)
        {
          LOG_ERROR ("Second header num_handlers > 16: %d\n",
                     FIELD_VALUE (num_handlers));
          FIELD_VALUE (num_handlers) = 14;
        }
      for (i = 0; i < FIELD_VALUE (num_handlers); i++)
        {
          FIELD_RC (handlers[i].size, 0);
          FIELD_RC (handlers[i].nr, 0);
          FIELD_VECTOR (handlers[i].data, RC, handlers[i].size, 0);
        }

      bit_write_CRC (dat, pvzadr, 0xC0C1);

      VERSION (R_14)
      {
        FIELD_RL (junk_r14_1, 0);
        FIELD_RL (junk_r14_2, 0);
      }
    }
    bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_END));
  }
  else if (dwg->header.num_sections >= 3)
  {
    dwg->header.section[3].number = 3;
    dwg->header.section[3].address = 0;
    dwg->header.section[3].size = 0;
  }

  /*------------------------------------------------------------
   * MEASUREMENT Section 4
   */
  if (dwg->header.num_sections >= 4)
    {
      dwg->header.section[4].number = 4;
      dwg->header.section[4].address = dat->byte;
      dwg->header.section[4].size = 4;
      bit_write_RL (dat, dwg->measurement);
    }

  /* End of the file
   */
  dat->size = dat->byte;

  /* Write section addresses (XXX pre2004 only?)
   */
  dat->byte = section_address;
  dat->bit = 0;
  for (j = 0; j < dwg->header.num_sections; j++)
    {
      bit_write_RC (dat, dwg->header.section[j].number);
      bit_write_RL (dat, dwg->header.section[j].address);
      bit_write_RL (dat, dwg->header.section[j].size);
    }

  /* Write CRC's
   */
  bit_write_CRC (dat, 0, 0);
  dat->byte -= 2;
  ckr = bit_read_CRC (dat);
  dat->byte -= 2;
  switch (dwg->header.num_sections)
    {
    case 3:
      bit_write_RS (dat, ckr ^ 0xA598);
      break;
    case 4:
      bit_write_RS (dat, ckr ^ 0x8101);
      break;
    case 5:
      bit_write_RS (dat, ckr ^ 0x3CC4);
      break;
    case 6:
      bit_write_RS (dat, ckr ^ 0x8461);
      break;
    default:
      bit_write_RS (dat, ckr);
    }

  return 0;
}

static int
encode_preR13 (Dwg_Data *dwg, Bit_Chain *dat)
{
  return DWG_ERR_NOTYETSUPPORTED;
}

// needed for r2004+ encode and decode (check-only) (unused)
// p 4.3: first calc with seed 0, then compress, then recalc with prev.
// checksum
uint32_t
dwg_section_page_checksum (const uint32_t seed, Bit_Chain *dat, uint32_t size)
{
  uint32_t sum1 = seed & 0xffff;
  uint32_t sum2 = seed >> 0x10;
  unsigned char *data = &(dat->chain[dat->byte]);

  while (size)
    {
      uint32_t i;
      uint32_t chunksize = size < 0x15b0 ? size : 0x15b0;
      size -= chunksize;
      for (i = 0; i < chunksize; i++)
        {
          sum1 += *data++;
          sum2 += sum1;
        }
      sum1 %= 0xFFF1;
      sum2 %= 0xFFF1;
    }
  return (sum2 << 0x10) | (sum1 & 0xffff);
}

#include "dwg.spec"

/** dwg_encode_variable_type
 * Encode object by class name, not type. if type > 500.
 * Returns 0 on success, else some Dwg_Error.
 */
static int
dwg_encode_variable_type (Dwg_Data *dwg, Bit_Chain *dat, Dwg_Object *obj)
{
  int i, error = 0;
  int is_entity;
  Dwg_Class *klass;

  i = obj->type - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    {
      LOG_WARN ("Invalid object type %d, only %u classes", obj->type,
                dwg->num_classes);
      return DWG_ERR_INVALIDTYPE;
    }

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  obj->dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity (klass);

#include "classes.inc"

  LOG_WARN ("Unknown Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",
            klass->number, klass->dxfname, klass->proxyflag,
            klass->wasazombie ? " was proxy" : "")

#undef WARN_UNHANDLED_CLASS
#undef WARN_UNSTABLE_CLASS

  return DWG_ERR_UNHANDLEDCLASS;
}

int
dwg_encode_add_object (Dwg_Object *obj, Bit_Chain *dat, unsigned long address)
{
  int error = 0;
  unsigned long previous_address;
  unsigned long object_address;
  unsigned char previous_bit;

  previous_address = dat->byte;
  previous_bit = dat->bit;
  dat->byte = address;
  dat->bit = 0;

  LOG_INFO ("Object number: %lu", (unsigned long)obj->index);
  while (dat->byte + obj->size >= dat->size)
    bit_chain_alloc (dat);

  // TODO: calculate size from the fields. either <0x7fff or more
  // patch it afterwards and check old<>new size if enough space allocated.
  bit_write_MS (dat, obj->size);
  PRE (R_2010) { bit_write_BS (dat, obj->type); }
  LATER_VERSIONS
  {
    if (!obj->handlestream_size && obj->bitsize)
      obj->handlestream_size = obj->size * 8 - obj->bitsize;
    bit_write_UMC (dat, obj->handlestream_size);
    bit_write_BOT (dat, obj->type);
  }

  LOG_INFO (", Size: %d, Type: %d\n", obj->size, obj->type)

  /* Write the specific type to dat */
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      error = dwg_encode_TEXT (dat, obj);
      break;
    case DWG_TYPE_ATTRIB:
      error = dwg_encode_ATTRIB (dat, obj);
      break;
    case DWG_TYPE_ATTDEF:
      error = dwg_encode_ATTDEF (dat, obj);
      break;
    case DWG_TYPE_BLOCK:
      error = dwg_encode_BLOCK (dat, obj);
      break;
    case DWG_TYPE_ENDBLK:
      error = dwg_encode_ENDBLK (dat, obj);
      break;
    case DWG_TYPE_SEQEND:
      error = dwg_encode_SEQEND (dat, obj);
      break;
    case DWG_TYPE_INSERT:
      error = dwg_encode_INSERT (dat, obj);
      break;
    case DWG_TYPE_MINSERT:
      error = dwg_encode_MINSERT (dat, obj);
      break;
    case DWG_TYPE_VERTEX_2D:
      error = dwg_encode_VERTEX_2D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_3D:
      error = dwg_encode_VERTEX_3D (dat, obj);
      break;
    case DWG_TYPE_VERTEX_MESH:
      error = dwg_encode_VERTEX_MESH (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE:
      error = dwg_encode_VERTEX_PFACE (dat, obj);
      break;
    case DWG_TYPE_VERTEX_PFACE_FACE:
      error = dwg_encode_VERTEX_PFACE_FACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_2D:
      error = dwg_encode_POLYLINE_2D (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_3D:
      error = dwg_encode_POLYLINE_3D (dat, obj);
      break;
    case DWG_TYPE_ARC:
      error = dwg_encode_ARC (dat, obj);
      break;
    case DWG_TYPE_CIRCLE:
      error = dwg_encode_CIRCLE (dat, obj);
      break;
    case DWG_TYPE_LINE:
      error = dwg_encode_LINE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      error = dwg_encode_DIMENSION_ORDINATE (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      error = dwg_encode_DIMENSION_LINEAR (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      error = dwg_encode_DIMENSION_ALIGNED (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      error = dwg_encode_DIMENSION_ANG3PT (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      error = dwg_encode_DIMENSION_ANG2LN (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      error = dwg_encode_DIMENSION_RADIUS (dat, obj);
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      error = dwg_encode_DIMENSION_DIAMETER (dat, obj);
      break;
    case DWG_TYPE_POINT:
      error = dwg_encode_POINT (dat, obj);
      break;
    case DWG_TYPE__3DFACE:
      error = dwg_encode__3DFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      error = dwg_encode_POLYLINE_PFACE (dat, obj);
      break;
    case DWG_TYPE_POLYLINE_MESH:
      error = dwg_encode_POLYLINE_MESH (dat, obj);
      break;
    case DWG_TYPE_SOLID:
      error = dwg_encode_SOLID (dat, obj);
      break;
    case DWG_TYPE_TRACE:
      error = dwg_encode_TRACE (dat, obj);
      break;
    case DWG_TYPE_SHAPE:
      error = dwg_encode_SHAPE (dat, obj);
      break;
    case DWG_TYPE_VIEWPORT:
      error = dwg_encode_VIEWPORT (dat, obj);
      break;
    case DWG_TYPE_ELLIPSE:
      error = dwg_encode_ELLIPSE (dat, obj);
      break;
    case DWG_TYPE_SPLINE:
      error = dwg_encode_SPLINE (dat, obj);
      break;
    case DWG_TYPE_REGION:
      error = dwg_encode_REGION (dat, obj);
      break;
    case DWG_TYPE__3DSOLID:
      error = dwg_encode__3DSOLID (dat, obj);
      break;
    case DWG_TYPE_BODY:
      error = dwg_encode_BODY (dat, obj);
      break;
    case DWG_TYPE_RAY:
      error = dwg_encode_RAY (dat, obj);
      break;
    case DWG_TYPE_XLINE:
      error = dwg_encode_XLINE (dat, obj);
      break;
    case DWG_TYPE_DICTIONARY:
      error = dwg_encode_DICTIONARY (dat, obj);
      break;
    case DWG_TYPE_MTEXT:
      error = dwg_encode_MTEXT (dat, obj);
      break;
    case DWG_TYPE_LEADER:
      error = dwg_encode_LEADER (dat, obj);
      break;
    case DWG_TYPE_TOLERANCE:
      error = dwg_encode_TOLERANCE (dat, obj);
      break;
    case DWG_TYPE_MLINE:
      error = dwg_encode_MLINE (dat, obj);
      break;
    case DWG_TYPE_BLOCK_CONTROL:
      error = dwg_encode_BLOCK_CONTROL (dat, obj);
      break;
    case DWG_TYPE_BLOCK_HEADER:
      error = dwg_encode_BLOCK_HEADER (dat, obj);
      break;
    case DWG_TYPE_LAYER_CONTROL:
      error = dwg_encode_LAYER_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LAYER:
      error = dwg_encode_LAYER (dat, obj);
      break;
    case DWG_TYPE_STYLE_CONTROL:
      error = dwg_encode_STYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_STYLE:
      error = dwg_encode_STYLE (dat, obj);
      break;
    case DWG_TYPE_LTYPE_CONTROL:
      error = dwg_encode_LTYPE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_LTYPE:
      error = dwg_encode_LTYPE (dat, obj);
      break;
    case DWG_TYPE_VIEW_CONTROL:
      error = dwg_encode_VIEW_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VIEW:
      error = dwg_encode_VIEW (dat, obj);
      break;
    case DWG_TYPE_UCS_CONTROL:
      error = dwg_encode_UCS_CONTROL (dat, obj);
      break;
    case DWG_TYPE_UCS:
      error = dwg_encode_UCS (dat, obj);
      break;
    case DWG_TYPE_VPORT_CONTROL:
      error = dwg_encode_VPORT_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VPORT:
      error = dwg_encode_VPORT (dat, obj);
      break;
    case DWG_TYPE_APPID_CONTROL:
      error = dwg_encode_APPID_CONTROL (dat, obj);
      break;
    case DWG_TYPE_APPID:
      error = dwg_encode_APPID (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE_CONTROL:
      error = dwg_encode_DIMSTYLE_CONTROL (dat, obj);
      break;
    case DWG_TYPE_DIMSTYLE:
      error = dwg_encode_DIMSTYLE (dat, obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      error = dwg_encode_VPORT_ENTITY_CONTROL (dat, obj);
      break;
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      error = dwg_encode_VPORT_ENTITY_HEADER (dat, obj);
      break;
    case DWG_TYPE_GROUP:
      error = dwg_encode_GROUP (dat, obj);
      break;
    case DWG_TYPE_MLINESTYLE:
      error = dwg_encode_MLINESTYLE (dat, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      error = dwg_encode_OLE2FRAME (dat, obj);
      break;
    case DWG_TYPE_DUMMY:
      error = dwg_encode_DUMMY (dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      error = dwg_encode_LONG_TRANSACTION (dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      error = dwg_encode_LWPOLYLINE (dat, obj);
      break;
    case DWG_TYPE_HATCH:
      error = dwg_encode_HATCH (dat, obj);
      break;
    case DWG_TYPE_XRECORD:
      error = dwg_encode_XRECORD (dat, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      error = dwg_encode_PLACEHOLDER (dat, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      error = dwg_encode_OLEFRAME (dat, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own section");
      // dwg_encode_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      error |= dwg_encode_LAYOUT (dat, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      error = dwg_encode_PROXY_ENTITY (dat, obj);
      break;
    case DWG_TYPE_PROXY_OBJECT:
      error = dwg_encode_PROXY_OBJECT (dat, obj);
      break;
    default:
      if (obj->type == obj->parent->layout_number)
        error = dwg_encode_LAYOUT (dat, obj);
      else if ((error = dwg_encode_variable_type (obj->parent, dat, obj))
               & DWG_ERR_UNHANDLEDCLASS)
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          dat->byte = address; // restart and write into the UNKNOWN_OBJ object
          dat->bit = 0;
          bit_write_MS (dat, obj->size); // unknown blobs have a known size
          bit_write_BS (dat, obj->type); // type

          if (i <= (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity (klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              error = dwg_encode_UNKNOWN_OBJ (dat, obj);
            }
          else if (klass)
            {
              error = dwg_encode_UNKNOWN_ENT (dat, obj);
            }
          else // not a class
            {
              LOG_WARN ("Unknown object, skipping eed/reactors/xdic");
              error = DWG_ERR_UNHANDLEDCLASS;
              SINCE (R_2000)
              {
                bit_write_RL (dat, obj->bitsize);
                LOG_INFO ("Object bitsize: " FORMAT_RL " @%lu.%u\n",
                          obj->bitsize, dat->byte, dat->bit);
              }
              bit_write_H (dat, &(obj->handle));
              LOG_INFO ("Object handle: %x.%d.%lX\n", obj->handle.code,
                        obj->handle.size, obj->handle.value);
              // write obj->size bytes, excl. bitsize and handle.
              // overshoot the bitsize and handle size.
              bit_write_TF (dat, obj->tio.unknown, obj->size);
            }
        }
    }

  /*
   if (obj->supertype != DWG_SUPERTYPE_UNKNOWN)
   {
     fprintf (stderr, "Begin address:\t%10lu\n", address);
     fprintf (stderr, "Last address:\t%10lu\tSize: %10lu\n", dat->byte,
   obj->size); fprintf (stderr, "End address:\t%10lu (calculated)\n", address +
   2 + obj->size);
   }
   */

  /* Now 1 padding bits until next byte, and then a RS CRC */
  if (dat->bit)
    {
      for (int i = dat->bit; i < 8; i++)
        {
          bit_write_B (dat, 1);
        }
    }
  bit_write_CRC (dat, address, 0xC0C1);

  {
    unsigned long next_addr = address + obj->size + 4;
    if (next_addr != dat->byte)
      {
        if (obj->size)
          LOG_WARN ("Wrong object size: %lu + %u + 4 != %lu: %ld off", address,
                    obj->size, dat->byte, (long)(next_addr - dat->byte));
        dat->byte = next_addr;
      }
  }
  dat->bit = 0;
  return error;
}

/** Only writes the raw part.
    Only members with size have raw and a handle.
 */
static int
dwg_encode_eed (Bit_Chain *restrict dat, Dwg_Object_Object *restrict ent)
{
  int i, num_eed = ent->num_eed;
  for (i = 0; i < num_eed; i++)
    {
      BITCODE_BS size = ent->eed[i].size;
      int code = (int)ent->eed[i].data->code;
      LOG_TRACE ("EED[%u] size: %d, code: %d\n", i, (int)size, code);
      if (size)
        {
          bit_write_BS (dat, size);
          bit_write_H (dat, &(ent->eed[i].handle));
          LOG_TRACE ("EED[%u] handle: %x.%d.%lX\n", i, ent->eed[i].handle.code,
                     ent->eed[i].handle.size, ent->eed[i].handle.value);
          bit_write_TF (dat, ent->eed[i].raw, size);
        }
    }
  bit_write_BS (dat, 0);
  return 0;
}

/* The first common part of every entity.

   The last common part is common_entity_handle_data.spec
   called by COMMON_ENTITY_HANDLE_DATA in dwg.spec
   See DWG_SUPERTYPE_ENTITY in dwg_encode().
 */
static int
dwg_encode_entity (Dwg_Object *obj, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
                   Bit_Chain *dat)
{
  int error = 0;
  Dwg_Object_Entity *ent = obj->tio.entity;
  Dwg_Object_Entity *_obj = ent;
  Dwg_Data *dwg = ent->dwg;

  if (!obj || !dat)
    return DWG_ERR_INVALIDDWG;
  PRE (R_13)
  {

    if (FIELD_VALUE (flag_r11) & 4 && FIELD_VALUE (kind_r11) > 2
        && FIELD_VALUE (kind_r11) != 22)
      FIELD_RD (elevation_r11, 30);
    if (FIELD_VALUE (flag_r11) & 8)
      FIELD_RD (thickness_r11, 39);
    if (FIELD_VALUE (flag_r11) & 0x20)
      {
        Dwg_Object_Ref *hdl
            = dwg_decode_handleref_with_code (dat, obj, dwg, 0);
        if (hdl)
          obj->handle = hdl->handleref;
      }
    if (FIELD_VALUE (extra_r11) & 4)
      FIELD_RS (paper_r11, 0);
  }

  SINCE (R_2007) { *str_dat = *dat; }
  VERSIONS (R_2000, R_2007)
  {
    obj->bitsize_pos = bit_position (dat);
    if (!obj->bitsize)
      bit_write_RL (dat, obj->size * 8);
    else
      bit_write_RL (dat, obj->bitsize);
  }
  SINCE (R_2007)
  {
    // The handle stream offset, i.e. end of the object, right after
    // the has_strings bit.
    obj->hdlpos = obj->address * 8 + obj->bitsize;
    SINCE (R_2010)
    {
      obj->hdlpos += 8;
      LOG_HANDLE ("(bitsize: " FORMAT_RL ", ", obj->bitsize);
      LOG_HANDLE ("hdlpos: %lu)\n", obj->hdlpos);
    }
    // and set the string stream (restricted to size)
    error |= obj_string_stream (dat, obj, str_dat);
  }

  bit_write_H (dat, &(obj->handle));
  LOG_TRACE ("handle: %x.%d.%lX [5]\n", obj->handle.code, obj->handle.size,
             obj->handle.value)
  PRE (R_13) { return DWG_ERR_NOTYETSUPPORTED; }

  error |= dwg_encode_eed (dat, (Dwg_Object_Object *)ent);
  // if (error & (DWG_ERR_INVALIDTYPE|DWG_ERR_VALUEOUTOFBOUNDS))
  //  return error;

  #include "common_entity_data.spec"

  return error;
}

static int
dwg_encode_common_entity_handle_data (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                      Dwg_Object *obj)
{
  Dwg_Object_Entity *ent;
  // Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  BITCODE_BL vcount;
  int error = 0;
  ent = obj->tio.entity;
  _obj = ent;

#include "common_entity_handle_data.spec"

  return error;
}

void
dwg_encode_handleref (Bit_Chain *hdl_dat, Dwg_Object *obj, Dwg_Data *dwg,
                      Dwg_Object_Ref *ref)
{
  // this function should receive a Object_Ref without an abs_ref, calculate it
  // and return a Dwg_Handle this should be a higher level function not sure if
  // the prototype is correct
  assert (obj);
}

/**
 * code:
 *  TYPEDOBJHANDLE:
 *   2 Soft owner
 *   3 Hard owner
 *   4 Soft pointer
 *   5 Hard pointer
 *  OFFSETOBJHANDLE for soft owners or pointers:
 *   6 ref + 1
 *   8 ref - 1
 *   a ref + offset
 *   c ref - offset
 */
void
dwg_encode_handleref_with_code (Bit_Chain *hdl_dat, Dwg_Object *obj,
                                Dwg_Data *dwg, Dwg_Object_Ref *ref,
                                unsigned int code)
{
  // XXX fixme. create the handle, then check the code. allow relative handle
  // soft codes.
  dwg_encode_handleref (hdl_dat, obj, dwg, ref);
  if (ref->absolute_ref == 0 && ref->handleref.code != code)
    {
      /*
       * With TYPEDOBJHANDLE 2-5 the code indicates the type of ownership.
       * With OFFSETOBJHANDLE >5 the handle is stored as an offset from some
       * other handle.
       */
      switch (ref->handleref.code)
        {
        case 0x06:
          ref->absolute_ref = (obj->handle.value + 1);
          break;
        case 0x08:
          ref->absolute_ref = (obj->handle.value - 1);
          break;
        case 0x0A:
          ref->absolute_ref = (obj->handle.value + ref->handleref.value);
          break;
        case 0x0C:
          ref->absolute_ref = (obj->handle.value - ref->handleref.value);
          break;
        case 2:
        case 3:
        case 4:
        case 5:
          ref->absolute_ref = ref->handleref.value;
          break;
        case 0: // ignore (ANYCODE)
          ref->absolute_ref = ref->handleref.value;
          break;
        default:
          LOG_WARN ("Invalid handle pointer code %d", ref->handleref.code);
          break;
        }
    }
}

/* The first common part of every object.

   There is no COMMON_ENTITY_HANDLE_DATA for objects.
   See DWG_SUPERTYPE_OBJECT in dwg_encode().
*/
static int
dwg_encode_object (Dwg_Object *obj, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
                   Bit_Chain *dat)
{
  int error = 0;
  Dwg_Object_Object *ord = obj->tio.object;

  VERSIONS (R_2000, R_2007)
  {
    obj->bitsize_pos = bit_position (dat);
    if (!obj->bitsize)
      bit_write_RL (dat, obj->size * 8);
    else
      bit_write_RL (dat, obj->bitsize);
    LOG_INFO ("Object bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize,
              dat->byte, dat->bit);
  }
  SINCE (R_2010)
  {
    obj->bitsize = dat->size - 0;
    LOG_INFO ("Object bitsize: " FORMAT_RL " @%lu.%u\n", obj->bitsize,
              dat->byte, dat->bit);
  }
  obj->hdlpos = bit_position (dat) + obj->bitsize; // the handle stream offset
  SINCE (R_2007) { obj_string_stream (dat, obj, str_dat); }

  bit_write_H (dat, &(obj->handle));
  error |= dwg_encode_eed (dat, ord);

  VERSIONS (R_13, R_14)
  {
    obj->bitsize_pos = bit_position (dat);
    if (!obj->bitsize)
      bit_write_RL (dat, obj->size * 8);
    else
      bit_write_RL (dat, obj->bitsize);
  }

  bit_write_BL (dat, ord->num_reactors);

  SINCE (R_2004) { bit_write_B (dat, ord->xdic_missing_flag); }
  return error;
}

static int
dwg_encode_header_variables (Bit_Chain *dat, Bit_Chain *hdl_dat,
                             Bit_Chain *str_dat, Dwg_Data *dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;

#include "header_variables.spec"

  return 0;
}

static int
dwg_encode_xdata (Bit_Chain *dat, Dwg_Object_XRECORD *obj, int size)
{
  Dwg_Resbuf *tmp, *rbuf = obj->xdata;
  short type;
  int i;

  while (rbuf)
    {
      tmp = rbuf->next;
      type = get_base_value_type (rbuf->type);
      switch (type)
        {
        case VT_STRING:
          UNTIL (R_2007)
          {
            bit_write_RS (dat, rbuf->value.str.size);
            bit_write_RC (dat, rbuf->value.str.codepage);
            bit_write_TF (dat, rbuf->value.str.u.data, rbuf->value.str.size);
          }
          LATER_VERSIONS
          {
            bit_write_RS (dat, rbuf->value.str.size);
            for (i = 0; i < rbuf->value.str.size; i++)
              bit_write_RS (dat, rbuf->value.str.u.wdata[i]);
          }
          break;
        case VT_REAL:
          bit_write_RD (dat, rbuf->value.dbl);
          break;
        case VT_BOOL:
        case VT_INT8:
          bit_write_RC (dat, rbuf->value.i8);
          break;
        case VT_INT16:
          bit_write_RS (dat, rbuf->value.i16);
          break;
        case VT_INT32:
          bit_write_RL (dat, rbuf->value.i32);
          break;
        case VT_POINT3D:
          bit_write_RD (dat, rbuf->value.pt[0]);
          bit_write_RD (dat, rbuf->value.pt[1]);
          bit_write_RD (dat, rbuf->value.pt[2]);
          break;
        case VT_BINARY:
          bit_write_RC (dat, rbuf->value.str.size);
          bit_write_TF (dat, rbuf->value.str.u.data, rbuf->value.str.size);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          for (i = 0; i < 8; i++)
            bit_write_RC (dat, rbuf->value.hdl[i]);
          break;
        case VT_INVALID:
        default:
          LOG_ERROR ("Invalid group code in xdata: %d", rbuf->type)
          return DWG_ERR_INVALIDEED;
        }
      rbuf = tmp;
    }
  return 0;
}

#undef IS_ENCODER

/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2010,2018-2020 Free Software Foundation, Inc.         */
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
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#ifdef HAVE_CTYPE_H
#  include <ctype.h>
#endif

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"
#include "decode.h"

// from dwg_api
bool is_dwg_object (const char *name);
bool is_dwg_entity (const char *name);

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
#define FIELD_BSd(nam, dxf) FIELD_CAST (nam, BS, BSd, dxf)
#define FIELD_RSx(nam, dxf) FIELD_CAST (nam, RS, RSx, dxf)
#define FIELD_RLx(nam, dxf) FIELD_CAST (nam, RL, RLx, dxf)
#define FIELD_BLx(nam, dxf) FIELD_CAST (nam, BL, BLx, dxf)
#define FIELD_BLd(nam, dxf) FIELD_CAST (nam, BL, BLd, dxf)
#define FIELD_RLd(nam, dxf) FIELD_CAST (nam, RL, RLd, dxf)
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
    IF_ENCODE_FROM_EARLIER                                                    \
    {                                                                         \
      if (!_obj->nam)                                                         \
        _obj->nam = strdup ("");                                              \
    }                                                                         \
    FIELDG (nam, TV, dxf);                                                    \
  }
// may need to convert from/to TV<=>TU
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (dat->version < R_2007)                                                \
      {                                                                       \
        bit_write_T (dat, _obj->nam);                                         \
        LOG_TRACE (#nam ": \"%s\" [T %d]\n", _obj->nam, dxf);                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        bit_write_T (str_dat, _obj->nam);                                     \
        LOG_TRACE_TU (#nam, _obj->nam, dxf);                                  \
      }                                                                       \
  }
#define FIELD_TF(nam, len, dxf)                                               \
  {                                                                           \
    if (len > 0)                                                              \
      {                                                                       \
        if (!_obj->nam)                                                       \
          { /* empty field, write zeros */                                    \
            for (int _i = 0; _i < (int)(len); _i++)                           \
              bit_write_RC (dat, 0);                                          \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            bit_write_TF (dat, (BITCODE_TF)_obj->nam, len);                   \
          }                                                                   \
      }                                                                       \
    LOG_TRACE_TF (FIELD_VALUE (nam), (int)len);                               \
  }
#define FIELD_TFF(nam, len, dxf) FIELD_TF (nam, len, dxf)
#define FIELD_TU(nam, dxf)                                                    \
  {                                                                           \
    if (_obj->nam)                                                            \
      bit_write_TU (str_dat, (BITCODE_TU)_obj->nam);                          \
    LOG_TRACE_TU (#nam, (BITCODE_TU)_obj->nam, dxf);                          \
  }
#define FIELD_BT(nam, dxf) FIELDG (nam, BT, dxf);

#define _FIELD_DD(nam, _default, dxf)                                         \
  bit_write_DD (dat, FIELD_VALUE (nam), _default);
#define FIELD_DD(nam, _default, dxf)                                          \
  _FIELD_DD (nam, _default, dxf);                                             \
  LOG_TRACE (#nam ": %f [DD %d]\n", _obj->nam, dxf)
#define FIELD_2DD(nam, d1, d2, dxf)                                           \
  {                                                                           \
    _FIELD_DD (nam.x, d1, dxf);                                               \
    _FIELD_DD (nam.y, d2, dxf + 10);                                          \
    LOG_TRACE (#nam ": (%f, %f) [2DD %d]\n", _obj->nam.x, _obj->nam.y, dxf)   \
  }
#define FIELD_3DD(nam, def, dxf)                                              \
  {                                                                           \
    _FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                              \
    _FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                         \
    _FIELD_DD (nam.z, FIELD_VALUE (def.z), dxf + 20);                         \
    LOG_TRACE (#nam ": (%f, %f, %f) [3DD %d]\n", _obj->nam.x, _obj->nam.y,    \
               _obj->nam.z, dxf)                                              \
  }
#define FIELD_2RD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_RD (dat, _obj->nam.x);                                          \
    bit_write_RD (dat, _obj->nam.y);                                          \
    LOG_TRACE (#nam ": (%f, %f) [3RD %d]\n", _obj->nam.x, _obj->nam.y, dxf)   \
  }
#define FIELD_2BD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_BD (dat, _obj->nam.x);                                          \
    bit_write_BD (dat, _obj->nam.y);                                          \
    LOG_TRACE (#nam ": (%f, %f) [3BD %d]\n", _obj->nam.x, _obj->nam.y, dxf)   \
  }
#define FIELD_2BD_1(nam, dxf) FIELD_2BD (nam, dxf)
#define FIELD_3RD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_RD (dat, _obj->nam.x);                                          \
    bit_write_RD (dat, _obj->nam.y);                                          \
    bit_write_RD (dat, _obj->nam.z);                                          \
    LOG_TRACE (#nam ": (%f, %f, %f) [3RD %d]\n", _obj->nam.x, _obj->nam.y,    \
               _obj->nam.z, dxf)                                              \
  }
#define FIELD_3BD(nam, dxf)                                                   \
  {                                                                           \
    bit_write_BD (dat, _obj->nam.x);                                          \
    bit_write_BD (dat, _obj->nam.y);                                          \
    bit_write_BD (dat, _obj->nam.z);                                          \
    LOG_TRACE (#nam ": (%f, %f, %f) [3BD %d]\n", _obj->nam.x, _obj->nam.y,    \
               _obj->nam.z, dxf)                                              \
  }
#define FIELD_3BD_1(nam, dxf) FIELD_3BD (nam, dxf)
#define FIELD_3DPOINT(nam, dxf) FIELD_3BD (nam, dxf)
#define FIELD_4BITS(nam, dxf)                                                 \
  {                                                                           \
    unsigned char _b = (unsigned char)_obj->nam;                              \
    bit_write_4BITS (dat, _b);                                                \
    LOG_TRACE (#nam ": b%d%d%d%d [4BITS %d]\n", _b & 8, _b & 4, _b & 2,       \
               _b & 1, dxf);                                                  \
  }
#define FIELD_TIMEBLL(nam, dxf)                                               \
  {                                                                           \
    bit_write_TIMEBLL (dat, (BITCODE_TIMEBLL)_obj->nam);                      \
    LOG_TRACE (#nam ": " FORMAT_BL "." FORMAT_BL "\n", _obj->nam.days,        \
               _obj->nam.ms);                                                 \
  }

#define FIELD_CMC(color, dxf1, dxf2)                                          \
  {                                                                           \
    bit_write_CMC (dat, &_obj->color);                                        \
    LOG_TRACE (#color ".index: %d [CMC.BS %d]\n", _obj->color.index, dxf1);   \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        LOG_TRACE (#color ".rgb: 0x%06x [CMC.BL %d]\n",                       \
                   (unsigned)_obj->color.rgb, dxf2);                          \
        LOG_TRACE (#color ".flag: 0x%x [CMC.RC]\n",                           \
                   (unsigned)_obj->color.flag);                               \
        if (_obj->color.flag & 1)                                             \
          LOG_TRACE (#color ".name: %s [CMC.TV]\n", _obj->color.name);        \
        if (_obj->color.flag & 2)                                             \
          LOG_TRACE (#color ".bookname: %s [CMC.TV]\n",                       \
                     _obj->color.book_name);                                  \
      }                                                                       \
  }

#define SUB_FIELD_CMC(o, nam, dxf1, dxf2) bit_write_CMC (dat, &_obj->o.nam)

#define LOG_TF(level, var, len)                                               \
  if (var)                                                                    \
    {                                                                         \
      int _i;                                                                 \
      for (_i = 0; _i < (len); _i++)                                          \
        {                                                                     \
          LOG (level, "%02x", (unsigned char)((char *)var)[_i]);              \
        }                                                                     \
      LOG (level, "\n");                                                      \
      if (DWG_LOGLEVEL >= DWG_LOGLEVEL_INSANE)                                \
        {                                                                     \
          for (_i = 0; _i < (len); _i++)                                      \
            {                                                                 \
              unsigned char c = ((unsigned char *)var)[_i];                   \
              LOG_INSANE ("%-2c", isprint (c) ? c : ' ');                     \
            }                                                                 \
          LOG_INSANE ("\n");                                                  \
        }                                                                     \
    }
#define LOG_TRACE_TF(var, len) LOG_TF (TRACE, var, len)
#define LOG_INSANE_TF(var, len) LOG_TF (INSANE, var, len)

#define FIELD_BE(nam, dxf)                                                    \
  bit_write_BE (dat, FIELD_VALUE (nam.x), FIELD_VALUE (nam.y),                \
                FIELD_VALUE (nam.z));

#define OVERFLOW_CHECK(nam, size)                                             \
  if ((long)(size) > 0xff00L || !_obj->nam)                                   \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " %ld", (long)size);                         \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define OVERFLOW_CHECK_LV(nam, size)                                          \
  if ((long)(size) > 0xff00L)                                                 \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " %ld, set to 0", (long)size);               \
      size = 0;                                                               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }
#define OVERFLOW_NULL_CHECK_LV(nam, size)                                     \
  if ((long)(size) > 0xff00L || !_obj->nam)                                   \
    {                                                                         \
      LOG_ERROR ("Invalid " #nam " %ld, set to 0", (long)size);               \
      size = 0;                                                               \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  OVERFLOW_NULL_CHECK_LV (nam, _obj->size)                                    \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2RD (nam[vcount], dxf);                                           \
    }

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  OVERFLOW_NULL_CHECK_LV (nam, _obj->size)                                    \
  if (_obj->size)                                                             \
    FIELD_2RD (nam[0], dxf);                                                  \
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2DD (nam[vcount], FIELD_VALUE (nam[vcount - 1].x),                \
                 FIELD_VALUE (nam[vcount - 1].y), dxf);                       \
    }

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  OVERFLOW_NULL_CHECK_LV (nam, _obj->size)                                    \
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
        null_handle.code = handle_code;                                       \
        bit_write_H (hdl_dat, &null_handle);                                  \
        LOG_TRACE (#nam ": (%d.0.0) abs:0 [H %d]\n", handle_code, dxf)        \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (handle_code != ANYCODE && hdlptr->handleref.code != handle_code   \
            && (handle_code == 4 && hdlptr->handleref.code < 6))              \
          {                                                                   \
            LOG_WARN ("Expected a CODE %d handle, got a %d", handle_code,     \
                      hdlptr->handleref.code);                                \
          }                                                                   \
        bit_write_H (hdl_dat, &hdlptr->handleref);                            \
        LOG_TRACE (#nam ": " FORMAT_REF " [H %d]\n", ARGS_REF (hdlptr), dxf)  \
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
      {                                                                       \
        bit_write_H (hdl_dat, NULL);                                          \
        LOG_TRACE (#nam "[%d]: NULL %d [H* %d]\n", (int)vcount, handle_code,  \
                   dxf)                                                       \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (handle_code != ANYCODE                                            \
            && _obj->nam->handleref.code != handle_code                       \
            && (handle_code == 4 && _obj->nam->handleref.code < 6))           \
          {                                                                   \
            LOG_WARN ("Expected a CODE %x handle, got a %x", handle_code,     \
                      _obj->nam->handleref.code);                             \
          }                                                                   \
        bit_write_H (hdl_dat, &_obj->nam->handleref);                         \
        LOG_TRACE (#nam "[%d]: " FORMAT_REF " [H* %d]\n", (int)vcount,        \
                   ARGS_REF (_obj->nam), dxf)                                 \
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
  bit_write_RC (dat, 0);                                                      \
  LOG_TRACE ("num_inserts: %d [RC* 0]\n", FIELD_VALUE (num_inserts))

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

#define FIELD_XDATA(nam, size)                                                \
  error |= dwg_encode_xdata (dat, _obj, _obj->size)

#define COMMON_ENTITY_HANDLE_DATA                                             \
  SINCE (R_13)                                                                \
  {                                                                           \
    START_HANDLE_STREAM;                                                      \
    PRE (R_2007)                                                              \
    {                                                                         \
      error |= dwg_encode_common_entity_handle_data (dat, hdl_dat, obj);      \
    }                                                                         \
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
  if (dat->version >= R_2007 && obj->bitsize)                                 \
    bit_set_position (hdl_dat, obj->hdlpos);                                  \
  if (!obj->bitsize)                                                          \
    {                                                                         \
      LOG_TRACE ("-bitsize calc from HANDLE_STREAM @%lu.%u (%lu)\n",          \
                 dat->byte, dat->bit, obj->address);                          \
      obj->bitsize = bit_position (dat) - (obj->address * 8);                 \
    }                                                                         \
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
  LOG_INFO ("Add entity " #token " ")           \
  dat.chain = calloc (dat.size, 1);             \
  dat.version = dwg->header.version;            \
  dat.from_version = dwg->header.from_version;  \
  bit_write_MS (&dat, dat.size);                \
  if (dat.version >= R_2010) {                  \
    /* FIXME: should be UMC handlestream_size */\
    bit_write_UMC (&dat, 8*sizeof(Dwg_Entity_##token)); \
    bit_write_BOT &dat, DWG_TYPE_##token);      \
  } else {                                      \
    bit_write_BS (&dat, DWG_TYPE_##token);      \
  }                                             \
  bit_set_position (&dat, 0);                   \
  error = dwg_decode_add_object (dwg, &dat, &dat, 0);\
  if (-1 == error)                              \
    dwg_resolve_objectrefs_silent (dwg);        \
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
  LOG_INFO ("Add object " #token " ")            \
  dat.chain = calloc (dat.size, 1);              \
  dat.version = dwg->header.version;             \
  dat.from_version = dwg->header.from_version;   \
  bit_write_MS (&dat, dat.size);                 \
  if (dat.version >= R_2010) {                   \
    /* FIXME: should be UMC handlestream_size */ \
    bit_write_UMC (&dat, 8*sizeof(Dwg_Object_##token)); \
    bit_write_BOT (&dat, DWG_TYPE_##token);      \
  } else {                                       \
    bit_write_BS (&dat, DWG_TYPE_##token);       \
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
    LOG_INFO ("Encode entity " #token "\n")                                   \
    error = dwg_encode_entity (obj, dat, hdl_dat, str_dat);                   \
    if (error)                                                                \
      return error;

#define DWG_ENTITY_END                                                        \
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
    LOG_INFO ("Encode object " #token "\n")

#define DWG_OBJECT_END                                                        \
  return error;                                                               \
  }

#define ENT_REACTORS(code)                                                    \
  if (dat->version >= R_2000 && _obj->num_reactors > 0x1000)                  \
    {                                                                         \
      LOG_ERROR ("Invalid num_reactors: %ld\n", (long)_obj->num_reactors);    \
      return DWG_ERR_VALUEOUTOFBOUNDS;                                        \
    }                                                                         \
  SINCE (R_13)                                                                \
  {                                                                           \
    if (_obj->num_reactors && !_obj->reactors)                                \
      {                                                                       \
        LOG_ERROR ("NULL entity.reactors");                                   \
        return DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      }                                                                       \
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
  unsigned long handle;
  long address;
  BITCODE_BL index;
} Object_Map;

/*--------------------------------------------------------------------------------
 * Private functions prototypes
 */
static int encode_preR13 (Dwg_Data *restrict dwg, Bit_Chain *restrict dat);

static int dwg_encode_entity (Dwg_Object *restrict obj, Bit_Chain *dat,
                              Bit_Chain *hdl_dat, Bit_Chain *str_dat);
static int dwg_encode_object (Dwg_Object *restrict obj, Bit_Chain *str_dat,
                              Bit_Chain *hdl_dat, Bit_Chain *dat);
static int dwg_encode_common_entity_handle_data (Bit_Chain *dat,
                                                 Bit_Chain *hdl_dat,
                                                 Dwg_Object *restrict obj);
static int dwg_encode_header_variables (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                        Bit_Chain *str_dat,
                                        Dwg_Data *restrict dwg);
static int dwg_encode_variable_type (Dwg_Data *restrict dwg,
                                     Bit_Chain *restrict dat,
                                     Dwg_Object *restrict obj);
void dwg_encode_handleref (Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                           Dwg_Data *restrict dwg,
                           Dwg_Object_Ref *restrict ref);
void dwg_encode_handleref_with_code (Bit_Chain *hdl_dat,
                                     Dwg_Object *restrict obj,
                                     Dwg_Data *restrict dwg,
                                     Dwg_Object_Ref *restrict ref,
                                     unsigned int code);
int dwg_encode_add_object (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                           unsigned long address);

static int dwg_encode_xdata (Bit_Chain *restrict dat,
                             Dwg_Object_XRECORD *restrict obj, int size);

/*--------------------------------------------------------------------------------
 * Public functions
 */

static BITCODE_RL
encode_patch_RLsize (Bit_Chain *dat, long unsigned int pvzadr)
{
  unsigned long pos;
  BITCODE_RL size;
  if (dat->bit) // padding
    {
      dat->bit = 0;
      dat->byte++;
    }
  size = dat->byte - pvzadr - 4; // minus the RL size
  pos = bit_position (dat);
  assert (pvzadr);
  bit_set_position (dat, pvzadr * 8);
  bit_write_RL (dat, size);
  LOG_TRACE ("size: " FORMAT_RL " [RL] @%lu\n", size, pvzadr);
  bit_set_position (dat, pos);
  return size;
}

/**
 * dwg_encode(): the current generic encoder entry point.
 *
 * TODO: preR13 tables, 2007 maps.
 * 2010+ uses the 2004 format.
 * Returns a summary bitmask of all errors.
 */
AFL_GCC_TOOBIG
int
dwg_encode (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  int ckr_missing = 1;
  int error = 0;
  BITCODE_BL i, j;
  long unsigned int section_address;
  unsigned char pvzbit;
  long unsigned int pvzadr;
  long unsigned int pvzadr_2;
  unsigned int ckr;
  unsigned int sec_size = 0;
  long unsigned int last_offset;
  BITCODE_BL last_handle;
  Object_Map *omap;
  Bit_Chain *hdl_dat;
  const char *section_names[]
      = { "AcDb:Header", "AcDb:Classes", "AcDb:Handles",
          "2NDHEADER",   "MEASUREMENT",  "AcDb:AuxHeader" };

  if (dwg->opts)
    loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
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
  if (dwg->header.version != dwg->header.from_version)
    LOG_TRACE ("Encode version %s from version %s\n",
               version_codes[dwg->header.version],
               version_codes[dwg->header.from_version])
  else
    LOG_TRACE ("Encode version %s\n", version_codes[dwg->header.version])
  dat->byte += 6;

  {
    struct Dwg_Header *_obj = &dwg->header;
    Dwg_Object *obj = NULL;
    if (!_obj->dwg_version)
      {
        _obj->is_maint = 0;
        switch (dwg->header.version)
          {
          case R_9:
            _obj->dwg_version = 0x11; // ?
            break;
          case R_10:
            _obj->dwg_version = 0x12; // ?
            break;
          case R_11:
            _obj->dwg_version = 0x13; // ?
            break;
          case R_13:
            _obj->dwg_version = 0x15;
            break;
          case R_14:
            _obj->dwg_version = 0x16;
            break;
          case R_2000:
            _obj->dwg_version = 0x17;
            _obj->is_maint = 0xf;
            break;
          case R_2004:
            _obj->dwg_version = 0x19;
            _obj->is_maint = 0x68;
            break;
          case R_2007:
            _obj->dwg_version = 0x1b;
            _obj->is_maint = 0x32;
            break;
          case R_2010:
            _obj->dwg_version = 0x1d;
            _obj->is_maint = 0x6d;
            break;
          case R_2013:
            _obj->dwg_version = 0x1f;
            _obj->is_maint = 0x7d;
            break;
          case R_2018:
            _obj->dwg_version = 0x21;
            _obj->is_maint = 0x4;
            break;
          case R_INVALID:
          case R_AFTER:
          case R_1_1:
          case R_1_2:
          case R_1_4:
          case R_2_0:
          case R_2_1:
          case R_2_5:
          case R_2_6:
          default:
            break;
          }
        if (!_obj->app_dwg_version)
          _obj->app_dwg_version = _obj->dwg_version;
      }
    if (!_obj->codepage)
      _obj->codepage = 30;

      // clang-format off
    #include "header.spec"
    // clang-format on
  }
  section_address = dat->byte;

#define WE_CAN                                                                \
  "This version of LibreDWG is only capable of encoding "                     \
  "version R13-R2000 (code: AC1012-AC1015) DWG files.\n"

  PRE (R_13)
  {
    // TODO: tables, entities, block entities
    LOG_ERROR (WE_CAN "We don't encode preR13 tables, entities, blocks yet")
#ifndef IS_RELEASE
    return encode_preR13 (dwg, dat);
#endif
  }

  PRE (R_2004)
  {
    /* section 0: header vars
     *         1: class section
     *         2: object map
     *         3: (R13 c3 and later): 2nd header (special table no sentinels)
     *         4: optional: MEASUREMENT
     *         5: optional: AuxHeader
     */
    /* Usually 3-5, max 6 */
    if (!dwg->header.num_sections
        || (dat->from_version >= R_2004 && dwg->header.num_sections > 6))
      {
        dwg->header.num_sections = dwg->header.version < R_2000 ? 5 : 6;
        // minimal DXF:
        if (!dwg->header_vars.HANDSEED || !dwg->header_vars.TDCREATE.days)
          {
            dwg->header.num_sections = 5;
            // to trigger IF_ENCODE_FROM_EARLIER defaults
            dat->from_version = R_11;
            if (dat->version <= dat->from_version)
              dat->from_version = dat->version - 1;
          }
      }
    LOG_TRACE ("num_sections: " FORMAT_RL " [RL]\n", dwg->header.num_sections);
    bit_write_RL (dat, dwg->header.num_sections);
    if (!dwg->header.section)
      dwg->header.section
          = calloc (dwg->header.num_sections, sizeof (Dwg_Section));
    section_address = dat->byte;                 // save section address
    dat->byte += (dwg->header.num_sections * 9); /* RC + 2*RL */
    bit_write_CRC (dat, 0, 0xC0C1);
    bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_HEADER_END));

    /*------------------------------------------------------------
     * AuxHeader section 5
     * R2000+, mostly redundant file header information
     */
    if (dwg->header.num_sections > 5)
      {
        struct Dwg_AuxHeader *_obj = &dwg->auxheader;
        Dwg_Object *obj = NULL;
        assert (!dat->bit);
        LOG_INFO ("\n=======> AuxHeader: %8u\n",
                  (unsigned)dat->byte); // size: 123

        dwg->header.section[SECTION_AUXHEADER_R2000].number = 5;
        dwg->header.section[SECTION_AUXHEADER_R2000].address = dat->byte;

        if (!_obj->dwg_version)
          {
            BITCODE_RS tmpunknown[] = { 4, 0x565, 0, 0, 2, 1 };
            LOG_TRACE ("Use AuxHeader defaults...\n");
            FIELD_VALUE (aux_intro[0]) = 0xff;
            FIELD_VALUE (aux_intro[1]) = 0x77;
            FIELD_VALUE (aux_intro[2]) = 0x01;
            FIELD_VALUE (minus_1) = -1;
            FIELD_VALUE (dwg_version) = dwg->header.dwg_version;
            FIELD_VALUE (maint_version) = dwg->header.maint_version;
            FIELD_VALUE (dwg_version_1) = dwg->header.dwg_version;
            FIELD_VALUE (dwg_version_2) = dwg->header.dwg_version;
            FIELD_VALUE (maint_version_1) = dwg->header.maint_version;
            FIELD_VALUE (maint_version_2) = dwg->header.maint_version;
            memcpy (FIELD_VALUE (unknown_rs), tmpunknown, sizeof (tmpunknown));
            FIELD_VALUE (TDCREATE) = dwg->header_vars.TDCREATE.value;
            FIELD_VALUE (TDUPDATE) = dwg->header_vars.TDUPDATE.value;
            if (dwg->header_vars.HANDSEED)
              FIELD_VALUE (HANDSEED) = dwg->header_vars.HANDSEED->absolute_ref;
          }

          // clang-format off
        #include "auxheader.spec"
        // clang-format on

        assert (!dat->bit);
        dwg->header.section[SECTION_AUXHEADER_R2000].size
            = dat->byte - dwg->header.section[SECTION_AUXHEADER_R2000].address;
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
    for (i = 0; i < (BITCODE_BL)size; i++)
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

    // clang-format off
    #include "r2004_file_header.spec"
    // clang-format on

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
   * THUMBNAIL preview pictures
   */
  if (!dwg->header.thumbnail_address)
    dwg->header.thumbnail_address = dat->byte;
  dat->bit = 0;
  LOG_TRACE ("\n=======> Thumbnail:       %4u\n", (unsigned)dat->byte);
  // dwg->thumbnail.size = 0; // to disable
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_THUMBNAIL_BEGIN));
  if (dwg->thumbnail.size == 0)
    {
      bit_write_RL (dat, 5); // overall size
      LOG_TRACE ("Thumbnail size: 5 [RL]\n");
      bit_write_RC (dat, 0); // num_pictures
      LOG_TRACE ("Thumbnail num_pictures: 0 [RC]\n");
    }
  else
    {
      bit_write_TF (dat, (char *)dwg->thumbnail.chain, dwg->thumbnail.size);
    }
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_THUMBNAIL_END));

  {
    BITCODE_RL size;
    dwg_bmp (dwg, &size);
    if (size > dwg->thumbnail.size)
      LOG_ERROR ("BMP size overflow: %i > %lu\n", size, dwg->thumbnail.size);
  }
  LOG_TRACE ("         Thumbnail (end): %4u\n", (unsigned)dat->byte);

  /*------------------------------------------------------------
   * Header Variables
   */
  assert (!dat->bit);
  LOG_INFO ("\n=======> Header Variables:   %4u\n", (unsigned)dat->byte);
  dwg->header.section[0].number = 0;
  dwg->header.section[0].address = dat->byte;
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_BEGIN));

  pvzadr = dat->byte;      // Size position
  bit_write_RL (dat, 540); // Size placeholder
  // if (dat->version >= R_2007)
  //  str_dat = dat;
  dwg_encode_header_variables (dat, hdl_dat, dat, dwg);
  encode_patch_RLsize (dat, pvzadr);
  bit_write_CRC (dat, pvzadr, 0xC0C1);

  // XXX trying to fix CRC 2-byte overflow. Must find actual reason
  // dat->byte -= 2;
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_END));
  assert ((long)dat->byte > (long)dwg->header.section[0].address);
  dwg->header.section[0].size
      = (BITCODE_RL) ((long)dat->byte - (long)dwg->header.section[0].address);
  LOG_TRACE ("         Header Variables (end): %4u\n", (unsigned)dat->byte);

  /*------------------------------------------------------------
   * Classes
   */
  LOG_INFO ("\n=======> Classes: %4u (%d)\n", (unsigned)dat->byte,
            dwg->num_classes);
  if (dwg->num_classes > 5000)
    {
      LOG_ERROR ("Invalid dwg->num_classes %d", dwg->num_classes)
      dwg->num_classes = 0;
    }
  dwg->header.section[SECTION_CLASSES_R13].number = 1;
  dwg->header.section[SECTION_CLASSES_R13].address = dat->byte;
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_BEGIN));
  pvzadr = dat->byte;    // Size position
  bit_write_RL (dat, 0); // Size placeholder

  for (j = 0; j < dwg->num_classes; j++)
    {
      Dwg_Class *klass;
      klass = &dwg->dwg_class[j];
      bit_write_BS (dat, klass->number);
      bit_write_BS (dat, klass->proxyflag);
      bit_write_TV (dat, klass->appname);
      bit_write_TV (dat, klass->cppname);
      bit_write_TV (dat, klass->dxfname);
      bit_write_B (dat, klass->is_zombie);
      bit_write_BS (dat, klass->item_class_id);
      LOG_TRACE ("Class %d 0x%x %s\n"
                 " %s \"%s\" %d 0x%x\n",
                 klass->number, klass->proxyflag, klass->dxfname,
                 klass->cppname, klass->appname, klass->is_zombie,
                 klass->item_class_id)

      SINCE (R_2007)
      {
        bit_write_BL (dat, klass->num_instances);
        bit_write_BL (dat, klass->dwg_version);
        bit_write_BL (dat, klass->maint_version);
        bit_write_BL (dat, klass->unknown_1);
        bit_write_BL (dat, klass->unknown_2);
        LOG_TRACE (" %d %d\n", (int)klass->num_instances,
                   (int)klass->dwg_version);
      }
    }

  /* Write the size of the section at its beginning
   */
  assert (pvzadr);
  encode_patch_RLsize (dat, pvzadr);
  bit_write_CRC (dat, pvzadr, 0xC0C1);
  bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_END));
  dwg->header.section[SECTION_CLASSES_R13].size
      = dat->byte - dwg->header.section[SECTION_CLASSES_R13].address;
  LOG_TRACE ("       Classes (end): %4u\n", (unsigned)dat->byte);

  bit_write_RL (dat, 0x0DCA); // 0xDCA Unknown bitlong inter class and objects
  LOG_TRACE ("unknown: %04X [RL]\n", 0x0DCA);

  /*------------------------------------------------------------
   * Objects
   */

  LOG_INFO ("\n=======> Objects: %4u\n", (unsigned)dat->byte);
  pvzadr = dat->byte;

  /* Sort object-map by ascending handles
   */
  LOG_TRACE ("num_objects: %i\n", dwg->num_objects);
  LOG_TRACE ("num_object_refs: %i\n", dwg->num_object_refs);
  omap = (Object_Map *)calloc (dwg->num_objects, sizeof (Object_Map));
  if (!omap)
    {
      LOG_ERROR ("Out of memory");
      return DWG_ERR_OUTOFMEM;
    }
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
    {
      LOG_HANDLE ("\nSorting objects...\n");
      for (i = 0; i < dwg->num_objects; i++)
        fprintf (OUTPUT, "Object(%3i): %4lX / idx: %u\n", i,
                 dwg->object[i].handle.value, dwg->object[i].index);
    }
  // init unsorted
  for (i = 0; i < dwg->num_objects; i++)
    {
      omap[i].index = i; // i.e. dwg->object[j].index
      omap[i].handle = dwg->object[i].handle.value;
    }
  // insertion sort
  for (i = 0; i < dwg->num_objects; i++)
    {
      Object_Map tmap;
      j = i;
      tmap = omap[i];
      while (j > 0 && omap[j - 1].handle > tmap.handle)
        {
          omap[j] = omap[j - 1];
          j--;
        }
      omap[j] = tmap;
    }
  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
    {
      LOG_HANDLE ("\nSorted handles:\n");
      for (i = 0; i < dwg->num_objects; i++)
        fprintf (OUTPUT, "Handle(%3i): %4lX / idx: %u\n", i, omap[i].handle,
                 omap[i].index);
    }

  /* Write the sorted objects
   */
  for (i = 0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj;
      BITCODE_BL index = omap[i].index;
      unsigned long hdloff = omap[i].handle - (i ? omap[i - 1].handle : 0);
      int off = dat->byte - (i ? omap[i - 1].address : 0);
      unsigned long address, end_address;
      LOG_TRACE ("\n> Next object: " FORMAT_BL
                 " Handleoff: %lX [UMC] Offset: %d [MC] @%lu\n"
                 "==========================================\n",
                 i, hdloff, off, dat->byte);
      omap[i].address = dat->byte;
      if (index > dwg->num_objects)
        {
          LOG_ERROR ("Invalid object map index " FORMAT_BL ", max " FORMAT_BL
                     ". Skipping",
                     index, dwg->num_objects)
          error |= DWG_ERR_VALUEOUTOFBOUNDS;
          continue;
        }
      obj = &dwg->object[index];
      // change the address to the linearly sorted one
      assert (dat->byte);
      if (!obj->parent)
        obj->parent = dwg;
      error |= dwg_encode_add_object (obj, dat, dat->byte);

#ifndef NDEBUG
      // check if this object overwrote at address 0
      if (dwg->header.version >= R_1_2)
        {
          assert (dat->chain[0] == 'A');
          assert (dat->chain[1] == 'C');
        }
#endif
      end_address = omap[i].address + (unsigned long)obj->size; // from RL
      if (end_address > dat->size)
        {
          dat->size = end_address;
          bit_chain_alloc (dat);
        }
    }

  if (DWG_LOGLEVEL >= DWG_LOGLEVEL_HANDLE)
    {
      LOG_HANDLE ("\nSorted objects:\n");
      for (i = 0; i < dwg->num_objects; i++)
        LOG_HANDLE ("Object(%d): %lX / Address: %ld / Idx: %d\n", i,
                    omap[i].handle, omap[i].address, omap[i].index);
    }

  /* Unknown CRC between objects and object map
   */
  bit_write_RS (dat, 0);
  LOG_TRACE ("unknown crc?: %04X [RS]\n", 0);

  /*------------------------------------------------------------
   * Object-map
   * split into chunks of max. 2030
   */
  LOG_INFO ("\n=======> Object Map: %4u\n", (unsigned)dat->byte);
  dwg->header.section[SECTION_HANDLES_R13].number = 2;
  dwg->header.section[SECTION_HANDLES_R13].address = dat->byte;

  sec_size = 0;
  pvzadr = dat->byte; // Correct value of section size must be written later
  dat->byte += 2;
  last_offset = 0;
  last_handle = 0;
  for (i = 0; i < dwg->num_objects; i++)
    {
      BITCODE_BL index;
      BITCODE_UMC handleoff;
      BITCODE_MC offset;

      index = omap[i].index;
      handleoff = omap[i].handle - last_handle;
      bit_write_UMC (dat, handleoff);
      LOG_HANDLE ("Handleoff(%3i): %4lX [UMC] (%4lX), ", index, handleoff,
                  omap[i].handle)
      last_handle = omap[i].handle;

      offset = omap[i].address - last_offset;
      bit_write_MC (dat, offset);
      last_offset = omap[i].address;
      LOG_HANDLE ("Offset: %8d [MC] @%lu\n", (int)offset, last_offset);

      ckr_missing = 1;
      if (dat->byte - pvzadr > 2030) // 2029
        {
          ckr_missing = 0;
          sec_size = dat->byte - pvzadr;
          assert (pvzadr);
          // i.e. encode_patch_RS_LE_size
          dat->chain[pvzadr] = sec_size >> 8;
          dat->chain[pvzadr + 1] = sec_size & 0xFF;
          LOG_TRACE ("Handles page size: %u [RS_LE] @%lu\n", sec_size, pvzadr);
          bit_write_CRC_LE (dat, pvzadr, 0xC0C1);

          pvzadr = dat->byte;
          dat->byte += 2;
          last_offset = 0;
          last_handle = 0;
        }
    }
  // printf ("Obj size: %u\n", i);
  if (ckr_missing)
    {
      sec_size = dat->byte - pvzadr;
      assert (pvzadr);
      // i.e. encode_patch_RS_LE_size
      dat->chain[pvzadr] = sec_size >> 8;
      dat->chain[pvzadr + 1] = sec_size & 0xFF;
      LOG_TRACE ("Handles page size: %u [RS_LE] @%lu\n", sec_size, pvzadr);
      bit_write_CRC_LE (dat, pvzadr, 0xC0C1);
    }
  if (dwg->header.version >= R_1_2)
    {
      assert (dat->chain[0] == 'A');
      assert (dat->chain[1] == 'C');
    }
  pvzadr = dat->byte;
  assert (pvzadr);
  bit_write_RS_LE (dat, 2); // last section_size 2
  LOG_TRACE ("Handles page size: %u [RS_LE] @%lu\n", 2, pvzadr);
  bit_write_CRC_LE (dat, pvzadr, 0xC0C1);

  /* Calculate and write the size of the object map
   */
  dwg->header.section[SECTION_HANDLES_R13].size
      = dat->byte - dwg->header.section[SECTION_HANDLES_R13].address;
  free (omap);

  /*------------------------------------------------------------
   * Second header, section 3. R13-R2000 only.
   * But partially also since r2004.
   */
  if (dwg->header.version >= R_13 && dwg->second_header.num_sections > 3)
    {
      struct _dwg_second_header *_obj = &dwg->second_header;
      Dwg_Object *obj = NULL;
      BITCODE_BL vcount;

      assert (dat->byte);
      if (!_obj->address)
        _obj->address = dat->byte;
      dwg->header.section[SECTION_2NDHEADER_R13].number = 3;
      dwg->header.section[SECTION_2NDHEADER_R13].address = _obj->address;
      dwg->header.section[SECTION_2NDHEADER_R13].size = _obj->size;
      LOG_INFO ("\n=======> Second Header: %4u\n", (unsigned)dat->byte);
      bit_write_sentinel (dat,
                          dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_BEGIN));

      pvzadr = dat->byte; // Keep the first address of the section to write its
                          // size later
      LOG_TRACE ("pvzadr: %u\n", (unsigned)pvzadr);
      if (!_obj->size && !_obj->num_sections)
        {
          LOG_TRACE ("Use second_header defaults...\n");
          strcpy ((char *)&_obj->version[0],
                  &version_codes[dwg->header.version][0]);
          memset (&_obj->version[7], 0, 4);
          _obj->version[11] = '\n';
          _obj->unknown_10 = 0x10;
          _obj->unknown_rc4[0] = 0x84;
          _obj->unknown_rc4[1] = 0x74;
          _obj->unknown_rc4[2] = 0x78;
          _obj->unknown_rc4[3] = 0x1;
          _obj->junk_r14_1 = 1957593121; //?
          _obj->junk_r14_2 = 2559919056; //?
          // TODO handlers defaults
        }
      // always recomputed, even with dwgrewrite
      if (dwg->header.version <= R_2000)
        {
          _obj->num_sections = dwg->header.num_sections;
          for (i = 0; i < _obj->num_sections; i++)
            {
              _obj->section[i].nr = dwg->header.section[i].number;
              _obj->section[i].address = dwg->header.section[i].address;
              _obj->section[i].size = dwg->header.section[i].size;
            }
        }
      FIELD_RL (size, 0);
      if (FIELD_VALUE (address) != (BITCODE_RL) (pvzadr - 16))
        {
          LOG_WARN ("second_header->address %u != %u", FIELD_VALUE (address),
                    (unsigned)(pvzadr - 16));
          FIELD_VALUE (address) = pvzadr - 16;
          dwg->header.section[SECTION_2NDHEADER_R13].address = _obj->address;
          dwg->header.section[SECTION_2NDHEADER_R13].size = _obj->size;
        }
      FIELD_BL (address, 0);

      // AC1012, AC1014 or AC1015. This is a char[11], zero padded.
      // with \n at 12.
      bit_write_TF (dat, (char *)_obj->version, 12);
      LOG_TRACE ("version: %s [TFF 12]\n", _obj->version)

      for (i = 0; i < 4; i++)
        FIELD_B (null_b[i], 0);
      FIELD_RC (unknown_10, 0); // 0x10
      for (i = 0; i < 4; i++)
        FIELD_RC (unknown_rc4[i], 0);

      UNTIL (R_2000)
      {
        FIELD_RC (num_sections, 0); // r14: 5, r2000: 6 (auxheader)
        for (i = 0; i < FIELD_VALUE (num_sections); i++)
          {
            FIELD_RC (section[i].nr, 0);
            FIELD_BL (section[i].address, 0);
            FIELD_BLd (section[i].size, 0);
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

        _obj->size = encode_patch_RLsize (dat, pvzadr);
        bit_write_CRC (dat, pvzadr, 0xC0C1);

        VERSION (R_14)
        {
          FIELD_RL (junk_r14_1, 0);
          FIELD_RL (junk_r14_2, 0);
        }
      }
      bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_END));
      dwg->header.section[SECTION_2NDHEADER_R13].size
          = dat->byte - _obj->address;
    }
  else if (dwg->header.num_sections > SECTION_2NDHEADER_R13)
    {
      dwg->header.section[SECTION_2NDHEADER_R13].number = 3;
      dwg->header.section[SECTION_2NDHEADER_R13].address = 0;
      dwg->header.section[SECTION_2NDHEADER_R13].size = 0;
    }

  /*------------------------------------------------------------
   * MEASUREMENT Section 4
   * In a DXF under header_vars
   */
  if (dwg->header.num_sections > SECTION_MEASUREMENT_R13)
    {
      LOG_INFO ("\n=======> MEASUREMENT: %4u\n", (unsigned)dat->byte);
      dwg->header.section[SECTION_MEASUREMENT_R13].number = 4;
      dwg->header.section[SECTION_MEASUREMENT_R13].address = dat->byte;
      dwg->header.section[SECTION_MEASUREMENT_R13].size = 4;
      // 0 - English, 1- Metric
      bit_write_RL (dat, (BITCODE_RL)dwg->header_vars.MEASUREMENT);
      LOG_TRACE ("HEADER.MEASUREMENT: %d [RL]\n",
                 dwg->header_vars.MEASUREMENT);
    }

  /* End of the file
   */
  dat->size = dat->byte;
  LOG_INFO ("\nFinal DWG size: %u\n", (unsigned)dat->size);

  /* Write section addresses
   */
  assert (section_address);
  dat->byte = section_address;
  dat->bit = 0;
  LOG_INFO ("\n=======> section addresses: %4u\n", (unsigned)dat->byte);
  for (j = 0; j < dwg->header.num_sections; j++)
    {
      bit_write_RC (dat, dwg->header.section[j].number);
      bit_write_RL (dat, dwg->header.section[j].address);
      bit_write_RL (dat, dwg->header.section[j].size);
      LOG_TRACE ("section[%u].number: %4d [RC] %s\n", j,
                 (int)dwg->header.section[j].number,
                 j < 6 ? section_names[j] : "")
      LOG_TRACE ("section[%u].offset: %4u [RL]\n", j,
                 (unsigned)dwg->header.section[j].address)
      LOG_TRACE ("section[%u].size:   %4u [RL]\n", j,
                 (int)dwg->header.section[j].size)
      if (dwg->header.section[j].address + dwg->header.section[j].size
          > dat->size)
        {
          LOG_ERROR ("section[%u] address or size overflow", j);
          return DWG_ERR_INVALIDDWG;
        }
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
      ckr ^= 0xA598;
      break;
    case 4:
      ckr ^= 0x8101;
      break;
    case 5:
      ckr ^= 0x3CC4;
      break;
    case 6:
      ckr ^= 0x8461;
      break;
    default:
      break;
    }
  bit_write_RS (dat, ckr);
  LOG_TRACE ("crc: %04X (from 0)\n", ckr);

  return 0;
}
AFL_GCC_POP

static int
encode_preR13 (Dwg_Data *restrict dwg, Bit_Chain *restrict dat)
{
  return DWG_ERR_NOTYETSUPPORTED;
}

#include "dwg.spec"

// expand aliases: name => CLASSES.dxfname
static const char *
dxf_encode_alias (char *restrict name)
{
  if (strEQc (name, "DICTIONARYWDFLT"))
    return "ACDBDICTIONARYWDFLT";
  else if (strEQc (name, "SECTIONVIEWSTYLE"))
    return "ACDBSECTIONVIEWSTYLE";
  else if (strEQc (name, "PLACEHOLDER"))
    return "ACDBPLACEHOLDER";
  else if (strEQc (name, "DETAILVIEWSTYLE"))
    return "ACDBDETAILVIEWSTYLE";
  else if (strEQc (name, "ASSOCPERSSUBENTMANAGER"))
    return "ACDBASSOCPERSSUBENTMANAGER";
  else if (strEQc (name, "EVALUATION_GRAPH"))
    return "ACAD_EVALUATION_GRAPH";
  else if (strEQc (name, "ASSOCACTION"))
    return "ACDBASSOCACTION";
  else if (strEQc (name, "ASSOCALIGNEDDIMACTIONBODY"))
    return "ACDBASSOCALIGNEDDIMACTIONBODY";
  else if (strEQc (name, "ASSOCOSNAPPOINTREFACTIONPARAM"))
    return "ACDBASSOCOSNAPPOINTREFACTIONPARAM";
  else if (strEQc (name, "ASSOCVERTEXACTIONPARAM"))
    return "ACDBASSOCVERTEXACTIONPARAM";
  else if (strEQc (name, "ASSOCGEOMDEPENDENCY"))
    return "ACDBASSOCGEOMDEPENDENCY";
  else if (strEQc (name, "ASSOCDEPENDENCY"))
    return "ACDBASSOCDEPENDENCY";
  else if (strEQc (name, "TABLE"))
    return "ACAD_TABLE";
  else
    return NULL;
}

Dwg_Class *
dwg_encode_get_class (Dwg_Data *dwg, Dwg_Object *obj)
{
  int i;
  Dwg_Class *klass = NULL;
  if (!dwg || !dwg->dwg_class)
    return NULL;
  // indxf has a different class order
  if (obj->dxfname) // search class by name, not offset
    {
      int invalid_klass = 0;
      for (i = 0; i < dwg->num_classes; i++)
        {
          klass = &dwg->dwg_class[i];
          if (!klass->dxfname)
            {
              invalid_klass++;
              continue;
            }
          if (strEQ (obj->dxfname, klass->dxfname))
            {
              obj->type = 500 + i;
              break;
            }
          else
            {
              // alias DICTIONARYWDFLT => ACDBDICTIONARYWDFLT
              const char *alias = dxf_encode_alias (obj->dxfname);
              if (alias && klass->dxfname && strEQ (alias, klass->dxfname))
                {
                  // a static string, which cannot be free'd. important for
                  // indxf
                  if (dwg->opts & DWG_OPTS_INDXF)
                    obj->dxfname = strdup ((char *)alias);
                  else
                    obj->dxfname = (char *)alias;
                  obj->type = 500 + i;
                  break;
                }
              klass = NULL; // inefficient

              if (invalid_klass > 2 && !(dwg->opts & DWG_OPTS_INDXF))
                goto search_by_index;
            }
        }
    }
  else // search by index
    {
    search_by_index:
      i = obj->type - 500;
      if (i < 0 || i >= (int)dwg->num_classes)
        {
          LOG_WARN ("Invalid object type %d, only %u classes", obj->type,
                    dwg->num_classes);
          return NULL;
        }

      klass = &dwg->dwg_class[i];
      if (!klass->dxfname)
        return NULL;
      obj->dxfname = klass->dxfname;
    }
  return klass;
}

/** dwg_encode_variable_type
 * Encode object by class name, not type. if type > 500.
 * Returns 0 on success, else some Dwg_Error.
 */
static int
dwg_encode_variable_type (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                          Dwg_Object *restrict obj)
{
  int error = 0;
  int is_entity;
  Dwg_Class *klass = dwg_encode_get_class (dwg, obj);

  if (!klass)
    return DWG_ERR_INVALIDTYPE;
  is_entity = dwg_class_is_entity (klass);
  // check if it really was an entity
  if ((is_entity && obj->supertype == DWG_SUPERTYPE_OBJECT)
      || (!is_entity && obj->supertype == DWG_SUPERTYPE_ENTITY))
    {
      if (is_dwg_object (obj->name))
        {
          if (is_entity)
            {
              LOG_INFO ("Fixup Class %s item_class_id to %s for %s\n",
                        klass->dxfname, "OBJECT", obj->name);
              klass->item_class_id = 0x1f2;
              if (strNE (klass->dxfname, obj->dxfname))
                {
                  free (klass->dxfname);
                  klass->dxfname = strdup (obj->dxfname);
                }
              is_entity = 0;
            }
          else
            {
              LOG_INFO ("Fixup %s.supertype to %s\n", obj->name, "OBJECT");
              obj->supertype = DWG_SUPERTYPE_OBJECT;
            }
        }
      else if (is_dwg_entity (obj->name))
        {
          if (!is_entity)
            {
              LOG_INFO ("Fixup Class %s item_class_id to %s for %s\n",
                        klass->dxfname, "ENTITY", obj->name);
              klass->item_class_id = 0x1f3;
              if (strNE (klass->dxfname, obj->dxfname))
                {
                  free (klass->dxfname);
                  klass->dxfname = strdup (obj->dxfname);
                }
              is_entity = 1;
            }
          else
            {
              LOG_INFO ("Fixup %s.supertype to %s", obj->name, "ENTITY");
              obj->supertype = DWG_SUPERTYPE_ENTITY;
            }
        }
      else
        {
          LOG_ERROR ("Illegal Class %s is_%s item_class_id for %s",
                     klass->dxfname, is_entity ? "entity" : "object",
                     obj->name);
          return DWG_ERR_INVALIDTYPE;
        }
    }

  if (dwg->opts & DWG_OPTS_INDXF) // DXF import
    {
      unsigned long pos = bit_position (dat);
      dat->byte = obj->address;
      dat->bit = 0;
      LOG_TRACE ("fixup Type: %d [BS] @%lu\n", obj->type, obj->address);
      bit_write_BS (dat, obj->type); // fixup wrong type
      bit_set_position (dat, pos);
    }

    // clang-format off
  #include "classes.inc"
  // clang-format on

  LOG_WARN ("Unknown Class %s %d %s (0x%x%s)", is_entity ? "entity" : "object",
            klass->number, klass->dxfname, klass->proxyflag,
            klass->is_zombie ? "is_zombie" : "")

#undef WARN_UNHANDLED_CLASS
#undef WARN_UNSTABLE_CLASS

  return DWG_ERR_UNHANDLEDCLASS;
}

int
dwg_encode_add_object (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                       unsigned long address)
{
  int error = 0;
  unsigned long oldpos;
  unsigned long end_address = address + obj->size;
  Dwg_Data *dwg = obj->parent;

  oldpos = bit_position (dat);
  assert (address);
  dat->byte = address;
  dat->bit = 0;

  LOG_INFO ("Object number: %lu", (unsigned long)obj->index);
  if (obj->size > 0x100000)
    {
      LOG_ERROR ("Object size %u overflow", obj->size);
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
  while (dat->byte + obj->size >= dat->size)
    bit_chain_alloc (dat);

  // TODO: calculate size from the fields. either <0x7fff or more
  // patch it afterwards and check old<>new size if enough space allocated.
  bit_write_MS (dat, obj->size);
  obj->address = dat->byte;
  PRE (R_2010)
  {
    bit_write_BS (dat, obj->type);
    LOG_INFO (", Size: %d [MS], Type: %d [BS]\n", obj->size, obj->type)
  }
  LATER_VERSIONS
  {
    if (!obj->handlestream_size && obj->bitsize)
      obj->handlestream_size = obj->size * 8 - obj->bitsize;
    bit_write_UMC (dat, obj->handlestream_size);
    obj->address = dat->byte;
    bit_write_BOT (dat, obj->type);
    LOG_INFO (", Size: %d [MS], Hdlsize: %lu [UMC], Type: %d [BOT]\n",
              obj->size, (unsigned long)obj->handlestream_size, obj->type)
  }

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
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_OLE2FRAME:
      error = dwg_encode_OLE2FRAME (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_DUMMY:
      error = dwg_encode_DUMMY (dat, obj);
      break;
    case DWG_TYPE_LONG_TRANSACTION:
      error = dwg_encode_LONG_TRANSACTION (dat, obj);
      break;
    case DWG_TYPE_LWPOLYLINE:
      error = dwg_encode_LWPOLYLINE (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_HATCH:
      error = dwg_encode_HATCH (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_XRECORD:
      error = dwg_encode_XRECORD (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_PLACEHOLDER:
      error = dwg_encode_PLACEHOLDER (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_OLEFRAME:
      error = dwg_encode_OLEFRAME (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own section");
      // dwg_encode_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      error |= dwg_encode_LAYOUT (dat, obj);
      (void)dwg_encode_get_class (dwg, obj);
      break;
    case DWG_TYPE_PROXY_ENTITY:
      error = dwg_encode_PROXY_ENTITY (dat, obj);
      break;
    case DWG_TYPE_PROXY_OBJECT:
      error = dwg_encode_PROXY_OBJECT (dat, obj);
      break;
    default:
      if (dwg && obj->type == dwg->layout_type
          && obj->fixedtype == DWG_TYPE_LAYOUT)
        {
          error = dwg_encode_LAYOUT (dat, obj);
          (void)dwg_encode_get_class (dwg, obj);
        }
      else if (dwg != NULL
               && (error = dwg_encode_variable_type (dwg, dat, obj))
                      & DWG_ERR_UNHANDLEDCLASS)
        {
          int is_entity;
          Dwg_Class *klass = dwg_encode_get_class (dwg, obj);

          assert (address);
          dat->byte = address; // restart and write into the UNKNOWN_OBJ object
          dat->bit = 0;
          bit_write_MS (dat, obj->size); // unknown blobs have a known size
          if (dat->version >= R_2010)
            {
              bit_write_UMC (dat, obj->handlestream_size);
              bit_write_BOT (dat, obj->type);
            }
          else
            bit_write_BS (dat, obj->type);

          if (klass)
            is_entity = klass->item_class_id == 0x1f2
                        && obj->supertype == DWG_SUPERTYPE_ENTITY;
          else
            is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (is_entity)
            error = dwg_encode_UNKNOWN_ENT (dat, obj);
          else
            error = dwg_encode_UNKNOWN_OBJ (dat, obj);
        }
    }

  /* DXF: patchup size and bitsize */
  if (!obj->size)
    {
      BITCODE_BL pos = bit_position (dat);
      assert (address);
      obj->size = dat->byte - address - 2; // excludes the CRC
      if (dat->bit)
        obj->size++;
      // assert (obj->bitsize); // on errors
      if (!obj->bitsize)
        {
          LOG_TRACE ("-bitsize calc from address (no handle) @%lu.%u\n",
                     dat->byte, dat->bit);
          obj->bitsize = pos - (obj->address * 8);
        }
      bit_set_position (dat, address * 8);
      if (obj->size > 0x7fff)
        // TODO: with overlarge sizes >0x7fff memmove dat
        LOG_ERROR ("Unhandled size %u > 0x7fff", (unsigned)obj->size);
      bit_write_MS (dat, obj->size);
      LOG_TRACE ("-size: %u [MS] @%lu\n", obj->size, address);
      SINCE (R_2013)
      {
        if (!obj->handlestream_size && obj->bitsize)
          obj->handlestream_size = obj->size * 8 - obj->bitsize;
        bit_write_UMC (dat, obj->handlestream_size);
        LOG_TRACE ("-handlestream_size: %lu [UMC]\n", obj->handlestream_size);
      }
      SINCE (R_2000)
      {
        if (obj->bitsize_pos && obj->bitsize)
          {
            bit_set_position (dat, obj->bitsize_pos);
            bit_write_RL (dat, obj->bitsize);
            LOG_TRACE ("-bitsize: %u [RL] @%lu.%lu\n", obj->bitsize,
                       obj->bitsize_pos / 8, obj->bitsize_pos % 8);
          }
      }
      bit_set_position (dat, pos);
    }

  /* Now 1 padding bits until next byte, and then a RS CRC */
  if (dat->bit)
    LOG_TRACE ("padding: +%d [*B]\n", 8 - dat->bit)
  while (dat->bit)
    bit_write_B (dat, 1);
  end_address = obj->address + obj->size;
  if (end_address != dat->byte)
    {
      if (obj->size)
        LOG_WARN ("Wrong object size: %lu + %u = %lu != %lu: %ld off",
                  obj->address, obj->size, end_address, dat->byte,
                  (long)(end_address - dat->byte));
      dat->byte = end_address;
    }
  assert (!dat->bit);
  bit_write_CRC (dat, address, 0xC0C1);
  return error;
}

/** writes the data part, if there's no raw.
 */
static int
dwg_encode_eed_data (Bit_Chain *restrict dat, Dwg_Eed_Data *restrict data,
                     const int size, const int i)
{
  bit_write_RC (dat, data->code);
  LOG_TRACE ("EED[%d] code: %d [RC] ", i, data->code);
  switch (data->code)
    {
    case 0:
      {
        PRE (R_2007)
        {
          if (data->u.eed_0.length + 3 <= size)
            {
              if (!*data->u.eed_0.string)
                data->u.eed_0.length = 0;
              bit_write_RC (dat, data->u.eed_0.length);
              bit_write_RS_LE (dat, data->u.eed_0.codepage);
              bit_write_TF (dat, data->u.eed_0.string, data->u.eed_0.length);
            }
          LOG_TRACE ("string: len=%d [RC] cp=%d [RS_LE] \"%s\" [TF]\n",
                     data->u.eed_0.length, data->u.eed_0.codepage,
                     data->u.eed_0.string);
        }
        LATER_VERSIONS
        {
          BITCODE_RS *s = (BITCODE_RS *)&data->u.eed_0_r2007.string;
          if (data->u.eed_0.length * 2 + 2 <= size)
            {
              bit_write_RS (dat, data->u.eed_0_r2007.length);
              for (int j = 0; j < data->u.eed_0_r2007.length; j++)
                bit_write_RS (dat, *s++);
            }
          else
            bit_write_RS (dat, 0);
#ifdef _WIN32
          LOG_TRACE ("wstring: len=%d [RS] \"" FORMAT_TU "\" [TU]\n",
                     (int)data->u.eed_0_r2007.length,
                     data->u.eed_0_r2007.string);
#else
          if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)
            {
              char *u8 = bit_convert_TU (data->u.eed_0_r2007.string);
              LOG_TRACE ("wstring: len=%d [RS] \"%s\" [TU]\n",
                         (int)data->u.eed_0_r2007.length, u8);
              free (u8);
            }
#endif
        }
      }
      break;
    case 2:
      if (1 <= size)
        bit_write_RC (dat, data->u.eed_2.byte);
      LOG_TRACE ("byte: %d [RC]\n", (int)data->u.eed_2.byte);
      break;
    case 3:
      if (4 <= size)
        bit_write_RL (dat, data->u.eed_3.layer);
      LOG_TRACE ("layer: %d [RL]\n", (int)data->u.eed_3.layer);
      break;
    case 4:
      if (data->u.eed_0.length + 1 <= size)
        {
          bit_write_RC (dat, data->u.eed_4.length);
          bit_write_TF (dat, data->u.eed_4.data, data->u.eed_4.length);
        }
      LOG_TRACE ("binary: \"%s\" [TF %d]\n", data->u.eed_4.data,
                 data->u.eed_4.length);
      break;
    case 5:
      if (8 <= size)
        bit_write_RLL (dat, data->u.eed_5.entity);
      LOG_TRACE ("entity: 0x%lX [RLL]\n", (unsigned long)data->u.eed_5.entity);
      break;
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      if (24 <= size)
        {
          bit_write_RD (dat, data->u.eed_10.point.x);
          bit_write_RD (dat, data->u.eed_10.point.y);
          bit_write_RD (dat, data->u.eed_10.point.z);
        }
      LOG_TRACE ("3dpoint: (%f, %f, %f) [3RD]\n", data->u.eed_10.point.x,
                 data->u.eed_10.point.y, data->u.eed_10.point.z);
      break;
    case 40:
    case 41:
    case 42:
      if (8 <= size)
        bit_write_RD (dat, data->u.eed_40.real);
      LOG_TRACE ("real: %f [RD]\n", data->u.eed_40.real);
      break;
    case 70:
      if (2 <= size)
        bit_write_RS (dat, data->u.eed_70.rs);
      LOG_TRACE ("short: " FORMAT_RS " [RS]\n", data->u.eed_70.rs);
      break;
    case 71:
      if (4 <= size)
        bit_write_RL (dat, data->u.eed_71.rl);
      LOG_TRACE ("long: " FORMAT_RL " [RL]\n", data->u.eed_71.rl);
      break;
    default:
      LOG_ERROR ("unknown EED code %d", data->code);
    }
  return 0;
}

/** Either writes the raw part.
    Only members with size have raw and a handle.
    Otherwise (indxf) defer to dwg_encode_eed_data.
 */
static int
dwg_encode_eed (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  unsigned long off = obj->address;
  unsigned long last_handle = 0;

#define LOG_POS LOG_INSANE (" @%lu.%u\n", dat->byte - off, dat->bit)

  int i, num_eed = obj->tio.object->num_eed;
  for (i = 0; i < num_eed; i++)
    {
      Dwg_Eed *eed = &obj->tio.object->eed[i];
      BITCODE_BS size = eed->size;
      if (size)
        {
          bit_write_BS (dat, size);
          LOG_TRACE ("EED[%d] size: " FORMAT_BS " [BS]\n", i, size);
          LOG_POS
          bit_write_H (dat, &eed->handle);
          LOG_TRACE ("EED[%d] handle: " FORMAT_H " [H]\n", i,
                     ARGS_H (eed->handle));
          LOG_POS
          if (eed->raw)
            {
              last_handle = eed->handle.value;
              LOG_TRACE ("EED[%d] raw [TF %d]\n", i, size);
              bit_write_TF (dat, eed->raw, size);
              LOG_TRACE_TF (eed->raw, size);
            }
          // indxf
          else if (eed->data)
            {
              dwg_encode_eed_data (dat, eed->data, size, i);
              LOG_POS
            }
        }
      // and if not already written by the previous raw (this has size=0)
      else if (eed->data != NULL && eed->handle.value != last_handle)
        {
          dwg_encode_eed_data (dat, eed->data, size, i);
          LOG_POS
        }
    }
  bit_write_BS (dat, 0);
  if (i)
    LOG_TRACE ("EED[%d] size: 0 [BS] (end)\n", i);
  LOG_TRACE ("num_eed: %d\n", num_eed);
  LOG_POS
#undef LOG_POS
  return 0;
}

/* The first common part of every entity.

   The last common part is common_entity_handle_data.spec
   which is read from the hdl stream.
   See DWG_SUPERTYPE_ENTITY in dwg_encode().
 */
static int
dwg_encode_entity (Dwg_Object *restrict obj, Bit_Chain *hdl_dat,
                   Bit_Chain *str_dat, Bit_Chain *dat)
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
    bit_write_RL (dat, obj->bitsize);
    LOG_TRACE ("bitsize: %u [RL] (@%lu.%lu)\n", obj->bitsize,
               obj->bitsize_pos / 8, obj->bitsize_pos % 8);
  }
  if (obj->bitsize)
    obj->hdlpos = obj->address * 8 + obj->bitsize;
  SINCE (R_2007)
  {
    // The handle stream offset, i.e. end of the object, right after
    // the has_strings bit.
    SINCE (R_2010)
    {
      if (obj->bitsize)
        {
          obj->hdlpos += 8;
          // LOG_HANDLE ("(bitsize: " FORMAT_RL ", ", obj->bitsize);
          LOG_HANDLE ("hdlpos: %lu\n", obj->hdlpos);
        }
    }
    // and set the string stream (restricted to size)
    error |= obj_string_stream (dat, obj, str_dat);
  }

  bit_write_H (dat, &obj->handle);
  LOG_TRACE ("handle: " FORMAT_H " [H 5]\n", ARGS_H (obj->handle))
  PRE (R_13) { return DWG_ERR_NOTYETSUPPORTED; }

  error |= dwg_encode_eed (dat, obj);
  // if (error & (DWG_ERR_INVALIDTYPE|DWG_ERR_VALUEOUTOFBOUNDS))
  //  return error;

  // clang-format off
  #include "common_entity_data.spec"
  // clang-format on

  return error;
}

static int
dwg_encode_common_entity_handle_data (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                      Dwg_Object *restrict obj)
{
  Dwg_Object_Entity *ent;
  // Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  BITCODE_BL vcount;
  int error = 0;
  ent = obj->tio.entity;
  _obj = ent;

  // clang-format off
  #include "common_entity_handle_data.spec"
  // clang-format on

  return error;
}

void
dwg_encode_handleref (Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                      Dwg_Data *restrict dwg, Dwg_Object_Ref *restrict ref)
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
dwg_encode_handleref_with_code (Bit_Chain *hdl_dat, Dwg_Object *restrict obj,
                                Dwg_Data *restrict dwg,
                                Dwg_Object_Ref *restrict ref,
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
dwg_encode_object (Dwg_Object *restrict obj, Bit_Chain *hdl_dat,
                   Bit_Chain *str_dat, Bit_Chain *dat)
{
  int error = 0;
  Dwg_Object_Object *ord = obj->tio.object;

  VERSIONS (R_2000, R_2007)
  {
    obj->bitsize_pos = bit_position (dat);
    bit_write_RL (dat, obj->bitsize);
    LOG_INFO ("bitsize: " FORMAT_RL " [RL] (@%lu.%u)\n", obj->bitsize,
              dat->byte - 4, dat->bit);
  }
  if (obj->bitsize)
    obj->hdlpos
        = bit_position (dat) + obj->bitsize; // the handle stream offset
  SINCE (R_2007) { obj_string_stream (dat, obj, str_dat); }

  bit_write_H (dat, &obj->handle);
  LOG_TRACE ("handle: " FORMAT_H " [H 5]\n", ARGS_H (obj->handle));
  error |= dwg_encode_eed (dat, obj);

  VERSIONS (R_13, R_14)
  {
    obj->bitsize_pos = bit_position (dat);
    bit_write_RL (dat, obj->bitsize);
    LOG_INFO ("bitsize: " FORMAT_RL " [RL] (@%lu.%u)\n", obj->bitsize,
              dat->byte - 4, dat->bit);
  }

  bit_write_BL (dat, ord->num_reactors);
  LOG_TRACE ("num_reactors: " FORMAT_BL " [BL]\n", ord->num_reactors);
  SINCE (R_2004)
  {
    bit_write_B (dat, ord->xdic_missing_flag);
    LOG_TRACE ("xdic_missing_flag: " FORMAT_B " [B]\n",
               ord->xdic_missing_flag);
  }
  SINCE (R_2013)
  {
    bit_write_B (dat, ord->has_ds_binary_data);
    LOG_TRACE ("has_ds_binary_data: " FORMAT_B " [B]\n",
               ord->has_ds_binary_data);
  }
  return error;
}

AFL_GCC_TOOBIG
static int
dwg_encode_header_variables (Bit_Chain *dat, Bit_Chain *hdl_dat,
                             Bit_Chain *str_dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  int old_from = (int)dat->from_version;

  if (!_obj->HANDSEED) // minimal or broken DXF
    {
      dwg->opts |= DWG_OPTS_MINIMAL;
      dat->from_version = dat->version - 1;
      LOG_TRACE ("encode from minimal DXF\n");
      _obj->HANDSEED = calloc (1, sizeof (Dwg_Object_Ref));
      _obj->HANDSEED->absolute_ref = 0x72E;
    }

    // clang-format off
  #include "header_variables.spec"
  // clang-format on

  dat->from_version = old_from;
  return 0;
}
AFL_GCC_POP

static int
dwg_encode_xdata (Bit_Chain *restrict dat, Dwg_Object_XRECORD *restrict obj,
                  int num_databytes)
{
  Dwg_Resbuf *rbuf = obj->xdata;
  enum RES_BUF_VALUE_TYPE type;
  int i;
  unsigned j = 0;
  BITCODE_BL num_xdata = obj->num_xdata;
  unsigned long start = dat->byte, end = start + num_databytes;
  int error = 0;

  while (rbuf)
    {
      LOG_INSANE ("xdata[%u] type: " FORMAT_RS " [RS]\n", num_xdata,
                  rbuf->type)
      bit_write_RS (dat, rbuf->type);
      type = get_base_value_type (rbuf->type);
      switch (type)
        {
        case VT_STRING:
          UNTIL (R_2007)
          {
            if (dat->byte + 3 + rbuf->value.str.size > end)
              break;
            bit_write_RS (dat, rbuf->value.str.size);
            bit_write_RC (dat, rbuf->value.str.codepage);
            if (rbuf->value.str.u.data)
              bit_write_TF (dat, rbuf->value.str.u.data, rbuf->value.str.size);
            else
              bit_write_TF (dat, (char *)"", 0);
            LOG_TRACE ("xdata[%u]: \"%s\" [TF %d %d]\n", j,
                       rbuf->value.str.u.data, rbuf->value.str.size,
                       rbuf->type);
          }
          LATER_VERSIONS
          {
            if (dat->byte + 2 + (2 * rbuf->value.str.size) > end
                || rbuf->value.str.size < 0)
              break;
            bit_write_RS (dat, rbuf->value.str.size);
            for (i = 0; i < rbuf->value.str.size; i++)
              bit_write_RS (dat, rbuf->value.str.u.wdata[i]);
          }
          break;
        case VT_REAL:
          if (dat->byte + 8 > end)
            break;
          bit_write_RD (dat, rbuf->value.dbl);
          LOG_TRACE ("xdata[%u]: %f [RD %d]\n", j, rbuf->value.dbl,
                     rbuf->type);
          break;
        case VT_BOOL:
        case VT_INT8:
          bit_write_RC (dat, rbuf->value.i8);
          LOG_TRACE ("xdata[%u]: %d [RC %d]\n", j, (int)rbuf->value.i8,
                     rbuf->type);
          break;
        case VT_INT16:
          if (dat->byte + 2 > end)
            break;
          bit_write_RS (dat, rbuf->value.i16);
          LOG_TRACE ("xdata[%u]: %d [RS %d]\n", j, (int)rbuf->value.i16,
                     rbuf->type);
          break;
        case VT_INT32:
          if (dat->byte + 4 > end)
            break;
          bit_write_RL (dat, rbuf->value.i32);
          LOG_TRACE ("xdata[%d]: %ld [RL %d]\n", j, (long)rbuf->value.i32,
                     rbuf->type);
          break;
        case VT_INT64:
          if (dat->byte + 8 > end)
            break;
          bit_write_BLL (dat, rbuf->value.i64);
          LOG_TRACE ("xdata[%u]: " FORMAT_BLL " [BLL %d]\n", j,
                     rbuf->value.i64, rbuf->type);
          break;
        case VT_POINT3D:
          if (dat->byte + 24 > end)
            break;
          bit_write_RD (dat, rbuf->value.pt[0]);
          bit_write_RD (dat, rbuf->value.pt[1]);
          bit_write_RD (dat, rbuf->value.pt[2]);
          LOG_TRACE ("xdata[%u]: (%f,%f,%f) [3RD %d]\n", j, rbuf->value.pt[0],
                     rbuf->value.pt[1], rbuf->value.pt[2], rbuf->type);
          break;
        case VT_BINARY:
          if (dat->byte + rbuf->value.str.size > end)
            break;
          bit_write_RC (dat, rbuf->value.str.size);
          bit_write_TF (dat, rbuf->value.str.u.data, rbuf->value.str.size);
          LOG_TRACE ("xdata[%u]: [TF %d %d] ", j, rbuf->value.str.size,
                     rbuf->type);
          LOG_TRACE_TF (rbuf->value.str.u.data, rbuf->value.str.size);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          if (dat->byte + 8 > end)
            break;
          for (i = 0; i < 8; i++)
            bit_write_RC (dat, rbuf->value.hdl[i]);
          LOG_TRACE ("xdata[%u]: " FORMAT_H " [H %d]\n", j,
                     ARGS_H (rbuf->value.h), rbuf->type);
          break;
        case VT_INVALID:
        default:
          LOG_ERROR ("Invalid group code in xdata: %d", rbuf->type);
          error = DWG_ERR_INVALIDEED;
          break;
        }
      rbuf = rbuf->next;
      if (j > obj->num_xdata)
        break;
      if (dat->byte - start >= (unsigned long)num_databytes)
        break;
      j++;
    }
  if (obj->num_databytes != dat->byte - start)
    {
      LOG_WARN ("xdata Written %lu, expected %d", dat->byte - start,
                obj->num_databytes);
      obj->num_databytes = dat->byte - start;
      return error ? error : 1;
    }
  return 0;
}

char *
encrypt_sat1 (BITCODE_BL blocksize, BITCODE_RC *acis_data, int *idx)
{
  char *encr_sat_data = calloc (blocksize, 1);
  int i = *idx;
  int j;
  for (j = 0; j < (int)blocksize; j++)
    {
      if (acis_data[j] <= 32)
        encr_sat_data[i++] = acis_data[j];
      else
        encr_sat_data[i++] = acis_data[j] - 159;
      /* TODO reversion of:
      if (encr_sat_data[j] <= 32)
        acis_data[i++] = encr_sat_data[j];
      else
        acis_data[i++] = 159 - encr_sat_data[j];
      */
    }
  *idx = i;
  return encr_sat_data;
}

#undef IS_ENCODER

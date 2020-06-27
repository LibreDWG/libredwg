/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2021 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * out_dxfb.c: write as Binary DXF
 * Does not work yet.
 * written by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define IS_DXF
#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "decode_r11.h"
#include "out_dxf.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block. */
static unsigned int cur_ver = 0;
static char buf[4096];
static BITCODE_BL rcount1, rcount2;

// imported
char *dwg_obj_table_get_name (const Dwg_Object *restrict obj,
                              int *restrict error);
#ifndef _DWG_API_H_
Dwg_Object *dwg_obj_generic_to_object (const void *restrict obj,
                                       int *restrict error);
#endif

// private
static int dxfb_common_entity_handle_data (Bit_Chain *restrict dat,
                                           const Dwg_Object *restrict obj);
static int dwg_dxfb_object (Bit_Chain *restrict dat,
                            const Dwg_Object *restrict obj, int *restrict);
static int dxfb_3dsolid (Bit_Chain *restrict dat,
                         const Dwg_Object *restrict obj,
                         Dwg_Entity_3DSOLID *restrict _obj);
/*static void dxfb_cvt_tablerecord (Bit_Chain *restrict dat,
                                  const Dwg_Object *restrict obj,
                                  char *restrict name, const int dxf); */

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION dxfb

#define FMT_H "%" PRIX64
#define FIELD(nam, type)
#define FIELDG(nam, type, dxf)                                                \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_##type (nam, dxf);                                                \
    }
#define SUB_FIELD(o, nam, type, dxf) FIELDG (o.nam, type, dxf)

#define HEADER_VALUE(nam, type, dxf, value)                                   \
  if (dxf)                                                                    \
    {                                                                         \
      GROUP (9);                                                              \
      fprintf (dat->fh, "$%s%c", #nam, 0);                                    \
      VALUE_##type (value, dxf);                                              \
    }

#define HEADER_VAR(nam, type, dxf)                                            \
  HEADER_VALUE (nam, type, dxf, dwg->header_vars.nam)
#define HEADER_VALUE_TV(nam, dxf, value) HEADER_VALUE (nam, TV, dxf, value)
#define HEADER_VALUE_TU(nam, dxf, value) HEADER_VALUE (nam, TU, dxf, value)
#define HEADER_VALUE_TU0(nam, dxf, value)                                     \
  if (dxf && value && !bit_empty_T (dat, (BITCODE_T)value))                   \
    {                                                                         \
      HEADER_VALUE (nam, TU, dxf, value);                                     \
    }

#define FIELD_CAST(nam, type, cast, dxf) FIELDG (nam, cast, dxf)
#define SUB_FIELD_CAST(o, nam, type, cast, dxf) FIELDG (o.nam, cast, dxf)
#define FIELD_TRACE(nam, type)
#define VALUE_TV(value, dxf)                                                  \
  {                                                                           \
    GROUP (dxf);                                                              \
    fprintf (dat->fh, "%s%c", value, 0);                                      \
  }
#define VALUE_TV0(value, dxf)                                                 \
  if (dxf && value && *value)                                                 \
    {                                                                         \
      GROUP (dxf);                                                            \
      fprintf (dat->fh, "%s%c", value, 0);                                    \
    }
#define VALUE_TU(wstr, dxf)                                                   \
  {                                                                           \
    if (dxf)                                                                  \
      {                                                                       \
        char *_u8 = bit_convert_TU ((BITCODE_TU)wstr);                        \
        GROUP (dxf);                                                          \
        if (_u8)                                                              \
          fprintf (dat->fh, "%s%c", _u8, 0);                                  \
        else                                                                  \
          fprintf (dat->fh, "%c", 0);                                         \
        free (_u8);                                                           \
      }                                                                       \
  }
#define VALUE_TFF(str, dxf) VALUE_TV (str, dxf)
#define VALUE_BINARY(value, size, dxf)                                        \
  {                                                                           \
    long _len = (long)(size);                                                 \
    do                                                                        \
      {                                                                       \
        short j;                                                              \
        long _l = _len > 127 ? 127 : _len;                                    \
        GROUP (dxf);                                                          \
        if (value)                                                            \
          for (j = 0; j < _l; j++)                                            \
            {                                                                 \
              fprintf (dat->fh, "%c", value[j]);                              \
            }                                                                 \
        fprintf (dat->fh, "%c", '\0');                                        \
        _len -= 127;                                                          \
      }                                                                       \
    while (_len > 127);                                                       \
  }
#define FIELD_BINARY(name, size, dxf) VALUE_BINARY (_obj->name, size, dxf)

#define FIELD_VALUE(nam) _obj->nam
#define ANYCODE -1
// a null-terminated string of the value as %X
#define VALUE_HANDLE(ref, nam, handle_code, dxf)                              \
  if (dxf)                                                                    \
    {                                                                         \
      char _s[18];                                                            \
      snprintf (_s, sizeof (_s), FMT_H, ref ? ref->absolute_ref : 0UL);       \
      _s[sizeof (_s) - 1] = '\0';                                             \
      VALUE_TV (_s, dxf);                                                     \
    }
// TODO: try to resolve the handle. rather write 0 than in invalid handle:
// if (_obj->nam->obj) ...
#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  VALUE_HANDLE (_obj->nam, nam, handle_code, dxf)
#define FIELD_HANDLE0(nam, handle_code, dxf)                                  \
  if (dxf && _obj->nam && _obj->nam->absolute_ref)                            \
    {                                                                         \
      FIELD_HANDLE (nam, handle_code, dxf);                                   \
    }
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  VALUE_HANDLE (_obj->o.nam, nam, handle_code, dxf)
#define SUB_FIELD_HANDLE0(o, nam, handle_code, dxf)                           \
  if (dxf && _obj->o.nam && _obj->o.nam->absolute_ref)                        \
    {                                                                         \
      VALUE_HANDLE (_obj->o.nam, nam, handle_code, dxf)                       \
    }

#define GROUP(code)                                                           \
  if (dat->version < R_14)                                                    \
    {                                                                         \
      if (code >= 1000)                                                       \
        {                                                                     \
          uint16_t icode = (uint16_t)(code);                                  \
          fputc (0xff, dat->fh);                                              \
          fwrite (&icode, 2, 1, dat->fh);                                     \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          uint8_t icode = (uint8_t)((code) & 0xff);                           \
          fwrite (&icode, 1, 1, dat->fh);                                     \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      uint16_t icode = (uint16_t)(code);                                      \
      fwrite (&icode, 2, 1, dat->fh);                                         \
    }
#define FIELD_TV(nam, dxf)                                                    \
  if (_obj->nam != NULL && dxf != 0)                                          \
    {                                                                         \
      VALUE_TV (_obj->nam, dxf)                                               \
    }
#define FIELD_TU(nam, dxf)                                                    \
  if (_obj->nam != NULL && dxf != 0)                                          \
    {                                                                         \
      VALUE_TU (_obj->nam, dxf)                                               \
    }
#define VALUE_T(value, dxf)                                                   \
  {                                                                           \
    if (IS_FROM_TU (dat))                                                     \
      {                                                                       \
        VALUE_TU (value, dxf)                                                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        VALUE_TV ((char *)value, dxf)                                         \
      }                                                                       \
  }
#define VALUE_T0(value, dxf)                                                  \
  if (!bit_empty_T (dat, value))                                              \
  VALUE_T (value, dxf)
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (IS_FROM_TU (dat))                                                     \
      {                                                                       \
        FIELD_TU (nam, dxf)                                                   \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_TV (nam, dxf)                                                   \
      }                                                                       \
  }
#define FIELD_TF(nam, len, dxf) VALUE_TV (_obj->nam, dxf)
#define FIELD_TFF(nam, len, dxf) VALUE_TV (_obj->nam, dxf)

// skip if 0
#define FIELD_RD0(nam, dxf) FIELD_BD0 (nam, dxf)
#define FIELD_BD0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0.0)                                                     \
      FIELD_BD (nam, dxf)                                                     \
  }
// for scale (1.0, 1.0, 1.0) and width_factor
#define FIELD_RD1(nam, dxf) FIELD_BD1 (nam, dxf)
#define FIELD_BD1(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 1.0)                                                     \
      FIELD_BD (nam, dxf)                                                     \
  }
#define FIELD_BL0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0)                                                       \
      FIELD_BL (nam, dxf)                                                     \
  }
#define SUB_FIELD_BL0(o, nam, dxf)                                            \
  {                                                                           \
    if (_obj->o.nam != 0)                                                     \
      SUB_FIELD_BL (o, nam, dxf)                                              \
  }
#define FIELD_BS0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0)                                                       \
      FIELD_BS (nam, dxf)                                                     \
  }
#define FIELD_BSd0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0)                                                       \
      FIELD_BSd (nam, dxf)                                                     \
  }
#define FIELD_BS1(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 1)                                                       \
      FIELD_BS (nam, dxf)                                                     \
  }
#define FIELD_B0(nam, dxf)                                                    \
  {                                                                           \
    if (_obj->nam)                                                            \
      FIELD_B (nam, dxf)                                                      \
  }
#define FIELD_B1(nam, dxf)                                                    \
  {                                                                           \
    if (!_obj->nam)                                                           \
      FIELD_B (nam, dxf)                                                      \
  }
#define FIELD_RC0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0)                                                       \
      FIELD_RC (nam, dxf)                                                     \
  }
#define FIELD_RS0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0)                                                       \
      FIELD_RS (nam, dxf)                                                     \
  }
#define FIELD_RL0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0)                                                       \
      FIELD_RL (nam, dxf)                                                     \
  }
#define FIELD_BT0(nam, dxf)                                                   \
  {                                                                           \
    if (_obj->nam != 0)                                                       \
      FIELD_BT (nam, dxf)                                                     \
  }
#define FIELD_T0(nam, dxf)                                                    \
  if (!bit_empty_T (dat, _obj->nam))                                          \
    {                                                                         \
      FIELD_T (nam, dxf)                                                      \
    }
#define FIELD_TV0(nam, dxf) VALUE_TV0 (_obj->nam, dxf)

#define HEADER_TV(nam, dxf)                                                   \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_TV (dwg->header_vars.nam, dxf);                                     \
  }
#define HEADER_TU(nam, dxf)                                                   \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_TU (dwg->header_vars.nam, dxf);                                     \
  }
#define HEADER_T(nam, dxf)                                                    \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_T (dwg->header_vars.nam, dxf);                                      \
  }
#define HEADER_T0(nam, dxf)                                                   \
  if (dxf && !bit_empty_T (dat, _obj->nam))                                   \
    {                                                                         \
      HEADER_9 (nam);                                                         \
      VALUE_T (_obj->nam, dxf);                                               \
    }
#define HEADER_VALUE_T(nam, dxf, value)                                       \
  if (dxf)                                                                    \
    {                                                                         \
      HEADER_9 (nam);                                                         \
      if (IS_FROM_TU (dat))                                                   \
        VALUE_T (value, dxf)                                                  \
      else                                                                    \
        VALUE_TV ((char *)value, dxf)                                         \
    }
#define HEADER_VALUE_T0(nam, dxf, value)                                      \
  if (dxf && !bit_empty_T (dat, value))                                       \
    {                                                                         \
      HEADER_9 (nam);                                                         \
      if (IS_FROM_TU (dat))                                                   \
        VALUE_T (value, dxf)                                                  \
      else                                                                    \
        VALUE_TV ((char *)value, dxf)                                         \
    }
#define POINT_3D(nam, var, c1, c2, c3)                                        \
  {                                                                           \
    VALUE_RD (dwg->var.x, c1);                                                \
    VALUE_RD (dwg->var.y, c2);                                                \
    VALUE_RD (dwg->var.z, c3);                                                \
  }
#define POINT_2D(nam, var, c1, c2)                                            \
  {                                                                           \
    VALUE_RD (dwg->var.x, c1);                                                \
    VALUE_RD (dwg->var.x, c2);                                                \
  }
#define HEADER_3D(nam)                                                        \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    POINT_3D (nam, header_vars.nam, 10, 20, 30);                              \
  }
#define HEADER_2D(nam)                                                        \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    POINT_2D (nam, header_vars.nam, 10, 20);                                  \
  }
#define SECTION(token)                                                        \
  VALUE_TV ("SECTION", 0);                                                    \
  VALUE_TV (#token, 2)
#define ENDSEC() VALUE_TV ("ENDSEC", 0)
#define TABLE(token)                                                          \
  VALUE_TV ("TABLE", 0);                                                      \
  VALUE_TV (#token, 2)
#define ENDTAB() VALUE_TV ("ENDTAB", 0)
#define RECORD(token) VALUE_TV (#token, 0)
#define SUBCLASS(text)                                                        \
  if (dat->version >= R_13b1)                                                 \
    {                                                                         \
      VALUE_TV (#text, 100)                                                   \
    }

/*
#define VALUE(code, value) \
  { \
    GCC30_DIAG_IGNORE (-Wformat-nonliteral) \
    snprintf (buf, 4096, "%3i\n%s\n", code, dxfb_format (code)); \
    fprintf(dat->fh, buf, value); \
    GCC_DIAG_RESTORE \
  }
 */

#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf)                         \
  FIELD_HANDLE (nam, handle_code, dxf)

#define FIELD_B(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_BB(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_3B(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_BS(nam, dxf) FIELD_RS (nam, dxf)
#define FIELD_BSd(nam, dxf) FIELD_RS (nam, dxf)
#define FIELD_BL(nam, dxf) FIELD_RL (nam, dxf)
#define HEADER_BLL(nam, dxf) HEADER_RLL (nam, dxf)
#define FIELD_BD(nam, dxf) FIELD_RD (nam, dxf)

#define HEADER_9(nam)                                                         \
  {                                                                           \
    GROUP (9);                                                                \
    fprintf (dat->fh, "$%s%c", #nam, 0);                                      \
  }
#define VALUE(value, type, dxf) VALUE_##type (value, dxf)
#define VALUE_B(value, dxf) VALUE_INT (value, dxf)
#define VALUE_BB(value, dxf) VALUE_INT (value, dxf)
#define VALUE_3B(value, dxf) VALUE_INT (value, dxf)
#define VALUE_RCs(value, dxf) VALUE_INT (value, dxf)
#define VALUE_BS(value, dxf) VALUE_INT (value, dxf)
#define VALUE_BL(value, dxf) VALUE_INT (value, dxf)
#define VALUE_BD(value, dxf) VALUE_RD (value, dxf)
#define VALUE_RC(value, dxf)                                                  \
  {                                                                           \
    BITCODE_RC _c = (BITCODE_RC)(value);                                      \
    GROUP (dxf);                                                              \
    fwrite (&_c, 1, 1, dat->fh);                                              \
  }
#define VALUE_RS(value, dxf)                                                  \
  {                                                                           \
    BITCODE_RS _s = (BITCODE_RS)(value);                                      \
    GROUP (dxf);                                                              \
    fwrite (&_s, 2, 1, dat->fh);                                              \
  }
#define VALUE_RSd(value, dxf)                                                 \
  {                                                                           \
    BITCODE_RSd _s = (BITCODE_RSd)(value);                                    \
    GROUP (dxf);                                                              \
    fwrite (&_s, 2, 1, dat->fh);                                              \
  }
#define VALUE_RL(value, dxf)                                                  \
  {                                                                           \
    BITCODE_RL _s = (BITCODE_RL)value;                                        \
    GROUP (dxf);                                                              \
    fwrite (&_s, 4, 1, dat->fh);                                              \
  }
#define VALUE_RLL(value, dxf)                                                 \
  {                                                                           \
    BITCODE_RLL _s = (BITCODE_RLL)value;                                      \
    GROUP (dxf);                                                              \
    fwrite (&_s, 8, 1, dat->fh);                                              \
  }
// most DXFB FIELD_RC are written as int16 actually
// we need to check dwg_resbuf_value_type()
#define FIELD_RC(nam, dxf) VALUE_INT (_obj->nam, dxf)
#define VALUE_INT(value, dxf)                                                 \
  {                                                                           \
    if (dxf == 0)                                                             \
      VALUE_RS (value, dxf)                                                   \
    else                                                                      \
      switch (dwg_resbuf_value_type (dxf))                                    \
        {                                                                     \
        case DWG_VT_BOOL:                                                     \
        case DWG_VT_INT8:                                                     \
          VALUE_RC (value, dxf);                                              \
          break;                                                              \
        case DWG_VT_INT16:                                                    \
          VALUE_RS (value, dxf);                                              \
          break;                                                              \
        case DWG_VT_INT32:                                                    \
          VALUE_RL (value, dxf);                                              \
          break;                                                              \
        case DWG_VT_INT64:                                                    \
          VALUE_RLL (value, dxf);                                             \
          break;                                                              \
        case DWG_VT_STRING:                                                   \
        case DWG_VT_POINT3D:                                                  \
        case DWG_VT_REAL:                                                     \
        case DWG_VT_BINARY:                                                   \
        case DWG_VT_HANDLE:                                                   \
        case DWG_VT_OBJECTID:                                                 \
        case DWG_VT_INVALID:                                                  \
        default:                                                              \
          LOG_ERROR ("Unhandled VALUE_INT code %d", dxf);                     \
        }                                                                     \
  }

#define VALUE_3BD(value, dxf)                                                 \
  {                                                                           \
    VALUE_RD (value.x, dxf);                                                  \
    VALUE_RD (value.y, dxf + 10);                                             \
    VALUE_RD (value.z, dxf + 20);                                             \
  }
#define HEADER_RC(nam, dxf)                                                   \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_INT (dwg->header_vars.nam, dxf);                                    \
  }
#define HEADER_RC0(nam, dxf)                                                  \
  if (dwg->header_vars.nam)                                                   \
    {                                                                         \
      HEADER_9 (nam);                                                         \
      VALUE_INT (dwg->header_vars.nam, dxf);                                  \
    }
#define HEADER_RS0(nam, dxf)                                                  \
  if (dwg->header_vars.nam)                                                   \
    {                                                                         \
      HEADER_9 (nam);                                                         \
      VALUE_RS (dwg->header_vars.nam, dxf);                                   \
    }
#define HEADER_B(nam, dxf) HEADER_RC (nam, dxf)
#define FIELD_RS(nam, dxf) VALUE_INT (_obj->nam, dxf)
#define HEADER_RS(nam, dxf)                                                   \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_RS (dwg->header_vars.nam, dxf);                                     \
  }
#define HEADER_RSd(nam, dxf)                                                  \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_RSd (dwg->header_vars.nam, dxf);                                    \
  }
#define VALUE_RD(value, dxf)                                                  \
  {                                                                           \
    double d = (value);                                                       \
    GROUP (dxf);                                                              \
    fwrite (&d, 8, 1, dat->fh);                                               \
  }
#define FIELD_RD(nam, dxf) VALUE_RD (_obj->nam, dxf)
#define HEADER_RD(nam, dxf)                                                   \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_RD (dwg->header_vars.nam, dxf);                                     \
  }

#define FIELD_RL(nam, dxf) VALUE_INT (_obj->nam, dxf)
#define HEADER_RL(nam, dxf)                                                   \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_RL (dwg->header_vars.nam, dxf);                                     \
  }

#define HEADER_B(nam, dxf) HEADER_RC (nam, dxf)
#define HEADER_BS(nam, dxf) HEADER_RS (nam, dxf)
#define HEADER_BSd(nam, dxf) HEADER_RS (nam, dxf)
#define HEADER_BD(nam, dxf) HEADER_RD (nam, dxf)
#define HEADER_BL(nam, dxf) HEADER_RL (nam, dxf)
#define HEADER_BLd(nam, dxf) HEADER_RL (nam, dxf)

#define FIELD_DATAHANDLE(nam, code, dxf)                                      \
  {                                                                           \
    Dwg_Object_Ref *ref = _obj->nam;                                          \
    HEADER_9 (nam);                                                           \
    VALUE_H (ref ? ref->handleref.value : 0UL, dxf);                          \
  }
// I would rather assume 8-byte LE
#define VALUE_H(value, dxf)                                                   \
  if (dxf)                                                                    \
    {                                                                         \
      char _s[18];                                                            \
      snprintf (_s, sizeof (_s), FMT_H, (BITCODE_RLL)(value));                \
      _s[sizeof (_s) - 1] = '\0';                                             \
      VALUE_TV (_s, dxf);                                                     \
    }
#define HEADER_H(nam, dxf)                                                    \
  if (dxf)                                                                    \
    {                                                                         \
      HEADER_9 (nam);                                                         \
      VALUE_HANDLE (dwg->header_vars.nam, nam, 0, dxf);                       \
    }
#define HEADER_H0(nam, dxf)                                                   \
  if (dxf && dwg->header_vars.nam && dwg->header_vars.nam->absolute_ref)      \
    {                                                                         \
      HEADER_9 (nam);                                                         \
      VALUE_H (dwg->header_vars.nam->absolute_ref, dxf);                      \
    }

#define HANDLE_NAME(nam, code, table)                                         \
  VALUE_HANDLE_NAME (dwg->header_vars.nam, dxf, table)
// TODO: convert STANDARD to Standard?
#define VALUE_HANDLE_NAME(value, dxf, table)                                  \
  {                                                                           \
    Dwg_Object_Ref *ref = value;                                              \
    if (ref && obj && obj->parent                                             \
        && (!ref->obj || ref->obj->supertype != DWG_SUPERTYPE_OBJECT          \
            || ref->obj->fixedtype != DWG_TYPE_##table))                      \
      ref->obj = dwg_resolve_handle (obj->parent, ref->absolute_ref);         \
    if (ref && ref->obj && ref->obj->supertype == DWG_SUPERTYPE_OBJECT        \
        && ref->obj->fixedtype == DWG_TYPE_##table)                           \
      {                                                                       \
        VALUE_TV (ref->obj->tio.object->tio.table->name, dxf)                 \
      }                                                                       \
    else                                                                      \
      VALUE_TV ("", dxf)                                                      \
  }
#define FIELD_HANDLE_NAME(nam, dxf, table)                                    \
  VALUE_HANDLE_NAME (_obj->nam, dxf, table)
#define SUB_FIELD_HANDLE_NAME(ob, nam, dxf, table)                            \
  VALUE_HANDLE_NAME (_obj->ob.nam, dxf, table)
#define HEADER_HANDLE_NAME(nam, dxf, table)                                   \
  HEADER_9 (nam);                                                             \
  VALUE_HANDLE_NAME (dwg->header_vars.nam, dxf, table)

#define FIELD_BLL(nam, dxf)                                                   \
  {                                                                           \
    BITCODE_BLL s = _obj->nam;                                                \
    GROUP (dxf);                                                              \
    fwrite (&s, 8, 1, dat->fh);                                               \
  }
#define FIELD_RLL(nam, dxf) FIELD_BLL (nam, dxf)
#define FIELD_HV(nam, dxf) FIELD_BLL (nam, dxf)
#define HEADER_RLL(nam, dxf)                                                  \
  {                                                                           \
    GROUP (9);                                                                \
    fprintf (dat->fh, "$%s%c", #nam, 0);                                      \
    FIELD_BLL (nam, dxf);                                                     \
  }

#define FIELD_MC(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_MS(nam, dxf) FIELD_RS (nam, dxf)
#define FIELD_BT(nam, dxf) FIELD_BD (nam, dxf);
#define FIELD_4BITS(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_BE(nam, dxf)                                                    \
  {                                                                           \
    if (dxf                                                                   \
        && !(_obj->nam.x == 0.0 && _obj->nam.y == 0.0 && _obj->nam.z == 1.0)) \
      FIELD_3RD (nam, dxf)                                                    \
  }
#define FIELD_DD(nam, _default, dxf) FIELD_RD (nam, dxf)
#define FIELD_2DD(nam, def, dxf)                                              \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                             \
      FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                        \
    }
#define FIELD_3DD(nam, def, dxf)                                              \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_DD (nam.x, FIELD_VALUE (def.x), dxf);                             \
      FIELD_DD (nam.y, FIELD_VALUE (def.y), dxf + 10);                        \
      FIELD_DD (nam.z, FIELD_VALUE (def.z), dxf + 20);                        \
    }
#define FIELD_2RD(nam, dxf)                                                   \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_RD (nam.x, dxf);                                                  \
      FIELD_RD (nam.y, dxf + 10);                                             \
    }
#define FIELD_2RD0(nam, dxf)                                                  \
  if (dxf && _obj->nam.x != 0.0 && _obj->nam.y != 0.0)                        \
    {                                                                         \
      FIELD_RD (nam.x, dxf);                                                  \
      FIELD_RD (nam.y, dxf + 10);                                             \
    }
#define FIELD_2BD(nam, dxf) FIELD_2RD (nam, dxf)
#define FIELD_2BD_1(nam, dxf)                                                 \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_RD (nam.x, dxf);                                                  \
      FIELD_RD (nam.y, dxf + 1);                                              \
    }
#define FIELD_3RD(nam, dxf)                                                   \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_RD (nam.x, dxf);                                                  \
      FIELD_RD (nam.y, dxf + 10);                                             \
      FIELD_RD (nam.z, dxf + 20);                                             \
    }
#define FIELD_3BD(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3BD_1(nam, dxf)                                                 \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_RD (nam.x, dxf);                                                  \
      FIELD_RD (nam.y, dxf + 1);                                              \
      FIELD_RD (nam.z, dxf + 2);                                              \
    }
#define FIELD_3DPOINT(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_CMC(color, dxf)                                                 \
  {                                                                           \
    if (dat->version < R_2004)                                                \
      {                                                                       \
        if (dat->from_version >= R_2004)                                      \
          bit_downconvert_CMC (dat, (Dwg_Color *)&_obj->color);               \
        VALUE_RS (_obj->color.index, dxf);                                    \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (dat->from_version < R_2004)                                       \
          bit_upconvert_CMC (dat, (Dwg_Color *)&_obj->color);                 \
        if (dxf >= 90)                                                        \
          {                                                                   \
            VALUE_RL (_obj->color.rgb, dxf);                                  \
          }                                                                   \
        else if (_obj->color.method == 0xc3)                                  \
          {                                                                   \
            VALUE_RL (_obj->color.rgb & 0x00ffffff, dxf);                     \
          }                                                                   \
        else if (_obj->color.method == 0xc8)                                  \
          {                                                                   \
            VALUE_RS (257, dxf);                                              \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            VALUE_RS (_obj->color.index, dxf);                                \
            if (_obj->color.method == 0xc2)                                   \
              VALUE_RL (_obj->color.rgb, (unsigned)(dxf + 420 - 62));         \
          }                                                                   \
      }                                                                       \
  }
#define FIELD_CMC0(color, dxf)                                                \
  {                                                                           \
    if (dat->version < R_2004)                                                \
      {                                                                       \
        if (dat->from_version >= R_2004)                                      \
          bit_downconvert_CMC (dat, (Dwg_Color *)&_obj->color);               \
        if (_obj->color.index)                                                \
          VALUE_RS (_obj->color.index, dxf);                                  \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (dat->from_version < R_2004)                                       \
          bit_upconvert_CMC (dat, (Dwg_Color *)&_obj->color);                 \
        if (dxf >= 90)                                                        \
          {                                                                   \
            VALUE_RL (_obj->color.rgb, dxf);                                  \
          }                                                                   \
        else if (_obj->color.method == 0xc3)                                  \
          {                                                                   \
            VALUE_RL (_obj->color.rgb & 0x00ffffff, dxf);                     \
          }                                                                   \
        else if (_obj->color.method == 0xc8)                                  \
          {                                                                   \
            VALUE_RS (257, dxf);                                              \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            if (_obj->color.index)                                            \
              VALUE_RS (_obj->color.index, dxf);                              \
            if (_obj->color.method == 0xc2)                                   \
              VALUE_RL (_obj->color.rgb, (unsigned)(dxf + 420 - 62));         \
          }                                                                   \
      }                                                                       \
  }
#define SUB_FIELD_CMC(o, color, dxf)                                          \
  {                                                                           \
    if (dat->version < R_2004)                                                \
      {                                                                       \
        if (dat->from_version >= R_2004)                                      \
          bit_downconvert_CMC (dat, (Dwg_Color *)&_obj->o.color);             \
        VALUE_RS (_obj->o.color.index, dxf);                                  \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if (dat->from_version < R_2004)                                       \
          bit_upconvert_CMC (dat, (Dwg_Color *)&_obj->o.color);               \
        if (dxf >= 90)                                                        \
          {                                                                   \
            VALUE_RL (_obj->o.color.rgb, dxf);                                \
          }                                                                   \
        else if (_obj->o.color.method == 0xc8)                                \
          {                                                                   \
            VALUE_RS (257, dxf);                                              \
          }                                                                   \
        else if (_obj->o.color.method == 0xc3)                                \
          {                                                                   \
            VALUE_RL (_obj->o.color.rgb & 0x00ffffff, dxf);                   \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            VALUE_RS (_obj->o.color.index, dxf);                              \
            if (_obj->o.color.method == 0xc2)                                 \
              VALUE_RL (_obj->o.color.rgb, (unsigned)(dxf + 420 - 62));       \
          }                                                                   \
      }                                                                       \
  }
#define HEADER_CMC(nam, dxf)                                                  \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    VALUE_RS (dwg->header_vars.nam.index, dxf);                               \
  }
#define HEADER_TIMEBLL(nam, dxf)                                              \
  {                                                                           \
    HEADER_9 (nam);                                                           \
    FIELD_TIMEBLL (nam, dxf);                                                 \
  }
#define FIELD_TIMEBLL(nam, dxf) VALUE_RD (_obj->nam.value, dxf)

// FIELD_VECTOR_N(nam, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'nam'.
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  if (dxf && _obj->nam)                                                       \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        VALUE (_obj->nam[vcount], type, dxf);                                 \
    }
#define FIELD_VECTOR_T(nam, type, size, dxf)                                  \
  if (dxf && _obj->nam)                                                       \
    {                                                                         \
      if (!IS_FROM_TU (dat))                                                  \
        {                                                                     \
          for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)         \
            VALUE_TV (_obj->nam[vcount], dxf)                                 \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)         \
            VALUE_TU (_obj->nam[vcount], dxf)                                 \
        }                                                                     \
    }

#define FIELD_VECTOR(nam, type, size, dxf)                                    \
  FIELD_VECTOR_N (nam, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  if (dxf && _obj->nam)                                                       \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2RD (nam[vcount], dxf);                                       \
        }                                                                     \
    }

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  FIELD_2RD (nam[0], dxf);                                                    \
  if (dxf && _obj->nam)                                                       \
    {                                                                         \
      for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2DD (nam[vcount], nam[vcount - 1], dxf);                      \
        }                                                                     \
    }

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  if (dxf && _obj->nam)                                                       \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_3DPOINT (nam[vcount], dxf);                                   \
        }                                                                     \
    }

#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  if (dxf && _obj->nam)                                                       \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIELD_HANDLE_N (nam[vcount], vcount, code, dxf);                    \
        }                                                                     \
    }

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  FIELDG (num_inserts, type, dxf)

#define FIELD_XDATA(nam, size)                                                \
  dxfb_write_xdata (dat, obj, _obj->nam, _obj->size)

#define _XDICOBJHANDLE(code)                                                  \
  if (dat->version >= R_13b1 && obj->tio.object->xdicobjhandle                \
      && obj->tio.object->xdicobjhandle->absolute_ref)                        \
    {                                                                         \
      VALUE_TV ("{ACAD_XDICTIONARY", 102);                                    \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
      VALUE_TV ("}", 102);                                                    \
    }
#define _REACTORS(code)                                                       \
  if (dat->version >= R_13b1 && obj->tio.object->num_reactors                 \
      && obj->tio.object->reactors)                                           \
    {                                                                         \
      VALUE_TV ("{ACAD_REACTORS", 102)                                        \
      for (vcount = 0; vcount < obj->tio.object->num_reactors; vcount++)      \
        {                                                                     \
          VALUE_HANDLE (obj->tio.object->reactors[vcount], reactors, code,    \
                        330);                                                 \
        }                                                                     \
      VALUE_TV ("}", 102)                                                     \
    }
#define ENT_REACTORS(code)                                                    \
  if (dat->version >= R_13b1 && _obj->num_reactors && _obj->reactors)         \
    {                                                                         \
      VALUE_TV ("{ACAD_REACTORS", 102)                                        \
      for (vcount = 0; vcount < _obj->num_reactors; vcount++)                 \
        {                                                                     \
          VALUE_HANDLE (_obj->reactors[vcount], reactors, code, 330);         \
        }                                                                     \
      VALUE_TV ("}", 102)                                                     \
    }

#define REACTORS(code)
#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code)                                               \
  if (dat->version >= R_13b1 && obj->tio.entity->xdicobjhandle                \
      && obj->tio.entity->xdicobjhandle->absolute_ref)                        \
    {                                                                         \
      VALUE_TV ("{ACAD_XDICTIONARY", 102);                                    \
      VALUE_HANDLE (obj->tio.entity->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
      VALUE_TV ("}", 102);                                                    \
    }
#define BLOCK_NAME(nam, dxf) dxfb_cvt_blockname (dat, _obj->nam, dxf)

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#ifndef DEBUG_CLASSES
static int
dwg_dxfb_TABLECONTENT (Bit_Chain *restrict dat, const Dwg_Object *restrict obj)
{
  (void)dat;
  (void)obj;
  return 0;
}
#else
static int dwg_dxfb_TABLECONTENT (Bit_Chain *restrict dat,
                                  const Dwg_Object *restrict obj);
#endif

#define DWG_ENTITY(token)                                                     \
  static int dwg_dxfb_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         const Dwg_Object *restrict obj);     \
  static int dwg_dxfb_##token (Bit_Chain *restrict dat,                       \
                               const Dwg_Object *restrict obj)                \
  {                                                                           \
    int error = 0;                                                            \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    if (obj->fixedtype != DWG_TYPE_##token)                                   \
      {                                                                       \
        LOG_ERROR ("Invalid type 0x%x, expected 0x%x %s", obj->fixedtype,     \
                   DWG_TYPE_##token, #token);                                 \
        return DWG_ERR_INVALIDTYPE;                                           \
      }                                                                       \
    if (strEQc (#token, "GEOPOSITIONMARKER"))                                 \
      RECORD (POSITIONMARKER)                                                 \
    else if (dat->version < R_13b1 && strlen (#token) == 10                   \
             && strEQc (#token, "LWPOLYLINE"))                                \
      RECORD (POLYLINE)                                                       \
    else if (strlen (#token) > 10 && !memcmp (#token, "DIMENSION_", 10))      \
      RECORD (DIMENSION)                                                      \
    else if (strlen (#token) > 9 && !memcmp (#token, "POLYLINE_", 9))         \
      RECORD (POLYLINE)                                                       \
    else if (strlen (#token) > 7 && !memcmp (#token, "VERTEX_", 7))           \
      RECORD (VERTEX)                                                         \
    else if (strEQc (#token, "MINSERT"))                                      \
      RECORD (INSERT)                                                         \
    else if (dat->version >= R_2010 && strEQc (#token, "TABLE"))              \
      {                                                                       \
        RECORD (ACAD_TABLE);                                                  \
        return dwg_dxfb_TABLECONTENT (dat, obj);                              \
      }                                                                       \
    else if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))               \
      VALUE_TV (obj->dxfname, 0)                                              \
    else if (obj->type >= 498 && obj->dxfname)                                \
      VALUE_TV (obj->dxfname, 0)                                              \
    else                                                                      \
      RECORD (token)                                                          \
    LOG_INFO ("Entity " #token ":\n")                                         \
    SINCE (R_11)                                                              \
    {                                                                         \
      LOG_TRACE ("Entity handle: " FORMAT_H "\n", ARGS_H (obj->handle))       \
      VALUE_H (obj->handle.value, 5);                                         \
    }                                                                         \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      VALUE_HANDLE_NAME (obj->parent->header_vars.BLOCK_RECORD_MSPACE, 330,   \
                         BLOCK_HEADER);                                       \
    }                                                                         \
    error |= dxfb_common_entity_handle_data (dat, obj);                       \
    error |= dwg_dxfb_##token##_private (dat, hdl_dat, str_dat, obj);         \
    error |= dxfb_write_eed (dat, obj->tio.object);                           \
    return error;                                                             \
  }                                                                           \
  static int dwg_dxfb_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         const Dwg_Object *restrict obj)      \
  {                                                                           \
    int error = 0;                                                            \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Entity_##token *_obj = obj->tio.entity->tio.token;                    \
    Dwg_Object_Entity *_ent = obj->tio.entity;

#define DWG_ENTITY_END                                                        \
  return error;                                                               \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_dxfb_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         const Dwg_Object *restrict obj);     \
  static int dwg_dxfb_##token (Bit_Chain *restrict dat,                       \
                               const Dwg_Object *restrict obj)                \
  {                                                                           \
    BITCODE_BL vcount;                                                        \
    int error = 0;                                                            \
    Bit_Chain *str_dat = dat;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    LOG_INFO ("Object " #token ":\n")                                         \
    if (obj->fixedtype != DWG_TYPE_##token)                                   \
      {                                                                       \
        LOG_ERROR ("Invalid type 0x%x, expected 0x%x %s", obj->fixedtype,     \
                   DWG_TYPE_##token, #token);                                 \
        return DWG_ERR_INVALIDTYPE;                                           \
      }                                                                       \
    PRE (R_14)                                                                \
    {                                                                         \
      if (obj->fixedtype == DWG_TYPE_PLACEHOLDER)                             \
        return 0;                                                             \
    }                                                                         \
    if (!dwg_obj_is_control (obj))                                            \
      {                                                                       \
        if (obj->fixedtype == DWG_TYPE_TABLE)                                 \
          ;                                                                   \
        else if (obj->type >= 500 && obj->dxfname)                            \
          VALUE_TV (obj->dxfname, 0)                                          \
        else if (obj->type == DWG_TYPE_PLACEHOLDER)                           \
          RECORD (ACDBPLACEHOLDER)                                            \
        else if (obj->fixedtype == DWG_TYPE_PROXY_OBJECT)                     \
          RECORD (ACAD_PROXY_OBJECT)                                          \
        else if (obj->type != DWG_TYPE_BLOCK_HEADER)                          \
          RECORD (token)                                                      \
        SINCE (R_13b1)                                                        \
        {                                                                     \
          const int dxf = obj->type == DWG_TYPE_DIMSTYLE ? 105 : 5;           \
          VALUE_H (obj->handle.value, dxf);                                   \
          _XDICOBJHANDLE (3);                                                 \
          _REACTORS (4);                                                      \
        }                                                                     \
        SINCE (R_14)                                                          \
        {                                                                     \
          VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 3, 330);   \
        }                                                                     \
      }                                                                       \
    if (DWG_LOGLEVEL >= DWG_LOGLEVEL_TRACE)                                   \
      {                                                                       \
        if (dwg_obj_is_table (obj))                                           \
          {                                                                   \
            char *_name = dwg_obj_table_get_name (obj, &error);               \
            LOG_TRACE ("Object handle: " FORMAT_H ", name: %s\n",             \
                       ARGS_H (obj->handle), _name);                          \
            if (IS_FROM_TU (dat))                                             \
              free (_name);                                                   \
          }                                                                   \
        else                                                                  \
          LOG_TRACE ("Object handle: " FORMAT_H "\n", ARGS_H (obj->handle))   \
      }                                                                       \
    error |= dwg_dxfb_##token##_private (dat, hdl_dat, str_dat, obj);         \
    error |= dxfb_write_eed (dat, obj->tio.object);                           \
    return error;                                                             \
  }                                                                           \
  static int dwg_dxfb_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         const Dwg_Object *restrict obj)      \
  {                                                                           \
    int error = 0;                                                            \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_##token *_obj = obj->tio.object->tio.token;

// then 330, SUBCLASS

#define DWG_OBJECT_END                                                        \
  return error;                                                               \
  }

static int
dxfb_write_eed (Bit_Chain *restrict dat, const Dwg_Object_Object *restrict obj)
{
  int error = 0;
  Dwg_Data *dwg = obj->dwg;
  for (BITCODE_BL i = 0; i < obj->num_eed; i++)
    {
      const Dwg_Eed *_obj = &obj->eed[i];
      if (_obj->size)
        {
          // name of APPID
          Dwg_Object *appid = dwg_resolve_handle (dwg, _obj->handle.value);
          if (appid && appid->fixedtype == DWG_TYPE_APPID)
            VALUE_T (appid->tio.object->tio.APPID->name, 1001)
          else
            VALUE_TFF ("ACAD", 1001);
        }
      if (_obj->data)
        {
          const Dwg_Eed_Data *data = _obj->data;
          const int dxf = data->code + 1000;
          switch (data->code)
            {
            case 0:
              if (IS_FROM_TU (dat))
                VALUE_TU (data->u.eed_0_r2007.string, 1000)
              else
                VALUE_TV (data->u.eed_0.string, 1000)
              break;
            case 2:
              if (data->u.eed_2.close)
                VALUE_TFF ("}", 1002)
              else
                VALUE_TFF ("{", 1002)
              break;
            case 3:
              GROUP (dxf);
              fprintf (dat->fh, "%9lu\r\n",
                       (unsigned long)data->u.eed_3.layer);
              // VALUE_RLL (data->u.eed_3.layer, dxf);
              break;
            case 4:
              VALUE_BINARY (data->u.eed_4.data, data->u.eed_4.length, dxf);
              break;
            case 5:
              VALUE_H (data->u.eed_5.entity, dxf);
              break; // not in DXF
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
              VALUE_3BD (data->u.eed_10.point, dxf);
              break;
            case 40:
            case 41:
            case 42:
              VALUE_RD (data->u.eed_40.real, dxf);
              break;
            case 70:
              VALUE_RS (data->u.eed_70.rs, dxf);
              break;
            case 71:
              VALUE_RL (data->u.eed_71.rl, dxf);
              break;
            default:
              VALUE_RC (0, dxf);
            }
        }
    }
  return error;
}

static void
dxfb_write_xdata (Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
                  Dwg_Resbuf *restrict rbuf, BITCODE_BL size)
{
  Dwg_Resbuf *tmp;
  int i;

  while (rbuf)
    {
      // const char* fmt = dxf_format(rbuf->type);
      short type = dwg_resbuf_value_type (rbuf->type);
      int dxftype = (rbuf->type > 1000 || obj->fixedtype == DWG_TYPE_XRECORD)
                        ? rbuf->type
                        : rbuf->type + 1000;

      tmp = rbuf->nextrb;
      switch (type)
        {
        case DWG_VT_STRING:
          if (IS_FROM_TU (dat))
            {
              VALUE_TU (rbuf->value.str.u.wdata, dxftype);
            }
          else
            {
              VALUE_TV (rbuf->value.str.u.data, dxftype);
            }
          break;
        case DWG_VT_REAL:
          VALUE_RD (rbuf->value.dbl, dxftype);
          break;
        case DWG_VT_BOOL:
        case DWG_VT_INT8:
          VALUE_RC (rbuf->value.i8, dxftype);
          break;
        case DWG_VT_INT16:
          VALUE_RS (rbuf->value.i16, dxftype);
          break;
        case DWG_VT_INT32:
          VALUE_RL (rbuf->value.i32, dxftype);
          break;
        case DWG_VT_INT64:
          VALUE_RLL (rbuf->value.i64, dxftype);
          break;
        case DWG_VT_POINT3D:
          VALUE_RD (rbuf->value.pt[0], dxftype);
          VALUE_RD (rbuf->value.pt[1], dxftype + 10);
          VALUE_RD (rbuf->value.pt[2], dxftype + 20);
          break;
        case DWG_VT_BINARY:
          VALUE_BINARY (rbuf->value.str.u.data, rbuf->value.str.size, dxftype);
          break;
        case DWG_VT_HANDLE:
        case DWG_VT_OBJECTID:
          VALUE_H (rbuf->value.absref, dxftype);
          break;
        case DWG_VT_INVALID:
        default:
          // fprintf(dat->fh, "");
          break;
        }
      rbuf = tmp;
    }
}

#undef DXF_3DSOLID
#define DXF_3DSOLID dxfb_3dsolid (dat, obj, (Dwg_Entity_3DSOLID *)_obj);

// r13+ converts STANDARD to Standard, BYLAYER to ByLayer, BYBLOCK to ByBlock
static void
dxfb_cvt_tablerecord (Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
                      char *restrict name, const int dxf)
{
  if (obj && obj->supertype == DWG_SUPERTYPE_OBJECT && name)
    {
      if (IS_FROM_TU (dat))
        {
          name = bit_convert_TU ((BITCODE_TU)name);
        }
      if (dat->from_version >= R_13b1 && dat->version < R_13b1)
        { // convert the other way round, from newer to older
          if (strEQc (name, "Standard"))
            VALUE_TV ("STANDARD", dxf)
          else if (strEQc (name, "ByLayer"))
            VALUE_TV ("BYLAYER", dxf)
          else if (strEQc (name, "ByBlock"))
            VALUE_TV ("BYBLOCK", dxf)
          else if (strEQc (name, "*Active"))
            VALUE_TV ("*ACTIVE", dxf)
          else
            VALUE_TV (name, dxf)
        }
      else
        { // convert some standard names
          if (dat->version >= R_13b1 && strEQc (name, "STANDARD"))
            VALUE_TV ("Standard", dxf)
          else if (dat->version >= R_13b1 && strEQc (name, "BYLAYER"))
            VALUE_TV ("ByLayer", dxf)
          else if (dat->version >= R_13b1 && strEQc (name, "BYBLOCK"))
            VALUE_TV ("ByBlock", dxf)
          else if (dat->version >= R_13b1 && strEQc (name, "*ACTIVE"))
            VALUE_TV ("*Active", dxf)
          else
            VALUE_TV (name, dxf)
        }
      if (IS_FROM_TU (dat))
        free (name);
    }
  else
    {
      VALUE_TV ("", dxf)
    }
}

/* pre-r13 mspace and pspace blocks have different names:
 *Model_Space => $MODEL_SPACE
 *Paper_Space => $PAPER_SPACE
 */
static void
dxfb_cvt_blockname (Bit_Chain *restrict dat, char *restrict name,
                    const int dxf)
{
  if (!name)
    {
      VALUE_TV ("", dxf)
      return;
    }
  if (IS_FROM_TU (dat)) // r2007+ unicode names
    {
      name = bit_convert_TU ((BITCODE_TU)name);
    }
  if (dat->version == dat->from_version) // no conversion
    {
      VALUE_TV (name, dxf)
    }
  else if (dat->version < R_13b1 && dat->from_version >= R_13b1) // to older
    {
      if (strlen (name) < 10)
        VALUE_TV (name, dxf)
      else if (strEQc (name, "*Model_Space"))
        VALUE_TV ("$MODEL_SPACE", dxf)
      else if (strEQc (name, "*Paper_Space"))
        VALUE_TV ("$PAPER_SPACE", dxf)
      else if (!memcmp (name, "*Paper_Space", sizeof ("*Paper_Space") - 1))
        {
          GROUP (dxf);
          fprintf (dat->fh, "$PAPER_SPACE%s%c", &name[12], 0);
        }
      else
        VALUE_TV (name, dxf)
    }
  else if (dat->version >= R_13b1 && dat->from_version < R_13b1) // to newer
    {
      if (strlen (name) < 10)
        VALUE_TV (name, dxf)
      else if (strEQc (name, "$MODEL_SPACE"))
        VALUE_TV ("*Model_Space", dxf)
      else if (strEQc (name, "$PAPER_SPACE"))
        VALUE_TV ("*Paper_Space", dxf)
      else if (!memcmp (name, "$PAPER_SPACE", sizeof ("$PAPER_SPACE") - 1))
        {
          GROUP (dxf);
          fprintf (dat->fh, "*Paper_Space%s%c", &name[12], 0);
        }
      else
        VALUE_TV (name, dxf)
    }
  if (IS_FROM_TU (dat))
    free (name);
}

#define START_OBJECT_HANDLE_STREAM

// Handle 5 written here first
#define COMMON_TABLE_CONTROL_FLAGS                                            \
  if (ctrl)                                                                   \
    {                                                                         \
      SINCE (R_13b1)                                                          \
      {                                                                       \
        BITCODE_BL vcount;                                                    \
        VALUE_H (ctrl->handle.value, 5);                                      \
        _XDICOBJHANDLE (3);                                                   \
        _REACTORS (4);                                                        \
      }                                                                       \
      SINCE (R_14)                                                            \
      {                                                                       \
        VALUE_HANDLE (ctrl->tio.object->ownerhandle, ownerhandle, 3, 330);    \
      }                                                                       \
    }                                                                         \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    VALUE_TV ("AcDbSymbolTable", 100);                                        \
  }

#define COMMON_TABLE_FLAGS(acdbname)                                          \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    VALUE_TV ("AcDbSymbolTableRecord", 100)                                   \
    VALUE_TV ("AcDb" #acdbname "TableRecord", 100)                            \
  }                                                                           \
  if (strEQc (#acdbname, "Block") && dat->version >= R_13b1)                  \
    {                                                                         \
      Dwg_Object *blk = dwg_ref_object (                                      \
          dwg, ((Dwg_Object_BLOCK_HEADER *)_obj)->block_entity);              \
      if (blk && blk->type == DWG_TYPE_BLOCK)                                 \
        {                                                                     \
          Dwg_Entity_BLOCK *_blk = blk->tio.entity->tio.BLOCK;                \
          VALUE_T (_blk->name, 2)                                             \
        }                                                                     \
      else if (_obj->name)                                                    \
        {                                                                     \
          VALUE_T (_obj->name, 2)                                             \
        }                                                                     \
      else                                                                    \
        VALUE_TV ("*", 2)                                                     \
    }                                                                         \
  /* Empty name with xref shape names */                                      \
  else if (strEQc (#acdbname, "TextStyle") && _obj->flag & 1                  \
           && dxf_is_xrefdep_name (dat, _obj->name))                          \
    VALUE_TV ("", 2)                                                          \
  else if (_obj->name)                                                        \
    dxfb_cvt_tablerecord (dat, obj, _obj->name, 2);                           \
  else                                                                        \
    VALUE_TV ("*", 2)                                                         \
  if (strEQc (#acdbname, "Layer") && dat->version >= R_2000)                  \
    {                                                                         \
      /* Mask off plotflag and linewt. */                                     \
      BITCODE_RC _flag = _obj->flag & ~0x3e0;                                 \
      /* Don't keep bit 16 when not xrefdep like "XREF|name" */               \
      if (_flag & 0x10 && !dxf_has_xrefdep_vertbar (dat, _obj->name))         \
        _flag &= ~0x10;                                                       \
      VALUE_RC (_flag, 70);                                                   \
    }                                                                         \
  else if (strEQc (#acdbname, "Block") && dat->version >= R_2000)             \
    ; /* skip 70 for AcDbBlockTableRecord here. done in AcDbBlockBegin */     \
  else                                                                        \
    {                                                                         \
      /* mask off 64, the loaded bit 6 */                                     \
      VALUE_RS (_obj->flag & ~64, 70);                                        \
    }

#define LAYER_TABLE_FLAGS(acdbname)                                           \
  SINCE (R_14)                                                                \
  {                                                                           \
    VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 3, 330);         \
  }                                                                           \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    VALUE_TV ("AcDbSymbolTableRecord", 100)                                   \
    VALUE_TV ("AcDb" #acdbname "TableRecord", 100)                            \
  }                                                                           \
  if (_obj->name)                                                             \
    dxfb_cvt_tablerecord (dat, obj, _obj->name, 2);                           \
  FIELD_RS (flag, 70)

#include "dwg.spec"

static int
dxfb_3dsolid (Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
              Dwg_Entity_3DSOLID *restrict _obj)
{
  BITCODE_BL i;
  int error = 0;

  COMMON_ENTITY_HANDLE_DATA;
  SUBCLASS (AcDbModelerGeometry);

  FIELD_B (acis_empty, 0);
  if (!FIELD_VALUE (acis_empty))
    {
      FIELD_B (unknown, 0);
      FIELD_BS (version, 70);
      if (FIELD_VALUE (version) == 1)
        {
          for (i = 0; i < FIELD_VALUE (num_blocks); i++)
            {
              char *s = FIELD_VALUE (encr_sat_data[i]);
              int len = FIELD_VALUE (block_size[i]);
              // DXF 1 + 3 if >255
              while (len > 0)
                {
                  char *n = strchr (s, '\n');
                  int l = len > 255 ? 255 : len & 0xff;
                  if (n && ((long)(n - s) < (long)len))
                    {
                      l = n - s;
                    }
                  if (l)
                    {
                      if (len < 255)
                        VALUE_BINARY (s, l, 1)
                      else
                        VALUE_BINARY (s, l, 3)
                      l++;
                      len -= l;
                      s += l;
                    }
                  else
                    {
                      len--;
                      s++;
                    }
                }
            }
          // LOG_TRACE("acis_data [1]:\n%s\n", FIELD_VALUE (acis_data));
        }
      else // if (FIELD_VALUE(version)==2)
        // must use ASCII out
        {
          LOG_ERROR (
              "ACIS BinaryFile v2 not yet supported. Use ASCII output.");
        }
    }
  return error;
}

/* returns 0 on success
 */
static int
dwg_dxfb_variable_type (const Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                        Dwg_Object *restrict obj)
{
  int i;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;
  if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT
      || obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
    return DWG_ERR_UNHANDLEDCLASS;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  // almost always false
  is_entity = dwg_class_is_entity (klass);
  if (dat->version < R_2000)
    {
      // keep only: IMAGE, LWPOLYLINE, HATCH
      if (is_entity && strNE (klass->dxfname, "IMAGE")
          && strNEc (klass->dxfname, "LWPOLYLINE")
          && strNEc (klass->dxfname, "HATCH"))
        {
          LOG_WARN ("Skip %s\n", klass->dxfname)
          return DWG_ERR_UNHANDLEDCLASS;
        }
      // keep only: DICTIONARYVAR, MATERIAL, RASTERVARIABLES, IMAGEDEF_REACTOR,
      // XRECORD
      else if (!is_entity && strNEc (klass->dxfname, "DICTIONARYVAR")
               && strNEc (klass->dxfname, "MATERIAL")
               && strNEc (klass->dxfname, "RASTERVARIABLES")
               && strNEc (klass->dxfname, "IDBUFFER")
               && strNEc (klass->dxfname, "IMAGEDEF_REACTOR")
               && strNEc (klass->dxfname, "XRECORD"))
        {
          LOG_WARN ("Skip %s\n", klass->dxfname)
          return DWG_ERR_UNHANDLEDCLASS;
        }
    }

    // clang-format off
  #include "classes.inc"
  // clang-format on

  return DWG_ERR_UNHANDLEDCLASS;
}

/* process unsorted vertices */
#define decl_dxfb_process_VERTEX(token)                                       \
  static int dxfb_process_VERTEX_##token (Bit_Chain *restrict dat,            \
                                          const Dwg_Object *restrict obj,     \
                                          int *restrict i)                    \
  {                                                                           \
    int error = 0;                                                            \
    Dwg_Entity_POLYLINE_##token *_obj                                         \
        = obj->tio.entity->tio.POLYLINE_##token;                              \
                                                                              \
    VERSIONS (R_13b1, R_2000)                                                 \
    {                                                                         \
      Dwg_Object *last_vertex                                                 \
          = _obj->last_vertex ? _obj->last_vertex->obj : NULL;                \
      Dwg_Object *o = _obj->first_vertex ? _obj->first_vertex->obj : NULL;    \
      if (!o || !last_vertex)                                                 \
        return DWG_ERR_INVALIDHANDLE;                                         \
      if (o->fixedtype == DWG_TYPE_VERTEX_##token)                            \
        error |= dwg_dxfb_VERTEX_##token (dat, o);                            \
      *i = *i + 1;                                                            \
      do                                                                      \
        {                                                                     \
          o = dwg_next_object (o);                                            \
          if (!o)                                                             \
            return DWG_ERR_INVALIDHANDLE;                                     \
          if (strEQc (#token, "PFACE")                                        \
              && o->fixedtype == DWG_TYPE_VERTEX_PFACE_FACE)                  \
            {                                                                 \
              error |= dwg_dxfb_VERTEX_PFACE_FACE (dat, o);                   \
            }                                                                 \
          else if (o->fixedtype == DWG_TYPE_VERTEX_##token)                   \
            {                                                                 \
              error |= dwg_dxfb_VERTEX_##token (dat, o);                      \
            }                                                                 \
          *i = *i + 1;                                                        \
        }                                                                     \
      while (o->fixedtype != DWG_TYPE_SEQEND && o != last_vertex);            \
      o = _obj->seqend ? _obj->seqend->obj : NULL;                            \
      if (o && o->fixedtype == DWG_TYPE_SEQEND)                               \
        error |= dwg_dxfb_SEQEND (dat, o);                                    \
      *i = *i + 1;                                                            \
    }                                                                         \
    SINCE (R_2004a)                                                           \
    {                                                                         \
      Dwg_Object *o;                                                          \
      for (BITCODE_BL j = 0; j < _obj->num_owned; j++)                        \
        {                                                                     \
          o = _obj->vertex && _obj->vertex[j] ? _obj->vertex[j]->obj : NULL;  \
          if (strEQc (#token, "PFACE") && o                                   \
              && o->fixedtype == DWG_TYPE_VERTEX_PFACE_FACE)                  \
            {                                                                 \
              error |= dwg_dxfb_VERTEX_PFACE_FACE (dat, o);                   \
            }                                                                 \
          else if (o && o->fixedtype == DWG_TYPE_VERTEX_##token)              \
            {                                                                 \
              error |= dwg_dxfb_VERTEX_##token (dat, o);                      \
            }                                                                 \
        }                                                                     \
      o = _obj->seqend ? _obj->seqend->obj : NULL;                            \
      if (o && o->fixedtype == DWG_TYPE_SEQEND)                               \
        error |= dwg_dxfb_SEQEND (dat, o);                                    \
      *i = *i + _obj->num_owned + 1;                                          \
    }                                                                         \
    return error;                                                             \
  }

// clang-format off
decl_dxfb_process_VERTEX (2D)
decl_dxfb_process_VERTEX (3D)
decl_dxfb_process_VERTEX (MESH)
decl_dxfb_process_VERTEX (PFACE)
// clang-format on

/* process seqend before attribs */
#define decl_dxfb_process_INSERT(token)                                       \
  static int dxfb_process_##token (Bit_Chain *restrict dat,                   \
                                   const Dwg_Object *restrict obj,            \
                                   int *restrict i)                           \
  {                                                                           \
    int error = 0;                                                            \
    Dwg_Entity_##token *_obj = obj->tio.entity->tio.token;                    \
    if (!_obj->has_attribs)                                                   \
      return 0;                                                               \
                                                                              \
    VERSIONS (R_13b1, R_2000)                                                 \
    {                                                                         \
      Dwg_Object *last_attrib                                                 \
          = _obj->last_attrib ? _obj->last_attrib->obj : NULL;                \
      Dwg_Object *o = _obj->first_attrib ? _obj->first_attrib->obj : NULL;    \
      if (!o || !last_attrib)                                                 \
        return DWG_ERR_INVALIDHANDLE;                                         \
      if (o->fixedtype == DWG_TYPE_ATTRIB)                                    \
        error |= dwg_dxfb_ATTRIB (dat, o);                                    \
      *i = *i + 1;                                                            \
      do                                                                      \
        {                                                                     \
          o = dwg_next_object (o);                                            \
          if (!o)                                                             \
            return DWG_ERR_INVALIDHANDLE;                                     \
          if (o->fixedtype == DWG_TYPE_ATTRIB)                                \
            error |= dwg_dxfb_ATTRIB (dat, o);                                \
          *i = *i + 1;                                                        \
        }                                                                     \
      while (o->fixedtype == DWG_TYPE_ATTRIB && o != last_attrib);            \
      o = _obj->seqend ? _obj->seqend->obj : NULL;                            \
      if (o && o->fixedtype == DWG_TYPE_SEQEND)                               \
        error |= dwg_dxfb_SEQEND (dat, o);                                    \
      *i = *i + 1;                                                            \
    }                                                                         \
    SINCE (R_2004a)                                                           \
    {                                                                         \
      Dwg_Object *o;                                                          \
      for (BITCODE_BL j = 0; j < _obj->num_owned; j++)                        \
        {                                                                     \
          o = _obj->attribs && _obj->attribs[j] ? _obj->attribs[j]->obj       \
                                                : NULL;                       \
          if (o && o->fixedtype == DWG_TYPE_ATTRIB)                           \
            error |= dwg_dxfb_ATTRIB (dat, o);                                \
        }                                                                     \
      o = _obj->seqend ? _obj->seqend->obj : NULL;                            \
      if (o && o->fixedtype == DWG_TYPE_SEQEND)                               \
        error |= dwg_dxfb_SEQEND (dat, o);                                    \
      *i = *i + _obj->num_owned + 1;                                          \
    }                                                                         \
    return error;                                                             \
  }

    // clang-format off
decl_dxfb_process_INSERT (INSERT)
decl_dxfb_process_INSERT (MINSERT)
    // clang-format on

    static int dwg_dxfb_object (Bit_Chain *restrict dat,
                                const Dwg_Object *restrict obj,
                                int *restrict i)
{
  int error = 0;
  int minimal;
  unsigned int type;

  if (!obj || !obj->parent)
    return DWG_ERR_INTERNALERROR;
  minimal = obj->parent->opts & DWG_OPTS_MINIMAL;
  if (dat->version < R_13b1)
    type = (unsigned int)obj->fixedtype;
  else
    {
      type = obj->type;
      if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT)
        type = DWG_TYPE_UNKNOWN_ENT;
      if (obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
        type = DWG_TYPE_UNKNOWN_OBJ;
    }

  switch (type)
    {
    case DWG_TYPE_TEXT:
      return dwg_dxfb_TEXT (dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_dxfb_ATTDEF (dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_dxfb_BLOCK (dat, obj);
    case DWG_TYPE_ENDBLK:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return 0; // dwg_dxfb_ENDBLK(dat, obj);
    case DWG_TYPE_SEQEND:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return 0; // dwg_dxfb_SEQEND(dat, obj);

    case DWG_TYPE_INSERT:
      error = dwg_dxfb_INSERT (dat, obj);
      return error | dxfb_process_INSERT (dat, obj, i);
    case DWG_TYPE_MINSERT:
      error = dwg_dxfb_MINSERT (dat, obj);
      return error | dxfb_process_MINSERT (dat, obj, i);
    case DWG_TYPE_POLYLINE_2D:
      error = dwg_dxfb_POLYLINE_2D (dat, obj);
      return error | dxfb_process_VERTEX_2D (dat, obj, i);
    case DWG_TYPE_POLYLINE_3D:
      error = dwg_dxfb_POLYLINE_3D (dat, obj);
      return error | dxfb_process_VERTEX_3D (dat, obj, i);
    case DWG_TYPE_POLYLINE_PFACE:
      error = dwg_dxfb_POLYLINE_PFACE (dat, obj);
      return error | dxfb_process_VERTEX_PFACE (dat, obj, i);
    case DWG_TYPE_POLYLINE_MESH:
      error = dwg_dxfb_POLYLINE_MESH (dat, obj);
      return error | dxfb_process_VERTEX_MESH (dat, obj, i);

    case DWG_TYPE_ATTRIB:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return dwg_dxfb_ATTRIB (dat, obj);
    case DWG_TYPE_VERTEX_2D:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return dwg_dxfb_VERTEX_2D (dat, obj);
    case DWG_TYPE_VERTEX_3D:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return dwg_dxfb_VERTEX_3D (dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return dwg_dxfb_VERTEX_MESH (dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return dwg_dxfb_VERTEX_PFACE (dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      LOG_WARN ("stale %s subentity", obj->dxfname);
      return dwg_dxfb_VERTEX_PFACE_FACE (dat, obj);

    case DWG_TYPE_ARC:
      return dwg_dxfb_ARC (dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_dxfb_CIRCLE (dat, obj);
    case DWG_TYPE_LINE:
      return dwg_dxfb_LINE (dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_dxfb_DIMENSION_ORDINATE (dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_dxfb_DIMENSION_LINEAR (dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_dxfb_DIMENSION_ALIGNED (dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_dxfb_DIMENSION_ANG3PT (dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_dxfb_DIMENSION_ANG2LN (dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_dxfb_DIMENSION_RADIUS (dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_dxfb_DIMENSION_DIAMETER (dat, obj);
    case DWG_TYPE_POINT:
      return dwg_dxfb_POINT (dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_dxfb__3DFACE (dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_dxfb_SOLID (dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_dxfb_TRACE (dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_dxfb_SHAPE (dat, obj);
    case DWG_TYPE_VIEWPORT:
      return minimal ? 0 : dwg_dxfb_VIEWPORT (dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_dxfb_ELLIPSE (dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_dxfb_SPLINE (dat, obj);
    case DWG_TYPE_REGION:
      return dwg_dxfb_REGION (dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_dxfb__3DSOLID (dat, obj);
    case DWG_TYPE_BODY:
      return dwg_dxfb_BODY (dat, obj);
    case DWG_TYPE_RAY:
      return dwg_dxfb_RAY (dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_dxfb_XLINE (dat, obj);
    case DWG_TYPE_DICTIONARY:
      return minimal ? 0 : dwg_dxfb_DICTIONARY (dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_dxfb_MTEXT (dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_dxfb_LEADER (dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_dxfb_TOLERANCE (dat, obj);
    case DWG_TYPE_MLINE:
      if (0)
        {
          // bypass -Wunused-function
          dwg_dxfb_JUMP (dat, obj);
          dwg_dxfb_LOAD (dat, obj);
        }
#ifdef DEBUG_CLASSES
      // TODO: looks good, but acad import crashes
      return dwg_dxfb_MLINE (dat, obj);
#else
      LOG_WARN ("Unhandled Entity MLINE in out_dxfb %u/" FMT_H, obj->index,
                obj->handle.value)
      if (0)
        // bypass -Wunused-function
        dwg_dxfb_MLINE (dat, obj);
      return DWG_ERR_UNHANDLEDCLASS;
#endif
    case DWG_TYPE_BLOCK_CONTROL:
    case DWG_TYPE_BLOCK_HEADER:
    case DWG_TYPE_LAYER_CONTROL:
    case DWG_TYPE_LAYER:
    case DWG_TYPE_STYLE_CONTROL:
    case DWG_TYPE_STYLE:
    case DWG_TYPE_LTYPE_CONTROL:
    case DWG_TYPE_LTYPE:
    case DWG_TYPE_VIEW_CONTROL:
    case DWG_TYPE_VIEW:
    case DWG_TYPE_UCS_CONTROL:
    case DWG_TYPE_UCS:
    case DWG_TYPE_VPORT_CONTROL:
    case DWG_TYPE_VPORT:
    case DWG_TYPE_APPID_CONTROL:
    case DWG_TYPE_APPID:
    case DWG_TYPE_DIMSTYLE_CONTROL:
    case DWG_TYPE_DIMSTYLE:
    case DWG_TYPE_VX_CONTROL:
    /* no dxf */
    case DWG_TYPE_VX_TABLE_RECORD:
    /* preR13: no dxfb */
    case DWG_TYPE_REPEAT:
    case DWG_TYPE_ENDREP:
    case DWG_TYPE__3DLINE:
    case DWG_TYPE_LOAD:
    case DWG_TYPE_JUMP:
      LOG_INFO ("Skip unsupported object %s\n", obj->name);
      break;

    case DWG_TYPE_GROUP:
      return dwg_dxfb_GROUP (dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return minimal ? 0 : dwg_dxfb_MLINESTYLE (dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return minimal ? 0 : dwg_dxfb_OLE2FRAME (dat, obj);
    case DWG_TYPE_DUMMY:
      return 0;
    case DWG_TYPE_LONG_TRANSACTION:
      return minimal ? 0 : dwg_dxfb_LONG_TRANSACTION (dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_dxfb_LWPOLYLINE (dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_dxfb_HATCH (dat, obj);
    case DWG_TYPE_XRECORD:
      return minimal ? 0 : dwg_dxfb_XRECORD (dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return minimal ? 0 : dwg_dxfb_PLACEHOLDER (dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return minimal ? 0 : dwg_dxfb_PROXY_ENTITY (dat, obj);
    case DWG_TYPE_OLEFRAME:
      return minimal ? 0 : dwg_dxfb_OLEFRAME (dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      if (!minimal)
        {
          LOG_ERROR ("Unhandled Object VBA_PROJECT");
          // dwg_dxfb_VBA_PROJECT(dat, obj);
          return DWG_ERR_UNHANDLEDCLASS;
        }
      return 0;
    case DWG_TYPE_LAYOUT:
      return minimal ? 0 : dwg_dxfb_LAYOUT (dat, obj);
    default:
      if (obj->type == obj->parent->layout_type)
        {
          return minimal ? 0 : dwg_dxfb_LAYOUT (dat, obj);
        }
      /* > 500 */
      else if (DWG_ERR_UNHANDLEDCLASS
               & (error = dwg_dxfb_variable_type (obj->parent, dat,
                                                  (Dwg_Object *)obj)))
        {
          Dwg_Data *dwg = obj->parent;
          int j = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (j >= 0 && j < (int)dwg->num_classes
              && obj->fixedtype < DWG_TYPE_FREED)
            klass = &dwg->dwg_class[j];
          if (!klass)
            {
              LOG_WARN ("Unknown object, skipping eed/reactors/xdic");
              return DWG_ERR_INVALIDTYPE;
            }
          return error;
        }
    }
  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dxfb_common_entity_handle_data (Bit_Chain *restrict dat,
                                const Dwg_Object *restrict obj)
{
  const Dwg_Data *dwg = obj->parent;
  const Dwg_Object_Entity *_ent = obj->tio.entity;
  const Dwg_Object_Entity *_obj = _ent;
  int error = 0;
  BITCODE_BL vcount = 0;

  // clang-format off
  if (dat->version >= R_13b1)
    {
      #include "common_entity_handle_data.spec"
    }
  #include "common_entity_data.spec"
  // clang-format on

  return error;
}

// see
// https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
AFL_GCC_TOOBIG
static int
dxfb_header_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  double ms;
  const int minimal = dwg->opts & DWG_OPTS_MINIMAL;
  const char *codepage = dxf_codepage (dwg->header.codepage, dwg);

  if (dwg->header.codepage != 30 && dwg->header.codepage != 29
      && dwg->header.codepage != 0 && dwg->header.version < R_2007)
    {
      // some asian or eastern-european codepage
      // see
      // https://github.com/mozman/ezdxf/blob/master/docs/source/dxfinternals/fileencoding.rst
      LOG_WARN ("Unknown codepage %d, assuming ANSI_1252",
                dwg->header.codepage);
    }

    // clang-format off
  #include "header_variables_dxf.spec"
  // clang-format on

  return 0;
}
AFL_GCC_POP

static int
dxfb_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BS j;

  SECTION (CLASSES);
  LOG_TRACE ("num_classes: %u\n", dwg->num_classes);
  for (j = 0; j < dwg->num_classes; j++)
    {
      const char *dxfname = dwg->dwg_class[j].dxfname;
      if (!dxfname)
        continue;
      // some classes are now builtin
      if (dat->version >= R_2004
          && (strEQc (dxfname, "ACDBPLACEHOLDER")
              || strEQc (dxfname, "LAYOUT")))
        continue;
      if (strEQc (dxfname, "DATATABLE"))
        dxfname = "ACDBDATATABLE";
      RECORD (CLASS);
      VALUE_TV (dxfname, 1);
      VALUE_T (dwg->dwg_class[j].cppname, 2);
      if (strEQc (dxfname, "SPATIAL_INDEX"))
        VALUE_TFF ("AutoCAD 2000", 3)
      else
        VALUE_T (dwg->dwg_class[j].appname, 3)
      VALUE_RL (dwg->dwg_class[j].proxyflag, 90);
      SINCE (R_2004a)
      {
        VALUE_RL (dwg->dwg_class[j].num_instances, 91);
      }
      VALUE_RS (dwg->dwg_class[j].is_zombie, 280); // acad: was-a-zombie
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      VALUE_RS (dwg->dwg_class[j].item_class_id == 0x1F2 ? 1 : 0, 281);
    }
  ENDSEC ();
  return 0;
}

static int
dxfb_tables_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  unsigned int i;

  SECTION (TABLES);
  SINCE (R_9c1)
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_VPORT_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.VPORT_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_VPORT_CONTROL *_ctrl = ctrl->tio.object->tio.VPORT_CONTROL;
        TABLE (VPORT);
        // add handle 5 here at first
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_VPORT_CONTROL (dat, ctrl);
        // TODO how far back can DXF read 1000?
        if (dat->version != dat->from_version && dat->from_version >= R_2000)
          {
            /* if saved from newer version, eg. AC1032: */
            VALUE_TV ("ACAD", 1001);
            VALUE_TV ("DbSaveVer", 1000);
            VALUE_RS (dwg->header.dwg_version, 1071); // so that 69 is R_2018
          }
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_VPORT)
              {
                // reordered in the DXF: 2,70,10,11,12,13,14,15,16,...
                // special-cased in the spec
                error |= dwg_dxfb_VPORT (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_LTYPE_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.LTYPE_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_LTYPE_CONTROL *_ctrl = ctrl->tio.object->tio.LTYPE_CONTROL;
        TABLE (LTYPE);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_LTYPE_CONTROL (dat, ctrl);
        // first the 2 builtin ltypes: ByBlock, ByLayer
        if ((obj = dwg_ref_object (dwg, dwg->header_vars.LTYPE_BYBLOCK)))
          {
            dwg_dxfb_LTYPE (dat, obj);
          }
        if ((obj = dwg_ref_object (dwg, dwg->header_vars.LTYPE_BYLAYER)))
          {
            error |= dwg_dxfb_LTYPE (dat, obj);
          }
        // here LTYPE_CONTINUOUS is already included
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_LTYPE)
              {
                error |= dwg_dxfb_LTYPE (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_LAYER_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.LAYER_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_LAYER_CONTROL *_ctrl = ctrl->tio.object->tio.LAYER_CONTROL;
        TABLE (LAYER);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_LAYER_CONTROL (dat, ctrl);
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_LAYER)
              {
                error |= dwg_dxfb_LAYER (dat, obj);
                // else if (obj && obj->type == DWG_TYPE_DICTIONARY)
                //  error |= dwg_dxfb_DICTIONARY(dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_STYLE_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.STYLE_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_STYLE_CONTROL *_ctrl = ctrl->tio.object->tio.STYLE_CONTROL;
        TABLE (STYLE);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_STYLE_CONTROL (dat, ctrl);
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_STYLE)
              {
                error |= dwg_dxfb_STYLE (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_VIEW_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.VIEW_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_VIEW_CONTROL *_ctrl = ctrl->tio.object->tio.VIEW_CONTROL;
        TABLE (VIEW);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_VIEW_CONTROL (dat, ctrl);
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_VIEW)
              error |= dwg_dxfb_VIEW (dat, obj);
          }
        ENDTAB ();
      }
  }
  SINCE (R_9c1)
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_UCS_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.UCS_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_UCS_CONTROL *_ctrl = ctrl->tio.object->tio.UCS_CONTROL;
        TABLE (UCS);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_UCS_CONTROL (dat, ctrl);
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_UCS)
              {
                error |= dwg_dxfb_UCS (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  SINCE (R_11b1)
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_APPID_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.APPID_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_APPID_CONTROL *_ctrl = ctrl->tio.object->tio.APPID_CONTROL;
        TABLE (APPID);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_APPID_CONTROL (dat, ctrl);
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_APPID)
              {
                error |= dwg_dxfb_APPID (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  SINCE (R_11b1)
  {
    Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_DIMSTYLE_CONTROL);
    if (ctrl && ctrl->tio.object && ctrl->tio.object->tio.DIMSTYLE_CONTROL)
      {
        Dwg_Object *obj = ctrl;
        Dwg_Object_DIMSTYLE_CONTROL *_ctrl
            = ctrl->tio.object->tio.DIMSTYLE_CONTROL;
        TABLE (DIMSTYLE);
        COMMON_TABLE_CONTROL_FLAGS;
        dwg_dxfb_DIMSTYLE_CONTROL (dat, ctrl);
        // ignoring morehandles
        for (i = 0; i < _ctrl->num_entries; i++)
          {
            if (!_ctrl->entries)
              break;
            if (!_ctrl->entries[i])
              continue;
            obj = dwg_ref_object (dwg, _ctrl->entries[i]);
            if (obj && obj->type == DWG_TYPE_DIMSTYLE)
              {
                error |= dwg_dxfb_DIMSTYLE (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  // fool the warnings. this table is nowhere to be found in the wild. maybe
  // pre-R11
  if (0)
    {
      Dwg_Object *ctrl = dwg_get_first_object (dwg, DWG_TYPE_VX_CONTROL);
      Dwg_Object_VX_CONTROL *_ctrl = ctrl->tio.object->tio.VX_CONTROL;
      if (ctrl && ctrl->fixedtype == DWG_TYPE_VX_CONTROL)
        {
          Dwg_Object *obj = ctrl;
          TABLE (VX_TABLE_RECORD);
          COMMON_TABLE_CONTROL_FLAGS;
          error |= dwg_dxfb_VX_CONTROL (dat, ctrl);
          for (i = 0; i < _ctrl->num_entries; i++)
            {
              if (!_ctrl->entries)
                break;
              if (!_ctrl->entries[i])
                continue;
              obj = dwg_ref_object (dwg, _ctrl->entries[i]);
              if (obj && obj->type == DWG_TYPE_VX_TABLE_RECORD)
                {
                  RECORD (VX_TABLE_RECORD);
                  error |= dwg_dxfb_VX_TABLE_RECORD (dat, obj);
                }
            }
          ENDTAB ();
        }
    }
  SINCE (R_12)
  {
    Dwg_Object *ctrl, *obj;
    Dwg_Object_BLOCK_CONTROL *_ctrl = dwg_block_control (dwg);
    Dwg_Object_Ref *ref;
    Dwg_Object *mspace = NULL, *pspace = NULL;
    if (!_ctrl)
      {
        LOG_ERROR ("BLOCK_CONTROL missing");
        return DWG_ERR_INVALIDDWG;
      }
    ctrl = dwg_obj_generic_to_object (_ctrl, &error);
    if (!ctrl || ctrl->fixedtype != DWG_TYPE_BLOCK_CONTROL)
      return DWG_ERR_INVALIDDWG;

    obj = ctrl;
    TABLE (BLOCK_RECORD);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_BLOCK_CONTROL (dat, ctrl);

#if 0
    for (i = 0; i < dwg->num_objects; i++)
      {
        Dwg_Object *hdr = &dwg->object[i];
        if (hdr && hdr->supertype == DWG_SUPERTYPE_OBJECT
            && hdr->type == DWG_TYPE_BLOCK_HEADER)
          {
            RECORD (BLOCK_RECORD);
            error |= dwg_dxfb_BLOCK_HEADER (dat, hdr);
          }
      }
#else
    mspace = dwg_model_space_object (dwg);
    if (!mspace)
      return DWG_ERR_INVALIDDWG;
    RECORD (BLOCK_RECORD);
    error |= dwg_dxfb_BLOCK_HEADER (dat, mspace);

    ref = dwg_paper_space_ref (dwg);
    pspace = ref ? dwg_ref_object (dwg, ref) : NULL;
    if (pspace)
      {
        RECORD (BLOCK_RECORD);
        error |= dwg_dxfb_BLOCK_HEADER (dat, pspace);
      }

    for (i = 0; i < _ctrl->num_entries; i++)
      {
        if (!_ctrl->entries)
          break;
        if (!_ctrl->entries[i])
          continue;
        obj = dwg_ref_object (dwg, _ctrl->entries[i]);
        if (obj && obj->type == DWG_TYPE_BLOCK_HEADER && obj != mspace
            && obj != pspace)
          {
            RECORD (BLOCK_RECORD);
            error |= dwg_dxfb_BLOCK_HEADER (dat, obj);
          }
      }
#endif

    ENDTAB ();
  }

  ENDSEC ();
  return 0;
}

static void
dxfb_ENDBLK_empty (Bit_Chain *restrict dat, const Dwg_Object *restrict hdr)
{
  Dwg_Object *obj = (Dwg_Object *)calloc (1, sizeof (Dwg_Object));
  obj->parent = hdr->parent;
  obj->index = obj->parent->num_objects;
  dwg_setup_ENDBLK (obj);
  obj->tio.entity->ownerhandle
      = (Dwg_Object_Ref *)calloc (1, sizeof (Dwg_Object_Ref));
  obj->tio.entity->ownerhandle->obj = (Dwg_Object *)hdr;
  obj->tio.entity->ownerhandle->handleref = hdr->handle;
  obj->tio.entity->ownerhandle->absolute_ref = hdr->handle.value;
  dwg_dxfb_ENDBLK (dat, obj);
  free (obj->tio.entity->tio.ENDBLK);
  free (obj->tio.entity->ownerhandle);
  free (obj->tio.entity);
  free (obj);
}

static int
dxfb_block_write (Bit_Chain *restrict dat, const Dwg_Object *restrict hdr,
                  const Dwg_Object *restrict mspace,
                  const Dwg_Object *restrict pspace, int *restrict i)
{
  int error = 0;
  Dwg_Object *restrict obj = get_first_owned_block (hdr); // BLOCK
  const Dwg_Object_BLOCK_HEADER *restrict _hdr
      = hdr->tio.object->tio.BLOCK_HEADER;
  Dwg_Object *restrict endblk;
  Dwg_Data *dwg = hdr->parent;
  BITCODE_RLL mspace_ref = mspace ? mspace->handle.value : 0;
  BITCODE_RLL pspace_ref = pspace ? pspace->handle.value : 0;

  if (obj)
    error |= dwg_dxfb_object (dat, obj, i);
  else
    {
      LOG_ERROR ("BLOCK_HEADER.block_entity missing");
      return DWG_ERR_INVALIDDWG;
    }
  // Skip all *Model_Space and *Paper_Space entities, esp. new ones: UNDERLAY,
  // MULTILEADER, ... They are all under ENTITIES later. Note: the objects may
  // vary (e.g. example_2000), but the index not
  if ((hdr == mspace) || (hdr->index == mspace->index))
    obj = NULL;
  else if ((hdr == pspace) || (pspace && hdr->index == pspace->index))
    obj = NULL;
  else
    obj = get_first_owned_entity (hdr); // first_entity
  while (obj)
    {
      if (obj->supertype == DWG_SUPERTYPE_ENTITY
          && obj->fixedtype != DWG_TYPE_ENDBLK && obj->tio.entity != NULL
          && (obj->tio.entity->entmode != 2
              || (obj->tio.entity->ownerhandle != NULL
                  && obj->tio.entity->ownerhandle->absolute_ref != mspace_ref
                  && obj->tio.entity->ownerhandle->absolute_ref
                         != pspace_ref)))
        error |= dwg_dxfb_object (dat, obj, i);
      obj = get_next_owned_entity (hdr, obj); // until last_entity
    }
  endblk = get_last_owned_block (hdr);
  if (endblk)
    error |= dwg_dxfb_ENDBLK (dat, endblk);
  else
    {
      LOG_WARN ("Empty ENDBLK for \"%s\" " FORMAT_BL, _hdr->name,
                hdr ? hdr->tio.object->objid : 0);
      dxfb_ENDBLK_empty (dat, hdr);
    }
  return error;
}

static int
dxfb_blocks_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  int i = 0;
  Dwg_Object *restrict mspace = dwg_model_space_object (dwg);
  Dwg_Object *restrict pspace = dwg_paper_space_object (dwg);

  if (!mspace)
    return DWG_ERR_UNHANDLEDCLASS;

  SECTION (BLOCKS);
  /* There may be unconnected blocks (not caught by above),
     such as pspace referred by a LAYOUT or DIMENSION, so for simplicity just
     scan all BLOCK_HEADER's and just skip *Model_Space and *Paper_Space.
     pspace might be NULL.
   */
  {
    for (i = 0; (BITCODE_BL)i < dwg->num_objects; i++)
      {
        const Dwg_Object *restrict obj = &dwg->object[i];
        if (obj->supertype == DWG_SUPERTYPE_OBJECT
            && obj->type == DWG_TYPE_BLOCK_HEADER)
          {
            error |= dxfb_block_write (dat, obj, mspace, pspace, &i);
          }
      }
  }

  ENDSEC ();
  return error;
}

static int
dxfb_entities_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  Dwg_Object *restrict ms = dwg_model_space_object (dwg);
  Dwg_Object *restrict ps = dwg_paper_space_object (dwg);
  Dwg_Object *obj;
  if (!ms)
    return DWG_ERR_INVALIDDWG;

  SECTION (ENTITIES);
  // First mspace
  obj = get_first_owned_entity (ms); // first_entity or entities[0]
  while (obj)
    {
      int i = obj->index;
      error |= dwg_dxfb_object (dat, obj, &i);
      obj = get_next_owned_block_entity (ms, obj); // until last_entity
    }
  // Then all pspace entities. just filter out other BLOCKS entities
  if (ps)
    {
      obj = get_first_owned_entity (ps);
      while (obj)
        {
          int i = obj->index;
          error |= dwg_dxfb_object (dat, obj, &i);
          obj = get_next_owned_block_entity (ps, obj);
        }
    }
  ENDSEC ();
  return error;
}

static int
dxfb_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  int i = 0;
  Dwg_Object *nod;

  SECTION (OBJECTS);
  // The NOD (Named Object Dict) must be always the very first OBJECT,
  // not just DICTIONARY.
  nod = dwg_get_first_object (dwg, DWG_TYPE_DICTIONARY);
  if (nod)
    error |= dwg_dxfb_object (dat, nod, &i);
  for (i = 0; (BITCODE_BL)i < dwg->num_objects; i++)
    {
      const Dwg_Object *restrict obj = &dwg->object[i];
      if (obj == nod)
        continue;
      if (obj->supertype == DWG_SUPERTYPE_OBJECT
          && obj->type != DWG_TYPE_BLOCK_HEADER && !dwg_obj_is_control (obj))
        error |= dwg_dxfb_object (dat, obj, &i);
    }
  ENDSEC ();
  return error;
}

static int
dxfb_thumbnail_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Bit_Chain *pic = (Bit_Chain *)&dwg->thumbnail;
  if (pic->chain && pic->size && pic->size > 10)
    {
      SECTION (THUMBNAILIMAGE);
      VALUE_RL (pic->size, 90);
      VALUE_BINARY (pic->chain, pic->size, 310);
      ENDSEC ();
    }
  return 0;
}

int
dwg_write_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  const int minimal = dwg->opts & DWG_OPTS_MINIMAL;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  if (dat->from_version == R_INVALID)
    dat->from_version = dat->version;
  if (dwg->header.version <= R_2000 && dwg->header.from_version > R_2000)
    dwg_fixup_BLOCKS_entities (dwg);
  dwg_resolve_objectrefs_silent (dwg);

  fprintf (dat->fh, "AutoCAD Binary DXF\r\n%c%c", 0x1a, 0);
  // VALUE_TV (PACKAGE_STRING, 999); // not used in binary DXF

  // a minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  dxfb_header_write (dat, dwg);

  if (!minimal)
    {
      // if downgraded to r13, but we still have classes, keep the
      // classes
      if ((dat->from_version >= R_13b1 && dwg->num_classes)
          || dat->version >= R_2000)
        {
          if (dxfb_classes_write (dat, dwg) >= DWG_ERR_CRITICAL)
            goto fail;
        }

      if (dxfb_tables_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;

      if (dxfb_blocks_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }

  if (dxfb_entities_write (dat, dwg) >= DWG_ERR_CRITICAL)
    goto fail;

  if (!minimal)
    {
      SINCE (R_13b1)
      {
        if (dxfb_objects_write (dat, dwg) >= DWG_ERR_CRITICAL)
          goto fail;
      }
      SINCE (R_2000b)
      {
        if (dxfb_thumbnail_write (dat, dwg) >= DWG_ERR_CRITICAL)
          goto fail;
      }
    }
  RECORD (EOF);

  return 0;
fail:
  return 1;
}

/* Dispatch to the impl on the type dynamically */
int
dwg_dxfb_subent (Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
                 Dwg_Object *restrict obj)
{

#undef DWG_ENTITY
#undef DWG_OBJECT
#define DISPATCH_TYPE(name)                                                   \
  case DWG_TYPE_##name:                                                       \
    return dwg_dxfb_##name##_impl (dat, hdl_dat, str_dat, obj);
#define DWG_ENTITY(name) DISPATCH_TYPE (name)
#define DWG_OBJECT(name) DISPATCH_TYPE (name)

  switch (obj->fixedtype)
    {
// clang-format off
    #include "objects.inc"
    // clang-format on
    case DWG_TYPE_FREED:
      break;
    case DWG_TYPE_UNUSED:
    case DWG_TYPE_ACDSRECORD:
    case DWG_TYPE_ACDSSCHEMA:
    case DWG_TYPE_NPOCOLLECTION:
    case DWG_TYPE_POINTCLOUD:
    case DWG_TYPE_RAPIDRTRENDERENVIRONMENT:
    case DWG_TYPE_XREFPANELOBJECT:
    default:
      LOG_ERROR ("Unhandled subent %s, fixedtype %d in objects.inc",
                 dwg_type_name (obj->fixedtype), (int)obj->fixedtype);
    }

#undef DWG_ENTITY
#undef DWG_OBJECT

  return DWG_ERR_UNHANDLEDCLASS;
}

#undef IS_PRINT
#undef IS_DXF

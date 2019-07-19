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
 * out_dxfb.c: write as Binary DXF
 * written by Reini Urban
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
#include "out_dxf.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];

// private
static int dxfb_common_entity_handle_data (Bit_Chain *restrict dat,
                                           const Dwg_Object *restrict obj);
static int dwg_dxfb_object (Bit_Chain *restrict dat,
                            const Dwg_Object *restrict obj, int *restrict);
static int dxfb_3dsolid (Bit_Chain *restrict dat,
                         const Dwg_Object *restrict obj,
                         Dwg_Entity_3DSOLID *restrict _obj);
static int dxfb_block_write (Bit_Chain *restrict dat, Dwg_Object *restrict hdr,
                             int *restrict i);
static void dxfb_cvt_tablerecord (Bit_Chain *restrict dat,
                                  const Dwg_Object *restrict obj,
                                  char *restrict name, const int dxf);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION dxfb
#define IS_PRINT
#define IS_DXF

#define FIELD(nam, type)
#define FIELDG(nam, type, dxf)                                                \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_##type (nam, dxf);                                                \
    }
#define SUB_FIELD(o, nam, type, dxf) FIELDG (o.nam, type, dxf)

#define HEADER_VALUE(nam, type, dxf, value)                                   \
  GROUP (9);                                                                  \
  fprintf (dat->fh, "$%s%c", #nam, 0);                                        \
  VALUE_##type (value, dxf)

#define HEADER_VAR(nam, type, dxf)                                            \
  HEADER_VALUE (nam, type, dxf, dwg->header_vars.nam)

#define FIELD_CAST(nam, type, cast, dxf) FIELDG (nam, cast, dxf)
#define FIELD_TRACE(nam, type)
// TODO length?
#define VALUE_TV(value, dxf)                                                  \
  {                                                                           \
    GROUP (dxf);                                                              \
    fprintf (dat->fh, "%s%c", value, 0);                                      \
  }
#ifdef HAVE_NATIVE_WCHAR2
#  define VALUE_TU(value, dxf)                                                \
    {                                                                         \
      GROUP (dxf);                                                            \
      fprintf (dat->fh, "%ls%c%c", (wchar_t *)value, 0, 0);                   \
    }
#else
#  define VALUE_TU(wstr, dxf)                                                 \
    {                                                                         \
      BITCODE_TU ws = (BITCODE_TU)wstr;                                       \
      uint16_t _c;                                                            \
      GROUP (dxf);                                                            \
      if (wstr)                                                               \
        while ((_c = *ws++))                                                  \
          {                                                                   \
            fprintf (dat->fh, "%c", (char)(_c & 0xff));                       \
          }                                                                   \
      fprintf (dat->fh, "%c%c", 0, 0);                                        \
    }
#endif
#define VALUE_TFF(str, dxf) VALUE_TV (str, dxf)
#define VALUE_BINARY(value, size, dxf)                                        \
  {                                                                           \
    long len = size;                                                          \
    do                                                                        \
      {                                                                       \
        short j;                                                              \
        long l = len > 127 ? 127 : len;                                       \
        GROUP (dxf);                                                          \
        if (value)                                                            \
          for (j = 0; j < l; j++)                                             \
            {                                                                 \
              fprintf (dat->fh, "%c", value[j]);                              \
            }                                                                 \
        fprintf (dat->fh, "%c", '\0');                                        \
        len -= 127;                                                           \
      }                                                                       \
    while (len > 127);                                                        \
  }
#define FIELD_BINARY(name, size, dxf) VALUE_BINARY (_obj->name, size, dxf)

#define FIELD_VALUE(nam) _obj->nam
#define ANYCODE -1
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  if (dxf)                                                                    \
    {                                                                         \
      uint32_t _j = (hdlptr != NULL)                                          \
                        ? (uint32_t) ((BITCODE_H)hdlptr)->absolute_ref        \
                        : 0;                                                  \
      GROUP (dxf);                                                            \
      fwrite (&_j, sizeof (uint32_t), 1, dat->fh);                            \
    }
// TODO: try to resolve the handle. rather write 0 than in invalid handle:
// if (_obj->nam->obj) ...
#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  VALUE_HANDLE (_obj->nam, nam, handle_code, dxf)
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  VALUE_HANDLE (_obj->o.nam, nam, handle_code, dxf)

#define GROUP(code)                                                           \
  {                                                                           \
    if (dat->version < R_14)                                                  \
      {                                                                       \
        unsigned char icode = (unsigned char)code;                            \
        fwrite (&icode, sizeof (unsigned char), 1, dat->fh);                  \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        short icode = code;                                                   \
        fwrite (&icode, sizeof (short), 1, dat->fh);                          \
      }                                                                       \
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
    if (dat->version >= R_2007)                                               \
      VALUE_TU (value, dxf)                                                   \
    else                                                                      \
      VALUE_TV (value, dxf)                                                   \
  }
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (dat->version >= R_2007)                                               \
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

#define HEADER_TV(nam, dxf)                                                   \
  HEADER_9 (nam);                                                             \
  VALUE_TV (dwg->header_vars.nam, dxf)
#define HEADER_TU(nam, dxf)                                                   \
  HEADER_9 (nam);                                                             \
  VALUE_TU (dwg->header_vars.nam, dxf)
#define HEADER_T(nam, dxf)                                                    \
  HEADER_9 (nam);                                                             \
  VALUE_T (dwg->header_vars.nam, dxf)
#define POINT_3D(nam, var, c1, c2, c3)                                        \
  VALUE_RD (dwg->var.x, c1);                                                  \
  VALUE_RD (dwg->var.y, c2);                                                  \
  VALUE_RD (dwg->var.z, c3)
#define POINT_2D(nam, var, c1, c2)                                            \
  VALUE_RD (dwg->var.x, c1);                                                  \
  VALUE_RD (dwg->var.x, c2)
#define HEADER_3D(nam)                                                        \
  HEADER_9 (nam);                                                             \
  POINT_3D (nam, header_vars.nam, 10, 20, 30)
#define HEADER_2D(nam)                                                        \
  HEADER_9 (nam);                                                             \
  POINT_2D (nam, header_vars.nam, 10, 20)
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
  if (dat->from_version >= R_13)                                              \
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

#define FIELD_DATAHANDLE(nam, code, dxf) FIELD_HANDLE (nam, code, dxf)
#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf)                         \
  FIELD_HANDLE (nam, handle_code, dxf)

#define FIELD_B(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_BB(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_3B(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_BS(nam, dxf) FIELD_RS (nam, dxf)
#define FIELD_BL(nam, dxf) FIELD_RL (nam, dxf)
#define HEADER_BLL(nam, dxf) HEADER_RLL (nam, dxf)
#define FIELD_BD(nam, dxf) FIELD_RD (nam, dxf)

#define HEADER_9(nam)                                                         \
  GROUP (9);                                                                  \
  fprintf (dat->fh, "$%s%c", #nam, 0)
#define VALUE(value, type, dxf) VALUE_##type (value, dxf)
#define VALUE_B(value, dxf) VALUE_RC (value, dxf)
#define VALUE_BB(value, dxf) VALUE_RC (value, dxf)
#define VALUE_3B(value, dxf) VALUE_RC (value, dxf)
#define VALUE_RCs(value, dxf) VALUE_RC (value, dxf)
#define VALUE_BS(value, dxf) VALUE_RS (value, dxf)
#define VALUE_BL(value, dxf) VALUE_RL (value, dxf)
#define VALUE_BD(value, dxf) VALUE_RD (value, dxf)
#define VALUE_RC(value, dxf)                                                  \
  {                                                                           \
    BITCODE_RC c = (value);                                                   \
    GROUP (dxf);                                                              \
    fwrite (&c, sizeof (BITCODE_RC), 1, dat->fh);                             \
  }
#define FIELD_RC(nam, dxf) VALUE_RC (_obj->nam, dxf)
#define HEADER_RC(nam, dxf)                                                   \
  HEADER_9 (nam);                                                             \
  VALUE_RC (dwg->header_vars.nam, dxf)
#define HEADER_B(nam, dxf) HEADER_RC (nam, dxf)

#define VALUE_RS(value, dxf)                                                  \
  {                                                                           \
    BITCODE_RS s = (value);                                                   \
    GROUP (dxf);                                                              \
    fwrite (&s, sizeof (BITCODE_RS), 1, dat->fh);                             \
  }
#define FIELD_RS(nam, dxf) VALUE_RS (_obj->nam, dxf)
#define HEADER_RS(nam, dxf)                                                   \
  HEADER_9 (nam);                                                             \
  VALUE_RS (dwg->header_vars.nam, dxf)

#define VALUE_RD(value, dxf)                                                  \
  {                                                                           \
    double d = (value);                                                       \
    GROUP (dxf);                                                              \
    fwrite (&d, sizeof (double), 1, dat->fh);                                 \
  }
#define FIELD_RD(nam, dxf) VALUE_RD (_obj->nam, dxf)
#define HEADER_RD(nam, dxf)                                                   \
  HEADER_9 (nam);                                                             \
  VALUE_RD (dwg->header_vars.nam, dxf)

#define VALUE_RL(value, dxf)                                                  \
  {                                                                           \
    BITCODE_RL _s = value;                                                    \
    GROUP (dxf);                                                              \
    fwrite (&_s, sizeof (BITCODE_RL), 1, dat->fh);                            \
  }
#define FIELD_RL(nam, dxf) VALUE_RL (_obj->nam, dxf)
#define HEADER_RL(nam, dxf)                                                   \
  HEADER_9 (nam);                                                             \
  VALUE_RL (dwg->header_vars.nam, dxf)

#define HEADER_B(nam, dxf) HEADER_RC (nam, dxf)
#define HEADER_BS(nam, dxf) HEADER_RS (nam, dxf)
#define HEADER_BD(nam, dxf) HEADER_RD (nam, dxf)
#define HEADER_BL(nam, dxf) HEADER_RL (nam, dxf)

#define VALUE_H(value, dxf)                                                   \
  {                                                                           \
    Dwg_Object_Ref *ref = value;                                              \
    if (ref && ref->obj)                                                      \
      {                                                                       \
        VALUE_RL (ref->absolute_ref, dxf);                                    \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        VALUE_RL (0, dxf);                                                    \
      }                                                                       \
  }
#define HEADER_H(nam, dxf)                                                    \
  HEADER_9 (nam);                                                             \
  VALUE_H (dwg->header_vars.nam, dxf)

#define HANDLE_NAME(nam, code, table)                                         \
  VALUE_HANDLE_NAME (dwg->header_vars.nam, dxf, table)
// TODO: convert STANDARD to Standard?
#define VALUE_HANDLE_NAME(value, dxf, table)                                  \
  {                                                                           \
    Dwg_Object_Ref *ref = value;                                              \
    if (ref && ref->obj && ref->obj->supertype == DWG_SUPERTYPE_OBJECT)       \
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

#define HEADER_RLL(nam, dxf)                                                  \
  {                                                                           \
    BITCODE_RLL s = _obj->nam;                                                \
    GROUP (9);                                                                \
    fprintf (dat->fh, "$%s%c", #nam, 0);                                      \
    GROUP (dxf);                                                              \
    fwrite (&s, sizeof (BITCODE_RLL), 1, dat->fh);                            \
  }

#define FIELD_MC(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_MS(nam, dxf) FIELD_RS (nam, dxf)
#define FIELD_BT(nam, dxf) FIELD_BD (nam, dxf);
#define FIELD_4BITS(nam, dxf) FIELD_RC (nam, dxf)
#define FIELD_BE(nam, dxf)                                                    \
  {                                                                           \
    if (!(_obj->nam.x == 0.0 && _obj->nam.y == 0.0 && _obj->nam.z == 1.0))    \
      FIELD_3RD (nam, dxf)                                                    \
  }
#define FIELD_DD(nam, _default, dxf) FIELD_RD (nam, dxf)
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
    FIELD_RD (nam.x, dxf);                                                    \
    FIELD_RD (nam.y, dxf + 10);                                               \
  }
#define FIELD_2BD(nam, dxf) FIELD_2RD (nam, dxf)
#define FIELD_2BD_1(nam, dxf)                                                 \
  {                                                                           \
    FIELD_RD (nam.x, dxf);                                                    \
    FIELD_RD (nam.y, dxf + 1);                                                \
  }
#define FIELD_3RD(nam, dxf)                                                   \
  {                                                                           \
    FIELD_RD (nam.x, dxf);                                                    \
    FIELD_RD (nam.y, dxf + 10);                                               \
    FIELD_RD (nam.z, dxf + 20);                                               \
  }
#define FIELD_3BD(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3BD_1(nam, dxf)                                                 \
  {                                                                           \
    FIELD_RD (nam.x, dxf);                                                    \
    FIELD_RD (nam.y, dxf + 1);                                                \
    FIELD_RD (nam.z, dxf + 2);                                                \
  }
#define FIELD_3DPOINT(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_CMC(color, dxf1, dxf2)                                          \
  {                                                                           \
    if (dxf1 > 0 && (dat->version < R_2004 || _obj->color.index == 256))      \
      {                                                                       \
        VALUE_RS ((_obj->color.index & 255), dxf1);                           \
      }                                                                       \
    if (dat->version >= R_2004 && dxf2 > 0 && _obj->color.index != 256)       \
      {                                                                       \
        VALUE_RS (_obj->color.rgb >> 24, dxf1);                               \
        VALUE_RL (_obj->color.rgb & 0x00ffffff, dxf2);                        \
      }                                                                       \
  }
#define SUB_FIELD_CMC(o, color, dxf1, dxf2)                                   \
  {                                                                           \
    if (dxf1 > 0 && (dat->version < R_2004 || _obj->o.color.index == 256))    \
      {                                                                       \
        VALUE_RS ((_obj->o.color.index & 255), dxf1);                         \
      }                                                                       \
    if (dat->version >= R_2004 && dxf2 > 0 && _obj->o.color.index != 256)     \
      {                                                                       \
        VALUE_RS (_obj->o.color.rgb >> 24, dxf1);                             \
        VALUE_RL (_obj->o.color.rgb & 0x00ffffff, dxf2);                      \
      }                                                                       \
  }
#define HEADER_CMC(nam, dxf)                                                  \
  HEADER_9 (nam);                                                             \
  VALUE_RS (dwg->header_vars.nam.index, dxf)
#define HEADER_TIMEBLL(nam, dxf)                                              \
  HEADER_9 (nam);                                                             \
  FIELD_TIMEBLL (nam, dxf)
#define FIELD_TIMEBLL(nam, dxf) VALUE_RD (_obj->nam.value, dxf)

// FIELD_VECTOR_N(nam, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'nam'.
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        VALUE (_obj->nam[vcount], type, dxf);                                 \
    }
#define FIELD_VECTOR_T(nam, size, dxf)                                        \
  if (dxf)                                                                    \
    {                                                                         \
      PRE (R_2007)                                                            \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          VALUE_TV (_obj->nam[vcount], dxf)                                   \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          VALUE_TU (_obj->nam[vcount], dxf)                                   \
      }                                                                       \
    }

#define FIELD_VECTOR(nam, type, size, dxf)                                    \
  FIELD_VECTOR_N (nam, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2RD (nam[vcount], dxf);                                       \
        }                                                                     \
    }

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  if (dxf)                                                                    \
    {                                                                         \
      FIELD_2RD (nam[0], 0);                                                  \
      for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2DD (nam[vcount], FIELD_VALUE (nam[vcount - 1].x),            \
                     FIELD_VALUE (nam[vcount - 1].y), dxf);                   \
        }                                                                     \
    }

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_3DPOINT (nam[vcount], dxf);                                   \
        }                                                                     \
    }

#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  if (dxf)                                                                    \
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
  if (dat->version >= R_13 && obj->tio.object->xdicobjhandle                  \
      && obj->tio.object->xdicobjhandle->absolute_ref)                        \
    {                                                                         \
      VALUE_TV ("{ACAD_XDICTIONARY", 102);                                    \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,      \
                    360);                                                     \
      VALUE_TV ("}", 102);                                                    \
    }
#define _REACTORS(code)                                                       \
  if (dat->version >= R_13 && obj->tio.object->num_reactors                   \
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
  if (dat->version >= R_13 && _obj->num_reactors && _obj->reactors)           \
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
  if (dat->version >= R_13 && obj->tio.entity->xdicobjhandle                  \
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
  static int dwg_dxfb_##token (Bit_Chain *restrict dat,                       \
                               const Dwg_Object *restrict obj)                \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Dwg_Data *dwg = obj->parent;                                              \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    int error = 0;                                                            \
    if (obj->fixedtype != DWG_TYPE_##token)                                   \
      {                                                                       \
        LOG_ERROR ("Invalid type 0x%x, expected 0x%x %s", obj->fixedtype,     \
                   DWG_TYPE_##token, #token);                                 \
        return DWG_ERR_INVALIDTYPE;                                           \
      }                                                                       \
    if (!strcmp (#token, "GEOPOSITIONMARKER"))                                \
      RECORD (POSITIONMARKER)                                                 \
    else if (dat->version < R_13 && strlen (#token) == 10                     \
             && !strcmp (#token, "LWPOLYLINE"))                               \
      RECORD (POLYLINE)                                                       \
    else if (strlen (#token) > 11 && !memcmp (#token, "DIMENSION_", 11))      \
      RECORD (DIMENSION)                                                      \
    else if (strlen (#token) > 9 && !memcmp (#token, "POLYLINE_", 9))         \
      RECORD (POLYLINE)                                                       \
    else if (strlen (#token) > 7 && !memcmp (#token, "VERTEX_", 7))           \
      RECORD (VERTEX)                                                         \
    else if (dat->version >= R_2010 && !strcmp (#token, "TABLE"))             \
      {                                                                       \
        RECORD (ACAD_TABLE);                                                  \
        return dwg_dxfb_TABLECONTENT (dat, obj);                              \
      }                                                                       \
    else if (strlen (#token) > 3 && !memcmp (#token, "_3D", 3))               \
      VALUE_TV (obj->dxfname, 0)                                              \
    else if (obj->type >= 500 && obj->dxfname)                                \
      VALUE_TV (obj->dxfname, 0)                                              \
    else                                                                      \
      RECORD (token)                                                          \
    LOG_INFO ("Entity " #token ":\n")                                         \
    _ent = obj->tio.entity;                                                   \
    _obj = ent = _ent->tio.token;                                             \
    SINCE (R_11)                                                              \
    {                                                                         \
      uint32_t i = (uint32_t)obj->handle.value;                               \
      LOG_TRACE ("Entity handle: %d.%d.%lX\n", obj->handle.code,              \
                 obj->handle.size, obj->handle.value)                         \
      GROUP (330);                                                            \
      fwrite (&i, sizeof (uint32_t), 1, dat->fh);                             \
    }                                                                         \
    SINCE (R_13)                                                              \
    {                                                                         \
      VALUE_HANDLE_NAME (obj->parent->header_vars.BLOCK_RECORD_MSPACE, 330,   \
                         BLOCK_HEADER);                                       \
      error |= dxfb_common_entity_handle_data (dat, obj);                     \
    }

#define DWG_ENTITY_END                                                        \
  return 0;                                                                   \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_dxfb_##token (Bit_Chain *restrict dat,                       \
                               const Dwg_Object *restrict obj)                \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Bit_Chain *hdl_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_##token *_obj;                                                 \
    int error = 0;                                                            \
    LOG_INFO ("Object " #token ":\n")                                         \
    if (obj->fixedtype != DWG_TYPE_##token)                                   \
      {                                                                       \
        LOG_ERROR ("Invalid type 0x%x, expected 0x%x %s", obj->fixedtype,     \
                   DWG_TYPE_##token, #token);                                 \
        return DWG_ERR_INVALIDTYPE;                                           \
      }                                                                       \
    _obj = obj->tio.object->tio.token;                                        \
    if (!dwg_obj_is_control (obj))                                            \
      {                                                                       \
        if (obj->fixedtype == DWG_TYPE_TABLE)                                 \
          ;                                                                   \
        else if (obj->type >= 500 && obj->dxfname)                            \
          VALUE_TV (obj->dxfname, 0)                                          \
        else if (obj->type == DWG_TYPE_PLACEHOLDER)                           \
          RECORD (ACDBPLACEHOLDER)                                            \
        else if (obj->type != DWG_TYPE_BLOCK_HEADER)                          \
          RECORD (token)                                                      \
        SINCE (R_13)                                                          \
        {                                                                     \
          const uint32_t _i = (uint32_t)obj->handle.value;                    \
          const int dxf = obj->type == DWG_TYPE_DIMSTYLE ? 105 : 5;           \
          GROUP (dxf);                                                        \
          fwrite (&_i, sizeof (uint32_t), 1, dat->fh);                        \
          _XDICOBJHANDLE (3);                                                 \
          _REACTORS (4);                                                      \
        }                                                                     \
      }

#define DWG_OBJECT_END                                                        \
  return 0;                                                                   \
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
      short type = get_base_value_type (rbuf->type);
      int dxftype = (rbuf->type > 1000 || obj->fixedtype == DWG_TYPE_XRECORD)
                        ? rbuf->type
                        : rbuf->type + 1000;

      tmp = rbuf->next;
      switch (type)
        {
        case VT_STRING:
          UNTIL (R_2007){ VALUE_TV (rbuf->value.str.u.data,
                                    dxftype) } LATER_VERSIONS
          {
            VALUE_TU (rbuf->value.str.u.wdata, dxftype)
          }
          break;
        case VT_REAL:
          VALUE_RD (rbuf->value.dbl, dxftype);
          break;
        case VT_BOOL:
        case VT_INT8:
          VALUE_RC (rbuf->value.i8, dxftype);
          break;
        case VT_INT16:
          VALUE_RS (rbuf->value.i16, dxftype);
          break;
        case VT_INT32:
          VALUE_RL (rbuf->value.i32, dxftype);
          break;
        case VT_POINT3D:
          VALUE_RD (rbuf->value.pt[0], dxftype);
          VALUE_RD (rbuf->value.pt[1], dxftype + 1);
          VALUE_RD (rbuf->value.pt[2], dxftype + 2);
          break;
        case VT_BINARY:
          VALUE_BINARY (rbuf->value.str.u.data, rbuf->value.str.size, dxftype);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          // fprintf(dat->fh, "%lX\r\n", (unsigned
          // long)*(uint64_t*)rbuf->value.hdl);
          GROUP (dxftype);
          fwrite (&rbuf->value.hdl, sizeof (uint32_t), 1, dat->fh);
          break;
        case VT_INVALID:
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
      if (dat->version >= R_2007) // r2007+ unicode names
        {
          name = bit_convert_TU ((BITCODE_TU)name);
        }
      if (dat->from_version >= R_2000 && dat->version < R_2000)
        { // convert the other way round, from newer to older
          if (!strcmp (name, "Standard"))
            VALUE_TV ("STANDARD", dxf)
          else if (!strcmp (name, "ByLayer"))
            VALUE_TV ("BYLAYER", dxf)
          else if (!strcmp (name, "ByBlock"))
            VALUE_TV ("BYBLOCK", dxf)
          else if (!strcmp (name, "*Active"))
            VALUE_TV ("*ACTIVE", dxf)
          else
            VALUE_TV (name, dxf)
        }
      else
        { // convert some standard names
          if (dat->version >= R_2000 && !strcmp (name, "STANDARD"))
            VALUE_TV ("Standard", dxf)
          else if (dat->version >= R_2000 && !strcmp (name, "BYLAYER"))
            VALUE_TV ("ByLayer", dxf)
          else if (dat->version >= R_2000 && !strcmp (name, "BYBLOCK"))
            VALUE_TV ("ByBlock", dxf)
          else if (dat->version >= R_2000 && !strcmp (name, "*ACTIVE"))
            VALUE_TV ("*Active", dxf)
          else
            VALUE_TV (name, dxf)
        }
      if (dat->version >= R_2007)
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
  if (dat->from_version >= R_2007) // r2007+ unicode names
    {
      name = bit_convert_TU ((BITCODE_TU)name);
    }
  if (dat->version == dat->from_version) // no conversion
    {
      VALUE_TV (name, dxf)
    }
  else if (dat->version < R_13 && dat->from_version >= R_13) // to older
    {
      if (strlen (name) < 10)
        VALUE_TV (name, dxf)
      else if (!strcmp (name, "*Model_Space"))
        VALUE_TV ("$MODEL_SPACE", dxf)
      else if (!strcmp (name, "*Paper_Space"))
        VALUE_TV ("$PAPER_SPACE", dxf)
      else if (!memcmp (name, "*Paper_Space", sizeof ("*Paper_Space") - 1))
        {
          GROUP (dxf);
          fprintf (dat->fh, "$PAPER_SPACE%s%c", &name[12], 0);
        }
      else
        VALUE_TV (name, dxf)
    }
  else if (dat->version >= R_13 && dat->from_version < R_13) // to newer
    {
      if (strlen (name) < 10)
        VALUE_TV (name, dxf)
      else if (!strcmp (name, "$MODEL_SPACE"))
        VALUE_TV ("*Model_Space", dxf)
      else if (!strcmp (name, "$PAPER_SPACE"))
        VALUE_TV ("*Paper_Space", dxf)
      else if (!memcmp (name, "$PAPER_SPACE", sizeof ("$PAPER_SPACE") - 1))
        {
          GROUP (dxf);
          fprintf (dat->fh, "*Paper_Space%s%c", &name[12], 0);
        }
      else
        VALUE_TV (name, dxf)
    }
  if (dat->from_version >= R_2007)
    free (name);
}

#define START_OBJECT_HANDLE_STREAM

// 5 written here first
#define COMMON_TABLE_CONTROL_FLAGS                                            \
  SINCE (R_13)                                                                \
  {                                                                           \
    uint32_t _i = (uint32_t)ctrl->handle.value;                               \
    GROUP (5);                                                                \
    fwrite (&_i, sizeof (uint32_t), 1, dat->fh);                              \
  }                                                                           \
  SINCE (R_14)                                                                \
  {                                                                           \
    VALUE_HANDLE (ctrl->tio.object->ownerhandle, ownerhandle, 3, 330);        \
  }                                                                           \
  SINCE (R_13) { VALUE_TV ("AcDbSymbolTable", 100); }

#define COMMON_TABLE_FLAGS(acdbname)                                          \
  /* TODO: ACAD_XDICTIONARY */                                                \
  SINCE (R_14)                                                                \
  {                                                                           \
    VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 3, 330);         \
  }                                                                           \
  SINCE (R_13)                                                                \
  {                                                                           \
    VALUE_TV ("AcDbSymbolTableRecord", 100)                                   \
    VALUE_TV ("AcDb" #acdbname "TableRecord", 100)                            \
  }                                                                           \
  if (!strcmp (#acdbname, "Block") && dat->version >= R_13)                   \
    {                                                                         \
      Dwg_Object *blk = dwg_ref_object (                                      \
          dwg, ((Dwg_Object_BLOCK_HEADER *)_obj)->block_entity);              \
      if (blk && blk->type == DWG_TYPE_BLOCK)                                 \
        {                                                                     \
          Dwg_Entity_BLOCK *_blk = blk->tio.entity->tio.BLOCK;                \
          if (dat->from_version >= R_2007)                                    \
            VALUE_TU (_blk->name, 2)                                          \
          else                                                                \
            VALUE_TV (_blk->name, 2)                                          \
        }                                                                     \
      else if (_obj->name)                                                    \
        {                                                                     \
          if (dat->from_version >= R_2007)                                    \
            VALUE_TU (_obj->name, 2)                                          \
          else                                                                \
            VALUE_TV (_obj->name, 2)                                          \
        }                                                                     \
      else                                                                    \
        VALUE_TV ("*", 2)                                                     \
    }                                                                         \
  else if (_obj->name)                                                        \
    dxfb_cvt_tablerecord (dat, obj, _obj->name, 2);                           \
  else                                                                        \
    VALUE_TV ("*", 2)                                                         \
  FIELD_RC (flag, 70)

#define LAYER_TABLE_FLAGS(acdbname)                                           \
  SINCE (R_14)                                                                \
  {                                                                           \
    VALUE_HANDLE (obj->tio.object->ownerhandle, ownerhandle, 3, 330);         \
  }                                                                           \
  SINCE (R_13)                                                                \
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
  Dwg_Data *dwg = obj->parent;
  unsigned long j;
  BITCODE_BL vcount, rcount1, rcount2;
  BITCODE_BL i;
  int error = 0;
  int index;
  int total_size = 0;
  int num_blocks = 0;

  COMMON_ENTITY_HANDLE_DATA;

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
              int len = strlen (s);
              // FIELD_BL (block_size[i], 0);
              // DXF 1 + 3 if >255
              while (len > 0)
                {
                  char *n = strchr (s, '\n');
                  int l = len > 255 ? 255 : len;
                  if (n && (n - s < len))
                    l = n - s;
                  if (l)
                    {
                      if (l < 255)
                        GROUP (1)
                      else
                        GROUP (3)
                      /* FIXME binary */
                      if (s[l - 1] == '\r')
                        fprintf (dat->fh, "%.*s\n", l, s);
                      else
                        fprintf (dat->fh, "%.*s\r\n", l, s);
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
        {
          // TODO
          LOG_ERROR ("TODO: Implement parsing of SAT file (version 2) "
                     "in entities 37,38 and 39.\n");
        }
      /*
            FIELD_B (wireframe_data_present, 0);
            if (FIELD_VALUE(wireframe_data_present))
              {
                FIELD_B (point_present, 0);
                if (FIELD_VALUE(point_present))
                  {
                    FIELD_3BD (point, 0);
                  }
                FIELD_BL (num_isolines, 0);
                FIELD_B (isoline_present, 0);
                if (FIELD_VALUE(isoline_present))
                  {
                    FIELD_BL (num_wires, 0);
                    REPEAT(num_wires, wires, Dwg_3DSOLID_wire)
                      {
                        PARSE_WIRE_STRUCT(wires[rcount1])
                      }
                    END_REPEAT(wires);
                    FIELD_BL (num_silhouettes, 0);
                    REPEAT(num_silhouettes, silhouettes,
         Dwg_3DSOLID_silhouette)
                      {
                        FIELD_BL (silhouettes[rcount1].vp_id, 0);
                        FIELD_3BD (silhouettes[rcount1].vp_target, 0);
                        FIELD_3BD (silhouettes[rcount1].vp_dir_from_target, 0);
                        FIELD_3BD (silhouettes[rcount1].vp_up_dir, 0);
                        FIELD_B (silhouettes[rcount1].vp_perspective, 0);
                        FIELD_BL (silhouettes[rcount1].num_wires, 0);
                        REPEAT2(silhouettes[rcount1].num_wires,
         silhouettes[rcount1].wires, Dwg_3DSOLID_wire)
                          {
                            PARSE_WIRE_STRUCT(silhouettes[rcount1].wires[rcount2])
                          }
                        END_REPEAT(silhouettes[rcount1].wires);
                      }
                    END_REPEAT(silhouettes);
                  }
              }
      */
      FIELD_B (acis_empty_bit, 0);
      if (!FIELD_VALUE (acis_empty_bit))
        {
          LOG_ERROR ("TODO: Implement parsing of ACIS data at the end "
                     "of 3dsolid object parsing (acis_empty_bit==0).\n");
        }

      SINCE (R_2007)
      {
        FIELD_BL (unknown_2007, 0);
        FIELD_HANDLE (history_id, ANYCODE, 350);
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

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  // almost always false
  is_entity = dwg_class_is_entity (klass);

#include "classes.inc"

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
    VERSIONS (R_13, R_2000)                                                   \
    {                                                                         \
      Dwg_Object *last_vertex = _obj->last_vertex->obj;                       \
      Dwg_Object *o = _obj->first_vertex ? _obj->first_vertex->obj : NULL;    \
      if (!o)                                                                 \
        return DWG_ERR_INVALIDHANDLE;                                         \
      if (o->fixedtype == DWG_TYPE_VERTEX_##token)                            \
        error |= dwg_dxfb_VERTEX_##token (dat, o);                            \
      *i = *i + 1;                                                            \
      do                                                                      \
        {                                                                     \
          o = dwg_next_object (o);                                            \
          if (!o)                                                             \
            return DWG_ERR_INVALIDHANDLE;                                     \
          if (!strcmp (#token, "PFACE")                                       \
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
    SINCE (R_2004)                                                            \
    {                                                                         \
      Dwg_Object *o;                                                          \
      for (BITCODE_BL j = 0; j < _obj->num_owned; j++)                        \
        {                                                                     \
          o = _obj->vertex[j] ? _obj->vertex[j]->obj : NULL;                  \
          if (!strcmp (#token, "PFACE") && o                                  \
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
decl_dxfb_process_VERTEX (2D) decl_dxfb_process_VERTEX (3D)
    decl_dxfb_process_VERTEX (MESH) decl_dxfb_process_VERTEX (PFACE)

/* process seqend before attribs */
#define decl_dxfb_process_INSERT(token)                                       \
  static int dxfb_process_##token (Bit_Chain *restrict dat,                   \
                                   const Dwg_Object *restrict obj,            \
                                   int *restrict i)                           \
  {                                                                           \
    int error = 0;                                                            \
    Dwg_Entity_##token *_obj = obj->tio.entity->tio.token;                    \
                                                                              \
    VERSIONS (R_13, R_2000)                                                   \
    {                                                                         \
      Dwg_Object *last_attrib = _obj->last_attrib->obj;                       \
      Dwg_Object *o = _obj->first_attrib ? _obj->first_attrib->obj : NULL;    \
      if (!o)                                                                 \
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
    SINCE (R_2004)                                                            \
    {                                                                         \
      Dwg_Object *o;                                                          \
      for (BITCODE_BL j = 0; j < _obj->num_owned; j++)                        \
        {                                                                     \
          o = _obj->attrib_handles[j] ? _obj->attrib_handles[j]->obj : NULL;  \
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
        decl_dxfb_process_INSERT (INSERT) decl_dxfb_process_INSERT (MINSERT)

            static int dwg_dxfb_object (Bit_Chain *restrict dat,
                                        const Dwg_Object *restrict obj,
                                        int *restrict i)
{
  int error = 0;
  int minimal;

  if (!obj || !obj->parent)
    return DWG_ERR_INTERNALERROR;
  if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
    return 0;
  minimal = obj->parent->opts & 0x10;

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_dxfb_TEXT (dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_dxfb_ATTDEF (dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_dxfb_BLOCK (dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_dxfb_ENDBLK (dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_dxfb_SEQEND (dat, obj);

    case DWG_TYPE_INSERT:
      error = dwg_dxfb_INSERT (dat, obj);
      {
        Dwg_Entity_INSERT *_obj = obj->tio.entity->tio.INSERT;
        if (_obj->has_attribs)
          return error | dxfb_process_INSERT (dat, obj, i);
        else
          return error;
      }
    case DWG_TYPE_MINSERT:
      error = dwg_dxfb_MINSERT (dat, obj);
      {
        Dwg_Entity_MINSERT *_obj = obj->tio.entity->tio.MINSERT;
        if (_obj->has_attribs)
          return error | dxfb_process_MINSERT (dat, obj, i);
        else
          return error;
      }

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
#ifdef DEBUG_CLASSES
      // TODO: looks good, but acad import crashes
      return dwg_dxfb_MLINE (dat, obj);
#else
      LOG_WARN ("Unhandled Entity MLINE in out_dxfb %u/%lX", obj->index,
                obj->handle.value)
      if (0)
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
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
    case DWG_TYPE_VPORT_ENTITY_HEADER:
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
      // TODO dwg_dxfb_PROXY_ENTITY(dat, obj);
      return DWG_ERR_UNHANDLEDCLASS;
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
      if (obj->type == obj->parent->layout_number)
        {
          return minimal ? 0 : dwg_dxfb_LAYOUT (dat, obj);
        }
      /* > 500 */
      else if (DWG_ERR_UNHANDLEDCLASS
               & (error = dwg_dxfb_variable_type (obj->parent, dat,
                                                  (Dwg_Object *)obj)))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int j = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (j >= 0 && j < (int)dwg->num_classes)
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
  Dwg_Object_Entity *ent;
  // Dwg_Data *dwg = obj->parent;
  Dwg_Object_Entity *_obj;
  int error = 0;
  BITCODE_BL vcount = 0;
  ent = obj->tio.entity;
  _obj = ent;

#include "common_entity_handle_data.spec"

  return error;
}

// see
// https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxfb_header_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  double ms;
  const int minimal = dwg->opts & 0x10;
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

#include "header_variables_dxf.spec"

  return 0;
}

static int
dxfb_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BS j;

  SECTION (CLASSES);
  LOG_TRACE ("num_classes: %u\n", dwg->num_classes);
  for (j = 0; j < dwg->num_classes; j++)
    {
      const char *dxfname = dwg->dwg_class[j].dxfname;
      // some classes are now builtin
      if (dat->version >= R_2004
          && (!strcmp (dxfname, "ACDBPLACEHOLDER")
              || !strcmp (dxfname, "LAYOUT")))
        continue;
      RECORD (CLASS);
      VALUE_TV (dxfname, 1);
      VALUE_T (dwg->dwg_class[j].cppname, 2);
      VALUE_T (dwg->dwg_class[j].appname, 3);
      VALUE_RS (dwg->dwg_class[j].proxyflag, 90);
      SINCE (R_2004) { VALUE_RC (dwg->dwg_class[j].num_instances, 91); }
      VALUE_RC (dwg->dwg_class[j].wasazombie, 280);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      VALUE_RC (dwg->dwg_class[j].item_class_id == 0x1F2 ? 1 : 0, 281);
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
  {
    Dwg_Object_VPORT_CONTROL *_ctrl = &dwg->vport_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
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
            VALUE_RS ((dat->from_version * 3) + 15,
                      1071); // so that 69 is R_2018
          }
        for (i = 0; i < dwg->vport_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object (dwg, _ctrl->vports[i]);
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
    Dwg_Object_LTYPE_CONTROL *_ctrl = &dwg->ltype_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        Dwg_Object *obj;
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
        for (i = 0; i < dwg->ltype_control.num_entries; i++)
          {
            obj = dwg_ref_object (dwg, _ctrl->linetypes[i]);
            if (obj && obj->type == DWG_TYPE_LTYPE)
              {
                error |= dwg_dxfb_LTYPE (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object_LAYER_CONTROL *_ctrl = &dwg->layer_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE (LAYER);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_LAYER_CONTROL (dat, ctrl);
        for (i = 0; i < dwg->layer_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object (dwg, _ctrl->layers[i]);
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
    Dwg_Object_STYLE_CONTROL *_ctrl = &dwg->style_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE (STYLE);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_STYLE_CONTROL (dat, ctrl);
        for (i = 0; i < dwg->style_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object (dwg, _ctrl->styles[i]);
            if (obj && obj->type == DWG_TYPE_STYLE)
              {
                error |= dwg_dxfb_STYLE (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object_VIEW_CONTROL *_ctrl = &dwg->view_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE (VIEW);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_VIEW_CONTROL (dat, ctrl);
        for (i = 0; i < dwg->view_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object (dwg, _ctrl->views[i]);
            // FIXME implement the other two
            if (obj && obj->type == DWG_TYPE_VIEW)
              error |= dwg_dxfb_VIEW (dat, obj);
            /*
              else if (obj && obj->fixedtype == DWG_TYPE_SECTIONVIEWSTYLE)
              error |= dwg_dxfb_SECTIONVIEWSTYLE(dat, obj);
              if (obj && obj->fixedtype == DWG_TYPE_DETAILVIEWSTYLE) {
              error |= dwg_dxfb_DETAILVIEWSTYLE(dat, obj);
            */
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object_UCS_CONTROL *_ctrl = &dwg->ucs_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE (UCS);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_UCS_CONTROL (dat, ctrl);
        for (i = 0; i < dwg->ucs_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object (dwg, _ctrl->ucs[i]);
            if (obj && obj->type == DWG_TYPE_UCS)
              {
                error |= dwg_dxfb_UCS (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  SINCE (R_13)
  {
    Dwg_Object_APPID_CONTROL *_ctrl = &dwg->appid_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE (APPID);
        COMMON_TABLE_CONTROL_FLAGS;
        error |= dwg_dxfb_APPID_CONTROL (dat, ctrl);
        for (i = 0; i < dwg->appid_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object (dwg, _ctrl->apps[i]);
            if (obj && obj->type == DWG_TYPE_APPID)
              {
                error |= dwg_dxfb_APPID (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  {
    Dwg_Object_DIMSTYLE_CONTROL *_ctrl = &dwg->dimstyle_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    if (ctrl)
      {
        TABLE (DIMSTYLE);
        COMMON_TABLE_CONTROL_FLAGS;
        dwg_dxfb_DIMSTYLE_CONTROL (dat, ctrl);
        // ignoring morehandles
        for (i = 0; i < dwg->dimstyle_control.num_entries; i++)
          {
            Dwg_Object *obj = dwg_ref_object (dwg, _ctrl->dimstyles[i]);
            if (obj && obj->type == DWG_TYPE_DIMSTYLE)
              {
                error |= dwg_dxfb_DIMSTYLE (dat, obj);
              }
          }
        ENDTAB ();
      }
  }
  // fool the warnings. this table is nowhere to be found in the wild. maybe
  // pre-R_11
  if (0 && dwg->vport_entity_control.num_entries)
    {
      Dwg_Object_VPORT_ENTITY_CONTROL *_ctrl = &dwg->vport_entity_control;
      Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
      if (ctrl)
        {
          TABLE (VPORT_ENTITY);
          COMMON_TABLE_CONTROL_FLAGS;
          error |= dwg_dxfb_VPORT_ENTITY_CONTROL (dat, ctrl);
          for (i = 0; i < dwg->vport_entity_control.num_entries; i++)
            {
              Dwg_Object *obj
                  = dwg_ref_object (dwg, _ctrl->vport_entity_headers[i]);
              if (obj && obj->type == DWG_TYPE_VPORT_ENTITY_HEADER)
                {
                  error |= dwg_dxfb_VPORT_ENTITY_HEADER (dat, obj);
                }
            }
          // avoid unused warnings
          dwg_dxfb_PROXY_ENTITY (dat, &dwg->object[0]);
          ENDTAB ();
        }
    }
  SINCE (R_13)
  {
    Dwg_Object_BLOCK_CONTROL *_ctrl = &dwg->block_control;
    Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
    // Dwg_Object *obj = dwg_ref_object(dwg, _ctrl->model_space);
    // Dwg_Object *mspace = NULL, *pspace = NULL;

    TABLE (BLOCK_RECORD);
    COMMON_TABLE_CONTROL_FLAGS;
    error |= dwg_dxfb_BLOCK_CONTROL (dat, ctrl);

#if 1
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
    if (obj && obj->type == DWG_TYPE_BLOCK_HEADER)
      {
        mspace = obj;
        RECORD (BLOCK_RECORD);
        error |= dwg_dxfb_BLOCK_HEADER (dat, obj);
      }
    if (_ctrl->paper_space)
      {
        obj = dwg_ref_object (dwg, _ctrl->paper_space);
        if (obj && obj->type == DWG_TYPE_BLOCK_HEADER)
          {
            pspace = obj;
            RECORD (BLOCK_RECORD);
            error |= dwg_dxfb_BLOCK_HEADER (dat, obj);
          }
      }
    for (i = 0; i < dwg->block_control.num_entries; i++)
      {
        Dwg_Object *_o
            = dwg_ref_object (dwg, dwg->block_control.block_headers[i]);
        if (_o && _o->type == DWG_TYPE_BLOCK_HEADER && _o != mspace
            && _o != pspace)
          {
            RECORD (BLOCK_RECORD);
            error |= dwg_dxfb_BLOCK_HEADER (dat, _o);
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
  Dwg_Object *obj = calloc (1, sizeof (Dwg_Object));
  obj->parent = hdr->parent;
  obj->index = obj->parent->num_objects;
  dwg_add_ENDBLK (obj);
  obj->tio.entity->ownerhandle = calloc (1, sizeof (Dwg_Object_Ref));
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
dxfb_block_write (Bit_Chain *restrict dat, Dwg_Object *restrict hdr,
                  int *restrict i)
{
  int error = 0;
  Dwg_Object *restrict obj = get_first_owned_block (hdr); // BLOCK
  const Dwg_Object_BLOCK_HEADER *restrict _hdr
      = hdr->tio.object->tio.BLOCK_HEADER;
  Dwg_Object *restrict endblk;
  Dwg_Data *dwg = hdr->parent;

  if (obj)
    error |= dwg_dxfb_object (dat, obj, i);
  else
    {
      LOG_ERROR ("BLOCK_HEADER.block_entity missing");
      return DWG_ERR_INVALIDDWG;
    }
  // skip *Model_Space UNDERLAY's, they are all under ENTITIES
  if (hdr == dwg->header_vars.BLOCK_RECORD_MSPACE->obj)
    obj = NULL;
  else
    obj = get_first_owned_entity (hdr); // first_entity
  while (obj)
    {
      if (obj->supertype == DWG_SUPERTYPE_ENTITY)
        error |= dwg_dxfb_object (dat, obj, i);
      obj = get_next_owned_entity (hdr, obj);
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
  Dwg_Object_BLOCK_CONTROL *_ctrl = &dwg->block_control;
  // Dwg_Object *ctrl = &dwg->object[_ctrl->objid];
  /* let's see if this control block is correct... */
  Dwg_Object_Ref *msref = dwg->header_vars.BLOCK_RECORD_MSPACE;
  // Dwg_Object_Ref *psref = dwg->header_vars.BLOCK_RECORD_PSPACE;
  Dwg_Object *mspace;
  int i = 0;

  // The modelspace header needs to have an block_entity.
  // There are cases (r2010 AEC dwgs) where they don't have one.
  if (msref && msref->obj && msref->obj->type == DWG_TYPE_BLOCK_HEADER
      && msref->obj->tio.object->tio.BLOCK_HEADER->block_entity)
    mspace = msref->obj;
  else
    mspace = _ctrl->model_space->obj; // these two really should be the same

  // If there's no *Model_Space block skip this BLOCKS section.
  // Or try handle 1F with r2000+, 17 with r14
  // obj = get_first_owned_block(hdr);
  // if (!obj)
  //  obj = dwg_resolve_handle(dwg, dwg->header.version >= R_2000 ? 0x1f :
  //  0x17);
  // if (!obj)
  //  return 1;

  SECTION (BLOCKS);
  /* There may be unconnected blocks (not caught by above),
     such as pspace referred by a LAYOUT or DIMENSION, so for simplicity just
     scan all BLOCK_HEADER's. #81 BLOCK_HEADER - LAYOUT - BLOCK - ENDBLK
   */
  {
    for (i = 0; (BITCODE_BL)i < dwg->num_objects; i++)
      {
        if (dwg->object[i].supertype == DWG_SUPERTYPE_OBJECT
            && dwg->object[i].type == DWG_TYPE_BLOCK_HEADER)
          {
            error |= dxfb_block_write (dat, &dwg->object[i], &i);
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
  int i;
  Dwg_Object *mspace = dwg->header_vars.BLOCK_RECORD_MSPACE->obj;

  SECTION (ENTITIES);
  for (i = 0; (BITCODE_BL)i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      if (obj->supertype == DWG_SUPERTYPE_ENTITY && obj->type != DWG_TYPE_BLOCK
          && obj->type != DWG_TYPE_ENDBLK)
        {
          Dwg_Object_Ref *owner = obj->tio.entity->ownerhandle;
          if (!owner || (owner && owner->obj == mspace))
            error |= dwg_dxfb_object (dat, obj, &i);
        }
    }
  ENDSEC ();
  return error;
}

static int
dxfb_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  int i;

  SECTION (OBJECTS);
  for (i = 0; (BITCODE_BL)i < dwg->num_objects; i++)
    {
      const Dwg_Object *restrict obj = &dwg->object[i];
      if (obj->supertype == DWG_SUPERTYPE_OBJECT
          && obj->type != DWG_TYPE_BLOCK_HEADER && !dwg_obj_is_control (obj))
        error |= dwg_dxfb_object (dat, obj, &i);
    }
  ENDSEC ();
  return error;
}

static int
dxfb_preview_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Bit_Chain *pic = (Bit_Chain *)&dwg->picture;
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
  const int minimal = dwg->opts & 0x10;

  loglevel = dwg->opts & 0xf;
  if (dat->from_version == R_INVALID)
    dat->from_version = dat->version;
  fprintf (dat->fh, "AutoCAD Binary DXF\r\n%c%c", 0x1a, 0);
  VALUE_TV (PACKAGE_STRING, 999);

  // a minimal header requires only $ACADVER, $HANDSEED, and then ENTITIES
  // see https://pythonhosted.org/ezdxf/dxfinternals/filestructure.html
  dxfb_header_write (dat, dwg);

  if (!minimal)
    {
      // if downgraded from r2000 to r14, but we still have classes, keep the
      // classes
      if ((dat->from_version >= R_2000 && dwg->num_classes)
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
      SINCE (R_13)
      {
        if (dxfb_objects_write (dat, dwg) >= DWG_ERR_CRITICAL)
          goto fail;
      }
      SINCE (R_2000)
      {
        if (dxfb_preview_write (dat, dwg) >= DWG_ERR_CRITICAL)
          goto fail;
      }
    }
  RECORD (EOF);

  return 0;
fail:
  return 1;
}

#undef IS_PRINT
#undef IS_DXF

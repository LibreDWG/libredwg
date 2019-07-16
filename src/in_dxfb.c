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
 * in_dxfb.c: read Binary DXF to dwg WIP
 * written by Reini Urban
 *
 * See in_dxf.c for the plan. Only the input methods are customized here.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "in_dxf.h"
#include "out_dxf.h"
#include "decode.h"
#include "encode.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];
static long start, end; // stream offsets

typedef struct _dxf_pair
{
  short code;
  enum RES_BUF_VALUE_TYPE type;
  union
  {
    int i;
    char *s;
    long l;
    double d;
  } value;
} Dxf_Pair;

static long num_dxf_objs;  // how many elements are added
static long size_dxf_objs; // how many elements are allocated
static Dxf_Objs *dxf_objs;

// static inline void dxf_skip_ws(Bit_Chain *dat)
//{
//  for (; !dat->chain[dat->byte] || isspace(dat->chain[dat->byte]);
//  dat->byte++) ;
//}

static int
dxf_read_code (Bit_Chain *dat)
{
  char *endptr;
  long num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
  dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
  // dxf_skip_ws(dat);
  if (num > INT_MAX)
    LOG_ERROR ("%s: int overflow %ld (at %lu)", __FUNCTION__, num, dat->byte)
  return (int)num;
}

static int
dxf_read_group (Bit_Chain *dat, int dxf)
{
  char *endptr;
  long num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
  if ((int)num == dxf)
    {
      LOG_HANDLE ("group %d\n", dxf);
      dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
      // dxf_skip_ws(dat);
      return 1;
    }
  return 0;
}

// TODO: binary strings are size prefixed
static void
dxfb_read_string (Bit_Chain *dat, char **string, int len)
{
  if (!len)
    len = strlen (buf);
  if (len)
    {
      if (!string)
        return; // ignore
      if (!*string)
        *string = malloc (len + 1);
      memcpy (*string, buf, len + 1);
    }
}

#define STRADD(field, string)                                                 \
  field = malloc (strlen (string) + 1);                                       \
  strcpy (field, string)

static void
dxf_free_pair (Dxf_Pair *pair)
{
  if (pair->type == VT_STRING || pair->type == VT_BINARY)
    {
      free (pair->value.s);
    }
  free (pair);
  pair = NULL;
}

static Dxf_Pair *
dxf_read_pair (Bit_Chain *dat)
{
  Dxf_Pair *pair = calloc (1, sizeof (Dxf_Pair));
  pair->code = (short)dxf_read_code (dat);
  pair->type = get_base_value_type (pair->code);
  switch (pair->type)
    {
    case VT_STRING:
      dxfb_read_string (dat, &pair->value.s, 0);
      LOG_TRACE ("dxf{%d, %s}\n", (int)pair->code, pair->value.s);
      SINCE (R_2007)
      {
        BITCODE_TU wstr = bit_utf8_to_TU (pair->value.s);
        free (pair->value.s);
        pair->value.s = (char *)wstr;
      }
      break;
    case VT_BOOL:
    case VT_INT8:
    case VT_INT16:
    case VT_INT32:
      pair->value.i = dxf_read_code (dat);
      LOG_TRACE ("dxf{%d, %d}\n", (int)pair->code, pair->value.i);
      break;
    case VT_REAL:
    case VT_POINT3D:
      // dxf_skip_ws(dat);
      sscanf ((char *)&dat->chain[dat->byte], "%lf", &pair->value.d);
      LOG_TRACE ("dxf{%d, %f}\n", pair->code, pair->value.d);
      break;
    case VT_BINARY:
      // read into buf only?
      dxfb_read_string (dat, &pair->value.s, 0);
      // TODO convert %02X to string
      LOG_TRACE ("dxf{%d, %s}\n", (int)pair->code, pair->value.s);
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      dxfb_read_string (dat, NULL, 0);
      sscanf (buf, "%X", &pair->value.i);
      LOG_TRACE ("dxf{%d,%X}\n", (int)pair->code, pair->value.i);
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
      return NULL;
    }
  return pair;
}

#define DXF_CHECK_EOF                                                         \
  if (dat->byte >= dat->size                                                  \
      || (pair->code == 0 && !strcmp (pair->value.s, "EOF")))                 \
  return 1

static int
dxf_skip_comment (Bit_Chain *dat, Dxf_Pair *pair)
{
  while (pair->code == 999)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  return 0;
}

static int
dxf_check_code (Bit_Chain *dat, Dxf_Pair *pair, int code)
{
  if (pair->code == code)
    {
      dxf_skip_comment (dat, pair);
      DXF_CHECK_EOF;
      dxf_free_pair (pair);
      return 1;
    }
  return 0;
}

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION indxfb
#define IS_ENCODER
#define IS_DXF

// for sscanf with BD we need to use %lf not %g
#undef FORMAT_BD
#define FORMAT_BD "%lf"

// add the name/type/dxf combo to some structure for this element.
#define FIELDG(name, type, dxf)                                               \
  dxf_add_field ((Dwg_Object *)obj, #name, #type, dxf)
//#define FIELD(name,type)     dxf_add_field((Dwg_Object*)obj, #name, #type, 0)
#define FIELD_CAST(name, type, cast, dxf) FIELDG (name, cast, dxf)
#define FIELD_TRACE(name, type)
// TODO
#define SUB_FIELD(o, name, type, dxf)                                         \
  dxf_add_field ((Dwg_Object *)obj, #name, #type, dxf)
#define VALUE_TV(value, dxf) dxfb_read_string (dat, (char **)&value, 0)
#define SUBCLASS(text) VALUE_TV (buf, 100);

#define VALUE_TU(value, dxf)                                                  \
  {                                                                           \
    BITCODE_TU wstr;                                                          \
    VALUE_TV (buf, dxf);                                                      \
    wstr = bit_utf8_to_TU (buf);                                              \
    /* TODO get TU length, copy to &value */                                  \
  }

#define FIELD_VALUE(name) _obj->name
#define ANYCODE -1
// TODO read
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  if (dxf && hdlptr)                                                          \
    {                                                                         \
      if (GROUP (dxf))                                                        \
        {                                                                     \
          int i = sscanf ((char *)&dat->chain[dat->byte], "%lX",              \
                          &hdlptr->absolute_ref);                             \
          dat->byte += i;                                                     \
        }                                                                     \
    }
#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  VALUE_HANDLE (_obj->nam, nam, handle_code, dxf)
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  VALUE_HANDLE (_obj->o.nam, nam, handle_code, dxf)
#define HEADER_9(name) GROUP (9)
#define VALUE_H(value, dxf)                                                   \
  {                                                                           \
    Dwg_Object_Ref *ref = value;                                              \
    if (ref && ref->obj)                                                      \
      {                                                                       \
        VALUE_RS (ref->absolute_ref, dxf);                                    \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        VALUE_RS (0, dxf);                                                    \
      }                                                                       \
  }
#define HEADER_H(name, dxf)                                                   \
  HEADER_9 (name);                                                            \
  VALUE_H (dwg->header_vars.name, dxf)

#define HEADER_VALUE(name, type, dxf, value)                                  \
  if (dxf)                                                                    \
    {                                                                         \
      char *headername = NULL;                                                \
      if (GROUP (9))                                                          \
        {                                                                     \
          dxfb_read_string (dat, &headername, 0);                             \
          LOG_TRACE ("9 %s:\n", #name);                                       \
          VALUE (value, type, dxf);                                           \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          FIELDG (name, type, dxf);                                           \
        }                                                                     \
    }
#define HEADER_VAR(name, type, dxf)                                           \
  HEADER_VALUE (name, type, dxf, dwg->header_vars.name)

#define HEADER_3D(name)                                                       \
  HEADER_9 (name);                                                            \
  POINT_3D (name, header_vars.name, 10, 20, 30)
#define HEADER_2D(name)                                                       \
  HEADER_9 (name);                                                            \
  POINT_2D (name, header_vars.name, 10, 20)
#define HEADER_BLL(name, dxf)                                                 \
  HEADER_9 (name);                                                            \
  VALUE_BLL (dwg->header_vars.name, dxf)
#define HEADER_TIMEBLL(name, dxf)                                             \
  HEADER_9 (name);                                                            \
  FIELD_TIMEBLL (name, dxf)

#define SECTION(section)                                                      \
  RECORD (SECTION);                                                           \
  PAIR (2, section)
#define ENDSEC() RECORD (ENDSEC)
#define TABLE(table)                                                          \
  RECORD (TABLE);                                                             \
  PAIR (2, table)
#define ENDTAB() RECORD (ENDTAB)
#define PAIR(n, record)                                                       \
  {                                                                           \
    GROUP (n);                                                                \
    dxfb_read_string (dat, NULL, 0);                                          \
  }
#define RECORD(record) PAIR (0, record)
#define GROUP(dxf) dxf_read_group (dat, dxf)

#define VALUE(value, type, dxf)                                               \
  {                                                                           \
  }

#define HEADER_HANDLE_NAME(name, dxf, section)                                \
  HEADER_9 (name);                                                            \
  {                                                                           \
    Dwg_Object_Ref *ref = dwg->header_vars.name;                              \
    pair = dxf_read_pair (dat);                                               \
    DXF_CHECK_EOF;                                                            \
    if (ref && ref->obj && pair->type == VT_HANDLE)                           \
      {                                                                       \
        /* TODO: set the table handle */                                      \
        ; /*ref->obj->handle.absolute_ref = pair->value.i; */                 \
        /*ref->obj->tio.object->tio.section->name = strdup(pair->value.s);*/  \
      }                                                                       \
  }
// FIXME
#define HANDLE_NAME(id, dxf)                                                  \
  {                                                                           \
    Dwg_Object_Ref *ref = id;                                                 \
    char *tmp;                                                                \
    Dwg_Object *o = ref ? ref->obj : NULL;                                    \
    VALUE_TV (tmp, dxf);                                                      \
  }

#define FIELD_DATAHANDLE(name, code, dxf) FIELD_HANDLE (name, code, dxf)
#define FIELD_HANDLE_N(name, vcount, handle_code, dxf)                        \
  FIELD_HANDLE (name, handle_code, dxf)

#define HEADER_RC(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  FIELDG (name, RC, dxf)
#define HEADER_RS(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  FIELDG (name, RS, dxf)
#define HEADER_RL(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  FIELDG (name, RL, dxf)
#define HEADER_RD(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  FIELDG (name, RD, dxf)
#define HEADER_RLL(name, dxf)                                                 \
  HEADER_9 (name);                                                            \
  FIELDG (name, RLL, dxf)
#define HEADER_TV(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  VALUE_TV (_obj->name, dxf)
#define HEADER_TU(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  VALUE_TU (_obj->name, dxf)
#define HEADER_T(name, dxf)                                                   \
  HEADER_9 (name);                                                            \
  VALUE_T (_obj->name, dxf)
#define HEADER_B(name, dxf)                                                   \
  HEADER_9 (name);                                                            \
  FIELDG (name, B, dxf)
#define HEADER_BS(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  FIELDG (name, BS, dxf)
#define HEADER_BL(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  FIELDG (name, BL, dxf)
#define HEADER_BD(name, dxf)                                                  \
  HEADER_9 (name);                                                            \
  FIELDG (name, BD, dxf)

#define VALUE_B(value, dxf) VALUE (value, RC, dxf)
#define VALUE_BB(value, dxf) VALUE (value, RC, dxf)
#define VALUE_3B(value, dxf) VALUE (value, RC, dxf)
#define VALUE_BS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_BL(value, dxf) VALUE (value, BL, dxf)
#define VALUE_BLL(value, dxf) VALUE (value, RLL, dxf)
#define VALUE_BD(value, dxf) VALUE (value, RD, dxf)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RLL(value, dxf) VALUE (value, RLL, dxf)
#define VALUE_MC(value, dxf) VALUE (value, MC, dxf)
#define VALUE_MS(value, dxf) VALUE (value, MS, dxf)
#define FIELD_B(name, dxf) FIELDG (name, B, dxf)
#define FIELD_BB(name, dxf) FIELDG (name, BB, dxf)
#define FIELD_3B(name, dxf) FIELDG (name, 3B, dxf)
#define FIELD_BS(name, dxf) FIELDG (name, BS, dxf)
#define FIELD_BL(name, dxf) FIELDG (name, BL, dxf)
#define FIELD_BLL(name, dxf) FIELDG (name, BLL, dxf)
#define FIELD_BD(name, dxf) FIELDG (name, BD, dxf)
#define FIELD_RC(name, dxf) FIELDG (name, RC, dxf)
#define FIELD_RS(name, dxf) FIELDG (name, RS, dxf)
#define FIELD_RD(name, dxf) FIELDG (name, RD, dxf)
#define FIELD_RL(name, dxf) FIELDG (name, RL, dxf)
#define FIELD_RLL(name, dxf) FIELDG (name, RLL, dxf)
#define FIELD_MC(name, dxf) FIELDG (name, MC, dxf)
#define FIELD_MS(name, dxf) FIELDG (name, MS, dxf)
#define FIELD_TF(name, len, dxf) VALUE_TV (_obj->name, dxf)
#define FIELD_TFF(name, len, dxf) VALUE_TV (_obj->name, dxf)
#define FIELD_TV(name, dxf)                                                   \
  if (_obj->name != NULL && dxf != 0)                                         \
    {                                                                         \
      GROUP (dxf);                                                            \
      VALUE_TV (_obj->name, dxf);                                             \
    }
#define FIELD_TU(name, dxf)                                                   \
  if (_obj->name != NULL && dxf != 0)                                         \
    {                                                                         \
      GROUP (dxf);                                                            \
      VALUE_TU ((BITCODE_TU)_obj->name, dxf);                                 \
    }
#define FIELD_T(name, dxf)                                                    \
  {                                                                           \
    if (dat->version >= R_2007)                                               \
      {                                                                       \
        FIELD_TU (name, dxf);                                                 \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_TV (name, dxf);                                                 \
      }                                                                       \
  }
#define VALUE_T(value, dxf)                                                   \
  {                                                                           \
    if (dat->version >= R_2007)                                               \
      {                                                                       \
        VALUE_TU (value, dxf);                                                \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        VALUE_TV (value, dxf);                                                \
      }                                                                       \
  }
#define FIELD_BT(name, dxf) FIELDG (name, BT, dxf);
#define FIELD_4BITS(name, dxf) FIELDG (name, 4BITS, dxf)
#define FIELD_BE(name, dxf) FIELD_3RD (name, dxf)
#define FIELD_DD(name, _default, dxf) FIELD_BD (name, dxf)
#define FIELD_2DD(name, d1, d2, dxf)                                          \
  {                                                                           \
    FIELD_DD (name.x, d1, dxf);                                               \
    FIELD_DD (name.y, d2, dxf + 10);                                          \
  }
#define FIELD_3DD(name, def, dxf)                                             \
  {                                                                           \
    FIELD_DD (name.x, FIELD_VALUE (def.x), dxf);                              \
    FIELD_DD (name.y, FIELD_VALUE (def.y), dxf + 10);                         \
    FIELD_DD (name.z, FIELD_VALUE (def.z), dxf + 20);                         \
  }
#define FIELD_2RD(name, dxf)                                                  \
  {                                                                           \
    FIELDG (name.x, RD, dxf);                                                 \
    FIELDG (name.y, RD, dxf + 10);                                            \
  }
#define FIELD_2BD(name, dxf)                                                  \
  {                                                                           \
    FIELDG (name.x, BD, dxf);                                                 \
    FIELDG (name.y, BD, dxf + 10);                                            \
  }
#define FIELD_2BD_1(name, dxf)                                                \
  {                                                                           \
    FIELDG (name.x, BD, dxf);                                                 \
    FIELDG (name.y, BD, dxf + 1);                                             \
  }
#define FIELD_3RD(name, dxf)                                                  \
  {                                                                           \
    FIELDG (name.x, RD, dxf);                                                 \
    FIELDG (name.y, RD, dxf + 10);                                            \
    FIELDG (name.z, RD, dxf + 20);                                            \
  }
#define FIELD_3BD(name, dxf)                                                  \
  {                                                                           \
    FIELDG (name.x, BD, dxf);                                                 \
    FIELDG (name.y, BD, dxf + 10);                                            \
    FIELDG (name.z, BD, dxf + 20);                                            \
  }
#define FIELD_3BD_1(name, dxf)                                                \
  {                                                                           \
    FIELDG (name.x, BD, dxf);                                                 \
    FIELDG (name.y, BD, dxf + 1);                                             \
    FIELDG (name.z, BD, dxf + 2);                                             \
  }
#define FIELD_3DPOINT(name, dxf) FIELD_3BD (name, dxf)
#define FIELD_CMC(color, dxf1, dxf2) VALUE_RS (_obj->color.index, dxf1)
#define SUB_FIELD_CMC(o, color, dxf1, dxf2)                                   \
  VALUE_RS (_obj->o.color.index, dxf1)
// TODO: rgb
#define FIELD_TIMEBLL(name, dxf)                                              \
  GROUP (dxf);                                                                \
  sscanf ((char *)&dat->chain[dat->byte], "%u.%u", &_obj->name.days,          \
          &_obj->name.ms)
#define HEADER_CMC(name, dxf)                                                 \
  HEADER_9 (name);                                                            \
  VALUE_RS (dwg->header_vars.name.index, dxf)

#define POINT_3D(name, var, c1, c2, c3)                                       \
  {                                                                           \
    pair = dxf_read_pair (dat);                                               \
    DXF_CHECK_EOF;                                                            \
    if (pair && pair->code == c1)                                             \
      {                                                                       \
        dwg->var.x = pair->value.d;                                           \
        dxf_free_pair (pair);                                                 \
        pair = dxf_read_pair (dat);                                           \
        if (pair && pair->code == c2)                                         \
          dwg->var.y = pair->value.d;                                         \
        dxf_free_pair (pair);                                                 \
        pair = dxf_read_pair (dat);                                           \
        if (pair && pair->code == c3)                                         \
          dwg->var.z = pair->value.d;                                         \
        dxf_free_pair (pair);                                                 \
      }                                                                       \
  }
#define POINT_2D(name, var, c1, c2)                                           \
  {                                                                           \
    pair = dxf_read_pair (dat);                                               \
    DXF_CHECK_EOF;                                                            \
    if (pair && pair->code == c1)                                             \
      {                                                                       \
        dwg->var.x = pair->value.d;                                           \
        dxf_free_pair (pair);                                                 \
        pair = dxf_read_pair (dat);                                           \
        if (pair && pair->code == c2)                                         \
          dwg->var.y = pair->value.d;                                         \
        dxf_free_pair (pair);                                                 \
      }                                                                       \
  }

// FIELD_VECTOR_N(name, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'name'.
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          sscanf ((char *)&dat->chain[dat->byte],                             \
                  #nam ": " FORMAT_##type ",\n", &_obj->nam[vcount]);         \
        }                                                                     \
    }

#define FIELD_VECTOR_T(nam, size, dxf)                                        \
  if (dxf)                                                                    \
    {                                                                         \
      PRE (R_2007)                                                            \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          VALUE_TV (_obj->nam[vcount], dxf);                                  \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          VALUE_TU (_obj->nam[vcount], dxf);                                  \
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
  FIELD_2RD (nam[0], dxf);                                                    \
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2DD (nam[vcount], FIELD_VALUE (nam[vcount - 1].x),                \
                 FIELD_VALUE (nam[vcount - 1].y), dxf);                       \
    }

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_3DPOINT (nam[vcount], dxf);                                   \
        }                                                                     \
    }

#define HANDLE_VECTOR_N(name, size, code, dxf)                                \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIELD_HANDLE_N (name[vcount], vcount, code, dxf);                   \
        }                                                                     \
    }

#define HANDLE_VECTOR(name, sizefield, code, dxf)                             \
  HANDLE_VECTOR_N (name, FIELD_VALUE (sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  FIELDG (num_inserts, type, dxf)

#define FIELD_XDATA(name, size)

#define REACTORS(code)                                                        \
  pair = dxf_read_pair (dat);                                                 \
  if (dxf_check_code (dat, pair, 102))                                        \
    { /* {ACAD_REACTORS */                                                    \
      dxf_free_pair (pair);                                                   \
      vcount = 0;                                                             \
      while (dxf_check_code (dat, pair, 330))                                 \
        {                                                                     \
          vcount++;                                                           \
          obj->tio.object->num_reactors++;                                    \
          VALUE_HANDLE (obj->tio.object->reactors[vcount], reactors, code,    \
                        330);                                                 \
        }                                                                     \
      dxf_check_code (dat, pair, 102);                                        \
    }

#define ENT_REACTORS(code)                                                    \
  pair = dxf_read_code (dat);                                                 \
  if (dxf_check_code (dat, pair, 102))                                        \
    { /* {ACAD_REACTORS */                                                    \
      dxf_free_pair (pair);                                                   \
      vcount = 0;                                                             \
      while (dxf_check_code (dat, pair, 330))                                 \
        {                                                                     \
          vcount++;                                                           \
          _obj->num_reactors++;                                               \
          VALUE_HANDLE (obj->tio.entity->reactors[vcount], reactors, code,    \
                        330);                                                 \
        }                                                                     \
      dxf_check_code (dat, pair, 102);                                        \
    }

#define XDICOBJHANDLE(code)
#define ENT_XDICOBJHANDLE(code)

#define COMMON_ENTITY_HANDLE_DATA                                             \
  SINCE (R_13) { error |= dxfb_common_entity_handle_data (dat, obj); }
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token)                                                     \
  static int dwg_indxfb_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Bit_Chain *hdl_dat = dat;                                                 \
    Bit_Chain *str_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_Entity *_ent;                                                  \
    Dxf_Pair *pair;                                                           \
    int error = 0;                                                            \
    LOG_INFO ("Entity " #token ":\n")                                         \
    _ent = obj->tio.entity;                                                   \
    _obj = ent = _ent->tio.token;                                             \
    obj->fixedtype = DWG_TYPE_##token;                                        \
    LOG_TRACE ("Entity handle: %d.%d.%lX\n", obj->handle.code,                \
               obj->handle.size, obj->handle.value)

#define DWG_ENTITY_END                                                        \
  return error;                                                               \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_indxfb_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Bit_Chain *hdl_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_##token *_obj;                                                 \
    Dxf_Pair *pair;                                                           \
    int error = 0;                                                            \
    obj->fixedtype = DWG_TYPE_##token;                                        \
    LOG_INFO ("Object " #token ":\n")                                         \
    _obj = obj->tio.object->tio.token;                                        \
    LOG_TRACE ("Object handle: %d.%d.%lX\n", obj->handle.code,                \
               obj->handle.size, obj->handle.value)

#define DWG_OBJECT_END                                                        \
  return error;                                                               \
  }

static int
dxfb_common_entity_handle_data (Bit_Chain *dat, Dwg_Object *obj)
{
  (void)dat;
  (void)obj;
  return 0;
}

#include "dwg.spec"

/* returns 0 if object was handled,
   DWG_ERR_UNHANDLEDCLASS or some other error otherwise
 */
static int
dwg_dxfb_variable_type (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
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

static int
dwg_indxfb_object (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error = 0;
  if (!obj || !obj->parent)
    return DWG_ERR_INTERNALERROR;

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_indxfb_TEXT (dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_indxfb_ATTRIB (dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_indxfb_ATTDEF (dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_indxfb_BLOCK (dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_indxfb_ENDBLK (dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_indxfb_SEQEND (dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_indxfb_INSERT (dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_indxfb_MINSERT (dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_indxfb_VERTEX_2D (dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_indxfb_VERTEX_3D (dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_indxfb_VERTEX_MESH (dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_indxfb_VERTEX_PFACE (dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_indxfb_VERTEX_PFACE_FACE (dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_indxfb_POLYLINE_2D (dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_indxfb_POLYLINE_3D (dat, obj);
    case DWG_TYPE_ARC:
      return dwg_indxfb_ARC (dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_indxfb_CIRCLE (dat, obj);
    case DWG_TYPE_LINE:
      return dwg_indxfb_LINE (dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_indxfb_DIMENSION_ORDINATE (dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_indxfb_DIMENSION_LINEAR (dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_indxfb_DIMENSION_ALIGNED (dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_indxfb_DIMENSION_ANG3PT (dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_indxfb_DIMENSION_ANG2LN (dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_indxfb_DIMENSION_RADIUS (dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_indxfb_DIMENSION_DIAMETER (dat, obj);
    case DWG_TYPE_POINT:
      return dwg_indxfb_POINT (dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_indxfb__3DFACE (dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_indxfb_POLYLINE_PFACE (dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_indxfb_POLYLINE_MESH (dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_indxfb_SOLID (dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_indxfb_TRACE (dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_indxfb_SHAPE (dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_indxfb_VIEWPORT (dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_indxfb_ELLIPSE (dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_indxfb_SPLINE (dat, obj);
    case DWG_TYPE_REGION:
      return dwg_indxfb_REGION (dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_indxfb__3DSOLID (dat, obj);
    case DWG_TYPE_BODY:
      return dwg_indxfb_BODY (dat, obj);
    case DWG_TYPE_RAY:
      return dwg_indxfb_RAY (dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_indxfb_XLINE (dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_indxfb_DICTIONARY (dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_indxfb_MTEXT (dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_indxfb_LEADER (dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_indxfb_TOLERANCE (dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_indxfb_MLINE (dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_indxfb_BLOCK_CONTROL (dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      return dwg_indxfb_BLOCK_HEADER (dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_indxfb_LAYER_CONTROL (dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_indxfb_LAYER (dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_indxfb_STYLE_CONTROL (dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_indxfb_STYLE (dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_indxfb_LTYPE_CONTROL (dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_indxfb_LTYPE (dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_indxfb_VIEW_CONTROL (dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_indxfb_VIEW (dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_indxfb_UCS_CONTROL (dat, obj);
    case DWG_TYPE_UCS:
      return dwg_indxfb_UCS (dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_indxfb_VPORT_CONTROL (dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_indxfb_VPORT (dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_indxfb_APPID_CONTROL (dat, obj);
    case DWG_TYPE_APPID:
      return dwg_indxfb_APPID (dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_indxfb_DIMSTYLE_CONTROL (dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_indxfb_DIMSTYLE (dat, obj);
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_indxfb_VPORT_ENTITY_CONTROL (dat, obj);
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_indxfb_VPORT_ENTITY_HEADER (dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_indxfb_GROUP (dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_indxfb_MLINESTYLE (dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_indxfb_OLE2FRAME (dat, obj);
    case DWG_TYPE_DUMMY:
      return 0; // dwg_indxfb_DUMMY(dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_indxfb_LONG_TRANSACTION (dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_indxfb_LWPOLYLINE (dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_indxfb_HATCH (dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_indxfb_XRECORD (dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_indxfb_PLACEHOLDER (dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_indxfb_OLEFRAME (dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own section\n");
      // dwg_indxfb_VBA_PROJECT(dat, obj);
      return DWG_ERR_INVALIDTYPE;
    case DWG_TYPE_LAYOUT:
      return dwg_indxfb_LAYOUT (dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_indxfb_PROXY_ENTITY (dat, obj);
    case DWG_TYPE_PROXY_OBJECT:
      return dwg_indxfb_PROXY_OBJECT (dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return dwg_indxfb_LAYOUT (dat, obj);
        }
      /* > 500 */
      if (DWG_ERR_UNHANDLEDCLASS
          & (error = dwg_dxfb_variable_type (obj->parent, dat, obj)))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (i >= 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity (klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              return error; // dwg_indxfb_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              return error; // dwg_indxfb_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN ("Unknown object, skipping eed/reactors/xdic");
              SINCE (R_2000){
                LOG_INFO ("Object bitsize: %u\n", obj->bitsize)
              } LOG_INFO ("Object handle: %d.%d.%lX\n", obj->handle.code,
                          obj->handle.size, obj->handle.value);
            }
        }
    }
  return error;
}

#define DXF_CHECK_ENDSEC                                                      \
  if (dat->byte >= dat->size || pair->code == 0)                              \
  return 0
#define DXF_BREAK_ENDSEC                                                      \
  if (dat->byte >= dat->size                                                  \
      || (pair->code == 0 && !strcmp (pair->value.s, "ENDSEC")))              \
  break
#define DXF_RETURN_ENDSEC(what)                                               \
  if (dat->byte >= dat->size                                                  \
      || (pair->code == 0 && !strcmp (pair->value.s, "ENDSEC")))              \
    {                                                                         \
      dxf_free_pair (pair);                                                   \
      return what;                                                            \
    }

static int
dxf_expect_code (Bit_Chain *dat, Dxf_Pair *pair, int code)
{
  while (pair->code != code)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      dxf_skip_comment (dat, pair);
      DXF_CHECK_EOF;
      if (pair->code != code)
        {
          LOG_ERROR ("Expecting DXF code %d, got %d (at %lu)", code,
                     pair->code, dat->byte);
        }
    }
  return 0;
}

// see
// https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxfb_header_read (Bit_Chain *dat, Dwg_Data *dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  const int minimal = dwg->opts & 0x10;
  double ms;
  char *codepage;
  Dxf_Pair *pair = dxf_read_pair (dat);

// define fields (unordered)
#include "header_variables_dxf.spec"

  while (pair->code != 0)
    {
      pair = dxf_read_pair (dat);
      DXF_BREAK_ENDSEC;

      // TODO find name in header struct and set value

      dxf_free_pair (pair);
    }
  dxf_free_pair (pair);

  // TODO: convert DWGCODEPAGE string to header.codepage number
  if (!strcmp (_obj->DWGCODEPAGE, "ANSI_1252"))
    dwg->header.codepage = 30;

  return 0;
}

static int
dxfb_classes_read (Bit_Chain *dat, Dwg_Data *dwg)
{
  BITCODE_BL i;
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Class *klass;

  while (1)
    { // read next class
      // add class (see decode)
      i = dwg->num_classes;
      if (i == 0)
        dwg->dwg_class = malloc (sizeof (Dwg_Class));
      else
        dwg->dwg_class
            = realloc (dwg->dwg_class, (i + 1) * sizeof (Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR ("Out of memory");
          return DWG_ERR_OUTOFMEM;
        }

      klass = &dwg->dwg_class[i];
      memset (klass, 0, sizeof (Dwg_Class));

      while (pair->code != 0)
        { // read until next 0 CLASS
          pair = dxf_read_pair (dat);
          switch (pair->code)
            {
            case 1:
              STRADD (klass->dxfname, pair->value.s);
              break;
            case 2:
              STRADD (klass->cppname, pair->value.s);
              break;
            case 3:
              STRADD (klass->appname, pair->value.s);
              break;
            case 90:
              klass->proxyflag = pair->value.l;
              break;
            case 91:
              klass->num_instances = pair->value.l;
              break;
            case 280:
              klass->wasazombie = (BITCODE_B)pair->value.i;
              break;
            case 281:
              klass->item_class_id = pair->value.i ? 0x1f3 : 0x1f2;
              break;
            default:
              LOG_WARN ("Unknown DXF code for class[%d].%d", i, pair->code);
              break;
            }
          dxf_free_pair (pair);
        }
      DXF_RETURN_ENDSEC (0); // next class or ENDSEC
      if (strcmp (pair->value.s, "CLASS"))
        { // or something else
          LOG_ERROR ("Unexpexted DXF 0 %s at class[%d]", pair->value.s, i);
          return DWG_ERR_CLASSESNOTFOUND;
        }
      dwg->num_classes++;
    }
  return 0;
}

static int
dxfb_tables_read (Bit_Chain *dat, Dwg_Data *dwg)
{
  (void)dwg;

  SECTION (TABLES);
  TABLE (VPORT);
  //...
  ENDTAB ();
  ENDSEC ();
  return 0;
}

static int
dxfb_blocks_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;

  SECTION (BLOCKS);
  //...
  ENDSEC ();
  return 0;
}

static int
dxfb_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair;
  Dwg_Object *obj = NULL;
  (void)dwg;

  SECTION (ENTITIES);
  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      dxf_expect_code (dat, pair, 0);
      DXF_CHECK_EOF;
      dwg_indxfb_object (dat, obj); // TODO obj must be already created here
    }
  ENDSEC ();
  return 0;
}

static int
dxfb_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair;
  Dwg_Object *obj = NULL;
  (void)dwg;

  SECTION (OBJECTS);
  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      dxf_expect_code (dat, pair, 0);
      DXF_CHECK_EOF;
      dwg_indxfb_object (dat, obj); // TODO obj must be already created here
    }
  ENDSEC ();
  return 0;
}

static int
dxfb_preview_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;
  SECTION (THUMBNAILIMAGE);
  // VALUE_RL(pic->size, 90);
  // VALUE_BINARY(pic->chain, pic->size, 310);
  ENDSEC ();
  return 0;
}

int
dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  const int minimal = dwg->opts & 0x10;
  Dxf_Pair *pair;
  // warn if minimal != 0
  // struct Dwg_Header *obj = &dwg->header;
  loglevel = dwg->opts & 0xf;

  num_dxf_objs = 0;
  size_dxf_objs = 1000;
  dxf_objs = malloc (1000 * sizeof (Dxf_Objs));

  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      dxf_expect_code (dat, pair, 0);
      DXF_CHECK_EOF;
      if (!strcmp (pair->value.s, "SECTION"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          dxf_expect_code (dat, pair, 2);
          DXF_CHECK_EOF;
          if (!strcmp (pair->value.s, "HEADER"))
            {
              dxf_free_pair (pair);
              dxfb_header_read (dat, dwg);
            }
          else if (!strcmp (pair->value.s, "CLASSES"))
            {
              dxf_free_pair (pair);
              dxfb_classes_read (dat, dwg);
            }
          else if (!strcmp (pair->value.s, "TABLES"))
            {
              dxf_free_pair (pair);
              dxfb_tables_read (dat, dwg);
            }
          else if (!strcmp (pair->value.s, "BLOCKS"))
            {
              dxf_free_pair (pair);
              dxfb_blocks_read (dat, dwg);
            }
          else if (!strcmp (pair->value.s, "ENTITIES"))
            {
              dxf_free_pair (pair);
              dxfb_entities_read (dat, dwg);
            }
          else if (!strcmp (pair->value.s, "OBJECTS"))
            {
              dxf_free_pair (pair);
              dxfb_objects_read (dat, dwg);
            }
          if (!strcmp (pair->value.s, "THUMBNAIL"))
            {
              dxf_free_pair (pair);
              dxfb_preview_read (dat, dwg);
            }
        }
    }
  return dwg->num_objects ? 1 : 0;
}

#undef IS_ENCODE
#undef IS_DXF

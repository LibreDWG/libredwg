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
 * in_dxf.c: read ascii DXF
 * written by Reini Urban
 */

#include "config.h"
#ifdef __STDC_ALLOC_LIB__
# define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
# define _USE_BSD 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
//#include <ctype.h>
#include <math.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "in_dxf.h"
#include "out_dxf.h"
#include "decode.h"
#include "encode.h"
#include "dynapi.h"

#ifndef _DWG_API_H_
Dwg_Object *dwg_obj_generic_to_object (const void *restrict obj,
                                       int *restrict error);
#endif

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];
static long start, end; // stream offsets
static array_hdls *header_hdls = NULL;

#define ARRAY_SIZE(arr) (sizeof (arr) / sizeof (arr[0]))

static long num_dxf_objs;  // how many elements are added
static long size_dxf_objs; // how many elements are allocated
static Dxf_Objs *dxf_objs;

static inline void
dxf_skip_ws (Bit_Chain *dat)
{
  for (;
       (!dat->chain[dat->byte] ||
        dat->chain[dat->byte] == ' ' ||
        dat->chain[dat->byte] == '\t' ||
        dat->chain[dat->byte] == '\r');
       dat->byte++)
    ;
}

static inline void
dxf_skip_ws_nl (Bit_Chain *dat)
{
  for (;
       (!dat->chain[dat->byte] ||
        dat->chain[dat->byte] == ' ' ||
        dat->chain[dat->byte] == '\n' ||
        dat->chain[dat->byte] == '\t' ||
        dat->chain[dat->byte] == '\r');
       dat->byte++)
    ;
}

static int
dxf_read_code (Bit_Chain *dat)
{
  char *endptr;
  long num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
  dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
  if (dat->chain[dat->byte] == '\r')
    dat->byte++;
  if (dat->chain[dat->byte] == '\n')
    dat->byte++;
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
      dxf_skip_ws_nl (dat);
      return 1;
    }
  return 0;
}

static void
dxf_read_string (Bit_Chain *dat, char **string)
{
  int i;
  dxf_skip_ws (dat);
  for (i = 0; dat->chain[dat->byte] != '\n'; dat->byte++)
    {
      buf[i++] = dat->chain[dat->byte];
    }
  if (i && buf[i-1] == '\r')
    buf[i-1] = '\0';
  else
    buf[i] = '\0';
  dat->byte++;
  //dxf_skip_ws (dat);
  if (!string)
    return; // ignore, just advanced dat

  if (!*string)
    *string = malloc (strlen (buf) + 1);
  else
    *string = realloc (*string, strlen (buf) + 1);
  strcpy (*string, buf);
}

#define STRADD(field, string)                                                 \
  field = malloc (strlen (string) + 1);                                       \
  strcpy (field, string)

static void
dxf_free_pair (Dxf_Pair *pair)
{
  if (!pair)
    return;
  if (pair->type == VT_STRING || pair->type == VT_BINARY)
    {
      free (pair->value.s);
      pair->value.s = NULL;
    }
  free (pair);
  pair = NULL;
}

static Dxf_Pair *
ATTRIBUTE_MALLOC
dxf_read_pair (Bit_Chain *dat)
{
  Dxf_Pair *pair = calloc (1, sizeof (Dxf_Pair));
  pair->code = (short)dxf_read_code (dat);
  pair->type = get_base_value_type (pair->code);
  switch (pair->type)
    {
    case VT_STRING:
      dxf_read_string (dat, &pair->value.s);
      LOG_TRACE ("  dxf (%d, \"%s\")\n", (int)pair->code, pair->value.s);
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
      LOG_TRACE ("  dxf (%d, %d)\n", (int)pair->code, pair->value.i);
      break;
    case VT_REAL:
    case VT_POINT3D:
      dxf_skip_ws (dat);
      {
        const char *str = (const char *)&dat->chain[dat->byte];
        char *endp;
        pair->value.d = strtod (str, &endp);
        if (endp)
          dat->byte += endp - str;
        //sscanf ((char *)&dat->chain[dat->byte], "%lf", &pair->value.d);
      }
      LOG_TRACE ("  dxf (%d, %f)\n", pair->code, pair->value.d);
      break;
    case VT_BINARY:
      // read into buf only?
      dxf_read_string (dat, &pair->value.s);
      // TODO convert %02X to string
      LOG_TRACE ("  dxf (%d, %s)\n", (int)pair->code, pair->value.s);
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      dxf_read_string (dat, NULL);
      sscanf (buf, "%X", &pair->value.u);
      LOG_TRACE ("  dxf (%d, %X)\n", (int)pair->code, pair->value.u);
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
      return NULL;
    }
  return pair;
}

#define DXF_CHECK_EOF                                         \
  if (dat->byte >= dat->size                                  \
      || (pair->code == 0 && !strcmp (pair->value.s, "EOF"))) \
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

#define ACTION indxf
#define IS_ENCODER
#define IS_DXF

// for sscanf with BD we need to use %lf not %g
#undef FORMAT_BD
#define FORMAT_BD "%lf"

#define FIELD(name, type) dxf_add_field ((Dwg_Object *)obj, #name, #type, 0)
#define FIELDG(name, type, dxf)                                               \
  dxf_add_field ((Dwg_Object *)obj, #name, #type, dxf)
#define FIELD_CAST(name, type, cast, dxf) FIELDG (name, cast, dxf)
#define FIELD_TRACE(name, type)
// TODO
#define SUB_FIELD(o, name, type, dxf)                                         \
  dxf_add_field ((Dwg_Object *)obj, #name, #type, dxf)
#define VALUE_TV(value, dxf) dxf_read_string (dat, (char **)&value)
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
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  if (dxf && hdlptr)                                                          \
    {                                                                         \
      if (GROUP (dxf))                                                        \
        {                                                                     \
          int i = sscanf ((char *)&dat->chain[dat->byte], "%X",               \
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
      char *headername;                                                       \
      if (GROUP (9))                                                          \
        {                                                                     \
          dxf_read_string (dat, &headername);                                 \
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
    dxf_read_string (dat, NULL);                                              \
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
#define FIELD_VECTOR_N(name, type, size, dxf)                                 \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          sscanf ((char *)&dat->chain[dat->byte],                             \
                  #name ": " FORMAT_##type ",\n", &_obj->name[vcount]);       \
        }                                                                     \
    }

#define FIELD_VECTOR_T(name, size, dxf)                                       \
  if (dxf)                                                                    \
    {                                                                         \
      PRE (R_2007)                                                            \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          VALUE_TV (_obj->name[vcount], dxf);                                 \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          VALUE_TU (_obj->name[vcount], dxf);                                 \
      }                                                                       \
    }

#define FIELD_VECTOR(name, type, size, dxf)                                   \
  FIELD_VECTOR_N (name, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(name, size, dxf)                                     \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_2RD (name[vcount], dxf);                                      \
        }                                                                     \
    }

#define FIELD_2DD_VECTOR(name, size, dxf)                                     \
  FIELD_2RD (name[0], dxf);                                                   \
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2DD (name[vcount], FIELD_VALUE (name[vcount - 1].x),              \
                 FIELD_VALUE (name[vcount - 1].y), dxf);                      \
    }

#define FIELD_3DPOINT_VECTOR(name, size, dxf)                                 \
  if (dxf)                                                                    \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIELD_3DPOINT (name[vcount], dxf);                                  \
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
  SINCE (R_13) { error |= dxf_common_entity_handle_data (dat, obj); }
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token)                                                     \
  static int dwg_indxf_##token (Bit_Chain *restrict dat,                      \
                                Dwg_Object *restrict obj)                     \
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
    LOG_TRACE ("Entity handle: " FORMAT_H "\n", ARGS_H(obj->handle))

#define DWG_ENTITY_END                                                        \
  return error;                                                               \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_indxf_##token (Bit_Chain *restrict dat,                      \
                                Dwg_Object *restrict obj)                     \
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
    LOG_TRACE ("Object handle: " FORMAT_H "\n", ARGS_H(obj->handle))

#define DWG_OBJECT_END                                                        \
  return error;                                                               \
  }

/* Store all handle fieldnames and string values into this flexarray.
   We need strdup'd copies, the dxf input will be freed.
 */
array_hdls *
array_push (array_hdls *restrict hdls, char *restrict field, char *restrict name)
{
  uint32_t i = hdls->nitems;
  if (i >= hdls->size)
    {
      hdls->size += 16;
      hdls = realloc (hdls, 8 + (hdls->size * sizeof (struct array_hdl)));
    }
  hdls->nitems = i+1;
  hdls->items[i].field = strdup (field);
  hdls->items[i].name = strdup (name);
  return hdls;
}

// TODO: we have only one obj per DXF context/section. simplify
void
dxf_add_field (Dwg_Object *restrict obj, const char *restrict name,
               const char *restrict type, int dxf)
{
  int i;
  Dxf_Objs *found = NULL;
  Dxf_Field *field;

  // array of [obj -> [fields], ...]
  if (num_dxf_objs >= size_dxf_objs)
    {
      size_dxf_objs += 1000;
      dxf_objs = realloc (dxf_objs, size_dxf_objs * sizeof (Dxf_Objs));
      memset (&dxf_objs[num_dxf_objs], 0, 1000 * sizeof (Dxf_Objs));
    }
  // search obj ptr in array
  for (i = 0; i < num_dxf_objs; i++)
    {
      if (dxf_objs[i].obj == obj)
        {
          found = &dxf_objs[i];
          break;
        }
    }
  if (!found) // new object (first field)
    {
      found = &dxf_objs[num_dxf_objs];
      found->obj = obj;
      found->num_fields = 1;
      found->size_fields = 16;
      found->fields = (Dxf_Field *)calloc (16, sizeof (Dxf_Field));
      num_dxf_objs++;
    }
  /*if (!found->fields)
    {
      found->num_fields = 1;
      found->size_fields = 16;
      found->fields = calloc(found->size_fields, sizeof(Dxf_Field));
    }
    else */
  if (found->num_fields > found->size_fields)
    {
      found->size_fields += 16;
      found->fields
          = realloc (found->fields, found->size_fields * sizeof (Dxf_Field));
      memset (&found->fields[found->num_fields], 0, 16 * sizeof (Dxf_Field));
    }

  // fill the new field
  field = &found->fields[found->num_fields];
  field->name = malloc (strlen (name) + 1);
  strcpy (field->name, (char *)name);
  field->type = malloc (strlen (type) + 1);
  strcpy (field->type, (char *)type);
  field->dxf = dxf;
  found->num_fields++;
}

// TODO: we have only one obj per DXF context/section. simplify
Dxf_Field *
dxf_search_field (Dwg_Object *restrict obj, const char *restrict name,
                  const char *restrict type, int dxf)
{
  int i;
  Dxf_Objs *found = NULL;

  // first search obj ptr in array
  for (i = 0; i < num_dxf_objs; i++)
    {
      if (dxf_objs[i].obj == obj)
        {
          found = &dxf_objs[i];
          break;
        }
    }
  if (!found)
    {
      LOG_ERROR ("obj not found\n");
      return NULL;
    }
  // then search field
  for (i = 0; i < found->num_fields; i++)
    {
      if (strEQ (found->fields[i].name, name))
        return &found->fields[i];
    }
  return NULL;
}

static int
dxf_common_entity_handle_data (Bit_Chain *restrict dat,
                               Dwg_Object *restrict obj)
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
dwg_indxf_variable_type (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
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
dwg_indxf_object (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error = 0;
  if (!obj || !obj->parent)
    return DWG_ERR_INTERNALERROR;

  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_indxf_TEXT (dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_indxf_ATTRIB (dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_indxf_ATTDEF (dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_indxf_BLOCK (dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_indxf_ENDBLK (dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_indxf_SEQEND (dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_indxf_INSERT (dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_indxf_MINSERT (dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_indxf_VERTEX_2D (dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_indxf_VERTEX_3D (dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_indxf_VERTEX_MESH (dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_indxf_VERTEX_PFACE (dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_indxf_VERTEX_PFACE_FACE (dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_indxf_POLYLINE_2D (dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_indxf_POLYLINE_3D (dat, obj);
    case DWG_TYPE_ARC:
      return dwg_indxf_ARC (dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_indxf_CIRCLE (dat, obj);
    case DWG_TYPE_LINE:
      return dwg_indxf_LINE (dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_indxf_DIMENSION_ORDINATE (dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_indxf_DIMENSION_LINEAR (dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_indxf_DIMENSION_ALIGNED (dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_indxf_DIMENSION_ANG3PT (dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_indxf_DIMENSION_ANG2LN (dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_indxf_DIMENSION_RADIUS (dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_indxf_DIMENSION_DIAMETER (dat, obj);
    case DWG_TYPE_POINT:
      return dwg_indxf_POINT (dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_indxf__3DFACE (dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_indxf_POLYLINE_PFACE (dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_indxf_POLYLINE_MESH (dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_indxf_SOLID (dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_indxf_TRACE (dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_indxf_SHAPE (dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_indxf_VIEWPORT (dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_indxf_ELLIPSE (dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_indxf_SPLINE (dat, obj);
    case DWG_TYPE_REGION:
      return dwg_indxf_REGION (dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_indxf__3DSOLID (dat, obj);
    case DWG_TYPE_BODY:
      return dwg_indxf_BODY (dat, obj);
    case DWG_TYPE_RAY:
      return dwg_indxf_RAY (dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_indxf_XLINE (dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_indxf_DICTIONARY (dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_indxf_MTEXT (dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_indxf_LEADER (dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_indxf_TOLERANCE (dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_indxf_MLINE (dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_indxf_BLOCK_CONTROL (dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      return dwg_indxf_BLOCK_HEADER (dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_indxf_LAYER_CONTROL (dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_indxf_LAYER (dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_indxf_STYLE_CONTROL (dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_indxf_STYLE (dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_indxf_LTYPE_CONTROL (dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_indxf_LTYPE (dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_indxf_VIEW_CONTROL (dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_indxf_VIEW (dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_indxf_UCS_CONTROL (dat, obj);
    case DWG_TYPE_UCS:
      return dwg_indxf_UCS (dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_indxf_VPORT_CONTROL (dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_indxf_VPORT (dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_indxf_APPID_CONTROL (dat, obj);
    case DWG_TYPE_APPID:
      return dwg_indxf_APPID (dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_indxf_DIMSTYLE_CONTROL (dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_indxf_DIMSTYLE (dat, obj);
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_indxf_VPORT_ENTITY_CONTROL (dat, obj);
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_indxf_VPORT_ENTITY_HEADER (dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_indxf_GROUP (dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_indxf_MLINESTYLE (dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_indxf_OLE2FRAME (dat, obj);
    case DWG_TYPE_DUMMY:
      return 0; // dwg_indxf_DUMMY(dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_indxf_LONG_TRANSACTION (dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_indxf_LWPOLYLINE (dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_indxf_HATCH (dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_indxf_XRECORD (dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_indxf_PLACEHOLDER (dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_indxf_OLEFRAME (dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own section\n");
      // dwg_indxf_VBA_PROJECT(dat, obj);
      return DWG_ERR_INVALIDTYPE;
    case DWG_TYPE_LAYOUT:
      return dwg_indxf_LAYOUT (dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_indxf_PROXY_ENTITY (dat, obj);
    case DWG_TYPE_PROXY_OBJECT:
      return dwg_indxf_PROXY_OBJECT (dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return dwg_indxf_LAYOUT (dat, obj);
        }
      /* > 500 */
      if (DWG_ERR_UNHANDLEDCLASS
          & (error = dwg_indxf_variable_type (obj->parent, dat, obj)))
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
              return error; // dwg_indxf_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              return error; // dwg_indxf_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN ("Unknown object, skipping eed/reactors/xdic");
              SINCE (R_2000){
                LOG_INFO ("Object bitsize: %u\n", obj->bitsize)
              }
              LOG_INFO ("Object handle: " FORMAT_H "\n", ARGS_H(obj->handle));
            }
        }
    }
  return error;
}

#define DXF_CHECK_ENDSEC                                                      \
  if (pair != NULL && (dat->byte >= dat->size || pair->code == 0))            \
  return 0
#define DXF_BREAK_ENDSEC                                                      \
  if (pair != NULL                                                            \
      && (dat->byte >= dat->size                                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))         \
  break
#define DXF_RETURN_ENDSEC(what)                                               \
  if (pair != NULL)                                                           \
    {                                                                         \
      if (dat->byte >= dat->size                                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC")))          \
        {                                                                     \
          dxf_free_pair (pair);                                               \
          return what;                                                        \
        }                                                                     \
    }

static int
dxf_expect_code (Bit_Chain *restrict dat, Dxf_Pair *restrict pair, int code)
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

int
matches_type (Dxf_Pair *restrict pair, const Dwg_DYNAPI_field *restrict f)
{
  switch (pair->type)
    {
    case VT_STRING:
      if (f->is_string) return 1;
      if (f->type[0] == 'H') return 1; // handles can be just names
      break;
    case VT_INT32:
      // BL or RL
      if (f->size == 4 && f->type[1] == 'L') return 1;
      // fall through
    case VT_INT16:
      // BS or RS or CMC
      if (f->size == 2 && f->type[1] == 'S') return 1;
      if (strEQc (f->type, "CMC")) return 1;
      // fall through
    case VT_INT8:
      if (strEQc (f->type, "RC")) return 1;
      // fall through
    case VT_BOOL:
      if (strEQc (f->type, "B")) return 1;
      break;
    case VT_REAL:
      // BD or RD
      if (f->size == 8 && f->type[1] == 'D') return 1;
      if (strEQc (f->type, "TIMEBLL")) return 1;
      break;
    case VT_POINT3D:
      // 3BD or 3RD or 3DPOINT
      if (f->size == 24 && f->type[0] == '3') return 1;
      // accept 2BD or 2RD or 2DPOINT also
      if (f->size == 16 && f->type[0] == '2') return 1;
      break;
    case VT_BINARY:
      if (f->is_string) return 1;
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      if (f->type[0] == 'H') return 1;
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
    }
  return 0;
}

static int
dxf_header_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  const int minimal = dwg->opts & 0x10;
  int is_utf = 0;
  int i = 0;

  // here SECTION(HEADER) was already consumed
  // read the first group 9, $field pair
  Dxf_Pair *pair = dxf_read_pair (dat);

  // Unneeded. field names and types are already defined statically in dynapi
  // #include "header_variables_dxf.spec"

  while (pair->code == 9)
    {
      char field[80];
      strcpy (field, pair->value.s);
      i = 0;

      // now read the code, value pair
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_BREAK_ENDSEC;
    next_hdrvalue:
      if (pair->code == 1 && strEQc (field, "$ACADVER"))
        {
          const char* version = pair->value.s;
          for (Dwg_Version_Type v = 0; v <= R_AFTER; v++)
            {
              if (strEQ (version, version_codes[v]))
                {
                  dwg->header.version = v;
                  dat->version = dat->from_version = dwg->header.version;
                  is_utf = dat->version >= R_2007;
                  LOG_TRACE ("HEADER.version = dat->version = %s\n",
                             version);
                  //dxf_free_pair (pair);
                  break;
                }
              if (v == R_AFTER)
                LOG_ERROR ("Invalid HEADER: 9 %s, 1 %s", field, version)
            }
        }
      else if (field[0] == '$')
        {
          const Dwg_DYNAPI_field *f = dwg_dynapi_header_field (&field[1]);
          if (!f)
            {
              LOG_ERROR ("skipping HEADER: 9 %s, unknown field with code %d",
                         field, pair->code);
              //dxf_free_pair (pair);
            }
          else if (!matches_type (pair, f))
            {
              LOG_ERROR ("skipping HEADER: 9 %s, wrong type code %d <=> field %s",
                         field, pair->code, f->type);
              //dxf_free_pair (pair);
            }
          else if (pair->type == VT_POINT3D)
            {
              BITCODE_3BD pt = {0.0, 0.0, 0.0};
              if      (i == 0) pt.x = pair->value.d;
              else if (i == 1) pt.y = pair->value.d;
              else if (i == 2) pt.z = pair->value.d;
              if (i > 2)
                {
                  LOG_ERROR ("skipping HEADER: 9 %s, too many point elements",
                             field);
                }
              else
                {
                  // yes, set it 2-3 times
                  LOG_TRACE ("HEADER.%s [%s][%d]\n", &field[1], f->type, i);
                  dwg_dynapi_header_set_value (dwg, &field[1], &pt, is_utf);
                  i++;
                }
              // dxf_free_pair (pair);
            }
          else if (pair->type == VT_STRING && strEQc (f->type, "H"))
            {
              char *key, *str;
              LOG_TRACE ("HEADER.%s %s [%s] later\n", &field[1],
                         pair->value.s, f->type);
              // name (which table?) => handle
              // needs to be postponed, because we don't have the tables yet.
              header_hdls = array_push (header_hdls, &field[1], pair->value.s);
              //dxf_free_pair (pair);
            }
          else if (strEQc (f->type, "CMC"))
            {
              static BITCODE_CMC color = {0};
              if (pair->code <= 70)
                {
                  LOG_TRACE ("HEADER.%s.index %d [CMC]\n", &field[1], pair->value.i);
                  color.index = pair->value.i;
                  dwg_dynapi_header_set_value (dwg, &field[1], &color, 0);
                }
              //dxf_free_pair (pair);
            }
          else if (pair->type == VT_REAL && strEQc (f->type, "TIMEBLL"))
            {
              static BITCODE_TIMEBLL date = {0,0,0};
              unsigned long j = 1;
              double ms;
              date.value = pair->value.d;
              date.days = (BITCODE_BL)trunc (pair->value.d);
              ms = date.value;
              while (ms > 1.0)
                {
                  j *= 10;
                  ms /= 10.0;
                }
              //date.ms = (BITCODE_BL)(1000000 * (date.value - date.days));
              date.ms = (BITCODE_BL)(j/10 * (date.value - date.days));
              LOG_TRACE ("HEADER.%s %f (%u, %u) [TIMEBLL]\n", &field[1],
                         date.value, date.days, date.ms);
              dwg_dynapi_header_set_value (dwg, &field[1], &date, 0);
              //dxf_free_pair (pair);
            }
          else
            {
              LOG_TRACE ("HEADER.%s [%s]\n", &field[1], f->type);
              dwg_dynapi_header_set_value (dwg, &field[1], &pair->value, is_utf);
              //free (pair); // but keep the string
              // primitives (like RC, BD) are copied
            }
        }
      else
        {
          LOG_ERROR ("skipping HEADER: 9 %s, missing the $", field);
        }

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_BREAK_ENDSEC;
      if (pair->code != 9 /* && pair->code != 0 */)
        goto next_hdrvalue; // for mult. 10,20,30 values
    }

  dxf_free_pair (pair);
  if (_obj->DWGCODEPAGE && strEQc (_obj->DWGCODEPAGE, "ANSI_1252"))
    dwg->header.codepage = 30;
  return 0;
}

static int
dxf_classes_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
      if (pair->code == 0 && strEQc (pair->value.s, "CLASS"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
      while (pair->code != 0)
        { // read until next 0 CLASS
          switch (pair->code)
            {
            case 1:
              STRADD (klass->dxfname, pair->value.s);
              LOG_TRACE ("CLASS[%d].dxfname = %s [1]\n", i, pair->value.s);
              break;
            case 2:
              STRADD (klass->cppname, pair->value.s);
              LOG_TRACE ("CLASS[%d].cppname = %s [2]\n", i, pair->value.s);
              break;
            case 3:
              STRADD (klass->appname, pair->value.s);
              LOG_TRACE ("CLASS[%d].appname = %s [3]\n", i, pair->value.s);
              break;
            case 90:
              klass->proxyflag = pair->value.l;
              LOG_TRACE ("CLASS[%d].proxyflag = %ld [90]\n", i, pair->value.l);
              break;
            case 91:
              klass->num_instances = pair->value.l;
              LOG_TRACE ("CLASS[%d].num_instances = %ld [91]\n", i, pair->value.l);
              break;
            case 280:
              klass->wasazombie = (BITCODE_B)pair->value.i;
              LOG_TRACE ("CLASS[%d].num_instances = %d [280]\n", i, pair->value.i);
              break;
            case 281:
              klass->item_class_id = pair->value.i ? 0x1f3 : 0x1f2;
              LOG_TRACE ("CLASS[%d].num_instances = %x [281]\n", i,
                         klass->item_class_id);
              break;
            default:
              LOG_WARN ("Unknown DXF code for class[%d].%d", i, pair->code);
              break;
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
      DXF_RETURN_ENDSEC (0); // next class or ENDSEC
      dwg->num_classes++;
    }
  dxf_free_pair (pair);
  return 0;
}

//Dwg_Object *
//new_object (Dwg_Data *restrict dwg)
//{
//  BITCODE_BL index = dwg->num_objects;
//  Dwg_Object *obj;
//  int realloced = dwg_add_object (dwg);
//  obj = &dwg->object[index];
//  return obj;
//}

// May need obj to shorten the code to relative offset, but not in header_vars.
int add_handle (Dwg_Handle *restrict hdl, BITCODE_RC code,
                BITCODE_RL value, Dwg_Object *restrict obj)
{
  int offset = obj ? (value - (int)obj->handle.value) : 0;
  int i;
  unsigned char *val = (unsigned char *)&hdl->value;
  hdl->code = code;
  hdl->value = value;
  // FIXME: little endian only
  for (i = 3; i >= 0; i--)
    if (val[i])
      break;
  hdl->size = i + 1;
  if (code != 5 &&
      obj && abs(offset) == 1)
    {
      // change code to 6.0.0 or 8.0.0
      if (offset == 1)
        {
          hdl->code = 6; hdl->value = 0; hdl->size = 0;
        }
      else if (offset == -1)
        {
          hdl->code = 8; hdl->value = 0; hdl->size = 0;
        }
    }
  return 0;
}

Dwg_Object_Ref *
add_handleref (BITCODE_RC code, BITCODE_RL value, Dwg_Object *obj)
{
  Dwg_Object_Ref *ref = calloc (1, sizeof (Dwg_Object_Ref));

  add_handle (&ref->handleref, code, value, obj);
  ref->absolute_ref = value;
  // fill ->obj later
  return ref;
}

static void
add_eed (Dwg_Object *restrict obj, const char *restrict name,
         Dxf_Pair *restrict pair)
{
  int i = obj->tio.object->num_eed; // same layout for Object and Entity
  int code, size, j = 0;
  Dwg_Eed *eed = obj->tio.object->eed;

  eed = (Dwg_Eed *)realloc (eed, (i + 1) * sizeof (Dwg_Eed));
  memset (&eed[i], 0, sizeof (Dwg_Eed));
  obj->tio.object->num_eed++;
  // handle: usually APPID "ACAD"
  code = pair->code - 1000; // 1000
  switch (code)
    {
    case 0:
      /* code [RC] + len+0 + length [RC] + codepage [RS] */
      size = 1 + strlen (pair->value.s) + 1 + 1 + 2;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1000
      eed[i].data->u.eed_0.length = strlen (pair->value.s);
      eed[i].data->u.eed_0.codepage = 30;
      strcpy (eed[i].data->u.eed_0.string, pair->value.s);
      eed[i].size += size;
      break;
    case 2:
      /* code [RC] + byte [RC] */
      size = 1 + 1;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1002
      eed[i].data->u.eed_2.byte = (BITCODE_RC)pair->value.i;
      eed[i].size += size;
      break;
    case 4:
      /* code [RC] + len+0 + length [RC] */
      size = 1 + strlen (pair->value.s) + 1 + 1;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1004
      eed[i].data->u.eed_4.length = strlen (pair->value.s);
      strcpy (eed[i].data->u.eed_4.data, pair->value.s);
      eed[i].size += size;
      break;
    case 40:
      /* code [RC] + RD */
      size = 1 + 8;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_40.real = pair->value.d;
      eed[i].size += size;
      break;
    case 70:
      /* code [RC] + RS */
      size = 1 + 2;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_70.rs = pair->value.i;
      eed[i].size += size;
      break;
    case 71:
      /* code [RC] + RL */
      size = 1 + 4;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_71.rl = pair->value.l;
      eed[i].size += size;
      break;
    case 1:
      {
        if (strEQc (pair->value.s, "ACAD"))
          {
            // search in APPID table
            Dwg_Handle hdl = { 5, 1, 0x12 };
            size = sizeof (Dwg_Handle);
            add_handle (&hdl, 5, 12, NULL);
            memcpy (&eed[i].handle, &hdl, sizeof (hdl));
            eed[i].size += size;
            break;
          }
      }
      // fall through
    default:
      LOG_ERROR ("Not yet implemented EED.code %d", pair->code);
    }
  return;
}

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
  _obj = calloc (1, sizeof (Dwg_Object_##token));              \
  obj->tio.object->tio.token = (Dwg_Object_##token *)_obj;     \
  obj->tio.object->tio.token->parent = obj->tio.object;        \
  obj->tio.object->objid = obj->index

#define ADD_ENTITY(token)                                      \
  obj->type = obj->fixedtype = DWG_TYPE_##token;               \
  obj->name = obj->dxfname = (char*)#token;                    \
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
        strcpy (ctrl_hdlv, "entries");                 \
    }

static int
is_table_name (const char *name)
{
  return strEQc (name, "LTYPE") || strEQc (name, "VPORT")
         || strEQc (name, "VPORT") || strEQc (name, "APPID")
         || strEQc (name, "BLOCK") || strEQc (name, "LAYER")
         || strEQc (name, "DIMSTYLE") || strEQc (name, "STYLE")
         || strEQc (name, "VIEW") || strEQc (name, "VPORT_ENTITY")
         || strEQc (name, "UCS") || strEQc (name, "BLOCK_RECORD");
}

static Dxf_Pair *
new_table_control (const char *restrict name, Bit_Chain *restrict dat,
                   Dwg_Data *restrict dwg)
{
  // VPORT_CONTROL.num_entries
  // VPORT_CONTROL.entries[num_entries] handles
  Dwg_Object *obj;
  Dxf_Pair *pair = NULL;
  Dwg_Object_LTYPE_CONTROL *_obj = NULL;
  int j = 0;
  int is_utf = dwg->header.version >= R_2007 ? 1 : 0;
  char *fieldname;
  char ctrlname[80];
  char ctrl_hdlv[80];
  ctrl_hdlv[0] = '\0';

  NEW_OBJECT(dwg, obj);

  if (strEQc (name, "BLOCK_RECORD"))
    strcpy (ctrlname, "BLOCK_CONTROL");
  else
    {
      strcpy (ctrlname, name);
      strcat (ctrlname, "_CONTROL");
    }
  LOG_TRACE ("add %s\n", ctrlname);

  ADD_TABLE_IF (LTYPE, LTYPE_CONTROL)
  else
  ADD_TABLE_IF (VPORT, VPORT_CONTROL)
  else
  ADD_TABLE_IF (APPID, APPID_CONTROL)
  else
  ADD_TABLE_IF (BLOCK, BLOCK_CONTROL)
  else
  ADD_TABLE_IF (DIMSTYLE, DIMSTYLE_CONTROL)
  else
  ADD_TABLE_IF (LAYER, LAYER_CONTROL)
  else
  ADD_TABLE_IF (STYLE, STYLE_CONTROL)
  else
  ADD_TABLE_IF (UCS, UCS_CONTROL)
  else
  ADD_TABLE_IF (VIEW, VIEW_CONTROL)
  else
  ADD_TABLE_IF (VPORT_ENTITY, VPORT_ENTITY_CONTROL)
  else
  ADD_TABLE_IF (BLOCK_RECORD, BLOCK_CONTROL)
  else
    {
      LOG_ERROR ("Unknown DXF TABLE %s nor %s_CONTROL", name, name);
      return pair;
    }
  if (!_obj)
    {
      LOG_ERROR ("Empty _obj at DXF TABLE %s nor %s_CONTROL", name, name);
      return pair;
    }
  dwg_dynapi_entity_set_value (_obj, obj->name, "objid", &obj->index, is_utf);

  pair = dxf_read_pair (dat);
  // read common table until next 0 table or endtab
  while (pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          return pair;
        case 5:
        case 105: // for DIMSTYLE
          {
            Dwg_Object_Ref *ref;
            char ctrlobj[80];
            add_handle (&obj->handle, 0, pair->value.u, NULL);
            LOG_TRACE ("%s.handle = " FORMAT_H " [5]\n", ctrlname,
                       ARGS_H(obj->handle));

            // also set the matching HEADER.*_CONTROL_OBJECT
            ref = add_handleref (3, pair->value.u, obj);
            strcpy (ctrlobj, ctrlname);
            strcat (ctrlobj, "_OBJECT");
            dwg_dynapi_header_set_value (dwg, ctrlobj, &ref, 0);
            LOG_TRACE ("HEADER.%s = " FORMAT_REF " [0]\n", ctrlobj,
                       ARGS_REF(ref));
          }
          break;
        case 330: // ownerhandle mostly 0
          if (pair->value.u)
            {
              obj->tio.object->ownerhandle = add_handleref (4, pair->value.u, obj);
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [330]\n", ctrlname,
                         ARGS_REF(obj->tio.object->ownerhandle));
            }
          break;
        case 340:
          if (pair->value.u && strEQc (ctrlname, "DIMSTYLE_CONTROL"))
            {
              Dwg_Object_DIMSTYLE_CONTROL *_o = (Dwg_Object_DIMSTYLE_CONTROL *)_obj;
              if (!_o->num_morehandles)
                {
                  LOG_ERROR ("Empty DIMSTYLE_CONTROL.num_morehandles")
                 break;
                }
              assert (_o->morehandles);
              _o->morehandles[j]  = add_handleref (4, pair->value.u, obj);
              LOG_TRACE ("%s.morehandles[%d] = " FORMAT_REF " [330]\n", ctrlname,
                         j, ARGS_REF(_o->morehandles[j]));
              j++;
            }
          break;
        case 100: // Always AcDbSymbolTable. ignore
          break;
        case 360: // {ACAD_XDICTIONARY TODO
          obj->tio.object->xdicobjhandle = add_handleref (0, pair->value.u, obj);
          LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [330]\n", ctrlname,
                     ARGS_REF(obj->tio.object->xdicobjhandle));
          break;
        case 102: // {ACAD_XDICTIONARY TODO
          break;
        case 70:
          if (pair->value.u)
            {
              BITCODE_H *hdls;
              BITCODE_BL num_entries = (BITCODE_BL)pair->value.u;
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_entries",
                                           &num_entries, is_utf);
              LOG_TRACE ("%s.num_entries = %u [70]\n", ctrlname,
                         num_entries);
              hdls = calloc (num_entries, sizeof (Dwg_Object_Ref*));
              dwg_dynapi_entity_set_value (_obj, obj->name, ctrl_hdlv,
                                           &hdls, 0);
              LOG_TRACE ("Add %s.%s[%d]\n", ctrlname, ctrl_hdlv, num_entries);
            }
          break;
        case 71:
          if (strEQc (name, "DIMSTYLE") && pair->value.u)
            {
              BITCODE_H *hdls;
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_morehandles",
                                           &pair->value, is_utf);
              LOG_TRACE ("%s.num_morehandles = %u [71]\n", ctrlname,
                         pair->value.u)
              hdls = calloc (pair->value.u, sizeof (Dwg_Object_Ref*));
              dwg_dynapi_entity_set_value (_obj, obj->name, "morehandles",
                                           &hdls, 0);
              LOG_TRACE ("Add %s.morehandles[%d]\n", ctrlname, pair->value.u);
              break;
            }
          // fall through
        default:
          if (pair->code >= 1000 && pair->code < 1999)
            {
              //LOG_WARN ("TODO %s.eed.%d", ctrlname, pair->code);
              add_eed (obj, ctrlname, pair);
            }
          else
            LOG_ERROR ("Unknown DXF code %d for %s", pair->code, ctrlname);
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

static Dxf_Pair *
new_table (const char *restrict name, Bit_Chain *restrict dat,
           Dwg_Data *restrict dwg, Dwg_Object *restrict ctrl, BITCODE_BL i)
{
  int is_utf = dwg->header.version >= R_2007 ? 1 : 0;
  Dwg_Object *obj;
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Object_LTYPE *_obj = NULL;
  char ctrl_hdlv[80];
  char ctrlname[80];
  ctrl_hdlv[0] = '\0';

  NEW_OBJECT (dwg, obj);
  strcpy (ctrlname, name);
  strcat (ctrlname, "_CONTROL");

  LOG_TRACE ("add %s [%d]\n", name, i);

  ADD_TABLE_IF (LTYPE, LTYPE)
  else
  ADD_TABLE_IF (VPORT, VPORT)
  else
  ADD_TABLE_IF (APPID, APPID)
  else
  ADD_TABLE_IF (DIMSTYLE, DIMSTYLE)
  else
  ADD_TABLE_IF (LAYER, LAYER)
  else
  ADD_TABLE_IF (STYLE, STYLE)
  else
  ADD_TABLE_IF (UCS, UCS)
  else
  ADD_TABLE_IF (VIEW, VIEW)
  else
  ADD_TABLE_IF (BLOCK_RECORD, BLOCK_HEADER)
  //else
  //ADD_TABLE_IF (VPORT_ENTITY, VPORT_ENTITY)
  else
    {
      LOG_ERROR ("Unknown DXF AcDbSymbolTableRecord %s", name);
      return pair;
    }
  if (!_obj)
    {
      LOG_ERROR ("Empty _obj at DXF AcDbSymbolTableRecord %s", name);
      return pair;
    }

  // read table fields until next 0 table or 0 ENDTAB
  while (pair->code != 0)
    {
      switch (pair->code)
        { // TABLE common flags: name, xrefref, xrefdep, ...
        case 0:
          return pair;
        case 5:
          {
            add_handle (&obj->handle, 0, pair->value.u, NULL);
            LOG_TRACE ("%s.handle = " FORMAT_H " [5]\n", name,
                       ARGS_H(obj->handle));
            if (*ctrl_hdlv)
              {
                // add to ctrl HANDLE_VECTOR "ctrl_hdlv"
                Dwg_Object_VPORT_CONTROL *_ctrl = ctrl->tio.object->tio.VPORT_CONTROL;
                BITCODE_H *hdls = NULL;
                dwg_dynapi_entity_value (_ctrl, ctrlname, ctrl_hdlv, &hdls, NULL);
                if (!hdls)
                  {
                    BITCODE_BL num_entries;
                    dwg_dynapi_entity_value (_ctrl, ctrlname, "num_entries",
                                             &num_entries, NULL);
                    if (!num_entries)
                      {
                        num_entries = i + 1;
                        dwg_dynapi_entity_set_value (
                            _ctrl, ctrlname, "num_entries", &num_entries, 0);
                      }
                    hdls = calloc (num_entries, sizeof (Dwg_Object_Ref*));
                    dwg_dynapi_entity_set_value (_ctrl, ctrlname, ctrl_hdlv,
                                                 &hdls, 0);
                  }
                hdls[i] = add_handleref (2, pair->value.u, obj);
                //dwg_dynapi_entity_set_value (_ctrl, ctrlname, ctrl_hdlv, &hdls);
              }
          }
          break;
        case 330:
          if (pair->value.u)
            {
              obj->tio.object->ownerhandle = add_handleref (4, pair->value.u, obj);
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [330]\n", name,
                         ARGS_REF(obj->tio.object->ownerhandle));
            }
          break;
        case 100: // Always AcDbSymbolTableRecord and then AcDb*TableRecord. ignore
          break;
        case 360: // {ACAD_XDICTIONARY TODO
          LOG_WARN ("Unhandled %s.xdicobjhandle = %X [360]\n", name,
                    /*obj->handle.code, obj->handle.size, obj->handle.value, */
                    pair->value.u);
          // fall through
        case 102: // {ACAD_XDICTIONARY TODO
          break;
        case 2:
          dwg_dynapi_entity_set_value (_obj, name, "name", &pair->value, is_utf);
          LOG_TRACE ("%s.name = %s [2]\n", name, pair->value.s);
          break;
        case 70:
          dwg_dynapi_entity_set_value (_obj, name, "flag",
                                       &pair->value, is_utf);
          LOG_TRACE ("%s.flag = %d [70]\n", name, pair->value.i)
          break;
        default:
          { // search all specific fields and common fields for the DXF
            const Dwg_DYNAPI_field *f;
            const Dwg_DYNAPI_field *fields = dwg_dynapi_entity_fields (name);
            for (f = &fields[0]; f->name; f++)
              {
                if (f->dxf == pair->code ||
                    (*f->type == '3' &&
                     (f->dxf + 10 == pair->code ||
                      f->dxf + 20 == pair->code)) ||
                    (*f->type == '2' &&
                     f->dxf + 10 == pair->code))
                  {
                    dwg_dynapi_entity_set_value (_obj, name, f->name,
                                                 &pair->value, is_utf);
                    if (f->is_string)
                      {
                        LOG_TRACE ("%s.%s = %s [%d %s]\n", name, f->name,
                                   pair->value.s, pair->code, f->type);
                      }
                    else
                      {
                        LOG_TRACE ("%s.%s = %ld [%d %s]\n", name, f->name,
                                   pair->value.l, pair->code, f->type);
                      }
                    goto next_pair; // found, early exit
                  }
              }
            fields = dwg_dynapi_common_object_fields ();
            for (f = &fields[0]; f->name; f++)
              {
                if (f->dxf == pair->code)
                  {
                    dwg_dynapi_common_set_value (_obj, f->name,
                                                 &pair->value, is_utf);
                    if (f->is_string)
                      {
                        LOG_TRACE ("%s = %s [%d %s]\n", f->name,
                                   pair->value.s, pair->code, f->type);
                      }
                    else
                      {
                        LOG_TRACE ("%s = %ld [%d %s]\n", f->name,
                                   pair->value.l, pair->code, f->type);
                      }
                    goto next_pair; // found, early exit
                  }
              }

            LOG_WARN ("Unknown DXF code %d for %s", pair->code, name);
          }
        }
    next_pair:
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}
 
static int
dxf_tables_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char table[80];
  Dxf_Pair *pair = dxf_read_pair (dat);

  table[0] = '\0'; // init
  while (1) // read next 0 TABLE
    {
      if (pair->code == 0) // TABLE or ENDTAB
        {
          if (strEQc (pair->value.s, "TABLE"))
            table[0] = '\0'; // new table coming up
          else if (strEQc (pair->value.s, "ENDTAB"))
            table[0] = '\0'; // close table
          else
            LOG_WARN ("Unknown 0 %s", pair->value.s);
        }
      if (pair->code == 2 &&
          strlen (pair->value.s) < 80 &&
          is_table_name (pair->value.s)) // new table NAME
        {
          Dwg_Object *ctrl;
          BITCODE_BL i = 0;
          strcpy (table, pair->value.s);
          pair = new_table_control (table, dat, dwg); // until 0 table
          ctrl = &dwg->object[dwg->num_objects - 1];
          while (pair->code == 0 && strEQ (pair->value.s, table))
            {
              // until 0 table or 0 ENDTAB
              pair = new_table (table, dat, dwg, ctrl, i++);
            }
        }
      DXF_RETURN_ENDSEC (0); // next TABLE or ENDSEC
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_blocks_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;

  SECTION (BLOCKS);
  //...
  ENDSEC ();
  return 0;
}

static int
dxf_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
      dwg_indxf_object (dat, obj); // TODO obj must be already created here
    }
  ENDSEC ();
  return 0;
}

static int
dxf_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
      dwg_indxf_object (dat, obj); // TODO obj must be already created here
    }
  ENDSEC ();
  return 0;
}

static int
dxf_preview_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;
  SECTION (THUMBNAILIMAGE);
  // VALUE_RL(pic->size, 90);
  // VALUE_BINARY(pic->chain, pic->size, 310);
  ENDSEC ();
  return 0;
}

int
dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  const int minimal = dwg->opts & 0x10;
  Dxf_Pair *pair;
  // warn if minimal != 0
  // struct Dwg_Header *obj = &dwg->header;
  loglevel = dwg->opts & 0xf;

  num_dxf_objs = 0;
  size_dxf_objs = 1000;
  dxf_objs = malloc (1000 * sizeof (Dxf_Objs));

  header_hdls = calloc (1, 8 + 16 * sizeof (struct array_hdl));
  header_hdls->size = 16;

  // start with the BLOCK_HEADER at objid 0
  if (!dwg->num_objects)
    {
      Dwg_Object *obj;
      Dwg_Object_BLOCK_HEADER *_obj;
      NEW_OBJECT (dwg, obj);
      ADD_OBJECT (BLOCK_HEADER);
    }

  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      dxf_expect_code (dat, pair, 0);
      DXF_CHECK_EOF;
      if (pair->type == VT_STRING && strEQc (pair->value.s, "SECTION"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          dxf_expect_code (dat, pair, 2);
          DXF_CHECK_EOF;
          if (strEQc (pair->value.s, "HEADER"))
            {
              dxf_free_pair (pair);
              dxf_header_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "CLASSES"))
            {
              dxf_free_pair (pair);
              dxf_classes_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "TABLES"))
            {
              dxf_free_pair (pair);
              dxf_tables_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "BLOCKS"))
            {
              dxf_free_pair (pair);
              dxf_blocks_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "ENTITIES"))
            {
              dxf_free_pair (pair);
              dxf_entities_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "OBJECTS"))
            {
              dxf_free_pair (pair);
              dxf_objects_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "THUMBNAIL"))
            {
              dxf_free_pair (pair);
              dxf_preview_read (dat, dwg);
            }
        }
    }
  return dwg->num_objects ? 1 : 0;
}

#undef IS_ENCODE
#undef IS_DXF

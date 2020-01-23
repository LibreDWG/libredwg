/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * in_json.c: parse JSON via jsmn/jsmn.c
 * written by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "dynapi.h"
#include "in_json.h"

// not exported
#define JSMN_STATIC
// probably not needed in the final version (counting keys)
#define JSMN_PARENT_LINKS
// our files are bigger than 8000
// In strict mode primitive must be followed by "," or "}" or "]"; comma/object/array
// In strict mode an object or array can't become a key
// In strict mode primitives are: numbers and booleans
#undef JSMN_STRICT
#include "../jsmn/jsmn.h"

typedef struct jsmntokens {
  unsigned int index;
  jsmntok_t *tokens;
  unsigned int num_tokens;
} jsmntokens_t;

// synced with enum jsmntype_t
static const char *const t_typename[]
    = { "UNDEFINED", "OBJECT", "ARRAY", "STRING", "PRIMITIVE" };

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char* created_by;
static Bit_Chain *g_dat;

#define json_expect(tokens, typ) \
  if (tokens->tokens[tokens->index].type != JSMN_##typ) \
    return DWG_ERR_INVALIDTYPE

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION injson
#define IS_ENCODE
#define IS_JSON

// TODO: read, not write. see in_dxf.c
#define PREFIX
#define ARRAY                                                                 \
  PREFIX fprintf (g_dat->fh, "[\n");                                          \
  g_dat->bit++
#define ENDARRAY                                                              \
  g_dat->bit--;                                                               \
  PREFIX fprintf (g_dat->fh, "],\n")
#define LASTENDARRAY                                                          \
  fprintf (g_dat->fh, "\n");                                                  \
  dat->bit--;                                                                 \
  PREFIX fprintf (g_dat->fh, "]\n")
#define KEYs(nam) PREFIX fprintf (g_dat->fh, "\"%s\": ", nam)
#define KEY(nam) PREFIX fprintf (g_dat->fh, "\"%s\": ", #nam)
#define HASH                                                                  \
  PREFIX fprintf (g_dat->fh, "{\n");                                          \
  g_dat->bit++
#define ENDHASH                                                               \
  g_dat->bit--;                                                               \
  PREFIX fprintf (g_dat->fh, "},\n")
#define NOCOMMA

#define TABLE(nam)                                                            \
  KEY (nam);                                                                  \
  HASH
#define ENDTAB()                                                              \
  NOCOMMA;                                                                    \
  ENDHASH
// a namd hash
#define RECORD(nam)                                                           \
  KEY (nam);                                                                  \
  HASH
#define ENDRECORD()                                                           \
  NOCOMMA;                                                                    \
  ENDHASH
// a named list
#define SECTION(nam)                                                          \
  PREFIX fprintf (dat->fh, "\"%s\": [\n", #nam);                              \
  dat->bit++;
#define ENDSEC() ENDARRAY

#undef FORMAT_RC
#define FORMAT_RC "%d"
#define VALUE(value, type, dxf) fprintf (g_dat->fh, FORMAT_##type, value);
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_BS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RD(value, dxf) VALUE (value, RD, dxf)
#define VALUE_2RD(pt, dxf) fprintf (dat->fh, "[ %f, %f ],\n", pt.x, pt.y)
#define VALUE_2DD(nam, d1, d2, dxf) VALUE_2RD (nam, dxf)
#define VALUE_3RD(pt, dxf)                                                   \
  fprintf (dat->fh, "[ %f, %f, %f ],\n", pt.x, pt.y, pt.z)
#define VALUE_3BD(nam, dxf) VALUE_3RD (pt, dxf)
#define VALUE_TV(nam, dxf)
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  if (hdlptr)                                                                 \
    {                                                                         \
      fprintf (dat->fh, "\"" FORMAT_H "\",\n", ARGS_H(hdlptr->handleref));    \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      fprintf (dat->fh, "\"(0.0.0)\",\n");                                    \
    }
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

#define FIELD(nam, type)                                                      \
  PREFIX fprintf (g_dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", _obj->nam)
#define FIELDG(nam, type, dxf) FIELD (nam, type)
#define _FIELD(nam, type, value)                                              \
  PREFIX fprintf (g_dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", obj->nam)
#define ENT_FIELD(nam, type, value)                                           \
  PREFIX fprintf (g_dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", _ent->nam)
#define SUB_FIELD(o, nam, type, dxf)                                          \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"" #nam "\": " FORMAT_##type ",\n",            \
                    _obj->o.nam);                                             \
  }

#define FIELD_CAST(nam, type, cast, dxf) FIELDG (nam, cast, dxf)
#define FIELD_TRACE(nam, type)
#define FIELD_TEXT(nam, str)                                                  \
  PREFIX fprintf (g_dat->fh, "\"" #nam "\": \"%s\",\n", str ? str : "")

#define VALUE_TEXT_TU(wstr)
#ifdef HAVE_NATIVE_WCHAR2
#  define FIELD_TEXT_TU(nam, wstr)                                            \
    PREFIX fprintf (g_dat->fh, "\"" #nam "\": \"%ls\",\n",                    \
                    wstr ? (wchar_t *)wstr : L"")
#else
#  define FIELD_TEXT_TU(nam, wstr)                                            \
    {                                                                         \
      BITCODE_TU ws = (BITCODE_TU)wstr;                                       \
      uint16_t _c;                                                            \
      PREFIX                                                                  \
      fprintf (g_dat->fh, "\"" #nam "\": \"");                                \
      if (ws)                                                                 \
        {                                                                     \
          while ((_c = *ws++))                                                \
            {                                                                 \
              fprintf (g_dat->fh, "%c", (char)(_c & 0xff));                   \
            }                                                                 \
        }                                                                     \
      fprintf (g_dat->fh, "\",\n");                                           \
    }
#endif

#define FIELD_VALUE(nam) _obj->nam
#define ANYCODE -1
// todo: only the nam, not the ref
#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  PREFIX if (_obj->nam)                                                       \
  {                                                                           \
    fprintf (g_dat->fh, "\"" #nam "\": \"" FORMAT_H "\",\n",                    \
             ARGS_H(_obj->nam->handleref));                                   \
  }                                                                           \
  else { fprintf (g_dat->fh, "\"" #nam "\": \"0.0.0\",\n"); }
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  PREFIX if (_obj->o.nam)                                                     \
  {                                                                           \
    fprintf (g_dat->fh, "\"" #nam "\": \"" FORMAT_H "\",\n",                    \
             ARGS_H(_obj->o.nam->handleref));                                 \
  }                                                                           \
  else { fprintf (g_dat->fh, "\"" #nam "\": \"0.0.0\",\n"); }
#define FIELD_DATAHANDLE(nam, code, dxf) FIELD_HANDLE (nam, code, dxf)
#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf)                         \
  PREFIX if (_obj->nam)                                                       \
  {                                                                           \
    fprintf (g_dat->fh, "\"" #nam "\": \"" FORMAT_H "\",\n",                  \
             ARGS_H(_obj->nam->handleref));                                   \
  }                                                                           \
  else { fprintf (g_dat->fh, "\"\",\n"); }
#define FIELD_BINARY(nam, size, dxf)                                          \
  {                                                                           \
    long len = size;                                                          \
    KEY (nam);                                                                \
    fprintf (g_dat->fh, "\"");                                                \
    if (_obj->nam)                                                            \
      {                                                                       \
        for (long j = 0; j < len; j++)                                        \
          {                                                                   \
            fprintf (g_dat->fh, "%02X", _obj->nam[j]);                        \
          }                                                                   \
      }                                                                       \
    fprintf (g_dat->fh, "\",\n");                                             \
  }
/******************************************************************/
#define _FIELD_FLOAT(nam, type)                                  \
      else if (strEQc (key, #nam))                               \
        {                                                        \
          _obj->nam = (BITCODE_##type)json_float (dat, tokens);  \
          LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)    \
        }
#define _FIELD_LONG(nam, type)                                   \
      else if (strEQc (key, #nam))                               \
        {                                                        \
          _obj->nam = (BITCODE_##type)json_long (dat, tokens);   \
          LOG_TRACE (#nam ": " FORMAT_##type "\n", _obj->nam)    \
        }

#define FIELD_B(nam, dxf)   _FIELD_LONG (nam, B)
#define FIELD_BB(nam, dxf)  _FIELD_LONG (nam, BB)
#define FIELD_3B(nam, dxf)  _FIELD_LONG (nam, 3B)
#define FIELD_BS(nam, dxf)  _FIELD_LONG (nam, BS)
#define FIELD_BL(nam, dxf)  _FIELD_LONG (nam, BL)
#define FIELD_BLL(nam, dxf) _FIELD_LONG (nam, BLL)
#define FIELD_RC(nam, dxf)  _FIELD_LONG (nam, RC)
#define FIELD_RS(nam, dxf)  _FIELD_LONG (nam, RS)
#define FIELD_RL(nam, dxf)  _FIELD_LONG (nam, RL)
#define FIELD_RLL(nam, dxf) _FIELD_LONG (nam, RLL)
#define FIELD_MC(nam, dxf)  _FIELD_LONG (nam, MC)
#define FIELD_MS(nam, dxf)  _FIELD_LONG (nam, MS)

#define FIELD_BD(nam, dxf) _FIELD_FLOAT (nam, BD)
#define FIELD_RD(nam, dxf) _FIELD_FLOAT (nam, RD)
  
#define FIELD_TF(nam, len, dxf) FIELD_TEXT (nam, _obj->nam)
#define FIELD_TFF(nam, len, dxf) FIELD_TEXT (nam, _obj->nam)
#define FIELD_TV(nam, dxf) FIELD_TEXT (nam, _obj->nam)
#define FIELD_TU(nam, dxf) FIELD_TEXT_TU (nam, (BITCODE_TU)_obj->nam)
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (g_dat->version >= R_2007)                                             \
      {                                                                       \
        FIELD_TU (nam, dxf);                                                  \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_TV (nam, dxf);                                                  \
      }                                                                       \
  }
#define FIELD_BT(nam, dxf) FIELDG (nam, BT, dxf);
#define FIELD_4BITS(nam, dxf) FIELDG (nam, 4BITS, dxf)
#define FIELD_BE(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_DD(nam, _default, dxf)                                          \
  PREFIX fprintf (g_dat->fh, "\"" #nam "\": " FORMAT_DD ",\n", _obj->nam)
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
#define FIELD_CMC(color, dxf1, dxf2)                                          \
  {                                                                           \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        RECORD (color);                                                       \
        PREFIX fprintf (dat->fh, "\"index\": %d,\n", _obj->color.index);      \
        PREFIX fprintf (dat->fh, "\"" #color ".rgb\": \"%06x\",\n",           \
                        (unsigned)_obj->color.rgb);                           \
        /*if (_obj->color.flag & 1) {                                         \
          PREFIX fprintf(dat->fh, "\"" #color ".name\": \"%s\",\n",           \
        _obj->color.name); } if (_obj->color.flag & 2) { PREFIX                                                                       \
        fprintf(dat->fh, "\"" #color ".bookname\": \"%s\",\n",                \
        _obj->color.book_name); } */                                          \
        ENDRECORD ();                                                         \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        PREFIX fprintf (dat->fh, "\"" #color "\": %d,\n", _obj->color.index); \
      }                                                                       \
  }
#define FIELD_TIMEBLL(nam, dxf)                                               \
  PREFIX fprintf (g_dat->fh, "\"" #nam "\": " FORMAT_BL "." FORMAT_BL ",\n",  \
                  _obj->nam.days, _obj->nam.ms)

#define SUB_FIELD_T(o, nam, dxf)                                              \
  {                                                                           \
    if (dat->version >= R_2007)                                               \
      {                                                                       \
        KEY (nam);                                                            \
        VALUE_TEXT_TU ((BITCODE_TU)_obj->o.nam);                              \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        PREFIX;                                                               \
        fprintf (dat->fh, "\"" #nam "\": \"%s\",\n", _obj->o.nam);            \
      }                                                                       \
  }
#define SUB_FIELD_B(o, nam, dxf) SUB_FIELD (o, nam, B, dxf)
#define SUB_FIELD_BB(o, nam, dxf) SUB_FIELD (o, nam, BB, dxf)
#define SUB_FIELD_3B(o, nam, dxf) SUB_FIELD (o, nam, 3B, dxf)
#define SUB_FIELD_BS(o, nam, dxf) SUB_FIELD (o, nam, BS, dxf)
#define SUB_FIELD_BL(o, nam, dxf) SUB_FIELD (o, nam, BL, dxf)
#define SUB_FIELD_BLd(o, nam, dxf) SUB_FIELD (o, nam, BLd, dxf)
#define SUB_FIELD_BLx(o, nam, dxf) SUB_FIELD (o, nam, BLx, dxf)
#define SUB_FIELD_BD(o, nam, dxf) SUB_FIELD (o, nam, BD, dxf)
#define SUB_FIELD_RC(o, nam, dxf) SUB_FIELD (o, nam, RC, dxf)
#define SUB_FIELD_RS(o, nam, dxf) SUB_FIELD (o, nam, RS, dxf)
#define SUB_FIELD_RD(o, nam, dxf) SUB_FIELD (o, nam, RD, dxf)
#define SUB_FIELD_RL(o, nam, dxf) SUB_FIELD (o, nam, RL, dxf)
#define SUB_FIELD_BLL(o, nam, dxf) SUB_FIELD (o, nam, BLL, dxf)
#define SUB_FIELD_RLL(o, nam, dxf) SUB_FIELD (o, nam, RLL, dxf)
#define SUB_FIELD_3BD_inl(o, nam, dxf)                                        \
  KEY (nam);                                                                  \
  VALUE_3RD (_obj->o, dxf)
#define SUB_FIELD_2BD_1(o, nam, dxf)                                          \
  KEY (nam);                                                                  \
  VALUE_2RD (_obj->o.nam, dxf)
#define SUB_FIELD_2RD(o, nam, dxf)                                            \
  KEY (nam);                                                                  \
  VALUE_2RD (_obj->o.nam, dxf)
#define SUB_FIELD_3RD(o, nam, dxf)                                            \
  KEY (nam);                                                                  \
  VALUE_3RD (_obj->o.nam, dxf)
#define SUB_FIELD_3BD(o, nam, dxf)                                            \
  KEY (nam);                                                                  \
  VALUE_3RD (_obj->o.nam, dxf)
#define SUB_FIELD_3DPOINT(o, nam, dxf)                                        \
  KEY (nam);                                                                  \
  VALUE_3RD (_obj->o.nam, dxf)

#define SUB_FIELD_CMC(o, color, dxf1, dxf2)                                   \
  {                                                                           \
    if (dat->version >= R_2004)                                               \
      {                                                                       \
        RECORD (color);                                                       \
        PREFIX fprintf (dat->fh, "\"index\": %d,\n", _obj->o.color.index);    \
        PREFIX fprintf (dat->fh, "\"" #color ".rgb\": \"%06x\",\n",           \
                        (unsigned)_obj->o.color.rgb);                         \
        /*if (_obj->color.flag & 1) {                                         \
          PREFIX fprintf(dat->fh, "\"" #color ".name\": \"%s\",\n",           \
        _obj->o.color.name); } if (_obj->color.flag & 2) { PREFIX                                                                       \
        fprintf(dat->fh, "\"" #color ".bookname\": \"%s\",\n",                \
        _obj->o.color.book_name); } */                                        \
        ENDRECORD ();                                                         \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        PREFIX fprintf (dat->fh, "\"" #color "\": %d,\n",                     \
                        _obj->o.color.index);                                 \
      }                                                                       \
  }

// FIELD_VECTOR_N(nam, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'nam'.
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  // json_expect(tokens, ARRAY);
  //...

#define FIELD_VECTOR_T(nam, size, dxf)                                        \
  ARRAY;                                                                      \
  PRE (R_2007)                                                                \
  {                                                                           \
    for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)               \
      {                                                                       \
        PREFIX fprintf (g_dat->fh, "\"" #nam "\": \"%s\",\n",                 \
                        _obj->nam[vcount]);                                   \
      }                                                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)               \
      FIELD_TEXT_TU (nam, _obj->nam[vcount]);                                 \
  }                                                                           \
  ENDARRAY;

#define FIELD_VECTOR(nam, type, size, dxf)                                    \
  FIELD_VECTOR_N (nam, type, _obj->size, dxf)

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2RD (nam[vcount], dxf);                                           \
    }                                                                         \
  ENDARRAY;

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  ARRAY;                                                                      \
  FIELD_2RD (nam[0], 0);                                                      \
  for (vcount = 1; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_2DD (nam[vcount], FIELD_VALUE (nam[vcount - 1].x),                \
                 FIELD_VALUE (nam[vcount - 1].y), dxf);                       \
    }                                                                         \
  ENDARRAY;

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      FIELD_3DPOINT (nam[vcount], dxf);                                       \
    }                                                                         \
  ENDARRAY;

#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                       \
    {                                                                         \
      FIELD_HANDLE_N (nam[vcount], vcount, code, dxf);                        \
    }                                                                         \
  ENDARRAY;

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  FIELDG (num_inserts, type, dxf)

#define FIELD_XDATA(nam, size)

#define REACTORS(code)                                                        \
  KEY (reactors);                                                             \
  ARRAY;                                                                      \
  if (obj->tio.object->reactors)                                              \
    {                                                                         \
      for (vcount = 0; vcount < obj->tio.object->num_reactors; vcount++)      \
        {                                                                     \
          PREFIX VALUE_HANDLE (obj->tio.object->reactors[vcount], reactors,   \
                               code, 330);                                    \
        }                                                                     \
      NOCOMMA;                                                                \
    }                                                                         \
  ENDARRAY;

#define XDICOBJHANDLE(code)                                                   \
  SINCE (R_2004)                                                              \
  {                                                                           \
    if (!obj->tio.object->xdic_missing_flag)                                  \
      {                                                                       \
        KEY (xdicobjhandle);                                                  \
        VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code,    \
                      -3);                                                    \
      }                                                                       \
  }                                                                           \
  PRIOR_VERSIONS                                                              \
  {                                                                           \
    KEY (xdicobjhandle);                                                      \
    VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code, -3);   \
  }

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM

#define DWG_ENTITY(token)                                                     \
  static int dwg_injson_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    Bit_Chain *str_dat = dat;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    LOG_INFO ("Entity " #token ":\n")                                         \
    _ent = obj->tio.entity;                                                   \
    _obj = ent = _ent->tio.token;                                             \
    FIELD_TEXT (entity, #token);                                              \
    _FIELD (type, RL, 0);                                                     \
    _FIELD (size, RL, 0);                                                     \
    _FIELD (bitsize, BL, 0);                                                  \
    ENT_FIELD (preview_exists, B, 0);

#define DWG_ENTITY_END                                                        \
  return 0;                                                                   \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_injson_##token (Bit_Chain *restrict dat,                     \
                                 Dwg_Object *restrict obj)                    \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    Bit_Chain *str_dat = dat;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    Dwg_Object_##token *_obj;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    int error = 0;                                                            \
    LOG_INFO ("Object " #token ":\n")                                         \
    _obj = obj->tio.object->tio.token;                                        \
    FIELD_TEXT (object, #token);                                              \
    _FIELD (type, RL, 0);                                                     \
    _FIELD (size, RL, 0);                                                     \
    _FIELD (bitsize, BL, 0);

#define DWG_OBJECT_END                                                        \
  return 0;                                                                   \
  }

#if 0
//#include "dwg.spec"

/* returns 0 on success
 */
static int
dwg_json_variable_type(Dwg_Data *restrict dwg,
                       Bit_Chain *restrict dat,
                       Dwg_Object *restrict  obj)
{
  int i;
  char *dxfname;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;

  klass = &dwg->dwg_class[i];
  if (!klass || ! klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  dxfname = klass->dxfname;
  // almost always false
  is_entity = dwg_class_is_entity(klass);

#  include "classes.inc"

  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dwg_json_object(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  switch (obj->type)
    {
    case DWG_TYPE_TEXT:
      return dwg_json_TEXT(dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_json_ATTRIB(dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_json_ATTDEF(dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_json_BLOCK(dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_json_ENDBLK(dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_json_SEQEND(dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_json_INSERT(dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_json_MINSERT(dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_json_VERTEX_2D(dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_json_VERTEX_3D(dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_json_VERTEX_MESH(dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_json_VERTEX_PFACE(dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_json_VERTEX_PFACE_FACE(dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_json_POLYLINE_2D(dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_json_POLYLINE_3D(dat, obj);
    case DWG_TYPE_ARC:
      return dwg_json_ARC(dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_json_CIRCLE(dat, obj);
    case DWG_TYPE_LINE:
      return dwg_json_LINE(dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_json_DIMENSION_ORDINATE(dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_json_DIMENSION_LINEAR(dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_json_DIMENSION_ALIGNED(dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_json_DIMENSION_ANG3PT(dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_json_DIMENSION_ANG2LN(dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_json_DIMENSION_RADIUS(dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_json_DIMENSION_DIAMETER(dat, obj);
    case DWG_TYPE_POINT:
      return dwg_json_POINT(dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_json__3DFACE(dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_json_POLYLINE_PFACE(dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_json_POLYLINE_MESH(dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_json_SOLID(dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_json_TRACE(dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_json_SHAPE(dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_json_VIEWPORT(dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_json_ELLIPSE(dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_json_SPLINE(dat, obj);
    case DWG_TYPE_REGION:
      return dwg_json_REGION(dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_json__3DSOLID(dat, obj);
    case DWG_TYPE_BODY:
      return dwg_json_BODY(dat, obj);
    case DWG_TYPE_RAY:
      return dwg_json_RAY(dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_json_XLINE(dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_json_DICTIONARY(dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_json_MTEXT(dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_json_LEADER(dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_json_TOLERANCE(dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_json_MLINE(dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_json_BLOCK_CONTROL(dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      return dwg_json_BLOCK_HEADER(dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_json_LAYER_CONTROL(dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_json_LAYER(dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_json_STYLE_CONTROL(dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_json_STYLE(dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_json_LTYPE_CONTROL(dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_json_LTYPE(dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_json_VIEW_CONTROL(dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_json_VIEW(dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_json_UCS_CONTROL(dat, obj);
    case DWG_TYPE_UCS:
      return dwg_json_UCS(dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_json_VPORT_CONTROL(dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_json_VPORT(dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_json_APPID_CONTROL(dat, obj);
    case DWG_TYPE_APPID:
      return dwg_json_APPID(dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_json_DIMSTYLE_CONTROL(dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_json_DIMSTYLE(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_json_VPORT_ENTITY_CONTROL(dat, obj);
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_json_VPORT_ENTITY_HEADER(dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_json_GROUP(dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_json_MLINESTYLE(dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_json_OLE2FRAME(dat, obj);
    case DWG_TYPE_DUMMY:
      return dwg_json_DUMMY(dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_json_LONG_TRANSACTION(dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_json_LWPOLYLINE(dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_json_HATCH(dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_json_XRECORD(dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_json_PLACEHOLDER(dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_json_PROXY_ENTITY(dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_json_OLEFRAME(dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR("Unhandled Object VBA_PROJECT. Has its own section\n");
      //dwg_json_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_LAYOUT:
      return dwg_json_LAYOUT(dat, obj);
    default:
      if (obj->type == obj->parent->layout_number)
        {
          return dwg_json_LAYOUT(dat, obj);
        }
      /* > 500 */
      else if (DWG_ERR_UNHANDLEDCLASS &
               dwg_json_variable_type(obj->parent, dat, obj))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;

          if (i >= 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity(klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              return dwg_json_UNKNOWN_OBJ(dat, obj);
            }
          else if (klass)
            {
              return dwg_json_UNKNOWN_ENT(dat, obj);
            }
          else // not a class
            {
              LOG_WARN("Unknown object, skipping eed/reactors/xdic");
              SINCE(R_2000)
                {
                  LOG_INFO("Object bitsize: %u\n", obj->bitsize)
                }
              LOG_INFO("Object handle: %d.%d.%lX\n",
                       obj->handle.code, obj->handle.size, obj->handle.value);
            }
        }
    }
  return DWG_ERR_INVALIDTYPE;
}

/*
static void
json_common_entity_handle_data(Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  (void)dat; (void)obj;
}
*/

static int
json_header_read(Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables* _obj = &dwg->header_vars;
  Dwg_Object* obj = NULL;
  const int minimal = dwg->opts & 0x10;
  char buf[4096];
  double ms;
  const char* codepage =
    (dwg->header.codepage == 30 || dwg->header.codepage == 0)
    ? "ANSI_1252"
    : (dwg->header.version >= R_2007)
      ? "UTF-8"
      : "ANSI_1252";

  SECTION(HEADER);
#  include "header_variables.spec"
  ENDSEC();
  return 0;
}

static int
json_classes_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  SECTION(CLASSES);
  LOG_TRACE("num_classes: %u\n", dwg->num_classes);
  for (i=0; i < dwg->num_classes; i++)
    {
      Dwg_Class *_obj = &dwg->dwg_class[i];
      HASH;
      FIELD_BS (number, 0);
      FIELD_TV (dxfname, 1);
      FIELD_T (cppname, 2);
      FIELD_T (appname, 3);
      FIELD_BS (proxyflag, 90);
      FIELD_BL (num_instances, 91);
      FIELD_B  (wasazombie, 280);
      FIELD_BS (item_class_id, 281);
      // Is-an-entity. 1f2 for entities, 1f3 for objects
      //VALUE (281, dwg->dwg_class[i].item_class_id == 0x1F2 ? 1 : 0);
      ENDHASH;
    }
  ENDSEC();
  return 0;
}

static int
json_tables_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;

  SECTION(TABLES);
  //...
  ENDSEC();
  return 0;
}

static int
json_blocks_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;

  SECTION(BLOCKS);
  //...
  ENDSEC();
  return 0;
}

static int
json_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  SECTION(ENTITIES);
  for (i=0; i < dwg->num_objects; i++)
    {
      Dwg_Object *obj = &dwg->object[i];
      HASH;
      dwg_json_object(dat, obj);
      ENDHASH;
    }
  ENDSEC();
  return 0;
}

/* The object map: we skip this. or not */
static int
json_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL j;

  SECTION(OBJECTS);
  for (j = 0; j < dwg->num_objects; j++)
    {
      Dwg_Object *obj = &dwg->object[j];
      HASH;
      dwg_json_object(dat, obj);
      ENDHASH;
    }
  ENDSEC();
  return 0;
}

#endif

// advance until next known first-level type
// on OBJECT to end of OBJECT
// on ARRAY to end of ARRAY
// on STRING (key) get next
// TODO: recursion depth check
static int
json_advance_unknown (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens,
                      int depth)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int error = 0;
  if (tokens->index >= tokens->num_tokens)
    {
      LOG_ERROR ("Unexpected end of JSON at %u of %u tokens",
                 tokens->index, tokens->num_tokens);
      return DWG_ERR_INVALIDDWG;
    }
  if (depth > 300)
    {
      LOG_ERROR ("JSON recursion limit");
      return DWG_ERR_INVALIDDWG;
    }
  LOG_TRACE ("Skip JSON %s %.*s at %u of %u tokens\n",
             t_typename[t->type], t->end - t->start, &dat->chain[t->start],
             tokens->index, tokens->num_tokens);
  switch (t->type)
    {
    case JSMN_OBJECT:
    case JSMN_ARRAY:
      for (int i = 0; i < t->size; i++)
        {
          tokens->index++;
          error |= json_advance_unknown (dat, tokens, depth + 1);
        }
      return error;
    case JSMN_STRING:
    case JSMN_PRIMITIVE:
      tokens->index++;
      return error;
    case JSMN_UNDEFINED:
    default:
      return error;
    }
  return error;
}

static void
json_fixed_key (char *key, Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  int len = t->end - t->start;
  *key = 0;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, 0);
      return;
    }
  if (len >= 80)
    {
      LOG_ERROR ("Expected JSON STRING");
      tokens->index++;
      return;
    }
  memcpy (key, &dat->chain[t->start], len);
  key[len] = '\0';
  tokens->index++;
  return;
}

ATTRIBUTE_MALLOC
static char *
json_string (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  char *key;
  int len = t->end - t->start;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected JSON STRING");
      json_advance_unknown (dat, tokens, 0);
      return NULL;
    }
  key = malloc (len + 1);
  memcpy (key, &dat->chain[t->start], len);
  key[len] = '\0';
  tokens->index++;
  return key;
}

static double
json_float (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  //int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, 0);
      return (double)NAN;
    }
  tokens->index++;
  return strtod ((char *)&dat->chain[t->start], NULL);
}
static long
json_long (Bit_Chain *restrict dat, jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  //int len = t->end - t->start;
  if (t->type != JSMN_PRIMITIVE)
    {
      LOG_ERROR ("Expected JSON PRIMITIVE");
      json_advance_unknown (dat, tokens, 0);
      return 0;
    }
  tokens->index++;
  return strtol ((char *)&dat->chain[t->start], NULL, 10);
}

ATTRIBUTE_MALLOC
static BITCODE_H
json_HANDLE (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens, const char *name)
{
  long code, value;
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type != JSMN_ARRAY || t->size != 2)
    {
      LOG_ERROR ("JSON HANDLE must be ARRAY of [ code, value ]")
      return NULL;
    }
  tokens->index++;
  code = json_long (dat, tokens);
  value = json_long (dat, tokens);
  LOG_TRACE ("%s [%u, %u] [H]\n", name, (unsigned)code, (unsigned)value);
  return dwg_add_handleref (dwg, code, value, NULL);
}

static void
json_CMC (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
          jsmntokens_t *restrict tokens, const char *name,
          Dwg_Color *restrict color)
{
  char key[80];
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  if (t->type == JSMN_OBJECT)
    {
      tokens->tokens++; // hash of index, rgb...
      for (int j = 0; j < t->size; j++)
        {
          json_fixed_key (key, dat, tokens);
          if (strEQc (key, "index"))
            {
              long num = json_long (dat, tokens);
              LOG_TRACE ("%s.index %ld [CMC]\n", name, num);
              color->index = (BITCODE_BSd)num;
            }
          else if (strEQc (key, "rgb"))
            {
              char hex[80];
              json_fixed_key (hex, dat, tokens);
              LOG_TRACE ("%s.rgb %s [CMC]\n", name, hex);
              sscanf (hex, "%x", &color->rgb);
            }
          else if (strEQc (key, "flag"))
            {
              long num = json_long (dat, tokens);
              LOG_TRACE ("%s.flag %u [CMC]\n", name, (unsigned)num);
              color->flag = (BITCODE_BS)num;
            }
          else if (strEQc (key, "alpha"))
            {
              long num = json_long (dat, tokens);
              LOG_TRACE ("%s.alpha %u [CMC]\n", name, (unsigned)num);
              color->alpha = (BITCODE_RC)num;
              color->alpha_type = 3;
            }
          else if (strEQc (key, "handle")) // [4, value] ARRAY
            {
              long code, value;
              tokens->index++;
              code = json_long (dat, tokens);
              value = json_long (dat, tokens);
              LOG_TRACE ("%s.handle [%u, %u] [H]\n", name, (unsigned)code, (unsigned)value);
              color->handle = dwg_add_handleref (dwg, code, value, NULL);
            }
          else if (strEQc (key, "name"))
            {
              char *str = json_string (dat, tokens);
              LOG_TRACE ("%s.name \"%s\" [CMC]\n", name, str);
              color->name = str;
            }
          else if (strEQc (key, "book_name"))
            {
              char *str = json_string (dat, tokens);
              LOG_TRACE ("%s.book_name \"%s\" [CMC]\n", name, str);
              color->book_name = str;
            }
          else
            {
              LOG_WARN ("Unknown key color.%s", key);
              tokens->index++;
            }
        }
    }
}

static void
json_TIMEBLL (Bit_Chain *restrict dat,
              jsmntokens_t *restrict tokens, const char *name,
              BITCODE_TIMEBLL *date)
{
  unsigned long j = 1;
  double ms;
  double num = json_float (dat, tokens);
  date->value = num;
  date->days = (BITCODE_BL)trunc (num);
  ms = date->value;
  while (ms > 1.0)
    {
      j *= 10;
      ms /= 10.0;
    }
  date->ms = (BITCODE_BL) (j / 10 * (date->value - date->days));
  LOG_TRACE ("%s %f (%u, %u) [TIMEBLL]\n", name,
             date->value, date->days, date->ms);
}

static int
json_created_by (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                 jsmntokens_t *restrict tokens)
{
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_STRING)
    {
      LOG_ERROR ("Expected %s STRING", "created_by");
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  created_by = json_string (dat, tokens);
  tokens->index--; // advanced by the loop
  return 0;
}

static int
json_FILEHEADER (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                 jsmntokens_t *restrict tokens)
{
  const char *section = "FILEHEADER";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  struct Dwg_Header *_obj = &dwg->header;
  Dwg_Object *obj = NULL;
  char version[80];
  int size = t->size;

  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  LOG_TRACE ("\n%s pos:%d [%d keys]\n", section, tokens->index, t->size);
  tokens->tokens++;
  // t = &tokens->tokens[tokens->index];
  // json_expect(tokens, STRING);
  // FIELD_TV (version, 0);
  for (int i = 0; i < size; i++)
    {
      char key[80];
      json_fixed_key (key, dat, tokens);
      if (strEQc (key, "version"))
        {
          version[0] = '\0';
          json_fixed_key (version, dat, tokens);
          // set version's
          for (Dwg_Version_Type v = 0; v <= R_AFTER; v++)
            {
              if (strEQ (version, version_codes[v]))
                {
                  dwg->header.version = dwg->header.from_version = v;
                  dat->version = dat->from_version = dwg->header.version;
                  //is_utf = dat->version >= R_2007;
                  LOG_TRACE ("HEADER.version = dat->version = %s\n", version);
                  /*
                    if (is_utf && dwg->num_objects && dwg->object[0].fixedtype == DWG_TYPE_BLOCK_HEADER)
                    {
                    Dwg_Object_BLOCK_HEADER *_o = dwg->object[0].tio.object->tio.BLOCK_HEADER;
                    free (_o->name);
                    _o->name = (char*)bit_utf8_to_TU ((char*)"*Model_Space");
                    }
                  */
                  break;
                }
              if (v == R_AFTER)
                LOG_ERROR ("Invalid FILEHEADER.version %s", version);
            }
        }
      else if (strEQc (key, "zero_5"))
        {
          json_expect(tokens, ARRAY);
          json_advance_unknown (dat, tokens, 0);
        }

      FIELD_RC (is_maint, 0)
      FIELD_RC (zero_one_or_three, 0)
      FIELD_RL (thumbnail_address, 0) //@0x0d
      FIELD_RC (dwg_version, 0)
      FIELD_RC (maint_version, 0)
      FIELD_RS (codepage, 0) //@0x13: 29/30 for ANSI_1252, since r2007 UTF-16
      //SINCE (R_2004)
      FIELD_RC (unknown_0, 0)
      FIELD_RC (app_dwg_version, 0)
      FIELD_RC (app_maint_version, 0)
      FIELD_RL (security_type, 0)
      FIELD_RL (rl_1c_address, 0) /* mostly 0 */
      FIELD_RL (summaryinfo_address, 0)
      FIELD_RL (vbaproj_address, 0)
      FIELD_RL (rl_28_80, 0) /* mostly 128/0x80 */

      else if (strEQc (key, "HEADER"))
        {
          LOG_WARN ("Unexpected next section %s", key)
          tokens->tokens--;
          tokens->tokens--;
          return 0;
        }
      else
        {
          LOG_ERROR ("Unknown %s.%s ignored", section, key);
          tokens->index++;
        }
    }
  return -1;
}

static int
json_HEADER (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens)
{
  const char *section = "HEADER";
  jsmntok_t *t = &tokens->tokens[tokens->index];
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  int size = t->size;
  int is_utf = 1;

  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  LOG_TRACE ("\n%s pos:%d [%d keys]\n", section, tokens->index, t->size);
  tokens->tokens++;
  for (int i = 0; i < size; i++)
    {
      char key[80];
      Dwg_DYNAPI_field *f;
      json_fixed_key (key, dat, tokens);
      t = &tokens->tokens[tokens->index];
      f = (Dwg_DYNAPI_field *)dwg_dynapi_header_field (key);
      if (!f)
        {
          LOG_WARN ("Unknown key HEADER.%s", key)
          json_advance_unknown (dat, tokens, 0);
          continue;
        }
      else if (strEQc (f->type, "BD") || strEQc (f->type, "RD"))
        {
          double num = json_float (dat, tokens);
          LOG_TRACE ("%s: " FORMAT_RD " [%s]\n", key, num, f->type)
          dwg_dynapi_header_set_value (dwg, key, &num, 0);
        }
      else if (strEQc (f->type, "RC") || strEQc (f->type, "B")
               || strEQc (f->type, "BB") || strEQc (f->type, "RS")
               || strEQc (f->type, "BS") || strEQc (f->type, "RL")
               || strEQc (f->type, "BL") || strEQc (f->type, "RLL")
               || strEQc (f->type, "BLL"))
        {
          long num = json_long (dat, tokens);
          LOG_TRACE ("%s: %ld [%s]\n", key, num, f->type)
          dwg_dynapi_header_set_value (dwg, key, &num, 0);
        }
      else if (strEQc (f->type, "TV") || strEQc (f->type, "T"))
        {
          char *str = json_string (dat, tokens);
          LOG_TRACE ("%s: \"%s\" [%s]\n", key, str, f->type)
          dwg_dynapi_header_set_value (dwg, key, &str, 1);
        }
      else if (strEQc (f->type, "TIMEBLL"))
        {
          static BITCODE_TIMEBLL date = { 0, 0, 0 };
          json_TIMEBLL (dat, tokens, key, &date);
          dwg_dynapi_header_set_value (dwg, key, &date, 0);
        }
      else if (strEQc (f->type, "CMC"))
        {
          static BITCODE_CMC color = { 0, 0, 0 };
          json_CMC (dat, dwg, tokens, key, &color);
          dwg_dynapi_header_set_value (dwg, key, &color, 0);
        }
      else if (strEQc (f->type, "H"))
        {
          BITCODE_H hdl = json_HANDLE (dat, dwg, tokens, key);
          dwg_dynapi_header_set_value (dwg, key, &hdl, 0);
        }
      //...
      else if (strEQc (key, "CLASSES"))
        {
          LOG_WARN ("Unexpected next section %s", key)
          tokens->tokens--;
          tokens->tokens--;
          return 0;
        }
      else
        {
          LOG_WARN ("Unhandled %s [%s]", key, f->type)
          tokens->tokens++;
          continue;
        }
      /*
      else if (t->type == JSMN_ARRAY) // points
        {
          dwg_dynapi_header_set_value (dwg, key, &pt, NULL);
        }
      */
    }
  
  return -1;
}

static int
json_CLASSES (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens)
{
  const char *section = "CLASSES";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_OBJECTS (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
             jsmntokens_t *restrict tokens)
{
  const char *section = "OBJECTS";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_HANDLES (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
              jsmntokens_t *restrict tokens)
{
  const char *section = "HANDLES";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_ARRAY)
    {
      LOG_ERROR ("Expected %s ARRAY at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_THUMBNAILIMAGE (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "THUMBNAILIMAGE";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_R2004_Header (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                   jsmntokens_t *restrict tokens)
{
  const char *section = "R2004_Header";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_SummaryInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "SummaryInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_AppInfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfo";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_AppInfoHistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "AppInfoHistory";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_FileDepList (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "FileDepList";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_Security (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "Security";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_RevHistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "RevHistory";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_ObjFreeSpace (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                  jsmntokens_t *restrict tokens)
{
  const char *section = "ObjFreeSpace";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

static int
json_Template (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
               jsmntokens_t *restrict tokens)
{
  const char *section = "Template";
  const jsmntok_t *t = &tokens->tokens[tokens->index];
  (void)dat; (void)dwg;
  if (t->type != JSMN_OBJECT)
    {
      LOG_ERROR ("Expected %s OBJECT at %u of %u tokens, got %s", section,
                 tokens->index, tokens->num_tokens, t_typename[t->type]);
      json_advance_unknown (dat, tokens, 0);
      return DWG_ERR_INVALIDTYPE;
    }
  // ... dynapi
  return -1;
}

EXPORT int
dwg_read_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_Header *obj = &dwg->header;
  jsmn_parser parser;
  //int num_tokens;
  //jsmntok_t *tokens;
  jsmntokens_t tokens;
  unsigned int i;
  int error = -1;

  loglevel = dwg->opts & 0xf;
  if (!dat->chain && dat->fh)
    {
      error = dat_read_stream (dat, dat->fh);
      if (error > DWG_ERR_CRITICAL)
        return error;
      LOG_TRACE ("  json file size: %lu\n", dat->size);
    }
  g_dat = dat;

  jsmn_init (&parser);
  // How big will it be? This is the max. memory variant.
  // we could also use less, see jsmn/examples/jsondump.c for small devices.
  tokens.num_tokens = jsmn_parse (&parser, (char *)dat->chain, dat->size, NULL, 0);
  if (tokens.num_tokens <= 0)
    {
      char err[21];
      memcpy (&err, &dat->chain[parser.pos - 10], 20);
      err[20] = 0;
      LOG_ERROR ("Invalid json. jsmn error at pos: %u (...%s...)", parser.pos,
                 err);
      return DWG_ERR_INVALIDDWG;
    }
  LOG_TRACE ("  num_tokens: %d\n", tokens.num_tokens);
  tokens.tokens = calloc (tokens.num_tokens + 1024, sizeof (jsmntok_t));
  if (!tokens.tokens)
    return DWG_ERR_OUTOFMEM;

  jsmn_init (&parser); // reset pos to 0
  error = jsmn_parse (&parser, (char *)dat->chain, dat->size, tokens.tokens,
                      tokens.num_tokens);
  if (error < 0)
    {
      char err[21];
      memcpy (&err, &dat->chain[parser.pos - 10], 20);
      err[20] = 0;
      LOG_ERROR (
          "Invalid json. jsmn error %d at the %u-th token, pos: %u (...%s...)",
          error, parser.toknext, parser.pos, err);
      return DWG_ERR_INVALIDDWG;
    }

  if (tokens.tokens[0].type != JSMN_OBJECT)
    {
      fprintf (stderr, "First JSON element is not an object/hash\n");
      exit (1);
    }

  // valid first level tokens:
  // created_by: string
  // section objects: FILEHEADER, HEADER, THUMBNAILIMAGE, R2004_Header, SummaryInfo, AppInfo,
  //                  AppInfoHistory, FileDepList, Security, RevHistory, ObjFreeSpace, Template
  // section arrays: CLASSES, OBJECTS, HANDLES
  for (tokens.index = 1; tokens.index < tokens.num_tokens; tokens.index++)
    {
      char key[80];
      const jsmntok_t *t = &tokens.tokens[tokens.index];
      if (t->type != JSMN_STRING)
        {
          LOG_ERROR ("Expected JSON key at %u of %u tokens, got %s",
                     tokens.index, tokens.num_tokens, t_typename[t->type]);
          return DWG_ERR_INVALIDDWG;
        }
        /*
        case JSMN_OBJECT:
          {
            //int j;
#ifdef JSMN_PARENT_LINKS
            printf ("parent: [%d]\n", t->parent); // always i-1 for arrays and objects
#endif
            // one of: FILEHEADER, HEADER, THUMBNAILIMAGE, R2004_Header, SummaryInfo, AppInfo, AppInfoHistory
            printf ("OBJECT[%d](%d) %.*s\n", i, t->size, t->end - t->start, &dat->chain[t->start]);
            // check keys
          }
          break;
        case JSMN_PRIMITIVE:
          printf ("[%d] %.*s\n", i, t->end - t->start, &dat->chain[t->start]);
          break;
        case JSMN_STRING:
          */
          {
            int len = t->end - t->start;
            if (len < 80)
              {
                memcpy (key, &dat->chain[t->start], len);
                key[len] = '\0';
                tokens.index++;
                if (tokens.index >= tokens.num_tokens)
                  {
                    LOG_ERROR ("Unexpected end of JSON at %u of %u tokens",
                               tokens.index, tokens.num_tokens);
                    return DWG_ERR_INVALIDDWG;
                  }
                if (strEQc (key, "created_by"))
                  error |= json_created_by (dat, dwg, &tokens);
                else if (strEQc (key, "FILEHEADER"))
                  error |= json_FILEHEADER (dat, dwg, &tokens);
                else if (strEQc (key, "HEADER"))
                  error |= json_HEADER (dat, dwg, &tokens);
                else if (strEQc (key, "CLASSES"))
                  error |= json_CLASSES (dat, dwg, &tokens);
                else if (strEQc (key, "OBJECTS"))
                  error |= json_OBJECTS (dat, dwg, &tokens);
                else if (strEQc (key, "THUMBNAILIMAGE"))
                  error |= json_THUMBNAILIMAGE (dat, dwg, &tokens);
                else if (strEQc (key, "R2004_Header"))
                  error |= json_R2004_Header (dat, dwg, &tokens);
                else if (strEQc (key, "SummaryInfo"))
                  error |= json_SummaryInfo (dat, dwg, &tokens);
                else if (strEQc (key, "AppInfo"))
                  error |= json_AppInfo (dat, dwg, &tokens);
                else if (strEQc (key, "AppInfoHistory"))
                  error |= json_AppInfoHistory (dat, dwg, &tokens);
                else if (strEQc (key, "FileDepList"))
                  error |= json_FileDepList (dat, dwg, &tokens);
                else if (strEQc (key, "Security"))
                  error |= json_Security (dat, dwg, &tokens);
                else if (strEQc (key, "RevHistory"))
                  error |= json_RevHistory (dat, dwg, &tokens);
                else if (strEQc (key, "ObjFreeSpace"))
                  error |= json_ObjFreeSpace (dat, dwg, &tokens);
                else if (strEQc (key, "Template"))
                  error |= json_Template (dat, dwg, &tokens);
                else if (strEQc (key, "HANDLES"))
                  error |= json_HANDLES (dat, dwg, &tokens);
                else
                  {
                    LOG_ERROR ("Unexpected JSON key %s at %u of %u tokens", key, tokens.index,
                               tokens.num_tokens);
                    return DWG_ERR_INVALIDDWG;
                  }
              }
            //printf ("[%d] \"%.*s\"\n", i, t->end - t->start, &dat->chain[t->start]);
          }
          /*
          break;
        case JSMN_ARRAY:
#ifdef JSMN_PARENT_LINKS
          printf ("parent: [%d]\n", t->parent);
#endif
          printf ("ARRAY[%d] %.*s\n", i, t->end - t->start, &dat->chain[t->start]);
          break;
        case JSMN_UNDEFINED:
        default:
          LOG_ERROR ("Invalid json token type %d", t->type);
          return DWG_ERR_INVALIDDWG;
        }

      // TODO walk the sections and keys
      // json_header_read   (tokens, num_tokens, dwg);
      // json_classes_read  (tokens, num_tokens, dwg);
      // json_tables_read   (tokens, num_tokens, dwg);
      // json_blocks_read   (tokens, num_tokens, dwg);
      // json_entities_read (tokens, num_tokens, dwg);
      // json_objects_read  (tokens, num_tokens, dwg);
      // json_preview_read  (tokens, num_tokens, dwg);
      */
    }

  return 0;
}

#undef IS_ENCODE
#undef IS_JSON

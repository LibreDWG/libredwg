/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2024 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * out_json.c: write as JSON
 * written by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#ifdef HAVE_ICONV
#  include <iconv.h>
#endif

#define IS_JSON
#include "common.h"
#include "codepages.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"
#include "out_json.h"
#include "free.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static BITCODE_BL rcount1, rcount2;

/* see also examples/unknown.c */
#ifdef HAVE_NATIVE_WCHAR2
static wchar_t *wcquote (wchar_t *restrict dest, const wchar_t *restrict src);
#else
static void print_wcquote (Bit_Chain *restrict dat,
                           dwg_wchar_t *restrict wstr);
#endif
void json_write_TFv (Bit_Chain *restrict dat, const BITCODE_TF restrict src,
                     const size_t len);
// write even past the \0, to keep existing slack
void json_write_TF (Bit_Chain *restrict dat, const BITCODE_TF restrict src,
                    const size_t len);

static int json_3dsolid (Bit_Chain *restrict dat,
                         const Dwg_Object *restrict obj,
                         Dwg_Entity_3DSOLID *restrict _obj);
static void _prefix (Bit_Chain *dat);
static char *_path_field (const char *path);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION json

#define ISFIRST (dat->opts & DWG_OPTS_JSONFIRST)
#define SETFIRST dat->opts |= DWG_OPTS_JSONFIRST
#define CLEARFIRST dat->opts &= ~DWG_OPTS_JSONFIRST

#define PREFIX _prefix (dat);
#define IS_MINJS (dat->opts & DWG_OPTS_MINIMAL)
#define JSON_SPC IS_MINJS ? "" : " "
#define JSON_NL IS_MINJS ? "" : "\n"
#define JSON_KEY "\"%s\":%s"

#define PRINTFIRST                                                            \
  {                                                                           \
    if (!ISFIRST)                                                             \
      fprintf (dat->fh, ",%s", JSON_NL);                                      \
    else                                                                      \
      CLEARFIRST;                                                             \
  }
#define FIRSTPREFIX PRINTFIRST PREFIX

#define KEYs(nam) FIRSTPREFIX fprintf (dat->fh, "\"%s\":%s", nam, JSON_SPC)
// strip path to field only
#define KEY(nam)                                                              \
  FIRSTPREFIX fprintf (dat->fh, "\"%s\":%s", _path_field (#nam), JSON_SPC)

#define ARRAY                                                                 \
  {                                                                           \
    fprintf (dat->fh, "[%s", JSON_NL);                                        \
    SETFIRST;                                                                 \
    dat->bit++;                                                               \
  }
#define ENDARRAY                                                              \
  {                                                                           \
    fprintf (dat->fh, "%s", JSON_NL);                                         \
    dat->bit--;                                                               \
    PREFIX fprintf (dat->fh, "]");                                            \
    CLEARFIRST;                                                               \
  }
#define HASH                                                                  \
  {                                                                           \
    fprintf (dat->fh, "{%s", JSON_NL);                                        \
    SETFIRST;                                                                 \
    dat->bit++;                                                               \
  }
#define ENDHASH                                                               \
  {                                                                           \
    fprintf (dat->fh, "%s", JSON_NL);                                         \
    dat->bit--;                                                               \
    PREFIX fprintf (dat->fh, "}");                                            \
    CLEARFIRST;                                                               \
  }

#define TABLE(nam)                                                            \
  KEY (nam);                                                                  \
  HASH
#define ENDTAB() ENDHASH
// a named hash
#define RECORD(nam)                                                           \
  KEY (nam);                                                                  \
  HASH
#define ENDRECORD() ENDHASH
// a named list
#define SECTION(nam)                                                          \
  KEY (nam);                                                                  \
  ARRAY
#define ENDSEC() ENDARRAY

#undef FORMAT_H
#undef FORMAT_RLL
#undef FORMAT_BLL
#undef FORMAT_RC
#undef FORMAT_RSx
#undef FORMAT_RLx
#undef FORMAT_BLx
#undef FORMAT_BLX
#undef FORMAT_4BITS
#undef FORMAT_RD
#undef FORMAT_BD
#define FORMAT_RD "%.14f"
#define FORMAT_BD FORMAT_RD
#define FORMAT_RLL "%" PRIu64
#define FORMAT_BLL "%" PRIu64
#define FORMAT_HREF "[%u,%u," FORMAT_RLL "," FORMAT_RLL "]"
#undef FORMAT_HREF11
#define FORMAT_HREF11 "[%u,%hd," FORMAT_RLL "]"
#undef ARGS_H
#define ARGS_H(hdl) hdl.code, hdl.size, hdl.value
#define FORMAT_H "[%u,%u," FORMAT_RLL "]"
#define ARGS_HREF(ref)                                                        \
  ref->handleref.code, ref->handleref.size, ref->handleref.value,             \
      ref->absolute_ref

#define ARGS_HREF11(ref) ref->handleref.size, ref->r11_idx, ref->absolute_ref
#define FORMAT_RC "%d"
#define FORMAT_RSx FORMAT_RS
#define FORMAT_RLx FORMAT_RL
#define FORMAT_BLx FORMAT_BL
#define FORMAT_BLX FORMAT_BL
#define FORMAT_4BITS FORMAT_RC

#define VALUE(value, type, dxf) fprintf (dat->fh, FORMAT_##type, value)
#define VALUE_B(value, dxf) VALUE (value, B, dxf)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
#define VALUE_RLx(value, dxf) VALUE ((BITCODE_RL)value, RL, dxf)
#define VALUE_RLL(value, dxf) VALUE (value, RLL, dxf)
#ifdef IS_RELEASE
#  define VALUE_RD(value, dxf)                                                \
    if (bit_isnan (value))                                                    \
      _VALUE_RD (0.0, dxf)                                                    \
    else                                                                      \
      _VALUE_RD (value, dxf)
#else
#  define VALUE_RD(value, dxf) _VALUE_RD (value, dxf)
#endif
#define _VALUE_RD(value, dxf)                                                 \
  {                                                                           \
    char _buf[256];                                                           \
    size_t k;                                                                 \
    snprintf (_buf, 255, FORMAT_RD, value);                                   \
    k = strlen (_buf);                                                        \
    if (k > 0 && strrchr (_buf, '.') && _buf[k - 1] == '0')                   \
      {                                                                       \
        for (k--; k > 1 && _buf[k - 1] != '.' && _buf[k] == '0'; k--)         \
          _buf[k] = '\0';                                                     \
      }                                                                       \
    fprintf (dat->fh, "%s", _buf);                                            \
  }
#define VALUE_2RD(pt, dxf)                                                    \
  {                                                                           \
    fprintf (dat->fh, "[%s", JSON_SPC);                                       \
    VALUE_RD (pt.x, 0);                                                       \
    fprintf (dat->fh, ",%s", JSON_SPC);                                       \
    VALUE_RD (pt.y, 0);                                                       \
    fprintf (dat->fh, "%s]%s", JSON_SPC, JSON_SPC);                           \
  }
#define VALUE_2DD(pt, def, dxf) VALUE_2RD (pt, dxf)
#define VALUE_3RD(pt, dxf)                                                    \
  {                                                                           \
    fprintf (dat->fh, "[%s", JSON_SPC);                                       \
    VALUE_RD (pt.x, 0);                                                       \
    fprintf (dat->fh, ",%s", JSON_SPC);                                       \
    VALUE_RD (pt.y, 0);                                                       \
    fprintf (dat->fh, ",%s", JSON_SPC);                                       \
    VALUE_RD (pt.z, 0);                                                       \
    fprintf (dat->fh, "%s]%s", JSON_SPC, JSON_SPC);                           \
  }
#define VALUE_3BD(pt, dxf) VALUE_3RD (pt, dxf)
#define VALUE_TV(nam, dxf)

#define FIELD(nam, type, dxf)                                                 \
  if (!memBEGINc (#nam, "num_"))                                              \
    {                                                                         \
      FIRSTPREFIX fprintf (dat->fh, JSON_KEY FORMAT_##type,                   \
                           _path_field (#nam), JSON_SPC, _obj->nam);          \
    }
#define _FIELD(nam, type, value)                                              \
  {                                                                           \
    FIRSTPREFIX fprintf (dat->fh, "\"" #nam "\":%s" FORMAT_##type, JSON_SPC,  \
                         obj->nam);                                           \
  }
#define ENT_FIELD(nam, type, value)                                           \
  {                                                                           \
    FIRSTPREFIX fprintf (dat->fh, JSON_KEY FORMAT_##type, _path_field (#nam), \
                         JSON_SPC, _ent->nam);                                \
  }
#define SUB_FIELD(o, nam, type, dxf)                                          \
  if (!memBEGINc (#nam, "num_"))                                              \
    {                                                                         \
      FIRSTPREFIX fprintf (dat->fh, JSON_KEY FORMAT_##type,                   \
                           _path_field (#nam), JSON_SPC, _obj->o.nam);        \
    }
#define FIELD_CAST(nam, type, cast, dxf) FIELD (nam, cast, dxf)
#define SUB_FIELD_CAST(o, nam, type, cast, dxf) SUB_FIELD (o, nam, cast, dxf)
#define FIELD_TRACE(nam, type)
#define FIELD_G_TRACE(nam, type, dxf)
#define FIELD_TEXT(nam, str)                                                  \
  {                                                                           \
    FIRSTPREFIX fprintf (dat->fh, JSON_KEY, _path_field (#nam), JSON_SPC);    \
    VALUE_TEXT ((char *)str)                                                  \
  }

// Converts to UTF-8
#define VALUE_TEXT(str)                                                       \
  {                                                                           \
    if (str)                                                                  \
      {                                                                       \
        const size_t len = strlen (str);                                      \
        if (len < 42)                                                         \
          {                                                                   \
            const size_t _len = 6 * len + 1;                                  \
            char _buf[256];                                                   \
            fprintf (dat->fh, "\"%s\"",                                       \
                     json_cquote (_buf, str, _len, dat->codepage));           \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            const size_t _len = 6 * len + 1;                                  \
            char *_buf = (char *)MALLOC (_len);                               \
            fprintf (dat->fh, "\"%s\"",                                       \
                     json_cquote (_buf, str, _len, dat->codepage));           \
            FREE (_buf);                                                      \
          }                                                                   \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        fprintf (dat->fh, "\"%s\"", str ? str : "");                          \
      }                                                                       \
  }

// Converts to UTF-8
#ifdef HAVE_NATIVE_WCHAR2
#  define VALUE_TEXT_TU(wstr)                                                 \
    if (wstr)                                                                 \
      {                                                                       \
        wchar_t *_buf = MALLOC (6 * wcslen ((wchar_t *)wstr) + 2);            \
        fprintf (dat->fh, "\"%ls\"", wcquote (_buf, (wchar_t *)wstr));        \
        FREE (_buf);                                                          \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        fprintf (dat->fh, "\"%ls\"", wstr ? (wchar_t *)wstr : L"");           \
      }
#else
#  define VALUE_TEXT_TU(wstr) print_wcquote (dat, (BITCODE_TU)wstr)
#endif
#define FIELD_TEXT_TU(nam, wstr)                                              \
  {                                                                           \
    KEY (nam);                                                                \
    VALUE_TEXT_TU ((BITCODE_TU)wstr);                                         \
  }
// may be downgraded from TV, thus shorter
#define FIELD_TFv(nam, len, dxf)                                              \
  {                                                                           \
    FIRSTPREFIX fprintf (dat->fh, JSON_KEY, _path_field (#nam), JSON_SPC);    \
    json_write_TFv (dat, (const BITCODE_TF)_obj->nam, len);                   \
  }
#define FIELD_TF(nam, len, dxf)                                               \
  {                                                                           \
    FIRSTPREFIX fprintf (dat->fh, JSON_KEY, _path_field (#nam), JSON_SPC);    \
    json_write_TF (dat, (const BITCODE_TF)_obj->nam, len);                    \
  }
#define FIELD_TFF(nam, len, dxf) FIELD_TF (nam, len, dxf)

#define FIELD_VALUE(nam) _obj->nam
#define ANYCODE -1
// todo: only the name, not the ref
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  if (hdlptr)                                                                 \
    {                                                                         \
      PRE (R_13b1)                                                            \
      {                                                                       \
        fprintf (dat->fh, FORMAT_HREF11 "", ARGS_HREF11 (hdlptr));            \
      }                                                                       \
      LATER_VERSIONS                                                          \
      {                                                                       \
        fprintf (dat->fh, FORMAT_HREF "", ARGS_HREF (hdlptr));                \
      }                                                                       \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      fprintf (dat->fh, "[0,0,0]");                                           \
    }
#define VALUE_H(hdl, dxf) fprintf (dat->fh, FORMAT_H "", ARGS_H (hdl))
#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  {                                                                           \
    if (_obj->nam)                                                            \
      {                                                                       \
        PRE (R_13b1)                                                          \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, JSON_KEY FORMAT_HREF11 "",            \
                               _path_field (#nam), JSON_SPC,                  \
                               ARGS_HREF11 (_obj->nam));                      \
        }                                                                     \
        LATER_VERSIONS                                                        \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, JSON_KEY FORMAT_HREF "",              \
                               _path_field (#nam), JSON_SPC,                  \
                               ARGS_HREF (_obj->nam));                        \
        }                                                                     \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIRSTPREFIX fprintf (dat->fh, JSON_KEY "[0,%s0]", _path_field (#nam), \
                             JSON_SPC, JSON_SPC);                             \
      }                                                                       \
  }
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  {                                                                           \
    if (_obj->o.nam)                                                          \
      {                                                                       \
        FIRSTPREFIX fprintf (dat->fh, JSON_KEY FORMAT_HREF "",                \
                             _path_field (#nam), JSON_SPC,                    \
                             ARGS_HREF (_obj->o.nam));                        \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIRSTPREFIX fprintf (dat->fh, JSON_KEY "[0,%s0]", _path_field (#nam), \
                             JSON_SPC, JSON_SPC);                             \
      }                                                                       \
  }
#define FIELD_DATAHANDLE(nam, code, dxf) FIELD_HANDLE (nam, code, dxf)
#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf)                         \
  PRINTFIRST;                                                                 \
  if (_obj->nam)                                                              \
    {                                                                         \
      PRE (R_13b1)                                                            \
      {                                                                       \
        PREFIX fprintf (dat->fh, FORMAT_HREF11 "", ARGS_HREF11 (_obj->nam));  \
      }                                                                       \
      LATER_VERSIONS                                                          \
      {                                                                       \
        PREFIX fprintf (dat->fh, FORMAT_HREF "", ARGS_HREF (_obj->nam));      \
      }                                                                       \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      PREFIX fprintf (dat->fh, "[0,%s0]", JSON_SPC);                          \
    }
#define SUB_FIELD_HANDLE_N(o, nam, handle_code, dxf)                          \
  PRINTFIRST;                                                                 \
  if (_obj->o.nam)                                                            \
    {                                                                         \
      PREFIX fprintf (dat->fh, FORMAT_HREF "", ARGS_HREF (_obj->o.nam));      \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      PREFIX fprintf (dat->fh, "[0,%s0]", JSON_SPC);                          \
    }
#define VALUE_BINARY(buf, len, dxf)                                           \
  {                                                                           \
    fprintf (dat->fh, "\"");                                                  \
    if (buf && len)                                                           \
      {                                                                       \
        for (long _j = 0; _j < (long)len; _j++)                               \
          {                                                                   \
            fprintf (dat->fh, "%02X", ((BITCODE_RC *)buf)[_j]);               \
          }                                                                   \
      }                                                                       \
    fprintf (dat->fh, "\"");                                                  \
  }
#define FIELD_BINARY(nam, size, dxf)                                          \
  {                                                                           \
    KEY (nam);                                                                \
    fprintf (dat->fh, "\"");                                                  \
    if (_obj->nam)                                                            \
      {                                                                       \
        for (long _j = 0; _j < (long)size; _j++)                              \
          {                                                                   \
            fprintf (dat->fh, "%02X", ((BITCODE_RC *)_obj->nam)[_j]);         \
          }                                                                   \
      }                                                                       \
    fprintf (dat->fh, "\"");                                                  \
  }

#define FIELD_B(nam, dxf) FIELD (nam, B, dxf)
#define FIELD_BB(nam, dxf) FIELD (nam, BB, dxf)
#define FIELD_3B(nam, dxf) FIELD (nam, 3B, dxf)
#define FIELD_BS(nam, dxf) FIELD (nam, BS, dxf)
#define FIELD_BSd(nam, dxf) FIELD (nam, BSd, dxf)
#define FIELD_BL(nam, dxf) FIELD (nam, BL, dxf)
#define FIELD_BLd(nam, dxf) FIELD (nam, BLd, dxf)
#define FIELD_BLL(nam, dxf) FIELD (nam, BLL, dxf)
#ifdef IS_RELEASE
#  define FIELD_BD(nam, dxf)                                                  \
    {                                                                         \
      if (!bit_isnan (_obj->nam))                                             \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, JSON_KEY, _path_field (#nam),         \
                               JSON_SPC);                                     \
          _VALUE_RD (_obj->nam, dxf);                                         \
        }                                                                     \
    }
#  define FIELD_2RD(nam, dxf)                                                 \
    {                                                                         \
      if (!bit_isnan (_obj->nam.x) && !bit_isnan (_obj->nam.y))               \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, "\"" #nam "\":%s", JSON_SPC);         \
          VALUE_2RD (_obj->nam, dxf);                                         \
        }                                                                     \
    }
#  define FIELD_3RD(nam, dxf)                                                 \
    {                                                                         \
      if (!bit_isnan (_obj->nam.x) && !bit_isnan (_obj->nam.y)                \
          && !bit_isnan (_obj->nam.z))                                        \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, "\"" #nam "\":%s", JSON_SPC);         \
          VALUE_3RD (_obj->nam, dxf);                                         \
        }                                                                     \
    }
#  define SUB_FIELD_BD(o, nam, dxf)                                           \
    {                                                                         \
      if (!bit_isnan (_obj->o.nam))                                           \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, JSON_KEY, _path_field (#nam),         \
                               JSON_SPC);                                     \
          _VALUE_RD (_obj->o.nam, dxf);                                       \
        }                                                                     \
    }
#else /* IS_RELEASE */
#  define FIELD_BD(nam, dxf)                                                  \
    {                                                                         \
      FIRSTPREFIX fprintf (dat->fh, JSON_KEY, _path_field (#nam), JSON_SPC);  \
      _VALUE_RD (_obj->nam, dxf);                                             \
    }
#  define FIELD_2RD(nam, dxf)                                                 \
    {                                                                         \
      FIRSTPREFIX fprintf (dat->fh, "\"" #nam "\":%s", JSON_SPC);             \
      VALUE_2RD (_obj->nam, dxf);                                             \
    }
#  define FIELD_3RD(nam, dxf)                                                 \
    {                                                                         \
      FIRSTPREFIX fprintf (dat->fh, "\"" #nam "\":%s", JSON_SPC);             \
      VALUE_3RD (_obj->nam, dxf);                                             \
    }
#  define SUB_FIELD_BD(o, nam, dxf)                                           \
    {                                                                         \
      FIRSTPREFIX fprintf (dat->fh, JSON_KEY, _path_field (#nam), JSON_SPC);  \
      _VALUE_RD (_obj->o.nam, dxf);                                           \
    }
#endif

#define FIELD_RC(nam, dxf) FIELD (nam, RC, dxf)
#define FIELD_RCx(nam, dxf) FIELD (nam, RC, dxf)
#define FIELD_RS(nam, dxf) FIELD (nam, RS, dxf)
#define FIELD_RSd(nam, dxf) FIELD (nam, RSd, dxf)
#define FIELD_RD(nam, dxf) FIELD_BD (nam, dxf)
#define FIELD_RL(nam, dxf) FIELD (nam, RL, dxf)
#define FIELD_RLd(nam, dxf) FIELD (nam, RLd, dxf)
#define FIELD_RLL(nam, dxf) FIELD (nam, RLL, dxf)
#define FIELD_MC(nam, dxf) FIELD (nam, MC, dxf)
#define FIELD_MS(nam, dxf) FIELD (nam, MS, dxf)
// #define FIELD_TF(nam, len, dxf) FIELD_TEXT (nam, _obj->nam)
// #define FIELD_TFF(nam, len, dxf) FIELD_TEXT (nam, (const char*)_obj->nam)
#define FIELD_TFFx(nam, len, dxf) FIELD_BINARY (nam, len, dxf)
#define FIELD_TV(nam, dxf) FIELD_TEXT (nam, _obj->nam)
#define FIELD_TU(nam, dxf) FIELD_TEXT_TU (nam, (BITCODE_TU)_obj->nam)
#define FIELD_T16(nam, dxf) FIELD_T (nam, dxf)
#define FIELD_TU16(nam, dxf) FIELD_TU (nam, dxf)
#define FIELD_T32(nam, dxf) FIELD_T (nam, dxf)
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (IS_FROM_TU (dat))                                                     \
      {                                                                       \
        FIELD_TU (nam, dxf);                                                  \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_TV (nam, dxf);                                                  \
      }                                                                       \
  }
#define _FIELD_T(nam, str)                                                    \
  {                                                                           \
    if (IS_FROM_TU (dat))                                                     \
      {                                                                       \
        FIELD_TEXT_TU (nam, str);                                             \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_TEXT (nam, str);                                                \
      }                                                                       \
  }
#define VALUE_T(str)                                                          \
  {                                                                           \
    if (IS_FROM_TU (dat))                                                     \
      {                                                                       \
        VALUE_TEXT_TU (str);                                                  \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        VALUE_TEXT (str)                                                      \
      }                                                                       \
  }
#define _FIELD_TV_ALPHA(nam, str)                                             \
  if ((str) && isalpha (*(str)))                                              \
    {                                                                         \
      FIELD_TEXT (nam, str)                                                   \
    }
#define FIELD_BT(nam, dxf) FIELD (nam, BT, dxf);
#define FIELD_4BITS(nam, dxf) FIELD (nam, 4BITS, dxf)
#define FIELD_BE(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_DD(nam, _default, dxf) FIELD_BD (nam, dxf)
#define FIELD_2BD(nam, dxf) FIELD_2RD (nam, dxf)
#define FIELD_2BD_1(nam, dxf) FIELD_2RD (nam, dxf)
#define FIELD_2DD(nam, def, dxf) FIELD_2RD (nam, dxf)
#define FIELD_3DD(nam, def, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3BD(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3BD_1(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3DPOINT(nam, dxf) FIELD_3BD (nam, dxf)

#define SUB_FIELD_T(o, nam, dxf)                                              \
  {                                                                           \
    KEY (nam);                                                                \
    VALUE_T (_obj->o.nam);                                                    \
  }
#define SUB_FIELD_B(o, nam, dxf) SUB_FIELD (o, nam, B, dxf)
#define SUB_FIELD_BB(o, nam, dxf) SUB_FIELD (o, nam, BB, dxf)
#define SUB_FIELD_3B(o, nam, dxf) SUB_FIELD (o, nam, 3B, dxf)
#define SUB_FIELD_BS(o, nam, dxf) SUB_FIELD (o, nam, BS, dxf)
#define SUB_FIELD_BSd(o, nam, dxf) SUB_FIELD (o, nam, BSd, dxf)
#define SUB_FIELD_BL(o, nam, dxf) SUB_FIELD (o, nam, BL, dxf)
#define SUB_FIELD_BLx(o, nam, dxf) SUB_FIELD (o, nam, BLx, dxf)
#define SUB_FIELD_BLd(o, nam, dxf) SUB_FIELD (o, nam, BLd, dxf)
#define SUB_FIELD_RC(o, nam, dxf) SUB_FIELD (o, nam, RC, dxf)
#define SUB_FIELD_RCd(o, nam, dxf) SUB_FIELD (o, nam, RCd, dxf)
#define SUB_FIELD_RS(o, nam, dxf) SUB_FIELD (o, nam, RS, dxf)
#define SUB_FIELD_RL(o, nam, dxf) SUB_FIELD (o, nam, RL, dxf)
#define SUB_FIELD_BLL(o, nam, dxf) SUB_FIELD (o, nam, BLL, dxf)
#define SUB_FIELD_RLL(o, nam, dxf) SUB_FIELD (o, nam, RLL, dxf)

#define SUB_FIELD_RD(o, nam, dxf) SUB_FIELD_BD (o, nam, dxf)
#define SUB_FIELD_3BD_inl(o, nam, dxf)                                        \
  SUB_FIELD_RD (o, x, dxf);                                                   \
  SUB_FIELD_RD (o, y, dxf);                                                   \
  SUB_FIELD_RD (o, z, dxf)
#define SUB_FIELD_2BD(o, nam, dxf)                                            \
  KEY (nam);                                                                  \
  VALUE_2RD (_obj->o.nam, dxf)
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

static void
field_cmc (Bit_Chain *dat, const char *restrict key,
           const Dwg_Color *restrict _obj)
{
  if (dat->version >= R_2004)
    {
      KEYs (_path_field (key));
      HASH;
      if (_obj->index)
        {
          FIELD_BS (index, 62);
        }
      FIRSTPREFIX fprintf (dat->fh, "\"rgb\":%s\"%06x\"", JSON_SPC,
                           (unsigned)_obj->rgb);
      if (_obj->flag)
        {
          FIELD_BS (flag, 0);
        }
      if (_obj->flag > 0 && _obj->flag < 8)
        {
          if (_obj->flag & 1)
            _FIELD_TV_ALPHA (name, _obj->name)
          if (_obj->flag & 2)
            _FIELD_TV_ALPHA (book_name, _obj->book_name)
        }
      ENDHASH;
    }
  else
    {
      FIRSTPREFIX fprintf (dat->fh, JSON_KEY FORMAT_RSd, _path_field (key),
                           JSON_SPC, _obj->index);
    }
}

#define FIELD_CMC(color, dxf) field_cmc (dat, #color, &_obj->color)
#define SUB_FIELD_CMC(o, color, dxf) field_cmc (dat, #color, &_obj->o.color)

#define FIELD_TIMEBLL(nam, dxf)                                               \
  FIRSTPREFIX fprintf (                                                       \
      dat->fh, "\"" #nam "\":%s[%s" FORMAT_BL ",%s" FORMAT_BL "%s]",          \
      JSON_SPC, JSON_SPC, _obj->nam.days, JSON_SPC, _obj->nam.ms, JSON_SPC)
#define FIELD_TIMERLL(nam, dxf) FIELD_TIMEBLL (nam, dxf)

// FIELD_VECTOR_N(nam, type, size):
// reads data of the type indicated by 'type' 'size' times and stores
// it all in the vector called 'nam'.
#define FIELD_VECTOR_N(nam, type, size, dxf)                                  \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  if (_obj->nam)                                                              \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, FORMAT_##type, _obj->nam[vcount]);    \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    FIRSTPREFIX                                                               \
  ENDARRAY;
#define SUB_FIELD_VECTOR_N(o, nam, type, size, dxf)                           \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  if (_obj->o.nam)                                                            \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIRSTPREFIX fprintf (dat->fh, FORMAT_##type, _obj->o.nam[vcount]);  \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    FIRSTPREFIX                                                               \
  ENDARRAY;
#define FIELD_VECTOR_T(nam, type, size, dxf)                                  \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  if (_obj->nam)                                                              \
    {                                                                         \
      BITCODE_BL _size = (BITCODE_BL)_obj->size;                              \
      if (IS_FROM_TU (dat))                                                   \
        {                                                                     \
          for (vcount = 0; vcount < _size; vcount++)                          \
            {                                                                 \
              FIRSTPREFIX VALUE_TEXT_TU (_obj->nam[vcount]);                  \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          for (vcount = 0; vcount < _size; vcount++)                          \
            {                                                                 \
              FIRSTPREFIX VALUE_TEXT (_obj->nam[vcount])                      \
            }                                                                 \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    FIRSTPREFIX                                                               \
  ENDARRAY;

#define SUB_FIELD_VECTOR_TYPESIZE(o, nam, size, typesize, dxf)                \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  if (_obj->o.size && _obj->o.nam)                                            \
    {                                                                         \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          switch (typesize)                                                   \
            {                                                                 \
            case 0:                                                           \
              break;                                                          \
            case 1:                                                           \
              FIRSTPREFIX fprintf (dat->fh, FORMAT_RC,                        \
                                   (BITCODE_RC)_obj->o.nam[vcount]);          \
              break;                                                          \
            case 2:                                                           \
              FIRSTPREFIX fprintf (dat->fh, FORMAT_RS,                        \
                                   (BITCODE_RS)_obj->o.nam[vcount]);          \
              break;                                                          \
            case 4:                                                           \
              FIRSTPREFIX fprintf (dat->fh, FORMAT_RL,                        \
                                   (BITCODE_RL)_obj->o.nam[vcount]);          \
              break;                                                          \
            case 8:                                                           \
              FIRSTPREFIX fprintf (dat->fh, FORMAT_RLL,                       \
                                   (BITCODE_RLL)_obj->o.nam[vcount]);         \
              break;                                                          \
            default:                                                          \
              LOG_ERROR ("Unknown SUB_FIELD_VECTOR_TYPESIZE " #nam            \
                         " typesize %d",                                      \
                         typesize);                                           \
              break;                                                          \
            }                                                                 \
        }                                                                     \
    }                                                                         \
  else                                                                        \
    FIRSTPREFIX                                                               \
  ENDARRAY;

#define FIELD_VECTOR(nam, type, size, dxf)                                    \
  FIELD_VECTOR_N (nam, type, _obj->size, dxf)
#define FIELD_VECTOR_INL(nam, type, size, dxf)                                \
  FIELD_VECTOR_N (nam, type, size, dxf)
#define SUB_FIELD_VECTOR(o, nam, type, size, dxf)                             \
  SUB_FIELD_VECTOR_N (o, nam, type, _obj->o.size, dxf)

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  if (_obj->nam)                                                              \
    {                                                                         \
      KEY (nam);                                                              \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIRSTPREFIX VALUE_2RD (FIELD_VALUE (nam[vcount]), dxf);             \
        }                                                                     \
      ENDARRAY;                                                               \
    }

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  if (_obj->nam)                                                              \
    {                                                                         \
      KEY (nam);                                                              \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIRSTPREFIX VALUE_2RD (_obj->nam[vcount], dxf);                     \
        }                                                                     \
      ENDARRAY;                                                               \
    }

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  if (_obj->nam)                                                              \
    {                                                                         \
      KEY (nam);                                                              \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)             \
        {                                                                     \
          FIRSTPREFIX VALUE_3BD (FIELD_VALUE (nam[vcount]), dxf);             \
        }                                                                     \
      ENDARRAY;                                                               \
    }

#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  if (_obj->nam)                                                              \
    {                                                                         \
      KEY (nam);                                                              \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                   \
        {                                                                     \
          FIELD_HANDLE_N (nam[vcount], vcount, code, dxf);                    \
        }                                                                     \
      ENDARRAY;                                                               \
    }

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

#define SUB_HANDLE_VECTOR(o, nam, size, code, dxf)                            \
  KEY (nam);                                                                  \
  if (_obj->o.nam)                                                            \
    {                                                                         \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < (BITCODE_BL)_obj->o.size; vcount++)           \
        {                                                                     \
          SUB_FIELD_HANDLE_N (o, nam[vcount], code, dxf);                     \
        }                                                                     \
      ENDARRAY;                                                               \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      ARRAY;                                                                  \
      ENDARRAY;                                                               \
    }

#define REACTORS(code)                                                        \
  if (dat->version >= R_13b1 && obj->tio.object->num_reactors                 \
      && obj->tio.object->reactors)                                           \
    {                                                                         \
      KEY (reactors);                                                         \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < obj->tio.object->num_reactors; vcount++)      \
        {                                                                     \
          FIRSTPREFIX VALUE_HANDLE (obj->tio.object->reactors[vcount],        \
                                    reactors, code, 330);                     \
        }                                                                     \
      ENDARRAY;                                                               \
    }
#define ENT_REACTORS(code)                                                    \
  if (dat->version >= R_13b1 && _ent->num_reactors && _ent->reactors)         \
    {                                                                         \
      KEY (reactors);                                                         \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < _ent->num_reactors; vcount++)                 \
        {                                                                     \
          FIRSTPREFIX VALUE_HANDLE (_ent->reactors[vcount], reactors, code,   \
                                    330);                                     \
        }                                                                     \
      ENDARRAY;                                                               \
    }

// violates duplicate keys
#define SUBCLASS(name)                                                        \
  SINCE (R_13b1)                                                              \
  {                                                                           \
    FIRSTPREFIX fprintf (dat->fh, "\"_subclass\":%s\"" #name "\"", JSON_SPC); \
  }

// FIXME: for KEY not the complete nam path, only the field.
// e.g. verts[rcount1].lines[rcount2].segparms
#define _REPEAT_N(times, nam, type, idx)                                      \
  if (_obj->nam)                                                              \
    {                                                                         \
      KEY (nam);                                                              \
      ARRAY;                                                                  \
      for (rcount##idx = 0; rcount##idx < (BITCODE_BL)times; rcount##idx++)   \
        {
#define REPEAT_N(times, nam, type) _REPEAT_N (times, nam, type, 1)
#define REPEAT_CN(times, nam, type) REPEAT_N (times, nam, type)
#define _REPEAT_C(times, nam, type, idx)                                      \
  _REPEAT_N (_obj->times, nam, type, idx)
#define _REPEAT(times, nam, type, idx) _REPEAT_N (_obj->times, nam, type, idx)
#define REPEAT(times, nam, type) _REPEAT (times, nam, type, 1)
#define REPEAT2(times, nam, type) _REPEAT (times, nam, type, 2)
#define REPEAT3(times, nam, type) _REPEAT (times, nam, type, 3)
#define REPEAT4(times, nam, type) _REPEAT (times, nam, type, 4)
#define REPEAT_C(times, nam, type) _REPEAT_C (times, nam, type, 1)
#define REPEAT2_C(times, nam, type) _REPEAT_C (times, nam, type, 2)
#define REPEAT3_C(times, nam, type) _REPEAT_C (times, nam, type, 3)
#define REPEAT4_C(times, nam, type) _REPEAT_C (times, nam, type, 4)
#define _REPEAT_CN(times, nam, type, idx) _REPEAT_N (times, nam, type, idx)
#define _REPEAT_CNF(times, nam, type, idx) _REPEAT_N (times, nam, type, idx)
#define _REPEAT_NF(times, nam, type, idx) _REPEAT_N (times, nam, type, idx)
#define REPEAT_F(times, size, nam, type)                                      \
  if (_obj->times > (BITCODE_BL)size)                                         \
    {                                                                         \
      LOG_WARN ("Invalid %s " FORMAT_BL " > %u", #nam,                        \
                (BITCODE_BL)_obj->times, (unsigned)size);                     \
      _obj->times = (BITCODE_BL)size;                                         \
    }                                                                         \
  _REPEAT_N (_obj->times, nam, type, 1)
#undef REPEAT_BLOCK
#define REPEAT_BLOCK FIRSTPREFIX HASH;
#undef END_REPEAT_BLOCK
#define END_REPEAT_BLOCK ENDHASH;
#undef END_REPEAT
#define END_REPEAT(nam)                                                       \
  }                                                                           \
  ENDARRAY;                                                                   \
  }
#undef END_REPEAT_F
#define END_REPEAT_F(nam) END_REPEAT (nam)

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  FIELD (num_inserts, type, dxf)

#define FIELD_XDATA(nam, size) error |= json_xdata (dat, _obj)

#define XDICOBJHANDLE(code)                                                   \
  if ((dat->version < R_2004 || obj->tio.object->is_xdic_missing == 0)        \
      && (obj->tio.object->xdicobjhandle != NULL)                             \
      && (obj->tio.object->xdicobjhandle->handleref.value != 0))              \
    {                                                                         \
      KEY (xdicobjhandle);                                                    \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code, -3); \
    }
#define ENT_XDICOBJHANDLE(code)                                               \
  if ((dat->version < R_2004 || _ent->is_xdic_missing == 0)                   \
      && (_ent->xdicobjhandle != NULL)                                        \
      && (_ent->xdicobjhandle->handleref.value != 0))                         \
    {                                                                         \
      KEY (xdicobjhandle);                                                    \
      VALUE_HANDLE (_ent->xdicobjhandle, xdicobjhandle, code, -3);            \
    }

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM
#define START_OBJECT_HANDLE_STREAM
#define CONTROL_HANDLE_STREAM

#define DWG_ENTITY(token)                                                     \
  static int dwg_json_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj);           \
  GCC30_DIAG_IGNORE (-Wshadow)                                                \
  static int dwg_json_##token (Bit_Chain *restrict dat,                       \
                               Dwg_Object *restrict obj)                      \
  {                                                                           \
    int error = 0;                                                            \
    Bit_Chain *str_dat = dat;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    const char *name = #token;                                                \
    LOG_INFO ("Entity " #token ":\n")                                         \
    _ent = obj->tio.entity;                                                   \
    if (!_ent || !_ent->tio.token)                                            \
      return DWG_ERR_INTERNALERROR;                                           \
    _obj = ent = _ent->tio.token;                                             \
    if (*name == '_')                                                         \
      FIELD_TEXT (entity, &name[1])                                           \
    else                                                                      \
      FIELD_TEXT (entity, name)                                               \
    if (obj->dxfname && strNE (obj->dxfname, #token))                         \
      FIELD_TEXT (dxfname, obj->dxfname);                                     \
    _FIELD (index, RL, 0);                                                    \
    _FIELD (type, RSd, 0);                                                    \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      KEY (handle);                                                           \
      VALUE_H (obj->handle, 5);                                               \
    }                                                                         \
    _FIELD (size, RL, 0);                                                     \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      _FIELD (bitsize, BL, 0);                                                \
      if (_ent->preview_exists)                                               \
        ENT_FIELD (preview_exists, B, 0);                                     \
    }                                                                         \
    error |= json_common_entity_data (dat, obj);                              \
    return error | dwg_json_##token##_private (dat, hdl_dat, str_dat, obj);   \
  }                                                                           \
  static int dwg_json_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj)            \
  {                                                                           \
    int error = 0;                                                            \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_Entity *_ent = obj->tio.entity;                                \
    Dwg_Entity_##token *_obj = _ent->tio.token;

#define DWG_ENTITY_END                                                        \
  if (obj->num_unknown_rest)                                                  \
    UNKNOWN_BITS_REST                                                         \
  return error;                                                               \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_json_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj);           \
  GCC30_DIAG_IGNORE (-Wshadow)                                                \
  static int dwg_json_##token (Bit_Chain *restrict dat,                       \
                               Dwg_Object *restrict obj)                      \
  {                                                                           \
    int error = 0;                                                            \
    Bit_Chain *str_dat = dat;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    const char *name = #token;                                                \
    Dwg_Object_##token *_obj;                                                 \
    LOG_INFO ("Object " #token ":\n")                                         \
    if (!obj->tio.object || !obj->tio.object->tio.token)                      \
      return DWG_ERR_INTERNALERROR;                                           \
    _obj = obj->tio.object->tio.token;                                        \
    if (*name == '_')                                                         \
      FIELD_TEXT (object, &name[1])                                           \
    else                                                                      \
      FIELD_TEXT (object, name)                                               \
    if (obj->dxfname && strNE (obj->dxfname, #token))                         \
      FIELD_TEXT (dxfname, obj->dxfname);                                     \
    _FIELD (index, RL, 0);                                                    \
    _FIELD (type, RL, 0);                                                     \
    KEY (handle);                                                             \
    VALUE_H (obj->handle, 5);                                                 \
    _FIELD (size, RL, 0);                                                     \
    SINCE (R_13b1)                                                            \
    {                                                                         \
      _FIELD (bitsize, BL, 0);                                                \
    }                                                                         \
    error |= json_eed (dat, obj->tio.object);                                 \
    error |= json_common_object_handle_data (dat, obj);                       \
    return error | dwg_json_##token##_private (dat, hdl_dat, str_dat, obj);   \
  }                                                                           \
  static int dwg_json_##token##_private (Bit_Chain *dat, Bit_Chain *hdl_dat,  \
                                         Bit_Chain *str_dat,                  \
                                         Dwg_Object *restrict obj)            \
  {                                                                           \
    int error = 0;                                                            \
    BITCODE_BL vcount, rcount3, rcount4;                                      \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Object_##token *_obj = obj->tio.object->tio.token;

#define DWG_OBJECT_END                                                        \
  if (obj->num_unknown_rest)                                                  \
    UNKNOWN_BITS_REST                                                         \
  return error;                                                               \
  }

#undef JSON_3DSOLID
#define JSON_3DSOLID json_3dsolid (dat, obj, (Dwg_Entity_3DSOLID *)_obj);

static void
_prefix (Bit_Chain *dat)
{
  if (!IS_MINJS)
    {
      for (int _i = 0; _i < dat->bit; _i++)
        {
          fprintf (dat->fh, "  ");
        }
    }
}

static char *
_path_field (const char *path)
{
  const char *s = strrchr (path, ']');
  if (s && s[1] == '.')
    {
      return (char *)&s[2];
    }
  return (char *)path;
}

static int
json_eed (Bit_Chain *restrict dat, const Dwg_Object_Object *restrict obj)
{
  int error = 0;
  if (!obj->num_eed)
    return 0;
  KEY (eed);
  ARRAY;
  for (BITCODE_BL i = 0; i < obj->num_eed; i++)
    {
      const Dwg_Eed *_obj = &obj->eed[i];
      FIRSTPREFIX HASH;
      if (_obj->size)
        {
          FIELD (size, RS, 0);
          KEY (handle);
          fprintf (dat->fh, FORMAT_H, ARGS_H (_obj->handle));
        }
      if (_obj->data)
        {
          const Dwg_Eed_Data *data = _obj->data;
          KEY (code);
          VALUE_RC (data->code, 0);
          KEY (value);
          switch (data->code)
            {
            case 0:
              if (!data->u.eed_0.is_tu)
                VALUE_TEXT (data->u.eed_0.string)
              else
                {
                  VALUE_TEXT_TU (data->u.eed_0_r2007.string);
                }
              break;
            case 1:
              VALUE_RS (data->u.eed_1.appid_index, 0);
              break;
            case 2:
              VALUE_RC (data->u.eed_2.close, 0);
              break;
            case 3:
              VALUE_RLL (data->u.eed_3.layer, 0);
              break;
            case 4:
              VALUE_BINARY (data->u.eed_4.data, data->u.eed_4.length, 0);
              break;
            case 5:
              fprintf (dat->fh, FORMAT_H "", 5, 8, data->u.eed_5.entity);
              break;
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
              VALUE_3RD (data->u.eed_10.point, 0);
              break;
            case 40:
            case 41:
            case 42:
              VALUE_RD (data->u.eed_40.real, 0);
              break;
            case 70:
              VALUE_RS (data->u.eed_70.rs, 0);
              break;
            case 71:
              VALUE_RL (data->u.eed_71.rl, 0);
              break;
            default:
              VALUE_RC (0, 0);
            }
        }
      ENDHASH
    }
  ENDARRAY;
  return error;
}

static int
json_xdata (Bit_Chain *restrict dat, const Dwg_Object_XRECORD *restrict obj)
{
  int error = 0;
  Dwg_Resbuf *rbuf = obj->xdata;
  KEY (xdata);
  ARRAY;
  for (BITCODE_BL i = 0; i < obj->num_xdata; i++)
    {
      enum RESBUF_VALUE_TYPE type;
      FIRSTPREFIX ARRAY;
      FIRSTPREFIX VALUE_RS (rbuf->type, 0);
      FIRSTPREFIX
      type = dwg_resbuf_value_type (rbuf->type);
      switch (type)
        {
        case DWG_VT_STRING:
          if (!rbuf->value.str.is_tu)
            {
              VALUE_TEXT (rbuf->value.str.u.data);
              LOG_TRACE ("xdata[%u]: \"%s\" [TV %d]\n", i,
                         rbuf->value.str.u.data, rbuf->type);
            }
          else
            {
              VALUE_TEXT_TU (rbuf->value.str.u.data);
              LOG_TRACE_TU ("xdata", rbuf->value.str.u.data, rbuf->type);
            }
          break;
        case DWG_VT_BINARY:
          VALUE_BINARY (rbuf->value.str.u.data, rbuf->value.str.size, 0);
          LOG_TRACE ("xdata[%u]: \"%s\" [TF %d]\n", i, rbuf->value.str.u.data,
                     rbuf->type);
          break;
        case DWG_VT_REAL:
          VALUE_RD (rbuf->value.dbl, 0);
          LOG_TRACE ("xdata[%u]: %f [RD %d]\n", i, rbuf->value.dbl,
                     rbuf->type);
          break;
        case DWG_VT_BOOL:
        case DWG_VT_INT8:
          VALUE_RC (rbuf->value.i8, 0);
          LOG_TRACE ("xdata[%u]: %d [RC %d]\n", i, (int)rbuf->value.i8,
                     rbuf->type);
          break;
        case DWG_VT_INT16:
          VALUE_RS (rbuf->value.i16, 0);
          LOG_TRACE ("xdata[%u]: %d [RS %d]\n", i, (int)rbuf->value.i16,
                     rbuf->type);
          break;
        case DWG_VT_INT32:
          VALUE_RL (rbuf->value.i32, 0);
          LOG_TRACE ("xdata[%u]: %d [RL %d]\n", i, (int)rbuf->value.i32,
                     rbuf->type);
          break;
        case DWG_VT_INT64:
          VALUE_RLL (rbuf->value.i64, 0);
          LOG_TRACE ("xdata[%u]: %ld [RLL %d]\n", i, (long)rbuf->value.i64,
                     rbuf->type);
          break;
        case DWG_VT_POINT3D:
          fprintf (dat->fh, "[" FORMAT_RD "," FORMAT_RD "," FORMAT_RD "]",
                   rbuf->value.pt[0], rbuf->value.pt[1], rbuf->value.pt[2]);
          LOG_TRACE ("xdata[%u]: (%f,%f,%f) [3RD %d]\n", i, rbuf->value.pt[0],
                     rbuf->value.pt[1], rbuf->value.pt[2], rbuf->type);
          break;
        case DWG_VT_HANDLE:
        case DWG_VT_OBJECTID:
          VALUE_RLL (rbuf->value.absref, 0);
          LOG_TRACE ("xdata[%u]: " FORMAT_RLLx " [H %d]\n", i,
                     rbuf->value.absref, rbuf->type);
          break;
        case DWG_VT_INVALID:
        default:
          break;
        }
      rbuf = rbuf->nextrb;
      ENDARRAY
    }
  ENDARRAY;
  return error;
}

static int
json_common_entity_data (Bit_Chain *restrict dat,
                         const Dwg_Object *restrict obj)
{
  Dwg_Object_Entity *_ent, *_obj;
  // Dwg_Data *dwg = obj->parent;
  int error = 0;
  BITCODE_BL vcount = 0;
  _ent = obj->tio.entity;
  _obj = _ent;

  error |= json_eed (dat, (Dwg_Object_Object *)_ent);

  // clang-format off
  #include "common_entity_handle_data.spec"
  #include "common_entity_data.spec"
  // clang-format on

  return error;
}

static int
json_common_object_handle_data (Bit_Chain *restrict dat,
                                const Dwg_Object *restrict obj)
{
  Dwg_Object_Object *_obj;
  Dwg_Data *dwg = obj->parent;
  int error = 0;
  BITCODE_BL vcount = 0;
  _obj = obj->tio.object;

  // clang-format off
  #include "common_object_handle_data.spec"
  // clang-format on
  return error;
}

#include "dwg.spec"

static int
ishex (int c)
{
  return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
          || (c >= 'A' && c <= 'F'));
}
// Usage: hex(c >> 4), hex(c & 0xf)
static int
hex (unsigned char c)
{
  c &= 0xf; // 0-15
  return c >= 10 ? 'a' + c - 10 : '0' + c;
}

#ifndef HAVE_NATIVE_WCHAR2

static void
print_wcquote (Bit_Chain *restrict dat, dwg_wchar_t *restrict wstr)
{
  BITCODE_TU ws = (BITCODE_TU)wstr;
  uint16_t c;
  if (!ws)
    {
      fprintf (dat->fh, "\"\"");
      return;
    }
  fprintf (dat->fh, "\"");
  while (1)
    {
#  ifdef HAVE_ALIGNED_ACCESS_REQUIRED
      // for strict alignment CPU's like sparc only. also for UBSAN.
      if ((uintptr_t)wstr % SIZEOF_SIZE_T)
        {
          unsigned char *b = (unsigned char *)ws;
          c = TU_to_int (b);
          ws++;
        }
      else
#  endif
        c = *ws++;
      if (!c)
        break;
      if (c == L'"')
        {
          fprintf (dat->fh, "\\\"");
        }
      else if (c == L'\\' && ws[0] == L'U' && ws[1] == L'+' && ishex (ws[2])
               && ishex (ws[3]) && ishex (ws[4]) && ishex (ws[5]))
        {
          fprintf (dat->fh, "\\u");
          ws += 2;
        }
      else if (c == L'\\')
        {
          fprintf (dat->fh, "\\\\");
        }
      else if (c == L'\n')
        {
          fprintf (dat->fh, "\\n");
        }
      else if (c == L'\r')
        {
          fprintf (dat->fh, "\\r");
        }
      // convert to utf-8
      else if (c < 0x1f || c > 0xff)
        {
          if (c < 0x80)
            {
              fprintf (dat->fh, "\\u%04x", c);
            }
          else if (c < 0x800)
            {
              fprintf (dat->fh, "%c%c", (c >> 6) | 0xC0, (c & 0x3F) | 0x80);
            }
          else /* if (c < 0x10000) */
            {
              fprintf (dat->fh, "%c%c%c", (c >> 12) | 0xE0,
                       ((c >> 6) & 0x3F) | 0x80, (c & 0x3F) | 0x80);
            }
#  if 0
          // FIXME: handle surrogate pairs properly
          if (c >= 0xd800 && c < 0xdc00)
            {
              fprintf (dat->fh, "\\u%04x", c - 0x1000);
            }
          else if (c >= 0xdc00 && c < 0xe000)
            ;
          else
            fprintf (dat->fh, "\\u%04x", c);
#  endif
        }
      else
        fprintf (dat->fh, "%c", (char)(c & 0xff));
    }
  fprintf (dat->fh, "\"");
}

#else

static wchar_t *
wcquote (wchar_t *restrict dest, const wchar_t *restrict src)
{
  wchar_t c;
  wchar_t *d = dest;
  wchar_t *s = (wchar_t *)src;
  while ((c = *s++))
    {
      if (c == L'"')
        {
          *dest++ = L'\\';
          *dest++ = c;
        }
      else if (c == L'\\' && s[0] == L'U' && s[1] == L'+' && ishex (s[2])
               && ishex (s[3]) && ishex (s[4]) && ishex (s[5]))
        {
          *dest++ = '\\';
          *dest++ = 'u';
          s += 2;
        }
      else if (c == L'\\')
        {
          *dest++ = L'\\';
          *dest++ = c;
        }
      else if (c == L'\n')
        {
          *dest++ = L'\\';
          *dest++ = L'n';
        }
      else if (c == L'\r')
        {
          *dest++ = L'\\';
          *dest++ = L'r';
        }
      else if (c < 0x1f)
        {
          *dest++ = L'\\';
          *dest++ = L'u';
          *dest++ = L'0';
          *dest++ = L'0';
          *dest++ = hex (c >> 4);
          *dest++ = hex (c & 0xf);
        }
      else
        *dest++ = c;
    }
  *dest = 0; // add final delim, skipped above
  return d;
}

#endif /* HAVE_NATIVE_WCHAR2 */

/* Don't write past the strlen.
   TODO: convert to utf8.
*/
void
json_write_TFv (Bit_Chain *restrict dat, const BITCODE_TF restrict src,
                const size_t len)
{
  const size_t slen = src ? strlen ((char *)src) : 0;
  fputc ('"', dat->fh);
  if (!slen)
    {
      fputc ('"', dat->fh);
      return;
    }
  for (size_t i = 0; i < MIN (slen, len); i++)
    {
      const unsigned char c = src[i];
      if (c == '\r' || c == '\n' || c == '"' || c == '\\')
        {
          fputc ('\\', dat->fh);
          if (c == '\r')
            fputc ('r', dat->fh);
          else if (c == '\n')
            fputc ('n', dat->fh);
          else
            fputc (c, dat->fh);
        }
      else if (c == '\\' && i + 6 < len && src[i] == 'U' && src[i + 1] == '+'
               && ishex (src[i + 2]) && ishex (src[i + 3])
               && ishex (src[i + 4]) && ishex (src[i + 5]))
        {
          fputc ('\\', dat->fh);
          fputc ('u', dat->fh);
          i += 3;
        }
      else if (c < 0x1f)
        fprintf (dat->fh, "\\u00%02x", c);
      else
        fputc (c, dat->fh);
    }
  fputc ('"', dat->fh);
}

/* Write even past the \0, to keep existing slack.
   TODO: convert to utf8.
*/
void
json_write_TF (Bit_Chain *restrict dat, const BITCODE_TF restrict src,
               const size_t len)
{
  const size_t slen = src ? strlen ((char *)src) : 0;
  bool has_slack = false;
  fputc ('"', dat->fh);
  if (!slen)
    {
      fputc ('"', dat->fh);
      return;
    }
  for (size_t i = 0; i < len; i++)
    {
      const unsigned char c = src[i];
      if (c == '\r' || c == '\n' || c == '"' || c == '\\')
        {
          fputc ('\\', dat->fh);
          if (c == '\r')
            fputc ('r', dat->fh);
          else if (c == '\n')
            fputc ('n', dat->fh);
          else
            fputc (c, dat->fh);
        }
      else if (c == '\\' && i + 6 < len && src[i] == 'U' && src[i + 1] == '+'
               && ishex (src[i + 2]) && ishex (src[i + 3])
               && ishex (src[i + 4]) && ishex (src[i + 5]))
        {
          fputc ('\\', dat->fh);
          fputc ('u', dat->fh);
          i += 3;
        }
      else if (c == '\0')
        {
          // see if there's slack after the NUL terminator
          if (!has_slack && i == slen)
            {
              for (size_t j = i; j < len; j++)
                if (src[j])
                  {
                    has_slack = true;
                    break;
                  }
            }
          if (has_slack)
            fprintf (dat->fh, "\\u0000");
          else
            {
              fputc ('"', dat->fh);
              return;
            }
        }
      else if (c < 0x1f)
        fprintf (dat->fh, "\\u00%02x", c);
      else
        fputc (c, dat->fh);
    }
  fputc ('"', dat->fh);
}

// also converts from codepage to utf8
char *
json_cquote (char *restrict dest, const char *restrict src, const size_t len,
             const BITCODE_RS codepage)
{
  unsigned char c;
  unsigned char *s = (unsigned char *)src;
  const char *endp = dest + len;
  char *d = dest;
  char *tmp = NULL;

  if (!src)
    return (char *)"";
  if (strlen (src) && codepage > CP_US_ASCII && codepage <= CP_ANSI_1258)
    {
      // may malloc
      tmp = bit_TV_to_utf8 ((char *restrict)src, codepage);
      if (tmp)
        s = (unsigned char *)tmp;
      // else conversion failed. ignore
    }
  while ((c = *s++))
    {
      if (dest >= endp)
        {
          *dest = 0;
          if (tmp && tmp != src)
            free (tmp);
          return d;
        }
      if (c == '"' && dest + 1 < endp)
        {
          *dest++ = '\\';
          *dest++ = c;
        }
      else if (c == '\\' && dest + 2 < endp)
        {
          if (dest + 5 < endp && s[0] == 'U' && s[1] == '+' && ishex (s[2])
              && ishex (s[3]) && ishex (s[4]) && ishex (s[5]))
            {
              *dest++ = '\\';
              *dest++ = 'u';
              s += 2;
            }
          else
            {
              *dest++ = '\\';
              *dest++ = c;
            }
        }
      else if (c == '\n' && dest + 1 < endp)
        {
          *dest++ = '\\';
          *dest++ = 'n';
        }
      else if (c == '\r' && dest + 1 < endp)
        {
          *dest++ = '\\';
          *dest++ = 'r';
        }
      else if (c < 0x1f && dest + 5 < endp)
        {
          *dest++ = '\\';
          *dest++ = 'u';
          *dest++ = '0';
          *dest++ = '0';
          *dest++ = hex (c >> 4);
          *dest++ = hex (c & 0xf);
        }
      else
        *dest++ = c;
    }
  *dest = 0; // add final delim, skipped above
  if (tmp && tmp != src)
    free (tmp);
  return d;
}

static int
json_3dsolid (Bit_Chain *restrict dat, const Dwg_Object *restrict obj,
              Dwg_Entity_3DSOLID *restrict _obj)
{
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL vcount;
  BITCODE_BL i;
  int error = 0;

  FIELD_B (acis_empty, 0);
  if (!FIELD_VALUE (acis_empty))
    {
      char *s, *p;
      FIELD_B (unknown, 0);
      FIELD_BS (version, 70);
      KEY (acis_data);
      ARRAY;
      s = p = (char *)_obj->acis_data;
      if (!p)
        {
          FIRSTPREFIX fprintf (dat->fh, "\"\"");
        }
      else if (_obj->version < 2)
        { // split lines by \n
          for (; *p; p++)
            {
              char buf[256]; // acis lines are not much longer
              // and skip the final ^M
              if ((*p == '\r' || *p == '\n') && p - s < 256)
                {
                  FIRSTPREFIX fprintf (dat->fh, "\"%.*s\"", (int)(p - s), s);
                  // json_cquote (buf, s, p - s, dat->codepage));
                  if (*p == '\r' && *(p + 1) == '\n')
                    p++;
                  s = p + 1;
                }
            }
          // the remainder
          if (s != p && *s)
            {
              FIRSTPREFIX VALUE_TEXT (s);
            }
        }
      else // version 2, SAB. split into two lines for easier identification
        {
          FIRSTPREFIX fprintf (dat->fh, "\"%.*s\"", 15, _obj->acis_data);
          FIRSTPREFIX VALUE_BINARY (&_obj->acis_data[15], _obj->sab_size - 15,
                                    1);
        }
      ENDARRAY;
      if (_obj->encr_sat_data) // dxfin/out may create this for SAB
        {
          KEY (encr_sat_data);
          ARRAY;
          for (i = 0; i < FIELD_VALUE (num_blocks); i++)
            {
              FIRSTPREFIX VALUE_BINARY (FIELD_VALUE (encr_sat_data[i]),
                                        FIELD_VALUE (block_size[i]), 1);
            }
          ENDARRAY;
        }

      COMMON_ENTITY_HANDLE_DATA;
      if (FIELD_VALUE (version) > 1)
        {
          SINCE (R_2007a)
          {
            FIELD_HANDLE (history_id, 0, 350);
          }
        }
    }
  return error;
}

/* returns 0 on success
 */
static int
dwg_json_variable_type (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                        Dwg_Object *restrict obj)
{
  int i;
  Dwg_Class *klass;
  int is_entity;

  i = obj->type - 500;
  if (i < 0 || i >= (int)dwg->num_classes)
    return DWG_ERR_INVALIDTYPE;
  if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT
      || obj->fixedtype == DWG_TYPE_UNKNOWN_OBJ)
    return DWG_ERR_UNHANDLEDCLASS;

  klass = &dwg->dwg_class[i];
  if (!klass || !klass->dxfname)
    return DWG_ERR_INTERNALERROR;
  is_entity = dwg_class_is_entity (klass);

  // clang-format off
  #include "classes.inc"
  // clang-format on

  return DWG_ERR_UNHANDLEDCLASS;
}

static int
dwg_json_object (Bit_Chain *restrict dat, Dwg_Object *restrict obj)
{
  int error = 0;
  unsigned int type;

  if (!obj || !obj->parent || !obj->tio.entity)
    return DWG_ERR_INTERNALERROR;
  type = (unsigned int)obj->fixedtype;
  switch (obj->fixedtype)
    {
    case DWG_TYPE_TEXT:
      return dwg_json_TEXT (dat, obj);
    case DWG_TYPE_ATTRIB:
      return dwg_json_ATTRIB (dat, obj);
    case DWG_TYPE_ATTDEF:
      return dwg_json_ATTDEF (dat, obj);
    case DWG_TYPE_BLOCK:
      return dwg_json_BLOCK (dat, obj);
    case DWG_TYPE_ENDBLK:
      return dwg_json_ENDBLK (dat, obj);
    case DWG_TYPE_SEQEND:
      return dwg_json_SEQEND (dat, obj);
    case DWG_TYPE_INSERT:
      return dwg_json_INSERT (dat, obj);
    case DWG_TYPE_MINSERT:
      return dwg_json_MINSERT (dat, obj);
    case DWG_TYPE_VERTEX_2D:
      return dwg_json_VERTEX_2D (dat, obj);
    case DWG_TYPE_VERTEX_3D:
      return dwg_json_VERTEX_3D (dat, obj);
    case DWG_TYPE_VERTEX_MESH:
      return dwg_json_VERTEX_MESH (dat, obj);
    case DWG_TYPE_VERTEX_PFACE:
      return dwg_json_VERTEX_PFACE (dat, obj);
    case DWG_TYPE_VERTEX_PFACE_FACE:
      return dwg_json_VERTEX_PFACE_FACE (dat, obj);
    case DWG_TYPE_POLYLINE_2D:
      return dwg_json_POLYLINE_2D (dat, obj);
    case DWG_TYPE_POLYLINE_3D:
      return dwg_json_POLYLINE_3D (dat, obj);
    case DWG_TYPE_ARC:
      return dwg_json_ARC (dat, obj);
    case DWG_TYPE_CIRCLE:
      return dwg_json_CIRCLE (dat, obj);
    case DWG_TYPE_LINE:
      return dwg_json_LINE (dat, obj);
    case DWG_TYPE_DIMENSION_ORDINATE:
      return dwg_json_DIMENSION_ORDINATE (dat, obj);
    case DWG_TYPE_DIMENSION_LINEAR:
      return dwg_json_DIMENSION_LINEAR (dat, obj);
    case DWG_TYPE_DIMENSION_ALIGNED:
      return dwg_json_DIMENSION_ALIGNED (dat, obj);
    case DWG_TYPE_DIMENSION_ANG3PT:
      return dwg_json_DIMENSION_ANG3PT (dat, obj);
    case DWG_TYPE_DIMENSION_ANG2LN:
      return dwg_json_DIMENSION_ANG2LN (dat, obj);
    case DWG_TYPE_DIMENSION_RADIUS:
      return dwg_json_DIMENSION_RADIUS (dat, obj);
    case DWG_TYPE_DIMENSION_DIAMETER:
      return dwg_json_DIMENSION_DIAMETER (dat, obj);
    case DWG_TYPE_POINT:
      return dwg_json_POINT (dat, obj);
    case DWG_TYPE__3DFACE:
      return dwg_json__3DFACE (dat, obj);
    case DWG_TYPE_POLYLINE_PFACE:
      return dwg_json_POLYLINE_PFACE (dat, obj);
    case DWG_TYPE_POLYLINE_MESH:
      return dwg_json_POLYLINE_MESH (dat, obj);
    case DWG_TYPE_SOLID:
      return dwg_json_SOLID (dat, obj);
    case DWG_TYPE_TRACE:
      return dwg_json_TRACE (dat, obj);
    case DWG_TYPE_SHAPE:
      return dwg_json_SHAPE (dat, obj);
    case DWG_TYPE_VIEWPORT:
      return dwg_json_VIEWPORT (dat, obj);
    case DWG_TYPE_ELLIPSE:
      return dwg_json_ELLIPSE (dat, obj);
    case DWG_TYPE_SPLINE:
      return dwg_json_SPLINE (dat, obj);
    case DWG_TYPE_REGION:
      return dwg_json_REGION (dat, obj);
    case DWG_TYPE__3DSOLID:
      return dwg_json__3DSOLID (dat, obj);
    case DWG_TYPE_BODY:
      return dwg_json_BODY (dat, obj);
    case DWG_TYPE_RAY:
      return dwg_json_RAY (dat, obj);
    case DWG_TYPE_XLINE:
      return dwg_json_XLINE (dat, obj);
    case DWG_TYPE_DICTIONARY:
      return dwg_json_DICTIONARY (dat, obj);
    case DWG_TYPE_MTEXT:
      return dwg_json_MTEXT (dat, obj);
    case DWG_TYPE_LEADER:
      return dwg_json_LEADER (dat, obj);
    case DWG_TYPE_TOLERANCE:
      return dwg_json_TOLERANCE (dat, obj);
    case DWG_TYPE_MLINE:
      return dwg_json_MLINE (dat, obj);
    case DWG_TYPE_BLOCK_CONTROL:
      return dwg_json_BLOCK_CONTROL (dat, obj);
    case DWG_TYPE_BLOCK_HEADER:
      /*
      if (dat->version <= R_12
          && obj->tio.object
          && obj->tio.object->tio.BLOCK_HEADER
          && obj->tio.object->tio.BLOCK_HEADER->name
          && strEQc (obj->tio.object->tio.BLOCK_HEADER->name, "*MODEL_SPACE"))
        {
          LOG_TRACE ("Skip *MODEL_SPACE\n");
          return 0;
        }
      */
      return dwg_json_BLOCK_HEADER (dat, obj);
    case DWG_TYPE_LAYER_CONTROL:
      return dwg_json_LAYER_CONTROL (dat, obj);
    case DWG_TYPE_LAYER:
      return dwg_json_LAYER (dat, obj);
    case DWG_TYPE_STYLE_CONTROL:
      return dwg_json_STYLE_CONTROL (dat, obj);
    case DWG_TYPE_STYLE:
      return dwg_json_STYLE (dat, obj);
    case DWG_TYPE_LTYPE_CONTROL:
      return dwg_json_LTYPE_CONTROL (dat, obj);
    case DWG_TYPE_LTYPE:
      return dwg_json_LTYPE (dat, obj);
    case DWG_TYPE_VIEW_CONTROL:
      return dwg_json_VIEW_CONTROL (dat, obj);
    case DWG_TYPE_VIEW:
      return dwg_json_VIEW (dat, obj);
    case DWG_TYPE_UCS_CONTROL:
      return dwg_json_UCS_CONTROL (dat, obj);
    case DWG_TYPE_UCS:
      return dwg_json_UCS (dat, obj);
    case DWG_TYPE_VPORT_CONTROL:
      return dwg_json_VPORT_CONTROL (dat, obj);
    case DWG_TYPE_VPORT:
      return dwg_json_VPORT (dat, obj);
    case DWG_TYPE_APPID_CONTROL:
      return dwg_json_APPID_CONTROL (dat, obj);
    case DWG_TYPE_APPID:
      return dwg_json_APPID (dat, obj);
    case DWG_TYPE_DIMSTYLE_CONTROL:
      return dwg_json_DIMSTYLE_CONTROL (dat, obj);
    case DWG_TYPE_DIMSTYLE:
      return dwg_json_DIMSTYLE (dat, obj);
    case DWG_TYPE_VX_CONTROL:
      return dwg_json_VX_CONTROL (dat, obj);
    case DWG_TYPE_VX_TABLE_RECORD:
      return dwg_json_VX_TABLE_RECORD (dat, obj);
    case DWG_TYPE_GROUP:
      return dwg_json_GROUP (dat, obj);
    case DWG_TYPE_MLINESTYLE:
      return dwg_json_MLINESTYLE (dat, obj);
    case DWG_TYPE_OLE2FRAME:
      return dwg_json_OLE2FRAME (dat, obj);
    case DWG_TYPE_DUMMY:
      return dwg_json_DUMMY (dat, obj);
    case DWG_TYPE_LONG_TRANSACTION:
      return dwg_json_LONG_TRANSACTION (dat, obj);
    case DWG_TYPE_LWPOLYLINE:
      return dwg_json_LWPOLYLINE (dat, obj);
    case DWG_TYPE_HATCH:
      return dwg_json_HATCH (dat, obj);
    case DWG_TYPE_XRECORD:
      return dwg_json_XRECORD (dat, obj);
    case DWG_TYPE_PLACEHOLDER:
      return dwg_json_PLACEHOLDER (dat, obj);
    case DWG_TYPE_PROXY_ENTITY:
      return dwg_json_PROXY_ENTITY (dat, obj);
    case DWG_TYPE_PROXY_OBJECT:
      return dwg_json_PROXY_OBJECT (dat, obj);
    case DWG_TYPE_OLEFRAME:
      return dwg_json_OLEFRAME (dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own section\n");
      // dwg_json_VBA_PROJECT(dat, obj);
      break;
    case DWG_TYPE_REPEAT:
      return dwg_json_REPEAT (dat, obj);
    case DWG_TYPE_ENDREP:
      return dwg_json_ENDREP (dat, obj);
    case DWG_TYPE__3DLINE:
      return dwg_json__3DLINE (dat, obj);
    case DWG_TYPE_LOAD:
      return dwg_json_LOAD (dat, obj);
    case DWG_TYPE_JUMP:
      return dwg_json_JUMP (dat, obj);
    case DWG_TYPE_LAYOUT:
      return dwg_json_LAYOUT (dat, obj);
    default:
      if (obj->type != 0 && obj->type == obj->parent->layout_type)
        {
          return dwg_json_LAYOUT (dat, obj);
        }
      /* > 500 */
      else if (DWG_ERR_UNHANDLEDCLASS
               & (error = dwg_json_variable_type (obj->parent, dat, obj)))
        {
          Dwg_Data *dwg = obj->parent;
          int is_entity = 0;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;
          int num_bytes = obj->num_unknown_bits / 8;
          if (obj->num_unknown_bits & 8)
            num_bytes++;

          if (obj->fixedtype == DWG_TYPE_FREED)
            goto invalid_type;
          if (i >= 0 && i < (int)dwg->num_classes
              && obj->fixedtype < DWG_TYPE_FREED)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity (klass);
            }
          else
            {
              if (obj->fixedtype == DWG_TYPE_UNKNOWN_ENT)
                is_entity = 1;
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (!is_entity)
            {
              error |= dwg_json_UNKNOWN_OBJ (dat, obj);
              return error;
            }
          else
            {
              error |= dwg_json_UNKNOWN_ENT (dat, obj);
              return error;
            }
        }
      else
        return 0;
    }
invalid_type:
  LOG_WARN ("Unknown object, skipping eed/reactors/xdic/...");
  FIELD_TEXT (object, obj->name);
  if (obj->dxfname && strNE (obj->dxfname, obj->name))
    FIELD_TEXT (dxfname, obj->dxfname);
  _FIELD (index, RL, 0);
  _FIELD (type, RL, 0);
  KEY (handle);
  VALUE_H (obj->handle, 5);
  _FIELD (size, RL, 0);
  _FIELD (bitsize, BL, 0);
  return DWG_ERR_INVALIDTYPE;
}

static int
json_fileheader_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header *_obj = &dwg->header;
  Dwg_Object *obj = NULL;
  int i;

  RECORD (FILEHEADER); // single hash
  KEY (version);
  fprintf (dat->fh, "\"%s\"", dwg_version_codes (dwg->header.version));
  // clang-format off
  #include "header.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

AFL_GCC_TOOBIG
static int
json_preR13_header_write_private (Bit_Chain *restrict dat,
                                  Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  char buf[4096];
  double ms;
  int error = 0;
  const char *codepage = "ANSI_1252";

  // clang-format off
  #include "header_variables_r11.spec"
  // clang-format on
  return error;
}

static int
json_header_write_private (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  char buf[4096];
  double ms;
  int error = 0;
  const char *codepage
      = (dwg->header.codepage == 30 || dwg->header.codepage == 0) ? "ANSI_1252"
        : (dwg->header.version >= R_2007)                         ? "UTF-8"
                                          : "ANSI_1252";

  // clang-format off
  #include "header_variables.spec"
  // clang-format on
  return error;
}
AFL_GCC_POP

static int
json_header_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int error = 0;
  RECORD (HEADER); // single hash
  // separate funcs to catch the return, and end with ENDRECORD
  PRE (R_13b1)
  {
    // if (dat->from_version >= R_13b1)
    //   downgrade_preR13_header_variables (dat, dwg);
    error = json_preR13_header_write_private (dat, dwg);
  }
  LATER_VERSIONS
  {
    error = json_header_write_private (dat, dwg);
  }
  ENDRECORD ();
  return error;
}

static int
json_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BS i;

  SECTION (CLASSES); // list of classes
  for (i = 0; i < dwg->num_classes; i++)
    {
      Dwg_Class *_obj = &dwg->dwg_class[i];
      FIRSTPREFIX HASH;
      FIELD_BS (number, 0);
      FIELD_TV (dxfname, 1);
      FIELD_T (cppname, 2);
      FIELD_T (appname, 3);
      FIELD_BS (proxyflag, 90);
      FIELD_BL (num_instances, 91);
      FIELD_B (is_zombie, 280);
      FIELD_BS (item_class_id, 281);
      ENDHASH
      CLEARFIRST;
    }
  ENDSEC ();
  return 0;
}

// unused
static int
json_tables_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  int max_id = SECTION_VIEW;
  VERSIONS (R_10, R_11)
  {
    max_id = SECTION_DIMSTYLE;
  }
  VERSIONS (R_11, R_13b1)
  {
    max_id = SECTION_VX;
  }
  CLEARFIRST;
  SECTION (TABLES);
  // FIXME __cplusplus
  for (int id = SECTION_BLOCK; id <= max_id; id++)
    {
      int error;
      Dwg_Section *tbl = &dwg->header.section[id];
      BITCODE_RL num = tbl->objid_r11;

      switch (id)
        {
        case SECTION_BLOCK:
          SECTION (BLOCK);
          {
            Dwg_Section *_obj = tbl;
            FIELD_RL (size, 0);
            FIELD_RL (number, 0);
            PRE (R_13b1)
            {
              FIELD_RS (flags_r11, 0);
            }
            FIELD_RLL (address, 0);
          }
          for (BITCODE_RLd i = 0; i < (BITCODE_RLd)tbl->number; i++)
            {
              Dwg_Object *obj = &dwg->object[num + i];
              Dwg_Object_BLOCK_HEADER *_obj
                  = obj->tio.object->tio.BLOCK_HEADER;
              PRE (R_13b1)
              {
                if (strEQc (_obj->name, "*MODEL_SPACE"))
                  {
                    LOG_TRACE ("Skip *MODEL_SPACE\n");
                    continue;
                  }
              }
              RECORD (BLOCK_HEADER);
              FIELD_RC (flag, 70);
              FIELD_TFv (name, 32, 2);
              SINCE (R_11)
              {
                FIELD_RSd (used, 0); // -1
              }
              PRE (R_13b1)
              {
                FIELD_RL (block_offset_r11, 0);
              }
              LATER_VERSIONS
              {
                FIELD_RC (block_scaling, 0);
              }
              SINCE (R_11)
              {
                FIELD_HANDLE (block_entity, 2, 0);
                FIELD_RC (flag2, 0);
                FIELD_RC (unknown_r11, 0);
              }
              ENDRECORD ();
            }
          ENDSEC ();
          break;
        case SECTION_LAYER:
          SECTION (LAYER);
          {
            Dwg_Section *_obj = tbl;
            FIELD_RL (size, 0);
            FIELD_RL (number, 0);
            PRE (R_13b1)
            {
              FIELD_RS (flags_r11, 0);
            }
            FIELD_RLL (address, 0);
          }
          for (BITCODE_RLd i = 0; i < (BITCODE_RLd)tbl->number; i++)
            {
              Dwg_Object *obj = &dwg->object[num + i];
              Dwg_Object_LAYER *_obj = obj->tio.object->tio.LAYER;
              RECORD (LAYER);
              FIELD_RC (flag, 70);
              FIELD_TFv (name, 32, 2);
              FIELD_CMC (color, 62); // off if negative
              FIELD_HANDLE (ltype, 2, 6);
              ENDRECORD ();
            }
          ENDSEC ();
          break;
        case SECTION_STYLE:
          SECTION (STYLE);
          {
            Dwg_Section *_obj = tbl;
            FIELD_RL (size, 0);
            FIELD_RL (number, 0);
            PRE (R_13b1)
            {
              FIELD_RS (flags_r11, 0);
            }
            FIELD_RLL (address, 0);
          }
          for (BITCODE_RLd i = 0; i < (BITCODE_RLd)tbl->number; i++)
            {
              Dwg_Object *obj = &dwg->object[num + i];
              Dwg_Object_STYLE *_obj = obj->tio.object->tio.STYLE;
              RECORD (STYLE);
              FIELD_RC (flag, 70);
              FIELD_TFv (name, 32, 2);

              FIELD_RD (text_size, 40); // ok
              FIELD_RD (width_factor, 41);
              FIELD_RD (oblique_angle, 50);
              FIELD_RC (generation, 71);
              FIELD_RD (last_height, 42);
              FIELD_TFv (font_file, 64, 3);    // 8ed
              FIELD_TFv (bigfont_file, 64, 4); // 92d
              ENDRECORD ();
            }
          ENDSEC ();
          break;
        case SECTION_LTYPE:
          SECTION (LTYPE);
          {
            Dwg_Section *_obj = tbl;
            FIELD_RL (size, 0);
            FIELD_RL (number, 0);
            PRE (R_13b1)
            {
              FIELD_RS (flags_r11, 0);
            }
            FIELD_RLL (address, 0);
          }
          for (BITCODE_RLd i = 0; i < (BITCODE_RLd)tbl->number; i++)
            {
              BITCODE_BL vcount;
              Dwg_Object *obj = &dwg->object[num + i];
              Dwg_Object_LTYPE *_obj = obj->tio.object->tio.LTYPE;
              RECORD (LTYPE);
              FIELD_RC (flag, 70);
              FIELD_TFv (name, 32, 2);
              FIELD_RS (used, 0);

              FIELD_TFv (description, 48, 3);
              FIELD_RC (alignment, 72);
              FIELD_RCu (numdashes, 73);
              FIELD_RD (pattern_len, 40);
              FIELD_VECTOR_INL (dashes_r11, RD, 12, 49);
              ENDRECORD ();
            }
          ENDSEC ();
          break;
        case SECTION_VIEW:
          SECTION (VIEW);
          {
            Dwg_Section *_obj = tbl;
            FIELD_RL (size, 0);
            FIELD_RL (number, 0);
            PRE (R_13b1)
            {
              FIELD_RS (flags_r11, 0);
            }
            FIELD_RLL (address, 0);
          }
          for (BITCODE_RLd i = 0; i < (BITCODE_RLd)tbl->number; i++)
            {
              BITCODE_BL vcount;
              Dwg_Object *obj = &dwg->object[num + i];
              Dwg_Object_VIEW *_obj = obj->tio.object->tio.VIEW;
              RECORD (VIEW);
              FIELD_RC (flag, 70);
              FIELD_TFv (name, 32, 2);
              FIELD_RS (used, 0);

              FIELD_RD (VIEWSIZE, 40);
              FIELD_2RD (VIEWCTR, 10);
              FIELD_RD (view_width, 41);
              FIELD_3RD (view_target, 12);
              FIELD_3RD (VIEWDIR, 11);
              FIELD_CAST (VIEWMODE, RS, 4BITS, 71);
              FIELD_RD (lens_length, 42);
              FIELD_RD (front_clip_z, 43);
              FIELD_RD (back_clip_z, 44);
              FIELD_RD (twist_angle, 50);
              ENDRECORD ();
            }
          ENDSEC ();
          break;
        case SECTION_UCS:
          SECTION (UCS);
          {
            Dwg_Section *_obj = tbl;
            FIELD_RL (size, 0);
            FIELD_RL (number, 0);
            PRE (R_13b1)
            {
              FIELD_RS (flags_r11, 0);
            }
            FIELD_RLL (address, 0);
          }
          for (BITCODE_RLd i = 0; i < (BITCODE_RLd)tbl->number; i++)
            {
              BITCODE_BL vcount;
              Dwg_Object *obj = &dwg->object[num + i];
              Dwg_Object_UCS *_obj = obj->tio.object->tio.UCS;
              RECORD (UCS);
              FIELD_RC (flag, 70);
              FIELD_TFv (name, 32, 2);
              FIELD_RS (used, 0);

              FIELD_2RD (ucsorg, 10);
              FIELD_2RD (ucsxdir, 11);
              FIELD_2RD (ucsydir, 12);
              ENDRECORD ();
            }
          ENDSEC ();
          break;
        default:
          LOG_WARN ("Missing TABLE %u", id)
        }
      CLEARFIRST;
    }
  ENDSEC ();
  return 0;
}

static int
json_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  CLEARFIRST;
  SECTION (OBJECTS);
  for (i = 0; i < dwg->num_objects; i++)
    {
      int error;
      Dwg_Object *obj = &dwg->object[i];
      FIRSTPREFIX HASH;
      error = dwg_json_object (dat, obj);
      ENDHASH
      CLEARFIRST;
    }
  ENDSEC ();
  return 0;
}

#if 0
/* The object map/handles section */
static int
json_handles_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL j;
  CLEARFIRST;
  SECTION (HANDLES);
  for (j = 0; j < dwg->num_objects; j++)
    {
      Dwg_Object *obj = &dwg->object[j];
      // handle => abs. offset
      // TODO: The real HANDLES section omap has handleoffset (deleted holes) and addressoffset
      FIRSTPREFIX fprintf (dat->fh, "[%lu,%s%lu]", obj->handle.value, JSON_SPC, obj->address);
    }
  ENDSEC ();
  return 0;
}
#endif

static int
json_thumbnail_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Bit_Chain *_obj = (Bit_Chain *)&dwg->thumbnail;
  if (_obj->chain && _obj->size && _obj->size > 10)
    {
      /* SECTION_PREVIEW includes the sentinel.
         _obj->byte is at the BMP offset, via dwg_bmp */
      if (dwg->header.from_version >= R_2004)
        _obj->chain += 16; /* skip the sentinel */
      KEY (THUMBNAILIMAGE);
      HASH;
      FIRSTPREFIX fprintf (dat->fh, "\"size\":%s%" PRIuSIZE, JSON_SPC,
                           _obj->size);
      FIELD_BINARY (chain, _obj->size, 310);
      if (dwg->header.from_version >= R_2004)
        _obj->chain -= 16; /* undo for FREE */
      ENDHASH;
    }
  return 0;
}

static int
json_section_r2004fileheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_R2004_Header *_obj = &dwg->fhdr.r2004_header;
  Dwg_Object *obj = NULL;
  int i;

  RECORD (R2004_Header); // single hash

  // clang-format off
  #include "r2004_file_header.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_r2007fileheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_R2007_Header *_obj = &dwg->fhdr.r2007_file_header;
  Dwg_Object *obj = NULL;
  int i;

  RECORD (R2007_Header); // single hash
  FIELD_RLL (header_size, 0);
  FIELD_RLL (file_size, 0);
  FIELD_RLL (pages_map_crc_compressed, 0);
  FIELD_RLL (pages_map_correction, 0);
  FIELD_RLL (pages_map_crc_seed, 0);
  FIELD_RLL (pages_map2_offset, 0);
  FIELD_RLL (pages_map2_id, 0);
  FIELD_RLL (pages_map_offset, 0);
  FIELD_RLL (pages_map_id, 0);
  FIELD_RLL (header2_offset, 0);
  FIELD_RLL (pages_map_size_comp, 0);
  FIELD_RLL (pages_map_size_uncomp, 0);
  FIELD_RLL (pages_amount, 0);
  FIELD_RLL (pages_maxid, 0);
  FIELD_RLL (unknown1, 0);
  FIELD_RLL (unknown2, 0);
  FIELD_RLL (pages_map_crc_uncomp, 0);
  FIELD_RLL (unknown3, 0);
  FIELD_RLL (unknown4, 0);
  FIELD_RLL (unknown5, 0);
  FIELD_RLL (num_sections, 0);
  FIELD_RLL (sections_map_crc_uncomp, 0);
  FIELD_RLL (sections_map_size_comp, 0);
  FIELD_RLL (sections_map2_id, 0);
  FIELD_RLL (sections_map_id, 0);
  FIELD_RLL (sections_map_size_uncomp, 0);
  FIELD_RLL (sections_map_crc_comp, 0);
  FIELD_RLL (sections_map_correction, 0);
  FIELD_RLL (sections_map_crc_seed, 0);
  FIELD_RLL (stream_version, 0);
  FIELD_RLL (crc_seed, 0);
  FIELD_RLL (crc_seed_encoded, 0);
  FIELD_RLL (random_seed, 0);
  FIELD_RLL (header_crc, 0);
  ENDRECORD ();

  return 0;
}

static int
json_section_summary (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (SummaryInfo); // single hash
  // clang-format off
  #include "summaryinfo.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_vbaproject (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_VBAProject *_obj = &dwg->vbaproject;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (VBAProject); // single hash
  HASH;
  // clang-format off
  //#include "vbaproject.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_appinfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_AppInfo *_obj = &dwg->appinfo;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (AppInfo); // single hash

  // clang-format off
  #include "appinfo.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_appinfohistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_AppInfoHistory *_obj = &dwg->appinfohistory;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (AppInfoHistory); // single hash
  FIRSTPREFIX fprintf (dat->fh, "\"size\":%s%d", JSON_SPC, _obj->size);
  FIELD_BINARY (unknown_bits, _obj->size, 0);
  // clang-format off
  //#include "appinfohistory.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_filedeplist (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_FileDepList *_obj = &dwg->filedeplist;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL vcount;

  RECORD (FileDepList); // single hash

  // clang-format off
  #include "filedeplist.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_security (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Security *_obj = &dwg->security;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (Security); // single hash

  // clang-format off
  #include "security.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_revhistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_RevHistory *_obj = &dwg->revhistory;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL vcount;

  RECORD (RevHistory); // single hash
  FIELD_RL (class_version, 0);
  FIELD_RL (class_minor, 0);
  FIELD_RL (num_histories, 0);
  FIELD_VECTOR (histories, RL, num_histories, 0)
  ENDRECORD ();
  return 0;
}

static int
json_section_objfreespace (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_ObjFreeSpace *_obj = &dwg->objfreespace;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (ObjFreeSpace); // single hash

  // clang-format off
  #include "objfreespace.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_acds (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_AcDs *_obj = &dwg->acds;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount3 = 0, rcount4, vcount;

  RECORD (AcDs); // single hash
  {

    // clang-format off
    #include "acds.spec"
    // clang-format on
  }
  ENDRECORD ();
  return 0;
}

static int
json_section_template (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Template *_obj = &dwg->Template;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (Template); // single hash. i.e MEASUREMENT metric/imperial

  // clang-format off
  #include "template.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_auxheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_AuxHeader *_obj = &dwg->auxheader;
  Dwg_Object *obj = NULL;
  int error = 0, i;
  BITCODE_RL vcount;

  RECORD (AuxHeader); // single hash

  // clang-format off
  #include "auxheader.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

#if 0
static int
json_section_signature (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_Signature *_obj = &dwg->signature;
  Dwg_Object *obj = NULL;
  int error = 0;

  RECORD (Signature); // single hash

  // clang-format off
  #include "signature.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}
#endif

static int
json_section_2ndheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct _dwg_secondheader *_obj = &dwg->secondheader;
  Dwg_Object *obj = NULL;
  BITCODE_BL vcount;
  int error = 0;

  RECORD (SecondHeader); // single hash
  // clang-format off
  #include "2ndheader.spec"
  // clang-format on
  VERSIONS (R_14, R_2000)
  {
    FIELD_RLL (junk_r14, 0);
  }
  ENDRECORD ();
  return 0;
}

EXPORT int
dwg_write_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header *obj = &dwg->header;
  int error = 0;

  if (!dat->version)
    dat->version = dat->from_version;
  if (!dat->codepage)
    dat->codepage = dwg->header.codepage;
  if (!dat->fh)
    goto fail;
  fprintf (dat->fh, "{%s%s%s\"created_by\":%s\"%s\"", JSON_NL, JSON_SPC,
           JSON_SPC, JSON_SPC, PACKAGE_STRING);
  dat->bit++; // ident

  json_fileheader_write (dat, dwg);

  // A minimal HEADER would require only $ACADVER, $HANDSEED, and then
  // ENTITIES. But we do that only for DXF
  json_header_write (dat, dwg);

  if (dat->version >= R_13b1)
    {
      if (json_classes_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }
  if (dat->version < R_13b1 && 0)
    {
      if (json_tables_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }

  if (json_objects_write (dat, dwg) >= DWG_ERR_CRITICAL)
    goto fail;

  if (dat->version >= R_13b1)
    {
      if (json_thumbnail_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
      /* the other sections */
      if (dat->version <= R_2000)
        {
          if (dwg->header.sections >= 3 && dwg->objfreespace.numnums)
            error |= json_section_objfreespace (dat, dwg); // 3
          if (dwg->secondheader.num_sections)
            error |= json_section_2ndheader (dat, dwg);
          if (dwg->header.sections >= 4)
            error |= json_section_template (dat, dwg); // 4 i.e. MEASUREMENT
          if (dwg->header.sections >= 6)
            error |= json_section_auxheader (dat, dwg); // 5
        }
      if (dat->version >= R_2004)
        {
          if (dat->version == R_2007)
            error |= json_section_r2007fileheader (dat, dwg);
          else
            error |= json_section_r2004fileheader (dat, dwg);
          if (dwg->header.summaryinfo_address)
            error |= json_section_summary (dat, dwg);
          if (dwg->header.vbaproj_address)
            error |= json_section_vbaproject (dat, dwg);
          error |= json_section_appinfo (dat, dwg);
          error |= json_section_appinfohistory (dat, dwg);
          error |= json_section_filedeplist (dat, dwg);
          error |= json_section_security (dat, dwg);
          error |= json_section_revhistory (dat, dwg);
          error |= json_section_objfreespace (dat, dwg);
          // error |= json_section_signature (dat, dwg);
          error |= json_section_template (dat, dwg);
          error |= json_section_acds (dat, dwg);
        }
    }

#if 0
  /* object map */
  if (dat->version >= R_13b1)
    {
      if (json_handles_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }
#endif

  dat->bit--;
  fprintf (dat->fh, "}%s", JSON_NL);
  return 0;
fail:
  return 1;
}

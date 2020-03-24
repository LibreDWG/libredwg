/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
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
/* TODO: NOCOMMA and \n not with stdout. stdout is line-buffered (#75),
 *       so NOCOMMA cannot backup past the previous \n to delete the comma.
 *       We really have to add the comma before, not after, and special case
 * the first field, not the last to omit the comma.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifdef HAVE_CTYPE_H
#  include <ctype.h>
#endif

#include "common.h"
#include "bits.h"
#include "myalloca.h"
#include "dwg.h"
#include "decode.h"
#include "out_json.h"

#define DWG_LOGLEVEL DWG_LOGLEVEL_NONE
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
/* see also examples/unknown.c */
#ifdef HAVE_NATIVE_WCHAR2
static wchar_t *wcquote (wchar_t *restrict dest, const wchar_t *restrict src);
#else
static void print_wcquote (Bit_Chain *restrict dat,
                           dwg_wchar_t *restrict wstr);
#endif

static void _prefix (Bit_Chain *dat);
static char* _path_field(const char *path);

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION json
#define IS_JSON

#define PREFIX _prefix (dat);
#define ARRAY                                                                 \
  fprintf (dat->fh, "[\n");                                                   \
  dat->bit++
#define ENDARRAY                                                              \
  fprintf (dat->fh, "\n");                                                    \
  dat->bit--;                                                                 \
  PREFIX fprintf (dat->fh, "],\n")
#define LASTENDARRAY                                                          \
  fprintf (dat->fh, "\n");                                                    \
  dat->bit--;                                                                 \
  PREFIX fprintf (dat->fh, "]\n")
#define KEYs(nam) PREFIX fprintf (dat->fh, "\"%s\": ", nam)
// strip path to field only
#define KEY(nam) PREFIX fprintf (dat->fh, "\"%s\": ", _path_field(#nam))
#define HASH                                                                  \
  PREFIX fprintf (dat->fh, "{\n");                                            \
  dat->bit++
#define ENDHASH                                                               \
  fprintf (dat->fh, "\n");                                                    \
  dat->bit--;                                                                 \
  PREFIX fprintf (dat->fh, "},\n")
#define LASTENDHASH                                                           \
  fprintf (dat->fh, "\n");                                                    \
  dat->bit--;                                                                 \
  PREFIX fprintf (dat->fh, "}")
#define NOCOMMA fseek (dat->fh, -2, SEEK_CUR)

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

#undef FORMAT_H
#undef FORMAT_RLL
#undef FORMAT_BLL
#undef FORMAT_RC
#undef FORMAT_RSx
#undef FORMAT_RLx
#undef FORMAT_BLx
#undef FORMAT_BLX
#undef FORMAT_4BITS
#undef ARGS_H
#define ARGS_H(hdl) hdl.code, hdl.value
#define FORMAT_H "[%u, %lu]"
#define ARGS_HREF(ref) ref->handleref.code, ref->handleref.size, ref->handleref.value, ref->absolute_ref
#undef FORMAT_RD
#undef FORMAT_BD
#define FORMAT_RD "%.14f"
#define FORMAT_BD FORMAT_RD
#define FORMAT_HREF "[%u, %u, %lu, %lu]"
#define FORMAT_RLL "%" PRIu64
#define FORMAT_BLL "%" PRIu64
#define FORMAT_RC "%d"
#define FORMAT_RSx FORMAT_RS
#define FORMAT_RLx FORMAT_RL
#define FORMAT_BLx FORMAT_BL
#define FORMAT_BLX FORMAT_BL
#define FORMAT_4BITS FORMAT_RC
#define VALUE(value, type, dxf) fprintf (dat->fh, FORMAT_##type ",\n", value)
#define VALUE_B(value, dxf) VALUE (value, B, dxf)
#define VALUE_RC(value, dxf) VALUE (value, RC, dxf)
#define VALUE_RS(value, dxf) VALUE (value, RS, dxf)
#define VALUE_RL(value, dxf) VALUE (value, RL, dxf)
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
    int k;                                                                    \
    snprintf (_buf, 255, FORMAT_RD, value);                                   \
    k = strlen (_buf);                                                        \
    if (strrchr (_buf, '.') && _buf[k - 1] == '0')                            \
      {                                                                       \
        for (k--; k > 1 && _buf[k - 1] != '.' && _buf[k] == '0'; k--)         \
          _buf[k] = '\0';                                                     \
      }                                                                       \
    fprintf (dat->fh, "%s", _buf);                                            \
  }
#define VALUE_2RD(pt, dxf)                                                    \
  {                                                                           \
    fprintf (dat->fh, "[ ");                                                  \
    VALUE_RD (pt.x, 0);                                                       \
    fprintf (dat->fh, ", ");                                                  \
    VALUE_RD (pt.y, 0);                                                       \
    fprintf (dat->fh, " ],\n");                                               \
  }
#define VALUE_2DD(pt, d1, d2, dxf) VALUE_2RD (pt, dxf)
#define VALUE_3RD(pt, dxf)                                                    \
  {                                                                           \
    fprintf (dat->fh, "[ ");                                                  \
    VALUE_RD (pt.x, 0);                                                       \
    fprintf (dat->fh, ", ");                                                  \
    VALUE_RD (pt.y, 0);                                                       \
    fprintf (dat->fh, ", ");                                                  \
    VALUE_RD (pt.z, 0);                                                       \
    fprintf (dat->fh, " ],\n");                                               \
  }
#define VALUE_3BD(pt, dxf) VALUE_3RD (pt, dxf)
#define VALUE_TV(nam, dxf)

#define FIELD(nam, type, dxf)                                                 \
  if (!memBEGINc (#nam, "num_"))                                              \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"%s\": " FORMAT_##type ",\n",                \
                      _path_field(#nam), _obj->nam);                          \
    }
#define _FIELD(nam, type, value)                                              \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"" #nam "\": " FORMAT_##type ",\n", obj->nam); \
  }
#define ENT_FIELD(nam, type, value)                                           \
  {                                                                           \
    PREFIX fprintf (dat->fh, "\"%s\": " FORMAT_##type ",\n",                  \
                    _path_field(#nam), _ent->nam);                            \
  }
#define SUB_FIELD(o, nam, type, dxf)                                          \
  if (!memBEGINc (#nam, "num_"))                                              \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"%s\": " FORMAT_##type ",\n",                \
                      _path_field(#nam), _obj->o.nam);                        \
    }
#define FIELD_CAST(nam, type, cast, dxf) FIELD (nam, cast, dxf)
#define FIELD_TRACE(nam, type)
#define FIELD_G_TRACE(nam, type, dxf)
#define FIELD_TEXT(nam, str)                                                  \
  {                                                                           \
    PREFIX                                                                    \
    fprintf (dat->fh, "\"%s\": ", _path_field(#nam));                         \
    VALUE_TEXT ((char*)str)                                                   \
  }

#define VALUE_TEXT(str)                                                       \
  {                                                                           \
    if (str                                                                   \
        && (1 || strchr (str, '"') || strchr (str, '\\')                      \
              || strchr (str, '\n')))                                         \
      {                                                                       \
        const int len = strlen (str);                                         \
        if (len < 4096 / 6)                                                   \
          {                                                                   \
            const int _len = 6 * len + 1;                                     \
            char *_buf = alloca (_len);                                       \
            fprintf (dat->fh, "\"%s\",\n", json_cquote (_buf, str, _len));    \
            freea (_buf);                                                     \
          }                                                                   \
        else                                                                  \
          {                                                                   \
            const int _len = 6 * len + 1;                                     \
            char *_buf = malloc (_len);                                       \
            fprintf (dat->fh, "\"%s\",\n", json_cquote (_buf, str, _len));    \
            free (_buf);                                                      \
          }                                                                   \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        fprintf (dat->fh, "\"%s\",\n", str ? str : "");                       \
      }                                                                       \
  }

#ifdef HAVE_NATIVE_WCHAR2
#  define VALUE_TEXT_TU(wstr)                                                 \
    if (wstr                                                                  \
        && (1 || wcschr ((wchar_t *)wstr, L'"')                               \
            || wcschr ((wchar_t *)wstr, L'\\')                                \
            || wcschr ((wchar_t *)wstr, L'\n')))                              \
      {                                                                       \
        wchar_t *_buf = malloc (6 * wcslen ((wchar_t *)wstr) + 2);            \
        fprintf (dat->fh, "\"%ls\",\n", wcquote (_buf, (wchar_t *)wstr));     \
        free (_buf);                                                          \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        fprintf (dat->fh, "\"%ls\",\n", wstr ? (wchar_t *)wstr : L"");        \
      }
#else
#  define VALUE_TEXT_TU(wstr) print_wcquote (dat, (BITCODE_TU)wstr)
#endif
#define FIELD_TEXT_TU(nam, wstr)                                              \
  KEY (nam);                                                                  \
  VALUE_TEXT_TU ((BITCODE_TU)wstr)

#define FIELD_VALUE(nam) _obj->nam
#define ANYCODE -1
// todo: only the name, not the ref
#define VALUE_HANDLE(hdlptr, nam, handle_code, dxf)                           \
  if (hdlptr)                                                                 \
    {                                                                         \
      fprintf (dat->fh, FORMAT_HREF ",\n", ARGS_HREF (hdlptr));               \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      fprintf (dat->fh, "[0, 0],\n");                                         \
    }
#define VALUE_H(hdl, dxf) \
  fprintf (dat->fh, FORMAT_H ",\n", hdl.code, hdl.value)
#define FIELD_HANDLE(nam, handle_code, dxf)                                   \
  {                                                                           \
    if (_obj->nam)                                                            \
      {                                                                       \
        PREFIX fprintf (dat->fh, "\"%s\": " FORMAT_HREF ",\n",                \
                        _path_field(#nam), ARGS_HREF (_obj->nam));            \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        PREFIX fprintf (dat->fh, "\"%s\": [0, 0],\n", _path_field(#nam));     \
      }                                                                       \
  }
#define SUB_FIELD_HANDLE(o, nam, handle_code, dxf)                            \
  {                                                                           \
    if (_obj->o.nam)                                                          \
      {                                                                       \
        PREFIX fprintf (dat->fh, "\"%s\": " FORMAT_HREF ",\n",                \
                        _path_field(#nam), ARGS_HREF (_obj->o.nam));          \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        PREFIX fprintf (dat->fh, "\"%s\": [0, 0],\n", _path_field(#nam));     \
      }                                                                       \
  }
#define FIELD_DATAHANDLE(nam, code, dxf) FIELD_HANDLE (nam, code, dxf)
#define FIELD_HANDLE_N(nam, vcount, handle_code, dxf)                         \
  if (_obj->nam)                                                              \
    {                                                                         \
      PREFIX fprintf (dat->fh, FORMAT_HREF ",\n", ARGS_HREF (_obj->nam));     \
    }                                                                         \
  else                                                                        \
    {                                                                         \
      PREFIX fprintf (dat->fh, "[0, 0],\n");                                  \
    }
#define VALUE_BINARY(buf, len, dxf)                                           \
  {                                                                           \
    fprintf (dat->fh, "\"");                                                  \
    if (buf && len)                                                           \
      {                                                                       \
        for (long j = 0; j < (long)len; j++)                                  \
          {                                                                   \
            fprintf (dat->fh, "%02X", ((BITCODE_RC*)buf)[j]);                 \
          }                                                                   \
      }                                                                       \
    fprintf (dat->fh, "\",\n");                                               \
  }
#define FIELD_BINARY(nam, size, dxf)                                          \
  {                                                                           \
    long len = (long)size;                                                    \
    KEY (nam);                                                                \
    fprintf (dat->fh, "\"");                                                  \
    if (_obj->nam)                                                            \
      {                                                                       \
        for (long j = 0; j < len; j++)                                        \
          {                                                                   \
            fprintf (dat->fh, "%02X", ((BITCODE_RC*)_obj->nam)[j]);           \
          }                                                                   \
      }                                                                       \
    fprintf (dat->fh, "\",\n");                                               \
  }

#define FIELD_B(nam, dxf) FIELD (nam, B, dxf)
#define FIELD_BB(nam, dxf) FIELD (nam, BB, dxf)
#define FIELD_3B(nam, dxf) FIELD (nam, 3B, dxf)
#define FIELD_BS(nam, dxf) FIELD (nam, BS, dxf)
#define FIELD_BL(nam, dxf) FIELD (nam, BL, dxf)
#define FIELD_BLL(nam, dxf) FIELD (nam, BLL, dxf)
#ifdef IS_RELEASE
#  define FIELD_BD(nam, dxf)                                                  \
    {                                                                         \
      if (!bit_isnan (_obj->nam))                                             \
        {                                                                     \
          PREFIX fprintf (dat->fh, "\"%s\": ", _path_field (#nam));           \
          _VALUE_RD (_obj->nam, dxf);                                         \
          fprintf (dat->fh, ",\n");                                           \
        }                                                                     \
    }
#  define FIELD_2RD(nam, dxf)                                                 \
    {                                                                         \
      if (!bit_isnan (_obj->nam.x) && !bit_isnan (_obj->nam.y))               \
        {                                                                     \
          PREFIX fprintf (dat->fh, "\"" #nam "\": ");                         \
          VALUE_2RD (_obj->nam, dxf);                                         \
        }                                                                     \
    }
#  define FIELD_3RD(nam, dxf)                                                 \
    {                                                                         \
      if (!bit_isnan (_obj->nam.x) && !bit_isnan (_obj->nam.y)                \
          && !bit_isnan (_obj->nam.z))                                        \
        {                                                                     \
          PREFIX fprintf (dat->fh, "\"" #nam "\": ");                         \
          VALUE_3RD (_obj->nam, dxf);                                         \
        }                                                                     \
    }
#  define SUB_FIELD_BD(o, nam, dxf)                                           \
    {                                                                         \
      if (!bit_isnan (_obj->o.nam))                                           \
        {                                                                     \
          PREFIX fprintf (dat->fh, "\"%s\": ", _path_field (#nam));           \
          _VALUE_RD (_obj->o.nam, dxf);                                       \
          fprintf (dat->fh, ",\n");                                           \
        }                                                                     \
    }
#else /* IS_RELEASE */
#  define FIELD_BD(nam, dxf)                                                  \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"%s\": ", _path_field (#nam));               \
      _VALUE_RD (_obj->nam, dxf);                                             \
      fprintf (dat->fh, ",\n");                                               \
    }
#  define FIELD_2RD(nam, dxf)                                                 \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"" #nam "\": ");                             \
      VALUE_2RD (_obj->nam, dxf);                                             \
    }
#  define FIELD_3RD(nam, dxf)                                                  \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"" #nam "\": ");                             \
      VALUE_3RD (_obj->nam, dxf);                                             \
    }
# define SUB_FIELD_BD(o, nam, dxf)                                            \
    {                                                                         \
      PREFIX fprintf (dat->fh, "\"%s\": ", _path_field (#nam));               \
      _VALUE_RD (_obj->o.nam, dxf);                                           \
      fprintf (dat->fh, ",\n");                                               \
    }
#endif

#define FIELD_RC(nam, dxf) FIELD (nam, RC, dxf)
#define FIELD_RCx(nam, dxf) FIELD (nam, RC, dxf)
#define FIELD_RS(nam, dxf) FIELD (nam, RS, dxf)
#define FIELD_RD(nam, dxf) FIELD_BD (nam, dxf)
#define FIELD_RL(nam, dxf) FIELD (nam, RL, dxf)
#define FIELD_RLL(nam, dxf) FIELD (nam, RLL, dxf)
#define FIELD_MC(nam, dxf) FIELD (nam, MC, dxf)
#define FIELD_MS(nam, dxf) FIELD (nam, MS, dxf)
#define FIELD_TF(nam, len, dxf) FIELD_TEXT (nam, _obj->nam)
#define FIELD_TFF(nam, len, dxf) FIELD_TEXT (nam, (const char*)_obj->nam)
#define FIELD_TFFx(nam, len, dxf) FIELD_BINARY (nam, len, dxf)
#define FIELD_TV(nam, dxf) FIELD_TEXT (nam, _obj->nam)
#define FIELD_TU(nam, dxf) FIELD_TEXT_TU (nam, (BITCODE_TU)_obj->nam)
#define FIELD_T16(nam, dxf) FIELD_TV (nam, dxf)
#define FIELD_TU16(nam, dxf) FIELD_TU (nam, dxf)
#define FIELD_T32(nam, dxf) FIELD_T (nam, dxf)
#define FIELD_T(nam, dxf)                                                     \
  {                                                                           \
    if (dat->version >= R_2007)                                               \
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
    if (dat->version >= R_2007)                                               \
      {                                                                       \
        FIELD_TEXT_TU (nam, str);                                             \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        FIELD_TEXT (nam, str);                                                \
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
#define FIELD_2DD(nam, d1, d2, dxf) FIELD_2RD (nam, dxf)
#define FIELD_3DD(nam, def, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3BD(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3BD_1(nam, dxf) FIELD_3RD (nam, dxf)
#define FIELD_3DPOINT(nam, dxf) FIELD_3BD (nam, dxf)

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
#define SUB_FIELD_BSd(o, nam, dxf) SUB_FIELD (o, nam, BSd, dxf)
#define SUB_FIELD_BL(o, nam, dxf) SUB_FIELD (o, nam, BL, dxf)
#define SUB_FIELD_BLx(o, nam, dxf) SUB_FIELD (o, nam, BLx, dxf)
#define SUB_FIELD_BLd(o, nam, dxf) SUB_FIELD (o, nam, BLd, dxf)
#define SUB_FIELD_RC(o, nam, dxf) SUB_FIELD (o, nam, RC, dxf)
#define SUB_FIELD_RS(o, nam, dxf) SUB_FIELD (o, nam, RS, dxf)
#define SUB_FIELD_RL(o, nam, dxf) SUB_FIELD (o, nam, RL, dxf)
#define SUB_FIELD_BLL(o, nam, dxf) SUB_FIELD (o, nam, BLL, dxf)
#define SUB_FIELD_RLL(o, nam, dxf) SUB_FIELD (o, nam, RLL, dxf)

#define SUB_FIELD_RD(o, nam, dxf) SUB_FIELD_BD (o, nam, dxf)
#define SUB_FIELD_3BD_inl(o, nam, dxf)                                        \
  SUB_FIELD_RD (o, x, dxf);                                                   \
  SUB_FIELD_RD (o, y, dxf);                                                   \
  SUB_FIELD_RD (o, z, dxf)
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
field_cmc (Bit_Chain *restrict dat, const char *restrict key,
           const Dwg_Color *restrict color)
{
  if (dat->version >= R_2004)
    {
      PREFIX fprintf (dat->fh, "\"%s\": {\n", _path_field (key));
      dat->bit++;
      if (color->index)
        {
          PREFIX fprintf (dat->fh, "\"index\": %d,\n", color->index);
        }
      PREFIX fprintf (dat->fh, "\"rgb\": \"%06x\",\n", (unsigned)color->rgb);
      if (color->flag)
        {
          PREFIX fprintf (dat->fh, "\"flag\": %d,\n", color->flag);
        }
      if (color->flag > 0 && color->flag < 8)
        {
          if (color->flag & 1)
            _FIELD_TV_ALPHA (name, color->name)
          if (color->flag & 2)
            _FIELD_TV_ALPHA (book_name, color->book_name)
        }
      ENDRECORD ();
    }
  else
    {
      PREFIX fprintf (dat->fh, "\"%s\": %d,\n", _path_field (key), color->index);
    }
}

#define FIELD_CMC(color, dxf1, dxf2) field_cmc (dat, #color, &_obj->color)
#define SUB_FIELD_CMC(o, color, dxf1, dxf2)                                   \
  field_cmc (dat, #color, &_obj->o.color)

#define FIELD_TIMEBLL(nam, dxf)                                               \
  PREFIX fprintf (dat->fh, "\"" #nam "\": [ " FORMAT_BL ", " FORMAT_BL " ],\n", \
                  _obj->nam.days, _obj->nam.ms)
#define FIELD_TIMERLL(nam, dxf) FIELD_TIMEBLL(nam, dxf)

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
          PREFIX fprintf (dat->fh, FORMAT_##type ",\n", _obj->nam[vcount]);   \
        }                                                                     \
      if (size)                                                               \
        NOCOMMA;                                                              \
    }                                                                         \
  ENDARRAY;
#define FIELD_VECTOR_T(nam, type, size, dxf)                                  \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  if (_obj->nam)                                                              \
    {                                                                         \
      PRE (R_2007)                                                            \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          {                                                                   \
            PREFIX VALUE_TEXT (_obj->nam[vcount])                             \
          }                                                                   \
      }                                                                       \
      else                                                                    \
      {                                                                       \
        for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)           \
          {                                                                   \
            PREFIX VALUE_TEXT_TU (_obj->nam[vcount]);                         \
          }                                                                   \
      }                                                                       \
      if (_obj->size)                                                         \
        NOCOMMA;                                                              \
    }                                                                         \
  ENDARRAY;

#define FIELD_VECTOR(nam, type, size, dxf)                                    \
  FIELD_VECTOR_N (nam, type, _obj->size, dxf)
#define FIELD_VECTOR_INL(nam, type, size, dxf)                                \
  FIELD_VECTOR_N(nam, type, size, dxf)

#define FIELD_2RD_VECTOR(nam, size, dxf)                                      \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      VALUE_2RD (FIELD_VALUE (nam[vcount]), dxf);                             \
    }                                                                         \
  if (_obj->size)                                                             \
    NOCOMMA;                                                                  \
  ENDARRAY;

#define FIELD_2DD_VECTOR(nam, size, dxf)                                      \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      PREFIX VALUE_2RD (_obj->nam[vcount], dxf);                              \
    }                                                                         \
  if (_obj->size)                                                             \
    NOCOMMA;                                                                  \
  ENDARRAY;

#define FIELD_3DPOINT_VECTOR(nam, size, dxf)                                  \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)_obj->size; vcount++)                 \
    {                                                                         \
      PREFIX VALUE_3BD (FIELD_VALUE (nam[vcount]), dxf);                      \
    }                                                                         \
  if (_obj->size)                                                             \
    NOCOMMA;                                                                  \
  ENDARRAY;

#define HANDLE_VECTOR_N(nam, size, code, dxf)                                 \
  KEY (nam);                                                                  \
  ARRAY;                                                                      \
  for (vcount = 0; vcount < (BITCODE_BL)size; vcount++)                       \
    {                                                                         \
      FIELD_HANDLE_N (nam[vcount], vcount, code, dxf);                        \
    }                                                                         \
  if (size)                                                                   \
    NOCOMMA;                                                                  \
  ENDARRAY;

#define HANDLE_VECTOR(nam, sizefield, code, dxf)                              \
  HANDLE_VECTOR_N (nam, FIELD_VALUE (sizefield), code, dxf)

// violates duplicate keys
#define SUBCLASS(name) \
  PREFIX fprintf (dat->fh, "\"_subclass\": \"" #name "\",\n");

// FIXME: for KEY not the complete nam path, only the field.
// e.g. verts[rcount1].lines[rcount2].segparms
#define _REPEAT_N(times, nam, type, idx)                                      \
  if (_obj->nam)                                                              \
    {                                                                         \
      KEY (nam);                                                              \
      ARRAY;                                                                  \
    }                                                                         \
  if (_obj->nam)                                                              \
    for (rcount##idx = 0; rcount##idx < (BITCODE_BL)times; rcount##idx++)
#define REPEAT_N(times, nam, type) _REPEAT_N(times, nam, type, 1)
#define REPEAT_CN(times, nam, type) REPEAT_N(times, nam, type)
#define _REPEAT_C(times, nam, type, idx) _REPEAT_N (_obj->times, nam, type, idx)
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

#undef REPEAT_BLOCK
#define REPEAT_BLOCK                                                          \
  {                                                                           \
    HASH;
#undef END_REPEAT_BLOCK
#define END_REPEAT_BLOCK                                                      \
  NOCOMMA;                                                                    \
  ENDHASH;                                                                    \
  }
#undef END_REPEAT
#define END_REPEAT(nam)                                                       \
  if (_obj->nam)                                                              \
    {                                                                         \
      NOCOMMA;                                                                \
      ENDARRAY;                                                               \
    }

#define FIELD_NUM_INSERTS(num_inserts, type, dxf)                             \
  FIELD (num_inserts, type, dxf)

#define FIELD_XDATA(nam, size)                                                \
  error |= json_xdata (dat, _obj)

#define REACTORS(code)                                                        \
  if (dat->version >= R_13 && obj->tio.object->num_reactors                   \
      && obj->tio.object->reactors)                                           \
    {                                                                         \
      KEY (reactors);                                                         \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < obj->tio.object->num_reactors; vcount++)      \
        {                                                                     \
          PREFIX VALUE_HANDLE (obj->tio.object->reactors[vcount], reactors,   \
                               code, 330);                                    \
        }                                                                     \
      NOCOMMA;                                                                \
      ENDARRAY;                                                               \
    }
#define ENT_REACTORS(code)                                                    \
  if (dat->version >= R_13 && ent->num_reactors && ent->reactors)             \
    {                                                                         \
      KEY (reactors);                                                         \
      ARRAY;                                                                  \
      for (vcount = 0; vcount < ent->num_reactors; vcount++)                  \
        {                                                                     \
          PREFIX VALUE_HANDLE (ent->reactors[vcount], reactors, code, 330);   \
        }                                                                     \
      NOCOMMA;                                                                \
      ENDARRAY;                                                               \
    }

#define XDICOBJHANDLE(code)                                                   \
  if ((dat->version < R_2004 || obj->tio.object->xdic_missing_flag != 0)      \
      && (obj->tio.object->xdicobjhandle != NULL)                             \
      && (obj->tio.object->xdicobjhandle->handleref.value != 0))              \
    {                                                                         \
      KEY (xdicobjhandle);                                                    \
      VALUE_HANDLE (obj->tio.object->xdicobjhandle, xdicobjhandle, code, -3); \
    }
#define ENT_XDICOBJHANDLE(code)                                               \
  if ((dat->version < R_2004 || (ent->xdic_missing_flag != 0))                \
      && (ent->xdicobjhandle != NULL)                                         \
      && (ent->xdicobjhandle->handleref.value != 0))                          \
    {                                                                         \
      KEY (xdicobjhandle);                                                    \
      VALUE_HANDLE (ent->xdicobjhandle, xdicobjhandle, code, -3);             \
    }

#define COMMON_ENTITY_HANDLE_DATA
#define SECTION_STRING_STREAM
#define START_STRING_STREAM
#define END_STRING_STREAM
#define START_HANDLE_STREAM KEY (ownerhandle)

static void
_prefix (Bit_Chain *dat)
{
  for (int _i = 0; _i < dat->bit; _i++)
    {
      fprintf (dat->fh, "  ");
    }
}

#define DWG_ENTITY(token)                                                     \
  static int dwg_json_##token (Bit_Chain *restrict dat,                       \
                               Dwg_Object *restrict obj)                      \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    int error = 0;                                                            \
    Bit_Chain *str_dat = dat;                                                 \
    Bit_Chain *hdl_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    Dwg_Entity_##token *ent, *_obj;                                           \
    Dwg_Object_Entity *_ent;                                                  \
    const char *name = #token;                                                \
    LOG_INFO ("Entity " #token ":\n")                                         \
    _ent = obj->tio.entity;                                                   \
    _obj = ent = _ent->tio.token;                                             \
    if (*name == '_')                                                         \
      FIELD_TEXT (entity, &name[1])                                           \
    else                                                                      \
      FIELD_TEXT (entity, name)                                               \
    if (obj->dxfname && strNE (obj->dxfname, #token))                         \
      FIELD_TEXT (dxfname, obj->dxfname);                                     \
    _FIELD (index, RL, 0);                                                    \
    _FIELD (type, RL, 0);                                                     \
    KEY (handle);                                                             \
    VALUE_H (obj->handle, 5);                                                 \
    _FIELD (size, RL, 0);                                                     \
    _FIELD (bitsize, BL, 0);                                                  \
    if (_ent->preview_exists)                                                 \
      ENT_FIELD (preview_exists, B, 0);                                       \
    error |= json_common_entity_data (dat, obj);

#define DWG_ENTITY_END                                                        \
  return 0;                                                                   \
  }

#define DWG_OBJECT(token)                                                     \
  static int dwg_json_##token (Bit_Chain *restrict dat,                       \
                               Dwg_Object *restrict obj)                      \
  {                                                                           \
    BITCODE_BL vcount, rcount1, rcount2, rcount3, rcount4;                    \
    int error = 0;                                                            \
    Bit_Chain *hdl_dat = dat;                                                 \
    Dwg_Data *dwg = obj->parent;                                              \
    const char *name = #token;                                                \
    Dwg_Object_##token *_obj;                                                 \
    LOG_INFO ("Object "#token ":\n")                                          \
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
    _FIELD (bitsize, BL, 0);                                                  \
    error |= json_eed (dat, obj->tio.object);

#define DWG_OBJECT_END                                                        \
  return 0;                                                                   \
  }

static char* _path_field(const char *path)
{
  char *s = strrchr (path, ']');
  if (s && s[1] == '.')
    {
      return &s[2];
    }
  return (char*)path;
}

static int
json_eed (Bit_Chain *restrict dat,
          const Dwg_Object_Object *restrict obj)
{
  int error = 0;
  if (!obj->num_eed)
    return 0;
  KEY (eed);
  ARRAY;
  for (BITCODE_BL i = 0; i < obj->num_eed; i++)
    {
      const Dwg_Eed* _obj = &obj->eed[i];
      HASH;
      if (_obj->size)
        {
          KEY (size); VALUE_RS (_obj->size, 0);
          KEY (handle);
          fprintf (dat->fh, FORMAT_H ",\n", ARGS_H (_obj->handle));
        }
      if (_obj->data)
        {
          const Dwg_Eed_Data *data = _obj->data;
          KEY (code); VALUE_RC (data->code, 0);
          KEY (value);
          switch (data->code)
            {
            case 0:
              PRE (R_2007) {
                VALUE_TEXT (data->u.eed_0.string);
              }
              LATER_VERSIONS {
                VALUE_TEXT_TU (data->u.eed_0.string);
              }
              break;
            case 2: VALUE_RC (data->u.eed_2.byte, 0); break;
            case 3: VALUE_RL (data->u.eed_3.layer, 0); break;
            case 4: VALUE_BINARY (data->u.eed_4.data, data->u.eed_4.length, 0); break;
            case 5: VALUE_RLL (data->u.eed_5.entity, 0); break;
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
                    VALUE_3RD (data->u.eed_10.point, 0); break;
            case 40:
            case 41:
            case 42:
                    VALUE_RD (data->u.eed_40.real, 0); break;
            case 70: VALUE_RS (data->u.eed_70.rs, 0); break;
            case 71: VALUE_RL (data->u.eed_71.rl, 0); break;
            default: VALUE_RC (0, 0);
            }
        }
      NOCOMMA;
      ENDHASH;
    }
  NOCOMMA;
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
      enum RES_BUF_VALUE_TYPE type;
      PREFIX ARRAY;
      PREFIX VALUE_RS (rbuf->type, 0);
      PREFIX
      type = get_base_value_type (rbuf->type);
      switch (type)
        {
        case VT_STRING:
          PRE (R_2007) {
            VALUE_TEXT (rbuf->value.str.u.data);
          }
          LATER_VERSIONS {
            VALUE_TEXT_TU (rbuf->value.str.u.data);
          }
          LOG_TRACE ("xdata[%u]: \"%s\" [TV %d]\n", i, rbuf->value.str.u.data,
                     rbuf->type);
          break;
        case VT_BINARY:
          VALUE_BINARY (rbuf->value.str.u.data, rbuf->value.str.size, 0);
          LOG_TRACE ("xdata[%u]: \"%s\" [TF %d]\n", i, rbuf->value.str.u.data,
                     rbuf->type);
          break;
        case VT_REAL:
          VALUE_RD (rbuf->value.dbl, 0);
          LOG_TRACE ("xdata[%u]: %f [RD %d]\n", i, rbuf->value.dbl,
                     rbuf->type);
          break;
        case VT_BOOL:
        case VT_INT8:
          VALUE_RC (rbuf->value.i8, 0);
          LOG_TRACE ("xdata[%u]: %d [RC %d]\n", i, (int)rbuf->value.i8,
                     rbuf->type);
          break;
        case VT_INT16:
          VALUE_RS (rbuf->value.i16, 0);
          LOG_TRACE ("xdata[%u]: %d [RS %d]\n", i, (int)rbuf->value.i16,
                     rbuf->type);
          break;
        case VT_INT32:
          VALUE_RL (rbuf->value.i32, 0);
          LOG_TRACE ("xdata[%u]: %d [RL %d]\n", i, (int)rbuf->value.i32,
                     rbuf->type);
          break;
        case VT_INT64:
          VALUE_RLL (rbuf->value.i64, 0);
          LOG_TRACE ("xdata[%u]: %ld [RLL %d]\n", i, (long)rbuf->value.i64,
                     rbuf->type);
          break;
        case VT_POINT3D:
          fprintf (dat->fh, "[ " FORMAT_RD ", " FORMAT_RD ", " FORMAT_RD " ],\n",
                   rbuf->value.pt[0], rbuf->value.pt[1], rbuf->value.pt[2]);
          LOG_TRACE ("xdata[%u]: (%f,%f,%f) [3RD %d]\n", i, rbuf->value.pt[0],
                     rbuf->value.pt[1], rbuf->value.pt[2], rbuf->type);
          break;
        case VT_HANDLE:
        case VT_OBJECTID:
          fprintf (dat->fh, FORMAT_H ",\n", ARGS_H (rbuf->value.h));
          break;
        case VT_INVALID:
        default:
          break;
        }
      rbuf = rbuf->next;
      NOCOMMA;
      ENDARRAY;
    }
  NOCOMMA;
  ENDARRAY;
  return error;
}

static int
json_common_entity_data (Bit_Chain *restrict dat,
                         const Dwg_Object *restrict obj)
{
  Dwg_Object_Entity *ent, *_obj;
  // Dwg_Data *dwg = obj->parent;
  int error = 0;
  BITCODE_BL vcount = 0;
  ent = obj->tio.entity;
  _obj = ent;

  error |= json_eed (dat, (Dwg_Object_Object *)ent);

  // clang-format off
  #include "common_entity_handle_data.spec"
  #include "common_entity_data.spec"
  // clang-format on

  return error;
}

#include "dwg.spec"

static int ishex (int c)
{
  return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
          || (c >= 'A' && c <= 'F'));
}
// Usage: hex(c >> 4), hex(c & 0xf)
static int hex (unsigned char c)
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
      fprintf (dat->fh, "\"\",\n");
      return;
    }
  fprintf (dat->fh, "\"");
  while ((c = *ws++))
    {
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
      else if (c < 0x1f || c > 0xff)
        {
          // FIXME: handle surrogate pairs properly
          if (c >= 0xd800 && c < 0xdc00)
            {
              fprintf (dat->fh, "\\u%04x", c - 0x1000);
            }
          else if (c >= 0xdc00 && c < 0xe000)
            ;
          else
            fprintf (dat->fh, "\\u%04x", c);
        }
      else
        fprintf (dat->fh, "%c", (char)(c & 0xff));
    }
  fprintf (dat->fh, "\",\n");
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

char *
json_cquote (char *restrict dest, const char *restrict src, const int len)
{
  unsigned char c;
  unsigned char *s = (unsigned char *)src;
  const char* endp = dest + len;
  char *d = dest;
  while ((c = *s++))
    {
      if (dest >= endp)
        {
          *dest = 0;
          return d;
        }
      if (c == '"' && dest+1 < endp)
        {
          *dest++ = '\\';
          *dest++ = c;
        }
      else if (c == '\\' && dest+2 < endp)
        {
          if (dest+5 < endp && s[0] == 'U' && s[1] == '+' && ishex (s[2])
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
      else if (c == '\n' && dest+1 < endp)
        {
          *dest++ = '\\';
          *dest++ = 'n';
        }
      else if (c == '\r' && dest+1 < endp)
        {
          *dest++ = '\\';
          *dest++ = 'r';
        }
      else if (c < 0x1f && dest+5 < endp)
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
  return d;
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
  switch (obj->type)
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
    case DWG_TYPE_VPORT_ENTITY_CONTROL:
      return dwg_json_VPORT_ENTITY_CONTROL (dat, obj);
    case DWG_TYPE_VPORT_ENTITY_HEADER:
      return dwg_json_VPORT_ENTITY_HEADER (dat, obj);
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
    case DWG_TYPE_OLEFRAME:
      return dwg_json_OLEFRAME (dat, obj);
    case DWG_TYPE_VBA_PROJECT:
      LOG_ERROR ("Unhandled Object VBA_PROJECT. Has its own section\n");
      // dwg_json_VBA_PROJECT(dat, obj);
      break;
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
          int is_entity;
          int i = obj->type - 500;
          Dwg_Class *klass = NULL;
          int num_bytes = obj->num_unknown_bits / 8;
          if (obj->num_unknown_bits & 8)
            num_bytes++;

          if (i >= 0 && i < (int)dwg->num_classes)
            {
              klass = &dwg->dwg_class[i];
              is_entity = dwg_class_is_entity (klass);
            }
          // properly dwg_decode_object/_entity for eed, reactors, xdic
          if (klass && !is_entity)
            {
              error |= dwg_json_UNKNOWN_OBJ (dat, obj);
              KEY (num_unknown_bits);
              VALUE_RL (obj->num_unknown_bits, 0);
              KEY (unknown_bits);
              VALUE_BINARY (obj->unknown_bits, num_bytes, 0);
              return error;
            }
          else if (klass)
            {
              error |= dwg_json_UNKNOWN_ENT (dat, obj);
              KEY (num_unknown_bits);
              VALUE_RL (obj->num_unknown_bits, 0);
              KEY (unknown_bits);
              VALUE_BINARY (obj->unknown_bits, num_bytes, 0);
              return error;
            }
          else // not a class
            {
              goto invalid_type;
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
  struct Dwg_Header *_obj = &dwg->header;
  Dwg_Object *obj = NULL;
  int i;

  RECORD (FILEHEADER); // single hash
  KEY (version);
  fprintf (dat->fh, "\"%s\",\n", version_codes[dwg->header.version]);

  // clang-format off
  #include "header.spec"
  // clang-format on

  ENDRECORD ();
  return 0;
}

static int
json_header_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  // const int minimal = 0;
  char buf[4096];
  double ms;
  const char *codepage
      = (dwg->header.codepage == 30 || dwg->header.codepage == 0)
            ? "ANSI_1252"
            : (dwg->header.version >= R_2007) ? "UTF-8" : "ANSI_1252";

  RECORD (HEADER); // single hash
                   // clang-format off
  #include "header_variables.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_classes_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  SECTION (CLASSES); // list of classes
  for (i = 0; i < dwg->num_classes; i++)
    {
      Dwg_Class *_obj = &dwg->dwg_class[i];
      HASH;
      FIELD_BS (number, 0);
      FIELD_TV (dxfname, 1);
      FIELD_T (cppname, 2);
      FIELD_T (appname, 3);
      FIELD_BS (proxyflag, 90);
      FIELD_BL (num_instances, 91);
      FIELD_B (is_zombie, 280);
      FIELD_BS (item_class_id, 281);
      NOCOMMA;
      ENDHASH;
    }
  NOCOMMA;
  ENDSEC ();
  return 0;
}

static int
json_objects_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;

  SECTION (OBJECTS);
  for (i = 0; i < dwg->num_objects; i++)
    {
      int error;
      Dwg_Object *obj = &dwg->object[i];
      HASH;
      error = dwg_json_object (dat, obj);
      NOCOMMA;
      ENDHASH;
    }
  if (dwg->num_objects)
    NOCOMMA;
  ENDSEC ();
  return 0;
}

#if 0
/* The object map/handles section */
static int
json_handles_write (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL j;
  SECTION (HANDLES);
  for (j = 0; j < dwg->num_objects; j++)
    {
      Dwg_Object *obj = &dwg->object[j];
      // handle => abs. offset
      // TODO: The real HANDLES section omap has handleoffset (deleted holes) and addressoffset
      PREFIX;
      fprintf (dat->fh, "[ %lu, %lu ],\n", obj->handle.value, obj->address);
    }
  NOCOMMA;
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
      PREFIX fprintf (dat->fh, "\"size\": %lu,\n", _obj->size);
      FIELD_BINARY (chain, _obj->size, 310);
      if (dwg->header.from_version >= R_2004)
        _obj->chain -= 16; /* undo for free */
      NOCOMMA;
      ENDHASH;
    }
  return 0;
}

static int
json_section_r2004fileheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_R2004_Header *_obj = &dwg->r2004_header;
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
json_section_summary (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

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
  struct Dwg_VBAProject *_obj = &dwg->vbaproject;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

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
  struct Dwg_AppInfo *_obj = &dwg->appinfo;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

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
  struct Dwg_AppInfoHistory *_obj = &dwg->appinfohistory;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

  RECORD (AppInfoHistory); // single hash
  PREFIX fprintf (dat->fh, "\"size\": %d,\n", _obj->size);
  FIELD_BINARY (unknown_bits, _obj->size, 0)
  // clang-format off
  //#include "appinfohistory.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_filedeplist (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_FileDepList *_obj = &dwg->filedeplist;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL vcount, rcount1;

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
  struct Dwg_Security *_obj = &dwg->security;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

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
  struct Dwg_RevHistory *_obj = &dwg->revhistory;
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
  struct Dwg_ObjFreeSpace *_obj = &dwg->objfreespace;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

  RECORD (ObjFreeSpace); // single hash
  // clang-format off
  #include "objfreespace.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

static int
json_section_template (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct Dwg_Template *_obj = &dwg->template;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

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
  struct Dwg_AuxHeader *_obj = &dwg->auxheader;
  Dwg_Object *obj = NULL;
  int error = 0, i;
  BITCODE_RL vcount, rcount1;

  RECORD (AuxHeader); // single hash
  // clang-format off
  #include "auxheader.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}

#if 0
static int
json_section_2ndheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  struct _dwg_second_header *_obj = &dwg->second_header;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1;

  RECORD (SecondHeader); // single hash
  HASH;
  // clang-format off
  //#include "2ndheader.spec"
  // clang-format on
  ENDRECORD ();
  return 0;
}
#endif

EXPORT int
dwg_write_json (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  const int minimal = dwg->opts & DWG_OPTS_MINIMAL;
  struct Dwg_Header *obj = &dwg->header;
  int error = 0;

  fprintf (dat->fh,
           "{\n"
           "  \"created_by\": \"%s\",\n",
           PACKAGE_STRING);
  dat->bit++; // ident

  if (!minimal)
    {
      json_fileheader_write (dat, dwg);
    }

  // A minimal HEADER requires only $ACADVER, $HANDSEED, and then ENTITIES
  json_header_write (dat, dwg);

  if (!minimal && dat->version >= R_13)
    {
      if (json_classes_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }

  if (json_objects_write (dat, dwg) >= DWG_ERR_CRITICAL)
    goto fail;

  if (!minimal && dat->version >= R_13)
    {
      if (json_thumbnail_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
      /* the other sections */
      if (dat->version <= R_2000)
        {
          error |= json_section_template (dat, dwg); // i.e. MEASUREMENT
          error |= json_section_auxheader (dat, dwg);
          //error |= json_section_2ndheader (dat, dwg);
        }
      if (dat->version >= R_2004)
        {
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
          error |= json_section_template (dat, dwg);
        }
    }

#if 0
  /* object map */
  if (!minimal && dat->version >= R_13)
    {
      if (json_handles_write (dat, dwg) >= DWG_ERR_CRITICAL)
        goto fail;
    }
#endif

  NOCOMMA;
  dat->bit--;
  fprintf (dat->fh, "}\n");
  return 0;
fail:
  return 1;
}

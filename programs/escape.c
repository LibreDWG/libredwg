/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019,2023,2026 Free Software Foundation, Inc.              */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * escape.c: SVG helpers
 * written by Reini Urban
 */

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "escape.h"

#define APPEND_GROW(d, dest, end, len, s)                                \
      if ((d) + (sizeof(s) - 1) >= (end))                                \
        {                                                                \
          const int _off = (d) - (dest);                                 \
          char *_new                                                     \
              = (char *)realloc ((dest), (len) + (sizeof(s) - 1) + 10);  \
          if (!_new)                                                     \
            return NULL;                                                 \
          (dest) = _new;                                                 \
          (len) += (sizeof(s) - 1) + 10;                                 \
          (d) = (dest) + _off;                                           \
          *(d) = 0;                                                      \
          (end) = (dest) + (len);                                        \
        }                                                                \
      memcpy ((d), (s), sizeof(s) - 1);                                  \
      (d) += (sizeof(s) - 1);                                            \
      *(d) = 0

char *ATTRIBUTE_MALLOC
htmlescape (const char *restrict src, const Dwg_Codepage cp)
{
  size_t len;
  char *dest, *d, *end;
  unsigned char *s;
  if (!src)
    return NULL;
  len = strlen (src) + 10;
  d = (char *)calloc (len, 1);
  if (!d)
    return NULL;
  s = (unsigned char *)src;
  dest = d;
  end = dest + len;
  while (*s)
    {
      switch (*s)
        {
        case '"':
          APPEND_GROW (d, dest, end, len, "&quot;");
          break;
        case '\'':
          APPEND_GROW (d, dest, end, len, "&#39;");
          break;
        case '`':
          APPEND_GROW (d, dest, end, len, "&#96;");
          break;
        case '&':
          APPEND_GROW (d, dest, end, len, "&amp;");
          break;
        case '<':
          APPEND_GROW (d, dest, end, len, "&lt;");
          break;
        case '>':
          APPEND_GROW (d, dest, end, len, "&gt;");
          break;
        case '{':
          APPEND_GROW (d, dest, end, len, "&#123;");
          break;
        case '}':
          APPEND_GROW (d, dest, end, len, "&#125;");
          break;
        default:
          {
            uint16_t cc = *s;
            wchar_t wc;
            if (end - d <= 16)
              {
                const int _off = d - dest;
                char *_new = (char *)realloc (dest, len + 16);
                if (!_new)
                  return NULL;
                dest = _new;
                len += 16;
                d = dest + _off;
                end = dest + len;
              }
            if (dwg_codepage_is_twobyte (cp, *s))
              cc = cc << 8 | *++s;
            wc = dwg_codepage_uwc (cp, cc);
            if (wc > 127 || wc < 0x20)
              {
                if (!d)
                  return NULL;
                snprintf (d, (size_t)(end - d), "&#x%X;",
                          (unsigned)wc); // 4 + 4
                d += strlen (d);
              }
            else
              {
                *d++ = *s;
              }
            *d = 0;
          }
        }
      s++;
    }
  *d = 0;
  return dest;
}

char *ATTRIBUTE_MALLOC
htmlwescape (BITCODE_TU wstr)
{
  int len = 0;
  char *dest, *d, *end;
  BITCODE_TU tmp = wstr;
  BITCODE_RS c;

  if (!wstr)
    return NULL;
  while ((c = *tmp++))
    len++;
  len += 16;
  d = dest = (char *)calloc (len, 1);
  if (!d)
    return NULL;
  end = dest + len;

  while (*wstr)
    {
      switch (*wstr)
        {
        case 34: // '"'
          APPEND_GROW (d, dest, end, len, "&quot;");
          break;
        case 39: // '\''
          APPEND_GROW (d, dest, end, len, "&#39;");
          break;
        case 38: // '&'
          APPEND_GROW (d, dest, end, len, "&amp;");
          break;
        case 60: // '<'
          APPEND_GROW (d, dest, end, len, "&lt;");
          break;
        case 62: // '>'
          APPEND_GROW (d, dest, end, len, "&gt;");
          break;
        case 96: // '`'
          APPEND_GROW (d, dest, end, len, "&#96;");
          break;
        case 123: // '{'
          APPEND_GROW (d, dest, end, len, "&#123;");
          break;
        case 125: // '}'
          APPEND_GROW (d, dest, end, len, "&#125;");
          break;
        default:
          if (end - d <= 16)
            {
              const int _off = d - dest;
              char *_new = (char *)realloc (dest, len + 16);
              if (!_new)
                return NULL;
              dest = _new;
              len += 16;
              d = dest + _off;
              end = dest + len;
            }
          if (*wstr >= 127 || *wstr < 20) // utf8 encodings
            {
              if (!d)
                return NULL;
              snprintf (d, (size_t)(end - d), "&#x%X;", *wstr);
              d += strlen (d);
              *d = 0;
            }
          else
            {
              *d++ = *wstr;
              *d = 0;
            }
        }
      wstr++;
    }
  *d = 0;
  return dest;
}

#undef APPEND_GROW

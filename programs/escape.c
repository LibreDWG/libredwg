/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019,2023 Free Software Foundation, Inc.                   */
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

#define SAFE_APPEND(d, end, s)                       \
  do {                                               \
    if ((d) + (sizeof (s) - 1) >= (end)) goto done;  \
    memcpy ((d), s, sizeof (s) - 1);                \
    (d) += sizeof (s) - 1;                           \
  } while (0)

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
      if (end - d <= 8)
        {
          const int off = d - dest;
          char *newdest = (char *)realloc (dest, len + 10);
          if (!newdest)
            return NULL;
          dest = newdest;
          len += 10;
          d = dest + off;
          *d = 0;
          end = dest + len;
        }
      switch (*s)
        {
        case '"':  SAFE_APPEND (d, end, "&quot;"); break;
        case '\'': SAFE_APPEND (d, end, "&#39;");  break;
        case '`':  SAFE_APPEND (d, end, "&#96;");  break;
        case '&':  SAFE_APPEND (d, end, "&amp;");  break;
        case '<':  SAFE_APPEND (d, end, "&lt;");   break;
        case '>':  SAFE_APPEND (d, end, "&gt;");   break;
        case '{':  SAFE_APPEND (d, end, "&#123;"); break;
        case '}':  SAFE_APPEND (d, end, "&#125;"); break;
        default:
          {
            uint16_t cc = *s;
            wchar_t wc;
            if (dwg_codepage_is_twobyte (cp, *s))
              cc = cc << 8 | *++s;
            wc = dwg_codepage_uwc (cp, cc);
            if (wc > 127 || wc < 0x20)
              {
                if (!d)
                  return NULL;
                snprintf (d, (size_t)(end - d), "&#x%X;", (unsigned)wc); // 4 + 4
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
done:
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
      const int off = d - dest;
      if (off >= len - 8)
        {
          char *newdest = (char *)realloc (dest, len + 16);
          if (!newdest)
            return NULL;
          dest = newdest;
          len += 16;
          d = dest + off;
          *d = 0;
          end = dest + len;
        }
      switch (*wstr)
        {
        case 34:  SAFE_APPEND (d, end, "&quot;"); break;
        case 39:  SAFE_APPEND (d, end, "&#39;");  break;
        case 38:  SAFE_APPEND (d, end, "&amp;");  break;
        case 60:  SAFE_APPEND (d, end, "&lt;");   break;
        case 62:  SAFE_APPEND (d, end, "&gt;");   break;
        case 96:  SAFE_APPEND (d, end, "&#96;");  break;
        case 123: SAFE_APPEND (d, end, "&#123;"); break;
        case 125: SAFE_APPEND (d, end, "&#125;"); break;
        default:
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
done:
  *d = 0;
  return dest;
}

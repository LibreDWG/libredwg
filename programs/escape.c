/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2019 Free Software Foundation, Inc.                        */
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

char * ATTRIBUTE_MALLOC
htmlescape (const char *restrict src, const int cp)
{
  int len = strlen (src) + 10;
  char *dest, *d = malloc (len);
  unsigned char *s = (unsigned char *)src;
  dest = d;
  while (*s++)
    {
      const int off = d - dest;
      if (off >= len - 8)
        {
          len += 10;
          dest = realloc (dest, len);
          d = dest + off;
        }
      switch (*s)
        {
        case '"': strcat (d, "&quot;"); d += 6; break;
        case '\'': strcat (d, "&#39;"); d += 5; break;
        case '`': strcat (d, "&#96;"); d += 5; break;
        case '&': strcat (d, "&amp;"); d += 5; break;
        case '<': strcat (d, "&lt;"); d += 4; break;
        case '>': strcat (d, "&gt;"); d += 4; break;
        case '{': strcat (d, "&#123;"); d += 6; break;
        case '}': strcat (d, "&#125;"); d += 6; break;
        default:
          if (*s >= 127) // maybe encodings, no utf8 (see htmlwescape)
            {
              sprintf (d, "&#x%X;", *s); // 4 + 4
              d += strlen (d);
            }
          else if (*s >= 20)
            *d++ = *s;
        }
    }
  *d = 0;
  return dest;
}

char * ATTRIBUTE_MALLOC
htmlwescape (BITCODE_TU wstr)
{
  int len = 0;
  char *dest, *d;
  BITCODE_TU tmp = wstr;
  BITCODE_RS c;

  if (!wstr)
    return NULL;
  while ((c = *tmp++))
    len++;
  len += 16;
  d = dest = malloc (len);

  while (*wstr++)
    {
      const int off = d - dest;
      if (off >= len - 8)
        {
          len += 16;
          dest = realloc (dest, len);
          d = dest + off;
        }
      switch (*wstr)
        {
        case 34: strcat (d, "&quot;"); d += 6; break;
        case 39: strcat (d, "&#39;"); d += 5; break;
        case 38: strcat (d, "&amp;"); d += 5; break;
        case 60: strcat (d, "&lt;"); d += 4; break;
        case 62: strcat (d, "&gt;"); d += 4; break;
        case 96: strcat (d, "&#96;"); d += 5; break;
        case 123: strcat (d, "&#123;"); d += 6; break;
        case 125: strcat (d, "&#125;"); d += 6; break;
        default:
          if (*wstr >= 127) // utf8 encodings
            {
              sprintf (d, "&#x%X;", *wstr);
              d += strlen (d);
            }
          else if (*wstr >= 20)
            *d++ = *wstr;
        }
    }
  *d = 0;
  return dest;
}

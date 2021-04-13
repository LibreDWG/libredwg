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
#include <ctype.h>
#include <math.h>

#include "common.h"
#include "escape.h"

#define APPEND_GROW(d, dest, end, len, s)                                \
      if ((d) + (sizeof(s) - 1) >= (end))                                \
        {                                                                \
          const int _off = (d) - (dest);                                 \
          char *_new                                                     \
              = (char *)REALLOC ((dest), (len) + (sizeof(s) - 1) + 10);  \
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
  d = (char *)CALLOC (len, 1);
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
                char *_new = (char *)REALLOC (dest, len + 16);
                if (!_new)
                  return NULL;
                dest = _new;
                len += 16;
                d = dest + _off;
                end = dest + len;
              }
            // a lead byte without a trail byte would consume the
            // terminator and scan past the end of src
            if (dwg_codepage_is_twobyte (cp, *s) && s[1])
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
  d = dest = (char *)CALLOC (len, 1);
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
              char *_new = (char *)REALLOC (dest, len + 16);
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

char *ATTRIBUTE_MALLOC
htmlutf8escape (const char *restrict utf8)
{
  size_t len;
  char *dest, *d, *end;
  const unsigned char *s;
  if (!utf8)
    return NULL;
  len = strlen (utf8) * 2 + 16;
  d = (char *)calloc (len, 1);
  if (!d)
    return NULL;
  s = (const unsigned char *)utf8;
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
        case '&':
          APPEND_GROW (d, dest, end, len, "&amp;");
          break;
        case '<':
          APPEND_GROW (d, dest, end, len, "&lt;");
          break;
        case '>':
          APPEND_GROW (d, dest, end, len, "&gt;");
          break;
        default:
          {
            if (*s < 0x20 && *s != 0x09 && *s != 0x0A && *s != 0x0D)
              {
                APPEND_GROW (d, dest, end, len, "&#xFFFD;");
                break;
              }
            if (end - d <= 4)
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
            *d++ = *s;
            *d = 0;
          }
        }
      s++;
    }
  *d = 0;
  return dest;
}

// Flatten MTEXT formatting controls to a plain-text intermediate
// representation, preserving \U+XXXX escapes for later expansion.
char *ATTRIBUTE_MALLOC
mtext_plaintext (const char *restrict src)
{
  size_t len;
  char *dest, *d, *end;
  const char *s;
  if (!src)
    return NULL;
  len = strlen (src) + 10;
  d = (char *)calloc (len, 1);
  if (!d)
    return NULL;
  s = src;
  dest = d;
  end = dest + len;
  while (*s)
    {
      if (end - d <= 8)
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
      if (*s == '\\' && s[1])
        {
          s++;
          switch (*s)
            {
            case 'P':
              *d++ = '\n';
              break;
            case '~':
              *d++ = ' ';
              break;
            case 'U':
              if (s[1] == '+' && isxdigit ((unsigned char)s[2])
                  && isxdigit ((unsigned char)s[3])
                  && isxdigit ((unsigned char)s[4])
                  && isxdigit ((unsigned char)s[5]))
                {
                  d[0] = '\\';
                  d[1] = 'U';
                  d[2] = '+';
                  d[3] = s[2];
                  d[4] = s[3];
                  d[5] = s[4];
                  d[6] = s[5];
                  d += 7;
                  s += 5;
                }
              else
                {
                  *d++ = '\\';
                  *d++ = 'U';
                }
              break;
            case 'L':
            case 'l':
            case 'O':
            case 'o':
            case 'K':
            case 'k':
            case 'X':
              // toggle codes - emit nothing
              break;
            case 'S':
            case 'A':
            case 'C':
            case 'H':
            case 'Q':
            case 'T':
            case 'W':
            case 'F':
            case 'p':
              // skip to ;
              while (*s && *s != ';')
                s++;
              break;
            default:
              *d++ = *s;
              break;
            }
          // an unterminated code stops at the NUL: do not step past it
          if (*s)
            s++;
          *d = 0;
          continue;
        }
      *d++ = *s;
      *d = 0;
      s++;
    }
  *d = 0;
  return dest;
}

// Expand \U+XXXX escapes and XML-escape a single line of MTEXT.
char *ATTRIBUTE_MALLOC
mtext_escape_line (const char *restrict line)
{
  size_t len;
  char *dest, *d, *end;
  const char *s;
  if (!line)
    return NULL;
  len = strlen (line) * 2 + 16;
  d = (char *)calloc (len, 1);
  if (!d)
    return NULL;
  s = line;
  dest = d;
  end = dest + len;
  while (*s)
    {
      if (*s == '\\' && s[1] == 'U' && s[2] == '+'
          && isxdigit ((unsigned char)s[3]) && isxdigit ((unsigned char)s[4])
          && isxdigit ((unsigned char)s[5]) && isxdigit ((unsigned char)s[6]))
        {
          unsigned int cp;
          if (sscanf (s + 3, "%4x", &cp) == 1)
            {
              char utf8[4];
              int n = 0;
              int i;
              if (cp < 0x80)
                utf8[n++] = (char)cp;
              else if (cp < 0x800)
                {
                  utf8[n++] = (char)(0xC0 | (cp >> 6));
                  utf8[n++] = (char)(0x80 | (cp & 0x3F));
                }
              else
                {
                  utf8[n++] = (char)(0xE0 | (cp >> 12));
                  utf8[n++] = (char)(0x80 | ((cp >> 6) & 0x3F));
                  utf8[n++] = (char)(0x80 | (cp & 0x3F));
                }
              for (i = 0; i < n; i++)
                {
                  if (end - d <= 4)
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
                  *d++ = utf8[i];
                }
              *d = 0;
              s += 7;
              continue;
            }
        }
      switch (*s)
        {
        case '"':
          APPEND_GROW (d, dest, end, len, "&quot;");
          break;
        case '\'':
          APPEND_GROW (d, dest, end, len, "&#39;");
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
        default:
          {
            if (*s < 0x20 && *s != 0x09 && *s != 0x0A && *s != 0x0D)
              {
                APPEND_GROW (d, dest, end, len, "&#xFFFD;");
                break;
              }
            if (end - d <= 4)
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
            *d++ = *s;
            *d = 0;
          }
        }
      s++;
    }
  *d = 0;
  return dest;
}

// Word-wrap to the MTEXT rectangle width using an approximate character
// advance (no font metrics). Sets *num_lines to the number of lines.
char *ATTRIBUTE_MALLOC
mtext_wrap_text (const char *restrict text, double width, double text_height,
                 double *num_lines)
{
  size_t len;
  char *dest, *d, *end;
  const char *s;
  double line_width;
  double char_advance;
  int cur_len;
  int line_count;
  if (!text)
    return NULL;
  char_advance = text_height * 0.6;
  if (char_advance <= 0.0)
    char_advance = 1.0;
  if (width <= 0.0)
    width = 1e9; // no wrapping
  len = strlen (text) + 16;
  d = (char *)calloc (len, 1);
  if (!d)
    return NULL;
  s = text;
  dest = d;
  end = dest + len;
  line_width = 0.0;
  cur_len = 0;
  line_count = 1;
  while (*s)
    {
      if (end - d <= 8)
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
      if (*s == '\n')
        {
          *d++ = '\n';
          *d = 0;
          line_width = 0.0;
          cur_len = 0;
          line_count++;
          s++;
          continue;
        }
      if (line_width + char_advance > width && cur_len > 0)
        {
          *d++ = '\n';
          *d = 0;
          line_width = 0.0;
          cur_len = 0;
          line_count++;
        }
      *d++ = *s;
      *d = 0;
      line_width += char_advance;
      cur_len++;
      s++;
    }
  *d = 0;
  if (num_lines)
    *num_lines = (double)line_count;
  return dest;
}

// Return the SVG text-anchor for the given MTEXT attachment point (1-9).
const char *
mtext_attachment_anchor (int attachment)
{
  switch (attachment)
    {
    case 1:
    case 4:
    case 7:
      return "start";
    case 2:
    case 5:
    case 8:
      return "middle";
    case 3:
    case 6:
    case 9:
      return "end";
    default:
      return "start";
    }
}

// Return the vertical offset from the insertion point to the first line.
double
mtext_attachment_first_offset (int attachment, double line_height,
                               double num_lines)
{
  double total_height = line_height * num_lines;
  switch (attachment)
    {
    case 1:
    case 2:
    case 3:
      return 0.0;
    case 4:
    case 5:
    case 6:
      return total_height / 2.0;
    case 7:
    case 8:
    case 9:
      return total_height;
    default:
      return 0.0;
    }
}

// Return the line pitch (distance between lines).
double
mtext_line_height (double text_height, int linespace_style,
                   double linespace_factor)
{
  if (linespace_style == 1 && linespace_factor > 0.0)
    return text_height * linespace_factor;
  return text_height * 1.6; // 3-on-5 default spacing
}

// Return the planar rotation (in degrees) from the XY projection of
// x_axis_dir, with its sign flipped for SVG's downward Y axis.
double
mtext_svg_angle (const BITCODE_3BD x_axis_dir)
{
  double angle;
  if (x_axis_dir.x == 0.0 && x_axis_dir.y == 0.0)
    return 0.0;
  angle = atan2 (x_axis_dir.y, x_axis_dir.x) * 180.0 / M_PI;
  return -angle;
}

#undef APPEND_GROW

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
#include <stdint.h>
#include <math.h>

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

/* Escape an already normalized UTF-8 string for use as XML text.  Invalid
   UTF-8 and code points which XML cannot represent are replaced, rather than
   copied as invalid output. */
char *ATTRIBUTE_MALLOC
htmlutf8escape (const char *restrict src)
{
  const unsigned char *s;
  char *dest;
  size_t cap;
  size_t used;

  if (!src)
    return NULL;
  if (strlen (src) > SIZE_MAX - 16)
    return NULL;
  cap = strlen (src) + 16;
  dest = (char *)malloc (cap);
  if (!dest)
    return NULL;
  used = 0;
  s = (const unsigned char *)src;
  while (*s)
    {
      uint32_t cp;
      size_t n;
      size_t avail;
      int valid;

      cp = *s;
      n = 1;
      valid = 1;
      avail = strnlen ((const char *)s, 4);
      if (cp >= 0xC2 && cp <= 0xDF && avail >= 2)
        {
          cp = ((uint32_t)(s[0] & 0x1F) << 6) | (s[1] & 0x3F);
          n = 2;
          if (s[1] < 0x80 || s[1] > 0xBF)
            valid = 0;
        }
      else if (cp >= 0xE0 && cp <= 0xEF && avail >= 3)
        {
          cp = ((uint32_t)(s[0] & 0x0F) << 12)
               | ((uint32_t)(s[1] & 0x3F) << 6) | (s[2] & 0x3F);
          n = 3;
          if (s[1] < 0x80 || s[1] > 0xBF || s[2] < 0x80 || s[2] > 0xBF
              || (s[0] == 0xE0 && s[1] < 0xA0)
              || (s[0] == 0xED && s[1] > 0x9F))
            valid = 0;
        }
      else if (cp >= 0xF0 && cp <= 0xF4 && avail >= 4)
        {
          cp = ((uint32_t)(s[0] & 0x07) << 18)
               | ((uint32_t)(s[1] & 0x3F) << 12)
               | ((uint32_t)(s[2] & 0x3F) << 6) | (s[3] & 0x3F);
          n = 4;
          if (s[1] < 0x80 || s[1] > 0xBF || s[2] < 0x80 || s[2] > 0xBF
              || s[3] < 0x80 || s[3] > 0xBF
              || (s[0] == 0xF0 && s[1] < 0x90)
              || (s[0] == 0xF4 && s[1] > 0x8F))
            valid = 0;
        }
      else if (cp >= 0x80)
        valid = 0;
      if ((cp >= 0xC2 && cp <= 0xDF && avail < 2)
          || (cp >= 0xE0 && cp <= 0xEF && avail < 3)
          || (cp >= 0xF0 && cp <= 0xF4 && avail < 4))
        valid = 0;

      if (!valid || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)
          || (cp < 0x20 && cp != 0x09 && cp != 0x0A && cp != 0x0D)
          || cp == 0xFFFE || cp == 0xFFFF)
        {
          cp = 0xFFFD;
          n = 1;
          valid = 0;
        }

      if (cp < 0x80)
        {
          const char *replacement;
          char one;

          replacement = NULL;
          switch (cp)
            {
            case '"': replacement = "&quot;"; break;
            case '\'': replacement = "&#39;"; break;
            case '`': replacement = "&#96;"; break;
            case '&': replacement = "&amp;"; break;
            case '<': replacement = "&lt;"; break;
            case '>': replacement = "&gt;"; break;
            case '{': replacement = "&#123;"; break;
            case '}': replacement = "&#125;"; break;
            default: break;
            }
          if (replacement)
            {
              size_t len = strlen (replacement);
              if (used + len + 1 > cap)
                {
                  size_t new_cap = cap;
                  char *new_dest;
                  while (used + len + 1 > new_cap)
                    {
                      if (new_cap > SIZE_MAX / 2)
                        {
                          free (dest);
                          return NULL;
                        }
                      new_cap *= 2;
                    }
                  new_dest = (char *)realloc (dest, new_cap);
                  if (!new_dest)
                    {
                      free (dest);
                      return NULL;
                    }
                  dest = new_dest;
                  cap = new_cap;
                }
              memcpy (dest + used, replacement, len);
              used += len;
            }
          else
            {
              one = (char)cp;
              if (used + 2 > cap)
                {
                  char *new_dest;
                  if (cap > SIZE_MAX / 2)
                    {
                      free (dest);
                      return NULL;
                    }
                  new_dest = (char *)realloc (dest, cap * 2);
                  if (!new_dest)
                    {
                      free (dest);
                      return NULL;
                    }
                  dest = new_dest;
                  cap *= 2;
                }
              dest[used++] = one;
            }
        }
      else if (cp == 0xFFFD && (!valid || n == 1))
        {
          static const char replacement[] = "&#xFFFD;";
          size_t len = sizeof (replacement) - 1;
          if (used + len + 1 > cap)
            {
              size_t new_cap;
              char *new_dest;
              if (cap > SIZE_MAX / 2)
                {
                  free (dest);
                  return NULL;
                }
              new_cap = cap * 2;
              new_dest = (char *)realloc (dest, new_cap);
              if (!new_dest)
                {
                  free (dest);
                  return NULL;
                }
              dest = new_dest;
              cap = new_cap;
            }
          memcpy (dest + used, replacement, len);
          used += len;
        }
      else
        {
          if (used + n + 1 > cap)
            {
              size_t new_cap = cap;
              char *new_dest;
              while (used + n + 1 > new_cap)
                {
                  if (new_cap > SIZE_MAX / 2)
                    {
                      free (dest);
                      return NULL;
                    }
                  new_cap *= 2;
                }
              new_dest = (char *)realloc (dest, new_cap);
              if (!new_dest)
                {
                  free (dest);
                  return NULL;
                }
              dest = new_dest;
              cap = new_cap;
            }
          memcpy (dest + used, s, n);
          used += n;
        }
      s += n;
    }
  dest[used] = '\0';
  return dest;
}

static int
mtext_hex_value (char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

static bool
mtext_param_end (const char *src, const char **end)
{
  const char *p;

  for (p = src; *p; p++)
    {
      if (*p == ';')
        {
          *end = p;
          return true;
        }
      if (*p == '\\' || *p == '{' || *p == '}' || *p == '\n'
          || *p == '\r')
        return false;
    }
  return false;
}

static bool
mtext_numeric_param (const char *src, const char *end, bool height)
{
  const char *p;
  bool digits;

  if (src == end)
    return false;
  p = src;
  if (*p == '+' || *p == '-')
    p++;
  digits = false;
  while (p < end && *p >= '0' && *p <= '9')
    {
      digits = true;
      p++;
    }
  if (p < end && *p == '.')
    {
      p++;
      while (p < end && *p >= '0' && *p <= '9')
        {
          digits = true;
          p++;
        }
    }
  if (!digits)
    return false;
  return p == end || (height && p + 1 == end && (*p == 'x' || *p == 'X'));
}

static bool
mtext_paragraph_param (const char *src, const char *end)
{
  const char *p;
  bool property;

  if (src == end)
    return false;
  p = src;
  if (*p == 'x')
    p++;
  property = false;
  while (p < end)
    {
      if (*p == 'i' || *p == 'l' || *p == 'r' || *p == 'q' || *p == 't')
        {
          property = true;
          p++;
        }
      else if ((*p >= '0' && *p <= '9') || *p == '+' || *p == '-'
               || *p == '.' || *p == ',')
        p++;
      else
        return false;
    }
  return property;
}

static bool
mtext_stacked_end (const char *src, const char **end)
{
  const char *p;
  bool delimiter;

  delimiter = false;
  for (p = src; *p; p++)
    {
      if (*p == '\\' && p[1])
        {
          p++;
          continue;
        }
      if (*p == ';')
        {
          if (p > src && delimiter)
            {
              *end = p;
              return true;
            }
          return false;
        }
      if (*p == '/' || *p == '#' || *p == '^')
        delimiter = true;
      if (*p == '\n' || *p == '\r' || *p == '{' || *p == '}')
        return false;
    }
  return false;
}

/* Flatten MTEXT controls to UTF-8 text.  Rich formatting is deliberately
   ignored here; malformed controls consume only their introducer/code so
   that following ordinary text remains visible. */
char *ATTRIBUTE_MALLOC
mtext_plaintext (const char *src)
{
  const char *s;
  const char *end;
  char *dest;
  char *d;
  size_t len;

  if (!src)
    return NULL;
  len = strlen (src);
  if (len == SIZE_MAX)
    return NULL;
  dest = (char *)malloc (len + 1);
  if (!dest)
    return NULL;
  d = dest;
  for (s = src; *s;)
    {
      char code;

      if (*s == '{' || *s == '}')
        {
          s++;
          continue;
        }
      if (*s != '\\')
        {
          *d++ = *s++;
          continue;
        }
      s++;
      if (!*s)
        break;
      code = *s++;
      switch (code)
        {
        case 'P':
          *d++ = '\n';
          break;
        case '\\':
          *d++ = '\\';
          break;
        case '~':
          *d++ = ' ';
          break;
        case '{':
        case '}':
        case ';':
          *d++ = code;
          break;
        case 'L':
        case 'l':
        case 'O':
        case 'o':
        case 'K':
        case 'k':
          break;
        case 'X':
        case 'x':
          *d++ = '\n';
          if (*s == ';')
            s++;
          break;
        case 'U':
          if (s[0] == '+' && s[1] && s[2] && s[3] && s[4]
              && mtext_hex_value (s[1]) >= 0 && mtext_hex_value (s[2]) >= 0
              && mtext_hex_value (s[3]) >= 0 && mtext_hex_value (s[4]) >= 0)
            {
              *d++ = '\\';
              *d++ = code;
              *d++ = *s++;
              *d++ = *s++;
              *d++ = *s++;
              *d++ = *s++;
              *d++ = *s++;
            }
          else
            *d++ = code;
          break;
        case 'S':
        case 's':
          if (mtext_stacked_end (s, &end))
            {
              while (s < end)
                {
                  if (end - s >= 7 && s[0] == '\\' && s[1] == 'U'
                      && s[2] == '+' && mtext_hex_value (s[3]) >= 0
                      && mtext_hex_value (s[4]) >= 0
                      && mtext_hex_value (s[5]) >= 0
                      && mtext_hex_value (s[6]) >= 0)
                    {
                      memcpy (d, s, 7);
                      d += 7;
                      s += 7;
                    }
                  else if (*s == '\\' && s + 1 < end)
                    {
                      *d++ = s[1];
                      s += 2;
                    }
                  else if (*s == '#' || *s == '^')
                    {
                      *d++ = '/';
                      if (*s == '^' && s + 1 < end && s[1] == ' ')
                        s++;
                      s++;
                    }
                  else
                    *d++ = *s++;
                }
              s = end + 1;
            }
          break;
        case 'A':
        case 'a':
        case 'C':
        case 'c':
        case 'H':
        case 'h':
        case 'Q':
        case 'q':
        case 'T':
        case 't':
        case 'W':
        case 'w':
          if (mtext_param_end (s, &end)
              && mtext_numeric_param (s, end, code == 'H' || code == 'h'))
            s = end + 1;
          break;
        case 'F':
        case 'f':
          if (mtext_param_end (s, &end) && s < end)
            s = end + 1;
          break;
        case 'p':
          if (mtext_param_end (s, &end) && mtext_paragraph_param (s, end))
            s = end + 1;
          break;
        default:
          /* Unknown controls have no visible syntax.  Preserve punctuation
             as text, but consume an unknown alphabetic control code. */
          if ((unsigned char)code < 'A' || (unsigned char)code > 'Z')
            if ((unsigned char)code < 'a' || (unsigned char)code > 'z')
              *d++ = code;
          break;
        }
    }
  *d = '\0';
  return dest;
}

static bool
mtext_append (char **dest, size_t *used, size_t *cap, const char *src)
{
  size_t len;
  size_t required;
  size_t new_cap;
  char *new_dest;

  len = strlen (src);
  if (*used == SIZE_MAX || len > SIZE_MAX - *used - 1)
    return false;
  required = *used + len + 1;
  if (required > *cap)
    {
      new_cap = *cap ? *cap : 32;
      while (required > new_cap)
        {
          if (new_cap > SIZE_MAX / 2)
            {
              new_cap = required;
              break;
            }
          new_cap *= 2;
        }
      new_dest = (char *)realloc (*dest, new_cap);
      if (!new_dest)
        return false;
      *dest = new_dest;
      *cap = new_cap;
    }
  memcpy (*dest + *used, src, len);
  *used += len;
  (*dest)[*used] = '\0';
  return true;
}

char *
mtext_escape_line (const char *line)
{
  const char *p;
  const char *u;
  const char *hex;
  char *part;
  char *escaped;
  char *dest;
  char unicode[32];
  size_t used;
  size_t cap;
  size_t part_len;
  int digits;
  int value;
  unsigned long codepoint;

  if (!line)
    return NULL;
  p = line;
  dest = NULL;
  used = 0;
  cap = 0;
  while ((u = strstr (p, "\\U+")))
    {
      hex = u + 3;
      codepoint = 0;
      digits = 0;
      for (; digits < 4; digits++)
        {
          if (!hex[digits])
            break;
          value = mtext_hex_value (hex[digits]);
          if (value < 0)
            break;
          codepoint = (codepoint << 4) | (unsigned)value;
        }
      if (digits < 4 || codepoint > 0x10FFFF
          || (codepoint >= 0xD800 && codepoint <= 0xDFFF)
          || (codepoint < 0x20 && codepoint != 0x09 && codepoint != 0x0A
              && codepoint != 0x0D)
          || codepoint == 0xFFFE || codepoint == 0xFFFF)
        {
          /* Keep malformed and non-XML Unicode controls as escaped text. */
          part_len = (size_t)(hex + (*hex ? 1 : 0) - p);
          if (part_len == SIZE_MAX)
            {
              free (dest);
              return NULL;
            }
          part = (char *)malloc (part_len + 1);
          if (!part)
            {
              free (dest);
              return NULL;
            }
          memcpy (part, p, part_len);
          part[part_len] = '\0';
          escaped = htmlutf8escape (part);
          free (part);
          if (escaped)
            {
              if (!mtext_append (&dest, &used, &cap, escaped))
                {
                  free (escaped);
                  free (dest);
                  return NULL;
                }
              free (escaped);
            }
          p = *hex ? hex + 1 : hex;
          continue;
        }
      part_len = (size_t)(u - p);
      if (part_len == SIZE_MAX)
        {
          free (dest);
          return NULL;
        }
      part = (char *)malloc (part_len + 1);
      if (!part)
        {
          free (dest);
          return NULL;
        }
      memcpy (part, p, part_len);
      part[part_len] = '\0';
      escaped = htmlutf8escape (part);
      free (part);
      if (escaped)
        {
          if (!mtext_append (&dest, &used, &cap, escaped))
            {
              free (escaped);
              free (dest);
              return NULL;
            }
          free (escaped);
        }
      snprintf (unicode, sizeof (unicode), "&#x%lX;", codepoint);
      if (!mtext_append (&dest, &used, &cap, unicode))
        {
          free (dest);
          return NULL;
        }
      p = hex + 4;
    }
  escaped = htmlutf8escape (p);
  if (escaped)
    {
      if (!mtext_append (&dest, &used, &cap, escaped))
        {
          free (escaped);
          free (dest);
          return NULL;
        }
      free (escaped);
    }
  return dest;
}

const char *
mtext_attachment_anchor (BITCODE_BS attachment)
{
  switch (attachment)
    {
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

double
mtext_svg_angle (double x_axis_x, double x_axis_y)
{
  double angle;

  if (!isfinite (x_axis_x) || !isfinite (x_axis_y)
      || (x_axis_x == 0.0 && x_axis_y == 0.0))
    return 0.0;
  angle = -atan2 (x_axis_y, x_axis_x) * 180.0 / M_PI;
  return isfinite (angle) ? angle : 0.0;
}

double
mtext_line_height (double text_height, double linespace_factor)
{
  double line_height;

  if (!isfinite (text_height) || text_height <= 0.0)
    return 0.0;
  line_height = text_height * (5.0 / 3.0);
  if (isfinite (linespace_factor) && linespace_factor > 0.0)
    line_height *= linespace_factor;
  if (!isfinite (line_height) || line_height <= 0.0)
    line_height = text_height * (5.0 / 3.0);
  return isfinite (line_height) && line_height > 0.0 ? line_height : 0.0;
}

double
mtext_attachment_first_offset (BITCODE_BS attachment, double text_height,
                               double line_height, unsigned int num_lines)
{
  double block_height;
  double ascent;

  if (!isfinite (text_height) || text_height <= 0.0)
    return 0.0;
  if (num_lines == 0)
    num_lines = 1;
  if (!isfinite (line_height) || line_height <= 0.0)
    line_height = text_height * (5.0 / 3.0);
  block_height = text_height
                 + line_height * (double)(num_lines - 1);
  if (!isfinite (block_height) || block_height <= 0.0)
    block_height = text_height;
  ascent = 0.8 * text_height;
  if (attachment >= 4 && attachment <= 6)
    return ascent - block_height / 2.0;
  if (attachment >= 7 && attachment <= 9)
    return ascent - block_height;
  return ascent;
}

/* Return the byte length of a visible MTEXT character.  \U+XXXX remains
   atomic here because mtext_escape_line() expands it after wrapping. */
static size_t
mtext_unit_len (const char *p, const char *end)
{
  size_t left;
  size_t n;

  left = (size_t)(end - p);
  if (left >= 7 && p[0] == '\\' && p[1] == 'U' && p[2] == '+'
      && mtext_hex_value (p[3]) >= 0 && mtext_hex_value (p[4]) >= 0
      && mtext_hex_value (p[5]) >= 0 && mtext_hex_value (p[6]) >= 0)
    return 7;
  if ((unsigned char)p[0] < 0x80)
    return 1;
  if ((unsigned char)p[0] >= 0xC2 && (unsigned char)p[0] <= 0xDF)
    n = 2;
  else if ((unsigned char)p[0] >= 0xE0 && (unsigned char)p[0] <= 0xEF)
    n = 3;
  else if ((unsigned char)p[0] >= 0xF0 && (unsigned char)p[0] <= 0xF4)
    n = 4;
  else
    return 1;
  if (left < n)
    return 1;
  while (--n)
    if ((p[n] & 0xC0) != 0x80)
      return 1;
  return (size_t)((unsigned char)p[0] < 0xE0
                      ? 2
                      : (unsigned char)p[0] < 0xF0 ? 3 : 4);
}

static bool
mtext_ascii_space (char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

static double
mtext_span_width (const char *p, const char *end, double advance)
{
  double width;

  width = 0.0;
  while (p < end)
    {
      width += advance;
      p += mtext_unit_len (p, end);
    }
  return width;
}

/* With no font metrics available to dwg2SVG, approximate every visible code
   point as 0.6 times its text height, scaled by the STYLE width factor. */
char *
mtext_wrap_text (const char *src, double rect_width, double text_height,
                 double width_factor)
{
  const char *end;
  const char *p;
  const char *word;
  const char *word_end;
  const char *space;
  const char *space_end;
  char *dest;
  char *d;
  double advance;
  double line_width;
  double word_width;
  double space_width;
  size_t len;

  if (!src)
    return NULL;
  len = strlen (src);
  if (len > (SIZE_MAX - 1) / 2)
    return NULL;
  dest = (char *)malloc (len * 2 + 1);
  if (!dest)
    return NULL;
  if (!isfinite (rect_width) || rect_width <= 0.0
      || !isfinite (text_height) || text_height <= 0.0
      || !isfinite (width_factor) || width_factor <= 0.0)
    {
      memcpy (dest, src, len + 1);
      return dest;
    }
  advance = 0.6 * text_height * width_factor;
  if (!isfinite (advance) || advance <= 0.0)
    {
      memcpy (dest, src, len + 1);
      return dest;
    }

  end = src + len;
  p = src;
  d = dest;
  line_width = 0.0;
  while (p < end)
    {
      if (*p == '\n')
        {
          *d++ = *p++;
          line_width = 0.0;
          continue;
        }
      space = p;
      while (p < end && mtext_ascii_space (*p))
        p++;
      space_end = p;
      word = p;
      while (p < end && *p != '\n' && !mtext_ascii_space (*p))
        p += mtext_unit_len (p, end);
      word_end = p;
      space_width = mtext_span_width (space, space_end, advance);
      word_width = mtext_span_width (word, word_end, advance);

      if (word < word_end && line_width > 0.0
          && line_width + space_width + word_width > rect_width)
        {
          *d++ = '\n';
          line_width = 0.0;
        }
      else
        {
          memcpy (d, space, (size_t)(space_end - space));
          d += space_end - space;
          line_width += space_width;
        }
      while (word < word_end)
        {
          size_t unit_len = mtext_unit_len (word, word_end);

          if (line_width > 0.0 && line_width + advance > rect_width)
            {
              *d++ = '\n';
              line_width = 0.0;
            }
          memcpy (d, word, unit_len);
          d += unit_len;
          word += unit_len;
          line_width += advance;
        }
    }
  *d = '\0';
  return dest;
}

#undef APPEND_GROW

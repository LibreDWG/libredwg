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

static bool
utf8_cont (unsigned char c)
{
  return (c & 0xC0) == 0x80;
}

/* Expected sequence length for a leading byte, or 0 if it cannot start one.
   0xC0/0xC1 would be overlong and 0xF5.. are out of range. */
static int
utf8_seq_len (unsigned char b0)
{
  if (b0 < 0x80)
    return 1;
  if (b0 < 0xC2)
    return 0;
  if (b0 < 0xE0)
    return 2;
  if (b0 < 0xF0)
    return 3;
  if (b0 < 0xF5)
    return 4;
  return 0;
}

/* Allowed range of the second byte, rejecting overlong forms, surrogates and
   code points beyond U+10FFFF. */
static bool
utf8_second_ok (unsigned char b0, unsigned char b1)
{
  unsigned char lo = 0x80;
  unsigned char hi = 0xBF;

  if (b0 == 0xE0)
    lo = 0xA0;
  else if (b0 == 0xED)
    hi = 0x9F;
  else if (b0 == 0xF0)
    lo = 0x90;
  else if (b0 == 0xF4)
    hi = 0x8F;
  return b1 >= lo && b1 <= hi;
}

static bool
utf8_tail_ok (const unsigned char *s, int len)
{
  int i;

  for (i = 2; i < len; i++)
    if (!utf8_cont (s[i]))
      return false;
  return true;
}

static uint32_t
utf8_decode (const unsigned char *s, int len)
{
  static const uint32_t lead_mask[5] = { 0x00, 0x7F, 0x1F, 0x0F, 0x07 };
  uint32_t cp;
  int i;

  cp = s[0] & lead_mask[len];
  for (i = 1; i < len; i++)
    cp = (cp << 6) | (s[i] & 0x3F);
  return cp;
}

/* Decode one UTF-8 sequence: *cp is the code point and *n its byte count.  On
   malformed input returns false with *cp set to the replacement character. */
static bool
utf8_next (const unsigned char *s, size_t avail, uint32_t *cp, size_t *n)
{
  int len;

  len = utf8_seq_len (s[0]);
  *n = 1;
  if (len == 0 || (size_t)len > avail)
    {
      *cp = 0xFFFD;
      return false;
    }
  if (len == 1)
    {
      *cp = s[0];
      return true;
    }
  if (!utf8_second_ok (s[0], s[1]))
    {
      *cp = 0xFFFD;
      return false;
    }
  if (len > 2 && !utf8_tail_ok (s, len))
    {
      *cp = 0xFFFD;
      return false;
    }
  *cp = utf8_decode (s, len);
  *n = (size_t)len;
  return true;
}

static const char *
html_entity (uint32_t cp)
{
  switch (cp)
    {
    case '"':
      return "&quot;";
    case '\'':
      return "&#39;";
    case '`':
      return "&#96;";
    case '&':
      return "&amp;";
    case '<':
      return "&lt;";
    case '>':
      return "&gt;";
    case '{':
      return "&#123;";
    case '}':
      return "&#125;";
    default:
      return NULL;
    }
}

static bool
str_reserve (char **dest, size_t *cap, size_t need)
{
  size_t new_cap;
  char *new_dest;

  if (need <= *cap)
    return true;
  new_cap = *cap ? *cap : 32;
  while (need > new_cap)
    {
      if (new_cap > SIZE_MAX / 2)
        {
          new_cap = need;
          break;
        }
      new_cap *= 2;
    }
  new_dest = (char *)realloc (*dest, new_cap);
  if (!new_dest)
    return false;
  *dest = new_dest;
  *cap = new_cap;
  return true;
}

static bool
html_append (char **dest, size_t *used, size_t *cap, const char *src,
             size_t len)
{
  if (!str_reserve (dest, cap, *used + len + 1))
    return false;
  memcpy (*dest + *used, src, len);
  *used += len;
  return true;
}

/* Code points XML text cannot carry: C0 controls except tab/LF/CR, surrogates
   and the two non-characters. */
static bool
xml_codepoint_forbidden (uint32_t cp)
{
  if (cp == 0xFFFE || cp == 0xFFFF)
    return true;
  if (cp < 0x20 && cp != 0x09 && cp != 0x0A && cp != 0x0D)
    return true;
  return cp >= 0xD800 && cp <= 0xDFFF;
}

static bool
xml_codepoint_usable (uint32_t cp, bool valid)
{
  if (!valid || cp > 0x10FFFF)
    return false;
  return !xml_codepoint_forbidden (cp);
}

/* Append one decoded code point, escaping the XML metacharacters. */
static bool
utf8_emit (char **dest, size_t *used, size_t *cap, uint32_t cp,
           const unsigned char *s, size_t n, bool valid)
{
  const char *replacement;
  char one;

  if (cp < 0x80)
    {
      replacement = html_entity (cp);
      if (replacement)
        return html_append (dest, used, cap, replacement,
                            strlen (replacement));
      one = (char)cp;
      return html_append (dest, used, cap, &one, 1);
    }
  if (cp == 0xFFFD && (!valid || n == 1))
    return html_append (dest, used, cap, "&#xFFFD;", sizeof ("&#xFFFD;") - 1);
  return html_append (dest, used, cap, (const char *)s, n);
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
      bool valid;

      avail = strnlen ((const char *)s, 4);
      valid = utf8_next (s, avail, &cp, &n);
      if (!xml_codepoint_usable (cp, valid))
        {
          cp = 0xFFFD;
          n = 1;
        }

      if (!utf8_emit (&dest, &used, &cap, cp, s, n, valid))
        {
          free (dest);
          return NULL;
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
      if (*p == '\\' || *p == '{' || *p == '}' || *p == '\n' || *p == '\r')
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
      /* Paragraph sub-codes: indents i/l/r, tab stops t, alignment q with
         l/c/r/j/d, line spacing s/m/a/e, plus the "*" wildcard. */
      if (*p == 'i' || *p == 'l' || *p == 'r' || *p == 'q' || *p == 't'
          || *p == 's' || *p == 'm' || *p == 'a' || *p == 'e' || *p == 'c'
          || *p == 'j' || *p == 'd')
        {
          property = true;
          p++;
        }
      else if ((*p >= '0' && *p <= '9') || *p == '+' || *p == '-' || *p == '.'
               || *p == ',' || *p == '*')
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

/* MTEXT control classification.  A lookup table keeps the dispatcher free of a
   large switch, so every helper stays simple. */
enum
{
  MTEXT_CTL_NONE = 0, /* unknown: keep non-alphabetic, drop alphabetic */
  MTEXT_CTL_COPY,     /* literal punctuation: \ { } ; */
  MTEXT_CTL_IGNORE,   /* formatting-only controls */
  MTEXT_CTL_NEWLINE,  /* \P and \X */
  MTEXT_CTL_NBSP,     /* \~ */
  MTEXT_CTL_UNICODE,  /* \U+XXXX */
  MTEXT_CTL_STACKED,  /* \S ... ; */
  MTEXT_CTL_PARAM     /* numeric/string/paragraph parameters */
};

static const unsigned char mtext_ctl_kind[256]
    = { ['\\'] = MTEXT_CTL_COPY,   ['{'] = MTEXT_CTL_COPY,
        ['}'] = MTEXT_CTL_COPY,    [';'] = MTEXT_CTL_COPY,
        ['L'] = MTEXT_CTL_IGNORE,  ['l'] = MTEXT_CTL_IGNORE,
        ['O'] = MTEXT_CTL_IGNORE,  ['o'] = MTEXT_CTL_IGNORE,
        ['K'] = MTEXT_CTL_IGNORE,  ['k'] = MTEXT_CTL_IGNORE,
        ['P'] = MTEXT_CTL_NEWLINE, ['X'] = MTEXT_CTL_NEWLINE,
        ['x'] = MTEXT_CTL_NEWLINE, ['~'] = MTEXT_CTL_NBSP,
        ['U'] = MTEXT_CTL_UNICODE, ['S'] = MTEXT_CTL_STACKED,
        ['s'] = MTEXT_CTL_STACKED, ['A'] = MTEXT_CTL_PARAM,
        ['a'] = MTEXT_CTL_PARAM,   ['C'] = MTEXT_CTL_PARAM,
        ['c'] = MTEXT_CTL_PARAM,   ['H'] = MTEXT_CTL_PARAM,
        ['h'] = MTEXT_CTL_PARAM,   ['Q'] = MTEXT_CTL_PARAM,
        ['q'] = MTEXT_CTL_PARAM,   ['T'] = MTEXT_CTL_PARAM,
        ['t'] = MTEXT_CTL_PARAM,   ['W'] = MTEXT_CTL_PARAM,
        ['w'] = MTEXT_CTL_PARAM,   ['F'] = MTEXT_CTL_PARAM,
        ['f'] = MTEXT_CTL_PARAM,   ['p'] = MTEXT_CTL_PARAM };

static bool
mtext_hex4_ok (const char *p)
{
  return p[0] && p[1] && p[2] && p[3] && mtext_hex_value (p[0]) >= 0
         && mtext_hex_value (p[1]) >= 0 && mtext_hex_value (p[2]) >= 0
         && mtext_hex_value (p[3]) >= 0;
}

static bool
mtext_is_alpha (char c)
{
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static bool
mtext_numeric_code (char c)
{
  return strchr ("AaCcHhQqTtWw", c) != NULL;
}

static bool
mtext_string_code (char c)
{
  return strchr ("Ff", c) != NULL;
}

static bool
mtext_unicode_at (const char *s, const char *end)
{
  return end - s >= 7 && s[0] == '\\' && s[1] == 'U' && s[2] == '+'
         && mtext_hex4_ok (s + 3);
}

static const char *
mtext_plain_copy (char **d, const char *s, char code)
{
  *(*d)++ = code;
  return s;
}

static const char *
mtext_plain_ignore (char **d, const char *s, char code)
{
  (void)d;
  (void)code;
  return s;
}

static const char *
mtext_plain_newline (char **d, const char *s, char code)
{
  *(*d)++ = '\n';
  if ((code == 'X' || code == 'x') && *s == ';')
    s++;
  return s;
}

static const char *
mtext_plain_nbsp (char **d, const char *s, char code)
{
  (void)code;
  *(*d)++ = (char)0xC2;
  *(*d)++ = (char)0xA0;
  return s;
}

static const char *
mtext_plain_unknown (char **d, const char *s, char code)
{
  if (!mtext_is_alpha (code))
    *(*d)++ = code;
  return s;
}

static const char *
mtext_plain_unicode (char **d, const char *s, char code)
{
  (void)code;
  if (s[0] == '+' && mtext_hex4_ok (s + 1))
    {
      *(*d)++ = '\\';
      *(*d)++ = 'U';
      memcpy (*d, s, 5);
      *d += 5;
      return s + 5;
    }
  *(*d)++ = 'U';
  return s;
}

static const char *
mtext_plain_stacked_unit (char **d, const char *s, const char *end)
{
  if (mtext_unicode_at (s, end))
    {
      memcpy (*d, s, 7);
      *d += 7;
      return s + 7;
    }
  if (*s == '\\' && s + 1 < end)
    {
      *(*d)++ = s[1];
      return s + 2;
    }
  if (*s == '#' || *s == '^')
    {
      *(*d)++ = '/';
      if (*s == '^' && s + 1 < end && s[1] == ' ')
        s++;
      return s + 1;
    }
  *(*d)++ = *s;
  return s + 1;
}

static const char *
mtext_plain_stacked (char **d, const char *s, char code)
{
  const char *end;

  (void)code;
  if (!mtext_stacked_end (s, &end))
    return s;
  while (s < end)
    s = mtext_plain_stacked_unit (d, s, end);
  return end + 1;
}

static const char *
mtext_plain_param (char **d, const char *s, char code)
{
  const char *end;

  (void)d;
  if (!mtext_param_end (s, &end))
    return s;
  if (code == 'p')
    {
      if (mtext_paragraph_param (s, end))
        return end + 1;
      return s;
    }
  if (mtext_numeric_code (code))
    {
      if (mtext_numeric_param (s, end, code == 'H' || code == 'h'))
        return end + 1;
      return s;
    }
  if (mtext_string_code (code) && s < end)
    return end + 1;
  return s;
}

typedef const char *(*mtext_plain_handler) (char **, const char *, char);

static const mtext_plain_handler mtext_plain_handlers[] = {
  mtext_plain_unknown, /* MTEXT_CTL_NONE */
  mtext_plain_copy,    /* MTEXT_CTL_COPY */
  mtext_plain_ignore,  /* MTEXT_CTL_IGNORE */
  mtext_plain_newline, /* MTEXT_CTL_NEWLINE */
  mtext_plain_nbsp,    /* MTEXT_CTL_NBSP */
  mtext_plain_unicode, /* MTEXT_CTL_UNICODE */
  mtext_plain_stacked, /* MTEXT_CTL_STACKED */
  mtext_plain_param    /* MTEXT_CTL_PARAM */
};

/* Consume one backslash control (s points at the code) and append its visible
   text.  Returns the next input position. */
static const char *
mtext_plain_control (char **d, const char *s)
{
  char code = *s++;
  unsigned char kind = mtext_ctl_kind[(unsigned char)code];

  return mtext_plain_handlers[kind](d, s, code);
}

/* Flatten MTEXT controls to UTF-8 text.  Rich formatting is deliberately
   ignored here; malformed controls consume only their introducer/code so
   that following ordinary text remains visible. */
char *ATTRIBUTE_MALLOC
mtext_plaintext (const char *src)
{
  const char *s;
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
      s = mtext_plain_control (&d, s);
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

char *ATTRIBUTE_MALLOC
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
  block_height = text_height + line_height * (double)(num_lines - 1);
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
  return (size_t)((unsigned char)p[0] < 0xE0   ? 2
                  : (unsigned char)p[0] < 0xF0 ? 3
                                               : 4);
}

static bool
mtext_ascii_space (char c)
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

static bool
mtext_nbsp (const char *p, const char *end)
{
  return end - p >= 2 && (unsigned char)p[0] == 0xC2
         && (unsigned char)p[1] == 0xA0;
}

static bool
mtext_nbsp_before (const char *p, const char *begin)
{
  return p - begin >= 2 && (unsigned char)p[-2] == 0xC2
         && (unsigned char)p[-1] == 0xA0;
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
char *ATTRIBUTE_MALLOC
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
  if (!isfinite (rect_width) || rect_width <= 0.0 || !isfinite (text_height)
      || text_height <= 0.0 || !isfinite (width_factor) || width_factor <= 0.0)
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
          && line_width + space_width + word_width > rect_width
          && !mtext_nbsp (word, word_end) && !mtext_nbsp_before (space, src))
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

          /* A non-breaking space must remain attached to both neighboring
             characters.  If either side would otherwise trigger a hard
             wrap, keep the unit on the current line and let it exceed the
             approximate width. */
          if (line_width > 0.0 && line_width + advance > rect_width
              && !mtext_nbsp (word, word_end)
              && !mtext_nbsp_before (word, src))
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

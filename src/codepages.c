/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2023-2025 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * codepages.c: preR2007 codepages support via iconv
 * written by Reini Urban
 *
 * See also the src mappings from https://www.unicode.org/Public/MAPPINGS/
 * or the libdxfrw/src/intern/drw_textcodec.cpp mappings.
 */

#include "config.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#if defined HAVE_ICONV && defined HAVE_ICONV_H
#  include <iconv.h>
#endif
#if defined HAVE_WCTYPE_H
#  include <wctype.h>
#endif
// #define CODEPAGES_C
#include "common.h"
#include "codepages.h"

#include "codepages/ISO-8859-2.h"
#include "codepages/ISO-8859-3.h"
#include "codepages/ISO-8859-4.h"
#include "codepages/ISO-8859-5.h"
#include "codepages/ISO-8859-6.h"
#include "codepages/ISO-8859-7.h"
#include "codepages/ISO-8859-8.h"
#include "codepages/ISO-8859-9.h"
#include "codepages/CP437.h"
#include "codepages/CP850.h"
#include "codepages/CP852.h"
#include "codepages/CP855.h"
#include "codepages/CP857.h"
#include "codepages/CP860.h"
#include "codepages/CP861.h"
#include "codepages/CP863.h"
#include "codepages/CP864.h"
#include "codepages/CP865.h"
#include "codepages/CP869.h"
#include "codepages/CP932.h"
#include "codepages/MACINTOSH.h"
#include "codepages/BIG5.h"
#include "codepages/CP949.h"
#include "codepages/JOHAB.h"
#include "codepages/CP866.h"
#include "codepages/WINDOWS-1250.h"
#include "codepages/WINDOWS-1251.h"
#include "codepages/WINDOWS-1252.h"
#include "codepages/GB2312.h"
#include "codepages/WINDOWS-1253.h"
#include "codepages/WINDOWS-1254.h"
#include "codepages/WINDOWS-1255.h"
#include "codepages/WINDOWS-1256.h"
#include "codepages/WINDOWS-1257.h"
#include "codepages/WINDOWS-874.h"
#include "codepages/WINDOWS-932.h"
#include "codepages/WINDOWS-936.h"
#include "codepages/WINDOWS-949.h"
#include "codepages/WINDOWS-950.h"
#include "codepages/WINDOWS-1361.h"
#include "codepages/WINDOWS-1258.h"

static const uint16_t *cp_fntbl[] = { NULL, // UTF8
                                      NULL, // US-ASCII
                                      NULL, // ISO-8859-1
                                      cptbl_iso_8859_2,
                                      cptbl_iso_8859_3,
                                      cptbl_iso_8859_4,
                                      cptbl_iso_8859_5,
                                      cptbl_iso_8859_6,
                                      cptbl_iso_8859_7,
                                      cptbl_iso_8859_8,
                                      cptbl_iso_8859_9,
                                      cptbl_cp437,
                                      cptbl_cp850,
                                      cptbl_cp852,
                                      cptbl_cp855,
                                      cptbl_cp857,
                                      cptbl_cp860,
                                      cptbl_cp861,
                                      cptbl_cp863,
                                      cptbl_cp864,
                                      cptbl_cp865,
                                      cptbl_cp869,
                                      cptbl_cp932, /* original shiftjis */
                                      cptbl_macintosh,
                                      cptbl_big5,
                                      cptbl_cp949, /* 25 */
                                      cptbl_johab, /* 26 */
                                      cptbl_cp866,
                                      cptbl_windows_1250,
                                      cptbl_windows_1251, /* 29 */
                                      cptbl_windows_1252, /* 30 */
                                      cptbl_gb2312,
                                      cptbl_windows_1253,
                                      cptbl_windows_1254,
                                      cptbl_windows_1255,
                                      cptbl_windows_1256,
                                      cptbl_windows_1257,
                                      cptbl_windows_874,
                                      cptbl_windows_932, /* windows-31j */
                                      cptbl_windows_936,
                                      cptbl_windows_949,
                                      cptbl_windows_950,
                                      cptbl_windows_1361, /* 42 */
                                      NULL,               /* 43 UTF16 */
                                      cptbl_windows_1258,
                                      NULL };

static const uint8_t *cp_alnumtbl[]
    = { NULL, // UTF8
        NULL, // US-ASCII
        NULL, // ISO-8859-1
        cptbl_alnum_iso_8859_2,
        cptbl_alnum_iso_8859_3,
        cptbl_alnum_iso_8859_4,
        cptbl_alnum_iso_8859_5,
        cptbl_alnum_iso_8859_6,
        cptbl_alnum_iso_8859_7,
        cptbl_alnum_iso_8859_8,
        cptbl_alnum_iso_8859_9,
        cptbl_alnum_cp437,
        cptbl_alnum_cp850,
        cptbl_alnum_cp852,
        cptbl_alnum_cp855,
        cptbl_alnum_cp857,
        cptbl_alnum_cp860,
        cptbl_alnum_cp861,
        cptbl_alnum_cp863,
        cptbl_alnum_cp864,
        cptbl_alnum_cp865,
        cptbl_alnum_cp869,
        NULL, // cptbl_alnum_cp932, /* original shiftjis */
        cptbl_alnum_macintosh,
        NULL, // cptbl_alnum_big5,
        NULL, // cptbl_alnum_cp949, /* 25 */
        NULL, // cptbl_alnum_johab, /* 26 */
        cptbl_alnum_cp866,
        cptbl_alnum_windows_1250,
        cptbl_alnum_windows_1251, /* 29 */
        cptbl_alnum_windows_1252, /* 30 */
        NULL,                     // cptbl_alnum_gb2312,
        cptbl_alnum_windows_1253,
        cptbl_alnum_windows_1254,
        cptbl_alnum_windows_1255,
        cptbl_alnum_windows_1256,
        cptbl_alnum_windows_1257,
        cptbl_alnum_windows_874,
        NULL, // cptbl_alnum_windows_932, /* windows-31j */
        NULL, // cptbl_alnum_windows_936,
        NULL, // cptbl_alnum_windows_949,
        NULL, // cptbl_alnum_windows_950,
        NULL, // cptbl_alnum_windows_1361, /* 42 */
        NULL, /* 43 UTF16 */
        cptbl_alnum_windows_1258,
        NULL };

static const uint16_t *cp_alnum16tbl[]
    = { NULL,              // UTF8
        NULL,              // US-ASCII
        NULL,              // ISO-8859-1
        NULL,              // cptbl_alnum_iso_8859_2,
        NULL,              // cptbl_alnum_iso_8859_3,
        NULL,              // cptbl_alnum_iso_8859_4,
        NULL,              // cptbl_alnum_iso_8859_5,
        NULL,              // cptbl_alnum_iso_8859_6,
        NULL,              // cptbl_alnum_iso_8859_7,
        NULL,              // cptbl_alnum_iso_8859_8,
        NULL,              // cptbl_alnum_iso_8859_9,
        NULL,              // cptbl_alnum_cp437,
        NULL,              // cptbl_alnum_cp850,
        NULL,              // cptbl_alnum_cp852,
        NULL,              // cptbl_alnum_cp855,
        NULL,              // cptbl_alnum_cp857,
        NULL,              // cptbl_alnum_cp860,
        NULL,              // cptbl_alnum_cp861,
        NULL,              // cptbl_alnum_cp863,
        NULL,              // cptbl_alnum_cp864,
        NULL,              // cptbl_alnum_cp865,
        NULL,              // cptbl_alnum_cp869,
        cptbl_alnum_cp932, /* original shiftjis */
        NULL,              // cptbl_alnum_macintosh,
        cptbl_alnum_big5,
        cptbl_alnum_cp949, /* 25 */
        cptbl_alnum_johab, /* 26 */
        NULL,              // cptbl_alnum_cp866,
        NULL,              // cptbl_alnum_windows_1250,
        NULL,              // cptbl_alnum_windows_1251, /* 29 */
        NULL,              // cptbl_alnum_windows_1252, /* 30 */
        cptbl_alnum_gb2312,
        NULL,                    // cptbl_alnum_windows_1253,
        NULL,                    // cptbl_alnum_windows_1254,
        NULL,                    // cptbl_alnum_windows_1255,
        NULL,                    // cptbl_alnum_windows_1256,
        NULL,                    // cptbl_alnum_windows_1257,
        NULL,                    // cptbl_alnum_windows_874,
        cptbl_alnum_windows_932, /* windows-31j */
        cptbl_alnum_windows_936,
        cptbl_alnum_windows_949,
        cptbl_alnum_windows_950,
        cptbl_alnum_windows_1361, /* 42 */
        NULL,                     /* 43 UTF16 */
        NULL,                     // cptbl_alnum_windows_1258,
        NULL };

// synced with typedef enum _dwg_codepage in codepages.h
#ifdef HAVE_ICONV

const char *
dwg_codepage_iconvstr (Dwg_Codepage cp)
{
  // for iconv
  const char *_codepage_iconvstr[] = { "UTF8",         "US-ASCII",
                                       "ISO-8859-1",   "ISO-8859-2",
                                       "ISO-8859-3",   "ISO-8859-4",
                                       "ISO-8859-5",   "ISO-8859-6",
                                       "ISO-8859-7",   "ISO-8859-8",
                                       "ISO-8859-9",   "CP437",
                                       "CP850",        "CP852",
                                       "CP855",        "CP857",
                                       "CP860",        "CP861",
                                       "CP863",        "CP864",
                                       "CP865",        "CP869",
                                       "CP932",        "MACINTOSH",
                                       "BIG5",         "CP949", /* 25 */
                                       "JOHAB",        "CP866",
                                       "WINDOWS-1250", "WINDOWS-1251", /* 29 */
                                       "WINDOWS-1252",                 /* 30 */
                                       "GB2312",       "WINDOWS-1253",
                                       "WINDOWS-1254", "WINDOWS-1255",
                                       "WINDOWS-1256", "WINDOWS-1257",
                                       "WINDOWS-874",  "WINDOWS-932",
                                       "WINDOWS-936",  "WINDOWS-949",
                                       "WINDOWS-950",  "WINDOWS-1361",
                                       "UTF16", /* 43 */
                                       "WINDOWS-1258", NULL };
  if (cp <= CP_ANSI_1258)
    return _codepage_iconvstr[cp];
  else
    return NULL;
}
#endif

const char *_codepage_dxfstr[]
    = { "UTF8",       "US_ASCII",   "ISO-8859-1", "ISO-8859-2", "ISO-8859-3",
        "ISO-8859-4", "ISO-8859-5", "ISO-8859-6", "ISO-8859-7", "ISO-8859-8",
        "ISO-8859-9", "CP437",      "CP850",      "CP852",      "CP855",
        "CP857",      "CP860",      "CP861",      "CP863",      "CP864",
        "CP865",      "CP869",      "CP932",      "MACINTOSH",  "BIG5",
        "CP949",                                               /* 25 */
        "JOHAB",      "CP866",      "ANSI_1250",  "ANSI_1251", /* 29 */
        "ANSI_1252", /* 30 WesternEurope Windows */
        "GB2312",     "ANSI_1253",  "ANSI_1254",  "ANSI_1255",  "ANSI_1256",
        "ANSI_1257",  "ANSI_874",   "ANSI_932",   "ANSI_936",   "ANSI_949",
        "ANSI_950",   "ANSI_1361",  "UTF16", /* 43 */
        "ANSI_1258",  NULL };

const char *
dwg_codepage_dxfstr (Dwg_Codepage cp)
{
  if (cp <= CP_ANSI_1258)
    return _codepage_dxfstr[cp];
  else if (cp == CP_UNDEFINED)
    return "undefined";
  else
    return NULL;
}

Dwg_Codepage
dwg_codepage_int (const char *s)
{
  for (int i = 0; i <= (int)CP_ANSI_1258; i++)
    {
      if (strEQ (s, _codepage_dxfstr[i]))
        return (Dwg_Codepage)i;
      if (islower (*s) && 0 == strcasecmp (s, _codepage_dxfstr[i]))
        return (Dwg_Codepage)i;
    }
  return CP_UNDEFINED;
}

/* helper to check if a codepoint exists in the codepage,
   and convert it to/from unicode.
   dir = 1: from unicode wc to charset
   asian = 1: 2-byte CJK charset, else 1-byte (0-255)
*/
static wchar_t
codepage_helper (const Dwg_Codepage codepage, const wchar_t wc, const int dir,
                 const int asian)
{
  const uint16_t *fntbl;
  uint16_t maxc;
  assert (codepage != CP_UTF8 && codepage != CP_UTF16
          && codepage != CP_US_ASCII && codepage != CP_ISO_8859_1);
  fntbl = cp_fntbl[codepage];
  maxc = fntbl[0];
  assert (maxc);
  if (dir) // from unicode to charset.
    {      // reverse lookup. unsorted rhs values so we cannot bsearch.
      for (uint16_t i = 0x80; i < maxc; i++)
        {
          if (wc == fntbl[i])
            return i;
        }
      return 0;
    }
  else
    {
      if (wc < maxc)
        return fntbl[wc];
      else
        return 0;
    }
}

// returns the matching unicode codepoint,
// or 0 if the codepage does not contain the character
wchar_t
dwg_codepage_uc (Dwg_Codepage cp, unsigned char c)
{
  if (c < 128)
    return (wchar_t)c;
  else if (cp == CP_US_ASCII)
    return 0;
  if (cp == CP_ISO_8859_1 || cp == CP_UTF8 || cp == CP_UTF16)
    return (wchar_t)c;
  return codepage_helper (cp, (wchar_t)c, 0, 0);
}
// for wide asian chars
wchar_t
dwg_codepage_uwc (Dwg_Codepage cp, uint16_t c)
{
  if (cp == CP_CP864 && c == 0x25)
    return 0x066a;
  else if (cp == CP_CP932 && c == 0x5c)
    return 0x00A5;
  else if (cp == CP_CP932 && c == 0x7e)
    return 0x203E;
  else if (cp == CP_JOHAB && c == 0x5c)
    return 0x20A9;
  else if (c < 128 || cp == CP_UTF8 || cp == CP_UTF16)
    return (wchar_t)c;
  return codepage_helper (cp, (wchar_t)c, 0, 1);
}
// returns the matching codepoint,
// or 0 if the codepage does not contain the wide character
unsigned char
dwg_codepage_c (Dwg_Codepage cp, wchar_t wc)
{
  if (wc < 128)
    {
      if (cp == CP_US_ASCII || cp == CP_UTF8 || cp == CP_UTF16)
        return wc & 0xff;
    }
  else if (cp == CP_US_ASCII)
    return 0;
  if (cp == CP_ISO_8859_1 || cp == CP_UTF8)
    return wc < 256 ? wc : 0;
  return (unsigned char)codepage_helper (cp, wc, 1, 0);
}
// for wide asian chars
uint16_t
dwg_codepage_wc (Dwg_Codepage cp, wchar_t wc)
{
  if (wc < 128 || cp == CP_UTF8 || cp == CP_UTF16)
    return wc & 0xffff;
  return (uint16_t)codepage_helper (cp, wc, 1, 1);
}

/* for possible wide asian chars:
   932 is single-byte for most chars, but 0x8*, 0x9*, 0xE* and 0xF* lead bytes
   CP949, JOHAB, ANSI_949, 936, 950 for all > 0x8* lead bytes
   1361 for all but 0x8[0123], 0xD[4567F], 0xF[A-F] lead bytes
   BIG5, GB2312 are two-byte only.

   none have valid 0x00 bytes, so strlen works as before in the TV case.
*/
bool
dwg_codepage_isasian (const Dwg_Codepage cp)
{
  if (cp >= CP_BIG5 && cp <= CP_JOHAB)
    return true;
  else if (cp >= CP_ANSI_932 && cp <= CP_ANSI_1258)
    return true;
  else if (cp == CP_GB2312)
    return true;
  else
    return false;
}

static int
b8_cmp (const void *a, const void *b)
{
  return *(uint8_t *)a < *(uint8_t *)b   ? -1
         : *(uint8_t *)a > *(uint8_t *)b ? 1
                                         : 0;
}

static int
b16_cmp (const void *a, const void *b)
{
  return *(uint16_t *)a < *(uint16_t *)b   ? -1
         : *(uint16_t *)a > *(uint16_t *)b ? 1
                                           : 0;
}

bool
dwg_codepage_isalnum (const Dwg_Codepage cp, const wchar_t c)
{
  if (c < 128)
    return isalnum ((int)c);
  switch (cp)
    {
    case CP_US_ASCII:
      return false;
    case CP_ISO_8859_1:
      return (c >= 0xC0 && c <= 0xD6) || (c >= 0xD8 && c <= 0xFF);
    case CP_UTF8:
    case CP_UTF16: // fallthru
#if defined HAVE_WCTYPE_H
      return iswalnum ((int)c);
#else
                   // TODO panic?
      return false;
#endif
    default:
      {
        const uint8_t *fntbl = cp_alnumtbl[cp];
        assert (cp != CP_UTF8 && cp != CP_UTF16 && cp != CP_US_ASCII
                && cp != CP_ISO_8859_1);
        // 8 or 16bit?
        if (fntbl)
          {
            const uint8_t key = c & 0xff;
            const uint8_t sz8 = fntbl[0];
            const size_t sz = (size_t)sz8;
            uint8_t *found
                = (uint8_t *)bsearch (&key, &fntbl[1], sz, 1, b8_cmp);
            if (!found || found == &fntbl[0])
              return false;
            else
              return true;
          }
        else
          {
            const uint16_t key = c & 0xffff;
            const uint16_t *fntbl16 = cp_alnum16tbl[cp];
            const uint16_t sz16 = fntbl16[0];
            const size_t sz = (size_t)sz16;
            uint16_t *found
                = (uint16_t *)bsearch (&key, &fntbl16[1], sz, 2, b16_cmp);
            if (!found || found == &fntbl16[0])
              return false;
            else
              return true;
          }
        return false;
      }
    }
}

bool
dwg_codepage_is_twobyte (const Dwg_Codepage cp, const unsigned char c)
{
  if (cp == CP_CP932 || cp == CP_ANSI_932)
    return (c >= 0x80 && c <= 0x9F) || (c >= 0xE0);
  else if (cp == CP_CP949 || cp == CP_ANSI_949 || cp == CP_ANSI_936
           || cp == CP_ANSI_950)
    return c & 0x80;
  else if (cp == CP_ANSI_1361)
    return (c >= 0x80 && c <= 0x83) || (c >= 0xD4 && c <= 0xD7) || (c == 0xDF)
           || (c >= 0xFA);
  else if (cp == CP_GB2312 || cp == CP_BIG5)
    return true;
  else
    return false;
}

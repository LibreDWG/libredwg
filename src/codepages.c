/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2023 Free Software Foundation, Inc.                        */
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
#if defined HAVE_ICONV && defined HAVE_ICONV_H
#  include <iconv.h>
#endif
//#define CODEPAGES_C
#include "common.h"
#include "codepages.h"

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

// synced with typedef enum _dwg_codepage in codepages.h
#ifdef HAVE_ICONV

const char *
dwg_codepage_iconvstr (Dwg_Codepage cp)
{
  // for iconv
  const char *_codepage_iconvstr[]
    = { "UTF8",         "US-ASCII",
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
        "BIG5",         "CP949",        /* 25 */
        "JOHAB",        "CP866",        /* also JOHAB */
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
  if (cp <= CP_DWG)
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
        "CP949",                   /* 25 */
        "JOHAB",      "CP866",     /* also JOHAB */
        "ANSI_1250",  "ANSI_1251", /* 29 */
        "ANSI_1252",               /* 30 WesternEurope Windows */
        "GB2312",     "ANSI_1253",  "ANSI_1254",  "ANSI_1255",  "ANSI_1256",
        "ANSI_1257",  "ANSI_874",   "ANSI_932",   "ANSI_936",   "ANSI_949",
        "ANSI_950",   "ANSI_1361",  "UTF16", /* 43 */
        "ANSI_1258",  NULL };

const char *
dwg_codepage_dxfstr (Dwg_Codepage cp)
{
  if (cp <= CP_DWG)
    return _codepage_dxfstr[cp];
  else
    return NULL;
}

Dwg_Codepage
dwg_codepage_int (const char *s)
{
  for (Dwg_Codepage i = CP_UTF8; i <= CP_ANSI_1258; i++)
    {
      if (strEQ (s, _codepage_dxfstr[i]))
        return i;
    }
  return CP_DWG;
}

/* helper to check if a codepoint exists in the codepage,
   and convert it to/from unicode.
   dir = 1: from unicode wc to charset
   asian = 1: 2-byte CJK charset, else 1-byte (0-255)
*/
static wchar_t
dwg_iconv_helper (Dwg_Codepage codepage, wchar_t wc, int dir, int asian)
{
  if (codepage == CP_UTF8)
    return wc;
  {
    union {
      wchar_t wc;
      char s[8];
      uint32_t w[2];
    } u1, u2;
    //const char *od = &u2.s[0];
    const char *charset = dwg_codepage_iconvstr (codepage);
    char *d = &u2.s[0];
    iconv_t cd;
    size_t nconv = (size_t)-1;
    size_t srclen = 2, destlen = 8;
    if (!charset)
      {
        loglevel = 1;
        LOG_ERROR ("Invalid codepage %u", codepage);
        return 0;
      }
    if (!dir)
      { // from charset to unicode
        cd = iconv_open ("UTF-32", charset);
      }
    else
      { // from unicode to charset
        cd = iconv_open (charset, "UTF-32");
      }
    if (cd == (iconv_t) -1)
      {
        loglevel = 1;
        LOG_ERROR ("iconv_open \"%s\" failed with errno %d",
                   charset, errno);
        return 0;
      }
    u1.wc = wc;
    u1.w[1] = 0;
    if (wc < 0xFF && !asian)
      srclen = 1;
    else if (asian && wc > 0xFFFF)
      srclen = 4;
    else if (wc > 0xFFFFFF)
      srclen = 4;
    else if (wc > 0xFFFF)
      srclen = 3;
    u2.wc = 0;
    //u2.s[4] = '\0';
    u2.w[1] = 0;
    nconv = iconv (cd, (char **)&u1.s, (size_t *)&srclen, (char **)&d,
                   (size_t *)&destlen);
    if (nconv == (size_t)-1)
      {
        loglevel = 1;
        LOG_ERROR ("iconv failed with errno %d", errno);
        //iconv_close (cd);
        //return 0;
      }
    // flush the remains
    iconv (cd, NULL, (size_t *)&srclen, (char **)&d, (size_t *)&destlen);
    iconv_close (cd);
    // convert dest to result
    return u2.wc;
  }
}

// returns the matching unicode codepoint,
// or 0 if the codepage does not contain the character
wchar_t
dwg_codepage_uc (Dwg_Codepage cp, unsigned char c)
{
  if (c < 128 || cp == CP_ISO_8859_1)
    return (wchar_t)c;
#ifdef HAVE_ICONV
  return dwg_iconv_helper (cp, (wchar_t)c, 0, 0);
#else
  return 0;
#endif
}
// for wide asian chars
wchar_t
dwg_codepage_uwc (Dwg_Codepage cp, uint16_t c)
{
  if (c < 128)
    return (wchar_t)c;
#ifdef HAVE_ICONV
  return dwg_iconv_helper (cp, (wchar_t)c, 0, 1);
#else
  return 0;
#endif
}
// returns the matching codepoint,
// or 0 if the codepage does not contain the wide character
unsigned char
dwg_codepage_c (Dwg_Codepage cp, wchar_t wc)
{
  if (wc < 128)
    return wc & 0xff;
  if (cp == CP_ISO_8859_1)
    return wc < 256 ? wc : 0;
#ifdef HAVE_ICONV
  return (unsigned char)dwg_iconv_helper (cp, wc, 1, 0);
#else
  return 0;
#endif
}
// for wide asian chars
uint16_t
dwg_codepage_wc (Dwg_Codepage cp, wchar_t wc)
{
  if (wc < 128)
    return wc & 0xffff;
#ifdef HAVE_ICONV
  return (uint16_t)dwg_iconv_helper (cp, wc, 1, 1);
#else
  return 0;
#endif
}

// for wide asian chars
bool
dwg_codepage_isasian (const Dwg_Codepage cp)
{
  if (cp >= CP_BIG5 && cp <= CP_CP866)
    return true;
  else if (cp == CP_UTF16 || cp == CP_GB2312)
    return true;
  else
    return false;
}

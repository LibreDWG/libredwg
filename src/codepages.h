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
 */

#ifndef CODEPAGES_H
#define CODEPAGES_H

#include "config.h"
#include <stdint.h>
#include <stdbool.h>
#if defined HAVE_WCHAR_H
#  include <wchar.h>
// clang/*/include/stddef.h(74,24): typedef __WCHAR_TYPE__ wchar_t
#elif defined __clang__
#else
typedef uint32_t wchar_t;
#endif
#include "dwg.h"

/* May not be changed, as it directly maps to the dwg->header.codepage number
 */
typedef enum _dwg_codepage
{
  CP_UTF8 = 0,
  CP_US_ASCII = 1,
  CP_ISO_8859_1,
  CP_ISO_8859_2,
  CP_ISO_8859_3,
  CP_ISO_8859_4,
  CP_ISO_8859_5,
  CP_ISO_8859_6,
  CP_ISO_8859_7,
  CP_ISO_8859_8,
  CP_ISO_8859_9,
  CP_CP437,     // DOS English
  CP_CP850,     // 12 DOS Latin-1
  CP_CP852,     // DOS Central European
  CP_CP855,     // DOS Cyrillic
  CP_CP857,     // DOS Turkish
  CP_CP860,     // DOS Portoguese
  CP_CP861,     // DOS Icelandic
  CP_CP863,     // DOS Hebrew
  CP_CP864,     // DOS Arabic (IBM)
  CP_CP865,     // DOS Nordic
  CP_CP869,     // DOS Greek
  CP_CP932,     // DOS Japanese (shiftjis)
  CP_MACINTOSH, // 23
  CP_BIG5,
  CP_CP949 = 25,     // Korean (Wansung + Johab)
  CP_JOHAB = 26,     // Johab?
  CP_CP866 = 27,     // Russian
  CP_ANSI_1250 = 28, // Central + Eastern European
  CP_ANSI_1251 = 29, // Cyrillic
  CP_ANSI_1252 = 30, // Western European
  CP_GB2312 = 31,    // EUC-CN Chinese
  CP_ANSI_1253,      // Greek
  CP_ANSI_1254,      // Turkish
  CP_ANSI_1255,      // Hebrew
  CP_ANSI_1256,      // Arabic
  CP_ANSI_1257,      // Baltic
  CP_ANSI_874,       // Thai
  CP_ANSI_932,       // 38 Japanese (extended shiftjis, windows-31j)
  CP_ANSI_936,       // 39 Simplified Chinese
  CP_ANSI_949,       // 40 Korean Wansung
  CP_ANSI_950,       // 41 Trad Chinese
  CP_ANSI_1361,      // 42 Korean Wansung
  CP_UTF16 = 43,
  CP_ANSI_1258 = 44,  // Vietnamese
  CP_UNDEFINED = 0xff // mostly R11
} Dwg_Codepage;

#ifdef HAVE_ICONV
const char *dwg_codepage_iconvstr (Dwg_Codepage cp);
#endif
const char *dwg_codepage_dxfstr (Dwg_Codepage cp);
Dwg_Codepage dwg_codepage_int (const char *s); // dxfstr

// returns the matching unicode codepoint,
// or 0 if the codepage does not contain the character
wchar_t dwg_codepage_uc (Dwg_Codepage cp, unsigned char c);

// returns the matching codepoint,
// or 0 if the codepage does not contain the wide character
unsigned char dwg_codepage_c (Dwg_Codepage cp, wchar_t wc);
// for wide asian chars
uint16_t dwg_codepage_wc (Dwg_Codepage cp, wchar_t wc);

#ifndef COMMON_TEST_C
// for wide asian chars
EXPORT wchar_t dwg_codepage_uwc (Dwg_Codepage cp, uint16_t c);
// these old codepages use 2-byte chars for some 0x8* bytes, all others only
// one byte.
EXPORT bool dwg_codepage_isasian (const Dwg_Codepage cp);
EXPORT bool dwg_codepage_is_twobyte (const Dwg_Codepage cp,
                                     const unsigned char c);
EXPORT bool dwg_codepage_isalnum (const Dwg_Codepage cp, const wchar_t wc);
#else
extern wchar_t dwg_codepage_uwc (Dwg_Codepage cp, uint16_t c);
extern bool dwg_codepage_isasian (const Dwg_Codepage cp);
extern bool dwg_codepage_is_twobyte (const Dwg_Codepage cp,
                                     const unsigned char c);
extern bool dwg_codepage_isalnum (const Dwg_Codepage cp, const wchar_t wc);
#endif

#endif

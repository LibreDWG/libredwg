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

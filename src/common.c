/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * common.c: common data arrays
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#include "config.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

unsigned char *
dwg_sentinel (Dwg_Sentinel s)
{
  static unsigned char sentinels[9][16] = {
    // DWG_SENTINEL_HEADER_END
    { 0x95, 0xA0, 0x4E, 0x28, 0x99, 0x82, 0x1A, 0xE5, 0x5E, 0x41, 0xE0, 0x5F,
      0x9D, 0x3A, 0x4D, 0x00 },
    // DWG_SENTINEL_THUMBNAIL_BEGIN
    { 0x1F, 0x25, 0x6D, 0x07, 0xD4, 0x36, 0x28, 0x28, 0x9D, 0x57, 0xCA, 0x3F,
      0x9D, 0x44, 0x10, 0x2B },
    // DWG_SENTINEL_THUMBNAIL_END
    { 0xE0, 0xDA, 0x92, 0xF8, 0x2B, 0xc9, 0xD7, 0xD7, 0x62, 0xA8, 0x35, 0xC0,
      0x62, 0xBB, 0xEF, 0xD4 },
    // DWG_SENTINEL_VARIABLE_BEGIN
    { 0xCF, 0x7B, 0x1F, 0x23, 0xFD, 0xDE, 0x38, 0xA9, 0x5F, 0x7C, 0x68, 0xB8,
      0x4E, 0x6D, 0x33, 0x5F },
    // DWG_SENTINEL_VARIABLE_END
    { 0x30, 0x84, 0xE0, 0xDC, 0x02, 0x21, 0xC7, 0x56, 0xA0, 0x83, 0x97, 0x47,
      0xB1, 0x92, 0xCC, 0xA0 },
    // DWG_SENTINEL_CLASS_BEGIN
    { 0x8D, 0xA1, 0xC4, 0xB8, 0xC4, 0xA9, 0xF8, 0xC5, 0xC0, 0xDC, 0xF4, 0x5F,
      0xE7, 0xCF, 0xB6, 0x8A },
    // DWG_SENTINEL_CLASS_END
    { 0x72, 0x5E, 0x3B, 0x47, 0x3B, 0x56, 0x07, 0x3A, 0x3F, 0x23, 0x0B, 0xA0,
      0x18, 0x30, 0x49, 0x75 },
    // DWG_SENTINEL_SECOND_HEADER_BEGIN
    { 0xD4, 0x7B, 0x21, 0xCE, 0x28, 0x93, 0x9F, 0xBF, 0x53, 0x24, 0x40, 0x09,
      0x12, 0x3C, 0xAA, 0x01 },
    // DWG_SENTINEL_SECOND_HEADER_END
    { 0x2B, 0x84, 0xDE, 0x31, 0xD7, 0x6C, 0x60, 0x40, 0xAC, 0xDB, 0xBF, 0xF6,
      0xED, 0xC3, 0x55, 0xFE },
  };
  return (sentinels[s]);
}

const char version_codes[DWG_VERSIONS][7] = {
  "INVALI", // R_INVALID
  "MC0.0",  /* DWG Release 1.1 (as MicroCAD) */
  "AC1.2",  /* DWG Release 1.2 (as AutoCAD) */
  "AC1.4",  /* DWG Release 1.4 */
  "AC1.50", /* DWG Release 2.0 */
  "AC2.10", /* DWG Release 2.10 */
  "AC1002", /* DWG Release 2.5 */
  "AC1003", /* DWG Release 2.6 */
  "AC1004", // R_9  DWG Release 9
  "AC1006", // R_10 DWG Release 10
  "AC1009", // R_11 DWG Release 11/12 (LT R1/R2)
  "AC1012", // R_13 and LT95, beware of R13C3
  "AC1014", // R_14
  "AC1015", // R_2000 (r15)
  "AC1018", // R_2004
  "AC1021", // R_2007
  "AC1024", // R_2010
  "AC1027", // R_2013
  "AC1032", // R_2018
  "------"  // R_AFTER
};

// keep in sync with common.h DWG_BITS
const char *dwg_bits_name[] = {
  "UNKNOWN", "RC",  "RS",  "RL",      "B",   "BB",     "3B",  "4BITS",
  "BS",      "BL",  "BLd", "RLL",     "RD",  "BD",     "MC",  "UMC",
  "MS",      "TV",  "TU",  "T",       "TF",  "HANDLE", "BE",  "DD",
  "BT",      "BOT", "BLL", "TIMEBLL", "CMC", "ENC",    "2RD", "3RD",
  "2BD",     "3BD", "2DD", "3DD",     "CRC", "CRC64",
};

// minimal size of type in bits
// keep in sync with above
const unsigned char dwg_bits_size[] = {
  0,   //"UNKNOWN",
  8,   //"RC",
  16,  //"RS",
  32,  //"RL",
  1,   //"B",
  2,   //"BB",
  3,   //"3B",
  4,   //"4BITS",
  2,   //"BS", 10,18
  2,   //"BL", 10,34
  2,   //"BLd", 10,34
  64,  //"RLL",
  64,  //"RD",
  2,   //"BD", 66
  1,   //"MC", 1-4
  1,   //"UMC", 1-4
  16,  //"MS", 32
  2,   //"TV",
  18,  //"TU",
  2,   //"T",
  1,   //"TF",
  8,   //"HANDLE",
  1,   //"BE", or 3BD
  2,   //"DD",
  1,   //"BT",
  10,  //"BOT",
  3,   //"BLL",
  4,   //"TIMEBLL", 2xBL
  2,   //"CMC", r2004+: +2
  4,   //"ENC", r2004+
  128, //"2RD",
  196, //"3RD",
  4,   //"2BD",
  6,   //"3BD",
  4,   //"2DD",
  6,   //"3DD",
  8,   //"CRC",
  64,  //"CRC64",
};

// map [rVER] to our enum number, not the dwg->header.dwgversion
// Acad 2018 offers SaveAs DWG: 2018,2013,2010,2007,2004,2004,2000,r14
//                         DXF: 2018,2013,2010,2007,2004,2004,2000,r12
// libdxfrw dwg2dxf offers R12, v2000, v2004, v2007, v2010
EXPORT Dwg_Version_Type
dwg_version_as (const char *version)
{
  if (strEQc (version, "r2000"))
    return R_2000;
  else if (strEQc (version, "r2004"))
    return R_2004;
  else if (strEQc (version, "r2007"))
    return R_2007;
  else if (strEQc (version, "r2010"))
    return R_2010;
  else if (strEQc (version, "r2013"))
    return R_2013;
  else if (strEQc (version, "r2018"))
    return R_2018;
  else if (strEQc (version, "r14"))
    return R_14;
  else if (strEQc (version, "r13"))
    return R_13;
  else if (strEQc (version, "r11") || strEQc (version, "r12"))
    return R_11;
  else if (strEQc (version, "r10"))
    return R_10;
  else if (strEQc (version, "r9"))
    return R_9;
  else if (strEQc (version, "r2.6"))
    return R_2_6;
  else if (strEQc (version, "r2.5"))
    return R_2_5;
  else if (strEQc (version, "r2.1"))
    return R_2_1;
  else if (strEQc (version, "r2.0"))
    return R_2_0;
  else if (strEQc (version, "r1.4"))
    return R_1_4;
  else if (strEQc (version, "r1.2"))
    return R_1_2;
  else if (strEQc (version, "r1.1"))
    return R_1_1;
  else
    return R_INVALID;
}

/* replace from ("[rcount1]") with to ("[%d]") in s (e.g.
   "ref[rcount1].classname"). s is a global constant (#nam), so we cannot
   change it in-place.
 */
char *
strrplc (const char *s, const char *from, const char *to)
{
  char *p = strstr (s, from);
  if (p)
    {
      int len = strlen (s) - (strlen (from) - strlen (to));
      char *dest = (char *)calloc (1, 80);
      int i = p - s;
      assert (len < 80);
      memcpy (dest, s, i);
      strcat (dest, to);
      strcat (dest, s + i + strlen (from));
      return dest;
    }
  else
    return NULL;
}

// print errors as string to stderr
EXPORT void
dwg_errstrings (int error)
{
  if (error & 1)
    fprintf (stderr, "WRONGCRC ");
  if (error & 2)
    fprintf (stderr, "NOTYETSUPPORTED ");
  if (error & 4)
    fprintf (stderr, "UNHANDLEDCLASS ");
  if (error & 8)
    fprintf (stderr, "INVALIDTYPE ");
  if (error & 16)
    fprintf (stderr, "INVALIDHANDLE ");
  if (error & 32)
    fprintf (stderr, "INVALIDEED ");
  if (error & 64)
    fprintf (stderr, "VALUEOUTOFBOUNDS ");
  // -- critical --
  if (error > 127)
    fprintf (stderr, "\nCritical: ");
  if (error & 128)
    fprintf (stderr, "CLASSESNOTFOUND ");
  if (error & 256)
    fprintf (stderr, "SECTIONNOTFOUND ");
  if (error & 512)
    fprintf (stderr, "PAGENOTFOUND ");
  if (error & 1024)
    fprintf (stderr, "INTERNALERROR ");
  if (error & 2048)
    fprintf (stderr, "INVALIDDWG ");
  if (error & 4096)
    fprintf (stderr, "IOERROR ");
  if (error & 8192)
    fprintf (stderr, "OUTOFMEM ");
  fprintf (stderr, "\n");
}

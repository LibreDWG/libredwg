/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2020 Free Software Foundation, Inc.                   */
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
#include "logging.h"

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
  "AC1002", // DWG Release 2.5                   9
  "AC1003", // DWG Release 2.6                   10
  "AC1004", // R_9  DWG Release 9                0x0b
  "AC1006", // R_10 DWG Release 10               0x0d
  "AC1009", // R_11 DWG Release 11/12 (LT R1/R2) 0x10
  "AC1012", // R_13 and LT95, beware of R13C3    0x13
  "AC1014", // R_14                              0x15
  "AC1015", // R_2000 (r15)                      0x17
  "AC1018", // R_2004                            0x18, 0x19
  "AC1021", // R_2007                            0x1b
  "AC1024", // R_2010                            0x1d
  "AC1027", // R_2013                            0x1f
  "AC1032", // R_2018                            0x21
  "------"  // R_AFTER
};

// keep in sync with common.h DWG_BITS
const char *dwg_bits_name[] = {
  "UNKNOWN", "RC",  "RS",  "RL",  "B",       "BB",  "3B",     "4BITS",
  "BS",      "BL",  "BLd", "RLL", "RD",      "BD",  "MC",     "UMC",
  "MS",      "TV",  "TU",  "T",   "TF",      "T32", "HANDLE", "BE",
  "DD",      "BT",  "BOT", "BLL", "TIMEBLL", "CMC", "ENC",    "2RD",
  "3RD",     "2BD", "3BD", "2DD", "3DD",     "CRC", "CRC64",
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
  2,   //"T32",
  4,   //"TU32",
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


enum RES_BUF_VALUE_TYPE
get_base_value_type (short gc)
{
  if (gc >= 300)
    {
      if (gc >= 440)
        {
          if (gc >= 1000) // 1000-1071
            {
              if (gc == 1004)
                return VT_BINARY;
              if (gc <= 1009)
                return VT_STRING;
              if (gc <= 1059)
                return VT_REAL;
              if (gc <= 1070)
                return VT_INT16;
              if (gc == 1071)
                return VT_INT32;
            }
          else // 440-999
            {
              if (gc <= 459)
                return VT_INT32;
              if (gc <= 469)
                return VT_REAL;
              if (gc <= 479)
                return VT_STRING;
              if (gc <= 998)
                return VT_INVALID;
              if (gc == 999)
                return VT_STRING; // lgtm [cpp/constant-comparison]
            }
        }
      else // <440
        {
          if (gc >= 390) // 390-439
            {
              if (gc <= 399)
                return VT_HANDLE;
              if (gc <= 409)
                return VT_INT16;
              if (gc <= 419)
                return VT_STRING;
              if (gc <= 429)
                return VT_INT32;
              if (gc <= 439)
                return VT_STRING; // lgtm [cpp/constant-comparison]
            }
          else // 330-389
            {
              if (gc <= 309)
                return VT_STRING;
              if (gc <= 319)
                return VT_BINARY;
              if (gc <= 329)
                return VT_HANDLE;
              if (gc <= 369)
                return VT_OBJECTID;
              if (gc <= 389)
                return VT_INT16; // lgtm [cpp/constant-comparison]
            }
        }
    }
  else if (gc >= 105)
    {
      if (gc >= 210) // 210-299
        {
          if (gc <= 239)
            return VT_REAL;
          if (gc <= 269)
            return VT_INVALID;
          if (gc <= 279)
            return VT_INT16;
          if (gc <= 289)
            return VT_INT8;
          if (gc <= 299)
            return VT_BOOL; // lgtm [cpp/constant-comparison]
        }
      else // 105-209
        {
          if (gc == 105)
            return VT_HANDLE;
          if (gc <= 109)
            return VT_INVALID;
          if (gc <= 149)
            return VT_REAL;
          if (gc <= 169) // e.g. REQUIREDVERSIONS 160 r2013+
            return VT_INT64;
          if (gc <= 179)
            return VT_INT16;
          if (gc <= 209)
            return VT_INVALID; // lgtm [cpp/constant-comparison]
        }
    }
  else // <105
    {
      if (gc >= 38) // 38-102
        {
          if (gc <= 59)
            return VT_REAL;
          if (gc <= 79)
            return VT_INT16;
          if (gc <= 99)
            return VT_INT32;
          if (gc <= 101)
            return VT_STRING;
          if (gc == 102)
            return VT_STRING;
        }
      else // 0-37
        {
          if (gc < 0)
            return VT_HANDLE;
          if (gc <= 4)
            return VT_STRING;
          if (gc == 5)
            return VT_HANDLE;
          if (gc <= 9)
            return VT_STRING; // but 9 never TU
          if (gc <= 37)
            return VT_POINT3D; // lgtm [cpp/constant-comparison]
        }
    }
  return VT_INVALID;
}

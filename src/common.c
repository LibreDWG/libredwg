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

const struct dwg_versions dwg_versions[] = {
  { R_INVALID,  "invalid", "INVALI",  "No DWG", 0 },
  { R_1_1,	"mc0.0", "MC0.0", "MicroCAD Release 1.1", 0 },
  { R_1_2,	"r1.2",  "AC1.2", "AutoCAD Release 1.2", 0 },
  { R_1_3,	"r1.3",  "AC1.3", "AutoCAD Release 1.3", 1 },
  { R_1_4,	"r1.40", "AC1.40", "AutoCAD Release 1.4", 2 },
  { R_1_402b,	"r402b", "AC402b", "AutoCAD Release 1.402b", 3 },
  { R_2_0,	"r1.50", "AC1.50", "AutoCAD Release 2.0", 4 },
  { R_2_1,	"r2.10", "AC2.10", "AutoCAD Release 2.10", 5 },
  { R_2_21,	"r2.21", "AC2.21", "AutoCAD Release 2.21", 6 },
  { R_2_22,	"r2.22", "AC2.22", "AutoCAD Release 2.22", 7 },
  { R_2_4,	"r2.4",  "AC1001", "AutoCAD Release 2.4", 8 },
  { R_2_5,	"r2.5",  "AC1002", "AutoCAD Release 2.5", 9 },
  { R_2_6,	"r2.6",  "AC1003", "AutoCAD Release 2.6", 10 },
  { R_9,	"r9",    "AC1004", "AutoCAD Release 9", 0xb },
  { R_9c1,	"r9c1",  "AC1005", "AutoCAD Release 9c1", 0xc },
  { R_10,	"r10",   "AC1006", "AutoCAD Release 10", 0xd },
  { R_10c1,	"r10c1", "AC1007", "AutoCAD Release 10c1", 0xe },
  { R_10c2,	"r10c2", "AC1008", "AutoCAD Release 10c2", 0xf },
  { R_11,	"r11",   "AC1009", "AutoCAD Release 11/12 (LT R1/R2)", 0x10 },
  { R_12,	"r12",   "AC1010", "AutoCAD Release 12", 0x11 },
  { R_12c1,	"r12c1", "AC1011", "AutoCAD Release 12c1", 0x12 },
  { R_13,	"r13",   "AC1012", "AutoCAD Release 13", 0x13 },
  { R_13c3,	"r13c3", "AC1013", "AutoCAD Release 13C3", 0x14 },
  { R_14,	"r14",   "AC1014", "AutoCAD Release 14", 0x15 },
  { R_2000,	"r2000", "AC1015", "AutoCAD Release 2000", 0x16 },
  { R_2000i,	"r200i", "AC1016", "AutoCAD Release 2000i", 0x17 },
  { R_2002,	"r2002", "AC1017", "AutoCAD Release 2002", 0x18 },
  // (includes versions AC1019/0x19 and AC1020/0x1a)
  { R_2004,	"r2004", "AC1018", "AutoCAD Release 2004", 0x18 },
  { R_2005,	"r2005", "AC1019", "AutoCAD Release 2005", 0x19 },
  { R_2006,	"r2006", "AC1020", "AutoCAD Release 2006", 0x1a },
  { R_2007,	"r2007", "AC1021", "AutoCAD Release 2007", 0x1b },
  { R_2008,	"r2008", "AC1022", "AutoCAD Release 2008", 0x1c },
  { R_2009,	"r2009", "AC1023", "AutoCAD Release 2009", 0x1c },
  { R_2010,	"r2010", "AC1024", "AutoCAD Release 2010", 0x1d },
  { R_2011,	"r2011", "AC1025", "AutoCAD Release 2011", 0x1e },
  { R_2012,	"r2012", "AC1026", "AutoCAD Release 2012", 0x1e },
  { R_2013,	"r2013", "AC1027", "AutoCAD Release 2013", 0x1f },
  { R_2014,	"r2014", "AC1028", "AutoCAD Release 2014", 0x1f },
  { R_2015,	"r2015", "AC1029", "AutoCAD Release 2015", 0x1f },
  { R_2016,	"r2016", "AC1030", "AutoCAD Release 2016", 0x1f },
  { R_2017,	"r2017", "AC1031", "AutoCAD Release 2017", 0x20 },
  { R_2018,	"r2018", "AC1032", "AutoCAD Release 2018", 0x21 },
  { R_2019,	"r2019", "AC1033", "AutoCAD Release 2019", 0x22 },
  { R_2020,	"r2020", "AC1034", "AutoCAD Release 2020", 0x23 },
  { R_2021,	"r2021", "AC1035", "AutoCAD Release 2021", 0x23 },
  { R_2022,	"r2022", "AC103-4","AutoCAD Release 2022", 0x24 },
  { R_AFTER,    "r>2022", NULL,    "AutoCAD Release >2022", 0x25 },
};

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

const char*
dwg_version_codes (const Dwg_Version_Type version)
{
  if (version < R_AFTER)
    return dwg_versions[version].hdr;
  else
    return "------";
}

// map [rVER] to our enum number, not the dwg->header.dwgversion
// Acad 2018 offers SaveAs DWG: 2018,2013,2010,2007,2004,2004,2000,r14
//                         DXF: 2018,2013,2010,2007,2004,2004,2000,r12
// libdxfrw dwg2dxf offers R12, v2000, v2004, v2007, v2010
EXPORT Dwg_Version_Type
dwg_version_as (const char *version)
{
  for (int i = 0; i < R_AFTER; i++)
    {
      if (strEQ (dwg_versions[i].type, version))
        return dwg_versions[i].r;
    }
  return R_INVALID;
}

/** The reverse of dwg_version_as(char*) */
EXPORT const char *
dwg_version_type (const Dwg_Version_Type version)
{
  if (version < R_AFTER)
    return dwg_versions[version].type;
  else
    return "invalid after";
}

/** The version from the magic char[6] header. */
EXPORT Dwg_Version_Type
dwg_version_hdr_type (const char *hdr)
{
  for (int i = 1; i < R_AFTER; i++)
    {
      if (strEQ (dwg_versions[i].hdr, hdr))
        return dwg_versions[i].r;
    }
  return R_INVALID;
}

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
  const char *p = strstr (s, from);
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

#if !defined(HAVE_MEMMEM) || defined(COMMON_TEST_C)
// naive from scratch implementation, not from glibc.
// see also examples/unknown.c:membits
void *  __nonnull((1, 3))
my_memmem (const void *h0, size_t k, const void *n0, size_t l)
{
  const unsigned char *h = h0, *n = n0;
  unsigned char *plast;

  if (!l)
    return (void *)h; // empty needle
  if (k < l)
    return NULL;      // needle longer than haystack
  h = memchr (h0, *n, k);
  if (!h || l == 1)
    return (void *)h; // first needle char not found
  k -= h - (const unsigned char *)h0;
  if (k < l)
    return NULL;      // no room for needle

  plast = (unsigned char*)h + (k - l);
  do // naive 2 loops: O(n^2)
    {
      size_t i = 0;
      while (h[i] == n[i])
        {
          if (++i == l)
            return (void *)h;
        }
    }
  while (++h <= plast);
  return NULL;
}
#endif

/*
 32 types, with 3 categories: Face, Edge, Display, plus 58 props r2013+
 */
const char *const _dwg_VISUALSTYLE_types[32] = { "Flat",
                                                 "FlatWithEdges",
                                                 "Gouraud",
                                                 "GouraudWithEdges",
                                                 "2DWireframe",
                                                 "3DWireFrame",
                                                 "Hidden",
                                                 "Basic",
                                                 "Realistic",
                                                 "Conceptual",
                                                 "Dim",
                                                 "Brighten",
                                                 "Thicken",
                                                 "LinePattern",
                                                 "Facepattern",
                                                 "ColorChange",
                                                 "FaceOnly",
                                                 "EdgeOnly",
                                                 "DisplayOnly",
                                                 "JitterOff",
                                                 "OverhangOff",
                                                 "EdgeColorOff",
                                                 "Shades of Gray",
                                                 "Sketchy",
                                                 "X-Ray",
                                                 "Shaded with edges",
                                                 "Shaded",
                                                 "ByViewport",
                                                 "ByLayer",
                                                 "ByBlock",
                                                 "ForEmptyStyle" };

/* types of the 58 rest r2013+ properties.
 * 1:
 * 2:
 * 3:
 * 4:
 * 5:
 */
const unsigned char _dwg_VISUALSTYLE_proptypes[58] =
  {
   /* [0]  */    2,  2,  2,  2,
   /* [4]  */    3,  3,  4,  2,
   /* [8]  */    2,  4,  4,  2,
   /* [12] */    2,  3,  2,  4,
   /* [16] */    3,  2,  2,  2,
   /* [20] */    4,  2,  2,  2,
   /* [24] */    1,  2,  3,  2,
   /* [28] */    1,  1,  1,  1,
   /* [32] */    1,  1,  1,  1,
   /* [36] */    1,  2,  3,  3,
   /* [40] */    2,  4,  2,  2,
   /* [44] */    4,  1,  2,  2,
   /* [48] */    2,  1,  2,  4,
   /* [52] */    3,  2,  5,  1,
   /* [56] */    3,  3
  };

// need to return the first ref from the handle vector.
BITCODE_H
shift_hv (BITCODE_H *hv, BITCODE_BL *num_p)
{
  BITCODE_H ref = hv[0];
  *num_p = *num_p - 1;
  memmove (&hv[0], &hv[1], *num_p * sizeof (BITCODE_H));
  return ref;
}

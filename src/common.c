/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2025 Free Software Foundation, Inc.                   */
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
#define COMMON_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include "common.h"
#include "logging.h"

// See also
// http://devel.gvsig.org/sites/org.gvsig.dwg/2.0-SNAPSHOT/gvsig-base-library-pom/org.gvsig.dwg/xref/org/gvsig/dwg/lib/DwgFile.html
const struct dwg_versions dwg_versions[] = {
  /* r,          type,    hdr,    desc,         dwg_version */
  { R_INVALID, "invalid", "INVALI", "No DWG", 0 },
  { R_1_1, "r1.1", "MC0.0", "MicroCAD Release 1.1", 0 },
  { R_1_2, "r1.2", "AC1.2", "AutoCAD Release 1.2", 0 },
  { R_1_3, "r1.3", "AC1.3", "AutoCAD Release 1.3", 1 },
  { R_1_4, "r1.4", "AC1.40", "AutoCAD Release 1.4", 2 },
  { R_2_0b, "r2.0b", "AC1.50", "AutoCAD 2.0 beta", 3 }, // not seen
  { R_2_0, "r2.0", "AC1.50", "AutoCAD Release 2.0", 4 },
  { R_2_10, "r2.10", "AC2.10", "AutoCAD Release 2.10", 5 },
  { R_2_21, "r2.21", "AC2.21", "AutoCAD Release 2.21", 6 },
  { R_2_22, "r2.22", "AC2.22", "AutoCAD Release 2.22", 7 },
  { R_2_4, "r2.4", "AC1001", "AutoCAD Release 2.4", 8 },
  { R_2_5, "r2.5", "AC1002", "AutoCAD Release 2.5", 9 },
  { R_2_6, "r2.6", "AC1003", "AutoCAD Release 2.6", 10 },
  { R_9, "r9", "AC1004", "AutoCAD Release 9", 0xb },
  { R_9c1, "r9c1", "AC1005", "AutoCAD Release 9c1", 0xc },
  { R_10, "r10", "AC1006", "AutoCAD Release 10", 0xd },
  { R_11b1, "r11b1", "AC1007", "AutoCAD 11 beta 1", 0xe },
  { R_11b2, "r11b2", "AC1008", "AutoCAD 11 beta 2", 0xf },
  { R_11, "r11", "AC1009", "AutoCAD Release 11/12 (LT R1/R2)", 0x10 },
  { R_13b1, "r13b1", "AC1010", "AutoCAD pre-R13 a", 0x11 },
  { R_13b2, "r13b2", "AC1011", "AutoCAD pre-R13 b", 0x12 },
  { R_13, "r13", "AC1012", "AutoCAD Release 13", 0x13 },
  { R_13c3, "r13c3", "AC1013", "AutoCAD Release 13c3", 0x14 },
  { R_14, "r14", "AC1014", "AutoCAD Release 14", 0x15 },
  { R_2000b, "r2000b", "AC1500", "AutoCAD 2000 beta", 0x16 },
  { R_2000, "r2000", "AC1015", "AutoCAD Release 2000", 0x17 },
  { R_2000i, "r2000i", "AC1016", "AutoCAD Release 2000i", 0x17 },
  { R_2002, "r2002", "AC1017", "AutoCAD Release 2002", 0x17 },
  { R_2004a, "r2004a", "AC402a", "AutoCAD 2004 alpha a", 0x18 },
  { R_2004b, "r2004b", "AC402b", "AutoCAD 2004 alpha b", 0x18 },
  { R_2004c, "r2004c", "AC1018", "AutoCAD 2004 beta", 0x18 },
  // (includes versions AC1019/0x19 and AC1020/0x1a)
  { R_2004, "r2004", "AC1018", "AutoCAD Release 2004", 0x19 },
  //{ R_2005, "r2005", "AC1019", "AutoCAD 2005", 0x19 }, // not seen
  //{ R_2006, "r2006", "AC1020", "AutoCAD 2006", 0x19 }, // not seen
  { R_2007a, "r2007a", "AC701a", "AutoCAD 2007 alpha", 0x1a },
  { R_2007b, "r2007b", "AC1021", "AutoCAD 2007 beta", 0x1a },
  { R_2007, "r2007", "AC1021", "AutoCAD Release 2007", 0x1b },
  //{ R_2008, "r2008", "AC1022", "AutoCAD 2008", 0x1b }, // not seen
  //{ R_2009, "r2009", "AC1023", "AutoCAD 2009", 0x1b }, // not seen
  { R_2010b, "r2010b", "AC1024", "AutoCAD 2010 beta", 0x1c },
  { R_2010, "r2010", "AC1024", "AutoCAD Release 2010", 0x1d },
  //{ R_2011, "r2011", "AC1025", "AutoCAD 2011", 0x1d }, // not seen
  //{ R_2012, "r2012", "AC1026", "AutoCAD 2012", 0x1e }, // not seen
  { R_2013b, "r2013b", "AC1027", "AutoCAD 2013 beta", 0x1e },
  { R_2013, "r2013", "AC1027", "AutoCAD Release 2013", 0x1f },
  //{ R_2014, "r2014", "AC1028", "AutoCAD 2014", 0x1f }, // not seen
  //{ R_2015, "r2015", "AC1029", "AutoCAD 2015", 0x1f }, // not seen
  //{ R_2016, "r2016", "AC1030", "AutoCAD 2016", 0x1f }, // not seen
  //{ R_2017, "r2017", "AC1031", "AutoCAD 2017", 0x20 }, // not seen
  { R_2018b, "r2018b", "AC1032", "AutoCAD 2018 beta", 0x20 },
  { R_2018, "r2018", "AC1032", "AutoCAD Release 2018", 0x21 },
  //{ R_2019, "r2019", "AC1033", "AutoCAD Release 2019", 0x22 }, // not seen
  //{ R_2020, "r2020", "AC1034", "AutoCAD Release 2020", 0x23 }, // not seen
  //{ R_2021, "r2021", "AC1035", "AutoCAD Release 2021", 0x23 }, // not seen
  { R_2022b, "r2022b", "AC103-4", "AutoCAD 2022 beta", 0x24 },
  { R_AFTER, "r>2022", NULL, "AutoCAD Release >2022", 0 },
};

const unsigned char *
dwg_sentinel (const Dwg_Sentinel sentinel_id)
{
  static const unsigned char sentinels[DWG_SENTINEL_R11_AUXHEADER_END + 2][16]
      = { // DWG_SENTINEL_HEADER_END
          { 0x95, 0xA0, 0x4E, 0x28, 0x99, 0x82, 0x1A, 0xE5, 0x5E, 0x41, 0xE0,
            0x5F, 0x9D, 0x3A, 0x4D, 0x00 },
          // DWG_SENTINEL_THUMBNAIL_BEGIN
          { 0x1F, 0x25, 0x6D, 0x07, 0xD4, 0x36, 0x28, 0x28, 0x9D, 0x57, 0xCA,
            0x3F, 0x9D, 0x44, 0x10, 0x2B },
          // DWG_SENTINEL_THUMBNAIL_END
          { 0xE0, 0xDA, 0x92, 0xF8, 0x2B, 0xc9, 0xD7, 0xD7, 0x62, 0xA8, 0x35,
            0xC0, 0x62, 0xBB, 0xEF, 0xD4 },
          // DWG_SENTINEL_VARIABLE_BEGIN
          { 0xCF, 0x7B, 0x1F, 0x23, 0xFD, 0xDE, 0x38, 0xA9, 0x5F, 0x7C, 0x68,
            0xB8, 0x4E, 0x6D, 0x33, 0x5F },
          // DWG_SENTINEL_VARIABLE_END
          { 0x30, 0x84, 0xE0, 0xDC, 0x02, 0x21, 0xC7, 0x56, 0xA0, 0x83, 0x97,
            0x47, 0xB1, 0x92, 0xCC, 0xA0 },
          // DWG_SENTINEL_CLASS_BEGIN
          { 0x8D, 0xA1, 0xC4, 0xB8, 0xC4, 0xA9, 0xF8, 0xC5, 0xC0, 0xDC, 0xF4,
            0x5F, 0xE7, 0xCF, 0xB6, 0x8A },
          // DWG_SENTINEL_CLASS_END
          { 0x72, 0x5E, 0x3B, 0x47, 0x3B, 0x56, 0x07, 0x3A, 0x3F, 0x23, 0x0B,
            0xA0, 0x18, 0x30, 0x49, 0x75 },
          // DWG_SENTINEL_2NDHEADER_BEGIN
          { 0xD4, 0x7B, 0x21, 0xCE, 0x28, 0x93, 0x9F, 0xBF, 0x53, 0x24, 0x40,
            0x09, 0x12, 0x3C, 0xAA, 0x01 },
          // DWG_SENTINEL_2NDHEADER_END
          { 0x2B, 0x84, 0xDE, 0x31, 0xD7, 0x6C, 0x60, 0x40, 0xAC, 0xDB, 0xBF,
            0xF6, 0xED, 0xC3, 0x55, 0xFE },
          // DWG_SENTINEL_R11_ENTITIES_BEGIN C46E6854F86E3330633EC1852ADC9401
          { 0xC4, 0x6E, 0x68, 0x54, 0xF8, 0x6E, 0x33, 0x30, 0x63, 0x3E, 0xC1,
            0x85, 0x2A, 0xDC, 0x94, 0x01 },
          // DWG_SENTINEL_R11_ENTITIES_END   3B9197AB0791CCCF9CC13E7AD5236BFE
          { 0x3B, 0x91, 0x97, 0xAB, 0x07, 0x91, 0xCC, 0xCF, 0x9C, 0xC1, 0x3E,
            0x7A, 0xD5, 0x23, 0x6B, 0xFE },
          // DWG_SENTINEL_R11_BLOCK_BEGIN DBEFB3F0C73E6DA6C9B6245C4C6F32CB
          { 0xDB, 0xEF, 0xB3, 0xF0, 0xC7, 0x3E, 0x6D, 0xA6, 0xC9, 0xB6, 0x24,
            0x5C, 0x4C, 0x6F, 0x32, 0xCB },
          // DWG_SENTINEL_R11_BLOCK_END   24104C0F38C192593649DBA3B390CD34
          { 0x24, 0x10, 0x4C, 0x0F, 0x38, 0xC1, 0x92, 0x59, 0x36, 0x49, 0xDB,
            0xA3, 0xB3, 0x90, 0xCD, 0x34 },
          // DWG_SENTINEL_R11_LAYER_BEGIN 0EC4646FBB1DD38B0049C2EF18EA6FFB
          { 0x0E, 0xC4, 0x64, 0x6F, 0xBB, 0x1D, 0xD3, 0x8B, 0x00, 0x49, 0xC2,
            0xEF, 0x18, 0xEA, 0x6F, 0xFB },
          // DWG_SENTINEL_R11_LAYER_END   F13B9B9044E22C74FFB63D10E7159004
          { 0xF1, 0x3B, 0x9B, 0x90, 0x44, 0xE2, 0x2C, 0x74, 0xFF, 0xB6, 0x3D,
            0x10, 0xE7, 0x15, 0x90, 0x04 },
          // DWG_SENTINEL_R11_STYLE_BEGIN E23EC182439F617750ABC76696000618
          { 0xE2, 0x3E, 0xC1, 0x82, 0x43, 0x9F, 0x61, 0x77, 0x50, 0xAB, 0xC7,
            0x66, 0x96, 0x00, 0x06, 0x18 },
          // DWG_SENTINEL_R11_STYLE_END   1DC13E7DBC609E88AF54389969FFF9E7
          { 0x1D, 0xC1, 0x3E, 0x7D, 0xBC, 0x60, 0x9E, 0x88, 0xAF, 0x54, 0x38,
            0x99, 0x69, 0xFF, 0xF9, 0xE7 },
          // DWG_SENTINEL_R11_LTYPE_BEGIN AC901ACA1CBD951516164C14CE1888AF
          { 0xAC, 0x90, 0x1A, 0xCA, 0x1C, 0xBD, 0x95, 0x15, 0x16, 0x16, 0x4C,
            0x14, 0xCE, 0x18, 0x88, 0xAF },
          // DWG_SENTINEL_R11_LTYPE_END   536FE535E3426AEAE9E9B3EB31E77750
          { 0x53, 0x6F, 0xE5, 0x35, 0xE3, 0x42, 0x6A, 0xEA, 0xE9, 0xE9, 0xB3,
            0xEB, 0x31, 0xE7, 0x77, 0x50 },
          // DWG_SENTINEL_R11_VIEW_BEGIN  C13CAA5668F4B41E4B74F408424DBFA5
          { 0xC1, 0x3C, 0xAA, 0x56, 0x68, 0xF4, 0xB4, 0x1E, 0x4B, 0x74, 0xF4,
            0x08, 0x42, 0x4D, 0xBF, 0xA5 },
          // DWG_SENTINEL_R11_VIEW_END    3EC355A9970B4BE1B48B0BF7BDB2405A
          { 0x3E, 0xC3, 0x55, 0xA9, 0x97, 0x0B, 0x4B, 0xE1, 0xB4, 0x8B, 0x0B,
            0xF7, 0xBD, 0xB2, 0x40, 0x5A },
          // DWG_SENTINEL_R11_UCS_BEGIN   604AFA3D8490CC5BEFE7D6A57F1E61CD
          { 0x60, 0x4A, 0xFA, 0x3D, 0x84, 0x90, 0xCC, 0x5B, 0xEF, 0xE7, 0xD6,
            0xA5, 0x7F, 0x1E, 0x61, 0xCD },
          // DWG_SENTINEL_R11_UCS_END     9FB505C27B6F33A41018295A80E19E32
          { 0x9F, 0xB5, 0x05, 0xC2, 0x7B, 0x6F, 0x33, 0xA4, 0x10, 0x18, 0x29,
            0x5A, 0x80, 0xE1, 0x9E, 0x32 },
          // DWG_SENTINEL_R11_VPORT_BEGIN F6ED44612ADCE47B4EB92BBB6660638D
          { 0xF6, 0xED, 0x44, 0x61, 0x2A, 0xDC, 0xE4, 0x7B, 0x4E, 0xB9, 0x2B,
            0xBB, 0x66, 0x60, 0x63, 0x8D },
          // DWG_SENTINEL_R11_VPORT_END   0912BB9ED5231B84B146D444999F9C72
          { 0x09, 0x12, 0xBB, 0x9E, 0xD5, 0x23, 0x1B, 0x84, 0xB1, 0x46, 0xD4,
            0x44, 0x99, 0x9F, 0x9C, 0x72 },
          // DWG_SENTINEL_R11_APPID_BEGIN E125C25036686C0C3BD35D56C1791C3A
          { 0xE1, 0x25, 0xC2, 0x50, 0x36, 0x68, 0x6C, 0x0C, 0x3B, 0xD3, 0x5D,
            0x56, 0xC1, 0x79, 0x1C, 0x3A },
          // DWG_SENTINEL_R11_APPID_END   1EDA3DAFC99793F3C42CA2A93E86E3C5
          { 0x1E, 0xDA, 0x3D, 0xAF, 0xC9, 0x97, 0x93, 0xF3, 0xC4, 0x2C, 0xA2,
            0xA9, 0x3E, 0x86, 0xE3, 0xC5 },
          // DWG_SENTINEL_R11_DIMSTYLE_BEGIN B4183E42C99FFFE5B6E2CBB375C3C3B0
          { 0xB4, 0x18, 0x3E, 0x42, 0xC9, 0x9F, 0xFF, 0xE5, 0xB6, 0xE2, 0xCB,
            0xB3, 0x75, 0xC3, 0xC3, 0xB0 },
          // DWG_SENTINEL_R11_DIMSTYLE_END   4BE7C1BD3660001A491D344C8A3C3C4F
          { 0x4B, 0xE7, 0xC1, 0xBD, 0x36, 0x60, 0x00, 0x1A, 0x49, 0x1D, 0x34,
            0x4C, 0x8A, 0x3C, 0x3C, 0x4F },
          // DWG_SENTINEL_R11_VX_BEGIN    E0CA367CCEE7586F2B7D745505F1447F
          { 0xE0, 0xCA, 0x36, 0x7C, 0xCE, 0xE7, 0x58, 0x6F, 0x2B, 0x7D, 0x74,
            0x55, 0x05, 0xF1, 0x44, 0x7F },
          // DWG_SENTINEL_R11_VX_END      1F35C9833118A790D4828BAAFA0EBB80
          { 0x1F, 0x35, 0xC9, 0x83, 0x31, 0x18, 0xA7, 0x90, 0xD4, 0x82, 0x8B,
            0xAA, 0xFA, 0x0E, 0xBB, 0x80 },
          // DWG_SENTINEL_R11_BLOCK_ENTITIES_BEGIN
          // 722B7DEC3E8C886C7A720AFDC86C8426
          { 0x72, 0x2B, 0x7D, 0xEC, 0x3E, 0x8C, 0x88, 0x6C, 0x7A, 0x72, 0x0A,
            0xFD, 0xC8, 0x6C, 0x84, 0x26 },
          // DWG_SENTINEL_R11_BLOCK_ENTITIES_END
          // 8DD48213C1737793858DF50237937BD9
          { 0x8D, 0xD4, 0x82, 0x13, 0xC1, 0x73, 0x77, 0x93, 0x85, 0x8D, 0xF5,
            0x02, 0x37, 0x93, 0x7B, 0xD9 },
          // DWG_SENTINEL_R11_EXTRA_ENTITIES_BEGIN
          // D5F9D3BB0AA969A6CD1C87C7EE804B17
          { 0xD5, 0xF9, 0xD3, 0xBB, 0x0A, 0xA9, 0x69, 0xA6, 0xCD, 0x1C, 0x87,
            0xC7, 0xEE, 0x80, 0x4B, 0x17 },
          // DWG_SENTINEL_R11_EXTRA_ENTITIES_END
          // 2A062C44F556965932E37838117FB4E8
          { 0x2A, 0x06, 0x2C, 0x44, 0xF5, 0x56, 0x96, 0x59, 0x32, 0xE3, 0x78,
            0x38, 0x11, 0x7F, 0xB4, 0xE8 },
          // DWG_SENTINEL_R11_AUXHEADER_BEGIN 298DD149A9731FEA99DE32F94D0AE019
          { 0x29, 0x8D, 0xD1, 0x49, 0xA9, 0x73, 0x1F, 0xEA, 0x99, 0xDE, 0x32,
            0xF9, 0x4D, 0x0A, 0xE0, 0x19 },
          // DWG_SENTINEL_R11_AUXHEADER_END D6722EB6568CE0156621CD06B2F51FE6
          { 0xD6, 0x72, 0x2E, 0xB6, 0x56, 0x8C, 0xE0, 0x15, 0x66, 0x21, 0xCD,
            0x06, 0xB2, 0xF5, 0x1F, 0xE6 },
          // DWG_SENTINEL_ILLEGAL (used for memcmp)
          { 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE,
            0xFE, 0xFE, 0xFE, 0xFE, 0xFE }
        };
  if ((unsigned)sentinel_id > DWG_SENTINEL_R11_AUXHEADER_END)
    {
      fprintf (stderr, "ERROR: Illegal sentinel_id %u\n",
               (unsigned)sentinel_id);
      return sentinels[DWG_SENTINEL_R11_AUXHEADER_END + 1];
    }
  return (const unsigned char *)sentinels[sentinel_id];
}

const struct dwg_versions *
dwg_version_struct (const Dwg_Version_Type version)
{
  if (version < R_AFTER)
    return &dwg_versions[version];
  else
    return NULL;
}

/* Returns the AC header magic string [6] */
const char *
dwg_version_codes (const Dwg_Version_Type version)
{
  if (version < R_AFTER)
    return dwg_versions[version].hdr;
  else
    return "------";
}

// map [rVER] to our enum number, not the dwg->header.dwgversion
// Acad 2018 offers SaveAs DWG: 2018,2013,2010,2007,2004,2000,r14
//                         DXF: 2018,2013,2010,2007,2004,2000,r12
// libdxfrw dwg2dxf offers R12, v2000, v2004, v2007, v2010
EXPORT Dwg_Version_Type
dwg_version_as (const char *version)
{
  for (int i = R_AFTER - 1; i > 0; i--)
    {
      if (strEQ (dwg_versions[i].type, version))
        return dwg_versions[i].r;
    }
  return R_INVALID;
}

/** The reverse of dwg_version_as (char*) */
EXPORT const char *
dwg_version_type (const Dwg_Version_Type version)
{
  if (version < R_AFTER)
    return dwg_versions[version].type;
  else
    return "invalid after";
}

/** The version from the magic char[6] header.
    The proper release must then be set when we read the dwg_version also. */
EXPORT Dwg_Version_Type
dwg_version_hdr_type (const char *hdr)
{
#ifndef HAVE_NONNULL
  if (!hdr)
    return R_INVALID;
#endif
  for (int i = R_AFTER - 1; i > 0; i--)
    {
      if (strEQ (dwg_versions[i].hdr, hdr))
        return dwg_versions[i].r;
    }
  return R_INVALID;
}

/** The version from the magic char[6] header and the matching dwg_version
 * number. */
Dwg_Version_Type
dwg_version_hdr_type2 (const char *hdr, unsigned dwg_version)
{
#ifndef HAVE_NONNULL
  if (!hdr)
    return R_INVALID;
#endif
  for (int i = R_AFTER - 1; i > 0; i--)
    {
      if (strEQ (dwg_versions[i].hdr, hdr))
        if (!dwg_version || dwg_versions[i].dwg_version == dwg_version)
          return dwg_versions[i].r;
    }
  return R_INVALID;
}

// keep in sync with common.h DWG_BITS
const char *dwg_bits_name[]
    = { "UNKNOWN", "RC",  "RS",  "RL",  "B",       "BB",  "3B",     "4BITS",
        "BS",      "BL",  "BLd", "RLL", "RD",      "BD",  "MC",     "UMC",
        "MS",      "TV",  "TU",  "T",   "TF",      "T32", "HANDLE", "BE",
        "DD",      "BT",  "BOT", "BLL", "TIMEBLL", "CMC", "ENC",    "2RD",
        "3RD",     "2BD", "3BD", "2DD", "3DD",     "CRC", "CRC64",  "RLLd" };

// minimal size of type in bits
// keep in sync with above
// used by unit-tests
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
  64,  //"RLLd",
};

/* replace from ("[rcount1]") with to ("[%d]") in s (e.g.
   "ref[rcount1].classname"). s is a global constant (#nam), so we cannot
   change it in-place.
 */
EXPORT char *
strrplc (const char *s, const char *from, const char *to)
{
  const char *p = strstr (s, from);
  if (p)
    {
      const size_t l1 = strlen (from);
      const size_t l2 = strlen (to);
      char *dest = (char *)CALLOC (1, 80);
      long i = p - s;
      assert (strlen (s) - ((long)l1 - l2) < 80);
      memcpy (dest, s, i);
      strncat (dest, to, 79 - l2);
      strncat (dest, s + i + l1, 79 - l1);
      return dest;
    }
  else
    return NULL;
}

#if !defined(HAVE_MEMMEM) || defined(COMMON_TEST_C)
// naive from scratch implementation, not from glibc.
// see also examples/unknown.c:membits
void *__nonnull ((1, 3))
    my_memmem (const void *h0, size_t k, const void *n0, size_t l)
{
  const unsigned char *h = h0, *n = n0;
  unsigned char *plast;

  if (!l)
    return (void *)h; // empty needle
  if (k < l)
    return NULL; // needle longer than haystack
  h = memchr (h0, *n, k);
  if (!h || l == 1)
    return (void *)h; // first needle char not found
  k -= h - (const unsigned char *)h0;
  if (k < l)
    return NULL; // no room for needle

  plast = (unsigned char *)h + (k - l);
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
const unsigned char _dwg_VISUALSTYLE_proptypes[58] = {
  /* [0]  */ 2, 2, 2, 2,
  /* [4]  */ 3, 3, 4, 2,
  /* [8]  */ 2, 4, 4, 2,
  /* [12] */ 2, 3, 2, 4,
  /* [16] */ 3, 2, 2, 2,
  /* [20] */ 4, 2, 2, 2,
  /* [24] */ 1, 2, 3, 2,
  /* [28] */ 1, 1, 1, 1,
  /* [32] */ 1, 1, 1, 1,
  /* [36] */ 1, 2, 3, 3,
  /* [40] */ 2, 4, 2, 2,
  /* [44] */ 4, 1, 2, 2,
  /* [48] */ 2, 1, 2, 4,
  /* [52] */ 3, 2, 5, 1,
  /* [56] */ 3, 3
};

// returns the first ref from the handle vector.
BITCODE_H
shift_hv (BITCODE_H *hv, BITCODE_BL *num_p)
{
  BITCODE_H ref = hv[0];
  *num_p = *num_p - 1;
  memmove (&hv[0], &hv[1], *num_p * sizeof (BITCODE_H));
  return ref;
}

// delete an entry from an HV ("handle vector") at index i
void
delete_hv (BITCODE_H *entries, BITCODE_BS *num_p, BITCODE_BS i)
{
  BITCODE_H ref;
  BITCODE_BS nume = *num_p;
  assert (i < nume);
  ref = entries[i];
  *num_p = *num_p - 1;
  nume--;
  if (!ref->handleref.is_global)
    FREE (ref);
  if (!nume || i != nume) // not the last?
    {
      memmove (&entries[i], &entries[i + 1], (nume - i) * sizeof (BITCODE_H));
    }
}

// find if handle already exists, returs index or -1
BITCODE_BSd find_hv (BITCODE_H *entries, BITCODE_BS num_entries,
                    BITCODE_RLL handle_value)
{
  BITCODE_BS i;
#ifndef HAVE_NONNULL
  if (!entries || !num_entries)
    return -1; // empty handle vector
#else
  if (!num_entries)
    return -1; // empty handle vector
#endif
  for (i = 0; i < num_entries; i++)
    {
      if (entries[i] && (entries[i]->handleref.value == handle_value))
        return i;
    }
  return -1; // not found
}

/* from my dwg11.c, 1995 - rurban */
struct tm *
cvt_TIMEBLL (struct tm *tm, BITCODE_TIMEBLL date)
{
  double t, ss;
  long ja, jalpha, jb, jc, jd, je;

#define TRUNC(n) (long)floor (n)

  t = 0.864 * date.ms / 1000.0; /*t=1000000 = 1 day, means 86400 in seconds */
  if (date.days > 2299161)
    {
      jalpha = TRUNC (((date.days - 1867216) - 0.25) / 36524.25);
      ja = (long)(date.days + 1 + jalpha - TRUNC (0.25 * jalpha));
    }
  else
    ja = (long)date.days;
  if (ja < 1000)
    {
      // TDINDWG: relative minutes
      memset (tm, 0, sizeof (struct tm));
    }
  else
    {
      jb = ja + 1524;
      jc = TRUNC (6680.0 + ((jb - 2439870) - 122.1) / 365.25);
      jd = 365 * jc + TRUNC (0.25 * jc);
      je = TRUNC ((jb - jd) / 30.6001);

      tm->tm_mday = (int)(jb - jd - TRUNC (30.6001 * je));
      if (tm->tm_mday < 1)
        tm->tm_mday = 1;
      else if (tm->tm_mday > 31)
        tm->tm_mday %= 31;
      tm->tm_mon = (int)(je - 1);
      if (tm->tm_mon > 12)
        tm->tm_mon -= 12;
      tm->tm_year = (int)(jc - 4715);
      if (tm->tm_mon > 2)
        tm->tm_year--;
      if (tm->tm_year <= 0)
        tm->tm_year--;
      tm->tm_year -= 1900; // epoch start
      tm->tm_mon--;        // zero-based
    }
  tm->tm_hour = (int)floor (t / 3600.0);
  t -= tm->tm_hour * 3600.0;
  if (ja >= 1000)
    tm->tm_hour = tm->tm_hour % 24;
  tm->tm_min = (int)floor (t / 60.0);
  ss = t - (tm->tm_min * 60.0);
  tm->tm_sec = (int)ss;
  // sprintf (s, "%02d.%02d.%4d  %02d:%02d:%05.2f", d, m, y, hh, mm, ss);
  return tm;
}

/* in seconds */
long
tm_offset (void)
{
  time_t gmt, rawtime = time (NULL);
  struct tm *tm;

#ifdef HAVE_GMTIME_R
  struct tm gbuf;
  tm = gmtime_r (&rawtime, &gbuf);
#else
  tm = gmtime (&rawtime);
#endif
  // Force mktime() lookup dst in timezone database
  tm->tm_isdst = -1;
  gmt = mktime (tm);
  return (long)difftime (rawtime, gmt);
}

// portability compat funcs

#ifndef HAVE_STRCASECMP
EXPORT int
strcasecmp (const char *a, const char *b)
{
  size_t l2;
  int r1 = strcmp (a, b);
  if (!r1)
    return 0;
  l2 = strlen (b);
  for (size_t i = 0; i < strlen (a); i++)
    {
      if (i > l2)
        return 1;
      if (toupper (a[i]) != toupper (b[i]))
        return toupper (a[i]) < toupper (b[i]);
    }
  return 0;
}
#endif

#ifdef _MSC_VER
EXPORT char *
basename (char *path)
{
  // I've looked at the winsdk basename implementation. It's a clusterfuck,
  // because they have 2-byte widechars. We only accept utf-8 paths
  char *p = strrchr (path, '/');
  if (!p)
    p = strrchr (path, '\\');
  return p;
}
#endif

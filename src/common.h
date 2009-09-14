/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*  http://code.google.com/p/libredwg/                                       */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Felipe Sanches <jucablues@users.sourceforge.net>      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/// Komunaj kaj gxeneralaj funkcioj

#ifndef COMMON_H
#define COMMON_H

typedef enum DWG_VERSION_TYPE
{
  R_BEFORE, R_13, R_14, R_2000, R_2004, R_2007, R_AFTER
} Dwg_Version_Type;
char version_codes[7][7];

/**
 Typej de (eble kompaktitaj) dateneroj, kiuj komponas dwg-dosierojn.
 */
typedef enum DWG_DATENERO_TIPO
{
  DWG_DT_B,   /** bit (1 or 0) */
  DWG_DT_BB,  /** special 2-bit code (entmode in entities, for instance) */
  DWG_DT_RC,  /** raw char (not compressed) */
  DWG_DT_RS,  /** raw short (not compressed) */
  DWG_DT_RD,  /** raw double (not compressed) */
  DWG_DT_RL,  /** raw long (not compressed) */
  DWG_DT_BS,  /** bitshort */
  DWG_DT_BL,  /** bitlong */
  DWG_DT_BD,  /** bitdouble */
  DWG_DT_MC,  /** modular char  */
  DWG_DT_MS,  /** modular short  */
  DWG_DT_BE,  /** BitExtrusion */
  DWG_DT_DD,  /** BitDouble With Default */
  DWG_DT_BT,  /** BitThickness */
  DWG_DT_H,   /** handle reference (see the HANDLE REFERENCES section) */
  DWG_DT_CMC, /** CmColor value */
  DWG_DT_T,   /** text (bitshort length, followed by the string) */
  DWG_DT_TU,  /** Unicode text (bitshort character length, followed by
                 Unicode string, 2 bytes per character). Unicode text is read from the
                 “string stream” within the object data, see the main Object description
                 section for details. */
  DWG_DT_2RD, /** 2 raw doubles **/
  DWG_DT_3RD, /** 3 raw doubles **/
  DWG_DT_2BD, /** 2D point (2 bitdoubles) **/
  DWG_DT_3BD, /** 3D point (3 bitdoubles) **/
  DWG_END_OF_HEADER_VARIABLES
} Dwg_Datenero_Type;

/**
 Identigaj referencoj de sentinels.
 */
typedef enum DWG_SENTINEL
{
  DWG_SENTINEL_HEADER_END,
  DWG_SENTINEL_PICTURE_BEGIN,
  DWG_SENTINEL_PICTURE_END,
  DWG_SENTINEL_VARIABLE_BEGIN,
  DWG_SENTINEL_VARIABLE_END,
  DWG_SENTINEL_CLASS_BEGIN,
  DWG_SENTINEL_CLASS_END,
  DWG_SENTINEL_SECOND_HEADER_BEGIN,
  DWG_SENTINEL_SECOND_HEADER_END
} Dwg_Sentinel;

Dwg_Datenero_Type
dwg_var_map(Dwg_Version_Type version, int index);

unsigned char *
dwg_sentinel(Dwg_Sentinel kiu_sentinel);

#endif

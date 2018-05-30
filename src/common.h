/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * common.c: common general functions and macros
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 */

#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include <stdint.h>
#include <inttypes.h>

#if defined(__clang__) || defined(__clang) || \
       (defined( __GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406)
#  define GCC_DIAG_PRAGMA(x) _Pragma (#x)
/* clang has "clang diagnostic" pragmas, but also understands gcc. */
#  define GCC_DIAG_IGNORE(x) _Pragma("GCC diagnostic push") \
                             GCC_DIAG_PRAGMA(GCC diagnostic ignored #x)
#  define GCC_DIAG_RESTORE   _Pragma("GCC diagnostic pop")
#else
#  define GCC_DIAG_IGNORE(w)
#  define GCC_DIAG_RESTORE
#endif

#define TODO_ENCODER fprintf(stderr, "TODO: Encoder\n");
#define TODO_DECODER fprintf(stderr, "TODO: Decoder\n");

#define VERSION(v) cur_ver = v; if (dat->version == v)
#define NOT_VERSION(v) cur_ver = v; if (dat->version != v)
#define VERSIONS(v1,v2) cur_ver = v2; if (dat->version >= v1 && dat->version <= v2)
#define OTHER_VERSIONS else
#define PRE(v) cur_ver = v; if (dat->version < v)
#define SINCE(v) cur_ver = v; if (dat->version >= v)
#define PRIOR_VERSIONS else
#define UNTIL(v) cur_ver = v; if (dat->version <= v)
#define LATER_VERSIONS else
#define RESET_VER cur_ver = dat->version;

#define DEBUG_HERE()

#define DWG_VERSIONS 20
typedef enum DWG_VERSION_TYPE
{
   R_INVALID,
   R_1_1, R_1_2, R_1_4,  R_2_0,  R_2_1, R_2_5,  R_2_6,  R_9,    R_10,   R_11,
/* MC0.0, AC1.2, AC1.4,  AC1.50, AC2.10, AC1002, AC1003, AC1004, AC1006, AC1009 (also R 12) */
   R_13,  R_14,  R_2000, R_2004, R_2007, R_2010, R_2013, R_2018, R_AFTER
/* AC1012,AC1014,AC1015, AC1018, AC1021, AC1024, AC1027, AC1032, ... */
} Dwg_Version_Type;
extern char version_codes[DWG_VERSIONS][7];

Dwg_Version_Type dwg_version_as(const char *);

/**
 Data types (including compressed forms) used through the project
*/
#if 0
  B,   /** bit (1 or 0) */
  BB,  /** special 2-bit code (entmode in entities, for instance) */
  RC,  /** raw char (not compressed) */
  RS,  /** raw short (not compressed, big-endian) */
  RD,  /** raw double (not compressed, big-endian) */
  RL,  /** raw long (not compressed, big-endian) */
  BS,  /** bitshort */
  BL,  /** bitlong */
  BD,  /** bitdouble */
  MC,  /** modular char  */
  MS,  /** modular short  */
  BE,  /** BitExtrusion */
  DD,  /** BitDouble With Default */
  BT,  /** BitThickness */
  H,   /** handle reference (see the HANDLE REFERENCES section) */
  CMC, /** CmColor value */
  T,   /** text (bitshort length, followed by the string) */
  TU,  /** Unicode text (bitshort character length, followed by
           Unicode string, 2 bytes per character). Unicode text is read from the
           “string stream” within the object data. */
  2RD, /** 2 raw doubles **/
  3RD, /** 3 raw doubles **/
  2BD, /** 2D point (2 bitdoubles) **/
  3BD, /** 3D point (3 bitdoubles) **/
  3B,  /** special 3-bit code R24+ */
  BLL, /** bitlonglong R24+ */
#endif

/**
 * References of sentinels
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

unsigned char *
dwg_sentinel(Dwg_Sentinel sentinel);

#endif

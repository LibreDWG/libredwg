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
 * common.h: common general functions and macros
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#ifndef COMMON_H
#define COMMON_H

#ifdef __XSI_VISIBLE
#  undef __XSI_VISIBLE /* redefined in config.h */
#endif
#include "config.h"
#include <stdint.h>
#include <inttypes.h>
#include "dwg.h"

/* Used warning suppressions:
   CLANG_DIAG_IGNORE (-Wpragma-pack)
   CLANG_DIAG_IGNORE (-Wmissing-prototypes) - also in gcc since 2.95, but not needed
   GCC30_DIAG_IGNORE (-Wformat-nonliteral)
   GCC31_DIAG_IGNORE (-Wdeprecated-declarations)
   GCC_DIAG_RESTORE
   CLANG_DIAG_RESTORE
*/
#if defined(__GNUC__)
#  define CC_DIAG_PRAGMA(x) _Pragma (#  x)
#  define GCC_DIAG_RESTORE   _Pragma ("GCC diagnostic pop")
#  define CLANG_DIAG_IGNORE(w)
#  define CLANG_DIAG_RESTORE
#elif defined(__clang__) || defined(__clang)
#  define CC_DIAG_PRAGMA(x) _Pragma (#  x)
#  define CLANG_DIAG_IGNORE(x)                                                \
     _Pragma ("clang diagnostic push")                                        \
     CC_DIAG_PRAGMA (clang diagnostic ignored #x)
#  define CLANG_DIAG_RESTORE _Pragma ("clang diagnostic pop")
#else
#  define GCC_DIAG_IGNORE(w)
#  define GCC_DIAG_RESTORE
#  define CLANG_DIAG_IGNORE(w)
#  define CLANG_DIAG_RESTORE
#endif
/* for GCC31_DIAG_IGNORE (-Wdeprecated-declarations) */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 310) ||      \
    defined(__clang__) || defined(__clang)
#  define GCC31_DIAG_IGNORE(x)                                                \
     _Pragma ("GCC diagnostic push")                                          \
     CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#else
#  define GCC31_DIAG_IGNORE(w)
#endif
/* for GCC30_DIAG_IGNORE (-Wformat-nonliteral) */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300) ||      \
    defined(__clang__) || defined(__clang)
#  define GCC30_DIAG_IGNORE(x)                                                \
    _Pragma ("GCC diagnostic push")                                           \
    CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#else
#  define GCC30_DIAG_IGNORE(w)
#endif

#ifndef __has_feature
#  define __has_feature(x) 0
#endif

/* The __nonnull function attribute marks pointer arguments which
   must not be NULL.  */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 303)
#  undef __nonnull
#  define __nonnull(params) __attribute__ ((__nonnull__ params))
#  define HAVE_NONNULL
#else
#  undef HAVE_NONNULL
#  define __nonnull(params)
#endif

#ifndef EXPORT
#  if defined(_WIN32) && defined(ENABLE_SHARED)
#    ifdef DLL_EXPORT
#      define EXPORT __declspec(dllexport)
#    else
#      define EXPORT __declspec(dllimport)
#    endif
#  elif defined HAVE_ATTRIBUTE_VISIBILITY_DEFAULT
#    define EXPORT __attribute__ ((visibility ("default")))
#  else
#    define EXPORT
#  endif
#endif

#define TODO_ENCODER fprintf (stderr, "TODO: Encoder\n");
#define TODO_DECODER fprintf (stderr, "TODO: Decoder\n");

#define VERSION(v)                                                            \
  cur_ver = v;                                                                \
  if (dat->version == v)
#define NOT_VERSION(v)                                                        \
  cur_ver = v;                                                                \
  if (dat->version != v)
#define VERSIONS(v1, v2)                                                      \
  cur_ver = v2;                                                               \
  if (dat->version >= v1 && dat->version <= v2)
#define OTHER_VERSIONS else
#define PRE(v)                                                                \
  cur_ver = v;                                                                \
  if (dat->version < v)
#define SINCE(v)                                                              \
  cur_ver = v;                                                                \
  if (dat->version >= v)
#define PRIOR_VERSIONS else
#define UNTIL(v)                                                              \
  cur_ver = v;                                                                \
  if (dat->version <= v)
#define LATER_VERSIONS else
#define RESET_VER cur_ver = dat->version;

#define DEBUG_POS
#define DEBUG_HERE
#define DEBUG_POS_OBJ
#define DEBUG_HERE_OBJ

extern const char version_codes[DWG_VERSIONS][7];

EXPORT Dwg_Version_Type dwg_version_as (const char *);

/**
 Data types (including compressed forms) used through the project
*/
// keep in sync with common.c dwg_bits_name
typedef enum DWG_BITS
{
  BITS_UNKNOWN,
  BITS_RC,      /** raw char (not compressed) */
  BITS_RS,      /** raw 2-byte short (not compressed, big-endian) */
  BITS_RL,      /** raw 4-byte long (not compressed, big-endian) */
  BITS_B,       /** bit (1 or 0) */
  BITS_BB,      /** special 2-bit code (entmode in entities, for instance) */
  BITS_3B,      /** special 3-bit code R24+ */
  BITS_4BITS,   /** 4 bits, r2000+ for VIEWMODE */
  BITS_BS,      /** bitshort */
  BITS_BL,      /** bitlong uint32_t */
  BITS_BLd,     /** signed bitlong int32_t */
  BITS_RLL,     /** raw 8-byte long long (not compressed, big-endian) */
  BITS_RD,      /** raw double (not compressed, big-endian) */
  BITS_BD,      /** bitdouble */
  BITS_MC,      /** modular char */
  BITS_UMC,     /** unsigned modular char, max 4 bytes (handlestream_size) */
  BITS_MS,      /** modular short */
  BITS_TV,      /** text value, -r2007 */
  BITS_TU,      /** Unicode text (bitshort character length, followed by
                    UCS-2 string). Unicode text is read from the
                    “string stream” within the object data. r2007+ */
  BITS_T,       /** text, version dependent: TV or TU */
  BITS_TF,      /** fixed-length text */
  BITS_HANDLE,  /** handle reference (see the HANDLE REFERENCES section) */
  BITS_BE,      /** BitExtrusion */
  BITS_DD,      /** BitDouble With Default */
  BITS_BT,      /** BitThickness */
  BITS_BOT,     /** Bit object type: 2010+ (BB + 1-2RC) */
  BITS_BLL,     /** bitlonglong R24+ */
  BITS_TIMEBLL, /** time long.long */
  BITS_CMC,     /** CmColor value */
  BITS_ENC,     /** Entity CmColor value */
  BITS_2RD,     /** 2 raw doubles **/
  BITS_3RD,     /** 3 raw doubles **/
  BITS_2BD,     /** 2D point (2 bitdoubles) **/
  BITS_3BD,     /** 3D point (3 bitdoubles) **/
  BITS_2DD,     /** 2 doubles with default **/
  BITS_3DD,     /** 3 doubles with default **/
  BITS_CRC,
  BITS_CRC64
} Dwg_Bits;

extern const char *dwg_bits_name[];
extern const unsigned char dwg_bits_size[];

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

unsigned char *dwg_sentinel (Dwg_Sentinel sentinel);
char *strrplc (const char* s, const char *from, const char *to);

#endif

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
   CLANG_DIAG_IGNORE (-Wmissing-prototypes) - also in gcc since 2.95, but not
   needed CLANG_DIAG_RESTORE

   GCC46_DIAG_IGNORE (-Wformat-nonliteral) + GCC46_DIAG_RESTORE
   GCC30_DIAG_IGNORE (-Wformat-nonliteral) w/o
   GCC31_DIAG_IGNORE (-Wdeprecated-declarations)
   GCC30_DIAG_IGNORE (-Wshadow)
*/
#if defined(__GNUC__)
#  define CC_DIAG_PRAGMA(x) _Pragma (#x)
#  define CLANG_DIAG_IGNORE(w)
#  define CLANG_DIAG_RESTORE
#elif defined(__clang__) || defined(__clang)
#  define CC_DIAG_PRAGMA(x) _Pragma (#x)
#  define CLANG_DIAG_IGNORE(x)                                                \
    _Pragma ("clang diagnostic push")                                         \
    CC_DIAG_PRAGMA (clang diagnostic ignored #x)
#  define CLANG_DIAG_RESTORE _Pragma ("clang diagnostic pop")
#  define GCC31_DIAG_IGNORE(w)
#  define GCC30_DIAG_IGNORE(w)
#else
#  define CC_DIAG_IGNORE(w)
#  define CLANG_DIAG_IGNORE(w)
#  define CLANG_DIAG_RESTORE
#endif
/* for GCC46_DIAG_IGNORE (-Wdeprecated-declarations) inside functions */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 460)         \
    || defined(__clang__) || defined(__clang)
#  define GCC46_DIAG_IGNORE(x)                                                \
     _Pragma ("GCC diagnostic push")                                          \
     CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#  define GCC46_DIAG_RESTORE _Pragma ("GCC diagnostic pop")
#else
#  define GCC46_DIAG_IGNORE(w)
#  define GCC46_DIAG_RESTORE
#endif
/* for GCC31_DIAG_IGNORE (-Wdeprecated-declarations) outside functions */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 310          \
     && ((__GNUC__ * 100) + __GNUC_MINOR__) < 460)
#  define GCC31_DIAG_IGNORE(x) CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#else
#  define GCC31_DIAG_IGNORE(w)
#endif
/* For GCC30_DIAG_IGNORE (-Wformat-nonliteral) outside functions */
#if (defined(__GNUC__) && ((__GNUC__ * 100) + __GNUC_MINOR__) >= 300          \
     && ((__GNUC__ * 100) + __GNUC_MINOR__) < 460)
#  define GCC30_DIAG_IGNORE(x) CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#else
#  define GCC30_DIAG_IGNORE(w)
#endif

#ifndef __has_feature
#  define __has_feature(x) 0
#endif

/* */
#if defined(__AFL_COMPILER) && defined(__clang__)
#  define AFL_GCC_TOOBIG __attribute__((optnone))
#  define AFL_GCC_POP
#elif defined(__AFL_COMPILER) && defined(__GNUC__)
#  define AFL_GCC_TOOBIG \
    _Pragma ("GCC push_options") \
    _Pragma ("GCC optimize (\"-fno-var-tracking-assignments\")")
#  define AFL_GCC_POP \
    _Pragma ("GCC pop_options")
#else
#  define AFL_GCC_TOOBIG
#  define AFL_GCC_POP
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

#ifdef HAVE_FUNC_ATTRIBUTE_MALLOC
#  define ATTRIBUTE_MALLOC __attribute__ ((malloc))
#else
#  define ATTRIBUTE_MALLOC
#endif

#ifdef HAVE_FUNC_ATTRIBUTE_RETURNS_NONNULL
#  define RETURNS_NONNULL __attribute__ ((returns_nonnull))
#else
#  define RETURNS_NONNULL
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

#define TODO_ENCODER HANDLER (OUTPUT, "TODO: Encoder\n");
#define TODO_DECODER HANDLER (OUTPUT, "TODO: Decoder\n");

// exporters are more common in the spec format, in_json and in_dxf are not using it.
// so default to the encode-to format. dec_macros needs to override them.
#define VERSION(v)                                                            \
  cur_ver = v;                                                                \
  if (dat->version == v)
#define NOT_VERSION(v)                                                        \
  cur_ver = v;                                                                \
  if (dat->version != v)
#define VERSIONS(v1, v2)                                                      \
  cur_ver = v1;                                                               \
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

#define SAFENAME(name) (name) ? (name) : ""
#define SAFEDXFNAME (obj && obj->dxfname ? obj->dxfname : "")
#define ARRAY_SIZE(arr) (int)(sizeof (arr) / sizeof ((arr)[0]))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

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
  BITS_T32,     /** String32 type */
  BITS_TU32,    /** StringU32 type (FileDepList.features) */
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

/* Globals inside the lib */
extern const char version_codes[DWG_VERSIONS][7];
extern const char *dwg_bits_name[];
extern const unsigned char dwg_bits_size[];

/**
 * References of sentinels
 */
typedef enum DWG_SENTINEL
{
  DWG_SENTINEL_HEADER_END,
  DWG_SENTINEL_THUMBNAIL_BEGIN,
  DWG_SENTINEL_THUMBNAIL_END,
  DWG_SENTINEL_VARIABLE_BEGIN,
  DWG_SENTINEL_VARIABLE_END,
  DWG_SENTINEL_CLASS_BEGIN,
  DWG_SENTINEL_CLASS_END,
  DWG_SENTINEL_SECOND_HEADER_BEGIN,
  DWG_SENTINEL_SECOND_HEADER_END
} Dwg_Sentinel;

enum RES_BUF_VALUE_TYPE
{
  VT_INVALID = 0,
  VT_STRING = 1,
  VT_POINT3D = 2,
  VT_REAL = 3,
  VT_INT16 = 4,
  VT_INT32 = 5,
  VT_INT8 = 6,
  VT_BINARY = 7,
  VT_HANDLE = 8,
  VT_OBJECTID = 9,
  VT_BOOL = 10,
  VT_INT64 = 11, // BLL
};

enum RES_BUF_VALUE_TYPE get_base_value_type (short gc);

unsigned char *dwg_sentinel (Dwg_Sentinel sentinel);
char *strrplc (const char *s, const char *from, const char *to);

#define strEQ(s1, s2) !strcmp ((s1), (s2))
#define strNE(s1, s2) strcmp ((s1), (s2))
#define strEQc(s1, s2) !strcmp ((s1), s2 "")

#define memBEGIN(s1, s2, len) (strlen (s1) >= len && !memcmp (s1, s2, len))
#define memBEGINc(s1, s2)                                                     \
  (strlen (s1) >= sizeof (s2 "") - 1 && !memcmp (s1, s2, sizeof (s2 "") - 1))

#ifndef M_PI_2
#  define M_PI_2 1.57079632679489661923132169163975144
#endif
#define rad2deg(ang) (ang) * 90.0 / M_PI_2
#define deg2rad(ang) (ang) * M_PI_2 / 90.0

#endif

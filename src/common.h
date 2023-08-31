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
 * common.h: common general functions and macros
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#ifndef COMMON_H
#define COMMON_H

#ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE 1 /* for __USE_MISC byteswap macros */
#endif
#ifdef __XSI_VISIBLE
#  undef __XSI_VISIBLE /* redefined in config.h (cygwin strdup only) */
#endif
#include "config.h"
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "dwg.h"

// #pragma pack()
//  use as printf("%" PRIuSIZE ", size)
#ifndef PRI_SIZE_T_MODIFIER
#  ifdef _WIN32
#    if SIZEOF_SIZE_T == 8
#      define PRI_SIZE_T_MODIFIER "ll"
#    else
#      define PRI_SIZE_T_MODIFIER ""
#    endif
#  else
#    if SIZEOF_SIZE_T == 8
#      define PRI_SIZE_T_MODIFIER "l"
#    else
#      define PRI_SIZE_T_MODIFIER ""
#    endif
#  endif
#endif
#define PRIuSIZE PRI_SIZE_T_MODIFIER "u"

// DoS limits
// limit number of vector elements to BS range
#define MAX_NUM 0xFFFE
// strings also 16bits
#define MAX_SIZE_TF 0xFFF0
#define MAX_SIZE_T 0xFFFE
// asan: allocation-size-too-big
#ifdef MAX_MEM
#  define MAX_SIZE_BUF MAX_MEM
#else
#  ifdef HAVE_ASAN
#    define MAX_SIZE_BUF UINT64_C (0x10000000000)
#else
#    define MAX_SIZE_BUF UINT64_C (0x7FFFFFFFFFFFFFFF)
#  endif
#endif

#if !defined AX_STRCASECMP_HEADER && !defined HAVE_STRCASECMP
EXPORT int strcasecmp (const char *a, const char *b);
#else
#  include AX_STRCASECMP_HEADER
#endif

#if defined WORDS_BIGENDIAN && !WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
#endif

#ifdef HAVE_ENDIAN_H
// #pragma message "_DEFAULT_SOURCE: " _DEFAULT_SOURCE
// #pragma message "HAVE_HTOBE64: " HAVE_HTOBE64
// #pragma message "HAVE_BYTESWAP_H: " HAVE_BYTESWAP_H
// #pragma message "__USE_MISC: " __USE_MISC
// #pragma message "LITTLE_ENDIAN: " LITTLE_ENDIAN
#  include <endian.h>
// #pragma message "htobe64: " htobe64
//  centos 7/glibc quirks
#  if defined(HAVE_BYTESWAP_H) && !defined(htobe64)
#    include <byteswap.h>
#    ifndef WORDS_BIGENDIAN
#      define le16toh(x) (x)
#      define htole32(x) (x)
#      define le32toh(x) (x)
#      define htole64(x) (x)
#      define le64toh(x) (x)
#      define htobe16(x) bswap_16 (x)
#      define htobe32(x) bswap_32 (x)
#      define htobe64(x) bswap_64 (x)
#      define be64toh(x) bswap_64 (x)
#    else
#      define le16toh(x) bswap_16 (x)
#      define htole32(x) bswap_32 (x)
#      define le32toh(x) bswap_32 (x)
#      define htole64(x) bswap_64 (x)
#      define le64toh(x) bswap_64 (x)
#      define htobe16(x) (x)
#      define htobe32(x) (x)
#      define htobe64(x) (x)
#      define be64toh(x) (x)
#    endif
#  endif
#elif defined HAVE_SYS_ENDIAN_H
#  include <sys/endian.h>
#elif defined HAVE_MACHINE_ENDIAN_H && defined __APPLE__
#  include <machine/endian.h>
#  include <libkern/OSByteOrder.h>
#  define le16toh OSSwapLittleToHostInt16
#  define htole32 OSSwapHostToLittleInt32
#  define le32toh OSSwapLittleToHostInt32
#  define htole64 OSSwapHostToLittleInt64
#  define le64toh OSSwapLittleToHostInt64
#  define htobe16 OSSwapHostToBigInt16
#  define htobe32 OSSwapHostToBigInt32
#  define htobe64 OSSwapHostToBigInt64
#  define be64toh OSSwapBigToHostInt64
#elif defined HAVE_WINSOCK2_H && defined __WINDOWS__
#  include <winsock2.h>
#  ifndef WORDS_BIGENDIAN
#    define le16toh(x) (x)
#    define htole32(x) (x)
#    define le32toh(x) (x)
#    define htole64(x) (x)
#    define le64toh(x) (x)
#    define htobe16(x) __builtin_bswap16 (x)
#    define htobe32(x) __builtin_bswap32 (x)
#    define htobe64(x) __builtin_bswap64 (x)
#    define be64toh(x) __builtin_bswap64 (x)
#  else /* e.g. xbox 360 */
#    define le16toh(x) __builtin_bswap16 (x)
#    define htole32(x) __builtin_bswap32 (x)
#    define le32toh(x) __builtin_bswap32 (x)
#    define htole64(x) __builtin_bswap64 (x)
#    define le64toh(x) __builtin_bswap64 (x)
#    define htobe16(x) (x)
#    define htobe32(x) (x)
#    define htobe64(x) (x)
#    define be64toh(x) (x)
#  endif
#elif defined WORDS_BIGENDIAN
/* TODO more converters */
#  if defined HAVE_SYS_PARAM_H
#    include <sys/param.h>
#  endif
#  if defined HAVE_SYS_BYTEORDER_H
/* e.g. solaris */
#    include <sys/byteorder.h>
#    define le16toh(x) BSWAP_16 (x)
#    define htole32(x) BSWAP_32 (x)
#    define le32toh(x) BSWAP_32 (x)
#    define htole64(x) BSWAP_64 (x)
#    define le64toh(x) BSWAP_64 (x)
#    define htobe16(x) (x)
#    define htobe32(x) (x)
#    define htobe64(x) (x)
#    define be64toh(x) (x)
#  elif defined HAVE_BYTESWAP_H
#    include <byteswap.h>
#    define le16toh(x) bswap16 (x)
#    define htole32(x) bswap32 (x)
#    define le32toh(x) bswap32 (x)
#    define htole64(x) bswap64 (x)
#    define le64toh(x) bswap64 (x)
#    define htobe16(x) (x)
#    define htobe32(x) (x)
#    define htobe64(x) (x)
#    define be64toh(x) (x)
#  elif defined HAVE_BYTEORDER_H
#    include <byteorder.h>
/* which os? riot-os */
#    ifdef RIOT_VERSION
#      define le16toh(x) byteorder_swap (x)
#      define htole32(x) byteorder_swapl (x)
#      define le32toh(x) byteorder_swapl (x)
#      define htole64(x) byteorder_swapll (x)
#      define le64toh(x) byteorder_swapll (x)
#      define htobe16(x) (x)
#      define htobe32(x) (x)
#      define htobe64(x) (x)
#      define be64toh(x) (x)
#    else
/* rtems/libcpu: ... */
#      error unsupported big-endian platform with byteorder.h
#    endif
#  else
#    error unsupported big-endian platform
#  endif
#else /* little endian: just pass-thru. i.e. mingw */
#  define NO_BYTESWAP_SUPPORT
// Warning: evaluates x times!
#  define bswap_constant_16(x) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8))
#  define bswap_constant_32(x)                                                \
    ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8)                   \
     | (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))
#  define bswap_constant_64(x)                                                \
    ((((x) & 0xff00000000000000ULL) >> 56)                                    \
     | (((x) & 0x00ff000000000000ULL) >> 40)                                  \
     | (((x) & 0x0000ff0000000000ULL) >> 24)                                  \
     | (((x) & 0x000000ff00000000ULL) >> 8)                                   \
     | (((x) & 0x00000000ff000000ULL) << 8)                                   \
     | (((x) & 0x0000000000ff0000ULL) << 24)                                  \
     | (((x) & 0x000000000000ff00ULL) << 40)                                  \
     | (((x) & 0x00000000000000ffULL) << 56))

#  define le16toh(x) (x)
#  define htole32(x) (x)
#  define le32toh(x) (x)
#  define htole64(x) (x)
#  define le64toh(x) (x)
#  define htobe16(x) bswap_constant_16 (x)
#  define htobe32(x) bswap_constant_32 (x)
#  define htobe64(x) bswap_constant_64 (x)
#  define be64toh(x) bswap_constant_64 (x)
#endif

#ifdef ENABLE_MIMALLOC
#  include <mimalloc-override.h>
#endif

/* Used warning suppressions:
   CLANG_DIAG_IGNORE (-Wpragma-pack)
   CLANG_DIAG_IGNORE (-Wmissing-prototypes) - also in gcc since 2.95, but not
   needed CLANG_DIAG_RESTORE

   GCC80_DIAG_IGNORE (-Wmaybe-uninitialized)
   GCC80_DIAG_IGNORE (-Wstringop-truncation)
   GCC80_DIAG_IGNORE (-Wstringop-overflow)
   GCC46_DIAG_IGNORE (-Wformat-nonliteral) + GCC46_DIAG_RESTORE
   GCC46_DIAG_IGNORE (-Wmissing-field-initializers) 4.7.1
   GCC30_DIAG_IGNORE (-Wformat-nonliteral) w/o
   GCC31_DIAG_IGNORE (-Wdeprecated-declarations)
   GCC30_DIAG_IGNORE (-Wshadow)
*/
#if defined(__GNUC__)
#  define _GNUC_VERSION ((__GNUC__ * 100) + __GNUC_MINOR__)
#  define CC_DIAG_PRAGMA(x) _Pragma (#x)
#else
#  define _GNUC_VERSION 0
#  define CC_DIAG_PRAGMA(x)
#endif

#define _STR(s) #s
#define _XSTR(s) _STR (s)
// #pragma message("_GNUC_VERSION " _XSTR(_GNUC_VERSION))

// clang-specifics (rarely needed, as they mimic GCC diagnostics closely, even
// down to bugs)
#if defined(__clang__) || defined(__clang)
#  define HAVE_CLANG
#  define CLANG_DIAG_IGNORE(x)                                                \
    _Pragma ("clang diagnostic push")                                         \
        CC_DIAG_PRAGMA (clang diagnostic ignored #x)
#  define CLANG_DIAG_RESTORE _Pragma ("clang diagnostic pop")
#elif defined(__GNUC__)
#  define CLANG_DIAG_IGNORE(w)
#  define CLANG_DIAG_RESTORE
#else
// MSVC has the __pragma() macro instead
#  define CLANG_DIAG_IGNORE(w)
#  define CLANG_DIAG_RESTORE
#endif

/* for GCC14_DIAG_IGNORE (-Wanalyzer-allocation-size) or -Wanalyzer-malloc-leak
   https://cwe.mitre.org/data/definitions/131.html
 */
#if _GNUC_VERSION >= 1400
#  define GCC14_DIAG_IGNORE(x)                                                \
    _Pragma ("GCC diagnostic push") CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#  define GCC14_DIAG_RESTORE _Pragma ("GCC diagnostic pop")
#else
#  define GCC14_DIAG_IGNORE(w)
#  define GCC14_DIAG_RESTORE
#endif

/* for GCC80_DIAG_IGNORE (-Wstringop-truncation)
   https://gcc.gnu.org/bugzilla/show_bug.cgi?id=88780
 */
#if _GNUC_VERSION >= 800
#  define GCC80_DIAG_IGNORE(x)                                                \
    _Pragma ("GCC diagnostic push") CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#  define GCC80_DIAG_RESTORE _Pragma ("GCC diagnostic pop")
#else
#  define GCC80_DIAG_IGNORE(w)
#  define GCC80_DIAG_RESTORE
#endif

/* for GCC46_DIAG_IGNORE (-Wdeprecated-declarations) or (-Wformat-nonliteral),
   stacked inside functions.
   clang 10.2 defines gcc compat version 4.2 though (402) */
#if _GNUC_VERSION >= 460 || (defined(HAVE_CLANG) && _GNUC_VERSION >= 400)
#  define HAVE_CC_DIAG_STACK
#  define GCC46_DIAG_IGNORE(x)                                                \
    _Pragma ("GCC diagnostic push") CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#  define GCC46_DIAG_RESTORE _Pragma ("GCC diagnostic pop")
#else
#  undef HAVE_CC_DIAG_STACK
#  define GCC46_DIAG_IGNORE(w)
#  define GCC46_DIAG_RESTORE
#endif

/* For GCC30_DIAG_IGNORE (-Wformat-nonliteral) outside functions */
#if _GNUC_VERSION >= 300 && !defined HAVE_CC_DIAG_STACK
#  define GCC30_DIAG_IGNORE(x) CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#else
#  define GCC30_DIAG_IGNORE(w)
#endif
/* for GCC31_DIAG_IGNORE (-Wdeprecated-declarations) outside functions */
#if _GNUC_VERSION >= 310 && !defined HAVE_CC_DIAG_STACK
#  define GCC31_DIAG_IGNORE(x) CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#else
#  define GCC31_DIAG_IGNORE(w)
#endif
/* for GCC33_DIAG_IGNORE (-Wswitch-enum) outside functions
   -Wswitch-enum appeared first with gcc 3.3.6 */
#if _GNUC_VERSION >= 330 && !defined HAVE_CC_DIAG_STACK
#  define GCC33_DIAG_IGNORE(x) CC_DIAG_PRAGMA (GCC diagnostic ignored #x)
#else
#  define GCC33_DIAG_IGNORE(w)
#endif

#ifndef __has_feature
#  define __has_feature(x) 0
#endif

/* */
#if defined(__AFL_COMPILER) && defined(__clang__)
#  define AFL_GCC_TOOBIG __attribute__ ((optnone))
#  define AFL_GCC_POP
#elif defined(__AFL_COMPILER) && defined(__GNUC__)
#  define AFL_GCC_TOOBIG                                                      \
    _Pragma ("GCC push_options")                                              \
        _Pragma ("GCC optimize (\"-fno-var-tracking-assignments\")")
#  define AFL_GCC_POP _Pragma ("GCC pop_options")
#else
#  define AFL_GCC_TOOBIG
#  define AFL_GCC_POP
#endif

/* The __nonnull function attribute marks pointer arguments which
   must not be NULL.  */
#if _GNUC_VERSION >= 303 && !defined(__cplusplus)
#  undef __nonnull
#  define __nonnull(params) __attribute__ ((__nonnull__ params))
#  define __nonnull_all __attribute__ ((__nonnull__))
#  define HAVE_NONNULL
#else
#  ifndef __nonnull
#    define __nonnull(params)
#    undef HAVE_NONNULL
#  else
#    define HAVE_NONNULL
#  endif
#  define __nonnull_all
#endif

#if HAVE_FUNC_ATTRIBUTE_ALIGNED
#  define ATTRIBUTE_ALIGNED(num) __attribute__ ((aligned (num)))
#elif defined(_MSC_VER)
#  define ATTRIBUTE_ALIGNED(num) __declspec (align (num))
#else
#  define ATTRIBUTE_ALIGNED(num)
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

#ifdef HAVE_FUNC_ATTRIBUTE_NORETURN
#  define ATTRIBUTE_NORETURN __attribute__ ((noreturn))
#elif defined(_MSC_VER)
#  define ATTRIBUTE_NORETURN __declspec (noreturn)
#else
#  define ATTRIBUTE_NORETURN
#endif

#ifndef __counted_by
#  ifdef HAVE_FUNC_ATTRIBUTE_COUNTED_BY
#    define __counted_by(x) __attribute__ ((__counted_by__ (x)))
#  elif defined(_MSC_VER)
#    define __counted_by(x)
#  else
#    define __counted_by(x)
#  endif
#endif

#if defined(_WIN32) && defined(HAVE_FUNC_ATTRIBUTE_MS_FORMAT)                 \
    && !defined(__USE_MINGW_ANSI_STDIO)
#  define ATTRIBUTE_FORMAT(x, y) __attribute__ ((format (ms_printf, x, y)))
#elif defined HAVE_FUNC_ATTRIBUTE_GNU_FORMAT
#  define ATTRIBUTE_FORMAT(x, y) __attribute__ ((format (gnu_printf, x, y)))
#elif defined HAVE_FUNC_ATTRIBUTE_FORMAT
#  define ATTRIBUTE_FORMAT(x, y) __attribute__ ((format (printf, x, y)))
#else
#  define ATTRIBUTE_FORMAT(x, y)
#endif

#ifndef EXPORT
#  if defined(_WIN32) && defined(ENABLE_SHARED)
#    ifdef DLL_EXPORT
#      define EXPORT __declspec (dllexport)
#    else
#      define EXPORT __declspec (dllimport)
#    endif
#  elif defined HAVE_ATTRIBUTE_VISIBILITY_DEFAULT
#    define EXPORT __attribute__ ((visibility ("default")))
#  else
#    define EXPORT
#  endif
#endif

#undef CAN_ACIS_IN_DS_DATA
#undef CAN_ACIS_HISTORY
#define TODO_ENCODER HANDLER (OUTPUT, "TODO: Encoder\n");
#define TODO_DECODER HANDLER (OUTPUT, "TODO: Decoder\n");

#ifndef _WIN32
#  define STRFTIME_DATE "%F %T"
#  define STRFTIME_TIME "%T"
#  define STRFTIME_DURATION "%e days and %T"
#else
/* windows/mingw misses those C99 formats */
#  define STRFTIME_DATE "%Y-%m-%d %X"
#  define STRFTIME_TIME "%X"
#  define STRFTIME_DURATION "%d days and %X"
#endif

// Exporters are more common in the spec format, in_json and in_dxf are not
// using it. So default to the encode-to format. dec_macros needs to override
// them. See importer.h for the other way: For decode, in_json, in_dxf.
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

#ifdef __cplusplus
extern "C"
{
#endif

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
  BITS_CRC64,
  BITS_RLLd
} Dwg_Bits;

/* Globals inside the lib */
#ifndef COMMON_C
extern const char version_codes[DWG_VERSIONS][7];
extern const char *dwg_bits_name[];
extern const unsigned char dwg_bits_size[];
#endif

/**
 * Index of sentinels into sentinels[]
 */
typedef enum DWG_SENTINEL
{
  DWG_SENTINEL_HEADER_END = 0,
  DWG_SENTINEL_THUMBNAIL_BEGIN,
  DWG_SENTINEL_THUMBNAIL_END,
  DWG_SENTINEL_VARIABLE_BEGIN,
  DWG_SENTINEL_VARIABLE_END,
  DWG_SENTINEL_CLASS_BEGIN,
  DWG_SENTINEL_CLASS_END,
  DWG_SENTINEL_2NDHEADER_BEGIN,
  DWG_SENTINEL_2NDHEADER_END,
  DWG_SENTINEL_R11_ENTITIES_BEGIN,
  DWG_SENTINEL_R11_ENTITIES_END,
  DWG_SENTINEL_R11_BLOCK_BEGIN,
  DWG_SENTINEL_R11_BLOCK_END,
  DWG_SENTINEL_R11_LAYER_BEGIN,
  DWG_SENTINEL_R11_LAYER_END,
  DWG_SENTINEL_R11_STYLE_BEGIN,
  DWG_SENTINEL_R11_STYLE_END,
  DWG_SENTINEL_R11_LTYPE_BEGIN,
  DWG_SENTINEL_R11_LTYPE_END,
  DWG_SENTINEL_R11_VIEW_BEGIN,
  DWG_SENTINEL_R11_VIEW_END,
  DWG_SENTINEL_R11_UCS_BEGIN,
  DWG_SENTINEL_R11_UCS_END,
  DWG_SENTINEL_R11_VPORT_BEGIN,
  DWG_SENTINEL_R11_VPORT_END,
  DWG_SENTINEL_R11_APPID_BEGIN,
  DWG_SENTINEL_R11_APPID_END,
  DWG_SENTINEL_R11_DIMSTYLE_BEGIN,
  DWG_SENTINEL_R11_DIMSTYLE_END,
  DWG_SENTINEL_R11_VX_BEGIN,
  DWG_SENTINEL_R11_VX_END,
  DWG_SENTINEL_R11_BLOCK_ENTITIES_BEGIN,
  DWG_SENTINEL_R11_BLOCK_ENTITIES_END,
  DWG_SENTINEL_R11_EXTRA_ENTITIES_BEGIN,
  DWG_SENTINEL_R11_EXTRA_ENTITIES_END,
  DWG_SENTINEL_R11_AUXHEADER_BEGIN,
  DWG_SENTINEL_R11_AUXHEADER_END
} Dwg_Sentinel;

const unsigned char *dwg_sentinel (const Dwg_Sentinel sentinel_id);
// used by unit-tests
EXPORT char *strrplc (const char *s, const char *from,
                      const char *to) __nonnull_all;

#define strEQ(s1, s2) !strcmp ((s1), (s2))
#define strNE(s1, s2) (strcmp ((s1), (s2)) != 0)
#define strEQc(s1, s2) !strcmp ((s1), s2 "")
#define strNEc(s1, s2) (strcmp ((s1), s2 "") != 0)

#define memBEGIN(s1, s2, len) (strlen (s1) >= len && !memcmp (s1, s2, len))
#define memBEGINc(s1, s2)                                                     \
  (strlen (s1) >= sizeof (s2 "") - 1 && !memcmp (s1, s2, sizeof (s2 "") - 1))

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#  define M_PI_2 1.57079632679489661923132169163975144
#endif
#define rad2deg(ang) (ang) * 90.0 / M_PI_2
#define deg2rad(ang) (ang) * M_PI_2 / 90.0

#if !defined(HAVE_MEMMEM) || defined(COMMON_TEST_C)
// only if _GNU_SOURCE
void *my_memmem (const void *h0, size_t k, const void *n0, size_t l)
    __nonnull ((1, 3));
#  define memmem my_memmem
#elif !defined(_GNU_SOURCE) && defined(IS_DECODER) && !defined(__linux__)
/* HAVE_MEMMEM and _GNU_SOURCE are unreliable on non-Linux systems.
   This fails on FreeBSD and macos.
   Rather declare it by ourselves, and don't use _GNU_SOURCE. */
void *memmem (const void *h0, size_t k, const void *n0, size_t l)
    __nonnull ((1, 3));
#endif

// push to handle vector at the end. It really is unshift.
#define PUSH_HV(_obj, numfield, hvfield, ref)                                 \
  if (_obj->numfield <= 0 || _obj->hvfield[_obj->numfield - 1] != ref)        \
    {                                                                         \
      _obj->hvfield = (BITCODE_H *)realloc (                                  \
          _obj->hvfield, (_obj->numfield + 1) * sizeof (BITCODE_H));          \
      _obj->hvfield[_obj->numfield] = ref;                                    \
      LOG_TRACE ("%s[%d] = " FORMAT_REF " [H]\n", #hvfield, _obj->numfield,   \
                 ARGS_REF (_obj->hvfield[_obj->numfield]));                   \
      _obj->numfield++;                                                       \
    }

// push to handle vector at the end if it not already exists.
#define PUSH_HV_NEW(_obj, numfield, hvfield, ref)                             \
  if (_obj->numfield <= 0                                                     \
      || find_hv ( _obj->hvfield, _obj->numfield, ref->absolute_ref) < 0)     \
    {                                                                         \
      _obj->hvfield = (BITCODE_H *)realloc (                                  \
          _obj->hvfield, (_obj->numfield + 1) * sizeof (BITCODE_H));          \
      _obj->hvfield[_obj->numfield] = ref;                                    \
      LOG_TRACE ("%s[%d] = " FORMAT_REF " [H]\n", #hvfield, _obj->numfield,   \
                 ARGS_REF (_obj->hvfield[_obj->numfield]));                   \
      _obj->numfield++;                                                       \
    }

// no need to free global handles, just the HV.
// returns the last
#define POP_HV(_obj, numfield, hvfield) _obj->hvfield[--_obj->numfield]
// returns the first
#define SHIFT_HV(_obj, numfield, hvfield)                                     \
  shift_hv (_obj->hvfield, &_obj->numfield)
BITCODE_H shift_hv (BITCODE_H *hv, BITCODE_BL *num_p) __nonnull_all;
// deletes an entry from an HV ("handle vector") at index i
void delete_hv (BITCODE_H *entries, BITCODE_BS *num_entriesp,
                BITCODE_BS index) __nonnull_all;
BITCODE_BSd find_hv (BITCODE_H *entries, BITCODE_BS num_entries,
                     BITCODE_RLL handle_value) __nonnull_all;

// used in dwg.spec
Dwg_Object *
dwg_find_first_type (const Dwg_Data *restrict dwg,
                     const enum DWG_OBJECT_TYPE type) __nonnull_all;
Dwg_Object *dwg_find_last_type (const Dwg_Data *restrict dwg,
                                const enum DWG_OBJECT_TYPE type) __nonnull_all;
Dwg_Handle *
dwg_find_first_type_handle (const Dwg_Data *restrict dwg,
                            const enum DWG_OBJECT_TYPE type) __nonnull_all;
// from dwg.c
void dwg_calc_hookline_on (Dwg_Entity_LEADER *_obj);

// <path-to>/dxf.ext => copy of "dxf", "ext"
// Returns a malloc'ed copy of basename, and
// sets ext to the char behind the last "." of filepath
char *split_filepath (const char *filepath, char **extp) __nonnull_all;

const struct dwg_versions *dwg_version_struct (const Dwg_Version_Type version);
/* Returns the AC header magic string [6] */
const char *dwg_version_codes (const Dwg_Version_Type version);
/* Finds version from the magic char[6] header and the matching
   dwg_version number. */
Dwg_Version_Type dwg_version_hdr_type2 (const char *hdr,
                                        unsigned dwg_version) __nonnull_all;

// converts TIMEBLL to struct tm. useful for asctime(tm) or strftime
struct tm *cvt_TIMEBLL (struct tm *tm, BITCODE_TIMEBLL date) __nonnull_all;

/* Exported functions not in the API */
const char *dwg_ref_tblname (const Dwg_Data *restrict dwg,
                             Dwg_Object_Ref *restrict ref,
                             int *alloced) __nonnull_all;
const char *dwg_ref_objname (const Dwg_Data *restrict dwg,
                             Dwg_Object_Ref *restrict ref) __nonnull_all;

int dwg_sections_init (Dwg_Data *dwg) __nonnull_all;

/* in seconds */
long tm_offset (void);
void dwg_log_proxyflag (const int _loglevel, const int maxlevel,
                        const BITCODE_BS flag);
void dwg_log_dataflags (const int _loglevel, const int maxlevel,
                        const BITCODE_RC flag);
// in encode, but also in dwg.spec
void
dwg_convert_LTYPE_strings_area (const Dwg_Data *restrict dwg,
                                Dwg_Object_LTYPE *restrict _obj) __nonnull_all;

// in the public API, but we don't use that for most internal modules
#if !defined _DWG_API_H_ && !defined _DWG_API_C && !defined DYNAPI_TEST_C     \
    && !defined ADD_TEST_C && !defined DXF_TEST_C
bool dwg_is_valid_tag (const char *tag) __nonnull_all;
#endif

bool dwg_has_eed_appid (Dwg_Object_Object *restrict obj,
                        const BITCODE_RLL absref) __nonnull_all;

#ifdef __cplusplus
}
#endif

#endif

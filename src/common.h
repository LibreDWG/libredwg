/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2009-2023 Free Software Foundation, Inc.                   */
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
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <time.h>
#include "dwg.h"

#if !defined AX_STRCASECMP_HEADER && !defined HAVE_STRCASECMP
EXPORT int strcasecmp (const char *a, const char *b);
#endif

#if defined WORDS_BIGENDIAN && !WORDS_BIGENDIAN
# undef WORDS_BIGENDIAN
#endif

#ifdef HAVE_ENDIAN_H
#  ifndef _DEFAULT_SOURCE
#    define _DEFAULT_SOURCE 1 /* for byteswap.h */
#  endif
#  include <endian.h>
// centos 7 quirks
#  if !defined(HAVE_BE64TOH) && defined(HAVE_BYTESWAP_H) && !defined(be64toh)
#    include <byteswap.h>
#    ifndef WORDS_BIGENDIAN
#      define htole32(x) (x)
#      define le32toh(x) (x)
#      define htole64(x) (x)
#      define le64toh(x) (x)
#      define htobe64(x) bswap_64(x)
#      define be64toh(x) bswap_64(x)
#    else
#      define htole32(x) bswap_32(x)
#      define le32toh(x) bswap_32(x)
#      define htole64(x) bswap_64(x)
#      define le64toh(x) bswap_64(x)
#      define htobe64(x) (x)
#      define be64toh(x) (x)
#    endif
#  endif
#elif defined HAVE_SYS_ENDIAN_H
#  include <sys/endian.h>
#elif defined HAVE_MACHINE_ENDIAN_H && defined __APPLE__
#  include <machine/endian.h>
#  include <libkern/OSByteOrder.h>
#  define htole32 OSSwapHostToLittleInt32
#  define le32toh OSSwapLittleToHostInt32
#  define htole64 OSSwapHostToLittleInt64
#  define le64toh OSSwapLittleToHostInt64
#  define htobe64 OSSwapHostToBigInt64
#  define be64toh OSSwapBigToHostInt64
#elif defined HAVE_WINSOCK2_H && defined __WINDOWS__
#  include <winsock2.h>
#  ifndef WORDS_BIGENDIAN
#    define htole32(x) (x)
#    define le32toh(x) (x)
#    define htole64(x) (x)
#    define le64toh(x) (x)
#    define htobe64(x) __builtin_bswap64(x)
#    define be64toh(x) __builtin_bswap64(x)
#  else /* e.g. xbox 360 */
#    define htole32(x) __builtin_bswap32(x)
#    define le32toh(x) __builtin_bswap32(x)
#    define htole64(x) __builtin_bswap64(x)
#    define le64toh(x) __builtin_bswap64(x)
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
#    define htole32(x) BSWAP_32(x)
#    define le32toh(x) BSWAP_32(x)
#    define htole64(x) BSWAP_64(x)
#    define le64toh(x) BSWAP_64(x)
#    define htobe64(x) (x)
#    define be64toh(x) (x)
#  elif defined HAVE_BYTESWAP_H
#    include <byteswap.h>
#    define htole32(x) bswap32 (x)
#    define le32toh(x) bswap32 (x)
#    define htole64(x) bswap64 (x)
#    define le64toh(x) bswap64 (x)
#    define htobe64(x) (x)
#    define be64toh(x) (x)
#  elif defined HAVE_BYTEORDER_H
#    include <byteorder.h>
/* which os? riot-os */
#    ifdef RIOT_VERSION
#      define htole32(x) byteorder_swapl (x)
#      define le32toh(x) byteorder_swapl (x)
#      define htole64(x) byteorder_swapll (x)
#      define le64toh(x) byteorder_swapll (x)
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
#  define htole32(x) (x)
#  define le32toh(x) (x)
#  define htole64(x) (x)
#  define le64toh(x) (x)
// for htobe64, be64toh see bits.h
#endif

#ifdef ENABLE_MIMALLOC
#  include <mimalloc-override.h>
#endif

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
#  define _GNUC_VERSION ((__GNUC__ * 100) + __GNUC_MINOR__)
#  define CC_DIAG_PRAGMA(x) _Pragma (#  x)
#else
#  define _GNUC_VERSION 0
#  define CC_DIAG_PRAGMA(x)
#endif

/*
#define _STR(x)  #x
#define STR(x)_STR(x)
#pragma message("_GNUC_VERSION " STR(_GNUC_VERSION))
*/

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
   -Wswitch-enum appeared first with gcc 3.3.6
 */
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
#else
#  define ATTRIBUTE_NORETURN
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

#undef CAN_ACIS_IN_DS_DATA
#undef CAN_ACIS_HISTORY
#define TODO_ENCODER HANDLER (OUTPUT, "TODO: Encoder\n");
#define TODO_DECODER HANDLER (OUTPUT, "TODO: Decoder\n");

#ifndef _WIN32
#  define STRFTIME_DATE "%F %T"
#  define STRFTIME_TIME "%T"
#else
/* windows/mingw misses those C99 formats */
#  define STRFTIME_DATE "%Y-%m-%d %X"
#  define STRFTIME_TIME "%X"
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
  DWG_SENTINEL_SECOND_HEADER_BEGIN,
  DWG_SENTINEL_SECOND_HEADER_END,
  DWG_SENTINEL_R11_ENTITIES_BEGIN,
  DWG_SENTINEL_R11_ENTITIES_END,
  DWG_SENTINEL_R11_BLOCKS_BEGIN,
  DWG_SENTINEL_R11_BLOCKS_END,
  DWG_SENTINEL_R11_LAYERS_BEGIN,
  DWG_SENTINEL_R11_LAYERS_END,
  DWG_SENTINEL_R11_STYLES_BEGIN,
  DWG_SENTINEL_R11_STYLES_END,
  DWG_SENTINEL_R11_LINETYPES_BEGIN,
  DWG_SENTINEL_R11_LINETYPES_END,
  DWG_SENTINEL_R11_VIEWS_BEGIN,
  DWG_SENTINEL_R11_VIEWS_END,
  DWG_SENTINEL_R11_UCS_BEGIN,
  DWG_SENTINEL_R11_UCS_END,
  DWG_SENTINEL_R11_VPORTS_BEGIN,
  DWG_SENTINEL_R11_VPORTS_END,
  DWG_SENTINEL_R11_APPIDS_BEGIN,
  DWG_SENTINEL_R11_APPIDS_END,
  DWG_SENTINEL_R11_DIMSTYLES_BEGIN,
  DWG_SENTINEL_R11_DIMSTYLES_END,
  DWG_SENTINEL_R11_VXS_BEGIN,
  DWG_SENTINEL_R11_VXS_END,
  DWG_SENTINEL_R11_BLOCK_ENTITIES_BEGIN,
  DWG_SENTINEL_R11_BLOCK_ENTITIES_END,
  DWG_SENTINEL_R11_EXTRA_ENTITIES_BEGIN,
  DWG_SENTINEL_R11_EXTRA_ENTITIES_END,
  DWG_SENTINEL_R11_AUX_HEADER_BEGIN,
  DWG_SENTINEL_R11_AUX_HEADER_END
} Dwg_Sentinel;

const unsigned char *dwg_sentinel (const Dwg_Sentinel sentinel_id);
// used by unit-tests
EXPORT char *strrplc (const char *s, const char *from, const char *to)
  __nonnull_all;

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
  {                                                                           \
    _obj->hvfield = (BITCODE_H *)realloc (                                    \
        _obj->hvfield, (_obj->numfield + 1) * sizeof (BITCODE_H));            \
    _obj->hvfield[_obj->numfield] = ref;                                      \
    LOG_TRACE ("%s[%d] = " FORMAT_REF " [H]\n", #hvfield, _obj->numfield,     \
               ARGS_REF (_obj->hvfield[_obj->numfield]));                     \
    _obj->numfield++;                                                         \
  }

// no need to free global handles, just the HV.
// returns the last
#define POP_HV(_obj, numfield, hvfield) _obj->hvfield[--_obj->numfield]
// returns the first
#define SHIFT_HV(_obj, numfield, hvfield)                                     \
  shift_hv (_obj->hvfield, &_obj->numfield)
BITCODE_H shift_hv (BITCODE_H *hv, BITCODE_BL *num_p) __nonnull_all;

// used in dwg.spec
Dwg_Handle *dwg_find_first_type_handle (Dwg_Data *restrict dwg,
                                        enum DWG_OBJECT_TYPE type) __nonnull_all;

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
                             Dwg_Object_Ref *restrict ref, int *alloced)
  __nonnull_all;
const char *dwg_ref_objname (const Dwg_Data *restrict dwg,
                             Dwg_Object_Ref *restrict ref) __nonnull_all;

int dwg_sections_init (Dwg_Data *dwg) __nonnull_all;

/* in seconds */
long tm_offset (void);
void dwg_log_proxyflag (const int _loglevel, const int maxlevel,
                        const BITCODE_BS flag);
void dwg_log_dataflags (const int _loglevel, const int maxlevel,
                        const BITCODE_RC flag);

// in the public API, but we don't use that for most internal modules
#if !defined _DWG_API_H_ && !defined _DWG_API_C && !defined DYNAPI_TEST_C && !defined ADD_TEST_C && !defined DXF_TEST_C
bool dwg_is_valid_tag (const char *tag) __nonnull_all;
#endif

#endif

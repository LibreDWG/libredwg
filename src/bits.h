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
 * bits.c: low level read and write function prototypes
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

/**
 The position of bits within bytes is numerically ordered as depicted below:

\code
 position: 01234567 01234567 01234567 ...
 bits:     76543210 76543210 76543210 ...
           \______/ \______/ \______/
            byte 1   byte 2   byte 3  ...
\endcode
(i.e. little endian)
 */

#ifndef BITS_H
#define BITS_H

#include "config.h"
#ifdef HAVE_WCHAR_H
// cross-compilation problem:
// /usr/lib/gcc/arm-linux-gnueabi/9/include-fixed/limits.h defines it as 1
#  if defined(MB_LEN_MAX) && MB_LEN_MAX != 16 && MB_LEN_MAX != 32
#    undef MB_LEN_MAX
#    define MB_LEN_MAX 16
#  endif
#  include <wchar.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include "common.h"
#include "dwg.h"

// DWG size limitations, invalid sizes and offsets
#if SIZEOF_SIZE_T == 8
#  define MAX_MEM_ALLOC 0x10000000000
#else
#  define MAX_MEM_ALLOC 0xF0000000
#endif

// avoid double linkage on windows with unit-testing
#if defined(BITS_TEST_C) || defined(DECODE_TEST_C) || defined(DXF_TEST_C)
#  undef EXPORT
#  define EXPORT
#endif

/**
 Structure for DWG-files raw data streams.
 */
typedef struct _bit_chain
{
  unsigned char *chain;
  size_t size;
  size_t byte;
  unsigned char bit;
  unsigned char opts; // from dwg->opts, see DWG_OPTS_*
  Dwg_Version_Type version;
  Dwg_Version_Type from_version;
  FILE *fh;
  BITCODE_RS codepage;
} Bit_Chain;

#define EMPTY_CHAIN(size)                                                     \
  { NULL, size, 0UL, 0, 0, R_INVALID, R_INVALID, NULL, 0 }

// only if from r2007+ DWG. not JSON, DXF (FIXME TABLE.name). add API converts to TU
#define IS_FROM_TU(dat)                                                       \
  (dat->from_version >= R_2007) && !(dat->opts & DWG_OPTS_IN)
#define IS_FROM_TU_DWG(dwg)                                                   \
  (dwg->header.from_version >= R_2007) && !(dwg->opts & DWG_OPTS_IN)
#define TU_to_int(b) le16toh (((uint16_t)b[1] << 8) + b[0])

/* Functions for raw data manipulations.
 */
void bit_advance_position (Bit_Chain *dat, long advance);
size_t bit_position (Bit_Chain *dat);
void bit_set_position (Bit_Chain *dat, size_t bitpos);
void bit_reset_chain (Bit_Chain *dat);

BITCODE_B bit_read_B (Bit_Chain *dat);
void bit_write_B (Bit_Chain *dat, unsigned char value);

BITCODE_BB bit_read_BB (Bit_Chain *dat);
void bit_write_BB (Bit_Chain *dat, unsigned char value);

BITCODE_4BITS bit_read_4BITS (Bit_Chain *dat);
void bit_write_4BITS (Bit_Chain *dat, unsigned char value);

BITCODE_RC bit_read_RC (Bit_Chain *dat);
void bit_write_RC (Bit_Chain *dat, unsigned char value);

BITCODE_RS bit_read_RS (Bit_Chain *dat);
void bit_write_RS (Bit_Chain *dat, BITCODE_RS value);
BITCODE_BS bit_read_RS_BE (Bit_Chain *dat);
void bit_write_RS_BE (Bit_Chain *dat, BITCODE_BS value);

BITCODE_RL bit_read_RL (Bit_Chain *dat);
BITCODE_RL bit_read_RL_BE (Bit_Chain *dat);
void bit_write_RL (Bit_Chain *dat, BITCODE_RL value);
void bit_write_RL_BE (Bit_Chain *dat, BITCODE_RL value);

BITCODE_RLL bit_read_RLL (Bit_Chain *dat);
BITCODE_RLL bit_read_RLL_BE (Bit_Chain *dat);
#define bit_read_RLLd(dat) (BITCODE_RLLd) bit_read_RLL (dat)
void bit_write_RLL (Bit_Chain *dat, BITCODE_RLL value);
#define bit_write_RLLd(dat, value) bit_write_RLL (dat, (BITCODE_RLL)value)
void bit_write_RLL_BE (Bit_Chain *dat, BITCODE_RLL value);

BITCODE_RD bit_read_RD (Bit_Chain *dat);
void bit_write_RD (Bit_Chain *dat, BITCODE_RD value);

/* Functions for manipulating compacted data
 */
BITCODE_BS bit_read_BS (Bit_Chain *dat);
void bit_write_BS (Bit_Chain *dat, BITCODE_BS value);

BITCODE_BL bit_read_BL (Bit_Chain *dat);
void bit_write_BL (Bit_Chain *dat, BITCODE_BL value);
#define bit_read_BLd(dat) (BITCODE_BLd) bit_read_BL (dat)
void bit_write_BLd (Bit_Chain *dat, BITCODE_BLd value);

BITCODE_BS bit_read_BOT (Bit_Chain *dat);
void bit_write_BOT (Bit_Chain *dat, BITCODE_BS value);

BITCODE_BLL bit_read_BLL (Bit_Chain *dat);
void bit_write_BLL (Bit_Chain *dat, BITCODE_BLL value);

#if 0
BITCODE_3B bit_read_3B (Bit_Chain *dat);
void bit_write_3B (Bit_Chain *dat, unsigned char value);
BITCODE_BLL bit_read_3BLL (Bit_Chain *dat); /*unused but as documented*/
void bit_write_3BLL (Bit_Chain *dat, BITCODE_BLL value);
#endif

BITCODE_BD bit_read_BD (Bit_Chain *dat);
void bit_write_BD (Bit_Chain *dat, BITCODE_BD value);

BITCODE_MC bit_read_MC (Bit_Chain *dat);
void bit_write_MC (Bit_Chain *dat, BITCODE_MC value);

BITCODE_UMC bit_read_UMC (Bit_Chain *dat);
void bit_write_UMC (Bit_Chain *dat, BITCODE_UMC value);

BITCODE_MS bit_read_MS (Bit_Chain *dat);
void bit_write_MS (Bit_Chain *dat, BITCODE_MS value);

void bit_read_BE (Bit_Chain *restrict dat, double *restrict x,
                  double *restrict y, double *restrict z);
void bit_write_BE (Bit_Chain *dat, double x, double y, double z);
void normalize_BE (BITCODE_3BD ext);

BITCODE_DD bit_read_DD (Bit_Chain *dat, double default_value);
BITCODE_BB bit_write_DD (Bit_Chain *dat, double value, double default_value);
int bit_eq_DD (double value, double default_value);

BITCODE_BT bit_read_BT (Bit_Chain *dat);
void bit_write_BT (Bit_Chain *dat, double value);

int bit_read_H (Bit_Chain *restrict dat, Dwg_Handle *restrict handle);
void bit_write_H (Bit_Chain *restrict dat, Dwg_Handle *restrict handle);
void bit_H_to_dat (Bit_Chain *restrict dat, Dwg_Handle *restrict handle);

uint16_t bit_read_CRC (Bit_Chain *dat);

int bit_check_CRC (Bit_Chain *dat, size_t start_address, const uint16_t seed);
uint16_t bit_write_CRC (Bit_Chain *dat, size_t start_address,
                        const uint16_t seed);
// object-map only
uint16_t bit_write_CRC_BE (Bit_Chain *dat, size_t start_address,
                           const uint16_t seed);

uint16_t bit_calc_CRC (const uint16_t seed, unsigned char *adr, size_t len);
uint32_t bit_calc_CRC32 (const uint32_t seed, unsigned char *adr, size_t len);

int bit_read_fixed (Bit_Chain *restrict dat, BITCODE_RC *restrict dest,
                    size_t length);

/* read fixed-length ASCII string */
BITCODE_TF bit_read_TF (Bit_Chain *dat, size_t length) ATTRIBUTE_MALLOC;
BITCODE_TF bit_read_bits (Bit_Chain *dat, size_t bits) ATTRIBUTE_MALLOC;
void bit_write_bits (Bit_Chain *restrict dat, BITCODE_TF restrict bits,
                     size_t numbits);

void bit_write_TF (Bit_Chain *restrict dat, BITCODE_TF restrict chain,
                   size_t length);
/** Write fixed-length text from variable length string
    (possibly downgraded from shorter string).
 */
void bit_write_TFv (Bit_Chain *restrict dat, BITCODE_TF restrict chain,
                    size_t length);

/* read ASCII string, with length as BS */
BITCODE_TV bit_read_TV (Bit_Chain *restrict dat);

void bit_write_TV (Bit_Chain *restrict dat, BITCODE_TV restrict value);

/* read UCS-2 string, with length as BS */
BITCODE_TU bit_read_TU (Bit_Chain *restrict dat) ATTRIBUTE_MALLOC;
BITCODE_TU bit_read_TU_len (Bit_Chain *restrict dat,
                            unsigned int *lenp) ATTRIBUTE_MALLOC;
BITCODE_TU bit_read_TU_size (Bit_Chain *restrict dat,
                             unsigned int len) ATTRIBUTE_MALLOC;

/* read ASCII string, with length as RS */
BITCODE_T16 bit_read_T16 (Bit_Chain *restrict dat) ATTRIBUTE_MALLOC;
/* read UCS-2 string, with length as RS */
BITCODE_TU bit_read_TU16 (Bit_Chain *restrict dat) ATTRIBUTE_MALLOC;
/* read ASCII/UCS-2 string, with length as RL */
BITCODE_T32 bit_read_T32 (Bit_Chain *restrict dat) ATTRIBUTE_MALLOC;
/* read ASCII/UCS-4 string, with length as RL */
BITCODE_TU32 bit_read_TU32 (Bit_Chain *restrict dat) ATTRIBUTE_MALLOC;

void bit_write_TU (Bit_Chain *restrict dat, BITCODE_TU restrict value);
void bit_write_TU16 (Bit_Chain *restrict dat, BITCODE_TU restrict value);
void bit_write_T16 (Bit_Chain *restrict dat, BITCODE_T16 restrict value);
void bit_write_T32 (Bit_Chain *restrict dat, BITCODE_T32 restrict value);
void bit_write_TU32 (Bit_Chain *restrict dat, BITCODE_TU32 restrict value);

BITCODE_T* bit_read_T (Bit_Chain *restrict dat) ATTRIBUTE_MALLOC;
void bit_write_T (Bit_Chain *restrict dat, BITCODE_T *restrict chain);

/* Converts UCS-2 to ASCII (with \U+XXXX), returning a copy. */
EXPORT char *bit_embed_TU (BITCODE_T *restrict tstr) ATTRIBUTE_MALLOC;
EXPORT char *bit_embed_TU_size (BITCODE_TU restrict wstr,
                                const int len) ATTRIBUTE_MALLOC;

#ifdef HAVE_NATIVE_WCHAR2
#  define bit_wcs2len(wstr) wcslen (wstr)
#  ifdef HAVE_WCSNLEN
#    define bit_wcs2nlen(wstr, maxlen) wcsnlen (wstr, maxlen)
#  else
size_t bit_wcs2nlen (const BITCODE_TU restrict wstr, const size_t maxlen);
#  endif
#  define bit_wcs2cpy(dest, src) wcscpy (dest, src)
#  ifdef _WIN32
#    define bit_wcs2dup(src) _wcsdup (src)
#  else
#    define bit_wcs2dup(src) wcsdup (src)
#  endif
#  define bit_wcs2cmp(dest, src) wcscmp (s1, s2)
#else
/* length of UCS-2 string */
size_t bit_wcs2len (const BITCODE_TU restrict wstr);
/* bounded length of UCS-2 string. stops scanning at maxlen.
   Beware: might overflow to negative lengths */
size_t bit_wcs2nlen (const BITCODE_TU restrict wstr, const size_t maxlen);
BITCODE_TU bit_wcs2cpy (BITCODE_TU restrict dest,
                        const BITCODE_TU restrict src);
BITCODE_TU bit_wcs2dup (const BITCODE_TU restrict src);
int bit_wcs2cmp (BITCODE_TU restrict s1, const BITCODE_TU restrict s2);
#endif

#ifndef HAVE_STRNLEN
size_t bit_strnlen (const char *restrict str, const size_t maxlen);
#  define strnlen(str, maxlen) bit_strnlen (str, maxlen)
#endif

/* Converts UCS-2 to UTF-8, returning a copy. */
EXPORT char *bit_convert_TU (BITCODE_TU restrict wstr) ATTRIBUTE_MALLOC;

/* Convert UCS-2LE to UTF-8, returning a copy. */
EXPORT char *bit_TU_to_utf8 (BITCODE_TU restrict wstr) ATTRIBUTE_MALLOC;
EXPORT char *bit_T_to_utf8 (const BITCODE_T *restrict tstr);

/** Converts UTF-8 (dxf,json) to ASCII TV.
    \uxxxx or other unicode => \U+XXXX if not representable in this codepage.
    If cquoted unquotes \" to ", undo json_cquote(),
    Returns NULL if not enough room in dest. */
EXPORT char *bit_utf8_to_TV (char *restrict dest,
                             const unsigned char *restrict src,
                             const size_t destlen, const size_t srclen,
                             const unsigned cquoted,
                             const BITCODE_RS codepage);
/** converts old codepage'd strings to UTF-8.
    convert \U+XXXX or \MnXXXX also if representable.
    returns NULL on errors, or the unchanged src string, or a copy.
 */
EXPORT
char *bit_TV_to_utf8 (const char *restrict src,
                      const BITCODE_RS codepage) ATTRIBUTE_MALLOC;

/** Converts UTF-8 to UCS-2. Returns a copy.
    Needed by dwg importers, writers (e.g. dxf2dwg)
    cquoted is needed by in_json, to unquote \"
 */
EXPORT BITCODE_TU bit_utf8_to_TU (char *restrict str,
                                  const unsigned cquoted) ATTRIBUTE_MALLOC;
// convert all \\U+XXXX and \\M+nXXXX sequences to UTF-8
char *bit_u_expand (char *src);

/* compare two strings, the 2nd string being raw */
int bit_eq_T (const BITCODE_T *restrict str1, const char *restrict str2);
/* compare two T strings */
int bit_eq_T_T (const BITCODE_T *restrict str1,
                const BITCODE_T *restrict str2);
/* compare an ASCII/utf-8 string to a r2007+ name */
int bit_eq_TU (const char *str, BITCODE_TU restrict wstr);
/* check if the string (ascii or unicode) is empty */
int bit_empty_T (Bit_Chain *restrict dat, BITCODE_T *restrict str);
BITCODE_T *bit_set_T (Bit_Chain *restrict dat,
                      const char *restrict src) ATTRIBUTE_MALLOC;

BITCODE_TIMEBLL bit_read_TIMEBLL (Bit_Chain *dat);
void bit_write_TIMEBLL (Bit_Chain *dat, BITCODE_TIMEBLL date);

BITCODE_TIMERLL bit_read_TIMERLL (Bit_Chain *dat);
void bit_write_TIMERLL (Bit_Chain *dat, BITCODE_TIMERLL date);

int bit_read_CMC (Bit_Chain *dat, Bit_Chain *str_dat,
                  Dwg_Color *restrict color);
void bit_write_CMC (Bit_Chain *dat, Bit_Chain *str_dat,
                    Dwg_Color *restrict color);
// Convert from truecolor (r2004+) to palette (-r2000)
void bit_downconvert_CMC (Bit_Chain *dat, Dwg_Color *restrict color);
void bit_upconvert_CMC (Bit_Chain *dat, Dwg_Color *restrict color);

void bit_read_ENC (Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
                   Dwg_Color *restrict color);
void bit_write_ENC (Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
                    Dwg_Color *restrict color);

int bit_search_sentinel (Bit_Chain *dat, const unsigned char sentinel[16]);

void bit_chain_init (Bit_Chain *dat, const size_t size);
void bit_chain_init_dat (Bit_Chain *restrict dat, const size_t size,
                         const Bit_Chain *restrict from_dat);
void bit_chain_alloc (Bit_Chain *dat);
void bit_chain_alloc_size (Bit_Chain *dat, const size_t size);
void bit_chain_free (Bit_Chain *dat);
// after bit_chain_init
#define bit_chain_set_version(to, from)                                       \
  (to)->opts = (from)->opts;                                                  \
  (to)->version = (from)->version;                                            \
  (to)->from_version = (from)->from_version;                                  \
  (to)->fh = (from)->fh;                                                      \
  (to)->codepage = (from)->codepage

void bit_print (Bit_Chain *dat, size_t size);

void bit_write_bits1 (Bit_Chain *restrict dat, const char *restrict bits);
long bit_write_hexbits (Bit_Chain *restrict dat, const char *restrict bytes);
void bit_print_bits (unsigned char *bits, size_t bitsize);
void bit_fprint_bits (FILE *fp, unsigned char *bits, size_t bitsize);
void bit_explore_chain (Bit_Chain *dat, size_t from, size_t size);

BITCODE_BD bit_nan (void);
int bit_isnan (BITCODE_BD number);

// which would require different text sizes and recalc.
bool does_cross_unicode_datversion (Bit_Chain *restrict dat);
/* Copy the whole content of tmp_data to dat, and reset tmp_dat */
void bit_copy_chain (Bit_Chain *restrict orig_dat,
                     Bit_Chain *restrict tmp_dat);

// for in_dxf and in_json
size_t in_hex2bin (unsigned char *restrict dest, char *restrict src,
                   size_t destlen) __nonnull_all;

// if there's a high char (> 0x7f)
bool bit_TF_contains_high (char *s, size_t len);

#endif // BITS_H

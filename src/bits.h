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
#    define MB_LEN_MAX	16
#  endif
#  include <wchar.h>
#endif
#include <stdio.h>
#include "common.h"
#include "dwg.h"

// avoid double linkage on windows with unit-testing
#if defined(BITS_TEST_C) || defined(DECODE_TEST_C) || defined(DXF_TEST_C)
#  undef EXPORT
#  define EXPORT
#endif

/**
 Structure for DWG-files raw data storage
 */
typedef struct _bit_chain
{
  unsigned char *chain;
  long unsigned int size;
  long unsigned int byte;
  unsigned char bit;
  FILE *fh;
  Dwg_Version_Type version;
  Dwg_Version_Type from_version;
  unsigned char opts; // from dwg->opts, see DWG_OPTS_*
  unsigned char flag; // flag to has print entity
} Bit_Chain;

/* Functions for raw data manipulations.
 */
void bit_advance_position (Bit_Chain *dat, long advance);
unsigned long bit_position (Bit_Chain *dat);
void bit_set_position (Bit_Chain *dat, unsigned long bitpos);
void bit_reset_chain (Bit_Chain *dat);

BITCODE_B
bit_read_B (Bit_Chain *dat);

void bit_write_B (Bit_Chain *dat, unsigned char value);

BITCODE_BB
bit_read_BB (Bit_Chain *dat);

void bit_write_BB (Bit_Chain *dat, unsigned char value);

BITCODE_3B
bit_read_3B (Bit_Chain *dat);

void bit_write_3B (Bit_Chain *dat, unsigned char value);

BITCODE_4BITS
bit_read_4BITS (Bit_Chain *dat);

void bit_write_4BITS (Bit_Chain *dat, unsigned char value);

BITCODE_RC
bit_read_RC (Bit_Chain *dat);

void bit_write_RC (Bit_Chain *dat, unsigned char value);

BITCODE_RS
bit_read_RS (Bit_Chain *dat);

void bit_write_RS (Bit_Chain *dat, BITCODE_RS value);

BITCODE_RS
bit_read_RS_LE (Bit_Chain *dat);

void bit_write_RS_LE (Bit_Chain *dat, BITCODE_RS value);

BITCODE_RL
bit_read_RL (Bit_Chain *dat);

void bit_write_RL (Bit_Chain *dat, BITCODE_RL value);

BITCODE_RL
bit_read_RL_LE (Bit_Chain *dat);

void bit_write_RL_LE (Bit_Chain *dat, BITCODE_RL value);

BITCODE_RLL
bit_read_RLL (Bit_Chain *dat);

void bit_write_RLL (Bit_Chain *dat, BITCODE_RLL value);

BITCODE_RD
bit_read_RD (Bit_Chain *dat);

void bit_write_RD (Bit_Chain *dat, BITCODE_RD value);

/* Functions for manipulating compacted data
 */
BITCODE_BS
bit_read_BS (Bit_Chain *dat);

void bit_write_BS (Bit_Chain *dat, BITCODE_BS value);

BITCODE_BL
bit_read_BL (Bit_Chain *dat);

void bit_write_BL (Bit_Chain *dat, BITCODE_BL value);

void bit_write_BLd (Bit_Chain *dat, BITCODE_BLd value);

BITCODE_BS
bit_read_BOT (Bit_Chain *dat);

void bit_write_BOT (Bit_Chain *dat, BITCODE_BS value);

BITCODE_BLL
bit_read_BLL (Bit_Chain *dat);
BITCODE_BLL
bit_read_3BLL (Bit_Chain *dat); /*unused but as documented*/

void bit_write_BLL (Bit_Chain *dat, BITCODE_BLL value);
void bit_write_3BLL (Bit_Chain *dat, BITCODE_BLL value);

BITCODE_BD
bit_read_BD (Bit_Chain *dat);

void bit_write_BD (Bit_Chain *dat, BITCODE_BD value);

BITCODE_MC
bit_read_MC (Bit_Chain *dat);

void bit_write_MC (Bit_Chain *dat, BITCODE_MC value);

BITCODE_UMC
bit_read_UMC (Bit_Chain *dat);

void bit_write_UMC (Bit_Chain *dat, BITCODE_UMC value);

BITCODE_MS
bit_read_MS (Bit_Chain *dat);

void bit_write_MS (Bit_Chain *dat, BITCODE_MS value);

void bit_read_BE (Bit_Chain *restrict dat, double *restrict x,
                  double *restrict y, double *restrict z);

void bit_write_BE (Bit_Chain *dat, double x, double y, double z);

BITCODE_DD bit_read_DD (Bit_Chain *dat, double default_value);
BITCODE_BB bit_write_DD (Bit_Chain *dat, double value, double default_value);

BITCODE_BT bit_read_BT (Bit_Chain *dat);
void bit_write_BT (Bit_Chain *dat, double value);

int bit_read_H (Bit_Chain *restrict dat, Dwg_Handle *restrict handle);
void bit_write_H (Bit_Chain *restrict dat, Dwg_Handle *restrict handle);

uint16_t bit_read_CRC (Bit_Chain *dat);

int bit_check_CRC (Bit_Chain *dat, long unsigned int start_address,
                   const uint16_t seed);
uint16_t bit_write_CRC (Bit_Chain *dat, long unsigned int start_address,
                        const uint16_t seed);
// object-map only
uint16_t bit_write_CRC_LE (Bit_Chain *dat, long unsigned int start_address,
                           const uint16_t seed);

uint16_t bit_calc_CRC (const uint16_t seed, unsigned char *adr, long len);
uint32_t bit_calc_CRC32 (const uint32_t seed, unsigned char *adr, long len);

void bit_read_fixed (Bit_Chain *restrict dat, BITCODE_RC *restrict dest,
                     unsigned int length);

/* read fixed-length ASCII string */
BITCODE_TF bit_read_TF (Bit_Chain *dat, unsigned int length) ATTRIBUTE_MALLOC;

void bit_write_TF (Bit_Chain *restrict dat, BITCODE_TF restrict chain,
                   unsigned int length);

/* read ASCII string, with length as BS */
BITCODE_TV bit_read_TV (Bit_Chain *restrict dat);

void bit_write_TV (Bit_Chain *restrict dat, BITCODE_TV restrict value);

/* read UCS-2 string, with length as BS */
BITCODE_TU bit_read_TU (Bit_Chain *restrict dat);
/* read ASCII string, with length as RS */
BITCODE_TV bit_read_T16 (Bit_Chain *restrict dat);
/* read UCS-2 string, with length as RS */
BITCODE_TU bit_read_TU16 (Bit_Chain *restrict dat);
/* read ASCII/UCS-2 string, with length as RL */
BITCODE_T32 bit_read_T32 (Bit_Chain *restrict dat);
/* read ASCII/UCS-4 string, with length as RL */
BITCODE_TU32 bit_read_TU32 (Bit_Chain *restrict dat);

void bit_write_TU (Bit_Chain *restrict dat, BITCODE_TU restrict value);

BITCODE_T bit_read_T (Bit_Chain *restrict dat);
void bit_write_T (Bit_Chain *restrict dat, BITCODE_T restrict chain);

/* Converts UCS-2 to ASCII (with \U+XXXX), returning a copy. */
EXPORT char *bit_embed_TU (BITCODE_TU restrict wstr) ATTRIBUTE_MALLOC;
/* Converts UCS-2 to UTF-8, returning a copy. */
EXPORT char *bit_convert_TU (BITCODE_TU restrict wstr) ATTRIBUTE_MALLOC;

/** Converts UTF-8 (dxf,json) to ASCII TV.
    Unquotes \" to ", undo json_cquote(), \uxxxx or other unicode => \U+XXXX
    Returns NULL if not enough room in dest. */
EXPORT char *
bit_utf8_to_TV (char *restrict dest, const unsigned char *restrict src, const int len);

/** Converts UTF-8 to UCS-2. Returns a copy.
    Needed by dwg importers, writers (e.g. dxf2dwg) */
EXPORT BITCODE_TU bit_utf8_to_TU (char *restrict str) ATTRIBUTE_MALLOC;

/* compare an ASCII/utf-8 string to a r2007+ name */
int bit_eq_TU (const char *str, BITCODE_TU restrict wstr);

BITCODE_RL bit_read_L (Bit_Chain *dat);
void bit_write_L (Bit_Chain *dat, BITCODE_RL value);

BITCODE_TIMEBLL bit_read_TIMEBLL (Bit_Chain *dat);
void bit_write_TIMEBLL (Bit_Chain *dat, BITCODE_TIMEBLL date);

BITCODE_TIMERLL bit_read_TIMERLL (Bit_Chain *dat);
void bit_write_TIMERLL (Bit_Chain *dat, BITCODE_TIMERLL date);

void bit_read_CMC (Bit_Chain *restrict dat, Dwg_Color *restrict color);
void bit_write_CMC (Bit_Chain *restrict dat, Dwg_Color *restrict color);

void bit_read_ENC (Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
                   Dwg_Color *restrict color);
void bit_write_ENC (Bit_Chain *dat, Bit_Chain *hdl_dat, Bit_Chain *str_dat,
                    Dwg_Color *restrict color);

int bit_search_sentinel (Bit_Chain *dat, unsigned char sentinel[16]);
void bit_write_sentinel (Bit_Chain *dat, unsigned char sentinel[16]);

void bit_chain_init (Bit_Chain *dat, const int size);
void bit_chain_alloc (Bit_Chain *dat);

void bit_print (Bit_Chain *dat, long unsigned int size);

void bit_write_bits (Bit_Chain *restrict dat, const char *restrict bits);
long bit_write_hexbits (Bit_Chain *restrict dat, const char *restrict bytes);
void bit_print_bits (unsigned char *bits, long unsigned int size);
void bit_fprint_bits (FILE *fp, unsigned char *bits, long unsigned int size);

void bit_explore_chain (Bit_Chain *dat, long unsigned int size);

BITCODE_BD
bit_nan (void);

int bit_isnan (BITCODE_BD number);

#endif

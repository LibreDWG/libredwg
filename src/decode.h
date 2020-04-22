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
 * decode.h: decoding function prototypes
 * written by Felipe Castro
 * modified by Felipe Corrêa da Silva Sances
 * modified by Rodrigo Rodrigues da Silva
 * modified by Reini Urban
 */

#ifndef DECODE_H
#define DECODE_H

#include "bits.h"
#include "dwg.h"

#ifdef IN_DXF_H
#error in_dxf.h must be included after decode.h because of FORMAT_BD
#endif

#define REFS_PER_REALLOC 128

int dwg_decode (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
int dwg_decode_unknown (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
Dwg_Object_Ref *dwg_find_objectref (const Dwg_Data *restrict dwg,
                                    const Dwg_Object *restrict obj);

/*------------------------------------------------------------------------------
 * Functions reused with decode_r2007
 */
Dwg_Object_Ref *dwg_decode_handleref (Bit_Chain *restrict hdl_dat,
                                      Dwg_Object *restrict obj,
                                      Dwg_Data *restrict dwg);

Dwg_Object_Ref *dwg_decode_handleref_with_code (Bit_Chain *restrict hdl_dat,
                                                Dwg_Object *restrict obj,
                                                Dwg_Data *restrict dwg,
                                                unsigned int code);
int dwg_decode_header_variables (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                 Bit_Chain *str_dat, Dwg_Data *restrict dwg);
int dwg_decode_add_object (Dwg_Data *restrict dwg, Bit_Chain *dat,
                           Bit_Chain *hdl_dat, long unsigned int address);
int obj_handle_stream (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                       Bit_Chain *restrict hdl_dat);
void bfr_read (void *restrict dst, BITCODE_RC *restrict *restrict src,
               size_t size);
void decrypt_R2004_header (BITCODE_RC *restrict dest,
                           const BITCODE_RC *restrict src, unsigned size);

/* reused with free */
void dwg_free_xdata_resbuf (Dwg_Resbuf *restrict rbuf);

/* reused with encode */
void dwg_resolve_objectrefs_silent (Dwg_Data *restrict dwg);
uint32_t dwg_section_page_checksum (const uint32_t seed, Bit_Chain *restrict dat,
                                    int32_t size);
unsigned int section_max_decomp_size (const Dwg_Data *dwg, const Dwg_Section_Type id);

/* reused with out_dxf */
char *dwg_dim_blockname (Dwg_Data *restrict dwg,
                         const Dwg_Object *restrict obj);

int dwg_validate_INSERT (Dwg_Object *restrict obj);
int dwg_validate_POLYLINE (Dwg_Object *restrict obj);
/* reused with many */
int dwg_fixup_BLOCKS_entities (Dwg_Data *restrict dwg);

/* from decode_r2007.c */
int obj_string_stream (Bit_Chain *dat, Dwg_Object *restrict obj,
                       Bit_Chain *str_dat);
unsigned long obj_stream_position (Bit_Chain *restrict dat,
                                   Bit_Chain *restrict hdl_dat,
                                   Bit_Chain *restrict str_dat);

void read_r2007_init (Dwg_Data *restrict dwg);
int read_r2007_meta_data (Bit_Chain *dat, Bit_Chain *hdl_dat,
                          Dwg_Data *restrict dwg);
void section_string_stream (Bit_Chain *restrict dat, BITCODE_RL bitsize,
                            Bit_Chain *restrict str);

/* from dwg.c */
int dat_read_file (Bit_Chain *restrict dat, FILE *restrict fp,
                   const char *restrict filename);
int dat_read_stream (Bit_Chain *restrict dat, FILE *restrict fp);

#endif

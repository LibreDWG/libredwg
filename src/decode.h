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
void bfr_read (void *restrict dst, BITCODE_RC *restrict *restrict src,
               size_t size);

/* reused with free */
void dwg_free_xdata_resbuf (Dwg_Resbuf *restrict rbuf);

/* reused with encode */
void dwg_resolve_objectrefs_silent (Dwg_Data *restrict dwg);
uint32_t dwg_section_page_checksum (const uint32_t seed, Bit_Chain *restrict dat,
                                    int32_t size);

/* reused with out_dxf */
char *dwg_dim_blockname (Dwg_Data *restrict dwg,
                         const Dwg_Object *restrict obj);

int dwg_validate_INSERT (Dwg_Object *restrict obj);
int dwg_validate_POLYLINE (Dwg_Object *restrict obj);

/* from decode_r2007.c */
int obj_handle_stream (Bit_Chain *dat, Dwg_Object *restrict obj,
                       Bit_Chain *hdl_dat);
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

#endif

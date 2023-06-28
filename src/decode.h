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
#include "decode_r11.h"

#ifdef IN_DXF_H
#  error in_dxf.h must be included after decode.h because of FORMAT_BD
#endif

#define REFS_PER_REALLOC 16384

typedef enum ENTITY_SECTION_INDEX_R11
{
  ENTITIES_SECTION_INDEX = 0,
  BLOCKS_SECTION_INDEX,
  EXTRAS_SECTION_INDEX
} EntitySectionIndexR11;

EXPORT int dwg_decode (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
int dwg_decode_unknown_bits (Bit_Chain *restrict dat,
                             Dwg_Object *restrict obj);
int dwg_decode_unknown_rest (Bit_Chain *restrict dat,
                             Dwg_Object *restrict obj);
Dwg_Object_Ref *dwg_find_objectref (const Dwg_Data *restrict dwg,
                                    const Dwg_Object *restrict obj);

int dwg_decode_BLOCK_HEADER (Bit_Chain *restrict dat,
                             Dwg_Object *restrict obj);
int dwg_decode_LAYER (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_STYLE (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_LTYPE (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_VIEW (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_UCS (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_VPORT (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_APPID (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_DIMSTYLE (Bit_Chain *restrict dat, Dwg_Object *restrict obj);
int dwg_decode_VX_TABLE_RECORD (Bit_Chain *restrict dat,
                                Dwg_Object *restrict obj);

// reused with the importers
void decode_BACKGROUND_type (const Dwg_Object *obj);

/*------------------------------------------------------------------------------
 * Functions reused with decode_r11
 */
int dwg_decode_eed (Bit_Chain *restrict dat, Dwg_Object_Object *restrict obj);

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
BITCODE_BL dwg_find_index_from_address (Dwg_Data *restrict dwg, size_t address);
int dwg_decode_add_object (Dwg_Data *restrict dwg, Bit_Chain *dat,
                           Bit_Chain *hdl_dat, size_t address, BITCODE_BL num,
                           const int prescan_objects);
int obj_handle_stream (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                       Bit_Chain *restrict hdl_dat);
void bfr_read (void *restrict dst, BITCODE_RC *restrict *restrict src,
               size_t size);
void bfr_read_32 (void *restrict dst, BITCODE_RC *restrict *restrict src,
                  size_t size);
void bfr_read_64 (void *restrict dst, BITCODE_RC *restrict *restrict src,
                  size_t size);

/* reused with free */
void dwg_free_xdata_resbuf (Dwg_Resbuf *restrict rbuf);

/* reused with encode */
void decrypt_R2004_header (BITCODE_RC *restrict dest,
                           const BITCODE_RC *restrict src, size_t size);
void dwg_resolve_objectrefs_silent (Dwg_Data *restrict dwg);
uint32_t dwg_section_page_checksum (const uint32_t seed,
                                    Bit_Chain *restrict dat, int32_t size,
                                    bool skip_checksum);
unsigned int section_max_decomp_size (const Dwg_Data *dwg,
                                      const Dwg_Section_Type id);

/* reused with out_dxf */
char *dwg_dim_blockname (Dwg_Data *restrict dwg,
                         const Dwg_Object *restrict obj);

int dwg_validate_INSERT (Dwg_Object *restrict obj);
int dwg_validate_POLYLINE (Dwg_Object *restrict obj);
/* reused with many */
int dwg_fixup_BLOCKS_entities (Dwg_Data *restrict dwg);
void dxf_3dsolid_revisionguid (Dwg_Entity_3DSOLID *_obj);

/* from decode_r2007.c */
int obj_string_stream (Bit_Chain *dat, Dwg_Object *restrict obj,
                       Bit_Chain *str_dat);
size_t obj_stream_position (Bit_Chain *restrict dat,
                            Bit_Chain *restrict hdl_dat,
                            Bit_Chain *restrict str_dat);

void read_r2007_init (Dwg_Data *restrict dwg);
int read_r2007_meta_data (Bit_Chain *dat, Bit_Chain *hdl_dat,
                          Dwg_Data *restrict dwg);
void section_string_stream (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
                            BITCODE_RL bitsize, Bit_Chain *restrict str);
/* for decode_r11.c */
int decode_r11_auxheader (Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
int decode_preR13_DIMENSION (Bit_Chain *restrict dat,
                             Dwg_Object *restrict obj);
int decode_preR13_sentinel (const Dwg_Sentinel sentinel,
                            const char *restrict sentinel_name,
                            Bit_Chain *restrict dat, Dwg_Data *restrict dwg);
int decode_preR13_entities (BITCODE_RL start, BITCODE_RL end,
                            unsigned num_entities, BITCODE_RL size,
                            Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                            const EntitySectionIndexR11 entity_section_index);

/* from dwg.c */
// from dat.fh
EXPORT int dat_read_size (Bit_Chain *restrict dat);
EXPORT int dat_read_file (Bit_Chain *restrict dat, FILE *restrict fp,
                          const char *restrict filename);
EXPORT int dat_read_stream (Bit_Chain *restrict dat, FILE *restrict fp);

#endif

/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010-2020 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode_r2007.c: functions to decode R2007 (AC1021) sections
 * written by Till Heuschmann
 * modified by Reini Urban
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include "common.h"
#include "bits.h"
#include "decode.h"

/* The logging level for the read (decode) path.  */
static unsigned int loglevel;
/* the current version per spec block */
static unsigned int cur_ver = 0;

#define DWG_LOGLEVEL loglevel
#include "logging.h"
#include "dec_macros.h"

// only for temp. debugging, to abort on obviously wrong sizes.
// should be a bit larger then the filesize.
#define DBG_MAX_COUNT 0x100000
#define DBG_MAX_SIZE 0xff0000 /* should be dat->size */

typedef struct r2007_file_header
{
  int64_t header_size; // 0x70
  int64_t file_size;
  int64_t pages_map_crc_compressed;
  int64_t pages_map_correction;
  int64_t pages_map_crc_seed;
  int64_t pages_map2_offset;
  int64_t pages_map2_id;
  int64_t pages_map_offset; // starting address of the Page Map section
  int64_t pages_map_id;
  int64_t header2_offset;
  int64_t pages_map_size_comp; // the compressed size of section
  int64_t pages_map_size_uncomp;
  int64_t pages_amount;
  int64_t pages_maxid;
  int64_t unknown1; // 0x20
  int64_t unknown2; // 0x40
  int64_t pages_map_crc_uncomp;
  int64_t unknown3; // 0xf800
  int64_t unknown4; // 4
  int64_t unknown5; // 1
  int64_t num_sections;
  int64_t sections_map_crc_uncomp;
  int64_t sections_map_size_comp;
  int64_t sections_map2_id;
  int64_t sections_map_id;
  int64_t sections_map_size_uncomp;
  int64_t sections_map_crc_comp;
  int64_t sections_map_correction;
  int64_t sections_map_crc_seed;
  int64_t stream_version; // 0x60100
  int64_t crc_seed;
  int64_t crc_seed_encoded;
  int64_t random_seed;
  int64_t header_crc;
} r2007_file_header;

/* page map */
typedef struct _r2007_page
{
  int64_t id;
  uint64_t size;
  uint64_t offset;
  struct _r2007_page *next;
} r2007_page;

/* section page */
typedef struct _r2007_section_page
{
  uint64_t offset;
  uint64_t size;
  int64_t id;
  uint64_t uncomp_size; // src_size
  uint64_t comp_size;
  uint64_t checksum;
  uint64_t crc;
} r2007_section_page;

/* section map */
typedef struct _r2007_section
{
  uint64_t data_size; // max size of page
  uint64_t max_size;
  int64_t encrypted;
  uint64_t hashcode;   // checksum in r2004
  int64_t name_length; // 0x22
  int64_t unknown;     // 0x00
  int64_t encoded;
  int64_t num_pages;
  DWGCHAR *name;
  Dwg_Section_Type type;
  r2007_section_page **pages;
  struct _r2007_section *next;
} r2007_section;

/* imported */
int rs_decode_block (BITCODE_RC *blk, int fix);

/* private */
static r2007_section *get_section (r2007_section *sections_map,
                                   Dwg_Section_Type sec_type);
static r2007_page *get_page (r2007_page *pages_map, int64_t id);
static void pages_destroy (r2007_page *page);
static void sections_destroy (r2007_section *section);
static r2007_section *read_sections_map (Bit_Chain *dat, int64_t size_comp,
                                         int64_t size_uncomp,
                                         int64_t correction) ATTRIBUTE_MALLOC;
static int read_data_section (Bit_Chain *sec_dat, Bit_Chain *dat,
                              r2007_section *restrict sections_map,
                              r2007_page *restrict pages_map,
                              Dwg_Section_Type sec_type);
static int read_2007_section_classes (Bit_Chain *restrict dat,
                                      Dwg_Data *restrict dwg,
                                      r2007_section *restrict sections_map,
                                      r2007_page *restrict pages_map);
static int read_2007_section_header (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                     Dwg_Data *restrict dwg,
                                     r2007_section *restrict sections_map,
                                     r2007_page *restrict pages_map);
static int read_2007_section_handles (Bit_Chain *dat, Bit_Chain *hdl_dat,
                                      Dwg_Data *restrict dwg,
                                      r2007_section *restrict sections_map,
                                      r2007_page *restrict pages_map);
static int read_2007_section_summary (Bit_Chain *restrict dat,
                                      Dwg_Data *restrict dwg,
                                      r2007_section *restrict sections_map,
                                      r2007_page *restrict pages_map);
static int read_2007_section_preview (Bit_Chain *restrict dat,
                                      Dwg_Data *restrict dwg,
                                      r2007_section *restrict sections_map,
                                      r2007_page *restrict pages_map);
static r2007_page *read_pages_map (Bit_Chain *dat, int64_t size_comp,
                                   int64_t size_uncomp,
                                   int64_t correction) ATTRIBUTE_MALLOC;
static int read_file_header (Bit_Chain *restrict dat,
                             r2007_file_header *restrict file_header);
static void read_instructions (BITCODE_RC *restrict *restrict src,
                               BITCODE_RC *restrict opcode,
                               uint32_t *restrict offset,
                               uint32_t *restrict length);
static inline BITCODE_RC *copy_bytes_2 (BITCODE_RC *restrict dst,
                                        const BITCODE_RC *restrict src);
static inline BITCODE_RC *copy_bytes_3 (BITCODE_RC *restrict dst,
                                        const BITCODE_RC *restrict src);
static void copy_bytes (BITCODE_RC *dst, uint32_t length, uint32_t offset);
static uint32_t read_literal_length (BITCODE_RC *restrict *restrict src,
                                     unsigned char opcode);
static void copy_compressed_bytes (BITCODE_RC *restrict dst,
                                   BITCODE_RC *restrict src,
                                   int length);
static DWGCHAR *bfr_read_string (BITCODE_RC *restrict *restrict src,
                                 int64_t size) ATTRIBUTE_MALLOC;
static BITCODE_RC *decode_rs (const BITCODE_RC *src, int block_count,
                              int data_size, const unsigned src_size) ATTRIBUTE_MALLOC;
static int decompress_r2007 (BITCODE_RC *restrict dst, const unsigned dst_size,
                             BITCODE_RC *restrict src, const unsigned src_size);

#define copy_1(offset) *dst++ = *(src + offset);

#define copy_2(offset) dst = copy_bytes_2 (dst, src + offset);

#define copy_3(offset) dst = copy_bytes_3 (dst, src + offset)

// 4 and 8 is not reverse, 16 is
#define copy_n(n, offset)                                                     \
  memcpy (dst, &src[offset], n);                                              \
  dst += n

#define copy_4(offset) copy_n (4, offset)
#define copy_8(offset) copy_n (8, offset)
#define copy_16(offset)                                                       \
  memcpy (dst, &src[offset + 8], 8);                                          \
  memcpy (&dst[8], &src[offset], 8);                                          \
  dst += 16

/* Don't use restrict here: GH #141 broken for most newer compilers */
static inline BITCODE_RC *
copy_bytes_2 (BITCODE_RC *dst, const BITCODE_RC *src)
{
  dst[0] = src[1];
  dst[1] = src[0];
  return dst + 2;
}

static inline BITCODE_RC *
copy_bytes_3 (BITCODE_RC *dst, const BITCODE_RC *src)
{
  dst[0] = src[2];
  dst[1] = src[1];
  dst[2] = src[0];
  return dst + 3;
}

static void
copy_bytes (BITCODE_RC *dst, uint32_t length, uint32_t offset)
{
  BITCODE_RC *src = dst - offset;

  while (length-- > 0)
    *dst++ = *src++;
}

/* See spec version 5.0 page 30 */
static void
copy_compressed_bytes (BITCODE_RC *restrict dst, BITCODE_RC *restrict src,
                       int length)
{
  while (length >= 32)
    {
      copy_16 (16);
      copy_16 (0);

      src += 32;
      length -= 32;
    }

  switch (length)
    {
    case 0:
      break;
    case 1:
      copy_1 (0);
      break;
    case 2:
      copy_2 (0);
      break;
    case 3:
      copy_3 (0);
      break;
    case 4:
      copy_4 (0);
      break;
    case 5:
      copy_1 (4);
      copy_4 (0);
      break;
    case 6:
      copy_1 (5);
      copy_4 (1);
      copy_1 (0);
      break;
    case 7:
      copy_2 (5);
      copy_4 (1);
      copy_1 (0);
      break;
    case 8:
      copy_8 (0);
      break;
    case 9:
      copy_1 (8);
      copy_8 (0);
      break;
    case 10:
      copy_1 (9);
      copy_8 (1);
      copy_1 (0);
      break;
    case 11:
      copy_2 (9);
      copy_8 (1);
      copy_1 (0);
      break;
    case 12:
      copy_4 (8);
      copy_8 (0);
      break;
    case 13:
      copy_1 (12);
      copy_4 (8);
      copy_8 (0);
      break;
    case 14:
      copy_1 (13);
      copy_4 (9);
      copy_8 (1);
      copy_1 (0);
      break;
    case 15:
      copy_2 (13);
      copy_4 (9);
      copy_8 (1);
      copy_1 (0);
      break;
    case 16:
      copy_16 (0);
      break;
    case 17:
      copy_8 (9);
      copy_1 (8);
      copy_8 (0);
      break;
    case 18:
      copy_1 (17);
      copy_16 (1);
      copy_1 (0);
      break;
    case 19:
      copy_3 (16);
      copy_16 (0);
      break;
    case 20:
      copy_4 (16);
      copy_16 (0);
      break;
    case 21:
      copy_1 (20);
      copy_4 (16);
      copy_16 (0);
      break;
    case 22:
      copy_2 (20);
      copy_4 (16);
      copy_16 (0);
      break;
    case 23:
      copy_3 (20);
      copy_4 (16);
      copy_16 (0);
      break;
    case 24:
      copy_8 (16);
      copy_16 (0);
      break;
    case 25:
      copy_8 (17);
      copy_1 (16);
      copy_16 (0);
      break;
    case 26:
      copy_1 (25);
      copy_8 (17);
      copy_1 (16);
      copy_16 (0);
      break;
    case 27:
      copy_2 (25);
      copy_8 (17);
      copy_1 (16);
      copy_16 (0);
      break;
    case 28:
      copy_4 (24);
      copy_8 (16);
      copy_16 (0);
      break;
    case 29:
      copy_1 (28);
      copy_4 (24);
      copy_8 (16);
      copy_16 (0);
      break;
    case 30:
      copy_2 (28);
      copy_4 (24);
      copy_8 (16);
      copy_16 (0);
      break;
    case 31:
      copy_1 (30);
      copy_4 (26);
      copy_8 (18);
      copy_16 (2);
      copy_2 (0);
      break;
    default:
      LOG_ERROR ("Wrong length %d", length);
    }
}

/* See spec version 5.1 page 50 */
static uint32_t
read_literal_length (BITCODE_RC *restrict *src, unsigned char opcode)
{
  uint32_t length = opcode + 8;

  if (length == 0x17)
    {
      int n = *(*src)++;

      length += n;

      if (n == 0xff)
        {
          do
            {
              n = *(*src)++;
              n |= (*(*src)++ << 8);

              length += n;
            }
          while (n == 0xFFFF);
        }
    }

  return length;
}

/* See spec version 5.1 page 53 */
static void
read_instructions (BITCODE_RC *restrict *src, unsigned char *restrict opcode,
                   uint32_t *restrict offset, uint32_t *restrict length)
{
  switch (*opcode >> 4)
    {
    case 0:
      *length = (*opcode & 0xf) + 0x13;
      *offset = *(*src)++;
      *opcode = *(*src)++;
      *length = ((*opcode >> 3) & 0x10) + *length;
      *offset = ((*opcode & 0x78) << 5) + 1 + *offset;
      break;

    case 1:
      *length = (*opcode & 0xf) + 3;
      *offset = *(*src)++;
      *opcode = *(*src)++;
      *offset = ((*opcode & 0xf8) << 5) + 1 + *offset;
      break;

    case 2:
      *offset = *(*src)++;
      *offset = ((*(*src)++ << 8) & 0xff00) | *offset;
      *length = *opcode & 7;

      if ((*opcode & 8) == 0)
        {
          *opcode = *(*src)++;
          *length = (*opcode & 0xf8) + *length;
        }
      else
        {
          (*offset)++;
          *length = (*(*src)++ << 3) + *length;
          *opcode = *(*src)++;
          *length = (((*opcode & 0xf8) << 8) + *length) + 0x100;
        }
      break;

    default:
      *length = *opcode >> 4;
      *offset = *opcode & 15;
      *opcode = *(*src)++;
      *offset = (((*opcode & 0xf8) << 1) + *offset) + 1;
      break;
    }
}

/* par 4.7 Compression, page 32 (same as format 2004)
   TODO: replace by decompress_R2004_section(dat, decomp, comp_data_size)
*/
static int
decompress_r2007 (BITCODE_RC *restrict dst, const unsigned dst_size,
                  BITCODE_RC *restrict src, const unsigned src_size)
{
  uint32_t length = 0;
  uint32_t offset = 0;

  BITCODE_RC *dst_start = dst;
  BITCODE_RC *dst_end = dst + dst_size;
  BITCODE_RC *src_end = src + src_size;
  unsigned char opcode;

  LOG_INSANE ("decompress_r2007 (%p, %d, %p, %d)\n", dst, dst_size, src, src_size);
  if (!dst || !src || !dst_size || src_size < 2)
    {
      LOG_ERROR ("Invalid argument to %s\n", __FUNCTION__);
      return DWG_ERR_INTERNALERROR;
    }

  opcode = *src++;
  if ((opcode & 0xf0) == 0x20)
    {
      src += 2;
      length = *src++ & 0x07;
      if (length == 0)
        {
          LOG_ERROR ("Decompression error: zero length")
          return DWG_ERR_INTERNALERROR;
        }
    }

  while (src < src_end)
    {
      if (length == 0)
        length = read_literal_length (&src, opcode);

      if ((dst + length) > dst_end || (src + length) > src_end)
        {
          LOG_ERROR ("Decompression error: length overflow");
          return DWG_ERR_INTERNALERROR;
        }

      LOG_INSANE("copy_compressed_bytes (%p, %p, %u)\n", dst, src, length);
      copy_compressed_bytes (dst, src, length);

      dst += length;
      src += length;

      length = 0;

      if (src >= src_end)
        return 0;

      opcode = *src++;

      read_instructions (&src, &opcode, &offset, &length);

      while (1)
        {
          if ((dst + length) > dst_end)
            {
              LOG_ERROR ("Decompression error: length overflow");
              return DWG_ERR_INTERNALERROR;
            }
          if (offset > (uint32_t) (dst - dst_start))
            {
              LOG_ERROR ("Decompression error: offset underflow");
              return DWG_ERR_INTERNALERROR;
            }
          LOG_INSANE ("copy_bytes (%p, %u, %u)\n", dst, length, offset);
          copy_bytes (dst, length, offset);

          dst += length;
          length = (opcode & 7);

          if (length != 0 || src >= src_end)
            break;

          opcode = *src++;

          if ((opcode >> 4) == 0)
            break;

          if ((opcode >> 4) == 0x0f)
            opcode &= 0xf;

          read_instructions ((unsigned char **)&src, &opcode, &offset,
                             &length);
        }
    }

  return 0;
}

// reed-solomon (255, 239) encoding with factor 3
// TODO: for now disabled, until we get proper data
ATTRIBUTE_MALLOC
static BITCODE_RC *
decode_rs (const BITCODE_RC *src, int block_count, int data_size,
           const unsigned src_size)
{
  int i, j;
  const BITCODE_RC *src_base = src;
  BITCODE_RC *dst_base, *dst;
  // TODO: round up data_size from 239 to 255

  if ((unsigned long)block_count * data_size > src_size)
    {
      LOG_ERROR ("decode_rs src overflow: %ld > %u",
                 (long)block_count * data_size, src_size)
      return NULL;
    }
  dst_base = dst = (BITCODE_RC *)calloc (block_count, data_size);
  if (!dst)
    {
      LOG_ERROR ("Out of memory")
      return NULL;
    }

  for (i = 0; i < block_count; ++i)
    {
      for (j = 0; j < data_size; ++j)
        {
          *dst++ = *src;
          src += block_count;
        }

      // rs_decode_block((unsigned char*)(dst_base + 239*i), 1);
      src = ++src_base;
    }

  return dst_base;
}

ATTRIBUTE_MALLOC
static BITCODE_RC *
read_system_page (Bit_Chain *dat, int64_t size_comp, int64_t size_uncomp,
                  int64_t repeat_count)
{
  int i;
  int error = 0;

  int64_t pesize;      // Pre RS encoded size
  int64_t block_count; // Number of RS encoded blocks
  int64_t page_size;
  long pedata_size;

  BITCODE_RC *rsdata; // RS encoded data
  BITCODE_RC *pedata; // Pre RS encoded data
  BITCODE_RC *data;   // The data RS unencoded and uncompressed

  // Round to a multiple of 8
  pesize = ((size_comp + 7) & ~7) * repeat_count;
  // Divide pre encoded size by RS k-value (239)
  block_count = (pesize + 238) / 239;
  // Multiply with codeword size (255) and round to a multiple of 8
  page_size = (block_count * 255 + 7) & ~7;

  assert ((uint64_t)size_comp < dat->size);
  assert ((uint64_t)size_uncomp < dat->size);
  assert ((uint64_t)repeat_count < DBG_MAX_COUNT);
  assert ((uint64_t)page_size < DBG_MAX_COUNT);

  if ((unsigned long)page_size > dat->size - dat->byte) // bytes left to read
    {
      LOG_ERROR ("Invalid page_size %ld > %lu bytes left", (long)page_size,
                 dat->size - dat->byte);
      return NULL;
    }
  data = (BITCODE_RC *)calloc (size_uncomp + page_size, 1);
  LOG_HANDLE ("Alloc system page of size %" PRId64 "\n", size_uncomp + page_size)
  if (!data)
    {
      LOG_ERROR ("Out of memory")
      return NULL;
    }

  rsdata = &data[size_uncomp];
  bit_read_fixed (dat, rsdata, page_size);
  pedata_size = block_count * 239;
  pedata = decode_rs (rsdata, block_count, 239, page_size);
  if (!pedata)
    {
      free (data);
      return NULL;
    }

  if (size_comp < size_uncomp)
    error = decompress_r2007 (data, size_uncomp, pedata, MIN (pedata_size, size_comp));
  else
    memcpy (data, pedata, size_uncomp);

  free (pedata);
  if (error >= DWG_ERR_CRITICAL)
    {
      free (data);
      return NULL;
    }
  return data;
}

static int
read_data_page (Bit_Chain *restrict dat, BITCODE_RC *restrict decomp,
                int64_t page_size, int64_t size_comp, int64_t size_uncomp)
{
  int i;
  int error = 0;

  int64_t pesize;      // Pre RS encoded size
  int64_t block_count; // Number of RS encoded blocks

  BITCODE_RC *rsdata; // RS encoded data
  BITCODE_RC *pedata; // Pre RS encoded data
  long pedata_size;

  // Round to a multiple of 8
  pesize = ((size_comp + 7) & ~7);
  block_count = (pesize + 0xFB - 1) / 0xFB;
  pedata_size = block_count * 0xFB;

  rsdata = (BITCODE_RC *)calloc (1, page_size);
  if (rsdata == NULL)
    {
      LOG_ERROR ("Out of memory")
      return DWG_ERR_OUTOFMEM;
    }
  bit_read_fixed (dat, rsdata, page_size);
  pedata = decode_rs (rsdata, block_count, 0xFB, page_size);
  if (!pedata)
    {
      free (rsdata);
      return DWG_ERR_OUTOFMEM;
    }

  if (size_comp < size_uncomp)
    error = decompress_r2007 (decomp, size_uncomp, pedata,
                              MIN (pedata_size, size_comp));
  else
    memcpy (decomp, pedata, size_uncomp);

  free (pedata);
  free (rsdata);

  return error;
}

static int
read_data_section (Bit_Chain *sec_dat, Bit_Chain *dat,
                   r2007_section *restrict sections_map,
                   r2007_page *restrict pages_map, Dwg_Section_Type sec_type)
{
  r2007_section *section;
  r2007_page *page;
  uint64_t max_decomp_size;
  BITCODE_RC *decomp;
  int error = 0, i;

  section = get_section (sections_map, sec_type);
  sec_dat->chain = NULL;
  if (section == NULL)
    {
      if (sec_type < SECTION_REVHISTORY)
        {
          LOG_ERROR ("Failed to find section %d", (int)sec_type)
        }
      else
        {
          LOG_TRACE ("Found no section with type %d\n", sec_type)
        }
      return DWG_ERR_SECTIONNOTFOUND;
    }

  max_decomp_size = section->data_size;
  if (max_decomp_size > 0x2f000000) // 790Mb
    {
      LOG_ERROR ("Invalid max decompression size %" PRIu64, max_decomp_size);
      return DWG_ERR_INVALIDDWG;
    }
  decomp = calloc (max_decomp_size, 1);
  if (decomp == NULL)
    {
      LOG_ERROR ("Out of memory")
      return DWG_ERR_OUTOFMEM;
    }
  LOG_HANDLE ("Alloc data section of size %" PRIu64 "\n", max_decomp_size)

  sec_dat->bit = 0;
  sec_dat->byte = 0;
  sec_dat->size = max_decomp_size;
  sec_dat->version = dat->version;
  sec_dat->from_version = dat->from_version;

  for (i = 0; i < (int)section->num_pages; i++)
    {
      r2007_section_page *section_page = section->pages[i];
      page = get_page (pages_map, section_page->id);
      if (page == NULL)
        {
          free (decomp);
          LOG_ERROR ("Failed to find page %d", (int)section_page->id)
          return DWG_ERR_PAGENOTFOUND;
        }
      if (section_page->offset > max_decomp_size)
        {
          free (decomp);
          LOG_ERROR ("Invalid section_page->offset %ld > %ld",
                     (long)section_page->offset, (long)max_decomp_size)
          return DWG_ERR_VALUEOUTOFBOUNDS;
        }

      dat->byte = page->offset;
      // only if compressed. TODO: Isn't there a compressed flag as with 2004+?
      // theoretically the sizes could still be the same.
      if (section_page->comp_size != section_page->uncomp_size)
        {
          error = read_data_page (dat, &decomp[section_page->offset],
                                  page->size, section_page->comp_size,
                                  section_page->uncomp_size);
          if (error)
            {
              free (decomp);
              LOG_ERROR ("Failed to read compressed page")
              return error;
            }
        }
      else
        {
          memcpy (&decomp[section_page->offset], &dat->chain[dat->byte],
                  section_page->uncomp_size);
        }
    }
  sec_dat->chain = decomp;
  return 0;
}

/* endian specific code: */
#define bfr_read_int16(_p)                                                    \
  *((int16_t *)_p);                                                           \
  _p += 2;
#define bfr_read_int64(_p)                                                    \
  *((int64_t *)_p);                                                           \
  _p += 8;
#define bfr_read_uint64(_p)                                                   \
  *((uint64_t *)_p);                                                          \
  _p += 8;

static DWGCHAR *
bfr_read_string (BITCODE_RC *restrict *restrict src, int64_t size)
{
  uint16_t *ptr = (uint16_t *)*src;
  int32_t length = 0, wsize;
  DWGCHAR *str, *str_base;
  int i;

  if (size <= 0)
    return NULL;
  while (*ptr != 0 && length * 2 < size)
    {
      ptr++;
      length++;
    }

  wsize = length * sizeof (DWGCHAR) + sizeof (DWGCHAR);

  str = str_base = (DWGCHAR *)malloc (wsize);
  if (!str)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  ptr = (uint16_t *)*src;
  for (i = 0; i < length; i++)
    {
      *str++ = (DWGCHAR) (*ptr++);
    }

  *src += size;
  *str = 0;

  return str_base;
}

static r2007_section *
read_sections_map (Bit_Chain *dat, int64_t size_comp, int64_t size_uncomp,
                   int64_t correction)
{
  BITCODE_RC *data;
  r2007_section *sections = NULL, *last_section = NULL, *section = NULL;
  BITCODE_RC *ptr, *ptr_end;
  int i, j = 0;

  data = read_system_page (dat, size_comp, size_uncomp, correction);
  if (!data)
    {
      LOG_ERROR ("Failed to read system page")
      return NULL;
    }

  ptr = data;
  ptr_end = data + size_uncomp;

  LOG_TRACE ("\n=== System Section (Section Map) ===\n")

  while (ptr < ptr_end)
    {
      section = (r2007_section *)calloc (1, sizeof (r2007_section));
      if (!section)
        {
          LOG_ERROR ("Out of memory");
          free (data);
          sections_destroy (sections); // the root
          return NULL;
        }

      bfr_read (section, &ptr, 64);

      LOG_TRACE ("\nSection [%d]:\n", j)
      LOG_TRACE ("  data size:     %" PRIu64 "\n", section->data_size)
      LOG_TRACE ("  max size:      %" PRIu64 "\n", section->max_size)
      LOG_TRACE ("  encryption:    %" PRIu64 "\n", section->encrypted)
      LOG_HANDLE ("  hashcode:      %" PRIx64 "\n", section->hashcode)
      LOG_HANDLE ("  name length:   %" PRIu64 "\n", section->name_length)
      LOG_TRACE ("  unknown:       %" PRIu64 "\n", section->unknown)
      LOG_TRACE ("  encoding:      %" PRIu64 "\n", section->encoded)
      LOG_TRACE ("  num pages:     %" PRIu64 "\n", section->num_pages);

      // debugging sanity
#if 1
      /* compressed */
      if (section->data_size > 10 * dat->size
          || section->name_length >= (int64_t)dat->size)
        {
          LOG_ERROR ("Invalid System Section");
          free (section);
          free (data);
          sections_destroy (sections); // the root
          return NULL;
        }
        // assert(section->data_size < dat->size + 0x100000);
        // assert(section->max_size  < dat->size + 0x100000);
        // assert(section->num_pages < DBG_MAX_COUNT);
#endif
      // section->next = NULL;
      // section->pages = NULL;
      // section->name = NULL;

      if (!sections)
        {
          sections = last_section = section;
        }
      else
        {
          last_section->next = section;
          last_section = section;
        }

      j++;
      if (ptr >= ptr_end)
        break;

      // Section Name (wchar)
      section->name = bfr_read_string (&ptr, section->name_length);
#ifdef HAVE_NATIVE_WCHAR2
      LOG_TRACE ("  name:          " FORMAT_TU "\n\n",
                 (BITCODE_TU)section->name)
#else
      LOG_TRACE ("  name:          ")
      LOG_TEXT_UNICODE (TRACE, section->name)
      LOG_TRACE ("\n\n")
#endif
      section->type = dwg_section_wtype (section->name);

      if (section->num_pages <= 0)
        continue;
      if (section->num_pages > 0xf0000)
        {
          LOG_ERROR ("Invalid num_pages %lu, skip", (unsigned long)section->num_pages);
          continue;
        }

      section->pages = (r2007_section_page **)calloc (
          (size_t)section->num_pages, sizeof (r2007_section_page *));
      if (!section->pages)
        {
          LOG_ERROR ("Out of memory");
          free (data);
          if (sections)
            sections_destroy (sections); // the root
          else
            sections_destroy (section);
          return NULL;
        }

      for (i = 0; i < section->num_pages; i++)
        {
          section->pages[i]
              = (r2007_section_page *)calloc (1, sizeof (r2007_section_page));
          if (!section->pages[i])
            {
              LOG_ERROR ("Out of memory");
              free (data);
              if (sections)
                sections_destroy (sections); // the root
              else
                sections_destroy (section);
              return NULL;
            }

          if (ptr + 56 >= ptr_end)
            {
              LOG_ERROR ("Section[%d]->pages[%d] overflow", j, i);
              free (section->pages[i]);
              section->num_pages = i; // skip this last section
              break;
            }
          bfr_read (section->pages[i], &ptr, 56);

          LOG_TRACE (" Page[%d]: ", i)
          LOG_TRACE (" offset: 0x%07" PRIx64, section->pages[i]->offset);
          LOG_TRACE (" size: %5" PRIu64, section->pages[i]->size);
          LOG_TRACE (" id: %4" PRIu64, section->pages[i]->id);
          LOG_TRACE (" uncomp_size: %5" PRIu64 "\n",
                     section->pages[i]->uncomp_size);
          LOG_HANDLE (" comp_size: %5" PRIu64, section->pages[i]->comp_size);
          LOG_HANDLE (" checksum: %016" PRIx64, section->pages[i]->checksum);
          LOG_HANDLE (" crc64: %016" PRIx64 "\n", section->pages[i]->crc);
          // debugging sanity
          if (section->pages[i]->size >= DBG_MAX_SIZE
              || section->pages[i]->uncomp_size >= DBG_MAX_SIZE
              || section->pages[i]->comp_size >= DBG_MAX_SIZE)
            {
              LOG_ERROR ("Invalid section->pages[%d] size", i);
              free (data);
              free (section->pages[i]);
              section->num_pages = i; // skip this last section
              return sections;
            }
          assert (section->pages[i]->size < DBG_MAX_SIZE);
          assert (section->pages[i]->uncomp_size < DBG_MAX_SIZE);
          assert (section->pages[i]->comp_size < DBG_MAX_SIZE);
        }
    }

  free (data);
  return sections;
}

static r2007_page *
read_pages_map (Bit_Chain *dat, int64_t size_comp, int64_t size_uncomp,
                int64_t correction)
{
  BITCODE_RC *data, *ptr, *ptr_end;
  r2007_page *pages = NULL, *last_page = NULL, *page;
  int64_t offset = 0x480; // dat->byte;
  // int64_t index;

  data = read_system_page (dat, size_comp, size_uncomp, correction);
  if (!data)
    {
      LOG_ERROR ("Failed to read system page")
      return NULL;
    }

  ptr = data;
  ptr_end = data + size_uncomp;

  LOG_TRACE ("\n=== System Section (Pages Map) ===\n")

  while (ptr < ptr_end)
    {
      page = (r2007_page *)malloc (sizeof (r2007_page));
      if (page == NULL)
        {
          LOG_ERROR ("Out of memory")
          free (data);
          pages_destroy (pages);
          return NULL;
        }
      if (ptr + 16 > ptr_end)
        {
          LOG_ERROR ("Page out of bounds")
          free (data);
          pages_destroy (pages);
          return NULL;
        }

      page->size = bfr_read_uint64 (ptr);
      page->id = bfr_read_int64 (ptr);
      page->offset = offset;
      offset += page->size;

      // index = page->id > 0 ? page->id : -page->id;

      LOG_TRACE ("Page [%3" PRId64 "]: ", page->id)
      LOG_TRACE ("size: %6" PRIu64 " ", page->size)
      LOG_TRACE ("id: 0x%04" PRIx64 " ", page->id)
      LOG_TRACE ("offset: 0x%" PRIx64 " \n", page->offset)

      page->next = NULL;

      if (pages == NULL)
        pages = last_page = page;
      else
        {
          last_page->next = page;
          last_page = page;
        }
    }
  free (data);
  return pages;
}

/* Lookup a page in the page map. The page is identified by its id.
 */
static r2007_page *
get_page (r2007_page *pages_map, int64_t id)
{
  r2007_page *page = pages_map;

  while (page != NULL)
    {
      if (page->id == id)
        break;
      page = page->next;
    }

  return page;
}

static void
pages_destroy (r2007_page *page)
{
  r2007_page *next;

  while (page != 0)
    {
      next = page->next;
      free (page);
      page = next;
    }
}

/* Lookup a section in the section map.
 * The section is identified by its numeric type.
 */
static r2007_section *
get_section (r2007_section *sections_map, Dwg_Section_Type sec_type)
{
  r2007_section *section = sections_map;
  while (section != NULL)
    {
      if (section->type == sec_type)
        break;
      section = section->next;
    }

  return section;
}

static void
sections_destroy (r2007_section *section)
{
  r2007_section *next;

  while (section != 0)
    {
      next = section->next;

      if (section->pages)
        {
          while (section->num_pages-- > 0)
            {
              free (section->pages[section->num_pages]);
            }
          free (section->pages);
        }

      if (section->name)
        free (section->name);

      free (section);
      section = next;
    }
}

static int
read_file_header (Bit_Chain *restrict dat,
                  r2007_file_header *restrict file_header)
{
  BITCODE_RC data[0x3d8]; // 0x400 - 5 long
  BITCODE_RC *pedata;
  uint64_t seqence_crc;
  uint64_t seqence_key;
  uint64_t compr_crc;
  int32_t compr_len, len2;
  int i;
  int error = 0, errcount = 0;
  const int pedata_size = 3 * 239; // size of pedata

  dat->byte = 0x80;
  LOG_TRACE ("\n=== File header ===\n")
  memset (file_header, 0, sizeof (r2007_file_header));
  bit_read_fixed (dat, data, 0x3d8);
  pedata = decode_rs (data, 3, 239, 0x3d8);
  if (!pedata)
    return DWG_ERR_OUTOFMEM;

  // Note: This is unportable to big-endian
  seqence_crc = *((uint64_t *)pedata);
  seqence_key = *((uint64_t *)&pedata[8]);
  compr_crc = *((uint64_t *)&pedata[16]);
  compr_len = *((int32_t *)&pedata[24]);
  len2 = *((int32_t *)&pedata[28]);
  LOG_TRACE ("seqence_crc64: %016lX\n", (unsigned long)seqence_crc);
  LOG_TRACE ("seqence_key:   %016lX\n", (unsigned long)seqence_key);
  LOG_TRACE ("compr_crc64:   %016lX\n", (unsigned long)compr_crc);
  LOG_TRACE ("compr_len:     %d\n", (int)compr_len); // only this is used
  LOG_TRACE ("len2:          %d\n", (int)len2);      // 0 when compressed

  if (compr_len > 0)
    error = decompress_r2007 ((BITCODE_RC *)file_header, 0x110, &pedata[32],
                              MIN (compr_len, pedata_size - 32));
  else
    memcpy (file_header, &pedata[32], sizeof (r2007_file_header));

  // check validity, for debugging only
  if (!error)
    {

#define VALID_SIZE(var)                                                       \
  if (var < 0 || (unsigned)var > dat->size)                                   \
    {                                                                         \
      errcount++;                                                             \
      error |= DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      LOG_ERROR ("%s Invalid %s %ld > MAX_SIZE", __FUNCTION__, #var,          \
                 (long)var)                                                   \
      var = 0;                                                                \
    }
#define VALID_COUNT(var)                                                      \
  if (var < 0 || (unsigned)var > dat->size)                                   \
    {                                                                         \
      errcount++;                                                             \
      error |= DWG_ERR_VALUEOUTOFBOUNDS;                                      \
      LOG_ERROR ("%s Invalid %s %ld > MAX_COUNT", __FUNCTION__, #var,         \
                 (long)var)                                                   \
      var = 0;                                                                \
    }

      VALID_SIZE (file_header->header_size);
      VALID_SIZE (file_header->file_size);
      VALID_SIZE (file_header->pages_map_offset);
      VALID_SIZE (file_header->header2_offset);
      VALID_SIZE (file_header->pages_map_offset);
      VALID_SIZE (file_header->pages_map_size_comp);
      VALID_SIZE (file_header->pages_map_size_uncomp);
      VALID_COUNT (file_header->pages_maxid);
      VALID_COUNT (file_header->pages_amount);
      VALID_COUNT (file_header->num_sections);
    }

  free (pedata);
  return error;
}

/* Return the latest dat position for all three independent streams
   data, handle and string.
*/
unsigned long
obj_stream_position (Bit_Chain *restrict dat, Bit_Chain *restrict hdl_dat,
                     Bit_Chain *restrict str_dat)
{
  unsigned long p1 = bit_position (dat);
  /* all 3 now relative to obj */
  unsigned long p2 = bit_position (hdl_dat);
  SINCE (R_2007)
    { // but only since 2007 there is a seperate string stream
      unsigned long p3 = bit_position (str_dat);
      if (p2 > p1)
        return p3 > p2 ? p3 : p2;
      else
        return p3 > p1 ? p3 : p1;
    }
  else
    {
      return (p2 > p1) ? p2 : p1;
    }
}

int
obj_string_stream (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
                   Bit_Chain *restrict str)
{
  BITCODE_RL start = obj->bitsize - 1; // in bits
  BITCODE_RL data_size = 0;            // in byte
  BITCODE_RL old_size;                 // in byte
  BITCODE_RL old_byte;
  assert (dat != str);
  old_size = str->size;
  old_byte = str->byte;

  str->chain += str->byte;
  // obj->strpos = str->byte * 8 + str->bit;

  str->byte = 0;
  str->bit = 0;
  str->size = (obj->bitsize / 8) + ((obj->bitsize % 8) ? 1 : 0);
  bit_advance_position (str, start - 8);

  if (str->byte >= old_size - old_byte)
    {
      LOG_WARN ("obj_string_stream overflow");
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
  LOG_HANDLE (" obj string stream +%u: @%lu.%u (%lu)", start, str->byte,
              str->bit & 7, bit_position (str));
  obj->has_strings = bit_read_B (str);
  LOG_TRACE (" has_strings: %d\n", (int)obj->has_strings);
  if (!obj->has_strings)
    {
      // FIXME wrong bit
      if (obj->fixedtype == DWG_TYPE_SCALE)
        obj->has_strings = 1;
      // str->size = 0;
      // bit_reset_chain (str);
      return 0;
    }

  bit_advance_position (str, -1); //-17
  str->byte -= 2;
  LOG_HANDLE (" @%lu.%u", str->byte, str->bit & 7);
  data_size = (BITCODE_RL)bit_read_RS (str);
  LOG_HANDLE (" data_size: %u/0x%x", data_size, data_size);

  if (data_size & 0x8000)
    {
      BITCODE_RS hi_size;
      bit_advance_position (str, -1); //-33
      str->byte -= 4;
      data_size &= 0x7FFF;
      hi_size = bit_read_RS (str);
      data_size |= (hi_size << 15);
      LOG_HANDLE (" data_size: %u/0x%x\n", data_size, data_size);
      // LOG_TRACE("  -33: @%lu\n", str->byte);
    }
  else
    LOG_HANDLE ("\n");
  str->byte -= 2;
  if (data_size > obj->bitsize)
    {
      LOG_WARN ("Invalid string stream data_size: @%lu.%u\n", str->byte,
                str->bit & 7);
      obj->has_strings = 0;
      bit_reset_chain (str);
      return DWG_ERR_NOTYETSUPPORTED; // a very low severity error
    }
  obj->stringstream_size = data_size;
  bit_advance_position (str, -(int)data_size);
  // bit_reset_chain (str);
  // LOG_TRACE(" %d: @%lu.%u (%lu)\n", -(int)data_size - 16, str->byte,
  // str->bit & 7,
  //          bit_position(str));
  // obj->strpos = obj->bitsize_pos + obj->bitsize - obj->stringstream_size;
  return 0;
}

void
section_string_stream (Bit_Chain *restrict dat, BITCODE_RL bitsize,
                       Bit_Chain *restrict str)
{
  BITCODE_RL start;     // in bits
  BITCODE_RL data_size; // in bits
  BITCODE_B endbit;
  PRE (R_2010)
  {
    // r2007: + 24 bytes (sentinel+size+hsize) - 1 bit (endbit)
    start = bitsize + 159;
  }
  else
  {
    // r2010: + 24 bytes (sentinel+size+hSize) - 1 bit (endbit)
    start = bitsize + 191; /* 8*24 = 192 */
  }
  *str = *dat;
  bit_set_position (str, start);
  LOG_TRACE ("section string stream\n  pos: " FORMAT_RL ", %lu/%u\n", start,
             str->byte, str->bit);
  endbit = bit_read_B (str);
  LOG_HANDLE ("  endbit: %d\n", (int)endbit);
  if (!endbit)
    return; // i.e. has no strings. without data_size should be 0
  start -= 16;
  bit_set_position (str, start);
  LOG_HANDLE ("  pos: " FORMAT_RL ", %lu\n", start, str->byte);
  // str->bit = start & 7;
  data_size = bit_read_RS (str);
  LOG_HANDLE ("  data_size: " FORMAT_RL "\n", data_size);
  if (data_size & 0x8000)
    {
      BITCODE_RS hi_size;
      start -= 16;
      data_size &= 0x7FFF;
      bit_set_position (str, start);
      LOG_HANDLE ("  pos: " FORMAT_RL ", %lu\n", start, str->byte);
      hi_size = bit_read_RS (str);
      data_size |= (hi_size << 15);
      LOG_HANDLE ("  hi_size: " FORMAT_RS ", data_size: " FORMAT_RL "\n",
                  hi_size, data_size);
    }
  start -= data_size;
  bit_set_position (str, start);
  LOG_HANDLE ("  pos: " FORMAT_RL ", %lu/%u\n", start, str->byte, str->bit);
}

// for string stream see p86
static int
read_2007_section_classes (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                           r2007_section *restrict sections_map,
                           r2007_page *restrict pages_map)
{
  BITCODE_RL size, i;
  BITCODE_BS max_num;
  Bit_Chain sec_dat = { 0 }, str = { 0 };
  int error;
  char c;

  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_CLASSES);
  if (error)
    {
      LOG_ERROR ("Failed to read class section");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  if (bit_search_sentinel (&sec_dat, dwg_sentinel (DWG_SENTINEL_CLASS_BEGIN)))
    {
      BITCODE_RL bitsize = 0;
      LOG_TRACE ("\nClasses\n-------------------\n")
      size = bit_read_RL (&sec_dat); // size of class data area
      LOG_TRACE ("size: " FORMAT_RL " [RL]\n", size)
      /*
      if (dat->from_version >= R_2010 && dwg->header.maint_version > 3)
        {
          BITCODE_RL hsize = bit_read_RL(&sec_dat);
          LOG_TRACE("hsize: " FORMAT_RL " [RL]\n", hsize)
        }
      */
      if (dat->from_version >= R_2007)
        {
          bitsize = bit_read_RL (&sec_dat);
          LOG_TRACE ("bitsize: " FORMAT_RL " [RL]\n", bitsize)
        }
      max_num = bit_read_BS (&sec_dat); // Maximum class number
      LOG_TRACE ("max_num: " FORMAT_BS " [BS]\n", max_num)
      c = bit_read_RC (&sec_dat); // 0x00
      LOG_HANDLE ("c: " FORMAT_RC " [RC]\n", c)
      c = bit_read_RC (&sec_dat); // 0x00
      LOG_HANDLE ("c: " FORMAT_RC " [RC]\n", c)
      c = bit_read_B (&sec_dat); // 1
      LOG_HANDLE ("c: " FORMAT_B " [B]\n", c);

      dwg->layout_type = 0;
      dwg->num_classes = max_num - 499;
      if (max_num < 500 || max_num > 5000)
        {
          LOG_ERROR ("Invalid max class number %d", max_num)
          dwg->num_classes = 0;
          if (sec_dat.chain)
            free (sec_dat.chain);
          return DWG_ERR_VALUEOUTOFBOUNDS;
        }
      assert (max_num >= 500);
      assert (max_num < 5000);

      section_string_stream (&sec_dat, bitsize, &str);

      dwg->dwg_class
          = (Dwg_Class *)calloc (dwg->num_classes, sizeof (Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR ("Out of memory");
          if (sec_dat.chain)
            free (sec_dat.chain);
          return DWG_ERR_OUTOFMEM;
        }

      for (i = 0; i < dwg->num_classes; i++)
        {
          dwg->dwg_class[i].number = bit_read_BS (&sec_dat);
          dwg->dwg_class[i].proxyflag = bit_read_BS (&sec_dat);
          dwg->dwg_class[i].appname = (char *)bit_read_TU (&str);
          dwg->dwg_class[i].cppname = (char *)bit_read_TU (&str);
          dwg->dwg_class[i].dxfname_u = bit_read_TU (&str);
          dwg->dwg_class[i].is_zombie = bit_read_B (&sec_dat); // DXF 280
          dwg->dwg_class[i].item_class_id = bit_read_BS (&sec_dat); // DXF 281, is_entity is 0x1f3

          dwg->dwg_class[i].num_instances = bit_read_BL (&sec_dat); // DXF 91
          dwg->dwg_class[i].dwg_version = bit_read_BS (&sec_dat);
          dwg->dwg_class[i].maint_version = bit_read_BS (&sec_dat);
          dwg->dwg_class[i].unknown_1 = bit_read_BL (&sec_dat); // 0
          dwg->dwg_class[i].unknown_2 = bit_read_BL (&sec_dat); // 0

          LOG_TRACE ("-------------------\n")
          LOG_TRACE ("Number:           %d\n", dwg->dwg_class[i].number)
          LOG_TRACE ("Proxyflag:        0x%x\n", dwg->dwg_class[i].proxyflag)
          LOG_TRACE_TU ("Application name", dwg->dwg_class[i].appname, 0)
          LOG_TRACE_TU ("C++ class name  ", dwg->dwg_class[i].cppname, 0)
          LOG_TRACE_TU ("DXF record name ", dwg->dwg_class[i].dxfname_u, 0)
          LOG_TRACE ("Class ID:         0x%x "
                     "(0x1f3 for object, 0x1f2 for entity)\n",
                     dwg->dwg_class[i].item_class_id)
          LOG_TRACE ("instance count:   %u\n", dwg->dwg_class[i].num_instances)
          LOG_TRACE ("dwg version:      %u (%u)\n",
                     dwg->dwg_class[i].dwg_version,
                     dwg->dwg_class[i].maint_version)
          LOG_HANDLE ("unknown:          %u %u\n", dwg->dwg_class[i].unknown_1,
                      dwg->dwg_class[i].unknown_2)

          dwg->dwg_class[i].dxfname
              = bit_convert_TU (dwg->dwg_class[i].dxfname_u);
          if (dwg->dwg_class[i].dxfname
              && strEQc (dwg->dwg_class[i].dxfname, "LAYOUT"))
            dwg->layout_type = dwg->dwg_class[i].number;
        }
    }
  else
    {
      LOG_ERROR ("Failed to find class section sentinel");
      free (sec_dat.chain);
      return DWG_ERR_CLASSESNOTFOUND;
    }

  if (sec_dat.chain)
    free (sec_dat.chain);

  return 0;
}

static int
read_2007_section_header (Bit_Chain *restrict dat, Bit_Chain *restrict hdl_dat,
                          Dwg_Data *restrict dwg,
                          r2007_section *restrict sections_map,
                          r2007_page *restrict pages_map)
{
  Bit_Chain sec_dat = { 0 }, str_dat = { 0 };
  int error;
  LOG_TRACE ("\nSection Header\n-------------------\n");
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_HEADER);
  if (error)
    {
      LOG_ERROR ("Failed to read header section");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }
  if (bit_search_sentinel (&sec_dat,
                           dwg_sentinel (DWG_SENTINEL_VARIABLE_BEGIN)))
    {
      BITCODE_RL endbits = 160; // start bit: 16 sentinel + 4 size
      dwg->header_vars.size = bit_read_RL (&sec_dat);
      LOG_TRACE ("size: " FORMAT_RL "\n", dwg->header_vars.size);
      *hdl_dat = sec_dat;
      // unused: later versions re-use the 2004 section format
      /*
      if (dat->from_version >= R_2010 && dwg->header.maint_version > 3)
        {
          dwg->header_vars.bitsize_hi = bit_read_RL(&sec_dat);
          LOG_TRACE("bitsize_hi: " FORMAT_RL " [RL]\n",
      dwg->header_vars.bitsize_hi) endbits += 32;
        }
      */
      if (dat->from_version == R_2007) // always true so far
        {
          dwg->header_vars.bitsize = bit_read_RL (&sec_dat);
          LOG_TRACE ("bitsize: " FORMAT_RL " [RL]\n",
                     dwg->header_vars.bitsize);
          endbits += dwg->header_vars.bitsize;
          bit_set_position (hdl_dat, endbits);
          section_string_stream (&sec_dat, dwg->header_vars.bitsize, &str_dat);
        }

      dwg_decode_header_variables (&sec_dat, hdl_dat, &str_dat, dwg);
    }
  else
    {
      DEBUG_HERE;
      error = DWG_ERR_SECTIONNOTFOUND;
    }

  if (sec_dat.chain)
    free (sec_dat.chain);

  return error;
}

static int
read_2007_section_handles (Bit_Chain *dat, Bit_Chain *hdl,
                           Dwg_Data *restrict dwg,
                           r2007_section *restrict sections_map,
                           r2007_page *restrict pages_map)
{
  static Bit_Chain obj_dat = { 0 }, hdl_dat = { 0 };
  BITCODE_RS section_size = 0;
  long unsigned int endpos;
  int error;

  error = read_data_section (&obj_dat, dat, sections_map, pages_map,
                             SECTION_OBJECTS);
  if (error >= DWG_ERR_CRITICAL || !obj_dat.chain)
    {
      LOG_ERROR ("Failed to read objects section");
      if (obj_dat.chain)
        free (obj_dat.chain);
      return error;
    }

  LOG_TRACE ("\nHandles\n-------------------\n")
  error = read_data_section (&hdl_dat, dat, sections_map, pages_map,
                             SECTION_HANDLES);
  if (error >= DWG_ERR_CRITICAL|| !hdl_dat.chain)
    {
      LOG_ERROR ("Failed to read handles section");
      if (obj_dat.chain)
        free (obj_dat.chain);
      if (hdl_dat.chain)
        free (hdl_dat.chain);
      return error;
    }

  /* From here on the same code as in decode:read_2004_section_handles */
  endpos = hdl_dat.byte + hdl_dat.size;
  dwg->num_objects = 0;

  do
    {
      long unsigned int last_offset;
      // long unsigned int last_handle;
      long unsigned int oldpos = 0;
      long unsigned int startpos = hdl_dat.byte;
      uint16_t crc1, crc2;

      section_size = bit_read_RS_LE (&hdl_dat);
      LOG_TRACE ("\nSection size: %u\n", section_size);
      if (section_size > 2050)
        {
          LOG_ERROR ("Object-map/handles section size greater than 2050!");
          return DWG_ERR_VALUEOUTOFBOUNDS;
        }

      last_offset = 0;
      while ((long)(hdl_dat.byte - startpos) < (long)section_size)
        {
          int added;
          BITCODE_UMC handleoff;
          BITCODE_MC offset;

          oldpos = hdl_dat.byte;
          handleoff = bit_read_UMC (&hdl_dat);
          offset = bit_read_MC (&hdl_dat);
          last_offset += offset;
          LOG_TRACE ("\nNext object: %lu ", (unsigned long)dwg->num_objects)
          LOG_TRACE ("Handleoff: " FORMAT_MC " [UMC] "
                     "Offset: " FORMAT_MC " [MC] @%lu\n",
                     handleoff, offset, last_offset)

          if (hdl_dat.byte == oldpos)
            break;

          added = dwg_decode_add_object (dwg, &obj_dat, hdl, last_offset);
          if (added > 0)
            error |= added;
        }

      if (hdl_dat.byte == oldpos)
        break;
#if 0
      if (!bit_check_CRC(&hdl_dat, startpos, 0xC0C1))
        LOG_WARN("Handles section CRC mismatch at offset %lx", startpos);
#else
      crc1 = bit_calc_CRC (0xC0C1, &(hdl_dat.chain[startpos]),
                           hdl_dat.byte - startpos);
      crc2 = bit_read_RS_LE (&hdl_dat);
      if (crc1 == crc2)
        {
          LOG_INSANE ("Handles section page CRC: %04X from %lx-%lx\n", crc2,
                      startpos, hdl_dat.byte - 2);
        }
      else
        {
          LOG_WARN ("Handles section page CRC mismatch: %04X vs calc. %04X "
                    "from %lx-%lx\n",
                    crc2, crc1, startpos, hdl_dat.byte - 2);
          error |= DWG_ERR_WRONGCRC;
        }
#endif

      if (hdl_dat.byte >= endpos)
        break;
    }
  while (section_size > 2);

  if (hdl_dat.chain)
    free (hdl_dat.chain);
  if (obj_dat.chain)
    free (obj_dat.chain);
  return error;
}

/* VBAProject Section
 */
static int
read_2007_section_vbaproject (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                              r2007_section *restrict sections_map,
                              r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  //Bit_Chain *str_dat;
  struct Dwg_VBAProject *_obj = &dwg->vbaproject;
  Dwg_Object *obj = NULL;
  int error = 0;
  //BITCODE_RL rcount1 = 0, rcount2 = 0;

  // not compressed, page size: 0x80
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_VBAPROJECT);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "VBAProject");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  LOG_TRACE ("\nVBAProject (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  dat = &sec_dat; // restrict in size

  DEBUG_HERE
  _obj->size = dat->size;
  _obj->unknown_bits = bit_read_TF (dat, _obj->size);
  LOG_TRACE_TF (_obj->unknown_bits, _obj->size)

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

static int
read_2007_section_summary (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                           r2007_section *restrict sections_map,
                           r2007_page *restrict pages_map)
{
  static Bit_Chain old_dat, sec_dat = { 0 };
  Bit_Chain *str_dat;
  struct Dwg_SummaryInfo *_obj = &dwg->summaryinfo;
  Dwg_Object *obj = NULL;
  int error;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  old_dat = *dat;
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_SUMMARYINFO);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_ERROR ("Failed to read SummaryInfo section");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  if (dwg->header.summaryinfo_address != (BITCODE_RL)dat->byte)
    LOG_WARN ("summaryinfo_address mismatch: " FORMAT_RL " != %lu",
              dwg->header.summaryinfo_address, dat->byte);
  LOG_TRACE ("\nSummaryInfo (%lu)\n-------------------\n", sec_dat.size)
  str_dat = dat = &sec_dat; // restrict in size

  // clang-format off
  #include "summaryinfo.spec"
  // clang-format on

  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

/* AppInfo Section
 */
static int
read_2007_section_appinfo (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                           r2007_section *restrict sections_map,
                           r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  Bit_Chain *str_dat;
  struct Dwg_AppInfo *_obj = &dwg->appinfo;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // not compressed, page size: 0x80
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_APPINFO);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "AppInfo");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  LOG_TRACE ("\nAppInfo (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  str_dat = dat = &sec_dat; // restrict in size

  // clang-format off
  #include "appinfo.spec"
  // clang-format on

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

/* Unknown AppInfoHistory Section
 */
static int
read_2007_section_appinfohistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                                  r2007_section *restrict sections_map,
                                  r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  //Bit_Chain *str_dat;
  struct Dwg_AppInfoHistory *_obj = &dwg->appinfohistory;
  Dwg_Object *obj = NULL;
  int error = 0;
  //BITCODE_RL rcount1 = 0, rcount2 = 0;

  // compressed, page size: 0x580
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_APPINFOHISTORY);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "AppInfoHistory");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  LOG_TRACE ("\nAppInfoHistory (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  dat = &sec_dat; // restrict in size

  DEBUG_HERE
  _obj->size = dat->size;
  _obj->unknown_bits = bit_read_TF (dat, _obj->size);
  LOG_TRACE_TF (_obj->unknown_bits, _obj->size)

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

/* RevHistory Section
 */
static int
read_2007_section_revhistory (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                              r2007_section *restrict sections_map,
                              r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  //Bit_Chain *str_dat;
  struct Dwg_RevHistory *_obj = &dwg->revhistory;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // compressed, page size: 0x7400
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_REVHISTORY);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "RevHistory");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  LOG_TRACE ("\nRevHistory (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  dat = &sec_dat; // restrict in size

  // clang-format off
  #include "revhistory.spec"
  // clang-format on

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

/* ObjFreeSpace Section
 */
static int
read_2007_section_objfreespace (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                                r2007_section *restrict sections_map,
                                r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  //Bit_Chain *str_dat;
  struct Dwg_ObjFreeSpace *_obj = &dwg->objfreespace;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // compressed, page size: 0x7400
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_OBJFREESPACE);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "ObjFreeSpace");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  LOG_TRACE ("\nObjFreeSpace (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  dat = &sec_dat; // restrict in size

  // clang-format off
  #include "objfreespace.spec"
  // clang-format on

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

/* Template Section. Optional r13-r15, mandatory r18+.
   Contains the MEASUREMENT variable (0 = English, 1 = Metric).
 */
static int
read_2007_section_template (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                            r2007_section *restrict sections_map,
                            r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  //Bit_Chain *str_dat;
  struct Dwg_Template *_obj = &dwg->template;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // compressed
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_TEMPLATE);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_ERROR ("%s section not found\n", "Template");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error | DWG_ERR_SECTIONNOTFOUND;
    }

  LOG_TRACE ("\nTemplate (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  dat = &sec_dat; // restrict in size

  // clang-format off
  #include "template.spec"
  // clang-format on

  dwg->header_vars.MEASUREMENT = _obj->MEASUREMENT;
  LOG_TRACE ("HEADER.MEASUREMENT: " FORMAT_BS " (0 English/1 Metric)\n",
             dwg->header_vars.MEASUREMENT)

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

/* r21 FileDepList Section
 */
static int
read_2007_section_filedeplist (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                               r2007_section *restrict sections_map,
                               r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  int error;
  Bit_Chain *str_dat;
  struct Dwg_FileDepList *_obj = &dwg->filedeplist;
  Dwg_Object *obj = NULL;
  BITCODE_BL vcount;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // not compressed, page size: 0x80. 0xc or 0xd
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_FILEDEPLIST);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "FileDepList");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return 0;
    }

  LOG_TRACE ("FileDepList (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  str_dat = dat = &sec_dat; // restrict in size

  // clang-format off
  #include "filedeplist.spec"
  // clang-format on

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

/* r21 Security Section, if saved with password
 */
static int
read_2007_section_security (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                           r2007_section *restrict sections_map,
                           r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  int error;
  Bit_Chain *str_dat;
  struct Dwg_Security *_obj = &dwg->security;
  Dwg_Object *obj = NULL;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // compressed, page size: 0x7400
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_SECURITY);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "Security");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return 0;
    }

  LOG_TRACE ("Security (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  str_dat = dat = &sec_dat; // restrict in size

  // clang-format off
  #include "security.spec"
  // clang-format on

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

#if 0
/* Signature Section, not written nor documented by Teigha
 */
static int
read_2007_section_signature (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                           r2007_section *restrict sections_map,
                           r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  int error;
  Bit_Chain *str_dat;
  struct Dwg_Signature *_obj = &dwg->signature;
  Dwg_Object *obj = NULL;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // compressed, page size: 0x7400
  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_SIGNATURE);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", "Signature");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return 0;
    }

  LOG_TRACE ("Signature (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  str_dat = dat = &sec_dat; // restrict in size

  // clang-format off
  #include "signature.spec"
  // clang-format on

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}
#endif

static int
acds_private (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Bit_Chain *str_dat = dat;
  struct Dwg_AcDs *_obj = &dwg->acds;
  Dwg_Object *obj = NULL;
  int error = 0;
  BITCODE_BL rcount1 = 0, rcount2 = 0;
  BITCODE_BL rcount3 = 0, rcount4, vcount;

  // clang-format off
  #include "acds.spec"
  // clang-format on

  return error;
}

/* r2013+ datastorage Section, if saved with binary ACIS SAB data
 */
static int
read_2007_section_acds (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                        r2007_section *restrict sections_map,
                        r2007_page *restrict pages_map)
{
  Bit_Chain old_dat, sec_dat = { 0 };
  int error;
  Bit_Chain *str_dat;
  const char *secname = "AcDsPrototype_1b";
  struct Dwg_AcDs *_obj = &dwg->acds;
  Dwg_Object *obj = NULL;
  BITCODE_RL rcount1 = 0, rcount2 = 0;

  // compressed, pagesize 0x7400, type 13
  error = read_data_section (&sec_dat, dat, sections_map, pages_map, SECTION_ACDS);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_INFO ("%s section not found\n", secname);
      if (sec_dat.chain)
        free (sec_dat.chain);
      return 0;
    }

  LOG_TRACE ("AcDs datastorage (%lu)\n-------------------\n", sec_dat.size)
  old_dat = *dat;
  str_dat = dat = &sec_dat; // restrict in size

  error |= acds_private (dat, dwg);
  error &= ~DWG_ERR_SECTIONNOTFOUND;

  LOG_TRACE ("\n")
  if (sec_dat.chain)
    free (sec_dat.chain);
  *dat = old_dat; // unrestrict
  return error;
}

static int
read_2007_section_preview (Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
                           r2007_section *restrict sections_map,
                           r2007_page *restrict pages_map)
{
  static Bit_Chain sec_dat = { 0 };
  int error;
  BITCODE_RL size;
  unsigned char *sentinel;

  error = read_data_section (&sec_dat, dat, sections_map, pages_map,
                             SECTION_PREVIEW);
  if (error >= DWG_ERR_CRITICAL || !sec_dat.chain)
    {
      LOG_ERROR ("Failed to read uncompressed %s section", "Preview");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  if (dwg->header.thumbnail_address != (BITCODE_RL)dat->byte)
    LOG_WARN ("thumbnail_address mismatch: " FORMAT_RL " != %lu",
              dwg->header.thumbnail_address, dat->byte);
  LOG_TRACE ("\nPreview (%lu)\n-------------------\n", sec_dat.size)
  if (!sec_dat.chain || sec_dat.size < 16)
    {
      LOG_WARN ("Empty thumbnail");
      if (sec_dat.chain)
        free (sec_dat.chain);
      return error;
    }

  sentinel = dwg_sentinel (DWG_SENTINEL_THUMBNAIL_BEGIN);
  if (memcmp (sentinel, sec_dat.chain, 16))
    {
      LOG_WARN ("thumbnail sentinel mismatch");
      return error |= DWG_ERR_WRONGCRC;
    }

  dwg->thumbnail.size = sec_dat.size - 32; // 2x sentinel
  dwg->thumbnail.chain = sec_dat.chain;
  dwg->thumbnail.byte = 16; // sentinel

  dwg_bmp (dwg, &size);
  if (abs ((int)((long)size - (long)dwg->thumbnail.size))
      > 200) // various headers
    LOG_WARN ("thumbnail.size mismatch: %lu != " FORMAT_RL,
              dwg->thumbnail.size, size);

  dat->byte += dwg->thumbnail.size;

  return error;
}

/* exported */
void
read_r2007_init (Dwg_Data *restrict dwg)
{
  if (dwg->opts)
    loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
}

int
read_r2007_meta_data (Bit_Chain *dat, Bit_Chain *hdl_dat,
                      Dwg_Data *restrict dwg)
{
  r2007_file_header file_header;
  r2007_page *restrict pages_map, *restrict page;
  r2007_section *restrict sections_map = NULL;
  int error;
#ifdef USE_TRACING
  char *probe;
#endif

  read_r2007_init (dwg);
#ifdef USE_TRACING
  probe = getenv ("LIBREDWG_TRACE");
  if (probe)
    loglevel = atoi (probe);
#endif
  // @ 0x62
  error = read_file_header (dat, &file_header);
  if (error >= DWG_ERR_VALUEOUTOFBOUNDS)
    return error;

  // Pages Map
  dat->byte += 0x28; // overread check data
  dat->byte += file_header.pages_map_offset;

  pages_map = read_pages_map (dat, file_header.pages_map_size_comp,
                              file_header.pages_map_size_uncomp,
                              file_header.pages_map_correction);
  if (!pages_map)
    return DWG_ERR_PAGENOTFOUND; // Error already logged

  // Sections Map
  page = get_page (pages_map, file_header.sections_map_id);
  if (!page)
    {
      LOG_ERROR ("Failed to find sections page map %d",
                 (int)file_header.sections_map_id);
      error |= DWG_ERR_SECTIONNOTFOUND;
      goto error;
    }
  dat->byte = page->offset;
  if ((unsigned long)file_header.sections_map_size_comp
      > dat->byte - dat->size)
    {
      LOG_ERROR ("%s Invalid comp_data_size %lu > %lu bytes left",
                 __FUNCTION__,
                 (unsigned long)file_header.sections_map_size_comp,
                 dat->size - dat->byte)
      error |= DWG_ERR_VALUEOUTOFBOUNDS;
      goto error;
    }
  sections_map = read_sections_map (dat, file_header.sections_map_size_comp,
                                    file_header.sections_map_size_uncomp,
                                    file_header.sections_map_correction);
  if (!sections_map)
    goto error;

  error = read_2007_section_header (dat, hdl_dat, dwg, sections_map,
                                    pages_map);
  if (dwg->header.summaryinfo_address)
    error |= read_2007_section_summary (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_classes (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_handles (dat, hdl_dat, dwg, sections_map,
                                      pages_map);
  if (dwg->header.thumbnail_address)
    error |= read_2007_section_preview (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_appinfo (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_appinfohistory (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_filedeplist (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_security (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_revhistory (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_objfreespace (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_template (dat, dwg, sections_map, pages_map);
  if (dwg->header.vbaproj_address)
    error |= read_2007_section_vbaproject (dat, dwg, sections_map, pages_map);
  //error |= read_2007_section_signature (dat, dwg, sections_map, pages_map);
  error |= read_2007_section_acds (dat, dwg, sections_map, pages_map);
  // read_2007_blocks (dat, hdl_dat, dwg, sections_map, pages_map);

error:
  pages_destroy (pages_map);
  if (sections_map)
    sections_destroy (sections_map);

  return error;
}

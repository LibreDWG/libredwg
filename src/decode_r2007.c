/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * decode_r2007.c: functions to decode R2007 (r21) files
 * written by Till Heuschmann
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>
#include <assert.h>
#include "bits.h"
#include "logging.h"
#include "dec_macros.h"

/* The logging level for the read (decode) path.  */
static unsigned int loglevel;
/* the current version per spec block */
static unsigned int cur_ver = 0;

/* imports */
Dwg_Object_Ref *
dwg_decode_handleref(Bit_Chain* hdl_dat, Dwg_Object* obj, Dwg_Data* dwg);

Dwg_Object_Ref *
dwg_decode_handleref_with_code(Bit_Chain* hdl_dat, Dwg_Object* obj,
                               Dwg_Data* dwg, unsigned int code);
void
dwg_decode_header_variables(Bit_Chain* dat, Bit_Chain* hdl_dat,
                            Bit_Chain* str_dat, Dwg_Data* dwg);
void
dwg_decode_add_object(Dwg_Data* dwg, Bit_Chain* dat, Bit_Chain* hdl_dat,
                      long unsigned int address);

// private
void
obj_string_stream(Bit_Chain *dat, Dwg_Object *obj, Bit_Chain *str_dat);


// only for temp. debugging, to abort on obviously wrong sizes.
// should be a bit larger then the filesize.
#define DBG_MAX_COUNT 0x10000
#define DBG_MAX_SIZE  0xf00000

#undef DWG_LOGLEVEL
#ifdef USE_TRACING
# define DWG_LOGLEVEL loglevel
#else
# define DWG_LOGLEVEL DWG_LOGLEVEL_TRACE
#endif

typedef struct r2007_file_header
{
  int64_t header_size;            // 0x70
  int64_t file_size;
  int64_t pages_map_crc_compressed;
  int64_t pages_map_correction;
  int64_t pages_map_crc_seed;
  int64_t pages_map2_offset;
  int64_t pages_map2_id;
  int64_t pages_map_offset;       // starting address of the Page Map section
  int64_t pages_map_id;
  int64_t header2_offset;
  int64_t pages_map_size_comp;    // the compressed size of section
  int64_t pages_map_size_uncomp;
  int64_t pages_amount;
  int64_t pages_maxid;
  int64_t unknown1;               // 0x20
  int64_t unknown2;               // 0x40
  int64_t pages_map_crc_uncomp;
  int64_t unknown3;               // 0xf800
  int64_t unknown4;               // 4
  int64_t unknown5;               // 1
  int64_t sections_amount;
  int64_t sections_map_crc_uncomp;
  int64_t sections_map_size_comp;
  int64_t sections_map2_id;
  int64_t sections_map_id;
  int64_t sections_map_size_uncomp;
  int64_t sections_map_crc_comp;
  int64_t sections_map_correction;
  int64_t sections_map_crc_seed;
  int64_t stream_version;         // 0x60100
  int64_t crc_seed;
  int64_t crc_seed_encoded;
  int64_t random_seed;
  int64_t header_crc;
} r2007_file_header;

/* page map */
typedef struct _r2007_page
{
  int64_t id;
  int64_t size;
  int64_t offset;
  struct _r2007_page *next;
} r2007_page;

/* section page */
typedef struct _r2007_section_page
{
  int64_t offset;
  int64_t size;
  int64_t id;
  int64_t uncomp_size;  // src_size
  int64_t comp_size;
  int64_t checksum;
  int64_t crc;
} r2007_section_page;

/* section map */
typedef struct _r2007_section
{
  int64_t  data_size;    // max size of page
  int64_t  max_size;
  int64_t  encrypted;
  int64_t  hashcode;
  int64_t  name_length;  // 0x22
  int64_t  unknown;      // 0x00
  int64_t  encoded;
  int64_t  num_pages;
  DWGCHAR *name;
  Dwg_Section_Type type;
  r2007_section_page **pages;
  struct _r2007_section *next;
} r2007_section;

/* exported */
int read_r2007_meta_data(Bit_Chain *dat, Bit_Chain *hdl_dat, Dwg_Data *dwg);
/* imported */
extern int rs_decode_block(unsigned char *blk, int fix);

/* private */
static r2007_section* get_section(r2007_section *sections_map,
                                  Dwg_Section_Type sec_type);
static r2007_page* get_page(r2007_page *pages_map, int64_t id);
static void pages_destroy(r2007_page *page);
static void sections_destroy(r2007_section *section);
static r2007_section* read_sections_map(Bit_Chain* dat, int64_t size_comp,
                                        int64_t size_uncomp,
                                        int64_t correction);
static int read_data_section(Bit_Chain *sec_dat, Bit_Chain *dat,
           r2007_section *sections_map, r2007_page *pages_map,
                             Dwg_Section_Type sec_type);
static int read_2007_section_classes(Bit_Chain* dat,
           Dwg_Data *dwg, r2007_section *sections_map, r2007_page *pages_map);
static int read_2007_section_header(Bit_Chain* dat, Bit_Chain* hdl_dat,
           Dwg_Data *dwg, r2007_section *sections_map, r2007_page *pages_map);
static int read_2007_section_handles(Bit_Chain* dat, Bit_Chain* hdl_dat,
           Dwg_Data *dwg, r2007_section *sections_map, r2007_page *pages_map);
static r2007_page* read_pages_map(Bit_Chain* dat, int64_t size_comp,
                                  int64_t size_uncomp, int64_t correction);
static void read_file_header(Bit_Chain* dat, r2007_file_header *file_header);
static void read_instructions(unsigned char **src, unsigned char *opcode,
                              uint32_t *offset, uint32_t *length);
static char* copy_bytes_2(char *dst, char *src);
static char* copy_bytes_3(char *dst, char *src);
static char* copy_bytes_16(char *dst, char *src);
static void  copy_bytes(char *dst, uint32_t length, uint32_t offset);
static uint32_t read_literal_length(unsigned char **src, unsigned char opcode);
static void copy_compressed_bytes(char *dst, char *src, int length);
static void  bfr_read(void *dst, char **src, size_t size);
static DWGCHAR* bfr_read_string(char **src);
static char* decode_rs(const char *src, int block_count, int data_size);
static int  decompress_r2007(char *dst, int dst_size, char *src, int src_size);

#define copy_1(offset) \
*dst++ = *(src + offset);

#define copy_2(offset) \
dst = copy_bytes_2(dst, src + offset);

#define copy_3(offset) \
dst = copy_bytes_3(dst, src + offset);

#define copy_4(offset) \
*(uint32_t*)dst = *(uint32_t*)(src + offset); \
dst += 4;

#define copy_8(offset) \
*(uint64_t*)dst = *(uint64_t*)(src + offset); \
dst += 8;

#define copy_16(offset) \
dst = copy_bytes_16(dst, src + offset);


static char*
copy_bytes_2(char *dst, char *src)
{
  dst[0] = src[1];
  dst[1] = src[0];
  return dst + 2;
}

static char*
copy_bytes_3(char *dst, char *src)
{
  dst[0] = src[2];
  dst[1] = src[1];
  dst[2] = src[0];
  return dst + 3;
}

static char*
copy_bytes_16(char *dst, char *src)
{
  *(uint64_t*)dst = *(uint64_t*)(src + 8);
  *(uint64_t*)(dst + 8) = *(uint64_t*)src;
  return dst + 16;
}

static void
copy_bytes(char *dst, uint32_t length, uint32_t offset)
{
  char *src = dst - offset;

  while (length-- > 0)
    *dst++ = *src++;
}


/* See spec version 5.0 page 30 */
static void
copy_compressed_bytes(char *dst, char *src, int length)
{
  while (length >= 32)
    {
      copy_16(16);
      copy_16(0);

      src += 32;
      length -= 32;
    }

  switch (length)
  {
    case 0:
      break;
    case 1:
      copy_1(0);
      break;
    case 2:
      copy_2(0);
      break;
    case 3:
      copy_3(0);
      break;
    case 4:
      copy_4(0);
      break;
    case 5:
      copy_1(4);
      copy_4(0);
      break;
    case 6:
      copy_1(5);
      copy_4(1);
      copy_1(0);
      break;
    case 7:
      copy_2(5);
      copy_4(1);
      copy_1(0);
      break;
    case 8:
      copy_8(0);
      break;
    case 9:
      copy_1(8);
      copy_8(0);
      break;
    case 10:
      copy_1(9);
      copy_8(1);
      copy_1(0);
      break;
    case 11:
      copy_2(9);
      copy_8(1);
      copy_1(0);
      break;
    case 12:
      copy_4(8);
      copy_8(0);
      break;
    case 13:
      copy_1(12);
      copy_4(8);
      copy_8(0);
      break;
    case 14:
      copy_1(13);
      copy_4(9);
      copy_8(1);
      copy_1(0);
      break;
    case 15:
      copy_2(13);
      copy_4(9);
      copy_8(1);
      copy_1(0);
      break;
    case 16:
      copy_16(0);
      break;
    case 17:
      copy_8(9);
      copy_1(8);
      copy_8(0);
      break;
    case 18:
      copy_1(17);
      copy_16(1);
      copy_1(0);
      break;
    case 19:
      copy_3(16);
      copy_16(0);
      break;
    case 20:
      copy_4(16);
      copy_16(0);
      break;
    case 21:
      copy_1(20);
      copy_4(16);
      copy_16(0);
      break;
    case 22:
      copy_2(20);
      copy_4(16);
      copy_16(0);
      break;
    case 23:
      copy_3(20);
      copy_4(16);
      copy_16(0);
      break;
    case 24:
      copy_8(16);
      copy_16(0);
      break;
    case 25:
      copy_8(17);
      copy_1(16);
      copy_16(0);
      break;
    case 26:
      copy_1(25);
      copy_8(17);
      copy_1(16);
      copy_16(0);
      break;
    case 27:
      copy_2(25);
      copy_8(17);
      copy_1(16);
      copy_16(0);
      break;
    case 28:
      copy_4(24);
      copy_8(16);
      copy_16(0);
      break;
    case 29:
      copy_1(28);
      copy_4(24);
      copy_8(16);
      copy_16(0);
      break;
    case 30:
      copy_2(28);
      copy_4(24);
      copy_8(16);
      copy_16(0);
      break;
    case 31:
      copy_1(30);
      copy_4(26);
      copy_8(18);
      copy_16(2);
      copy_2(0);
      break;
    default:
      LOG_ERROR("Wrong length %d", length);
  }
}

/* See spec version 5.1 page 50 */
static uint32_t
read_literal_length(unsigned char **src, unsigned char opcode)
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
read_instructions(unsigned char **src, unsigned char *opcode, uint32_t *offset,
                  uint32_t *length)
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
decompress_r2007(char *dst, int dst_size, char *src, int src_size)
{
  uint32_t length = 0;
  uint32_t offset = 0;

  char *dst_end = dst + dst_size;
  char *src_end = src + src_size;

  unsigned char opcode = *src++;

  if ((opcode & 0xf0) == 0x20)
    {
      src += 2;
      length = *src++ & 0x07;

      if (length == 0) {
        LOG_ERROR("Decompression error: zero length")
        return 1;
      }
    }

  while (src < src_end)
    {
      if (length == 0)
        length = read_literal_length((unsigned char**)&src, opcode);

      if ((dst + length) > dst_end) {
        LOG_ERROR("Decompression error: length overflow")
        return 1;
      }

      copy_compressed_bytes(dst, src, length);

      dst += length;
      src += length;

      length = 0;

      if (src >= src_end)
        return 0;

      opcode = *src++;

      read_instructions((unsigned char**)&src, &opcode, &offset, &length);

      while (1)
        {
          copy_bytes(dst, length, offset);

          dst += length;
          length = (opcode & 7);

          if (length != 0 || src >= src_end)
            break;

          opcode = *src++;

          if ((opcode >> 4) == 0)
            break;

          if ((opcode >> 4) == 0x0f)
            opcode &= 0xf;

          read_instructions((unsigned char**)&src, &opcode, &offset, &length);
        }
    }

  return 0;
}


// reed-solomon (255, 239) encoding with factor 3
// TODO: for now disabled, until we get proper data
static char*
decode_rs(const char *src, int block_count, int data_size)
{
  int i, j;
  const char *src_base = src;
  char *dst_base, *dst;
  //TODO: round up data_size from 239 to 255

  dst_base = dst = (char*)malloc(block_count * data_size);
  if (!dst)
    {
      LOG_ERROR("Out of memory")
      return NULL;
    }

  for (i = 0; i < block_count; ++i)
    {
      for (j = 0; j < data_size; ++j)
        {
          *dst++ = *src;
          src += block_count;
        }

      //rs_decode_block((unsigned char*)(dst_base + 239*i), 1);
      src = ++src_base;
    }

  return (dst_base);
}

static char*
read_system_page(Bit_Chain* dat, int64_t size_comp, int64_t size_uncomp,
                 int64_t repeat_count)
{
  int i;

  int64_t pesize;      // Pre RS encoded size
  int64_t block_count; // Number of RS encoded blocks
  int64_t page_size;

  char *rsdata;        // RS encoded data
  char *pedata;        // Pre RS encoded data
  char *data;          // The data RS unencoded and uncompressed

  // Round to a multiple of 8
  pesize = ((size_comp + 7) & ~7) * repeat_count;
  // Devide pre encoded size by RS k-value (239)
  block_count = (pesize + 238) / 239;
  // Multiply with codeword size (255) and round to a multiple of 8
  page_size = (block_count * 255 + 7) & ~7;

  assert((uint64_t)size_comp < DBG_MAX_SIZE);
  assert((uint64_t)size_uncomp < DBG_MAX_SIZE);
  assert((uint64_t)repeat_count < DBG_MAX_COUNT);
  assert((uint64_t)page_size < DBG_MAX_COUNT);

  data = (char*)malloc(size_uncomp + page_size);
  if (!data) {
    LOG_ERROR("Out of memory")
    return NULL;
  }

  rsdata = &data[size_uncomp];
  bit_read_fixed(dat, rsdata, page_size);
  pedata = decode_rs(rsdata, block_count, 239);

  if (size_comp < size_uncomp)
    decompress_r2007(data, size_uncomp, pedata, size_comp);
  else
    memcpy(data, pedata, size_uncomp);

  free(pedata);

  return data;
}

static int
read_data_page(Bit_Chain* dat, unsigned char *decomp, int64_t page_size,
               int64_t size_comp, int64_t size_uncomp)
{
  int i;

  int64_t pesize;      // Pre RS encoded size
  int64_t block_count; // Number of RS encoded blocks

  char *rsdata;        // RS encoded data
  char *pedata;        // Pre RS encoded data

    // Round to a multiple of 8
  pesize = ((size_comp + 7) & ~7);

  block_count = (pesize + 0xFB - 1) / 0xFB;

  rsdata = (char*)malloc(page_size * sizeof(char));
  if (rsdata == NULL) {
    LOG_ERROR("Out of memory")
    return 1;
  }
  bit_read_fixed(dat, rsdata, page_size);
  pedata = decode_rs(rsdata, block_count, 0xFB);

  if (size_comp < size_uncomp)
    decompress_r2007((char*)decomp, size_uncomp, pedata, size_comp);
  else
    memcpy(decomp, pedata, size_uncomp);

  free(pedata);

  return 0;
}

static int
read_data_section(Bit_Chain *sec_dat, Bit_Chain *dat, r2007_section *sections_map,
                  r2007_page *pages_map, Dwg_Section_Type sec_type)
{
  r2007_section *section;
  r2007_page *page;
  int64_t max_decomp_size;
  unsigned char *decomp;
  int i;

  section = get_section(sections_map, sec_type);
  if (section == NULL) {
    LOG_ERROR("Failed to find section %d", (int)sec_type)
    return 1;
  }

  max_decomp_size = section->data_size;
  decomp = (unsigned char *)malloc(max_decomp_size * sizeof(char));
  if (decomp == NULL) {
    LOG_ERROR("Out of memory")
    return 2;
  }

  for (i = 0; i < (int)section->num_pages; i++)
    {
      r2007_section_page *section_page = section->pages[i];
      page = get_page(pages_map, section_page->id);
      if (page == NULL)
        {
          free(decomp);
          LOG_ERROR("Failed to find page %d", (int)section_page->id)
          return 3;
        }

      dat->byte = page->offset;
      if (read_data_page(dat, &decomp[section_page->offset], page->size,
                         section_page->comp_size, section_page->uncomp_size)
          != 0)
        {
          free(decomp);
          LOG_ERROR("Failed to read page")
          return 4;
        }
    }

  sec_dat->bit     = 0;
  sec_dat->byte    = 0;
  sec_dat->chain   = decomp;
  sec_dat->size    = max_decomp_size;
  sec_dat->version = dat->version;

  return 0;
}

#define bfr_read_int16(_p)   *((int16_t*)_p);  _p += 2;
#define bfr_read_int64(_p)   *((int64_t*)_p);  _p += 8;

static void
bfr_read(void *dst, char **src, size_t size)
{
  memcpy(dst, *src, size);
  *src += size;
}

static DWGCHAR*
bfr_read_string(char **src)
{
  uint16_t *ptr = (uint16_t*)*src;
  int32_t length = 0, wsize;
  DWGCHAR *str, *str_base;
  int i;

  while (*ptr != 0)
    {
      ptr++;
      length++;
    }

  wsize = length * sizeof(DWGCHAR) + sizeof(DWGCHAR);

  str = str_base = (DWGCHAR*) malloc(wsize);
  if (!str)
    {
      LOG_ERROR("Out of memory");
      return NULL;
    }
  ptr = (uint16_t*)*src;
  for (i = 0; i < length; i++)
    {
      *str++ = (DWGCHAR)(*ptr++);
    }

  *src += length * 2 + 2;
  *str = 0;

  return str_base;
}

static r2007_section*
read_sections_map(Bit_Chain* dat, int64_t size_comp,
                  int64_t size_uncomp, int64_t correction)
{
  char *data;
  r2007_section *sections = 0, *last_section = 0, *section;
  char *ptr, *ptr_end;
  int i, j = 0;

  data = read_system_page(dat, size_comp, size_uncomp, correction);
  if (!data) {
    LOG_ERROR("Failed to read system page")
    return NULL;
  }

  ptr = data;
  ptr_end = data + size_uncomp;

  LOG_TRACE("\n=== System Section (Section Map) ===\n")

  while (ptr < ptr_end)
    {
      section = (r2007_section*) malloc(sizeof(r2007_section));
      if (!section)
        {
          LOG_ERROR("Out of memory");
          return NULL;
        }

      bfr_read(section, &ptr, 64);

      LOG_TRACE("Section [%d]:\n", j)
      LOG_TRACE("  data size:     %"PRIu64"\n", section->data_size)
      LOG_TRACE("  max size:      %"PRIu64"\n", section->max_size)
      LOG_TRACE("  encryption:    %"PRIu64"\n", section->encrypted)
      LOG_TRACE("  hashcode:      %"PRIx64"\n", section->hashcode)
      LOG_TRACE("  name length:   %"PRIu64"\n", section->name_length)
      LOG_TRACE("  unknown:       %"PRIu64"\n", section->unknown)
      LOG_TRACE("  encoding:      %"PRIu64"\n", section->encoded)
      LOG_TRACE("  num pages:     %"PRIu64"\n", section->num_pages)

      //debugging sanity
      assert(section->data_size <  DBG_MAX_SIZE);
      assert(section->max_size  <  DBG_MAX_SIZE);
      assert(section->name_length <  DBG_MAX_SIZE);
      assert(section->num_pages < 0x10000);

      section->next  = 0;
      section->pages = 0;

      if (sections == 0)
        sections = last_section = section;
      else
        {
          last_section->next = section;
          last_section = section;
        }

      j++;
      if (ptr >= ptr_end)
        break;

      // Section Name
      section->name = bfr_read_string(&ptr);
#ifdef HAVE_NATIVE_WCHAR2
      LOG_TRACE("  name:          " FORMAT_TU "\n", (BITCODE_TU)section->name)
#else
      LOG_TRACE("  name:          ")
      LOG_TEXT_UNICODE(TRACE, section->name)
      LOG_TRACE("\n")
#endif
      section->type = dwg_section_type(section->name);

      section->pages = (r2007_section_page**) malloc(
        (size_t)section->num_pages * sizeof(r2007_section_page*));
      if (!section->pages)
        {
          LOG_ERROR("Out of memory");
          return NULL;
        }

      for (i = 0; i < section->num_pages; i++)
        {
          section->pages[i] = (r2007_section_page*) malloc(
                                  sizeof(r2007_section_page));
          if (!section->pages[i])
            {
              LOG_ERROR("Out of memory");
              return NULL;
            }

          bfr_read(section->pages[i], &ptr, 56);

          LOG_TRACE("\n  Page[%d]:\n", i)
          LOG_TRACE("   offset:        %"PRIu64"\n", section->pages[i]->offset);
          LOG_TRACE("   size:          %"PRIu64"\n", section->pages[i]->size);
          LOG_TRACE("   id:            %"PRIu64"\n", section->pages[i]->id);
          LOG_TRACE("   uncomp_size:   %"PRIu64"\n",
                    section->pages[i]->uncomp_size);
          LOG_TRACE("   comp_size:     %"PRIu64"\n",
                    section->pages[i]->comp_size);
          LOG_TRACE("   checksum:      %"PRIx64"\n",
                    section->pages[i]->checksum);
          LOG_TRACE("   crc:           %"PRIx64"\n\n", section->pages[i]->crc);
          //debugging sanity
          assert(section->pages[i]->size < DBG_MAX_SIZE);
          assert(section->pages[i]->uncomp_size < DBG_MAX_SIZE);
          assert(section->pages[i]->comp_size < DBG_MAX_SIZE);
        }
    }

  free(data);

  return sections;
}

static r2007_page*
read_pages_map(Bit_Chain* dat, int64_t size_comp,
               int64_t size_uncomp, int64_t correction)
{
  char *data, *ptr, *ptr_end;
  r2007_page *pages = 0, *last_page = 0, *page;
  int64_t offset = 0x480;   //dat->byte;
  int64_t index;

  data = read_system_page(dat, size_comp, size_uncomp, correction);
  if (!data) {
    LOG_ERROR("Failed to read system page")
    return NULL;
  }

  ptr = data;
  ptr_end = data + size_uncomp;

  LOG_TRACE("\n=== System Section (Pages Map) ===\n")

  while (ptr < ptr_end)
    {
      page = (r2007_page*) malloc(sizeof(r2007_page));
      if (page == NULL)
        {
          LOG_ERROR("Out of memory")
          free(data);
          pages_destroy(pages);
          return NULL;
        }

      page->size   = bfr_read_int64(ptr);
      page->id     = bfr_read_int64(ptr);
      page->offset = offset;
      offset += page->size;

      index = page->id > 0 ? page->id : -page->id;

      LOG_TRACE("Page [%2"PRId64"]: ", page->id)
      LOG_TRACE("size: 0x%05"PRIx64" ", page->size)
      //LOG_TRACE("id:      0x%"PRId64" ", page->id)
      LOG_TRACE("offset: 0x6%"PRIx64" \n", page->offset)

      page->next = 0;

      if (pages == 0)
        pages = last_page = page;
      else
        {
          last_page->next = page;
          last_page = page;
        }
    }

  free(data);

  return pages;
}

/* Lookup a page in the page map. The page is identified by its id.
 */
static r2007_page*
get_page(r2007_page *pages_map, int64_t id)
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
pages_destroy(r2007_page *page)
{
  r2007_page *next;

  while (page != 0)
    {
      next = page->next;
      free(page);
      page = next;
    }
}

/* Lookup a section in the section map.
 * The section is identified by its numeric type.
 */
static r2007_section*
get_section(r2007_section *sections_map, Dwg_Section_Type sec_type)
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
sections_destroy(r2007_section *section)
{
  r2007_section *next;

  while (section != 0)
    {
      next = section->next;

      if (section->pages != 0)
        {
          while (section->num_pages-- > 0)
            {
              free(section->pages[section->num_pages]);
            }

          free(section->pages);
        }

      free(section);
      section = next;
    }
}

static void
read_file_header(Bit_Chain* dat, r2007_file_header *file_header)
{
  char data[0x3d8]; //0x400 - 5 long
  char *pedata;
  int64_t seqence_crc;
  int64_t seqence_key;
  int64_t compr_crc;
  int32_t compr_len;
  int i;

  dat->byte = 0x80;
  bit_read_fixed(dat, data, 0x3d8);
  pedata = decode_rs(data, 3, 239);

  seqence_crc = *((int64_t*)pedata);
  seqence_key = *((int64_t*)&pedata[8]);
  compr_crc   = *((int64_t*)&pedata[16]);
  compr_len   = *((int32_t*)&pedata[24]);

  if (compr_len > 0)
    decompress_r2007((char*)file_header, 0x110, &pedata[32], compr_len);
  else
    memcpy(file_header, &pedata[32], sizeof(r2007_file_header));

  // check validity, for debugging only
  assert((uint64_t)file_header->header_size < DBG_MAX_SIZE);
  assert((uint64_t)file_header->file_size < DBG_MAX_SIZE);
  assert((uint64_t)file_header->pages_map_offset < DBG_MAX_SIZE);
  assert((uint64_t)file_header->header2_offset < DBG_MAX_SIZE);
  assert((uint64_t)file_header->pages_map_offset < DBG_MAX_SIZE);
  assert((uint64_t)file_header->pages_map_size_comp < DBG_MAX_SIZE);
  assert((uint64_t)file_header->pages_map_size_uncomp < DBG_MAX_SIZE);
  assert((uint64_t)file_header->header2_offset < DBG_MAX_SIZE);

  assert((uint64_t)file_header->pages_maxid < DBG_MAX_COUNT);
  assert((uint64_t)file_header->pages_amount < DBG_MAX_COUNT);
  assert((uint64_t)file_header->sections_amount < DBG_MAX_COUNT);

  free(pedata);
}

void
obj_string_stream(Bit_Chain *dat, Dwg_Object *obj, Bit_Chain *str)
{
  BITCODE_RL start = obj->bitsize - 1; // in bits
  BITCODE_RL data_size; // in byte
  *str = *dat;
  bit_advance_position(str, start);
  LOG_TRACE("obj string stream +%u: @%lu/%u %lu", start,
            str->byte, str->bit & 7, bit_position(str));
  obj->has_strings = bit_read_B(str);
  LOG_TRACE(" has_strings: %d\n", (int)obj->has_strings);
  if (!obj->has_strings)
    return;
  bit_advance_position(str, -17);
  LOG_TRACE("  -17: @%lu/%u\n", str->byte, str->bit & 7);
  data_size = (BITCODE_RL)bit_read_RS(str);
  LOG_TRACE("  data_size: %u\n", data_size);
  if (data_size & 0x8000) {
    BITCODE_RS hi_size;
    bit_advance_position(str, -33);
    data_size &= 0x7FFF;
    hi_size = bit_read_RS(str);
    data_size |= (hi_size << 15);
    LOG_TRACE("  -33: @%lu\n", str->byte);
  }
  if (data_size > obj->bitsize)
    {
      bit_advance_position(str, -16);
      LOG_ERROR("Invalid string stream data_size: @%lu/%u", str->byte, str->bit & 7);
      obj->has_strings = 0;
      return;
    }
  bit_advance_position(str, -(int)data_size - 16);
  LOG_TRACE("  %d: @%lu/%u %lu\n", -(int)data_size - 16, str->byte, str->bit & 7,
            bit_position(str));
}

static void
section_string_stream(Bit_Chain *dat, BITCODE_RL bitsize, Bit_Chain *str)
{
  // 24 bytes (sentinel+size+hsize) - 1 bit (endbit)
  BITCODE_RL start = bitsize + 159; // in bits
  BITCODE_RS data_size; // in byte
  BITCODE_B endbit;
  *str = *dat;
  bit_set_position(str, start);
  LOG_TRACE("section string stream\n  pos: %u, %lu/%u\n", start, str->byte, str->bit);
  endbit = bit_read_B(str);
  LOG_TRACE("  endbit: %d\n", (int)endbit);
  start -= 16;
  bit_set_position(str, start);
  LOG_TRACE("  pos: %u, %lu\n", start, str->byte);
  //str->bit = start & 7;
  data_size = bit_read_RS(str);
  LOG_TRACE("  data_size: %u\n", data_size);
  if (data_size & 0x8000) {
    BITCODE_RS hi_size;
    start -= 16;
    data_size &= 0x7FFF;
    bit_set_position(str, start);
    LOG_TRACE("  pos: %u, %lu\n", start, str->byte);
    hi_size = bit_read_RS(str);
    data_size |= (hi_size << 15);
  }
  start -= data_size;
  bit_set_position(str, start);
  LOG_TRACE("  pos: %u, %lu/%u\n", start, str->byte, str->bit);
}

// for string stream see p86
static int
read_2007_section_classes(Bit_Chain* dat, Dwg_Data *dwg,
                          r2007_section *sections_map, r2007_page *pages_map)
{
  BITCODE_RL size;
  BITCODE_BS max_num;
  Bit_Chain sec_dat, str;
  int idc;
  int error;
  char c;

  sec_dat.chain = NULL;
  error = read_data_section(&sec_dat, dat, sections_map,
                            pages_map, SECTION_CLASSES);
  if (error)
    {
      LOG_ERROR("Failed to read class section");
      if (sec_dat.chain)
        free(sec_dat.chain);
      return error;
    }

  if (bit_search_sentinel(&sec_dat, dwg_sentinel(DWG_SENTINEL_CLASS_BEGIN)))
    {
      BITCODE_RL bitsize = 0;
      LOG_TRACE("\nClasses\n-------------------\n")
      size = bit_read_RL(&sec_dat);  // size of class data area
      LOG_TRACE("size: " FORMAT_RL " [RL]\n", size)
      if (dat->version >= R_2010 && dwg->header.maint_version > 3)
        {
          BITCODE_RL hsize = bit_read_RL(&sec_dat);
          LOG_TRACE("hsize: " FORMAT_RL " [RL]\n", hsize)
        }
      if (dat->version >= R_2007)
        {
          bitsize = bit_read_RL(&sec_dat);
          LOG_TRACE("bitsize: " FORMAT_RL " [RL]\n", bitsize)
        }
      max_num = bit_read_BS(&sec_dat);  // Maximum class number
      LOG_TRACE("max_num: " FORMAT_BS " [BS]\n", max_num)
      c = bit_read_RC(&sec_dat);        // 0x00
      LOG_TRACE("c: " FORMAT_RC " [RC]\n", c)
      c = bit_read_RC(&sec_dat);        // 0x00
      LOG_TRACE("c: " FORMAT_RC " [RC]\n", c)
      c = bit_read_B(&sec_dat);         // 1
      LOG_TRACE("c: " FORMAT_B " [B]\n", c);

      dwg->layout_number = 0;
      dwg->num_classes = max_num - 500;
      if (max_num < 500 || max_num > 5000)
        {
          LOG_ERROR("Invalid max class number %d", max_num)
          return 1;
        }
      assert(max_num >= 500);
      assert(max_num < 5000);

      section_string_stream(&sec_dat, bitsize, &str);

      dwg->dwg_class = (Dwg_Class *) calloc(max_num-500, sizeof(Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR("Out of memory");
          if (sec_dat.chain)
            free(sec_dat.chain);
          return 2;
        }

      for (idc = 0; idc < max_num-500; idc++)
        {
          char name[8];
          dwg->dwg_class[idc].number        = bit_read_BS(&sec_dat);
          dwg->dwg_class[idc].proxyflag     = bit_read_BS(&sec_dat);
          dwg->dwg_class[idc].appname       = (char*)bit_read_TU(&str);
          dwg->dwg_class[idc].cppname       = (char*)bit_read_TU(&str);
          dwg->dwg_class[idc].dxfname       = (char*)bit_read_TU(&str);
          dwg->dwg_class[idc].wasazombie    = bit_read_B(&sec_dat);
          dwg->dwg_class[idc].item_class_id = bit_read_BS(&sec_dat);

          dwg->dwg_class[idc].instance_count = bit_read_BL(&sec_dat);  // DXF 91
          dwg->dwg_class[idc].dwg_version   = bit_read_BS(&sec_dat);
          dwg->dwg_class[idc].maint_version = bit_read_BS(&sec_dat);
          dwg->dwg_class[idc].unknown_1     = bit_read_BL(&sec_dat);  // 0
          dwg->dwg_class[idc].unknown_2     = bit_read_BL(&sec_dat);  // 0

          LOG_TRACE("-------------------\n")
          LOG_TRACE("Number:           %d\n", dwg->dwg_class[idc].number)
          LOG_TRACE("Proxyflag:        0x%x\n", dwg->dwg_class[idc].proxyflag)
          LOG_TRACE_TU("Application name: ", dwg->dwg_class[idc].appname)
          LOG_TRACE_TU("C++ class name:   ", dwg->dwg_class[idc].cppname)
          LOG_TRACE_TU("DXF record name:  ", dwg->dwg_class[idc].dxfname)
          LOG_TRACE("Class ID:         0x%x "
                    "(0x1f3 for object, 0x1f2 for entity)\n",
                    dwg->dwg_class[idc].item_class_id)
          LOG_TRACE("instance count:   %u\n",
                    dwg->dwg_class[idc].instance_count)
          LOG_TRACE("dwg version:      %u (%u)\n",
                    dwg->dwg_class[idc].dwg_version,
                    dwg->dwg_class[idc].maint_version)
          LOG_TRACE("unknown:          %u %u\n", dwg->dwg_class[idc].unknown_1,
                    dwg->dwg_class[idc].unknown_2)
          {
            BITCODE_TU ws = (BITCODE_TU)dwg->dwg_class[idc].dxfname;
            for (int i=0; i<7; i++) {
              uint16_t _c = *ws++;
              name[i] = _c & 0xff;
              if (!_c)
                break;
            }
          }
          if (strcmp(name, "LAYOUT") == 0)
            dwg->layout_number = dwg->dwg_class[idc].number;
        }
    }
  else
    {
      LOG_ERROR("Failed to find class section sentinel");
      free(sec_dat.chain);
      return 1;
    }
  free(sec_dat.chain);
  return 0;
}

static int
read_2007_section_header(Bit_Chain* dat, Bit_Chain* hdl_dat, Dwg_Data *dwg,
                         r2007_section *sections_map, r2007_page *pages_map)
{
  Bit_Chain sec_dat, str_dat;
  int error;
  LOG_TRACE("\nHeader\n-------------------\n")
  error = read_data_section(&sec_dat, dat, sections_map,
                            pages_map, SECTION_HEADER);
  if (error)
    {
      LOG_ERROR("Failed to read header section");
      if (sec_dat.chain)
        free(sec_dat.chain);
      return error;
    }
  if (bit_search_sentinel(&sec_dat, dwg_sentinel(DWG_SENTINEL_VARIABLE_BEGIN)))
    {
      unsigned long int size = bit_read_RL(&sec_dat);
      BITCODE_RL bitsize = 0;
      BITCODE_RL endbits = 160; //start bit: 16 sentinel + 4 size
      LOG_TRACE("Length: %lu\n", size);
      *hdl_dat = sec_dat;
      if (dat->version >= R_2010 && dwg->header.maint_version > 3)
        {
          BITCODE_RL hsize = bit_read_RL(&sec_dat);
          LOG_TRACE("hsize: " FORMAT_RL " [RL]\n", hsize)
        }
      if (dat->version >= R_2007)
        {
          Bit_Chain sav_dat = sec_dat;
          bitsize = bit_read_RL(&sec_dat);
          LOG_TRACE("bitsize: " FORMAT_RL " [RL]\n", bitsize);
          endbits += bitsize;
          bit_set_position(hdl_dat, endbits);

          section_string_stream(&sec_dat, bitsize, &str_dat);
          sec_dat = sav_dat;
        }

      dwg_decode_header_variables(&sec_dat, hdl_dat, &str_dat, dwg);
    }
  else {
    DEBUG_HERE();
    error = 1;
  }
  return error;
}

static int
read_2007_section_handles(Bit_Chain* dat, Bit_Chain* hdl, Dwg_Data *dwg,
                          r2007_section *sections_map, r2007_page *pages_map)
{
  Bit_Chain obj_dat, hdl_dat;
  BITCODE_RS section_size = 0;
  long unsigned int endpos;
  int error;

  error = read_data_section(&obj_dat, dat, sections_map,
                            pages_map, SECTION_OBJECTS);
  if (error)
    {
      LOG_ERROR("Failed to read objects section");
      if (obj_dat.chain)
        free(obj_dat.chain);
      return error;
    }

  LOG_TRACE("\nHandles\n-------------------\n")
  error = read_data_section(&hdl_dat, dat, sections_map,
                            pages_map, SECTION_HANDLES);
  if (error)
    {
      LOG_ERROR("Failed to read handles section");
      if (hdl_dat.chain)
        free(hdl_dat.chain);
      return error;
    }

  endpos = hdl_dat.byte + hdl_dat.size;
  dwg->num_objects = 0;

  do
    {
      long unsigned int last_offset;
      long unsigned int last_handle;
      long unsigned int oldpos = 0;
      long unsigned int startpos = hdl_dat.byte;

      section_size = bit_read_RS_LE(&hdl_dat);
      LOG_TRACE("Section size: %u\n", section_size);
      if (section_size > 2034)
        {
          LOG_ERROR("Object-map section size greater than 2034!");
          return 1;
        }

      last_handle = 0;
      last_offset = 0;
      while (hdl_dat.byte - startpos < section_size)
        {
          long int handle, offset;
          oldpos = hdl_dat.byte;

          handle = bit_read_MC(&hdl_dat);
          offset = bit_read_MC(&hdl_dat);
          last_handle += handle;
          last_offset += offset;
          LOG_TRACE("\nNext object: %li\t", dwg->num_objects)
          LOG_TRACE("Handle: %li\tOffset: %li\n", handle, offset)

          dwg_decode_add_object(dwg, &obj_dat, hdl, last_offset);
        }

      if (hdl_dat.byte == oldpos)
        break;
      hdl_dat.byte += 2; // CRC

      if (hdl_dat.byte >= endpos)
        break;
    }
  while (section_size > 2);

  LOG_TRACE("\nNum objects: %lu\n", dwg->num_objects);

  free(hdl_dat.chain);
  free(obj_dat.chain);

  return error;
}

/* exported */
int
read_r2007_meta_data(Bit_Chain *dat, Bit_Chain *hdl_dat, Dwg_Data *dwg)
{
  r2007_file_header file_header;
  r2007_page *pages_map, *page;
  r2007_section *sections_map;
  int error;

#ifdef USE_TRACING
  char *probe = getenv ("LIBREDWG_TRACE");
  if (probe)
    loglevel = atoi (probe);
#endif
  // @ 0x62
  read_file_header(dat, &file_header);

  // Pages Map
  dat->byte += 0x28;  // overread check data
  dat->byte += file_header.pages_map_offset;

  pages_map = read_pages_map(dat, file_header.pages_map_size_comp,
    file_header.pages_map_size_uncomp, file_header.pages_map_correction);
  if (!pages_map)
    return 1;

  // Sections Map
  page = get_page(pages_map, file_header.sections_map_id);
  if (!page)
    {
      LOG_ERROR("Failed to find sections page map %d", (int)file_header.sections_map_id);
      return 3;
    }
  dat->byte = page->offset;
  sections_map = read_sections_map(dat, file_header.sections_map_size_comp,
                                   file_header.sections_map_size_uncomp,
                                   file_header.sections_map_correction);

  error = read_2007_section_classes(dat, dwg, sections_map, pages_map);
  error += read_2007_section_header(dat, hdl_dat, dwg, sections_map, pages_map);
  error += read_2007_section_handles(dat, hdl_dat, dwg, sections_map, pages_map);
  //read_2007_blocks(dat, hdl_dat, dwg, sections_map, pages_map);

  /////////////////////////////////////////
  //	incomplete implementation
  /////////////////////////////////////////

  pages_destroy(pages_map);
  if (page)
    sections_destroy(sections_map);

  return error;
}

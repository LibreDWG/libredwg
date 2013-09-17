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
#include "bits.h"
#include "logging.h"

static unsigned int loglevel;

#define DWG_LOGLEVEL loglevel

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
  wchar_t *name;
  r2007_section_page **pages;
  struct _r2007_section *next;
} r2007_section;


r2007_section* get_section(r2007_section *sections_map, int64_t hashcode);
r2007_page* get_page(r2007_page *pages_map, int64_t id);
void pages_destroy(r2007_page *page);
void sections_destroy(r2007_section *section);

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


char* 
copy_bytes_2(char *dst, char *src)
{
  dst[0] = src[1];
  dst[1] = src[0];
  return dst + 2;
}

char* 
copy_bytes_3(char *dst, char *src)
{
  dst[0] = src[2];
  dst[1] = src[1];
  dst[2] = src[0];
  return dst + 3;
}

char* 
copy_bytes_16(char *dst, char *src)
{
  *(uint64_t*)dst = *(uint64_t*)(src + 8);
  *(uint64_t*)(dst + 8) = *(uint64_t*)src;  
  return dst + 16;
}

void 
copy_bytes(char *dst, uint32_t length, uint32_t offset)
{
  char *src = dst - offset;
  
  while (length-- > 0)
    *dst++ = *src++;
}


/* See spec version 5.0 page 30 */
void
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
      //default:
      //nop
  }
}

/* See spec version 5.1 page 50 */
uint32_t
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
void 
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

/* See spec version 5.0 pp. 29 */
int 
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
    
      if (length == 0)
        return 1;   
    } 
  
  while (src < src_end)
    {   
      if (length == 0)
	      length = read_literal_length((unsigned char**)&src, opcode);
      
      if ((dst + length) > dst_end)
        return 1;
      
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


char*
decode_rs(const char *src, int block_count, int data_size)
{
  int i, j;
  const char *src_base = src;
  char *dst_base, *dst;  
  
  dst_base = dst = (char*)malloc(block_count * data_size);
  
  for (i = 0; i < block_count; ++i)
    {      
      for (j = 0; j < data_size; ++j)
        {
          *dst++ = *src;
          src += block_count;
        }
      
      src = ++src_base;
    }
  
  return (dst_base);
}

char*
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
  
  
  data = (char*)malloc(size_uncomp + page_size);
  if (data == 0)
    {
      //TODO: report error
      return 0;
    }
  
  rsdata = &data[size_uncomp];
  
  for (i = 0; i < page_size; i++)
    rsdata[i] = bit_read_RC(dat);
  
  pedata = decode_rs(rsdata, block_count, 239);
  
  if (size_comp < size_uncomp)
    decompress_r2007(data, size_uncomp, pedata, size_comp);
  else
    memcpy(data, pedata, size_uncomp);
  
  free(pedata);
  
  return data;
}

int
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
  if (rsdata == NULL)
    return 1;
  
  for (i = 0; i < page_size; i++)
    rsdata[i] = bit_read_RC(dat);  
  
  pedata = decode_rs(rsdata, block_count, 0xFB);
  
  if (size_comp < size_uncomp)
    decompress_r2007((char*)decomp, size_uncomp, pedata, size_comp);
  else
    memcpy(decomp, pedata, size_uncomp);
  
  free(pedata);
  
  return 0;
}

int
read_data_section(Bit_Chain *sec_dat, Bit_Chain *dat, r2007_section *sections_map, 
                  r2007_page *pages_map, int64_t hashcode)
{
  r2007_section *section;
  r2007_page *page;
  int64_t max_decomp_size;
  unsigned char *decomp;
  int i;
  
  section = get_section(sections_map, hashcode);
  if (section == NULL)
    return 1;   // Failed to find section
  
  max_decomp_size = section->data_size;
  
  decomp = (unsigned char *)malloc(max_decomp_size * sizeof(char));
  if (decomp == NULL)
    return 2;   // No memory  
  
  for (i = 0; i < (int)section->num_pages; i++)
    {
      page = get_page(pages_map, section->pages[i]->id);
      if (page == NULL)
        {
          free(decomp);
          return 3;   // Failed to find page
        }
    
      dat->byte = page->offset; 
      if (read_data_page(dat, &decomp[section->pages[i]->offset], page->size, 
                         section->pages[i]->comp_size, section->pages[i]->uncomp_size) != 0)
        {
          free(decomp);
          return 4;   // Failed to read page
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

void
bfr_read(void *dst, char **src, size_t size)
{
  memcpy(dst, *src, size);
  *src += size;
}

DWGCHAR*
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
  
  ptr = (uint16_t*)*src;
  for (i = 0; i < length; i++)
    {
      *str++ = (DWGCHAR)(*ptr++);
    }
  
  *src += length * 2 + 2;
  *str = 0;
  
  return str_base;
}

r2007_section*
read_sections_map(Bit_Chain* dat, int64_t size_comp, 
                  int64_t size_uncomp, int64_t correction)
{
  char *data;
  r2007_section *sections = 0, *last_section = 0, *section;
  char *ptr, *ptr_end;
  int i;
  
  data = read_system_page(dat, size_comp, size_uncomp, correction);
  
  ptr = data;
  ptr_end = data + size_uncomp;
  
  LOG_TRACE("\n=== System Section (Section Map) ===\n")
  
  while (ptr < ptr_end)
    {
      section = (r2007_section*) malloc(sizeof(r2007_section));
    
      bfr_read(section, &ptr, 64);
    
      LOG_TRACE("\n--- Section ---\n")
      LOG_TRACE("data size:     %lld\n", section->data_size)
      LOG_TRACE("max size:      %lld\n", section->max_size)
      LOG_TRACE("encryption:    %lld\n", section->encrypted)
      LOG_TRACE("hashcode:      %llx\n", section->hashcode)
      LOG_TRACE("name length:   %lld\n", section->name_length)
      LOG_TRACE("unknown:       %lld\n", section->unknown)
      LOG_TRACE("encoding:      %lld\n", section->encoded)
      LOG_TRACE("num pages:     %lld\n", section->num_pages)      
    
      section->next  = 0;
      section->pages = 0;
    
      if (sections == 0)
        sections = last_section = section;
      else
        {
          last_section->next = section;
          last_section = section;
        } 
    
      if (ptr >= ptr_end)
        break;
    
      // Section Name
      section->name = bfr_read_string(&ptr);
    
      LOG_TRACE("Section name:  %ls\n", (DWGCHAR*)section->name)      
    
      section->pages = (r2007_section_page**) malloc(
        (size_t)section->num_pages * sizeof(r2007_section_page*));
    
      for (i = 0; i < section->num_pages; i++)
        {
          section->pages[i] = (r2007_section_page*) malloc(
                                                       sizeof(r2007_section_page));
      
          bfr_read(section->pages[i], &ptr, 56);
      
          LOG_TRACE("\n   --- Page ---\n")
          LOG_TRACE("   offset:        %lld\n", section->pages[i]->offset);
          LOG_TRACE("   size:          %lld\n", section->pages[i]->size);
          LOG_TRACE("   id:            %lld\n", section->pages[i]->id);
          LOG_TRACE("   uncomp_size:   %lld\n", section->pages[i]->uncomp_size);
          LOG_TRACE("   comp_size:     %lld\n", section->pages[i]->comp_size);
          LOG_TRACE("   checksum:      %llx\n", section->pages[i]->checksum);
          LOG_TRACE("   crc:           %llx\n\n", section->pages[i]->crc);
        }
    }
  
  free(data);
  
  return sections;
}

r2007_page*
read_pages_map(Bit_Chain* dat, int64_t size_comp,
               int64_t size_uncomp, int64_t correction)
{
  char *data, *ptr, *ptr_end;  
  r2007_page *pages = 0, *last_page = 0, *page;
  int64_t offset = 0x480;   //dat->byte;
  int64_t index;
  
  data = read_system_page(dat, size_comp, size_uncomp, correction);
  
  if (data == NULL)
    return NULL;  
  
  ptr = data;
  ptr_end = data + size_uncomp;
  
  LOG_TRACE("\n=== System Section (Pages Map) ===\n")
  
  while (ptr < ptr_end)
    {
      page = (r2007_page*) malloc(sizeof(r2007_page));
      if (page == NULL)
        {
          //TODO: report error
          free(data);
          pages_destroy(pages);
          return NULL;
        }
    
      page->size   = bfr_read_int64(ptr);
      page->id     = bfr_read_int64(ptr);
      page->offset = offset;
      offset += page->size;
    
      index = page->id > 0 ? page->id : -page->id;
    
      LOG_TRACE("\n--- Page ---\n")
      LOG_TRACE("size:    0x%llx\n", page->size)
      LOG_TRACE("id:      0x%llx\n", page->id)
      LOG_TRACE("offset:  0x%llx\n\n", page->offset)
    
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
r2007_page*
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

void
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

/* Lookup a section in the section map. The section is identified by its hashcode.
 */
r2007_section*
get_section(r2007_section *sections_map, int64_t hashcode)
{
  r2007_section *section = sections_map;
  
  while (section != NULL)
    {
      if (section->hashcode == hashcode)
        break;
      section = section->next;
    }
  
  return section;
}

void
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

void 
read_file_header(Bit_Chain* dat, r2007_file_header *file_header)
{
  char data[0x3d8];
  char *pedata;
  int64_t seqence_crc;
  int64_t seqence_key;
  int64_t compr_crc;
  int32_t compr_len;
  int i;
  
  dat->byte = 0x80;
  for (i = 0; i < 0x3d8; i++)
    data[i] = bit_read_RC(dat);
  
  pedata = decode_rs(data, 3, 239);
  
  seqence_crc = *((int64_t*)pedata);
  seqence_key = *((int64_t*)&pedata[8]);
  compr_crc   = *((int64_t*)&pedata[16]);
  compr_len   = *((int32_t*)&pedata[24]);
  
  if (compr_len > 0)
    decompress_r2007((char*)file_header, 0x110, &pedata[32], compr_len);
  else
    memcpy(file_header, &pedata[32], sizeof(r2007_file_header));
  
  free(pedata);
}

int
read_r2007_meta_data(Bit_Chain *dat, Dwg_Data *dwg)
{
  r2007_file_header file_header;
  r2007_page *pages_map, *page;
  r2007_section *sections_map;
   
  loglevel = 9;

  read_file_header(dat, &file_header);
  
    // Pages Map
  dat->byte += 0x28;  // overread check data
  dat->byte += file_header.pages_map_offset;
  
  pages_map = read_pages_map(dat, file_header.pages_map_size_comp,
    file_header.pages_map_size_uncomp, file_header.pages_map_correction);  
  
    // Sections Map
  page = get_page(pages_map, file_header.sections_map_id);
  
  if (page != NULL)
    {
      dat->byte = page->offset;
      sections_map = read_sections_map(dat, file_header.sections_map_size_comp,
        file_header.sections_map_size_uncomp, file_header.sections_map_correction);
    }
  
  // Section Classes
  //read_r2007_section_classes(dat, dwg, sections_map, pages_map);
  
  // Section Header
  //read_r2007_section_header(dat, dwg, sections_map, pages_map);
  
  pages_destroy(pages_map);
  sections_destroy(sections_map);   

  return 0;
}

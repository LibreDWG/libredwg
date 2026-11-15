/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2026 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * encode_r2007.c: writes the container of a r2007 DWG (AC1021), from the
 * section buffers which dwg_encode() builds for r2004+.
 *
 * Layout of the file (the offsets of the pages are relative to 0x480):
 *
 *   0x000  the fixed header, 0x80 bytes, written by dwg_encode
 *   0x080  the file header: 0x400 bytes. 3 Reed-Solomon (255,239) blocks
 *          with the compressed Dwg_R2007_Header, and some check data
 *   0x480  the pages map, and its copy
 *          the data pages of all sections, Reed-Solomon (255,251) coded
 *          the sections map, and its copy
 *          a copy of the file header
 *
 * The compression is the same LZ77 as of r2004, see compress_R2004_section.
 * The system pages (the maps) are stored uncompressed, with their Reed-Solomon
 * parity, repeated "correction" times.
 */

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"
#include "reedsolomon.h"
#include "logging.h"

#define R2007_DATA_START 0x480
#define R2007_HEADER_SIZE 0x400
#define R2007_PAGE_MAX 0xF800 /* the uncompressed size of a data page */
#define R2007_FIRST_ID 3
#define R2007_CORRECTION 2 /* how often a system page is repeated */

typedef struct
{
  int64_t id;
  size_t size;           /* the size on disk */
  unsigned char *data;   /* the size bytes on disk, malloced */
} Page;

typedef struct
{
  Dwg_Section_Type type;
  uint64_t data_size;
  uint64_t max_size;
  int num_pages;
  int first_page; /* index into the pages array */
  uint64_t *uncomp_size;
  uint64_t *comp_size;
  uint64_t *offset;
  size_t *size;
  int64_t *id;
} Sec;

static void
put_le64 (unsigned char *p, const uint64_t v)
{
  int i;
  for (i = 0; i < 8; i++)
    p[i] = (unsigned char)(v >> (8 * i));
}

static void
put_le32 (unsigned char *p, const uint32_t v)
{
  int i;
  for (i = 0; i < 4; i++)
    p[i] = (unsigned char)(v >> (8 * i));
}

/* round up to a multiple of 8, 32 */
#define ROUND8(x) (((x) + 7) & ~(size_t)7)
#define ROUND32(x) (((x) + 31) & ~(size_t)31)

/* The size of a Reed-Solomon coded page of size_comp payload bytes, repeated
   repeat times, with blocks of data_size data bytes. The same as in the
   decoder, page_size_if_rs_coded. */
static size_t
rs_blocks (const size_t size_comp, const int repeat, const int data_size)
{
  const size_t pesize = ROUND8 (size_comp) * (size_t)repeat;
  return (pesize + data_size - 1) / data_size;
}

/* Reed-Solomon encode the payload. The blocks of data_size data bytes get
   their parity, and the bytes of all the blocks are interleaved. Returns a
   malloced buffer of block_count * 255 bytes, or NULL. */
static unsigned char *
rs_encode_page (const unsigned char *src, const size_t size_comp,
                const int repeat, const int data_size, size_t *restrict nblocks)
{
  const size_t block_count = rs_blocks (size_comp, repeat, data_size);
  const size_t unit = ROUND8 (size_comp);
  unsigned char *pe, *out;
  size_t i, j;
  int r;

  pe = (unsigned char *)calloc (block_count, data_size);
  out = (unsigned char *)calloc (block_count, 255);
  if (!pe || !out)
    {
      free (pe);
      free (out);
      return NULL;
    }
  for (r = 0; r < repeat; r++)
    if (size_comp)
      memcpy (&pe[r * unit], src, size_comp);
  for (i = 0; i < block_count; i++)
    {
      unsigned char blk[255];
      memcpy (blk, &pe[i * data_size], data_size);
      if (data_size == 239)
        rs_encode_block (&blk[239], blk, 239);
      else
        rs_encode_data_block (&blk[251], blk, 251);
      for (j = 0; j < 255; j++)
        out[i + j * block_count] = blk[j];
    }
  free (pe);
  *nblocks = block_count;
  return out;
}

/* A page of the file: the payload, optionally compressed, Reed-Solomon coded
   with 251 data bytes, and padded to 32. Fills size_comp. */
static int
make_data_page (Page *restrict page, Bit_Chain *restrict tmp,
                BITCODE_RC *restrict src, const uint32_t size_uncomp,
                uint64_t *restrict size_comp)
{
  const unsigned char *payload = src;
  uint32_t comp = 0;
  size_t block_count;
  unsigned char *rs;

  (void)tmp;
  if (!comp)
    comp = size_uncomp;
  *size_comp = comp;
  rs = rs_encode_page (payload, comp, 1, 251, &block_count);
  if (!rs)
    return DWG_ERR_OUTOFMEM;
  page->size = ROUND32 (block_count * 255);
  page->data = (unsigned char *)calloc (1, page->size);
  if (!page->data)
    {
      free (rs);
      return DWG_ERR_OUTOFMEM;
    }
  memcpy (page->data, rs, block_count * 255);
  free (rs);
  return 0;
}

/* A system page: stored uncompressed, Reed-Solomon coded with 239 data bytes,
   repeated R2007_CORRECTION times, and padded to 32. */
static int
make_system_page (Page *restrict page, const unsigned char *src,
                  const size_t size)
{
  size_t block_count;
  unsigned char *rs
      = rs_encode_page (src, size, R2007_CORRECTION, 239, &block_count);
  if (!rs)
    return DWG_ERR_OUTOFMEM;
  page->size = ROUND32 (block_count * 255);
  page->data = (unsigned char *)calloc (1, page->size);
  if (!page->data)
    {
      free (rs);
      return DWG_ERR_OUTOFMEM;
    }
  memcpy (page->data, rs, block_count * 255);
  free (rs);
  return 0;
}

/* The 0x400 bytes of the file header: the compressed Dwg_R2007_Header in 3
   Reed-Solomon blocks, and the check data. */
static int
make_file_header (unsigned char *restrict out,
                  const unsigned char *restrict hdr, Bit_Chain *restrict tmp)
{
  unsigned char pe[3 * 239];
  memset (pe, 0, sizeof (pe));
  memset (out, 0, R2007_HEADER_SIZE);
  (void)tmp;
  /* a negative length: not compressed */
  put_le32 (&pe[24], (uint32_t)-(int32_t)sizeof (Dwg_R2007_Header));
  put_le32 (&pe[28], (uint32_t)sizeof (Dwg_R2007_Header));
  memcpy (&pe[32], hdr, sizeof (Dwg_R2007_Header));
  /* 3 blocks of 239 consecutive bytes, interleaved */
  {
    size_t i, j;
    for (i = 0; i < 3; i++)
      {
        unsigned char blk[255];
        memcpy (blk, &pe[i * 239], 239);
        rs_encode_block (&blk[239], blk, 239);
        for (j = 0; j < 255; j++)
          out[i + 3 * j] = blk[j];
      }
  }
  return 0;
}

static void
free_pages (Page *pages, const int n)
{
  int i;
  for (i = 0; i < n; i++)
    free (pages[i].data);
  free (pages);
}

static void
free_secs (Sec *secs, const int n)
{
  int i;
  for (i = 0; i < n; i++)
    {
      free (secs[i].uncomp_size);
      free (secs[i].comp_size);
      free (secs[i].offset);
      free (secs[i].size);
      free (secs[i].id);
    }
  free (secs);
}

/* The sections map: for each section its description, the UTF-16LE name, and
   the descriptions of its pages. */
static unsigned char *
make_sections_map (const Dwg_Data *restrict dwg, const Sec *restrict secs,
                   const int num_secs, size_t *restrict size)
{
  size_t total = 0, pos = 0;
  unsigned char *buf;
  int i, k;

  for (i = 0; i < num_secs; i++)
    {
      const char *name = dwg_section_name (dwg, secs[i].type);
      total += 64 + 2 * strlen (name) + 56 * (size_t)secs[i].num_pages;
    }
  buf = (unsigned char *)calloc (1, total + 8);
  if (!buf)
    return NULL;
  for (i = 0; i < num_secs; i++)
    {
      const char *name = dwg_section_name (dwg, secs[i].type);
      const size_t len = strlen (name);
      size_t c;
      put_le64 (&buf[pos], secs[i].data_size);
      put_le64 (&buf[pos + 8], secs[i].max_size);
      put_le64 (&buf[pos + 16], 0); // encrypted
      put_le64 (&buf[pos + 24], 0); // hashcode
      put_le64 (&buf[pos + 32], 2 * len);
      put_le64 (&buf[pos + 40], 0);
      put_le64 (&buf[pos + 48], 4); // RS encoded
      put_le64 (&buf[pos + 56], (uint64_t)secs[i].num_pages);
      pos += 64;
      for (c = 0; c < len; c++)
        {
          buf[pos + 2 * c] = (unsigned char)name[c];
          buf[pos + 2 * c + 1] = 0;
        }
      pos += 2 * len;
      for (k = 0; k < secs[i].num_pages; k++)
        {
          put_le64 (&buf[pos], secs[i].offset[k]);
          put_le64 (&buf[pos + 8], ROUND8 (secs[i].uncomp_size[k]));
          put_le64 (&buf[pos + 16], (uint64_t)secs[i].id[k]);
          put_le64 (&buf[pos + 24], secs[i].uncomp_size[k]);
          put_le64 (&buf[pos + 32], secs[i].comp_size[k]);
          put_le64 (&buf[pos + 40], 0); // checksum
          put_le64 (&buf[pos + 48], 0); // crc
          pos += 56;
        }
    }
  *size = pos;
  return buf;
}

/* Write the r2007 file after the 0x80 bytes of the fixed header, from the
   section buffers sec_dat, indexed by the Dwg_Section_Type. */
int
encode_r2007 (Dwg_Data *restrict dwg, Bit_Chain *restrict dat,
              Bit_Chain *restrict sec_dat)
{
  int error = 0;
  Dwg_R2007_Header hdr;
  Sec *secs = NULL;
  Page *pages = NULL; /* the data pages, then the system pages */
  int num_secs = 0, num_data_pages = 0, npages = 0;
  int type, i, k;
  int64_t next_id = R2007_FIRST_ID;
  unsigned char *map = NULL, *secmap = NULL;
  size_t map_size = 0, secmap_size = 0, total = 0, off;
  Page pm[2] = { { 0, 0, NULL }, { 0, 0, NULL } };
  Page sm[2] = { { 0, 0, NULL }, { 0, 0, NULL } };
  Bit_Chain tmp = { 0 };
  unsigned char fhdr[R2007_HEADER_SIZE];
  unsigned char hbuf[sizeof (Dwg_R2007_Header)];

  LOG_TRACE ("\n#### r2007 File ####\n");
  bit_chain_init_dat (&tmp, 0x10000, dat);

  /* count the sections and their pages */
  for (type = SECTION_HEADER; type < SECTION_SYSTEM_MAP; type++)
    if (sec_dat[type].chain && sec_dat[type].byte)
      {
        num_secs++;
        num_data_pages
            += (int)((sec_dat[type].byte + R2007_PAGE_MAX - 1) / R2007_PAGE_MAX);
      }
  secs = (Sec *)calloc (num_secs ? num_secs : 1, sizeof (Sec));
  pages = (Page *)calloc (num_data_pages + 4, sizeof (Page));
  if (!secs || !pages)
    {
      error = DWG_ERR_OUTOFMEM;
      goto done;
    }

  /* the data pages */
  i = 0;
  for (type = SECTION_HEADER; type < SECTION_SYSTEM_MAP; type++)
    {
      const size_t size = sec_dat[type].byte;
      Sec *sec;
      if (!sec_dat[type].chain || !size)
        continue;
      sec = &secs[i++];
      sec->type = (Dwg_Section_Type)type;
      sec->data_size = size;
      sec->num_pages = (int)((size + R2007_PAGE_MAX - 1) / R2007_PAGE_MAX);
      sec->first_page = npages;
      sec->max_size = size < R2007_PAGE_MAX ? ROUND8 (size) : R2007_PAGE_MAX;
      sec->uncomp_size = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      sec->comp_size = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      sec->offset = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      sec->size = (size_t *)calloc (sec->num_pages, sizeof (size_t));
      sec->id = (int64_t *)calloc (sec->num_pages, sizeof (int64_t));
      if (!sec->uncomp_size || !sec->comp_size || !sec->offset || !sec->size
          || !sec->id)
        {
          error = DWG_ERR_OUTOFMEM;
          goto done;
        }
      for (k = 0; k < sec->num_pages; k++)
        {
          const size_t pos = (size_t)k * R2007_PAGE_MAX;
          const uint32_t n = (uint32_t)(size - pos < R2007_PAGE_MAX
                                            ? size - pos
                                            : R2007_PAGE_MAX);
          Page *page = &pages[npages++];
          error |= make_data_page (page, &tmp, &sec_dat[type].chain[pos], n,
                                   &sec->comp_size[k]);
          if (error >= DWG_ERR_CRITICAL)
            goto done;
          page->id = next_id++;
          sec->id[k] = page->id;
          sec->offset[k] = pos;
          sec->uncomp_size[k] = n;
          sec->size[k] = page->size;
          LOG_TRACE ("section %s page %d: id %d, %u => %u bytes, %u on disk\n",
                     dwg_section_name (dwg, type), k, (int)page->id, (unsigned)n,
                     (unsigned)sec->comp_size[k], (unsigned)page->size);
        }
    }

  /* the sections map, and its copy */
  secmap = make_sections_map (dwg, secs, num_secs, &secmap_size);
  if (!secmap)
    {
      error = DWG_ERR_OUTOFMEM;
      goto done;
    }
  error |= make_system_page (&sm[0], secmap, secmap_size);
  error |= make_system_page (&sm[1], secmap, secmap_size);
  sm[0].id = next_id++;
  sm[1].id = next_id++;

  /* the pages map, and its copy. Its size depends on the number of pages. */
  {
    const int num_listed = npages + 4; // all pages, with the 4 system pages
    unsigned char *p;
    map_size = 16 * (size_t)num_listed;
    map = (unsigned char *)calloc (1, map_size);
    if (!map)
      {
        error = DWG_ERR_OUTOFMEM;
        goto done;
      }
    pm[0].id = next_id++;
    pm[1].id = next_id++;
    /* a pages map page has a fixed size, independent of its content */
    error |= make_system_page (&pm[0], map, map_size);
    error |= make_system_page (&pm[1], map, map_size);
    if (error >= DWG_ERR_CRITICAL)
      goto done;
    /* the pages in the file order: the pages maps, the data pages, the
       sections maps */
    p = map;
    put_le64 (p, pm[0].size);
    put_le64 (p + 8, (uint64_t)pm[0].id);
    p += 16;
    put_le64 (p, pm[1].size);
    put_le64 (p + 8, (uint64_t)pm[1].id);
    p += 16;
    for (i = 0; i < npages; i++)
      {
        put_le64 (p, pages[i].size);
        put_le64 (p + 8, (uint64_t)pages[i].id);
        p += 16;
      }
    for (i = 0; i < 2; i++)
      {
        put_le64 (p, sm[i].size);
        put_le64 (p + 8, (uint64_t)sm[i].id);
        p += 16;
      }
    free (pm[0].data);
    free (pm[1].data);
    pm[0].data = pm[1].data = NULL;
    error |= make_system_page (&pm[0], map, map_size);
    error |= make_system_page (&pm[1], map, map_size);
    if (error >= DWG_ERR_CRITICAL)
      goto done;
  }

  /* the file size */
  total = R2007_DATA_START + pm[0].size + pm[1].size;
  for (i = 0; i < npages; i++)
    total += pages[i].size;
  total += sm[0].size + sm[1].size;
  off = total - R2007_DATA_START; // the offset of the copy of the header
  total += R2007_HEADER_SIZE;

  /* the file header */
  memset (&hdr, 0, sizeof (hdr));
  hdr.header_size = 0x70;
  hdr.file_size = (int64_t)total;
  hdr.pages_map_correction = R2007_CORRECTION;
  hdr.pages_map2_offset = (int64_t)pm[0].size;
  hdr.pages_map2_id = pm[1].id;
  hdr.pages_map_offset = 0;
  hdr.pages_map_id = pm[0].id;
  hdr.header2_offset = (int64_t)off;
  hdr.pages_map_size_comp = (int64_t)map_size;
  hdr.pages_map_size_uncomp = (int64_t)map_size;
  hdr.pages_amount = npages + 4;
  hdr.pages_maxid = next_id - 1;
  hdr.unknown1 = 0x20;
  hdr.unknown2 = 0x40;
  hdr.unknown3 = 0xf800;
  hdr.unknown4 = 4;
  hdr.unknown5 = 1;
  hdr.num_sections = num_secs;
  hdr.sections_map_size_comp = (int64_t)secmap_size;
  hdr.sections_map2_id = sm[1].id;
  hdr.sections_map_id = sm[0].id;
  hdr.sections_map_size_uncomp = (int64_t)secmap_size;
  hdr.sections_map_correction = R2007_CORRECTION;
  hdr.stream_version = 0x60100;
  memcpy (hbuf, &hdr, sizeof (hdr));
#ifdef WORDS_BIGENDIAN
  for (i = 0; i < (int)(sizeof (hdr) / 8); i++)
    {
      unsigned char *q = &hbuf[8 * i];
      uint64_t v;
      memcpy (&v, q, 8);
      put_le64 (q, v);
    }
#endif
  error |= make_file_header (fhdr, hbuf, &tmp);
  if (error >= DWG_ERR_CRITICAL)
    goto done;

  /* write it all */
  if (dat->size < total)
    bit_chain_alloc_size (dat, total - dat->size + 0x100);
  if (dat->size < total)
    {
      error = DWG_ERR_OUTOFMEM;
      goto done;
    }
  memset (&dat->chain[0x80], 0, total - 0x80);
  memcpy (&dat->chain[0x80], fhdr, R2007_HEADER_SIZE);
  dat->byte = R2007_DATA_START;
  for (i = 0; i < 2; i++)
    {
      memcpy (&dat->chain[dat->byte], pm[i].data, pm[i].size);
      dat->byte += pm[i].size;
    }
  for (i = 0; i < npages; i++)
    {
      memcpy (&dat->chain[dat->byte], pages[i].data, pages[i].size);
      dat->byte += pages[i].size;
    }
  for (i = 0; i < 2; i++)
    {
      memcpy (&dat->chain[dat->byte], sm[i].data, sm[i].size);
      dat->byte += sm[i].size;
    }
  memcpy (&dat->chain[dat->byte], fhdr, R2007_HEADER_SIZE);
  dat->byte += R2007_HEADER_SIZE;
  dat->bit = 0;
  assert (dat->byte == total);
  LOG_TRACE ("r2007 file size: %" PRIuSIZE ", %d pages\n", total, npages + 4);

done:
  bit_chain_free (&tmp);
  free_pages (pages, num_data_pages + 4);
  free_secs (secs, num_secs);
  free (map);
  free (secmap);
  free (pm[0].data);
  free (pm[1].data);
  free (sm[0].data);
  free (sm[1].data);
  return error;
}

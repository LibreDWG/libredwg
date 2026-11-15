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


/* The order of the sections in the file, as ODA writes them. */
static const Dwg_Section_Type section_order[] = {
  SECTION_SUMMARYINFO,    SECTION_PREVIEW,   SECTION_APPINFO,
  SECTION_APPINFOHISTORY, SECTION_REVHISTORY, SECTION_OBJECTS,
  SECTION_OBJFREESPACE,   SECTION_TEMPLATE,  SECTION_HANDLES,
  SECTION_CLASSES,        SECTION_AUXHEADER, SECTION_HEADER,
  SECTION_FILEDEPLIST,    SECTION_SECURITY,  SECTION_VBAPROJECT
};
#define R2007_NUM_ORDER ((int)(sizeof (section_order) / sizeof (section_order[0])))
typedef struct
{
  int64_t id;
  size_t size;           /* the size on disk */
  unsigned char *data;   /* the size bytes on disk, malloced */
  int repeat;            /* system pages: how often the data is repeated */
  uint64_t size_comp, size_uncomp; /* system pages: the data sizes */
  uint64_t crc_comp, crc_uncomp;   /* system pages: the crcs of the data */
} Page;

typedef struct
{
  Dwg_Section_Type type;
  uint64_t data_size;
  uint64_t max_size;
  uint64_t hashcode;
  int encoding; /* 1: raw pages, 4: Reed-Solomon coded pages */
  int num_pages;
  int first_page; /* index into the pages array */
  uint64_t *uncomp_size;
  uint64_t *comp_size;
  uint64_t *offset;
  size_t *size;
  int64_t *id;
  uint64_t *checksum;
  uint64_t *crc;
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

#define R2007_RANDOM_SEED 0x24cc9552adddcbdbULL

/* The hashcode, the encoding and the page size of the known sections. */
static void
section_info (const char *restrict name, int *restrict encoding,
              uint64_t *restrict hashcode, uint64_t *restrict pagesize)
{
  static const struct
  {
    const char *name;
    uint32_t hashcode;
    int encoding;
    uint32_t pagesize;
  } tab[] = {
    { "AcDb:Security", 0x4a0204ea, 1, 0xf800 },
    { "AcDb:FileDepList", 0x6c4205ca, 1, 0x100 },
    { "AcDb:VBAProject", 0x586e0544, 1, 0x80 },
    { "AcDb:AppInfo", 0x3fa0043e, 1, 0x300 },
    { "AcDb:Preview", 0x40aa0473, 1, 0x400 },
    { "AcDb:SummaryInfo", 0x717a060f, 1, 0x80 },
    { "AcDb:RevHistory", 0x60a205b3, 4, 0x1000 },
    { "AcDb:AcDbObjects", 0x674c05a9, 4, 0xf800 },
    { "AcDb:ObjFreeSpace", 0x77e2061f, 4, 0xf800 },
    { "AcDb:Template", 0x4a1404ce, 4, 0x400 },
    { "AcDb:Handles", 0x3f6e0450, 4, 0xf800 },
    { "AcDb:Classes", 0x3f54045f, 4, 0xf800 },
    { "AcDb:AuxHeader", 0x54f0050a, 4, 0x800 },
    { "AcDb:Header", 0x32b803d9, 4, 0x800 },
  };
  size_t i;
  for (i = 0; i < sizeof (tab) / sizeof (tab[0]); i++)
    if (strcmp (name, tab[i].name) == 0)
      {
        *encoding = tab[i].encoding;
        *hashcode = tab[i].hashcode;
        *pagesize = tab[i].pagesize;
        return;
      }
  *encoding = 4;
  *hashcode = 0;
  *pagesize = R2007_PAGE_MAX;
}

/* The 64-bit CRCs, p 5.12. The normal one is the ECMA-182 CRC, the mirrored
   one the same with the bits reflected, with the polynom 0x95ac9329ac4bc9b5. */
typedef struct
{
  uint64_t normal[256];
  uint64_t mirrored[256];
} Crc64;

static void
crc64_init (Crc64 *c)
{
  int i, j;
  for (i = 0; i < 256; i++)
    {
      uint64_t n = (uint64_t)i << 56;
      uint64_t m = (uint64_t)i;
      for (j = 0; j < 8; j++)
        {
          n = (n >> 63) ? (n << 1) ^ 0x42f0e1eba9ea3693ULL : n << 1;
          m = (m & 1) ? (m >> 1) ^ 0x95ac9329ac4bc9b5ULL : m >> 1;
        }
      c->normal[i] = n;
      c->mirrored[i] = m;
    }
}

static uint64_t
crc_seed1 (uint64_t seed, const uint32_t len)
{
  seed = (seed + len) * 0x343fdULL + 0x269ec3ULL;
  seed |= seed * (0x343fdULL << 32) + (0x269ec3ULL << 32);
  return ~seed;
}

static uint64_t
crc_seed2 (uint64_t seed, const uint32_t len)
{
  seed = (seed + len) * 0x343fdULL + 0x269ec3ULL;
  seed = seed * ((1ULL << 32) + 0x343fdULL) + (len + 0x269ec3ULL);
  return ~seed;
}

/* The crc of n bytes, with the start value crc (see crc_seed1/2). The bytes
   are taken in blocks of 8, in the order of the 2 byte words 3,2,1,0 of the
   little endian 64bit value. The rest is taken in the order of p 5.12. */
static uint64_t
crc64_calc (const Crc64 *restrict c, const int normal, uint64_t crc,
            const unsigned char *restrict p, size_t n)
{
  static const unsigned char blk[8] = { 6, 7, 4, 5, 2, 3, 0, 1 };
  static const unsigned char tail[8][7] = {
    { 0 },
    { 0 },
    { 0, 1 },
    { 0, 1, 2 },
    { 2, 3, 0, 1 },
    { 2, 3, 0, 1, 4 },
    { 2, 3, 0, 1, 4, 5 },
    { 2, 3, 0, 1, 4, 5, 6 },
  };
  size_t i, rest = n & 7;
  int j;
#define CRC64_STEP(b)                                                         \
  crc = normal ? c->normal[((b) ^ (crc >> 56)) & 0xff] ^ (crc << 8)           \
               : c->mirrored[(crc ^ (b)) & 0xff] ^ (crc >> 8)
  for (i = 0; i + 8 <= n; i += 8)
    for (j = 0; j < 8; j++)
      CRC64_STEP (p[i + blk[j]]);
  for (j = 0; j < (int)rest; j++)
    CRC64_STEP (p[i + tail[rest][j]]);
#undef CRC64_STEP
  return normal ? ~crc : crc;
}

/* The 32-bit checksum of a data page, p 5.4.1. */
static uint32_t
page_checksum (const uint64_t seed0, const unsigned char *restrict p,
               size_t length)
{
  uint64_t seed = (seed0 + length) * 0x343fdULL + 0x269ec3ULL;
  uint32_t sum1 = (uint32_t)(seed & 0xffff);
  uint32_t sum2 = (uint32_t)((seed >> 16) & 0xffff);
  static const unsigned char order[8][7] = {
    { 0 },
    { 0 },
    { 0, 1 },
    { 0, 1, 2 },
    { 2, 3, 0, 1 },
    { 2, 3, 0, 1, 4 },
    { 2, 3, 0, 1, 4, 5 },
    { 2, 3, 0, 1, 4, 5, 6 },
  };
  while (length)
    {
      size_t big = length < 0x15b0 ? length : 0x15b0;
      size_t i;
      length -= big;
      for (i = 0; i + 8 <= big; i += 8)
        {
          int j;
          /* the 2 byte words 3,2,1,0, each byte in order */
          for (j = 3; j >= 0; j--)
            {
              sum1 += p[2 * j];
              sum2 += sum1;
              sum1 += p[2 * j + 1];
              sum2 += sum1;
            }
          p += 8;
        }
      {
        const size_t rest = big & 7;
        size_t j;
        /* the rest: words 1,0,2,3 for 4-7 bytes, 1 byte words last */
        for (j = 0; j < rest; j++)
          {
            sum1 += p[order[rest][j]];
            sum2 += sum1;
          }
        p += rest;
      }
      sum1 %= 0xfff1;
      sum2 %= 0xfff1;
    }
  return (sum2 << 16) | (sum1 & 0xffff);
}

/* The rotation of value by control & 0x1f bits. p 5.2.1.1.5 */
static uint64_t
crc_shift (const uint64_t value, const uint64_t control)
{
  const int shift = (int)(control & 0x1f);
  return shift ? (value << shift) | (value >> (64 - shift)) : value;
}

/* The pseudo random encoder, p 5.11. */
typedef struct
{
  uint32_t table[0x270];
  unsigned index;
} Crc_Rng;

static void
rng_init (Crc_Rng *r, const uint64_t seed)
{
  uint32_t value;
  unsigned i;
  r->table[0] = (uint32_t)seed * 0x343fdU + 0x269ec3U;
  r->table[1] = (uint32_t)(seed >> 32) * 0x343fdU + 0x269ec3U;
  value = r->table[1];
  for (i = 2; i < 0x270; i++)
    {
      value = ((value >> 0x1e) ^ value) * 0x6c078965U + i;
      r->table[i] = value;
    }
  r->index = 0x80; /* after the padding table */
}

static uint64_t
rng_next (Crc_Rng *r)
{
  r->index += 2;
  if (r->index >= 0x270)
    r->index = 0;
  return (uint64_t)r->table[r->index]
         | ((uint64_t)r->table[r->index + 1] << 32);
}

/* Encodes the 10 low bits of value into a random 64-bit value. */
static uint64_t
rng_encode (Crc_Rng *r, const uint32_t value)
{
  const uint64_t random = rng_next (r);
  uint32_t lo = (uint32_t)(random & 0x0df7df7dfU);
  uint32_t hi = (uint32_t)((random >> 32) & 0x0f7df7df7U);
  if (value & 0x200) lo |= 0x20;
  if (value & 0x100) lo |= 0x800;
  if (value & 0x80) lo |= 0x20000;
  if (value & 0x40) lo |= 0x800000;
  if (value & 0x20) lo |= 0x20000000;
  if (value & 0x10) hi |= 0x08;
  if (value & 0x8) hi |= 0x200;
  if (value & 0x4) hi |= 0x8000;
  if (value & 0x2) hi |= 0x200000;
  if (value & 0x1) hi |= 0x8000000;
  return lo | ((uint64_t)hi << 32);
}

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

/* The R2007 compression, a LZ77 variant. The inverse of decompress_r2007.
   The stream consists of a first literal run, followed by matches, each
   with an optional literal run. The literal bytes are permuted. The
   instructions have 3 forms, depending on the length and the distance. */
typedef struct
{
  unsigned char type; /* 0 straight, 1 reversed bytes, 2 16 byte halves */
  unsigned char size;
  unsigned char off;
} Lit_Piece;
#define PS(sz, off) { 0, sz, off }
#define PR(sz, off) { 1, sz, off }
#define PH(off) { 2, 16, off }
/* How a literal run of 1-31 bytes is permuted, as copy_compressed_bytes. */
static const Lit_Piece lit_pieces[32][6] = {
  { { 0, 0, 0 } },
  { PS (1, 0) },
  { PR (2, 0) },
  { PR (3, 0) },
  { PS (4, 0) },
  { PS (1, 4), PS (4, 0) },
  { PS (1, 5), PS (4, 1), PS (1, 0) },
  { PR (2, 5), PS (4, 1), PS (1, 0) },
  { PS (8, 0) },
  { PS (1, 8), PS (8, 0) },
  { PS (1, 9), PS (8, 1), PS (1, 0) },
  { PR (2, 9), PS (8, 1), PS (1, 0) },
  { PS (4, 8), PS (8, 0) },
  { PS (1, 12), PS (4, 8), PS (8, 0) },
  { PS (1, 13), PS (4, 9), PS (8, 1), PS (1, 0) },
  { PR (2, 13), PS (4, 9), PS (8, 1), PS (1, 0) },
  { PH (0) },
  { PS (8, 9), PS (1, 8), PS (8, 0) },
  { PS (1, 17), PH (1), PS (1, 0) },
  { PR (3, 16), PH (0) },
  { PS (4, 16), PH (0) },
  { PS (1, 20), PS (4, 16), PH (0) },
  { PR (2, 20), PS (4, 16), PH (0) },
  { PR (3, 20), PS (4, 16), PH (0) },
  { PS (8, 16), PH (0) },
  { PS (8, 17), PS (1, 16), PH (0) },
  { PS (1, 25), PS (8, 17), PS (1, 16), PH (0) },
  { PR (2, 25), PS (8, 17), PS (1, 16), PH (0) },
  { PS (4, 24), PS (8, 16), PH (0) },
  { PS (1, 28), PS (4, 24), PS (8, 16), PH (0) },
  { PR (2, 28), PS (4, 24), PS (8, 16), PH (0) },
  { PS (1, 30), PS (4, 26), PS (8, 18), PH (2), PR (2, 0) },
};

/* Writes the n literal bytes permuted, so that decompress_r2007 gets them
   back. All the copies are involutions: the same, with the sides swapped. */
static void
put_literal (unsigned char *restrict out, const unsigned char *restrict in,
             const size_t n)
{
  size_t d = 0, pos = 0, rest;
  const Lit_Piece *p;

  while (n - d >= 32)
    {
      memcpy (&out[pos + 24], &in[d], 8);
      memcpy (&out[pos + 16], &in[d + 8], 8);
      memcpy (&out[pos + 8], &in[d + 16], 8);
      memcpy (&out[pos], &in[d + 24], 8);
      d += 32;
      pos += 32;
    }
  rest = n - d;
  for (p = lit_pieces[rest]; p->size; p++)
    {
      unsigned t;
      for (t = 0; t < p->size; t++)
        {
          const unsigned so = p->type == 0   ? p->off + t
                              : p->type == 1 ? p->off + p->size - 1 - t
                              : t < 8        ? p->off + 8 + t
                                             : p->off + t - 8;
          out[pos + so] = in[d + t];
        }
      d += p->size;
    }
}

/* The length of a literal run of k >= 8 bytes. Returns the bytes written. */
static size_t
put_literal_length (unsigned char *out, const size_t k)
{
  size_t p = 0, r;
  if (k <= 0x16)
    {
      out[p++] = (unsigned char)(k - 8);
      return p;
    }
  out[p++] = 0x0f;
  r = k - 0x17;
  if (r < 0xff)
    {
      out[p++] = (unsigned char)r;
      return p;
    }
  out[p++] = 0xff;
  r -= 0xff;
  while (r >= 0xffff)
    {
      out[p++] = 0xff;
      out[p++] = 0xff;
      r -= 0xffff;
    }
  out[p++] = (unsigned char)(r & 0xff);
  out[p++] = (unsigned char)(r >> 8);
  return p;
}

/* The size of the instruction for a match of length l at distance d,
   or 0 if it does not pay off. */
static unsigned
match_cost (const size_t l, const size_t d)
{
  if (l >= 3 && l <= 14 && d <= 512)
    return 2;
  if (l >= 4 && l <= 18 && d <= 8192)
    return 3;
  if (l >= 5 && l <= 255 && d <= 0xffff)
    return 4;
  return 0;
}

/* A match of length l at distance d. k = 0-7 literal bytes follow. */
static size_t
put_match (unsigned char *out, const size_t l, const size_t d,
           const unsigned k)
{
  const unsigned cost = match_cost (l, d);
  if (cost == 2)
    {
      out[0] = (unsigned char)((l << 4) | ((d - 1) & 15));
      out[1] = (unsigned char)((((d - 1) >> 4) << 3) | k);
    }
  else if (cost == 3)
    {
      out[0] = (unsigned char)(0x10 | (l - 3));
      out[1] = (unsigned char)((d - 1) & 0xff);
      out[2] = (unsigned char)((((d - 1) >> 8) << 3) | k);
    }
  else
    {
      out[0] = (unsigned char)(0x20 | (l & 7));
      out[1] = (unsigned char)(d & 0xff);
      out[2] = (unsigned char)(d >> 8);
      out[3] = (unsigned char)((l & 0xf8) | k);
    }
  return cost;
}

#define LZ_NONE 0xffffffffU
#define LZ_HASH(p) ((((p)[0] << 8) ^ ((p)[1] << 4) ^ (p)[2]) & 0x7fff)

/* Compresses n bytes of src into out, which has room for n + n/8 + 64 bytes.
   *outlen is 0 if the result is not smaller than the input. */
int
compress_r2007 (unsigned char *restrict out, const unsigned char *restrict src,
                const size_t n, size_t *restrict outlen)
{
  typedef struct
  {
    uint32_t lit, len, dist;
  } Token;
  uint32_t *head, *prev;
  Token *tok;
  size_t ntok = 0, i = 0, lit_start = 0, pos = 0, j;

  *outlen = 0;
  if (n < 16)
    return 0;
  head = (uint32_t *)malloc (0x8000 * sizeof (uint32_t));
  prev = (uint32_t *)malloc (n * sizeof (uint32_t));
  tok = (Token *)malloc ((n / 3 + 2) * sizeof (Token));
  if (!head || !prev || !tok)
    {
      free (head);
      free (prev);
      free (tok);
      return DWG_ERR_OUTOFMEM;
    }
  memset (head, 0xff, 0x8000 * sizeof (uint32_t));

#define LZ_INSERT(p)                                                          \
  if ((p) + 3 <= n)                                                           \
    {                                                                         \
      const unsigned h = LZ_HASH (&src[p]);                                   \
      prev[p] = head[h];                                                      \
      head[h] = (uint32_t)(p);                                                \
    }
  while (i < n)
    {
      size_t best = 0, bestd = 0;
      /* the first literal run needs 8 bytes at least */
      if (i >= 8 && i + 3 <= n)
        {
          uint32_t cand = head[LZ_HASH (&src[i])];
          int depth = 0;
          const size_t max = n - i < 255 ? n - i : 255;
          while (cand != LZ_NONE && depth++ < 48 && i - cand <= 0xffff)
            {
              size_t l = 0;
              while (l < max && src[cand + l] == src[i + l])
                l++;
              if (l > best)
                {
                  best = l;
                  bestd = i - cand;
                }
              cand = prev[cand];
            }
        }
      if (best >= 3 && match_cost (best, bestd) && match_cost (best, bestd) < best)
        {
          tok[ntok].lit = (uint32_t)(i - lit_start);
          tok[ntok].len = (uint32_t)best;
          tok[ntok].dist = (uint32_t)bestd;
          ntok++;
          for (j = 0; j < best; j++)
            {
              LZ_INSERT (i + j);
            }
          i += best;
          lit_start = i;
        }
      else
        {
          LZ_INSERT (i);
          i++;
        }
    }
#undef LZ_INSERT
  /* emit */
  {
    const size_t first = ntok ? tok[0].lit : n;
    size_t src_pos = first;
    if (first < 8)
      goto done;
    pos += put_literal_length (&out[pos], first);
    put_literal (&out[pos], src, first);
    pos += first;
    for (j = 0; j < ntok; j++)
      {
        const size_t k = j + 1 < ntok ? tok[j + 1].lit : n - lit_start;
        pos += put_match (&out[pos], tok[j].len, tok[j].dist,
                          k < 8 ? (unsigned)k : 0);
        src_pos += tok[j].len;
        if (k >= 8)
          pos += put_literal_length (&out[pos], k);
        if (k)
          {
            put_literal (&out[pos], &src[src_pos], k);
            pos += k;
            src_pos += k;
          }
      }
    if (pos < n)
      *outlen = pos;
  }
done:
  free (head);
  free (prev);
  free (tok);
  return 0;
}

/* A page of the file: the payload, compressed if that is smaller,
   Reed-Solomon coded with 251 data bytes (encoding 4), or raw (encoding 1),
   and padded to 32. Fills the sizes, the 32-bit checksum of the data and the
   64-bit crc of the stored data. */
static int
make_data_page (Page *restrict page, const Crc64 *restrict crc,
                const BITCODE_RC *restrict src, const uint32_t size_uncomp,
                const int encoding, uint64_t *restrict size_comp,
                uint64_t *restrict checksum, uint64_t *restrict crc64)
{
  size_t block_count, clen = 0;
  const unsigned char *payload = src;
  unsigned char *rs, *cbuf = NULL;

  *checksum = page_checksum (0, src, size_uncomp);
  if (encoding == 1)
    {
      *size_comp = size_uncomp;
      *crc64 = crc64_calc (crc, 0, crc_seed1 (0, size_uncomp), src, size_uncomp);
      page->size = ROUND32 (size_uncomp ? size_uncomp : 1);
      page->data = (unsigned char *)calloc (1, page->size);
      if (!page->data)
        return DWG_ERR_OUTOFMEM;
      memcpy (page->data, src, size_uncomp);
      return 0;
    }
  *size_comp = size_uncomp;
  cbuf = (unsigned char *)malloc (size_uncomp + size_uncomp / 8 + 64);
  if (!cbuf)
    return DWG_ERR_OUTOFMEM;
  if (!compress_r2007 (cbuf, src, size_uncomp, &clen) && clen)
    {
      payload = cbuf;
      *size_comp = clen;
    }
  *crc64 = crc64_calc (crc, 0, crc_seed1 (0, (uint32_t)*size_comp), payload,
                       *size_comp);
  rs = rs_encode_page (payload, *size_comp, 1, 251, &block_count);
  free (cbuf);
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

/* A system page: compressed if that is smaller, Reed-Solomon coded with 239
   data bytes. The page has the size of p 5.3.1, from the uncompressed size,
   the data is repeated as often as it fits. Fills the sizes and the crcs. */
static int
make_system_page (Page *restrict page, const Crc64 *restrict crc,
                  const unsigned char *src, const size_t size)
{
  size_t pagesize = ((ROUND8 (size) * 2) + 238) / 239 * 255;
  size_t block_count, plen = size, clen = 0, aligned;
  const unsigned char *payload = src;
  unsigned char *cbuf, *rs;

  cbuf = (unsigned char *)malloc (size + size / 8 + 64);
  if (!cbuf)
    return DWG_ERR_OUTOFMEM;
  if (!compress_r2007 (cbuf, src, size, &clen) && clen)
    {
      payload = cbuf;
      plen = clen;
    }
  page->size_comp = plen;
  page->size_uncomp = size;
  page->crc_uncomp = crc64_calc (crc, 0, crc_seed1 (0, (uint32_t)size), src, size);
  page->crc_comp = crc64_calc (crc, 0, crc_seed1 (0, (uint32_t)plen), payload, plen);
  pagesize = pagesize < 0x400 ? 0x400 : ROUND32 (pagesize);
  aligned = ROUND8 (plen);
  page->repeat = (int)((pagesize / 255 * 239) / aligned);
  rs = rs_encode_page (payload, plen, page->repeat, 239, &block_count);
  free (cbuf);
  if (!rs)
    return DWG_ERR_OUTOFMEM;
  page->size = pagesize;
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

/* The 0x400 bytes of the file header: the Dwg_R2007_Header, compressed if that
   is smaller, with the checking sequence, in 3 Reed-Solomon blocks, and the
   check data. p 5.2.1.4 - 5.2.1.7 */
static int
make_file_header (unsigned char *restrict out,
                  const unsigned char *restrict hbuf, const Crc64 *restrict crc,
                  const uint64_t key, const uint64_t r1, const uint64_t r2,
                  const uint64_t chk_seed)
{
  unsigned char pe[3 * 239];
  unsigned char seq[16], buf[64];
  unsigned char cbuf[sizeof (Dwg_R2007_Header) + 64 + sizeof (Dwg_R2007_Header) / 8];
  const unsigned char *data = hbuf;
  int32_t len = (int32_t)sizeof (Dwg_R2007_Header);
  size_t clen = 0, unit;
  uint64_t b[8], normal, mirrored;
  size_t i, j;

  memset (pe, 0, sizeof (pe));
  memset (out, 0, R2007_HEADER_SIZE);
  if (!compress_r2007 (cbuf, hbuf, sizeof (Dwg_R2007_Header), &clen) && clen)
    {
      data = cbuf;
      len = (int32_t)clen;
    }
  else
    len = -len; /* a negative length: not compressed */
  unit = ROUND8 (32 + (size_t)abs (len));
  put_le64 (&seq[0], key);
  put_le64 (&seq[8], crc_shift (key, key));
  put_le64 (&pe[0], crc64_calc (crc, 1, crc_seed1 (0, 16), seq, 16));
  put_le64 (&pe[8], key);
  put_le64 (&pe[16],
            crc64_calc (crc, 1, crc_seed2 (0, (uint32_t)abs (len)), data,
                        (size_t)abs (len)));
  put_le32 (&pe[24], (uint32_t)len);
  put_le32 (&pe[28], 0);
  memcpy (&pe[32], data, (size_t)abs (len));
  /* repeat this block as often as it fits */
  for (i = 1; (i + 1) * unit <= sizeof (pe); i++)
    memcpy (&pe[i * unit], pe, unit);
  /* 3 blocks of 239 consecutive bytes, interleaved */
  for (i = 0; i < 3; i++)
    {
      unsigned char blk[255];
      memcpy (blk, &pe[i * 239], 239);
      rs_encode_block (&blk[239], blk, 239);
      for (j = 0; j < 255; j++)
        out[i + 3 * j] = blk[j];
    }
  /* the check data at 0x3d8: normal crc, mirrored crc, r1, r2, encoded seed */
  b[0] = crc_shift (r1, r2);
  b[1] = crc_shift (b[0], b[0]);
  b[2] = crc_shift (r2, b[1]);
  b[3] = crc_shift (b[2], b[2]);
  b[4] = crc_shift (r1, b[3]);
  b[5] = crc_shift (b[4], b[4]);
  b[6] = crc_shift (b[5], b[5]);
  b[7] = crc_shift (b[6], b[6]);
  for (i = 0; i < 8; i++)
    put_le64 (&buf[8 * i], b[i]);
  normal = crc64_calc (crc, 1, ~r2, buf, 64);
  b[1] = crc_shift (normal, b[0]);
  b[3] = crc_shift (normal, b[2]);
  b[5] = crc_shift (normal, b[4]);
  b[6] = crc_shift (r2, b[5]);
  b[7] = crc_shift (b[6], b[6]);
  for (i = 0; i < 8; i++)
    put_le64 (&buf[8 * i], b[i]);
  mirrored = crc64_calc (crc, 0, ~r1, buf, 64);
  put_le64 (&out[0x3d8], normal);
  put_le64 (&out[0x3e0], mirrored);
  put_le64 (&out[0x3e8], r1);
  put_le64 (&out[0x3f0], r2);
  put_le64 (&out[0x3f8], chk_seed);
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
      free (secs[i].checksum);
      free (secs[i].crc);
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
      total += 64 + 2 * strlen (name) + 2 + 56 * (size_t)secs[i].num_pages;
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
      put_le64 (&buf[pos + 24], secs[i].hashcode);
      put_le64 (&buf[pos + 32], 2 * len + 2);
      put_le64 (&buf[pos + 40], 0);
      put_le64 (&buf[pos + 48], (uint64_t)secs[i].encoding);
      put_le64 (&buf[pos + 56], (uint64_t)secs[i].num_pages);
      pos += 64;
      for (c = 0; c < len; c++)
        {
          buf[pos + 2 * c] = (unsigned char)name[c];
          buf[pos + 2 * c + 1] = 0;
        }
      pos += 2 * len + 2; /* with the zero terminator */
      for (k = 0; k < secs[i].num_pages; k++)
        {
          put_le64 (&buf[pos], secs[i].offset[k]);
          put_le64 (&buf[pos + 8], ROUND8 (secs[i].uncomp_size[k]));
          put_le64 (&buf[pos + 16], (uint64_t)secs[i].id[k]);
          put_le64 (&buf[pos + 24], secs[i].uncomp_size[k]);
          put_le64 (&buf[pos + 32], secs[i].comp_size[k]);
          put_le64 (&buf[pos + 40], secs[i].checksum[k]);
          put_le64 (&buf[pos + 48], secs[i].crc[k]);
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
  int type, i, k, oi;
  int64_t next_id = R2007_FIRST_ID;
  unsigned char *map = NULL, *secmap = NULL;
  size_t map_size = 0, secmap_size = 0, total = 0, off;
  Page pm[2] = { { 0, 0, NULL }, { 0, 0, NULL } };
  Page sm[2] = { { 0, 0, NULL }, { 0, 0, NULL } };
  Bit_Chain tmp = { 0 };
  unsigned char fhdr[R2007_HEADER_SIZE];
  unsigned char hbuf[sizeof (Dwg_R2007_Header)];
  Crc64 crc;
  Crc_Rng rng;
  uint64_t r1, r2, chk_seed, key;

  LOG_TRACE ("\n#### r2007 File ####\n");
  bit_chain_init_dat (&tmp, 0x10000, dat);
  crc64_init (&crc);

  /* count the sections and their pages */
  for (oi = 0; oi < R2007_NUM_ORDER; oi++)
    {
      type = section_order[oi];
      if (sec_dat[type].chain && sec_dat[type].byte)
        {
          num_secs++;
          num_data_pages += (int)((sec_dat[type].byte + R2007_PAGE_MAX - 1)
                                  / R2007_PAGE_MAX);
        }
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
  for (oi = 0; oi < R2007_NUM_ORDER; oi++)
    {
      const int t = section_order[oi];
      const size_t size = sec_dat[t].byte;
      Sec *sec;
      type = t;
      if (!sec_dat[type].chain || !size)
        continue;
      sec = &secs[i++];
      sec->type = (Dwg_Section_Type)type;
      sec->data_size = size;
      sec->num_pages = (int)((size + R2007_PAGE_MAX - 1) / R2007_PAGE_MAX);
      sec->first_page = npages;
      section_info (dwg_section_name (dwg, type), &sec->encoding,
                    &sec->hashcode, &sec->max_size);
      sec->uncomp_size = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      sec->comp_size = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      sec->offset = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      sec->size = (size_t *)calloc (sec->num_pages, sizeof (size_t));
      sec->id = (int64_t *)calloc (sec->num_pages, sizeof (int64_t));
      sec->checksum = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      sec->crc = (uint64_t *)calloc (sec->num_pages, sizeof (uint64_t));
      if (!sec->uncomp_size || !sec->comp_size || !sec->offset || !sec->size
          || !sec->id || !sec->checksum || !sec->crc)
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
          error |= make_data_page (page, &crc, &sec_dat[type].chain[pos], n,
                                   sec->encoding, &sec->comp_size[k],
                                   &sec->checksum[k], &sec->crc[k]);
          if (error >= DWG_ERR_CRITICAL)
            goto done;
          page->id = next_id++;
          sec->id[k] = page->id;
          sec->offset[k] = pos;
          sec->uncomp_size[k] = n;
          sec->size[k] = page->size;
          if (ROUND8 (n) > sec->max_size)
            sec->max_size = ROUND8 (n);
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
  error |= make_system_page (&sm[0], &crc, secmap, secmap_size);
  error |= make_system_page (&sm[1], &crc, secmap, secmap_size);
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
    error |= make_system_page (&pm[0], &crc, map, map_size);
    error |= make_system_page (&pm[1], &crc, map, map_size);
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
    error |= make_system_page (&pm[0], &crc, map, map_size);
    error |= make_system_page (&pm[1], &crc, map, map_size);
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

  /* the check values of the file header, p 5.2.1.1 */
  rng_init (&rng, R2007_RANDOM_SEED);
  r1 = rng_next (&rng);
  r2 = rng_next (&rng);
  chk_seed = rng_encode (&rng, 0);
  key = rng_next (&rng);

  /* the file header */
  memset (&hdr, 0, sizeof (hdr));
  hdr.header_size = 0x70;
  hdr.file_size = (int64_t)total;
  hdr.pages_map_correction = pm[0].repeat;
  hdr.pages_map2_offset = (int64_t)pm[0].size;
  hdr.pages_map2_id = pm[1].id;
  hdr.pages_map_offset = 0;
  hdr.pages_map_id = pm[0].id;
  hdr.header2_offset = (int64_t)off;
  hdr.pages_map_size_comp = (int64_t)pm[0].size_comp;
  hdr.pages_map_size_uncomp = (int64_t)pm[0].size_uncomp;
  hdr.pages_amount = npages + 4;
  hdr.pages_maxid = next_id - 1;
  hdr.unknown1 = 0x20;
  hdr.unknown2 = 0x40;
  hdr.unknown3 = 0xf800;
  hdr.unknown4 = 4;
  hdr.unknown5 = 1;
  hdr.num_sections = num_secs + 1;
  hdr.sections_map_size_comp = (int64_t)sm[0].size_comp;
  hdr.sections_map2_id = sm[1].id;
  hdr.sections_map_id = sm[0].id;
  hdr.sections_map_size_uncomp = (int64_t)sm[0].size_uncomp;
  hdr.sections_map_correction = sm[0].repeat;
  hdr.stream_version = 0x60100;
  hdr.pages_map_crc_compressed = (int64_t)pm[0].crc_comp;
  hdr.pages_map_crc_uncomp = (int64_t)pm[0].crc_uncomp;
  hdr.sections_map_crc_comp = (int64_t)sm[0].crc_comp;
  hdr.sections_map_crc_uncomp = (int64_t)sm[0].crc_uncomp;
  hdr.pages_map_crc_seed = (int64_t)rng_encode (&rng, 0);
  hdr.sections_map_crc_seed = (int64_t)rng_encode (&rng, 0);
  hdr.crc_seed = 0;
  hdr.crc_seed_encoded = (int64_t)rng_encode (&rng, 0);
  hdr.random_seed = (int64_t)R2007_RANDOM_SEED;
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
  /* the CRC of the header with the CRC field being 0 */
  put_le64 (&hbuf[0x108], 0);
  put_le64 (&hbuf[0x108],
            crc64_calc (&crc, 1, crc_seed2 (0, sizeof (hdr)), hbuf,
                        sizeof (hdr)));
  error |= make_file_header (fhdr, hbuf, &crc, key, r1, r2, chk_seed);
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
  /* the addresses of the pages in the fixed header */
  for (i = 0; i < num_secs; i++)
    {
      size_t a = R2007_DATA_START + pm[0].size + pm[1].size;
      for (k = 0; k < secs[i].first_page; k++)
        a += pages[k].size;
      if (secs[i].type == SECTION_PREVIEW)
        put_le32 (&dat->chain[0x0D], (uint32_t)a);
      else if (secs[i].type == SECTION_SUMMARYINFO)
        put_le32 (&dat->chain[0x20], (uint32_t)a);
      else if (secs[i].type == SECTION_APPINFO)
        put_le32 (&dat->chain[0x2C], (uint32_t)a);
    }
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

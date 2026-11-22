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

/* The r2007 container as encode_r2007.c writes it, as AutoCAD writes it:
   the section order, the page padding, the raw pages and the sections map.
   The static functions are tested directly, and the pages of whole files,
   AutoCAD's and the encoder's, are walked with decode_r2007.c's readers. */

#define ENCODE_R2007_TEST_C
#include <stdlib.h>
#include "encode_r2007.c"
#undef DWG_LOGLEVEL /* decode_r2007.c defines it again */
#include "decode_r2007.c"
#include "common.c"
#include "tests_common.h"

static unsigned int test_loglevel;
/* from dwg_api.h, which clashes with the included sources */
EXPORT int dwg_add_Document (Dwg_Data *restrict dwg, const int imperial);

static uint64_t
get_le64 (const unsigned char *p)
{
  uint64_t v = 0;
  int i;
  for (i = 7; i >= 0; i--)
    v = (v << 8) | p[i];
  return v;
}

/* AutoCAD's order of the sections in the file (ODA writes SummaryInfo
   first and FileDepList last). */
static void
section_order_test (void)
{
  static const Dwg_Section_Type acad[]
      = { SECTION_FILEDEPLIST, SECTION_APPINFOHISTORY, SECTION_APPINFO,
          SECTION_PREVIEW,     SECTION_SUMMARYINFO,    SECTION_REVHISTORY,
          SECTION_OBJECTS,     SECTION_OBJFREESPACE,   SECTION_TEMPLATE,
          SECTION_HANDLES,     SECTION_CLASSES,        SECTION_AUXHEADER,
          SECTION_HEADER,      SECTION_SECURITY,       SECTION_VBAPROJECT };
  if (R2007_NUM_ORDER == (int)(sizeof (acad) / sizeof (acad[0]))
      && !memcmp (section_order, acad, sizeof (acad)))
    ok ("r2007 section order as AutoCAD writes it");
  else
    fail ("r2007 section order");
}

/* The padding: the first 0x80 words of the random encoding table (p 5.11),
   little endian, from the given offset, repeated every 512 bytes, as in
   AutoCAD's files. */
static void
pad_fill_test (void)
{
  Crc_Rng rng;
  unsigned char got[3 * 0x80 * 4 + 20];
  size_t i, bad = 0;
  const size_t off = 3;
  rng_init (&rng, R2007_RANDOM_SEED);
  pad_fill (got, sizeof (got), off);
  for (i = 0; i < sizeof (got); i++)
    {
      const size_t k = (off + i) % (0x80 * 4);
      const unsigned char want
          = (unsigned char)(rng.table[k / 4] >> (8 * (k % 4)));
      if (got[i] != want)
        bad++;
    }
  if (bad)
    fail ("pad_fill: %u bytes differ from the random table", (unsigned)bad);
  else
    ok ("pad_fill: the random table, little endian, every 512 bytes");
}

/* A raw page (encoding 1) as AutoCAD writes it: the data padded to 8, then
   the 4 Reed-Solomon parity bytes of each block of 251 data bytes (the last
   block completed with the padding table), then padding to 32. */
static void
raw_page_test (void)
{
  static const uint32_t sizes[] = { 1, 126, 251, 252, 1240 };
  unsigned char src[1240];
  Crc64 crc;
  size_t s, i;
  int bad = 0;
  crc64_init (&crc);
  for (i = 0; i < sizeof (src); i++)
    src[i] = (unsigned char)(i * 7 + 3);
  for (s = 0; s < sizeof (sizes) / sizeof (sizes[0]); s++)
    {
      const uint32_t n = sizes[s];
      const size_t n8 = ROUND8 (n), bc = (n8 + 250) / 251;
      Page page = { 0 };
      uint64_t size_comp, checksum, crc64;
      unsigned char pad[32];
      if (make_data_page (&page, &crc, src, n, 1, &size_comp, &checksum,
                          &crc64)
          || !page.data)
        {
          fail ("raw page of %u bytes: not made", (unsigned)n);
          bad++;
          continue;
        }
      if (page.size != ROUND32 (n8 + 4 * bc) || size_comp != n
          || memcmp (page.data, src, n))
        {
          fail ("raw page of %u bytes: size %u", (unsigned)n,
                (unsigned)page.size);
          bad++;
        }
      for (i = n; i < n8; i++)
        if (page.data[i])
          {
            fail ("raw page of %u bytes: data not padded with 0 to 8",
                  (unsigned)n);
            bad++;
            break;
          }
      for (i = 0; i < bc; i++)
        {
          unsigned char blk[255];
          const size_t have = n8 - i * 251 < 251 ? n8 - i * 251 : 251;
          memcpy (blk, &page.data[i * 251], have);
          if (have < 251)
            pad_fill (&blk[have], 251 - have, 0);
          memcpy (&blk[251], &page.data[n8 + 4 * i], 4);
          if (rs_decode_data_block (blk, 0) != 0)
            {
              fail ("raw page of %u bytes: block %u parity", (unsigned)n,
                    (unsigned)i);
              bad++;
            }
        }
      if (page.size > n8 + 4 * bc)
        {
          const size_t fill = page.size - (n8 + 4 * bc);
          pad_fill (pad, fill, 0);
          if (memcmp (&page.data[n8 + 4 * bc], pad, fill))
            {
              fail ("raw page of %u bytes: not padded with the random table",
                    (unsigned)n);
              bad++;
            }
        }
      free (page.data);
    }
  if (!bad)
    ok ("raw pages: data, Reed-Solomon parity, random table padding");
}

/* The size of a raw page, from its section's max_size, as in AutoCAD's
   r2007 files: AutoCAD LT 2027's small ones and the large previews of
   2007/Line.dwg and example_2007.dwg. */
static void
raw_page_size_test (void)
{
  static const size_t sizes[][2]
      = { { 128, 160 },     { 768, 800 },     { 1024, 1056 },
          { 1280, 1312 },   { 2048, 2112 },   { 0x6c00, 28096 },
          { 303104, 307936 } };
  size_t i, bad = 0;
  for (i = 0; i < sizeof (sizes) / sizeof (sizes[0]); i++)
    if (raw_page_size (sizes[i][0]) != sizes[i][1])
      {
        fail ("raw_page_size (%u): %u, not %u", (unsigned)sizes[i][0],
              (unsigned)raw_page_size (sizes[i][0]), (unsigned)sizes[i][1]);
        bad++;
      }
  if (!bad)
    ok ("raw_page_size: the Reed-Solomon coded size of max_size");
}

/* The sections map: the page sizes are allocation sizes (at least the
   section's max size), and the map ends with an empty section without a
   name: max size 0xf800, encoding 4, no pages. */
static void
sections_map_test (void)
{
  Dwg_Data dwg = { 0 };
  Sec secs[2] = { 0 };
  uint64_t uncomp[2] = { 1240, 100 }, comp[2] = { 1240, 60 };
  uint64_t offset[2] = { 0x480, 0x980 }, checksum[2] = { 1, 2 },
           crcs[2] = { 3, 4 };
  int64_t id[2] = { 3, 4 };
  size_t sizes[2] = { 1312, 1024 }, size = 0, pos = 0;
  unsigned char *map;
  int i, bad = 0;

  dwg.header.version = dwg.header.from_version = R_2007;
  secs[0].type = SECTION_APPINFOHISTORY;
  secs[0].max_size = 1280;
  secs[0].encoding = 1;
  secs[1].type = SECTION_OBJECTS;
  secs[1].max_size = 0xF800;
  secs[1].encoding = 4;
  for (i = 0; i < 2; i++)
    {
      secs[i].data_size = uncomp[i];
      secs[i].num_pages = 1;
      secs[i].uncomp_size = &uncomp[i];
      secs[i].comp_size = &comp[i];
      secs[i].offset = &offset[i];
      secs[i].size = &sizes[i];
      secs[i].id = &id[i];
      secs[i].checksum = &checksum[i];
      secs[i].crc = &crcs[i];
    }
  map = make_sections_map (&dwg, secs, 2, &size);
  if (!map)
    {
      fail ("sections map: not made");
      return;
    }
  for (i = 0; i < 2 && pos + 64 <= size; i++)
    {
      const uint64_t namelen = get_le64 (&map[pos + 32]);
      const uint64_t want = i ? 0xF800 : 1280; /* the allocation size */
      pos += 64 + namelen;
      if (pos + 56 > size || get_le64 (&map[pos + 8]) != want)
        {
          fail ("sections map: page size of %s",
                dwg_section_name (&dwg, secs[i].type));
          bad++;
        }
      pos += 56;
    }
  if (pos + 64 != size || get_le64 (&map[pos]) != 0
      || get_le64 (&map[pos + 8]) != 0xF800 || get_le64 (&map[pos + 32]) != 0
      || get_le64 (&map[pos + 48]) != 4 || get_le64 (&map[pos + 56]) != 0)
    {
      fail ("sections map: no empty section at the end");
      bad++;
    }
  free (map);
  if (!bad)
    ok ("sections map: allocation sizes, the empty section at the end");
}

/* The padding of a file: the first 0x80 words of the random table of the
   file's own random_seed (in its header), little endian, from offset off,
   repeated every 512 bytes. pad_fill is the same for R2007_RANDOM_SEED, which
   the encoder writes as random_seed. */
static void
pad_seeded (unsigned char *dst, size_t len, size_t off, uint64_t seed)
{
  Crc_Rng rng;
  unsigned char table[0x80 * 4];
  size_t i;
  rng_init (&rng, seed);
  for (i = 0; i < 0x80; i++)
    put_le32 (&table[4 * i], rng.table[i]);
  for (i = 0; i < len; i++)
    dst[i] = table[(off + i) % sizeof (table)];
}

/* Every page of an r2007 file, found with decode_r2007.c's own readers of
   the file header, the pages map and the sections map:
   - an RS coded page (encoding 4): each block of 251 data bytes and 4 parity
     bytes, interleaved, is a valid Reed-Solomon codeword;
   - a raw page (encoding 1), as AutoCAD writes them (e.g. AppInfoHistory):
     the data padded to 8, then the 4 parity bytes of each block of 251 data
     bytes, the last block completed with the padding for the parity, then
     the padding from its start; the page has the Reed-Solomon coded size
     of max_size: max_size and 4 parity bytes per block of 251, rounded to
     32.
   The padding is the random table of the file's random_seed: AutoCAD seeds
   it per file.
   Returns the number of problems. *HISTORY is set once the AppInfoHistory
   page passed. */
static int
check_r2007_pages (const char *what, unsigned char *buf, size_t size,
                   int *history)
{
  Bit_Chain dat = { 0 };
  Dwg_R2007_Header hdr;
  r2007_page *pages_map, *pg;
  r2007_section *sections_map, *sec;
  int bad = 0, blocks = 0, pages = 0;
  int64_t k;

  dat.chain = buf;
  dat.size = size;
  dat.version = dat.from_version = R_2007;
  if (read_file_header (&dat, &hdr) >= DWG_ERR_VALUEOUTOFBOUNDS)
    {
      fail ("%s: file header", what);
      return 1;
    }
  dat.byte += 0x28 + hdr.pages_map_offset;
  pages_map = read_pages_map (&dat, hdr.pages_map_size_comp,
                              hdr.pages_map_size_uncomp,
                              hdr.pages_map_correction);
  pg = pages_map ? get_page (pages_map, hdr.sections_map_id) : NULL;
  if (!pg)
    {
      fail ("%s: no pages map or sections map page", what);
      pages_destroy (pages_map);
      return 1;
    }
  dat.byte = pg->offset;
  sections_map = read_sections_map (&dat, hdr.sections_map_size_comp,
                                    hdr.sections_map_size_uncomp,
                                    hdr.sections_map_correction);
  for (sec = sections_map; sec; sec = sec->next)
    for (k = 0; k < sec->num_pages; k++)
      {
        const r2007_section_page *sp = sec->pages[k];
        const unsigned char *pd;
        size_t n8, bc, i, j;
        int page_bad = 0;
        pg = sp ? get_page (pages_map, sp->id) : NULL;
        if (!pg || pg->offset > size || pg->size > size - pg->offset)
          {
            fail ("%s: section %d page %d not in the file", what,
                  (int)sec->type, (int)k);
            bad++;
            continue;
          }
        pd = &buf[pg->offset];
        pages++;
        if (sec->encoded == 1)
          {
            const uint64_t want = raw_page_size (sec->max_size);
            unsigned char pad[0x2000];
            n8 = ROUND8 (sp->uncomp_size);
            bc = (n8 + 250) / 251;
            if (pg->size != want || pg->size < n8 + 4 * bc
                || pg->size - (n8 + 4 * bc) > sizeof (pad))
              {
                fail ("%s: raw page of section %d: %u bytes, not %u", what,
                      (int)sec->type, (unsigned)pg->size, (unsigned)want);
                bad++;
                continue;
              }
            for (i = 0; i < bc; i++)
              {
                unsigned char blk[255];
                const size_t have = n8 - i * 251 < 251 ? n8 - i * 251 : 251;
                memcpy (blk, &pd[i * 251], have);
                if (have < 251)
                  pad_seeded (&blk[have], 251 - have, 0,
                              (uint64_t)hdr.random_seed);
                memcpy (&blk[251], &pd[n8 + 4 * i], 4);
                blocks++;
                if (rs_decode_data_block (blk, 0) != 0)
                  page_bad++;
              }
            pad_seeded (pad, pg->size - (n8 + 4 * bc), 0,
                        (uint64_t)hdr.random_seed);
            if (memcmp (&pd[n8 + 4 * bc], pad, pg->size - (n8 + 4 * bc)))
              page_bad++;
            if (page_bad)
              fail ("%s: raw page of section %d: %d bad blocks or padding",
                    what, (int)sec->type, page_bad);
            else if (sec->type == SECTION_APPINFOHISTORY && history)
              *history = 1;
          }
        else
          {
            n8 = ROUND8 (sp->comp_size);
            bc = (n8 + 250) / 251;
            if (bc * 255 > pg->size)
              {
                fail ("%s: page of section %d: %u bytes for %u blocks", what,
                      (int)sec->type, (unsigned)pg->size, (unsigned)bc);
                bad++;
                continue;
              }
            for (i = 0; i < bc; i++)
              {
                unsigned char blk[255];
                for (j = 0; j < 255; j++)
                  blk[j] = pd[i + j * bc];
                blocks++;
                if (rs_decode_data_block (blk, 0) != 0)
                  page_bad++;
              }
            if (page_bad)
              fail ("%s: page of section %d: %d blocks with bad parity", what,
                    (int)sec->type, page_bad);
          }
        bad += page_bad;
      }
  if (!sections_map)
    {
      fail ("%s: no sections map", what);
      bad++;
    }
  sections_destroy (sections_map);
  pages_destroy (pages_map);
  if (!bad)
    ok ("%s: parity of %d blocks in %d pages", what, blocks, pages);
  return bad;
}

static unsigned char *
read_fixture (const char *name, size_t *size)
{
  const char *srcdir = getenv ("top_srcdir");
  char path[1024];
  unsigned char *buf;
  long len;
  FILE *fp;
  if (!srcdir)
    srcdir = "../..";
  if (snprintf (path, sizeof (path), "%s/test/test-data/%s", srcdir, name)
      >= (int)sizeof (path))
    return NULL;
  fp = fopen (path, "rb");
  if (!fp)
    return NULL;
  fseek (fp, 0, SEEK_END);
  len = ftell (fp);
  fseek (fp, 0, SEEK_SET);
  buf = len > 0 ? (unsigned char *)malloc (len) : NULL;
  if (buf && fread (buf, 1, len, fp) != (size_t)len)
    {
      free (buf);
      buf = NULL;
    }
  fclose (fp);
  *size = buf ? (size_t)len : 0;
  return buf;
}

/* AutoCAD's pages follow the model of the raw and RS coded pages above,
   AppInfoHistory included, with the padding of the file's random_seed
   (not R2007_RANDOM_SEED). */
static void
acad_pages_test (void)
{
  /* AutoCAD LT 2027, and older AutoCAD r2007 files with other seeds */
  static const char *files[]
      = { "2007/block_acad2027.dwg", "2007/Line.dwg", "example_2007.dwg" };
  size_t f;
  for (f = 0; f < sizeof (files) / sizeof (files[0]); f++)
    {
      size_t size;
      int history = 0;
      char what[80];
      unsigned char *buf = read_fixture (files[f], &size);
      snprintf (what, sizeof (what), "AutoCAD r2007 pages of %s", files[f]);
      if (!buf)
        {
          fail ("%s: cannot read it", what);
          continue;
        }
      check_r2007_pages (what, buf, size, &history);
      if (!history)
        fail ("%s: no valid AppInfoHistory page", what);
      free (buf);
    }
}

/* The encoder's pages follow the same model: an AppInfoHistory of 1240
   bytes (5 blocks) is written as a raw page with its parity and padding,
   and read back unchanged. */
static void
encoded_pages_test (void)
{
  Dwg_Data dwg = { 0 }, back = { 0 };
  Bit_Chain out = { 0 };
  unsigned char *hist;
  int history = 0, error;
  unsigned i;

  dwg.header.version = dwg.header.from_version = R_2007;
  error = dwg_add_Document (&dwg, 0);
  hist = (unsigned char *)malloc (1240);
  if (error || !hist)
    {
      fail ("encoded r2007 pages: cannot construct the document");
      free (hist);
      dwg_free (&dwg);
      return;
    }
  for (i = 0; i < 1240; i++)
    hist[i] = (unsigned char)(i * 13 + 1);
  dwg.appinfohistory.size = 1240;
  dwg.appinfohistory.unknown_bits = (BITCODE_TF)hist;
  out.version = out.from_version = R_2007;
  out.opts = dwg.opts;
  error = dwg_encode (&dwg, &out);
  loglevel = test_loglevel; /* dwg_encode sets it from dwg.opts */
  if (error >= DWG_ERR_CRITICAL || !out.chain)
    fail ("encoded r2007 pages: encode error 0x%x", error);
  else
    {
      check_r2007_pages ("encoded r2007 pages", out.chain, out.byte,
                         &history);
      if (!history)
        fail ("encoded r2007 pages: no valid AppInfoHistory page");
      out.size = out.byte;
      out.byte = out.bit = 0;
      out.opts = 0;
      error = dwg_decode (&out, &back);
      loglevel = test_loglevel;
      if (error >= DWG_ERR_CRITICAL || back.appinfohistory.size != 1240
          || !back.appinfohistory.unknown_bits
          || memcmp (back.appinfohistory.unknown_bits, hist, 1240))
        fail ("encoded r2007 pages: AppInfoHistory not read back");
      else
        ok ("encoded r2007 pages: AppInfoHistory read back");
    }
  dwg_free (&dwg);
  dwg_free (&back);
  free (out.chain);
}

int
main (int argc, char const *argv[])
{
  loglevel = test_loglevel = is_make_silent () ? 0 : 2;
  section_order_test ();
  pad_fill_test ();
  raw_page_test ();
  raw_page_size_test ();
  sections_map_test ();
  acad_pages_test ();
  encoded_pages_test ();
  return failed;
}

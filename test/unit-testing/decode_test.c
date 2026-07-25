#define DECODE_TEST_C
#define _DEFAULT_SOURCE 1
#if defined(__linux__)
#  define _GNU_SOURCE 1 /* for memmem on linux */
#endif
#define _BSD_SOURCE 1
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif

extern unsigned int loglevel;

#define IS_DECODER
#include <stdlib.h>
// #include "common.h"
// CLANG_DIAG_IGNORE (-Wpragma-pack)
#include "decode.c"
#include "common.c"
// CLANG_DIAG_RESTORE
#include "tests_common.h"

static void
read_literal_length_tests (void)
{
  struct _test
  {
    const char *bits;
    unsigned char opcode;
    int result;
  } tests[] = {
    { "00000101", 5, 8 },
    { "00000001", 14, 17 },
    { "1000000010000000", 0xf0, 146 },
  };
  for (int i = 0; i < ARRAY_SIZE (tests); i++)
    {
      Bit_Chain bitchain = strtobt (tests[i].bits);
      int result = read_literal_length (&bitchain, tests[i].opcode);
      if (result == tests[i].result)
        {
          if (i == ARRAY_SIZE (tests) - 1)
            ok ("read_literal_length");
          else
            pass ();
        }
      else
        fail ("read_literal_length [%d]: %d", i, result);
      bitfree (&bitchain);
    }
}

static void
read_compressed_bytes_tests (void)
{
  struct _test
  {
    const char *bits;
    unsigned char opcode;
    unsigned mask;
    int result;
  } tests[] = {
    { "11011101", 5, 7, 7 },
    { "11011101", 5, 1, 3 },
    { "0000000010000000", 4, 1, 386 },
  };
  for (int i = 0; i < ARRAY_SIZE (tests); i++)
    {
      Bit_Chain bitchain = strtobt (tests[i].bits);
      int result
          = read_compressed_bytes (&bitchain, tests[i].opcode, tests[i].mask);
      if (result == tests[i].result)
        {
          if (i == ARRAY_SIZE (tests) - 1)
            ok ("read_compressed_bytes");
          else
            pass ();
        }
      else
        fail ("read_compressed_bytes [%d]: %d", i, result);
      bitfree (&bitchain);
    }
}

static void
two_byte_offset_tests (void)
{
  struct _test
  {
    const char *bits;
    int offset;
    BITCODE_RC result;
  } tests[] = {
    { "1111000000000111", 509, 0xf0 },
    { "1111000000000110", 445, 0xf0 },
    { "1100000000000110", 433, 0xc0 },
  };
  for (int i = 0; i < ARRAY_SIZE (tests); i++)
    {
      Bit_Chain bitchain = strtobt (tests[i].bits);
      int offset = 0;
      BITCODE_RC result = two_byte_offset (&bitchain, 1, &offset);
      if (result == tests[i].result && offset == tests[i].offset)
        {
          if (i == ARRAY_SIZE (tests) - 1)
            ok ("two_byte_offset");
          else
            pass ();
        }
      else
        fail ("two_byte_offset [%d]: %d, 0x%x", i, offset, result);
      bitfree (&bitchain);
    }
}

static void
decompress_R2004_section_tests (void)
{
  int result;
  static Bit_Chain src, dec = { 0 };
  // from example_2004 via DEBUG
  unsigned char comp_auxh_bin[225] = {
    // very bad compression indeed
    0x00, 0x01, 0xff, 0x88, 0x01, 0x21, 0x00, 0x1d, 0x00, 0x19, 0x00, 0x00,
    0x00, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x02, 0x00, 0x50, 0x00, 0x01,
    0x16, 0x00, 0x2e, 0x00, 0x5c, 0x00, 0x01, 0x04, 0x00, 0x65, 0x05, 0x5f,
    0x03, 0x02, 0x00, 0x01, 0x58, 0x01, 0xbc, 0x00, 0x00, 0x07, 0x01, 0x00,
    0x00, 0x02, 0x00, 0x07, 0x00, 0xea, 0x74, 0x25, 0x00, 0x9a, 0xe6, 0x33,
    0x04, 0xb0, 0x82, 0x25, 0x00, 0xe0, 0x1c, 0xf7, 0x01, 0xe9, 0x0b, 0x9a,
    0x08, 0x0d, 0x00, 0x54, 0x08, 0x94, 0x03, 0x5c, 0x17, 0x5d, 0x02, 0x11,
    0x50, 0x01, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xce, 0x0c,
    0x00, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00
  };
  unsigned char decomp_auxh_bin[123]
      = { 0xff, 0x88, 0x01, 0x21, 0x00, 0x1d, 0x00, 0x19, 0x00, 0x00, 0x00,
          0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x16, 0x00, 0x2e, 0x00, 0x16, 0x00, 0x2e, 0x00, 0x04, 0x00,
          0x65, 0x05, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x01, 0x00, 0x00, 0x02, 0x00, 0x07, 0x00, 0xea, 0x74, 0x25,
          0x00, 0x9a, 0xe6, 0x33, 0x04, 0xb0, 0x82, 0x25, 0x00, 0xe0, 0x1c,
          0xf7, 0x01, 0xe9, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x0d, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00 };
  unsigned char comp_ofs_bin[181]
      = { 0x00, 0x04, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xb0,
          0x82, 0x25, 0x00, 0xe0, 0x1c, 0xf7, 0x01, 0x00, 0x00, 0x00, 0x00,
          0x04, 0x32, 0x54, 0x01, 0x01, 0x00, 0x00, 0x00, 0x64, 0x8e, 0x01,
          0x00, 0x02, 0x70, 0x02, 0x01, 0xff, 0xff, 0xff, 0xff, 0x74, 0x02,
          0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x14, 0x00, 0x0a,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x11, 0x00, 0x00 };
  unsigned char decomp_ofs_bin[53]
      = { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xb0, 0x82, 0x25,
          0x00, 0xe0, 0x1c, 0xf7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x32,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 };

  src.chain = comp_auxh_bin;
  src.size = sizeof comp_auxh_bin;
  src.bit = 0;
  src.byte = 0UL;
  /* The real decoder allocates dec at max_decomp_size (0x7400=29696).
   * auxh: the LZ77 stream ends with a >C 29564 copy at dec[116], giving
   * end=29680 < 29696.  The test was broken: using sizeof(expected)=123
   * caused DWG_ERR_VALUEOUTOFBOUNDS.  Use 29696 like the real decoder. */
  bit_chain_alloc_size (&dec, 29696);
  dec.size = 29696;
  dec.bit = 0;
  dec.byte = 0UL;
  result = decompress_R2004_section (&src, &dec);
  if (result == 0
      && memcmp (dec.chain, decomp_auxh_bin, sizeof decomp_auxh_bin) == 0)
    pass ();
  else
    fail ("decompress_R2004_section auxh %d", result);

  src.chain = comp_ofs_bin;
  src.size = sizeof comp_ofs_bin;
  src.byte = 0;
  /* ofs: the LZ77 stream ends with a >C 30137 copy landing at dec[30192],
   * then 13 literal bytes -> total 30205.  32768 covers both sections. */
  bit_chain_alloc_size (&dec, 32768);
  dec.size = 32768;
  dec.byte = 0;
  result = decompress_R2004_section (&src, &dec);
  if (result == 0
      && memcmp (dec.chain, decomp_ofs_bin, sizeof decomp_ofs_bin) == 0)
    ok ("decompress_R2004_section");
  else
    fail ("decompress_R2004_section ofs %d", result);
}

/* Regression tests for CWE-125 OOB reads in decompress_r2007().
 *
 * Test B: read_literal_length() 0xFFFF tail-walk.
 *   opcode 0x0f enters the length==0x17 path; the 0xff-filled tail keeps
 *   n==0xFFFF in the do/while loop.  Unguarded: walks past the allocation.
 *   With fix: returns DWG_ERR_INTERNALERROR before the first OOB read.
 *
 * Test A: read_instructions() case-0 two-byte read at the buffer boundary.
 *   Stream ends exactly after the instruction opcode (case 0 needs 2 more
 *   bytes that aren't there).  With fix: returns DWG_ERR_INTERNALERROR.
 *
 * src_size = 8*251 = 2008 mirrors calloc(block_count=8, 251) in
 * read_data_page(); size_comp is an attacker-controlled page-header field.
 */
static void
decompress_r2007_tests (void)
{
  /* Test B */
  {
    const size_t block_count = 8;
    const size_t src_size = block_count * 251; /* 2008 */
    BITCODE_RC *src = (BITCODE_RC *)calloc (block_count, 251);
    BITCODE_RC *dst = (BITCODE_RC *)calloc (1, 1 << 20);
    int r;

    memset (src, 0xff, src_size);
    src[0] = 0x0f; /* read_literal_length: opcode+8==0x17, then 0xff-loop */

    r = decompress_r2007 (dst, 1 << 20, src, (unsigned)src_size,
                          dst + (1 << 20));
    if (r == (int)DWG_ERR_INTERNALERROR)
      pass ();
    else
      fail ("decompress_r2007 B (read_literal_length OOB): r=%d", r);
    free (src);
    free (dst);
  }

  /* Test A */
  {
    /* Stream: opcode 0x02 => literal length 10, 10 literal bytes, then
     * instruction opcode 0x05 (>>4 == 0, needs 2 more bytes) at src_end. */
    const unsigned char stream[]
        = { 0x02, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 0x05 };
    BITCODE_RC *src = (BITCODE_RC *)calloc (1, sizeof stream);
    BITCODE_RC *dst = (BITCODE_RC *)calloc (1, 4096);
    int r;

    memcpy (src, stream, sizeof stream);

    r = decompress_r2007 (dst, 4096, src, (unsigned)sizeof stream, dst + 4096);
    if (r == (int)DWG_ERR_INTERNALERROR)
      ok ("decompress_r2007 (A read_instructions, B read_literal_length OOB)");
    else
      fail ("decompress_r2007 A (read_instructions OOB): r=%d", r);
    free (src);
    free (dst);
  }
}

/* Regression test for GHSA-pcp5-hv9w-8f78: read_data_section() set the
 * source cursor from an unbounded page->offset (dat->byte = page->offset)
 * without checking it against dat->size. In the uncompressed branch
 * (comp_size == uncomp_size) the guard
 *   uncomp_size > dat->size - dat->byte
 * then underflowed (both size_t) once page->offset > dat->size, letting the
 * following memcpy read past the file buffer. Build a minimal one-page,
 * one-section, uncompressed setup with page->offset past dat->size and
 * confirm it is rejected up front instead of read OOB. */
static void
read_data_section_tests (void)
{
  Bit_Chain dat = { 0 };
  Bit_Chain sec_dat = { 0 };
  unsigned char filebuf[64];
  r2007_page page = { 0 };
  r2007_section_page section_page = { 0 };
  r2007_section_page *pages_arr[1];
  r2007_section section = { 0 };
  int error;

  memset (filebuf, 0, sizeof (filebuf));
  dat.chain = filebuf;
  dat.size = sizeof (filebuf);

  page.id = 1;
  page.size = 32;
  page.offset = dat.size + 1000; /* attacker: past the file buffer */

  section_page.offset = 0; /* dest into decomp: in-bounds */
  section_page.size = 16;
  section_page.id = 1; /* matches page.id */
  section_page.uncomp_size = 16;
  section_page.comp_size
      = 16; /* uncompressed branch: comp_size==uncomp_size */
  pages_arr[0] = &section_page;

  section.data_size = 64; /* max_decomp_size */
  section.num_pages = 1;
  section.type = SECTION_HEADER;
  section.pages = pages_arr;

  error = read_data_section (&sec_dat, &dat, &section, &page, SECTION_HEADER);
  if (error == (int)DWG_ERR_VALUEOUTOFBOUNDS)
    ok ("read_data_section: rejects out-of-bounds page->offset");
  else
    fail ("read_data_section: page->offset OOB not rejected: 0x%x", error);
  free (sec_dat.chain);
}

/* Regression test for GHSA-2m5x-9p64-6m3f: read_2004_compressed_section()
 * computed
 *   max_decomp_size = info->num_sections * info->max_decomp_size;
 * with both factors uint32 (BITCODE_RL) into a uint32_t, wrapping mod 2^32.
 * The old guard checked the WRAPPED value against 0x2f000000 and passed,
 * while a second guard used a non-wrapping int64 product and admitted
 * info->size up to ~4.29e9 -- under-allocating the section buffer while
 * later stamping sec_dat->size from the huge info->size, letting a
 * subsequent sentinel/bit_read_* scan run gigabytes past the allocation.
 * Reproduce the advisory's exact numbers (num_sections=3235,
 * max_decomp_size=0x144400 -> true product 4,296,494,080, 32-bit wrap
 * 1,526,784) and confirm the fixed 64-bit guard rejects it up front,
 * before any allocation or per-section scan happens. */
static void
read_2004_compressed_section_tests (void)
{
  Dwg_Data dwg;
  Bit_Chain dat = { 0 };
  Bit_Chain sec_dat = { 0 };
  Dwg_Section_Info info;
  int error;

  memset (&dwg, 0, sizeof (dwg));
  memset (&info, 0, sizeof (info));
  info.num_sections = 3235;
  info.max_decomp_size = 0x144400;
  info.size = 0xFFFFFFFF;
  info.compressed = 2;
  info.fixedtype = SECTION_CLASSES;
  strcpy (info.name, "AcDb:Classes");
  info.sections = (Dwg_Section **)calloc ((size_t)info.num_sections,
                                          sizeof (Dwg_Section *));

  dwg.header.section_infohdr.num_desc = 1;
  dwg.header.section_info = &info;

  error = read_2004_compressed_section (&dat, &dwg, &sec_dat, SECTION_CLASSES);
  if (error == (int)DWG_ERR_VALUEOUTOFBOUNDS && sec_dat.chain == NULL)
    ok ("read_2004_compressed_section: rejects 32-bit overflow "
        "(num_sections * max_decomp_size)");
  else
    fail ("read_2004_compressed_section: overflow not rejected: err=0x%x "
          "chain=%p",
          error, (void *)sec_dat.chain);
  free (info.sections);
}

/* Regression test for GHSA-6v3v-3gmq-63v4: decode_3dsolid()'s bound check
 *   AVAIL_BITS(dat) > 8 * FIELD_VALUE(block_size[i])
 * multiplied in 32-bit unsigned, wrapping for block_size > 0x1FFFFFFF and
 * bypassing the guard, so FIELD_TFv() then tried to read the attacker's
 * huge block_size from the bitstream -- a heap OOB read that manifested
 * as heap corruption / a double-free during dwg_free cleanup on a crafted
 * DWG. Encode a minimal ACIS v1 3DSOLID prefix (acis_empty=0, unknown=0,
 * version=1, block_size[0]=0x20000000) into a tiny 16-byte Bit_Chain:
 * 8 * 0x20000000 wraps to 0 in 32-bit, so the old check
 * "AVAIL_BITS(dat) > 0" would pass and attempt to read ~512MB past the
 * buffer. Confirm the fix rejects the oversized block_size instead. */
static void
decode_3dsolid_tests (void)
{
  Bit_Chain dat = { 0 };
  Bit_Chain hdl_dat = { 0 };
  Dwg_Object obj;
  Dwg_Entity_3DSOLID _obj;
  int error;

  bitprepare (&dat, 16);
  bit_write_B (&dat, 0);           /* acis_empty = 0 */
  bit_write_B (&dat, 0);           /* unknown = 0 */
  bit_write_BS (&dat, 1);          /* version = 1 (SAT) */
  bit_write_BL (&dat, 0x20000000); /* block_size[0]: overflows *8 in 32-bit */
  dat.bit = 0;
  dat.byte = 0;

  memset (&obj, 0, sizeof (obj));
  memset (&_obj, 0, sizeof (_obj));

  error = decode_3dsolid (&dat, &hdl_dat, &obj, &_obj);
  if (error < DWG_ERR_CRITICAL && _obj.block_size && _obj.block_size[0] == 0)
    ok ("decode_3dsolid: rejects oversized block_size (32-bit *8 overflow)");
  else
    fail ("decode_3dsolid: oversized block_size not rejected: err=0x%x "
          "block_size[0]=%u",
          error, _obj.block_size ? (unsigned)_obj.block_size[0] : ~0u);

  free (_obj.block_size);
  if (_obj.encr_sat_data)
    {
      free (_obj.encr_sat_data[0]);
      free (_obj.encr_sat_data);
    }
  free (_obj.acis_data);
  bitfree (&dat);
}

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 2;

  read_literal_length_tests ();
  read_compressed_bytes_tests ();
  two_byte_offset_tests ();
  decompress_R2004_section_tests ();
  decompress_r2007_tests ();
  read_data_section_tests ();
  read_2004_compressed_section_tests ();
  decode_3dsolid_tests ();

  return numfailed () ? 1 : 0;
}

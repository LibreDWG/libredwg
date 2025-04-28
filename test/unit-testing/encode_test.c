#define ENCODE_TEST_C
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

#define IS_ENCODER
#include <stdlib.h>
#include "../../src/common.h"
// CLANG_DIAG_IGNORE (-Wpragma-pack)
#include "encode.c"
// CLANG_DIAG_RESTORE
#include "tests_common.h"

BITCODE_RL size = SECTION_R13_SIZE;
// Dwg_Section_Type_r13 section_order[SECTION_R13_SIZE] = { 0 };

static int
find_duplicates (const Dwg_Data *dwg)
{
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size; i++)
    {
      Dwg_Section_Type_r13 id = section_order[i];
      for (Dwg_Section_Type_r13 j = 0; (unsigned)j < size; j++)
        {
          if (i == j)
            continue;
          if (section_order[j] == id && (unsigned)id != size)
            {
              fail ("find_duplicate %u", (unsigned)id);
              section_order_trace (dwg, size,
                                   (Dwg_Section_Type_r13 *)&section_order);
              return 1;
            }
        }
    }
  return 0;
}

static void
section_reset (const Dwg_Data *dwg)
{
  memset (section_order, 0, sizeof section_order);
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size; i++)
    section_order[i] = i;
  if (dwg->opts & DWG_OPTS_LOGLEVEL)
    fprintf (stderr, "---\n");
  section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
}

static void
test_section_find (const Dwg_Data *dwg)
{
  unsigned id;
  size = SECTION_R13_SIZE;
  section_reset (dwg);
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i <= size; i++)
    {
      id = section_find ((Dwg_Section_Type_r13 *)&section_order, size, i);
      if (id != (unsigned)i) // 7 not found, returns SECTION_R13_SIZE
        {
          fail ("section_find %u => %u", (unsigned)i, (unsigned)id);
          section_order_trace (dwg, size,
                               (Dwg_Section_Type_r13 *)&section_order);
        }
    }
  id = section_find ((Dwg_Section_Type_r13 *)&section_order, size, size + 1);
  if (id != SECTION_R13_SIZE) // not found
    {
      fail ("section_find %u => %u", size + 1, id);
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
    }
}

static void
test_section_move_top (const Dwg_Data *dwg)
{
  int err = 0;
  size = SECTION_R13_SIZE - 1;
  section_reset (dwg);
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size - 1; i++)
    {
      // without insert
      if (section_move_top ((Dwg_Section_Type_r13 *)&section_order, &size, i))
        {
          err++;
          fail ("section_move_top existing %u", (unsigned)i);
        }
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
      if (section_order[0] != i)
        {
          err++;
          fail ("section_move_top %u not first", (unsigned)i);
        }
      if (find_duplicates (dwg))
        {
          err++;
          fail ("duplicates");
        }
    }
  // with insert
  if (!section_move_top ((Dwg_Section_Type_r13 *)&section_order, &size, size))
    {
      err++;
      fail ("section_move_top insert");
    }
  section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
  if (!err)
    ok ("test_section_move_top");
  else
    fail ("test_section_move_top");
  size = SECTION_R13_SIZE;
}

static inline unsigned
maxrand (unsigned max)
{
  unsigned rnd = (unsigned)rand ();
  return rnd % max;
}

static void
test_section_remove (const Dwg_Data *dwg)
{
  int err = 0;
  int sz = size - 1;
  section_reset (dwg);
  for (unsigned i = 0; i < size; i++)
    {
      Dwg_Section_Type_r13 id = maxrand (size);
      if (section_remove ((Dwg_Section_Type_r13 *)&section_order, &size, id))
        sz--;
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
      if (find_duplicates (dwg))
        {
          err++;
          fail ("test_section_remove");
        }
    }
  if (!err)
    ok ("test_section_remove");
  else
    fail ("test_section_remove");
}

static void
test_section_move_before (const Dwg_Data *dwg)
{
  int err = 0;
  size = SECTION_R13_SIZE - 2;
  section_reset (dwg);
  for (unsigned i = 0; i < size; i++)
    {
      Dwg_Section_Type_r13 id = maxrand (size);
      Dwg_Section_Type_r13 before = maxrand (size);
      while (before == id)
        before = maxrand (size);
      if (section_move_before ((Dwg_Section_Type_r13 *)&section_order, &size,
                               id, before))
        {
          fail ("move_before %u", (unsigned)id);
          err++;
        }
      section_order_trace (dwg, size, (Dwg_Section_Type_r13 *)&section_order);
      err += find_duplicates (dwg);
    }
  if (!section_move_before ((Dwg_Section_Type_r13 *)&section_order, &size, 6,
                            4))
    {
      fail ("move_before %u inserts", 6);
      err++;
    }
  err += find_duplicates (dwg);
  if (!err)
    ok ("test_section_move_before");
  else
    fail ("test_section_move_before");
  size = SECTION_R13_SIZE;
}

static void
test_compress_R2004_section (void)
{
  int result;
  static Bit_Chain src, dat, dec = { 0 };
  // from example_2004 via DEBUG
  unsigned char const comp_auxh_bin[225] = {
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
  unsigned char const decomp_auxh_bin[123]
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
  unsigned char const comp_ofs_bin[181]
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
  unsigned char const decomp_ofs_bin[53]
      = { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xb0, 0x82, 0x25,
          0x00, 0xe0, 0x1c, 0xf7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x32,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 };
  uint32_t comp_data_size;

  // compress src => dat
  src.chain = (unsigned char *)decomp_auxh_bin;
  src.size = sizeof decomp_auxh_bin;
  src.bit = 0;
  src.byte = 0UL;

  bit_chain_alloc_size (&dat, 2 * sizeof comp_auxh_bin);
  dat.size = sizeof comp_auxh_bin;
  dat.bit = 0;
  dat.byte = 0UL;
  comp_data_size = dat.size & 0xFFFFFFFF;
  result = compress_R2004_section (&src, &dat);
  if (result == 0)
    {
      // decompress dat => dec
      result = decompress_R2004_section (&dat, &dec);
      if (result == 0
          && memcmp (dec.chain, decomp_auxh_bin, sizeof decomp_ofs_bin) == 0)
        pass ();
      else
        {
          fail ("decompress compressed section back auxh %d %lu => %lu", result,
                (unsigned long)dat.size, (unsigned long)dec.size);
          if (loglevel >= 3)
            {
              if (loglevel >= 6)
                {
                  bit_explore_chain (&dat, 0, dat.size);
                  bit_explore_chain (&dec, 0, MIN (dec.size, 0x1000));
                }
              for (unsigned i=0; i < sizeof decomp_auxh_bin; i++)
                {
                  if (dec.chain[i] != decomp_auxh_bin[i])
                    {
                      fprintf (
                          stderr,
                          "diff 0x%x: %02x%02x%02x%02x != %02x%02x%02x%02x\n", i,
                          dec.chain[i], dec.chain[i + 1], dec.chain[i + 2],
                          dec.chain[i + 3], decomp_auxh_bin[i],
                          decomp_auxh_bin[i + 1], decomp_auxh_bin[i + 2],
                          decomp_auxh_bin[i + 3]);
                      break;
                    }
                }
            }
        }
    }
  else
    fail ("compress_R2004_section auxh %d %lu", result,
          (unsigned long)comp_data_size);

  // compress src => dat
  src.chain = (unsigned char *)decomp_ofs_bin;
  src.size = sizeof decomp_ofs_bin;
  src.bit = 0;
  src.byte = 0UL;

  bit_chain_alloc_size (&dat, 2 * sizeof comp_ofs_bin);
  dat.size = sizeof comp_ofs_bin;
  dat.bit = 0;
  dat.byte = 0UL;
  comp_data_size = dat.size & 0xFFFFFFFF;
  result = compress_R2004_section (&src, &dat);
  if (result == 0)
    {
      // decompress dat => dec
      result = decompress_R2004_section (&dat, &dec);
      if (result == 0
          && memcmp (dec.chain, decomp_ofs_bin, sizeof decomp_ofs_bin) == 0)
        ok ("compress_R2004_section");
      else
        {
          fail ("decompress compressed section back ofs %d %lu => %lu", result,
                (unsigned long)dat.size, (unsigned long)dec.size);
          if (loglevel >= 3)
            {
              if (loglevel >= 6)
                {
                  bit_explore_chain (&dat, 0, dat.size);
                  bit_explore_chain (&dec, 0, MIN (dec.size, 0x1000));
                }
              for (unsigned i=0; i < sizeof decomp_ofs_bin; i++)
                {
                  if (dec.chain[i] != decomp_ofs_bin[i])
                    {
                      fprintf (
                          stderr,
                          "diff 0x%x: %02x%02x%02x%02x != %02x%02x%02x%02x\n", i,
                          dec.chain[i], dec.chain[i + 1], dec.chain[i + 2],
                          dec.chain[i + 3], decomp_ofs_bin[i],
                          decomp_ofs_bin[i + 1], decomp_ofs_bin[i + 2],
                          decomp_ofs_bin[i + 3]);
                      break;
                    }
                }
            }
        }
    }
  else
    fail ("compress_R2004_section ofs %d %lu", result,
          (unsigned long)comp_data_size);
}

int
main (int argc, char const *argv[])
{
  Dwg_Data dwg;
  dwg.opts = loglevel = loglevel_from_env ();
  dwg.header.version = R_2000;

  test_section_find (&dwg);
  test_section_move_top (&dwg);
  test_section_remove (&dwg);
  test_section_move_before (&dwg);

  test_compress_R2004_section ();

  return failed;
}

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

extern unsigned int loglevel;

#define IS_ENCODER
#include <stdlib.h>
// #include "common.h"
// CLANG_DIAG_IGNORE (-Wpragma-pack)
#include "encode.c"
#include "common.c"
// CLANG_DIAG_RESTORE
#include "decode.h"
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
compress_R2004_section_tests (void)
{
  int result;
  uint32_t comp_data_size;
  Bit_Chain comp = { 0 }, dec = { 0 };
  /* from decode_test.c: decompressed AuxHeader data (123 bytes) */
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
  unsigned char decomp_ofs_bin[53]
      = { 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xb0, 0x82, 0x25,
          0x00, 0xe0, 0x1c, 0xf7, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x32,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 };

  /* Test 1: compress auxh, decompress, compare */
  bit_chain_alloc_size (&comp, sizeof decomp_auxh_bin * 2);
  comp.byte = 0;
  comp.bit = 0;
  result = compress_R2004_section (&comp, decomp_auxh_bin,
                                   sizeof decomp_auxh_bin, &comp_data_size);
  if (result)
    {
      fail ("compress_R2004_section auxh returned %d", result);
      goto test2;
    }
  /* decompress and verify roundtrip */
  comp.byte = 0;
  comp.bit = 0;
  comp.size = comp_data_size;
  bit_chain_alloc_size (&dec, sizeof decomp_auxh_bin);
  dec.size = sizeof decomp_auxh_bin;
  dec.byte = 0;
  dec.bit = 0;
  result = decompress_R2004_section (&comp, &dec);
  if (result == 0 && dec.size == sizeof decomp_auxh_bin
      && memcmp (dec.chain, decomp_auxh_bin, sizeof decomp_auxh_bin) == 0)
    pass ();
  else
    fail ("compress_R2004_section auxh roundtrip %d %lu", result,
          (unsigned long)dec.size);

test2:
  /* Test 2: compress ofs, decompress, compare */
  comp.byte = 0;
  comp.bit = 0;
  if (comp.size < sizeof decomp_ofs_bin * 2)
    bit_chain_alloc_size (&comp, sizeof decomp_ofs_bin * 2);
  result = compress_R2004_section (&comp, decomp_ofs_bin,
                                   sizeof decomp_ofs_bin, &comp_data_size);
  if (result)
    {
      fail ("compress_R2004_section ofs returned %d", result);
      goto cleanup;
    }
  comp.byte = 0;
  comp.bit = 0;
  comp.size = comp_data_size;
  dec.byte = 0;
  dec.bit = 0;
  dec.size = sizeof decomp_ofs_bin;
  result = decompress_R2004_section (&comp, &dec);
  if (result == 0
      && memcmp (dec.chain, decomp_ofs_bin, sizeof decomp_ofs_bin) == 0)
    ok ("compress_R2004_section");
  else
    fail ("compress_R2004_section ofs roundtrip %d %lu", result,
          (unsigned long)dec.size);

cleanup:
  free (comp.chain);
  free (dec.chain);
}

int
main (int argc, char const *argv[])
{
  Dwg_Data dwg;
  loglevel = is_make_silent () ? 0 : 3;
  dwg.header.version = R_2000;

  test_section_find (&dwg);
  test_section_move_top (&dwg);
  // loglevel = is_make_silent () ? 0 : 3;
  test_section_remove (&dwg);
  test_section_move_before (&dwg);

  compress_R2004_section_tests ();
  return failed;
}

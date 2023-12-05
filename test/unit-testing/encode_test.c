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

unsigned size = SECTION_R13_SIZE;
Dwg_Section_Type_r13 section_order[SECTION_R13_SIZE] = { 0 };

static int
find_duplicates (void)
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
              section_order_trace (size, (Dwg_Section_Type_r13 *)&section_order);
              return 1;
            }
        }
    }
  return 0;
}

static void
section_reset (void)
{
  memset (section_order, 0, sizeof section_order);
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size; i++)
    section_order[i] = i;
  fprintf (stderr, "---\n");
  section_order_trace (size, (Dwg_Section_Type_r13 *)&section_order);
}

static void
test_section_find (void)
{
  unsigned id;
  size = SECTION_R13_SIZE;
  section_reset ();
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i <= size; i++)
    {
      id = section_find ((Dwg_Section_Type_r13 *)&section_order, i);
      if (id != (unsigned)i) // 7 not found, returns SECTION_R13_SIZE
        {
          fail ("section_find %u => %u", (unsigned)i, (unsigned)id);
          section_order_trace (size, (Dwg_Section_Type_r13 *)&section_order);
        }
    }
  id = section_find ((Dwg_Section_Type_r13 *)&section_order, size + 1);
  if (id != SECTION_R13_SIZE) // not found
    {
      fail ("section_find %u => %u", size + 1, id);
      section_order_trace (size, (Dwg_Section_Type_r13 *)&section_order);
    }
}

static void
test_section_move_top (void)
{
  int err = 0;
  size = SECTION_R13_SIZE - 1;
  section_reset ();
  for (Dwg_Section_Type_r13 i = 0; (unsigned)i < size - 1; i++)
    {
      // without insert
      if (section_move_top ((Dwg_Section_Type_r13 *)&section_order, i))
        {
          err++;
          fail ("section_move_top existing %u", (unsigned)i);
        }
      section_order_trace (size, (Dwg_Section_Type_r13 *)&section_order);
      if (section_order[0] != i)
        {
          err++;
          fail ("section_move_top %u not first", (unsigned)i);
        }
      if (find_duplicates ())
        {
          err++;
          fail ("duplicates");
        }
    }
  // with insert
  if (!section_move_top ((Dwg_Section_Type_r13 *)&section_order, size))
    {
      err++;
      fail ("section_move_top insert");
    }
  section_order_trace (size, (Dwg_Section_Type_r13 *)&section_order);
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
test_section_remove (void)
{
  int err = 0;
  int sz = size - 1;
  section_reset ();
  for (unsigned i = 0; i < size; i++)
    {
      Dwg_Section_Type_r13 id = maxrand (size);
      if (section_remove ((Dwg_Section_Type_r13 *)&section_order, id))
        sz--;
      section_order_trace (sz, (Dwg_Section_Type_r13 *)&section_order);
      if (find_duplicates ())
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

static void test_section_move_before(void) {
  int err = 0;
  size = SECTION_R13_SIZE - 1;
  section_reset ();
  for (unsigned i = 0; i < size; i++)
    {
      Dwg_Section_Type_r13 id = maxrand(size);
      Dwg_Section_Type_r13 before = maxrand(size);
      while (before == id)
        before = maxrand(size);
      if (section_move_before ((Dwg_Section_Type_r13 *)&section_order,
                                id, before))
        {
          fail ("move_before %u", (unsigned)id);
          err++;
        }
      section_order_trace (size,(Dwg_Section_Type_r13 *)&section_order);
      err += find_duplicates ();
    }
  if (!section_move_before ((Dwg_Section_Type_r13 *)&section_order,
                            6, 4))
    {
      fail ("move_before %u inserts", 6);
      err++;
    }
  err += find_duplicates ();
  if (!err)
    ok ("test_section_move_before");
  else
    fail ("test_section_move_before");
  size = SECTION_R13_SIZE;
}

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 3;

  test_section_find ();
  test_section_move_top ();
  loglevel = is_make_silent () ? 0 : 3;
  test_section_remove ();
  test_section_move_before ();

  return failed;
}

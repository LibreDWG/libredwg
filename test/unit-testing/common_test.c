#define COMMON_TEST_C
#include <stddef.h>
#include "tests_common.h"
#include "../../src/common.c"

static void
common_memmem_tests (void)
{
  static char *hay = (char *)"0123456789";
  char *needle = (char *)"01";
  void *p;

  // positive
  if (my_memmem (hay, 10, needle, 2) != hay)
    fail ("memmem %s not at 0", needle);
  needle = (char *)"1234567890";
  if (my_memmem (hay, 10, needle, 9) != &hay[1])
    fail ("memmem %s not at 0", needle);
  needle = (char *)"789";
  if ((p = my_memmem (hay, 10, needle, 3)) != &hay[7])
    fail ("memmem %s not at 7 but at %p of %p", needle, p, hay);

  // not found
  needle = (char *)"012344567890";
  if (my_memmem (hay, 10, needle, 11))
    fail ("memmem %s found", needle);
  needle = (char *)"1234456780";
  if (my_memmem (hay, 10, needle, 9))
    fail ("memmem %s found", needle);
  needle = (char *)"7890";
  if (my_memmem (hay, 10, needle, 4))
    fail ("memmem %s found", needle);
  else
    ok ("memmem");
}

// test versions:
// strict ordering of dwg_version
// matching enum <=> release type
static void
common_versions_tests (void)
{
  uint8_t prev_version = 0;
  assert (R_AFTER + 1 == ARRAY_SIZE (dwg_versions));
  assert (strEQc (dwg_versions[R_2018].type, "r2018"));
  assert (strEQc (dwg_versions[R_2007].type, "r2007"));
  assert (strEQc (dwg_versions[R_2004].type, "r2004"));
  assert (strEQc (dwg_versions[R_2000].type, "r2000"));
  assert (strEQc (dwg_versions[R_13].type, "r13"));
  assert (strEQc (dwg_versions[R_11].type, "r11"));
  assert (strEQc (dwg_versions[R_2_0].type, "r2.0"));
  assert (strEQc (dwg_versions[R_AFTER].type, "r>2022"));
  assert (!dwg_versions[R_AFTER].hdr);
  if (!failed)
    ok ("dwg_versions messup");

  for (int i = 0; i < R_AFTER; i++)
    {
      const uint8_t dwg_version = dwg_versions[i].dwg_version;
      // strict ordering of dwg_versions
      if (dwg_version < prev_version)
        fail ("dwg_versions[%d].dwg_version %x >= %x", i, dwg_version,
              prev_version);
      prev_version = dwg_version;
    }
  if (!failed)
    ok ("dwg_versions ordered");

  if (!dwg_version_struct (R_AFTER))
    ok ("!dwg_version_struct (R_AFTER)");
  else
    fail ("!dwg_version_struct (R_AFTER)");

  assert (strEQc (dwg_version_codes (R_2018), "AC1032"));
  assert (strEQc (dwg_version_codes (R_2000), "AC1015"));
  assert (dwg_version_as ("r2000") == R_2000);
  assert (dwg_version_as ("r2018") == R_2018);

  assert (dwg_version_hdr_type ("AC1032") == R_2018);
  assert (dwg_version_hdr_type ("AC1015") == R_2000);
  assert (dwg_version_hdr_type2 ("AC1500", 0x16) == R_2000b);
  assert (dwg_version_hdr_type2 ("AC1016", 0x17) == R_2000i);
  assert (dwg_version_hdr_type2 ("AC1018", 0x18) == R_2004c);
  assert (dwg_version_hdr_type2 ("AC1017", 0x18) == R_2002);
  assert (dwg_version_hdr_type2 ("AC402b", 0) == R_2004b);
  assert (dwg_version_hdr_type2 ("AC1018", 0x19) == R_2004);
  assert (dwg_version_hdr_type2 ("AC1019", 0x18) == R_INVALID);
  assert (dwg_version_hdr_type2 ("AC1019", 0x19) == R_2005);
  assert (dwg_version_hdr_type2 ("AC1020", 0x19) == R_2006);
  assert (dwg_version_hdr_type2 ("AC1021", 0x1a) == R_2007b);
  assert (dwg_version_hdr_type2 ("AC1021", 0x1b) == R_2007);
  assert (dwg_version_hdr_type ("AC1018") == R_2004);

  ok ("versions");
}

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 2;
  common_memmem_tests ();
  common_versions_tests ();
  return failed;
}

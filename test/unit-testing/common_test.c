#define COMMON_TEST_C
#include <stddef.h>
#include "../../src/common.c"
#include "tests_common.h"

static void
common_memmem_tests (void)
{
  static char *hay = (char*)"0123456789";
  char *needle = (char*)"01";
  void *p;

  // positive
  if (my_memmem (hay, 10, needle, 2) != hay)
    fail ("memmem %s not at 0", needle);
  needle = (char*)"1234567890";
  if (my_memmem (hay, 10, needle, 9) != &hay[1])
    fail ("memmem %s not at 0", needle);
  needle = (char*)"789";
  if ((p = my_memmem (hay, 10, needle, 3)) != &hay[7])
    fail ("memmem %s not at 7 but at %p of %p", needle, p, hay);

  // not found
  needle = (char*)"012344567890";
  if (my_memmem (hay, 10, needle, 11))
    fail ("memmem %s found", needle);
  needle = (char*)"1234456780";
  if (my_memmem (hay, 10, needle, 9))
    fail ("memmem %s found", needle);
  needle = (char*)"7890";
  if (my_memmem (hay, 10, needle, 4))
    fail ("memmem %s found", needle);
  else
    ok("memmem");
}

int
main (int argc, char const *argv[])
{
  common_memmem_tests ();
  return failed;
}

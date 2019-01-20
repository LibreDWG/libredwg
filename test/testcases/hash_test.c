#include "../../src/hash.h"

#include "../../src/common.h"
CLANG_DIAG_IGNORE(-Wformat-nonliteral)
CLANG_DIAG_IGNORE(-Wmissing-prototypes)
#include <dejagnu.h>
CLANG_DIAG_RESTORE
CLANG_DIAG_RESTORE
#include <string.h>
#include <stdlib.h>
#include "tests_common.h"

#define MAX_SIZE 2453916
// 1 for full pressure, 5 for light pressure
#define PRESSURE_FACTOR 1

static inline uint32_t maxrand(int max)
{
  uint32_t rnd = (uint32_t)rand();
  return rnd % (uint32_t)max;
}

int
main (int argc, char const *argv[])
{
  const int max = MAX_SIZE;
  int i;
  dwg_inthash *hash;

  hash = hash_new(max);
  for (i=1; i < max/PRESSURE_FACTOR; i++)
    {
      int32_t rnd = i; //we need to ensure full coverage.
      // i = maxrand(i)+1; for a more realistic workload
      hash_set(hash, rnd, rnd+1);
    }
  pass("hash size(%d) => %u", max, hash->size);

  for (i=1; i<max/PRESSURE_FACTOR; i++)
    {
      uint32_t v;
      uint32_t rnd = maxrand(i)+1;
      if ((v = hash_get(hash, rnd)) != rnd+1)
        fail("hash_get(%d) => %d", rnd, v);
    }

  return 0;
}

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

#define IS_DECODER
#include <stdlib.h>
#include "../../src/common.h"
// CLANG_DIAG_IGNORE (-Wpragma-pack)
#include "decode.c"
// CLANG_DIAG_RESTORE
#include "tests_common.h"

void read_literal_length_tests (void);
void read_long_compression_offset_tests (void);
void read_two_byte_offset_tests (void);

void
read_literal_length_tests (void)
{
  // Prepare the sample data
  Bit_Chain bitchain = strtobt ("00000101");
  unsigned char opcode = 0x05;
  if (read_literal_length (&bitchain, &opcode) == 0x08)
    ok ("read_literal_length");
  else
    fail ("read_literal_length");
  bitfree (&bitchain);
}

void
read_long_compression_offset_tests (void)
{
  Bit_Chain bitchain = strtobt ("11011101");
  if (read_long_compression_offset (&bitchain) == 0xDD)
    pass ();
  else
    fail ("read_long_compression_offset");
  bitfree (&bitchain);
}

/* This functions calls tests for read_two_byte_offset()
 */
void
read_two_byte_offset_tests (void)
{
  Bit_Chain bitchain = strtobt ("1111000000000111");
  unsigned int litlength = 0x03;
  int result = read_two_byte_offset (&bitchain, &litlength);
  if (result == 508 && litlength == 0)
    pass ();
  else
    fail ("read_two_byte_offset");
  bitfree (&bitchain);
}

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 2;

  read_literal_length_tests ();
  read_long_compression_offset_tests ();
  read_two_byte_offset_tests ();

  return 0;
}

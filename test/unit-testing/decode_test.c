#define DECODE_TEST_C
#include "../../src/common.h"
CLANG_DIAG_IGNORE (-Wpragma-pack)
#include "../../src/decode.c"
CLANG_DIAG_RESTORE
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
}

void
read_long_compression_offset_tests (void)
{
  Bit_Chain bitchain = strtobt ("11011101");
  if (read_long_compression_offset (&bitchain) == 0xDD)
    {
      pass ();
    }
  else
    {
      fail ("read_long_compression_offset");
    }
}

/* This functions calls tests for read_two_byte_offset()
 */
void
read_two_byte_offset_tests (void)
{
  Bit_Chain bitchain = strtobt ("1111000000000111");
  int litlength = 0x03;
  int result = read_two_byte_offset (&bitchain, &litlength);
  if (result == 508 && litlength == 0)
    {
      pass ();
    }
  else
    {
      fail ("read_two_byte_offset");
    }
}

int
main (int argc, char const *argv[])
{
  read_literal_length_tests ();
  read_long_compression_offset_tests ();
  read_two_byte_offset_tests ();

  return 0;
}

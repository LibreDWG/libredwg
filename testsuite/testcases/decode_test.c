#include "../../src/decode.c"
#include <dejagnu.h>
#include "tests_common.h"

/* This functions calls tests for read_literal_length()
 */
void
read_literal_length_tests ()
{
  //Prepare the sample data
  Bit_Chain bitchain = strtobt ("00000101");
  unsigned char opcode = 0x05;
  if (read_literal_length (&bitchain, &opcode) == 0x08)
    {
      pass ("read_literal_length");
    }
  else
    {
      fail ("read_literal_length");
    }
}

/* This functions calls tests for read_long_compression_offset()
 */
void
read_long_compression_offset_tests ()
{
  Bit_Chain bitchain = strtobt ("11011101");
  if (read_long_compression_offset (&bitchain) == 0xDD)
    {
      pass ("read_long_compression_offset");
    }
  else
    {
      fail ("read_long_compression_offset");
    }
}

/* This functions calls tests for read_two_byte_offset()
 */
void
read_two_byte_offset_tests ()
{
  Bit_Chain bitchain = strtobt ("1111000000000111");
  int litlength = 0x03;
  int result = read_two_byte_offset (&bitchain, &litlength);
  if (result == 508 && litlength == 0)
    {
      pass ("read_two_byte_offset");
    }
  else
    {
      fail ("read_two_byte_offset");
    }
}

int
main (int argc, char const *argv[])
{
  /* Tests for read_literal_ength */
  read_literal_length_tests ();
  /* End of tests for read_literal_length */

  /* Tests for read_long_compression_offset */
  read_long_compression_offset_tests ();
  /* End of tests for read_long_compression_offset */

  /* Tests for read_two_byte_offset() */
  read_two_byte_offset_tests ();
  /* End of tests for read_two_byte_offset() */

  return 0;
}

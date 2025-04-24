#define BITS_TEST_C
#include "config.h"
#include "bits.h"
#include "common.h"
#include "codepages.h"
#include "decode_r11.h"
#include <string.h>
#include <stdlib.h>
#include "tests_common.h"
#include <assert.h>

static void
bit_advance_position_tests (void)
{
  Bit_Chain bitchain = strtobt ("10101010");
  if (bitchain.bit == 0 && bitchain.byte == 0)
    ok ("bit_advance_position");
  else
    fail ("bit_advance_position");
  bitfree (&bitchain);
}

static void
bit_read_B_tests (void)
{
  Bit_Chain bitchain = strtobt ("101010");
  unsigned char result = bit_read_B (&bitchain);
  if (result == 0x01)
    pass ();
  else
    fail ("bit_read_B %c", result);
  bitfree (&bitchain);
}

static void
bit_write_B_tests (void)
{
  Bit_Chain bitchain = strtobt ("0000000");
  bit_write_B (&bitchain, 1);

  if (bitchain.chain[0] == 0x80)
    pass ();
  else
    fail ("bit_write_B");
  bitfree (&bitchain);
}

static void
bit_read_BB_tests (void)
{
  Bit_Chain bitchain = strtobt ("10101010");
  unsigned char result = bit_read_BB (&bitchain);
  if (result == 2)
    pass ();
  else
    fail ("bit_read_BB %d", result);
  bitfree (&bitchain);
}

static void
bit_write_BB_tests (void)
{
  Bit_Chain bitchain = strtobt ("01000000");
  bit_advance_position (&bitchain, 2);
  bit_write_BB (&bitchain, 0x2);

  if (bitchain.chain[0] == 0x60)
    pass ();
  else
    fail ("bit_write_BB %d", bitchain.chain[0]);
  bitfree (&bitchain);
}

#if 0
static void
bit_read_3B_tests (void)
{
  Bit_Chain bitchain = strtobt ("100111");
  unsigned char result = bit_read_3B (&bitchain);
  if (result == 2)
    pass ();
  else
    fail ("bit_read_3B %d", result);
  result = bit_read_3B (&bitchain);
  if (result == 0)
    pass ();
  else
    fail ("bit_read_3B %d", result);
  result = bit_read_3B (&bitchain);
  if (result == 7)
    pass ();
  else
    fail ("bit_read_3B %d", result);
  bitfree (&bitchain);
}

static void
bit_write_3B_tests (void)
{
  Bit_Chain bitchain = strtobt ("01000000");
  bit_advance_position (&bitchain, 2);
  bit_write_3B (&bitchain, 0x2);

  if (bitchain.chain[0] == 80)
    pass ();
  else
    fail ("bit_write_3B %d", bitchain.chain[0]);
  bitfree (&bitchain);
}
#endif

/* This function calls tests for bit_write_4BITS_tests()
   Used in VIEW view_mode, type 71
 */
static void
bit_write_4BITS_tests (void)
{
  Bit_Chain bitchain = strtobt ("00000000");
  bit_write_4BITS (&bitchain, 0x0F);
  // printf ("%x\n", bitchain.chain[0]);

  if (bitchain.chain[0] == 0xF0)
    pass ();
  else
    fail ("bit_write_4BITS %x", bitchain.chain[0]);
  bitfree (&bitchain);
}

static void
bit_read_4BITS_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111");
  unsigned char result = bit_read_4BITS (&bitchain);
  if (result == 0xF)
    pass ();
  else
    fail ("bit_read_4BITS 0x%X != 0xF dat:%x", (unsigned)result,
          bitchain.chain[0]);
  bitfree (&bitchain);

  // FIXME chibicc
#define test_4bits(s, x)                                                      \
  bitchain = strtobt (s);                                                     \
  result = bit_read_4BITS (&bitchain);                                        \
  if (result == x)                                                            \
    pass ();                                                                  \
  else                                                                        \
    fail ("bit_read_4BITS 0x%X != 0x%X dat:%x", (unsigned)result, x,          \
          bitchain.chain[0]);                                                 \
  bitfree (&bitchain)

  test_4bits ("0000", 0x0);
#ifdef __chibicc__
  printf ("chibicc bugs:\n");
#endif
  test_4bits ("0001", 0x1);
  test_4bits ("0010", 0x2);
  test_4bits ("0011", 0x3);
  test_4bits ("0100", 0x4);
  test_4bits ("0101", 0x5);
  test_4bits ("0111", 0x7);
  test_4bits ("1000", 0x8);
  test_4bits ("1001", 0x9);
  test_4bits ("1010", 0xA);
  test_4bits ("1011", 0xB);
  test_4bits ("1100", 0xC);
  test_4bits ("1101", 0xD);
  test_4bits ("1110", 0xE);
  test_4bits ("1111", 0xF);
}

static void
bit_BLL_tests (void)
{
  /* 3: 001 => 1, 00000011 */
  Bit_Chain bitchain = strtobt ("00100000011");
  BITCODE_BLL result = bit_read_BLL (&bitchain);
  if (result == 3)
    pass ();
  else
    fail ("bit_read_BLL 3 => " FORMAT_BLL, result);
  bit_set_position (&bitchain, 0);

  bit_write_BLL (&bitchain, 0xF); // => 00100001 11100000
  if (bitchain.chain[0] == 0x21 && bitchain.chain[1] == 0xe0)
    pass ();
  else
    fail ("bit_write_BLL 0xF => %02X %02X", bitchain.chain[0],
          bitchain.chain[1]);
  bit_set_position (&bitchain, 0);

  bit_write_BLL (&bitchain, 0x6C4E);
  // 0b010 + 0b110110001001110. p/x 0b010 rev(00110100 01001110) => 0x49 0xcd
  // 0x80
  if (bitchain.chain[0] == 0x49 && bitchain.chain[1] == 0xcd
      && bitchain.chain[2] == 0x80)
    pass ();
  else
    fail ("bit_write_BLL 0x6C4E => %02X %02X %02X", bitchain.chain[0],
          bitchain.chain[1], bitchain.chain[2]);
  bit_set_position (&bitchain, 0);

  bit_write_BLL (&bitchain, 0x100000);
  // 0b011 1000000000000000000000
  if (bitchain.chain[0] == 0x60 && bitchain.chain[1] == 0x00
      && bitchain.chain[2] == 0x02)
    pass ();
  else
    fail ("bit_write_BLL 0x100000 => %02X %02X %02X", bitchain.chain[0],
          bitchain.chain[1], bitchain.chain[2]);
  bit_set_position (&bitchain, 0);

  for (int j = 0; j < 5; j++)
    {
      for (int len = 0; len <= 7; len++)
        {
          // generate random number with len bytes
          BITCODE_BLL r = 0;
          for (int i = 0; i < len; i++)
            {
              r <<= 8;
              r |= (uint8_t)(rand () & 0xFF);
            }

          // 0xc669 => 010 + 0xc669
          bit_write_BLL (&bitchain, r);
          bit_set_position (&bitchain, 0);
          result = bit_read_BLL (&bitchain);
          if (result == r)
            pass ();
          else
            fail ("bit_read_BLL len=%d " FORMAT_RLL " => " FORMAT_RLL, len, r,
                  result);
          bit_set_position (&bitchain, 0);
        }
    }
  bitfree (&bitchain);
}

static void
bit_read_RC_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111");
  unsigned char result = bit_read_RC (&bitchain);
  if (result == 0xFF)
    pass ();
  else
    fail ("bit_read_RC");
  bitfree (&bitchain);
}

static void
bit_write_RC_tests (void)
{
  Bit_Chain bitchain = strtobt ("00000000");
  bit_write_RC (&bitchain, 0xFF);
  if (bitchain.chain[0] == 0xFF)
    pass ();
  else
    fail ("bit_write_RC");
  bitfree (&bitchain);
}

static void
bit_RS_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111"
                                "00000001");
  BITCODE_RS result = bit_read_RS (&bitchain);

  if (result == 511)
    pass ();
  else
    fail ("bit_write_RS");
  bitfree (&bitchain);

  bitchain = strtobt ("1111111111111111");
  bit_write_RS (&bitchain, 511);

  if (bitchain.byte == 2 && bitchain.chain[0] == 255 && bitchain.chain[1] == 1)
    pass ();
  else
    fail ("bit_write_RS %x %x", bitchain.chain[0], bitchain.chain[1]);

  bit_set_position (&bitchain, 0);
  result = bit_read_RS (&bitchain);
  if (result == 511)
    pass ();
  else
    fail ("bit_read_RS %u", result);
  bitfree (&bitchain);
}

static void
bit_RS_BE_tests (void)
{
  Bit_Chain bitchain = strtobt ("00000001"
                                "11111111");
  BITCODE_RS result = bit_read_RS_BE (&bitchain);

  if (result == 511)
    pass ();
  else
    fail ("bit_read_RS_BE %u", result);
  bitfree (&bitchain);

  bitchain = strtobt ("1111111111111111");
  bit_write_RS_BE (&bitchain, 511);
  if (bitchain.byte == 2 && bitchain.chain[0] == 1 && bitchain.chain[1] == 255)
    pass ();
  else
    fail ("bit_write_RS_LE %x %x", bitchain.chain[0], bitchain.chain[1]);

  bit_set_position (&bitchain, 0);
  result = bit_read_RS_BE (&bitchain);
  if (result == 511)
    pass ();
  else
    fail ("bit_write_RS_BE %u", result);
  bitfree (&bitchain);
}

static void
bit_BS_tests (void)
{
  Bit_Chain bitchain;
  // special cases >256, 0, 256, 1-255
  const BITCODE_BS values[] = { 1024, 0, 256, 2, 1 };
  const size_t sizes[] = { 2 + 16, 2, 2, 2 + 8, 2 + 8 };
  BITCODE_BS bs;

  assert (ARRAY_SIZE (values) == ARRAY_SIZE (sizes));
  bitprepare (&bitchain, 6);
  for (int i = 0; i <= 1; i++)
    {
      for (int j = 0; j < ARRAY_SIZE (values); j++)
        {
          size_t pos;
          bs = values[j];
          bit_set_position (&bitchain, i);
          bit_write_BS (&bitchain, bs); // 00 + RS
          pos = bit_position (&bitchain);
          if (pos == sizes[j] + i)
            pass ();
          else
            {
              bit_set_position (&bitchain, i);
              bit_print (&bitchain, 3);
              fail ("bit_write_BS (%u) @%" PRIuSIZE ".%u", (unsigned)bs,
                    bitchain.byte, bitchain.bit);
            }

          bit_set_position (&bitchain, i);
          if ((bs = bit_read_BS (&bitchain)) == values[j])
            pass ();
          else
            {
              bit_set_position (&bitchain, i);
              bit_print (&bitchain, 4);
              fail ("bit_read_BS %d", bs);
            }
        }
    }
  bitfree (&bitchain);
}

static void
bit_RL_tests (void)
{ /* 7f         f7         bf        7d */
  Bit_Chain bitchain = strtobt ("01111111"
                                "11110111"
                                "10111111"
                                "01111101");
  BITCODE_RL result = bit_read_RL (&bitchain);
  if (result == (BITCODE_RL)0x7DBFF77F)
    pass ();
  else
    fail ("bit_read_RL 0x%x", (unsigned)result);

  bit_set_position (&bitchain, 0);
  bit_write_RL (&bitchain, 0x8000ffff);
  if (bitchain.chain[0] == 0xff && bitchain.chain[1] == 0xff
      && bitchain.chain[2] == 0 && bitchain.chain[3] == 0x80)
    pass ();
  else
    fail ("bit_write_RL %x %x %x %x", bitchain.chain[0], bitchain.chain[1],
          bitchain.chain[2], bitchain.chain[3]);

  bit_set_position (&bitchain, 0);
  result = bit_read_RL (&bitchain);
  if (result == UINT64_C (0x8000ffff))
    pass ();
  else
    fail ("bit_read_RL 0x%08" PRIX32, result);

  bitfree (&bitchain);
}

#if 0
// MEASUREMENT section
static void
bit_RL_LE_tests (void)
{                               /* bf 7d 7f f7 */
  Bit_Chain bitchain = strtobt ("10111111" "01111101" "01111111" "11110111");
  BITCODE_RL result = bit_read_RL_LE (&bitchain);
  if (result == (BITCODE_RL)0x7DBFF77F)
    pass ();
  else
    fail ("bit_read_RL_LE 0x%x", (unsigned)result);

  bit_set_position (&bitchain, 0);
  bit_write_RL_LE (&bitchain, 0x8000ffff);
  if (bitchain.chain[3] == 255 && bitchain.chain[2] == 255
      && bitchain.chain[0] == 0 && bitchain.chain[1] == 128)
    pass ();
  else
    fail ("bit_write_RL_LE %u %u %u %u", bitchain.chain[0], bitchain.chain[1],
          bitchain.chain[2], bitchain.chain[3]);
  bitfree (&bitchain);
}
#endif

static void
bit_BL_tests (void)
{
  Bit_Chain bitchain;
  // special cases >255, 0, 1-255
  const BITCODE_BL values[] = { 1024, 0, 2, 1 };
  const size_t sizes[] = { 2 + 32, 2, 2 + 8, 2 + 8 };
  BITCODE_BL bl;

  assert (ARRAY_SIZE (values) == ARRAY_SIZE (sizes));
  bitprepare (&bitchain, 6);
  for (int i = 0; i <= 1; i++)
    {
      for (int j = 0; j < ARRAY_SIZE (values); j++)
        {
          size_t pos;
          bl = values[j];
          bit_set_position (&bitchain, i);
          bit_write_BL (&bitchain, bl);
          pos = bit_position (&bitchain);
          if (pos == sizes[j] + i)
            pass ();
          else
            {
              bit_set_position (&bitchain, i);
              bit_print (&bitchain, 5);
              fail ("bit_write_BL (%u) @%" PRIuSIZE ".%u", (unsigned)bl,
                    bitchain.byte, bitchain.bit);
            }

          bit_set_position (&bitchain, i);
          if ((bl = bit_read_BL (&bitchain)) == values[j])
            pass ();
          else
            {
              bit_set_position (&bitchain, i);
              bit_print (&bitchain, 5);
              fail ("bit_read_BL %d", bl);
            }
        }
    }
  bitfree (&bitchain);
}

// seperate branches for bit=0 and bit=1
static void
bit_RD_tests (void)
{
  Bit_Chain bitchain;
  const double values[] = { 0.0, 1.0, 25.21241 };
  double result;
  union
  {
    double d;
    uint64_t u;
  } u;

  bitprepare (&bitchain, 9);
  for (int i = 0; i <= 1; i++)
    {
      for (int j = 0; j < 3; j++)
        {
          size_t p1, p2;
          u.d = values[j];
          bit_set_position (&bitchain, i);
          bit_write_RD (&bitchain, u.d);
          if ((size_t)i + 64L == bit_position (&bitchain))
            pass ();
          else
            fail ("bit_write_RD bit=%d @%" PRIuSIZE ".%u", i, bitchain.byte,
                  bitchain.bit);
          bit_set_position (&bitchain, i);
          result = bit_read_RD (&bitchain);
          if (result == u.d)
            pass (); // ok ("bit_read_RD bit=%d %g", i, result);
          else
            fail ("bit_read_RD bit=%d %g != %g", i, result, u.d);
        }
    }
  bitfree (&bitchain);
}

// seperate branches for bit=0 and bit=1
static void
bit_BD_tests (void)
{
  Bit_Chain bitchain;
  const double values[] = { 0.0, 1.0, 25.21241 };
  double result;
  union
  {
    double d;
    uint64_t u;
  } u;

  bitprepare (&bitchain, 9);
  for (int i = 0; i <= 1; i++)
    {
      for (int j = 0; j < 3; j++)
        {
          u.d = values[j];
          bit_set_position (&bitchain, i);
          bit_write_BD (&bitchain, u.d);
          if (j < 2 || (size_t)i + 66L == bit_position (&bitchain))
            pass ();
          else
            fail ("bit_write_RD bit=%d @%" PRIuSIZE ".%u", i, bitchain.byte,
                  bitchain.bit);
          bit_set_position (&bitchain, i);
          result = bit_read_BD (&bitchain);
          if (result == u.d)
            pass (); // ok ("bit_read_BD bit=%d %g", i, result);
          else
            fail ("bit_read_BD bit=%d %g != %g", i, result, u.d);
        }
    }
  bitfree (&bitchain);

  bitchain = strtobt ("10");
  result = bit_read_BD (&bitchain);
  if (result == 0.0)
    pass (); // ok ("bit_read_BD (0.0)");
  else
    fail ("bit_read_BD %f (0.0)", result);
  bitfree (&bitchain);

  bitchain = strtobt ("01");
  result = bit_read_BD (&bitchain);
  if (result == 1.0)
    pass (); // ok ("bit_read_BD (1.0)");
  else
    fail ("bit_read_BD %f (1.0)", result);

  bit_set_position (&bitchain, 0);
  bit_write_BB (&bitchain, 0);
  bit_write_RD (&bitchain, 1.2345);
#ifdef WORDS_BIGENDIAN
  bit_set_position (&bitchain, 0);
  bit_print (&bitchain, 9);
#endif
  bit_set_position (&bitchain, 0);
  result = bit_read_BD (&bitchain);
  if (result == 1.2345)
    pass (); // ok ("bit_read_BD (1.2345)");
  else
    {
      fail ("bit_read_BD %f (1.2345)", result);
      bit_set_position (&bitchain, 0);
      bit_print (&bitchain, 9);
    }

  bit_set_position (&bitchain, 0);
  bit_write_BD (&bitchain, 0.0);
  if (bitchain.byte == 0 && bitchain.bit == 2)
    pass (); // ok ("bit_write_BD (0.0)");
  else
    fail ("bit_write_BD (0.0)");

  bit_set_position (&bitchain, 0);
  bit_write_BD (&bitchain, 1.0);
  if (bitchain.byte == 0 && bitchain.bit == 2)
    pass (); // ok ("bit_write_BD (1.0)");
  else
    fail ("bit_write_BD (1.0)");

  bit_set_position (&bitchain, 0);
  bit_write_BD (&bitchain, 1.2345);
  if (bitchain.byte == 8 && bitchain.bit == 2)
    pass (); // ok ("bit_write_BD (1.2345)");
  else
    fail ("bit_write_BD (1.2345)");
#ifdef WORDS_BIGENDIAN
  bit_set_position (&bitchain, 0);
  bit_print (&bitchain, 9);
#endif

  bitfree (&bitchain);
}

static void
bit_RLL_tests (void)
{
  Bit_Chain bitchain = strtobt ("01111111"
                                "11110111"
                                "10111111"
                                "01111101"
                                "00000000"
                                "00000000"
                                "00000000"
                                "00000001");
  BITCODE_RLL result = bit_read_RLL (&bitchain);
  if (result == UINT64_C (0x010000007DBFF77F))
    pass ();
  else
    fail ("bit_read_RLL 0x%016" PRIX64, result);

  bit_set_position (&bitchain, 0);
  bit_write_RLL (&bitchain, UINT64_C (0x8000ffff00000001));
  if (memcmp (bitchain.chain, "\x01\x00\x00\x00\xff\xff\x00\x80", 8) == 0)
    pass ();
  else
    fail ("bit_write_RLL %x %x %x %x %x %x %x %x", bitchain.chain[0],
          bitchain.chain[1], bitchain.chain[2], bitchain.chain[3],
          bitchain.chain[4], bitchain.chain[5], bitchain.chain[6],
          bitchain.chain[7]);

  bit_set_position (&bitchain, 0);
  result = bit_read_RLL (&bitchain);
  if (result == UINT64_C (0x8000ffff00000001))
    pass ();
  else
    fail ("bit_read_RLL 0x%016" PRIX64, result);
  bitfree (&bitchain);
  // ----------------------------------------------------------------
  bitchain = strtobt ("0"
                      "01111111"
                      "11110111"
                      "10111111"
                      "01111101"
                      "00000000"
                      "00000000"
                      "00000000"
                      "00000001");
  bit_set_position (&bitchain, 1);
  result = bit_read_RLL (&bitchain);
  if (result == UINT64_C (0x010000007DBFF77F))
    pass ();
  else
    fail ("bit_read_RLL bit=1 0x%016" PRIX64, result);

  bit_set_position (&bitchain, 1);
  bit_write_RLL (&bitchain, UINT64_C (0x8000ffff00000001));
  if (memcmp (bitchain.chain, "\x00\x80\x00\x00\x7f\xff\x80\x40", 8) == 0)
    pass ();
  else
    fail ("bit_write_RLL bit=1 %x %x %x %x %x %x %x %x", bitchain.chain[0],
          bitchain.chain[1], bitchain.chain[2], bitchain.chain[3],
          bitchain.chain[4], bitchain.chain[5], bitchain.chain[6],
          bitchain.chain[7]);

  bit_set_position (&bitchain, 1);
  result = bit_read_RLL (&bitchain);
  if (result == UINT64_C (0x8000ffff00000001))
    pass ();
  else
    fail ("bit_read_RLL bit=1 0x%016" PRIX64, result);
  bitfree (&bitchain);
}

static void
bit_RLL_BE_tests (void)
{ /*                             0x7f 0xf7 0xbf 0x7d 0x00 0x00 0x00 0x01 */
  Bit_Chain bitchain = strtobt ("01111111"
                                "11110111"
                                "10111111"
                                "01111101"
                                "00000000"
                                "00000000"
                                "00000000"
                                "00000001");
  BITCODE_RLL result = bit_read_RLL_BE (&bitchain);
  if (result == UINT64_C (0x7FF7BF7D00000001))
    pass ();
  else
    fail ("bit_read_RLL_BE 0x%016" PRIX64, result);

  bit_set_position (&bitchain, 0);
  bit_write_RLL_BE (&bitchain, UINT64_C (0x8000ffff00000001));
  if (memcmp (bitchain.chain, "\x80\x00\xff\xff\x00\x00\x00\x01", 8) == 0)
    pass ();
  else
    fail ("bit_write_RLL_BE %x %x %x %x %x %x %x %x", bitchain.chain[0],
          bitchain.chain[1], bitchain.chain[2], bitchain.chain[3],
          bitchain.chain[4], bitchain.chain[5], bitchain.chain[6],
          bitchain.chain[7]);

  bit_set_position (&bitchain, 0);
  result = bit_read_RLL_BE (&bitchain);
  if (result == UINT64_C (0x8000ffff00000001))
    pass ();
  else
    fail ("bit_read_RLL_BE 0x%016" PRIX64, result);
  bitfree (&bitchain);

  // ----------------------------------------------------------------
  bitchain = strtobt ("0"
                      "01111111"
                      "11110111"
                      "10111111"
                      "01111101"
                      "00000000"
                      "00000000"
                      "00000000"
                      "00000001");
  bit_set_position (&bitchain, 1);
  result = bit_read_RLL_BE (&bitchain);
  if (result == UINT64_C (0x7FF7BF7D00000001))
    pass ();
  else
    fail ("bit_read_RLL_BE bit=1 0x%016" PRIX64, result);

  bit_set_position (&bitchain, 1);
  bit_write_RLL_BE (&bitchain, UINT64_C (0x8000ffff00000001));
  if (memcmp (bitchain.chain, "\x40\x00\x7F\xFF\x80\x00\x00\x00", 8) == 0)
    pass ();
  else
    fail ("bit_write_RLL_BE bit=1 %x %x %x %x %x %x %x %x", bitchain.chain[0],
          bitchain.chain[1], bitchain.chain[2], bitchain.chain[3],
          bitchain.chain[4], bitchain.chain[5], bitchain.chain[6],
          bitchain.chain[7]);

  bit_set_position (&bitchain, 1);
  result = bit_read_RLL_BE (&bitchain);
  if (result == UINT64_C (0x8000ffff00000001))
    pass ();
  else
    fail ("bit_read_RLL_BE bit=1 0x%016" PRIX64, result);
  bitfree (&bitchain);
}

static void
bit_read_H_tests (void)
{
  // #if defined(__MINGW64_VERSION_MAJOR) && defined(__GNUC__) && __GNUC__ >= 9
  //   return;
  // #endif
  Bit_Chain bitchain;
  Dwg_Handle result;
  int ret;

#define test_H_r_case(s, dwg_ver, r, c, si, v)                                \
  bitchain = strtobt (s);                                                     \
  bitchain.version = dwg_ver;                                                 \
  ret = bit_read_H (&bitchain, &result);                                      \
  if (ret == r && result.code == c && result.size == si && result.value == v) \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      fail ("bit_read_H: %s (result " FORMAT_H ")", s, ARGS_H (result));      \
      /*bit_print (&bitchain, sizeof (Dwg_Handle)); */                        \
    }                                                                         \
  bitfree (&bitchain)

  //              code   size   value
  test_H_r_case ("0100"
                 "0001"
                 "00000101",
                 R_14, 0, 4, 1, 5);
  test_H_r_case ("1100"
                 "0001"
                 "00001011",
                 R_14, 0, 12, 1, 11);
  test_H_r_case ("0100"
                 "0001"
                 "00001100",
                 R_14, 0, 4, 1, 12);
  test_H_r_case ("0011"
                 "0000",
                 R_14, 0, 3, 0, 0);
  test_H_r_case ("0101"
                 "0000",
                 R_14, 0, 5, 0, 0);
  test_H_r_case ("0010"
                 "0001"
                 "00011000",
                 R_14, 0, 2, 1, 24);
  test_H_r_case ("0000"
                 "0001"
                 "00000001",
                 R_14, 0, 0, 1, 1);
  test_H_r_case ("0010"
                 "0010"
                 "00000010"
                 "00001010",
                 R_14, 0, 2, 2, 522);
  test_H_r_case ("0101"
                 "0001"
                 "01011110",
                 R_14, 0, 5, 1, 94);
  test_H_r_case ("0010"
                 "0001"
                 "01100100",
                 R_14, 0, 2, 1, 100);
  // preR13
  //              size       value
  test_H_r_case ("00000001"
                 "00000010",
                 R_11, 0, 0, 1, 2);
  test_H_r_case ("00000010"
                 "00000010"
                 "00001010",
                 R_11, 0, 0, 2, 522);
}

static void
bit_write_H_tests (void)
{
  Bit_Chain bitchain;
  size_t byte;
  Dwg_Handle handle;

#define test_H_w_case(c, si, v, dwg_ver)                                      \
  handle.code = c;                                                            \
  handle.size = si;                                                           \
  handle.value = v;                                                           \
  byte = 1 + handle.size;                                                     \
  bitprepare (&bitchain, sizeof (Dwg_Handle));                                \
  bitchain.version = dwg_ver;                                                 \
  bit_write_H (&bitchain, &handle);                                           \
  if (bitchain.byte == byte && bitchain.bit == 0)                             \
    pass ();                                                                  \
  else                                                                        \
    {                                                                         \
      bit_print (&bitchain, sizeof (Dwg_Handle));                             \
      fail ("bit_write_H (" FORMAT_H ") @%" PRIuSIZE ".%u", ARGS_H (handle),  \
            bitchain.byte, bitchain.bit);                                     \
    }                                                                         \
  bitfree (&bitchain)

  test_H_w_case (4, 1, 5, R_14);
  test_H_w_case (12, 1, 11, R_14);
  test_H_w_case (4, 1, 12, R_14);
  test_H_w_case (3, 0, 0, R_14);
  test_H_w_case (5, 0, 0, R_14);
  test_H_w_case (2, 1, 24, R_14);
  test_H_w_case (0, 1, 1, R_14);
  test_H_w_case (2, 2, 522, R_14);
  test_H_w_case (5, 1, 94, R_14);
  test_H_w_case (2, 1, 100, R_14);
  // preR13
  test_H_w_case (0, 1, 2, R_11);
  test_H_w_case (0, 2, 522, R_11);
}

static void
bit_UMC_bug_tests (void)
{
  BITCODE_UMC umc;
  BITCODE_MC mc;
  Bit_Chain bitchain;
  bitprepare (&bitchain, 6);

  bit_write_TF (&bitchain, (BITCODE_TF) "\x01\xc6\x00", 3);
  bitchain.byte = 0;
  if ((umc = bit_read_UMC (&bitchain)) == 1UL)
    {
      ok ("bit_read_UMC 1");
      if ((mc = bit_read_MC (&bitchain)) == 70)
        ok ("bit_read_MC 70");
      else
        {
          fail ("bit_read_MC " FORMAT_MC " != 70", mc);
          bitchain.byte = 0;
          bit_print (&bitchain, 3);
        }
    }
  else
    {
      fail ("bit_read_UMC " FORMAT_UMC " != 1", umc);
      bitchain.byte = 0;
      bit_print (&bitchain, 6);
    }

  bitchain.byte = 0;
  // UMC bug GH #662 (also #386, #126)
  bit_write_TF (&bitchain, (BITCODE_TF) "\xd2\xec\xa9\xf2\x92\xa2\x01", 7);
  bitchain.byte = 0;
  if ((umc = bit_read_UMC (&bitchain)) == 5571349214802UL) // 0x5112E4A7652
    {
      ok ("bit_read_UMC 0x5112E4A7652");
    }
  else
    {
      fail ("bit_read_UMC " FORMAT_UMC " != 0x5112E4A7652", umc);
      bitchain.byte = 0;
      bit_print (&bitchain, 6);
    }

  bitfree (&bitchain);
}

static void
bit_utf8_to_TV_tests (void)
{
  char dest[128];
  char *p;
  const char *src1 = "TestË\\\"END"; // \xc3\x8b
  const char *src2 =
#ifndef _MSC_VER
      "Test\u0234\"END"; // Latin Small Letter L with Curl, not in any codepage
#else
      "Test\xc8\xb4\"END";
#endif

  p = bit_utf8_to_TV (dest, (const unsigned char *)src1, sizeof (dest),
                      strlen (src1), 0, CP_ISO_8859_1);
  // Ë can really be represented in ISO_8859_1,
  // so no conversion to \U+XXXX needed. just convert from utf-8 to latin-1
  if (strEQc (p, "Test\xcb\\\"END"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s as ISO_8859_1", p);
  // cquoted
  p = bit_utf8_to_TV (dest, (const unsigned char *)src1, sizeof (dest),
                      strlen (src1), 1, CP_ISO_8859_1);
  if (strEQc (p, "Test\xcb\"END"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s cquoted", p);

  p = bit_utf8_to_TV (dest, (const unsigned char *)src1, sizeof (dest),
                      strlen (src1), 0, CP_ISO_8859_7);
  // But Ë (U+00CB) can not be represented in ISO_8859_7
  if (strEQc (p, "Test\\U+00CB\\\"END"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s as ISO_8859_7", p);

  // \u0234 must be converted to \U+0234 in any codepage
  p = bit_utf8_to_TV (dest, (const unsigned char *)src2, sizeof (dest),
                      strlen (src1), 0, CP_ISO_8859_1);
  if (strEQc (p, "Test\\U+0234\"END"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s => %s ISO_8859_1", src2, p);

  // asian double-byte codepages
  p = bit_utf8_to_TV (dest, (const unsigned char *)src1, sizeof (dest),
                      strlen (src1), 0, CP_CP949);
  // And Ë (U+00CB) can not be represented in CP949
  if (strEQc (p, "Test\\U+00CB\\\"END"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s as CP949", p);
  p = bit_utf8_to_TV (dest, (const unsigned char *)"시험", sizeof (dest),
                      strlen ("시험"), 0, CP_CP949);
  if (strEQc (p, "\xdc\xc3\x7e\xe8"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s as CP949", p);
  p = bit_utf8_to_TV (dest, (const unsigned char *)"█", sizeof (dest),
                      strlen ("█"), 0, CP_CP869);
  if (strEQc (p, "\xdb"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s as CP869 (DB %2X)", p, (unsigned)*p);
  p = bit_utf8_to_TV (dest, (const unsigned char *)"δ", sizeof (dest),
                      strlen ("δ"), 0, CP_CP869);
  if (strEQc (p, "\xdd"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s as CP869 (DD %2X)", p, (unsigned)*p);
}

static void
bit_utf8_to_TU_tests (void)
{
  BITCODE_TU p;

  p = bit_utf8_to_TU ((char *)"Ë", 0); // "\xc3\x8b"
  if (*p == 0xCB && !p[1])
    pass ();
  else
    fail ("bit_utf8_to_TU U+00CB => %2X%2X", p[0], p[1]);
  free (p);

  p = bit_utf8_to_TU ((char *)"█", 0); // \xe2\x96\x88
  if (*p == 0x2588 && !p[1])
    pass ();
  else
    fail ("bit_utf8_to_TU U+2588 => %2X%2X", p[0], p[1]);
  free (p);

  p = bit_utf8_to_TU ((char *)"δ", 0); // "\xce\xb4"
  if (*p == 0x03B4 && !p[1])
    pass ();
  else
    fail ("bit_utf8_to_TU U+03B4 => %2X%2X", p[0], p[1]);
  free (p);
}

static void
bit_TV_to_utf8_tests (void)
{
  char *p;
  char *srcu = strdup ("Test\\U+0234"); // ȴ
  const char *src1 = "Test\xc4";        // Ä
  const char *src2 = "Test\xc6";        // Ć \U+0106
  const char *src7 = "Test\xd3";        // Σ
  // echo -n "시험" | iconv -f utf8 -t cp949 | od -t x1
  char *src_kor = strdup ("\xbd\xc3\xc7\xe8");

  ok ("bit_TV_to_utf8_tests init");
  p = bit_TV_to_utf8 ((char *)srcu, CP_ISO_8859_1);
  if (strEQc (p, "Test\xc8\xb4"))
    ok ("bit_TV_to_utf8_tests 8859-1");
  else
    {
      // on ppc64 it was U+3402	㐂 instead of U+0234
      fail ("bit_TV_to_utf8 %s ISO_8859_1, not Testȴ", p);
      for (size_t i = 0; i < strlen (p); i++)
        printf ("\\x%02x", (unsigned char)p[i]);
      printf ("\n");
    }
  if (p != srcu)
    free (p);
  free (srcu);

  p = bit_TV_to_utf8 ((char *)src1, CP_ISO_8859_1);
  if (strEQc (p, "TestÄ")) // \xc3\x84 as utf-8
    ok ("bit_TV_to_utf8_tests 8859-1");
  else
    fail ("bit_TV_to_utf8 %s ISO_8859_1", p);
  if (p != src1)
    free (p);

  p = bit_TV_to_utf8 ((char *)src2, CP_ISO_8859_2);
  if (strEQc (p, "TestĆ"))
    ok ("bit_TV_to_utf8_tests 8859-2");
  else
    fail ("bit_TV_to_utf8 %s ISO_8859_2", p);
  if (p != src2)
    free (p);

  p = bit_TV_to_utf8 ((char *)src7, CP_ISO_8859_7);
  if (strEQc (p, "TestΣ"))
    ok ("bit_TV_to_utf8_tests 8859-7");
  else
    fail ("bit_TV_to_utf8 %s ISO_8859_7", p);
  if (p != src7)
    free (p);

  p = bit_TV_to_utf8 (src_kor, CP_CP949);
  // echo "시험" | od -t x1
  // U+feff U+c2dc U+d5d8
  if (strEQc (p, "\xec\x8b\x9c\xed\x97\x98")
      || strEQc (p, "\xc2\xbd\xc3\x83\xc3\x87\xc3\xa8")) // macOS
    ok ("bit_TV_to_utf8_tests CP949");
  else
    {
      fail ("bit_TV_to_utf8 %s CP_CP949 (len=%" PRIuSIZE ")", p, strlen (p));
      for (size_t i = 0; i < strlen (p); i++)
        printf ("\\x%02x", (unsigned char)p[i]);
      printf ("\n");
    }
  if (p != src_kor)
    free (p);
  free (src_kor);

  p = bit_TV_to_utf8 ("\x83\x82\x83\x6d", CP_ANSI_932);
  // echo "モノ" | od -t x1
  if (strEQc (p, "モノ") || strEQc (p, "\xe3\x83\xa2\xe3\x83\x8e"))
    ok ("bit_TV_to_utf8_tests 932");
  else
    {
      fail ("bit_TV_to_utf8 %s 932 (len=%" PRIuSIZE ")", p, strlen (p));
      for (size_t i = 0; i < strlen (p); i++)
        printf ("\\x%02x", (unsigned char)p[i]);
      printf ("\n");
    }
  free (p);

  p = bit_TV_to_utf8 ("0\\M+18382", CP_ANSI_932); // MO
  if (strEQc (p, "0モ") || strEQc (p, "0\xe3\x83\xa2"))
    ok ("bit_TV_to_utf8_tests MIF-1 932");
  else
    {
      fail ("bit_TV_to_utf8 %s MIF-1 932 (len=%" PRIuSIZE ")", p, strlen (p));
      for (size_t i = 0; i < strlen (p); i++)
        printf ("\\x%02x", (unsigned char)p[i]);
      printf ("\n");
    }
  free (p);
}

static void
bit_read_TV_tests (void)
{
  char *result;
  Bit_Chain bitchain = strtobt ("00000011"   // 03 (3)
                                "00000000"   // 00
                                "01000111"   // 47 (G)
                                "01001110"   // 4E (N)
                                "01010101"); // 55 (U)
  bitchain.from_version = R_11;
  result = (char *)bit_read_TV (&bitchain);
  if (!strcmp (result, "GNU"))
    ok ("bit_read_TV (<R_13)");
  else
    fail ("bit_read_TV (<R_13): %s", result);
  free (result);
  bitfree (&bitchain);

  bitchain = strtobt ("01"         // BB (1)
                      "00000100"   // RC (3)
                      "01000111"   // 47 (G)
                      "01001110"   // 4E (N)
                      "01010101"   // 55 (U)
                      "00000000"); // 00 (\0)
  bitchain.from_version = R_13;
  result = (char *)bit_read_TV (&bitchain);
  if (!strcmp (result, "GNU"))
    ok ("bit_read_TV (>R_13)");
  else
    fail ("bit_read_TV (>R_13): %s", result);
  free (result);
  bitfree (&bitchain);
}

static void
bit_write_TV_tests (void)
{
  Bit_Chain bitchain;
  bitprepare (&bitchain, 6);
  bitchain.from_version = R_11;
  bit_write_TV (&bitchain, (char *)"GNU");
  if (bitchain.byte == 5 && bitchain.bit == 0)
    ok ("bit_write_TV (<R_13)");
  else
    fail ("bit_write_TV @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, 0);
  bitchain.from_version = R_13;
  bit_write_TV (&bitchain, (char *)"GNU");
  if (bitchain.byte == 5 && bitchain.bit == 2)
    ok ("bit_write_TV (>R_13)");
  else
    fail ("bit_write_TV @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

  bitfree (&bitchain);
}

static void
bit_read_TF_tests (void)
{
  Bit_Chain bitchain = strtobt ("01000111"   // 47 (G)
                                "01001110"   // 4E (N)
                                "01010101"); // 55 (U)
  char *result = (char *)bit_read_TF (&bitchain, 3);
  if (!strcmp (result, "GNU"))
    ok ("bit_read_TF");
  else
    fail ("bit_read_TF");

  free (result);
  bitfree (&bitchain);
}

static void
bit_write_TF_tests (void)
{
  Bit_Chain bitchain;
  bitprepare (&bitchain, 3);
  bit_write_TF (&bitchain, (BITCODE_TF) "GNU", 3);
  if (bitchain.byte == 3 && bitchain.bit == 0)
    ok ("bit_write_TF");
  else
    fail ("bit_write_TF @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

  bitfree (&bitchain);
}

static void
bit_read_BE_tests (void)
{
  double x, y, z;
  Bit_Chain bitchain;

  bitchain = strtobt ("1");
  bitchain.from_version = R_2000;
  bit_read_BE (&bitchain, &x, &y, &z);
  if (x == 0.0 && y == 0.0 && z == 1.0)
    ok ("bit_read_BE (0.0 0.0 1.1) R_2000");
  else
    fail ("bit_read_BE (%f,%f,%f) R_2000", x, y, z);
  bitfree (&bitchain);

  bitprepare (&bitchain, 25);
  bitchain.from_version = R_2000;
  bit_write_B (&bitchain, 0);
  bit_write_BD (&bitchain, 20.2535);
  bit_write_BD (&bitchain, 10.2523);
  bit_write_BD (&bitchain, 52.32563);
  bit_set_position (&bitchain, 0);
  bit_read_BE (&bitchain, &x, &y, &z);
  if (x == 20.2535 && y == 10.2523 && z == 52.32563)
    ok ("bit_read_BE (20.2535 10.2523 52.32563) R_2000");
  else
    fail ("bit_read_BE (%f,%f,%f) R_2000", x, y, z);
  bitfree (&bitchain);

  bitchain = strtobt ("10100100");
  bitchain.from_version = R_14;
  bit_read_BE (&bitchain, &x, &y, &z);
  if (x == 0.0 && y == 0.0 && z == 1.0)
    ok ("bit_read_BE (0.0 0.0 1.1) R_14");
  else
    fail ("bit_read_BE (%f,%f,%f) R_14", x, y, z);
  bitfree (&bitchain);

  bitprepare (&bitchain, 25);
  bitchain.from_version = R_14;
  bit_write_BD (&bitchain, 20.2535);
  bit_write_BD (&bitchain, 10.2523);
  bit_write_BD (&bitchain, 52.32563);
  bit_set_position (&bitchain, 0);
  bit_read_BE (&bitchain, &x, &y, &z);
  if (x == 20.2535 && y == 10.2523 && z == 52.32563)
    ok ("bit_read_BE (20.2535 10.2523 52.32563) R_14");
  else
    fail ("bit_read_BE (%f,%f,%f) R_14", x, y, z);

  bitfree (&bitchain);
}

static void
bit_write_BE_tests (void)
{
  Bit_Chain bitchain;

  bitprepare (&bitchain, 25);
  bitchain.version = R_2000;
  bit_write_BE (&bitchain, 0.0, 0.0, 1.0);
  if (bitchain.byte == 0 && bitchain.bit == 1)
    ok ("bit_write_BE (0.0 0.0 1.1) R_2000");
  else
    fail ("bit_write_BE @%" PRIuSIZE ".%u R_2000", bitchain.byte,
          bitchain.bit);

  bit_set_position (&bitchain, 0);
  bit_write_BE (&bitchain, 20.2535, 10.2523, 52.32563);
  if (bitchain.byte == 24 && bitchain.bit == 7)
    ok ("bit_write_BE (20.2535 10.2523 52.32563) R_2000");
  else
    fail ("bit_write_BE @%" PRIuSIZE ".%u R_2000", bitchain.byte,
          bitchain.bit);

  bit_set_position (&bitchain, 0);
  bitchain.version = R_14;
  bit_write_BE (&bitchain, 0.0, 0.0, 1.0);
  if (bitchain.byte == 0 && bitchain.bit == 6)
    ok ("bit_write_BE (0.0 0.0 1.1) R_14");
  else
    fail ("bit_write_BE @%" PRIuSIZE ".%u R_14", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, 0);
  bitchain.version = R_14;
  bit_write_BE (&bitchain, 20.2535, 10.2523, 52.32563);
  if (bitchain.byte == 24 && bitchain.bit == 6)
    ok ("bit_write_BE (20.2535 10.2523 52.32563) R_14");
  else
    fail ("bit_write_BE @%" PRIuSIZE ".%u R_14", bitchain.byte, bitchain.bit);

  bitfree (&bitchain);
}

static void
bit_read_CMC_tests (void)
{
  Dwg_Color color;
  unsigned int result;
  Bit_Chain bitchain;

  /* pre R_13 version. */
  bitchain = strtobt ("1111111100000001"); // RS
  bitchain.from_version = bitchain.version = R_11;
  result = bit_read_CMC (&bitchain, &bitchain, &color);
  if (result == 0 && color.index == 511)
    ok ("bit_read_CMC (<R_13)");
  else
    fail ("bit_read_CMC: index=%d (<R_13)", color.index);
  bitfree (&bitchain);

  /* pre R_2004 version. */
  bitchain = strtobt ("00"                 // BB
                      "1111111100000001"); // index RS
  bitchain.from_version = bitchain.version = R_2000;
  result = bit_read_CMC (&bitchain, &bitchain, &color);
  if (result == 0 && color.index == 511)
    ok ("bit_read_CMC (<R_2004)");
  else
    fail ("bit_read_CMC: index=%d (<R_2004)", color.index);
  bitfree (&bitchain);

  /* R_2004 version. */
  bitchain = strtobt ("10" // index BB
                      "00" // rgb BB
                      // B       G          R          method
                      "00000000"
                      "00000000"
                      "00000000"
                      "11000011"   // rgb RL
                      "00000000"); // flag RC
  bitchain.from_version = bitchain.version = R_2004;
  result = bit_read_CMC (&bitchain, &bitchain, &color);
  if (result == 0 && color.index == 0 && color.rgb == 0xc3000000
      && color.method == 195 && color.flag == 0)

    ok ("bit_read_CMC (R_2004 - white)");
  else
    fail ("bit_read_CMC: index=%d, flag=%d, method=0x%0x, rgb=0x%08x (R_2004 "
          "- white)",
          color.index, color.flag, color.method, color.rgb);
  bitfree (&bitchain);

  /* R_2004 version. */
  bitchain = strtobt ("10" // index BB
                      "00" // rgb BB
                      // B       G          R          method
                      "00000000"
                      "00000000"
                      "11111111"
                      "11000011"   // rgb RL
                      "00000000"); // flag RC
  bitchain.from_version = bitchain.version = R_2004;
  result = bit_read_CMC (&bitchain, &bitchain, &color);
  if (result == 0 && color.index == 1 && color.rgb == 0xc3ff0000
      && color.method == 195 && color.flag == 0)

    ok ("bit_read_CMC (R_2004 - red)");
  else
    fail ("bit_read_CMC: index=%d, flag=%d, method=0x%0x, rgb=0x%08x (R_2004 "
          "- red)",
          color.index, color.flag, color.method, color.rgb);
  bitfree (&bitchain);
}

static void
bit_read_MS_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111" "01111111");
  unsigned int result = bit_read_MS (&bitchain);
  if (result == 32767)
    ok ("bit_read_MS - 32767");
  else
    fail ("bit_read_MS");
  bitfree (&bitchain);

  bitchain = strtobt ("11111111" "11111111" "11111111" "01111111");
  result = bit_read_MS (&bitchain);
  if (result == 1073741823)
    ok ("bit_read_MS - 1073741823");
  else
    fail ("bit_read_MS");
  bitfree (&bitchain);
}

static void
bit_read_UMC_tests (void)
{
  Bit_Chain bitchain;
  BITCODE_UMC umc;

#define test_UMC(s, x)                                                        \
  bitchain = strtobt (s);                                                     \
  umc = bit_read_UMC (&bitchain);                                             \
  if (umc == x)                                                               \
    ok ("bit_read_UMC - " FORMAT_UMC, (BITCODE_UMC)x);                        \
  else                                                                        \
    fail ("bit_read_UMC " FORMAT_UMC " != " FORMAT_UMC, umc, (BITCODE_UMC)x); \
  bitfree (&bitchain)

  test_UMC("00000000", 0x0);
  test_UMC("01111111", 0x7F);
  test_UMC("11100101" "10001110" "00100110", 0x98765);
  test_UMC("11010010" "11101100" "10101001" "11110010" "10010010" "10100010" "00000001", 0x5112E4A7652);
  test_UMC("11111111" "11111111" "11111111" "11111111" "11111111" "11111111" "11111111" "01111111", 0xFFFFFFFFFFFFFF);
}

static void
bit_read_MC_tests (void)
{
  Bit_Chain bitchain;
  BITCODE_MC mc;

#define test_MC(s, x)                                                     \
  bitchain = strtobt (s);                                                 \
  mc = bit_read_MC (&bitchain);                                           \
  if (mc == x)                                                            \
    ok ("bit_read_MC - %d", (BITCODE_MC)x);                               \
  else                                                                    \
    fail ("bit_read_MC " FORMAT_MC " != " FORMAT_MC, mc, (BITCODE_MC)x);  \
  bitfree (&bitchain)

  test_MC("00000000", 0);
  test_MC("00111111", 63);
  test_MC("01111111", -63);
  test_MC("11111111" "00000000", 127);
  test_MC("11111111" "01000000", -127);
  test_MC("11111111" "00111111", 8191);
  test_MC("11111111" "01111111", -8191);
  test_MC("11111111" "11111111" "00000000", 16383);
  test_MC("11111111" "11111111" "01000000", -16383);
  test_MC("11000000" "10111011" "01111000", -925120);
  test_MC("11111111" "11111111" "11111111" "00111111", 134217727);
  test_MC("11111111" "11111111" "11111111" "01111111", -134217727);
  test_MC("11111111" "11111111" "11111111" "11111111", 268435455);
}

static void
in_hexbin_tests (void)
{
  Bit_Chain dat = { 0 };
  char hex[] = "0921FFA02921FF302821FF302821FF302821FF30";
  const unsigned char result[]
      = { 0x09, 0x21, 0xFF, 0xA0, 0x29, 0x21, 0xFF, 0x30, 0x28, 0x21,
          0xFF, 0x30, 0x28, 0x21, 0xFF, 0x30, 0x28, 0x21, 0xFF, 0x30 };
  size_t written;
  dat.size = ((sizeof (hex) - 1) / 2);
  dat.chain = calloc (dat.size + 1, 1);
  written = in_hex2bin (dat.chain, hex, dat.size);
  if (written == dat.size && memcmp (dat.chain, result, dat.size) == 0)
    {
      ok ("in_hexbin");
    }
  else
    {
      fail ("in_hexbin \"%.*s\" %" PRIuSIZE " (%" PRIuSIZE ")", (int)written,
            dat.chain, written, dat.size);
      for (size_t i = 0; i < dat.size; i++)
        {
          unsigned char c = bit_read_RC (&dat);
          fprintf (stderr, "0x%02x", c);
        }
      fputs ("", stderr);
    }
  // all valid hex bytes
  memset (dat.chain, 0, dat.size);
  dat.size = 1;
  hex[2] = '\0';
  for (unsigned char c = '0'; c <= '9'; c++)
    {
      hex[0] = (char)c;
      for (unsigned char d = '0'; d <= '9'; d++)
        {
          hex[1] = (char)d;
          if (1 != in_hex2bin (dat.chain, hex, 1)
              || dat.chain[0] != (unsigned char)(((c - '0') << 4) + (d - '0')))
            fail ("in_hexbin \"%c%c\" => %02X", c, d, dat.chain[0]);
        }
    }
  for (unsigned char c = 'A'; c <= 'F'; c++)
    {
      hex[0] = (char)c;
      for (unsigned char d = 'A'; d <= 'F'; d++)
        {
          hex[1] = (char)d;
          if (1 != in_hex2bin (dat.chain, hex, 1)
              || dat.chain[0]
                     != (unsigned char)(((c - 'A' + 10) << 4)
                                        + (d - 'A' + 10)))
            fail ("in_hexbin \"%c%c\" => %02X", c, d, dat.chain[0]);
        }
    }
    // only with DEBUG we can detect wrong chars
#ifndef NDEBUG
  hex[0] = 'g';
  fprintf (stderr,
           "NOTE: ignore the next ERROR Invalid hex string member gF:\n");
  written = in_hex2bin (dat.chain, hex, dat.size);
  if (written == 0)
    ok ("in_hexbin error (ignore the ERROR above)");
  else
    fail ("in_hexbin error");
#endif

  free (dat.chain);
}

int
main (int argc, char const *argv[])
{

  Bit_Chain bitchain;
  BITCODE_BD dbl;
  BITCODE_RLL rll;
  BITCODE_BL bl;
  BITCODE_BS bs;
  BITCODE_MC mc;
  BITCODE_UMC umc;
  size_t pos, size;
  char *str;
  unsigned char sentinel[]
      = { 0xCF, 0x7B, 0x1F, 0x23, 0xFD, 0xDE, 0x38, 0xA9,
          0x5F, 0x7C, 0x68, 0xB8, 0x4E, 0x6D, 0x33, 0x5F };
  loglevel = loglevel_from_env ();

  bit_read_B_tests ();
  bit_write_B_tests ();
  bit_advance_position_tests ();
  bit_read_BB_tests ();
  bit_write_BB_tests ();
#if 0
  bit_read_3B_tests ();
  bit_write_3B_tests ();
#endif
  bit_read_4BITS_tests ();
  bit_write_4BITS_tests ();
  bit_BLL_tests ();
  bit_read_RC_tests ();
  bit_write_RC_tests ();
  bit_RS_tests ();
  bit_RS_BE_tests ();
  bit_BS_tests ();
  bit_RL_tests ();
  bit_BL_tests ();
  bit_RD_tests ();
  bit_BD_tests ();
  bit_RLL_tests ();
  bit_RLL_BE_tests ();
  bit_utf8_to_TV_tests ();
  bit_utf8_to_TU_tests ();
  bit_TV_to_utf8_tests ();
  bit_read_H_tests ();
  bit_write_H_tests ();
  bit_UMC_bug_tests ();
  bit_read_TV_tests ();
  bit_write_TV_tests ();
  bit_read_TF_tests ();
  bit_write_TF_tests ();
  bit_read_BE_tests ();
  bit_write_BE_tests ();
  bit_read_CMC_tests ();
  bit_read_MS_tests ();
  bit_read_UMC_tests ();
  bit_read_MC_tests ();
  in_hexbin_tests ();

  // Prepare the testcase
  bitchain.size = 100;
  bitchain.bit = 0;
  bitchain.byte = 0;
  bitchain.opts = 1;
  bitchain.fh = NULL;
  bitchain.version = bitchain.from_version = R_2000;
  bitchain.chain = (unsigned char *)calloc (bitchain.size, 1);

  bit_write_RD (&bitchain, 1.2345);
  if (bitchain.byte == 8 && bitchain.bit == 0)
    pass ();
  else
    fail ("bit_write_RD %g @%" PRIuSIZE ".%u", 1.2345, bitchain.byte,
          bitchain.bit);

  bit_set_position (&bitchain, 0);
  // #ifdef WORDS_BIGENDIAN
  //   bit_print (&bitchain, sizeof (double));
  // #endif
  if ((dbl = bit_read_RD (&bitchain)) == 1.2345)
    pass ();
  else
    fail ("bit_read_RD %g != 1.2345", dbl);

  bit_set_position (&bitchain, 1);
  bit_write_RD (&bitchain, 1.2345);
  if (bitchain.byte == 8 && bitchain.bit == 1)
    pass ();
  else
    fail ("bit_write_RD bit=1 %g @%" PRIuSIZE ".%u", 1.2345, bitchain.byte,
          bitchain.bit);

  bit_set_position (&bitchain, 1);
  // #ifdef WORDS_BIGENDIAN
  //   bit_print (&bitchain, sizeof (double));
  // #endif
  if ((dbl = bit_read_RD (&bitchain)) == 1.2345)
    pass ();
  else
    fail ("bit_read_RD bit=1 %g != 1.2345", dbl);

  bit_set_position (&bitchain, 9 * 8); // 9.0
  for (umc = 1; umc <= 0x88000000UL; umc <<= 4)
    {
      umc += 7;
      pos = bit_position (&bitchain);
      bit_write_MC (&bitchain, umc);
      if (bitchain.byte <= 14)
        pass ();
      else
        fail ("bit_write_MC @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

      bit_set_position (&bitchain, pos);
      // bit_print (&bitchain, 5);
      if ((mc = bit_read_MC (&bitchain) == (BITCODE_MC)umc))
        pass ();
      else
        fail ("bit_read_MC " FORMAT_MC, mc);
      bit_set_position (&bitchain, pos);
      // beware of wrap here on 32bit: 0x8777 7777 => 0x7777 7777
      if (umc >= 0x87777777UL)
        break;
    }

  for (umc = 1; umc <= 0x88000000UL; umc <<= 4)
    {
      BITCODE_UMC umc1;
      umc += 7;
      pos = bit_position (&bitchain);
      bit_write_UMC (&bitchain, umc);
      if (bitchain.byte <= 14)
        pass ();
      else
        fail ("bit_write_UMC @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

      bit_set_position (&bitchain, pos);
      // bit_print (&bitchain, 5);
      if ((umc1 = bit_read_UMC (&bitchain) == umc))
        pass ();
      else
        fail ("bit_read_UMC " FORMAT_UMC, umc1);
      bit_set_position (&bitchain, pos);
      // beware of wrap here on 32bit: 0x8777 7777 => 0x7777 7777
      if (umc >= 0x87777777UL)
        break;
    }

  bit_advance_position (&bitchain, 16L);
  bit_write_MS (&bitchain, 5000);
  if (bitchain.byte == 13)
    pass ();
  else
    fail ("bit_write_MS @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -16L);
  if ((bs = bit_read_MS (&bitchain)) == 5000)
    pass ();
  else
    fail ("bit_read_MS %d", bs);

  bit_advance_position (&bitchain, 199L);
  pos = bit_position (&bitchain);
  bit_write_DD (&bitchain, 1.2345, 1.0);
  if ((bitchain.byte == 42 || bitchain.byte == 46) && bitchain.bit == 1)
    pass ();
  else
    fail ("bit_write_DD @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, pos);
  dbl = bit_read_DD (&bitchain, 1.0);
  if (dbl == 1.2345 || fabs (dbl - 1.2345) < 1e-6)
    pass ();
  else
    fail ("bit_read_DD %f", dbl);

  bit_advance_position (&bitchain, 64L);
  pos = bit_position (&bitchain);
  bit_write_BT (&bitchain, 3.0);
  if ((bitchain.byte == 58 || bitchain.byte == 62) && bitchain.bit == 4)
    pass ();
  else
    fail ("bit_write_BT @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, pos);
  if ((dbl = bit_read_BT (&bitchain)) == 3.0)
    pass ();
  else
    fail ("bit_read_BT %f", dbl);

#ifndef WORDS_BIGENDIAN
#  define _CRC 0x141C
#else
#  define _CRC 0xDC89
#endif

  bit_advance_position (&bitchain, -2L);
  {
    uint16_t crc = bit_write_CRC (&bitchain, 0UL, 0x64);
    if (crc == _CRC)
      pass ();
    else
      fail ("bit_write_CRC %X", crc);
  }

  bit_advance_position (&bitchain, -16L);
  if (bit_check_CRC (&bitchain, 0UL, 0x64))
    pass ();
  else
    fail ("bit_check_CRC");

  bit_advance_position (&bitchain, -16L);
  if ((bs = bit_read_CRC (&bitchain)) == _CRC)
    pass ();
  else
    fail ("bit_read_CRC %X", bs);
  bit_advance_position (&bitchain, -16L);

  if ((bs = bit_calc_CRC (0x64, (unsigned char *)&bitchain.chain, 0L)) == 0x64)
    pass ();
  else
    fail ("bit_calc_CRC %X", bs);
  bit_advance_position (&bitchain, 16L);

  bit_set_position (&bitchain, 65 * 8);
  for (int i = 0; i < 6; i++)
    // padding for the T BS, to have aligned strings at 67
    bit_write_B (&bitchain, 0);
  // printf ("@%" PRIuSIZE ".%u\n", bitchain.byte, bitchain.bit);
  pos = bit_position (&bitchain);

  {
    BITCODE_T wstr;
#ifdef WORDS_BIGENDIAN
    BITCODE_TU ws;
#endif
    const uint16_t exp[] = { 'T', 'e', 'i', 'g', 'h', 'a', 0x2122, 0 };
    BITCODE_TV emb = bit_embed_TU ((BITCODE_TU)exp);
    if (strEQc (emb, "Teigha\\U+2122"))
      pass ();
    else
      fail ("bit_embed_TU \"%s\"", emb);
    bitchain.from_version = R_2000;
    bitchain.version = R_2007; // @65.6
    // wlen = 8
    bit_write_T (&bitchain, (char *)"Teigha\\U+2122"); // convert to unicode
    // containing the ending 0L
    // printf ("TU @%" PRIuSIZE ".%u\n", bitchain.byte, bitchain.bit);
    if (bitchain.byte == 83 && bitchain.bit == 0 && bitchain.chain[79] == 0x22
        && bitchain.chain[80] == 0x21)
      pass ();
    else
      fail ("bit_write_T => TU %x %x @%" PRIuSIZE ".%u", bitchain.chain[79],
            bitchain.chain[80], bitchain.byte, bitchain.bit);

    bit_set_position (&bitchain, pos);
    // fprintf (stderr, "bit_write_T => TU\n");
    // bit_print (&bitchain, 18);
    bitchain.from_version = R_2007;
    bitchain.version = R_2007;
    wstr = bit_read_T (&bitchain);
#ifdef WORDS_BIGENDIAN
    ws = (BITCODE_TU)wstr;
#endif
    // printf ("@%" PRIuSIZE ".%u\n", bitchain.byte, bitchain.bit);
    if (bitchain.byte == 83 && bitchain.bit == 0 &&
#ifndef WORDS_BIGENDIAN
        wstr && !memcmp (wstr, exp, sizeof (exp))
#else
        ws[0] == 'T' && ws[1] == 'e' && ws[6] == 0x2122 && ws[7] == 0
#endif
    )
      pass ();
    else
      {
        fail ("bit_read_T => TU @%" PRIuSIZE ".%u", bitchain.byte,
              bitchain.bit);
#ifdef WORDS_BIGENDIAN
        bit_set_position (&bitchain, pos);
        bit_print (&bitchain, 18);
        fprintf (stderr, "ws[]: %u %u ... %x %u\n", ws[0], ws[1], ws[6],
                 ws[7]);
#endif
      }
    bit_set_position (&bitchain, pos);
    free (wstr);

    bitchain.from_version = R_2007;
    bitchain.version = R_2000;
    bit_write_T (&bitchain, (char *)exp); // convert to ASCII via embed
    // printf ("TV @%" PRIuSIZE ".%u\n", bitchain.byte, bitchain.bit);
    if (bitchain.byte == 81 && bitchain.bit == 0)
      pass ();
    else
      fail ("bit_write_T => TV \"%s\" @%" PRIuSIZE ".%u", &bitchain.chain[67],
            bitchain.byte, bitchain.bit);

    bit_set_position (&bitchain, pos);
    bitchain.from_version = R_2000;
    bitchain.version = R_2000;
    str = bit_read_T (&bitchain); // via bit_read_TV
    // printf ("@%" PRIuSIZE ".%u\n", bitchain.byte, bitchain.bit);
    if (str && !strcmp (str, "Teigha\\U+2122"))
      pass ();
    else
      {
        size_t pos1;
        fail ("bit_read_T => TV \"%s\" @%" PRIuSIZE ".%u", str, bitchain.byte,
              bitchain.bit);
        pos1 = bit_position (&bitchain);
        bit_set_position (&bitchain, pos);
        bit_print (&bitchain, 16);
        bit_set_position (&bitchain, pos1);
      }
    bitchain.from_version = bitchain.version = R_2004;
    free (str);
    free (emb);
  }

  pos = bit_position (&bitchain);
  // printf ("@%" PRIuSIZE ".%u\n", bitchain.byte, bitchain.bit);
  {
    Dwg_Color color;
    bitchain.from_version = bitchain.version = R_2000;
    memset (&color, 0, sizeof (color));
    color.index = 19;
    bit_write_CMC (&bitchain, &bitchain, &color);
    if (bitchain.byte == 82 && bitchain.bit == 2)
      pass ();
    else
      fail ("bit_write_CMC @%" PRIuSIZE ".%u", bitchain.byte, bitchain.bit);
  }
  size = bit_position (&bitchain) - pos;
  if (size == 10)
    pass ();
  else
    fail ("CMC size: %" PRIuSIZE "", size);

  bit_advance_position (&bitchain, -(long)size);
  {
    Dwg_Color color_read;
    bit_read_CMC (&bitchain, &bitchain, &color_read);

    if (color_read.index == 19)
      pass ();
    else
      fail ("bit_read_CMC %d (r2000)", color_read.index);
    free (color_read.name);
    free (color_read.book_name);
  }

  bit_advance_position (&bitchain, -(long)size);
  {
    Dwg_Color color;
    pos = bit_position (&bitchain);
    bitchain.from_version = bitchain.version = R_2004;
    memset (&color, 0, sizeof (color));
    color.index = 19;
    color.rgb = 0xc2ff00ff;
    color.flag = 3; // write both. to skip book set it to NULL
    color.name = (char *)"Some color";
    color.book_name = (char *)"DIC(3) Catalog";
    bit_write_CMC (&bitchain, &bitchain, &color);
    if (bitchain.byte == 115 && bitchain.bit == 0)
      pass ();
    else
      fail ("bit_write_CMC (r2004) @%" PRIuSIZE ".%u", bitchain.byte,
            bitchain.bit);

    bit_set_position (&bitchain, pos);
    bit_read_CMC (&bitchain, &bitchain, &color);

    // index is fixed up for dxf
    if (color.rgb == 0xc2ff00ff && color.method == 0xc2 && color.flag == 3
        && !strcmp (color.name, "Some color"))
      pass ();
    else
      fail ("bit_read_CMC (%d,%x,%d) %s", color.index, color.rgb, color.flag,
            color.name);
    free (color.name);
    free (color.book_name);
  }

  bitchain.byte++;
  bitchain.bit = 0;
  {
    bit_write_TF (&bitchain, sentinel, 16);
    if (bitchain.chain[--bitchain.byte] == 0x5F)
      pass ();
    else
      fail ("bit_write_sentinel 0x%X", bitchain.chain[bitchain.byte]);
  }

  bitchain.bit = 0;
  bitchain.byte = 0;
  {
    int ret = bit_search_sentinel (&bitchain, sentinel);
    if (bitchain.byte == 132)
      pass ();
    else
      {
        fail ("bit_search_sentinel %" PRIuSIZE "", bitchain.byte);
        bitchain.byte = 150;
      }
  }
  {
    unsigned int check
        = bit_calc_CRC (0xC0C1, (unsigned char *)bitchain.chain, 124L);
    if (check == 0x572B)
      pass ();
    else
      fail ("bit_calc_CRC %04X", check);
  }

  bit_chain_alloc (&bitchain);
  if (bitchain.size == 8292)
    pass ();
  else
    fail ("bit_chain_alloc %" PRIuSIZE, bitchain.size);

  {
    dwg_wchar_t w[] = { 0x4e2d, 0x5fc3, 0x7ebf, 0 };
    unsigned char expect[]
        = { 0xe4, 0xb8, 0xad, 0xe5, 0xbf, 0x83, 0xe7, 0xba, 0xbf, 0 };
    unsigned char *s = (unsigned char *)bit_convert_TU (w);
    int fails = 0;
    for (unsigned int i = 0; i < sizeof (expect); i++)
      {
        if (s[i] != expect[i])
          {
            fails++;
            fail ("bit_convert_TU s[%d] %x != %x", i, s[i], expect[i]);
          }
      }
    if (!fails)
      pass ();
    free (s);
  }

  free (bitchain.chain);
  return failed;
}

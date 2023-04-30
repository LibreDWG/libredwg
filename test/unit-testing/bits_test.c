#define BITS_TEST_C
#include "config.h"
#include "bits.h"
#include "common.h"
#include "codepages.h"
#include "decode_r11.h"
#include <string.h>
#include <stdlib.h>
#include "tests_common.h"

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
    fail ("bit_read_4BITS 0x%X != 0x%X dat:%x", (unsigned)result,             \
          x, bitchain.chain[0]);                                              \
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
bit_read_BLL_tests (void)
{
  /* 001 => 1, 00000011 */
  Bit_Chain bitchain = strtobt ("00100000011");
  BITCODE_BLL result = bit_read_BLL (&bitchain);
  if (result == 3)
    pass ();
  else
    fail ("bit_read_BLL " FORMAT_BLL, result);
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
bit_read_RS_tests (void)
{
  Bit_Chain bitchain = strtobt ("1111111100000001");
  unsigned int result = bit_read_RS (&bitchain);

  if (result == 511)
    pass ();
  else
    fail ("bit_write_RS");
  bitfree (&bitchain);
}

static void
bit_write_RS_tests (void)
{
  Bit_Chain bitchain = strtobt ("1111111111111111");
  bit_write_RS (&bitchain, 511);

  if (bitchain.chain[0] == 255 && bitchain.chain[1] == 0x01)
    pass ();
  else
    fail ("bit_write_RS");
  bitfree (&bitchain);
}

static void
bit_write_RL_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111"
                                "11111111"
                                "11111111"
                                "11111111");
  bit_write_RL (&bitchain, 2147549183);
  if (bitchain.chain[0] == 255 && bitchain.chain[1] == 255
      && bitchain.chain[2] == 0 && bitchain.chain[3] == 128)
    pass ();
  else
    fail ("bit_write_RL %d %d %d %d", bitchain.chain[0], bitchain.chain[1],
          bitchain.chain[2], bitchain.chain[3]);
  bitfree (&bitchain);
}

static void
bit_read_RL_tests (void)
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
  bitfree (&bitchain);
}

static void
bit_read_RD_tests (void)
{
  Bit_Chain bitchain;
  double value = 25.21241;
  double result;

  bitprepare (&bitchain, sizeof (double));
  free (bitchain.chain);
  bitchain.chain = (unsigned char *)&value;
  // bit_print (&bitchain, sizeof (double));

  result = bit_read_RD (&bitchain);

  if (result == value)
    pass ();
  else
    fail ("bit_read_RD");
}

static void
bit_write_RD_tests (void)
{
  Bit_Chain bitchain;
  double value = 25.2547841;
  double *result;

  bitprepare (&bitchain, sizeof (double));
  bit_write_RD (&bitchain, value);
  // bit_print (&bitchain, sizeof (double));

  result = (double *)bitchain.chain;
  if (*result == value)
    pass ();
  else
    fail ("bit_write_RD");
  bitfree (&bitchain);
}

static void
bit_read_H_tests (void)
{
#if defined(__MINGW64_VERSION_MAJOR) && defined(__GNUC__) && __GNUC__ >= 9
  return;
#endif

  Bit_Chain bitchain;
  Dwg_Handle result;
  int ret;

#define test_H_r_case(s, dwg_ver, r, c, si, v)                                \
  bitchain = strtobt (s);                                                     \
  bitchain.version = dwg_ver;                                                 \
  ret = bit_read_H (&bitchain, &result);                                      \
  if (ret == r && result.code == c && result.size == si                       \
      && result.value == v)                                                   \
    ok ("bit_read_H: " FORMAT_H, ARGS_H (result));                            \
  else {                                                                      \
    fail ("bit_read_H: %s (result " FORMAT_H ")", s, ARGS_H (result));        \
    /*bit_print (&bitchain, sizeof (Dwg_Handle));                           */\
  }                                                                           \
  bitfree (&bitchain)

  //              code   size   value
  test_H_r_case ("0100" "0001" "00000101", R_14, 0, 4, 1, 5);
  test_H_r_case ("1100" "0001" "00001011", R_14, 0, 12, 1, 11);
  test_H_r_case ("0100" "0001" "00001100", R_14, 0, 4, 1, 12);
  test_H_r_case ("0011" "0000", R_14, 0, 3, 0, 0);
  test_H_r_case ("0101" "0000", R_14, 0, 5, 0, 0);
  test_H_r_case ("0010" "0001" "00011000", R_14, 0, 2, 1, 24);
  test_H_r_case ("0000" "0001" "00000001", R_14, 0, 0, 1, 1);
  test_H_r_case ("0010" "0010" "00000010" "00001010", R_14, 0, 2, 2, 522);
  test_H_r_case ("0101" "0001" "01011110", R_14, 0, 5, 1, 94);
  test_H_r_case ("0010" "0001" "01100100", R_14, 0, 2, 1, 100);
  // preR13
  //              size       value
  test_H_r_case ("00000001" "00000010", R_11, 0, 0, 1, 2);
  test_H_r_case ("00000010" "00000010" "00001010", R_11, 0, 0, 2, 522);
}

static void
bit_write_H_tests (void)
{
  Bit_Chain bitchain;
  unsigned long byte;
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
    ok ("bit_write_H: " FORMAT_H, ARGS_H (handle));                           \
  else                                                                        \
    {                                                                         \
      bit_print (&bitchain, sizeof (Dwg_Handle));                             \
      fail ("bit_write_H (" FORMAT_H ") @%lu.%d", ARGS_H (handle),            \
            bitchain.byte, bitchain.bit);                                     \
    }                                                                         \
  bitfree (&bitchain)

  test_H_w_case(4, 1, 5, R_14);
  test_H_w_case(12, 1, 11, R_14);
  test_H_w_case(4, 1, 12, R_14);
  test_H_w_case(3, 0, 0, R_14);
  test_H_w_case(5, 0, 0, R_14);
  test_H_w_case(2, 1, 24, R_14);
  test_H_w_case(0, 1, 1, R_14);
  test_H_w_case(2, 2, 522, R_14);
  test_H_w_case(5, 1, 94, R_14);
  test_H_w_case(2, 1, 100, R_14);
  // preR13
  test_H_w_case(0, 1, 2, R_11);
  test_H_w_case(0, 2, 522, R_11);
}

static void
bit_UMC_bug_tests (void)
{
  BITCODE_UMC umc;
  BITCODE_MC mc;
  Bit_Chain bitchain;
  bitprepare (&bitchain, 6);

  bit_write_TF (&bitchain, (BITCODE_TF)"\x01\xc6\x00", 3);
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
  bit_write_TF (&bitchain, (BITCODE_TF)"\xd2\xec\xa9\xf2\x92\xa2\x01", 7);
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
                      strlen (src1), 0, CP_CP949);
  if (strEQc (p, "\xdc\xc3\x7e\xe8"))
    pass ();
  else
    fail ("bit_utf8_to_TV %s as CP949", p);
}

static void
bit_TV_to_utf8_tests (void)
{
  char *p;
  char *srcu = strdup ("Test\\U+0234");
  const char *src1 = "Test\xc4"; // Ä
  const char *src2 = "Test\xc6"; // Ć \U+0106
  const char *src7 = "Test\xd3"; // Σ
  // echo -n "시험" | iconv -f utf8 -t cp949 | od -t x1
  char *src_kor = strdup ("\xbd\xc3\xc7\xe8");

  ok ("bit_TV_to_utf8_tests init");
  p = bit_TV_to_utf8 ((char *)srcu, CP_ISO_8859_1);
  if (strEQc (p, "Test\xc8\xb4"))
    ok ("bit_TV_to_utf8_tests 8859-1");
  else
    fail ("bit_TV_to_utf8 %s ISO_8859_1", p);
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
  if
#ifndef _MSC_VER
    (strEQc (p, "시험"))
#else
    // echo "시험" | od -t x1
    // U+feff U+c2dc U+d5d8
    (strEQc (p, "\xc2\xbd\xc3\x83\xc3\x87\xc3\xa8"))
             /* "\xec\x8b\x9c\xed\x97\x98" */
#endif
    ok ("bit_TV_to_utf8_tests CP949");
  else
    {
#ifdef _MSC_VER
      ok ("TODO bit_TV_to_utf8 %s CP_CP949 (len=%zu)", p, strlen (p));
#else
      fail ("bit_TV_to_utf8 %s CP_CP949 (len=%zu)", p, strlen (p));
#endif
      for (size_t i = 0; i < strlen (p); i++)
        printf ("\\x%02x", (unsigned char)p[i]);
      printf ("\n");
    }
  if (p != src_kor)
    free (p);
  free (src_kor);
}

int
main (int argc, char const *argv[])
{

  Bit_Chain bitchain;
  BITCODE_BD dbl;
  BITCODE_BL bl;
  BITCODE_BS bs;
  BITCODE_MC mc;
  BITCODE_UMC umc;
  unsigned long pos;
  long size;
  char *str;
  unsigned char sentinel[]
      = { 0xCF, 0x7B, 0x1F, 0x23, 0xFD, 0xDE, 0x38, 0xA9,
          0x5F, 0x7C, 0x68, 0xB8, 0x4E, 0x6D, 0x33, 0x5F };
  loglevel = is_make_silent () ? 0 : 3;

  bit_read_B_tests ();
  bit_write_B_tests ();
  bit_advance_position_tests ();
  bit_read_BB_tests ();
  bit_write_BB_tests ();
  bit_read_3B_tests ();
  bit_write_3B_tests ();
  bit_read_4BITS_tests ();
  bit_write_4BITS_tests ();
  bit_read_BLL_tests ();
  bit_read_RC_tests ();
  bit_write_RC_tests ();
  bit_read_RS_tests ();
  bit_write_RS_tests ();
  bit_read_RL_tests ();
  bit_write_RL_tests ();
  bit_read_RD_tests ();
  bit_write_RD_tests ();
  bit_utf8_to_TV_tests ();
  bit_TV_to_utf8_tests ();
  bit_read_H_tests ();
  bit_write_H_tests ();
  bit_UMC_bug_tests ();

  // Prepare the testcase
  bitchain.size = 100;
  bitchain.bit = 0;
  bitchain.byte = 0;
  bitchain.opts = 1;
  bitchain.fh = NULL;
  bitchain.version = bitchain.from_version = R_2000;
  bitchain.chain = (unsigned char *)calloc (bitchain.size, 1);

  bit_write_RD (&bitchain, 0xBA43B7400);
  if (bitchain.byte == 8 && bitchain.bit == 0)
    pass ();
  else
    fail ("bit_write_RD @%lu.%d", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -64L);
  // bit_print (&bitchain, sizeof (double));
  if ((dbl = bit_read_RD (&bitchain)) == 0xBA43B7400)
    pass ();
  else
    fail ("bit_read_RD %g", dbl);

  bit_write_BS (&bitchain, 32767);
  if (bitchain.byte == 10 && bitchain.bit == 2)
    pass ();
  else
    fail ("bit_write_BS @%lu.%d", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -18L);
  // bit_print (&bitchain, 4);
  if ((bs = bit_read_BS (&bitchain)) == 32767)
    pass ();
  else
    fail ("bit_read_BS %d", bs);
  bit_advance_position (&bitchain, -18L);

  bit_write_BS (&bitchain, 256);
  if (bitchain.byte == 8 && bitchain.bit == 2)
    pass ();
  else
    fail ("bit_write_BS @%lu.%d", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -2L);
  // bit_print (&bitchain, 1);
  if ((bs = bit_read_BS (&bitchain)) == 256)
    pass ();
  else
    fail ("bit_read_BS %d", bs);

  bit_write_BL (&bitchain, 0);
  if (bitchain.byte == 8 && bitchain.bit == 4)
    pass ();
  else
    fail ("bit_write_BL @%lu.%d", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -2L);
  // bit_print (&bitchain, 1);
  if ((bl = bit_read_BL (&bitchain)) == 0)
    pass ();
  else
    fail ("bit_read_BL " FORMAT_BL, bl);

  pos = bit_position (&bitchain);
  bit_write_BD (&bitchain, 1.0);
  if (bitchain.byte == 8 && bitchain.bit == 6)
    pass ();
  else
    fail ("bit_write_BD @%lu.%d", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, pos);
  // bit_print (&bitchain, 8);
  if ((dbl = bit_read_BD (&bitchain)) == 1.0)
    pass ();
  else
    fail ("bit_read_BD %f", dbl);

  bit_advance_position (&bitchain, 2L); // 9.0

  for (umc = 1; umc <= 0x88000000UL; umc <<= 4)
    {
      umc += 7;
      pos = bit_position (&bitchain);
      bit_write_MC (&bitchain, umc);
      if (bitchain.byte <= 14)
        pass ();
      else
        fail ("bit_write_MC @%lu.%d", bitchain.byte, bitchain.bit);

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
        fail ("bit_write_UMC @%lu.%d", bitchain.byte, bitchain.bit);

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
    fail ("bit_write_MS @%lu.%d", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -16L);
  if ((bs = bit_read_MS (&bitchain)) == 5000)
    pass ();
  else
    fail ("bit_read_MS %d", bs);

  bit_write_BE (&bitchain, 20.2535, 10.2523, 52.32563);
  if (bitchain.bit == 7 && bitchain.byte == 37)
    pass ();
  else
    fail ("bit_write_BE @%lu.%d", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -199L);
  {
    double x, y, z;
    bit_read_BE (&bitchain, &x, &y, &z);
    if (x == 20.2535 && y == 10.2523 && z == 52.32563)
      pass ();
    else
      fail ("bit_read_BE (%f,%f,%f)", x, y, z);
  }

  pos = bit_position (&bitchain);
  bit_write_DD (&bitchain, 20.256, 50.252);
  if (bitchain.bit == 1 && bitchain.byte == 46)
    pass ();
  else
    fail ("bit_write_DD @%lu.%d", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, pos);
  if ((dbl = bit_read_DD (&bitchain, 50.252)) == 20.256)
    pass ();
  else
    fail ("bit_read_DD %f", dbl);

  bit_advance_position (&bitchain, 64L);
  pos = bit_position (&bitchain);
  bit_write_BT (&bitchain, 3.0);
  if (bitchain.bit == 4 && bitchain.byte == 62)
    pass ();
  else
    fail ("bit_write_BT @%lu.%d", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, pos);
  if ((dbl = bit_read_BT (&bitchain)) == 3.0)
    pass ();
  else
    fail ("bit_read_BT %f", dbl);

#define _CRC 0x6024
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
  pos = bit_position (&bitchain);

  bit_write_TV (&bitchain, (char *)"GNU"); // we don't store the \0
  if (bitchain.byte == 69 && bitchain.bit == 2)
    pass ();
  else
    fail ("bit_write_TV @%ld.%d", bitchain.byte, bitchain.bit);

  bit_set_position (&bitchain, pos);
  str = bit_read_TV (&bitchain);
  if (!strcmp (str, "GNU"))
    pass ();
  else
    fail ("bit_read_TV");
  bit_set_position (&bitchain, pos);
  free (str);
  for (int i = 0; i < 6; i++)
    bit_write_B (&bitchain,
                 0); // padding for the T BS, to have aligned strings at 67
  pos = bit_position (&bitchain); // 526

  {
    BITCODE_T wstr;
    const uint16_t exp[] = { 'T', 'e', 'i', 'g', 'h', 'a', 0x2122, 0 };
    bitchain.version = R_2007;                         // @65.6
    bit_write_T (&bitchain, (char *)"Teigha\\U+2122"); // convert to unicode
    if (bitchain.byte == 83 && bitchain.bit == 0 && // containing the ending 0L
        bitchain.chain[79] == 0x22 && bitchain.chain[80] == 0x21)
      pass ();
    else
      fail ("bit_write_T => TU @%ld.%d", bitchain.byte, bitchain.bit);

    bit_set_position (&bitchain, pos);
    bitchain.from_version = R_2007;
    bitchain.version = R_2000;
    wstr = bit_read_T (&bitchain);
    if (wstr && !memcmp (wstr, exp, sizeof (exp)))
      pass ();
    else
      fail ("bit_read_T => TU @%ld.%d", bitchain.byte, bitchain.bit);
    bit_set_position (&bitchain, pos);
    free (wstr);

    bit_write_T (&bitchain, (char *)exp); // convert to ASCII via embed
    if (bitchain.byte == 81 && bitchain.bit == 0)
      pass ();
    else
      fail ("bit_write_T => TV \"%s\" @%ld.%d", &bitchain.chain[67],
            bitchain.byte, bitchain.bit);

    bit_set_position (&bitchain, pos);
    bitchain.version = R_2000;
    bitchain.from_version = R_2007;
    str = bit_read_T (&bitchain);
    if (str && !strcmp (str, "Teigha\\U+2122"))
      pass ();
    else
      fail ("bit_read_T => TV \"%s\" @%ld.%d", str, bitchain.byte,
            bitchain.bit);
    bitchain.from_version = bitchain.version = R_2004;
    free (str);
  }

  bit_write_L (&bitchain, 20);
  if (bitchain.byte == 99 && bitchain.bit == 0)
    pass ();
  else
    fail ("bit_write_L @%ld.%d", bitchain.byte, bitchain.bit);

  bit_advance_position (&bitchain, -32L);
  if ((bl = bit_read_L (&bitchain)) == 20)
    pass ();
  else
    fail ("bit_read_L %ul", bl);

  pos = bit_position (&bitchain);
  {
    Dwg_Color color;
    bitchain.from_version = bitchain.version = R_2000;
    memset (&color, 0, sizeof (color));
    color.index = 19;
    bit_write_CMC (&bitchain, &bitchain, &color);
    if (bitchain.byte == 100 && bitchain.bit == 2)
      pass ();
    else
      fail ("bit_write_CMC @%ld.%d", bitchain.byte, bitchain.bit);
  }
  size = bit_position (&bitchain) - pos;
  if (size == 10)
    pass ();
  else
    fail ("CMC size: %ld", size);

  bit_advance_position (&bitchain, -size);
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

  bit_advance_position (&bitchain, -size);
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
    if (bitchain.byte == 133 && bitchain.bit == 0)
      pass ();
    else
      fail ("bit_write_CMC (r2004) @%ld.%d", bitchain.byte, bitchain.bit);

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
    if (bitchain.byte == 150)
      pass ();
    else
      {
        fail ("bit_search_sentinel %lu", bitchain.byte);
        bitchain.byte = 150;
      }
  }
  {
    unsigned int check
        = bit_calc_CRC (0xC0C1, (unsigned char *)bitchain.chain, 124L);
    if (check == 0xFEC1)
      pass ();
    else
      fail ("bit_calc_CRC %04X", check);
  }

  bit_chain_alloc (&bitchain);
  if (bitchain.size == 8292)
    pass ();
  else
    fail ("bit_chain_alloc %ld", bitchain.size);

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

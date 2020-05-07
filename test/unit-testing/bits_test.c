#define BITS_TEST_C
#include "../../src/bits.h"
#include "../../src/common.h"
#include <string.h>
#include <stdlib.h>
#include "tests_common.h"

void bit_advance_position_tests (void);
void bit_read_B_tests (void);
void bit_write_B_tests (void);
void bit_read_BB_tests (void);
void bit_write_BB_tests (void);
void bit_read_3B_tests (void);
void bit_write_3B_tests (void);
void bit_read_4BITS_tests (void);
void bit_write_4BITS_tests (void);
void bit_read_BLL_tests (void);
void bit_read_RC_tests (void);
void bit_write_RC_tests (void);
void bit_read_RS_tests (void);
void bit_write_RS_tests (void);
void bit_read_RL_tests (void);
void bit_write_RL_tests (void);
void bit_read_RD_tests (void);
void bit_write_RD_tests (void);
void bit_read_H_tests (void);
void bit_write_H_tests (void);

void
bit_advance_position_tests (void)
{
  Bit_Chain bitchain = strtobt ("10101010");
  if (bitchain.bit == 0 && bitchain.byte == 0)
    ok ("bit_advance_position");
  else
    fail ("bit_advance_position");
  bitfree (&bitchain);
}

void
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

void
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

void
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

void
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

void
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

void
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
void
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

void
bit_read_4BITS_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111");
  unsigned char result = bit_read_4BITS (&bitchain);
  if (result == 0xF)
    pass ();
  else
    fail ("bit_read_4BITS 0x%X != 0xF dat:%x", (unsigned)result, bitchain.chain[0]);
  bitfree (&bitchain);

#define test_4bits(s, x)                                                      \
  bitchain = strtobt (s);                                                     \
  result = bit_read_4BITS (&bitchain);                                        \
  if (result == x)                                                            \
    pass ();                                                                  \
  else                                                                        \
    fail ("bit_read_4BITS 0x%X != 0xF dat:%x", (unsigned)result,              \
          bitchain.chain[0]);                                                 \
  bitfree (&bitchain)

  test_4bits ("0000", 0x0);
  test_4bits ("0001", 0x1);
  test_4bits ("0010", 0x2);
  test_4bits ("0011", 0x3);
  test_4bits ("0100", 0x4);
  test_4bits ("0101", 0x5);
  test_4bits ("0111", 0x7);
  test_4bits ("1000", 0x8);
  test_4bits ("1001", 0x9);
  test_4bits ("1100", 0xC);
  test_4bits ("1101", 0xD);
  test_4bits ("1111", 0xF);
}

void
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

void
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

void
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

void
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

void
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

void
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

void
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

void
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

void
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
  // TODO
  // bit_read_H_tests();
  // bit_write_H_tests();

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
        fail ("bit_read_MC %ld", (long)mc);
      bit_set_position (&bitchain, pos);
      // beware of wrap here on 32bit: 0x8777 7777 => 0x7777 7777
      if (umc >= 0x87777777UL)
        break;
    }

  for (umc = 1; umc <= 0x88000000UL; umc <<= 4)
    {
      umc += 7;
      pos = bit_position (&bitchain);
      bit_write_UMC (&bitchain, umc);
      if (bitchain.byte <= 14)
        pass ();
      else
        fail ("bit_write_UMC @%lu.%d", bitchain.byte, bitchain.bit);

      bit_set_position (&bitchain, pos);
      // bit_print (&bitchain, 5);
      if ((mc = (BITCODE_UMC)bit_read_UMC (&bitchain) == umc))
        pass ();
      else
        fail ("bit_read_UMC %lu", (BITCODE_UMC)mc);
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

  // mingw-w64 gcc-9.2.0 miscompilation in bit_read_H with val[i]: (%rbx) being dat+1
#if defined(__MINGW64_VERSION_MAJOR) && defined(__GNUC__) && __GNUC__ >= 9
  if (0)
#endif
  {
    int i;
    Dwg_Handle handles[]
        = { { 4, 1, 5 },  { 12, 1, 11 }, { 4, 1, 12 }, { 3, 0, 0 },
            { 5, 0, 0 },  { 2, 1, 24 },  { 0, 1, 1 },  { 2, 2, 522 },
            { 5, 1, 94 }, { 2, 1, 100 } };
    Dwg_Handle handle;
    pos = bit_position (&bitchain);
    for (i = 0; i < 10; i++)
      {
        unsigned long byte = 63 + handles[i].size;
        bit_write_H (&bitchain, &handles[i]);
        if (bitchain.byte == byte && bitchain.bit == 4)
          pass ();
        else
          fail ("bit_write_H (" FORMAT_H ") @%lu.%d", ARGS_H (handles[i]),
                bitchain.byte, bitchain.bit);

        bit_set_position (&bitchain, pos);
        bit_read_H (&bitchain, &handle);
        if (handle.code == handles[i].code && handle.size == handles[i].size
            && handle.value == handles[i].value)
          pass ();
        else
          fail ("bit_read_H (" FORMAT_H ")", ARGS_H (handle));

        bit_set_position (&bitchain, pos);
      }
  }
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
  if (bitchain.byte == 70 && bitchain.bit == 2)
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
    bit_write_B (&bitchain, 0); // padding for the T BS, to have aligned strings at 67
  pos = bit_position (&bitchain); // 526

  {
    BITCODE_T wstr;
    const uint16_t exp[] = {'T', 'e', 'i', 'g', 'h', 'a', 0x2122, 0};
    bitchain.version = R_2007; // @65.6
    bit_write_T (&bitchain, (char *)"Teigha\\U+2122"); // convert to unicode
    if (bitchain.byte == 83 && bitchain.bit == 0 &&    // containing the ending 0L
        bitchain.chain[79] == 0x22 &&
        bitchain.chain[80] == 0x21)
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

    bit_write_T (&bitchain, (char*)exp); // convert to ASCII via embed
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
      fail ("bit_read_T => TV \"%s\" @%ld.%d", str, bitchain.byte, bitchain.bit);
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
    bitchain.version = R_2000;
    color.index = 19;
    color.rgb = 0xc2ff00ff;
    color.flag = 3; // write both. to skip book set it to NULL
    color.name = (char *)"Some color";
    color.book_name = (char *)"DIC(3) Catalog";
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
      fail ("bit_read_CMC %d", color_read.index);
    free (color_read.name);
    free (color_read.book_name);
  }

  bit_advance_position (&bitchain, -size);
  {
    Dwg_Color color;
    pos = bit_position (&bitchain);
    bitchain.from_version = bitchain.version = R_2004;
    color.index = 19;
    color.rgb = 0xc2ff00ff;
    color.flag = 3; // write both. to skip book set it to NULL
    color.name = (char *)"Some color";
    color.book_name = (char *)"DIC(3) Catalog";
    bit_write_CMC (&bitchain, &bitchain, &color);
    if (bitchain.byte == 138 && bitchain.bit == 2)
      pass ();
    else
      fail ("bit_write_CMC (true) @%ld.%d", bitchain.byte, bitchain.bit);

    bit_set_position (&bitchain, pos);
    bit_read_CMC (&bitchain, &bitchain, &color);

    // index is ignored
    if (color.index == 0 && color.rgb == 0xc2ff00ff && color.flag == 3
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
    bit_write_sentinel (&bitchain, sentinel);

    if (bitchain.chain[--bitchain.byte] == 0x5F)
      pass ();
    else
      fail ("bit_write_sentinel 0x%X", bitchain.chain[bitchain.byte]);
  }

  bitchain.bit = 0;
  bitchain.byte = 0;
  {
    int ret = bit_search_sentinel (&bitchain, sentinel);
    if (bitchain.byte == 155)
      pass ();
    else
      {
        fail ("bit_search_sentinel %lu", bitchain.byte);
        bitchain.byte = 155;
      }
  }
  {
    unsigned int check
        = bit_calc_CRC (0xC0C1, (unsigned char *)bitchain.chain, 124L);
    if (check == 0x6EF6)
      pass ();
    else
      fail ("bit_calc_CRC %04X", check);
  }

  bit_chain_alloc (&bitchain);
  if (bitchain.size == 2148)
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

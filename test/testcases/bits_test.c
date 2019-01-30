#include "../../src/bits.h"

#include "../../src/common.h"
CLANG_DIAG_IGNORE(-Wformat-nonliteral)
CLANG_DIAG_IGNORE(-Wmissing-prototypes)
#include <dejagnu.h>
CLANG_DIAG_RESTORE
CLANG_DIAG_RESTORE
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
    pass ("bit_advance_position");
  else
    fail ("bit_advance_position");
}

void
bit_read_B_tests (void)
{
  Bit_Chain bitchain = strtobt ("101010");
  unsigned char result = bit_read_B (&bitchain);
  if (result == 0x01)
    pass ("bit_read_B");
  else
    fail ("bit_read_B %c", result);
}

void
bit_write_B_tests (void)
{
  Bit_Chain bitchain = strtobt ("0000000");
  bit_write_B (&bitchain, 1);

  if (bitchain.chain[0] == 0x80)
    pass ("bit_write_B");
  else
    fail ("bit_write_B");
}

void
bit_read_BB_tests (void)
{
  Bit_Chain bitchain = strtobt ("10101010");
  unsigned char result = bit_read_BB (&bitchain);
  if (result == 2)
    pass ("bit_read_BB");
  else
    fail ("bit_read_BB %d", result);
}

void
bit_write_BB_tests (void)
{
  Bit_Chain bitchain = strtobt ("01000000");
  bit_advance_position (&bitchain, 2);
  bit_write_BB (&bitchain, 0x2);

  if (bitchain.chain[0] == 0x60)
    pass ("bit_write_BB");
  else
    fail ("bit_write_BB %d", bitchain.chain[0]);
}

void
bit_read_3B_tests (void)
{
  Bit_Chain bitchain = strtobt ("100111");
  unsigned char result = bit_read_3B (&bitchain);
  if (result == 2)
    pass ("bit_read_3B");
  else
    fail ("bit_read_3B %d", result);
  result = bit_read_3B (&bitchain);
  if (result == 0)
    pass ("bit_read_3B");
  else
    fail ("bit_read_3B %d", result);
  result = bit_read_3B (&bitchain);
  if (result == 7)
    pass ("bit_read_3B");
  else
    fail ("bit_read_3B %d", result);
}

void
bit_write_3B_tests (void)
{
  Bit_Chain bitchain = strtobt ("01000000");
  bit_advance_position (&bitchain, 2);
  bit_write_3B (&bitchain, 0x2);

  if (bitchain.chain[0] == 80)
    pass ("bit_write_3B");
  else
    fail ("bit_write_3B %d", bitchain.chain[0]);
}

/* This function calls tests for bit_write_4BITS_tests()
   Used in VIEW view_mode, type 71
 */
void
bit_write_4BITS_tests (void)
{
  Bit_Chain bitchain = strtobt ("00000000");
  bit_write_4BITS (&bitchain, 0x0F);
  printf ("%x\n", bitchain.chain[0]);

  if (bitchain.chain[0] == 0xF0)
    pass ("bit_write_4BITS");
  else
    fail ("bit_write_4BITS %x", bitchain.chain[0]);
}

void
bit_read_4BITS_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111");
  unsigned char result = bit_read_4BITS (&bitchain);
  if (result == 0xF)
    pass ("bit_read_4BITS");
  else
    fail ("bit_read_4BITS");
}

void
bit_read_BLL_tests (void)
{
  /* 001 => 1, 00000011 */
  Bit_Chain bitchain = strtobt ("00100000011");
  BITCODE_BLL result = bit_read_BLL (&bitchain);
  if (result == 3)
    pass ("bit_read_BLL");
  else
    fail ("bit_read_BLL %lu", result);
}

void
bit_read_RC_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111");
  unsigned char result = bit_read_RC (&bitchain);
  if (result == 0xFF)
    pass ("bit_read_RC");
  else
    fail ("bit_read_RC");
}

void
bit_write_RC_tests (void)
{
  Bit_Chain bitchain = strtobt ("00000000");
  bit_write_RC (&bitchain, 0xFF);
  if (bitchain.chain[0] == 0xFF)
    pass ("bit_write_RC");
  else
    fail ("bit_write_RC");
}

void
bit_read_RS_tests (void)
{
  Bit_Chain bitchain = strtobt ("1111111100000001");
  unsigned int result = bit_read_RS (&bitchain);

  if (result == 511)
    pass ("bit_read_RS");
  else
    fail ("bit_write_RS");
}

void
bit_write_RS_tests (void)
{
  Bit_Chain bitchain = strtobt ("1111111111111111");
  bit_write_RS (&bitchain, 511);

  if (bitchain.chain[0] == 255 && bitchain.chain[1] == 0x01)
    pass ("bit_write_RS");
  else
    fail ("bit_write_RS");
}

void
bit_write_RL_tests (void)
{
  Bit_Chain bitchain = strtobt ("11111111" "11111111" "11111111" "11111111");
  bit_write_RL (&bitchain, 2147549183);
  if (bitchain.chain[0] == 255 && bitchain.chain[1] == 255 &&
      bitchain.chain[2] == 0 && bitchain.chain[3] == 128)
    pass ("bit_write_RL");
  else
    fail ("bit_write_RL %d %d %d %d",
          bitchain.chain[0], bitchain.chain[1], bitchain.chain[2], bitchain.chain[3]);
}

void
bit_read_RL_tests (void)
{                             /* 7f         f7         bf        7d */
  Bit_Chain bitchain = strtobt ("01111111" "11110111" "10111111" "01111101");
  BITCODE_RL result = bit_read_RL (&bitchain);

  if (result == (BITCODE_RL)0x7DBFF77F)
      pass ("bit_read_RL");
  else
    fail ("bit_read_RL 0x%x", (unsigned)result);
}

void
bit_read_RD_tests (void)
{
  Bit_Chain bitchain;
  double value = 25.21241;
  double result;

  bitprepare(&bitchain, sizeof(double));
  bitchain.chain = (unsigned char *) &value;
  bit_print(&bitchain, sizeof(double));

  result = bit_read_RD(&bitchain);

  if (result == value)
    pass("bit_read_RD");
  else
    fail("bit_read_RD");
}

void
bit_write_RD_tests (void)
{
  Bit_Chain bitchain;
  double value = 25.2547841;
  double *result;

  bitprepare(&bitchain, sizeof(double));
  bit_write_RD(&bitchain, value);
  bit_print(&bitchain, sizeof(double));

  result = (double *) bitchain.chain;
  if (*result == value)
    pass("bit_write_RD");
  else
    fail("bit_write_RD");
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
  unsigned char sentinel[] =
    {0xCF,0x7B,0x1F,0x23,0xFD,0xDE,0x38,0xA9,0x5F,0x7C,0x68,0xB8,0x4E,0x6D,0x33,0x5F};

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
  bit_read_RD_tests();
  bit_write_RD_tests();
  //bit_read_H_tests();
  //bit_write_H_tests();

  //Prepare the testcase
  bitchain.size = 100;
  bitchain.bit = 0;
  bitchain.byte = 0;
  bitchain.version = R_2000;
  bitchain.chain = (unsigned char *) calloc (bitchain.size, 1);

  bit_write_RD(&bitchain, 0xBA43B7400);
  if (bitchain.byte == 8 && bitchain.bit == 0)
    pass("bit_write_RD");
  else
    fail("bit_write_RD @%d.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -64);
  bit_print(&bitchain, sizeof(double));
  if ((dbl = bit_read_RD(&bitchain)) == 0xBA43B7400)
    pass("bit_read_RD");
  else
    fail("bit_read_RD %g", dbl);

  bit_write_BS(&bitchain, 32767);
  if (bitchain.byte == 10 && bitchain.bit == 2)
    pass("bit_write_BS");
  else
    fail("bit_write_BS @%d.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -18);
  bit_print(&bitchain, 4);
  if ((bs = bit_read_BS(&bitchain)) == 32767)
    pass("bit_read_BS");
  else
    fail("bit_read_BS %d", bs);
  bit_advance_position(&bitchain, -18);

  bit_write_BS(&bitchain, 256);
  if (bitchain.byte == 8 && bitchain.bit == 2)
    pass("bit_write_BS");
  else
    fail("bit_write_BS @%d.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -2);
  bit_print(&bitchain, 1);
  if ((bs = bit_read_BS(&bitchain)) == 256)
    pass("bit_read_BS");
  else
    fail("bit_read_BS %d", bs);

  bit_write_BL(&bitchain, 0);
  if (bitchain.byte == 8 && bitchain.bit == 4)
    pass("bit_write_BL");
  else
    fail("bit_write_BL @%d.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -2);
  bit_print(&bitchain, 1);
  if ((bl = bit_read_BL(&bitchain)) == 0)
    pass("bit_read_BL");
  else
    fail("bit_read_BL " FORMAT_BL, bl);

  pos = bit_position(&bitchain);
  bit_write_BD(&bitchain, 1.0);
  if (bitchain.byte == 8 && bitchain.bit == 6)
    pass("bit_write_BD");
  else
    fail("bit_write_BD @%d.%d", bitchain.byte, bitchain.bit);

  bit_set_position(&bitchain, pos);
  bit_print(&bitchain, 8);
  if ((dbl = bit_read_BD(&bitchain)) == 1.0)
    pass("bit_read_BD");
  else
    fail("bit_read_BD %f", dbl);

  bit_advance_position(&bitchain, 2); //9.0

  for (umc=1; umc <= 0x88000000UL; umc <<= 4) {
    umc += 7;
    pos = bit_position(&bitchain);
    bit_write_MC(&bitchain, umc);
    if (bitchain.byte <= 14)
      pass("bit_write_MC 0x%x", umc);
    else
      fail("bit_write_MC @%d.%d", bitchain.byte, bitchain.bit);

    bit_set_position(&bitchain, pos);
    bit_print(&bitchain, 5);
    if ((mc = bit_read_MC(&bitchain) == (BITCODE_MC)umc))
      pass("bit_read_MC");
    else
      fail("bit_read_MC %ld", (long)mc);
    bit_set_position(&bitchain, pos);
    // beware of wrap here on 32bit: 0x8777 7777 => 0x7777 7777
    if (umc >= 0x87777777UL)
      break;
  }

  for (umc=1; umc <= 0x88000000UL; umc <<= 4) {
    umc += 7;
    pos = bit_position(&bitchain);
    bit_write_UMC(&bitchain, umc);
    if (bitchain.byte <= 14)
      pass("bit_write_UMC 0x%x", umc);
    else
      fail("bit_write_UMC @%d.%d", bitchain.byte, bitchain.bit);

    bit_set_position(&bitchain, pos);
    bit_print(&bitchain, 5);
    if ((mc = (BITCODE_UMC)bit_read_UMC(&bitchain) == umc))
      pass("bit_read_UMC");
    else
      fail("bit_read_UMC %lu", (BITCODE_UMC)mc);
    bit_set_position(&bitchain, pos);
    // beware of wrap here on 32bit: 0x8777 7777 => 0x7777 7777
    if (umc >= 0x87777777UL)
      break;
  }

  bit_advance_position(&bitchain, 16);
  bit_write_MS(&bitchain, 5000);
  if (bitchain.byte == 13)
    pass("bit_write_MS");
  else
    fail("bit_write_MS @%d.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -16);
  if ((bs = bit_read_MS(&bitchain)) == 5000)
    pass("bit_read_MS");
  else
    fail("bit_read_MS %d", bs);

  bit_write_BE(&bitchain, 20.2535, 10.2523, 52.32563);
  if (bitchain.bit == 7 && bitchain.byte == 37)
    pass("bit_write_BE");
  else
    fail("bit_write_BE @%d.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -199);
  {
    double x,y,z;
    bit_read_BE(&bitchain, &x, &y, &z);
    if (x == 20.2535 && y == 10.2523 && z == 52.32563)
      pass("bit_read_BE");
    else
      fail("bit_read_BE (%f,%f,%f)", x, y, z);
  }

  pos = bit_position(&bitchain);
  bit_write_DD(&bitchain, 20.256, 50.252);
  if (bitchain.bit == 1 && bitchain.byte == 46)
    pass("bit_write_DD");
  else
    fail("bit_write_DD @%d.%d", bitchain.byte, bitchain.bit);

  bit_set_position(&bitchain, pos);
  if ((dbl = bit_read_DD(&bitchain, 50.252)) == 20.256)
    pass("bit_read_DD");
  else
    fail("bit_read_DD %f", dbl);

  bit_advance_position(&bitchain, 64);
  pos = bit_position(&bitchain);
  bit_write_BT(&bitchain, 3.0);
  if (bitchain.bit == 4 && bitchain.byte == 62)
    pass("bit_write_BT");
  else
    fail("bit_write_BT @%d.%d", bitchain.byte, bitchain.bit);

  bit_set_position(&bitchain, pos);
  if ((dbl = bit_read_BT(&bitchain)) == 3.0)
    pass("bit_read_BT");
  else
    fail("bit_read_BT %f", dbl);

  {
    int i;
    Dwg_Handle handles[] = {{4,1,5}, {12,1,11}, {4,1,12}, {3,0,0}, {5,0,0},
                            {2,1,24}, {0,1,1}, {2,2,522}, {5,1,94}, {2,1,100}};
    Dwg_Handle handle;
    pos = bit_position(&bitchain);
    for (i=0; i<10; i++)
      {
        unsigned long byte = 63 + handles[i].size;
        bit_write_H(&bitchain, &handles[i]);
        if (bitchain.byte == byte && bitchain.bit == 4)
          pass("bit_write_H (%d.%d.%ld)",
               handles[i].code, handles[i].size, handles[i].value);
        else
          fail("bit_write_H (%d.%d.%ld) @%d.%d",
               handles[i].code, handles[i].size, handles[i].value,
               bitchain.byte, bitchain.bit);

        bit_set_position(&bitchain, pos);
        bit_read_H(&bitchain, &handle);
        if (handle.code == handles[i].code &&
            handle.size == handles[i].size &&
            handle.value == handles[i].value)
          pass("bit_read_H (%d.%d.%ld)", handle.code, handle.size, handle.value);
        else
          fail("bit_read_H (%d.%d.%lu)", handle.code, handle.size, handle.value);

        bit_set_position(&bitchain, pos);
      }
  }
#define _CRC 0x6024
  bit_advance_position(&bitchain, -2);
  {
    unsigned int crc = bit_write_CRC(&bitchain, 0, 0x64);
    if (crc == _CRC)
      pass("bit_write_CRC");
    else
      fail("bit_write_CRC %X", crc);
  }

  bit_advance_position(&bitchain, -16);
  if (bit_check_CRC(&bitchain, 0, 0x64))
    pass("bit_check_CRC");
  else
    fail("bit_check_CRC");

  bit_advance_position(&bitchain, -16);
  if ((bs = bit_read_CRC(&bitchain)) == _CRC)
    pass("bit_read_CRC");
  else
    fail("bit_read_CRC %X", bs);
  bit_advance_position(&bitchain, -16);

  if ((bs = bit_calc_CRC(0x64, (unsigned char *)&bitchain.chain, 0)) == 0x64)
    pass("bit_calc_CRC");
  else
    fail("bit_calc_CRC %X", bs);
  bit_advance_position(&bitchain, 16);

  bit_write_TV(&bitchain, (char*)"GNU"); //now we store the \0 also
  if (bitchain.byte == 70 && bitchain.bit == 2)
    pass("bit_write_TV");
  else
    fail("bit_write_TV @%ld.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -42);
  if (!strcmp((const char*) bit_read_TV(&bitchain), "GNU"))
    pass("bit_read_TV");
  else
    fail("bit_read_TV");
  bit_advance_position(&bitchain, -8);

  bit_write_L(&bitchain, 20);
  if (bitchain.byte == 73 && bitchain.bit == 2)
    pass("bit_write_L");
  else
    fail("bit_write_L @%ld.%d", bitchain.byte, bitchain.bit);

  bit_advance_position(&bitchain, -32);
  if ((bl = bit_read_L(&bitchain)) == 20)
    pass("bit_read_L");
  else
    fail("bit_read_L %ul", bl);

  pos = bit_position(&bitchain);
  {
    Dwg_Color color;
    bitchain.version = R_2000;
    color.index = 19;
    color.rgb = 5190965;
    color.flag = 1;
    color.name = (char*)"Some name";
    color.book_name = (char*)"book_name";
    bit_write_CMC(&bitchain, &color);
    if (bitchain.byte == 74 && bitchain.bit == 4)
      pass("bit_write_CMC r2000");
    else
      fail("bit_write_CMC @%ld.%d", bitchain.byte, bitchain.bit);
  }
  size = bit_position(&bitchain) - pos;
  if (size == 10)
    pass ("CMC size");
  else
    fail ("CMC size: %ld", size);

  bit_advance_position(&bitchain, -size);
  {
    Dwg_Color color_read;
    bit_read_CMC(&bitchain, &color_read);

    if (color_read.index == 19)
      pass("bit_read_CMC");
    else
      fail("bit_read_CMC %d", color_read.index);
  }

  bit_advance_position(&bitchain, -size);
  {
    Dwg_Color color;
    bitchain.version = R_2004;
    color.index = 19;
    color.rgb = 5190965;
    color.flag = 1;
    color.name = (char*)"Some name";
    color.book_name = (char*)"book_name";
    bit_write_CMC(&bitchain, &color);
    if (bitchain.byte == 91 && bitchain.bit == 0)
      pass("bit_write_CMC r2004");
    else
      fail("bit_write_CMC @%ld.%d", bitchain.byte, bitchain.bit);

    bit_set_position(&bitchain, pos);
    bit_read_CMC(&bitchain, &color);

    if (color.index == 19 &&
        color.rgb == 5190965 &&
        color.flag == 1 &&
        !strcmp(color.name, "Some name"))
      pass("bit_read_CMC r2004");
    else
      fail("bit_read_CMC (%d,%ld,%d) %s",
           color.index, color.rgb, color.flag, color.name);
  }

  bitchain.byte++;
  bitchain.bit = 0;
  {
    bit_write_sentinel(&bitchain, sentinel);

    if (bitchain.chain[--bitchain.byte] == 0x5F)
      pass("bit_write_sentinel");
    else
      fail("bit_write_sentinel 0x%X", bitchain.chain[bitchain.byte]);
  }

  bitchain.bit = 0;
  bitchain.byte = 0;
  {
    int ret = bit_search_sentinel(&bitchain, sentinel);
    if (bitchain.byte == 108)
      pass("bit_search_sentinel");
    else
      fail("bit_search_sentinel %d", bitchain.byte);
  }
  {
    unsigned int check = bit_calc_CRC(0xC0C1, (unsigned char *)bitchain.chain, 108);
    if (check == 0xEB57)
      pass("bit_calc_CRC");
    else
      fail("bit_calc_CRC %04X", check);
  }

  bit_chain_alloc(&bitchain);
  if (bitchain.size == 82020)
    pass("bit_chain_alloc");
  else
    fail("bit_chain_alloc %ld", bitchain.size);

  {
    dwg_wchar_t w[] = { 0x4e2d, 0x5fc3, 0x7ebf, 0 };
    unsigned char expect[] = { 0xe4,0xb8,0xad, 0xe5,0xbf,0x83, 0xe7,0xba,0xbf, 0 };
    unsigned char *s = (unsigned char *)bit_convert_TU(w);
    int fails = 0;
    for (unsigned int i=0; i<sizeof(expect); i++)
      {
        if (s[i] != expect[i])
          {
            fails++;
            fail("bit_convert_TU s[%d] %x != %x", i, s[i], expect[i]);
          }
      }
    if (!fails)
      pass("bit_convert_TU");
    free(s);
  }

  free (bitchain.chain);
  return 0;
}

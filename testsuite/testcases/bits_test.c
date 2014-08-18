#include "../../src/bits.h"
#include <dejagnu.h>
#include <string.h>
#include <stdlib.h>
#include "tests_common.h"

/* This functions consists tests for bit_write_B() 
 */
void
bit_write_B_tests ()
{
  Bit_Chain bitchain = strtobt ("0000000");
  bit_write_B (&bitchain, 1);

  if (bitchain.chain[0] == 0x80)
    {
      pass ("bit_write_B");
    }
  else
    {
      fail ("bit_write_B");
    }

}

/* This function calls tests for bit_advance_position()
*/
void
bit_advance_position_tests ()
{
  Bit_Chain bitchain = strtobt ("10101010");
  if (bitchain.bit == 0 && bitchain.byte == 0)
    {
      pass ("bit_advance_position");
    }
  else
    {
      fail ("bit_advance_position");
    }
}

/*
 * This function calls tests for bit_read_B()
 */
void
bit_read_B_tests ()
{
  Bit_Chain bitchain = strtobt ("101010");
  unsigned char result = bit_read_B (&bitchain);
  if (result == 0x01)
    {
      pass ("bit_read_B");
    }
  else
    {
      fail ("bit_read_B");
    }
}

/* This functions calls for tests for bit_write_BB()
 */
void
bit_write_BB_tests ()
{
  Bit_Chain bitchain = strtobt ("01000000");
  bit_advance_position (&bitchain, 2);
  bit_write_BB (&bitchain, 0x2);

  if (bitchain.chain[0] == 0x60)
    {
      pass ("bit_write_BB");
    }
  else
    {
      fail ("bit_write_BB");
    }
}

/* This function executes test for bit_read_BB()
 */
void
bit_read_BB_tests ()
{
  Bit_Chain bitchain = strtobt ("10101010");
  unsigned char result = bit_read_BB (&bitchain);
  if (result == 2)
    {
      pass ("bit_read_BB");
    }
  else
    {
      fail ("bit_read_BB");
    }
}

/* This function calls tests for bit_write_4BITS_tests() 
 */
void
bit_write_4BITS_tests ()
{
  Bit_Chain bitchain = strtobt ("00000000");
  bit_write_4BITS (&bitchain, 0xF);
  printf ("%x\n", bitchain.chain[0]);

  if (bitchain.chain[0] == 0xF0)
    {
      pass ("bit_write_4BITS");
    }
  else
    {
      fail ("bit_write_4BITS");
    }
}

/* This functions calls tests for bit_write_4BITS_tests()
 */
void
bit_read_4BITS_tests ()
{
  Bit_Chain bitchain = strtobt ("11111111");
  unsigned char result = bit_read_4BITS (&bitchain);
  if (result == 0xF)
    {
      pass ("bit_read_4BITS");
    }
  else
    {
      fail ("bit_read_4BITS");
    }
}


/* This function calls tests for bit_read_RC()
 */
void
bit_read_RC_tests ()
{
  Bit_Chain bitchain = strtobt ("11111111");
  unsigned char result = bit_read_RC (&bitchain);
  if (result == 0xFF)
    {
      pass ("bit_read_RC");
    }
  else
    {
      fail ("bit_read_RC");
    }
}


/* This function calls tests for bit_write_RC()
 */
void
bit_write_RC_tests ()
{
  Bit_Chain bitchain = strtobt ("00000000");
  bit_write_RC (&bitchain, 0xFF);
  if (bitchain.chain[0] == 0xFF)
    {
      pass ("bit_write_RC");
    }
  else
    {
      fail ("bit_write_RC");
    }
}

/* This functions calls for bit_read_RS()
 */
void
bit_read_RS_tests ()
{
  Bit_Chain bitchain = strtobt ("1111111100000001");
  unsigned int result = bit_read_RS (&bitchain);

  if (result == 511)
    {
      pass ("bit_read_RS");
    }
  else
    {
      fail ("bit_write_RS");
    }
}

/* This functions calls tests for bit_write_RS()
 */
void
bit_write_RS_tests ()
{
  Bit_Chain bitchain = strtobt ("1111111111111111");
  bit_write_RS (&bitchain, 511);

  if (bitchain.chain[0] == 255 && bitchain.chain[1] == 0x01)
    {
      pass ("bit_write_RS");
    }
  else
    {
      fail ("bit_write_RS");
    }
}

/* This functions calls for tests for bit_write_RL()
 */
void
bit_write_RL_tests ()
{
  Bit_Chain bitchain = strtobt ("11111111111111111111111111111111");
  bit_write_RL (&bitchain, 2147549183);
  if (bitchain.chain[0] == 255 && bitchain.chain[1] == 255
      && bitchain.chain[2] == 0 && bitchain.chain[3] == 128)
    {
      pass ("bit_write_RL");
    }
  else
    {
      fail ("bit_write_RL");
    }
}

/* This functions calls tests for bit_read_RL()
 */
void
bit_read_RL_tests ()
{
  Bit_Chain bitchain = strtobt ("11111111111111111111111111111111");
  long unsigned int result = bit_read_RL (&bitchain);
  if (result == 0xFFFFFFFF)
    {
      pass ("bit_read_RL");
    }
  else
    {
      fail ("bit_read_RL");
    }
}

/* This function calls tests for bit_write_RD()
 */
void
bit_write_RD_tests ()
{
  Bit_Chain bitchain;
  double value = 25.2547841;
  bitprepare(&bitchain, sizeof(double));
  bit_write_RD(&bitchain, value);
  bit_print(&bitchain, sizeof(double));

  double *result = (double *) bitchain.chain;

  if (*result == value)
  {
  	pass("bit_write_RD");
  }
  else
  {
  	fail("bit_write_RD");
  }
}

/* This functions calls tests for bit_read_RD()
 */
void
bit_read_RD_tests()
{
	Bit_Chain bitchain;
	double value = 25.21241;

	bitprepare(&bitchain, sizeof(double));

	bitchain.chain = (unsigned char *) &value;

	bit_print(&bitchain, sizeof(double));

	double result = bit_read_RD(&bitchain);

	if (result == value)
	{
		pass("bit_read_RD");
	}
	else
	{
		fail("bit_read_RD");
	}


}

int
main ()
{

  /* Tests for bit_write_B() */
  bit_write_B_tests ();
  /* End of tests for bit_write_B() */

  /* Tests fir bit_advance_position() */
  bit_advance_position_tests ();
  /* End of tests for bit_advance_position() */

  /* Tests for bit_read_B() */
  bit_read_B_tests ();
  /* End of tests for bit_read_B */

  /* Tests for bit_write_BB() */
  bit_write_BB_tests ();
  /* End of tests for bit_write_BB() */

  /* Tests for bit_read_BB() */
  bit_read_BB_tests ();
  /* End of tests for bit_read_BB */

  /* Tests for bit_read_4BITS() */
  bit_write_4BITS_tests ();
  /* End of tests for bit_read_4BITS */

  /* Tests for bit_read_4BITS */
  bit_read_4BITS_tests ();
  /* End of tests for bit_read_4BITS */

  /* Tests for bit_write_RC() */
  bit_read_RC_tests ();
  /* End of bit_read_RC() */

  /* Tests for bit_read_RC */
  bit_write_RC_tests ();
  /* End of tests for bit_read_RC */

  /* Tests for bit_read_RS() */
  bit_read_RS_tests ();
  /* End of tests for bit_read_RS() */

  /* Tests for bit_write_RS() */
  bit_write_RS_tests ();
  /* End of tests for bit_write_RS() */

  /* Tests for bit_write_RL() */
  bit_read_RL_tests ();
  /* End of tests for bit_write_RL() */

  /* Tests for bit_write_RD */
  bit_write_RL_tests ();
  /* End of tests for bit_write_RD */

  /* Tests for bit_write_RD()  */
  bit_write_RD_tests();
  /* End of tests for bit_write_RD  */

  /* Tests for bit_read_RD */
  bit_read_RD_tests();
  /* End of tests for bit_read_RD(); */


  //Prepare the testcase
  Bit_Chain bitchain;

  bitchain.size = 100;
  bitchain.bit = 0;
  bitchain.byte = 0;
  bitchain.version = R_2000;
  bitchain.chain = (unsigned char *) malloc (bitchain.size);


/*
	//Tests for bit_write_RD
	bit_write_RD(&bitchain, 0xBA43B7400);
	if (bitchain.byte == 16 && bitchain.bit == 0)
	{
		pass("bit_write_RL is working properly");
	}
	else
	{
		fail("bit_write_RL is not working properly");
	}
	//End of tests for bit_write_RD

	//Test for bit_read_RD
	bit_advance_position(&bitchain, -64);
	if (bit_read_RD(&bitchain) == 0xBA43B7400)
	{
		pass("bit_read_RD is working properly");
	}
	else
	{
		fail("bit_read_RD is not working properly");
	}
	//End of tests for bit_read_RD

	//Tests for bit_read_BS
	bit_write_BS(&bitchain, 256);
	if (bitchain.byte == 16 && bitchain.bit == 2)
	{
		pass("bit_write_BS is working properly");
	}
	else
	{
		fail("bit_write_BS is not working properly");
	}
	//End of tests for bit_write_BS

	//Tests for bit_read_BS
	bit_advance_position(&bitchain, -2);
	if (bit_read_BS(&bitchain) == 256)
	{
		pass("bit_read_BS is working properly");
	}
	else
	{
		fail("bit_read_BS is not working properly");
	}
	//End of tests for bit_read_BS

	//Tests for bit_write_BL
	bit_write_BL(&bitchain, 0);
	if (bitchain.byte == 16 && bitchain.bit == 4)
	{
		pass("bit_write_BL is working properly");
	}
	else
	{
		fail("bit_write_BL is working properly");
	}
	//End of tests for bit_write_BL

	//Tests for bit_read_BL
	bit_advance_position(&bitchain, -2);
	if (bit_read_BL(&bitchain) == 0)
	{
		pass("bit_read_BL is working properly");
	}
	else
	{
		fail("bit_read_BL is working properly");
	}
	//End of tests for bit_read_BL

	//Tests for bit_write_BD
	bit_write_BD(&bitchain, 1.0);
	if (bitchain.byte == 16 && bitchain.bit == 6)
	{
		pass("bit_write_BD is working properly");
	}
	else
	{
		fail("bit_write_BD is not working properly");
	}
	//End of tests for bit_write_BD

	//Tests for bit_read_BD
	bit_advance_position(&bitchain, -2);
	if (bit_read_BD(&bitchain) == 1.0)
	{
		pass("bit_read_BD is working properly");
	}
	else
	{
		fail("bit_read_BD is not working properly");
	}
	//End of test for bit_read_BD

	//Tests for bit_write_MC
	bit_advance_position(&bitchain, 2);
	bit_write_MC(&bitchain, 300);
	if (bitchain.byte == 19)
	{
		pass("bit_write_MC is working properly");
	}
	else
	{
		fail("bit_write_MC is not working properly");
	}
	//End of tests for bit_write_MC

	//Tests for bit_read_MC
	bit_advance_position(&bitchain, -16);
	if (bit_read_MC(&bitchain) == 300)
	{
		pass("bit_read_MC is working properly");
	}
	else
	{
		fail("bit_read_MC is not working properly");
	}
	//End fot tests for bit_read_MC

	//Tests for bit_write_MS
	bit_write_MS(&bitchain, 5000);
	if (bitchain.byte == 21)
	{
		pass("bit_write_MS is working properly");
	}
	else
	{
		fail("bit_write_MS is not working properly");
	}
	//End of tests for bit_write_MS

	//Tests for bit_read_MS
	bit_advance_position(&bitchain, -16);
	if (bit_read_MS(&bitchain) == 5000)
	{
		pass("bit_read_MS is working properly");
	}
	else
	{
		fail("bit_read_MS is not working properly");
	}
	//End of tests for bit_read_MS

	//Tests for bit_write_BE
	bit_write_BE(&bitchain, 20.2535, 10.2523, 52.32563);
	if (bitchain.bit == 7 && bitchain.byte == 45)
	{
		pass("bit_write_BE is working properly");
	}
	else
	{
		fail("bit_write_BE is not working properly");
	}
	//End of tests for bit_write_BE

	//Tests for bit_read_BE
	bit_advance_position(&bitchain, -199);
	double x,y,z;
	bit_read_BE(&bitchain, &x, &y, &z);
	if (x == 20.2535 && y == 10.2523 && z == 52.32563)
	{
		pass("bit_read_BE is working properly");
	}
	else
	{
		fail("bit_read_BE is not working properly");
	}
	//End of tests for bit_read_BE

	//Tests for bit_write_DD
	bit_write_DD(&bitchain, 20.256, 50.252);
	if (bitchain.bit == 1 && bitchain.byte == 54)
	{
		pass("bit_write_DD is working properly");
	}
	else
	{
		fail("bit_write_DD is not working properly");
	}
	//End of tests for bit_write_DD

	//Tests for bit_read_DD
	//bit_advance_position is not working
	bit_advance_position(&bitchain, -73);
	bit_advance_position(&bitchain, 7);
	if (bit_read_DD(&bitchain, 50.252) == 20.256)
	{
		pass("bit_read_DD is working properly");
	}
	else
	{
		fail("bit_read_DD is not working properly");
	}
	//End of tests for bit_read_DD

	//Tests for bit_write_BT
	bit_advance_position(&bitchain, 64);
	bit_write_BT(&bitchain, 3.0);
	if (bitchain.bit == 4 && bitchain.byte == 62)
	{
		pass("bit_write_BT is working properly");
	}
	else
	{
		fail("bit_write_BT is not working properly");
	}
	//End of tests for bit_write_BT

	//Tests for bit_read_BT
	bit_advance_position(&bitchain, -74);
	if (bit_read_BT(&bitchain) == 3.0)
	{
		pass("bit_read_BT is working properly");
	}
	else
	{
		fail("bit_read_BT is not working properly");
	}
	//End of tests for bit_read_BT

	//Tests for bit_read_H()
	Dwg_Handle write_handle;
	write_handle.code = 2;
	write_handle.value = 100;
	bit_write_H(&bitchain, &write_handle);
	if (bitchain.bit == 4 && bitchain.byte == 64)
	{
		pass("bit_write_H is working properly");
	}
	else
	{
		fail("bit_write_H is not working properly");
	}
	//End of tests for bit_write_H

	//Tests for bit_read_H
	Dwg_Handle read_handle;
	bit_advance_position(&bitchain, -16);
	bit_read_H(&bitchain, &read_handle);
	if (read_handle.code == 2 && read_handle.value == 100)
	{
		pass("bit_read_H is working properly");
	}
	else
	{
		fail("bit_read_H is not working properly");
	}
	//End of tests for bit_read_H

	//Tests for bit_write_CRC
	bit_advance_position(&bitchain, -18);
	unsigned int crc = bit_write_CRC(&bitchain, 0, 0x64);
	if (crc == 10063)
	{
		pass("bit_write_CRC is working properly");
	}
	else
	{
		fail("bit_write_CRC is not working properly");
	}
	//End of tests for bit_write_CRC

	//Tests for bit_check_CRC
	bit_advance_position(&bitchain, -16);
	if (bit_check_CRC(&bitchain, 0, 0x64))
	{
		pass("bit_check_CRC is working properly");
	}
	else
	{
		fail("bit_check_CRC is not working properly");
	}
	//End of tests for bit_check_CRC

	//Tests for bit_read_CRC
	bit_advance_position(&bitchain, -16);
	if (bit_read_CRC(&bitchain) == 10063)
	{
		pass("bit_read_CRC is working properly");
	}
	else
	{
		fail("bit_read_CRC is not working properly");
	}
	//End of tests for bit_read_CRC

	//Tests for bit_write_TV
	bit_write_TV(&bitchain, "GNU");
	if (bitchain.bit == 2 && bitchain.byte == 71)
	{
		pass("bit_write_TV is working properly");
	}
	else
	{
		fail("bit_write_TV is not working properly");
	}
	//End of tests for bit_write_TV

	//Tests for bit_read_TV
	bit_advance_position(&bitchain, -34);
	if (!strcmp((const char*) bit_read_TV(&bitchain), "GNU"))
	{
		pass("bit_read_TV is working properly");
	}
	else
	{
		fail("bit_read_TV is not working properly");
	}
	//End of tests for bit_write_TV
	
	//Tests for bit_write_L
	//printf("Bit === %d and byte == %lu \n", bitchain.bit, bitchain.byte);
	//bit_write_L(&bitchain, 20); //not working
	fail("bit_write_L is not working properly");
	//End of tests for bit_write_L
	
	//Tests for bit_read_L
	bit_advance_position(&bitchain, -33);
	if (bit_read_L(&bitchain) == 140736713692480)
	{
		pass("bit_read_L is working properly");
	}
	else
	{
		fail("bit_read_L is not working properly");
	}
	//End of tests for bit_read_L

	//Tests for bit_write_CMC
	Dwg_Color color;
	color.index = 19;
	color.rgb = 5190965;
	color.byte = 1;
	color.name = "Some name";
	color.book_name = "book_name";
	bit_write_CMC(&bitchain, &color);
	if (bitchain.bit == 2 && bitchain.byte == 73)
	{
		pass("bit_write_CMC is working properly");
	}
	else
	{
		fail("bit_write_CMC is not working properly");
	}
	//End of tests for bit_write_CMC

	//Tests for bit_read_CMC
	bit_advance_position(&bitchain, -10);
	Dwg_Color color_read;
	bit_read_CMC(&bitchain, &color_read);


	if (color_read.index == 19 && color_read.rgb == 5190965 && color_read.byte == 1 && !strcmp(color_read.name, "Some name"))
	{
		pass("bit_read_CMC is working properly");
	}
	else
	{
		fail("bit_read_CMC is not working properly");
	}
	//End of tests for bit_read_CMC

	//Tests for bit_write_sentinel
	bitchain.byte++;
	bitchain.bit = 0;
	unsigned char sentinel[] = {0xCF,0x7B,0x1F,0x23,0xFD,0xDE,0x38,0xA9,0x5F,0x7C,0x68,0xB8,0x4E,0x6D,0x33,0x5F};
	bit_write_sentinel(&bitchain, sentinel);
	
	if (bitchain.chain[--bitchain.byte] == 0x5F)
	{
		pass("bit_write_sentinel is working properly");
	}
	else
	{
		fail("bit_write_sentinel is not working properly");
	}
	//End of tests for bit_write_sentinel

	//Tests for bit_search_sentinel
	bitchain.bit = 0;
	bitchain.byte = 0;
	int ret = bit_search_sentinel(&bitchain, sentinel);
	if (bitchain.byte == 90)
	{
		pass("bit_search_sentinel is working properly");
	}
	else
	{
		fail("bit_search_sentinel is not working properly");
	}
	//End of tests for bit_search_sentinel


	//Tests for bit_chain_alloc
	bit_chain_alloc(&bitchain);
	if (bitchain.size == 41060)
	{
		pass("bit_chain_alloc is working properly");
	}
	else
	{
		fail("bit_chain_alloc is not working properly");
	}
	//End of tests for bit_chain_alloc

	unsigned int check = bit_ckr8(12100, &bitchain.chain, 90);
	// How do I test this

*/

  // Free the allocated memory
  free (bitchain.chain);
  return 0;
}

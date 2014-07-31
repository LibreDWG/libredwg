#include "../../src/bits.h"
#include <dejagnu.h>
#include <string.h>
#include <stdlib.h>

int
main()
{
	//Lets write some tests


	//Prepare the testcase
	Bit_Chain bitchain;

	bitchain.size = 100;
	bitchain.bit = 0;
	bitchain.byte = 0;
	bitchain.version = R_2000;
	bitchain.chain = (unsigned char *) malloc(bitchain.size);
	printf("%lu\n", sizeof(bitchain.chain));

	//Tests for bit_write_B()
	bit_write_B(&bitchain, 0x01);
	if (bitchain.bit == 1)
	{
		pass("bit_write_B is working properly");
	}
	else
	{
		fail("bit_write_B is not working properly");
	}
	//End of test for bit_write_B()

	//Test for bit_advance_position
	bit_advance_position(&bitchain, -1);
	if (bitchain.bit == 0)
	{
		pass("bit_advance_position is working properly");
	}
	else
	{
		fail("bit_advance_position is not working properly");
	}
	//End of test for bit_advance_position

	unsigned char result = bit_read_B(&bitchain);
	if (result == 0x01)
	{
		pass("bit_read_B is working");
	}
	else
	{
		fail("bit_read_B is not working properly");
	}

	//Tests for bit_write_BB
	bit_write_BB(&bitchain, 0x02);
	if (bitchain.bit == 3)
	{
		pass("bit_write_BB is working properly");
	}
	else
	{
		fail("bit_write_BB is not working properly");
	}

	//Tests for bit_read_BB()
	bit_advance_position(&bitchain, -2);
	if (bit_read_BB(&bitchain) == 0x02)
	{
		pass("bit_read_BB is working properly");
	}
	else
	{
		fail("bit_read_BB is not working properly");
	}
	//End of test of bit_read_BB()

	//Test for bit_write_4BITS
	bit_write_4BITS(&bitchain, 0xF);
	if (bitchain.bit == 7)
	{
		pass("bit_write_4BITS is working properly");
	}
	else
	{
		fail("bit_write_4BITS is not working properly");
	}
	//End of tests for bit_write_4BITS

	//Test for bit_read_4BITS
	bit_advance_position(&bitchain, -7);
	if (bit_read_4BITS(&bitchain) == 0xC)
	{
		pass("bit_read_4BITS is working properly");
	}
	else
	{
		fail("bit_read_4BITS is not working properly");
	}
	//End of test for bit_read_4BITS

	//Tests of bit_write_RC
	bitchain.bit = 0;
	bitchain.byte = 1;
	bit_write_RC(&bitchain, 0xC8);
	
	if (bitchain.bit == 0 && bitchain.byte == 2)
	{
		pass("bit_write_RC is working properly");
	}
	else
	{
		fail("bit_write_RC is not working properly");
	}
	//End of Tests for bit_write_RC

	//Tests for bit_read_RC
	bit_advance_position(&bitchain, -8);
	if (bit_read_RC(&bitchain) == 0xC8)
	{
		pass("bit_read_RC is working properly");
	}
	else
	{
		fail("bit_read_RC is not working properly");
	}
	//End of tests for bit_read_RC

	//Tests for bit_write_RS()
	bit_write_RS(&bitchain, 0x1F4);
	if (bitchain.byte == 4 && bitchain.bit == 0)
	{
		pass("bit_write_RS is working properly");
	}
	else
	{
		fail("bit_write_RS is not working properly");
	}
	//end of test for bit_write_RS

	//Tests for bit_read_RS()
	bit_advance_position(&bitchain, -16);
	if (bit_read_RS(&bitchain) == 0x1F4)
	{
		pass("bit_write_RS is working properly");
	}
	else
	{
		fail("bit_write_RS is not working properly");
	}
	//End of tests for bit_read_RS()

	//Tests for bit_write_RL()
	bit_write_RL(&bitchain, 0x7A120);
	if (bitchain.byte == 8 && bitchain.bit == 0)
	{
		pass("bit_write_RL is working properly");
	}
	else
	{
		fail("bit_write_RL is not working properly");
	}
	//End of tests for bit_write_RL()

	//Tests for bit_read_RL
	bit_advance_position(&bitchain, -32);
	if (bit_read_RL(&bitchain) == 0x7A120)
	{
		pass("bit_read_RL is working properly");
	}
	else
	{
		fail("bit_read_RL is not working properly");
	}
	//End of tests for bit_read_RL

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



	// Free the allocated memory
	free(bitchain.chain);
	return 0;
}

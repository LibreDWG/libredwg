#include "../../src/bits.h"
#include<dejagnu.h>

int
main()
{
	//Lets write some tests


	//Prepare the testcase
	char *bits = "011010110101010101010101010101000010101";
	Bit_Chain bitchain;
	bitchain.chain = bits;
	bitchain.size = sizeof(bits);
	bitchain.bit = 0;
	bitchain.byte = 0;
	bit_advance_position(&bitchain, 10);
	
	if (bitchain.bit == 2 && bitchain.byte == 1)
	{
		pass("bit_advance_position is working properly");
	}
	else
	{
		fail("bit_advance_position is not working properly");
	}

	return 0;
}

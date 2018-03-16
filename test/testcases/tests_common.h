#include <string.h>
#include "../../src/bits.h"
#include <stdlib.h>
#include <math.h>

/*
 * This functions initializes bitchain and allocates the given
 * size
 * @param Bit_Chain* bitchain
 * @param size_t size
 */
void bitprepare(Bit_Chain * bitchain, size_t size)
{
	bitchain->bit = 0;
	bitchain->byte = 0;
	//bitchain.version = R_2000;
	bitchain->size = size;
	bitchain->chain = (unsigned char *) malloc(size);

}


/*
 * This function converts the given binary string to binary
 * value
 * for eg "010101" => 010101
 * and puts it in bitchain to be examined
 */
Bit_Chain strtobt(const char * binarystring)
{
	Bit_Chain bitchain;

	int length = strlen(binarystring);

	//Calculate the space needed
	double celi = ceil((double)length/8);
	int size_need = celi;

	//Prepare bitchain
	bitprepare(&bitchain, size_need);


	int i = 0;
	for (i = 0; i < length; ++i)
	{
		if (binarystring[i] == '0')
		{
			bit_write_B(&bitchain, 0);
		}
		else
		{
			bit_write_B(&bitchain, 1);
		}
	}

	bit_print(&bitchain, size_need);

	//Reset the bit position
	bitchain.bit = 0;
	bitchain.byte = 0;

	return bitchain;
}

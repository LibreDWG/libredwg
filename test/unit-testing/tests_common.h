#include <string.h>
#include "../../src/bits.h"
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

static int num = 0;
static int failed = 0;
static int passed = 0;
static char buffer[512];

static inline void pass (void);
static void fail (const char* fmt, ...)
#ifdef HAVE_FUNC_ATTRIBUTE_FORMAT
  __attribute__((format(printf, 1, 2)))
#endif
  ;

static inline void pass (void)
{
  passed++; num++;
}

static void fail (const char* fmt, ...)
{
  va_list ap;

  failed++;
  va_start (ap, fmt);
  vsnprintf (buffer, sizeof (buffer), fmt, ap);
  va_end (ap);
  printf ("not ok %d\t# %s\n", ++num, buffer);
}

void bitprepare (Bit_Chain *bitchain, size_t size);
Bit_Chain strtobt (const char *binarystring);

/*
 * This functions initializes bitchain and allocates the given
 * size
 * @param Bit_Chain* bitchain
 * @param size_t size
 */
void
bitprepare (Bit_Chain *bitchain, size_t size)
{
  bitchain->bit = 0;
  bitchain->byte = 0;
  bitchain->version = R_2000;
  bitchain->size = size;
  bitchain->chain = (unsigned char *)calloc (size, 1);
}

/*
 * This function converts the given binary string to binary
 * value
 * for eg "010101" => 010101
 * and puts it in bitchain to be examined
 */
Bit_Chain
strtobt (const char *binarystring)
{
  Bit_Chain bitchain;
  int i;
  int length = strlen (binarystring);

  // Calculate the space needed
  double celi = ceil ((double)length / 8);
  int size_need = celi;

  // Prepare bitchain
  bitprepare (&bitchain, size_need);

  for (i = 0; i < length; ++i)
    {
      if (binarystring[i] == '0')
        bit_write_B (&bitchain, 0);
      else
        bit_write_B (&bitchain, 1);
    }

  //bit_print (&bitchain, size_need);

  // Reset the bit position
  bitchain.bit = 0;
  bitchain.byte = 0;

  return bitchain;
}

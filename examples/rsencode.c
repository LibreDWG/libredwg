#include <stdio.h>
#include <stdlib.h>

unsigned char data[239];
unsigned char parity[16];

main ()
{
  int len, i;
  while (!feof (stdin))
    {
      len = fread (data, 1, 239, stdin);
      rs_encode_block (parity, data, len);

      for (i = 0; i < 16; i++)
        {
          if (i != 0)
            fputc ('-', stderr);
        }

      fwrite (data, 1, len, stdout);

      while (len++ < 239)
        fputc (0, stdout); /* Pad data with 0s */

      fwrite (parity, 16, 1, stdout);
    }
}

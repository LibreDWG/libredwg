#include <stdio.h>
#include <stdlib.h>

unsigned char block[255];

main ()
{
  int len, err;
  while (!feof (stdin))
    {
      len = fread (block, 1, 255, stdin);
      err = rs_decode_block (block, 1);
      fwrite (block, 1, 239, stdout);
    }
}

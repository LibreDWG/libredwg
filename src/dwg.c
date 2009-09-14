/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*  http://code.google.com/p/libredwg/                                       */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Felipe Sanches <jucablues@users.sourceforge.net>      */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either versionn 3 of the License, or (at your option) any later versionn.*/
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/* Main source file of the library, whith the API functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "bits.h"
#include "common.h"
#include "decode.h"
#include "print.h"
#include "dwg.h"

/*------------------------------------------------------------------------------
 * Public functions
 */
int
dwg_read_file(char *filename, Dwg_Structure * dwg_struct)
{
  int sign;
  FILE *fp;
  struct stat attrib;
  size_t size;
  Bit_Chain bit_chain;

  if (stat(filename, &attrib))
    {
      fprintf(stderr, "File not found: %s\n", filename);
      return -1;
    }
  if (!S_ISREG (attrib.st_mode))
    {
      fprintf(stderr, "Error: %s\n", filename);
      return -1;
    }
  fp = fopen(filename, "rb");
  if (!fp)
    {
      fprintf(stderr, "Error while opening the file: %s\n", filename);
      return -1;
    }

  /* Load file to memory
   */
  bit_chain.bit = 0;
  bit_chain.byte = 0;
  bit_chain.size = attrib.st_size;
  bit_chain.chain = (char *) malloc(bit_chain.size);
  if (!bit_chain.chain)
    {
      fprintf(stderr, "Not enough memory.\n");
      fclose(fp);
      return -1;
    }
  size = 0;
  size = fread(bit_chain.chain, sizeof(char), bit_chain.size, fp);
  if (size != bit_chain.size)
    {
      fprintf(stderr, "Could not read the entire file (%lu out of %lu): %s\n",
          (long unsigned int) size, bit_chain.size, filename);
      fclose(fp);
      free(bit_chain.chain);
      return -1;
    }
  fclose(fp);

  /* Decode the dwg structure
   */
  if (dwg_decode_structures(&bit_chain, dwg_struct))
    {
      fprintf(stderr, "Failed to decode file: %s\n", filename);
      free(bit_chain.chain);
      return -1;
    }
  free(bit_chain.chain);

  return 0;
}

int
dwg_write_file(char *filename, Dwg_Structure * dwg_struct)
{
  FILE *dt;
  struct stat atrib;
  Bit_Chain bit_chain;
  bit_chain.version = dwg_struct->header.version;

  /* Encode the DWG struct
   bit_chain.size = 0;
   if (dwg_encode_chains (dwg_struct, &bit_chain))
   {
   fprintf (stderr, "Failed to encode datastructure.\n");
   if (bit_chain.size > 0)
   free (bit_chain.chain);
   return -1;
   }
   */

  /* try opening the output file in write mode
   if (!stat (filename, &atrib))
   {
   fprintf (stderr, "The file already exists. We won't overwrite it.");
   return -1;
   }
   dt = fopen (filename, "w");
   if (!dt)
   {
   fprintf (stderr, "Failed to create the file: %s\n", filename);
   return -1;
   }
   */

  /* Write the data into the file
   if (fwrite (bit_chain.chain, sizeof (char), bit_chain.size, dt) != bit_chain.size)
   {
   fprintf (stderr, "Failed to write data into the file: %s\n", filename);
   fclose (dt);
   free (bit_chain.chain);
   return -1;
   }
   fclose (dt);

   if (bit_chain.size > 0)
   free (bit_chain.chain);
   */
  return 0;
}

unsigned char *
dwg_bmp(Dwg_Structure *stk, long int *size)
{
  char num_pictures;
  char code;
  unsigned i;
  int plene;
  long int header_size;
  Bit_Chain *dat;

  dat = (Bit_Chain*) &stk->picture;
  dat->bit = 0;
  dat->byte = 0;

  bit_read_RL(dat);
  num_pictures = bit_read_RC(dat);
  //printf ("num_pictures: %i\n", num_pictures);

  *size = 0;
  plene = 0;
  header_size = 0;
  for (i = 0; i < num_pictures; i++)
    {
      code = bit_read_RC(dat);
      //printf ("\t%i - Code: %i\n", i, code);
      //printf ("\t\tAdress: 0x%x\n", bit_read_RL (dat));
      bit_read_RL(dat);
      if (code == 1)
        {
          header_size += bit_read_RL(dat);
          //printf ("\t\tHeader size: %i\n", header_size);
        }
      else if (code == 2 && plene == 0)
        {
          *size = bit_read_RL(dat);
          plene = 1;
          //printf ("\t\tBMP size: %i\n", *size);
        }
      else if (code == 3)
        {
          bit_read_RL(dat);
          //printf ("\t\tWMF size: 0x%x\n", bit_legi_RL (dat));
        }
      else
        {
          bit_read_RL(dat);
          //printf ("\t\tSize: 0x%x\n", bit_read_RL (dat));
        }
    }
  dat->byte += header_size;
  //printf ("Current adress: 0x%x\n", dat->byte);

  if (*size > 0)
    return (dat->chain + dat->byte);
  else
    return NULL;
}

double
dwg_model_x_min(Dwg_Structure *dwg)
{
  return dwg->var[116].xyz[0];
}

double
dwg_model_x_max(Dwg_Structure *dwg)
{
  return dwg->var[117].xyz[0];
}

double
dwg_model_y_min(Dwg_Structure *dwg)
{
  return dwg->var[116].xyz[1];
}

double
dwg_model_y_max(Dwg_Structure *dwg)
{
  return dwg->var[117].xyz[1];
}

double
dwg_model_z_min(Dwg_Structure *dwg)
{
  return dwg->var[116].xyz[2];
}

double
dwg_model_z_max(Dwg_Structure *dwg)
{
  return dwg->var[117].xyz[2];
}

double
dwg_model_page_x_min(Dwg_Structure *dwg)
{
  return dwg->var[118].xy[0];
}

double
dwg_model_page_x_max(Dwg_Structure *dwg)
{
  return dwg->var[119].xy[0];
}

double
dwg_model_page_y_min(Dwg_Structure *dwg)
{
  return dwg->var[118].xy[1];
}

double
dwg_model_page_y_max(Dwg_Structure *dwg)
{
  return dwg->var[119].xy[1];
}

void
dwg_free(Dwg_Structure * dwg)
{
  if (dwg->header.section)
    free(dwg->header.section);
}

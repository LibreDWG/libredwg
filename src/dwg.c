/*****************************************************************************/
/*  LibDWG - Free DWG read-only library                                      */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either versionn 3 of the License, or (at your option) any later versionn.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/* Cxefa fontkoda dosiero de la biblioteko, kie restas la interfacaj funkcioj.
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
dwg_read_file (char *filename, Dwg_Structure * dwg_struct)
{
	int signo;
	FILE *fp;
	struct stat atrib;
	size_t size;
	Bit_Chain bitaro;

	if (stat (filename, &atrib))
	{
		fprintf (stderr, "File not found: %s\n", filename);
		return -1;
	}
	if (!S_ISREG (atrib.st_mode))
	{
		fprintf (stderr, "Error: %s\n", filename);
		return -1;
	}
	fp = fopen (filename, "rb");
	if (!fp)
	{
		fprintf (stderr, "Error while opening the file: %s\n", filename);
		return -1;
	}

	/* Sxargi la memoron je la dosiero
	 */
	bitaro.bit = 0;
	bitaro.byte = 0;
	bitaro.size = atrib.st_size;
	bitaro.chain = (char *) malloc (bitaro.size);
	if (!bitaro.chain)
	{
		fprintf (stderr, "Not enough memory.\n");
		fclose (fp);
		return -1;
	}
	size = 0;
	size = fread (bitaro.chain, sizeof (char), bitaro.size, fp);
	if (size != bitaro.size)
	{
		fprintf (stderr, "Could not read the entire file (%lu out of %lu): %s\n", (long unsigned int) size, bitaro.size,
			filename);
		fclose (fp);
		free (bitaro.chain);
		return -1;
	}
	fclose (fp);

	/* Dekodigi la dwg-datenaron
	 */
	if (dwg_decode_structures (&bitaro, dwg_struct))
	{
		fprintf (stderr, "Failed to decode file: %s\n", filename);
		free (bitaro.chain);
		return -1;
	}
	free (bitaro.chain);

	return 0;
}

int
dwg_write_file (char *filename, Dwg_Structure * dwg_struct)
{
	FILE *dt;
	struct stat atrib;
	Bit_Chain bitaro;
    bitaro.version = dwg_struct->header.version;

	/* Enkodigi la dwg-datenaron
	bitaro.size = 0;
	if (dwg_encode_chains (dwg_struct, &bitaro))
	{
		fprintf (stderr, "Failed to encode datastructure.\n");
		if (bitaro.size > 0)
			free (bitaro.chain);
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
	if (fwrite (bitaro.chain, sizeof (char), bitaro.size, dt) != bitaro.size)
	{
		fprintf (stderr, "Failed to write data into the file: %s\n", filename);
		fclose (dt);
		free (bitaro.chain);
		return -1;
	}
	fclose (dt);

	if (bitaro.size > 0)
		free (bitaro.chain);
	 */
	return 0;
}

/* Liveras la datumaron de DIB-bitmapo (kap-datumaro plus bitmapo mem).
 * La grandeco (size) ampleksas ambaŭ partoj.
 */

unsigned char *
dwg_bmp (Dwg_Structure *stk, long int *size)
{
	char num_pictures;
	char kodo;
	unsigned i;
	int plene;
	long int size_kapo;
	Bit_Chain *dat;
	
	dat = (Bit_Chain*) &stk->picture;
	dat->bit = 0;
	dat->byte = 0;
 
	bit_read_RL (dat);
	num_pictures = bit_read_RC (dat);
	//printf ("Kiom bildetoj: %i\n", num_pictures);
 
	*size = 0;
	plene = 0;
	size_kapo = 0;
	for (i = 0; i < num_pictures; i++)
 	{
		kodo = bit_read_RC (dat);
		//printf ("\t%i - Kodo: %i\n", i, kodo);
		//printf ("\t\tAdreso: 0x%x\n", bit_legi_RL (dat));
		bit_read_RL (dat);
		if (kodo == 1)
 		{
			size_kapo += bit_read_RL (dat);
			//printf ("\t\tGrandeco de kapo: %i\n", size_kapo);
 		}
		else if (kodo == 2 && plene == 0)
 		{
			*size = bit_read_RL (dat);
			plene = 1;
			//printf ("\t\tGrandeco de BMP: %i\n", *size);
 		}
		else if (kodo == 3)
		{
			bit_read_RL (dat);
			//printf ("\t\tGrandeco de WMF: 0x%x\n", bit_legi_RL (dat));
		}
		else
		{
			bit_read_RL (dat);
			//printf ("\t\tGrandeco: 0x%x\n", bit_read_RL (dat));
		}
 	}
	dat->byte += size_kapo;
	//printf ("Adreso nun: 0x%x\n", dat->byte);
 
	if (*size > 0)
		return (dat->chain + dat->byte);
	else
		return NULL;
}

double dwg_model_x_min(Dwg_Structure *dwg){
	return dwg->var[116].xyz[0];
}

double dwg_model_x_max(Dwg_Structure *dwg){
	return dwg->var[117].xyz[0];
}

double dwg_model_y_min(Dwg_Structure *dwg){
	return dwg->var[116].xyz[1];
}

double dwg_model_y_max(Dwg_Structure *dwg){
	return dwg->var[117].xyz[1];
}

double dwg_model_z_min(Dwg_Structure *dwg){
	return dwg->var[116].xyz[2];
}

double dwg_model_z_max(Dwg_Structure *dwg){
    return dwg->var[117].xyz[2];
}

double dwg_model_page_x_min(Dwg_Structure *dwg){
	return dwg->var[118].xy[0];
}

double dwg_model_page_x_max(Dwg_Structure *dwg){
	return dwg->var[119].xy[0];
}

double dwg_model_page_y_min(Dwg_Structure *dwg){
	return dwg->var[118].xy[1];
}
 
double dwg_model_page_y_max(Dwg_Structure *dwg){
	return dwg->var[119].xy[1];
}

void dwg_free (Dwg_Structure * dwg){
    if (dwg->header.section)
        free(dwg->header.section);
}

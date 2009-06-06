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

/// Decode

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"

/*--------------------------------------------------------------------------------
 * Private functions
 */
static void dwg_decode_aldoni_object (Dwg_Structure * skt, Bit_Chain * dat, long unsigned int address);

/*--------------------------------------------------------------------------------
 * Public variables
 */
long unsigned int ktl_lastaddress;
static int loglevel = 0;

/*--------------------------------------------------------------------------------
 * Public function definitions
 */
int
dwg_decode_structures (Bit_Chain * dat, Dwg_Structure * skt)
{
	unsigned char sig;
	unsigned int seksize = 0;
	unsigned char sgdc[2];
	unsigned int i, j;
	unsigned int ckr, ckr2, antckr;
	long unsigned int size;
	long unsigned int lasta;
	long unsigned int maplasta;
	long unsigned int duabyte;
	long unsigned int obek;
	long unsigned int obfin;
	long unsigned int pvz;

	/* Version */
	dat->byte = 0;
	dat->bit = 0;
	char version[7];
	strncpy (version, dat->chain, 6);
	version[6] = '\0';

	skt->header.version=0;
	if (!strcmp (version, version_codes[R_13])) skt->header.version = R_13;
	if (!strcmp (version, version_codes[R_14])) skt->header.version = R_14;
	if (!strcmp (version, version_codes[R_2000])) skt->header.version = R_2000;
	if (!strcmp (version, version_codes[R_2004])) skt->header.version = R_2004;
	if (!strcmp (version, version_codes[R_2007])) skt->header.version = R_2007;
    if (skt->header.version==0){
        fprintf (stderr, "Invalid or unimplemented version code!"
			"This file's version code is: %s\n", version);
        return -1;
    }
	dat->version = skt->header.version;

	if (skt->header.version != R_2000)
	{
		fprintf (stderr, "This version of Libredwg is only capable of decoding version R2000 (code: AC1015) dwg-files. "
			"This file's version code is: %s\n", version);
		return -1;
	}

	// Still unknown values: 6 'zeroes' and a 'one'
	dat->byte = 0x06;
	if (loglevel) printf ("Still unknown values: 6 'zeroes' and a 'one': ");
	for (i = 0; i < 7; i++)
	{
		sig = bit_read_RC (dat);
		if (loglevel) printf ("0x%02X ", sig);
	}
	if (loglevel) puts("");

	/* Image Seeker */
	pvz = bit_read_RL (dat);
	if (loglevel) printf ("Image seeker: 0x%08X\n", (unsigned int) pvz);

	// unknown
	sig = bit_read_RC (dat);
    if (loglevel) printf ("Version: %u\n", sig);
	sig = bit_read_RC (dat);
    if (loglevel) printf ("Lancxo: %u\n", sig);

	/* Codepage */
	dat->byte = 0x13;
	skt->header.codepage = bit_read_RS (dat);
    if (loglevel) printf ("Codepage: %u\n", skt->header.codepage);

	/* Section Locator Records */
	dat->byte = 0x15;
	skt->header.num_sections = bit_read_RL (dat);

    //  why do we have this limit to only 6 sections?
    //  It seems to be a bug, so I'll comment it out and will add dinamic
    //  allocation of the sections vector.
    //  OpenDWG spec speaks of 6 possible values for the record number
    //  Maybe the original libdwg author got confused about that.
    /*
	if (skt->header.num_sections > 6)
		skt->header.num_sections = 6;
    */
    skt->header.section = (Dwg_Section*) malloc(sizeof(Dwg_Section) * skt->header.num_sections);

	for (i = 0; i < skt->header.num_sections; i++)
	{
		skt->header.section[i].address = 0;
		skt->header.section[i].size = 0;

		skt->header.section[i].number = bit_read_RC (dat);
		skt->header.section[i].address = bit_read_RL (dat);
		skt->header.section[i].size = bit_read_RL (dat);
	}

	// Kontroli CKR-on
	/*
	   ckr = bit_read_CRC (dat);
	   dat->byte -= 2;
	   bit_krei_CRC (dat, 0, 0);
	   dat->byte -= 2;
	   ckr2 = bit_read_CRC (dat);
	   dat->byte -= 2;
	   bit_write_RS (dat, ckr2 ^ 0x8461);
	   dat->byte -= 2;
	   ckr2 = bit_read_CRC (dat);
	   if (loglevel) printf ("Legita: %X\nKreita: %X\n", ckr, ckr2);
	 */

	if (bit_search_sentinel (dat, dwg_sentinel (DWG_SENTINEL_HEADER_END)) && loglevel)
		printf ("=======> HEADER (end): %8X\n", (unsigned int) dat->byte);

	/*-------------------------------------------------------------------------
	 * Unknown section 1
	 */

	if (skt->header.num_sections == 6)
	{
        if (loglevel){
            printf ("========> UNKNOWN 1: %8X\n", (unsigned int) skt->header.section[5].address);
		    printf ("   UNKNOWN 1 (end): %8X\n", (unsigned int) (skt->header.section[5].address + skt->header.section[5].size));
		}
		dat->byte = skt->header.section[5].address;
		skt->unknown1.size = DWG_UNKNOWN1_KIOM;
		skt->unknown1.byte = skt->unknown1.bit = 0;
		skt->unknown1.chain = malloc (skt->unknown1.size);
		memcpy (skt->unknown1.chain, &dat->chain[dat->byte], skt->unknown1.size);
		//bit_esplori_chain ((Bit_Chain *) &skt->unknown1, skt->unknown1.size);
		//bit_print ((Bit_Chain *) &skt->unknown1, skt->unknown1.size);
	}


	/*-------------------------------------------------------------------------
	 * Antauxrigarda picture
	 */

	if (bit_search_sentinel (dat, dwg_sentinel (DWG_SENTINEL_PICTURE_BEGIN)))
	{
		unsigned long int start_address;

		dat->bit = 0;
		start_address = dat->byte;
        if (loglevel) printf ("=============> PICTURE: %8X\n", (unsigned int) start_address - 16);
		if (bit_search_sentinel (dat, dwg_sentinel (DWG_SENTINEL_PICTURE_END)))
		{
            if (loglevel) printf ("        PICTURE (end): %8X\n", (unsigned int) dat->byte);
			skt->picture.size = (dat->byte - 16) - start_address;
			skt->picture.chain = (char *) malloc (skt->picture.size);
			memcpy (skt->picture.chain, &dat->chain[start_address], skt->picture.size);
		}
		else
			skt->picture.size = 0;
	}


	/*-------------------------------------------------------------------------
	 * Kap-variabloj
	 */

    if (loglevel){
    	printf ("=====> KAP-VARIABLEJ: %8X\n", (unsigned int) skt->header.section[0].address);
	    printf ("KAP-VARIABLEJ (end): %8X\n", (unsigned int) (skt->header.section[0].address + skt->header.section[0].size));
    }
	dat->byte = skt->header.section[0].address + 16;
	pvz = bit_read_RL (dat);
	if (loglevel) printf ("Length: %lu\n", pvz);

	dat->bit = 0;

	/* Legi la kap-variablojn
	 */
	for (i = 0; i < DWG_NUM_VARIABLES; i++)
	{
		if (loglevel) printf ("[%03i] - ", i + 1);
		if (i == 221 && skt->var[220].dubitoko != 3)
		{
			skt->var[i].handle.code = 0;
			skt->var[i].handle.value = 0;
			//puts ("(NE EKZISTANTA)");
			continue;
		}
		switch (dwg_var_map (skt->header.version, i))
		{
		case DWG_DT_B:
			skt->var[i].bitoko = bit_read_B (dat);
			if (loglevel) printf ("B: %u", skt->var[i].bitoko);
			break;
		case DWG_DT_BS:
			skt->var[i].dubitoko = bit_read_BS (dat);
			if (loglevel) printf ("BS: %u", skt->var[i].dubitoko);
			break;
		case DWG_DT_BL:
			skt->var[i].kvarbitoko = bit_read_BL (dat);
			if (loglevel) printf ("BL: %lu", skt->var[i].kvarbitoko);
			break;
		case DWG_DT_BD:
			skt->var[i].duglitajxo = bit_read_BD (dat);
			if (loglevel) printf ("BD: %lg", skt->var[i].duglitajxo);
			break;
		case DWG_DT_H:
			bit_read_H (dat, &skt->var[i].handle);
			if (loglevel) printf ("H: %i.%i.0x%08X", skt->var[i].handle.code, skt->var[i].handle.size, (unsigned int) skt->var[i].handle.value);
			break;
		case DWG_DT_T:
			skt->var[i].text = bit_read_T (dat);
			if (loglevel) printf ("T: \"%s\"", skt->var[i].text);
			break;
		case DWG_DT_CMC:
			skt->var[i].dubitoko = bit_read_BS (dat);
			if (loglevel) printf ("CMC: %u", skt->var[i].dubitoko);
			break;
		case DWG_DT_2RD:
			skt->var[i].xy[0] = bit_read_RD (dat);
			skt->var[i].xy[1] = bit_read_RD (dat);
			if (loglevel){
			 printf ("X: %lg\t", skt->var[i].xy[0]);
			 printf ("Y: %lg", skt->var[i].xy[1]);
			}
			break;
		case DWG_DT_3BD:
			skt->var[i].xyz[0] = bit_read_BD (dat);
			skt->var[i].xyz[1] = bit_read_BD (dat);
			skt->var[i].xyz[2] = bit_read_BD (dat);
			if (loglevel) {
			 printf ("X: %lg\t", skt->var[i].xyz[0]);
			 printf ("Y: %lg\t", skt->var[i].xyz[1]);
			 printf ("Z: %lg", skt->var[i].xyz[2]);
			}
			break;
		default:
	        if (loglevel) printf ("Ne traktebla type: %i (var: %i)\n", dwg_var_map (skt->header.version, i), i);
		}
		//puts ("");
	}

	// Kontroli CKR-on
	ckr = bit_read_CRC (dat);
	/*
	   for (i = 0xC001; i != 0xC000; i++)
	   {
	   dat->byte -= 2;
	   bit_krei_CRC (dat, skt->header.section[0].address + 16, i);
	   dat->byte -= 2;
	   ckr2 = bit_read_CRC (dat);
	   if (ckr == ckr2)
	   {
	        if (loglevel) printf ("Legita: %X\nKreita: %X\t SEMO: %02X\n", ckr, ckr2, i);
	        break;
	   }
	   }
	 */


	/*-------------------------------------------------------------------------
	 * Classes
	 */
    if (loglevel){
    	printf ("============> CLASS: %8X\n", (unsigned int) skt->header.section[1].address);
	    printf ("       CLASS (end): %8X\n", (unsigned int) (skt->header.section[1].address + skt->header.section[1].size));
    }
	dat->byte = skt->header.section[1].address + 16;
	dat->bit = 0;

	size = bit_read_RL (dat);
	lasta = dat->byte + size;
	//if (loglevel) printf ("Length: %lu\n", size);

	/* read the classes
	 */
	skt->dwg_ot_layout = 0;
	skt->num_classes = 0;
	i = 0;
	do
	{
		unsigned int idc;

		idc = skt->num_classes;
		if (idc == 0)
			skt->class = (Dwg_Class *) malloc (sizeof (Dwg_Class));
		else
			skt->class =
				(Dwg_Class *) realloc (skt->class, (idc + 1) * sizeof (Dwg_Class));

		skt->class[idc].number = bit_read_BS (dat);
		skt->class[idc].version = bit_read_BS (dat);
		skt->class[idc].appname = bit_read_T (dat);
		skt->class[idc].cppname = bit_read_T (dat);
		skt->class[idc].dxfname = bit_read_T (dat);
		skt->class[idc].estisfantomo = bit_read_B (dat);
		skt->class[idc].eroid = bit_read_BS (dat);

		if (strcmp (skt->class[idc].dxfname, "LAYOUT") == 0)
			skt->dwg_ot_layout = skt->class[idc].number;

		skt->num_classes++;
		if (skt->num_classes > 100)
			break;
	}
	while (dat->byte < (lasta - 1));

	// Kontroli CKR-on
	ckr = bit_read_CRC (dat);
	/*
	   for (i = 0xC001; i != 0xC000; i++)
	   {
	   dat->byte -= 2;
	   bit_krei_CRC (dat, skt->header.section[1].address + 16, i);
	   dat->byte -= 2;
	   ckr2 = bit_read_CRC (dat);
	   if (ckr == ckr2)
	   {
	    if (loglevel) printf ("Legita: %X\nKreita: %X\t SEMO: %02X\n", ckr, ckr2, i);
	   break;
	   }
	   }
	 */

	dat->byte += 16;
	pvz = bit_read_RL (dat);	// Nekonata kvarbitoko inter class kaj objektaro
	//if (loglevel) {
	// printf ("Adreso: %lu / Enhavo: 0x%08X\n", dat->byte - 4, pvz);
	// printf ("Kiom class readtaj: %u\n", skt->num_classes);
    //}

	/*-------------------------------------------------------------------------
	 * Object-mapo (kaj objectj mem)
	 */

	dat->byte = skt->header.section[2].address;
	dat->bit = 0;

	maplasta = dat->byte + skt->header.section[2].size;	// 4
	skt->num_objects = 0;
	obek = dat->size;
	obfin = 0;
	do
	{
		long unsigned int lastadres;
		long unsigned int lastatrakt;
		long unsigned int antauxadr;

		duabyte = dat->byte;
		sgdc[0] = bit_read_RC (dat);
		sgdc[1] = bit_read_RC (dat);
		seksize = (sgdc[0] << 8) | sgdc[1];
		//if (loglevel) printf ("seksize: %u\n", seksize);
		if (seksize > 2034)	// 2032 + 2
		{
			fprintf (stderr, "Error: Object-map section size greater than 2034!");
			return -1;
		}

		lastatrakt = 0;
		lastadres = 0;
		while (dat->byte - duabyte < seksize)
		{
			long unsigned int kobj;
			long int pvztkt;
			long int pvzadr;

			antauxadr = dat->byte;
			pvztkt = bit_read_MC (dat);
			lastatrakt += pvztkt;
			pvzadr = bit_read_MC (dat);
			lastadres += pvzadr;
			//if (loglevel) {
			// printf ("Idc: %li\t", skt->num_objects);
			// printf ("Trakt: %li\tAdres: %li\n", pvztkt, pvzadr);
			//}
			if (dat->byte == antauxadr)
				break;
			//if (dat->byte - duabyte >= seksize)
			//break;

			if (obfin < lastadres)
				obfin = lastadres;
			if (obek > lastadres)
				obek = lastadres;

			kobj = skt->num_objects;
			dwg_decode_aldoni_object (skt, dat, lastadres);
			if (skt->num_objects > kobj)
				skt->object[skt->num_objects - 1].trakt = lastatrakt;

		}
		if (dat->byte == antauxadr)
			break;
		dat->byte += 2;	// CKR

		if (dat->byte >= maplasta)
			break;
	}
	while (seksize > 2);
	if (loglevel){
    	printf ("Kiom objectj: %lu\n", skt->num_objects);
    	printf ("=========> OBJEKTARO: %8X\n", (unsigned int) obek);
    }
	dat->byte = obfin;
	obek = bit_read_MS (dat);	// La komenco de la lasta object readta
	if (loglevel) printf ("    OBJEKTARO (end): %8X\n", (unsigned int) (obfin + obek + 2));

	/*
	   dat->byte = skt->header.section[2].address - 2;
	   antckr = bit_read_CRC (dat); // Nekonata dubitoko inter objektaro kaj object-mapo
	   if (loglevel) printf ("Adreso: %08X / Enhavo: 0x%04X\n", dat->byte - 2, antckr);

	   // Kontroli CKR-ojn
	   antckr = 0xC0C1;
	   do
	   {
	   duabyte = dat->byte;
	   sgdc[0] = bit_read_RC (dat);
	   sgdc[1] = bit_read_RC (dat);
	   seksize = (sgdc[0] << 8) | sgdc[1];
	   seksize -= 2;
	   dat->byte += seksize;
	   ckr = bit_read_CRC (dat);
	   dat->byte -= 2;
	   bit_krei_CRC (dat, duabyte, antckr);
	   dat->byte -= 2;
	   ckr2 = bit_read_CRC (dat);
	   if (loglevel) printf ("Legita: %X\nKreita: %X\t SEMO: %X\n", ckr, ckr2, antckr);
	   //antckr = ckr;
	   } while (seksize > 0);
	 */
    if (loglevel) {
    	printf ("======> OBJECT-MAPO: %8X\n", (unsigned int) skt->header.section[2].address);
	    printf (" OBJECT-MAPO (end): %8X\n", (unsigned int) (skt->header.section[2].address + skt->header.section[2].size));
	}

	/*-------------------------------------------------------------------------
	 * Dua kap-datenaro
	 */

	if (bit_search_sentinel (dat, dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_BEGIN)))
	{
		long unsigned int pvzadr;
		long unsigned int pvz;
		unsigned char sig, sig2;

		if (loglevel) printf ("==> DUA KAP-DATENARO: %8X\n", (unsigned int) dat->byte - 16);
		pvzadr = dat->byte;

		pvz = bit_read_RL (dat);
		//if (loglevel) printf ("Kiomo: %lu\n", pvz);

		pvz = bit_read_BL (dat);
		//if (loglevel) printf ("Ekaddress: %8X\n", pvz);

		//if (loglevel) printf ("AC1015?: ");
		for (i = 0; i < 6; i++)
		{
			sig = bit_read_RC (dat);
			//if (loglevel) printf ("%c", sig >= ' ' && sig < 128 ? sig : '.');
		}

		//if (loglevel) printf ("\nNuloj?:");
		for (i = 0; i < 5; i++)	// 6 se estas pli malnova...
		{
			sig = bit_read_RC (dat);
			//if (loglevel) printf (" 0x%02X", sig);
		}

		//if (loglevel) printf ("\n4 nulaj bitj?: ");
		for (i = 0; i < 4; i++)
		{
			sig = bit_read_B (dat);
			//if (loglevel) printf (" %c", sig ? '1' : '0');
		}

		//if (loglevel) printf ("\nChain?: ");
		for (i = 0; i < 6; i++)
		{
			skt->second_header.unknownjxo[i] = bit_read_RC (dat);
			//if (loglevel) printf (" 0x%02X", skt->second_header.unknownjxo[i]);
		}
		if (skt->second_header.unknownjxo[3] != 0x78 || skt->second_header.unknownjxo[5] != 0x06)
			sig = bit_read_RC (dat);	// por kompenso okaze de eventuala kroma nulo ne readta antauxe

		//puts("");
		for (i = 0; i < 6; i++)
		{
			sig = bit_read_RC (dat);
			//if (loglevel) printf ("[%u]\n", sig);
			pvz = bit_read_BL (dat);
			//if (loglevel) printf (" Adreso: %8X\n", pvz);
			pvz = bit_read_BL (dat);
			//if (loglevel) printf ("  Kiomo: %8X\n", pvz);
		}

		bit_read_BS (dat);
		//if (loglevel) printf ("\n14 --------------");
		for (i = 0; i < 14; i++)
		{
			sig2 = bit_read_RC (dat);
			skt->second_header.traktrik[i].size = sig2;
			//if (loglevel) printf ("\nLongo: %u\n", sig2);
			sig = bit_read_RC (dat);
			//if (loglevel) printf ("\t[%u]\n", sig);
			//if (loglevel) printf ("\tChain:");
			for (j = 0; j < sig2; j++)
			{
				sig = bit_read_RC (dat);
				skt->second_header.traktrik[i].chain[j] = sig;
				//if (loglevel) printf (" %02X", sig);
			}
		}

		// Kontroli CKR-on
		ckr = bit_read_CRC (dat);
		/*
		   puts ("");
		   for (i = 0; i != 0xFFFF; i++)
		   {
		   dat->byte -= 2;
		   bit_krei_CRC (dat, pvzadr, i);
		   dat->byte -= 2;
		   ckr2 = bit_read_CRC (dat);
		   if (ckr == ckr2)
		   {
		        if (loglevel) printf ("Legita: %X\nKreita: %X\t SEMO: %02X\n", ckr, ckr2, i);
		   break;
		   }
		   }
		   if (loglevel) {
		        printf (" Rubajxo 1: %08X\n", bit_read_RL (dat));
		        printf (" Rubajxo 1: %08X\n", bit_read_RL (dat));
		   }
		 */

		if (loglevel && bit_search_sentinel (dat, dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_END)))
			printf (" DUA KAP-DAT. (end): %8X\n", (unsigned int) dat->byte);
	}

	/*-------------------------------------------------------------------------
	 * Sekcio MEASUREMENT
	 */

    if (loglevel) {
	    printf ("========> UNKNOWN 2: %8X\n", (unsigned int) skt->header.section[4].address);
	    printf ("   UNKNOWN 2 (end): %8X\n", (unsigned int) (skt->header.section[4].address + skt->header.section[4].size));
	}
	dat->byte = skt->header.section[4].address;
	dat->bit = 0;
	skt->measurement = bit_read_RL (dat);

    if (loglevel) printf ("KIOM BAJTOJ :\t%lu\n", dat->size);

	//exit (0);
	return 0;
}

/*--------------------------------------------------------------------------------
 * Privataj funkcioj
 */

static void
dwg_decode_entity (Bit_Chain * dat, Dwg_Object_Entity * ent)
{
	unsigned int i;
	unsigned int size;
	int error = 2;

	if (dat->version >= R_2000)
	{
		ent->bitsize = bit_read_RL (dat);
	}

	error = bit_read_H (dat, &ent->handle);
	if (error)
	{
		fprintf (stderr, "dwg_decode_entity:\tError in object handle! Current Bit_Chain address: 0x%0x\n", (unsigned int) dat->byte);
		ent->bitsize = 0;
		ent->extended_size = 0;
		ent->picture_exists = 0;
		ent->traktref_size = 0;
		return;
	}
	ent->extended_size = 0;
	while (size = bit_read_BS (dat))
	{
		if (size > 10210)
		{
			fprintf (stderr, "dwg_decode_entity: Absurd! Extended object data size: %lu. Object: %lu (handle).\n", (long unsigned int) size, ent->handle.value);
			ent->bitsize = 0;
			ent->extended_size = 0;
			ent->picture_exists = 0;
			ent->traktref_size = 0;
			return;
		}
		if (ent->extended_size == 0)
		{
			ent->extended = malloc (size);
			ent->extended_size = size;
		}
		else
		{
			ent->extended_size += size;
			ent->extended = realloc (ent->extended, ent->extended_size);
		}
		error = bit_read_H (dat, &ent->extended_trakt);
		if (error)
			fprintf (stderr, "Ops...\n");
		for (i = ent->extended_size - size; i < ent->extended_size; i++)
			ent->extended[i] = bit_read_RC (dat);
	}
	ent->picture_exists = bit_read_B (dat);
	if (ent->picture_exists)
	{
		ent->picture_size = bit_read_RL (dat);
		if (ent->picture_size < 210210)
		{
			ent->picture = malloc (ent->picture_size);
			for (i = 0; i < ent->picture_size; i++)
				ent->picture[i] = bit_read_RC (dat);
		}
		else
		{
			fprintf (stderr, "dwg_decode_entity:  Absurd! Picture-size: %lu kB. Object: %lu (handle).\n",
				ent->picture_size / 1000, ent->handle.value);
			bit_advance_position (dat, -(4 * 8 + 1));
		}
	}

	ent->regime = bit_read_BB (dat);
	ent->reagilo_size = bit_read_BL (dat);
	ent->senligiloj = bit_read_B (dat);
	ent->colour = bit_read_BS (dat);
	ent->linetype_scale = bit_read_BD (dat);
	ent->linetype = bit_read_BB (dat);
	ent->plot_style = bit_read_BB (dat);

	ent->invisible = bit_read_BS (dat);
	ent->lineweight = bit_read_RC (dat);
}

static void
dwg_decode_object (Bit_Chain * dat, Dwg_Object_Object * ord)
{
	unsigned int i;
	unsigned int size;
	int error = 2;

	if (dat->version >= R_2000){
		ord->bitsize = bit_read_RL (dat);
	}

	error = bit_read_H (dat, &ord->handle);
	if (error)
	{
		fprintf (stderr, "\tError in object handle! Bit_Chain current address: 0x%0x\n", (unsigned int) dat->byte);
		ord->bitsize = 0;
		ord->extended_size = 0;
		ord->traktref_size = 0;
		return;
	}
	ord->extended_size = 0;
	while (size = bit_read_BS (dat))
	{
		if (size > 10210)
		{
			fprintf (stderr, "dwg_decode_object: Absurd! Extended object data size: %lu. Object: %lu (handle).\n", (long unsigned int) size, ord->handle.value);
			ord->bitsize = 0;
			ord->extended_size = 0;
			ord->traktref_size = 0;
			return;
		}
		if (ord->extended_size == 0)
		{
			ord->extended = malloc (size);
			ord->extended_size = size;
		}
		else
		{
			ord->extended_size += size;
			ord->extended = realloc (ord->extended, ord->extended_size);
		}
		error = bit_read_H (dat, &ord->extended_trakt);
		if (error)
			fprintf (stderr, "Ops...\n");
		for (i = ord->extended_size - size; i < ord->extended_size; i++)
			ord->extended[i] = bit_read_RC (dat);
	}

	ord->reagilo_size = bit_read_BL (dat);
}

static void
dwg_decode_traktref (Bit_Chain * dat, Dwg_Object * obj)
{
	int i;

	if (obj->supertype == DWG_SUPERTYPE_ENTITY)
	{
		Dwg_Object_Entity *ent;

		ent = obj->tio.entity;

		ent->traktref = (Dwg_Handle *) calloc (sizeof (Dwg_Handle), 10);
		i = 0;
		while (1)
		{
			if (i % 10 == 0)
				ent->traktref =
					(Dwg_Handle *) realloc (ent->traktref,
								  (i + 10) * sizeof (Dwg_Handle));
			if (bit_read_H (dat, &ent->traktref[i]))
			{
				fprintf (stderr, "\tEraro en tiu handle: %lu\n", ent->handle.value);
				break;
			}
			if (!(dat->byte == ktl_lastaddress + 1 && dat->bit == 0))
			{
				if (dat->byte > ktl_lastaddress)
					break;
			}
			i++;
		}
		ent->traktref_size = i;
	}
	else
	{
		Dwg_Object_Object *ord;

		ord = obj->tio.object;

		ord->traktref = (Dwg_Handle *) calloc (sizeof (Dwg_Handle), 10);
		i = 0;
		while (1)
		{
			if (i % 10 == 0)
				ord->traktref =
					(Dwg_Handle *) realloc (ord->traktref,
								  (i + 10) * sizeof (Dwg_Handle));
			if (bit_read_H (dat, &ord->traktref[i]))
			{
				//fprintf (stderr, "\tEraro en tiu handle: %lu\n", ent->handle.value);
				break;
			}
			if (!(dat->byte == ktl_lastaddress + 1 && dat->bit == 0))
			{
				if (dat->byte > ktl_lastaddress)
					break;
			}
			i++;
		}
		ord->traktref_size = i;
	}
}

/* OBJECTS *******************************************************************/

static void
dwg_decode_UNUSED (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_UNUSED *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.UNUSED = calloc (sizeof (Dwg_Entity_UNUSED), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.UNUSED;

	/* Read values
	 */


	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_TEXT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_TEXT *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.TEXT = calloc (sizeof (Dwg_Entity_TEXT), 1);
	ent = obj->tio.entity->tio.TEXT;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */

        if (dat->version == R_13 ||
            dat->version == R_14){

                ent->elevation = bit_read_BD (dat);
                ent->x0 = bit_read_RD (dat);
                ent->y0 = bit_read_RD (dat);
                ent->alignment.x = bit_read_RD (dat);
                ent->alignment.y = bit_read_RD (dat);
                bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
                ent->thickness = bit_read_BD (dat);
                ent->oblique_ang = bit_read_BD (dat);
                ent->rotation_ang = bit_read_BD (dat);
                ent->height = bit_read_BD (dat);
                ent->width_factor = bit_read_BD (dat);
                ent->text = bit_read_T (dat);
                ent->generation = bit_read_BS (dat);
                ent->alignment.h = bit_read_BS (dat);
                ent->alignment.v = bit_read_BS (dat);
        }

        if (dat->version >= R_2000){
                ent->dataflags = bit_read_RC (dat);
                if ((!ent->dataflags & 0x01))
                        ent->elevation = bit_read_RD (dat);
                ent->x0 = bit_read_RD (dat);
                ent->y0 = bit_read_RD (dat);
                if (!(ent->dataflags & 0x02))
                {
                        ent->alignment.x = bit_read_DD (dat, 10);
                        ent->alignment.y = bit_read_DD (dat, 20);
                }
                bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
                ent->thickness = bit_read_BT (dat);
                if (!(ent->dataflags & 0x04))
                        ent->oblique_ang = bit_read_RD (dat);
                if (!(ent->dataflags & 0x08))
                        ent->rotation_ang = bit_read_RD (dat);
                ent->height = bit_read_RD (dat);
                if (!(ent->dataflags & 0x10))
                        ent->width_factor = bit_read_RD (dat);
                ent->text = bit_read_T (dat);
                if (!(ent->dataflags & 0x20))
                        ent->generation = bit_read_BS (dat);
                if (!(ent->dataflags & 0x40))
                        ent->alignment.h = bit_read_BS (dat);
                if (!(ent->dataflags & 0x80))
                        ent->alignment.v = bit_read_BS (dat);
        }
	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ATTRIB (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ATTRIB *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ATTRIB = calloc (sizeof (Dwg_Entity_ATTRIB), 1);
	ent = obj->tio.entity->tio.ATTRIB;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
    if (dat->version == R_13 ||
        dat->version == R_14){

	    ent->elevation = bit_read_BD (dat);
	    ent->x0 = bit_read_RD (dat);
	    ent->y0 = bit_read_RD (dat);
	    ent->alignment.x = bit_read_RD (dat);
	    ent->alignment.y = bit_read_RD (dat);
	    bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
	    ent->thickness = bit_read_BD (dat);
	    ent->oblique_ang = bit_read_BD (dat);
	    ent->rotation_ang = bit_read_BD (dat);
	    ent->height = bit_read_BD (dat);
	    ent->width_factor = bit_read_BD (dat);
	    ent->text = bit_read_T (dat);
	    ent->generation = bit_read_BS (dat);
	    ent->alignment.h = bit_read_BS (dat);
	    ent->alignment.v = bit_read_BS (dat);
    }

    if (dat->version >= R_2000){
	    ent->dataflags = bit_read_RC (dat);
	    if ((!ent->dataflags & 0x01))
		    ent->elevation = bit_read_RD (dat);
	    ent->x0 = bit_read_RD (dat);
	    ent->y0 = bit_read_RD (dat);
	    if (!(ent->dataflags & 0x02))
	    {
		    ent->alignment.x = bit_read_DD (dat, 10);
		    ent->alignment.y = bit_read_DD (dat, 20);
	    }
	    bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
	    ent->thickness = bit_read_BT (dat);
	    if (!(ent->dataflags & 0x04))
		    ent->oblique_ang = bit_read_RD (dat);
	    if (!(ent->dataflags & 0x08))
		    ent->rotation_ang = bit_read_RD (dat);
	    ent->height = bit_read_RD (dat);
	    if (!(ent->dataflags & 0x10))
		    ent->width_factor = bit_read_RD (dat);
	    ent->text = bit_read_T (dat);
	    if (!(ent->dataflags & 0x20))
		    ent->generation = bit_read_BS (dat);
	    if (!(ent->dataflags & 0x40))
		    ent->alignment.h = bit_read_BS (dat);
	    if (!(ent->dataflags & 0x80))
		    ent->alignment.v = bit_read_BS (dat);
    }

	ent->tag = bit_read_T (dat);
	ent->field_length = bit_read_BS (dat);
	ent->flags = bit_read_RC (dat);

    if (dat->version >= R_2007){
        ent->lock_position_flag = bit_read_B (dat);
    }

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ATTDEF (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ATTDEF *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ATTDEF = calloc (sizeof (Dwg_Entity_ATTDEF), 1);
	ent = obj->tio.entity->tio.ATTDEF;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
    if (dat->version == R_13 ||
        dat->version == R_14){

	    ent->elevation = bit_read_BD (dat);
	    ent->x0 = bit_read_RD (dat);
	    ent->y0 = bit_read_RD (dat);
	    ent->alignment.x = bit_read_RD (dat);
	    ent->alignment.y = bit_read_RD (dat);
	    bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
	    ent->thickness = bit_read_BD (dat);
	    ent->oblique_ang = bit_read_BD (dat);
	    ent->rotation_ang = bit_read_BD (dat);
	    ent->height = bit_read_BD (dat);
	    ent->width_factor = bit_read_BD (dat);
	    ent->text = bit_read_T (dat);
	    ent->generation = bit_read_BS (dat);
	    ent->alignment.h = bit_read_BS (dat);
	    ent->alignment.v = bit_read_BS (dat);
    }

    if (dat->version >= R_2000){
	    ent->dataflags = bit_read_RC (dat);
	    if ((!ent->dataflags & 0x01))
		    ent->elevation = bit_read_RD (dat);
	    ent->x0 = bit_read_RD (dat);
	    ent->y0 = bit_read_RD (dat);
	    if (!(ent->dataflags & 0x02))
	    {
		    ent->alignment.x = bit_read_DD (dat, 10);
		    ent->alignment.y = bit_read_DD (dat, 20);
	    }
	    bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
	    ent->thickness = bit_read_BT (dat);
	    if (!(ent->dataflags & 0x04))
		    ent->oblique_ang = bit_read_RD (dat);
	    if (!(ent->dataflags & 0x08))
		    ent->rotation_ang = bit_read_RD (dat);
	    ent->height = bit_read_RD (dat);
	    if (!(ent->dataflags & 0x10))
		    ent->width_factor = bit_read_RD (dat);
	    ent->text = bit_read_T (dat);
	    if (!(ent->dataflags & 0x20))
		    ent->generation = bit_read_BS (dat);
	    if (!(ent->dataflags & 0x40))
		    ent->alignment.h = bit_read_BS (dat);
	    if (!(ent->dataflags & 0x80))
		    ent->alignment.v = bit_read_BS (dat);
    }

	ent->tag = bit_read_T (dat);
	ent->field_length = bit_read_BS (dat);
	ent->flags = bit_read_RC (dat);

    if (dat->version >= R_2007){
        ent->lock_position_flag = bit_read_B(dat);
    }
    ent->prompt = bit_read_T (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_BLOCK (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_BLOCK *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.BLOCK = calloc (sizeof (Dwg_Entity_BLOCK), 1);
	ent = obj->tio.entity->tio.BLOCK;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
	ent->name = bit_read_T (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ENDBLK (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ENDBLK *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ENDBLK = calloc (sizeof (Dwg_Entity_ENDBLK), 1);
	ent = obj->tio.entity->tio.ENDBLK;
	dwg_decode_entity (dat, obj->tio.entity);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_SEQEND (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_SEQEND *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.SEQEND = calloc (sizeof (Dwg_Entity_SEQEND), 1);
	ent = obj->tio.entity->tio.SEQEND;
	dwg_decode_entity (dat, obj->tio.entity);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_INSERT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_INSERT *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.INSERT = calloc (sizeof (Dwg_Entity_INSERT), 1);
	ent = obj->tio.entity->tio.INSERT;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);

	if (dat->version == R_13 || dat->version == R_14)
        {
		ent->scale.x = bit_read_BD (dat);
		ent->scale.y = bit_read_BD (dat);
		ent->scale.z = bit_read_BD (dat);
	}

        if (dat->version >= R_2000)
        {
		ent->scale_flag = bit_read_BB (dat);
		if (ent->scale_flag == 3)
			ent->scale.x = ent->scale.y = ent->scale.z = 1.0;
		else if (ent->scale_flag == 1)
		{
			ent->scale.x = 1.0;
			ent->scale.y = bit_read_DD (dat, 1.0);
			ent->scale.z = bit_read_DD (dat, 1.0);
		}
		else if (ent->scale_flag == 2)
			ent->scale.x = ent->scale.y = ent->scale.z = bit_read_RD (dat);
		else //if (ent->scale_flag == 0)
		{
			ent->scale.x = bit_read_RD (dat);
			ent->scale.y = bit_read_DD (dat, ent->scale.x);
			ent->scale.z = bit_read_DD (dat, ent->scale.x);
		}
	}

	ent->rotation_ang = bit_read_BD (dat);
	ent->extrusion.x = bit_read_BD (dat);
	ent->extrusion.y = bit_read_BD (dat);
	ent->extrusion.z = bit_read_BD (dat);
	ent->has_attribs = bit_read_B (dat);

	if (dat->version >= R_2004)
        {
		ent->owned_obj_count = bit_read_BL(dat);
	}

//TODO: incomplete implementation. Check spec!

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_MINSERT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_MINSERT *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.MINSERT = calloc (sizeof (Dwg_Entity_MINSERT), 1);
	ent = obj->tio.entity->tio.MINSERT;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);

	if (dat->version == R_13 ||
	    dat->version == R_14){
		ent->scale.x = bit_read_BD (dat);
		ent->scale.y = bit_read_BD (dat);
		ent->scale.z = bit_read_BD (dat);
	}

	if (dat->version >= R_2000){
		ent->scale_flag = bit_read_BB (dat);
		if (ent->scale_flag == 3)
			ent->scale.x = ent->scale.y = ent->scale.y = 1.0;
		else if (ent->scale_flag == 1)
		{
			ent->scale.x = 1.0;
			ent->scale.y = bit_read_DD (dat, 1.0);
			ent->scale.z = bit_read_DD (dat, 1.0);
		}
		else if (ent->scale_flag == 2)
			ent->scale.x = ent->scale.y = ent->scale.y = bit_read_RD (dat);
		else //if (ent->scale_flag == 0)
		{
			ent->scale.x = bit_read_RD (dat);
			ent->scale.y = bit_read_DD (dat, ent->scale.x);
			ent->scale.z = bit_read_DD (dat, ent->scale.x);
		}
	}

	ent->rotation_ang = bit_read_BD (dat);
	ent->extrusion.x = bit_read_BD (dat);
	ent->extrusion.y = bit_read_BD (dat);
	ent->extrusion.z = bit_read_BD (dat);
	ent->has_attribs = bit_read_B (dat);

	if (dat->version >= R_2004){
		ent->owned_obj_count = bit_read_BL(dat);
	}

	ent->column.size = bit_read_BS (dat);
	ent->line.size = bit_read_BS (dat);
	ent->column.dx = bit_read_BD (dat);
	ent->line.dy = bit_read_BD (dat);

//TODO: incomplete implementation. Check spec!
	dwg_decode_traktref (dat, obj);
}


static void
dwg_decode_VERTEX_2D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_VERTEX_2D *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.VERTEX_2D = calloc (sizeof (Dwg_Entity_VERTEX_2D), 1);
	ent = obj->tio.entity->tio.VERTEX_2D;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
	ent->flags = bit_read_RC (dat);
	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);
	ent->start_width = bit_read_BD (dat);
	if (ent->start_width < 0)
		ent->end_width = ent->start_width = -ent->start_width;
	else
		ent->end_width = bit_read_BD (dat);
	ent->bulge = bit_read_BD (dat);
	ent->tangent_dir = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_3D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_VERTEX_3D *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.VERTEX_3D = calloc (sizeof (Dwg_Entity_VERTEX_3D), 1);
	ent = obj->tio.entity->tio.VERTEX_3D;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
	ent->flags = bit_read_RC (dat);
	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_PFACE_FACE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_VERTEX_PFACE_FACE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.VERTEX_PFACE_FACE = calloc (sizeof (Dwg_Entity_VERTEX_PFACE_FACE), 1);
	ent = obj->tio.entity->tio.VERTEX_PFACE_FACE;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Read values
	 */
	ent->vertind[0] = bit_read_BS (dat);
	ent->vertind[1] = bit_read_BS (dat);
	ent->vertind[2] = bit_read_BS (dat);
	ent->vertind[3] = bit_read_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_2D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_POLYLINE_2D *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POLYLINE_2D = calloc (sizeof (Dwg_Entity_POLYLINE_2D), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.POLYLINE_2D;

	/* Read values
	 */
	ent->flags = bit_read_BS (dat);
	ent->curve_type = bit_read_BS (dat);
	ent->start_width = bit_read_BD (dat);
	ent->end_width = bit_read_BD (dat);
	ent->thickness = bit_read_BT (dat);
	ent->elevation = bit_read_BD (dat);
	bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);

	if (dat->version >= R_2004){
		ent->owned_obj_count = bit_read_BL(dat);
	}

//TODO: incomplete implementation. Check spec!

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_3D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_POLYLINE_3D *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POLYLINE_3D = calloc (sizeof (Dwg_Entity_POLYLINE_3D), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.POLYLINE_3D;

	/* Read values
	 */
	ent->flags_1 = bit_read_RC (dat);
	ent->flags_2 = bit_read_RC (dat);

	if (dat->version >= R_2004){
		ent->owned_obj_count = bit_read_BL(dat);
	}

//TODO: incomplete implementation. Check spec!

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ARC (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ARC *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ARC = calloc (sizeof (Dwg_Entity_ARC), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.ARC;

	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);
	ent->radius = bit_read_BD (dat);
	ent->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
	ent->start_angle = bit_read_BD (dat);
	ent->end_angle = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_CIRCLE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_CIRCLE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.CIRCLE = calloc (sizeof (Dwg_Entity_CIRCLE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.CIRCLE;

	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);
	ent->radius = bit_read_BD (dat);
	ent->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_LINE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_LINE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.LINE = calloc (sizeof (Dwg_Entity_LINE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.LINE;

	if (dat->version == R_13 ||
	    dat->version == R_14){
		ent->x0 = bit_read_BD (dat);
		ent->y0 = bit_read_BD (dat);
		ent->z0 = bit_read_BD (dat);

		ent->x1 = bit_read_BD (dat);
		ent->y1 = bit_read_BD (dat);
		ent->z1 = bit_read_BD (dat);		
	}

	if (dat->version == R_2000){
		ent->Zs_are_zero = bit_read_B (dat);
		ent->x0 = bit_read_RD (dat);
		ent->x1 = bit_read_DD (dat, ent->x0);
		ent->y0 = bit_read_RD (dat);
		ent->y1 = bit_read_DD (dat, ent->y0);
		ent->z0 = ent->z1 = 0.0;
		if (!ent->Zs_are_zero)
		{
			ent->z0 = bit_read_RD (dat);
			ent->z1 = bit_read_DD (dat, ent->z0);
		}
	}
	ent->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_DIMENSION_ORDINATE (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_DIMENSION_ORDINATE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.DIMENSION_ORDINATE = calloc (sizeof (Dwg_Entity_DIMENSION_ORDINATE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.DIMENSION_ORDINATE;

        //TODO: check extrusion reading
        //bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);
        ent->x0 = bit_read_RD (dat);
        ent->y0 = bit_read_RD (dat);
        ent->elevation.ecs_11 = bit_read_BD(dat);
        ent->elevation.ecs_12 = bit_read_BD(dat);
        //spec: flag bit 6 indicates ORDINATE dimension
        ent->flags_1 = bit_read_RC(dat);
        ent->user_text = bit_read_T(dat);
        ent->text_rot = bit_read_BD(dat);
        ent->horiz_dir = bit_read_BD(dat);
        ent->ins_scale.x = bit_read_BD(dat);
        ent->ins_scale.y = bit_read_BD(dat);
        ent->ins_scale.z = bit_read_BD(dat);
        ent->ins_rotation = bit_read_BD(dat);

        if(dat->version >= R_2000){
            ent->attachment_point = bit_read_BS(dat);
            ent->lspace_style = bit_read_BS(dat);
            ent->lspace_factor = bit_read_BD(dat);
            ent->act_measurement = bit_read_BD(dat);
        }
        if(dat->version >= R_2007){
            ent->unknown = bit_read_B(dat);
            ent->flip_arrow1 = bit_read_B(dat);
            ent->flip_arrow2 = bit_read_B(dat);
        }

        ent->_12_pt.x = bit_read_RD(dat);
        ent->_12_pt.y = bit_read_RD(dat);
        ent->_10_pt.x = bit_read_BD(dat);
        ent->_10_pt.y = bit_read_BD(dat);
        ent->_10_pt.z = bit_read_BD(dat);
        ent->_13_pt.x = bit_read_BD(dat);
        ent->_13_pt.y = bit_read_BD(dat);
        ent->_13_pt.z = bit_read_BD(dat);
        ent->_14_pt.x = bit_read_BD(dat);
        ent->_14_pt.y = bit_read_BD(dat);
        ent->_14_pt.z = bit_read_BD(dat);

        ent->flags_2 = bit_read_RC(dat);
        
        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_DIMENSION_LINEAR (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_DIMENSION_LINEAR *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.DIMENSION_LINEAR = calloc (sizeof (Dwg_Entity_DIMENSION_LINEAR), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.DIMENSION_LINEAR;

        //TODO: check extrusion reading
        //bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);
        ent->x0 = bit_read_RD (dat);
        ent->y0 = bit_read_RD (dat);
        ent->elevation.ecs_11 = bit_read_BD(dat);
        ent->elevation.ecs_12 = bit_read_BD(dat);
        //spec: flag bit 0 indicates LINEAR dimension
        ent->flags = bit_read_RC(dat);
        ent->user_text = bit_read_T(dat);
        ent->text_rot = bit_read_BD(dat);
        ent->horiz_dir = bit_read_BD(dat);
        ent->ins_scale.x = bit_read_BD(dat);
        ent->ins_scale.y = bit_read_BD(dat);
        ent->ins_scale.z = bit_read_BD(dat);
        ent->ins_rotation = bit_read_BD(dat);

        if(dat->version >= R_2000){
            ent->attachment_point = bit_read_BS(dat);
            ent->lspace_style = bit_read_BS(dat);
            ent->lspace_factor = bit_read_BD(dat);
            ent->act_measurement = bit_read_BD(dat);
        }
        if(dat->version >= R_2007){
            ent->unknown = bit_read_B(dat);
            ent->flip_arrow1 = bit_read_B(dat);
            ent->flip_arrow2 = bit_read_B(dat);
        }

        ent->_12_pt.x = bit_read_RD(dat);
        ent->_12_pt.y = bit_read_RD(dat);        
        ent->_13_pt.x = bit_read_BD(dat);
        ent->_13_pt.y = bit_read_BD(dat);
        ent->_13_pt.z = bit_read_BD(dat);
        ent->_14_pt.x = bit_read_BD(dat);
        ent->_14_pt.y = bit_read_BD(dat);
        ent->_14_pt.z = bit_read_BD(dat);
        ent->_10_pt.x = bit_read_BD(dat);
        ent->_10_pt.y = bit_read_BD(dat);
        ent->_10_pt.z = bit_read_BD(dat);

        ent->ext_line_rot = bit_read_BD(dat);
        ent->dim_rot = bit_read_BD(dat);

        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_DIMENSION_ALIGNED (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_DIMENSION_ALIGNED *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.DIMENSION_ALIGNED = calloc (sizeof (Dwg_Entity_DIMENSION_ALIGNED), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.DIMENSION_ALIGNED;

        //TODO: check extrusion reading
        //bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);
        ent->x0 = bit_read_RD (dat);
        ent->y0 = bit_read_RD (dat);
        ent->elevation.ecs_11 = bit_read_BD(dat);
        ent->elevation.ecs_12 = bit_read_BD(dat);
        //spec: flag bit 1 indicates ALIGNED dimension
        ent->flags = bit_read_RC(dat);
        ent->user_text = bit_read_T(dat);
        ent->text_rot = bit_read_BD(dat);
        ent->horiz_dir = bit_read_BD(dat);
        ent->ins_scale.x = bit_read_BD(dat);
        ent->ins_scale.y = bit_read_BD(dat);
        ent->ins_scale.z = bit_read_BD(dat);
        ent->ins_rotation = bit_read_BD(dat);

        if(dat->version >= R_2000){
            ent->attachment_point = bit_read_BS(dat);
            ent->lspace_style = bit_read_BS(dat);
            ent->lspace_factor = bit_read_BD(dat);
            ent->act_measurement = bit_read_BD(dat);
        }
        if(dat->version >= R_2007){
            ent->unknown = bit_read_B(dat);
            ent->flip_arrow1 = bit_read_B(dat);
            ent->flip_arrow2 = bit_read_B(dat);
        }

        ent->_12_pt.x = bit_read_RD(dat);
        ent->_12_pt.y = bit_read_RD(dat);
        ent->_13_pt.x = bit_read_BD(dat);
        ent->_13_pt.y = bit_read_BD(dat);
        ent->_13_pt.z = bit_read_BD(dat);
        ent->_14_pt.x = bit_read_BD(dat);
        ent->_14_pt.y = bit_read_BD(dat);
        ent->_14_pt.z = bit_read_BD(dat);
        ent->_10_pt.x = bit_read_BD(dat);
        ent->_10_pt.y = bit_read_BD(dat);
        ent->_10_pt.z = bit_read_BD(dat);

        ent->ext_line_rot = bit_read_BD(dat);

        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_DIMENSION_ANG3PT (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_DIMENSION_ANG3PT *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.DIMENSION_ANG3PT = calloc (sizeof (Dwg_Entity_DIMENSION_ANG3PT), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.DIMENSION_ANG3PT;

        //TODO: check extrusion reading
        //bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);
        ent->x0 = bit_read_RD (dat);
        ent->y0 = bit_read_RD (dat);
        ent->elevation.ecs_11 = bit_read_BD(dat);
        ent->elevation.ecs_12 = bit_read_BD(dat);
        //spec: flag bit 5 indicates ANGULAR 3PT dimension
        ent->flags = bit_read_RC(dat);
        ent->user_text = bit_read_T(dat);
        ent->text_rot = bit_read_BD(dat);
        ent->horiz_dir = bit_read_BD(dat);
        ent->ins_scale.x = bit_read_BD(dat);
        ent->ins_scale.y = bit_read_BD(dat);
        ent->ins_scale.z = bit_read_BD(dat);
        ent->ins_rotation = bit_read_BD(dat);

        if(dat->version >= R_2000){
            ent->attachment_point = bit_read_BS(dat);
            ent->lspace_style = bit_read_BS(dat);
            ent->lspace_factor = bit_read_BD(dat);
            ent->act_measurement = bit_read_BD(dat);
        }
        if(dat->version >= R_2007){
            ent->unknown = bit_read_B(dat);
            ent->flip_arrow1 = bit_read_B(dat);
            ent->flip_arrow2 = bit_read_B(dat);
        }

        ent->_12_pt.x = bit_read_RD(dat);
        ent->_12_pt.y = bit_read_RD(dat);
        ent->_10_pt.x = bit_read_BD(dat);
        ent->_10_pt.y = bit_read_BD(dat);
        ent->_10_pt.z = bit_read_BD(dat);
        ent->_13_pt.x = bit_read_BD(dat);
        ent->_13_pt.y = bit_read_BD(dat);
        ent->_13_pt.z = bit_read_BD(dat);
        ent->_14_pt.x = bit_read_BD(dat);
        ent->_14_pt.y = bit_read_BD(dat);
        ent->_14_pt.z = bit_read_BD(dat);
        ent->_15_pt.x = bit_read_BD(dat);
        ent->_15_pt.y = bit_read_BD(dat);
        ent->_15_pt.z = bit_read_BD(dat);

        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_DIMENSION_ANG2LN (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_DIMENSION_ANG2LN *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.DIMENSION_ANG2LN = calloc (sizeof (Dwg_Entity_DIMENSION_ANG2LN), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.DIMENSION_ANG2LN;

        //TODO: check extrusion reading
        //bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);
        ent->x0 = bit_read_RD (dat);
        ent->y0 = bit_read_RD (dat);
        ent->elevation.ecs_11 = bit_read_BD(dat);
        ent->elevation.ecs_12 = bit_read_BD(dat);
        //spec: flag bit 2 indicates ANGULAR 2 line dimension
        ent->flags = bit_read_RC(dat);
        ent->user_text = bit_read_T(dat);
        ent->text_rot = bit_read_BD(dat);
        ent->horiz_dir = bit_read_BD(dat);
        ent->ins_scale.x = bit_read_BD(dat);
        ent->ins_scale.y = bit_read_BD(dat);
        ent->ins_scale.z = bit_read_BD(dat);
        ent->ins_rotation = bit_read_BD(dat);

        if(dat->version >= R_2000){
            ent->attachment_point = bit_read_BS(dat);
            ent->lspace_style = bit_read_BS(dat);
            ent->lspace_factor = bit_read_BD(dat);
            ent->act_measurement = bit_read_BD(dat);
        }
        if(dat->version >= R_2007){
            ent->unknown = bit_read_B(dat);
            ent->flip_arrow1 = bit_read_B(dat);
            ent->flip_arrow2 = bit_read_B(dat);
        }

        ent->_12_pt.x = bit_read_RD(dat);
        ent->_12_pt.y = bit_read_RD(dat);
        ent->_16_pt.x = bit_read_RD(dat);
        ent->_16_pt.y = bit_read_RD(dat);
        ent->_13_pt.x = bit_read_BD(dat);
        ent->_13_pt.y = bit_read_BD(dat);
        ent->_13_pt.z = bit_read_BD(dat);
        ent->_14_pt.x = bit_read_BD(dat);
        ent->_14_pt.y = bit_read_BD(dat);
        ent->_14_pt.z = bit_read_BD(dat);
        ent->_15_pt.x = bit_read_BD(dat);
        ent->_15_pt.y = bit_read_BD(dat);
        ent->_15_pt.z = bit_read_BD(dat);
        ent->_10_pt.x = bit_read_BD(dat);
        ent->_10_pt.y = bit_read_BD(dat);
        ent->_10_pt.z = bit_read_BD(dat);

        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_DIMENSION_RADIUS (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_DIMENSION_RADIUS *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.DIMENSION_RADIUS = calloc (sizeof (Dwg_Entity_DIMENSION_RADIUS), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.DIMENSION_RADIUS;

        //TODO: check extrusion reading
        //bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);
        ent->x0 = bit_read_RD (dat);
        ent->y0 = bit_read_RD (dat);
        ent->elevation.ecs_11 = bit_read_BD(dat);
        ent->elevation.ecs_12 = bit_read_BD(dat);
        //spec: flag bit 4 indicates RADIUS dimension
        ent->flags = bit_read_RC(dat);
        ent->user_text = bit_read_T(dat);
        ent->text_rot = bit_read_BD(dat);
        ent->horiz_dir = bit_read_BD(dat);
        ent->ins_scale.x = bit_read_BD(dat);
        ent->ins_scale.y = bit_read_BD(dat);
        ent->ins_scale.z = bit_read_BD(dat);
        ent->ins_rotation = bit_read_BD(dat);

        if(dat->version >= R_2000){
            ent->attachment_point = bit_read_BS(dat);
            ent->lspace_style = bit_read_BS(dat);
            ent->lspace_factor = bit_read_BD(dat);
            ent->act_measurement = bit_read_BD(dat);
        }
        if(dat->version >= R_2007){
            ent->unknown = bit_read_B(dat);
            ent->flip_arrow1 = bit_read_B(dat);
            ent->flip_arrow2 = bit_read_B(dat);
        }

        ent->_12_pt.x = bit_read_RD(dat);
        ent->_12_pt.y = bit_read_RD(dat);
        ent->_10_pt.x = bit_read_BD(dat);
        ent->_10_pt.y = bit_read_BD(dat);
        ent->_10_pt.z = bit_read_BD(dat);
        ent->_15_pt.x = bit_read_BD(dat);
        ent->_15_pt.y = bit_read_BD(dat);
        ent->_15_pt.z = bit_read_BD(dat);
        ent->leader_len = bit_read_BD(dat);

        dwg_decode_traktref (dat, obj);
}
static void
dwg_decode_DIMENSION_DIAMETER (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_DIMENSION_DIAMETER *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.DIMENSION_DIAMETER = calloc (sizeof (Dwg_Entity_DIMENSION_DIAMETER), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.DIMENSION_DIAMETER;

        //TODO: check extrusion reading
        //bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);
        ent->x0 = bit_read_RD (dat);
        ent->y0 = bit_read_RD (dat);
        ent->elevation.ecs_11 = bit_read_BD(dat);
        ent->elevation.ecs_12 = bit_read_BD(dat);
        //spec: flag bit 3 indicates DIAMETER dimension
        //(spec says actually RADIUS but seems to be wrong)
        ent->flags = bit_read_RC(dat);
        ent->user_text = bit_read_T(dat);
        ent->text_rot = bit_read_BD(dat);
        ent->horiz_dir = bit_read_BD(dat);
        ent->ins_scale.x = bit_read_BD(dat);
        ent->ins_scale.y = bit_read_BD(dat);
        ent->ins_scale.z = bit_read_BD(dat);
        ent->ins_rotation = bit_read_BD(dat);

        if(dat->version >= R_2000){
            ent->attachment_point = bit_read_BS(dat);
            ent->lspace_style = bit_read_BS(dat);
            ent->lspace_factor = bit_read_BD(dat);
            ent->act_measurement = bit_read_BD(dat);
        }
        if(dat->version >= R_2007){
            ent->unknown = bit_read_B(dat);
            ent->flip_arrow1 = bit_read_B(dat);
            ent->flip_arrow2 = bit_read_B(dat);
        }

        ent->_12_pt.x = bit_read_RD(dat);
        ent->_12_pt.y = bit_read_RD(dat);
        ent->_15_pt.x = bit_read_BD(dat);
        ent->_15_pt.y = bit_read_BD(dat);
        ent->_15_pt.z = bit_read_BD(dat);
        ent->_10_pt.x = bit_read_BD(dat);
        ent->_10_pt.y = bit_read_BD(dat);
        ent->_10_pt.z = bit_read_BD(dat);
        ent->leader_len = bit_read_BD(dat);

        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POINT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_POINT *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POINT = calloc (sizeof (Dwg_Entity_POINT), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.POINT;

	ent->x = bit_read_BD (dat);
	ent->y = bit_read_BD (dat);
	ent->z = bit_read_BD (dat);
	ent->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);
	ent->x_ang = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_3DFACE (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_3D_FACE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio._3DFACE = calloc (sizeof (Dwg_Entity_3D_FACE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio._3DFACE;

        if (dat->version == R_13 || dat->version == R_14)
        {
            ent->corner1.x = bit_read_BD(dat);
            ent->corner1.y = bit_read_BD(dat);
            ent->corner1.z = bit_read_BD(dat);
            ent->corner2.x = bit_read_BD(dat);
            ent->corner2.y = bit_read_BD(dat);
            ent->corner2.z = bit_read_BD(dat);
            ent->corner3.x = bit_read_BD(dat);
            ent->corner3.y = bit_read_BD(dat);
            ent->corner3.z = bit_read_BD(dat);
            ent->corner4.x = bit_read_BD(dat);
            ent->corner4.y = bit_read_BD(dat);
            ent->corner4.z = bit_read_BD(dat);
            ent->invis_flags = bit_read_BS(dat);
        }
        if (dat->version >= R_2000)
        {
            ent->has_no_flags = bit_read_B (dat);
            ent->z_is_zero = bit_read_B (dat);
            ent->corner1.x = bit_read_BD(dat);
            ent->corner1.y = bit_read_BD(dat);
            ent->corner1.z = bit_read_BD(dat);
            ent->corner2.x = bit_read_BD(dat);
            ent->corner2.y = bit_read_BD(dat);
            ent->corner2.z = bit_read_BD(dat);
            ent->corner3.x = bit_read_BD(dat);
            ent->corner3.y = bit_read_BD(dat);
            ent->corner3.z = bit_read_BD(dat);
            ent->corner4.x = bit_read_BD(dat);
            ent->corner4.y = bit_read_BD(dat);
            ent->corner4.z = bit_read_BD(dat);
        }
        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_PFACE (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_POLYLINE_PFACE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POLYLINE_PFACE = calloc (sizeof (Dwg_Entity_POLYLINE_PFACE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.POLYLINE_PFACE;

        ent->numverts = bit_read_BS(dat);
        ent->numfaces = bit_read_BS(dat);

        if (dat->version >= R_2004)
        {
            ent->owned_object_count = bit_read_BL(dat);
        }

        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_MESH (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_POLYLINE_MESH *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POLYLINE_MESH = calloc (sizeof (Dwg_Entity_POLYLINE_MESH), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.POLYLINE_MESH;

        ent->flags = bit_read_BS(dat);
        ent->curve_type = bit_read_BS(dat);
        ent->m_vert_count = bit_read_BS(dat);
        ent->n_vert_count = bit_read_BS(dat);
        ent->m_density = bit_read_BS(dat);
        ent->n_density = bit_read_BS(dat);

        if (dat->version >= R_2004)
        {
            ent->owned_object_count = bit_read_BL(dat);
        }

        dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_SOLID (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_SOLID *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.SOLID = calloc (sizeof (Dwg_Entity_SOLID), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.SOLID;

	ent->thickness = bit_read_BT (dat);
	ent->corner1.z = ent->corner2.z = ent->corner3.z = ent->corner4.z = bit_read_BD(dat);
	ent->corner1.x = bit_read_RD (dat);
	ent->corner1.y = bit_read_RD (dat);
        ent->corner2.x = bit_read_RD (dat);
	ent->corner2.y = bit_read_RD (dat);
        ent->corner3.x = bit_read_RD (dat);
	ent->corner3.y = bit_read_RD (dat);
        ent->corner4.x = bit_read_RD (dat);
	ent->corner4.y = bit_read_RD (dat);
        bit_read_BE(dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_TRACE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_TRACE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.TRACE = calloc (sizeof (Dwg_Entity_TRACE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.TRACE;

	ent->thickness = bit_read_BT (dat);
	ent->corner1.z = ent->corner2.z = ent->corner3.z = ent->corner4.z = bit_read_BD(dat);
	ent->corner1.x = bit_read_RD (dat);
	ent->corner1.y = bit_read_RD (dat);
        ent->corner2.x = bit_read_RD (dat);
	ent->corner2.y = bit_read_RD (dat);
        ent->corner3.x = bit_read_RD (dat);
	ent->corner3.y = bit_read_RD (dat);
        ent->corner4.x = bit_read_RD (dat);
	ent->corner4.y = bit_read_RD (dat);
        bit_read_BE(dat, &ent->extrusion.x, &ent->extrusion.y, &ent->extrusion.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_SHAPE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_SHAPE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.SHAPE = calloc (sizeof (Dwg_Entity_SHAPE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.SHAPE;

        ent->ins_pt.x = bit_read_BD(dat);
        ent->ins_pt.y = bit_read_BD(dat);
        ent->ins_pt.z = bit_read_BD(dat);
        ent->scale = bit_read_BD(dat);
        ent->rotation = bit_read_BD(dat);
        ent->width_factor = bit_read_BD(dat);
        ent->oblique = bit_read_BD(dat);
        ent->thickness = bit_read_BD(dat);
        ent->shape_no = bit_read_BS(dat);
        ent->extrusion.x = bit_read_BD(dat);
        ent->extrusion.y = bit_read_BD(dat);
        ent->extrusion.z = bit_read_BD(dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ELLIPSE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ELLIPSE *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ELLIPSE = calloc (sizeof (Dwg_Entity_ELLIPSE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.ELLIPSE;

	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);
	ent->x1 = bit_read_BD (dat);
	ent->y1 = bit_read_BD (dat);
	ent->z1 = bit_read_BD (dat);
	ent->extrusion.x = bit_read_BD (dat);
	ent->extrusion.y = bit_read_BD (dat);
	ent->extrusion.z = bit_read_BD (dat);
	ent->radiusproporcion = bit_read_BD (dat);
	ent->start_angle = bit_read_BD (dat);
	ent->end_angle = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_SPLINE (Bit_Chain * dat, Dwg_Object * obj)
{
        Dwg_Entity_SPLINE *ent;
        int i;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.SPLINE = calloc (sizeof (Dwg_Entity_SPLINE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.SPLINE;

        ent->scenario = bit_read_BS(dat);
        ent->degree = bit_read_BS(dat);
        if(ent->scenario == 2)
        {
            ent->fit_tol = bit_read_BD(dat);
            ent->beg_tan_vec.x = bit_read_BD(dat);
            ent->beg_tan_vec.y = bit_read_BD(dat);
            ent->beg_tan_vec.z = bit_read_BD(dat);
            ent->end_tan_vec.x = bit_read_BD(dat);
            ent->end_tan_vec.y = bit_read_BD(dat);
            ent->end_tan_vec.z = bit_read_BD(dat);
            ent->num_fit_pts = bit_read_BS(dat);
            ent->fit_pts = malloc(ent->num_fit_pts*
                    sizeof(((Dwg_Entity_SPLINE *)(NULL))->fit_pts));
            for (i=0;i<ent->num_fit_pts;i++)
            {
                ent->fit_pts[i].x = bit_read_BD(dat);
                ent->fit_pts[i].y = bit_read_BD(dat);
                ent->fit_pts[i].z = bit_read_BD(dat);
            }
        } else
        {
            if (ent->scenario == 1)
            {
                    ent->rational = bit_read_B(dat);
                    ent->closed_b = bit_read_B(dat);
                    ent->periodic = bit_read_B(dat);
                    ent->knot_tol = bit_read_BD(dat);
                    ent->ctrl_tol = bit_read_BD(dat);
                    ent->num_knots = bit_read_BL(dat);
                    ent->num_ctrl_pts = bit_read_BL(dat);
                    ent->weighted = bit_read_B(dat);
                    
                    ent->knots = malloc(ent->num_knots *
                        sizeof (((Dwg_Entity_SPLINE *) (NULL))->knots));
                    for (i=0;i<ent->num_knots;i++)
                        ent->knots[i].value = bit_read_BD(dat);
                    
                    ent->ctrl_pts = malloc(ent->num_ctrl_pts *
                        sizeof (((Dwg_Entity_SPLINE *) (NULL))->ctrl_pts));
                    
                    for (i=0;i<ent->num_ctrl_pts;i++) 
                    {
                            ent->ctrl_pts[i].x = bit_read_BD(dat);
                            ent->ctrl_pts[i].y = bit_read_BD(dat);
                            ent->ctrl_pts[i].z = bit_read_BD(dat);                
                            if (ent->weighted)
                                //TODO check what "D" means on spec. 
                                //assuming typo - should be BD
                                ent->ctrl_pts[i].w = bit_read_BD(dat);
                    }
            } else
            {
                 fprintf (stderr, "Error: unknown scenario %d", ent->scenario);
            }
        }
	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_RAY (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_RAY *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.RAY = calloc (sizeof (Dwg_Entity_RAY), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.RAY;

	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->z0 = bit_read_BD (dat);
	ent->x1 = bit_read_BD (dat);
	ent->y1 = bit_read_BD (dat);
	ent->z1 = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_DICTIONARY (Bit_Chain *dat, Dwg_Object *obj)
{
	int i;
	Dwg_Object_DICTIONARY *dict;

	obj->supertype = DWG_SUPERTYPE_OBJECT;
	obj->tio.object = malloc (sizeof (Dwg_Object_Object));
	obj->tio.object->tio.DICTIONARY = calloc (sizeof (Dwg_Object_DICTIONARY), 1);
	dwg_decode_object (dat, obj->tio.object);
	dict = obj->tio.object->tio.DICTIONARY;

	dict->size = bit_read_BS (dat);
	dict->cloning = bit_read_BS (dat);
	dict->hard_owner = bit_read_RC (dat);
	if (dict->size > 10000)
	{
//TODO:Dwg_Handle
//		fprintf (stderr, "Strange: dictionary with more than 10 thousand entries! Handle: %u\n", obj->trakt.value);
		return;
	}
	dict->name = calloc (sizeof (char *), dict->size);
	for (i = 0; i < dict->size; i++)
		dict->name[i] = bit_read_T (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_MTEXT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_MTEXT *ent;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.MTEXT = calloc (sizeof (Dwg_Entity_MTEXT), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	ent = obj->tio.entity->tio.MTEXT;

	ent->x0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->y0 = bit_read_BD (dat);
	ent->extrusion.x = bit_read_BD (dat);
	ent->extrusion.y = bit_read_BD (dat);
	ent->extrusion.z = bit_read_BD (dat);
	ent->x1 = bit_read_BD (dat);
	ent->y1 = bit_read_BD (dat);
	ent->z1 = bit_read_BD (dat);
	ent->width = bit_read_BD (dat);
	ent->height = bit_read_BD (dat);
	ent->kunmeto = bit_read_BS (dat);
	ent->direkto = bit_read_BS (dat);
	ent->etendo = bit_read_BD (dat);
	ent->etendlargxo = bit_read_BD (dat);
	ent->text = bit_read_T (dat);
	ent->linispaco_stilo = bit_read_BS (dat);
	ent->linispaco_faktoro = bit_read_BD (dat);
	ent->ia_bit = bit_read_B (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_BLOCK_CONTROL (Bit_Chain *dat, Dwg_Object *obj)
{
	int i;
	Dwg_Object_BLOCK_CONTROL *blk;

	obj->supertype = DWG_SUPERTYPE_OBJECT;
	obj->tio.object = malloc (sizeof (Dwg_Object_Object));
	obj->tio.object->tio.BLOCK_CONTROL = calloc (sizeof (Dwg_Object_BLOCK_CONTROL), 1);
	dwg_decode_object (dat, obj->tio.object);
	blk = obj->tio.object->tio.BLOCK_CONTROL;
	
	blk->size = bit_read_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_LAYER (Bit_Chain * dat, Dwg_Object * obj)
{
    if (loglevel) fprintf(stderr, "dwg_decode_LAYER\n");
	Dwg_Object_LAYER *ord;

	obj->supertype = DWG_SUPERTYPE_OBJECT;
	obj->tio.object = malloc (sizeof (Dwg_Object_Object));
	obj->tio.object->tio.LAYER = calloc (sizeof (Dwg_Object_LAYER), 1);
	dwg_decode_object (dat, obj->tio.object);
	ord = obj->tio.object->tio.LAYER;

	/* Read values
	 */
	ord->name = bit_read_T (dat);
	ord->bit64 = bit_read_B (dat);
	ord->xrefi = bit_read_BS (dat);
	ord->xrefdep = bit_read_B (dat);
	ord->ecoj = bit_read_BS (dat);
	ord->colour = bit_read_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_LAYOUT (Bit_Chain * dat, Dwg_Object * obj)
{
    if (loglevel) fprintf(stderr, "dwg_decode_LAYOUT\n");
	Dwg_Object_LAYOUT *ord;

	obj->supertype = DWG_SUPERTYPE_OBJECT;
	obj->tio.object = malloc (sizeof (Dwg_Object_Object));
	obj->tio.object->tio.LAYOUT = calloc (sizeof (Dwg_Object_LAYOUT), 1);
	dwg_decode_object (dat, obj->tio.object);
	ord = obj->tio.object->tio.LAYOUT;

	/* Read values
	 */
	ord->pagxo.agordo = bit_read_T (dat);
	ord->pagxo.printilo = bit_read_T (dat);
	ord->pagxo.flags = bit_read_BS (dat);
	ord->pagxo.maldekstre = bit_read_BD (dat);
	ord->pagxo.malsupre = bit_read_BD (dat);
	ord->pagxo.dekstre = bit_read_BD (dat);
	ord->pagxo.supre = bit_read_BD (dat);
	ord->pagxo.width = bit_read_BD (dat);
	ord->pagxo.height = bit_read_BD (dat);
	ord->pagxo.size = bit_read_T (dat);
	ord->pagxo.dx = bit_read_BD (dat);
	ord->pagxo.dy = bit_read_BD (dat);
	ord->pagxo.unuoj = bit_read_BS (dat);
	ord->pagxo.rotacio = bit_read_BS (dat);
	ord->pagxo.type = bit_read_BS (dat);
	ord->pagxo.x_min = bit_read_BD (dat);
	ord->pagxo.y_min = bit_read_BD (dat);
	ord->pagxo.x_maks = bit_read_BD (dat);
	ord->pagxo.y_maks = bit_read_BD (dat);
	ord->pagxo.name = bit_read_T (dat);
	ord->pagxo.scale.A = bit_read_BD (dat);
	ord->pagxo.scale.B = bit_read_BD (dat);
	ord->pagxo.stilfolio = bit_read_T (dat);
	ord->pagxo.scale.type = bit_read_BS (dat);
	ord->pagxo.scale.faktoro = bit_read_BD (dat);
	ord->pagxo.x0 = bit_read_BD (dat);
	ord->pagxo.y0 = bit_read_BD (dat);

	ord->name = bit_read_T (dat);
	ord->ordo = bit_read_BS (dat);
	ord->flags = bit_read_BS (dat);
	ord->x0 = bit_read_BD (dat);
	ord->y0 = bit_read_BD (dat);
	ord->z0 = bit_read_BD (dat);
	ord->x_min = bit_read_RD (dat);
	ord->y_min = bit_read_RD (dat);
	ord->x_maks = bit_read_RD (dat);
	ord->y_maks = bit_read_RD (dat);
	ord->enmeto.x0 = bit_read_BD (dat);
	ord->enmeto.y0 = bit_read_BD (dat);
	ord->enmeto.z0 = bit_read_BD (dat);
	ord->akso_X.x0 = bit_read_BD (dat);
	ord->akso_X.y0 = bit_read_BD (dat);
	ord->akso_X.z0 = bit_read_BD (dat);
	ord->akso_Y.x0 = bit_read_BD (dat);
	ord->akso_Y.y0 = bit_read_BD (dat);
	ord->akso_Y.z0 = bit_read_BD (dat);
	ord->elevation = bit_read_BD (dat);
	ord->rigardtype = bit_read_BS (dat);
	ord->limo.x_min = bit_read_BD (dat);
	ord->limo.y_min = bit_read_BD (dat);
	ord->limo.z_min = bit_read_BD (dat);
	ord->limo.x_maks = bit_read_BD (dat);
	ord->limo.y_maks = bit_read_BD (dat);
	ord->limo.z_maks = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

/*--------------------------------------------------------------------------------
 * Privata funkcio, kiu dependas de la antaŭaj
 */
static void
dwg_decode_aldoni_object (Dwg_Structure * skt, Bit_Chain * dat, long unsigned int address)
{
	long unsigned int antauxa_address;
	long unsigned int objekadres;
	unsigned char antauxa_bit;
	Dwg_Object *obj;

	/* Gardi la antauxan addressn
	 */
	antauxa_address = dat->byte;
	antauxa_bit = dat->bit;

	/* Uzi la indikitan addressn por la object
	 */
	dat->byte = address;
	dat->bit = 0;

	/* Rezervi memor-spacon por plia object
	 */
	if (skt->num_objects == 0)
		skt->object = (Dwg_Object *) malloc (sizeof (Dwg_Object));
	else
		skt->object =
			(Dwg_Object *) realloc (skt->object,
						 (skt->num_objects + 1) * sizeof (Dwg_Object));

	//printf ("Object number: %u\n", skt->num_objects);

	obj = &skt->object[skt->num_objects];
	skt->num_objects++;

	obj->size = bit_read_MS (dat);
	objekadres = dat->byte;
	ktl_lastaddress = dat->byte + obj->size;	/* (de cxi tie oni kalkulas la bitsizen) */
	obj->type = bit_read_BS (dat);

	/* Kontroli la typen de object
	 */
	switch (obj->type)
	{
	case DWG_TYPE_TEXT:
		dwg_decode_TEXT (dat, obj);
		break;
	case DWG_TYPE_ATTRIB:
		dwg_decode_ATTRIB (dat, obj);
		break;
	case DWG_TYPE_ATTDEF:
		dwg_decode_ATTDEF (dat, obj);
		break;
	case DWG_TYPE_BLOCK:
		dwg_decode_BLOCK (dat, obj);
		break;
	case DWG_TYPE_ENDBLK:
		dwg_decode_ENDBLK (dat, obj);
		break;
	case DWG_TYPE_SEQEND:
		dwg_decode_SEQEND (dat, obj);
		break;
	case DWG_TYPE_INSERT:
		dwg_decode_INSERT (dat, obj);
		break;
	case DWG_TYPE_MINSERT:
		dwg_decode_MINSERT (dat, obj);
		break;
	case DWG_TYPE_VERTEX_2D:
		dwg_decode_VERTEX_2D (dat, obj);
		break;
	case DWG_TYPE_VERTEX_3D:
	case DWG_TYPE_VERTEX_MESH:
	case DWG_TYPE_VERTEX_PFACE:
		dwg_decode_VERTEX_3D (dat, obj);
		break;
	case DWG_TYPE_VERTEX_PFACE_FACE:
		dwg_decode_VERTEX_PFACE_FACE (dat, obj);
		break;
	case DWG_TYPE_POLYLINE_2D:
		dwg_decode_POLYLINE_2D (dat, obj);
		break;
	case DWG_TYPE_POLYLINE_3D:
		dwg_decode_POLYLINE_3D (dat, obj);
		break;
	case DWG_TYPE_ARC:
		dwg_decode_ARC (dat, obj);
		break;
	case DWG_TYPE_CIRCLE:
		dwg_decode_CIRCLE (dat, obj);
		break;
	case DWG_TYPE_LINE:
		dwg_decode_LINE (dat, obj);
		break;
        case DWG_TYPE_DIMENSION_ORDINATE:
                dwg_decode_DIMENSION_ORDINATE(dat, obj);
                break;
        case DWG_TYPE_DIMENSION_LINEAR:
                dwg_decode_DIMENSION_LINEAR(dat, obj);
                break;
        case DWG_TYPE_DIMENSION_ALIGNED:
                dwg_decode_DIMENSION_ALIGNED(dat, obj);
                break;
        case DWG_TYPE_DIMENSION_ANG3PT:
                dwg_decode_DIMENSION_ANG3PT(dat, obj);
                break;
        case DWG_TYPE_DIMENSION_ANG2LN:
                dwg_decode_DIMENSION_ANG2LN(dat, obj);
                break;
        case DWG_TYPE_DIMENSION_RADIUS:
                dwg_decode_DIMENSION_RADIUS(dat, obj);
                break;
        case DWG_TYPE_DIMENSION_DIAMETER:
                dwg_decode_DIMENSION_DIAMETER(dat, obj);
                break;
	case DWG_TYPE_POINT:
		dwg_decode_POINT (dat, obj);
		break;
	case DWG_TYPE_3DFACE:
		dwg_decode_3DFACE (dat, obj);
		break;
	case DWG_TYPE_POLYLINE_PFACE:
		dwg_decode_POLYLINE_PFACE (dat, obj);
		break;
	case DWG_TYPE_POLYLINE_MESH:
		dwg_decode_POLYLINE_MESH (dat, obj);
		break;
	case DWG_TYPE_SOLID:
		dwg_decode_SOLID (dat, obj);
		break;
	case DWG_TYPE_TRACE:
		dwg_decode_TRACE (dat, obj);
		break;
	case DWG_TYPE_SHAPE:
		dwg_decode_SHAPE (dat, obj);
		break;
	case DWG_TYPE_ELLIPSE:
		dwg_decode_ELLIPSE (dat, obj);
		break;
// Commenting out code to avoid segfault (TODO: review spline)
//        case DWG_TYPE_SPLINE:
//		dwg_decode_SPLINE (dat, obj);
//                break;
	case DWG_TYPE_RAY:
	case DWG_TYPE_XLINE:
		dwg_decode_RAY (dat, obj);
		break;
	case DWG_TYPE_MTEXT:
		dwg_decode_MTEXT (dat, obj);
		break;
        case DWG_TYPE_BLOCK_CONTROL:
                dwg_decode_BLOCK_CONTROL (dat, obj);
                break;
        case DWG_TYPE_DICTIONARY:
                dwg_decode_DICTIONARY (dat, obj);
                break;
	case DWG_TYPE_LAYER:
		dwg_decode_LAYER (dat, obj);
		break;
	default:
		if (obj->type == skt->dwg_ot_layout)
			dwg_decode_LAYOUT (dat, obj);
		else
		{
			obj->supertype = DWG_SUPERTYPE_UNKNOWN;
			obj->tio.unknownjxo = malloc (obj->size);
			memcpy (obj->tio.unknownjxo, &dat->chain[objekadres], obj->size);
		}
	}

	/*
	   if (obj->supertype != DWG_SUPERTYPE_UNKNOWN)
	   {
	   printf (" Ekadr:\t%10lu\n", address);
	   printf (" Lasta:\t%10lu\tSize: %10lu\n", dat->byte, obj->size);
	   printf ("Finadr:\t%10lu (kalkulite)\n", address + 2 + obj->size);
	   }
	 */

	/* Restarigi la antauxan addressn por returni
	 */
	dat->byte = antauxa_address;
	dat->bit = antauxa_bit;
}


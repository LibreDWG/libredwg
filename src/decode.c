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
			skt->var[i].traktilo.code = 0;
			skt->var[i].traktilo.value = 0;
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
			bit_read_H (dat, &skt->var[i].traktilo);
			if (loglevel) printf ("H: %i.%i.0x%08X", skt->var[i].traktilo.code, skt->var[i].traktilo.size, (unsigned int) skt->var[i].traktilo.value);
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
dwg_decode_entity (Bit_Chain * dat, Dwg_Object_Entity * est)
{
	unsigned int i;
	unsigned int grando;
	int error = 2;

	est->bitsize = bit_read_RL (dat);
	error = bit_read_H (dat, &est->traktilo);
	if (error)
	{
		fprintf (stderr, "dwg_decode_entity:\tEraro en traktilo de object! Adreso en la ĉeno: 0x%0x\n", (unsigned int) dat->byte);
		est->bitsize = 0;
		est->kromdat_size = 0;
		est->picture_ekzistas = 0;
		est->traktref_size = 0;
		return;
	}
	est->kromdat_size = 0;
	while (grando = bit_read_BS (dat))
	{
		if (grando > 10210)
		{
			fprintf (stderr, "dwg_decode_entity: Absurdo! Kromdato-size: %lu. Object: %lu (traktilo).\n", (long unsigned int) grando, est->traktilo.value);
			est->bitsize = 0;
			est->kromdat_size = 0;
			est->picture_ekzistas = 0;
			est->traktref_size = 0;
			return;
		}
		if (est->kromdat_size == 0)
		{
			est->kromdat = malloc (grando);
			est->kromdat_size = grando;
		}
		else
		{
			est->kromdat_size += grando;
			est->kromdat = realloc (est->kromdat, est->kromdat_size);
		}
		error = bit_read_H (dat, &est->kromdat_trakt);
		if (error)
			fprintf (stderr, "Ops...\n");
		for (i = est->kromdat_size - grando; i < est->kromdat_size; i++)
			est->kromdat[i] = bit_read_RC (dat);
	}
	est->picture_ekzistas = bit_read_B (dat);
	if (est->picture_ekzistas)
	{
		est->picture_size = bit_read_RL (dat);
		if (est->picture_size < 210210)
		{
			est->picture = malloc (est->picture_size);
			for (i = 0; i < est->picture_size; i++)
				est->picture[i] = bit_read_RC (dat);
		}
		else
		{
			fprintf (stderr, "dwg_decode_entity:  Absurdo! Bildo-size: %lu kB. Object: %lu (traktilo).\n",
				est->picture_size / 1000, est->traktilo.value);
			bit_advance_position (dat, -(4 * 8 + 1));
		}
	}

	est->regime = bit_read_BB (dat);
	est->reagilo_size = bit_read_BL (dat);
	est->senligiloj = bit_read_B (dat);
	est->colour = bit_read_BS (dat);
	est->linitypescale = bit_read_BD (dat);
	est->linitype = bit_read_BB (dat);
	est->printstilo = bit_read_BB (dat);
	est->malvidebleco = bit_read_BS (dat);
	est->linithickness = bit_read_RC (dat);
}

static void
dwg_decode_object (Bit_Chain * dat, Dwg_Object_Object * ord)
{
	unsigned int i;
	unsigned int grando;
	int error = 2;

	ord->bitsize = bit_read_RL (dat);
	error = bit_read_H (dat, &ord->traktilo);
	if (error)
	{
		fprintf (stderr, "\tEraro en traktilo de object! Adreso en la ĉeno: 0x%0x\n", (unsigned int) dat->byte);
		ord->bitsize = 0;
		ord->kromdat_size = 0;
		ord->traktref_size = 0;
		return;
	}
	ord->kromdat_size = 0;
	while (grando = bit_read_BS (dat))
	{
		if (grando > 10210)
		{
			fprintf (stderr, "dwg_decode_object: Absurdo! Kromdato-size: %lu. Object: %lu (traktilo).\n", (long unsigned int) grando, ord->traktilo.value);
			ord->bitsize = 0;
			ord->kromdat_size = 0;
			ord->traktref_size = 0;
			return;
		}
		if (ord->kromdat_size == 0)
		{
			ord->kromdat = malloc (grando);
			ord->kromdat_size = grando;
		}
		else
		{
			ord->kromdat_size += grando;
			ord->kromdat = realloc (ord->kromdat, ord->kromdat_size);
		}
		error = bit_read_H (dat, &ord->kromdat_trakt);
		if (error)
			fprintf (stderr, "Ops...\n");
		for (i = ord->kromdat_size - grando; i < ord->kromdat_size; i++)
			ord->kromdat[i] = bit_read_RC (dat);
	}

	ord->reagilo_size = bit_read_BL (dat);
}

static void
dwg_decode_traktref (Bit_Chain * dat, Dwg_Object * obj)
{
	int i;

	if (obj->supertype == DWG_SUPERTYPE_ENTITY)
	{
		Dwg_Object_Entity *est;

		est = obj->tio.entity;

		est->traktref = (Dwg_Traktilo *) calloc (sizeof (Dwg_Traktilo), 10);
		i = 0;
		while (1)
		{
			if (i % 10 == 0)
				est->traktref =
					(Dwg_Traktilo *) realloc (est->traktref,
								  (i + 10) * sizeof (Dwg_Traktilo));
			if (bit_read_H (dat, &est->traktref[i]))
			{
				fprintf (stderr, "\tEraro en tiu traktilo: %lu\n", est->traktilo.value);
				break;
			}
			if (!(dat->byte == ktl_lastaddress + 1 && dat->bit == 0))
			{
				if (dat->byte > ktl_lastaddress)
					break;
			}
			i++;
		}
		est->traktref_size = i;
	}
	else
	{
		Dwg_Object_Object *ord;

		ord = obj->tio.object;

		ord->traktref = (Dwg_Traktilo *) calloc (sizeof (Dwg_Traktilo), 10);
		i = 0;
		while (1)
		{
			if (i % 10 == 0)
				ord->traktref =
					(Dwg_Traktilo *) realloc (ord->traktref,
								  (i + 10) * sizeof (Dwg_Traktilo));
			if (bit_read_H (dat, &ord->traktref[i]))
			{
				//fprintf (stderr, "\tEraro en tiu traktilo: %lu\n", est->traktilo.value);
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
dwg_decode_TEXT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_TEXT *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.TEXT = calloc (sizeof (Dwg_Entity_TEXT), 1);
	est = obj->tio.entity->tio.TEXT;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->dataflags = bit_read_RC (dat);
	if ((!est->dataflags & 0x01))
		est->elevation = bit_read_RD (dat);
	est->x0 = bit_read_RD (dat);
	est->y0 = bit_read_RD (dat);
	if (!(est->dataflags & 0x02))
	{
		est->alignment.x = bit_read_DD (dat, 10);
		est->alignment.y = bit_read_DD (dat, 20);
	}
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);
	est->thickness = bit_read_BT (dat);
	if (!(est->dataflags & 0x04))
		est->oblique_ang = bit_read_RD (dat);
	if (!(est->dataflags & 0x08))
		est->rotation_ang = bit_read_RD (dat);
	est->height = bit_read_RD (dat);
	if (!(est->dataflags & 0x10))
		est->width_factor = bit_read_RD (dat);
	est->text = bit_read_T (dat);
	if (!(est->dataflags & 0x20))
		est->generation = bit_read_BS (dat);
	if (!(est->dataflags & 0x40))
		est->alignment.h = bit_read_BS (dat);
	if (!(est->dataflags & 0x80))
		est->alignment.v = bit_read_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ATTRIB (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ATTRIB *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ATTRIB = calloc (sizeof (Dwg_Entity_ATTRIB), 1);
	est = obj->tio.entity->tio.ATTRIB;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->dataflags = bit_read_RC (dat);
	if ((!est->dataflags & 0x01))
		est->elevation = bit_read_RD (dat);
	est->x0 = bit_read_RD (dat);
	est->y0 = bit_read_RD (dat);
	if (!(est->dataflags & 0x02))
	{
		est->alignment.x = bit_read_DD (dat, 10);
		est->alignment.y = bit_read_DD (dat, 20);
	}
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);
	est->thickness = bit_read_BT (dat);
	if (!(est->dataflags & 0x04))
		est->oblique_ang = bit_read_RD (dat);
	if (!(est->dataflags & 0x08))
		est->rotation_ang = bit_read_RD (dat);
	est->height = bit_read_RD (dat);
	if (!(est->dataflags & 0x10))
		est->width_factor = bit_read_RD (dat);
	est->text = bit_read_T (dat);
	if (!(est->dataflags & 0x20))
		est->generation = bit_read_BS (dat);
	if (!(est->dataflags & 0x40))
		est->alignment.h = bit_read_BS (dat);
	if (!(est->dataflags & 0x80))
		est->alignment.v = bit_read_BS (dat);
	est->tag = bit_read_T (dat);
	est->field_length = bit_read_BS (dat);
	est->flags = bit_read_RC (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ATTDEF (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ATTDEF *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ATTDEF = calloc (sizeof (Dwg_Entity_ATTDEF), 1);
	est = obj->tio.entity->tio.ATTDEF;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->dataflags = bit_read_RC (dat);
	if ((!est->dataflags & 0x01))
		est->elevation = bit_read_RD (dat);
	est->x0 = bit_read_RD (dat);
	est->y0 = bit_read_RD (dat);
	if (!(est->dataflags & 0x02))
	{
		est->alignment.x = bit_read_DD (dat, 10);
		est->alignment.y = bit_read_DD (dat, 20);
	}
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);
	est->thickness = bit_read_BT (dat);
	if (!(est->dataflags & 0x04))
		est->oblique_ang = bit_read_RD (dat);
	if (!(est->dataflags & 0x08))
		est->rotation_ang = bit_read_RD (dat);
	est->height = bit_read_RD (dat);
	if (!(est->dataflags & 0x10))
		est->width_factor = bit_read_RD (dat);
	est->text = bit_read_T (dat);
	if (!(est->dataflags & 0x20))
		est->generation = bit_read_BS (dat);
	if (!(est->dataflags & 0x40))
		est->alignment.h = bit_read_BS (dat);
	if (!(est->dataflags & 0x80))
		est->alignment.v = bit_read_BS (dat);
	est->tag = bit_read_T (dat);
	est->field_length = bit_read_BS (dat);
	est->flags = bit_read_RC (dat);
	est->prompt = bit_read_T (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_BLOCK (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_BLOCK *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.BLOCK = calloc (sizeof (Dwg_Entity_BLOCK), 1);
	est = obj->tio.entity->tio.BLOCK;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->name = bit_read_T (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ENDBLK (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ENDBLK *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ENDBLK = calloc (sizeof (Dwg_Entity_ENDBLK), 1);
	est = obj->tio.entity->tio.ENDBLK;
	dwg_decode_entity (dat, obj->tio.entity);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_INSERT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_INSERT *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.INSERT = calloc (sizeof (Dwg_Entity_INSERT), 1);
	est = obj->tio.entity->tio.INSERT;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);
	est->scale_flag = bit_read_BB (dat);
	if (est->scale_flag == 3)
		est->scale.x = est->scale.y = est->scale.y = 1.0;
	else if (est->scale_flag == 1)
	{
		est->scale.x = 1.0;
		est->scale.y = bit_read_DD (dat, 1.0);
		est->scale.z = bit_read_DD (dat, 1.0);
	}
	else if (est->scale_flag == 2)
		est->scale.x = est->scale.y = est->scale.y = bit_read_RD (dat);
	else //if (est->scale_flag == 0)
	{
		est->scale.x = bit_read_RD (dat);
		est->scale.y = bit_read_DD (dat, est->scale.x);
		est->scale.z = bit_read_DD (dat, est->scale.x);
	}
	est->rotation_ang = bit_read_BD (dat);
	est->extrusion.x = bit_read_BD (dat);
	est->extrusion.y = bit_read_BD (dat);
	est->extrusion.z = bit_read_BD (dat);
	est->has_attribs = bit_read_B (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_MINSERT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_MINSERT *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.MINSERT = calloc (sizeof (Dwg_Entity_MINSERT), 1);
	est = obj->tio.entity->tio.MINSERT;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);
	est->scale_flag = bit_read_BB (dat);
	if (est->scale_flag == 3)
		est->scale.x = est->scale.y = est->scale.y = 1.0;
	else if (est->scale_flag == 1)
	{
		est->scale.x = 1.0;
		est->scale.y = bit_read_DD (dat, 1.0);
		est->scale.z = bit_read_DD (dat, 1.0);
	}
	else if (est->scale_flag == 2)
		est->scale.x = est->scale.y = est->scale.y = bit_read_RD (dat);
	else //if (est->scale_flag == 0)
	{
		est->scale.x = bit_read_RD (dat);
		est->scale.y = bit_read_DD (dat, est->scale.x);
		est->scale.z = bit_read_DD (dat, est->scale.x);
	}
	est->rotation_ang = bit_read_BD (dat);
	est->extrusion.x = bit_read_BD (dat);
	est->extrusion.y = bit_read_BD (dat);
	est->extrusion.z = bit_read_BD (dat);
	est->has_attribs = bit_read_B (dat);
	est->column.size = bit_read_BS (dat);
	est->line.size = bit_read_BS (dat);
	est->column.dx = bit_read_BD (dat);
	est->line.dy = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_2D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_VERTEX_2D *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.VERTEX_2D = calloc (sizeof (Dwg_Entity_VERTEX_2D), 1);
	est = obj->tio.entity->tio.VERTEX_2D;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->flags = bit_read_RC (dat);
	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);
	est->start_width = bit_read_BD (dat);
	if (est->start_width < 0)
		est->end_width = est->start_width = -est->start_width;
	else
		est->end_width = bit_read_BD (dat);
	est->bulge = bit_read_BD (dat);
	est->tangent_dir = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_3D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_VERTEX_3D *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.VERTEX_3D = calloc (sizeof (Dwg_Entity_VERTEX_3D), 1);
	est = obj->tio.entity->tio.VERTEX_3D;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->flags = bit_read_RC (dat);
	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_PFACE_FACE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_VERTEX_PFACE_FACE *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.VERTEX_PFACE_FACE = calloc (sizeof (Dwg_Entity_VERTEX_PFACE_FACE), 1);
	est = obj->tio.entity->tio.VERTEX_PFACE_FACE;
	dwg_decode_entity (dat, obj->tio.entity);

	/* Legitaj valuej
	 */
	est->vertind[0] = bit_read_BS (dat);
	est->vertind[1] = bit_read_BS (dat);
	est->vertind[2] = bit_read_BS (dat);
	est->vertind[3] = bit_read_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_2D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_POLYLINE_2D *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POLYLINE_2D = calloc (sizeof (Dwg_Entity_POLYLINE_2D), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.POLYLINE_2D;

	/* Legitaj valuej
	 */
	est->flags = bit_read_BS (dat);
	est->curve_type = bit_read_BS (dat);
	est->start_width = bit_read_BD (dat);
	est->end_width = bit_read_BD (dat);
	est->thickness = bit_read_BT (dat);
	est->elevation = bit_read_BD (dat);
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_3D (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_POLYLINE_3D *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POLYLINE_3D = calloc (sizeof (Dwg_Entity_POLYLINE_3D), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.POLYLINE_3D;

	/* Legitaj valuej
	 */
	est->flags_1 = bit_read_RC (dat);
	est->flags_2 = bit_read_RC (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ARC (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ARC *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ARC = calloc (sizeof (Dwg_Entity_ARC), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.ARC;

	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);
	est->radius = bit_read_BD (dat);
	est->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);
	est->start_angle = bit_read_BD (dat);
	est->end_angle = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_CIRCLE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_CIRCLE *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.CIRCLE = calloc (sizeof (Dwg_Entity_CIRCLE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.CIRCLE;

	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);
	est->radius = bit_read_BD (dat);
	est->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_LINE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_LINE *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.LINE = calloc (sizeof (Dwg_Entity_LINE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.LINE;

	est->nur_2D = bit_read_B (dat);
	est->x0 = bit_read_RD (dat);
	est->x1 = bit_read_DD (dat, est->x0);
	est->y0 = bit_read_RD (dat);
	est->y1 = bit_read_DD (dat, est->y0);
	est->z0 = est->z1 = 0.0;
	if (!est->nur_2D)
	{
		est->z0 = bit_read_RD (dat);
		est->z1 = bit_read_DD (dat, est->z0);
	}
	est->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POINT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_POINT *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.POINT = calloc (sizeof (Dwg_Entity_POINT), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.POINT;

	est->x = bit_read_BD (dat);
	est->y = bit_read_BD (dat);
	est->z = bit_read_BD (dat);
	est->thickness = bit_read_BT (dat);
	bit_read_BE (dat, &est->extrusion.x, &est->extrusion.y, &est->extrusion.z);
	est->x_ang = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ELLIPSE (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_ELLIPSE *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.ELLIPSE = calloc (sizeof (Dwg_Entity_ELLIPSE), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.ELLIPSE;

	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);
	est->x1 = bit_read_BD (dat);
	est->y1 = bit_read_BD (dat);
	est->z1 = bit_read_BD (dat);
	est->extrusion.x = bit_read_BD (dat);
	est->extrusion.y = bit_read_BD (dat);
	est->extrusion.z = bit_read_BD (dat);
	est->radiusproporcio = bit_read_BD (dat);
	est->start_angle = bit_read_BD (dat);
	est->end_angle = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_RAY (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_RAY *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.RAY = calloc (sizeof (Dwg_Entity_RAY), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.RAY;

	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->z0 = bit_read_BD (dat);
	est->x1 = bit_read_BD (dat);
	est->y1 = bit_read_BD (dat);
	est->z1 = bit_read_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_MTEXT (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_MTEXT *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.MTEXT = calloc (sizeof (Dwg_Entity_MTEXT), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.MTEXT;

	est->x0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->y0 = bit_read_BD (dat);
	est->extrusion.x = bit_read_BD (dat);
	est->extrusion.y = bit_read_BD (dat);
	est->extrusion.z = bit_read_BD (dat);
	est->x1 = bit_read_BD (dat);
	est->y1 = bit_read_BD (dat);
	est->z1 = bit_read_BD (dat);
	est->largxeco = bit_read_BD (dat);
	est->height = bit_read_BD (dat);
	est->kunmeto = bit_read_BS (dat);
	est->direkto = bit_read_BS (dat);
	est->etendo = bit_read_BD (dat);
	est->etendlargxo = bit_read_BD (dat);
	est->text = bit_read_T (dat);
	est->linispaco_stilo = bit_read_BS (dat);
	est->linispaco_faktoro = bit_read_BD (dat);
	est->ia_bit = bit_read_B (dat);

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

	/* Legitaj valuej
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

	/* Legitaj valuej
	 */
	ord->pagxo.agordo = bit_read_T (dat);
	ord->pagxo.printilo = bit_read_T (dat);
	ord->pagxo.flags = bit_read_BS (dat);
	ord->pagxo.maldekstre = bit_read_BD (dat);
	ord->pagxo.malsupre = bit_read_BD (dat);
	ord->pagxo.dekstre = bit_read_BD (dat);
	ord->pagxo.supre = bit_read_BD (dat);
	ord->pagxo.largxeco = bit_read_BD (dat);
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

static void
dwg_decode_UNUSED (Bit_Chain * dat, Dwg_Object * obj)
{
	Dwg_Entity_UNUSED *est;

	obj->supertype = DWG_SUPERTYPE_ENTITY;
	obj->tio.entity = malloc (sizeof (Dwg_Object_Entity));
	obj->tio.entity->tio.UNUSED = calloc (sizeof (Dwg_Entity_UNUSED), 1);
	dwg_decode_entity (dat, obj->tio.entity);
	est = obj->tio.entity->tio.UNUSED;

	/* Legitaj valuej
	 */


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
	case DWG_TYPE_SEQEND:
		dwg_decode_ENDBLK (dat, obj);
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
	case DWG_TYPE_POINT:
		dwg_decode_POINT (dat, obj);
		break;
	case DWG_TYPE_ELLIPSE:
		dwg_decode_ELLIPSE (dat, obj);
		break;
	case DWG_TYPE_RAY:
	case DWG_TYPE_XLINE:
		dwg_decode_RAY (dat, obj);
		break;
	case DWG_TYPE_MTEXT:
		dwg_decode_MTEXT (dat, obj);
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


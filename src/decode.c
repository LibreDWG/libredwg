/*****************************************************************************/
/*  LibDWG - Free DWG read-only library                                      */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/// Dekodigo

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "decode.h"

/*--------------------------------------------------------------------------------
 * Privataj funkcioj
 */
static void dwg_decode_aldoni_objekto (Dwg_Structure * skt, Bit_Cxeno * dat, long unsigned int adreso);

/*--------------------------------------------------------------------------------
 * Publikaj variabloj
 */
long unsigned int ktl_lastadreso;

/*--------------------------------------------------------------------------------
 * Difino de publikaj funkcioj
 */
int
dwg_decode_strukturigi (Bit_Cxeno * dat, Dwg_Structure * skt)
{
	unsigned char sig;
	unsigned int sekgrandeco = 0;
	unsigned char sgdc[2];
	unsigned int i, j;
	unsigned int ckr, ckr2, antckr;
	long unsigned int kiom;
	long unsigned int lasta;
	long unsigned int maplasta;
	long unsigned int duabajto;
	long unsigned int obek;
	long unsigned int obfin;
	long unsigned int pvz;

	/*-------------------------------------------------------------------------
	 * Kap-datenaro
	 */

	/* Versio
	 */
	dat->bajto = 0;
	dat->bito = 0;
	strncpy (skt->kapo.versio, dat->cxeno, 6);
	skt->kapo.versio[6] = '\0';
	if (strcmp (skt->kapo.versio, "AC1015") != 0)
	{
		printf ("Nur eblas dekodigi dvg-dosierojn laux la versio R2000 (AC1015). "
			"La trovita versio kodo estas: %s\n", skt->kapo.versio);
		return -1;
	}
	dat->bajto = 0x06;

	// 6 Nuloj kaj valoro
	for (i = 0; i < 7; i++)
	{
		sig = bit_legi_RC (dat);
		//printf ("0x%02X ", sig);
	}
	//puts ("");

	/* Bildadresilo
	 */
	pvz = bit_legi_RL (dat);
	//printf ("0x%08X\n", pvz);

	// Versio kaj lancxo
	sig = bit_legi_RC (dat);
	printf ("Versio: %u\n", sig);
	sig = bit_legi_RC (dat);
	printf ("Lancxo: %u\n", sig);

	/* Kodpagxo
	 */
	dat->bajto = 0x13;
	skt->kapo.kodpagxo = bit_legi_RS (dat);
	printf ("Kodpagxo: %u\n", skt->kapo.kodpagxo);

	/* Sekcioj
	 */
	dat->bajto = 0x15;
	skt->kapo.sekcio_kiom = bit_legi_RL (dat);
	if (skt->kapo.sekcio_kiom > 6)
		skt->kapo.sekcio_kiom = 6;
	for (i = 0; i < skt->kapo.sekcio_kiom; i++)
	{
		skt->kapo.sekcio[i].adresilo = 0;
		skt->kapo.sekcio[i].grandeco = 0;

		skt->kapo.sekcio[i].numero = bit_legi_RC (dat);
		skt->kapo.sekcio[i].adresilo = bit_legi_RL (dat);
		skt->kapo.sekcio[i].grandeco = bit_legi_RL (dat);
	}

	// Kontroli CKR-on
	/*
	   ckr = bit_legi_CRC (dat);
	   dat->bajto -= 2;
	   bit_krei_CRC (dat, 0, 0);
	   dat->bajto -= 2;
	   ckr2 = bit_legi_CRC (dat);
	   dat->bajto -= 2;
	   bit_skribi_RS (dat, ckr2 ^ 0x8461);
	   dat->bajto -= 2;
	   ckr2 = bit_legi_CRC (dat);
	   printf ("Legita: %X\nKreita: %X\n", ckr, ckr2);
	 */

	if (bit_sercxi_gardostaranto (dat, dvg_gardostaranto (DVG_GS_KAPO_FINO)))
		printf ("=======> KAPO (fino): %8X\n", dat->bajto);


	/*-------------------------------------------------------------------------
	 * Nekonata sekcio 1
	 */

	if (skt->kapo.sekcio_kiom == 6)
	{
		printf ("========> NEKONATA 1: %8X\n", skt->kapo.sekcio[5].adresilo);
		printf ("   NEKONATA 1 (fino): %8X\n",
			skt->kapo.sekcio[5].adresilo + skt->kapo.sekcio[5].grandeco);
		dat->bajto = skt->kapo.sekcio[5].adresilo;
		skt->nekonata1.kiom = DVG_NEKONATA1_KIOM;
		skt->nekonata1.bajto = skt->nekonata1.bito = 0;
		skt->nekonata1.cxeno = malloc (skt->nekonata1.kiom);
		memcpy (skt->nekonata1.cxeno, &dat->cxeno[dat->bajto], skt->nekonata1.kiom);
		//bit_esplori_cxeno ((Bit_Cxeno *) &skt->nekonata1, skt->nekonata1.kiom);
		//bit_montri ((Bit_Cxeno *) &skt->nekonata1, skt->nekonata1.kiom);
	}


	/*-------------------------------------------------------------------------
	 * Antauxrigarda bildo
	 */

	if (bit_sercxi_gardostaranto (dat, dvg_gardostaranto (DVG_GS_BILDO_EKO)))
	{
		unsigned long int ekadreso;

		dat->bito = 0;
		ekadreso = dat->bajto;
		printf ("=============> BILDO: %8X\n", ekadreso - 16);
		if (bit_sercxi_gardostaranto (dat, dvg_gardostaranto (DVG_GS_BILDO_FINO)))
		{
			printf ("        BILDO (fino): %8X\n", dat->bajto);
			skt->bildo.kiom = (dat->bajto - 16) - ekadreso;
			skt->bildo.cxeno = (char *) malloc (skt->bildo.kiom);
			memcpy (skt->bildo.cxeno, &dat->cxeno[ekadreso], skt->bildo.kiom);
		}
		else
			skt->bildo.kiom = 0;
	}


	/*-------------------------------------------------------------------------
	 * Kap-variabloj
	 */

	printf ("=====> KAP-VARIABLOJ: %8X\n", skt->kapo.sekcio[0].adresilo);
	printf ("KAP-VARIABLOJ (fino): %8X\n",
		skt->kapo.sekcio[0].adresilo + skt->kapo.sekcio[0].grandeco);
	dat->bajto = skt->kapo.sekcio[0].adresilo + 16;
	pvz = bit_legi_RL (dat);
	//printf ("Longeco: %lu\n", pvz);

	dat->bito = 0;

	/* Legi la kap-variablojn
	 */
	for (i = 0; i < DVG_KIOM_VARIABLOJ; i++)
	{
		//printf ("[%03i] - ", i + 1);
		if (i == 221 && skt->var[220].dubitoko != 3)
		{
			skt->var[i].traktilo.kodo = 0;
			skt->var[i].traktilo.valoro = 0;
			//puts ("(NE EKZISTANTA)");
			continue;
		}
		switch (dvg_varmapo (i))
		{
		case DVG_DT_B:
			skt->var[i].bitoko = bit_legi_B (dat);
			//printf ("B: %u", skt->var[i].bitoko);
			break;
		case DVG_DT_BS:
			skt->var[i].dubitoko = bit_legi_BS (dat);
			//printf ("BS: %u", skt->var[i].dubitoko);
			break;
		case DVG_DT_BL:
			skt->var[i].kvarbitoko = bit_legi_BL (dat);
			//printf ("BL: %lu", skt->var[i].kvarbitoko);
			break;
		case DVG_DT_BD:
			skt->var[i].duglitajxo = bit_legi_BD (dat);
			//printf ("BD: %lg", skt->var[i].duglitajxo);
			break;
		case DVG_DT_H:
			bit_legi_H (dat, &skt->var[i].traktilo);
			//printf ("H: %i.%i.0x%08X", skt->var[i].traktilo.kodo, skt->var[i].traktilo.kiom, skt->var[i].traktilo.valoro);
			break;
		case DVG_DT_T:
			skt->var[i].teksto = bit_legi_T (dat);
			//printf ("T: \"%s\"", skt->var[i].teksto);
			break;
		case DVG_DT_CMC:
			skt->var[i].dubitoko = bit_legi_BS (dat);
			//printf ("CMC: %u", skt->var[i].dubitoko);
			break;
		case DVG_DT_2RD:
			skt->var[i].xy[0] = bit_legi_RD (dat);
			skt->var[i].xy[1] = bit_legi_RD (dat);
			//printf ("X: %lg\t", skt->var[i].xy[0]);
			//printf ("Y: %lg", skt->var[i].xy[1]);
			break;
		case DVG_DT_3BD:
			skt->var[i].xyz[0] = bit_legi_BD (dat);
			skt->var[i].xyz[1] = bit_legi_BD (dat);
			skt->var[i].xyz[2] = bit_legi_BD (dat);
			//printf ("X: %lg\t", skt->var[i].xyz[0]);
			//printf ("Y: %lg\t", skt->var[i].xyz[1]);
			//printf ("Z: %lg", skt->var[i].xyz[2]);
			break;
		default:
			printf ("Ne traktebla tipo: %i (var: %i)\n", dvg_varmapo (i), i);
		}
		//puts ("");
	}

	// Kontroli CKR-on
	ckr = bit_legi_CRC (dat);
	/*
	   for (i = 0xC001; i != 0xC000; i++)
	   {
	   dat->bajto -= 2;
	   bit_krei_CRC (dat, skt->kapo.sekcio[0].adresilo + 16, i);
	   dat->bajto -= 2;
	   ckr2 = bit_legi_CRC (dat);
	   if (ckr == ckr2)
	   {
	   printf ("Legita: %X\nKreita: %X\t SEMO: %02X\n", ckr, ckr2, i);
	   break;
	   }
	   }
	 */


	/*-------------------------------------------------------------------------
	 * Klasoj
	 */

	printf ("============> KLASOJ: %8X\n", skt->kapo.sekcio[1].adresilo);
	printf ("       KLASOJ (fino): %8X\n",
		skt->kapo.sekcio[1].adresilo + skt->kapo.sekcio[1].grandeco);
	dat->bajto = skt->kapo.sekcio[1].adresilo + 16;
	dat->bito = 0;

	kiom = bit_legi_RL (dat);
	lasta = dat->bajto + kiom;
	//printf ("Longeco: %lu\n", kiom);

	/* Legi la klasojn
	 */
	skt->dvg_ot_layout = 0;
	skt->klaso_kiom = 0;
	i = 0;
	do
	{
		unsigned int idc;

		idc = skt->klaso_kiom;
		if (idc == 0)
			skt->klaso = (Dvg_Klaso *) malloc (sizeof (Dvg_Klaso));
		else
			skt->klaso =
				(Dvg_Klaso *) realloc (skt->klaso, (idc + 1) * sizeof (Dvg_Klaso));

		skt->klaso[idc].numero = bit_legi_BS (dat);
		skt->klaso[idc].versio = bit_legi_BS (dat);
		skt->klaso[idc].apnomo = bit_legi_T (dat);
		skt->klaso[idc].cpliplinomo = bit_legi_T (dat);
		skt->klaso[idc].dxfnomo = bit_legi_T (dat);
		skt->klaso[idc].estisfantomo = bit_legi_B (dat);
		skt->klaso[idc].eroid = bit_legi_BS (dat);

		if (strcmp (skt->klaso[idc].dxfnomo, "LAYOUT") == 0)
			skt->dvg_ot_layout = skt->klaso[idc].numero;

		skt->klaso_kiom++;
		if (skt->klaso_kiom > 100)
			break;
	}
	while (dat->bajto < (lasta - 1));

	// Kontroli CKR-on
	ckr = bit_legi_CRC (dat);
	/*
	   for (i = 0xC001; i != 0xC000; i++)
	   {
	   dat->bajto -= 2;
	   bit_krei_CRC (dat, skt->kapo.sekcio[1].adresilo + 16, i);
	   dat->bajto -= 2;
	   ckr2 = bit_legi_CRC (dat);
	   if (ckr == ckr2)
	   {
	   printf ("Legita: %X\nKreita: %X\t SEMO: %02X\n", ckr, ckr2, i);
	   break;
	   }
	   }
	 */

	dat->bajto += 16;
	pvz = bit_legi_RL (dat);	// Nekonata kvarbitoko inter klasoj kaj objektaro
	//printf ("Adreso: %lu / Enhavo: 0x%08X\n", dat->bajto - 4, pvz);
	//printf ("Kiom klasoj legitaj: %u\n", skt->klaso_kiom);


	/*-------------------------------------------------------------------------
	 * Objekto-mapo (kaj objektoj mem)
	 */

	dat->bajto = skt->kapo.sekcio[2].adresilo;
	dat->bito = 0;

	maplasta = dat->bajto + skt->kapo.sekcio[2].grandeco;	// 4
	skt->objekto_kiom = 0;
	obek = dat->kiom;
	obfin = 0;
	do
	{
		long unsigned int lastadres;
		long unsigned int lastatrakt;
		long unsigned int antauxadr;

		duabajto = dat->bajto;
		sgdc[0] = bit_legi_RC (dat);
		sgdc[1] = bit_legi_RC (dat);
		sekgrandeco = (sgdc[0] << 8) | sgdc[1];
		//printf ("sekgrandeco: %u\n", sekgrandeco);
		if (sekgrandeco > 2034)	// 2032 + 2
		{
			puts ("Eraro: Objekto-mapa sekcio pli granda ol 2034!");
			return -1;
		}

		lastatrakt = 0;
		lastadres = 0;
		while (dat->bajto - duabajto < sekgrandeco)
		{
			long unsigned int kobj;
			long int pvztkt;
			long int pvzadr;

			antauxadr = dat->bajto;
			pvztkt = bit_legi_MC (dat);
			lastatrakt += pvztkt;
			pvzadr = bit_legi_MC (dat);
			lastadres += pvzadr;
			//printf ("Idc: %li\t", skt->objekto_kiom);
			//printf ("Trakt: %li\tAdres: %li\n", pvztkt, pvzadr);
			if (dat->bajto == antauxadr)
				break;
			//if (dat->bajto - duabajto >= sekgrandeco)
			//break;

			if (obfin < lastadres)
				obfin = lastadres;
			if (obek > lastadres)
				obek = lastadres;

			kobj = skt->objekto_kiom;
			dwg_decode_aldoni_objekto (skt, dat, lastadres);
			if (skt->objekto_kiom > kobj)
				skt->objekto[skt->objekto_kiom - 1].trakt = lastatrakt;

		}
		if (dat->bajto == antauxadr)
			break;
		dat->bajto += 2;	// CKR

		if (dat->bajto >= maplasta)
			break;
	}
	while (sekgrandeco > 2);
	printf ("Kiom objektoj: %lu\n", skt->objekto_kiom);

	printf ("=========> OBJEKTARO: %8X\n", obek);
	dat->bajto = obfin;
	obek = bit_legi_MS (dat);	// La komenco de la lasta objekto legita
	printf ("    OBJEKTARO (fino): %8X\n", obfin + obek + 2);

	/*
	   dat->bajto = skt->kapo.sekcio[2].adresilo - 2;
	   antckr = bit_legi_CRC (dat); // Nekonata dubitoko inter objektaro kaj objekto-mapo
	   printf ("Adreso: %08X / Enhavo: 0x%04X\n", dat->bajto - 2, antckr);

	   // Kontroli CKR-ojn
	   antckr = 0xC0C1;
	   do
	   {
	   duabajto = dat->bajto;
	   sgdc[0] = bit_legi_RC (dat);
	   sgdc[1] = bit_legi_RC (dat);
	   sekgrandeco = (sgdc[0] << 8) | sgdc[1];
	   sekgrandeco -= 2;
	   dat->bajto += sekgrandeco;
	   ckr = bit_legi_CRC (dat);
	   dat->bajto -= 2;
	   bit_krei_CRC (dat, duabajto, antckr);
	   dat->bajto -= 2;
	   ckr2 = bit_legi_CRC (dat);
	   printf ("Legita: %X\nKreita: %X\t SEMO: %X\n", ckr, ckr2, antckr);
	   //antckr = ckr;
	   } while (sekgrandeco > 0);
	 */

	printf ("======> OBJEKTO-MAPO: %8X\n", skt->kapo.sekcio[2].adresilo);
	printf (" OBJEKTO-MAPO (fino): %8X\n",
		skt->kapo.sekcio[2].adresilo + skt->kapo.sekcio[2].grandeco);

	/*-------------------------------------------------------------------------
	 * Dua kap-datenaro
	 */

	if (bit_sercxi_gardostaranto (dat, dvg_gardostaranto (DVG_GS_DUAKAPO_EKO)))
	{
		long unsigned int pvzadr;
		long unsigned int pvz;
		unsigned char sig, sig2;

		printf ("==> DUA KAP-DATENARO: %8X\n", dat->bajto - 16);
		pvzadr = dat->bajto;

		pvz = bit_legi_RL (dat);
		//printf ("Kiomo: %lu\n", pvz);

		pvz = bit_legi_BL (dat);
		//printf ("Ekadreso: %8X\n", pvz);

		//printf ("AC1015?: ");
		for (i = 0; i < 6; i++)
		{
			sig = bit_legi_RC (dat);
			//printf ("%c", sig >= ' ' && sig < 128 ? sig : '.');
		}

		//printf ("\nNuloj?:");
		for (i = 0; i < 5; i++)	// 6 se estas pli malnova...
		{
			sig = bit_legi_RC (dat);
			//printf (" 0x%02X", sig);
		}

		//printf ("\n4 nulaj bitoj?: ");
		for (i = 0; i < 4; i++)
		{
			sig = bit_legi_B (dat);
			//printf (" %c", sig ? '1' : '0');
		}

		//printf ("\nCxeno?: ");
		for (i = 0; i < 6; i++)
		{
			skt->duakapo.nekonatajxo[i] = bit_legi_RC (dat);
			//printf (" 0x%02X", skt->duakapo.nekonatajxo[i]);
		}
		if (skt->duakapo.nekonatajxo[3] != 0x78 || skt->duakapo.nekonatajxo[5] != 0x06)
			sig = bit_legi_RC (dat);	// por kompenso okaze de eventuala kroma nulo ne legita antauxe

		//puts("");
		for (i = 0; i < 6; i++)
		{
			sig = bit_legi_RC (dat);
			//printf ("[%u]\n", sig);
			pvz = bit_legi_BL (dat);
			//printf (" Adreso: %8X\n", pvz);
			pvz = bit_legi_BL (dat);
			//printf ("  Kiomo: %8X\n", pvz);
		}

		bit_legi_BS (dat);
		//printf ("\n14 --------------");
		for (i = 0; i < 14; i++)
		{
			sig2 = bit_legi_RC (dat);
			skt->duakapo.traktrik[i].kiom = sig2;
			//printf ("\nLongo: %u\n", sig2);
			sig = bit_legi_RC (dat);
			//printf ("\t[%u]\n", sig);
			//printf ("\tCxeno:");
			for (j = 0; j < sig2; j++)
			{
				sig = bit_legi_RC (dat);
				skt->duakapo.traktrik[i].cxeno[j] = sig;
				//printf (" %02X", sig);
			}
		}

		// Kontroli CKR-on
		ckr = bit_legi_CRC (dat);
		/*
		   puts ("");
		   for (i = 0; i != 0xFFFF; i++)
		   {
		   dat->bajto -= 2;
		   bit_krei_CRC (dat, pvzadr, i);
		   dat->bajto -= 2;
		   ckr2 = bit_legi_CRC (dat);
		   if (ckr == ckr2)
		   {
		   printf ("Legita: %X\nKreita: %X\t SEMO: %02X\n", ckr, ckr2, i);
		   break;
		   }
		   }
		   printf (" Rubajxo 1: %08X\n", bit_legi_RL (dat));
		   printf (" Rubajxo 1: %08X\n", bit_legi_RL (dat));
		 */

		if (bit_sercxi_gardostaranto (dat, dvg_gardostaranto (DVG_GS_DUAKAPO_FINO)))
			printf (" DUA KAP-DAT. (fino): %8X\n", dat->bajto);
	}

	/*-------------------------------------------------------------------------
	 * Sekcio MEASUREMENT
	 */

	printf ("========> NEKONATA 2: %8X\n", skt->kapo.sekcio[4].adresilo);
	printf ("   NEKONATA 2 (fino): %8X\n",
		skt->kapo.sekcio[4].adresilo + skt->kapo.sekcio[4].grandeco);
	dat->bajto = skt->kapo.sekcio[4].adresilo;
	dat->bito = 0;
	skt->mezuro = bit_legi_RL (dat);

	printf ("KIOM BAJTOJ :\t%lu\n", dat->kiom);

	//exit (0);
	return 0;
}

/*--------------------------------------------------------------------------------
 * Privataj funkcioj
 */

static void
dwg_decode_estajxo (Bit_Cxeno * dat, Dwg_Object_Estajxo * est)
{
	unsigned int i;
	unsigned int grando;
	int malsukceso = 2;

	est->bitgrandeco = bit_legi_RL (dat);
	malsukceso = bit_legi_H (dat, &est->traktilo);
	if (malsukceso)
	{
		printf ("\tEraro en traktilo de objekto! Adreso en la ĉeno: 0x%0x\n", dat->bajto);
		est->bitgrandeco = 0;
		est->kromdat_kiom = 0;
		est->bildo_ekzistas = 0;
		est->traktref_kiom = 0;
		return;
	}
	est->kromdat_kiom = 0;
	while (grando = bit_legi_BS (dat))
	{
		if (grando > 10210)
		{
			printf ("Absurdo! Kromdato-grandeco: %lu. Objekto: %lu (traktilo).\n",
				grando, est->traktilo.valoro);
			est->bitgrandeco = 0;
			est->kromdat_kiom = 0;
			est->bildo_ekzistas = 0;
			est->traktref_kiom = 0;
			return;
		}
		if (est->kromdat_kiom == 0)
		{
			est->kromdat = malloc (grando);
			est->kromdat_kiom = grando;
		}
		else
		{
			est->kromdat_kiom += grando;
			est->kromdat = realloc (est->kromdat, est->kromdat_kiom);
		}
		malsukceso = bit_legi_H (dat, &est->kromdat_trakt);
		if (malsukceso)
			printf ("Ops...\n");
		for (i = est->kromdat_kiom - grando; i < est->kromdat_kiom; i++)
			est->kromdat[i] = bit_legi_RC (dat);
	}
	est->bildo_ekzistas = bit_legi_B (dat);
	if (est->bildo_ekzistas)
	{
		est->bildo_kiom = bit_legi_RL (dat);
		if (est->bildo_kiom < 210210)
		{
			est->bildo = malloc (est->bildo_kiom);
			for (i = 0; i < est->bildo_kiom; i++)
				est->bildo[i] = bit_legi_RC (dat);
		}
		else
		{
			printf ("Absurdo! Bildo-grandeco: %lu kB. Objekto: %lu (traktilo).\n",
				est->bildo_kiom / 1000, est->traktilo.valoro);
			bit_ref_salti (dat, -(4 * 8 + 1));
		}
	}

	est->regximo = bit_legi_BB (dat);
	est->reagilo_kiom = bit_legi_BL (dat);
	est->senligiloj = bit_legi_B (dat);
	est->koloro = bit_legi_BS (dat);
	est->linitiposkalo = bit_legi_BD (dat);
	est->linitipo = bit_legi_BB (dat);
	est->printstilo = bit_legi_BB (dat);
	est->malvidebleco = bit_legi_BS (dat);
	est->linidikeco = bit_legi_RC (dat);
}

static void
dwg_decode_ordinarajxo (Bit_Cxeno * dat, Dwg_Object_Ordinarajxo * ord)
{
	unsigned int i;
	unsigned int grando;
	int malsukceso = 2;

	ord->bitgrandeco = bit_legi_RL (dat);
	malsukceso = bit_legi_H (dat, &ord->traktilo);
	if (malsukceso)
	{
		printf ("\tEraro en traktilo de objekto! Adreso en la ĉeno: 0x%0x\n", dat->bajto);
		ord->bitgrandeco = 0;
		ord->kromdat_kiom = 0;
		ord->traktref_kiom = 0;
		return;
	}
	ord->kromdat_kiom = 0;
	while (grando = bit_legi_BS (dat))
	{
		if (grando > 10210)
		{
			printf ("Absurdo! Kromdato-grandeco: %lu. Objekto: %lu (traktilo).\n",
				grando, ord->traktilo.valoro);
			ord->bitgrandeco = 0;
			ord->kromdat_kiom = 0;
			ord->traktref_kiom = 0;
			return;
		}
		if (ord->kromdat_kiom == 0)
		{
			ord->kromdat = malloc (grando);
			ord->kromdat_kiom = grando;
		}
		else
		{
			ord->kromdat_kiom += grando;
			ord->kromdat = realloc (ord->kromdat, ord->kromdat_kiom);
		}
		malsukceso = bit_legi_H (dat, &ord->kromdat_trakt);
		if (malsukceso)
			printf ("Ops...\n");
		for (i = ord->kromdat_kiom - grando; i < ord->kromdat_kiom; i++)
			ord->kromdat[i] = bit_legi_RC (dat);
	}

	ord->reagilo_kiom = bit_legi_BL (dat);
}

static void
dwg_decode_traktref (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	int i;

	if (obj->supertipo == DWG_OST_ESTAJXO)
	{
		Dwg_Object_Estajxo *est;

		est = obj->tio.estajxo;

		est->traktref = (Dvg_Traktilo *) calloc (sizeof (Dvg_Traktilo), 10);
		i = 0;
		while (1)
		{
			if (i % 10 == 0)
				est->traktref =
					(Dvg_Traktilo *) realloc (est->traktref,
								  (i + 10) * sizeof (Dvg_Traktilo));
			if (bit_legi_H (dat, &est->traktref[i]))
			{
				//printf ("\tEraro en tiu traktilo: %lu\n", est->traktilo.valoro);
				break;
			}
			if (!(dat->bajto == ktl_lastadreso + 1 && dat->bito == 0))
			{
				if (dat->bajto > ktl_lastadreso)
					break;
			}
			i++;
		}
		est->traktref_kiom = i;
	}
	else
	{
		Dwg_Object_Ordinarajxo *ord;

		ord = obj->tio.ordinarajxo;

		ord->traktref = (Dvg_Traktilo *) calloc (sizeof (Dvg_Traktilo), 10);
		i = 0;
		while (1)
		{
			if (i % 10 == 0)
				ord->traktref =
					(Dvg_Traktilo *) realloc (ord->traktref,
								  (i + 10) * sizeof (Dvg_Traktilo));
			if (bit_legi_H (dat, &ord->traktref[i]))
			{
				//printf ("\tEraro en tiu traktilo: %lu\n", est->traktilo.valoro);
				break;
			}
			if (!(dat->bajto == ktl_lastadreso + 1 && dat->bito == 0))
			{
				if (dat->bajto > ktl_lastadreso)
					break;
			}
			i++;
		}
		ord->traktref_kiom = i;
	}
}

/* OBJEKTOJ *******************************************************************/

static void
dwg_decode_TEXT (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_TEXT *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.TEXT = calloc (sizeof (Dvg_Estajxo_TEXT), 1);
	est = obj->tio.estajxo->tio.TEXT;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->datumindik = bit_legi_RC (dat);
	if ((!est->datumindik & 0x01))
		est->levigxo = bit_legi_RD (dat);
	est->x0 = bit_legi_RD (dat);
	est->y0 = bit_legi_RD (dat);
	if (!(est->datumindik & 0x02))
	{
		est->gxisrandigo.x = bit_legi_DD (dat, 10);
		est->gxisrandigo.y = bit_legi_DD (dat, 20);
	}
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);
	est->dikeco = bit_legi_BT (dat);
	if (!(est->datumindik & 0x04))
		est->klinang = bit_legi_RD (dat);
	if (!(est->datumindik & 0x08))
		est->turnang = bit_legi_RD (dat);
	est->alteco = bit_legi_RD (dat);
	if (!(est->datumindik & 0x10))
		est->largxfaktoro = bit_legi_RD (dat);
	est->teksto = bit_legi_T (dat);
	if (!(est->datumindik & 0x20))
		est->generacio = bit_legi_BS (dat);
	if (!(est->datumindik & 0x40))
		est->gxisrandigo.h = bit_legi_BS (dat);
	if (!(est->datumindik & 0x80))
		est->gxisrandigo.v = bit_legi_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ATTRIB (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_ATTRIB *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.ATTRIB = calloc (sizeof (Dvg_Estajxo_ATTRIB), 1);
	est = obj->tio.estajxo->tio.ATTRIB;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->datumindik = bit_legi_RC (dat);
	if ((!est->datumindik & 0x01))
		est->levigxo = bit_legi_RD (dat);
	est->x0 = bit_legi_RD (dat);
	est->y0 = bit_legi_RD (dat);
	if (!(est->datumindik & 0x02))
	{
		est->gxisrandigo.x = bit_legi_DD (dat, 10);
		est->gxisrandigo.y = bit_legi_DD (dat, 20);
	}
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);
	est->dikeco = bit_legi_BT (dat);
	if (!(est->datumindik & 0x04))
		est->klinang = bit_legi_RD (dat);
	if (!(est->datumindik & 0x08))
		est->turnang = bit_legi_RD (dat);
	est->alteco = bit_legi_RD (dat);
	if (!(est->datumindik & 0x10))
		est->largxfaktoro = bit_legi_RD (dat);
	est->teksto = bit_legi_T (dat);
	if (!(est->datumindik & 0x20))
		est->generacio = bit_legi_BS (dat);
	if (!(est->datumindik & 0x40))
		est->gxisrandigo.h = bit_legi_BS (dat);
	if (!(est->datumindik & 0x80))
		est->gxisrandigo.v = bit_legi_BS (dat);
	est->etikedo = bit_legi_T (dat);
	est->kamplong = bit_legi_BS (dat);
	est->indikiloj = bit_legi_RC (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ATTDEF (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_ATTDEF *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.ATTDEF = calloc (sizeof (Dvg_Estajxo_ATTDEF), 1);
	est = obj->tio.estajxo->tio.ATTDEF;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->datumindik = bit_legi_RC (dat);
	if ((!est->datumindik & 0x01))
		est->levigxo = bit_legi_RD (dat);
	est->x0 = bit_legi_RD (dat);
	est->y0 = bit_legi_RD (dat);
	if (!(est->datumindik & 0x02))
	{
		est->gxisrandigo.x = bit_legi_DD (dat, 10);
		est->gxisrandigo.y = bit_legi_DD (dat, 20);
	}
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);
	est->dikeco = bit_legi_BT (dat);
	if (!(est->datumindik & 0x04))
		est->klinang = bit_legi_RD (dat);
	if (!(est->datumindik & 0x08))
		est->turnang = bit_legi_RD (dat);
	est->alteco = bit_legi_RD (dat);
	if (!(est->datumindik & 0x10))
		est->largxfaktoro = bit_legi_RD (dat);
	est->teksto = bit_legi_T (dat);
	if (!(est->datumindik & 0x20))
		est->generacio = bit_legi_BS (dat);
	if (!(est->datumindik & 0x40))
		est->gxisrandigo.h = bit_legi_BS (dat);
	if (!(est->datumindik & 0x80))
		est->gxisrandigo.v = bit_legi_BS (dat);
	est->etikedo = bit_legi_T (dat);
	est->kamplong = bit_legi_BS (dat);
	est->indikiloj = bit_legi_RC (dat);
	est->invitilo = bit_legi_T (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_BLOCK (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_BLOCK *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.BLOCK = calloc (sizeof (Dvg_Estajxo_BLOCK), 1);
	est = obj->tio.estajxo->tio.BLOCK;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->nomo = bit_legi_T (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ENDBLK (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_ENDBLK *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.ENDBLK = calloc (sizeof (Dvg_Estajxo_ENDBLK), 1);
	est = obj->tio.estajxo->tio.ENDBLK;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_INSERT (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_INSERT *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.INSERT = calloc (sizeof (Dvg_Estajxo_INSERT), 1);
	est = obj->tio.estajxo->tio.INSERT;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->skalindik = bit_legi_BB (dat);
	if (est->skalindik == 3)
		est->skalo.x = est->skalo.y = est->skalo.y = 1.0;
	else if (est->skalindik == 1)
	{
		est->skalo.x = 1.0;
		est->skalo.y = bit_legi_DD (dat, 1.0);
		est->skalo.z = bit_legi_DD (dat, 1.0);
	}
	else if (est->skalindik == 2)
		est->skalo.x = est->skalo.y = est->skalo.y = bit_legi_RD (dat);
	else //if (est->skalindik == 0)
	{
		est->skalo.x = bit_legi_RD (dat);
		est->skalo.y = bit_legi_DD (dat, est->skalo.x);
		est->skalo.z = bit_legi_DD (dat, est->skalo.x);
	}
	est->turnang = bit_legi_BD (dat);
	est->forpusxigo.x = bit_legi_BD (dat);
	est->forpusxigo.y = bit_legi_BD (dat);
	est->forpusxigo.z = bit_legi_BD (dat);
	est->kun_attrib = bit_legi_B (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_MINSERT (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_MINSERT *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.MINSERT = calloc (sizeof (Dvg_Estajxo_MINSERT), 1);
	est = obj->tio.estajxo->tio.MINSERT;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->skalindik = bit_legi_BB (dat);
	if (est->skalindik == 3)
		est->skalo.x = est->skalo.y = est->skalo.y = 1.0;
	else if (est->skalindik == 1)
	{
		est->skalo.x = 1.0;
		est->skalo.y = bit_legi_DD (dat, 1.0);
		est->skalo.z = bit_legi_DD (dat, 1.0);
	}
	else if (est->skalindik == 2)
		est->skalo.x = est->skalo.y = est->skalo.y = bit_legi_RD (dat);
	else //if (est->skalindik == 0)
	{
		est->skalo.x = bit_legi_RD (dat);
		est->skalo.y = bit_legi_DD (dat, est->skalo.x);
		est->skalo.z = bit_legi_DD (dat, est->skalo.x);
	}
	est->turnang = bit_legi_BD (dat);
	est->forpusxigo.x = bit_legi_BD (dat);
	est->forpusxigo.y = bit_legi_BD (dat);
	est->forpusxigo.z = bit_legi_BD (dat);
	est->kun_attrib = bit_legi_B (dat);
	est->kol.kiom = bit_legi_BS (dat);
	est->lin.kiom = bit_legi_BS (dat);
	est->kol.dx = bit_legi_BD (dat);
	est->lin.dy = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_2D (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_VERTEX_2D *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.VERTEX_2D = calloc (sizeof (Dvg_Estajxo_VERTEX_2D), 1);
	est = obj->tio.estajxo->tio.VERTEX_2D;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->indikiloj = bit_legi_RC (dat);
	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->eklargxo = bit_legi_BD (dat);
	if (est->eklargxo < 0)
		est->finlargxo = est->eklargxo = -est->eklargxo;
	else
		est->finlargxo = bit_legi_BD (dat);
	est->protub = bit_legi_BD (dat);
	est->tangxdir = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_3D (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_VERTEX_3D *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.VERTEX_3D = calloc (sizeof (Dvg_Estajxo_VERTEX_3D), 1);
	est = obj->tio.estajxo->tio.VERTEX_3D;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->indikiloj = bit_legi_RC (dat);
	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_VERTEX_PFACE_FACE (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_VERTEX_PFACE_FACE *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.VERTEX_PFACE_FACE = calloc (sizeof (Dvg_Estajxo_VERTEX_PFACE_FACE), 1);
	est = obj->tio.estajxo->tio.VERTEX_PFACE_FACE;
	dwg_decode_estajxo (dat, obj->tio.estajxo);

	/* Legitaj valoroj
	 */
	est->vertind[0] = bit_legi_BS (dat);
	est->vertind[1] = bit_legi_BS (dat);
	est->vertind[2] = bit_legi_BS (dat);
	est->vertind[3] = bit_legi_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_2D (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_POLYLINE_2D *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.POLYLINE_2D = calloc (sizeof (Dvg_Estajxo_POLYLINE_2D), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.POLYLINE_2D;

	/* Legitaj valoroj
	 */
	est->indikiloj = bit_legi_BS (dat);
	est->kurbtipo = bit_legi_BS (dat);
	est->eklargxo = bit_legi_BD (dat);
	est->finlargxo = bit_legi_BD (dat);
	est->dikeco = bit_legi_BT (dat);
	est->levigxo = bit_legi_BD (dat);
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POLYLINE_3D (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_POLYLINE_3D *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.POLYLINE_3D = calloc (sizeof (Dvg_Estajxo_POLYLINE_3D), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.POLYLINE_3D;

	/* Legitaj valoroj
	 */
	est->indikiloj_1 = bit_legi_RC (dat);
	est->indikiloj_2 = bit_legi_RC (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ARC (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_ARC *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.ARC = calloc (sizeof (Dvg_Estajxo_ARC), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.ARC;

	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->radiuso = bit_legi_BD (dat);
	est->dikeco = bit_legi_BT (dat);
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);
	est->ekangulo = bit_legi_BD (dat);
	est->finangulo = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_CIRCLE (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_CIRCLE *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.CIRCLE = calloc (sizeof (Dvg_Estajxo_CIRCLE), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.CIRCLE;

	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->radiuso = bit_legi_BD (dat);
	est->dikeco = bit_legi_BT (dat);
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_LINE (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_LINE *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.LINE = calloc (sizeof (Dvg_Estajxo_LINE), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.LINE;

	est->nur_2D = bit_legi_B (dat);
	est->x0 = bit_legi_RD (dat);
	est->x1 = bit_legi_DD (dat, est->x0);
	est->y0 = bit_legi_RD (dat);
	est->y1 = bit_legi_DD (dat, est->y0);
	est->z0 = est->z1 = 0.0;
	if (!est->nur_2D)
	{
		est->z0 = bit_legi_RD (dat);
		est->z1 = bit_legi_DD (dat, est->z0);
	}
	est->dikeco = bit_legi_BT (dat);
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_POINT (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_POINT *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.POINT = calloc (sizeof (Dvg_Estajxo_POINT), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.POINT;

	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->dikeco = bit_legi_BT (dat);
	bit_legi_BE (dat, &est->forpusxigo.x, &est->forpusxigo.y, &est->forpusxigo.z);
	est->x_ang = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_ELLIPSE (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_ELLIPSE *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.ELLIPSE = calloc (sizeof (Dvg_Estajxo_ELLIPSE), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.ELLIPSE;

	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->x1 = bit_legi_BD (dat);
	est->y1 = bit_legi_BD (dat);
	est->z1 = bit_legi_BD (dat);
	est->forpusxigo.x = bit_legi_BD (dat);
	est->forpusxigo.y = bit_legi_BD (dat);
	est->forpusxigo.z = bit_legi_BD (dat);
	est->radiusproporcio = bit_legi_BD (dat);
	est->ekangulo = bit_legi_BD (dat);
	est->finangulo = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_RAY (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_RAY *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.RAY = calloc (sizeof (Dvg_Estajxo_RAY), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.RAY;

	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->z0 = bit_legi_BD (dat);
	est->x1 = bit_legi_BD (dat);
	est->y1 = bit_legi_BD (dat);
	est->z1 = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_MTEXT (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_MTEXT *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.MTEXT = calloc (sizeof (Dvg_Estajxo_MTEXT), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.MTEXT;

	est->x0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->y0 = bit_legi_BD (dat);
	est->forpusxigo.x = bit_legi_BD (dat);
	est->forpusxigo.y = bit_legi_BD (dat);
	est->forpusxigo.z = bit_legi_BD (dat);
	est->x1 = bit_legi_BD (dat);
	est->y1 = bit_legi_BD (dat);
	est->z1 = bit_legi_BD (dat);
	est->largxeco = bit_legi_BD (dat);
	est->alteco = bit_legi_BD (dat);
	est->kunmeto = bit_legi_BS (dat);
	est->direkto = bit_legi_BS (dat);
	est->etendo = bit_legi_BD (dat);
	est->etendlargxo = bit_legi_BD (dat);
	est->teksto = bit_legi_T (dat);
	est->linispaco_stilo = bit_legi_BS (dat);
	est->linispaco_faktoro = bit_legi_BD (dat);
	est->ia_bito = bit_legi_B (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_LAYER (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Ordinarajxo_LAYER *ord;

	obj->supertipo = DWG_OST_ORDINARAJXO;
	obj->tio.ordinarajxo = malloc (sizeof (Dwg_Object_Ordinarajxo));
	obj->tio.ordinarajxo->tio.LAYER = calloc (sizeof (Dvg_Ordinarajxo_LAYER), 1);
	dwg_decode_ordinarajxo (dat, obj->tio.ordinarajxo);
	ord = obj->tio.ordinarajxo->tio.LAYER;

	/* Legitaj valoroj
	 */
	ord->nomo = bit_legi_T (dat);
	ord->bito64 = bit_legi_B (dat);
	ord->xrefi = bit_legi_BS (dat);
	ord->xrefdep = bit_legi_B (dat);
	ord->ecoj = bit_legi_BS (dat);
	ord->koloro = bit_legi_BS (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_LAYOUT (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Ordinarajxo_LAYOUT *ord;

	obj->supertipo = DWG_OST_ORDINARAJXO;
	obj->tio.ordinarajxo = malloc (sizeof (Dwg_Object_Ordinarajxo));
	obj->tio.ordinarajxo->tio.LAYOUT = calloc (sizeof (Dvg_Ordinarajxo_LAYOUT), 1);
	dwg_decode_ordinarajxo (dat, obj->tio.ordinarajxo);
	ord = obj->tio.ordinarajxo->tio.LAYOUT;

	/* Legitaj valoroj
	 */
	ord->pagxo.agordo = bit_legi_T (dat);
	ord->pagxo.printilo = bit_legi_T (dat);
	ord->pagxo.indikiloj = bit_legi_BS (dat);
	ord->pagxo.maldekstre = bit_legi_BD (dat);
	ord->pagxo.malsupre = bit_legi_BD (dat);
	ord->pagxo.dekstre = bit_legi_BD (dat);
	ord->pagxo.supre = bit_legi_BD (dat);
	ord->pagxo.largxeco = bit_legi_BD (dat);
	ord->pagxo.alteco = bit_legi_BD (dat);
	ord->pagxo.grandeco = bit_legi_T (dat);
	ord->pagxo.dx = bit_legi_BD (dat);
	ord->pagxo.dy = bit_legi_BD (dat);
	ord->pagxo.unuoj = bit_legi_BS (dat);
	ord->pagxo.rotacio = bit_legi_BS (dat);
	ord->pagxo.tipo = bit_legi_BS (dat);
	ord->pagxo.x_min = bit_legi_BD (dat);
	ord->pagxo.y_min = bit_legi_BD (dat);
	ord->pagxo.x_maks = bit_legi_BD (dat);
	ord->pagxo.y_maks = bit_legi_BD (dat);
	ord->pagxo.nomo = bit_legi_T (dat);
	ord->pagxo.skalo.A = bit_legi_BD (dat);
	ord->pagxo.skalo.B = bit_legi_BD (dat);
	ord->pagxo.stilfolio = bit_legi_T (dat);
	ord->pagxo.skalo.tipo = bit_legi_BS (dat);
	ord->pagxo.skalo.faktoro = bit_legi_BD (dat);
	ord->pagxo.x0 = bit_legi_BD (dat);
	ord->pagxo.y0 = bit_legi_BD (dat);

	ord->nomo = bit_legi_T (dat);
	ord->ordo = bit_legi_BS (dat);
	ord->indikiloj = bit_legi_BS (dat);
	ord->x0 = bit_legi_BD (dat);
	ord->y0 = bit_legi_BD (dat);
	ord->z0 = bit_legi_BD (dat);
	ord->x_min = bit_legi_RD (dat);
	ord->y_min = bit_legi_RD (dat);
	ord->x_maks = bit_legi_RD (dat);
	ord->y_maks = bit_legi_RD (dat);
	ord->enmeto.x0 = bit_legi_BD (dat);
	ord->enmeto.y0 = bit_legi_BD (dat);
	ord->enmeto.z0 = bit_legi_BD (dat);
	ord->akso_X.x0 = bit_legi_BD (dat);
	ord->akso_X.y0 = bit_legi_BD (dat);
	ord->akso_X.z0 = bit_legi_BD (dat);
	ord->akso_Y.x0 = bit_legi_BD (dat);
	ord->akso_Y.y0 = bit_legi_BD (dat);
	ord->akso_Y.z0 = bit_legi_BD (dat);
	ord->levigxo = bit_legi_BD (dat);
	ord->rigardtipo = bit_legi_BS (dat);
	ord->limo.x_min = bit_legi_BD (dat);
	ord->limo.y_min = bit_legi_BD (dat);
	ord->limo.z_min = bit_legi_BD (dat);
	ord->limo.x_maks = bit_legi_BD (dat);
	ord->limo.y_maks = bit_legi_BD (dat);
	ord->limo.z_maks = bit_legi_BD (dat);

	dwg_decode_traktref (dat, obj);
}

static void
dwg_decode_NEUZATA (Bit_Cxeno * dat, Dvg_Objekto * obj)
{
	Dvg_Estajxo_NEUZATA *est;

	obj->supertipo = DWG_OST_ESTAJXO;
	obj->tio.estajxo = malloc (sizeof (Dwg_Object_Estajxo));
	obj->tio.estajxo->tio.NEUZATA = calloc (sizeof (Dvg_Estajxo_NEUZATA), 1);
	dwg_decode_estajxo (dat, obj->tio.estajxo);
	est = obj->tio.estajxo->tio.NEUZATA;

	/* Legitaj valoroj
	 */


	dwg_decode_traktref (dat, obj);
}

/*--------------------------------------------------------------------------------
 * Privata funkcio, kiu dependas de la antaŭaj
 */
static void
dwg_decode_aldoni_objekto (Dwg_Structure * skt, Bit_Cxeno * dat, long unsigned int adreso)
{
	long unsigned int antauxa_adreso;
	long unsigned int objekadres;
	unsigned char antauxa_bito;
	Dvg_Objekto *obj;

	/* Gardi la antauxan adreson
	 */
	antauxa_adreso = dat->bajto;
	antauxa_bito = dat->bito;

	/* Uzi la indikitan adreson por la objekto
	 */
	dat->bajto = adreso;
	dat->bito = 0;

	/* Rezervi memor-spacon por plia objekto
	 */
	if (skt->objekto_kiom == 0)
		skt->objekto = (Dvg_Objekto *) malloc (sizeof (Dvg_Objekto));
	else
		skt->objekto =
			(Dvg_Objekto *) realloc (skt->objekto,
						 (skt->objekto_kiom + 1) * sizeof (Dvg_Objekto));

	//printf ("Objekto numero: %u\n", skt->objekto_kiom);

	obj = &skt->objekto[skt->objekto_kiom];
	skt->objekto_kiom++;

	obj->grandeco = bit_legi_MS (dat);
	objekadres = dat->bajto;
	ktl_lastadreso = dat->bajto + obj->grandeco;	/* (de cxi tie oni kalkulas la bitgrandecon) */
	obj->tipo = bit_legi_BS (dat);

	/* Kontroli la tipon de objekto
	 */
	switch (obj->tipo)
	{
	case DWG_OT_TEXT:
		dwg_decode_TEXT (dat, obj);
		break;
	case DWG_OT_ATTRIB:
		dwg_decode_ATTRIB (dat, obj);
		break;
	case DWG_OT_ATTDEF:
		dwg_decode_ATTDEF (dat, obj);
		break;
	case DWG_OT_BLOCK:
		dwg_decode_BLOCK (dat, obj);
		break;
	case DWG_OT_ENDBLK:
	case DWG_OT_SEQEND:
		dwg_decode_ENDBLK (dat, obj);
		break;
	case DWG_OT_INSERT:
		dwg_decode_INSERT (dat, obj);
		break;
	case DWG_OT_MINSERT:
		dwg_decode_MINSERT (dat, obj);
		break;
	case DWG_OT_VERTEX_2D:
		dwg_decode_VERTEX_2D (dat, obj);
		break;
	case DWG_OT_VERTEX_3D:
	case DWG_OT_VERTEX_MESH:
	case DWG_OT_VERTEX_PFACE:
		dwg_decode_VERTEX_3D (dat, obj);
		break;
	case DWG_OT_VERTEX_PFACE_FACE:
		dwg_decode_VERTEX_PFACE_FACE (dat, obj);
		break;
	case DWG_OT_POLYLINE_2D:
		dwg_decode_POLYLINE_2D (dat, obj);
		break;
	case DWG_OT_POLYLINE_3D:
		dwg_decode_POLYLINE_3D (dat, obj);
		break;
	case DWG_OT_ARC:
		dwg_decode_ARC (dat, obj);
		break;
	case DWG_OT_CIRCLE:
		dwg_decode_CIRCLE (dat, obj);
		break;
	case DWG_OT_LINE:
		dwg_decode_LINE (dat, obj);
		break;
	case DWG_OT_POINT:
		dwg_decode_POINT (dat, obj);
		break;
	case DWG_OT_ELLIPSE:
		dwg_decode_ELLIPSE (dat, obj);
		break;
	case DWG_OT_RAY:
	case DWG_OT_XLINE:
		dwg_decode_RAY (dat, obj);
		break;
	case DWG_OT_MTEXT:
		dwg_decode_MTEXT (dat, obj);
		break;
	case DWG_OT_LAYER:
		dwg_decode_LAYER (dat, obj);
		break;
	default:
		if (obj->tipo == skt->dvg_ot_layout)
			dwg_decode_LAYOUT (dat, obj);
		else
		{
			obj->supertipo = DWG_OST_NEKONATAJXO;
			obj->tio.nekonatajxo = malloc (obj->grandeco);
			memcpy (obj->tio.nekonatajxo, &dat->cxeno[objekadres], obj->grandeco);
		}
	}

	/*
	   if (obj->supertipo != DWG_OST_NEKONATAJXO)
	   {
	   printf (" Ekadr:\t%10lu\n", adreso);
	   printf (" Lasta:\t%10lu\tGrandeco: %10lu\n", dat->bajto, obj->grandeco);
	   printf ("Finadr:\t%10lu (kalkulite)\n", adreso + 2 + obj->grandeco);
	   }
	 */

	/* Restarigi la antauxan adreson por returni
	 */
	dat->bajto = antauxa_adreso;
	dat->bito = antauxa_bito;
}


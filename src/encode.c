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

/// Enkodigo - tio cxi ne funkciis...

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "encode.h"

typedef struct
{
	long int traktilo;
	long int adreso;
	unsigned int idc;
} Objekto_Mapo;

/*--------------------------------------------------------------------------------
 * Prototipoj de privataj funkcioj
 */
static void dwg_encode_estajxo (Dwg_Objekto * obj, Bit_Chain * dat);
static void dwg_encode_ordinarajxo (Dwg_Objekto * obj, Bit_Chain * dat);
static void dwg_encode_LINE (Dwg_Estajxo_LINE * est, Bit_Chain * dat);
static void dwg_encode_CIRCLE (Dwg_Estajxo_CIRCLE * est, Bit_Chain * dat);

/*--------------------------------------------------------------------------------
 * Publikaj funkcioj
 */
int
dwg_encode_cxenigi (Dwg_Structure * skt, Bit_Chain * dat)
{
	int ckr_mankanta;
	long unsigned int i, j;
	long unsigned int sekciadresaro;
	unsigned char pvzbit;
	long unsigned int pvzadr;
	long unsigned int pvzadr_2;
	unsigned int ckr;
	unsigned int sekcigrandeco = 0;
	long unsigned int lastadres;
	long unsigned int lastatrakt;
	Objekto_Mapo *omap;
	Objekto_Mapo pvzmap;
	Dwg_Objekto *obj;

	bit_chain_rezervi (dat);

	/*------------------------------------------------------------
	 * Kap-datenaro
	 */
	//strcpy (dat->chain, skt->kapo.versio); // Chain pri versio: devas esti AC1015
	strcpy (dat->chain, "AC1015");	// Chain pri versio: devas esti AC1015
	dat->bajto += 6;

	for (i = 0; i < 5; i++)
		bit_write_RC (dat, 0);	// Nekonata sekcio
	bit_write_RC (dat, 0x0F);	// Nekonatajxo
	bit_write_RC (dat, 0x01);	// Nekonatajxo
	bit_write_RL (dat, 0);	// Bildo-adreso
	bit_write_RC (dat, 25);	// Versio
	bit_write_RC (dat, 0);	// Lancxo
	bit_write_RS (dat, skt->kapo.kodpagxo);	// Kodpagxo

	//skt->kapo.sekcio_kiom = 5; // Cxu kasxi la nekonatan sekcion 1 ?
	bit_write_RL (dat, skt->kapo.sekcio_kiom);
	sekciadresaro = dat->bajto;	// Salti sekciadresaron
	dat->bajto += (skt->kapo.sekcio_kiom * 9);
	bit_read_CRC (dat);	// Salti CKR-on

	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_KAPO_FINO));

	/*------------------------------------------------------------
	 * Nekonata sekcio 1
	 */

	skt->kapo.sekcio[5].numero = 5;
	skt->kapo.sekcio[5].adresilo = 0;
	skt->kapo.sekcio[5].grandeco = 0;
	if (skt->kapo.sekcio_kiom == 6)
	{
		skt->kapo.sekcio[5].adresilo = dat->bajto;
		skt->kapo.sekcio[5].grandeco = DWG_NEKONATA1_KIOM;

		skt->nekonata1.kiom = skt->kapo.sekcio[5].grandeco;
		skt->nekonata1.bajto = skt->nekonata1.bito = 0;
		while (dat->bajto + skt->nekonata1.kiom >= dat->kiom)
			bit_chain_rezervi (dat);
		memcpy (&dat->chain[dat->bajto], skt->nekonata1.chain, skt->nekonata1.kiom);
		dat->bajto += skt->nekonata1.kiom;

	}

	/*------------------------------------------------------------
	 * Antauxrigarda bildo
	 */

	/* Finfine write la adreson de la bildo
	 */
	pvzadr = dat->bajto;
	dat->bajto = 0x0D;
	bit_write_RL (dat, pvzadr);
	dat->bajto = pvzadr;

	/* Kopii la bildon
	 */
	//skt->bildo.kiom = 0; // Se oni deziras ne kopii bildon, malkomentu tiun cxi linion
	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_BILDO_EKO));
	for (i = 0; i < skt->bildo.kiom; i++)
		bit_write_RC (dat, skt->bildo.chain[i]);
	if (skt->bildo.kiom == 0)
	{
		bit_write_RL (dat, 5);
		bit_write_RC (dat, 0);
	}
	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_BILDO_FINO));


	/*------------------------------------------------------------
	 * Kap-variabloj
	 */

	skt->kapo.sekcio[0].numero = 0;
	skt->kapo.sekcio[0].adresilo = dat->bajto;
	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_VARIABLO_EKO));
	pvzadr = dat->bajto;	// poste oni devas rewrite la korektan valuen de grandeco cxi tie:
	bit_write_RL (dat, 0);	// Grandeco de la sekcio

	for (i = 0; i < DWG_KIOM_VARIABLOJ; i++)
	{
		if (i == 221 && skt->var[220].dubitoko != 3)
			continue;
		switch (dwg_varmapo (i))
		{
		case DWG_DT_B:
			bit_write_B (dat, skt->var[i].bitoko);
			break;
		case DWG_DT_BS:
			bit_write_BS (dat, skt->var[i].dubitoko);
			break;
		case DWG_DT_BL:
			bit_write_BL (dat, skt->var[i].kvarbitoko);
			break;
		case DWG_DT_BD:
			bit_write_BD (dat, skt->var[i].duglitajxo);
			break;
		case DWG_DT_H:
			bit_write_H (dat, &skt->var[i].traktilo);
			break;
		case DWG_DT_T:
			bit_write_T (dat, skt->var[i].teksto);
			break;
		case DWG_DT_CMC:
			bit_write_BS (dat, skt->var[i].dubitoko);
			break;
		case DWG_DT_2RD:
			bit_write_RD (dat, skt->var[i].xy[0]);
			bit_write_RD (dat, skt->var[i].xy[1]);
			break;
		case DWG_DT_3BD:
			bit_write_BD (dat, skt->var[i].xyz[0]);
			bit_write_BD (dat, skt->var[i].xyz[1]);
			bit_write_BD (dat, skt->var[i].xyz[2]);
			break;
		default:
			printf ("Ne traktebla tipo: %i (var: %i)\n", dwg_varmapo (i), (int) i);
		}
	}

	/* Skribi la grandecon de la sekcio cxe gxia komenco
	 */
	pvzadr_2 = dat->bajto;
	pvzbit = dat->bito;
	dat->bajto = pvzadr;
	dat->bito = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->bajto = pvzadr_2;
	dat->bito = pvzbit;
	//printf ("Grandeco: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

	/* CKR kaj gardostaranto
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);
	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_VARIABLO_FINO));
	skt->kapo.sekcio[0].grandeco = dat->bajto - skt->kapo.sekcio[0].adresilo;

	/*------------------------------------------------------------
	 * Klasoj
	 */
	skt->kapo.sekcio[1].numero = 1;
	skt->kapo.sekcio[1].adresilo = dat->bajto;
	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_KLASO_EKO));
	pvzadr = dat->bajto;	// poste oni devas rewrite la korektan valuen de grandeco cxi tie:
	bit_write_RL (dat, 0);	// Grandeco de la sekcio

	for (i = 0; i < skt->klaso_kiom; i++)
	{
		bit_write_BS (dat, skt->klaso[i].numero);
		bit_write_BS (dat, skt->klaso[i].versio);
		bit_write_T (dat, skt->klaso[i].apnomo);
		bit_write_T (dat, skt->klaso[i].cpliplinomo);
		bit_write_T (dat, skt->klaso[i].dxfnomo);
		bit_write_B (dat, skt->klaso[i].estisfantomo);
		bit_write_BS (dat, skt->klaso[i].eroid);
	}

	/* Skribi la grandecon de la sekcio cxe gxia komenco
	 */
	pvzadr_2 = dat->bajto;
	pvzbit = dat->bito;
	dat->bajto = pvzadr;
	dat->bito = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->bajto = pvzadr_2;
	dat->bito = pvzbit;
	//printf ("Grandeco: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

	/* CKR kaj gardostaranto
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_KLASO_FINO));
	skt->kapo.sekcio[1].grandeco = dat->bajto - skt->kapo.sekcio[1].adresilo;


	/*------------------------------------------------------------
	 * Objektoj
	 */
	bit_write_RL (dat, 0x00000000);	// 0xDCA Nekonata kvarbitoko inter klasoj kaj objektaro
	pvzadr = dat->bajto;

	/* Ekdifini objekto-mapon
	 */
	omap = (Objekto_Mapo *) malloc (skt->objekto_kiom * sizeof (Objekto_Mapo));
	for (i = 0; i < skt->objekto_kiom; i++)
	{
		Bit_Chain nkn;
		Dwg_Traktilo tkt;

		/* Difini la traktilojn de cxiuj objektoj, inkluzive la nekonataj */
		omap[i].idc = i;
		if (skt->objekto[i].supertipo == DWG_SUPERTYPE_ESTAJXO)
			omap[i].traktilo = skt->objekto[i].tio.estajxo->traktilo.value;
		else if (skt->objekto[i].supertipo == DWG_SUPERTYPE_ORDINARAJXO)
			omap[i].traktilo = skt->objekto[i].tio.ordinarajxo->traktilo.value;
		else if (skt->objekto[i].supertipo == DWG_SUPERTYPE_NEKONATAJXO)
		{
			nkn.chain = skt->objekto[i].tio.nekonatajxo;
			nkn.kiom = skt->objekto[i].grandeco;
			nkn.bajto = nkn.bito = 0;
			bit_read_BS (&nkn);
			bit_read_RL (&nkn);
			bit_read_H (&nkn, &tkt);
			omap[i].traktilo = tkt.value;
		}
		else
			omap[i].traktilo = 0x7FFFFFFF;	/* Eraro! */

		/* Ordigi la sekvon de traktiloj laux kreskanta ordo */
		if (i > 0)
		{
			j = i;
			while (omap[j].traktilo < omap[j - 1].traktilo)
			{
				omap[j - 1].traktilo = pvzmap.traktilo;
				omap[j - 1].idc = pvzmap.idc;

				omap[j - 1].traktilo = omap[j].traktilo;
				omap[j - 1].idc = omap[j].idc;

				omap[j].traktilo = pvzmap.traktilo;
				omap[j].idc = pvzmap.idc;
				j--;
				if (j == 0)
					break;
			}
		}
	}
	//for (i = 0; i < skt->objekto_kiom; i++) printf ("Trakt(%i): %lu / Idc: %u\n", i, omap[i].traktilo, omap[i].idc);

	/* Skribi la objektaron
	 */
	for (i = 0; i < skt->objekto_kiom; i++)
	{
		omap[i].adreso = dat->bajto;
		obj = &skt->objekto[omap[i].idc];
		if (obj->supertipo == DWG_SUPERTYPE_NEKONATAJXO)
		{
			bit_write_MS (dat, obj->grandeco);
			if (dat->bajto + obj->grandeco >= dat->kiom - 2)
				bit_chain_rezervi (dat);
			memcpy (&dat->chain[dat->bajto], obj->tio.nekonatajxo, obj->grandeco);
			dat->bajto += obj->grandeco;
		}
		else
		{
			if (obj->supertipo == DWG_SUPERTYPE_ESTAJXO)
				dwg_encode_estajxo (obj, dat);
			else if (obj->supertipo == DWG_SUPERTYPE_ORDINARAJXO)
				dwg_encode_ordinarajxo (obj, dat);
			else
			{
				printf ("Eraro: ne difinita (super)tipo de objekto por write\n");
				exit (-1);
			}
		}
		bit_krei_CRC (dat, omap[i].adreso, 0xC0C1);
	}
	//for (i = 0; i < skt->objekto_kiom; i++) printf ("Trakt(%i): %6lu / Adreso: %08X / Idc: %u\n", i, omap[i].traktilo, omap[i].adreso, omap[i].idc);

	/* Nekonata dubitoko inter la objektaron kaj la objekto-mapo
	 */
	bit_write_RS (dat, 0);

	/*------------------------------------------------------------
	 * Objekto-mapo
	 */
	skt->kapo.sekcio[2].numero = 2;
	skt->kapo.sekcio[2].adresilo = dat->bajto;	// poste oni devas kalkuli la valuen de grandeco
	//printf ("Ekadreso: 0x%08X\n", dat->bajto);

	sekcigrandeco = 0;
	pvzadr = dat->bajto;	// poste oni devas write cxi tie la korektan valuen de grandeco de la unua sekcio
	dat->bajto += 2;
	lastadres = 0;
	lastatrakt = 0;
	for (i = 0; i < skt->objekto_kiom; i++)
	{
		unsigned int idc;
		long int pvz;

		idc = omap[i].idc;

		pvz = omap[idc].traktilo - lastatrakt;
		bit_write_MC (dat, pvz);
		//printf ("Trakt(%i): %6lu / ", i, pvz);
		lastatrakt = omap[idc].traktilo;

		pvz = omap[idc].adreso - lastadres;
		bit_write_MC (dat, pvz);
		//printf ("Adreso: %08X\n", pvz);
		lastadres = omap[idc].adreso;

		ckr_mankanta = 1;
		if (dat->bajto - pvzadr > 2030)	// 2029
		{
			ckr_mankanta = 0;
			sekcigrandeco = dat->bajto - pvzadr;
			dat->chain[pvzadr] = sekcigrandeco >> 8;
			dat->chain[pvzadr + 1] = sekcigrandeco & 0xFF;
			bit_krei_CRC (dat, pvzadr, 0xC0C1);

			pvzadr = dat->bajto;
			dat->bajto += 2;
			lastadres = 0;
			lastatrakt = 0;
		}
	}
	//printf ("Obj kiom: %u\n", i);
	if (ckr_mankanta)
	{
		sekcigrandeco = dat->bajto - pvzadr;
		dat->chain[pvzadr] = sekcigrandeco >> 8;
		dat->chain[pvzadr + 1] = sekcigrandeco & 0xFF;
		bit_krei_CRC (dat, pvzadr, 0xC0C1);
	}
	pvzadr = dat->bajto;
	bit_write_RC (dat, 0);
	bit_write_RC (dat, 2);
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	/* Kalkuli kaj write la grandecon de la objekto-mapo
	 */
	skt->kapo.sekcio[2].grandeco = dat->bajto - skt->kapo.sekcio[2].adresilo;
	free (omap);

	/*------------------------------------------------------------
	 * Dua kap-datenaro
	 */
	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_DUAKAPO_EKO));

	pvzadr = dat->bajto;	// Gardi la unuan adreson de la sekcio por write ties grandecon poste
	bit_write_RL (dat, 0);

	bit_write_BL (dat, pvzadr - 16);	// ekadreso de la sekcio

	/* Chain "AC1015"
	 */
	bit_write_RC (dat, 'A');
	bit_write_RC (dat, 'C');
	bit_write_RC (dat, '1');
	bit_write_RC (dat, '0');
	bit_write_RC (dat, '1');
	bit_write_RC (dat, '5');

	/* 5 (aux 6) nuloj
	 */
	for (i = 0; i < 5; i++)	// 6 se estas pli malnova...
		bit_write_RC (dat, 0);

	/* 4 nulaj bitoj
	 */
	bit_write_BB (dat, 0);
	bit_write_BB (dat, 0);

	/* Fiksa chain
	 */
	bit_write_RC (dat, 0x0F);
	bit_write_RC (dat, 0x14);
	bit_write_RC (dat, 0x64);
	bit_write_RC (dat, 0x78);
	bit_write_RC (dat, 0x01);
	bit_write_RC (dat, 0x06);

	/* Adresaro
	 */
	for (i = 0; i < 6; i++)
	{
		bit_write_RC (dat, 0);
		bit_write_BL (dat, skt->kapo.sekcio[0].adresilo);
		bit_write_BL (dat, skt->kapo.sekcio[0].grandeco);
	}

	/* Traktilaro
	 */
	bit_write_BS (dat, 14);
	for (i = 0; i < 14; i++)
	{
		bit_write_RC (dat, skt->duakapo.traktrik[i].kiom);
		bit_write_RC (dat, i);
		for (j = 0; j < skt->duakapo.traktrik[i].kiom; j++)
			bit_write_RC (dat, skt->duakapo.traktrik[i].chain[j]);
	}

	/* Returni al la komenco por write la grandecon
	 */
	pvzadr_2 = dat->bajto;
	dat->bajto = pvzadr;
	bit_write_RL (dat, pvzadr_2 - pvzadr + 10);
	dat->bajto = pvzadr_2;

	/* CKR
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	/* Jen 8 bitokoj da rubajxo
	 */
	bit_write_RL (dat, 0);
	bit_write_RL (dat, 0);

	bit_write_gardostaranto (dat, dwg_gardostaranto (DWG_GS_DUAKAPO_FINO));

	/*------------------------------------------------------------
	 * MEASUREMENT
	 */
	skt->kapo.sekcio[3].numero = 3;
	skt->kapo.sekcio[3].adresilo = 0;
	skt->kapo.sekcio[3].grandeco = 0;
	skt->kapo.sekcio[4].numero = 4;
	skt->kapo.sekcio[4].adresilo = dat->bajto;
	skt->kapo.sekcio[4].grandeco = 4;
	bit_write_RL (dat, skt->mezuro);

	/* Fino de la dosiero
	 */
	dat->kiom = dat->bajto;

	/* Skribi sekciadresaron
	 */
	dat->bajto = sekciadresaro;
	dat->bito = 0;
	for (i = 0; i < skt->kapo.sekcio_kiom; i++)
	{
		bit_write_RC (dat, skt->kapo.sekcio[i].numero);
		bit_write_RL (dat, skt->kapo.sekcio[i].adresilo);
		bit_write_RL (dat, skt->kapo.sekcio[i].grandeco);
	}

	/* Skribi CKR-on
	 */
	bit_krei_CRC (dat, 0, 0);
	dat->bajto -= 2;
	ckr = bit_read_CRC (dat);
	dat->bajto -= 2;
	switch (skt->kapo.sekcio_kiom)
	{
	case 3:
		bit_write_RS (dat, ckr ^ 0xA598);
		break;
	case 4:
		bit_write_RS (dat, ckr ^ 0x8101);
		break;
	case 5:
		bit_write_RS (dat, ckr ^ 0x3CC4);
		break;
	case 6:
		bit_write_RS (dat, ckr ^ 0x8461);
		break;
	default:
		bit_write_RS (dat, ckr);
	}

	return 0;
}

static void
dwg_encode_estajxo (Dwg_Objekto * obj, Bit_Chain * dat)
{
	unsigned int i;
	long unsigned int longo;
	Bit_Chain gdadr;
	Bit_Chain ekadr;
	Bit_Chain bgadr;
	Bit_Chain pvadr;
	Dwg_Object_Estajxo *est;

	est = obj->tio.estajxo;

	gdadr.bajto = dat->bajto;
	gdadr.bito = dat->bito;

	bit_write_MS (dat, obj->grandeco);

	ekadr.bajto = dat->bajto;	// Por kalkuli poste la bajta kaj bita grandecoj de la objekto
	ekadr.bito = dat->bito;

	bit_write_BS (dat, obj->tipo);

	bgadr.bajto = dat->bajto;
	bgadr.bito = dat->bito;

	bit_write_RL (dat, 0);	// Nulo nun, kalkulendas por write poste

	bit_write_H (dat, &est->traktilo);
	bit_write_BS (dat, est->kromdat_kiom);
	if (est->kromdat_kiom > 0)
	{
		bit_write_H (dat, &est->kromdat_trakt);
		for (i = 0; i < est->kromdat_kiom; i++)
			bit_write_RC (dat, est->kromdat[i]);
	}

	bit_write_B (dat, est->bildo_ekzistas);
	if (est->bildo_ekzistas)
	{
		bit_write_RL (dat, est->bildo_kiom);
		for (i = 0; i < est->bildo_kiom; i++)
			bit_write_RC (dat, est->bildo[i]);
	}

	bit_write_BB (dat, est->regximo);
	bit_write_BL (dat, est->reagilo_kiom);
	bit_write_B (dat, est->senligiloj);
	bit_write_BS (dat, est->koloro);
	bit_write_BD (dat, est->linitiposkalo);
	bit_write_BB (dat, est->linitipo);
	bit_write_BB (dat, est->printstilo);
	bit_write_BS (dat, est->malvidebleco);
	bit_write_RC (dat, est->linidikeco);

	switch (obj->tipo)
	{
	case DWG_TYPE_LINE:
		dwg_encode_LINE (est->tio.LINE, dat);
		break;
	case DWG_TYPE_CIRCLE:
		dwg_encode_CIRCLE (est->tio.CIRCLE, dat);
		break;

	default:
		printf ("Eraro: nekonata objekto-tipo dum enkodigo de estaĵo\n");
		exit (-1);
	}

	/* Finfine kalkuli kaj write la bit-grandecon de la objekto
	 */
	pvadr.bajto = dat->bajto;
	pvadr.bito = dat->bito;

	dat->bajto = bgadr.bajto;
	dat->bito = bgadr.bito;

	longo = 8 * (pvadr.bajto - ekadr.bajto) + (pvadr.bito);
	bit_write_RL (dat, longo);
	//printf ("Longo (bit): %lu\t", longo);

	dat->bajto = pvadr.bajto;
	dat->bito = pvadr.bito;

	/* Traktilaj referencoj
	 */
	for (i = 0; i < est->traktref_kiom; i++)
		bit_write_H (dat, &est->traktref[i]);

	/* Finfine kalkuli kaj write la bajt-grandecon de la objekto (cxu estas erara?)
	 */
	pvadr.bajto = dat->bajto;
	pvadr.bito = dat->bito;

	dat->bajto = gdadr.bajto;
	dat->bito = gdadr.bito;

	longo = pvadr.bajto - ekadr.bajto;
	bit_write_MS (dat, longo);
	//printf ("Longo: %lu\n", longo);

	dat->bajto = pvadr.bajto;
	dat->bito = pvadr.bito;
}

static void
dwg_encode_ordinarajxo (Dwg_Objekto * obj, Bit_Chain * dat)
{
	Bit_Chain ekadr;

	bit_write_MS (dat, obj->grandeco);
	ekadr.bajto = dat->bajto;	// Por kalkuli poste la bita grandeco de la objekto
	ekadr.bito = dat->bito;
	bit_write_BS (dat, obj->tipo);
}

static void
dwg_encode_LINE (Dwg_Estajxo_LINE * est, Bit_Chain * dat)
{
	bit_write_B (dat, est->nur_2D);
	bit_write_RD (dat, est->x0);
	bit_write_DD (dat, est->x1, est->x0);
	bit_write_RD (dat, est->y0);
	bit_write_DD (dat, est->y1, est->y0);
	if (!est->nur_2D)
	{
		bit_write_RD (dat, est->z0);
		bit_write_DD (dat, est->z1, est->z0);
	}
	bit_write_BT (dat, est->dikeco);
	bit_write_BE (dat, est->forpusxigo.x, est->forpusxigo.y, est->forpusxigo.z);
}

static void
dwg_encode_CIRCLE (Dwg_Estajxo_CIRCLE * est, Bit_Chain * dat)
{
	bit_write_BD (dat, est->x0);
	bit_write_BD (dat, est->y0);
	bit_write_BD (dat, est->z0);
	bit_write_BD (dat, est->radiuso);
	bit_write_BT (dat, est->dikeco);
	bit_write_BE (dat, est->forpusxigo.x, est->forpusxigo.y, est->forpusxigo.z);
}

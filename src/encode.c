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
	long int address;
	unsigned int idc;
} Object_Mapo;

/*--------------------------------------------------------------------------------
 * Private functions prototypes
 */
static void dwg_encode_estajxo (Dwg_Object * obj, Bit_Chain * dat);
static void dwg_encode_ordinarajxo (Dwg_Object * obj, Bit_Chain * dat);
static void dwg_encode_LINE (Dwg_Estajxo_LINE * est, Bit_Chain * dat);
static void dwg_encode_CIRCLE (Dwg_Estajxo_CIRCLE * est, Bit_Chain * dat);

/*--------------------------------------------------------------------------------
 * Public functions
 */
int
dwg_encode_chains (Dwg_Structure * skt, Bit_Chain * dat)
{
	int ckr_mankanta;
	long unsigned int i, j;
	long unsigned int sekciadresaro;
	unsigned char pvzbit;
	long unsigned int pvzadr;
	long unsigned int pvzadr_2;
	unsigned int ckr;
	unsigned int sekcisize = 0;
	long unsigned int lastadres;
	long unsigned int lastatrakt;
	Object_Mapo *omap;
	Object_Mapo pvzmap;
	Dwg_Object *obj;

	bit_chain_rezervi (dat);

	/*------------------------------------------------------------
	 * Kap-datenaro
	 */
	//strcpy (dat->chain, skt->header.version); // Chain pri version: devas esti AC1015
	strcpy (dat->chain, "AC1015");	// Chain pri version: devas esti AC1015
	dat->bajto += 6;

	for (i = 0; i < 5; i++)
		bit_write_RC (dat, 0);	// Nekonata section
	bit_write_RC (dat, 0x0F);	// Nekonatajxo
	bit_write_RC (dat, 0x01);	// Nekonatajxo
	bit_write_RL (dat, 0);	// Bildo-address
	bit_write_RC (dat, 25);	// Versio
	bit_write_RC (dat, 0);	// Lancxo
	bit_write_RS (dat, skt->header.codepage);	// Codepage

	//skt->header.num_sections = 5; // Cxu kasxi la unknownn sectionn 1 ?
	bit_write_RL (dat, skt->header.num_sections);
	sekciadresaro = dat->bajto;	// Salti sekciadresaron
	dat->bajto += (skt->header.num_sections * 9);
	bit_read_CRC (dat);	// Salti CKR-on

	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_HEAD_END));

	/*------------------------------------------------------------
	 * Nekonata section 1
	 */

	skt->header.section[5].number = 5;
	skt->header.section[5].adresilo = 0;
	skt->header.section[5].size = 0;
	if (skt->header.num_sections == 6)
	{
		skt->header.section[5].adresilo = dat->bajto;
		skt->header.section[5].size = DWG_NBEGINNATA1_KIOM;

		skt->unknown1.kiom = skt->header.section[5].size;
		skt->unknown1.bajto = skt->unknown1.bito = 0;
		while (dat->bajto + skt->unknown1.kiom >= dat->kiom)
			bit_chain_rezervi (dat);
		memcpy (&dat->chain[dat->bajto], skt->unknown1.chain, skt->unknown1.kiom);
		dat->bajto += skt->unknown1.kiom;

	}

	/*------------------------------------------------------------
	 * Antauxrigarda picture
	 */

	/* Finfine write la addressn de la picture
	 */
	pvzadr = dat->bajto;
	dat->bajto = 0x0D;
	bit_write_RL (dat, pvzadr);
	dat->bajto = pvzadr;

	/* Kopii la picturen
	 */
	//skt->picture.kiom = 0; // Se oni deziras ne kopii picturen, malkomentu tiun cxi linion
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_PICTURE_BEGIN));
	for (i = 0; i < skt->picture.kiom; i++)
		bit_write_RC (dat, skt->picture.chain[i]);
	if (skt->picture.kiom == 0)
	{
		bit_write_RL (dat, 5);
		bit_write_RC (dat, 0);
	}
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_PICTURE_END));


	/*------------------------------------------------------------
	 * Kap-variabloj
	 */

	skt->header.section[0].number = 0;
	skt->header.section[0].adresilo = dat->bajto;
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_BEGIN));
	pvzadr = dat->bajto;	// poste oni devas rewrite la korektan valuen de size cxi tie:
	bit_write_RL (dat, 0);	// Size de la section

	for (i = 0; i < DWG_NUM_VARIABLES; i++)
	{
		if (i == 221 && skt->var[220].dubitoko != 3)
			continue;
		switch (dwg_var_map (i))
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
			bit_write_T (dat, skt->var[i].text);
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
			printf ("Ne traktebla tipo: %i (var: %i)\n", dwg_var_map (i), (int) i);
		}
	}

	/* Skribi la sizen de la section cxe gxia komenco
	 */
	pvzadr_2 = dat->bajto;
	pvzbit = dat->bito;
	dat->bajto = pvzadr;
	dat->bito = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->bajto = pvzadr_2;
	dat->bito = pvzbit;
	//printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

	/* CKR kaj sentinel
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_END));
	skt->header.section[0].size = dat->bajto - skt->header.section[0].adresilo;

	/*------------------------------------------------------------
	 * Classj
	 */
	skt->header.section[1].number = 1;
	skt->header.section[1].adresilo = dat->bajto;
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_BEGIN));
	pvzadr = dat->bajto;	// poste oni devas rewrite la korektan valuen de size cxi tie:
	bit_write_RL (dat, 0);	// Size de la section

	for (i = 0; i < skt->class_kiom; i++)
	{
		bit_write_BS (dat, skt->class[i].number);
		bit_write_BS (dat, skt->class[i].version);
		bit_write_T (dat, skt->class[i].apname);
		bit_write_T (dat, skt->class[i].cplipliname);
		bit_write_T (dat, skt->class[i].dxfname);
		bit_write_B (dat, skt->class[i].estisfantomo);
		bit_write_BS (dat, skt->class[i].eroid);
	}

	/* Skribi la sizen de la section cxe gxia komenco
	 */
	pvzadr_2 = dat->bajto;
	pvzbit = dat->bito;
	dat->bajto = pvzadr;
	dat->bito = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->bajto = pvzadr_2;
	dat->bito = pvzbit;
	//printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

	/* CKR kaj sentinel
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_END));
	skt->header.section[1].size = dat->bajto - skt->header.section[1].adresilo;


	/*------------------------------------------------------------
	 * Objectj
	 */
	bit_write_RL (dat, 0x00000000);	// 0xDCA Nekonata kvarbitoko inter classj kaj objektaro
	pvzadr = dat->bajto;

	/* Ekdifini object-mapon
	 */
	omap = (Object_Mapo *) malloc (skt->object_kiom * sizeof (Object_Mapo));
	for (i = 0; i < skt->object_kiom; i++)
	{
		Bit_Chain nkn;
		Dwg_Traktilo tkt;

		/* Difini la traktilojn de cxiuj objectj, inkluzive la unknownj */
		omap[i].idc = i;
		if (skt->object[i].supertipo == DWG_SUPERTYPE_ESTAJXO)
			omap[i].traktilo = skt->object[i].tio.estajxo->traktilo.value;
		else if (skt->object[i].supertipo == DWG_SUPERTYPE_ORDINARAJXO)
			omap[i].traktilo = skt->object[i].tio.ordinarajxo->traktilo.value;
		else if (skt->object[i].supertipo == DWG_SUPERTYPE_UNKNOWN)
		{
			nkn.chain = skt->object[i].tio.unknownjxo;
			nkn.kiom = skt->object[i].size;
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
	//for (i = 0; i < skt->object_kiom; i++) printf ("Trakt(%i): %lu / Idc: %u\n", i, omap[i].traktilo, omap[i].idc);

	/* Skribi la objektaron
	 */
	for (i = 0; i < skt->object_kiom; i++)
	{
		omap[i].address = dat->bajto;
		obj = &skt->object[omap[i].idc];
		if (obj->supertipo == DWG_SUPERTYPE_UNKNOWN)
		{
			bit_write_MS (dat, obj->size);
			if (dat->bajto + obj->size >= dat->kiom - 2)
				bit_chain_rezervi (dat);
			memcpy (&dat->chain[dat->bajto], obj->tio.unknownjxo, obj->size);
			dat->bajto += obj->size;
		}
		else
		{
			if (obj->supertipo == DWG_SUPERTYPE_ESTAJXO)
				dwg_encode_estajxo (obj, dat);
			else if (obj->supertipo == DWG_SUPERTYPE_ORDINARAJXO)
				dwg_encode_ordinarajxo (obj, dat);
			else
			{
				printf ("Eraro: ne difinita (super)tipo de object por write\n");
				exit (-1);
			}
		}
		bit_krei_CRC (dat, omap[i].address, 0xC0C1);
	}
	//for (i = 0; i < skt->object_kiom; i++) printf ("Trakt(%i): %6lu / Adreso: %08X / Idc: %u\n", i, omap[i].traktilo, omap[i].address, omap[i].idc);

	/* Nekonata dubitoko inter la objektaron kaj la object-mapo
	 */
	bit_write_RS (dat, 0);

	/*------------------------------------------------------------
	 * Object-mapo
	 */
	skt->header.section[2].number = 2;
	skt->header.section[2].adresilo = dat->bajto;	// poste oni devas kalkuli la valuen de size
	//printf ("Ekaddress: 0x%08X\n", dat->bajto);

	sekcisize = 0;
	pvzadr = dat->bajto;	// poste oni devas write cxi tie la korektan valuen de size de la unua section
	dat->bajto += 2;
	lastadres = 0;
	lastatrakt = 0;
	for (i = 0; i < skt->object_kiom; i++)
	{
		unsigned int idc;
		long int pvz;

		idc = omap[i].idc;

		pvz = omap[idc].traktilo - lastatrakt;
		bit_write_MC (dat, pvz);
		//printf ("Trakt(%i): %6lu / ", i, pvz);
		lastatrakt = omap[idc].traktilo;

		pvz = omap[idc].address - lastadres;
		bit_write_MC (dat, pvz);
		//printf ("Adreso: %08X\n", pvz);
		lastadres = omap[idc].address;

		ckr_mankanta = 1;
		if (dat->bajto - pvzadr > 2030)	// 2029
		{
			ckr_mankanta = 0;
			sekcisize = dat->bajto - pvzadr;
			dat->chain[pvzadr] = sekcisize >> 8;
			dat->chain[pvzadr + 1] = sekcisize & 0xFF;
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
		sekcisize = dat->bajto - pvzadr;
		dat->chain[pvzadr] = sekcisize >> 8;
		dat->chain[pvzadr + 1] = sekcisize & 0xFF;
		bit_krei_CRC (dat, pvzadr, 0xC0C1);
	}
	pvzadr = dat->bajto;
	bit_write_RC (dat, 0);
	bit_write_RC (dat, 2);
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	/* Kalkuli kaj write la sizen de la object-mapo
	 */
	skt->header.section[2].size = dat->bajto - skt->header.section[2].adresilo;
	free (omap);

	/*------------------------------------------------------------
	 * Dua kap-datenaro
	 */
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_DUAHEAD_BEGIN));

	pvzadr = dat->bajto;	// Gardi la unuan addressn de la section por write ties sizen poste
	bit_write_RL (dat, 0);

	bit_write_BL (dat, pvzadr - 16);	// ekaddress de la section

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
		bit_write_BL (dat, skt->header.section[0].adresilo);
		bit_write_BL (dat, skt->header.section[0].size);
	}

	/* Traktilaro
	 */
	bit_write_BS (dat, 14);
	for (i = 0; i < 14; i++)
	{
		bit_write_RC (dat, skt->duaheader.traktrik[i].kiom);
		bit_write_RC (dat, i);
		for (j = 0; j < skt->duaheader.traktrik[i].kiom; j++)
			bit_write_RC (dat, skt->duaheader.traktrik[i].chain[j]);
	}

	/* Returni al la komenco por write la sizen
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

	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_DUAHEAD_END));

	/*------------------------------------------------------------
	 * MEASUREMENT
	 */
	skt->header.section[3].number = 3;
	skt->header.section[3].adresilo = 0;
	skt->header.section[3].size = 0;
	skt->header.section[4].number = 4;
	skt->header.section[4].adresilo = dat->bajto;
	skt->header.section[4].size = 4;
	bit_write_RL (dat, skt->mezuro);

	/* Fino de la dosiero
	 */
	dat->kiom = dat->bajto;

	/* Skribi sekciadresaron
	 */
	dat->bajto = sekciadresaro;
	dat->bito = 0;
	for (i = 0; i < skt->header.num_sections; i++)
	{
		bit_write_RC (dat, skt->header.section[i].number);
		bit_write_RL (dat, skt->header.section[i].adresilo);
		bit_write_RL (dat, skt->header.section[i].size);
	}

	/* Skribi CKR-on
	 */
	bit_krei_CRC (dat, 0, 0);
	dat->bajto -= 2;
	ckr = bit_read_CRC (dat);
	dat->bajto -= 2;
	switch (skt->header.num_sections)
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
dwg_encode_estajxo (Dwg_Object * obj, Bit_Chain * dat)
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

	bit_write_MS (dat, obj->size);

	ekadr.bajto = dat->bajto;	// Por kalkuli poste la bajta kaj bita sizej de la object
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

	bit_write_B (dat, est->picture_ekzistas);
	if (est->picture_ekzistas)
	{
		bit_write_RL (dat, est->picture_kiom);
		for (i = 0; i < est->picture_kiom; i++)
			bit_write_RC (dat, est->picture[i]);
	}

	bit_write_BB (dat, est->regime);
	bit_write_BL (dat, est->reagilo_kiom);
	bit_write_B (dat, est->senligiloj);
	bit_write_BS (dat, est->colour);
	bit_write_BD (dat, est->linitiposkalo);
	bit_write_BB (dat, est->linitipo);
	bit_write_BB (dat, est->printstilo);
	bit_write_BS (dat, est->malvidebleco);
	bit_write_RC (dat, est->linithickness);

	switch (obj->tipo)
	{
	case DWG_TYPE_LINE:
		dwg_encode_LINE (est->tio.LINE, dat);
		break;
	case DWG_TYPE_CIRCLE:
		dwg_encode_CIRCLE (est->tio.CIRCLE, dat);
		break;

	default:
		printf ("Eraro: unknown object-tipo dum enkodigo de estaĵo\n");
		exit (-1);
	}

	/* Finfine kalkuli kaj write la bit-sizen de la object
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

	/* Finfine kalkuli kaj write la bajt-sizen de la object (cxu estas erara?)
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
dwg_encode_ordinarajxo (Dwg_Object * obj, Bit_Chain * dat)
{
	Bit_Chain ekadr;

	bit_write_MS (dat, obj->size);
	ekadr.bajto = dat->bajto;	// Por kalkuli poste la bita size de la object
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
	bit_write_BT (dat, est->thickness);
	bit_write_BE (dat, est->extrusion.x, est->extrusion.y, est->extrusion.z);
}

static void
dwg_encode_CIRCLE (Dwg_Estajxo_CIRCLE * est, Bit_Chain * dat)
{
	bit_write_BD (dat, est->x0);
	bit_write_BD (dat, est->y0);
	bit_write_BD (dat, est->z0);
	bit_write_BD (dat, est->radius);
	bit_write_BT (dat, est->thickness);
	bit_write_BE (dat, est->extrusion.x, est->extrusion.y, est->extrusion.z);
}

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
static void dwg_encode_entity (Dwg_Object * obj, Bit_Chain * dat);
static void dwg_encode_object (Dwg_Object * obj, Bit_Chain * dat);
static void dwg_encode_LINE (Dwg_Entity_LINE * est, Bit_Chain * dat);
static void dwg_encode_CIRCLE (Dwg_Entity_CIRCLE * est, Bit_Chain * dat);

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
	dat->byte += 6;

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
	sekciadresaro = dat->byte;	// Salti sekciadresaron
	dat->byte += (skt->header.num_sections * 9);
	bit_read_CRC (dat);	// Salti CKR-on

	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_HEADER_END));

	/*------------------------------------------------------------
	 * Nekonata section 1
	 */

	skt->header.section[5].number = 5;
	skt->header.section[5].address = 0;
	skt->header.section[5].size = 0;
	if (skt->header.num_sections == 6)
	{
		skt->header.section[5].address = dat->byte;
		skt->header.section[5].size = DWG_NBEGINNATA1_KIOM;

		skt->unknown1.kiom = skt->header.section[5].size;
		skt->unknown1.byte = skt->unknown1.bito = 0;
		while (dat->byte + skt->unknown1.kiom >= dat->kiom)
			bit_chain_rezervi (dat);
		memcpy (&dat->chain[dat->byte], skt->unknown1.chain, skt->unknown1.kiom);
		dat->byte += skt->unknown1.kiom;

	}

	/*------------------------------------------------------------
	 * Antauxrigarda picture
	 */

	/* Finfine write la addressn de la picture
	 */
	pvzadr = dat->byte;
	dat->byte = 0x0D;
	bit_write_RL (dat, pvzadr);
	dat->byte = pvzadr;

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
	skt->header.section[0].address = dat->byte;
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_BEGIN));
	pvzadr = dat->byte;	// poste oni devas rewrite la korektan valuen de size cxi tie:
	bit_write_RL (dat, 0);	// Size de la section

	for (i = 0; i < DWG_NUM_VARIABLES; i++)
	{
		if (i == 221 && skt->var[220].dubitoko != 3)
			continue;
		switch (dwg_var_map (skt->header.version, i))
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
			printf ("Ne traktebla type: %i (var: %i)\n", dwg_var_map (skt->header.version, i), (int) i);
		}
	}

	/* Skribi la sizen de la section cxe gxia komenco
	 */
	pvzadr_2 = dat->byte;
	pvzbit = dat->bito;
	dat->byte = pvzadr;
	dat->bito = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->byte = pvzadr_2;
	dat->bito = pvzbit;
	//printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

	/* CKR kaj sentinel
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_VARIABLE_END));
	skt->header.section[0].size = dat->byte - skt->header.section[0].address;

	/*------------------------------------------------------------
	 * Classj
	 */
	skt->header.section[1].number = 1;
	skt->header.section[1].address = dat->byte;
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_BEGIN));
	pvzadr = dat->byte;	// poste oni devas rewrite la korektan valuen de size cxi tie:
	bit_write_RL (dat, 0);	// Size de la section

	for (i = 0; i < skt->num_classes; i++)
	{
		bit_write_BS (dat, skt->class[i].number);
		bit_write_BS (dat, skt->class[i].version);
		bit_write_T (dat, skt->class[i].appname);
		bit_write_T (dat, skt->class[i].cppname);
		bit_write_T (dat, skt->class[i].dxfname);
		bit_write_B (dat, skt->class[i].estisfantomo);
		bit_write_BS (dat, skt->class[i].eroid);
	}

	/* Skribi la sizen de la section cxe gxia komenco
	 */
	pvzadr_2 = dat->byte;
	pvzbit = dat->bito;
	dat->byte = pvzadr;
	dat->bito = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->byte = pvzadr_2;
	dat->bito = pvzbit;
	//printf ("Size: %lu\n", pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));

	/* CKR kaj sentinel
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_CLASS_END));
	skt->header.section[1].size = dat->byte - skt->header.section[1].address;


	/*------------------------------------------------------------
	 * Objectj
	 */
	bit_write_RL (dat, 0x00000000);	// 0xDCA Nekonata kvarbitoko inter classj kaj objektaro
	pvzadr = dat->byte;

	/* Ekdifini object-mapon
	 */
	omap = (Object_Mapo *) malloc (skt->num_objects * sizeof (Object_Mapo));
	for (i = 0; i < skt->num_objects; i++)
	{
		Bit_Chain nkn;
		Dwg_Traktilo tkt;

		/* Difini la traktilojn de cxiuj objectj, inkluzive la unknownj */
		omap[i].idc = i;
		if (skt->object[i].supertype == DWG_SUPERTYPE_ENTITY)
			omap[i].traktilo = skt->object[i].tio.entity->traktilo.value;
		else if (skt->object[i].supertype == DWG_SUPERTYPE_OBJECT)
			omap[i].traktilo = skt->object[i].tio.object->traktilo.value;
		else if (skt->object[i].supertype == DWG_SUPERTYPE_UNKNOWN)
		{
			nkn.chain = skt->object[i].tio.unknownjxo;
			nkn.kiom = skt->object[i].size;
			nkn.byte = nkn.bito = 0;
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
	//for (i = 0; i < skt->num_objects; i++) printf ("Trakt(%i): %lu / Idc: %u\n", i, omap[i].traktilo, omap[i].idc);

	/* Skribi la objektaron
	 */
	for (i = 0; i < skt->num_objects; i++)
	{
		omap[i].address = dat->byte;
		obj = &skt->object[omap[i].idc];
		if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
		{
			bit_write_MS (dat, obj->size);
			if (dat->byte + obj->size >= dat->kiom - 2)
				bit_chain_rezervi (dat);
			memcpy (&dat->chain[dat->byte], obj->tio.unknownjxo, obj->size);
			dat->byte += obj->size;
		}
		else
		{
			if (obj->supertype == DWG_SUPERTYPE_ENTITY)
				dwg_encode_entity (obj, dat);
			else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
				dwg_encode_object (obj, dat);
			else
			{
				printf ("Eraro: ne difinita (super)type de object por write\n");
				exit (-1);
			}
		}
		bit_krei_CRC (dat, omap[i].address, 0xC0C1);
	}
	//for (i = 0; i < skt->num_objects; i++) printf ("Trakt(%i): %6lu / Adreso: %08X / Idc: %u\n", i, omap[i].traktilo, omap[i].address, omap[i].idc);

	/* Nekonata dubitoko inter la objektaron kaj la object-mapo
	 */
	bit_write_RS (dat, 0);

	/*------------------------------------------------------------
	 * Object-mapo
	 */
	skt->header.section[2].number = 2;
	skt->header.section[2].address = dat->byte;	// poste oni devas kalkuli la valuen de size
	//printf ("Ekaddress: 0x%08X\n", dat->byte);

	sekcisize = 0;
	pvzadr = dat->byte;	// poste oni devas write cxi tie la korektan valuen de size de la unua section
	dat->byte += 2;
	lastadres = 0;
	lastatrakt = 0;
	for (i = 0; i < skt->num_objects; i++)
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
		if (dat->byte - pvzadr > 2030)	// 2029
		{
			ckr_mankanta = 0;
			sekcisize = dat->byte - pvzadr;
			dat->chain[pvzadr] = sekcisize >> 8;
			dat->chain[pvzadr + 1] = sekcisize & 0xFF;
			bit_krei_CRC (dat, pvzadr, 0xC0C1);

			pvzadr = dat->byte;
			dat->byte += 2;
			lastadres = 0;
			lastatrakt = 0;
		}
	}
	//printf ("Obj kiom: %u\n", i);
	if (ckr_mankanta)
	{
		sekcisize = dat->byte - pvzadr;
		dat->chain[pvzadr] = sekcisize >> 8;
		dat->chain[pvzadr + 1] = sekcisize & 0xFF;
		bit_krei_CRC (dat, pvzadr, 0xC0C1);
	}
	pvzadr = dat->byte;
	bit_write_RC (dat, 0);
	bit_write_RC (dat, 2);
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	/* Kalkuli kaj write la sizen de la object-mapo
	 */
	skt->header.section[2].size = dat->byte - skt->header.section[2].address;
	free (omap);

	/*------------------------------------------------------------
	 * Dua kap-datenaro
	 */
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_BEGIN));

	pvzadr = dat->byte;	// Gardi la unuan addressn de la section por write ties sizen poste
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
		bit_write_BL (dat, skt->header.section[0].address);
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
	pvzadr_2 = dat->byte;
	dat->byte = pvzadr;
	bit_write_RL (dat, pvzadr_2 - pvzadr + 10);
	dat->byte = pvzadr_2;

	/* CKR
	 */
	bit_krei_CRC (dat, pvzadr, 0xC0C1);

	/* Jen 8 bitokoj da rubajxo
	 */
	bit_write_RL (dat, 0);
	bit_write_RL (dat, 0);

	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_SECOND_HEADER_END));

	/*------------------------------------------------------------
	 * MEASUREMENT
	 */
	skt->header.section[3].number = 3;
	skt->header.section[3].address = 0;
	skt->header.section[3].size = 0;
	skt->header.section[4].number = 4;
	skt->header.section[4].address = dat->byte;
	skt->header.section[4].size = 4;
	bit_write_RL (dat, skt->measurement);

	/* Fino de la dosiero
	 */
	dat->kiom = dat->byte;

	/* Skribi sekciadresaron
	 */
	dat->byte = sekciadresaro;
	dat->bito = 0;
	for (i = 0; i < skt->header.num_sections; i++)
	{
		bit_write_RC (dat, skt->header.section[i].number);
		bit_write_RL (dat, skt->header.section[i].address);
		bit_write_RL (dat, skt->header.section[i].size);
	}

	/* Skribi CKR-on
	 */
	bit_krei_CRC (dat, 0, 0);
	dat->byte -= 2;
	ckr = bit_read_CRC (dat);
	dat->byte -= 2;
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
dwg_encode_entity (Dwg_Object * obj, Bit_Chain * dat)
{
	unsigned int i;
	long unsigned int longo;
	Bit_Chain gdadr;
	Bit_Chain ekadr;
	Bit_Chain bgadr;
	Bit_Chain pvadr;
	Dwg_Object_Entity *est;

	est = obj->tio.entity;

	gdadr.byte = dat->byte;
	gdadr.bito = dat->bito;

	bit_write_MS (dat, obj->size);

	ekadr.byte = dat->byte;	// Por kalkuli poste la bajta kaj bita sizej de la object
	ekadr.bito = dat->bito;

	bit_write_BS (dat, obj->type);

	bgadr.byte = dat->byte;
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
	bit_write_BD (dat, est->linitypeskalo);
	bit_write_BB (dat, est->linitype);
	bit_write_BB (dat, est->printstilo);
	bit_write_BS (dat, est->malvidebleco);
	bit_write_RC (dat, est->linithickness);

	switch (obj->type)
	{
	case DWG_TYPE_LINE:
		dwg_encode_LINE (est->tio.LINE, dat);
		break;
	case DWG_TYPE_CIRCLE:
		dwg_encode_CIRCLE (est->tio.CIRCLE, dat);
		break;

	default:
		printf ("Eraro: unknown object-type dum enkodigo de estaĵo\n");
		exit (-1);
	}

	/* Finfine kalkuli kaj write la bit-sizen de la object
	 */
	pvadr.byte = dat->byte;
	pvadr.bito = dat->bito;

	dat->byte = bgadr.byte;
	dat->bito = bgadr.bito;

	longo = 8 * (pvadr.byte - ekadr.byte) + (pvadr.bito);
	bit_write_RL (dat, longo);
	//printf ("Longo (bit): %lu\t", longo);

	dat->byte = pvadr.byte;
	dat->bito = pvadr.bito;

	/* Traktilaj referencoj
	 */
	for (i = 0; i < est->traktref_kiom; i++)
		bit_write_H (dat, &est->traktref[i]);

	/* Finfine kalkuli kaj write la bajt-sizen de la object (cxu estas erara?)
	 */
	pvadr.byte = dat->byte;
	pvadr.bito = dat->bito;

	dat->byte = gdadr.byte;
	dat->bito = gdadr.bito;

	longo = pvadr.byte - ekadr.byte;
	bit_write_MS (dat, longo);
	//printf ("Longo: %lu\n", longo);

	dat->byte = pvadr.byte;
	dat->bito = pvadr.bito;
}

static void
dwg_encode_object (Dwg_Object * obj, Bit_Chain * dat)
{
	Bit_Chain ekadr;

	bit_write_MS (dat, obj->size);
	ekadr.byte = dat->byte;	// Por kalkuli poste la bita size de la object
	ekadr.bito = dat->bito;
	bit_write_BS (dat, obj->type);
}

static void
dwg_encode_LINE (Dwg_Entity_LINE * est, Bit_Chain * dat)
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
dwg_encode_CIRCLE (Dwg_Entity_CIRCLE * est, Bit_Chain * dat)
{
	bit_write_BD (dat, est->x0);
	bit_write_BD (dat, est->y0);
	bit_write_BD (dat, est->z0);
	bit_write_BD (dat, est->radius);
	bit_write_BT (dat, est->thickness);
	bit_write_BE (dat, est->extrusion.x, est->extrusion.y, est->extrusion.z);
}

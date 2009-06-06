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
	long int handle;
	long int address;
	unsigned int idc;
} Object_Mapo;

/*--------------------------------------------------------------------------------
 * Private functions prototypes
 */
static void dwg_encode_entity (Dwg_Object * obj, Bit_Chain * dat);
static void dwg_encode_object (Dwg_Object * obj, Bit_Chain * dat);

static void dwg_encode_TEXT (Dwg_Entity_TEXT * est, Bit_Chain * dat);
static void dwg_encode_ATTRIB (Dwg_Entity_ATTRIB * est, Bit_Chain * dat);
static void dwg_encode_ATTDEF (Dwg_Entity_ATTDEF * est, Bit_Chain * dat);
static void dwg_encode_BLOCK (Dwg_Entity_BLOCK * est, Bit_Chain * dat);
static void dwg_encode_ENDBLK (Dwg_Entity_ENDBLK * est, Bit_Chain * dat);
static void dwg_encode_SEQEND (Dwg_Entity_SEQEND * est, Bit_Chain * dat);
static void dwg_encode_INSERT (Dwg_Entity_INSERT * est, Bit_Chain * dat);
static void dwg_encode_MINSERT (Dwg_Entity_MINSERT * est, Bit_Chain * dat);
static void dwg_encode_VERTEX_2D (Dwg_Entity_VERTEX_2D * est, Bit_Chain * dat);
static void dwg_encode_VERTEX_3D (Dwg_Entity_VERTEX_3D * est, Bit_Chain * dat);
static void dwg_encode_VERTEX_MESH (Dwg_Entity_VERTEX_MESH * est, Bit_Chain * dat);
static void dwg_encode_VERTEX_PFACE (Dwg_Entity_VERTEX_PFACE * est, Bit_Chain * dat);
static void dwg_encode_VERTEX_PFACE_FACE (Dwg_Entity_VERTEX_PFACE_FACE * est, Bit_Chain * dat);
static void dwg_encode_POLYLINE_2D (Dwg_Entity_POLYLINE_2D * est, Bit_Chain * dat);
static void dwg_encode_POLYLINE_3D (Dwg_Entity_POLYLINE_3D * est, Bit_Chain * dat);
static void dwg_encode_ARC (Dwg_Entity_ARC * est, Bit_Chain * dat);
static void dwg_encode_CIRCLE (Dwg_Entity_CIRCLE * est, Bit_Chain * dat);
static void dwg_encode_LINE (Dwg_Entity_LINE * est, Bit_Chain * dat);
static void dwg_encode_DIMENSION_ORDINATE (Dwg_Entity_DIMENSION_ORDINATE * est, Bit_Chain * dat);
static void dwg_encode_DIMENSION_LINEAR (Dwg_Entity_DIMENSION_LINEAR * est, Bit_Chain * dat);
static void dwg_encode_DIMENSION_ALIGNED (Dwg_Entity_DIMENSION_ALIGNED * est, Bit_Chain * dat);
static void dwg_encode_DIMENSION_ANG3PT (Dwg_Entity_DIMENSION_ANG3PT * est, Bit_Chain * dat);
static void dwg_encode_DIMENSION_ANG2LN (Dwg_Entity_DIMENSION_ANG2LN * est, Bit_Chain * dat);
static void dwg_encode_DIMENSION_RADIUS (Dwg_Entity_DIMENSION_RADIUS * est, Bit_Chain * dat);
static void dwg_encode_DIMENSION_DIAMETER (Dwg_Entity_DIMENSION_DIAMETER * est, Bit_Chain * dat);
static void dwg_encode_POINT (Dwg_Entity_POINT * est, Bit_Chain * dat);
static void dwg_encode_3DFACE (Dwg_Entity_3D_FACE * est, Bit_Chain * dat);
static void dwg_encode_POLYLINE_PFACE (Dwg_Entity_POLYLINE_PFACE * est, Bit_Chain * dat);
static void dwg_encode_POLYLINE_MESH (Dwg_Entity_POLYLINE_MESH * est, Bit_Chain * dat);
static void dwg_encode_SOLID (Dwg_Entity_SOLID * est, Bit_Chain * dat);
static void dwg_encode_TRACE (Dwg_Entity_TRACE * est, Bit_Chain * dat);
static void dwg_encode_SHAPE (Dwg_Entity_SHAPE * est, Bit_Chain * dat);
static void dwg_encode_VIEWPORT (Dwg_Entity_VIEWPORT *ent, Bit_Chain * dat);
static void dwg_encode_ELLIPSE (Dwg_Entity_ELLIPSE * est, Bit_Chain * dat);
static void dwg_encode_SPLINE (Dwg_Entity_SPLINE * est, Bit_Chain * dat);
static void dwg_encode_RAY (Dwg_Entity_RAY * est, Bit_Chain * dat);
static void dwg_encode_XLINE (Dwg_Entity_XLINE * est, Bit_Chain * dat);
static void dwg_encode_MTEXT (Dwg_Entity_MTEXT * est, Bit_Chain * dat);
static void dwg_encode_BLOCK_CONTROL (Dwg_Object_BLOCK_CONTROL * obj, Bit_Chain * dat);
static void dwg_encode_DICTIONARY (Dwg_Object_DICTIONARY * obj, Bit_Chain * dat);
static void dwg_encode_LAYER (Dwg_Object_LAYER * obj, Bit_Chain * dat);

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

	bit_chain_alloc (dat);

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
		skt->header.section[5].size = DWG_UNKNOWN1_KIOM;

		skt->unknown1.size = skt->header.section[5].size;
		skt->unknown1.byte = skt->unknown1.bit = 0;
		while (dat->byte + skt->unknown1.size >= dat->size)
			bit_chain_alloc (dat);
		memcpy (&dat->chain[dat->byte], skt->unknown1.chain, skt->unknown1.size);
		dat->byte += skt->unknown1.size;

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
	//skt->picture.size = 0; // Se oni deziras ne kopii picturen, malkomentu tiun cxi linion
	bit_write_sentinel (dat, dwg_sentinel (DWG_SENTINEL_PICTURE_BEGIN));
	for (i = 0; i < skt->picture.size; i++)
		bit_write_RC (dat, skt->picture.chain[i]);
	if (skt->picture.size == 0)
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
			bit_write_H (dat, &skt->var[i].handle);
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
	pvzbit = dat->bit;
	dat->byte = pvzadr;
	dat->bit = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->byte = pvzadr_2;
	dat->bit = pvzbit;
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
	pvzbit = dat->bit;
	dat->byte = pvzadr;
	dat->bit = 0;
	bit_write_RL (dat, pvzadr_2 - pvzadr - (pvzbit ? 3 : 4));
	dat->byte = pvzadr_2;
	dat->bit = pvzbit;
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
		Dwg_Handle tkt;

		/* Difini la handlejn de cxiuj objectj, inkluzive la unknownj */
		omap[i].idc = i;
		if (skt->object[i].supertype == DWG_SUPERTYPE_ENTITY)
			omap[i].handle = skt->object[i].tio.entity->handle.value;
		else if (skt->object[i].supertype == DWG_SUPERTYPE_OBJECT)
			omap[i].handle = skt->object[i].tio.object->handle.value;
		else if (skt->object[i].supertype == DWG_SUPERTYPE_UNKNOWN)
		{
			nkn.chain = skt->object[i].tio.unknownjxo;
			nkn.size = skt->object[i].size;
			nkn.byte = nkn.bit = 0;
			bit_read_BS (&nkn);
			bit_read_RL (&nkn);
			bit_read_H (&nkn, &tkt);
			omap[i].handle = tkt.value;
		}
		else
			omap[i].handle = 0x7FFFFFFF;	/* Eraro! */

		/* Ordigi la sekvon de handlej laux kreskanta ordo */
		if (i > 0)
		{
			j = i;
			while (omap[j].handle < omap[j - 1].handle)
			{
				omap[j - 1].handle = pvzmap.handle;
				omap[j - 1].idc = pvzmap.idc;

				omap[j - 1].handle = omap[j].handle;
				omap[j - 1].idc = omap[j].idc;

				omap[j].handle = pvzmap.handle;
				omap[j].idc = pvzmap.idc;
				j--;
				if (j == 0)
					break;
			}
		}
	}
	//for (i = 0; i < skt->num_objects; i++) printf ("Trakt(%i): %lu / Idc: %u\n", i, omap[i].handle, omap[i].idc);

	/* Skribi la objektaron
	 */
	for (i = 0; i < skt->num_objects; i++)
	{
		omap[i].address = dat->byte;
		obj = &skt->object[omap[i].idc];
		if (obj->supertype == DWG_SUPERTYPE_UNKNOWN)
		{
			bit_write_MS (dat, obj->size);
			if (dat->byte + obj->size >= dat->size - 2)
				bit_chain_alloc (dat);
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
	//for (i = 0; i < skt->num_objects; i++) printf ("Trakt(%i): %6lu / Adreso: %08X / Idc: %u\n", i, omap[i].handle, omap[i].address, omap[i].idc);

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

		pvz = omap[idc].handle - lastatrakt;
		bit_write_MC (dat, pvz);
		//printf ("Trakt(%i): %6lu / ", i, pvz);
		lastatrakt = omap[idc].handle;

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
	//printf ("Obj size: %u\n", i);
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

	bit_write_BL (dat, pvzadr - 16);	// start_address de la section

	/* Version Code
	 */
	for (i-0;i<6;i++)
		bit_write_RC (dat, version_codes[dat->version][i]);

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
		bit_write_RC (dat, skt->second_header.traktrik[i].size);
		bit_write_RC (dat, i);
		for (j = 0; j < skt->second_header.traktrik[i].size; j++)
			bit_write_RC (dat, skt->second_header.traktrik[i].chain[j]);
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
	dat->size = dat->byte;

	/* Skribi sekciadresaron
	 */
	dat->byte = sekciadresaro;
	dat->bit = 0;
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
	gdadr.bit = dat->bit;

	bit_write_MS (dat, obj->size);

	ekadr.byte = dat->byte;	// Por kalkuli poste la bajta kaj bita sizej de la object
	ekadr.bit = dat->bit;

	bit_write_BS (dat, obj->type);

	bgadr.byte = dat->byte;
	bgadr.bit = dat->bit;

	bit_write_RL (dat, 0);	// Nulo nun, kalkulendas por write poste

	bit_write_H (dat, &est->handle);
	bit_write_BS (dat, est->extended_size);
	if (est->extended_size > 0)
	{
		bit_write_H (dat, &est->extended_trakt);
		for (i = 0; i < est->extended_size; i++)
			bit_write_RC (dat, est->extended[i]);
	}

	bit_write_B (dat, est->picture_exists);
	if (est->picture_exists)
	{
		bit_write_RL (dat, est->picture_size);
		for (i = 0; i < est->picture_size; i++)
			bit_write_RC (dat, est->picture[i]);
	}

	bit_write_BB (dat, est->regime);
	bit_write_BL (dat, est->reagilo_size);
	bit_write_B (dat, est->senligiloj);
	bit_write_BS (dat, est->colour);
	bit_write_BD (dat, est->linetype_scale);
	bit_write_BB (dat, est->linetype);
	bit_write_BB (dat, est->plot_style);
	bit_write_BS (dat, est->invisible);
	bit_write_RC (dat, est->lineweight);

	switch (obj->type)
	{
	case DWG_TYPE_TEXT:
		dwg_encode_TEXT (est->tio.TEXT, dat);
		break;
	case DWG_TYPE_ATTRIB:
		dwg_encode_ATTRIB (est->tio.ATTRIB, dat);
		break;
	case DWG_TYPE_ATTDEF:
		dwg_encode_ATTDEF (est->tio.ATTDEF, dat);
		break;
	case DWG_TYPE_BLOCK:
		dwg_encode_BLOCK (est->tio.BLOCK, dat);
		break;
	case DWG_TYPE_ENDBLK:
		dwg_encode_ENDBLK (est->tio.ENDBLK, dat);
		break;
	case DWG_TYPE_SEQEND:
		dwg_encode_SEQEND (est->tio.SEQEND, dat);
		break;
	case DWG_TYPE_INSERT:
		dwg_encode_INSERT (est->tio.INSERT, dat);
		break;
	case DWG_TYPE_MINSERT:
		dwg_encode_MINSERT (est->tio.MINSERT, dat);
		break;
	case DWG_TYPE_VERTEX_2D:
		dwg_encode_VERTEX_2D (est->tio.VERTEX_2D, dat);
		break;
	case DWG_TYPE_VERTEX_3D:
		dwg_encode_VERTEX_3D (est->tio.VERTEX_3D, dat);
		break;
	case DWG_TYPE_VERTEX_MESH:
		dwg_encode_VERTEX_MESH (est->tio.VERTEX_MESH, dat);
		break;
	case DWG_TYPE_VERTEX_PFACE:
		dwg_encode_VERTEX_PFACE (est->tio.VERTEX_PFACE, dat);
		break;
	case DWG_TYPE_VERTEX_PFACE_FACE:
		dwg_encode_VERTEX_PFACE_FACE (est->tio.VERTEX_PFACE_FACE, dat);
		break;
	case DWG_TYPE_POLYLINE_2D:
		dwg_encode_POLYLINE_2D (est->tio.POLYLINE_2D, dat);
		break;
	case DWG_TYPE_POLYLINE_3D:
		dwg_encode_POLYLINE_3D (est->tio.POLYLINE_3D, dat);
		break;
	case DWG_TYPE_ARC:
		dwg_encode_ARC (est->tio.ARC, dat);
		break;
	case DWG_TYPE_LINE:
		dwg_encode_LINE (est->tio.LINE, dat);
		break;
	case DWG_TYPE_CIRCLE:
		dwg_encode_CIRCLE (est->tio.CIRCLE, dat);
		break;
	case DWG_TYPE_DIMENSION_ORDINATE:
		dwg_encode_DIMENSION_ORDINATE (est->tio.DIMENSION_ORDINATE, dat);
		break;
	case DWG_TYPE_DIMENSION_LINEAR:
		dwg_encode_DIMENSION_LINEAR (est->tio.DIMENSION_LINEAR, dat);
		break;
	case DWG_TYPE_DIMENSION_ALIGNED:
		dwg_encode_DIMENSION_ALIGNED (est->tio.DIMENSION_ALIGNED, dat);
		break;
	case DWG_TYPE_DIMENSION_ANG3PT:
		dwg_encode_DIMENSION_ANG3PT (est->tio.DIMENSION_ANG3PT, dat);
		break;
	case DWG_TYPE_DIMENSION_ANG2LN:
		dwg_encode_DIMENSION_ANG2LN (est->tio.DIMENSION_ANG2LN, dat);
		break;
	case DWG_TYPE_DIMENSION_RADIUS:
		dwg_encode_DIMENSION_RADIUS (est->tio.DIMENSION_RADIUS, dat);
		break;
	case DWG_TYPE_DIMENSION_DIAMETER:
		dwg_encode_DIMENSION_DIAMETER (est->tio.DIMENSION_DIAMETER, dat);
		break;
	case DWG_TYPE_POINT:
		dwg_encode_POINT (est->tio.POINT, dat);
		break;
	case DWG_TYPE_3DFACE:
		dwg_encode_3DFACE (est->tio._3DFACE, dat);
		break;
	case DWG_TYPE_POLYLINE_PFACE:
		dwg_encode_POLYLINE_PFACE (est->tio.POLYLINE_PFACE, dat);
		break;
	case DWG_TYPE_POLYLINE_MESH:
		dwg_encode_POLYLINE_MESH (est->tio.POLYLINE_MESH, dat);
		break;
	case DWG_TYPE_SOLID:
		dwg_encode_SOLID (est->tio.SOLID, dat);
		break;
	case DWG_TYPE_TRACE:
		dwg_encode_TRACE (est->tio.TRACE, dat);
		break;
	case DWG_TYPE_SHAPE:
		dwg_encode_SHAPE (est->tio.SHAPE, dat);
		break;
	case DWG_TYPE_VIEWPORT:
		dwg_encode_VIEWPORT (est->tio.VIEWPORT, dat);
		break;
	case DWG_TYPE_ELLIPSE:
		dwg_encode_ELLIPSE (est->tio.ELLIPSE, dat);
		break;
	case DWG_TYPE_SPLINE:
		dwg_encode_SPLINE (est->tio.SPLINE, dat);
		break;
	case DWG_TYPE_RAY:
		dwg_encode_RAY (est->tio.RAY, dat);
		break;
	case DWG_TYPE_XLINE:
		dwg_encode_XLINE (est->tio.XLINE, dat);
		break;
	case DWG_TYPE_MTEXT:
		dwg_encode_MTEXT (est->tio.MTEXT, dat);
		break;
	default:
		printf ("Eraro: unknown object-type dum enkodigo de estaĵo\n");
		exit (-1);
	}

	/* Finfine kalkuli kaj write la bit-sizen de la object
	 */
	pvadr.byte = dat->byte;
	pvadr.bit = dat->bit;

	dat->byte = bgadr.byte;
	dat->bit = bgadr.bit;

	longo = 8 * (pvadr.byte - ekadr.byte) + (pvadr.bit);
	bit_write_RL (dat, longo);
	//printf ("Longo (bit): %lu\t", longo);

	dat->byte = pvadr.byte;
	dat->bit = pvadr.bit;

	/* Traktilaj referencoj
	 */
	for (i = 0; i < est->traktref_size; i++)
		bit_write_H (dat, &est->traktref[i]);

	/* Finfine kalkuli kaj write la bajt-sizen de la object (cxu estas erara?)
	 */
	pvadr.byte = dat->byte;
	pvadr.bit = dat->bit;

	dat->byte = gdadr.byte;
	dat->bit = gdadr.bit;

	longo = pvadr.byte - ekadr.byte;
	bit_write_MS (dat, longo);
	//printf ("Longo: %lu\n", longo);

	dat->byte = pvadr.byte;
	dat->bit = pvadr.bit;
}

static void
dwg_encode_object (Dwg_Object * obj, Bit_Chain * dat)
{
	Bit_Chain ekadr;

	bit_write_MS (dat, obj->size);
	ekadr.byte = dat->byte;	// Por kalkuli poste la bita size de la object
	ekadr.bit = dat->bit;
	bit_write_BS (dat, obj->type);
}

static void
dwg_encode_TEXT (Dwg_Entity_TEXT * ent, Bit_Chain * dat)
{
        //TODO: check
        if (dat->version == R_13 ||
            dat->version == R_14)
        {
                bit_write_BD (dat, ent->elevation);
                bit_write_RD (dat, ent->x0);
                bit_write_RD (dat, ent->y0);
                bit_write_RD (dat, ent->alignment.x);
                bit_write_RD (dat, ent->alignment.y);
                bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
                bit_write_BD (dat, ent->thickness);
                bit_write_BD (dat, ent->oblique_ang);
                bit_write_BD (dat, ent->rotation_ang);
                bit_write_BD (dat, ent->height);
                bit_write_BD (dat, ent->width_factor);
                bit_write_T (dat, ent->text);
                bit_write_BS (dat, ent->generation);
                bit_write_BS (dat, ent->alignment.h);
                bit_write_BS (dat, ent->alignment.v);
        }

        if (dat->version >= R_2000)
        {
                bit_write_RC (dat, ent->dataflags);
                if ((!ent->dataflags & 0x01))
                        bit_write_RD (dat, ent->elevation);
                bit_write_RD (dat, ent->x0);
                bit_write_RD (dat, ent->y0);
                if (!(ent->dataflags & 0x02))
                {
                        bit_write_DD (dat, ent->alignment.x, 10);
                        bit_write_DD (dat, ent->alignment.y, 20);
                }
                bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
                bit_write_BT (dat, ent->thickness);
                if (!(ent->dataflags & 0x04))
                        bit_write_RD (dat, ent->oblique_ang);
                if (!(ent->dataflags & 0x08))
                        bit_write_RD (dat, ent->rotation_ang);
                bit_write_RD (dat, ent->height);
                if (!(ent->dataflags & 0x10))
                        bit_write_RD (dat, ent->width_factor);
                bit_write_T (dat, ent->text);
                if (!(ent->dataflags & 0x20))
                        bit_write_BS (dat, ent->generation);
                if (!(ent->dataflags & 0x40))
                        bit_write_BS (dat, ent->alignment.h);
                if (!(ent->dataflags & 0x80))
                        bit_write_BS (dat, ent->alignment.v);
        }
}

static void
dwg_encode_ATTRIB (Dwg_Entity_ATTRIB * ent, Bit_Chain * dat)
{
    //TODO: check
    if (dat->version == R_13 ||
        dat->version == R_14)
    {
	    bit_write_BD (dat, ent->elevation);
	    bit_write_RD (dat,ent->x0);
	    bit_write_RD (dat, ent->y0);
	    bit_write_RD (dat, ent->alignment.x);
	    bit_write_RD (dat, ent->alignment.y);
	    bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
	    bit_write_BD (dat, ent->thickness);
	    bit_write_BD (dat, ent->oblique_ang);
	    bit_write_BD (dat, ent->rotation_ang);
	    bit_write_BD (dat, ent->height);
	    bit_write_BD (dat, ent->width_factor);
	    bit_write_T (dat, ent->text);
	    bit_write_BS (dat, ent->generation);
	    bit_write_BS (dat, ent->alignment.h);
	    bit_write_BS (dat, ent->alignment.v);
    }

    if (dat->version >= R_2000)
    {
	    bit_write_RC (dat, ent->dataflags);
	    if ((!ent->dataflags & 0x01))
		    bit_write_RD (dat, ent->elevation);
	    bit_write_RD (dat, ent->x0);
	    bit_write_RD (dat, ent->y0);
	    if (!(ent->dataflags & 0x02))
	    {
		    bit_write_DD (dat, ent->alignment.x, 10);
		    bit_write_DD (dat, ent->alignment.y, 20);
	    }
	    bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
	    bit_write_BT (dat, ent->thickness);
	    if (!(ent->dataflags & 0x04))
		    bit_write_RD (dat, ent->oblique_ang);
	    if (!(ent->dataflags & 0x08))
		    bit_write_RD (dat, ent->rotation_ang);
	    bit_write_RD (dat, ent->height);
	    if (!(ent->dataflags & 0x10))
		    bit_write_RD (dat, ent->width_factor);
	    bit_write_T (dat, ent->text);
	    if (!(ent->dataflags & 0x20))
		    bit_write_BS (dat, ent->generation);
	    if (!(ent->dataflags & 0x40))
		    bit_write_BS (dat, ent->alignment.h);
	    if (!(ent->dataflags & 0x80))
		    bit_write_BS (dat, ent->alignment.v);
    }

	bit_write_T (dat, ent->tag);
	bit_write_BS (dat, ent->field_length);
	bit_write_RC (dat, ent->flags);

    if (dat->version >= R_2007){
        bit_write_B (dat, ent->lock_position_flag);
    }
}

static void
dwg_encode_ATTDEF (Dwg_Entity_ATTDEF * ent, Bit_Chain * dat)
{
    //TODO: check
    if (dat->version == R_13 ||
        dat->version == R_14)
    {

	    bit_write_BD (dat, ent->elevation);
	    bit_write_RD (dat, ent->x0);
	    bit_write_RD (dat, ent->y0);
	    bit_write_RD (dat, ent->alignment.x);
	    bit_write_RD (dat, ent->alignment.y);
	    bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
	    bit_write_BD (dat, ent->thickness);
	    bit_write_BD (dat, ent->oblique_ang);
	    bit_write_BD (dat, ent->rotation_ang);
	    bit_write_BD (dat, ent->height);
	    bit_write_BD (dat, ent->width_factor);
	    bit_write_T (dat, ent->text);
	    bit_write_BS (dat, ent->generation);
	    bit_write_BS (dat, ent->alignment.h);
	    bit_write_BS (dat, ent->alignment.v);
    }

    if (dat->version >= R_2000)
    {
	    bit_write_RC (dat, ent->dataflags);
	    if ((!ent->dataflags & 0x01))
		    bit_write_RD (dat, ent->elevation);
	    bit_write_RD (dat, ent->x0);
	    bit_write_RD (dat, ent->y0);
	    if (!(ent->dataflags & 0x02))
	    {
		    bit_write_DD (dat, ent->alignment.x, 10);
		    bit_write_DD (dat, ent->alignment.y, 20);
	    }
	    bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
	    bit_write_BT (dat, ent->thickness);
	    if (!(ent->dataflags & 0x04))
		    bit_write_RD (dat, ent->oblique_ang);
	    if (!(ent->dataflags & 0x08))
		    bit_write_RD (dat, ent->rotation_ang);
	    bit_write_RD (dat, ent->height);
	    if (!(ent->dataflags & 0x10))
		    bit_write_RD (dat, ent->width_factor);
	    bit_write_T (dat, ent->text);
	    if (!(ent->dataflags & 0x20))
		    bit_write_BS (dat, ent->generation);
	    if (!(ent->dataflags & 0x40))
		    bit_write_BS (dat, ent->alignment.h);
	    if (!(ent->dataflags & 0x80))
		    bit_write_BS (dat, ent->alignment.v);
    }

	bit_write_T (dat, ent->tag);
	bit_write_BS (dat, ent->field_length);
	bit_write_RC (dat, ent->flags);

    if (dat->version >= R_2007)
    {
        bit_write_B(dat, ent->lock_position_flag);
    }
    bit_write_T (dat, ent->prompt);
}

static void
dwg_encode_BLOCK (Dwg_Entity_BLOCK *ent, Bit_Chain * dat)
{
    //TODO: check
	bit_write_T (dat, ent->name);

}

static void
dwg_encode_ENDBLK (Dwg_Entity_ENDBLK *ent, Bit_Chain * dat)
{
    //TODO: check
    //nothing to do
}

static void
dwg_encode_SEQEND (Dwg_Entity_SEQEND *ent, Bit_Chain * dat)
{
    //TODO: check
    //nothing to do
}

static void
dwg_encode_INSERT (Dwg_Entity_INSERT *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
        if (ent->scale.x == ent->scale.y == ent->scale.z == 1.0)
                bit_write_BB (dat, 3);
        else if (ent->scale.x == 1.0)
        {
                bit_write_BB (dat, 1);
                bit_write_DD (dat, ent->scale.y, 1.0);
		bit_write_DD (dat, ent->scale.z, 1.0);
        }
        else if (ent->scale.x == ent->scale.y == ent->scale.z)
        {
                bit_write_BB (dat, 2);
                bit_write_RD (dat, ent->scale.x);
        }
        else
        {
                bit_write_BB (dat, 0);
                bit_write_RD (dat, ent->scale.x);
		bit_write_DD (dat, ent->scale.y, ent->scale.x);
		bit_write_DD (dat, ent->scale.z, ent->scale.x);
        }
	bit_write_BD (dat, ent->rotation_ang);
	bit_write_BD (dat, ent->extrusion.x);
	bit_write_BD (dat, ent->extrusion.y);
	bit_write_BD (dat, ent->extrusion.z);
	bit_write_B (dat, ent->has_attribs);
}

static void
dwg_encode_MINSERT (Dwg_Entity_MINSERT *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
        if (ent->scale.x == ent->scale.y == ent->scale.z == 1.0)
                bit_write_BB (dat, 3);
        else if (ent->scale.x == 1.0)
        {
                bit_write_BB (dat, 1);
                bit_write_DD (dat, ent->scale.y, 1.0);
		bit_write_DD (dat, ent->scale.z, 1.0);
        }
        else if (ent->scale.x == ent->scale.y == ent->scale.z)
        {
                bit_write_BB (dat, 2);
                bit_write_RD (dat, ent->scale.x);
        }
        else
        {
                bit_write_BB (dat, 0);
                bit_write_RD (dat, ent->scale.x);
		bit_write_DD (dat, ent->scale.y, ent->scale.x);
		bit_write_DD (dat, ent->scale.z, ent->scale.x);
        }
	bit_write_BD (dat, ent->rotation_ang);
	bit_write_BD (dat, ent->extrusion.x);
	bit_write_BD (dat, ent->extrusion.y);
	bit_write_BD (dat, ent->extrusion.z);
	bit_write_B (dat, ent->has_attribs);
	bit_write_BS (dat, ent->column.size);
	bit_write_BS (dat, ent->line.size);
	bit_write_BD (dat, ent->column.dx);
	bit_write_BD (dat, ent->line.dy);
}

static void
dwg_encode_VERTEX_2D (Dwg_Entity_VERTEX_2D *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_RC (dat, ent->flags);
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
        if ((ent->start_width == ent->end_width) && (ent->end_width!= 0))
        {
            bit_write_BD (dat, -ent->start_width);
        }
        else
        {
            bit_write_BD (dat, ent->start_width);
            bit_write_BD (dat, ent->end_width);
        }
	bit_write_BD (dat, ent->bulge);
	bit_write_BD (dat, ent->tangent_dir);
}

static void
dwg_encode_VERTEX_3D (Dwg_Entity_VERTEX_3D *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_RC (dat, ent->flags);
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
}

static void
dwg_encode_VERTEX_MESH (Dwg_Entity_VERTEX_MESH *ent, Bit_Chain * dat)
{
    //TODO: implement-me!
}

static void
dwg_encode_VERTEX_PFACE (Dwg_Entity_VERTEX_PFACE *ent, Bit_Chain * dat)
{
    //TODO: implement-me!
}

static void
dwg_encode_VERTEX_PFACE_FACE (Dwg_Entity_VERTEX_PFACE_FACE *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_BS (dat, ent->vertind[0]);
	bit_write_BS (dat, ent->vertind[1]);
	bit_write_BS (dat, ent->vertind[2]);
	bit_write_BS (dat, ent->vertind[3]);
}

static void
dwg_encode_POLYLINE_2D (Dwg_Entity_POLYLINE_2D *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_BS (dat, ent->flags);
	bit_write_BS (dat, ent->curve_type);
	bit_write_BD (dat, ent->start_width);
	bit_write_BD (dat, ent->end_width);
	bit_write_BT (dat, ent->thickness);
	bit_write_BD (dat, ent->elevation);
	bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
}

static void
dwg_encode_POLYLINE_3D (Dwg_Entity_POLYLINE_3D *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_RC (dat, ent->flags_1);
	bit_write_RC (dat, ent->flags_2);
}

static void
dwg_encode_ARC (Dwg_Entity_ARC *ent, Bit_Chain * dat)
{
        //TODO: check
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
	bit_write_BD (dat, ent->radius);
	bit_write_BT (dat, ent->thickness);
	bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
	bit_write_BD (dat, ent->start_angle);
	bit_write_BD (dat, ent->end_angle);
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

static void
dwg_encode_LINE (Dwg_Entity_LINE * est, Bit_Chain * dat)
{
	bit_write_B (dat, est->Zs_are_zero);
	bit_write_RD (dat, est->x0);
	bit_write_DD (dat, est->x1, est->x0);
	bit_write_RD (dat, est->y0);
	bit_write_DD (dat, est->y1, est->y0);
	if (!est->Zs_are_zero)
	{
		bit_write_RD (dat, est->z0);
		bit_write_DD (dat, est->z1, est->z0);
	}
	bit_write_BT (dat, est->thickness);
	bit_write_BE (dat, est->extrusion.x, est->extrusion.y, est->extrusion.z);
}

static void
dwg_encode_DIMENSION_ORDINATE (Dwg_Entity_DIMENSION_ORDINATE *ent, Bit_Chain * dat)
{
    //TODO: check extrusion writing:
    //bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
    bit_write_RD(dat, ent->x0);
    bit_write_RD(dat, ent->y0);
    
    //TODO:review the parsing of these elevation values in the spec:
    //TODO: shouldnt we store only once this value in our data-struct?
    if (ent->elevation.ecs_11 != ent->elevation.ecs_12){
        fprintf(stderr, "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
    bit_write_BD(dat, ent->elevation.ecs_11);

    //spec: flag bit 6 indicates ORDINATE dimension
    bit_write_RC(dat, ent->flags_1);
    bit_write_T(dat, ent->user_text);
    bit_write_BD(dat, ent->text_rot);
    bit_write_BD(dat, ent->horiz_dir);
    bit_write_BD(dat, ent->ins_scale.x);
    bit_write_BD(dat, ent->ins_scale.y);
    bit_write_BD(dat, ent->ins_scale.z);
    bit_write_BD(dat, ent->ins_rotation);

    if(dat->version >= R_2000){
        bit_write_BS(dat, ent->attachment_point);
        bit_write_BS(dat, ent->lspace_style);
        bit_write_BD(dat, ent->lspace_factor);
        bit_write_BD(dat, ent->act_measurement);
    }

    if(dat->version >= R_2007){
        bit_write_B(dat, ent->unknown);
        bit_write_B(dat, ent->flip_arrow1);
        bit_write_B(dat, ent->flip_arrow2);
    }

    bit_write_RD(dat, ent->_12_pt.x);
    bit_write_RD(dat, ent->_12_pt.y);
    bit_write_RD(dat, ent->_10_pt.x);
    bit_write_RD(dat, ent->_10_pt.y);
    bit_write_RD(dat, ent->_10_pt.z);
    bit_write_RD(dat, ent->_13_pt.x);
    bit_write_RD(dat, ent->_13_pt.y);
    bit_write_RD(dat, ent->_13_pt.z);
    bit_write_RD(dat, ent->_14_pt.x);
    bit_write_RD(dat, ent->_14_pt.y);
    bit_write_RD(dat, ent->_14_pt.z);

    bit_write_RC(dat, ent->flags_2);
}

static void
dwg_encode_DIMENSION_LINEAR (Dwg_Entity_DIMENSION_LINEAR *ent, Bit_Chain * dat)
{
    //TODO: check extrusion writing:
    //bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
    bit_write_RD(dat, ent->x0);
    bit_write_RD(dat, ent->y0);
    
    //TODO:review the parsing of these elevation values in the spec:
    //TODO: shouldnt we store only once this value in our data-struct?
    if (ent->elevation.ecs_11 != ent->elevation.ecs_12){
        fprintf(stderr, "encode_DIMENSION_ORDINARY: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
    bit_write_BD(dat, ent->elevation.ecs_11);

    //spec: flag bit 6 indicates ORDINATE dimension
    bit_write_RC(dat, ent->flags);
    bit_write_T(dat, ent->user_text);
    bit_write_BD(dat, ent->text_rot);
    bit_write_BD(dat, ent->horiz_dir);
    bit_write_BD(dat, ent->ins_scale.x);
    bit_write_BD(dat, ent->ins_scale.y);
    bit_write_BD(dat, ent->ins_scale.z);
    bit_write_BD(dat, ent->ins_rotation);

    if(dat->version >= R_2000){
        bit_write_BS(dat, ent->attachment_point);
        bit_write_BS(dat, ent->lspace_style);
        bit_write_BD(dat, ent->lspace_factor);
        bit_write_BD(dat, ent->act_measurement);
    }

    if(dat->version >= R_2007){
        bit_write_B(dat, ent->unknown);
        bit_write_B(dat, ent->flip_arrow1);
        bit_write_B(dat, ent->flip_arrow2);
    }

    bit_write_RD(dat, ent->_12_pt.x);
    bit_write_RD(dat, ent->_12_pt.y);
    bit_write_BD(dat, ent->_13_pt.x);
    bit_write_BD(dat, ent->_13_pt.y);
    bit_write_BD(dat, ent->_13_pt.z);
    bit_write_BD(dat, ent->_14_pt.x);
    bit_write_BD(dat, ent->_14_pt.y);
    bit_write_BD(dat, ent->_14_pt.z);
    bit_write_BD(dat, ent->_10_pt.x);
    bit_write_BD(dat, ent->_10_pt.y);
    bit_write_BD(dat, ent->_10_pt.z);

    bit_write_BD(dat, ent->ext_line_rot);
    bit_write_BD(dat, ent->dim_rot);
}

static void
dwg_encode_DIMENSION_ALIGNED (Dwg_Entity_DIMENSION_ALIGNED *ent, Bit_Chain * dat)
{
    //TODO: check extrusion writing:
    //bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
    bit_write_RD(dat, ent->x0);
    bit_write_RD(dat, ent->y0);
    
    //TODO:review the parsing of these elevation values in the spec:
    //TODO: shouldnt we store only once this value in our data-struct?
    if (ent->elevation.ecs_11 != ent->elevation.ecs_12){
        fprintf(stderr, "encode_DIMENSION_ALIGNED: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
    bit_write_BD(dat, ent->elevation.ecs_11);

    //spec: flag bit 6 indicates ORDINATE dimension
    bit_write_RC(dat, ent->flags);
    bit_write_T(dat, ent->user_text);
    bit_write_BD(dat, ent->text_rot);
    bit_write_BD(dat, ent->horiz_dir);
    bit_write_BD(dat, ent->ins_scale.x);
    bit_write_BD(dat, ent->ins_scale.y);
    bit_write_BD(dat, ent->ins_scale.z);
    bit_write_BD(dat, ent->ins_rotation);

    if(dat->version >= R_2000){
        bit_write_BS(dat, ent->attachment_point);
        bit_write_BS(dat, ent->lspace_style);
        bit_write_BD(dat, ent->lspace_factor);
        bit_write_BD(dat, ent->act_measurement);
    }

    if(dat->version >= R_2007){
        bit_write_B(dat, ent->unknown);
        bit_write_B(dat, ent->flip_arrow1);
        bit_write_B(dat, ent->flip_arrow2);
    }

    bit_write_RD(dat, ent->_12_pt.x);
    bit_write_RD(dat, ent->_12_pt.y);
    bit_write_BD(dat, ent->_13_pt.x);
    bit_write_BD(dat, ent->_13_pt.y);
    bit_write_BD(dat, ent->_13_pt.z);
    bit_write_BD(dat, ent->_14_pt.x);
    bit_write_BD(dat, ent->_14_pt.y);
    bit_write_BD(dat, ent->_14_pt.z);
    bit_write_BD(dat, ent->_10_pt.x);
    bit_write_BD(dat, ent->_10_pt.y);
    bit_write_BD(dat, ent->_10_pt.z);

    bit_write_BD(dat, ent->ext_line_rot);
}

static void
dwg_encode_DIMENSION_ANG3PT (Dwg_Entity_DIMENSION_ANG3PT *ent, Bit_Chain * dat)
{
    //TODO: check extrusion writing:
    //bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
    bit_write_RD(dat, ent->x0);
    bit_write_RD(dat, ent->y0);
    
    //TODO:review the parsing of these elevation values in the spec:
    //TODO: shouldnt we store only once this value in our data-struct?
    if (ent->elevation.ecs_11 != ent->elevation.ecs_12){
        fprintf(stderr, "encode_DIMENSION_ALIGNED: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
    bit_write_BD(dat, ent->elevation.ecs_11);

    //spec: flag bit 6 indicates ORDINATE dimension
    bit_write_RC(dat, ent->flags);
    bit_write_T(dat, ent->user_text);
    bit_write_BD(dat, ent->text_rot);
    bit_write_BD(dat, ent->horiz_dir);
    bit_write_BD(dat, ent->ins_scale.x);
    bit_write_BD(dat, ent->ins_scale.y);
    bit_write_BD(dat, ent->ins_scale.z);
    bit_write_BD(dat, ent->ins_rotation);

    if(dat->version >= R_2000){
        bit_write_BS(dat, ent->attachment_point);
        bit_write_BS(dat, ent->lspace_style);
        bit_write_BD(dat, ent->lspace_factor);
        bit_write_BD(dat, ent->act_measurement);
    }

    if(dat->version >= R_2007){
        bit_write_B(dat, ent->unknown);
        bit_write_B(dat, ent->flip_arrow1);
        bit_write_B(dat, ent->flip_arrow2);
    }

    bit_write_RD(dat, ent->_12_pt.x);
    bit_write_RD(dat, ent->_12_pt.y);
    bit_write_BD(dat, ent->_10_pt.x);
    bit_write_BD(dat, ent->_10_pt.y);
    bit_write_BD(dat, ent->_10_pt.z);
    bit_write_BD(dat, ent->_13_pt.x);
    bit_write_BD(dat, ent->_13_pt.y);
    bit_write_BD(dat, ent->_13_pt.z);
    bit_write_BD(dat, ent->_14_pt.x);
    bit_write_BD(dat, ent->_14_pt.y);
    bit_write_BD(dat, ent->_14_pt.z);
    bit_write_BD(dat, ent->_15_pt.x);
    bit_write_BD(dat, ent->_15_pt.y);
    bit_write_BD(dat, ent->_15_pt.z);
}

static void
dwg_encode_DIMENSION_ANG2LN (Dwg_Entity_DIMENSION_ANG2LN *ent, Bit_Chain * dat)
{
    //TODO: check extrusion writing:
    //bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
    bit_write_RD(dat, ent->x0);
    bit_write_RD(dat, ent->y0);
    
    //TODO:review the parsing of these elevation values in the spec:
    //TODO: shouldnt we store only once this value in our data-struct?
    if (ent->elevation.ecs_11 != ent->elevation.ecs_12){
        fprintf(stderr, "encode_DIMENSION_ALIGNED: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
    bit_write_BD(dat, ent->elevation.ecs_11);

    //spec: flag bit 6 indicates ORDINATE dimension
    bit_write_RC(dat, ent->flags);
    bit_write_T(dat, ent->user_text);
    bit_write_BD(dat, ent->text_rot);
    bit_write_BD(dat, ent->horiz_dir);
    bit_write_BD(dat, ent->ins_scale.x);
    bit_write_BD(dat, ent->ins_scale.y);
    bit_write_BD(dat, ent->ins_scale.z);
    bit_write_BD(dat, ent->ins_rotation);

    if(dat->version >= R_2000){
        bit_write_BS(dat, ent->attachment_point);
        bit_write_BS(dat, ent->lspace_style);
        bit_write_BD(dat, ent->lspace_factor);
        bit_write_BD(dat, ent->act_measurement);
    }

    if(dat->version >= R_2007){
        bit_write_B(dat, ent->unknown);
        bit_write_B(dat, ent->flip_arrow1);
        bit_write_B(dat, ent->flip_arrow2);
    }

    bit_write_RD(dat, ent->_12_pt.x);
    bit_write_RD(dat, ent->_12_pt.y);
    bit_write_RD(dat, ent->_16_pt.x);
    bit_write_RD(dat, ent->_16_pt.y);
    bit_write_BD(dat, ent->_13_pt.x);
    bit_write_BD(dat, ent->_13_pt.y);
    bit_write_BD(dat, ent->_13_pt.z);
    bit_write_BD(dat, ent->_14_pt.x);
    bit_write_BD(dat, ent->_14_pt.y);
    bit_write_BD(dat, ent->_14_pt.z);
    bit_write_BD(dat, ent->_15_pt.x);
    bit_write_BD(dat, ent->_15_pt.y);
    bit_write_BD(dat, ent->_15_pt.z);
    bit_write_BD(dat, ent->_10_pt.x);
    bit_write_BD(dat, ent->_10_pt.y);
    bit_write_BD(dat, ent->_10_pt.z);
}

static void
dwg_encode_DIMENSION_RADIUS (Dwg_Entity_DIMENSION_RADIUS *ent, Bit_Chain * dat)
{
    //TODO: check extrusion writing:
    //bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
    bit_write_RD(dat, ent->x0);
    bit_write_RD(dat, ent->y0);
    
    //TODO:review the parsing of these elevation values in the spec:
    //TODO: shouldnt we store only once this value in our data-struct?
    if (ent->elevation.ecs_11 != ent->elevation.ecs_12){
        fprintf(stderr, "encode_DIMENSION_ALIGNED: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
    bit_write_BD(dat, ent->elevation.ecs_11); //Spec-typo? It says: D instead of BD...

    //spec: flag bit 6 indicates ORDINATE dimension
    bit_write_RC(dat, ent->flags);//Spec-typo? It says: EC instead of RC...
    bit_write_T(dat, ent->user_text);

//Spec-typo? For all these values the spec says: D instead of BD... (could eventually be RD)
    bit_write_BD(dat, ent->text_rot);
    bit_write_BD(dat, ent->horiz_dir);
    bit_write_BD(dat, ent->ins_scale.x);
    bit_write_BD(dat, ent->ins_scale.y);
    bit_write_BD(dat, ent->ins_scale.z);
    bit_write_BD(dat, ent->ins_rotation);

    if(dat->version >= R_2000){
        bit_write_BS(dat, ent->attachment_point);
        bit_write_BS(dat, ent->lspace_style);
        bit_write_BD(dat, ent->lspace_factor);
        bit_write_BD(dat, ent->act_measurement);
    }

    if(dat->version >= R_2007){
        bit_write_B(dat, ent->unknown);
        bit_write_B(dat, ent->flip_arrow1);
        bit_write_B(dat, ent->flip_arrow2);
    }

    bit_write_RD(dat, ent->_12_pt.x);
    bit_write_RD(dat, ent->_12_pt.y);
    bit_write_BD(dat, ent->_10_pt.x);
    bit_write_BD(dat, ent->_10_pt.y);
    bit_write_BD(dat, ent->_10_pt.z);
    bit_write_BD(dat, ent->_15_pt.x);
    bit_write_BD(dat, ent->_15_pt.y);
    bit_write_BD(dat, ent->_15_pt.z);

    bit_write_BD(dat, ent->leader_len);
}

static void
dwg_encode_DIMENSION_DIAMETER (Dwg_Entity_DIMENSION_DIAMETER *ent, Bit_Chain * dat)
{
    //TODO: check extrusion writing:
    //bit_write_BE (dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
    bit_write_RD(dat, ent->x0);
    bit_write_RD(dat, ent->y0);
    
    //TODO:review the parsing of these elevation values in the spec:
    //TODO: shouldnt we store only once this value in our data-struct?
    if (ent->elevation.ecs_11 != ent->elevation.ecs_12){
        fprintf(stderr, "encode_DIMENSION_ALIGNED: Maybe there is something wrong here. Elevation values should be all the same.\n");
    }
    bit_write_BD(dat, ent->elevation.ecs_11);

    //spec: flag bit 6 indicates ORDINATE dimension
    bit_write_RC(dat, ent->flags);

    bit_write_T(dat, ent->user_text);
    bit_write_BD(dat, ent->text_rot);
    bit_write_BD(dat, ent->horiz_dir);
    bit_write_BD(dat, ent->ins_scale.x);
    bit_write_BD(dat, ent->ins_scale.y);
    bit_write_BD(dat, ent->ins_scale.z);
    bit_write_BD(dat, ent->ins_rotation);

    if(dat->version >= R_2000){
        bit_write_BS(dat, ent->attachment_point);
        bit_write_BS(dat, ent->lspace_style);
        bit_write_BD(dat, ent->lspace_factor);
        bit_write_BD(dat, ent->act_measurement);
    }

    if(dat->version >= R_2007){
        bit_write_B(dat, ent->unknown);
        bit_write_B(dat, ent->flip_arrow1);
        bit_write_B(dat, ent->flip_arrow2);
    }

    bit_write_RD(dat, ent->_12_pt.x);
    bit_write_RD(dat, ent->_12_pt.y);
    bit_write_BD(dat, ent->_15_pt.x);
    bit_write_BD(dat, ent->_15_pt.y);
    bit_write_BD(dat, ent->_15_pt.z);
    bit_write_BD(dat, ent->_10_pt.x);
    bit_write_BD(dat, ent->_10_pt.y);
    bit_write_BD(dat, ent->_10_pt.z);

    bit_write_BD(dat, ent->leader_len);
}

static void
dwg_encode_POINT (Dwg_Entity_POINT *ent, Bit_Chain * dat)
{
    bit_write_BD(dat, ent->x);
    bit_write_BD(dat, ent->y);
    bit_write_BD(dat, ent->z);
    bit_write_BT(dat, ent->thickness);
    bit_write_BE(dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
    bit_write_BD(dat, ent->x_ang);
}

static void
dwg_encode_3DFACE (Dwg_Entity_3D_FACE *ent, Bit_Chain * dat)
{
	if (dat->version == R_13 || dat->version == R_14){
		bit_write_BD(dat, ent->corner1.x);
		bit_write_BD(dat, ent->corner1.y);
		bit_write_BD(dat, ent->corner1.z);
		bit_write_BD(dat, ent->corner2.x);
		bit_write_BD(dat, ent->corner2.y);
		bit_write_BD(dat, ent->corner2.z);
		bit_write_BD(dat, ent->corner3.x);
		bit_write_BD(dat, ent->corner3.y);
		bit_write_BD(dat, ent->corner3.z);
		bit_write_BD(dat, ent->corner4.x);
		bit_write_BD(dat, ent->corner4.y);
		bit_write_BD(dat, ent->corner4.z);
		bit_write_BS(dat, ent->invis_flags);
	}

	if (dat->version >= R_2000){
		bit_write_B(dat, ent->invis_flags);
		bit_write_B(dat, ent->z_is_zero);
		bit_write_RD(dat, ent->corner1.x);
		bit_write_RD(dat, ent->corner1.y);
		if (ent->z_is_zero)
			bit_write_RD(dat, ent->corner1.z);
		bit_write_DD(dat, ent->corner2.x, ent->corner1.x);
		bit_write_DD(dat, ent->corner2.y, ent->corner1.y);
		bit_write_DD(dat, ent->corner2.z, ent->corner1.z);
		bit_write_DD(dat, ent->corner3.x, ent->corner2.x);
		bit_write_DD(dat, ent->corner3.y, ent->corner2.y);
		bit_write_DD(dat, ent->corner3.z, ent->corner2.z);
		bit_write_DD(dat, ent->corner4.x, ent->corner3.x);
		bit_write_DD(dat, ent->corner4.y, ent->corner3.y);
		bit_write_DD(dat, ent->corner4.z, ent->corner3.z);
		bit_write_BS(dat, ent->invis_flags);
	}

}

static void
dwg_encode_POLYLINE_PFACE (Dwg_Entity_POLYLINE_PFACE* ent, Bit_Chain * dat)
{
        bit_write_BS(dat, ent->numverts);
        bit_write_BS(dat, ent->numfaces);

        if (dat->version >= R_2004)
        {
            bit_write_BL(dat, ent->owned_object_count);
        }
        //TODO: what about the handles?
}

static void
dwg_encode_POLYLINE_MESH (Dwg_Entity_POLYLINE_MESH *ent, Bit_Chain * dat)
{
        bit_write_BS(dat, ent->flags);
        bit_write_BS(dat, ent->curve_type);
        bit_write_BS(dat, ent->m_vert_count);
        bit_write_BS(dat, ent->n_vert_count);
        bit_write_BS(dat, ent->m_density);
        bit_write_BS(dat, ent->n_density);

        if (dat->version >= R_2004)
        {
            bit_write_BL(dat, ent->owned_object_count);
        }
}

static void
dwg_encode_SOLID (Dwg_Entity_SOLID *ent, Bit_Chain * dat)
{
	bit_write_BT (dat, ent->thickness);
    //TODO: shouldn't we store only env->elevation instead of storing the same value 4 times?
    if (ent->corner1.z != ent->corner2.z ||
        ent->corner1.z != ent->corner3.z ||
        ent->corner1.z != ent->corner4.z){
        fprintf(stderr, "warning: dwg_encode_SOLID: There is something wrong here. Z coordinate for the 4 corners should be equal (elevation value).\n");
    }

    //elevation:
	bit_write_BD(dat, ent->corner1.z);

	bit_write_RD(dat, ent->corner1.x);
	bit_write_RD(dat, ent->corner1.y);
    bit_write_RD(dat, ent->corner2.x);
	bit_write_RD(dat, ent->corner2.y);
    bit_write_RD(dat, ent->corner3.x);
	bit_write_RD(dat, ent->corner3.y);
    bit_write_RD(dat, ent->corner4.x);
	bit_write_RD(dat, ent->corner4.y);
    bit_write_BE(dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
}

static void
dwg_encode_TRACE (Dwg_Entity_TRACE *ent, Bit_Chain * dat)
{
	bit_write_BT (dat, ent->thickness);
    //TODO: shouldn't we store only env->elevation instead of storing the same value 4 times?
    if (ent->corner1.z != ent->corner2.z ||
        ent->corner1.z != ent->corner3.z ||
        ent->corner1.z != ent->corner4.z){
        fprintf(stderr, "warning: dwg_encode_TRACE: There is something wrong here. Z coordinate for the 4 corners should be equal (elevation value).\n");
    }

    //elevation:
	bit_write_BD(dat, ent->corner1.z);

	bit_write_RD(dat, ent->corner1.x);
	bit_write_RD(dat, ent->corner1.y);
    bit_write_RD(dat, ent->corner2.x);
	bit_write_RD(dat, ent->corner2.y);
    bit_write_RD(dat, ent->corner3.x);
	bit_write_RD(dat, ent->corner3.y);
    bit_write_RD(dat, ent->corner4.x);
	bit_write_RD(dat, ent->corner4.y);
    bit_write_BE(dat, ent->extrusion.x, ent->extrusion.y, ent->extrusion.z);
}

static void
dwg_encode_SHAPE (Dwg_Entity_SHAPE *ent, Bit_Chain * dat)
{
    bit_write_BD(dat, ent->ins_pt.x);
    bit_write_BD(dat, ent->ins_pt.y);
    bit_write_BD(dat, ent->ins_pt.z);
    bit_write_BD(dat, ent->scale);
    bit_write_BD(dat, ent->rotation);
    bit_write_BD(dat, ent->width_factor);
    bit_write_BD(dat, ent->oblique);
    bit_write_BD(dat, ent->thickness);
    bit_write_BS(dat, ent->shape_no);
    bit_write_BD(dat, ent->extrusion.x);
    bit_write_BD(dat, ent->extrusion.y);
    bit_write_BD(dat, ent->extrusion.z);
}

static void
dwg_encode_VIEWPORT (Dwg_Entity_VIEWPORT *ent, Bit_Chain * dat)
{
    //TODO: implement-me!
}

static void
dwg_encode_ELLIPSE (Dwg_Entity_ELLIPSE *ent, Bit_Chain * dat)
{
    //TODO: implement-me!
}

static void
dwg_encode_SPLINE (Dwg_Entity_SPLINE * ent, Bit_Chain * dat)
{
        //TODO: check
        int i;

        bit_write_BS(dat, ent->scenario);
        bit_write_BS(dat, ent->degree);
        if(ent->scenario == 2)
        {
            bit_write_BD(dat, ent->fit_tol);
            bit_write_BD(dat, ent->beg_tan_vec.x);
            bit_write_BD(dat, ent->beg_tan_vec.y);
            bit_write_BD(dat, ent->beg_tan_vec.z);
            bit_write_BD(dat, ent->end_tan_vec.x);
            bit_write_BD(dat, ent->end_tan_vec.y);
            bit_write_BD(dat, ent->end_tan_vec.z);
            bit_write_BS(dat, ent->num_fit_pts);
            for (i=0;i<ent->num_fit_pts;i++)
            {
                bit_write_BD(dat,ent->fit_pts[i].x);
                bit_write_BD(dat, ent->fit_pts[i].y);
                bit_write_BD(dat, ent->fit_pts[i].z);
            }
        } else
        {
            if (ent->scenario == 1)
            {
                    bit_write_B(dat, ent->rational);
                    bit_write_B(dat, ent->closed_b);
                    bit_write_B(dat, ent->periodic);
                    bit_write_BD(dat, ent->knot_tol);
                    bit_write_BD(dat,ent->ctrl_tol);
                    bit_write_BL(dat, ent->num_knots);
                    bit_write_BL(dat, ent->num_ctrl_pts);
                    bit_write_B(dat, ent->weighted);

                    for (i=0;i<ent->num_knots;i++)
                        bit_write_BD(dat, ent->knots[i].value);

                    for (i=0;i<ent->num_ctrl_pts;i++)
                    {
                            bit_write_BD(dat, ent->ctrl_pts[i].x);
                            bit_write_BD(dat, ent->ctrl_pts[i].y);
                            bit_write_BD(dat, ent->ctrl_pts[i].z);
                            if (ent->weighted)
                                //TODO check what "D" means on spec.
                                //assuming typo - should be BD
                                bit_write_BD(dat, ent->ctrl_pts[i].w);
                    }
            } else
            {
                 fprintf (stderr, "Error: unknown scenario %d", ent->scenario);
            }
        }
}

static void
dwg_encode_RAY (Dwg_Entity_RAY * ent, Bit_Chain * dat)
{
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
	bit_write_BD (dat, ent->x1);
	bit_write_BD (dat, ent->y1);
	bit_write_BD (dat, ent->z1);
}

static void
dwg_encode_XLINE (Dwg_Entity_XLINE *ent, Bit_Chain * dat)
{
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
	bit_write_BD (dat, ent->x1);
	bit_write_BD (dat, ent->y1);
	bit_write_BD (dat, ent->z1);
}

static void
dwg_encode_MTEXT (Dwg_Entity_MTEXT *ent, Bit_Chain * dat)
{
    //spec-typo ? Spec says BD but we think it might be 3BD:
	bit_write_BD (dat, ent->x0);
	bit_write_BD (dat, ent->y0);
	bit_write_BD (dat, ent->z0);
	bit_write_BD (dat, ent->extrusion.x);
	bit_write_BD (dat, ent->extrusion.y);
	bit_write_BD (dat, ent->extrusion.z);
	bit_write_BD (dat, ent->x1);
	bit_write_BD (dat, ent->y1);
	bit_write_BD (dat, ent->z1);

    if (dat->version >= R_2007){
    	bit_write_BD (dat, ent->rect_height);
    }

	bit_write_BD (dat, ent->rect_width);
	bit_write_BD (dat, ent->text_height);
	bit_write_BS (dat, ent->attachment);
	bit_write_BS (dat, ent->drawing_dir);
	bit_write_BD (dat, ent->extends); //not documented
	bit_write_BD (dat, ent->extends_wid);
	bit_write_T (dat, ent->text);

    if (dat->version >= R_2000){
    	bit_write_BS (dat, ent->linespace_style);
    	bit_write_BD (dat, ent->linespace_factor);
    	bit_write_B (dat, ent->unknown_bit);
    }	

    if (dat->version >= R_2004){
    	bit_write_BL (dat, ent->unknown_long);
    }
}

static void
dwg_encode_DICTIONARY (Dwg_Object_DICTIONARY *ent, Bit_Chain * dat)
{
    //Implement-me!
}

static void dwg_encode_BLOCK_CONTROL (Dwg_Object_BLOCK_CONTROL * obj, Bit_Chain * dat)
{
    //Implement-me!
}

static void dwg_encode_LAYER (Dwg_Object_LAYER * obj, Bit_Chain * dat)
{
    //Implement-me!
}


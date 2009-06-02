#include "print.h"
#include <stdio.h>

/*------------------------------------------------------------------------------
 * Private functions
 */

void
dwg_print_entity (Dwg_Object_Entity * ent)
{
	printf ("Bitsize: %lu\n", ent->bitsize);
	printf ("Vera traktilo: %i.%i.%lu\n", ent->traktilo.code, ent->traktilo.size,
		ent->traktilo.value);
	printf ("Kroma datenaro: %lu B\n", (long unsigned int) ent->kromdat_size);
	printf ("Ĉu picture?: %s", ent->picture_ekzistas ? "Yes" : "Ne");
	if (ent->picture_ekzistas)
		printf ("\tSize: %lu B\n", ent->picture_size);
	else
		puts ("");
	printf ("Reĝimo: %i\n", ent->regime);
	printf ("Kiom reagiloj: %lu\n", ent->reagilo_size);
	printf ("Ĉu senligiloj?: %s\n", ent->senligiloj ? "Yes" : "Ne");
	printf ("Koloro: %u\n", ent->colour);
	printf ("Skalo de linitype: %1.13g\n", ent->linitypeskalo);
	printf ("Linitype: 0x%02X\n", ent->linitype);
	printf ("Printstilo: 0x%02X\n", ent->printstilo);
	printf ("Malvidebleco: 0x%04X\n", ent->malvidebleco);
	printf ("Linithickness: %u\n", ent->linithickness);
}

void
dwg_print_object (Dwg_Object_Object *ord)
{
	printf ("Bitsize: %lu\n", ord->bitsize);
	printf ("Vera traktilo: %i.%i.%lu\n", ord->traktilo.code, ord->traktilo.size,
		ord->traktilo.value);
	printf ("Kroma datenaro: %lu B\n", (long unsigned int) ord->kromdat_size);
	printf ("Kiom reagiloj: %lu\n", ord->reagilo_size);
}


void
dwg_print_traktref (Dwg_Object * obj)
{
	unsigned int i;

	if (obj->supertype == DWG_SUPERTYPE_ENTITY)
	{
		Dwg_Object_Entity *ent;

		ent = obj->tio.entity;

		printf ("\tTraktil-referencoj (%u): ", ent->traktref_size);
		if (ent->traktref_size == 0)
		{
			puts ("");
			return;
		}
		for (i = 0; i < ent->traktref_size - 1; i++)
			printf ("%i.%i.%li / ", ent->traktref[i].code, ent->traktref[i].size,
				ent->traktref[i].value);
		printf ("%i.%i.%li\n", ent->traktref[i].code, ent->traktref[i].size,
			ent->traktref[i].value);
	}
	else if (obj->supertype == DWG_SUPERTYPE_OBJECT)
	{
		Dwg_Object_Object *ord;

		ord = obj->tio.object;

		printf ("\tTraktil-referencoj (%u): ", ord->traktref_size);
		if (ord->traktref_size == 0)
		{
			puts ("");
			return;
		}
		for (i = 0; i < ord->traktref_size - 1; i++)
			printf ("%i.%i.%li / ", ord->traktref[i].code, ord->traktref[i].size,
				ord->traktref[i].value);
		printf ("%i.%i.%li\n", ord->traktref[i].code, ord->traktref[i].size,
			ord->traktref[i].value);
	}
}

/* OBJECTS *******************************************************************/

void
dwg_print_TEXT (Dwg_Entity_TEXT * ent)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", ent->datumindik);

	printf ("\tLeviĝo: %1.13g\n", ent->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", ent->gxisrandigo.x, ent->gxisrandigo.y);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tKlina angulo: %1.13g\n", ent->klinang);
	printf ("\tTurna angulo: %1.13g\n", ent->turnang);
	printf ("\tAlteco: %1.13g\n", ent->height);
	printf ("\tLarĝ-faktoro: %1.13g\n", ent->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", ent->text);
	printf ("\tGeneracio: %u\n", ent->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", ent->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", ent->gxisrandigo.v);
}

void
dwg_print_ATTRIB (Dwg_Entity_ATTRIB * ent)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", ent->datumindik);

	printf ("\tLeviĝo: %1.13g\n", ent->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", ent->gxisrandigo.x, ent->gxisrandigo.y);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tKlina angulo: %1.13g\n", ent->klinang);
	printf ("\tTurna angulo: %1.13g\n", ent->turnang);
	printf ("\tAlteco: %1.13g\n", ent->height);
	printf ("\tLarĝ-faktoro: %1.13g\n", ent->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", ent->text);
	printf ("\tGeneracio: %u\n", ent->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", ent->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", ent->gxisrandigo.v);
	printf ("\tEtikedo: %s\n", ent->etikedo);
	printf ("\tKamp-length: %i (ne uzata)\n", ent->kamplong);
	printf ("\tIndikiloj: 0x%02x\n", ent->indikiloj);
}

void
dwg_print_ATTDEF (Dwg_Entity_ATTDEF * ent)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", ent->datumindik);

	printf ("\tLeviĝo: %1.13g\n", ent->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", ent->gxisrandigo.x, ent->gxisrandigo.y);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tKlina angulo: %1.13g\n", ent->klinang);
	printf ("\tTurna angulo: %1.13g\n", ent->turnang);
	printf ("\tAlteco: %1.13g\n", ent->height);
	printf ("\tLarĝ-faktoro: %1.13g\n", ent->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", ent->text);
	printf ("\tGeneracio: %u\n", ent->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", ent->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", ent->gxisrandigo.v);
	printf ("\tEtikedo: %s\n", ent->etikedo);
	printf ("\tKamp-length: %i (ne uzata)\n", ent->kamplong);
	printf ("\tIndikilo: 0x%02x\n", ent->indikiloj);
	printf ("\tInvitilo (prompt): %s\n", ent->invitilo);
}

void
dwg_print_BLOCK (Dwg_Entity_BLOCK * ent)
{
	printf ("\tName: %s\n", ent->name);
}

void
dwg_print_INSERT (Dwg_Entity_INSERT * ent)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", ent->skalindik);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", ent->skalo.x, ent->skalo.y, ent->skalo.z);
	printf ("\tTurna angulo: %1.13g\n", ent->turnang);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", ent->kun_attrib ? "Yes" : "Ne");
}

void
dwg_print_MINSERT (Dwg_Entity_MINSERT * ent)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", ent->skalindik);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", ent->skalo.x, ent->skalo.y, ent->skalo.z);
	printf ("\tTurna angulo: %1.13g\n", ent->turnang);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", ent->kun_attrib ? "Yes" : "Ne");
	printf ("\tKolumnoj: %02i\tInterspaco: %1.13g\n", ent->kol.size, ent->kol.dx);
	printf ("\t  Linioj: %02i\tInterspaco: %1.13g\n", ent->lin.size, ent->lin.dy);
}

void
dwg_print_VERTEX_2D (Dwg_Entity_VERTEX_2D * ent)
{
	printf ("\tIndikiloj: 0x%02x\n", ent->indikiloj);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tEklarĝo: %1.13g\n", ent->eklargxo);
	printf ("\tFinlarĝo: %1.13g\n", ent->finlargxo);
	printf ("\tProtuberanco: %1.13g\n", ent->protub);
	printf ("\tTanĝento dir.: %1.13g\n", ent->tangxdir);
}

void
dwg_print_VERTEX_3D (Dwg_Entity_VERTEX_3D * ent)
{
	printf ("\tIndikiloj: 0x%02x\n", ent->indikiloj);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
}

void
dwg_print_VERTEX_PFACE_FACE (Dwg_Entity_VERTEX_PFACE_FACE * ent)
{
	printf ("\tVertic-index 1: %i\n", ent->vertind[0]);
	printf ("\tVertic-index 2: %i\n", ent->vertind[1]);
	printf ("\tVertic-index 3: %i\n", ent->vertind[2]);
	printf ("\tVertic-index 4: %i\n", ent->vertind[3]);
}

void
dwg_print_POLYLINE_2D (Dwg_Entity_POLYLINE_2D * ent)
{
	printf ("\tIndikiloj: 0x%02x\n", ent->indikiloj);
	printf ("\tKurbtype: 0x%02x\n", ent->kurbtype);
	printf ("\tEklarĝo: %1.13g\n", ent->eklargxo);
	printf ("\tFinlarĝo: %1.13g\n", ent->finlargxo);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tLeviĝo: %1.13g\n", ent->levigxo);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
}

void
dwg_print_POLYLINE_3D (Dwg_Entity_POLYLINE_3D * ent)
{
	printf ("\tIndikiloj: 0x%02x / 0x%02x\n", ent->indikiloj_1, ent->indikiloj_2);
}

void
dwg_print_ARC (Dwg_Entity_ARC * ent)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tRadiuso: %1.13g\n", ent->radius);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tEkangulo: %1.13g\n", ent->ekangulo);
	printf ("\tFinangulo: %1.13g\n", ent->finangulo);
}

void
dwg_print_CIRCLE (Dwg_Entity_CIRCLE * ent)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tRadiuso: %1.13g\n", ent->radius);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
}

void
dwg_print_LINE (Dwg_Entity_LINE * ent)
{
	printf ("\tĈu nur 2D?: %s\n", ent->nur_2D ? "Yes" : "Ne");
	printf ("\t1-a punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0,
		ent->nur_2D ? 0 : ent->z0);
	printf ("\t2-a punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1,
		ent->nur_2D ? 0 : ent->z1);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
}

void
dwg_print_POINT (Dwg_Entity_POINT *ent)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tX-angulo: %1.13g\n", ent->x_ang);
}

void
dwg_print_ELLIPSE (Dwg_Entity_ELLIPSE *ent)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tDuonĉef-aksa vektoro: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tRadius-proporcio: %1.13g\n", ent->radiusproporcio);
	printf ("\tEkangulo: %1.13g\n", ent->ekangulo);
	printf ("\tFinangulo: %1.13g\n", ent->finangulo);
}

void
dwg_print_RAY (Dwg_Entity_RAY *ent)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tVektoro: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
}

void
dwg_print_MTEXT (Dwg_Entity_MTEXT *ent)
{
	printf ("\tEnmeto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tX-direkto: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
	printf ("\tLargxeco: %1.13g\n", ent->largxeco);
	printf ("\tAlteco: %1.13g\n", ent->height);
	printf ("\tKunmeto: 0x%0x\n", ent->kunmeto);
	printf ("\tDirekto: 0x%0x\n", ent->direkto);
	printf ("\tTeksto: %s\n", ent->text);
	printf ("\tLinispaca stilo: 0x%0x\n", ent->linispaco_stilo);
	printf ("\tLinispaca faktoro: %1.13g\n", (double) ent->linispaco_faktoro);
}

void
dwg_print_LAYER (Dwg_Object_LAYER *ord)
{
	printf ("\tName: %s\n", ord->name);
	printf ("\tĈu 64?: %s\n", ord->bit64 ? "Yes" : "Ne");
	printf ("\tIndico Xref: %u\n", ord->xrefi);
	printf ("\tXref-dependa?: %s\n", ord->xrefdep ? "Yes" : "Ne");
	printf ("\tEcoj: 0x%0x\n", ord->ecoj);
	printf ("\tKoloro: %u\n", ord->colour);
}

void
dwg_print_LAYOUT (Dwg_Object_LAYOUT *ord)
{
	printf ("\tAgord-name: %s\n", ord->pagxo.agordo);
	printf ("\tPrintilo: %s\n", ord->pagxo.printilo);
	printf ("\tPrint-indikiloj: 0x%0x\n", ord->pagxo.indikiloj);
	printf ("\tMarĝenoj. maldekstre: %1.13g; malsupre: %1.13g; dekstre: %1.13g; supre: %1.13g\n",
		ord->pagxo.maldekstre,
		ord->pagxo.malsupre,
		ord->pagxo.dekstre,
		ord->pagxo.supre);
	printf ("\tLargxeco: %1.13g\n", ord->pagxo.largxeco);
	printf ("\tAlteco: %1.13g\n", ord->pagxo.height);
	printf ("\tSize: %s\n", ord->pagxo.size);
	printf ("\tDeŝovo: (%1.13g, %1.13g)\n", ord->pagxo.dx, ord->pagxo.dy);
	printf ("\tUnuoj: %u\n", ord->pagxo.unuoj);
	printf ("\tRotacio: %u\n", ord->pagxo.rotacio);
	printf ("\tType: %u\n", ord->pagxo.type);
	printf ("\tPrint-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n",
		ord->pagxo.x_min,
		ord->pagxo.y_min,
		ord->pagxo.x_maks,
		ord->pagxo.y_maks);
	printf ("\tPaĝ-name: %s\n", ord->pagxo.name);
	printf ("\tSkal-proporcio: %u:%u\n", (unsigned int) ord->pagxo.skalo.A, (unsigned int) ord->pagxo.skalo.B);
	printf ("\tStilfolio: %s\n", ord->pagxo.stilfolio);
	printf ("\tSkal-type: %u\n", ord->pagxo.skalo.type);
	printf ("\tSkal-faktoro: %u\n", (unsigned int) ord->pagxo.skalo.faktoro);
	printf ("\tPaĝ-origino: (%1.13g, %1.13g)\n", ord->pagxo.x0, ord->pagxo.y0);
	puts ("");
	printf ("\tAranĝ-name: %s\n", ord->name);
	printf ("\tOrdo: %u\n", ord->ordo);
	printf ("\tIndikiloj: 0x%0x\n", ord->indikiloj);
	printf ("\tUCS-origino: (%1.13g, %1.13g, %1.13g)\n", ord->x0, ord->y0, ord->z0);
	printf ("\tAranĝ-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n",
		ord->x_min,
		ord->y_min,
		ord->x_maks,
		ord->y_maks);
	printf ("\tEnmeto: (%1.13g, %1.13g, %1.13g)\n", ord->enmeto.x0, ord->enmeto.y0, ord->enmeto.z0);
	printf ("\tUCS-akso_X: (%1.13g, %1.13g, %1.13g)\n", ord->akso_X.x0, ord->akso_X.y0, ord->akso_X.z0);
	printf ("\tUCS-akso_Y: (%1.13g, %1.13g, %1.13g)\n", ord->akso_Y.x0, ord->akso_Y.y0, ord->akso_Y.z0);
	printf ("\tLevigxo: %1.13g\n", ord->levigxo);
	printf ("\tRigard-type: %u\n", ord->rigardtype);
	printf ("\tMinimumo: (%1.13g, %1.13g, %1.13g)\n",
		ord->limo.x_min,
		ord->limo.y_min,
		ord->limo.z_min);
	printf ("\tMaksimumo: (%1.13g, %1.13g, %1.13g)\n",
		ord->limo.x_maks,
		ord->limo.y_maks,
		ord->limo.z_maks);
}

/*------------------------------------------------------------------------------
 * Special public function for printing values of an object
 */
void
dwg_print (Dwg_Structure *dwg_struct)
{
	unsigned char sig;
	unsigned int i, j;
	const char *dwg_obtype[81] = {
	"UNUSED", "TEXT", "ATTRIB", "ATTDEF",	"BLOCK",
       	"ENDBLK", "SEQEND", "INSERT", "MINSERT", "NULL_09",
	"VERTEX_2D", "VERTEX_3D", "VERTEX_MESH", "VERTEX_PFACE", "VERTEX_PFACE_FACE",
       	"POLYLINE_2D", "POLYLINE_3D", "ARC", "CIRCLE", "LINE",
	"20", "21", "22", "23", "24", "25", "26", "POINT", "28", "29",
       	"30", "31", "32", "33",	"34", "ELLIPSE", "36", "37", "38", "39",
       	"RAY", "XLINE", "42", "43", "MTEXT", "45", "46", "47", "48", "49",
       	"50", "LAYER", "52", "53", "54", "55", "56", "57", "58", "59",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
	"80"};


	puts ("**************************************************");
	puts ("Section HEADER");
	puts ("**************************************************");
	printf ("Version: %s\n", version_codes[dwg_struct->header.version]);
	printf ("Codepage: %u\n", dwg_struct->header.codepage);
	for (i = 0; i < dwg_struct->header.num_sections; i++)
		printf ("Section %i\t Address: %7lu\t Size: %7lu B\n",
			dwg_struct->header.section[i].number,
			dwg_struct->header.section[i].address, dwg_struct->header.section[i].size);
	puts ("");

	if (dwg_struct->header.num_sections == 6)
	{
		puts ("**************************************************");
		puts ("Section UNKNOWN 1");
		puts ("**************************************************");
		printf ("Size: %lu B\n", dwg_struct->unknown1.size);
		bit_print ((Bit_Chain *) & dwg_struct->unknown1, dwg_struct->unknown1.size);
		puts ("");
	}

	puts ("**************************************************");
	puts ("Section PICTURE");
	puts ("**************************************************");
	printf ("Size: %lu B\n", dwg_struct->picture.size);
	//bit_print ((Bit_Chain *) &dwg_struct->picture, dwg_struct->picture.size);
	puts ("");

	puts ("**************************************************");
	puts ("Section VARIABLES");
	puts ("**************************************************");
	for (i = 0; i < DWG_NUM_VARIABLES; i++)
	{
		printf ("[%03i] - ", i + 1);
		if (i == 221 && dwg_struct->var[220].dubitoko != 3)
		{
			puts ("(Non-Existant)");
			continue;
		}
		switch (dwg_var_map (dwg_struct->header.version, i))
		{
		case DWG_DT_B:
			printf ("B: %u", dwg_struct->var[i].bitoko);
			break;
		case DWG_DT_BS:
			printf ("BS: %u", dwg_struct->var[i].dubitoko);
			break;
		case DWG_DT_BL:
			printf ("BL: %lu", dwg_struct->var[i].kvarbitoko);
			break;
		case DWG_DT_BD:
			printf ("BD: %lg", dwg_struct->var[i].duglitajxo);
			break;
		case DWG_DT_H:
			printf ("H: %i.%i.%li", dwg_struct->var[i].traktilo.code,
				dwg_struct->var[i].traktilo.size, dwg_struct->var[i].traktilo.value);
			break;
		case DWG_DT_T:
			printf ("T: \"%s\"", dwg_struct->var[i].text);
			break;
		case DWG_DT_CMC:
			printf ("CMC: %u", dwg_struct->var[i].dubitoko);
			break;
		case DWG_DT_2RD:
			printf ("X: %lg\t", dwg_struct->var[i].xy[0]);
			printf ("Y: %lg", dwg_struct->var[i].xy[1]);
			break;
		case DWG_DT_3BD:
			printf ("X: %lg\t", dwg_struct->var[i].xyz[0]);
			printf ("Y: %lg\t", dwg_struct->var[i].xyz[1]);
			printf ("Z: %lg", dwg_struct->var[i].xyz[2]);
			break;
		default:
			printf ("Ne traktebla type: %i (var: %i)\n", dwg_var_map (dwg_struct->header.version, i), i);
		}
		puts ("");
	}
	puts ("");

	puts ("**************************************************");
	puts ("Section CLASSES");
	puts ("**************************************************");
	for (i = 0; i < dwg_struct->num_classes; i++)
	{
		printf ("Class: [%02u]\n", i);
		printf ("\tNumber: %u\n", dwg_struct->class[i].number);
		printf ("\tVersion: %u\n", dwg_struct->class[i].version);
		printf ("\tApplication Name: \"%s\"\n", dwg_struct->class[i].appname);
		printf ("\tC++ Name: \"%s\"\n", dwg_struct->class[i].cppname);
		printf ("\tDXF Name: \"%s\"\n", dwg_struct->class[i].dxfname);
		printf ("\tWas ghost: \"%s\"\n", dwg_struct->class[i].estisfantomo ? "Yes" : "No");
		printf ("\tEroId: %u\n", dwg_struct->class[i].eroid);
	}
	puts ("");

	puts ("**************************************************");
	puts ("Section OBJECTS");
	puts ("**************************************************");
	for (i = 0; i < dwg_struct->num_objects; i++)
	{
		Dwg_Object *obj;

		printf ("(%u) ", i);
		obj = &dwg_struct->object[i];

		printf ("Type: %s (%03i)\t", obj->type > 80 ? (obj->type == dwg_struct->dwg_ot_layout ? "LAYOUT" : "??") : dwg_obtype[obj->type], obj->type);
		printf ("Size: %u\t", obj->size);
		printf ("Traktilo: (%lu)\t", obj->trakt);
		printf ("Super-type: ");
		switch (obj->supertype)
		{
		case DWG_SUPERTYPE_ENTITY:
			puts ("entity");
			dwg_print_entity (obj->tio.entity);
			break;
		case DWG_SUPERTYPE_OBJECT:
			puts ("object");
			dwg_print_object (obj->tio.object);
			break;
		default:
			puts ("unknown");
			continue;
		}

		switch (obj->type)
		{
		case DWG_TYPE_TEXT:
			dwg_print_TEXT (obj->tio.entity->tio.TEXT);
			break;
		case DWG_TYPE_ATTRIB:
			dwg_print_ATTRIB (obj->tio.entity->tio.ATTRIB);
			break;
		case DWG_TYPE_ATTDEF:
			dwg_print_ATTDEF (obj->tio.entity->tio.ATTDEF);
			break;
		case DWG_TYPE_BLOCK:
			dwg_print_BLOCK (obj->tio.entity->tio.BLOCK);
			break;
		case DWG_TYPE_ENDBLK:
		case DWG_TYPE_SEQEND:
			break;
		case DWG_TYPE_INSERT:
			dwg_print_INSERT (obj->tio.entity->tio.INSERT);
			break;
		case DWG_TYPE_MINSERT:
			dwg_print_MINSERT (obj->tio.entity->tio.MINSERT);
			break;
		case DWG_TYPE_VERTEX_2D:
			dwg_print_VERTEX_2D (obj->tio.entity->tio.VERTEX_2D);
			break;
		case DWG_TYPE_VERTEX_3D:
		case DWG_TYPE_VERTEX_MESH:
		case DWG_TYPE_VERTEX_PFACE:
			dwg_print_VERTEX_3D (obj->tio.entity->tio.VERTEX_3D);
			break;
		case DWG_TYPE_VERTEX_PFACE_FACE:
			dwg_print_VERTEX_PFACE_FACE (obj->tio.entity->tio.VERTEX_PFACE_FACE);
			break;
		case DWG_TYPE_POLYLINE_2D:
			dwg_print_POLYLINE_2D (obj->tio.entity->tio.POLYLINE_2D);
			break;
		case DWG_TYPE_POLYLINE_3D:
			dwg_print_POLYLINE_3D (obj->tio.entity->tio.POLYLINE_3D);
			break;
		case DWG_TYPE_ARC:
			dwg_print_ARC (obj->tio.entity->tio.ARC);
			break;
		case DWG_TYPE_CIRCLE:
			dwg_print_CIRCLE (obj->tio.entity->tio.CIRCLE);
			break;
		case DWG_TYPE_LINE:
			dwg_print_LINE (obj->tio.entity->tio.LINE);
			break;
		case DWG_TYPE_POINT:
			dwg_print_POINT (obj->tio.entity->tio.POINT);
			break;
		case DWG_TYPE_ELLIPSE:
			dwg_print_ELLIPSE (obj->tio.entity->tio.ELLIPSE);
			break;
		case DWG_TYPE_RAY:
		case DWG_TYPE_XLINE:
			dwg_print_RAY (obj->tio.entity->tio.RAY);
			break;
		case DWG_TYPE_MTEXT:
			dwg_print_MTEXT (obj->tio.entity->tio.MTEXT);
			break;
		case DWG_TYPE_LAYER:
			dwg_print_LAYER (obj->tio.object->tio.LAYER);
			break;
		default:
			if (obj->type == dwg_struct->dwg_ot_layout)
				dwg_print_LAYOUT (obj->tio.object->tio.LAYOUT);
			else
				continue;
		}

		dwg_print_traktref (obj);

		puts ("");
	}

	puts ("**************************************************");
	puts ("Section DUA KAP-DATENARO ");
	puts ("**************************************************");
	for (i = 0; i < 14; i++)
	{
		printf ("Rikordo[%02i] Longo: %u\tChain:", i, dwg_struct->second_header.traktrik[i].size);
		for (j = 0; j < dwg_struct->second_header.traktrik[i].size; j++)
			printf (" %02X", dwg_struct->second_header.traktrik[i].chain[j]);
		puts ("");
	}
	puts ("");

	puts ("**************************************************");
	puts ("Section MEASUREMENT");
	puts ("**************************************************");
	printf ("MEASUREMENT: 0x%08X\n", (unsigned int) dwg_struct->measurement);
	puts ("");
}

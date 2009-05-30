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
	size_t kiom;
	Bit_Chain bitaro;

	if (stat (filename, &atrib))
	{
		printf ("File not found:\n %s\n", filename);
		return -1;
	}
	if (!S_ISREG (atrib.st_mode))
	{
		printf ("Error:\n %s\n", filename);
		return -1;
	}
	fp = fopen (filename, "rb");
	if (!fp)
	{
		printf ("Error while opening the file:\n %s\n", filename);
		return -1;
	}

	/* Sxargi la memoron je la dosiero
	 */
	bitaro.bito = 0;
	bitaro.bajto = 0;
	bitaro.kiom = atrib.st_size;
	bitaro.chain = (char *) malloc (bitaro.kiom);
	if (!bitaro.chain)
	{
		puts ("Not enough memory.");
		fclose (fp);
		return -1;
	}
	kiom = 0;
	kiom = fread (bitaro.chain, sizeof (char), bitaro.kiom, fp);
	if (kiom != bitaro.kiom)
	{
		printf ("Ne eblis read la tutan dosieron (%lu el %lu):\n %s\n", (long unsigned int) kiom, bitaro.kiom,
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
		printf ("Ni ne sukcesis dekodigi la dosieron:\n %s\n", filename);
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

	/* Enkodigi la dwg-datenaron
	bitaro.kiom = 0;
	if (dwg_encode_chains (dwg_struct, &bitaro))
	{
		puts ("Ni ne sukcesis enkodigi la strukturon.");
		if (bitaro.kiom > 0)
			free (bitaro.chain);
		return -1;
	}
	 */

	/* Testi kaj malfermi dosieron por write
	if (!stat (filename, &atrib))
	{
		puts ("La skribota dosiero jam ekzistas, ni ne povas surwrite gxin.");
		return -1;
	}
	dt = fopen (filename, "w");
	if (!dt)
	{
		printf ("Ne eblas krei la dosieron:\n %s\n", filename);
		return -1;
	}
	 */

	/* Skribi la datenaron en la dosiero
	if (fwrite (bitaro.chain, sizeof (char), bitaro.kiom, dt) != bitaro.kiom)
	{
		printf ("Ne eblis write la tutan dosieron:\n %s\n", filename);
		fclose (dt);
		free (bitaro.chain);
		return -1;
	}
	fclose (dt);

	if (bitaro.kiom > 0)
		free (bitaro.chain);
	 */
	return 0;
}

/*------------------------------------------------------------------------------
 * Private functions
 */

static void
dwg_print_estajxo (Dwg_Object_Estajxo * est)
{
	printf ("Bitsize: %lu\n", est->bitsize);
	printf ("Vera traktilo: %i.%i.%lu\n", est->traktilo.code, est->traktilo.kiom,
		est->traktilo.value);
	printf ("Kroma datenaro: %lu B\n", (long unsigned int) est->kromdat_kiom);
	printf ("Ĉu picture?: %s", est->picture_ekzistas ? "Jes" : "Ne");
	if (est->picture_ekzistas)
		printf ("\tSize: %lu B\n", est->picture_kiom);
	else
		puts ("");
	printf ("Reĝimo: %i\n", est->regime);
	printf ("Kiom reagiloj: %lu\n", est->reagilo_kiom);
	printf ("Ĉu senligiloj?: %s\n", est->senligiloj ? "Jes" : "Ne");
	printf ("Koloro: %u\n", est->colour);
	printf ("Skalo de linitipo: %1.13g\n", est->linitiposkalo);
	printf ("Linitipo: 0x%02X\n", est->linitipo);
	printf ("Printstilo: 0x%02X\n", est->printstilo);
	printf ("Malvidebleco: 0x%04X\n", est->malvidebleco);
	printf ("Linithickness: %u\n", est->linithickness);
}

static void
dwg_print_ordinarajxo (Dwg_Object_Ordinarajxo *ord)
{
	printf ("Bitsize: %lu\n", ord->bitsize);
	printf ("Vera traktilo: %i.%i.%lu\n", ord->traktilo.code, ord->traktilo.kiom,
		ord->traktilo.value);
	printf ("Kroma datenaro: %lu B\n", (long unsigned int) ord->kromdat_kiom);
	printf ("Kiom reagiloj: %lu\n", ord->reagilo_kiom);
}


static void
dwg_print_traktref (Dwg_Object * obj)
{
	unsigned int i;

	if (obj->supertipo == DWG_SUPERTYPE_ESTAJXO)
	{
		Dwg_Object_Estajxo *est;

		est = obj->tio.estajxo;

		printf ("\tTraktil-referencoj (%u): ", est->traktref_kiom);
		if (est->traktref_kiom == 0)
		{
			puts ("");
			return;
		}
		for (i = 0; i < est->traktref_kiom - 1; i++)
			printf ("%i.%i.%li / ", est->traktref[i].code, est->traktref[i].kiom,
				est->traktref[i].value);
		printf ("%i.%i.%li\n", est->traktref[i].code, est->traktref[i].kiom,
			est->traktref[i].value);
	}
	else if (obj->supertipo == DWG_SUPERTYPE_ORDINARAJXO)
	{
		Dwg_Object_Ordinarajxo *ord;

		ord = obj->tio.ordinarajxo;

		printf ("\tTraktil-referencoj (%u): ", ord->traktref_kiom);
		if (ord->traktref_kiom == 0)
		{
			puts ("");
			return;
		}
		for (i = 0; i < ord->traktref_kiom - 1; i++)
			printf ("%i.%i.%li / ", ord->traktref[i].code, ord->traktref[i].kiom,
				ord->traktref[i].value);
		printf ("%i.%i.%li\n", ord->traktref[i].code, ord->traktref[i].kiom,
			ord->traktref[i].value);
	}
}

/* OBJEKTOJ *******************************************************************/

static void
dwg_print_TEXT (Dwg_Estajxo_TEXT * est)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", est->datumindik);

	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", est->x0, est->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", est->gxisrandigo.x, est->gxisrandigo.y);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tKlina angulo: %1.13g\n", est->klinang);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tAlteco: %1.13g\n", est->height);
	printf ("\tLarĝ-faktoro: %1.13g\n", est->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", est->text);
	printf ("\tGeneracio: %u\n", est->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", est->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", est->gxisrandigo.v);
}

static void
dwg_print_ATTRIB (Dwg_Estajxo_ATTRIB * est)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", est->datumindik);

	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", est->x0, est->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", est->gxisrandigo.x, est->gxisrandigo.y);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tKlina angulo: %1.13g\n", est->klinang);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tAlteco: %1.13g\n", est->height);
	printf ("\tLarĝ-faktoro: %1.13g\n", est->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", est->text);
	printf ("\tGeneracio: %u\n", est->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", est->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", est->gxisrandigo.v);
	printf ("\tEtikedo: %s\n", est->etikedo);
	printf ("\tKamp-longeco: %i (ne uzata)\n", est->kamplong);
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
}

static void
dwg_print_ATTDEF (Dwg_Estajxo_ATTDEF * est)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", est->datumindik);

	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", est->x0, est->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", est->gxisrandigo.x, est->gxisrandigo.y);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tKlina angulo: %1.13g\n", est->klinang);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tAlteco: %1.13g\n", est->height);
	printf ("\tLarĝ-faktoro: %1.13g\n", est->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", est->text);
	printf ("\tGeneracio: %u\n", est->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", est->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", est->gxisrandigo.v);
	printf ("\tEtikedo: %s\n", est->etikedo);
	printf ("\tKamp-longeco: %i (ne uzata)\n", est->kamplong);
	printf ("\tIndikilo: 0x%02x\n", est->indikiloj);
	printf ("\tInvitilo (prompt): %s\n", est->invitilo);
}

static void
dwg_print_BLOCK (Dwg_Estajxo_BLOCK * est)
{
	printf ("\tName: %s\n", est->name);
}

static void
dwg_print_INSERT (Dwg_Estajxo_INSERT * est)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", est->skalindik);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", est->skalo.x, est->skalo.y, est->skalo.z);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", est->kun_attrib ? "Jes" : "Ne");
}

static void
dwg_print_MINSERT (Dwg_Estajxo_MINSERT * est)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", est->skalindik);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", est->skalo.x, est->skalo.y, est->skalo.z);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", est->kun_attrib ? "Jes" : "Ne");
	printf ("\tKolumnoj: %02i\tInterspaco: %1.13g\n", est->kol.kiom, est->kol.dx);
	printf ("\t  Linioj: %02i\tInterspaco: %1.13g\n", est->lin.kiom, est->lin.dy);
}

static void
dwg_print_VERTEX_2D (Dwg_Estajxo_VERTEX_2D * est)
{
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tEklarĝo: %1.13g\n", est->eklargxo);
	printf ("\tFinlarĝo: %1.13g\n", est->finlargxo);
	printf ("\tProtuberanco: %1.13g\n", est->protub);
	printf ("\tTanĝento dir.: %1.13g\n", est->tangxdir);
}

static void
dwg_print_VERTEX_3D (Dwg_Estajxo_VERTEX_3D * est)
{
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
}

static void
dwg_print_VERTEX_PFACE_FACE (Dwg_Estajxo_VERTEX_PFACE_FACE * est)
{
	printf ("\tVertic-index 1: %i\n", est->vertind[0]);
	printf ("\tVertic-index 2: %i\n", est->vertind[1]);
	printf ("\tVertic-index 3: %i\n", est->vertind[2]);
	printf ("\tVertic-index 4: %i\n", est->vertind[3]);
}

static void
dwg_print_POLYLINE_2D (Dwg_Estajxo_POLYLINE_2D * est)
{
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
	printf ("\tKurbtipo: 0x%02x\n", est->kurbtipo);
	printf ("\tEklarĝo: %1.13g\n", est->eklargxo);
	printf ("\tFinlarĝo: %1.13g\n", est->finlargxo);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
}

static void
dwg_print_POLYLINE_3D (Dwg_Estajxo_POLYLINE_3D * est)
{
	printf ("\tIndikiloj: 0x%02x / 0x%02x\n", est->indikiloj_1, est->indikiloj_2);
}

static void
dwg_print_ARC (Dwg_Estajxo_ARC * est)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tRadiuso: %1.13g\n", est->radius);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tEkangulo: %1.13g\n", est->ekangulo);
	printf ("\tFinangulo: %1.13g\n", est->finangulo);
}

static void
dwg_print_CIRCLE (Dwg_Estajxo_CIRCLE * est)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tRadiuso: %1.13g\n", est->radius);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
}

static void
dwg_print_LINE (Dwg_Estajxo_LINE * est)
{
	printf ("\tĈu nur 2D?: %s\n", est->nur_2D ? "Jes" : "Ne");
	printf ("\t1-a punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0,
		est->nur_2D ? 0 : est->z0);
	printf ("\t2-a punkto: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1,
		est->nur_2D ? 0 : est->z1);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
}

static void
dwg_print_POINT (Dwg_Estajxo_POINT *est)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tthickness: %1.13g\n", est->thickness);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tX-angulo: %1.13g\n", est->x_ang);
}

static void
dwg_print_ELLIPSE (Dwg_Estajxo_ELLIPSE *est)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tDuonĉef-aksa vektoro: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1, est->z1);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tRadius-proporcio: %1.13g\n", est->radiusproporcio);
	printf ("\tEkangulo: %1.13g\n", est->ekangulo);
	printf ("\tFinangulo: %1.13g\n", est->finangulo);
}

static void
dwg_print_RAY (Dwg_Estajxo_RAY *est)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tVektoro: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1, est->z1);
}

static void
dwg_print_MTEXT (Dwg_Estajxo_MTEXT *est)
{
	printf ("\tEnmeto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->extrusion.x, est->extrusion.y,
		est->extrusion.z);
	printf ("\tX-direkto: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1, est->z1);
	printf ("\tLargxeco: %1.13g\n", est->largxeco);
	printf ("\tAlteco: %1.13g\n", est->height);
	printf ("\tKunmeto: 0x%0x\n", est->kunmeto);
	printf ("\tDirekto: 0x%0x\n", est->direkto);
	printf ("\tTeksto: %s\n", est->text);
	printf ("\tLinispaca stilo: 0x%0x\n", est->linispaco_stilo);
	printf ("\tLinispaca faktoro: %1.13g\n", (double) est->linispaco_faktoro);
}

static void
dwg_print_LAYER (Dwg_Ordinarajxo_LAYER *ord)
{
	printf ("\tName: %s\n", ord->name);
	printf ("\tĈu 64?: %s\n", ord->bito64 ? "Jes" : "Ne");
	printf ("\tIndico Xref: %u\n", ord->xrefi);
	printf ("\tXref-dependa?: %s\n", ord->xrefdep ? "Jes" : "Ne");
	printf ("\tEcoj: 0x%0x\n", ord->ecoj);
	printf ("\tKoloro: %u\n", ord->colour);
}

static void
dwg_print_LAYOUT (Dwg_Ordinarajxo_LAYOUT *ord)
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
	printf ("\tTipo: %u\n", ord->pagxo.tipo);
	printf ("\tPrint-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n",
		ord->pagxo.x_min,
		ord->pagxo.y_min,
		ord->pagxo.x_maks,
		ord->pagxo.y_maks);
	printf ("\tPaĝ-name: %s\n", ord->pagxo.name);
	printf ("\tSkal-proporcio: %u:%u\n", (unsigned int) ord->pagxo.skalo.A, (unsigned int) ord->pagxo.skalo.B);
	printf ("\tStilfolio: %s\n", ord->pagxo.stilfolio);
	printf ("\tSkal-tipo: %u\n", ord->pagxo.skalo.tipo);
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
	printf ("\tRigard-tipo: %u\n", ord->rigardtipo);
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
	const char *dwg_obtipo[81] = {
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
	printf ("Version: %s\n", dwg_struct->header.version);
	printf ("Codepage: %u\n", dwg_struct->header.codepage);
	for (i = 0; i < dwg_struct->header.num_sections; i++)
		printf ("Section %i\t Address: %7lu\t Size: %7lu B\n",
			dwg_struct->header.section[i].number,
			dwg_struct->header.section[i].adresilo, dwg_struct->header.section[i].size);
	puts ("");

	if (dwg_struct->header.num_sections == 6)
	{
		puts ("**************************************************");
		puts ("Section UNKNOWN 1");
		puts ("**************************************************");
		printf ("Size: %lu B\n", dwg_struct->unknown1.kiom);
		bit_print ((Bit_Chain *) & dwg_struct->unknown1, dwg_struct->unknown1.kiom);
		puts ("");
	}

	puts ("**************************************************");
	puts ("Section PICTURE");
	puts ("**************************************************");
	printf ("Size: %lu B\n", dwg_struct->picture.kiom);
	//bit_print ((Bit_Chain *) &dwg_struct->picture, dwg_struct->picture.kiom);
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
		switch (dwg_var_map (i))
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
				dwg_struct->var[i].traktilo.kiom, dwg_struct->var[i].traktilo.value);
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
			printf ("Ne traktebla tipo: %i (var: %i)\n", dwg_var_map (i), i);
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
		printf ("\tEstis fantomo: \"%s\"\n", dwg_struct->class[i].estisfantomo ? "Jes" : "Ne");
		printf ("\tEroId: %u\n", dwg_struct->class[i].eroid);
	}
	puts ("");

	puts ("**************************************************");
	puts ("Section OBJEKTOJ");
	puts ("**************************************************");
	for (i = 0; i < dwg_struct->object_kiom; i++)
	{
		Dwg_Object *obj;

		printf ("(%u) ", i);
		obj = &dwg_struct->object[i];

		printf ("Tipo: %s (%03i)\t", obj->tipo > 80 ? (obj->tipo == dwg_struct->dwg_ot_layout ? "LAYOUT" : "??") : dwg_obtipo[obj->tipo], obj->tipo);
		printf ("Size: %u\t", obj->size);
		printf ("Traktilo: (%lu)\t", obj->trakt);
		printf ("Super-tipo: ");
		switch (obj->supertipo)
		{
		case DWG_SUPERTYPE_ESTAJXO:
			puts ("estajxo");
			dwg_print_estajxo (obj->tio.estajxo);
			break;
		case DWG_SUPERTYPE_ORDINARAJXO:
			puts ("ordinarajxo");
			dwg_print_ordinarajxo (obj->tio.ordinarajxo);
			break;
		default:
			puts ("unknownjxo");
			continue;
		}

		switch (obj->tipo)
		{
		case DWG_TYPE_TEXT:
			dwg_print_TEXT (obj->tio.estajxo->tio.TEXT);
			break;
		case DWG_TYPE_ATTRIB:
			dwg_print_ATTRIB (obj->tio.estajxo->tio.ATTRIB);
			break;
		case DWG_TYPE_ATTDEF:
			dwg_print_ATTDEF (obj->tio.estajxo->tio.ATTDEF);
			break;
		case DWG_TYPE_BLOCK:
			dwg_print_BLOCK (obj->tio.estajxo->tio.BLOCK);
			break;
		case DWG_TYPE_ENDBLK:
		case DWG_TYPE_SEQEND:
			break;
		case DWG_TYPE_INSERT:
			dwg_print_INSERT (obj->tio.estajxo->tio.INSERT);
			break;
		case DWG_TYPE_MINSERT:
			dwg_print_MINSERT (obj->tio.estajxo->tio.MINSERT);
			break;
		case DWG_TYPE_VERTEX_2D:
			dwg_print_VERTEX_2D (obj->tio.estajxo->tio.VERTEX_2D);
			break;
		case DWG_TYPE_VERTEX_3D:
		case DWG_TYPE_VERTEX_MESH:
		case DWG_TYPE_VERTEX_PFACE:
			dwg_print_VERTEX_3D (obj->tio.estajxo->tio.VERTEX_3D);
			break;
		case DWG_TYPE_VERTEX_PFACE_FACE:
			dwg_print_VERTEX_PFACE_FACE (obj->tio.estajxo->tio.VERTEX_PFACE_FACE);
			break;
		case DWG_TYPE_POLYLINE_2D:
			dwg_print_POLYLINE_2D (obj->tio.estajxo->tio.POLYLINE_2D);
			break;
		case DWG_TYPE_POLYLINE_3D:
			dwg_print_POLYLINE_3D (obj->tio.estajxo->tio.POLYLINE_3D);
			break;
		case DWG_TYPE_ARC:
			dwg_print_ARC (obj->tio.estajxo->tio.ARC);
			break;
		case DWG_TYPE_CIRCLE:
			dwg_print_CIRCLE (obj->tio.estajxo->tio.CIRCLE);
			break;
		case DWG_TYPE_LINE:
			dwg_print_LINE (obj->tio.estajxo->tio.LINE);
			break;
		case DWG_TYPE_POINT:
			dwg_print_POINT (obj->tio.estajxo->tio.POINT);
			break;
		case DWG_TYPE_ELLIPSE:
			dwg_print_ELLIPSE (obj->tio.estajxo->tio.ELLIPSE);
			break;
		case DWG_TYPE_RAY:
		case DWG_TYPE_XLINE:
			dwg_print_RAY (obj->tio.estajxo->tio.RAY);
			break;
		case DWG_TYPE_MTEXT:
			dwg_print_MTEXT (obj->tio.estajxo->tio.MTEXT);
			break;
		case DWG_TYPE_LAYER:
			dwg_print_LAYER (obj->tio.ordinarajxo->tio.LAYER);
			break;
		default:
			if (obj->tipo == dwg_struct->dwg_ot_layout)
				dwg_print_LAYOUT (obj->tio.ordinarajxo->tio.LAYOUT);
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
		printf ("Rikordo[%02i] Longo: %u\tChain:", i, dwg_struct->duaheader.traktrik[i].kiom);
		for (j = 0; j < dwg_struct->duaheader.traktrik[i].kiom; j++)
			printf (" %02X", dwg_struct->duaheader.traktrik[i].chain[j]);
		puts ("");
	}
	puts ("");

	puts ("**************************************************");
	puts ("Section MEZURO (MEASUREMENT)");
	puts ("**************************************************");
	printf ("MEZURO: 0x%08X\n", (unsigned int) dwg_struct->mezuro);
	puts ("");
}

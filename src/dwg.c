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
 * Publikaj funkcioj
 */
int
dvg_legi_dosiero (char *dosiernomo, Dvg_Strukturo * dvg_strukt)
{
	int signo;
	FILE *dt;
	struct stat atrib;
	size_t kiom;
	Bit_Cxeno bitaro;

	/* Testi kaj sxargi je la dosiero
	 */
	if (stat (dosiernomo, &atrib))
	{
		printf ("Ne ekzistas tiu dosiero:\n %s\n", dosiernomo);
		return -1;
	}
	if (!S_ISREG (atrib.st_mode))
	{
		printf ("Ne eblas trakti dosierujojn, aparat-dosierojn, ligil-dosierojn, ktp:\n %s\n", dosiernomo);
		return -1;
	}
	dt = fopen (dosiernomo, "rb");
	if (!dt)
	{
		printf ("Ne eblas malfermi la dosieron:\n %s\n", dosiernomo);
		return -1;
	}

	/* Sxargi la memoron je la dosiero
	 */
	bitaro.bito = 0;
	bitaro.bajto = 0;
	bitaro.kiom = atrib.st_size;
	bitaro.cxeno = (char *) malloc (bitaro.kiom);
	if (!bitaro.cxeno)
	{
		puts ("Manko de memoro.");
		fclose (dt);
		return -1;
	}
	kiom = 0;
	kiom = fread (bitaro.cxeno, sizeof (char), bitaro.kiom, dt);
	if (kiom != bitaro.kiom)
	{
		printf ("Ne eblis legi la tutan dosieron (%lu el %lu):\n %s\n", kiom, bitaro.kiom,
			dosiernomo);
		fclose (dt);
		free (bitaro.cxeno);
		return -1;
	}
	fclose (dt);

	/* Dekodigi la dvg-datenaron
	 */
	if (dvg_dek_strukturigi (&bitaro, dvg_strukt))
	{
		printf ("Ni ne sukcesis dekodigi la dosieron:\n %s\n", dosiernomo);
		free (bitaro.cxeno);
		return -1;
	}
	free (bitaro.cxeno);

	return 0;
}

int
dvg_skribi_dosiero (char *dosiernomo, Dvg_Strukturo * strukt)
{
	FILE *dt;
	struct stat atrib;
	Bit_Cxeno bitaro;

	/* Enkodigi la dvg-datenaron
	bitaro.kiom = 0;
	if (dvg_enk_cxenigi (strukt, &bitaro))
	{
		puts ("Ni ne sukcesis enkodigi la strukturon.");
		if (bitaro.kiom > 0)
			free (bitaro.cxeno);
		return -1;
	}
	 */

	/* Testi kaj malfermi dosieron por skribi
	if (!stat (dosiernomo, &atrib))
	{
		puts ("La skribota dosiero jam ekzistas, ni ne povas surskribi gxin.");
		return -1;
	}
	dt = fopen (dosiernomo, "w");
	if (!dt)
	{
		printf ("Ne eblas krei la dosieron:\n %s\n", dosiernomo);
		return -1;
	}
	 */

	/* Skribi la datenaron en la dosiero
	if (fwrite (bitaro.cxeno, sizeof (char), bitaro.kiom, dt) != bitaro.kiom)
	{
		printf ("Ne eblis skribi la tutan dosieron:\n %s\n", dosiernomo);
		fclose (dt);
		free (bitaro.cxeno);
		return -1;
	}
	fclose (dt);

	if (bitaro.kiom > 0)
		free (bitaro.cxeno);
	 */
	return 0;
}

/*------------------------------------------------------------------------------
 * Privataj funkcioj
 */

static void
dvg_montri_estajxo (Dvg_Objekto_Estajxo * est)
{
	printf ("Bitgrandeco: %lu\n", est->bitgrandeco);
	printf ("Vera traktilo: %i.%i.%lu\n", est->traktilo.kodo, est->traktilo.kiom,
		est->traktilo.valoro);
	printf ("Kroma datenaro: %lu B\n", est->kromdat_kiom);
	printf ("Ĉu bildo?: %s", est->bildo_ekzistas ? "Jes" : "Ne");
	if (est->bildo_ekzistas)
		printf ("\tGrandeco: %lu B\n", est->bildo_kiom);
	else
		puts ("");
	printf ("Reĝimo: %i\n", est->regximo);
	printf ("Kiom reagiloj: %lu\n", est->reagilo_kiom);
	printf ("Ĉu senligiloj?: %s\n", est->senligiloj ? "Jes" : "Ne");
	printf ("Koloro: %u\n", est->koloro);
	printf ("Skalo de linitipo: %1.13g\n", est->linitiposkalo);
	printf ("Linitipo: 0x%02X\n", est->linitipo);
	printf ("Printstilo: 0x%02X\n", est->printstilo);
	printf ("Malvidebleco: 0x%04X\n", est->malvidebleco);
	printf ("Linidikeco: %u\n", est->linidikeco);
}

static void
dvg_montri_ordinarajxo (Dvg_Objekto_Ordinarajxo *ord)
{
	printf ("Bitgrandeco: %lu\n", ord->bitgrandeco);
	printf ("Vera traktilo: %i.%i.%lu\n", ord->traktilo.kodo, ord->traktilo.kiom,
		ord->traktilo.valoro);
	printf ("Kroma datenaro: %lu B\n", ord->kromdat_kiom);
	printf ("Kiom reagiloj: %lu\n", ord->reagilo_kiom);
}


static void
dvg_montri_traktref (Dvg_Objekto * obj)
{
	unsigned int i;

	if (obj->supertipo == DVG_OST_ESTAJXO)
	{
		Dvg_Objekto_Estajxo *est;

		est = obj->tio.estajxo;

		printf ("\tTraktil-referencoj (%u): ", est->traktref_kiom);
		if (est->traktref_kiom == 0)
		{
			puts ("");
			return;
		}
		for (i = 0; i < est->traktref_kiom - 1; i++)
			printf ("%i.%i.%li / ", est->traktref[i].kodo, est->traktref[i].kiom,
				est->traktref[i].valoro);
		printf ("%i.%i.%li\n", est->traktref[i].kodo, est->traktref[i].kiom,
			est->traktref[i].valoro);
	}
	else if (obj->supertipo == DVG_OST_ORDINARAJXO)
	{
		Dvg_Objekto_Ordinarajxo *ord;

		ord = obj->tio.ordinarajxo;

		printf ("\tTraktil-referencoj (%u): ", ord->traktref_kiom);
		if (ord->traktref_kiom == 0)
		{
			puts ("");
			return;
		}
		for (i = 0; i < ord->traktref_kiom - 1; i++)
			printf ("%i.%i.%li / ", ord->traktref[i].kodo, ord->traktref[i].kiom,
				ord->traktref[i].valoro);
		printf ("%i.%i.%li\n", ord->traktref[i].kodo, ord->traktref[i].kiom,
			ord->traktref[i].valoro);
	}
}

/* OBJEKTOJ *******************************************************************/

static void
dvg_montri_TEXT (Dvg_Estajxo_TEXT * est)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", est->datumindik);

	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", est->x0, est->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", est->gxisrandigo.x, est->gxisrandigo.y);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tKlina angulo: %1.13g\n", est->klinang);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tAlteco: %1.13g\n", est->alteco);
	printf ("\tLarĝ-faktoro: %1.13g\n", est->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", est->teksto);
	printf ("\tGeneracio: %u\n", est->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", est->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", est->gxisrandigo.v);
}

static void
dvg_montri_ATTRIB (Dvg_Estajxo_ATTRIB * est)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", est->datumindik);

	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", est->x0, est->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", est->gxisrandigo.x, est->gxisrandigo.y);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tKlina angulo: %1.13g\n", est->klinang);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tAlteco: %1.13g\n", est->alteco);
	printf ("\tLarĝ-faktoro: %1.13g\n", est->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", est->teksto);
	printf ("\tGeneracio: %u\n", est->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", est->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", est->gxisrandigo.v);
	printf ("\tEtikedo: %s\n", est->etikedo);
	printf ("\tKamp-longeco: %i (ne uzata)\n", est->kamplong);
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
}

static void
dvg_montri_ATTDEF (Dvg_Estajxo_ATTDEF * est)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", est->datumindik);

	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", est->x0, est->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", est->gxisrandigo.x, est->gxisrandigo.y);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tKlina angulo: %1.13g\n", est->klinang);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tAlteco: %1.13g\n", est->alteco);
	printf ("\tLarĝ-faktoro: %1.13g\n", est->largxfaktoro);
	printf ("\tTeksto: \"%s\"\n", est->teksto);
	printf ("\tGeneracio: %u\n", est->generacio);
	printf ("\tĜisrandigo (horiz.): %u\n", est->gxisrandigo.h);
	printf ("\tĜisrandigo (vert.): %u\n", est->gxisrandigo.v);
	printf ("\tEtikedo: %s\n", est->etikedo);
	printf ("\tKamp-longeco: %i (ne uzata)\n", est->kamplong);
	printf ("\tIndikilo: 0x%02x\n", est->indikiloj);
	printf ("\tInvitilo (prompt): %s\n", est->invitilo);
}

static void
dvg_montri_BLOCK (Dvg_Estajxo_BLOCK * est)
{
	printf ("\tNomo: %s\n", est->nomo);
}

static void
dvg_montri_INSERT (Dvg_Estajxo_INSERT * est)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", est->skalindik);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", est->skalo.x, est->skalo.y, est->skalo.z);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", est->kun_attrib ? "Jes" : "Ne");
}

static void
dvg_montri_MINSERT (Dvg_Estajxo_MINSERT * est)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", est->skalindik);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", est->skalo.x, est->skalo.y, est->skalo.z);
	printf ("\tTurna angulo: %1.13g\n", est->turnang);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", est->kun_attrib ? "Jes" : "Ne");
	printf ("\tKolumnoj: %02i\tInterspaco: %1.13g\n", est->kol.kiom, est->kol.dx);
	printf ("\t  Linioj: %02i\tInterspaco: %1.13g\n", est->lin.kiom, est->lin.dy);
}

static void
dvg_montri_VERTEX_2D (Dvg_Estajxo_VERTEX_2D * est)
{
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tEklarĝo: %1.13g\n", est->eklargxo);
	printf ("\tFinlarĝo: %1.13g\n", est->finlargxo);
	printf ("\tProtuberanco: %1.13g\n", est->protub);
	printf ("\tTanĝento dir.: %1.13g\n", est->tangxdir);
}

static void
dvg_montri_VERTEX_3D (Dvg_Estajxo_VERTEX_3D * est)
{
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
}

static void
dvg_montri_VERTEX_PFACE_FACE (Dvg_Estajxo_VERTEX_PFACE_FACE * est)
{
	printf ("\tVertic-indico 1: %i\n", est->vertind[0]);
	printf ("\tVertic-indico 2: %i\n", est->vertind[1]);
	printf ("\tVertic-indico 3: %i\n", est->vertind[2]);
	printf ("\tVertic-indico 4: %i\n", est->vertind[3]);
}

static void
dvg_montri_POLYLINE_2D (Dvg_Estajxo_POLYLINE_2D * est)
{
	printf ("\tIndikiloj: 0x%02x\n", est->indikiloj);
	printf ("\tKurbtipo: 0x%02x\n", est->kurbtipo);
	printf ("\tEklarĝo: %1.13g\n", est->eklargxo);
	printf ("\tFinlarĝo: %1.13g\n", est->finlargxo);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tLeviĝo: %1.13g\n", est->levigxo);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
}

static void
dvg_montri_POLYLINE_3D (Dvg_Estajxo_POLYLINE_3D * est)
{
	printf ("\tIndikiloj: 0x%02x / 0x%02x\n", est->indikiloj_1, est->indikiloj_2);
}

static void
dvg_montri_ARC (Dvg_Estajxo_ARC * est)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tRadiuso: %1.13g\n", est->radiuso);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tEkangulo: %1.13g\n", est->ekangulo);
	printf ("\tFinangulo: %1.13g\n", est->finangulo);
}

static void
dvg_montri_CIRCLE (Dvg_Estajxo_CIRCLE * est)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tRadiuso: %1.13g\n", est->radiuso);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
}

static void
dvg_montri_LINE (Dvg_Estajxo_LINE * est)
{
	printf ("\tĈu nur 2D?: %s\n", est->nur_2D ? "Jes" : "Ne");
	printf ("\t1-a punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0,
		est->nur_2D ? 0 : est->z0);
	printf ("\t2-a punkto: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1,
		est->nur_2D ? 0 : est->z1);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
}

static void
dvg_montri_POINT (Dvg_Estajxo_POINT *est)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tDikeco: %1.13g\n", est->dikeco);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tX-angulo: %1.13g\n", est->x_ang);
}

static void
dvg_montri_ELLIPSE (Dvg_Estajxo_ELLIPSE *est)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tDuonĉef-aksa vektoro: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1, est->z1);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tRadius-proporcio: %1.13g\n", est->radiusproporcio);
	printf ("\tEkangulo: %1.13g\n", est->ekangulo);
	printf ("\tFinangulo: %1.13g\n", est->finangulo);
}

static void
dvg_montri_RAY (Dvg_Estajxo_RAY *est)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tVektoro: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1, est->z1);
}

static void
dvg_montri_MTEXT (Dvg_Estajxo_MTEXT *est)
{
	printf ("\tEnmeto: (%1.13g, %1.13g, %1.13g)\n", est->x0, est->y0, est->z0);
	printf ("\tForpuŝigo: (%1.13g, %1.13g, %1.13g)\n", est->forpusxigo.x, est->forpusxigo.y,
		est->forpusxigo.z);
	printf ("\tX-direkto: (%1.13g, %1.13g, %1.13g)\n", est->x1, est->y1, est->z1);
	printf ("\tLargxeco: %1.13g\n", est->largxeco);
	printf ("\tAlteco: %1.13g\n", est->alteco);
	printf ("\tKunmeto: 0x%0x\n", est->kunmeto);
	printf ("\tDirekto: 0x%0x\n", est->direkto);
	printf ("\tTeksto: %s\n", est->teksto);
	printf ("\tLinispaca stilo: 0x%0x\n", est->linispaco_stilo);
	printf ("\tLinispaca faktoro: %1.13g\n", est->linispaco_faktoro);
}

static void
dvg_montri_LAYER (Dvg_Ordinarajxo_LAYER *ord)
{
	printf ("\tNomo: %s\n", ord->nomo);
	printf ("\tĈu 64?: %s\n", ord->bito64 ? "Jes" : "Ne");
	printf ("\tIndico Xref: %u\n", ord->xrefi);
	printf ("\tXref-dependa?: %s\n", ord->xrefdep ? "Jes" : "Ne");
	printf ("\tEcoj: 0x%0x\n", ord->ecoj);
	printf ("\tKoloro: %u\n", ord->koloro);
}

static void
dvg_montri_LAYOUT (Dvg_Ordinarajxo_LAYOUT *ord)
{
	printf ("\tAgord-nomo: %s\n", ord->pagxo.agordo);
	printf ("\tPrintilo: %s\n", ord->pagxo.printilo);
	printf ("\tPrint-indikiloj: 0x%0x\n", ord->pagxo.indikiloj);
	printf ("\tMarĝenoj. maldekstre: %1.13g; malsupre: %1.13g; dekstre: %1.13g; supre: %1.13g\n",
		ord->pagxo.maldekstre,
		ord->pagxo.malsupre,
		ord->pagxo.dekstre,
		ord->pagxo.supre);
	printf ("\tLargxeco: %1.13g\n", ord->pagxo.largxeco);
	printf ("\tAlteco: %1.13g\n", ord->pagxo.alteco);
	printf ("\tGrandeco: %s\n", ord->pagxo.grandeco);
	printf ("\tDeŝovo: (%1.13g, %1.13g)\n", ord->pagxo.dx, ord->pagxo.dy);
	printf ("\tUnuoj: %u\n", ord->pagxo.unuoj);
	printf ("\tRotacio: %u\n", ord->pagxo.rotacio);
	printf ("\tTipo: %u\n", ord->pagxo.tipo);
	printf ("\tPrint-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n",
		ord->pagxo.x_min,
		ord->pagxo.y_min,
		ord->pagxo.x_maks,
		ord->pagxo.y_maks);
	printf ("\tPaĝ-nomo: %s\n", ord->pagxo.nomo);
	printf ("\tSkal-proporcio: %u:%u\n", ord->pagxo.skalo.A, ord->pagxo.skalo.B);
	printf ("\tStilfolio: %s\n", ord->pagxo.stilfolio);
	printf ("\tSkal-tipo: %u\n", ord->pagxo.skalo.tipo);
	printf ("\tSkal-faktoro: %u\n", ord->pagxo.skalo.faktoro);
	printf ("\tPaĝ-origino: (%1.13g, %1.13g)\n", ord->pagxo.x0, ord->pagxo.y0);
	puts ("");
	printf ("\tAranĝ-nomo: %s\n", ord->nomo);
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
 * Speciala publika funkcio por montrigi valorojn de la objektoj
 */
void
dvg_montri (Dvg_Strukturo *skt)
{
	unsigned char sig;
	unsigned int i, j;
	const char *dvg_obtipo[81] = {
	"NE_UZATA", "TEXT", "ATTRIB", "ATTDEF",	"BLOCK",
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
	puts ("Sekcio KAPO");
	puts ("**************************************************");
	printf ("Versio: %s\n", skt->kapo.versio);
	printf ("Kodpagxo: %u\n", skt->kapo.kodpagxo);
	for (i = 0; i < skt->kapo.sekcio_kiom; i++)
		printf ("Sekcio %i\t Kie: %7lu\t Kiom: %7lu B\n",
			skt->kapo.sekcio[i].numero,
			skt->kapo.sekcio[i].adresilo, skt->kapo.sekcio[i].grandeco);
	puts ("");

	if (skt->kapo.sekcio_kiom == 6)
	{
		puts ("**************************************************");
		puts ("Sekcio NEKONATA 1");
		puts ("**************************************************");
		printf ("Grandeco: %lu B\n", skt->nekonata1.kiom);
		bit_montri ((Bit_Cxeno *) & skt->nekonata1, skt->nekonata1.kiom);
		puts ("");
	}

	puts ("**************************************************");
	puts ("Sekcio BILDO");
	puts ("**************************************************");
	printf ("Grandeco: %lu B\n", skt->bildo.kiom);
	//bit_montri ((Bit_Cxeno *) &skt->bildo, skt->bildo.kiom);
	puts ("");

	puts ("**************************************************");
	puts ("Sekcio VARIABLOJ");
	puts ("**************************************************");
	for (i = 0; i < DVG_KIOM_VARIABLOJ; i++)
	{
		printf ("[%03i] - ", i + 1);
		if (i == 221 && skt->var[220].dubitoko != 3)
		{
			puts ("(NE EKZISTANTA)");
			continue;
		}
		switch (dvg_varmapo (i))
		{
		case DVG_DT_B:
			printf ("B: %u", skt->var[i].bitoko);
			break;
		case DVG_DT_BS:
			printf ("BS: %u", skt->var[i].dubitoko);
			break;
		case DVG_DT_BL:
			printf ("BL: %lu", skt->var[i].kvarbitoko);
			break;
		case DVG_DT_BD:
			printf ("BD: %lg", skt->var[i].duglitajxo);
			break;
		case DVG_DT_H:
			printf ("H: %i.%i.%li", skt->var[i].traktilo.kodo,
				skt->var[i].traktilo.kiom, skt->var[i].traktilo.valoro);
			break;
		case DVG_DT_T:
			printf ("T: \"%s\"", skt->var[i].teksto);
			break;
		case DVG_DT_CMC:
			printf ("CMC: %u", skt->var[i].dubitoko);
			break;
		case DVG_DT_2RD:
			printf ("X: %lg\t", skt->var[i].xy[0]);
			printf ("Y: %lg", skt->var[i].xy[1]);
			break;
		case DVG_DT_3BD:
			printf ("X: %lg\t", skt->var[i].xyz[0]);
			printf ("Y: %lg\t", skt->var[i].xyz[1]);
			printf ("Z: %lg", skt->var[i].xyz[2]);
			break;
		default:
			printf ("Ne traktebla tipo: %i (var: %i)\n", dvg_varmapo (i), i);
		}
		puts ("");
	}
	puts ("");

	puts ("**************************************************");
	puts ("Sekcio KLASOJ");
	puts ("**************************************************");
	for (i = 0; i < skt->klaso_kiom; i++)
	{
		printf ("Klaso: [%02u]\n", i);
		printf ("\tNumero: %u\n", skt->klaso[i].numero);
		printf ("\tVersio: %u\n", skt->klaso[i].versio);
		printf ("\tAp Nomo: \"%s\"\n", skt->klaso[i].apnomo);
		printf ("\tC++ Nomo: \"%s\"\n", skt->klaso[i].cpliplinomo);
		printf ("\tDXF Nomo: \"%s\"\n", skt->klaso[i].dxfnomo);
		printf ("\tEstis fantomo: \"%s\"\n", skt->klaso[i].estisfantomo ? "Jes" : "Ne");
		printf ("\tEroId: %u\n", skt->klaso[i].eroid);
	}
	puts ("");

	puts ("**************************************************");
	puts ("Sekcio OBJEKTOJ");
	puts ("**************************************************");
	for (i = 0; i < skt->objekto_kiom; i++)
	{
		Dvg_Objekto *obj;

		printf ("(%u) ", i);
		obj = &skt->objekto[i];

		printf ("Tipo: %s (%03i)\t", obj->tipo > 80 ? (obj->tipo == skt->dvg_ot_layout ? "LAYOUT" : "??") : dvg_obtipo[obj->tipo], obj->tipo);
		printf ("Grandeco: %u\t", obj->grandeco);
		printf ("Traktilo: (%lu)\t", obj->trakt);
		printf ("Super-tipo: ");
		switch (obj->supertipo)
		{
		case DVG_OST_ESTAJXO:
			puts ("estajxo");
			dvg_montri_estajxo (obj->tio.estajxo);
			break;
		case DVG_OST_ORDINARAJXO:
			puts ("ordinarajxo");
			dvg_montri_ordinarajxo (obj->tio.ordinarajxo);
			break;
		default:
			puts ("nekonatajxo");
			continue;
		}

		switch (obj->tipo)
		{
		case DVG_OT_TEXT:
			dvg_montri_TEXT (obj->tio.estajxo->tio.TEXT);
			break;
		case DVG_OT_ATTRIB:
			dvg_montri_ATTRIB (obj->tio.estajxo->tio.ATTRIB);
			break;
		case DVG_OT_ATTDEF:
			dvg_montri_ATTDEF (obj->tio.estajxo->tio.ATTDEF);
			break;
		case DVG_OT_BLOCK:
			dvg_montri_BLOCK (obj->tio.estajxo->tio.BLOCK);
			break;
		case DVG_OT_ENDBLK:
		case DVG_OT_SEQEND:
			break;
		case DVG_OT_INSERT:
			dvg_montri_INSERT (obj->tio.estajxo->tio.INSERT);
			break;
		case DVG_OT_MINSERT:
			dvg_montri_MINSERT (obj->tio.estajxo->tio.MINSERT);
			break;
		case DVG_OT_VERTEX_2D:
			dvg_montri_VERTEX_2D (obj->tio.estajxo->tio.VERTEX_2D);
			break;
		case DVG_OT_VERTEX_3D:
		case DVG_OT_VERTEX_MESH:
		case DVG_OT_VERTEX_PFACE:
			dvg_montri_VERTEX_3D (obj->tio.estajxo->tio.VERTEX_3D);
			break;
		case DVG_OT_VERTEX_PFACE_FACE:
			dvg_montri_VERTEX_PFACE_FACE (obj->tio.estajxo->tio.VERTEX_PFACE_FACE);
			break;
		case DVG_OT_POLYLINE_2D:
			dvg_montri_POLYLINE_2D (obj->tio.estajxo->tio.POLYLINE_2D);
			break;
		case DVG_OT_POLYLINE_3D:
			dvg_montri_POLYLINE_3D (obj->tio.estajxo->tio.POLYLINE_3D);
			break;
		case DVG_OT_ARC:
			dvg_montri_ARC (obj->tio.estajxo->tio.ARC);
			break;
		case DVG_OT_CIRCLE:
			dvg_montri_CIRCLE (obj->tio.estajxo->tio.CIRCLE);
			break;
		case DVG_OT_LINE:
			dvg_montri_LINE (obj->tio.estajxo->tio.LINE);
			break;
		case DVG_OT_POINT:
			dvg_montri_POINT (obj->tio.estajxo->tio.POINT);
			break;
		case DVG_OT_ELLIPSE:
			dvg_montri_ELLIPSE (obj->tio.estajxo->tio.ELLIPSE);
			break;
		case DVG_OT_RAY:
		case DVG_OT_XLINE:
			dvg_montri_RAY (obj->tio.estajxo->tio.RAY);
			break;
		case DVG_OT_MTEXT:
			dvg_montri_MTEXT (obj->tio.estajxo->tio.MTEXT);
			break;
		case DVG_OT_LAYER:
			dvg_montri_LAYER (obj->tio.ordinarajxo->tio.LAYER);
			break;
		default:
			if (obj->tipo == skt->dvg_ot_layout)
				dvg_montri_LAYOUT (obj->tio.ordinarajxo->tio.LAYOUT);
			else
				continue;
		}

		dvg_montri_traktref (obj);

		puts ("");
	}

	puts ("**************************************************");
	puts ("Sekcio DUA KAP-DATENARO ");
	puts ("**************************************************");
	for (i = 0; i < 14; i++)
	{
		printf ("Rikordo[%02i] Longo: %u\tCxeno:", i, skt->duakapo.traktrik[i].kiom);
		for (j = 0; j < skt->duakapo.traktrik[i].kiom; j++)
			printf (" %02X", skt->duakapo.traktrik[i].cxeno[j]);
		puts ("");
	}
	puts ("");

	puts ("**************************************************");
	puts ("Sekcio MEZURO (MEASUREMENT)");
	puts ("**************************************************");
	printf ("MEZURO: 0x%08X\n", skt->mezuro);
	puts ("");
}

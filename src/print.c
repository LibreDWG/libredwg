/*****************************************************************************/
/*  LibreDWG - Free DWG library                                              */
/*  http://code.google.com/p/libredwg/                                       */
/*                                                                           */
/*    based on LibDWG - Free DWG read-only library                           */
/*    http://sourceforge.net/projects/libdwg                                 */
/*    originally written by Felipe Castro <felipo at users.sourceforge.net>  */
/*                                                                           */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.                  */
/*  Copyright (C) 2009 Felipe Sanches <jucablues@users.sourceforge.net>      */
/*  Copyright (C) 2009 Rodrigo Rodrigues da Silva <rodrigopitanga@gmail.com> */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#include "print.h"
#include <stdio.h>

/*------------------------------------------------------------------------------
 * Private functions
 */

void
dwg_print_entity (Dwg_Object_Entity * ent)
{
	printf ("Bitsize: %lu\n", ent->bitsize);
	printf ("Vera handle: %i.%i.%lu\n", ent->handle.code, ent->handle.size,
		ent->handle.value);
	printf ("Kroma datenaro: %lu B\n", (long unsigned int) ent->extended_size);
	printf ("Ĉu picture?: %s", ent->picture_exists ? "Yes" : "Ne");
	if (ent->picture_exists)
		printf ("\tSize: %lu B\n", ent->picture_size);
	else
		puts ("");
	printf ("Reĝimo: %i\n", ent->regime);
	printf ("Kiom reagiloj: %lu\n", ent->reagilo_size);
	printf ("Ĉu senligiloj?: %s\n", ent->senligiloj ? "Yes" : "Ne");
	printf ("Koloro: %u\n", ent->colour);
	printf ("Skalo de linetype: %1.13g\n", ent->linetype_scale);
	printf ("Linitype: 0x%02X\n", ent->linetype);
	printf ("Printstilo: 0x%02X\n", ent->plot_style);
	printf ("Malvidebleco: 0x%04X\n", ent->invisible);
	printf ("Linithickness: %u\n", ent->lineweight);
}

void
dwg_print_object (Dwg_Object_Object *ord)
{
	printf ("Bitsize: %lu\n", ord->bitsize);
	printf ("Vera handle: %i.%i.%lu\n", ord->handle.code, ord->handle.size,
		ord->handle.value);
	printf ("Kroma datenaro: %lu B\n", (long unsigned int) ord->extended_size);
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
dwg_print_UNUSED (Dwg_Entity_UNUSED * ent)
{
}

void
dwg_print_TEXT (Dwg_Entity_TEXT * ent)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", ent->dataflags);

	printf ("\tLeviĝo: %1.13g\n", ent->elevation);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", ent->alignment.x, ent->alignment.y);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tKlina angulo: %1.13g\n", ent->oblique_ang);
	printf ("\tTurna angulo: %1.13g\n", ent->rotation_ang);
	printf ("\tAlteco: %1.13g\n", ent->height);
	printf ("\tLarĝ-factor: %1.13g\n", ent->width_factor);
	printf ("\tTeksto: \"%s\"\n", ent->text);
	printf ("\tGeneracio: %u\n", ent->generation);
	printf ("\tĜisrandigo (horiz.): %u\n", ent->alignment.h);
	printf ("\tĜisrandigo (vert.): %u\n", ent->alignment.v);
}

void
dwg_print_ATTRIB (Dwg_Entity_ATTRIB * ent)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", ent->dataflags);

	printf ("\tLeviĝo: %1.13g\n", ent->elevation);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", ent->alignment.x, ent->alignment.y);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tKlina angulo: %1.13g\n", ent->oblique_ang);
	printf ("\tTurna angulo: %1.13g\n", ent->rotation_ang);
	printf ("\tAlteco: %1.13g\n", ent->height);
	printf ("\tLarĝ-factor: %1.13g\n", ent->width_factor);
	printf ("\tTeksto: \"%s\"\n", ent->text);
	printf ("\tGeneracio: %u\n", ent->generation);
	printf ("\tĜisrandigo (horiz.): %u\n", ent->alignment.h);
	printf ("\tĜisrandigo (vert.): %u\n", ent->alignment.v);
	printf ("\tEtikedo: %s\n", ent->tag);
	printf ("\tKamp-length: %i (ne uzata)\n", ent->field_length);
	printf ("\tIndikiloj: 0x%02x\n", ent->flags);
}

void
dwg_print_ATTDEF (Dwg_Entity_ATTDEF * ent)
{
	printf ("\tDatumar-indikilo: 0x%02x\n", ent->dataflags);

	printf ("\tLeviĝo: %1.13g\n", ent->elevation);
	printf ("\tEnmeta punkto: (%1.13g, %1.13g)\n", ent->x0, ent->y0);
	printf ("\tEnmeta ĝisrandigo: (%1.13g, %1.13g)\n", ent->alignment.x, ent->alignment.y);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tKlina angulo: %1.13g\n", ent->oblique_ang);
	printf ("\tTurna angulo: %1.13g\n", ent->rotation_ang);
	printf ("\tAlteco: %1.13g\n", ent->height);
	printf ("\tLarĝ-factor: %1.13g\n", ent->width_factor);
	printf ("\tTeksto: \"%s\"\n", ent->text);
	printf ("\tGeneracio: %u\n", ent->generation);
	printf ("\tĜisrandigo (horiz.): %u\n", ent->alignment.h);
	printf ("\tĜisrandigo (vert.): %u\n", ent->alignment.v);
	printf ("\tEtikedo: %s\n", ent->tag);
	printf ("\tKamp-length: %i (ne uzata)\n", ent->field_length);
	printf ("\tIndikilo: 0x%02x\n", ent->flags);
	printf ("\tInvitilo (prompt): %s\n", ent->prompt);
}

void
dwg_print_BLOCK (Dwg_Entity_BLOCK * ent)
{
	printf ("\tName: %s\n", ent->name);
}

void
dwg_print_ENDBLK (Dwg_Entity_ENDBLK * ent)
{
}

void
dwg_print_SEQEND (Dwg_Entity_SEQEND * ent)
{
}

void
dwg_print_INSERT (Dwg_Entity_INSERT * ent)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", ent->scale_flag);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", ent->scale.x, ent->scale.y, ent->scale.z);
	printf ("\tTurna angulo: %1.13g\n", ent->rotation_ang);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", ent->has_attribs ? "Yes" : "Ne");
}

void
dwg_print_MINSERT (Dwg_Entity_MINSERT * ent)
{
	printf ("\tEnmeta punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tSkal-indikilo: 0x%02x\n", ent->scale_flag);
	printf ("\tSkalo: (%1.13g, %1.13g, %1.13g)\n", ent->scale.x, ent->scale.y, ent->scale.z);
	printf ("\tTurna angulo: %1.13g\n", ent->rotation_ang);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tĈu kun ATTRIB-oj?: %s\n", ent->has_attribs ? "Yes" : "Ne");
	printf ("\tKolumnoj: %02i\tInterspaco: %1.13g\n", ent->column.size, ent->column.dx);
	printf ("\t  Linioj: %02i\tInterspaco: %1.13g\n", ent->line.size, ent->line.dy);
}

void
dwg_print_VERTEX_2D (Dwg_Entity_VERTEX_2D * ent)
{
	printf ("\tIndikiloj: 0x%02x\n", ent->flags);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tEklarĝo: %1.13g\n", ent->start_width);
	printf ("\tFinlarĝo: %1.13g\n", ent->end_width);
	printf ("\tProtuberanco: %1.13g\n", ent->bulge);
	printf ("\tTanĝento dir.: %1.13g\n", ent->tangent_dir);
}

void
dwg_print_VERTEX_3D (Dwg_Entity_VERTEX_3D * ent)
{
	printf ("\tIndikiloj: 0x%02x\n", ent->flags);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
}

void
dwg_print_VERTEX_MESH (Dwg_Entity_VERTEX_MESH * ent)
{
	printf ("\tIndikiloj: 0x%02x\n", ent->flags);
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
}

void
dwg_print_VERTEX_PFACE (Dwg_Entity_VERTEX_PFACE * ent)
{
	printf ("\tIndikiloj: 0x%02x\n", ent->flags);
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
	printf ("\tIndikiloj: 0x%02x\n", ent->flags);
	printf ("\tKurbtype: 0x%02x\n", ent->curve_type);
	printf ("\tEklarĝo: %1.13g\n", ent->start_width);
	printf ("\tFinlarĝo: %1.13g\n", ent->end_width);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tLeviĝo: %1.13g\n", ent->elevation);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
}

void
dwg_print_POLYLINE_3D (Dwg_Entity_POLYLINE_3D * ent)
{
	printf ("\tIndikiloj: 0x%02x / 0x%02x\n", ent->flags_1, ent->flags_2);
}

void
dwg_print_ARC (Dwg_Entity_ARC * ent)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tRadiuso: %1.13g\n", ent->radius);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tEkangulo: %1.13g\n", ent->start_angle);
	printf ("\tFinangulo: %1.13g\n", ent->end_angle);
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
	printf ("\tĈu nur 2D?: %s\n", ent->Zs_are_zero ? "Yes" : "Ne");
	printf ("\t1-a punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0,
		ent->Zs_are_zero ? 0 : ent->z0);
	printf ("\t2-a punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1,
		ent->Zs_are_zero ? 0 : ent->z1);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
}

void
dwg_print_DIMENSION_ORDINATE (Dwg_Entity_DIMENSION_ORDINATE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_DIMENSION_LINEAR (Dwg_Entity_DIMENSION_LINEAR * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_DIMENSION_ALIGNED (Dwg_Entity_DIMENSION_ALIGNED * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_DIMENSION_ANG3PT (Dwg_Entity_DIMENSION_ANG3PT * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_DIMENSION_ANG2LN (Dwg_Entity_DIMENSION_ANG2LN * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_DIMENSION_RADIUS (Dwg_Entity_DIMENSION_RADIUS * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_DIMENSION_DIAMETER (Dwg_Entity_DIMENSION_DIAMETER * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_POINT (Dwg_Entity_POINT *ent)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x, ent->y, ent->z);
	printf ("\tthickness: %1.13g\n", ent->thickness);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tX-angulo: %1.13g\n", ent->x_ang);
}

void
dwg_print_3DFACE (Dwg_Entity_3DFACE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_POLYLINE_PFACE (Dwg_Entity_POLYLINE_PFACE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_POLYLINE_MESH (Dwg_Entity_POLYLINE_MESH * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_SOLID (Dwg_Entity_SOLID * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_TRACE (Dwg_Entity_TRACE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_SHAPE (Dwg_Entity_SHAPE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_VIEWPORT (Dwg_Entity_VIEWPORT * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_ELLIPSE (Dwg_Entity_ELLIPSE *ent)
{
	printf ("\tCentra punkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tDuonĉef-aksa vektoro: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tRadius-proporcion: %1.13g\n", ent->axis_ratio);
	printf ("\tEkangulo: %1.13g\n", ent->start_angle);
	printf ("\tFinangulo: %1.13g\n", ent->end_angle);
}

void
dwg_print_SPLINE (Dwg_Entity_SPLINE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_REGION (Dwg_Entity_REGION * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_3DSOLID (Dwg_Entity_3DSOLID * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_BODY (Dwg_Entity_BODY * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_RAY (Dwg_Entity_RAY *ent)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tVektoro: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
}

void
dwg_print_XLINE (Dwg_Entity_XLINE * ent)
{
	printf ("\tPunkto: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tVektoro: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);
}

void
dwg_print_DICTIONARY (Dwg_Object_DICTIONARY *obj)
{
	int i;

	printf ("\tSize: %lu\n", (long unsigned int) obj->size);
	printf ("\tCloning flag: 0x%X\n", obj->cloning);
	printf ("\tHard Owner flag: 0x%X\n", obj->hard_owner);
	printf ("\tNames:");
	for (i = 0; i < obj->size; i++)
		printf (" %s,", obj->name[i]);
	printf ("\n");
}

void
dwg_print_MTEXT (Dwg_Entity_MTEXT *ent)
{
	printf ("\tInsertion pt3: (%1.13g, %1.13g, %1.13g)\n", ent->x0, ent->y0, ent->z0);
	printf ("\tExtrusion: (%1.13g, %1.13g, %1.13g)\n", ent->extrusion.x, ent->extrusion.y,
		ent->extrusion.z);
	printf ("\tX-axis dir: (%1.13g, %1.13g, %1.13g)\n", ent->x1, ent->y1, ent->z1);

	//TODO: a way to figure out DWG version 
/*	if (version >= R_2007){
    	printf ("\tRect height: %1.13g\n", ent->rect_height);	
	}
*/
	printf ("\tRect width: %1.13g\n", ent->rect_width);
	printf ("\tText height: %1.13g\n", ent->text_height);
	printf ("\tAttachment: 0x%0x\n", ent->attachment);
	printf ("\tDrawing dir: 0x%0x\n", ent->drawing_dir);
	printf ("\tText: %s\n", ent->text);
//	if (version >= R_2000){
    	printf ("\tLinespacing Style: 0x%0x\n", ent->linespace_style);
	    printf ("\tLinespacing Factor: %1.13g\n", (double) ent->linespace_factor);
	    printf ("\tUnknown bit: %s\n", ent->unknown_bit ? "ON":"OFF");
//  }
//	if (version >= R_2000){
//      printf ("\tUnknown long: %f\n", ent->unknown_long);
//  }
}

void
dwg_print_LEADER (Dwg_Entity_LEADER * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_TOLERANCE (Dwg_Entity_TOLERANCE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_MLINE (Dwg_Entity_MLINE * ent)
{
    //TODO: implement me!
    puts("print not implemented!");
}

void
dwg_print_BLOCK_CONTROL (Dwg_Object_BLOCK_CONTROL *obj)
{
	printf ("\tSize: %u\n", obj->size);
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
	printf ("\tAgord-name: %s\n", ord->page.agordo);
	printf ("\tPrintilo: %s\n", ord->page.printilo);
	printf ("\tPrint-flags: 0x%0x\n", ord->page.flags);
	printf ("\tMarĝenoj. maldekstre: %1.13g; malsupre: %1.13g; dekstre: %1.13g; supre: %1.13g\n",
		ord->page.maldekstre,
		ord->page.malsupre,
		ord->page.dekstre,
		ord->page.supre);
	printf ("\tLargxeco: %1.13g\n", ord->page.width);
	printf ("\tAlteco: %1.13g\n", ord->page.height);
	printf ("\tSize: %s\n", ord->page.size);
	printf ("\tDeŝovo: (%1.13g, %1.13g)\n", ord->page.dx, ord->page.dy);
	printf ("\tUnuoj: %u\n", ord->page.unuoj);
	printf ("\tRotacio: %u\n", ord->page.rotacio);
	printf ("\tType: %u\n", ord->page.type);
	printf ("\tPrint-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n",
		ord->page.x_min,
		ord->page.y_min,
		ord->page.x_max,
		ord->page.y_max);
	printf ("\tPaĝ-name: %s\n", ord->page.name);
	printf ("\tSkal-proporcio: %u:%u\n", (unsigned int) ord->page.scale.A, (unsigned int) ord->page.scale.B);
	printf ("\tStilfolio: %s\n", ord->page.stilfolio);
	printf ("\tSkal-type: %u\n", ord->page.scale.type);
	printf ("\tSkal-factor: %u\n", (unsigned int) ord->page.scale.factor);
	printf ("\tPaĝ-origino: (%1.13g, %1.13g)\n", ord->page.x0, ord->page.y0);
	puts ("");
	printf ("\tAranĝ-name: %s\n", ord->name);
	printf ("\tOrdo: %u\n", ord->ordo);
	printf ("\tIndikiloj: 0x%0x\n", ord->flags);
	printf ("\tUCS-origino: (%1.13g, %1.13g, %1.13g)\n", ord->x0, ord->y0, ord->z0);
	printf ("\tAranĝ-limoj: (%1.13g, %1.13g) / (%1.13g, %1.13g)\n",
		ord->x_min,
		ord->y_min,
		ord->x_max,
		ord->y_max);
	printf ("\tEnmeto: (%1.13g, %1.13g, %1.13g)\n", ord->enmeto.x0, ord->enmeto.y0, ord->enmeto.z0);
	printf ("\tUCS-axis_X: (%1.13g, %1.13g, %1.13g)\n", ord->axis_X.x0, ord->axis_X.y0, ord->axis_X.z0);
	printf ("\tUCS-axis_Y: (%1.13g, %1.13g, %1.13g)\n", ord->axis_Y.x0, ord->axis_Y.y0, ord->axis_Y.z0);
	printf ("\tLevigxo: %1.13g\n", ord->elevation);
	printf ("\tRigard-type: %u\n", ord->rigardtype);
	printf ("\tMinimumo: (%1.13g, %1.13g, %1.13g)\n",
		ord->limo.x_min,
		ord->limo.y_min,
		ord->limo.z_min);
	printf ("\tMaksimumo: (%1.13g, %1.13g, %1.13g)\n",
		ord->limo.x_max,
		ord->limo.y_max,
		ord->limo.z_max);
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
	"DIMENSION_ORDINATE", "DIMENSION_LINEAR", "DIMENSION_ALIGNED", "DIMENSION_ANG3PT",
        "DIMENSION_ANG2LN", "DIMENSION_RADIUS", "DIMENSION_DIAMETER", "POINT", "3DFACE", "POLYLINE_PFACE",
       	"POLYLINE_MESH", "SOLID", "TRACE", "SHAPE",	"VIEWPORT", "ELLIPSE", "SPLINE", "REGION", "3DSOLID", "BODY",
       	"RAY", "XLINE", "DICTIONARY", "NULL_43", "MTEXT", "LEADER", "TOLERANCE", "MLINE", "BLOCK_CONTROL", "BLOCK_HEADER",
       	"LAYER_CONTROL", "LAYER", "STYLE_CONTROL", "STYLE", "NULL_54", "NULL_55", "LTYPE_CONTROL", "LTYPE", "NULL_58", "NULL_59",
	"VIEW_CONTROL", "VIEW", "UCS_CONTROL", "UCS", "VPORT_CONTROL", "VPORT", "APPID_CONTROL", "APPID", "DIMSTYLE_CONTROL", "DIMSTYLE",
	"VP_ENT_HDR_CTRL", "VP_ENT_HDR", "GROUP", "MLINESTYLE", "DICTIONARYVAR", "DICTIONARYWDLFT", "HATCH", "IDBUFFER", "IMAGE", "IMAGEDEF",
	"IMAGEDEFREACTOR"};


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
			printf ("H: %i.%i.%li", dwg_struct->var[i].handle.code,
				dwg_struct->var[i].handle.size, dwg_struct->var[i].handle.value);
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
		printf ("\tWas ghost: \"%s\"\n", dwg_struct->class[i].wasazombie ? "Yes" : "No");
		printf ("\tEroId: %u\n", dwg_struct->class[i].item_class_id);
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
		printf ("Handle: (%lu)\t", obj->trakt);
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

		switch  (obj->type)
		{
                    case DWG_TYPE_UNUSED:
                            break;
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
                            dwg_print_VERTEX_3D (obj->tio.entity->tio.VERTEX_3D);
                            break;
                    case DWG_TYPE_VERTEX_MESH:
                            dwg_print_VERTEX_MESH (obj->tio.entity->tio.VERTEX_MESH);
                            break;
                    case DWG_TYPE_VERTEX_PFACE:
                            dwg_print_VERTEX_PFACE (obj->tio.entity->tio.VERTEX_PFACE);
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
                    case DWG_TYPE_DIMENSION_ORDINATE:
                            dwg_print_DIMENSION_ORDINATE (obj->tio.entity->tio.DIMENSION_ORDINATE);
                            break;
                    case DWG_TYPE_DIMENSION_LINEAR:
                            dwg_print_DIMENSION_LINEAR (obj->tio.entity->tio.DIMENSION_LINEAR);
                            break;
                    case DWG_TYPE_DIMENSION_ALIGNED:
                            dwg_print_DIMENSION_ALIGNED (obj->tio.entity->tio.DIMENSION_ALIGNED);
                            break;
                    case DWG_TYPE_DIMENSION_ANG3PT:
                            dwg_print_DIMENSION_ANG3PT (obj->tio.entity->tio.DIMENSION_ANG3PT);
                            break;
                    case DWG_TYPE_DIMENSION_ANG2LN:
                            dwg_print_DIMENSION_ANG2LN (obj->tio.entity->tio.DIMENSION_ANG2LN);
                            break;
                    case DWG_TYPE_DIMENSION_RADIUS:
                            dwg_print_DIMENSION_RADIUS (obj->tio.entity->tio.DIMENSION_RADIUS);
                            break;
                    case DWG_TYPE_DIMENSION_DIAMETER:
                            dwg_print_DIMENSION_DIAMETER (obj->tio.entity->tio.DIMENSION_DIAMETER);
                            break;
                    case DWG_TYPE_POINT:
                            dwg_print_POINT (obj->tio.entity->tio.POINT);
                            break;
                    case DWG_TYPE_3DFACE:
                            dwg_print_3DFACE (obj->tio.entity->tio._3DFACE);
                            break;
                    case DWG_TYPE_POLYLINE_PFACE:
                            dwg_print_POLYLINE_PFACE (obj->tio.entity->tio.POLYLINE_PFACE);
                            break;
                    case DWG_TYPE_POLYLINE_MESH:
                            dwg_print_POLYLINE_MESH (obj->tio.entity->tio.POLYLINE_MESH);
                            break;
                    case DWG_TYPE_SOLID:
                            dwg_print_SOLID (obj->tio.entity->tio.SOLID);
                            break;
                    case DWG_TYPE_TRACE:
                            dwg_print_TRACE (obj->tio.entity->tio.TRACE);
                            break;
                    case DWG_TYPE_SHAPE:
                            dwg_print_SHAPE (obj->tio.entity->tio.SHAPE);
                            break;
                    case DWG_TYPE_VIEWPORT:
                            dwg_print_VIEWPORT (obj->tio.entity->tio.VIEWPORT);
                            break;
                    case DWG_TYPE_ELLIPSE:
                            dwg_print_ELLIPSE (obj->tio.entity->tio.ELLIPSE);
                            break;
                    case DWG_TYPE_SPLINE:
                            dwg_print_SPLINE (obj->tio.entity->tio.SPLINE);
                            break;
                    case DWG_TYPE_REGION:
                            dwg_print_REGION (obj->tio.entity->tio.REGION);
                            break;
                    case DWG_TYPE_3DSOLID:
                            dwg_print_3DSOLID (obj->tio.entity->tio._3DSOLID);
                            break;
                    case DWG_TYPE_BODY:
                            dwg_print_BODY (obj->tio.entity->tio.BODY);
                            break;
                    case DWG_TYPE_RAY:
                            dwg_print_RAY (obj->tio.entity->tio.RAY);
                            break;
                    case DWG_TYPE_XLINE:
                            dwg_print_XLINE (obj->tio.entity->tio.XLINE);
                            break;
                    case DWG_TYPE_DICTIONARY:
                            dwg_print_DICTIONARY (obj->tio.object->tio.DICTIONARY);
                            break;
                    case DWG_TYPE_MTEXT:
                            dwg_print_MTEXT (obj->tio.entity->tio.MTEXT);
                            break;
                    case DWG_TYPE_LEADER:
                            dwg_print_LEADER (obj->tio.entity->tio.LEADER);
                            break;
                    case DWG_TYPE_TOLERANCE:
                            dwg_print_TOLERANCE (obj->tio.entity->tio.TOLERANCE);
                            break;
                    case DWG_TYPE_MLINE:
                            dwg_print_MLINE (obj->tio.entity->tio.MLINE);
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

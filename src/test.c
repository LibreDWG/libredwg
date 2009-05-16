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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bite.h"
#include "dwg.h"

#define CXEN_LONGO (80)

Bit_Cxeno dat;
Bit_Cxeno dat_2;

int testo_dvg_c (char *dosiero);

int
main (int argc, char *argv[])
{
	if (argc > 1)
		return (testo_dvg_c (argv[1]));
	else
		return (testo_dvg_c (NULL));
}

#define DOSIERO "ekzemplo"
int
testo_dvg_c (char *dosiero)
{
	int malsukceso = 0;
	Dvg_Strukturo strukt;

	/* Komenci testojn
	 */
	puts (" ---------------------------------------> Oni testas: \"dvg.c\"");

	if (dosiero)
		malsukceso = malsukceso || dvg_legi_dosiero (dosiero, &strukt);
	else
		malsukceso = malsukceso || dvg_legi_dosiero (DOSIERO ".dwg", &strukt);

	if (!malsukceso)
	{
		dvg_montri (&strukt);
		/*
		   unlink ("nova_rezulto.dwg");
		   malsukceso = malsukceso || dvg_skribi_dosiero ("nova_rezulto.dwg", &strukt);
		 */
	}

	if (malsukceso)
		puts (" \"dvg.c\" ==> Malsukcesis...");
	else
	{
		puts (" \"dvg.c\" ==> SUKCESO!");
	}

	return malsukceso;
}

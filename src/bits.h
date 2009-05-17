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

///  Funkcioj por legi/skribi kompaktitajn datenerojn en bitok-cxenoj.
/**
 La pozicioj de bitoj en cxiu bitoko (bajto) estas numeritaj laux jene:

 pozicio: 01234567 01234567 01234567 ...
 bitoj:   76543210 76543210 76543210 ...
          \______/ \______/ \______/
          bajto 1  bajto 2  bajto 3  ...
*/

#ifndef BITS_H
#define BITS_H

#ifndef DWG_H
#include "dwg.h"
#endif

/**
 Strukturo por enteni la krudan datenaron de dwg-dosieroj.
 */
typedef struct _bit_cxeno
{
	unsigned char *cxeno;
	long unsigned int kiom;
	long unsigned int bajto;
	unsigned char bito;
} Bit_Cxeno;

/* Funkcioj por manipuli krudan dateneron.
 */
void bit_ref_salti (Bit_Cxeno * dat, int salto);

unsigned char bit_legi_B (Bit_Cxeno * bit_datenaro);

void bit_skribi_B (Bit_Cxeno * bit_datenaro, unsigned char valoro);

unsigned char bit_legi_BB (Bit_Cxeno * bit_datenaro);

void bit_skribi_BB (Bit_Cxeno * bit_datenaro, unsigned char valoro);

unsigned char bit_legi_RC (Bit_Cxeno * bit_datenaro);

void bit_skribi_RC (Bit_Cxeno * bit_datenaro, unsigned char valoro);

unsigned int bit_legi_RS (Bit_Cxeno * bit_datenaro);

void bit_skribi_RS (Bit_Cxeno * bit_datenaro, unsigned int valoro);

long unsigned int bit_legi_RL (Bit_Cxeno * bit_datenaro);

void bit_skribi_RL (Bit_Cxeno * bit_datenaro, long unsigned int valoro);

double bit_legi_RD (Bit_Cxeno * bit_datenaro);

void bit_skribi_RD (Bit_Cxeno * bit_datenaro, double valoro);

/* Funkcioj por manipuli kompaktecan datenaron.
 */
unsigned int bit_legi_BS (Bit_Cxeno * bit_datenaro);

void bit_skribi_BS (Bit_Cxeno * bit_datenaro, unsigned int valoro);

long unsigned int bit_legi_BL (Bit_Cxeno * bit_datenaro);

void bit_skribi_BL (Bit_Cxeno * bit_datenaro, long unsigned int valoro);

double bit_legi_BD (Bit_Cxeno * bit_datenaro);

void bit_skribi_BD (Bit_Cxeno * bit_datenaro, double valoro);

long int bit_legi_MC (Bit_Cxeno * bit_datenaro);

void bit_skribi_MC (Bit_Cxeno * bit_datenaro, long int valoro);

long unsigned int bit_legi_MS (Bit_Cxeno * bit_datenaro);

void bit_skribi_MS (Bit_Cxeno * bit_datenaro, long unsigned int valoro);

void bit_legi_BE (Bit_Cxeno * bit_datenaro, double *x, double *y, double *z);

void bit_skribi_BE (Bit_Cxeno * bit_datenaro, double x, double y, double z);

double bit_legi_DD (Bit_Cxeno * bit_datenaro, double antauxdifinajxo);

void bit_skribi_DD (Bit_Cxeno * bit_datenaro, double valoro, double antauxdifinajxo);

double bit_legi_BT (Bit_Cxeno * bit_datenaro);

void bit_skribi_BT (Bit_Cxeno * bit_datenaro, double valoro);

int bit_legi_H (Bit_Cxeno * bit_datenaro, Dwg_Traktilo * traktilo);

void bit_skribi_H (Bit_Cxeno * dat, Dwg_Traktilo * traktilo);

unsigned int bit_legi_CRC (Bit_Cxeno * bit_datenaro);

int bit_konfirmi_CRC (Bit_Cxeno * bit_datenaro, long unsigned int ekadreso, unsigned int semo);

unsigned int bit_krei_CRC (Bit_Cxeno * bit_datenaro, long unsigned int ekadreso, unsigned int semo);

unsigned char *bit_legi_T (Bit_Cxeno * bit_datenaro);

void bit_skribi_T (Bit_Cxeno * bit_datenaro, unsigned char *valoro);

long unsigned int bit_legi_L (Bit_Cxeno * dat);

void bit_skribi_L (Bit_Cxeno * dat, long unsigned int valoro);

int bit_sercxi_gardostaranto (Bit_Cxeno * dat, unsigned char gdst[16]);

void bit_skribi_gardostaranto (Bit_Cxeno * dat, unsigned char gdst[16]);

void bit_cxeno_rezervi (Bit_Cxeno * dat);

void bit_montri (Bit_Cxeno * dat, long unsigned int kiom);

void bit_esplori_cxeno (Bit_Cxeno * dat, long unsigned int kiom);

#endif

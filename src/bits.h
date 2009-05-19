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

///  Funkcioj por read/write kompaktitajn datenerojn en bitok-cxenoj.
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

unsigned char bit_read_B (Bit_Cxeno * bit_datenaro);

void bit_write_B (Bit_Cxeno * bit_datenaro, unsigned char value);

unsigned char bit_read_BB (Bit_Cxeno * bit_datenaro);

void bit_write_BB (Bit_Cxeno * bit_datenaro, unsigned char value);

unsigned char bit_read_RC (Bit_Cxeno * bit_datenaro);

void bit_write_RC (Bit_Cxeno * bit_datenaro, unsigned char value);

unsigned int bit_read_RS (Bit_Cxeno * bit_datenaro);

void bit_write_RS (Bit_Cxeno * bit_datenaro, unsigned int value);

long unsigned int bit_read_RL (Bit_Cxeno * bit_datenaro);

void bit_write_RL (Bit_Cxeno * bit_datenaro, long unsigned int value);

double bit_read_RD (Bit_Cxeno * bit_datenaro);

void bit_write_RD (Bit_Cxeno * bit_datenaro, double value);

/* Funkcioj por manipuli kompaktecan datenaron.
 */
unsigned int bit_read_BS (Bit_Cxeno * bit_datenaro);

void bit_write_BS (Bit_Cxeno * bit_datenaro, unsigned int value);

long unsigned int bit_read_BL (Bit_Cxeno * bit_datenaro);

void bit_write_BL (Bit_Cxeno * bit_datenaro, long unsigned int value);

double bit_read_BD (Bit_Cxeno * bit_datenaro);

void bit_write_BD (Bit_Cxeno * bit_datenaro, double value);

long int bit_read_MC (Bit_Cxeno * bit_datenaro);

void bit_write_MC (Bit_Cxeno * bit_datenaro, long int value);

long unsigned int bit_read_MS (Bit_Cxeno * bit_datenaro);

void bit_write_MS (Bit_Cxeno * bit_datenaro, long unsigned int value);

void bit_read_BE (Bit_Cxeno * bit_datenaro, double *x, double *y, double *z);

void bit_write_BE (Bit_Cxeno * bit_datenaro, double x, double y, double z);

double bit_read_DD (Bit_Cxeno * bit_datenaro, double antauxdifinajxo);

void bit_write_DD (Bit_Cxeno * bit_datenaro, double value, double antauxdifinajxo);

double bit_read_BT (Bit_Cxeno * bit_datenaro);

void bit_write_BT (Bit_Cxeno * bit_datenaro, double value);

int bit_read_H (Bit_Cxeno * bit_datenaro, Dwg_Traktilo * traktilo);

void bit_write_H (Bit_Cxeno * dat, Dwg_Traktilo * traktilo);

unsigned int bit_read_CRC (Bit_Cxeno * bit_datenaro);

int bit_check_CRC (Bit_Cxeno * bit_datenaro, long unsigned int ekadreso, unsigned int semo);

unsigned int bit_krei_CRC (Bit_Cxeno * bit_datenaro, long unsigned int ekadreso, unsigned int semo);

unsigned char *bit_read_T (Bit_Cxeno * bit_datenaro);

void bit_write_T (Bit_Cxeno * bit_datenaro, unsigned char *value);

long unsigned int bit_read_L (Bit_Cxeno * dat);

void bit_write_L (Bit_Cxeno * dat, long unsigned int value);

int bit_sercxi_gardostaranto (Bit_Cxeno * dat, unsigned char gdst[16]);

void bit_write_gardostaranto (Bit_Cxeno * dat, unsigned char gdst[16]);

void bit_cxeno_rezervi (Bit_Cxeno * dat);

void bit_print (Bit_Cxeno * dat, long unsigned int kiom);

void bit_esplori_cxeno (Bit_Cxeno * dat, long unsigned int kiom);

#endif

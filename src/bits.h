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

///  Funkcioj por read/write kompaktitajn datenerojn en bitok-chainj.
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
 Structure for DWG-files raw data storage
 */
typedef struct _bit_chain
{
	unsigned char *chain;
	long unsigned int kiom;
	long unsigned int bajto;
	unsigned char bito;
} Bit_Chain;

/* Functions for raw data manipulations.
 */
void bit_ref_salti (Bit_Chain * dat, int salto);

unsigned char bit_read_B (Bit_Chain * bit_datenaro);

void bit_write_B (Bit_Chain * bit_datenaro, unsigned char value);

unsigned char bit_read_BB (Bit_Chain * bit_datenaro);

void bit_write_BB (Bit_Chain * bit_datenaro, unsigned char value);

unsigned char bit_read_RC (Bit_Chain * bit_datenaro);

void bit_write_RC (Bit_Chain * bit_datenaro, unsigned char value);

unsigned int bit_read_RS (Bit_Chain * bit_datenaro);

void bit_write_RS (Bit_Chain * bit_datenaro, unsigned int value);

long unsigned int bit_read_RL (Bit_Chain * bit_datenaro);

void bit_write_RL (Bit_Chain * bit_datenaro, long unsigned int value);

double bit_read_RD (Bit_Chain * bit_datenaro);

void bit_write_RD (Bit_Chain * bit_datenaro, double value);

/* Funkcioj por manipuli kompaktecan datenaron.
 */
unsigned int bit_read_BS (Bit_Chain * bit_datenaro);

void bit_write_BS (Bit_Chain * bit_datenaro, unsigned int value);

long unsigned int bit_read_BL (Bit_Chain * bit_datenaro);

void bit_write_BL (Bit_Chain * bit_datenaro, long unsigned int value);

double bit_read_BD (Bit_Chain * bit_datenaro);

void bit_write_BD (Bit_Chain * bit_datenaro, double value);

long int bit_read_MC (Bit_Chain * bit_datenaro);

void bit_write_MC (Bit_Chain * bit_datenaro, long int value);

long unsigned int bit_read_MS (Bit_Chain * bit_datenaro);

void bit_write_MS (Bit_Chain * bit_datenaro, long unsigned int value);

void bit_read_BE (Bit_Chain * bit_datenaro, double *x, double *y, double *z);

void bit_write_BE (Bit_Chain * bit_datenaro, double x, double y, double z);

double bit_read_DD (Bit_Chain * bit_datenaro, double antauxdifinajxo);

void bit_write_DD (Bit_Chain * bit_datenaro, double value, double antauxdifinajxo);

double bit_read_BT (Bit_Chain * bit_datenaro);

void bit_write_BT (Bit_Chain * bit_datenaro, double value);

int bit_read_H (Bit_Chain * bit_datenaro, Dwg_Traktilo * traktilo);

void bit_write_H (Bit_Chain * dat, Dwg_Traktilo * traktilo);

unsigned int bit_read_CRC (Bit_Chain * bit_datenaro);

int bit_check_CRC (Bit_Chain * bit_datenaro, long unsigned int ekadreso, unsigned int semo);

unsigned int bit_krei_CRC (Bit_Chain * bit_datenaro, long unsigned int ekadreso, unsigned int semo);

unsigned char *bit_read_T (Bit_Chain * bit_datenaro);

void bit_write_T (Bit_Chain * bit_datenaro, unsigned char *value);

long unsigned int bit_read_L (Bit_Chain * dat);

void bit_write_L (Bit_Chain * dat, long unsigned int value);

int bit_sercxi_gardostaranto (Bit_Chain * dat, unsigned char gdst[16]);

void bit_write_gardostaranto (Bit_Chain * dat, unsigned char gdst[16]);

void bit_chain_rezervi (Bit_Chain * dat);

void bit_print (Bit_Chain * dat, long unsigned int kiom);

void bit_esplori_chain (Bit_Chain * dat, long unsigned int kiom);

#endif

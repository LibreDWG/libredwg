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
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

///  Functions for read/write access to data in bit-chains.
/**
 The position of bits within bytes is numerically ordered as depicted below:

 position: 01234567 01234567 01234567 ...
 bits:     76543210 76543210 76543210 ...
           \______/ \______/ \______/
            byte 1   byte 2   byte 3  ...
*/

#ifndef BITS_H
#define BITS_H

#ifndef DWG_H
#include "dwg.h"
#endif

#include "common.h"

/**
 Structure for DWG-files raw data storage
 */
typedef struct _bit_chain
{
	unsigned char *chain;
	long unsigned int size;
	long unsigned int byte;
	unsigned char bit;
	Dwg_Version_Type version;
} Bit_Chain;

/* Functions for raw data manipulations.
 */
void bit_advance_position (Bit_Chain * dat, int salto);

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

/* Functions for manipulating compacted data
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

int bit_read_H (Bit_Chain * bit_datenaro, Dwg_Handle * handle);

void bit_write_H (Bit_Chain * dat, Dwg_Handle * handle);

unsigned int bit_read_CRC (Bit_Chain * bit_datenaro);

int bit_check_CRC (Bit_Chain * bit_datenaro, long unsigned int start_address, unsigned int semo);

unsigned int bit_krei_CRC (Bit_Chain * bit_datenaro, long unsigned int start_address, unsigned int semo);

unsigned char *bit_read_T (Bit_Chain * bit_datenaro);

void bit_write_T (Bit_Chain * bit_datenaro, unsigned char *value);

long unsigned int bit_read_L (Bit_Chain * dat);

void bit_write_L (Bit_Chain * dat, long unsigned int value);

int bit_search_sentinel (Bit_Chain * dat, unsigned char sentinel[16]);

void bit_write_sentinel (Bit_Chain * dat, unsigned char sentinel[16]);

void bit_chain_alloc (Bit_Chain * dat);

void bit_print (Bit_Chain * dat, long unsigned int size);

void bit_esplori_chain (Bit_Chain * dat, long unsigned int size);

#endif

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
#include <ctype.h>

#include "bits.h"

/*------------------------------------------------------------------------------
 * Private functions prototypes
 */
static unsigned int bit_ckr8 (unsigned int dx, unsigned char *adr, long n);

/*------------------------------------------------------------------------------
 * Public functions
 */

/* Pretersalti tiom da bitoj (antauxen aux malantauxen)
 */
void
bit_ref_salti (Bit_Chain * dat, int salto)
{
	int finpoz;

	finpoz = dat->bito + salto;
	if (dat->byte >= dat->kiom - 1 && finpoz > 7)
	{
		dat->bito = 7;
		return;
	}
	dat->bito = finpoz % 8;
	dat->byte += finpoz / 8;
}

/** Read 1 biton.
 */
unsigned char
bit_read_B (Bit_Chain * dat)
{
	unsigned char result;
	unsigned char bitoko;

	bitoko = dat->chain[dat->byte];
	result = (bitoko & (0x80 >> dat->bito)) >> (7 - dat->bito);

	bit_ref_salti (dat, 1);
	return result;
}

/** Write 1 biton.
 */
void
bit_write_B (Bit_Chain * dat, unsigned char value)
{
	if (dat->byte >= dat->kiom - 1)
		bit_chain_rezervi (dat);

	if (value)
		dat->chain[dat->byte] |= 0x80 >> dat->bito;
	else
		dat->chain[dat->byte] &= ~(0x80 >> dat->bito);

	bit_ref_salti (dat, 1);
}

/** Read 2 bitojn.
 */
unsigned char
bit_read_BB (Bit_Chain * dat)
{
	unsigned char result;
	unsigned char bitoko;

	bitoko = dat->chain[dat->byte];
	if (dat->bito < 7)
		result = (bitoko & (0xc0 >> dat->bito)) >> (6 - dat->bito);
	else
	{
		result = (bitoko & 0x01) << 1;
		if (dat->byte < dat->kiom - 1)
		{
			bitoko = dat->chain[dat->byte + 1];
			result |= (bitoko & 0x80) >> 7;
		}
	}

	bit_ref_salti (dat, 2);
	return result;
}

/** Write 2 bitojn.
 */
void
bit_write_BB (Bit_Chain * dat, unsigned char value)
{
	unsigned char masko;
	unsigned char bitoko;

	if (dat->byte >= dat->kiom - 1)
		bit_chain_rezervi (dat);

	bitoko = dat->chain[dat->byte];
	if (dat->bito < 7)
	{
		masko = 0xc0 >> dat->bito;
		dat->chain[dat->byte] = (bitoko & ~masko) | (value << (6 - dat->bito));
	}
	else
	{
		dat->chain[dat->byte] = (bitoko & 0xfe) | (value >> 1);
		if (dat->byte < dat->kiom - 1)
		{
			bitoko = dat->chain[dat->byte + 1];
			dat->chain[dat->byte + 1] = (bitoko & 0x7f) | ((value & 0x01) << 7);
		}
	}

	bit_ref_salti (dat, 2);
}

/** Read 1 bitokon.
 */
unsigned char
bit_read_RC (Bit_Chain * dat)
{
	unsigned char result;
	unsigned char bitoko;

	bitoko = dat->chain[dat->byte];
	if (dat->bito == 0)
		result = bitoko;
	else
	{
		result = bitoko << dat->bito;
		if (dat->byte < dat->kiom - 1)
		{
			bitoko = dat->chain[dat->byte + 1];
			result |= bitoko >> (8 - dat->bito);
		}
	}

	bit_ref_salti (dat, 8);
	return ((unsigned char) result);
}

/** Write 1 bitokon.
 */
void
bit_write_RC (Bit_Chain * dat, unsigned char value)
{
	unsigned char bitoko;
	unsigned char cetero;

	if (dat->byte >= dat->kiom - 1)
		bit_chain_rezervi (dat);

	if (dat->bito == 0)
	{
		dat->chain[dat->byte] = value;
	}
	else
	{
		bitoko = dat->chain[dat->byte];
		cetero = bitoko & (0xff << (8 - dat->bito));
		dat->chain[dat->byte] = cetero | (value >> dat->bito);
		if (dat->byte < dat->kiom - 1)
		{
			bitoko = dat->chain[dat->byte + 1];
			cetero = bitoko & (0xff >> dat->bito);
			dat->chain[dat->byte + 1] = cetero | (value << (8 - dat->bito));
		}
	}

	bit_ref_salti (dat, 8);
}

/** Read 1 dubitokon.
 */
unsigned int
bit_read_RS (Bit_Chain * dat)
{
	unsigned char btk1, btk2;

	btk1 = bit_read_RC (dat);
	btk2 = bit_read_RC (dat);

	/* Malinversigi la pez-finan ordon
	 */
	return ((unsigned int) ((btk2 << 8) | btk1));
}

/** Write 1 dubitokon.
 */
void
bit_write_RS (Bit_Chain * dat, unsigned int value)
{
	/* Inversigi al pez-fina ordo
	 */
	bit_write_RC (dat, value & 0xFF);
	bit_write_RC (dat, value >> 8);
}

/** Read 1 kvarbitokon.
 */
long unsigned int
bit_read_RL (Bit_Chain * dat)
{
	unsigned int dbtk1, dbtk2;

	dbtk1 = bit_read_RS (dat);
	dbtk2 = bit_read_RS (dat);

	/* Malinversigi la pez-finan ordon
	 */
	return ((((long unsigned int) dbtk2) << 16) | ((long unsigned int) dbtk1));
}

/** Write 1 kvarbitokon.
 */
void
bit_write_RL (Bit_Chain * dat, long unsigned int value)
{
	/* Inversigi al la pez-fina ordo
	 */
	bit_write_RS (dat, value & 0xFFFF);
	bit_write_RS (dat, value >> 16);
}


/** Read 1 duglitajxon.
 */
double
bit_read_RD (Bit_Chain * dat)
{
	int i;
	unsigned char btk[8];
	double *result;

	for (i = 0; i < 8; i++)
		btk[i] = bit_read_RC (dat);

	result = (double *) btk;
	return (*result);
}

/** Write 1 duglitajxon.
 */
void
bit_write_RD (Bit_Chain * dat, double value)
{
	int i;
	unsigned char *val;

	val = (unsigned char *) &value;

	for (i = 0; i < 8; i++)
		bit_write_RC (dat, val[i]);
}

/** Read 1 kompaktitan dubitokon.
 */
unsigned int
bit_read_BS (Bit_Chain * dat)
{
	unsigned char bitduo;
	unsigned int result;

	bitduo = bit_read_BB (dat);

	if (bitduo == 0)
	{
		result = bit_read_RS (dat);
		return (result);
	}
	else if (bitduo == 1)
	{
		result = bit_read_RC (dat);
		return (result);
	}
	else if (bitduo == 2)
		return (0);
	else			/* if (bitduo == 3) */
		return (256);
}

/** Write 1 kompaktitan dubitokon.
 */
void
bit_write_BS (Bit_Chain * dat, unsigned int value)
{

	if (value > 256)
	{
		bit_write_BB (dat, 0);
		bit_write_RS (dat, value);
	}
	else if (value == 0)
		bit_write_BB (dat, 2);
	else if (value == 256)
		bit_write_BB (dat, 3);
	else
	{
		bit_write_BB (dat, 1);
		bit_write_RC (dat, value);
	}
}

/** Read 1 kompaktitan kvarbitokon.
 */
long unsigned int
bit_read_BL (Bit_Chain * dat)
{
	unsigned char bitduo;
	long unsigned int result;

	bitduo = bit_read_BB (dat);

	if (bitduo == 0)
	{
		result = bit_read_RL (dat);
		return (result);
	}
	else if (bitduo == 1)
	{
		result = bit_read_RC (dat);
		return (result);
	}
	else if (bitduo == 2)
		return (0);
	else			/* if (bitduo == 3) */
	{
		fprintf (stderr, "Error: bit_read_BL: unexpected 2-bit code: '11'\n");
		return (256);
	}
}

/** Write 1 kompaktitan kvarbitokon.
 */
void
bit_write_BL (Bit_Chain * dat, long unsigned int value)
{
	if (value > 255)
	{
		bit_write_BB (dat, 0);
		bit_write_RL (dat, value);
	}
	else if (value == 0)
		bit_write_BB (dat, 2);
	else
	{
		bit_write_BB (dat, 1);
		bit_write_RC (dat, value);
	}
}

/** Read 1 kompaktitan duglitajxon.
 */
double
bit_read_BD (Bit_Chain * dat)
{
	unsigned char bitduo;
	long int *res;
	double result;

	bitduo = bit_read_BB (dat);

	if (bitduo == 0)
	{
		result = bit_read_RD (dat);
		return (result);
	}
	else if (bitduo == 1)
		return (1.0);
	else if (bitduo == 2)
		return (0.0);
	else			/* if (bitduo == 3) */
	{
		fprintf (stderr, "Error: bit_read_BD: unexpected 2-bit code: '11'\n");
		/* create a Not-A-Number (NaN) */
		res = (long int *) &result;
		res[0] = -1;
		res[1] = -1;
		return (result);
	}
}

/** Write 1 kompaktitan duglitajxon.
 */
void
bit_write_BD (Bit_Chain * dat, double value)
{
	if (value == 0.0)
		bit_write_BB (dat, 2);
	else if (value == 1.0)
		bit_write_BB (dat, 1);
	else
	{
		bit_write_BB (dat, 0);
		bit_write_RD (dat, value);
	}
}

/** Read 1 kompaktitan entjeron, laux moduleca bitoka formo (maksimume 4 bitokojn).
 */
long int
bit_read_MC (Bit_Chain * dat)
{
	int i, j;
	int negativi;
	unsigned char bitoko[4];
	long unsigned int result;

	negativi = 0;
	result = 0;
	for (i = 3, j = 0; i > -1; i--, j += 7)
	{
		bitoko[i] = bit_read_RC (dat);
		if (!(bitoko[i] & 0x80))
		{
			if ((bitoko[i] & 0x40))
			{
				negativi = 1;
				bitoko[i] &= 0xbf;
			}
			result |= (((long unsigned int) bitoko[i]) << j);
			return (negativi ? -((long int) result) : (long int) result);
		}
		else
			bitoko[i] &= 0x7f;
		result |= ((long unsigned int) bitoko[i]) << j;
	}

	return 0;		/* malsukcese... */
}

/** Write 1 kompaktitan entjeron, laux moduleca bitoka formo (maksimume 4 bitokojn).
 */
void
bit_write_MC (Bit_Chain * dat, long int val)
{
	int i, j;
	int negativi;
	unsigned char bitoko[4];
	long unsigned int masko;
	long unsigned int value;

	if (val < 0)
	{
		negativi = 1;
		value = (long unsigned int) -val;
	}
	else
	{
		negativi = 0;
		value = (long unsigned int) val;
	}

	masko = 0x0000007f;
	for (i = 3, j = 0; i > -1; i--, j += 7)
	{
		bitoko[i] = (unsigned char) ((value & masko) >> j);
		bitoko[i] |= 0x80;
		masko = masko << 7;
	}
	for (i = 0; i < 3; i++)
		if (bitoko[i] & 0x7f)
			break;

	if (bitoko[i] & 0x40)
		i--;
	bitoko[i] &= 0x7f;
	if (negativi)
		bitoko[i] |= 0x40;
	for (j = 3; j >= i; j--)
		bit_write_RC (dat, bitoko[j]);
//if (value == 64) printf ("(%2X) \n", bitoko[i]);
}

/** Read 1 kompaktitan entjeron, laux moduleca dubitoka formo (maksimume 2 dubitokojn).
 */
long unsigned int
bit_read_MS (Bit_Chain * dat)
{
	int i, j;
	unsigned int dubitoko[2];
	long unsigned int result;

	result = 0;
	for (i = 1, j = 0; i > -1; i--, j += 15)
	{
		dubitoko[i] = bit_read_RS (dat);
		if (!(dubitoko[i] & 0x8000))
		{
			result |= (((long unsigned int) dubitoko[i]) << j);
			return (result);
		}
		else
			dubitoko[i] &= 0x7fff;
		result |= ((long unsigned int) dubitoko[i]) << j;
	}

	return 0;		/* malsukcese... */
}

/** Write 1 kompaktitan entjeron, laux moduleca dubitoka formo (maksimume 2 dubitokojn).
 */
void
bit_write_MS (Bit_Chain * dat, long unsigned int value)
{
	int i, j;
	unsigned int dubitoko[4];
	long unsigned int masko;

	masko = 0x00007fff;
	for (i = 1, j = 0; i > -1; i--, j += 15)
	{
		dubitoko[i] = ((unsigned int) ((value & masko) >> j)) | 0x8000;
		masko = masko << 15;
	}
	/* Ne uzu tion sube: cxiam faru gxin kvarbitoka!
	   for (i = 0; i < 1; i++)
	   if (dubitoko[i] & 0x7fff)
	   break;
	 */
	i = 1;
	dubitoko[i] &= 0x7fff;
	for (j = 1; j >= i; j--)
		bit_write_RS (dat, dubitoko[j]);
}

/** Read bit-extrusionn.
 */
void
bit_read_BE (Bit_Chain * dat, double *x, double *y, double *z)
{
	if (bit_read_B (dat))
	{
		*x = 0.0;
		*y = 0.0;
		*y = 1.0;
	}
	else
	{
		*x = bit_read_BD (dat);
		*y = bit_read_BD (dat);
		*z = bit_read_BD (dat);
	}
}

/** Write bit-extrusionn.
 */
void
bit_write_BE (Bit_Chain * dat, double x, double y, double z)
{
	if (x == 0.0 && y == 0.0 && z == 1.0)
		bit_write_B (dat, 1);
	else
	{
		bit_write_B (dat, 0);
		bit_write_BD (dat, x);
		bit_write_BD (dat, y);
		bit_write_BD (dat, z);
	}
}

/** Read duglitajxon kun antauxdifinajxo.
 */
double
bit_read_DD (Bit_Chain * dat, double antauxdif)
{
	unsigned char kodero;
	unsigned char *uc_rez;

	kodero = bit_read_BB (dat);
	if (kodero == 0)
		return antauxdif;
	if (kodero == 3)
		return (bit_read_RD (dat));
	if (kodero == 2)
	{
		uc_rez = (char *) &antauxdif;
		uc_rez[3] = bit_read_RC (dat);
		uc_rez[2] = bit_read_RC (dat);
		uc_rez[7] = bit_read_RC (dat);
		uc_rez[6] = bit_read_RC (dat);
		uc_rez[5] = bit_read_RC (dat);
		uc_rez[4] = bit_read_RC (dat);
		return antauxdif;
	}
	else			/* if (kodero == 1) */
	{
		uc_rez = (char *) &antauxdif;
		uc_rez[7] = bit_read_RC (dat);
		uc_rez[6] = bit_read_RC (dat);
		uc_rez[5] = bit_read_RC (dat);
		uc_rez[4] = bit_read_RC (dat);
		return antauxdif;
	}
}

/** Write duglitajxon kun antauxdifinajxo.
 */
void
bit_write_DD (Bit_Chain * dat, double value, double antauxdif)
{
	unsigned char *uc_val;

	unsigned int *ui_val;
	unsigned int *ui_ant;

	if (value == antauxdif)
		bit_write_BB (dat, 0);
	else
	{
		uc_val = (char *) &value;
		ui_val = (int *) &value;
		ui_ant = (int *) &antauxdif;
		if (ui_val[0] == ui_ant[0])
		{
			if (ui_val[1] != ui_ant[1])
			{
				bit_write_BB (dat, 2);
				bit_write_RC (dat, uc_val[3]);
				bit_write_RC (dat, uc_val[2]);
				bit_write_RC (dat, uc_val[7]);
				bit_write_RC (dat, uc_val[6]);
				bit_write_RC (dat, uc_val[5]);
				bit_write_RC (dat, uc_val[4]);
			}
			else
			{
				bit_write_BB (dat, 1);
				bit_write_RC (dat, uc_val[7]);
				bit_write_RC (dat, uc_val[6]);
				bit_write_RC (dat, uc_val[5]);
				bit_write_RC (dat, uc_val[4]);
			}
		}
		else
		{
			bit_write_BB (dat, 0);
			bit_write_RD (dat, value);
		}
	}
}

/** Read dikec-valuen.
 */
double
bit_read_BT (Bit_Chain * dat)
{
	int modo;

	modo = bit_read_B (dat);
	return (modo ? 0.0 : bit_read_BD (dat));
}

/** Write dikec-valuen.
 */
void
bit_write_BT (Bit_Chain * dat, double value)
{
	if (value == 0.0)
		bit_write_B (dat, 1);
	else
	{
		bit_write_B (dat, 0);
		bit_write_BD (dat, value);
	}
}

/** Read traktilo-referencon.
 */
int
bit_read_H (Bit_Chain * dat, Dwg_Traktilo * trakt)
{
	Bit_Chain tmpdat;
	unsigned char *val;
	int i;

	trakt->code = bit_read_RC (dat);
	trakt->kiom = trakt->code & 0x0f;
	trakt->code = (trakt->code & 0xf0) >> 4;

	trakt->value = 0;
	if (trakt->kiom > 4)
	{
		/*
		printf ("Eraro: traktilo-referenco pli longa ol 4 bitokoj: %i.%i.%lu\n",
			trakt->code, trakt->kiom, trakt->value);
		*/
		trakt->kiom = 0;
		return (-1);
	}

	val = (char *) &trakt->value;
	for (i = trakt->kiom - 1; i >= 0; i--)
		val[i] = bit_read_RC (dat);

	return (0);
}

/** Write traktilo-referencon.
 */
void
bit_write_H (Bit_Chain * dat, Dwg_Traktilo * trakt)
{
	int i, j;
	unsigned char *val;
	unsigned char code_nombrilo;

	if (trakt->value == 0)
	{
		bit_write_RC (dat, (trakt->code << 4));
		return;
	}

	val = (char *) &trakt->value;
	for (i = 3; i >= 0; i--)
		if (val[i])
			break;

	code_nombrilo = trakt->code << 4;
	code_nombrilo |= i + 1;

	bit_write_RC (dat, code_nombrilo);

	for (; i >= 0; i--)
		bit_write_RC (dat, val[i]);
}

/** Nur read CRK-numbern, sen iu ajn kontrolo, nur por iri al la sekva byte,
 * saltante eventualajn neuzitajn bitojn.
 */
unsigned int
bit_read_CRC (Bit_Chain * dat)
{
	unsigned int result;
	unsigned char res[2];

	if (dat->bito > 0)
	{
		dat->byte++;
		dat->bito = 0;
	}

	res[0] = bit_read_RC (dat);
	res[1] = bit_read_RC (dat);

	result = (unsigned int) (res[0] << 8 | res[1]);

	return result;
}

/** Read and check CRC-number.
 */
int
bit_check_CRC (Bit_Chain * dat, long unsigned int start_address, unsigned int seed)
{
	unsigned int calculated;
	unsigned int read;
	unsigned char res[2];

	if (dat->bito > 0)
		dat->byte++;
	dat->bito = 0;

	calculated = bit_ckr8 (seed, &(dat->chain[start_address]), dat->byte - start_address);

	res[0] = bit_read_RC (dat);
	res[1] = bit_read_RC (dat);

	read = (unsigned int) (res[0] << 8 | res[1]);

	return (calculated == read);
}

/** Create and write CRC-number.
 */
unsigned int
bit_krei_CRC (Bit_Chain * dat, long unsigned int start_address, unsigned int seed)
{
	unsigned int crc;

	while (dat->bito > 0)
		bit_write_B (dat, 0);

	crc = bit_ckr8 (seed, &(dat->chain[start_address]), dat->byte - start_address);

	bit_write_RC (dat, (unsigned char) (crc >> 8));
	bit_write_RC (dat, (unsigned char) (crc & 0xFF));

	return (crc);
}

/** Read simple text. After usage, the allocated memory must be proprly freed.
 */
unsigned char *
bit_read_T (Bit_Chain * dat)
{
	unsigned int i;
	unsigned int length;
	unsigned char *chain;

	length = bit_read_BS (dat);
	chain = (char *) malloc (length + 1);
	for (i = 0; i < length; i++)
	{
		chain[i] = bit_read_RC (dat);
		if (chain[i] == 0)
			chain[i] = '*';
		else if (!isprint (chain[i]))
			chain[i] = '~';
	}
	chain[i] = '\0';

	return (chain);
}

/** Write simple text.
 */
void
bit_write_T (Bit_Chain * dat, unsigned char *chain)
{
	int i;
	int length;

	length = strlen (chain);
	bit_write_BS (dat, length);
	for (i = 0; i < length; i++)
		bit_write_RC (dat, chain[i]);
}

/** Read 1 kvarbitokon laux normala (komenc-peza) ordo.
 */
long unsigned int
bit_read_L (Bit_Chain * dat)
{
	unsigned char btk[4];

	btk[3] = bit_read_RC (dat);
	btk[2] = bit_read_RC (dat);
	btk[1] = bit_read_RC (dat);
	btk[0] = bit_read_RC (dat);

	return (*((long unsigned int *) btk));
}

/** Write 1 kvarbitokon laux normala ordo.
 */
void
bit_write_L (Bit_Chain * dat, long unsigned int value)
{
	unsigned char *btk;

	btk = (char *) value;
	bit_write_RC (dat, btk[3]);
	bit_write_RC (dat, btk[2]);
	bit_write_RC (dat, btk[1]);
	bit_write_RC (dat, btk[0]);
}

/** Search for a sentinel; if found, positions "dat->byte" imediatly after it.
 */
int
bit_search_sentinel (Bit_Chain * dat, unsigned char sentinel[16])
{
	long unsigned int i, j;

	for (i = 0; i < dat->kiom; i++)
	{
		for (j = 0; j < 16; j++)
		{
			if (dat->chain[i + j] != sentinel[j])
				break;
		}
		if (j == 16)
		{
			dat->byte = i + j;
			dat->bito = 0;
			return -1;
		}
	}
	return 0;
}

void
bit_write_sentinel (Bit_Chain * dat, unsigned char sentinel[16])
{
	int i;

	for (i = 0; i < 16; i++)
		bit_write_RC (dat, sentinel[i]);
}

/*
 * Rezervi spacon por write en bitara chain
 */
#define CXENO_BLOKO 40960
void
bit_chain_rezervi (Bit_Chain * dat)
{
	if (dat->kiom == 0)
	{
		dat->chain = calloc (1, CXENO_BLOKO);
		dat->kiom = CXENO_BLOKO;
		dat->byte = 0;
		dat->bito = 0;
	}
	else
	{
		dat->chain = realloc (dat->chain, dat->kiom + CXENO_BLOKO);
		dat->kiom += CXENO_BLOKO;
	}
}

void
bit_print (Bit_Chain * dat, long unsigned int kiom)
{
	unsigned char sig;
	long unsigned int i, j, k;

	printf ("---------------------------------------------------------");
	if (kiom > dat->kiom)
		kiom = dat->kiom;
	for (i = 0; i < kiom; i++)
	{
		if (i % 16 == 0)
			printf ("\n[0x%04X]: ", (unsigned int) i);
		printf ("%02X ", dat->chain[i]);
		if (i % 16 == 15)
			for (j = i - 15; j <= i; j++)
			{
				sig = dat->chain[j];
				printf ("%c", sig >= ' ' && sig < 128 ? sig : '.');
			}
	}
	puts ("");
	puts ("---------------------------------------------------------");
}

void
bit_esplori_chain (Bit_Chain * dat, long unsigned int kiom)
{
	unsigned char sig;
	long unsigned int i, j, k;

	if (kiom > dat->kiom)
		kiom = dat->kiom;

	for (k = 0; k < 8; k++)
	{
		printf ("---------------------------------------------------------");
		dat->byte = 0;
		dat->bito = k;
		for (i = 0; i < kiom - 1; i++)
		{
			if (i % 16 == 0)
				printf ("\n[0x%04X]: ", (unsigned int) i);
			sig = bit_read_RC (dat);
			printf ("%c", sig >= ' ' && sig < 128 ? sig : '.');
		}
		puts ("");
	}
	puts ("---------------------------------------------------------");
}

/*------------------------------------------------------------------------------
 * Privataj funkcioj
 */

static unsigned int
bit_ckr8 (unsigned int dx, unsigned char *adr, long n)
{
	register unsigned char al;

	static unsigned int ckrtabelo[256] = {
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
		0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
		0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
		0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
		0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
		0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
		0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
		0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
		0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
		0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
		0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
		0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
		0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
		0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
		0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
		0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
		0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
		0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
		0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
		0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
		0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
		0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
		0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
		0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
		0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
		0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
		0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
		0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
		0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
		0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
		0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
		0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
	};

	for (; n > 0; n--)
	{
		al = (unsigned char) ((*adr) ^ ((unsigned char) (dx & 0xFF)));
		dx = (dx >> 8) & 0xFF;
		dx = dx ^ ckrtabelo[al & 0xFF];
		adr++;
	}
	return (dx);
}

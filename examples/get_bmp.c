/**************************************************************************/
/*  LibreDWG - Free DWG read-only library                                 */
/*  Copyright (C) 2008, 2009 Free Software Foundation, Inc.               */
/*  Copyright (C) 2009       Felipe Corrêa da Silva Sanches               */
/*                                                                        */
/*  This program is free software, licensed under the terms of the GNU    */
/*  General Public License as published by the Free Software Foundation,  */
/*  which is also included in this package, in a file named "COPYING".    */
/**************************************************************************/

/* Simple way to get the bmp thumbnail in a dwg file
 */

#include <stdio.h>
#include <string.h>
#include <dwg.h>

#define INPUT_FILE "sample.dwg"
#define OUTPUT_FILE "sample.bmp"

int get_bmp (char *filename)
{
	char *data;
	int success;
	long size;
	long tmp;
	FILE *fh;
	Dwg_Structure dwg;
	struct _BITMAP_HEADER
	{
		char magic[2];
		long file_size;
		long reserved;
		long offset;
	} bmp_h;

	/* Read dwg data */
	success = dwg_read_file (filename, &dwg);
	if (success != 0)
	{
		puts ("Unable to read sample file");
		return success;
	}

	/* Get DIB bitmap data */
	data = dwg_bmp (&dwg, &size);
	if (!data)
	{
		puts ("No thumb in dwg file");
		return -2;
	}
	if (size < 1)
	{
		puts ("No thumb data in dwg file");
		return -3;
	}

	fh = fopen (OUTPUT_FILE, "w");
	if (!fh)
	{
		puts ("Unable to write a '" OUTPUT_FILE "' file");
		return -4;
	}

	/* Write bmp file header */
	bmp_h.magic[0] = 'B';
	bmp_h.magic[1] = 'M';
	bmp_h.file_size = 14 + size; // file header + DIB data
	bmp_h.reserved = 0;
	bmp_h.offset = 14 + 40 + 4*256; // file header + DIB header + color table
	fwrite (&bmp_h.magic[0], 2, sizeof (char), fh);
	fwrite (&bmp_h.file_size, 3, sizeof (long), fh);

	/* Write data (DIB header + bitmap) */
	fwrite (data, 1, size, fh); 
	fclose (fh);

	puts ("Success! See the file '" OUTPUT_FILE "'");
	return success;
}

int main ()
{
	get_bmp (INPUT_FILE);
	return 0;
}


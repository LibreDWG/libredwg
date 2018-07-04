/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * unknown.c: decode unknown bitstreams
 * written by Reini Urban
 *
 * A sample program to find the most likely
 * fields for all unknown dwg entities and objects.
 * gather all binary raw data from all unknown dwg entities and objects
 * into examples/alldwg.inc and examples/alldxf.inc
 * with the available likely fields try permutations of most likely types.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dwg.h"
#include "../src/bits.h"
#include "../src/logging.h"

typedef enum DWG_BITS
{
  BITS_UNKNOWN,
  BITS_RC,
  BITS_RS,
  BITS_RL,
  BITS_B,
  BITS_BB,
  BITS_3B,
  BITS_4BITS,
  BITS_BS,
  BITS_BL,
  BITS_RLL,
  BITS_RD,
  BITS_BD,
  BITS_MC,
  BITS_UMC,
  BITS_MS,
  BITS_TV,
  BITS_TU,
  BITS_T,
  BITS_TF,
  BITS_HANDLE,
  BITS_BE,
  BITS_DD,
  BITS_BT,
  BITS_CRC,
  BITS_BOT,
  BITS_BLL,
  BITS_TIMEBLL,
  BITS_CMC,
} Dwg_Bits;

struct _unknown_field {
  int code;
  char *value;
  char *bits;
  int bitsize;
  Dwg_Bits type;
  int pos;
};
static struct _unknown_dxf {
  const char *name;
  const char *dxf;
  const unsigned int handle;
  const char *bytes;
  const char *bits;
  const struct _unknown_field *fields;
} unknown_dxf[] = {
    // see log_unknown_dxf.pl
    #include "alldxf_0.inc"
    { NULL, NULL, 0, "", "", NULL }
};
#include "alldxf_1.inc"  

/* not needed for the solver, only to check against afterwards */
#if 0
static struct _unknown {
  const char *name;
  const char *bytes;
  const char *bits;
  const char *log; const char *dxf;
  const unsigned int handle; const int bitsize;
} unknowns[] =
  {
   { "MATERIAL", "0e0101000f010100", "10", "150DSC4_AO-46050_QDC-fixed_2000.log", NULL, 400137, 1426 },
    /* the following types:
      5 ACDBASSOCGEOMDEPENDENCY
      3 ACDBASSOCNETWORK
     53 ACDBDETAILVIEWSTYLE
     53 ACDBSECTIONVIEWSTYLE
      5 ACDB_LEADEROBJECTCONTEXTDATA_CLASS
     63 CELLSTYLEMAP
    245 MATERIAL
      1 MULTILEADER
      1 SUN
     97 TABLESTYLE
    */
    // see log_unknown.pl
    #include "alldwg.inc"

    { 0, NULL, "", "", NULL, 0L, 0L }
};
#endif

static void bits_string(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  bit_write_TV(dat, (BITCODE_TV)g->value);
  g->type = BITS_TV;
}

static void bits_B(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  if (*g->value == '0') {
    bit_write_B(dat, 0);
    g->type = BITS_B;
  }
  else if (*g->value == '1') {
    bit_write_B(dat, 0);
    g->type = BITS_B;
  }
  else
    LOG_ERROR("Invalid B %s", g->value);
}

static void bits_BD(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  double d;
  sscanf(g->value, "%f", &d);
  bit_write_BD(dat, d);
  g->type = BITS_BD;
}

static void bits_RC(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  unsigned int l;
  sscanf(g->value, "%u", &l);
  bit_write_RC(dat, (unsigned char)l);
  g->type = BITS_RC;
}

static void bits_BS(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  unsigned int l;
  sscanf(g->value, "%u", &l);
  bit_write_BS(dat, (unsigned short)l);
  g->type = BITS_BS;
}

static void bits_BL(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  unsigned int l;
  sscanf(g->value, "%u", &l);
  bit_write_BL(dat, l);
  g->type = BITS_BL;
}

static void bits_handle(Bit_Chain *restrict dat, struct _unknown_field *restrict g, int code)
{
    Dwg_Handle handle;
    //parse hex -> owner handle
    sscanf(g->value, "%X", &handle.value);
    handle.code = code;
    if (handle.value < 0xff)
      handle.size = 1;
    else if (handle.value < 0xffff)
      handle.size = 2;
    else if (handle.value < 0xffffff)
      handle.size = 3;
    else
      handle.size = 4;
    bit_write_H(dat, &handle);
    g->type = BITS_HANDLE;
}

static void
bits_format (struct _unknown_field *g)
{
  int code = g->code;
  static Bit_Chain dat;
  dat.size = 16;
  dat.chain = calloc(16,1);

  if (0 <= code && code < 5)
    bits_string(&dat, g);
  else if (code == 5 || code == -5)
    bits_handle(&dat, g, 0);
  else if (5 < code && code < 10)
    bits_string(&dat, g);
  else if (code < 60)
    bits_BD(&dat, g); //todo deg2rad for angles
  else if (code < 80)
    bits_BS(&dat, g);
  else if (80 <= code && code <= 99) //BL int32
    bits_BL(&dat, g);
  else if (code == 100)
    return;
  else if (code == 102)
    bits_string(&dat, g);
  else if (code == 105)
    bits_handle(&dat, g, 3);
  else if (110 <= code && code <= 149)
    bits_BD(&dat, g);
  else if (160 <= code && code <= 169)
    bits_BL(&dat, g);
  else if (170 <= code && code <= 179)
    bits_BS(&dat, g);
  else if (210 <= code && code <= 239)
    bits_BD(&dat, g);
  else if (270 <= code && code <= 289)
    bits_BS(&dat, g);
  else if (290 <= code && code <= 299)
    bits_B(&dat, g);
  else if (300 <= code && code <= 319)
    bits_string(&dat, g);
  else if (320 <= code && code <= 369)
    bits_handle(&dat, g, 5);
  else if (370 <= code && code <= 389)
    bits_BS(&dat, g);
  else if (390 <= code && code <= 399)
    bits_handle(&dat, g, 5);
  else if (400 <= code && code <= 409)
    bits_BS(&dat, g);
  else if (410 <= code && code <= 419)
    bits_string(&dat, g);
  else if (420 <= code && code <= 429)
    bits_BL(&dat, g); //int32_t
  else if (430 <= code && code <= 439)
    bits_string(&dat, g);
  else if (440 <= code && code <= 449)
    bits_BL(&dat, g);//int32_t
  else if (450 <= code && code <= 459)
    bits_BL(&dat, g);//long
  else if (460 <= code && code <= 469)
    bits_BD(&dat, g);
  else if (470 <= code && code <= 479)
    bits_string(&dat, g);
  else if (480 <= code && code <= 481)
    bits_handle(&dat, g, 5);
  else if (code == 999)
    return;
  else if (1000 <= code && code <= 1009)
    bits_string(&dat, g);
  else if (1010 <= code && code <= 1059)
    bits_BD(&dat, g);
  else if (1060 <= code && code <= 1070)
    bits_BS(&dat, g);
  else if (code == 1071)
    bits_BL(&dat, g);//int32_t

  if (g->type) {
    g->bits = dat.chain;
    g->bitsize = (dat.byte * 8) + dat.bit;
  } else {
    free (dat.chain);
  }
}

int
main (int argc, char *argv[])
{
  int i, j;
  #include "alldxf_2.inc"
  for (i=0; unknown_dxf[i].name; i++)
    {
      int num_fields;
      struct _unknown_field *g = (struct _unknown_field *)unknown_dxf[i].fields;
      int len = strlen(unknown_dxf[i].bytes);
      //TODO offline: find the shortest objects.
      printf("\n%s: %X %s (%d)\n", unknown_dxf[i].name, unknown_dxf[i].handle,
             len < 200 ? unknown_dxf[i].bytes : "...", len);
      for (j=0; g[j].code; j++)
        {
          if (g[j].code == 100)
            printf("%d: %s\n", g[j].code, g[j].value);
          //store the binary repr
          bits_format(&g[j]);
        }
      num_fields = j;

      //TODO: try likely field combinations and print the top 3.
      //there are various heuristics, like the handle stream at the end
    }

  return 0;
}

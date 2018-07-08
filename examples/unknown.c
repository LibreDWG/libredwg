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

#include "../src/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <math.h>

#include "dwg.h"
#include "../src/bits.h"
#include "../src/logging.h"

#ifndef M_PI_2
# define M_PI_2      1.57079632679489661923132169163975144
#endif
#define rad2deg(ang) (ang)*180.0/M_PI_2
#define deg2rad(ang) (ang) ? M_PI_2/((ang)*180.0) : 0.0

void *memmem(const void *big, size_t big_len, const void *little, size_t little_len);

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

static const char *dwg_bits_name[] =
{
  "UNKNOWN",
  "RC",
  "RS",
  "RL",
  "B",
  "BB",
  "3B",
  "4BITS",
  "BS",
  "BL",
  "RLL",
  "RD",
  "BD",
  "MC",
  "UMC",
  "MS",
  "TV",
  "TU",
  "T",
  "TF",
  "HANDLE",
  "BE",
  "DD",
  "BT",
  "CRC",
  "BOT",
  "BLL",
  "TIMEBLL",
  "CMC",
};

struct _unknown_field {
  int code;
  char *value;
  char *bytes;
  int bitsize;
  Dwg_Bits type;
  int pos[5]; //5x found bit offset in dxf->bytes or -1 if not found
  // many typical values are 5x found (handle 0, BL 2)
};
static struct _unknown_dxf {
  const char *name;
  const char *dxf;
  const unsigned int handle;
  const char *bytes;
  const int bitsize;
  const struct _unknown_field *fields;
} unknown_dxf[] = {
    // see log_unknown_dxf.pl
    #include "alldxf_0.inc"
    { NULL, NULL, 0, "", 0, NULL }
};
#include "alldxf_1.inc"

//dynamic helper companian
struct _dxf {
  unsigned char *found;    //coverage per bit for found 1
  unsigned char *possible; //coverage for mult. finds >1
  int num_filled;
  int num_empty;
  int num_possible;
  double percentage;
};

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
  if (dat->version >= R_2007) {
    bit_write_TU(dat, (BITCODE_TU)g->value);
    g->type = BITS_TU;
  }
  else {
    bit_write_TV(dat, g->value);
    g->type = BITS_TV;
  }
}

static void bits_TV(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  bit_write_TV(dat, g->value);
  g->type = BITS_TV;
}

static void bits_hexstring(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  //convert hex to string
  int len = strlen(g->value)/2;
  unsigned char buf[1024];
  for (int i=0; i<len; i++) {
    unsigned char *s = &g->value[i*2];
    buf[i] = ((*s < 'A') ? *s - '0' : *s + 10 - 'A') << 4;
    s++;
    buf[i] += (*s < 'A') ? *s - '0' : *s + 10 - 'A';
  }
  bit_write_TF(dat, buf, len);
  g->type = BITS_TF;
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
  else {
    LOG_ERROR("Invalid B %s", g->value);
  }
}

static void bits_RD(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  double d = strtod(g->value, NULL);
  bit_write_RD(dat, d);
  g->type = BITS_RD;
}

static void bits_BD(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  double d = strtod(g->value, NULL);
  //properly found are: 0.0, 1.0, 5929.403601723592, 607.2183823688756,
  // 4.0, 2.0, 0.36, 5.0, 4.131293495034893, 4701.847034571434, 0.5024999976158142
  //hard-code some special values not properly converted and found.
  if (!strcmp(g->value, "0.5"))
    d = 0.5;
  else if (!strcmp(g->value, "10.0"))
    d = 10.0;
  else if (!strcmp(g->value, "11.0"))
    d = 11.0;
  else if (!strcmp(g->value, "63.5"))
    d = 63.5;
  //sscanf(g->value, "%lf", &d);
  bit_write_BD(dat, d);
  g->type = BITS_BD;
}

static void bits_angle_BD(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  double d = strtod(g->value, NULL);
  //sscanf(g->value, "%lf", &d);
  d = deg2rad(d);
  bit_write_BD(dat, d);
  g->type = BITS_BD;
}

static void bits_RC(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  unsigned int l = strtol(g->value, NULL, 10);
  bit_write_RC(dat, (unsigned char)l);
  g->type = BITS_RC;
}

static void bits_BS(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  unsigned int l = strtol(g->value, NULL, 10);
  bit_write_BS(dat, (unsigned short)l);
  g->type = BITS_BS;
}

static void bits_BL(Bit_Chain *restrict dat, struct _unknown_field *restrict g)
{
  unsigned int l = strtol(g->value, NULL, 10);
  bit_write_BL(dat, l);
  g->type = BITS_BL;
}

static void bits_handle(Bit_Chain *restrict dat, struct _unknown_field *restrict g,
                        int code, unsigned int objhandle)
{
  Dwg_Handle handle;
  //parse hex -> owner handle;
  sscanf(g->value, "%X", &handle.value);
  handle.code = code;
  if (code > 5) { //relative offset to objhandle
    switch (code) {
    case 6: handle.value = 0; break;
    case 8: handle.value = 0; break;
    case 0xA: handle.value += objhandle; break;
    case 0xC: handle.value -= objhandle; break;
    }
  }
  if (handle.value == 0)
    handle.size = 0;
  else if (handle.value < 0xff)
    handle.size = 1;
  else if (handle.value < 0xffff)
    handle.size = 2;
  else if (handle.value < 0xffffff)
    handle.size = 3;
  else
    handle.size = 4;
  printf("  handle %x.%d.%X (%X)\n", code, handle.size, handle.value, objhandle);
  bit_write_H(dat, &handle);
  g->type = BITS_HANDLE;
}

static int is_handle(int code)
{
  return code == 5 ||
         code == 105 ||
         (code >= 320 && code <= 369) ||
         (code >= 390 && code <= 399) ||
         (code >= 480 && code <= 481);
}

static void
bits_try_handle (struct _unknown_field *g, int code, unsigned int objhandle)
{
  Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
  dat.chain = calloc(16,1);

  bits_handle(&dat, g, code, objhandle);

  g->bytes = dat.chain;
  g->bitsize = (dat.byte * 8) + dat.bit;
}

static void
bits_format (struct _unknown_field *g, int is16)
{
  int code = g->code;
  Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
  dat.chain = calloc(16,1);
  if (is16)
    dat.version = R_2007;

  if (0 <= code && code < 5)
    bits_string(&dat, g);
  else if (code == 5 || code == -5)
    bits_handle(&dat, g, 0, 0);
  else if (5 < code && code < 10)
    // 6 ltype handle or BS index, 7 style handle, 8 layer handle
    bits_string(&dat, g);
  else if (code < 50)
    bits_BD(&dat, g);
  else if (code < 60)
    bits_angle_BD(&dat, g); //deg2rad for angles
  else if (code < 80)
    bits_BS(&dat, g);
  else if (80 <= code && code <= 99) //BL int32
    bits_BL(&dat, g);
  else if (code == 100)
    return;
  else if (code == 102) //this is never stored in a DWG
    bits_string(&dat, g);
  else if (code == 105)
    bits_handle(&dat, g, 3, 0);
  else if (110 <= code && code <= 149)
    bits_BD(&dat, g);
  else if (160 <= code && code <= 169)
    bits_BL(&dat, g);
  else if (code <= 179)
    bits_BS(&dat, g);
  else if (210 <= code && code <= 239)
    bits_BD(&dat, g);
  else if (270 <= code && code <= 289)
    bits_BS(&dat, g);
  else if (code <= 299)
    bits_B(&dat, g);
  else if (code <= 309)
    bits_TV(&dat, g);
  else if (code <= 319)
    bits_hexstring(&dat, g);
  else if (code >= 320 && code < 360)
    bits_handle(&dat, g, 4, 0); //2 or 4 or 3.0
  else if (code >= 340 && code < 360)
    bits_handle(&dat, g, 5, 0);
  else if (code >= 360 && code <= 369)
    bits_handle(&dat, g, 3, 0);
  else if (code <= 389)
    bits_BS(&dat, g);
  else if (code <= 399)
    bits_handle(&dat, g, 5, 0);
  else if (code <= 409)
    bits_BS(&dat, g);
  else if (code <= 419)
    bits_string(&dat, g);
  else if (code <= 429)
    bits_BL(&dat, g); //int32_t
  else if (code <= 439)
    bits_string(&dat, g);
  else if (code <= 449)
    bits_BL(&dat, g);//int32_t
  else if (code <= 459)
    bits_BL(&dat, g);//long
  else if (code <= 469)
    bits_BD(&dat, g);
  else if (code <= 479)
    bits_string(&dat, g);
  else if (code <= 481)
    bits_handle(&dat, g, 5, 0);
  else if (code == 999)
    return;
  else if (1000 <= code && code <= 1009)
    bits_string(&dat, g);
  else if (1010 <= code && code <= 1049)
    bits_BD(&dat, g);
  else if (1050 <= code && code <= 1059)
    bits_angle_BD(&dat, g);
  else if (1060 <= code && code <= 1070)
    bits_BS(&dat, g);
  else if (code == 1071)
    bits_BL(&dat, g);//int32_t
  else
    fprintf(stderr, "Unknown DXF code %d\n", code);

  if (g->type) {
    g->bytes = dat.chain;
    g->bitsize = (dat.byte * 8) + dat.bit;
  } else {
    free (dat.chain);
  }
}

static int set_found (struct _dxf *dxf, const struct _unknown_field *g) {
  // check for overlap, if already found by some other field
  int overlap = 0;
  for (int k=g->pos[0]; k<g->pos[0]+g->bitsize; k++) {
    if (dxf->found[k] && !overlap) {
      overlap = 1;
      printf("field %d already found at %d\n", g->code, k);
    }
    dxf->found[k]++;
  }
  return overlap;
}

static void set_possible_pos(struct _dxf *dxf, const struct _unknown_field *g, const int pos)
{
  // add coverage counter for each bit
  for (int k=pos; k<pos+g->bitsize; k++) {
    dxf->possible[k]++;
  }
}

static void set_possible(struct _dxf *dxf, const struct _unknown_field *g, const int i)
{
  // add coverage counter for each bit
  for (int j=0; j<i; j++) {
    for (int k=g->pos[j]; k<g->pos[j]+g->bitsize; k++) {
      dxf->possible[k]++;
    }
  }
}

#define BIT(b,i) ((b[(i)/8] >> (i)%8) & 1)

// like memmem but for bits, not bytes.
// search for the bits of small in big. returns the bit offset in big or -1.
// handle bits before and after.
int membits(const unsigned char *restrict big, const int bigsize,
            const unsigned char *restrict small, const int smallsize,
            int offset)
{
  int pos = offset;
  if (smallsize > bigsize)
    return -1;
  while (pos < bigsize) {
    int i = 0;
    while (i < smallsize) {
      if (BIT(big, pos+i) != BIT(small, i))
        break;
      i++; //found, check next bit
    }
    if (i == smallsize) //found all smallsize bits
      return pos;
    pos++;
  }
  return -1;
}

int search_bits(int j, struct _unknown_field *g, struct _unknown_dxf *udxf,
                struct _dxf *dxf, int offset)
{
  int size;
  unsigned char *s;
  unsigned char* found;
  int num_found = 0;
  int dxf_bitsize = udxf->bitsize;
  int dxf_size = dxf_bitsize/8;

  if (!g->type || !g->bitsize || !dxf_size)
    return 0;
  size = (g->bitsize/8) + (g->bitsize % 8 ? 1 : 0);
  dxf_size += (dxf_size % 8 ? 1 : 0);
  if (size > dxf_size)
    return 0;
  s = alloca(size);
  printf("  search %d:\"%s\" (%d bits of type %s [%d]) in %d bits\n",
         g->code, g->value, g->bitsize, dwg_bits_name[g->type], j, dxf_bitsize-offset);
  printf("  =search (%d): ", offset); bit_print_bits(g->bytes, g->bitsize);
  while ((offset = membits(udxf->bytes, udxf->bitsize, g->bytes, g->bitsize, offset)) != -1) {
    if (num_found < 5) //record only the first 5 offsets for the solver
      g->pos[num_found] = offset;
    num_found++;
    set_possible_pos(dxf, g, offset); //but record all other offsets here as counts 0-255
    //if (num_found > 5)
    //  break;
    offset++;
  }
  return num_found;
}

int
main (int argc, char *argv[])
{
  int i, j;
  long sum_filled = 0, sum_size = 0;
  struct _dxf *dxf = calloc(sizeof(unknown_dxf)/sizeof(unknown_dxf[0]), sizeof(struct _dxf));
  #include "alldxf_2.inc"
  for (i=0; unknown_dxf[i].name; i++)
    {
      int num_fields;
      int num_found;
      int size = unknown_dxf[i].bitsize;
      struct _unknown_field *g = (struct _unknown_field *)unknown_dxf[i].fields;
      int is16 = strstr(unknown_dxf[i].dxf, "_2007.dxf") ? 1 : 0;
      if (!is16)
        strstr(unknown_dxf[i].dxf, "_201") ? 1 : 0;
      dxf[i].found = calloc(unknown_dxf[i].bitsize, 1);
      dxf[i].possible = calloc(unknown_dxf[i].bitsize, 1);
      //TODO offline: find the shortest objects.
      printf("\n%s: 0x%X (%d)\n", unknown_dxf[i].name, unknown_dxf[i].handle,
             size);
      printf("  =bits: "); bit_print_bits((unsigned char*)unknown_dxf[i].bytes,
                                          size);
      for (j=0; g[j].code; j++)
        {
          int offset = 0;
          if (g[j].code == 100 || g[j].code == 102) {
            printf("%d: %s\n", g[j].code, g[j].value);
            continue;
          }
          //store the binary repr
          bits_format(&g[j], is16);
        SEARCH:
          //searching for it in the stream and store found position if found only once
          num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
          if (!num_found) {

            // try alternate formats than the standard one from bits_format:
            int code = g[j].code;
            if (is_handle(code) && code != 330 && code != 5) {
              int handles[] = {2,3,4,5,6,8,0xa,0xc};
              unsigned int hdl;
              sscanf(g[j].value, "%X", &hdl);
              for (int c=0; c<8; c++) {
                if (handles[c]==6 && unknown_dxf[i].handle - hdl != 1) //+1
                  continue;
                if (handles[c]==8 && unknown_dxf[i].handle - hdl != -1) //-1
                  continue;
                if (handles[c]==0xa && unknown_dxf[i].handle - hdl < 0)
                  continue;
                if (handles[c]==0xc && unknown_dxf[i].handle - hdl > 0)
                  continue;
                bits_try_handle (&g[j], handles[c], unknown_dxf[i].handle);
                num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
                if (num_found)
                  goto FOUND;
              }
            }
            if (g[j].type == BITS_BS && strlen(g[j].value) < 3) {
              Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
              dat.chain = calloc(16,1);
              bits_RC (&dat, &g[j]);
              g[j].bytes = dat.chain;
              g[j].bitsize = (dat.byte * 8) + dat.bit;
              num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
              if (num_found) {
                goto FOUND;
              }
            }
            if (g[j].type == BITS_RC) {
              Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
              dat.chain = calloc(16,1);
              bits_BS (&dat, &g[j]);
              g[j].bytes = dat.chain;
              g[j].bitsize = (dat.byte * 8) + dat.bit;
              num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
              if (num_found) {
                goto FOUND;
              }
            }
            if (g[j].type == BITS_BD) {
              Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
              dat.chain = calloc(16,1);
              bits_RD (&dat, &g[j]);
              g[j].bytes = dat.chain;
              g[j].bitsize = (dat.byte * 8) + dat.bit;
              num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
              if (num_found) {
                goto FOUND;
              }
            }
            //relaxed BD search, less mantissa precision.
            //e.g. 49:"0.0008202099737533" (66 bits of type BD)
            //52 -> 44 bit
            if (g[j].type == BITS_BD && strlen(g[j].value) >= 3) {
              double d;
              Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
              dat.chain = calloc(16,1);

              bits_BD (&dat, &g[j]); //g.value -> dat
              g[j].bytes = dat.chain;

              if (dat.byte == 8)
                g[j].bitsize = 58; // from 66
              else
                goto FOUND;
              //print rounded found value and show bit diff
              printf("  unprecise BD search, 42bit mantissa precision\n");
              num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
              if (num_found) {
                dat.chain = (unsigned char *)unknown_dxf[i].bytes;
                dat.size = unknown_dxf[i].bitsize / 8;
                bit_set_position(&dat, g[j].pos[0]);
                d = bit_read_BD(&dat);
                if (fabs(d - strtod(g[j].value, NULL)) < 0.001) {
                  printf("  found unprecise %f value (42bit)\n", d);
                  goto FOUND;
                } else {
                  printf("  result too unprecise %f (42bit)\n", d);
                }
              }
              else {
                g[j].bitsize = 54;    // from 66
                //printf("  more unprecise BD search, 38bit mantissa precision\n");
                num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
                if (num_found) {
                  dat.chain = (unsigned char *)unknown_dxf[i].bytes;
                  dat.size = unknown_dxf[i].bitsize / 8;
                  bit_set_position(&dat, g[j].pos[0]);
                  d = bit_read_BD(&dat);
                  if (fabs(d - strtod(g[j].value, NULL)) < 0.001) {
                    printf("  found unprecise BD %f value (38bit)\n", d);
                    goto FOUND;
                  } else {
                    printf("  result too unprecise BD %f (38bit)\n", d);
                  }
                }
              }
            }
            //ditto relaxed RD search, without the BB prefix
            if (g[j].type == BITS_BD && strlen(g[j].value) >= 3) {
              double d;
              Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
              dat.chain = calloc(16,1);

              bits_RD (&dat, &g[j]); //g.value -> dat
              g[j].bytes = dat.chain;

              if (dat.byte == 8)
                g[j].bitsize = 56; // from 64
              else
                goto FOUND;
              //print rounded found value and show bit diff
              printf("  unprecise RD search, 42bit mantissa precision\n");
              num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
              if (num_found) {
                dat.chain = (unsigned char *)unknown_dxf[i].bytes;
                dat.size = unknown_dxf[i].bitsize / 8;
                bit_set_position(&dat, g[j].pos[0]);
                d = bit_read_BD(&dat);
                if (fabs(d - strtod(g[j].value, NULL)) < 0.001) {
                  printf("  found unprecise RD %f value (42bit)\n", d);
                  goto FOUND;
                } else {
                  printf("  result too unprecise RD %f (42bit)\n", d);
                }
              }
              else {
                g[j].bitsize = 52;    // from 64
                //printf("  more unprecise RD search, 38bit mantissa precision\n");
                num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
                if (num_found) {
                  dat.chain = (unsigned char *)unknown_dxf[i].bytes;
                  dat.size = unknown_dxf[i].bitsize / 8;
                  bit_set_position(&dat, g[j].pos[0]);
                  d = bit_read_BD(&dat);
                  if (fabs(d - strtod(g[j].value, NULL)) < 0.001) {
                    printf("  found unprecise RD %f value (38bit)\n", d);
                    goto FOUND;
                  } else {
                    printf("  result too unprecise RD %f (38bit)\n", d);
                  }
                }
              }
            }

            if (code == 1 && is16) { // TU not found, try TV (unsuccessful)
              Bit_Chain dat = {NULL,16,0,0,NULL,0,0};
              dat.chain = calloc(16,1);

              bits_TV (&dat, &g[j]);

              g[j].bytes = dat.chain;
              g[j].bitsize = (dat.byte * 8) + dat.bit;
              num_found = search_bits(j, &g[j], &unknown_dxf[i], &dxf[i], offset);
              if (num_found)
                goto FOUND;
            }
            continue;
          }
        FOUND:
          if (num_found == 1) {
            //oops, this looks wrong. it would rather return 2+ founds
            //still we still need to skip already reserved offsets
            if (set_found(&dxf[i], &g[j])) { //same pos, search for next
              offset = g[j].pos[0]+1;
              goto SEARCH;
            }
            else {
              printf("%d: %s [%s] found 1 at offset %d /%d\n", g[j].code, g[j].value,
                   dwg_bits_name[g[j].type], g[j].pos[0], size);
              dxf[i].num_filled += g[j].bitsize;
            }
          } else if (num_found == 2) {
            printf("%d: %s [%s] found 2 at offsets %d, %d /%d\n",
                   g[j].code, g[j].value,
                   dwg_bits_name[g[j].type],
                   g[j].pos[0], g[j].pos[1],
                   size);
          } else if (num_found == 3) {
            printf("%d: %s [%s] found 3 at offsets %d, %d, %d /%d\n",
                   g[j].code, g[j].value,
                   dwg_bits_name[g[j].type],
                   g[j].pos[0], g[j].pos[1], g[j].pos[2],
                   size);
          } else if (num_found == 4) {
            printf("%d: %s [%s] found 4 at offsets %d, %d, %d, %d /%d\n",
                   g[j].code, g[j].value,
                   dwg_bits_name[g[j].type],
                   g[j].pos[0], g[j].pos[1], g[j].pos[2], g[j].pos[3],
                   size);
          } else if (num_found == 5) {
            printf("%d: %s [%s] found 5 at offsets %d, %d, %d, %d /%d\n",
                   g[j].code, g[j].value,
                   dwg_bits_name[g[j].type],
                   g[j].pos[0], g[j].pos[1], g[j].pos[2], g[j].pos[3], g[j].pos[4],
                   size);
          } else if (num_found > 5) {
            printf("%d: %s [%s] found >5 at offsets %d, %d, %d, %d, %d, ... /%d\n",
                   g[j].code, g[j].value,
                   dwg_bits_name[g[j].type],
                   g[j].pos[0], g[j].pos[1], g[j].pos[2], g[j].pos[3], g[j].pos[4],
                   size);
          }
        }
      num_fields = j;
      // check for holes and percentage of found ranges
      /*printf("coverage: [");
      for (j=0; j<size; j++) {
        if (dxf[i].found[j]) {
          dxf[i].num_filled++;
          printf("1");
        } else {
          printf(" ");
        }
      }*/
      printf("%d/%d=%.1f%%\n", dxf[i].num_filled, size,
             100.0*dxf[i].num_filled/size);
      sum_filled += dxf[i].num_filled;
      sum_size += size;
      printf("possible: [");
      for (j=0; j<size; j++) {
        if (dxf[i].found[j]) {
          printf("x");
        } else if (dxf[i].possible[j]) {
          dxf[i].num_possible++;
          printf("%c", dxf[i].possible[j] >= 10 ? '.' : dxf[i].possible[j] + '0');
        } else {
          printf(" ");
        }
      }
      printf("]\n");

      //TODO: try likely field combinations and print the top 3.
      //there are various heuristics, like the handle stream at the end
    }

  printf("summary: %d/%d=%.2f%%\n", sum_filled, sum_size,
         100.0*sum_filled/sum_size);
  return 0;
}

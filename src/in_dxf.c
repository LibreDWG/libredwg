/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * in_dxf.c: read ascii DXF
 * written by Reini Urban
 */

#include "config.h"
#ifdef __STDC_ALLOC_LIB__
#  define __STDC_WANT_LIB_EXT2__ 1 /* for strdup */
#else
#  define _USE_BSD 1
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
//#include <ctype.h>
#include <math.h>
// strings.h or string.h
#ifdef AX_STRCASECMP_HEADER
#  include AX_STRCASECMP_HEADER
#endif

#include "common.h"
#include "bits.h"
#include "dwg.h"
#include "in_dxf.h"
#include "out_dxf.h"
#include "decode.h"
#include "encode.h"
#include "dynapi.h"
#include "hash.h"

#ifndef _DWG_API_H_
Dwg_Object *dwg_obj_generic_to_object (const void *restrict obj,
                                       int *restrict error);
#endif

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];
static long start, end; // stream offsets
static array_hdls *header_hdls = NULL;
static array_hdls *eed_hdls = NULL;

static long num_dxf_objs;  // how many elements are added
static long size_dxf_objs; // how many elements are allocated
static Dxf_Objs *dxf_objs;

static inline void
dxf_skip_ws (Bit_Chain *dat)
{
  // clang-format off
  for (; (!dat->chain[dat->byte] ||
          dat->chain[dat->byte] == ' ' ||
          dat->chain[dat->byte] == '\t' ||
          dat->chain[dat->byte] == '\r');
       dat->byte++)
    ;
  // clang-format on
}

static int
dxf_read_code (Bit_Chain *dat)
{
  char *endptr;
  long num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
  dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
  if (dat->chain[dat->byte] == '\r')
    dat->byte++;
  if (dat->chain[dat->byte] == '\n')
    dat->byte++;
  if (num > INT_MAX)
    LOG_ERROR ("%s: int overflow %ld (at %lu)", __FUNCTION__, num, dat->byte);
  return (int)num;
}

static long
dxf_read_long (Bit_Chain *dat)
{
  char *endptr;
  long num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
  dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
  if (dat->chain[dat->byte] == '\r')
    dat->byte++;
  if (dat->chain[dat->byte] == '\n')
    dat->byte++;
  if ((unsigned long)num > LONG_MAX) // or wrap to negative
    LOG_ERROR ("%s: long overflow %ld (at %lu)", __FUNCTION__, num, dat->byte);
  return num;
}

static void
dxf_read_string (Bit_Chain *dat, char **string)
{
  int i;
  dxf_skip_ws (dat);
  for (i = 0; dat->chain[dat->byte] != '\n'; dat->byte++)
    {
      buf[i++] = dat->chain[dat->byte];
    }
  if (i && buf[i - 1] == '\r')
    buf[i - 1] = '\0';
  else
    buf[i] = '\0';
  dat->byte++;
  // dxf_skip_ws (dat);
  if (!string)
    return; // ignore, just advanced dat

  if (!*string)
    *string = malloc (strlen (buf) + 1);
  else
    *string = realloc (*string, strlen (buf) + 1);
  strcpy (*string, buf);
}

static void
dxf_free_pair (Dxf_Pair *pair)
{
  if (!pair)
    return;
  if (pair->type == VT_STRING || pair->type == VT_BINARY)
    {
      free (pair->value.s);
      pair->value.s = NULL;
    }
  free (pair);
}

static Dxf_Pair *ATTRIBUTE_MALLOC
dxf_read_pair (Bit_Chain *dat)
{
  Dxf_Pair *pair = calloc (1, sizeof (Dxf_Pair));
  pair->code = (short)dxf_read_code (dat);
  pair->type = get_base_value_type (pair->code);
  switch (pair->type)
    {
    case VT_STRING:
      dxf_read_string (dat, &pair->value.s);
      LOG_TRACE ("  dxf (%d, \"%s\")\n", (int)pair->code, pair->value.s);
      // dynapi_set_helper converts from utf-8 to unicode, not here.
      // we need to know the type of the target field, if TV or T
      break;
    case VT_BOOL:
    case VT_INT8:
    case VT_INT16:
      pair->value.i = dxf_read_code (dat);
      LOG_TRACE ("  dxf (%d, %d)\n", (int)pair->code, pair->value.i);
      break;
    case VT_INT32:
      pair->value.l = dxf_read_code (dat);
      LOG_TRACE ("  dxf (%d, %ld)\n", (int)pair->code, pair->value.l);
      break;
    case VT_INT64:
      pair->value.bll = dxf_read_long (dat);
      LOG_TRACE ("  dxf (%d, " FORMAT_BLL ")\n", (int)pair->code,
                 pair->value.bll);
      break;
    case VT_REAL:
    case VT_POINT3D:
      dxf_skip_ws (dat);
      {
        const char *str = (const char *)&dat->chain[dat->byte];
        char *endp;
        pair->value.d = strtod (str, &endp);
        if (endp)
          dat->byte += endp - str;
        // sscanf ((char *)&dat->chain[dat->byte], "%lf", &pair->value.d);
      }
      LOG_TRACE ("  dxf (%d, %f)\n", pair->code, pair->value.d);
      break;
    case VT_BINARY:
      // read into buf only?
      dxf_read_string (dat, &pair->value.s);
      // cannot convert %02X to string here, because we don't store the length
      // in the pair, and binary contains \0
      LOG_TRACE ("  dxf (%d, %s)\n", (int)pair->code, pair->value.s);
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      dxf_read_string (dat, NULL);
      sscanf (buf, "%X", &pair->value.u);
      LOG_TRACE ("  dxf (%d, %X)\n", (int)pair->code, pair->value.u);
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
      return NULL;
    }
  return pair;
}

#define DXF_CHECK_EOF                                                         \
  if (dat->byte >= dat->size                                                  \
      || (pair != NULL && pair->code == 0 && strEQc (pair->value.s, "EOF")))  \
  return 1
#define DXF_RETURN_EOF(what)                                                  \
  if (dat->byte >= dat->size                                                  \
      || (pair != NULL && pair->code == 0 && strEQc (pair->value.s, "EOF")))  \
  return what
#define DXF_BREAK_EOF                                                         \
  if (dat->byte >= dat->size                                                  \
      || (pair != NULL && pair->code == 0 && strEQc (pair->value.s, "EOF")))  \
  break

static int
dxf_skip_comment (Bit_Chain *dat, Dxf_Pair *pair)
{
  while (pair->code == 999)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  return 0;
}

/*--------------------------------------------------------------------------------
 * MACROS
 */

#define ACTION indxf
//#define IS_ENCODER
//#define IS_DXF
#define IS_INDXF

/* Store all handle fieldnames and string values into this flexarray.
   We need strdup'd copies, the dxf input will be freed.
 */
array_hdls *
array_push (array_hdls *restrict hdls, char *restrict field,
            char *restrict name, short code)
{
  uint32_t i = hdls->nitems;
  if (i >= hdls->size)
    {
      hdls->size += 16;
      hdls = realloc (hdls, 8 + (hdls->size * sizeof (struct array_hdl)));
    }
  hdls->nitems = i + 1;
  hdls->items[i].field = strdup (field);
  hdls->items[i].name = strdup (name);
  hdls->items[i].code = code;
  return hdls;
}

void
free_array_hdls (array_hdls *hdls)
{
  for (uint32_t i = 0; i < hdls->nitems; i++)
    {
      free (hdls->items[i].field);
      free (hdls->items[i].name);
    }
  free (hdls);
}

#define DXF_CHECK_ENDSEC                                                      \
  if (pair != NULL && (dat->byte >= dat->size || pair->code == 0))            \
  return 0
#define DXF_BREAK_ENDSEC                                                      \
  if (pair != NULL                                                            \
      && (dat->byte >= dat->size                                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))          \
  break
#define DXF_RETURN_ENDSEC(what)                                               \
  if (pair != NULL                                                            \
      && (dat->byte >= dat->size                                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))          \
    {                                                                         \
      dxf_free_pair (pair);                                                   \
      return what;                                                            \
    }

static Dxf_Pair *
dxf_expect_code (Bit_Chain *restrict dat, Dxf_Pair *restrict pair, int code)
{
  while (pair->code != code)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      dxf_skip_comment (dat, pair);
      DXF_RETURN_EOF (pair);
      if (pair->code != code)
        {
          LOG_ERROR ("Expecting DXF code %d, got %d (at %lu)", code,
                     pair->code, dat->byte);
        }
    }
  return pair;
}

int
matches_type (Dxf_Pair *restrict pair, const Dwg_DYNAPI_field *restrict f)
{
  switch (pair->type)
    {
    case VT_STRING:
      if (f->is_string)
        return 1;
      if (f->type[0] == 'H')
        return 1; // handles can be just names
      break;
    case VT_INT64:
      // BLL or RLL
      if (f->size == 8 && f->type[1] == 'L' && f->type[2] == 'L')
        return 1;
      // fall through
    case VT_INT32:
      // BL or RL
      if (f->size == 4 && f->type[1] == 'L')
        return 1;
      // fall through
    case VT_INT16:
      // BS or RS or CMC
      if (f->size == 2 && f->type[1] == 'S')
        return 1;
      if (strEQc (f->type, "CMC"))
        return 1;
      if (strEQc (f->type, "BSd"))
        return 1;
      // fall through
    case VT_INT8:
      if (strEQc (f->type, "RC"))
        return 1;
      // fall through
    case VT_BOOL:
      if (strEQc (f->type, "B"))
        return 1;
      break;
    case VT_REAL:
      // BD or RD
      if (f->size == 8 && f->type[1] == 'D')
        return 1;
      if (strEQc (f->type, "TIMEBLL"))
        return 1;
      break;
    case VT_POINT3D:
      // 3BD or 3RD or 3DPOINT or BE
      if (f->size == 24 &&
          (f->type[0] == '3' || strEQc (f->type, "BE")))
        return 1;
      // accept 2BD or 2RD or 2DPOINT also
      if (f->size == 16 && f->type[0] == '2')
        return 1;
      break;
    case VT_BINARY:
      if (f->is_string)
        return 1;
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      if (f->type[0] == 'H')
        return 1;
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
    }
  return 0;
}

static int
dxf_header_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  // const int minimal = dwg->opts & 0x10;
  int is_utf = 1;
  int i = 0;
  Dxf_Pair *pair;

  // defaults, not often found in a DXF
  _obj->ISOLINES = 4;
  _obj->TEXTQLTY = 50;
  _obj->FACETRES = 0.5;

  // here SECTION(HEADER) was already consumed
  // read the first group 9, $field pair
  pair = dxf_read_pair (dat);

  while (pair->code == 9)
    {
      char field[80];
      strncpy (field, pair->value.s, 79);
      i = 0;

      // now read the code, value pair
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_BREAK_ENDSEC;
    next_hdrvalue:
      if (pair->code == 1 && strEQc (field, "$ACADVER"))
        {
          // Note: Here version is still R_INVALID, thus pair->value.s
          // is never TU.
          const char *version = pair->value.s;
          for (Dwg_Version_Type v = 0; v <= R_AFTER; v++)
            {
              if (strEQ (version, version_codes[v]))
                {
                  dwg->header.version = dwg->header.from_version = v;
                  dat->version = dat->from_version = dwg->header.version;
                  is_utf = dat->version >= R_2007;
                  LOG_TRACE ("HEADER.version = dat->version = %s\n", version);
                  break;
                }
              if (v == R_AFTER)
                LOG_ERROR ("Invalid HEADER: 9 %s, 1 %s", field, version)
            }
        }
      else if (field[0] == '$')
        {
          const Dwg_DYNAPI_field *f = dwg_dynapi_header_field (&field[1]);
          if (!f)
            {
              if (strEQc (field, "$3DDWFPREC"))
                {
                  LOG_TRACE ("HEADER.%s [%s %d]\n", &field[1], "BD", pair->code);
                  dwg->header_vars._3DDWFPREC = pair->value.d;
                }

#define SUMMARY_T(name)                                                       \
  (strEQc (field, "$" #name))                                                 \
  {                                                                           \
    LOG_TRACE ("SUMMARY.%s = %s [T 1]\n", &field[1], pair->value.s);          \
    if (dwg->header.version >= R_2007)                                        \
      dwg->summaryinfo.name = (BITCODE_T)bit_utf8_to_TU (pair->value.s);      \
    else                                                                      \
      dwg->summaryinfo.name = strdup (pair->value.s);                         \
  }

              else if
                SUMMARY_T (TITLE)
              else if
                SUMMARY_T (AUTHOR)
              else if
                SUMMARY_T (SUBJECT)
              else if
                SUMMARY_T (KEYWORDS)
              else if
                SUMMARY_T (COMMENTS)
              else if
                SUMMARY_T (LASTSAVEDBY)
              else
                LOG_ERROR ("skipping HEADER: 9 %s, unknown field with code %d",
                           field, pair->code);
            }
          else if (!matches_type (pair, f) && strNE (field, "$XCLIPFRAME")
                   && strNE (field, "$TIMEZONE"))
            {
              // XCLIPFRAME is 280 RC or 290 B in dynapi.
              // TIMEZONE is BLd (signed)
              LOG_ERROR (
                  "skipping HEADER: 9 %s, wrong type code %d <=> field %s",
                  field, pair->code, f->type);
            }
          else if (pair->type == VT_POINT3D)
            {
              BITCODE_3BD pt = { 0.0, 0.0, 0.0 };
              if (i)
                dwg_dynapi_header_value (dwg, &field[1], &pt, NULL);
              if (i == 0)
                pt.x = pair->value.d;
              else if (i == 1)
                pt.y = pair->value.d;
              else if (i == 2)
                pt.z = pair->value.d;
              if (i > 2)
                {
                  LOG_ERROR ("skipping HEADER: 9 %s, too many point elements",
                             field);
                }
              else
                {
                  // yes, set it 2-3 times
                  LOG_TRACE ("HEADER.%s [%s %d][%d]\n", &field[1], f->type,
                             pair->code, i);
                  dwg_dynapi_header_set_value (dwg, &field[1], &pt, is_utf);
                  i++;
                }
            }
          else if (pair->type == VT_STRING && strEQc (f->type, "H"))
            {
              char *key, *str;
              if (strlen (pair->value.s))
                {
                  LOG_TRACE ("HEADER.%s %s [%s %d] later\n", &field[1],
                             pair->value.s, f->type, (int)pair->code);
                  // name (which table?) => handle
                  // needs to be postponed, because we don't have the tables yet.
                  header_hdls = array_push (header_hdls, &field[1], pair->value.s,
                                            pair->code);
                }
              else
                {
                  BITCODE_H hdl = dwg_add_handleref (dwg, 5, 0, NULL);
                  LOG_TRACE ("HEADER.%s NULL 5 [H %d]\n", &field[1], pair->code);
                  dwg_dynapi_header_set_value (dwg, &field[1], &hdl, is_utf);
                }
            }
          else if (strEQc (f->type, "H"))
            {
              BITCODE_H hdl;
              hdl = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("HEADER.%s %X [H %d]\n", &field[1], pair->value.u,
                         pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &hdl, is_utf);
            }
          else if (strEQc (f->type, "CMC"))
            {
              static BITCODE_CMC color = { 0 };
              if (pair->code <= 70)
                {
                  LOG_TRACE ("HEADER.%s.index %d [CMC %d]\n", &field[1],
                             pair->value.i, pair->code);
                  color.index = pair->value.i;
                  dwg_dynapi_header_set_value (dwg, &field[1], &color, 0);
                }
            }
          else if (pair->type == VT_REAL && strEQc (f->type, "TIMEBLL"))
            {
              static BITCODE_TIMEBLL date = { 0, 0, 0 };
              unsigned long j = 1;
              double ms;
              date.value = pair->value.d;
              date.days = (BITCODE_BL)trunc (pair->value.d);
              ms = date.value;
              while (ms > 1.0)
                {
                  j *= 10;
                  ms /= 10.0;
                }
              // date.ms = (BITCODE_BL)(1000000 * (date.value - date.days));
              date.ms = (BITCODE_BL) (j / 10 * (date.value - date.days));
              LOG_TRACE ("HEADER.%s %f (%u, %u) [TIMEBLL %d]\n", &field[1],
                         date.value, date.days, date.ms, pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &date, 0);
            }
          else if (pair->type == VT_STRING)
            {
              LOG_TRACE ("HEADER.%s [%s %d]\n", &field[1], f->type, pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &pair->value, 1);
            }
          else
            {
              LOG_TRACE ("HEADER.%s [%s %d]\n", &field[1], f->type, pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &pair->value, is_utf);
            }
        }
      else
        {
          LOG_ERROR ("skipping HEADER: 9 %s, missing the $", field);
        }

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_BREAK_ENDSEC;
      if (pair->code != 9 /* && pair->code != 0 */)
        goto next_hdrvalue; // for mult. 10,20,30 values
    }

  SINCE(R_2000)
  {
    _obj->CELWEIGHT = dxf_revcvt_lweight (_obj->CELWEIGHT);
    LOG_TRACE ("HEADER.%s => %d\n", "CELWEIGHT", _obj->CELWEIGHT);
    // clang-format off
    _obj->FLAGS = (_obj->CELWEIGHT & 0x1f) |
          (_obj->ENDCAPS     ? 0x60   : 0) |
          (_obj->JOINSTYLE   ? 0x180  : 0) |
          (_obj->LWDISPLAY   ? 0 : 0x200)  |
          (_obj->XEDIT       ? 0 : 0x400)  |
          (_obj->EXTNAMES    ? 0x800  : 0) |
          (_obj->PSTYLEMODE  ? 0x2000 : 0) |
          (_obj->OLESTARTUP  ? 0x4000 : 0);
    // clang-format on
    LOG_TRACE ("HEADER.%s => 0x%x\n", "FLAGS", (unsigned)_obj->FLAGS);
  }

  dxf_free_pair (pair);
  return 0;
}

static void
dxf_fixup_header (Dwg_Data *dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  struct Dwg_Header *hdr = &dwg->header;
  //struct Dwg_AuxHeader *aux = &dwg->auxheader;
  LOG_TRACE ("dxf_fixup_header\n");

  vars->HANDSEED->handleref.code = 0;
  if (vars->DWGCODEPAGE)
    {
      if (strEQc (vars->DWGCODEPAGE, "ANSI_1252"))
        hdr->codepage = 30;
      else if (strEQc (vars->DWGCODEPAGE, "UTF-8"))
        hdr->codepage = 30;
      else if (strEQc (vars->DWGCODEPAGE, "US_ASCII"))
        hdr->codepage = 1;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-1"))
        hdr->codepage = 2;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-2"))
        hdr->codepage = 3;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-3")) //unused?
        hdr->codepage = 4;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-4"))
        hdr->codepage = 5;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-5"))
        hdr->codepage = 6;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-6"))
        hdr->codepage = 7;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-7"))
        hdr->codepage = 8;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-8"))
        hdr->codepage = 9;
      else if (strEQc (vars->DWGCODEPAGE, "ISO-8859-9"))
        hdr->codepage = 10;
      else if (strEQc (vars->DWGCODEPAGE, "CP437"))
        hdr->codepage = 11;
      else if (strEQc (vars->DWGCODEPAGE, "CP850"))
        hdr->codepage = 12;
      else if (strEQc (vars->DWGCODEPAGE, "CP852"))
        hdr->codepage = 13;
      else if (strEQc (vars->DWGCODEPAGE, "CP855"))
        hdr->codepage = 14;
      else if (strEQc (vars->DWGCODEPAGE, "CP857"))
        hdr->codepage = 15;
      else if (strEQc (vars->DWGCODEPAGE, "CP860"))
        hdr->codepage = 16;
      else if (strEQc (vars->DWGCODEPAGE, "CP861"))
        hdr->codepage = 17;
      else if (strEQc (vars->DWGCODEPAGE, "CP863"))
        hdr->codepage = 18;
      else if (strEQc (vars->DWGCODEPAGE, "CP864"))
        hdr->codepage = 19;
      else if (strEQc (vars->DWGCODEPAGE, "CP865"))
        hdr->codepage = 20;
      else if (strEQc (vars->DWGCODEPAGE, "CP869"))
        hdr->codepage = 21;
      else if (strEQc (vars->DWGCODEPAGE, "CP932"))
        hdr->codepage = 22;
      else if (strEQc (vars->DWGCODEPAGE, "MACINTOSH"))
        hdr->codepage = 23;
      else if (strEQc (vars->DWGCODEPAGE, "BIG5"))
        hdr->codepage = 24;
      else if (strEQc (vars->DWGCODEPAGE, "CP949"))
        hdr->codepage = 25;
      else if (strEQc (vars->DWGCODEPAGE, "JOHAB"))
        hdr->codepage = 27;
      else if (strEQc (vars->DWGCODEPAGE, "CP866"))
        hdr->codepage = 27;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1250"))
        hdr->codepage = 28;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1251"))
        hdr->codepage = 29;
      else if (strEQc (vars->DWGCODEPAGE, "GB2312"))
        hdr->codepage = 31;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1253"))
        hdr->codepage = 32;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1254"))
        hdr->codepage = 33;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1255"))
        hdr->codepage = 34;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1256"))
        hdr->codepage = 35;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1257"))
        hdr->codepage = 36;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_874"))
        hdr->codepage = 37;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_932"))
        hdr->codepage = 38;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_936"))
        hdr->codepage = 39;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_949"))
        hdr->codepage = 40;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_950"))
        hdr->codepage = 41;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1258"))
        hdr->codepage = 44;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1361"))
        hdr->codepage = 42;
      else if (strEQc (vars->DWGCODEPAGE, "UTF-16"))
        hdr->codepage = 43;
      else if (strEQc (vars->DWGCODEPAGE, "ANSI_1258"))
        hdr->codepage = 44;
      else
        hdr->codepage = 0;
    }
  LOG_TRACE ("HEADER.codepage = %d [%s]\n", hdr->codepage, vars->DWGCODEPAGE);

  // R_2007:
  // is_maint: 0x32 [RC 0]
  // zero_one_or_three: 0x3 [RC 0]
  // thumbnail_addr: 3360 [RL 0]
  // dwg_version: 0x1f [RC 0]
  // maint_version: 0x8 [RC 0]
  // codepage: 30 [RS 0]
  // R_2004+:
  // unknown_0: 0x0 [RC 0]
  // app_dwg_version: 0x1f [RC 0]
  // app_maint_version: 0x8 [RC 0]
  // security_type: 0 [RL 0]
  // rl_1c_address: 0 [RL 0]
  // summary_info_address: 3200 [RL 0]
  // vba_proj_address: 0 [RL 0]
  // rl_28_80: 128 [RL 0]

  // R_2000:
  // is_maint: 0xf [RC 0]
  // zero_one_or_three: 0x1 [RC 0]
  // thumbnail_addr: 220 [RL 0]
  // dwg_version: 0x1f [RC 0]
  // maint_version: 0x8 [RC 0]
  // codepage: 30 [RS 0]

  if (hdr->version <= R_14)
    hdr->is_maint = 0x0;
  else if (hdr->version <= R_2000)
    {
      hdr->is_maint = 0xf; // 0x6 - 0xf
      hdr->zero_one_or_three = 1;
      hdr->thumbnail_address = 220;
      hdr->dwg_version = 0x21;
      hdr->maint_version = 0x8;
    }
  else if (hdr->version <= R_2004)
    hdr->is_maint = 0x68;
  else if (hdr->version <= R_2007)
    hdr->is_maint = 0x32;
  else if (hdr->version <= R_2010)
    hdr->is_maint = 0x6d;
  else if (hdr->version <= R_2013)
    hdr->is_maint = 0x7d;
  else if (hdr->version <= R_2018)
    hdr->is_maint = 0x4;

  if (!vars->FINGERPRINTGUID)
    vars->FINGERPRINTGUID = strdup ("{00000000-0000-0000-0000-000000000000}");
  if (!vars->VERSIONGUID)
    vars->VERSIONGUID
        = strdup ("{DE6A95C3-2D01-4A77-AC28-3C42FCFFF657}"); // R_2000
}

static int
dxf_classes_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  BITCODE_BL i;
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Class *klass;

  while (1)
    { // read next class
      // add class (see decode)
      i = dwg->num_classes;
      if (i == 0)
        dwg->dwg_class = malloc (sizeof (Dwg_Class));
      else
        dwg->dwg_class
            = realloc (dwg->dwg_class, (i + 1) * sizeof (Dwg_Class));
      if (!dwg->dwg_class)
        {
          LOG_ERROR ("Out of memory");
          return DWG_ERR_OUTOFMEM;
        }

      klass = &dwg->dwg_class[i];
      memset (klass, 0, sizeof (Dwg_Class));
      if (pair->code == 0 && strEQc (pair->value.s, "CLASS"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
      klass->number = 500 + i;
      while (pair->code != 0)
        { // read until next 0 CLASS
          switch (pair->code)
            {
            case 1:
              {
                const char *n = strEQc (pair->value.s, "ACDBDATATABLE")
                  ? "DATATABLE" : pair->value.s;
                STRADD (klass->dxfname, n);
                LOG_TRACE ("CLASS[%d].dxfname = %s [TV 1]\n", i, n);
                break;
              }
            case 2:
              STRADD (klass->cppname, pair->value.s);
              LOG_TRACE ("CLASS[%d].cppname = %s [TV 2]\n", i, pair->value.s);
              break;
            case 3:
              STRADD (klass->appname, pair->value.s);
              LOG_TRACE ("CLASS[%d].appname = %s [TV 3]\n", i, pair->value.s);
              break;
            case 90:
              klass->proxyflag = pair->value.l;
              LOG_TRACE ("CLASS[%d].proxyflag = %ld [BS 90]\n", i, pair->value.l);
              break;
            case 91:
              klass->num_instances = pair->value.l;
              LOG_TRACE ("CLASS[%d].num_instances = %ld [BL 91]\n", i,
                         pair->value.l);
              break;
            case 280:
              klass->wasazombie = (BITCODE_B)pair->value.i;
              LOG_TRACE ("CLASS[%d].wasazombie = %d [B 280]\n", i,
                         pair->value.i);
              break;
            case 281:
              klass->item_class_id = pair->value.i ? 0x1f2 : 0x1f3;
              LOG_TRACE ("CLASS[%d].item_class_id = %x [BS 281]\n", i,
                         klass->item_class_id);
              break;
            default:
              LOG_WARN ("Unknown DXF code for class[%d].%d", i, pair->code);
              break;
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
      dwg->num_classes++;
      DXF_RETURN_ENDSEC (0) // next class or ENDSEC
    }
  dxf_free_pair (pair);
  return 0;
}

void
add_eed (Dwg_Object *restrict obj, const char *restrict name,
         Dxf_Pair *restrict pair)
{
  int code, size, j = 0;
  int i;
  Dwg_Eed *eed;
  Dwg_Data *dwg = obj->parent;

  i = obj->tio.object->num_eed; // same layout for Object and Entity
  eed = obj->tio.object->eed;

  if (pair->code < 1020 || pair->code > 1035) // followup y and z pairs
    {
      if (i)
        {
          eed = (Dwg_Eed *)realloc (eed, (i + 1) * sizeof (Dwg_Eed));
          memset (&eed[i], 0, sizeof (Dwg_Eed));
        }
      else
        {
          eed = (Dwg_Eed *)calloc (1, sizeof (Dwg_Eed));
        }
      obj->tio.object->eed = eed;
      obj->tio.object->num_eed++;
    }
  else
    i--;
  code = pair->code - 1000; // 1000
  //LOG_TRACE ("EED[%d] code:%d\n", i, code);
  switch (code)
    {
    case 0:
      {
        int len = strlen (pair->value.s);
        if (dwg->header.version < R_2007)
          {
            /* code [RC] + len+0 + length [RC] + codepage [RS] */
            size = 1 + len + 1 + 1 + 2;
            eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
            eed[i].data->code = code; // 1000
            eed[i].data->u.eed_0.length = len;
            eed[i].data->u.eed_0.codepage = dwg->header.codepage;
            if (len && len < 256)
              {
                LOG_HANDLE ("EED[%d] \"%s\" [TV %d]\n", i, pair->value.s, code);
                memcpy (eed[i].data->u.eed_0.string, pair->value.s, len + 1);
              }
          }
        else
          {
            /* code [RC] + 2*len+00 + length [TU] + codepage [RS] */
            size = 1 + len * 2 + 2 + 2 + 2;
            eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
            eed[i].data->code = code;
            eed[i].data->u.eed_0_r2007.length = len;
            eed[i].data->u.eed_0.codepage = obj->parent->header.codepage;
            if (len && len < 32767)
              {
                BITCODE_TU tu = bit_utf8_to_TU (pair->value.s);
                LOG_HANDLE ("EED[%d] \"%s\" [TU %d]\n", i, pair->value.s, code);
                memcpy (eed[i].data->u.eed_0_r2007.string, tu, 2 * (len + 1));
                free (tu);
              }
          }
        eed[i].size += size;
      }
      break;
    case 2:
      /* code [RC] + byte [RC] */
      size = 1 + 1;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1002
      eed[i].data->u.eed_2.byte = (BITCODE_RC)pair->value.i;
      LOG_HANDLE ("EED[%d] %d [RC %d]\n", i, pair->value.i, code);
      eed[i].size += size;
      break;
    case 4:
      {
        // BINARY
        const char *pos = pair->value.s;
        int len = strlen (pos);
        /* code [RC] + len+0 + length [RC] */
        size = 1 + len / 2 + 1 + 1;
        eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
        eed[i].data->code = code; // 1004
        eed[i].data->u.eed_4.length = len / 2;
        LOG_HANDLE ("EED[%d] [TF %d %d]\n", i, len, code);
        for (j = 0; j < len / 2; j++)
          {
            sscanf (pos, "%2hhX", &eed[i].data->u.eed_4.data[j]);
            pos += 2;
          }
        eed[i].size += size;
      }
      break;
    case 10: // VT_POINT3D
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
      /* code [RC] + 3*RD */
      size = 1 + 3 * 8;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code;
      eed[i].data->u.eed_10.point.x = pair->value.d;
      eed[i].size += size;
      break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
      if (i < 0)
        return;
      eed[i].data->u.eed_10.point.y = pair->value.d;
      break;
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
      if (i < 0)
        return;
      eed[i].data->u.eed_10.point.z = pair->value.d;
      LOG_HANDLE ("EED[%d] (%f,%f,%f) [3RD %d]\n", i, eed[i].data->u.eed_10.point.x,
                  eed[i].data->u.eed_10.point.y, pair->value.d, code-20);
      break;
    case 40:
    case 41:
    case 42:
      /* code [RC] + 3*RD */
      size = 1 + 8;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_40.real = pair->value.d;
      LOG_HANDLE ("EED[%d] %f [RD %d]\n", i, pair->value.d, code);
      eed[i].size += size;
      break;
    case 70:
      /* code [RC] + RS */
      size = 1 + 2;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_70.rs = pair->value.i;
      LOG_HANDLE ("EED[%d] %d [RS %d]\n", i, pair->value.i, code);
      eed[i].size += size;
      break;
    case 71:
      /* code [RC] + RL */
      size = 1 + 4;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_71.rl = pair->value.l;
      LOG_HANDLE ("EED[%d] %ld [RL %d]\n", i, pair->value.l, code);
      eed[i].size += size;
      break;
    case 1:
      if (!i)
        obj->tio.object->num_eed--;
      eed[i].size += sizeof (Dwg_Handle);
      if (strEQc (pair->value.s, "ACAD"))
        {
          dwg_add_handle (&eed[i].handle, 5, 0x12, NULL);
          LOG_TRACE ("EED[%d] 12 [H] for APPID.%s\n", i, pair->value.s);
        }
      else
        {
          // search name in APPID table (if already added)
          BITCODE_H hdl;
          hdl = dwg_find_tablehandle (dwg, pair->value.s, "APPID");
          if (hdl)
            {
              memcpy (&eed[i].handle, &hdl->handleref, sizeof (Dwg_Handle));
              LOG_TRACE ("EED[%d] %lX [H] for APPID.%s\n", i, hdl->absolute_ref,
                         pair->value.s);
            }
          // needs to be postponed, because we don't have the tables yet
          else
            {
              char idx[12];
              snprintf (idx, 12, "%d", obj->index);
              eed_hdls = array_push (eed_hdls, idx, pair->value.s, (short)i);
              LOG_TRACE ("EED[%d] ? [H} for APPID.%s later\n", i,
                          pair->value.s);
            }
        }
      break;
    case 5:
      {
        // HANDLE (absref)
        const char *pos = pair->value.s;
        unsigned long l = 0;
        /* code [RC] + BLL */
        size = 1 + 8;
        eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
        eed[i].data->code = code; // 1005
        sscanf (pos, "%lX", &l);
        eed[i].data->u.eed_5.entity = (BITCODE_RLL)l;
        LOG_TRACE ("EED[%d] %lX [H %d]\n", i, l, code);
        eed[i].size += size;
        break;
      }
    default:
      LOG_ERROR ("Not yet implemented EED[%d] code %d", i, pair->code);
    }
  return;
}

int
is_table_name (const char *name)
{
  return strEQc (name, "LTYPE") || strEQc (name, "VPORT")
         || strEQc (name, "VPORT") || strEQc (name, "APPID")
         || strEQc (name, "BLOCK") || strEQc (name, "LAYER")
         || strEQc (name, "DIMSTYLE") || strEQc (name, "STYLE")
         || strEQc (name, "VIEW") || strEQc (name, "VPORT_ENTITY")
         || strEQc (name, "UCS") || strEQc (name, "BLOCK_RECORD")
         || strEQc (name, "BLOCK_HEADER");
}

static Dxf_Pair *
new_MLINESTYLE_lines (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                      Dxf_Pair *restrict pair)
{
  int num_lines = pair->value.i;
  Dwg_Object_MLINESTYLE *_o = obj->tio.object->tio.MLINESTYLE;
  Dwg_Data *dwg = obj->parent;
  _o->lines = calloc (num_lines, sizeof (Dwg_MLINESTYLE_line));
  for (int j = -1; j < (int)num_lines;)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (pair->code == 0)
        return pair;
      else if (pair->code == 49)
        {
          j++;
          assert (j < num_lines);
          _o->lines[j].offset = pair->value.d;
          LOG_TRACE ("MLINESTYLE.lines[%d].offset = %f [BD 49]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 62)
        {
          if (j < 0)
            j++;
          assert (j < num_lines);
          _o->lines[j].color.index = pair->value.i;
          LOG_TRACE ("MLINESTYLE.lines[%d].color.index = %d [CMC 62]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 420)
        {
          if (j < 0)
            j++;
          assert (j < num_lines);
          _o->lines[j].color.rgb = pair->value.u;
          LOG_TRACE ("MLINESTYLE.lines[%d].color.rgb = %06X [CMC 420]\n", j,
                     pair->value.u);
        }
      else if (pair->code == 6)
        {
          if (j < 0)
            j++;
          assert (j < num_lines);
          if (strEQc (pair->value.s, "BYLAYER")
              || strEQc (pair->value.s, "ByLayer"))
            _o->lines[j].lt.index
                = 32767; // TODO SHRT_MAX, but should be -1 really
          else if (strEQc (pair->value.s, "BYBLOCK")
                   || strEQc (pair->value.s, "ByBlock"))
            _o->lines[j].lt.index = 32766; // TODO should be -2 really
          else if (strEQc (pair->value.s, "CONTINUOUS")
                   || strEQc (pair->value.s, "Continuous"))
            _o->lines[j].lt.index = 0;
          else // lookup on LTYPE_CONTROL list
            {
              BITCODE_H hdl;
              if ((hdl = dwg_find_tablehandle (dwg, pair->value.s, "LTYPE")))
                {
                  hdl->handleref.code = 5;
                  _o->lines[j].lt.ltype = hdl;
                  LOG_TRACE ("MLINESTYLE.lines[%d].lt.ltype %s => " FORMAT_REF
                             " [H 6]\n",
                             j, pair->value.s, ARGS_REF (hdl));
                }
            }
          if (_o->lines[j].lt.index >= -2 && _o->lines[j].lt.index <= 0)
            LOG_TRACE ("MLINESTYLE.lines[%d].lt.index = %d [BSd 6]\n", j,
                       _o->lines[j].lt.index);
        }
      else
        break; // not a Dwg_MLINESTYLE_line
    }
  return pair;
}

static Dxf_Pair *
new_LWPOLYLINE (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                Dxf_Pair *restrict pair)
{
  BITCODE_BL num_points = pair->value.u;
  Dwg_Entity_LWPOLYLINE *_o = obj->tio.entity->tio.LWPOLYLINE;
  int j = -1;

  _o->num_points = num_points;
  _o->points = calloc (num_points, sizeof (BITCODE_2RD));
  _o->bulges = calloc (num_points, sizeof (BITCODE_BD));
  _o->num_bulges = num_points;
  LOG_TRACE ("LWPOLYLINE.num_points = %u [BS 90]\n", num_points);

  while (pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (pair->code == 0)
        return pair;
      else if (pair->code == 43)
        {
          _o->const_width = pair->value.d;
          _o->flag |= 4;
          LOG_TRACE ("LWPOLYLINE.const_width = %f [BD 43]\n", pair->value.d);
        }
      else if (pair->code == 70)
        {
          _o->flag |= pair->value.i; /* only if closed or not */
          LOG_TRACE ("LWPOLYLINE.flag = %d [BS 70]\n", pair->value.i);
        }
      else if (pair->code == 38)
        {
          _o->elevation = pair->value.d;
          _o->flag |= 8;
          LOG_TRACE ("LWPOLYLINE.elevation = %f [38 BD]\n", pair->value.d);
        }
      else if (pair->code == 39)
        {
          _o->thickness = pair->value.d;
          _o->flag |= 2;
          LOG_TRACE ("LWPOLYLINE.thickness = %f [BD 39]\n", pair->value.d);
        }
      else if (pair->code == 210)
        {
          _o->extrusion.x = pair->value.d;
        }
      else if (pair->code == 220)
        {
          _o->extrusion.y = pair->value.d;
        }
      else if (pair->code == 230)
        {
          _o->extrusion.z = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.extrusion = (%f, %f, %f) [3BD 210]\n",
                     _o->extrusion.x, _o->extrusion.y, _o->extrusion.z);
        }
      else if (pair->code == 10)
        {
          j++; // we always start with 10 (I hope)
          assert (j < (int)_o->num_points);
          _o->points[j].x = pair->value.d;
        }
      else if (pair->code == 20)
        {
          assert (j < (int)_o->num_points);
          _o->points[j].y = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.points[%d] = (%f, %f) [2RD 10]\n", j,
                     _o->points[j].x, _o->points[j].y);
        }
      else if (pair->code == 42)
        {
          assert (j < (int)_o->num_bulges);
          _o->bulges[j] = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.bulges[%d] = %f [BD 42]\n", j, pair->value.d);
        }
      else if (pair->code == 91)
        {
          if (!j)
            {
              _o->vertexids = calloc (num_points, sizeof (BITCODE_2RD));
              _o->num_vertexids = num_points;
            }
          assert (j < (int)_o->num_vertexids);
          _o->vertexids[j] = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.vertexids[%d] = %f [BD 91]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 40) // not const_width
        {
          if (!j)
            {
              _o->flag |= 4;
              _o->widths = calloc (num_points, sizeof (Dwg_LWPOLYLINE_width));
              _o->num_widths = num_points;
            }
          assert (j < (int)_o->num_widths);
          _o->widths[j].start = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.widths[%d].start = %f [BD 40]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 41 && (_o->flag & 4)) // not const_width
        {
          assert (j < (int)_o->num_widths);
          _o->widths[j].end = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.widths[%d].end = %f [BD 41]\n", j,
                     pair->value.d);
        }
      else if (pair->code >= 1000 && pair->code < 1999)
        {
          add_eed (obj, "LWPOLYLINE", pair);
        }
      else
        LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "LWPOLYLINE");
    }
  return pair;
}

// only code 1
static Dxf_Pair *
add_3DSOLID_encr (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                  Dxf_Pair *restrict pair)
{
  Dwg_Entity_3DSOLID *o = obj->tio.entity->tio._3DSOLID;
  int i = 0, total = 0;
  o->num_blocks = 1;
  o->encr_sat_data = calloc (2, sizeof (char *));
  o->encr_sat_data[0] = NULL;
  o->block_size = calloc (2, sizeof (BITCODE_BL));

  while (pair->code == 1)
    {
      int len = strlen (pair->value.s) + 1; // + the \n
      if (!total)
        {
          total = len;
          o->encr_sat_data[0] = malloc (total + 1); // + the \0
          // memcpy (o->encr_sat_data[0], pair->value.s, len + 1);
          strcpy (o->encr_sat_data[0], pair->value.s);
        }
      else
        {
          total += len;
          o->encr_sat_data[0] = realloc (o->encr_sat_data[0], total + 1);
          strcat (o->encr_sat_data[0], pair->value.s);
        }
      strcat (o->encr_sat_data[0], "\n");
      i++;

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  LOG_TRACE ("%s.block_size[0]: %d\n", obj->name, total);

  if (o->version == 1)
    {
      int idx = 0;
      o->unknown = 1; // ??
      o->acis_data = calloc (1, total + 1);
      for (i = 0; i < total; i++)
        {
          if (o->encr_sat_data[0][i] == '^' && i <= total
              && o->encr_sat_data[0][i + 1] == ' ')
            {
              o->acis_data[idx++] = 'A';
              i++;
            }
          else if (o->encr_sat_data[0][i] <= 32)
            o->acis_data[idx++] = o->encr_sat_data[0][i];
          else
            o->acis_data[idx++] = 159 - o->encr_sat_data[0][i];
        }
      o->acis_data[idx] = '\0';
      o->block_size[0] = idx;
      LOG_TRACE ("%s.acis_data:\n%s\n", obj->name, o->acis_data);
    }

  return pair;
}

static Dxf_Pair *
add_MESH (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
          Dxf_Pair *restrict pair)
{
  Dwg_Entity_MESH *o = obj->tio.entity->tio.MESH;
  BITCODE_BL j = 0;
  int vector = 0;

  // valid entry code: 91
  if (pair->code == 91)
    {
      vector = pair->code;
      o->num_subdiv_vertex = pair->value.u;
      if (pair->value.u)
        o->subdiv_vertex = calloc (o->num_subdiv_vertex, sizeof (BITCODE_3BD));
      LOG_TRACE ("MESH.num_subdiv_vertex = %u [BL 91]\n", pair->value.u);
    }

  while (pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);

      if (pair->code == 0)
        return pair;
      else if (pair->code == 92)
        {
          j = 0;
          vector = pair->code;
          o->num_vertex = pair->value.u;
          LOG_TRACE ("MESH.num_vertex = %u [BL 92]\n", pair->value.u);
          if (pair->value.u)
            o->vertex = calloc (o->num_vertex, sizeof (BITCODE_3BD));
        }
      else if (pair->code == 93)
        {
          j = 0;
          vector = pair->code;
          o->num_faces = pair->value.u;
          LOG_TRACE ("MESH.num_faces = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u)
            o->faces = calloc (o->num_faces, sizeof (BITCODE_BL));
        }
      else if (pair->code == 94)
        {
          j = 0;
          vector = pair->code;
          o->num_edges = pair->value.u;
          LOG_TRACE ("MESH.num_edges = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u) // from face - to face
            o->edges = calloc (o->num_edges, sizeof (Dwg_MESH_edge));
        }
      else if (pair->code == 95)
        {
          j = 0;
          vector = pair->code;
          o->num_crease = pair->value.u;
          LOG_TRACE ("MESH.num_crease = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u)
            o->crease = calloc (o->num_crease, sizeof (BITCODE_BD));
        }
      else if (pair->code == 10)
        {
          if (vector == 91)
            {
              assert (j < o->num_subdiv_vertex);
              o->subdiv_vertex[j].x = pair->value.d;
            }
          else if (vector == 92)
            {
              assert (j < o->num_vertex);
              o->vertex[j].x = pair->value.d;
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 20)
        {
          if (vector == 91)
            {
              assert (j < o->num_subdiv_vertex);
              o->subdiv_vertex[j].y = pair->value.d;
            }
          else if (vector == 92)
            {
              assert (j < o->num_vertex);
              o->vertex[j].y = pair->value.d;
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 30)
        {
          if (vector == 91)
            {
              assert (j < o->num_subdiv_vertex);
              o->subdiv_vertex[j].z = pair->value.d;
              LOG_TRACE ("MESH.subdiv_vertex[%d] = (%f, %f, %f) [3BD 10]\n", j,
                         o->subdiv_vertex[j].x, o->subdiv_vertex[j].y,
                         o->subdiv_vertex[j].z);
              j++;
            }
          else if (vector == 92)
            {
              assert (j < o->num_vertex);
              o->vertex[j].z = pair->value.d;
              LOG_TRACE ("MESH.vertex[%d] = (%f, %f, %f) [3BD 10]\n", j,
                         o->vertex[j].x, o->vertex[j].y, o->vertex[j].z);
              j++;
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 90)
        {
          if (vector == 93)
            {
              assert (j < o->num_faces);
              o->faces[j] = pair->value.u;
              LOG_TRACE ("MESH.faces[%d] = %u [BL %d]\n", j, pair->value.u,
                         pair->code);
              j++;
            }
          else if (vector == 94)
            {
              int i = j / 2;
              assert (j < 2 * o->num_edges);
              if (j % 2 == 0)
                {
                  o->edges[i].from = pair->value.u;
                }
              else
                {
                  o->edges[i].to = pair->value.u;
                  LOG_TRACE ("MESH.edges[%d] = (%u, %u) [2BL %d]\n", i,
                             o->edges[i].from, pair->value.u, pair->code);
                }
              j++;
            }
          else if (vector == 95) //??
            {
              o->class_version = pair->value.u;
              LOG_TRACE ("MESH.class_version = %u [BL %d]\n", pair->value.u,
                         pair->code);
            }
          else
            goto mesh_error;
        }
      else if (pair->code == 140)
        {
          if (vector == 95)
            {
              assert (j < o->num_crease);
              o->crease[j] = pair->value.u;
              LOG_TRACE ("MESH.crease[%d] = %u [BD %d]\n", j, pair->value.u,
                         pair->code);
              j++;
            }
          else
            goto mesh_error;
        }
      else if (pair->code >= 1000 && pair->code < 1999)
        {
          add_eed (obj, "MESH", pair);
        }
      else
        {
        mesh_error:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "MESH");
        }
    }
  return pair;
}

static Dxf_Pair *
add_HATCH (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
           Dxf_Pair *restrict pair)
{
  BITCODE_BL num_paths; // 91
  Dwg_Entity_HATCH *o = obj->tio.entity->tio.HATCH;
  int is_plpath = 0;
  int j = -1;
  int k = -1;
  int l = -1;

  // valid entry codes
  if (pair->code == 91)
    {
      o->num_paths = pair->value.u;
      o->paths = calloc (o->num_paths, sizeof (Dwg_HATCH_Path));
      LOG_TRACE ("HATCH.num_paths = %u [BS 91]\n", o->num_paths);
    }
  else if (pair->code == 78)
    {
      o->num_deflines = pair->value.l;
      LOG_TRACE ("HATCH.num_deflines = %ld [BS 78]\n", pair->value.l);
      o->deflines = calloc (pair->value.l, sizeof (Dwg_HATCH_DefLine));
    }
  if (pair->code == 453)
    {
      o->num_colors = pair->value.l;
      LOG_TRACE ("HATCH.num_colors = %ld [BL 453]\n", pair->value.l);
      o->colors = calloc (pair->value.l, sizeof (Dwg_HATCH_Color));
    }

  while (pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);

      if (pair->code == 0 || pair->code == 75)
        return pair;
      else if (pair->code == 92)
        {
          j++;
          assert (j < (int)o->num_paths);
          o->paths[j].flag = pair->value.u;
          LOG_TRACE ("HATCH.paths[%d].flag = %u [BL 92]\n", j, pair->value.u);
          is_plpath = pair->value.u & 2;
          o->has_derived = pair->value.u & 4;
          LOG_TRACE ("HATCH.has_derived = %d [B 0]\n", o->has_derived);
        }
      else if (pair->code == 93)
        {
          assert (j < (int)o->num_paths);
          o->paths[j].num_segs_or_paths = pair->value.u;
          LOG_TRACE ("HATCH.paths[%d].num_segs_or_paths = %u [BL 93]\n", j,
                     pair->value.u);
          k = -1;
          if (!is_plpath)
            { /* segs */
              o->paths[j].segs
                  = calloc (pair->value.u, sizeof (Dwg_HATCH_PathSeg));
            }
          else
            { /* polyline path */
              o->paths[j].polyline_paths
                  = calloc (pair->value.u, sizeof (Dwg_HATCH_PolylinePath));
            }
        }
      else if (pair->code == 72)
        {
          assert (j < (int)o->num_paths);
          if (!is_plpath)
            {
              k++;
              if (j < (int)o->num_paths
                  && k < (int)o->paths[j].num_segs_or_paths)
                {
                  o->paths[j].segs[k].type_status = pair->value.i;
                  LOG_TRACE (
                      "HATCH.paths[%d].segs[%d].type_status = %d [RC 72]\n", j,
                      k, pair->value.i);
                }
            }
          else
            {
              o->paths[j].bulges_present = pair->value.i;
              LOG_TRACE ("HATCH.paths[%d].bulges_present = %d [RC 72]\n", j,
                         pair->value.i);
            }
        }
      else if (pair->code == 73 && is_plpath && pair->value.i)
        {
          assert (j < (int)o->num_paths);
          o->paths[j].closed = pair->value.i;
          LOG_TRACE ("HATCH.paths[%d].closed = %d [RC 73]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 94 && !is_plpath && pair->value.l)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].degree = pair->value.l;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].degree = %ld [BL 94]\n", j, k,
                     pair->value.l);
        }
      else if (pair->code == 74 && !is_plpath && pair->value.i)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].is_periodic = pair->value.i;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].is_periodic = %d [B 74]\n", j,
                     k, pair->value.i);
        }
      else if (pair->code == 95 && !is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].num_knots = pair->value.l;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].num_knots = %ld [BL 95]\n", j,
                     k, pair->value.l);
          o->paths[j].segs[k].knots = calloc (pair->value.l, sizeof (double));
          l = -1;
        }
      else if (pair->code == 96 && !is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].num_control_points = pair->value.l;
          LOG_TRACE (
              "HATCH.paths[%d].segs[%d].num_control_points = %ld [BL 96]\n", j,
              k, pair->value.l);
          o->paths[j].segs[k].control_points
              = calloc (pair->value.l, sizeof (Dwg_HATCH_ControlPoint));
          l = -1;
        }
      else if (pair->code == 10 && !is_plpath && !o->num_seeds)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].first_endpoint.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].first_endpoint.x = %f [10
              // 2BD]\n",
              //           j, k, pair->value.d);
              break;
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].center.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].center.x = %f [10
              // 2BD]\n",
              //           j, k, pair->value.d);
              break;
            case 4: /* SPLINE */
              l++;
              assert (l < (int)o->paths[j].segs[k].num_control_points);
              o->paths[j].segs[k].control_points[l].point.x = pair->value.d;
              // LOG_TRACE
              // ("HATCH.paths[%d].segs[%d].control_points[%d].point.x = %f [10
              // 2BD]\n",
              //           j, k, l, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].type_status %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].type_status, pair->code);
            }
        }
      else if (pair->code == 11 && !is_plpath && !o->num_seeds)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].second_endpoint.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].second_endpoint.x = %f
              // [2BD 11]\n",
              //           j, k, pair->value.d);
              break;
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].endpoint.x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].endpoint.x = %f [
              // 2BD 11]\n",
              //           j, k, pair->value.d);
              break;
            case 4: /* SPLINE */
              l++;
              assert (l < (int)o->paths[j].segs[k].num_fitpts);
              o->paths[j].segs[k].fitpts[l].x = pair->value.d;
              // LOG_TRACE ("HATCH.paths[%d].segs[%d].fitpts[%d].x = %f [
              // 2RD 11]\n",
              //           j, k, l, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].type_status %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].type_status, pair->code);
            }
        }
      else if (pair->code == 20 && !is_plpath && !o->num_seeds)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].first_endpoint.y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].first_endpoint = (%f, %f) "
                         "[2RD 10]\n",
                         j, k, o->paths[j].segs[k].first_endpoint.x,
                         pair->value.d);
              break;
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].center.y = pair->value.d;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].center = (%f, %f) [2RD 10]\n", j,
                  k, o->paths[j].segs[k].center.x, pair->value.d);
              break;
            case 4: /* SPLINE */
              assert (l < (int)o->paths[j].segs[k].num_control_points);
              o->paths[j].segs[k].control_points[l].point.y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].control_points[%d].point = "
                         "(%f, %f) [2RD 10]\n",
                         j, k, l,
                         o->paths[j].segs[k].control_points[l].point.x,
                         pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].type_status %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].type_status, pair->code);
            }
        }
      else if (pair->code == 21 && !is_plpath && !o->num_seeds
               && pair->value.d != 0.0)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 1: /* LINE */
              o->paths[j].segs[k].second_endpoint.y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].second_endpoint = (%f, %f) "
                         "[2RD 11]\n",
                         j, k, o->paths[j].segs[k].second_endpoint.x,
                         pair->value.d);
              break;
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].endpoint.y = pair->value.d;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].endpoint = (%f, %f) [2RD 11]\n", j,
                  k, o->paths[j].segs[k].endpoint.x, pair->value.d);
              break;
            case 4: /* SPLINE */
              assert (l < (int)o->paths[j].segs[k].num_fitpts);
              o->paths[j].segs[k].fitpts[l].y = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].fitpts[%d].y = (%f, %f) "
                         "[2RD 11]\n",
                         j, k, l, o->paths[j].segs[k].fitpts[l].x,
                         pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].type_status %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].type_status, pair->code);
            }
        }
      else if (pair->code == 40 && !is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 2: /* CIRCULAR ARC */
              o->paths[j].segs[k].radius = pair->value.d;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].radius = %f [BD 40]\n", j,
                         k, pair->value.d);
              break;
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].minor_major_ratio = pair->value.d;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].minor_major_ratio = %f [BD 40]\n",
                  j, k, pair->value.d);
              break;
            case 4: /* SPLINE */
              if (l >= 0 && o->paths[j].segs[k].is_rational)
                {
                  assert (l < (int)o->paths[j].segs[k].num_control_points);
                  o->paths[j].segs[k].control_points[l].weight = pair->value.d;
                  LOG_TRACE ("HATCH.paths[%d].segs[%d].control_points[%d]."
                             "weight = %f [BD 40]\n",
                             j, k, l, pair->value.d);
                }
              else
                {
                  l++;
                  assert (l < (int)o->paths[j].segs[k].num_knots);
                  o->paths[j].segs[k].knots[l] = pair->value.d;
                  LOG_TRACE (
                      "HATCH.paths[%d].segs[%d].knots[%d] = %f [BD 40]\n", j,
                      k, l, pair->value.d);
                }
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].type_status %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].type_status, pair->code);
            }
        }
      else if (pair->code == 50 && !is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].start_angle = deg2rad (pair->value.d);
              LOG_TRACE ("HATCH.paths[%d].segs[%d].start_angle = %f [BD 50]\n",
                         j, k, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].type_status %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].type_status, pair->code);
            }
        }
      else if (pair->code == 51 && !is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].end_angle = deg2rad (pair->value.d);
              LOG_TRACE ("HATCH.paths[%d].segs[%d].end_angle = %f [BD 51]\n",
                         j, k, pair->value.d);
              break;
            default:
              LOG_WARN ("Unhandled HATCH.paths[%d].segs[%d].type_status %d "
                        "for DXF %d",
                        j, k, o->paths[j].segs[k].type_status, pair->code);
            }
        }
      else if (pair->code == 73 && !is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          switch (o->paths[j].segs[k].type_status)
            {
            case 2: /* CIRCULAR ARC */
            case 3: /* ELLIPTICAL ARC */
              o->paths[j].segs[k].is_ccw = pair->value.i;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].is_ccw = %d [B 73]\n", j, k,
                         pair->value.i);
              break;
            default:
              o->paths[j].segs[k].is_rational = pair->value.i;
              LOG_TRACE ("HATCH.paths[%d].segs[%d].is_rational = %d [B 73]\n",
                         j, k, pair->value.i);
            }
        }
      else if (pair->code == 10 && is_plpath && !o->num_seeds)
        {
          k++;
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].polyline_paths[k].point.x = pair->value.d;
          // LOG_TRACE ("HATCH.paths[%d].polyline_paths[%d].point.x = %f [BD 10
          // ]\n",
          //           j, k, pair->value.d);
        }
      else if (pair->code == 20 && is_plpath && !o->num_seeds)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].polyline_paths[k].point.y = pair->value.d;
          LOG_TRACE (
              "HATCH.paths[%d].polyline_paths[%d].point = (%f, %f) [2RD 10]\n",
              j, k, o->paths[j].polyline_paths[k].point.x, pair->value.d);
        }
      else if (pair->code == 42 && is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].polyline_paths[k].bulge = pair->value.d;
          LOG_TRACE ("HATCH.paths[%d].polyline_paths[%d].bulge = %f [BD 42]\n",
                     j, k, pair->value.d);
        }
      else if (pair->code == 97 && !is_plpath)
        {
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          if (k < 0 || o->paths[j].segs[k].type_status != 4)
            {
              o->paths[j].num_boundary_handles = pair->value.l;
              o->num_boundary_handles += pair->value.l;
              LOG_TRACE (
                  "HATCH.paths[%d].num_boundary_handles = %ld [BL 97]\n", j,
                  pair->value.l);
              k = 0;
            }
          else
            {
              o->paths[j].segs[k].num_fitpts = pair->value.l;
              LOG_TRACE (
                  "HATCH.paths[%d].segs[%d].num_fitpts  = %ld [BL 97]\n", j, k,
                  pair->value.l);
            }
        }
      else if (pair->code == 97 && is_plpath)
        {
          assert (j < (int)o->num_paths);
          o->paths[j].num_boundary_handles = pair->value.l;
          o->num_boundary_handles += pair->value.l;
          LOG_TRACE ("HATCH.paths[%d].num_boundary_handles = %ld [BL 97]\n", j,
                     pair->value.l);
          k = 0;
        }
      else if (pair->code == 78)
        {
          o->num_deflines = pair->value.l;
          LOG_TRACE ("HATCH.num_deflines = %ld [BS 78]\n", pair->value.l);
          o->deflines = calloc (pair->value.l, sizeof (Dwg_HATCH_DefLine));
          j = -1;
        }
      else if (pair->code == 53 && o->num_deflines)
        {
          j++;
          assert (j < (int)o->num_deflines);
          o->deflines[j].angle = deg2rad (pair->value.d);
          LOG_TRACE ("HATCH.deflines[%d].angle = %f [BD 53]\n", j,
                     o->deflines[j].angle);
        }
      else if (pair->code == 43 && o->num_deflines)
        {
          assert (j < (int)o->num_deflines);
          o->deflines[j].pt0.x = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].pt0.x = %f [BD 43]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 44 && o->num_deflines)
        {
          assert (j < (int)o->num_deflines);
          o->deflines[j].pt0.y = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].pt0.y = %f [BD 44]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 45 && o->num_deflines)
        {
          assert (j < (int)o->num_deflines);
          o->deflines[j].offset.x = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].offset.x = %f [BD 45]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 46 && o->num_deflines)
        {
          assert (j < (int)o->num_deflines);
          o->deflines[j].offset.y = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].offset.y = %f [BD 46]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 79 && o->num_deflines)
        {
          assert (j < (int)o->num_deflines);
          o->deflines[j].num_dashes = pair->value.u;
          LOG_TRACE ("HATCH.deflines[%d].num_dashes = %u [BS 79]\n", j,
                     pair->value.u);
          if (pair->value.u)
            o->deflines[j].dashes
                = calloc (pair->value.u, sizeof (BITCODE_BD));
          k = -1;
        }
      else if (pair->code == 49 && o->num_deflines && o->deflines[j].dashes)
        {
          assert (j < (int)o->num_deflines);
          k++;
          assert (k < (int)o->deflines[j].num_dashes);
          o->deflines[j].dashes[k] = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].dashes[%d] = %f [BD 49]\n", j, k,
                     pair->value.d);
        }
      else if (pair->code == 47)
        {
          o->pixel_size = pair->value.d;
          LOG_TRACE ("HATCH.pixel_size = %f [BD 47]\n", pair->value.d);
        }
      else if (pair->code == 98)
        {
          o->num_seeds = pair->value.u;
          LOG_TRACE ("HATCH.num_seeds = %u [BL 98]\n", pair->value.u);
          if (pair->value.u)
            o->seeds = calloc (pair->value.u, sizeof (BITCODE_2RD));
          k = -1;
        }
      else if (pair->code == 10 && o->num_seeds)
        {
          k++;
          assert (k < (int)o->num_seeds);
          o->seeds[k].x = pair->value.d;
          // LOG_TRACE ("HATCH.seeds[%d].x = %f [10 2RD]\n",
          //           k, pair->value.d);
        }
      else if (pair->code == 20 && o->num_seeds)
        {
          assert (k < (int)o->num_seeds);
          o->seeds[k].y = pair->value.d;
          LOG_TRACE ("HATCH.seeds[%d] = (%f, %f) [2RD 10]\n", k, o->seeds[k].x,
                     pair->value.d);
        }
      else if (pair->code == 330 && o->num_boundary_handles)
        {
          BITCODE_H ref
              = dwg_add_handleref (obj->parent, 3, pair->value.u, obj);
          // o->boundary_handles[k++] = ref;
          LOG_TRACE ("HATCH.boundary_handles[%d] = " FORMAT_REF " [H 330]\n",
                     k, ARGS_REF (ref));
        }
      else if (pair->code == 453)
        {
          o->num_colors = pair->value.u;
          LOG_TRACE ("HATCH.num_colors = %u [BL 453]\n", pair->value.u);
          if (pair->value.u)
            o->colors = calloc (pair->value.u, sizeof (Dwg_HATCH_Color));
          j = -1;
        }
      else if (pair->code == 463 && o->num_colors)
        {
          j++;
          assert (j < (int)o->num_colors);
          o->colors[j].shift_value = pair->value.d;
          LOG_TRACE ("HATCH.colors[%d].shift_value = %f [BD 463]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 63 && o->num_colors)
        {
          assert (j < (int)o->num_colors);
          o->colors[j].color.index = pair->value.i;
          LOG_TRACE ("HATCH.colors[%d].color.index = %u [CMC 63]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 421 && o->num_colors)
        {
          assert (j < (int)o->num_colors);
          o->colors[j].color.rgb = pair->value.u;
          LOG_TRACE ("HATCH.colors[%d].color.rgb = %06X [CMC 421]\n", j,
                     pair->value.u);
        }
      else if (pair->code == 431 && o->num_colors)
        {
          assert (j < (int)o->num_colors);
          o->colors[j].color.name = strdup (pair->value.s);
          LOG_TRACE ("HATCH.colors[%d].color.name = %s [CMC 431]\n", j,
                     pair->value.s);
        }
      else if (pair->code == 470)
        {
          dwg_dynapi_entity_set_value (o, "HATCH", "gradient_name",
                                       &pair->value, 1);
          // o->gradient_name = strdup (pair->value.s);
          LOG_TRACE ("HATCH.gradient_name = %s [T 470]\n", pair->value.s);
        }
      else if (pair->code == 462)
        {
          o->gradient_tint = pair->value.d;
          LOG_TRACE ("HATCH.gradient_tint = %f [BD 462]\n", pair->value.d);
        }
      else if (pair->code == 452)
        {
          o->single_color_gradient = pair->value.u;
          LOG_TRACE ("HATCH.single_color_gradient = %u [BL 452]\n",
                     pair->value.u);
        }
      else if (pair->code >= 1000 && pair->code < 1999)
        {
          add_eed (obj, "HATCH", pair);
        }
      else
        LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "HATCH");
    }
  return pair;
}

static Dxf_Pair *
add_MULTILEADER_lines (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                       Dxf_Pair *restrict pair, Dwg_LEADER_Node *lnode)
{
  Dwg_Entity_MULTILEADER *o = obj->tio.entity->tio.MULTILEADER;
  if (pair->code == 304 && strEQc (pair->value.s, "LEADER_LINE{"))
    {
      int i = -1, j = -1, k = -1;
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      lnode->lines = calloc (1, sizeof (Dwg_LEADER_Line));

      // lines and breaks
      while (pair->code != 305 && pair->code != 0)
        {
          Dwg_LEADER_Line *lline = &lnode->lines[0];
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          switch (pair->code)
            {
            case 10:
              i++;
              lnode->num_lines = i + 1;
              if (i > 0)
                lnode->lines = realloc (
                    lnode->lines, lnode->num_lines * sizeof (Dwg_LEADER_Line));
              lline = &lnode->lines[i];
              j++;
              lline->num_points = j + 1;
              lline->points
                  = realloc (lline->points, (j + 1) * sizeof (BITCODE_3BD));
              lline->points[j].x = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].points[%d].x = %f [BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 20:
              assert (j >= 0);
              lline->points[j].y = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].points[%d].y = %f [BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 30:
              assert (j >= 0);
              lline->points[j].z = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].points[%d].z = %f [BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 11:
              k++;
              lline->num_breaks = k + 1;
              lline->breaks = realloc (lline->breaks,
                                       (k + 1) * sizeof (Dwg_LEADER_Break));
              lline->breaks[k].start.x = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].start.x = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 21:
              assert (k >= 0);
              lline->breaks[k].start.y = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].start.y = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 31:
              assert (k >= 0);
              lline->breaks[k].start.z = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].start.z = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 12:
              assert (k >= 0);
              lline->breaks[k].end.x = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].end.x = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 22:
              assert (k >= 0);
              lline->breaks[k].end.y = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].end.y = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 32:
              assert (k >= 0);
              lline->breaks[k].end.z = pair->value.d;
              LOG_TRACE (
                  "%s.leaders[].lines[%d].breaks[%d].end.z = %f [3BD %d]\n",
                  obj->name, i, k, pair->value.d, pair->code);
              break;
            case 91:
              lline->line_index = pair->value.u;
              LOG_TRACE ("%s.leaders[].lines[%d].line_index = %u [BL %d]\n",
                         obj->name, i, pair->value.u, pair->code);
              break;
            case 170:
              lline->type = pair->value.i;
              LOG_TRACE ("%s.leaders[].lines[%d].line_index = %d [BS %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 92:
              if (pair->value.u > 256)
                {
                  lline->color.index = 256;
                  lline->color.rgb = pair->value.u & 0xFFFFFF;
                  lline->color.alpha = pair->value.u & 0xFF000000;
                  LOG_TRACE (
                      "%s.leaders[].lines[%d].color.rgb = %06X [CMC %d]\n",
                      obj->name, i, pair->value.u, pair->code);
                }
              else
                {
                  lline->color.index = pair->value.i;
                  LOG_TRACE (
                      "%s.leaders[].lines[%d].color.index = %d [CMC %d]\n",
                      obj->name, i, pair->value.i, pair->code);
                }
              break;
            case 171:
              lline->linewt = pair->value.i;
              LOG_TRACE ("%s.leaders[].lines[%d].linewt = %d [BL %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 40:
              lline->arrow_size = pair->value.d;
              LOG_TRACE ("%s.leaders[].lines[%d].arrow_size = %f [BD %d]\n",
                         obj->name, i, pair->value.d, pair->code);
              break;
            case 93:
              lline->flags = pair->value.i;
              LOG_TRACE ("%s.leaders[].lines[%d].line_index = %d [BL %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 305: // end
              break;
            default:
              LOG_ERROR (
                  "Unknown DXF code %d for MULTILEADER.leaders[].lines[%d]",
                  pair->code, i);
            }
        }
    }
  if (!lnode->num_lines)
    {
      free (lnode->lines);
      lnode->lines = NULL;
    }
  return pair;
}

static Dxf_Pair *
add_MULTILEADER_leaders (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                         Dxf_Pair *restrict pair)
{
  Dwg_Entity_MULTILEADER *o = obj->tio.entity->tio.MULTILEADER;
  if (pair->code == 302 && strEQc (pair->value.s, "LEADER{"))
    {
      int i = -1, j = -1;
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      while (pair->code != 303 && pair->code != 0)
        {
          Dwg_LEADER_Node *lnode = i >= 0 ? &ctx->leaders[i] : NULL;
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);

          if (!lnode && pair->code != 290 && pair->code != 304
              && pair->code != 303)
            {
              LOG_ERROR ("Missing MULTILEADER.LEADER{ 290 start");
              return pair;
            }

          switch (pair->code)
            {
            case 290:
              i++;
              ctx->num_leaders = i + 1;
              ctx->leaders
                  = realloc (ctx->leaders, (i + 1) * sizeof (Dwg_LEADER_Node));
              ctx->leaders[i].has_lastleaderlinepoint = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].has_lastleaderlinepoint = %d [B %d]\n",
                  obj->name, i, pair->value.i, pair->code);
              break;
            case 291:
              lnode->has_dogleg = pair->value.i;
              LOG_TRACE ("%s.ctx.leaders[%d].has_dogleg = %d [B %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 90:
              lnode->branch_index = pair->value.u;
              LOG_TRACE ("%s.ctx.leaders[%d].branch_index = %u [BL %d]\n",
                         obj->name, i, pair->value.u, pair->code);
              break;
            case 271:
              lnode->attach_dir = pair->value.i;
              LOG_TRACE ("%s.ctx.leaders[%d].branch_index = %d [BS %d]\n",
                         obj->name, i, pair->value.i, pair->code);
              break;
            case 40:
              lnode->dogleg_length = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].dogleg_length = %f [BD %d]\n",
                         obj->name, i, pair->value.d, pair->code);
              break;
            case 10:
              lnode->lastleaderlinepoint.x = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].lastleaderlinepoint.x = %f [3BD %d]\n",
                  obj->name, i, pair->value.d, pair->code);
              break;
            case 20:
              lnode->lastleaderlinepoint.y = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].lastleaderlinepoint.y = %f [3BD %d]\n",
                  obj->name, i, pair->value.d, pair->code);
              break;
            case 30:
              lnode->lastleaderlinepoint.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.leaders[%d].lastleaderlinepoint.z = %f [BD %d]\n",
                  obj->name, i, pair->value.d, pair->code);
              break;
            case 11:
              if (lnode->has_dogleg)
                {
                  lnode->dogleg_vector.x = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].dogleg_vector.x = %f [3BD %d]\n",
                      obj->name, i, pair->value.d, pair->code);
                }
              else
                {
                  j++;
                  lnode->num_breaks = j + 1;
                  lnode->breaks = realloc (
                      lnode->breaks, (j + 1) * sizeof (Dwg_LEADER_Break));
                  lnode->breaks[j].start.x = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].breaks[%d].start.x = %f [3BD %d]\n",
                      obj->name, i, j, pair->value.d, pair->code);
                }
              break;
            case 21:
              if (lnode->has_dogleg)
                {
                  lnode->dogleg_vector.y = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].dogleg_vector.y = %f [3BD %d]\n",
                      obj->name, i, pair->value.d, pair->code);
                }
              else
                {
                  lnode->breaks[j].start.y = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].breaks[%d].start.y = %f [3BD %d]\n",
                      obj->name, i, j, pair->value.d, pair->code);
                }
              break;
            case 31:
              if (lnode->has_dogleg)
                {
                  lnode->dogleg_vector.z = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].dogleg_vector.z = %f [3BD %d]\n",
                      obj->name, i, pair->value.d, pair->code);
                }
              else
                {
                  lnode->breaks[j].start.z = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].breaks[%d].start.z = %f [3BD %d]\n",
                      obj->name, i, j, pair->value.d, pair->code);
                }
              break;
            case 12:
              lnode->breaks[j].end.x = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.x = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 22:
              lnode->breaks[j].end.y = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.y = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 32:
              lnode->breaks[j].end.z = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.z = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 304:
              if (strEQc (pair->value.s, "LEADER_LINE{"))
                pair = add_MULTILEADER_lines (obj, dat, pair, lnode);
              break;
            case 303: // end
              break;
            default:
              LOG_ERROR ("Unknown DXF code %d for MULTILEADER.leaders[]",
                         pair->code);
            }
        }
    }
  return pair;
}

static Dxf_Pair *
add_MULTILEADER (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                 Dxf_Pair *restrict pair)
{
  Dwg_Entity_MULTILEADER *o = obj->tio.entity->tio.MULTILEADER;
  int i = -1, j = -1;

  if (pair->code == 300 && strEQc (pair->value.s, "CONTEXT_DATA{"))
    {
      // const Dwg_DYNAPI_field *fields
      // = dwg_dynapi_subclass_fields ("MLEADER_AnnotContext");
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      while (pair->code != 301 && pair->code != 0)
        {
          switch (pair->code)
            {
            case 300:
              break;
            case 40:
              ctx->scale = pair->value.d;
              LOG_TRACE ("%s.ctx.scale = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 10:
              ctx->content_base.x = pair->value.d;
              break;
            case 20:
              ctx->content_base.y = pair->value.d;
              break;
            case 30:
              ctx->content_base.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content_base = (%f, %f, %f) [10 3BD]\n",
                         obj->name, ctx->content_base.x, ctx->content_base.y,
                         ctx->content_base.z);
              break;
            case 41:
              ctx->text_height = pair->value.d;
              LOG_TRACE ("%s.ctx.text_height = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 140:
              ctx->arrow_size = pair->value.d;
              LOG_TRACE ("%s.ctx.arrow_size = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 145:
              ctx->landing_gap = pair->value.d;
              LOG_TRACE ("%s.ctx.landing_gap = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 174:
              ctx->text_left = pair->value.i;
              LOG_TRACE ("%s.ctx.text_left = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 175:
              ctx->text_right = pair->value.i;
              LOG_TRACE ("%s.ctx.text_right = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 176:
              ctx->text_alignment = pair->value.i;
              LOG_TRACE ("%s.ctx.text_alignment = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 177:
              ctx->attach_type = pair->value.i;
              LOG_TRACE ("%s.ctx.attach_type = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 290:
              ctx->has_content_txt = pair->value.i;
              LOG_TRACE ("%s.ctx.has_content_txt = %d [B %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 302:
              if (strEQc (pair->value.s, "LEADER{"))
                pair = add_MULTILEADER_leaders (obj, dat, pair);
              break;
            case 304:
              if (ctx->has_content_txt)
                {
                  if (dat->version >= R_2007)
                    ctx->content.txt.default_text
                        = (char *)bit_utf8_to_TU (pair->value.s);
                  else
                    ctx->content.txt.default_text = strdup (pair->value.s);
                  LOG_TRACE ("%s.ctx.content.txt.default_text = %s [%d T]\n",
                             obj->name, pair->value.s, pair->code);
                }
              break;
            case 340:
              if (ctx->has_content_txt)
                {
                  ctx->content.txt.style
                      = dwg_add_handleref (obj->parent, 5, pair->value.u, obj);
                  LOG_TRACE ("%s.ctx.content.txt.style = " FORMAT_REF
                             " [%d H]\n",
                             obj->name, ARGS_REF (ctx->content.txt.style),
                             pair->code);
                }
              break;
            case 11:
              ctx->content.txt.normal.x = pair->value.d;
              break;
              break;
            case 21:
              ctx->content.txt.normal.y = pair->value.d;
              break;
            case 31:
              ctx->content.txt.normal.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.normal = (%f, %f, %f) [11 3BD]\n",
                         obj->name, ctx->content.txt.normal.x,
                         ctx->content.txt.normal.y, ctx->content.txt.normal.z);
              break;
            case 12:
              ctx->content.txt.location.x = pair->value.d;
              break;
              break;
            case 22:
              ctx->content.txt.location.y = pair->value.d;
              break;
            case 32:
              ctx->content.txt.location.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.location = (%f, %f, %f) [12 3BD]\n",
                  obj->name, ctx->content.txt.location.x,
                  ctx->content.txt.location.y, ctx->content.txt.location.z);
              break;
            case 13:
              ctx->content.txt.direction.x = pair->value.d;
              break;
              break;
            case 23:
              ctx->content.txt.direction.y = pair->value.d;
              break;
            case 33:
              ctx->content.txt.direction.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.direction = (%f, %f, %f) [13 3BD]\n",
                  obj->name, ctx->content.txt.direction.x,
                  ctx->content.txt.direction.y, ctx->content.txt.direction.z);
              break;
            case 42:
              ctx->content.txt.rotation = deg2rad (pair->value.d);
              LOG_TRACE ("%s.ctx.content.txt.rotation = %f [BD %d]\n",
                         obj->name, ctx->content.txt.rotation, pair->code);
              break;
            case 43:
              ctx->content.txt.width = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.width = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 44:
              ctx->content.txt.height = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.height = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 45:
              ctx->content.txt.line_spacing_factor = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.line_spacing_factor = %f [BD %d]\n",
                  obj->name, pair->value.d, pair->code);
              break;
            case 170:
              ctx->content.txt.line_spacing_style = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.content.txt.line_spacing_style = %d [BS %d]\n",
                  obj->name, pair->value.i, pair->code);
              break;
            case 171:
              ctx->content.txt.alignment = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.alignment = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 172:
              ctx->content.txt.flow = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.flow = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 90:
              ctx->content.txt.color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.color.index = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 91:
              ctx->content.txt.bg_color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.bg_color.index = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 141:
              ctx->content.txt.bg_scale = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.bg_scale = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 142:
              ctx->content.txt.col_width = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_width = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 143:
              ctx->content.txt.col_gutter = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_gutter = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 92:
              ctx->content.txt.bg_transparency = pair->value.u;
              LOG_TRACE ("%s.ctx.content.txt.bg_transparency = %u [BL %d]\n",
                         obj->name, pair->value.u, pair->code);
              break;
            case 291:
              ctx->content.txt.is_bg_fill = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_bg_fill = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 292:
              ctx->content.txt.is_bg_mask_fill = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_bg_mask_fill = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 293:
              ctx->content.txt.is_height_auto = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_height_auto = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 294:
              ctx->content.txt.is_col_flow_reversed = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.content.txt.is_col_flow_reversed = %i [B %d]\n",
                  obj->name, pair->value.i, pair->code);
              break;
            case 295:
              ctx->content.txt.word_break = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.word_break = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 173:
              ctx->content.txt.col_type = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.col_type = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 144:
              i++;
              ctx->content.txt.num_col_sizes = i + 1;
              ctx->content.txt.col_sizes = realloc (ctx->content.txt.col_sizes,
                                                    (i + 1) * sizeof (double));
              ctx->content.txt.col_sizes[i] = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_sizes[%d] = %f [BD %d]\n",
                         obj->name, i, pair->value.d, pair->code);
              break;
            case 296:
              ctx->has_content_blk = pair->value.i;
              LOG_TRACE ("%s.ctx.has_content_blk = %i [B %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 14: // has_block
              ctx->content.blk.normal.x = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.x = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 24:
              ctx->content.blk.normal.y = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.y = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 34:
              ctx->content.blk.normal.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.z = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 341:
              ctx->content.blk.block_table
                  = dwg_add_handleref (obj->parent, 4, pair->value.u, obj);
              LOG_TRACE ("%s.ctx.content.blk.block_table = " FORMAT_REF
                         " [%d H]\n",
                         obj->name, ARGS_REF (ctx->content.blk.block_table),
                         pair->code);
              break;
            case 15: // has_block
              ctx->content.blk.location.x = pair->value.d;
              break;
            case 25:
              ctx->content.blk.location.y = pair->value.d;
              break;
            case 35:
              ctx->content.blk.location.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.blk.location = (%f, %f, %f) [3BD %d]\n",
                  obj->name, ctx->content.blk.location.x,
                  ctx->content.blk.location.y, ctx->content.blk.location.z,
                  pair->code);
              break;
            case 16: // has_block
              ctx->content.blk.scale.x = pair->value.d;
              break;
            case 26:
              ctx->content.blk.scale.y = pair->value.d;
              break;
            case 36:
              ctx->content.blk.scale.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.scale = (%f, %f, %f) [3BD %d]\n",
                         obj->name, ctx->content.blk.scale.x,
                         ctx->content.blk.scale.y, ctx->content.blk.scale.z,
                         pair->code);
              break;
            case 46:
              ctx->content.blk.rotation = pair->value.d; // deg2rad?
              LOG_TRACE ("%s.ctx.content.blk.rotation = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 93:
              ctx->content.blk.color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.blk.color.index = %d [CMC %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 47:
              j++;
              if (!j)
                ctx->content.blk.transform = calloc (16, sizeof (double));
              ctx->content.blk.transform[j] = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.transform[%d] = %f [BD %d]\n",
                         obj->name, j, pair->value.d, pair->code);
              break;
            case 110:
              ctx->base.x = pair->value.d;
              break;
            case 120:
              ctx->base.y = pair->value.d;
              break;
            case 130:
              ctx->base.z = pair->value.d;
              LOG_TRACE ("%s.ctx.base = (%f, %f, %f) [3BD %d]\n", obj->name,
                         ctx->base.x, ctx->base.y, ctx->base.z, pair->code);
              break;
            case 111:
              ctx->base_dir.x = pair->value.d;
              break;
            case 121:
              ctx->base_dir.y = pair->value.d;
              break;
            case 131:
              ctx->base_dir.z = pair->value.d;
              LOG_TRACE ("%s.ctx.base = (%f, %f, %f) [3BD %d]\n", obj->name,
                         ctx->base_dir.x, ctx->base_dir.y, ctx->base_dir.z,
                         pair->code);
              break;
            case 112:
              ctx->base_vert.x = pair->value.d;
              break;
            case 122:
              ctx->base_vert.y = pair->value.d;
              break;
            case 132:
              ctx->base_vert.z = pair->value.d;
              LOG_TRACE ("%s.ctx.base = (%f, %f, %f) [3BD %d]\n", obj->name,
                         ctx->base_vert.x, ctx->base_vert.y, ctx->base_vert.z,
                         pair->code);
              break;
            case 297:
              ctx->is_normal_reversed = pair->value.i;
              LOG_TRACE ("%s.ctx.is_normal_reversed = %i [B %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 273:
              ctx->text_top = pair->value.i;
              LOG_TRACE ("%s.ctx.text_top = %i [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 272:
              ctx->text_bottom = pair->value.i;
              LOG_TRACE ("%s.ctx.text_bottom = %i [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;

            case 301: // end ctx
              return pair;
            default:
              LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                         "MULTILEADER");
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
    }
  return pair;
}

// with ASSOC2DCONSTRAINTGROUP, ASSOCNETWORK, ASSOCACTION
static Dxf_Pair *
add_ASSOCACTION (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                 Dxf_Pair *restrict pair)
{
  Dwg_Object_ASSOCACTION *o = obj->tio.object->tio.ASSOCACTION;
  Dwg_Data *dwg = obj->parent;

#define EXPECT_INT_DXF(field, dxf, type)                                      \
  pair = dxf_read_pair (dat);                                                 \
  if (pair->code != dxf)                                                      \
    {                                                                         \
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s", obj->name, \
                 pair->code, dxf, field);                                     \
      return pair;                                                            \
    }                                                                         \
  dwg_dynapi_entity_set_value (o, obj->name, field, &pair->value, 1);         \
  LOG_TRACE ("%s.%s = %d [" #type " %d]\n", obj->name, field, pair->value.i,  \
             pair->code);                                                     \
  dxf_free_pair (pair)

#define EXPECT_H_DXF(field, htype, dxf, type)                                 \
  pair = dxf_read_pair (dat);                                                 \
  if (pair->code != dxf)                                                      \
    {                                                                         \
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s", obj->name, \
                 pair->code, dxf, field);                                     \
      return pair;                                                            \
    }                                                                         \
  if (pair->value.u)                                                          \
    {                                                                         \
      BITCODE_H hdl = dwg_add_handleref (dwg, htype, pair->value.u, obj);     \
      dwg_dynapi_entity_set_value (o, obj->name, field, &hdl, 1);             \
      LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n", obj->name, field,         \
                 ARGS_REF (hdl), pair->code);                                 \
    }                                                                         \
  dxf_free_pair (pair)

  EXPECT_INT_DXF ("solution_status", 90, BL);
  EXPECT_INT_DXF ("geometry_status", 90, BL);
  EXPECT_H_DXF ("readdep", 5, 330, H); // or vector?
  EXPECT_H_DXF ("writedep", 5, 360, H);
  EXPECT_INT_DXF ("constraint_status", 90, BL);
  EXPECT_INT_DXF ("dof", 90, BL);
  EXPECT_INT_DXF ("is_body_a_proxy", 90, B);

  return NULL;
}

Dwg_Object *
find_prev_entity (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  if (obj->supertype != DWG_SUPERTYPE_ENTITY)
    return NULL;
  for (BITCODE_BL i = obj->index - 1; i > 0; i--)
    {
      Dwg_Object *prev = &dwg->object[i];
      if (prev->supertype == DWG_SUPERTYPE_ENTITY
          && prev->tio.entity->entmode == obj->tio.entity->entmode)
        return prev;
    }
  return NULL;
}

static Dxf_Pair *
new_table_control (const char *restrict name, Bit_Chain *restrict dat,
                   Dwg_Data *restrict dwg)
{
  // VPORT_CONTROL.num_entries
  // VPORT_CONTROL.entries[num_entries] handles
  Dwg_Object *obj;
  Dxf_Pair *pair = NULL;
  Dwg_Object_APPID_CONTROL *_obj = NULL;
  int j = 0;
  int is_utf = dwg->header.version >= R_2007 ? 1 : 0;
  char *fieldname;
  char ctrlname[80];
  char dxfname[80];
  BITCODE_B xrefref;

  NEW_OBJECT (dwg, obj);

  if (strEQc (name, "BLOCK_RECORD"))
    strcpy (ctrlname, "BLOCK_CONTROL");
  else
    {
      strncpy (ctrlname, name, 70);
      strcat (ctrlname, "_CONTROL");
    }
  LOG_TRACE ("add %s\n", ctrlname);
  strcpy (dxfname, ctrlname);

  // clang-format off
  ADD_TABLE_IF (LTYPE, LTYPE_CONTROL)
  else
  ADD_TABLE_IF (VPORT, VPORT_CONTROL)
  else
  ADD_TABLE_IF (APPID, APPID_CONTROL)
  else
  ADD_TABLE_IF (BLOCK_RECORD, BLOCK_CONTROL)
  else
  ADD_TABLE_IF (DIMSTYLE, DIMSTYLE_CONTROL)
  else
  ADD_TABLE_IF (LAYER, LAYER_CONTROL)
  else
  ADD_TABLE_IF (STYLE, STYLE_CONTROL)
  else
  ADD_TABLE_IF (UCS, UCS_CONTROL)
  else
  ADD_TABLE_IF (VIEW, VIEW_CONTROL)
  else
  ADD_TABLE_IF (VPORT_ENTITY, VPORT_ENTITY_CONTROL)
  else
  ADD_TABLE_IF (BLOCK_RECORD, BLOCK_CONTROL)
  else
  // clang-format on
  {
    LOG_ERROR ("Unknown DXF TABLE %s nor %s_CONTROL", name, name);
    return pair;
  }
  if (!_obj)
    {
      LOG_ERROR ("Empty _obj at DXF TABLE %s nor %s_CONTROL", name, name);
      return pair;
    }
  dwg_dynapi_entity_set_value (_obj, obj->name, "objid", &obj->index, is_utf);
  xrefref = 1;
  if (dwg_dynapi_entity_field (obj->name, "xrefref"))
    dwg_dynapi_entity_set_value (_obj, obj->name, "xrefref", &xrefref, is_utf);
  // default xdic_missing_flag
  if (dwg->header.version >= R_2004)
    obj->tio.object->xdic_missing_flag = 1;

  pair = dxf_read_pair (dat);
  // read common table until next 0 table or endtab
  while (pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          goto do_return;
        case 5:
        case 105: // for DIMSTYLE
          {
            Dwg_Object_Ref *ref;
            char ctrlobj[80];
            dwg_add_handle (&obj->handle, 0, pair->value.u, obj);
            ref = dwg_add_handleref (dwg, 3, pair->value.u, obj);
            LOG_TRACE ("%s.handle = " FORMAT_H " [H %d]\n", ctrlname,
                       ARGS_H (obj->handle), pair->code);
            // also set the matching HEADER.*_CONTROL_OBJECT
            strncpy (ctrlobj, ctrlname, 70);
            strcat (ctrlobj, "_OBJECT");
            dwg_dynapi_header_set_value (dwg, ctrlobj, &ref, 0);
            LOG_TRACE ("HEADER.%s = " FORMAT_REF " [H 0]\n", ctrlobj,
                       ARGS_REF (ref));
          }
          break;
        case 100: // AcDbSymbolTableRecord, ... ignore
          break;
        case 102: // TODO {ACAD_XDICTIONARY {ACAD_REACTORS {BLKREFS
          break;
        case 330: // TODO: most likely {ACAD_REACTORS
            {
              BITCODE_H owh = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              obj->tio.object->ownerhandle = owh;
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [H 330]\n", ctrlname,
                         ARGS_REF (owh));
            }
          break;
        case 340:
          if (pair->value.u && strEQc (ctrlname, "DIMSTYLE_CONTROL"))
            {
              Dwg_Object_DIMSTYLE_CONTROL *_o
                  = (Dwg_Object_DIMSTYLE_CONTROL *)_obj;
              if (!_o->num_morehandles)
                {
                  LOG_ERROR ("Empty DIMSTYLE_CONTROL.num_morehandles")
                  break;
                }
              assert (_o->morehandles);
              _o->morehandles[j]
                  = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.morehandles[%d] = " FORMAT_REF " [H 330]\n",
                         ctrlname, j, ARGS_REF (_o->morehandles[j]));
              j++;
            }
          break;
        case 360: // {ACAD_XDICTIONARY TODO
          obj->tio.object->xdicobjhandle
              = dwg_add_handleref (dwg, 3, pair->value.u, obj);
          obj->tio.object->xdic_missing_flag = 0;
          LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [H 360]\n", ctrlname,
                     ARGS_REF (obj->tio.object->xdicobjhandle));
          break;
        case 70:
          if (pair->value.u)
            {
              BITCODE_H *hdls;
              BITCODE_BL num_entries = (BITCODE_BL)pair->value.u;
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_entries",
                                           &num_entries, is_utf);
              LOG_TRACE ("%s.num_entries = %u [BL 70]\n", ctrlname, num_entries);
              hdls = calloc (num_entries, sizeof (Dwg_Object_Ref *));
              dwg_dynapi_entity_set_value (_obj, obj->name, "entries", &hdls,
                                           0);
              LOG_TRACE ("Add %s.%s[%d]\n", ctrlname, "entries", num_entries);
            }
          break;
        case 71:
          if (strEQc (ctrlname, "DIMSTYLE_CONTROL"))
            {
              if (pair->value.u)
                {
                  BITCODE_H *hdls;
                  dwg_dynapi_entity_set_value (_obj, obj->name,
                                               "num_morehandles", &pair->value,
                                               is_utf);
                  LOG_TRACE ("%s.num_morehandles = %u [BL 71]\n", ctrlname,
                             pair->value.u);
                  hdls = calloc (pair->value.u, sizeof (Dwg_Object_Ref *));
                  dwg_dynapi_entity_set_value (_obj, obj->name, "morehandles",
                                               &hdls, 0);
                  LOG_TRACE ("Add %s.morehandles[%d]\n", ctrlname,
                             pair->value.u);
                }
              else
                {
                  LOG_TRACE ("%s.num_morehandles = %u [BL 71]\n", ctrlname,
                             pair->value.u)
                }
              break;
            }
          // fall through
        default:
          if (pair->code >= 1000 && pair->code < 1999)
            {
              add_eed (obj, obj->name, pair);
            }
          else
            LOG_ERROR ("Unknown DXF code %d for %s", pair->code, ctrlname);
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
do_return:
  // default NULL handle
  if (!obj->tio.object->xdicobjhandle)
    {
      obj->tio.object->xdic_missing_flag = 1;
      if (dwg->header.version >= R_13 && dwg->header.version < R_2004)
        obj->tio.object->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
    }
  return pair;
}

/* by name or by ref.
   Note that we don't get the ref->obj here, as it may still move
   by realloc dwg->object[].
*/
BITCODE_H
find_tablehandle (Dwg_Data *restrict dwg, Dxf_Pair *restrict pair)
{
  BITCODE_H ref = NULL;
  if (pair->code == 8)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "LAYER");
  else if (pair->code == 1) // $DIMBLK
    ref = dwg_find_tablehandle (dwg, pair->value.s, "BLOCK");
  else if (pair->code
           == 2) // some name: $DIMSTYLE, $UCSBASE, $UCSORTHOREF, $CMLSTYLE
    ;            // not enough info, decide later
  else if (pair->code == 3)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "DIMSTYLE");
  // what is/was 4 and 5? VIEW? VPORT_ENTITY?
  else if (pair->code == 6)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "LTYPE");
  else if (pair->code == 7)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "STYLE");

  if (ref) // turn a 2 (hardowner) into a 5 (softref)
    return dwg_add_handleref (dwg, 5, ref->absolute_ref, NULL);
  /* I think all these >300 are given by hex value, not by name */
  if (!ref && pair->code > 300)
    {
      for (BITCODE_BL i = 0; i < dwg->num_object_refs; i++)
        {
          Dwg_Object_Ref *refi = dwg->object_ref[i];
          if (refi->absolute_ref == (BITCODE_BL)pair->value.u)
            {
              // no relative offset
              ref = dwg_add_handleref (dwg, 5, pair->value.u, NULL);
              break;
            }
        }
      if (!ref)
        {
          // no relative offset
          ref = dwg_add_handleref (dwg, 5, pair->value.u, NULL);
        }
    }
#if 0
  else if (pair->code == 331)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "VPORT");
  else if (pair->code == 390)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "PLOTSTYLENAME");
  else if (pair->code == 347)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "MATERIAL");
  else if (pair->code == 345 || pair->code == 346)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "UCS");
  else if (pair->code == 361) // SUN
    ref = dwg_find_tablehandle (dwg, pair->value.s, "SHADOW");
  else if (pair->code == 340) // or TABLESTYLE or LAYOUT ...
    ref = dwg_find_tablehandle (dwg, pair->value.s, "STYLE");
  else if (pair->code == 342 || pair->code == 343)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "STYLE");
  else if (pair->code == 348)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "VISUALSTYLE");
  else if (pair->code == 332)
    ref = dwg_find_tablehandle (dwg, pair->value.s, "BACKGROUND");
#endif
  return ref;
}

// add pair to XRECORD
Dxf_Pair *
add_xdata (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
           Dxf_Pair *restrict pair)
{
  BITCODE_BL num_xdata, num_databytes;
  // add pairs to xdata linked list
  Dwg_Resbuf *rbuf;
  Dwg_Object_XRECORD *_obj = obj->tio.object->tio.XRECORD;

  num_xdata = _obj->num_xdata;
  num_databytes = _obj->num_databytes;
  rbuf = calloc (1, sizeof (Dwg_Resbuf));
  if (!rbuf)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  if (num_xdata && _obj->xdata)
    {
      Dwg_Resbuf *xdata, *prev;
      // add to end, not front
      prev = xdata = _obj->xdata;
      while (xdata)
        {
          prev = xdata;
          xdata = xdata->next;
        }
      prev->next = rbuf;
      xdata = _obj->xdata;
    }
  else
    _obj->xdata = rbuf;

  num_databytes += 2; // RS
  rbuf->type = pair->code;
  switch (get_base_value_type (rbuf->type))
    {
    case VT_STRING:
      PRE (R_2007)
      {
        Dwg_Data *dwg = obj->parent;
        rbuf->value.str.size = strlen (pair->value.s);
        rbuf->value.str.codepage = dwg->header.codepage;
        rbuf->value.str.u.data = strdup (pair->value.s);
        LOG_TRACE ("xdata[%d]: \"%s\" [%d]\n", num_xdata,
                   rbuf->value.str.u.data, rbuf->type);
        num_databytes += 3 + rbuf->value.str.size;
      }
      LATER_VERSIONS
      {
        int length = rbuf->value.str.size = strlen (pair->value.s);
        if (length > 0)
          {
            rbuf->value.str.u.wdata = bit_utf8_to_TU (pair->value.s);
          }
        num_databytes += 2 + 2 * rbuf->value.str.size;
      }
      break;
    case VT_REAL:
      rbuf->value.dbl = pair->value.d;
      LOG_TRACE ("xdata[%d]: %f [%d]\n", num_xdata, rbuf->value.dbl,
                 rbuf->type);
      num_databytes += 8;
      break;
    case VT_BOOL:
    case VT_INT8:
      rbuf->value.i8 = pair->value.i;
      LOG_TRACE ("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i8,
                 rbuf->type);
      num_databytes += 1;
      break;
    case VT_INT16:
      rbuf->value.i16 = pair->value.i;
      LOG_TRACE ("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i16,
                 rbuf->type);
      num_databytes += 2;
      break;
    case VT_INT32:
      rbuf->value.i32 = pair->value.l;
      LOG_TRACE ("xdata[%d]: %ld [%d]\n", num_xdata, (long)rbuf->value.i32,
                 rbuf->type);
      num_databytes += 4;
      break;
    case VT_INT64:
      rbuf->value.i64 = (BITCODE_BLL)pair->value.bll;
      LOG_TRACE ("xdata[%d]: " FORMAT_BLL " [%d]\n", num_xdata,
                 rbuf->value.i64, rbuf->type);
      num_databytes += 8;
      break;
    case VT_POINT3D:
      rbuf->value.pt[0] = pair->value.d;
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      rbuf->value.pt[1] = pair->value.d;
      dxf_free_pair (pair);
      num_databytes += 24;
      { // if 30
        long pos = bit_position (dat);
        pair = dxf_read_pair (dat);
        if (get_base_value_type (pair->code) == VT_POINT3D)
          {
            rbuf->value.pt[2] = pair->value.d;
            LOG_TRACE ("xdata[%d]: (%f,%f,%f) [%d]\n", num_xdata,
                       rbuf->value.pt[0], rbuf->value.pt[1], rbuf->value.pt[2],
                       rbuf->type);
          }
        else
          {
            bit_set_position (dat, pos); // reset stream
            rbuf->value.pt[2] = 0;
            LOG_TRACE ("xdata[%d]: (%f,%f) [%d]\n", num_xdata,
                       rbuf->value.pt[0], rbuf->value.pt[1], rbuf->type);
          }
      }
      break;
    case VT_BINARY:
      // convert from hex
      {
        int i;
        int len = strlen (pair->value.s);
        int blen = len / 2;
        char *s = malloc (blen);
        const char *pos = pair->value.s;
        rbuf->value.str.u.data = s;
        rbuf->value.str.size = blen;
        for (i = 0; i < blen; i++)
          {
            sscanf (pos, "%2hhX", &s[i]);
            pos += 2;
          }
        num_databytes += 1 + len;
        LOG_TRACE ("xdata[%d]: ", num_xdata);
        // LOG_TRACE_TF (rbuf->value.str.u.data, rbuf->value.str.size);
      }
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      num_databytes += 8;
      dwg_add_handle (&rbuf->value.h, 0, pair->value.u, obj);
      LOG_TRACE ("xdata[%d]: " FORMAT_H " [H %d]\n", num_xdata,
                 ARGS_H (rbuf->value.h), rbuf->type);
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid group code in rbuf: %d", rbuf->type)
    }

  num_xdata++;
  _obj->num_xdata = num_xdata;
  _obj->num_databytes = num_databytes;
  return pair;
}

// 350 or 360
void
add_dictionary_itemhandles (Dwg_Object *restrict obj, Dxf_Pair *restrict pair,
                            char *restrict text)
{
  // but not DICTIONARYVAR
  Dwg_Object_DICTIONARY *_obj
      = obj->tio.object->tio.DICTIONARY; // also DICTIONARYWDFLT
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL num;
  BITCODE_H hdl;

  if (pair->code == 360)
    _obj->hard_owner = 1;
  num = _obj->numitems;
  hdl = dwg_add_handleref (dwg, 2, pair->value.u, obj);
  LOG_TRACE ("%s.itemhandles[%d] = " FORMAT_REF " [H* %d]\n", obj->name, num,
             ARGS_REF (hdl), pair->code);
  _obj->itemhandles
      = realloc (_obj->itemhandles, (num + 1) * sizeof (BITCODE_H));
  _obj->itemhandles[num] = hdl;
  _obj->texts = realloc (_obj->texts, (num + 1) * sizeof (BITCODE_TV));
  if (dwg->header.version >= R_2007)
    _obj->texts[num] = (char *)bit_utf8_to_TU (text);
  else
    _obj->texts[num] = strdup (text);
  LOG_TRACE ("%s.texts[%d] = %s [T* 3]\n", obj->name, num, text);
  _obj->numitems = num + 1;
}

/* convert to flag */
static BITCODE_RC
dxf_find_lweight (const int lw)
{
  // See acdb.h: 100th of a mm, enum of
  const int lweights[] = { 0,
                           5,
                           9,
                           13,
                           15,
                           18,
                           20,
                           25,
                           30,
                           35,
                           40,
                           50,
                           53,
                           60,
                           70,
                           80,
                           90,
                           100,
                           106,
                           120,
                           140,
                           158,
                           200,
                           211,
                           /*illegal/reserved:*/ 0,
                           0,
                           0,
                           0,
                           0,
                           /*29:*/ -1, // BYLAYER
                           -2,         // BYBLOCK
                           -3 };       // BYLWDEFAULT
  for (int i = 0; i < 32; i++)
    {
      if (lweights[i] == lw)
        return i;
    }
  return 0;
}

/* read to ent->preview, starting with code 160 for bitmap previews,
   Or with code 92 for PROXY GRAPHICS vector data for newer variable
   entities, like WIPEOUT, LIGHT, MULTILEADER, ARC_DIMENSION, ...
*/
static Dxf_Pair *
add_ent_preview (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                 Dxf_Pair *restrict pair)
{
  Dwg_Object_Entity *ent = obj->tio.entity;
  unsigned written = 0;

  if (obj->supertype != DWG_SUPERTYPE_ENTITY)
    {
      LOG_ERROR ("%s is no entity for a preview", obj->name);
      return pair;
    }
  ent->preview_size = pair->code == 160 ? pair->value.bll : pair->value.u;
  if (!ent->preview_size)
    {
      dxf_free_pair (pair);
      return dxf_read_pair (dat);
    }
  ent->preview = calloc (ent->preview_size, 1);
  if (!ent->preview)
    {
      LOG_ERROR ("Out of memory");
      return NULL;
    }
  LOG_TRACE ("%s.preview_size = " FORMAT_BLL " [BLL %d]\n", obj->name,
             ent->preview_size, pair->code);
  ent->preview_exists = 1;

  dxf_free_pair (pair);
  pair = dxf_read_pair (dat);
  while (pair->code == 310)
    {
      unsigned len = strlen (pair->value.s);
      unsigned blen = len / 2;
      const char *pos = pair->value.s;
      char *s = &ent->preview[written];
      if (blen + written > ent->preview_size)
        {
          LOG_ERROR (
              "%s.preview overflow: %u + written %u > size: " FORMAT_BLL,
              obj->name, blen, written, ent->preview_size);
          return pair;
        }
      for (unsigned i = 0; i < blen; i++)
        {
          sscanf (pos, "%2hhX", &s[i]);
          pos += 2;
        }
      written += blen;
      LOG_TRACE ("%s.preview += %u (%u/" FORMAT_BLL ")\n", obj->name, blen,
                 written, ent->preview_size);

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

// read to BLOCK_HEADER.preview_data (310), filling in the size
static Dxf_Pair *
add_block_preview (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                   Dxf_Pair *restrict pair)
{
  Dwg_Object_BLOCK_HEADER *_obj = obj->tio.object->tio.BLOCK_HEADER;
  unsigned written = 0;

  if (obj->type != DWG_TYPE_BLOCK_HEADER)
    {
      LOG_ERROR ("%s is no BLOCK_HEADER for a preview", obj->name);
      return pair;
    }
  if (pair->code != 310)
    {
      LOG_ERROR ("Invalid %s.preview_data code %d, need 310", obj->name,
                 pair->code);
      return pair;
    }
  while (pair->code == 310)
    {
      unsigned len = strlen (pair->value.s);
      unsigned blen = len / 2;
      const char *pos = pair->value.s;
      char *s;

      _obj->preview_data = realloc (_obj->preview_data, written + blen);
      s = &_obj->preview_data[written];
      for (unsigned i = 0; i < blen; i++)
        {
          sscanf (pos, "%2hhX", &s[i]);
          pos += 2;
        }
      written += blen;
      LOG_TRACE ("BLOCK_HEADER.preview_data += %u (%u)\n", blen, written);

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  _obj->preview_data_size = written;
  LOG_TRACE ("BLOCK_HEADER.preview_data_size = %u [BL 0]\n", written);
  return pair;
}

static int
add_SPLINE (Dwg_Entity_SPLINE *restrict _o, Bit_Chain *restrict dat,
            Dxf_Pair *restrict pair, int *restrict jp,
            BITCODE_RS *restrict flagp)
{
  int j = *jp;
  if (pair->code == 210 || pair->code == 220 || pair->code == 230)
    return 0; // ignore extrusion in the dwg (planar only)
  else if (pair->code == 70)
    {
      BITCODE_RS flag = *flagp;
      *flagp = flag = pair->value.i;
      _o->flag = flag;
      LOG_TRACE ("SPLINE.flag = %d [RS 70]\n", flag);
      if (flag & 1)
        {
          _o->closed_b = 1;
          LOG_TRACE ("SPLINE.closed_b = 1 [B 0] (1st bit)\n");
        }
      if (flag & 2)
        {
          _o->periodic = 1;
          LOG_TRACE ("SPLINE.periodic = 1 [B 0] (2nd bit)\n");
        }
      if (flag & 4)
        {
          _o->rational = 1;
          LOG_TRACE ("SPLINE.rational = 1 [B 0] (3rd bit)\n");
        }
      if (flag & 8)
        {
          _o->weighted = 1;
          LOG_TRACE ("SPLINE.weighted = 1 [B 0] (4th bit)\n");
        }
      if (flag & 1024)
        {
          _o->scenario = 2; // bezier: planar, not rational (8+32)
          LOG_TRACE ("SPLINE.scenario = 2 [B 0] (bezier)\n");
        }
      return 1; // found
    }
  else if (pair->code == 72)
    {
      _o->num_knots = pair->value.i;
      *jp = 0;
      _o->knots = calloc (_o->num_knots, sizeof (BITCODE_BD));
      LOG_TRACE ("SPLINE.num_knots = %d [BS 72]\n", _o->num_knots);
      return 1; // found
    }
  else if (pair->code == 73)
    {
      _o->num_ctrl_pts = pair->value.i;
      *jp = 0;
      _o->ctrl_pts
          = calloc (_o->num_ctrl_pts, sizeof (Dwg_SPLINE_control_point));
      LOG_TRACE ("SPLINE.ctrl_pts = %d [BS 73]\n", _o->num_ctrl_pts);
      return 1; // found
    }
  else if (pair->code == 74)
    {
      _o->num_fit_pts = pair->value.i;
      *jp = 0;
      _o->fit_pts = calloc (_o->num_fit_pts, sizeof (Dwg_SPLINE_point));
      _o->scenario = 2;
      LOG_TRACE ("SPLINE.num_fit_pts = %d [BS 74]\n", _o->num_fit_pts);
      return 1; // found
    }
  else if (pair->code == 40) // knots[] BD*
    {
      if (*jp >= (int)_o->num_knots)
        {
          LOG_ERROR ("SPLINE.knots[%d] overflow, max %d", *jp, _o->num_knots);
          return 1; // found
        }
      _o->knots[j] = pair->value.d;
      LOG_TRACE ("SPLINE.knots[%d] = %f [BD* 40]\n", *jp, pair->value.d);
      j++;
      *jp = j;
      if (j == (int)_o->num_knots)
        *jp = 0;
      return 1; // found
    }
  else if (pair->code == 10) // ctrl_pts[].x 3BD
    {
      if (*jp >= (int)_o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", *jp,
                     _o->num_ctrl_pts);
          return 1; // found
        }
      _o->ctrl_pts[j].parent = _o;
      _o->ctrl_pts[j].x = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 20) // ctrl_pts[].y 3BD
    {
      _o->ctrl_pts[j].y = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 30) // ctrl_pts[].z 3BD
    {
      _o->ctrl_pts[j].z = pair->value.d;
      LOG_TRACE ("SPLINE.ctrl_pts[%d] = (%f, %f, %f) [3BD* 10]\n", *jp,
                 _o->ctrl_pts[j].x, _o->ctrl_pts[j].y, _o->ctrl_pts[j].z);
      j++;
      *jp = j;
      if (j == (int)_o->num_ctrl_pts)
        *jp = 0;
      return 1; // found
    }
  else if (pair->code == 41) // ctrl_pts[].z 3BD
    {
      _o->ctrl_pts[j].w = pair->value.d;
      LOG_TRACE ("SPLINE.ctrl_pts[%d].w = %f [BD* 41]\n", *jp,
                 _o->ctrl_pts[j].w);
      j++;
      *jp = j;
      return 1; // found
    }
  else if (pair->code == 11) // fit_pts[].x 3BD
    {
      if (*jp >= _o->num_fit_pts)
        {
          LOG_ERROR ("SPLINE.fit_pts[%d] overflow, max %d", *jp,
                     _o->num_fit_pts);
          return 1; // found
        }
      _o->fit_pts[j].parent = _o;
      _o->fit_pts[j].x = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 21) // fit_pts[].y 3BD
    {
      _o->fit_pts[j].y = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 31) // fit_pts[].z 3BD
    {
      _o->fit_pts[j].z = pair->value.d;
      LOG_TRACE ("SPLINE.fit_pts[%d] = (%f, %f, %f) [3BD* 11]\n", *jp,
                 _o->fit_pts[j].x, _o->fit_pts[j].y, _o->fit_pts[j].z);
      j++;
      *jp = j;
      if (j == (int)_o->num_fit_pts)
        *jp = 0;
      return 1; // found
    }
  return 0;
}

static int
add_MLINE (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
           Dxf_Pair *restrict pair, int *restrict jp, int *restrict kp,
           int *restrict lp)
{
  Dwg_Entity_MLINE *_o = obj->tio.entity->tio.MLINE;
  int found = 1;
  int j = *jp;
  int k = *kp;
  int l = *lp;

  if (pair->code == 72)
    {
      _o->num_verts = pair->value.i;
      _o->parent = obj->tio.entity;
      _o->verts = calloc (_o->num_verts, sizeof (Dwg_MLINE_vertex));
      LOG_TRACE ("MLINE.num_verts = %d [BS 72]\n", _o->num_verts);
      *jp = 0;
    }
  else if (pair->code == 73)
    {
      _o->num_lines = pair->value.i;
      for (int _j = 0; _j < _o->num_verts; _j++)
        {
          _o->verts[_j].lines
              = calloc (_o->num_lines, sizeof (Dwg_MLINE_line));
        }
      LOG_TRACE ("MLINE.num_lines = %d [BS 73]\n", _o->num_lines);
      *kp = 0;
    }
  else if (pair->code == 11 && _o->num_verts)
    {
      _o->verts[j].parent = _o;
      _o->verts[j].vertex.x = pair->value.d;
    }
  else if (pair->code == 21 && _o->num_verts)
    _o->verts[j].vertex.y = pair->value.d;
  else if (pair->code == 31 && _o->num_verts)
    {
      _o->verts[j].vertex.z = pair->value.d;
      LOG_TRACE ("MLINE.verts[%d] = (%f, %f, %f) [3BD* 11]\n", j,
                 _o->verts[j].vertex.x, _o->verts[j].vertex.y,
                 _o->verts[j].vertex.z);
    }
  else if (pair->code == 12 && _o->num_verts)
    _o->verts[j].vertex_direction.x = pair->value.d;
  else if (pair->code == 22 && _o->num_verts)
    _o->verts[j].vertex_direction.y = pair->value.d;
  else if (pair->code == 32 && _o->num_verts)
    {
      _o->verts[j].vertex_direction.z = pair->value.d;
      LOG_TRACE ("MLINE.vertex_direction[%d] = (%f, %f, %f) [3BD* 12]\n", j,
                 _o->verts[j].vertex_direction.x,
                 _o->verts[j].vertex_direction.y,
                 _o->verts[j].vertex_direction.z);
    }
  else if (pair->code == 13 && _o->num_verts)
    _o->verts[j].miter_direction.x = pair->value.d;
  else if (pair->code == 23 && _o->num_verts)
    _o->verts[j].miter_direction.y = pair->value.d;
  else if (pair->code == 33 && _o->num_verts)
    {
      _o->verts[j].miter_direction.z = pair->value.d;
      LOG_TRACE ("MLINE.miter_direction[%d] = (%f, %f, %f) [3BD* 13]\n", j,
                 _o->verts[j].miter_direction.x,
                 _o->verts[j].miter_direction.y,
                 _o->verts[j].miter_direction.z);
      // if (j != _o->num_verts - 1)
      //  j++; // not the last
      *kp = 0;
    }
  else if (pair->code == 74 && _o->num_lines)
    {
      if (j >= _o->num_verts || k >= _o->num_lines)
        {
          LOG_ERROR ("MLINE overflow %d, %d", j, k);
          return 2;
        }
      _o->verts[j].lines[k].parent = &_o->verts[j];
      _o->verts[j].lines[k].num_segparms = pair->value.i;
      _o->verts[j].lines[k].segparms
          = calloc (pair->value.i, sizeof (BITCODE_BD));
      LOG_TRACE ("MLINE.v[%d].l[%d].num_segparms = %d [BS 74]\n", j, k,
                 pair->value.i);
      *lp = 0;
    }
  else if (pair->code == 41 && _o->num_lines)
    {
      _o->verts[j].lines[k].segparms[l] = pair->value.d;
      LOG_TRACE ("MLINE.v[%d].l[%d].segparms[%d] = %f [BD 41]\n", j, k, l,
                 pair->value.d);
      l++;
      *lp = l;
    }
  else if (pair->code == 75 && _o->num_lines)
    {
      _o->verts[j].lines[k].num_areafillparms = pair->value.i;
      LOG_TRACE ("MLINE.v[%d].l[%d].num_areafillparms = %d [BS 75]\n", j, k,
                 pair->value.i);
      if (!pair->value.i)
        {
          k++; // next line
          if (k == _o->num_lines)
            {
              j++; // next vertex
              *jp = j;
              k = 0;
            }
          *kp = k;
        }
      *lp = 0;
    }
  else if (pair->code == 42 && _o->num_lines)
    {
      _o->verts[j].lines[k].areafillparms[l] = pair->value.d;
      LOG_TRACE ("MLINE.v[%d].l[%d].areafillparms[%d] = %f [BD 42]\n", j, k, l,
                 pair->value.d);
      l++;
      *lp = l;
      if (l == _o->verts[j].lines[k].num_areafillparms)
        {
          l = 0;
          *lp = l;
          k++; // next line
          if (k == _o->num_lines)
            {
              j++; // next vertex
              *jp = j;
              k = 0;
            }
          *kp = k;
        }
    }
  else
    found = 0;

  return found;
}

// see GH #138. add vertices / attribs
static void
postprocess_SEQEND (Dwg_Object *obj)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_SEQEND *o = obj->tio.entity->tio.SEQEND;
  Dwg_Object *owner = dwg_ref_object (dwg, obj->tio.entity->ownerhandle);
  Dwg_Entity_POLYLINE_2D *ow;
  BITCODE_BL i, j, num_owned = 0;
  BITCODE_H seqend;
  BITCODE_H *owned = NULL;
  const char *owhdls;

  // r12 and earlier: search for owner backwards
  if (dwg->header.version < R_13 && !owner && !obj->tio.entity->ownerhandle)
    {
      for (i = obj->index - 1; i > 0; i--)
        {
          Dwg_Object *_o = &dwg->object[i];
          if (_o->type == DWG_TYPE_INSERT || _o->type == DWG_TYPE_MINSERT
              || _o->type == DWG_TYPE_POLYLINE_2D
              || _o->type == DWG_TYPE_POLYLINE_3D
              || _o->type == DWG_TYPE_POLYLINE_PFACE
              || _o->type == DWG_TYPE_POLYLINE_MESH)
            {
              owner = _o;
              obj->tio.entity->ownerhandle
                  = dwg_add_handleref (dwg, 4, _o->handle.value, obj);
              LOG_TRACE ("SEQEND.owner = " FORMAT_H " (%s) [H* 0]\n",
                         ARGS_H (_o->handle), _o->name);
              break;
            }
        }
    }
  if (!owner)
    {
      if (obj->tio.entity->ownerhandle)
        LOG_WARN ("Missing owner from " FORMAT_REF " [H 330]",
                  ARGS_REF (obj->tio.entity->ownerhandle))
      else
        LOG_WARN ("Missing owner")
      return;
    }
  obj->tio.entity->ownerhandle->obj = NULL;
  owhdls = memBEGINc (owner->name, "POLYLINE_") ? "vertex" : "attrib_handles";
  ow = owner->tio.entity->tio.POLYLINE_2D;
  seqend = dwg_add_handleref (dwg, 3, obj->handle.value, owner);
  dwg_dynapi_entity_set_value (ow, owner->name, "seqend", &seqend, 0);
  LOG_TRACE ("%s.seqend = " FORMAT_REF " [H 0]\n", owner->name,
             ARGS_REF (seqend));
  // num_owned is not properly stored in a DXF
  // collect children hdls. all objects from owner to here
  for (j = 0, i = owner->index + 1; i < obj->index; i++, j++)
    {
      Dwg_Object *_o = &dwg->object[i];
      num_owned = j + 1;
      if (dwg->header.version >= R_13)
        {
          owned = realloc (owned, num_owned * sizeof (BITCODE_H));
          owned[j] = dwg_add_handleref (dwg, 4, _o->handle.value, owner);
          LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [H* 0]\n", owner->name, owhdls,
                     j, ARGS_REF (owned[j]));
        }
    }
  if (!num_owned)
    return;
  dwg_dynapi_entity_set_value (ow, owner->name, "num_owned", &num_owned, 0);
  LOG_TRACE ("%s.num_owned = " FORMAT_BL " [BL 0]\n", owner->name, num_owned);
  // TODO: store all these fields, or just the ones for the requested version?
  if (dwg->header.version >= R_13 && dwg->header.version <= R_2000)
    {
      const char *firstfield;
      const char *lastfield;
      if (memBEGINc (owner->name, "POLYLINE_"))
        {
          firstfield = "first_vertex";
          lastfield = "last_vertex";
        }
      else
        {
          firstfield = "first_attrib";
          lastfield = "last_attrib";
        }
      dwg_dynapi_entity_set_value (ow, owner->name, firstfield, &owned[0], 0);
      LOG_TRACE ("%s.%s = " FORMAT_REF "[H 0]\n", owner->name, firstfield,
                 ARGS_REF (owned[0]));
      dwg_dynapi_entity_set_value (ow, owner->name, lastfield,
                                   &owned[num_owned - 1], 0);
      LOG_TRACE ("%s.%s] = " FORMAT_REF "[H 0]\n", owner->name, lastfield,
                 ARGS_REF (owned[num_owned - 1]));
      free (owned);
    }
  else if (dwg->header.version >= R_2004)
    {
      dwg_dynapi_entity_set_value (ow, owner->name, owhdls, &owned, 0);
    }
}

// seperate model_space and model_space into its own fields, out of entries[]
static void
move_out_BLOCK_CONTROL (Dwg_Object *restrict obj,
                        Dwg_Object_BLOCK_CONTROL *restrict _ctrl,
                        const char *f)
{
  // move out of entries
  for (BITCODE_BL j = 0; j < _ctrl->num_entries; j++)
    {
      if (_ctrl->entries[j]
          && _ctrl->entries[j]->absolute_ref == obj->handle.value)
        {
          LOG_TRACE ("remove %s from entries[%d]: " FORMAT_H "\n", f, j,
                     ARGS_H (obj->handle));
          _ctrl->num_entries--;
          LOG_TRACE ("BLOCK_CONTROL.num_entries = " FORMAT_BL "\n", _ctrl->num_entries);
          if (j < _ctrl->num_entries)
            memmove (&_ctrl->entries[j], &_ctrl->entries[j + 1],
                     (_ctrl->num_entries - 1) * sizeof (BITCODE_H));
          _ctrl->entries = realloc (_ctrl->entries,
                                    _ctrl->num_entries * sizeof (BITCODE_H));
          break;
        }
    }
}

static void
move_out_LTYPE_CONTROL (Dwg_Object *restrict obj,
                        Dwg_Object_LTYPE_CONTROL *restrict _ctrl,
                        const char *f)
{
  // move out of entries
  for (BITCODE_BL j = 0; j < _ctrl->num_entries; j++)
    {
      if (_ctrl->entries[j]
          && _ctrl->entries[j]->absolute_ref == obj->handle.value)
        {
          LOG_TRACE ("remove %s from entries[%d]: " FORMAT_H "\n", f, j,
                     ARGS_H (obj->handle));
          _ctrl->num_entries--;
          LOG_TRACE ("LTYPE_CONTROL.num_entries = " FORMAT_BL "\n", _ctrl->num_entries);
          if (j < _ctrl->num_entries)
            memmove (&_ctrl->entries[j], &_ctrl->entries[j + 1],
                     (_ctrl->num_entries - 1) * sizeof (BITCODE_H));
          _ctrl->entries = realloc (_ctrl->entries,
                                    _ctrl->num_entries * sizeof (BITCODE_H));
          break;
        }
    }
}

#define UPGRADE_ENTITY(FROM, TO)                                              \
  obj->type = obj->fixedtype = DWG_TYPE_##TO;                                 \
  obj->name = obj->dxfname = (char *)#TO;                                     \
  strcpy (name, obj->name);                                                   \
  LOG_TRACE ("change type to %s\n", name);                                    \
  if (sizeof (Dwg_Entity_##TO) > sizeof (Dwg_Entity_##FROM))                  \
    {                                                                         \
      LOG_TRACE ("realloc to %s\n", name);                                    \
      _obj = realloc (_obj, sizeof (Dwg_Entity_##TO));                        \
      obj->tio.entity->tio.TO = (Dwg_Entity_##TO *)_obj;                      \
    }

/* For tables, entities and objects.
 */
static Dxf_Pair *
new_object (char *restrict name, char *restrict dxfname,
            Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
            BITCODE_BL ctrl_id, BITCODE_BL i)
{
  int is_utf = dwg->header.version >= R_2007 ? 1 : 0;
  Dwg_Object *obj;
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Object_APPID *_obj = NULL; // the smallest
  // we'd really need a Dwg_Object_TABLE or Dwg_Object_Generic type
  char ctrlname[80];
  char subclass[80];
  char text[256]; // FIXME
  int in_xdict = 0;
  int in_reactors = 0;
  int in_blkrefs = 0;
  int is_entity = is_dwg_entity (name) || strEQc (name, "DIMENSION");
  // BITCODE_BL rcount1, rcount2, rcount3, vcount;
  // Bit_Chain *hdl_dat, *str_dat;
  int j = 0, k = 0, l = 0, error = 0;
  BITCODE_RL curr_inserts = 0;
  BITCODE_RS flag = 0;
  BITCODE_BB scale_flag;
  BITCODE_3BD pt;
  Dwg_Object *ctrl;
  subclass[0] = '\0';

  if (ctrl_id || i)
    {
      LOG_TRACE ("add %s [%d]\n", name, i)
    }
  else
    {
      if (strcmp (name, dxfname) != 0)
        LOG_TRACE ("add %s (%s)\n", name, dxfname)
      else
        LOG_TRACE ("add %s\n", name)
    }

  if (is_entity)
    {
      NEW_ENTITY (dwg, obj);

      obj->tio.entity->xdic_missing_flag = 1;
      obj->tio.entity->color.index = 256; // ByLayer
      obj->tio.entity->ltype_scale = 1.0;
      obj->tio.entity->linewt = 0x1d;

      if (*name == '3')
        {
          // Looks dangerous but name[80] is big enough
          memmove (&name[1], &name[0], strlen (name) + 1);
          *name = '_';
        }
      if (strEQc (name, "DIMENSION"))
        {
          ADD_ENTITY (DIMENSION_ANG2LN);
        }
      else
        {
          // clang-format off
          // ADD_ENTITY by name
          // check all objects
          #undef DWG_ENTITY
          #define DWG_ENTITY(token)             \
          if (strEQc (name, #token))            \
            {                                   \
              ADD_ENTITY (token);               \
            }                                   \
          else

          #include "objects.inc"
          //final else
          LOG_WARN ("Unknown object %s", name);

          #undef DWG_ENTITY
          #define DWG_ENTITY(token)
          // clang-format on
        }
    }
  else
    {
      NEW_OBJECT (dwg, obj);

      obj->tio.object->xdic_missing_flag = 1;
      if (!ctrl_id) // no table
        {
          // clang-format off

          // ADD_OBJECT by name
          // check all objects
          #undef DWG_OBJECT
          #define DWG_OBJECT(token)         \
            if (strEQc (name, #token))      \
              {                             \
                ADD_OBJECT (token);         \
              }

          #include "objects.inc"

          #undef DWG_OBJECT
          #define DWG_OBJECT(token)

          // clang-format on
        }
      else // a table
        {
          if (strEQc (name, "BLOCK_RECORD"))
            {
              // strcpy (name, "BLOCK_HEADER");
              strcpy (ctrlname, "BLOCK_CONTROL");
            }
          else
            {
              strncpy (ctrlname, name, 70);
              strcat (ctrlname, "_CONTROL");
            }

          // clang-format off
          ADD_TABLE_IF (LTYPE, LTYPE)
          else
          ADD_TABLE_IF (VPORT, VPORT)
          else
          ADD_TABLE_IF (APPID, APPID)
          else
          ADD_TABLE_IF (DIMSTYLE, DIMSTYLE)
          else
          ADD_TABLE_IF (LAYER, LAYER)
          else
          ADD_TABLE_IF (STYLE, STYLE)
          else
          ADD_TABLE_IF (UCS, UCS)
          else
          ADD_TABLE_IF (VIEW, VIEW)
          else
          ADD_TABLE_IF (BLOCK_RECORD, BLOCK_HEADER)
          // else
          // ADD_TABLE_IF (BLOCK_HEADER, BLOCK_HEADER)
          // else
          // ADD_TABLE_IF (VPORT_ENTITY, VPORT_ENTITY)
          else
          // clang-format on
          {
            LOG_ERROR ("Unknown DXF AcDbSymbolTableRecord %s", name);
            return pair;
          }
        }
    }

  if (!_obj)
    {
      LOG_ERROR ("Empty _obj at DXF AcDbSymbolTableRecord %s", name);
      return pair;
    }
  ctrl = &dwg->object[ctrl_id];

  {
    BITCODE_B xrefref = 1;
    // set defaults not in dxf:
    if (dwg_dynapi_entity_field (obj->name, "xrefref"))
      dwg_dynapi_entity_set_value (_obj, obj->name, "xrefref", &xrefref, 0);
    if (dwg_dynapi_entity_field (obj->name, "scale_flag"))
      {
        scale_flag = 3;
        dwg_dynapi_entity_set_value (_obj, obj->name, "scale_flag",
                                     &scale_flag, 0);
        LOG_TRACE ("%s.scale_flag = 3 (default)\n", obj->name);
      }
    if (dwg_dynapi_entity_field (obj->name, "scale"))
      {
        pt.x = pt.y = pt.z = 1.0;
        dwg_dynapi_entity_set_value (_obj, obj->name, "scale", &pt, 0);
        LOG_TRACE ("%s.scale = (1,1,1) (default)\n", obj->name);
        pt.x = pt.y = pt.z = 0.0;
      }
    if (dwg_dynapi_entity_field (obj->name, "extrusion"))
      {
        pt.x = pt.y = 0.0;
        pt.z = 1.0;
        dwg_dynapi_entity_set_value (_obj, obj->name, "extrusion", &pt, 0);
        LOG_TRACE ("%s.extrusion = (0,0,1) (default)\n", obj->name);
        pt.z = 0.0;
      }
  }

  // read table fields until next 0 table or 0 ENDTAB
  while (pair->code != 0)
    {
    start_loop:
#if 0
      // don't set defaults. TODO but needed to reset counters j, k, l
      if ((pair->type == VT_INT8 || pair->type == VT_INT16 || pair->type == VT_BOOL) &&
          pair->value.i == 0)
        goto next_pair;
      else if (pair->type == VT_REAL && pair->value.d == 0.0)
        goto next_pair;
      else if ((pair->type == VT_INT32 || pair->type == VT_INT64) &&
               pair->value.l == 0L)
        goto next_pair;
#endif
      switch (pair->code)
        { // common flags: name, xrefref, xrefdep, ...
        case 0:
          if (strEQc (name, "SEQEND"))
            postprocess_SEQEND (obj);
          return pair;
        case 105: /* DIMSTYLE only for 5 */
          if (strNE (name, "DIMSTYLE"))
            goto object_default;
          // fall through
        case 5:
          {
            obj->handle.value = pair->value.u;
            // check for existing BLOCK_HEADER.*Model_Space
            if (obj->fixedtype == DWG_TYPE_BLOCK_HEADER &&
                dwg->object[0].handle.value == pair->value.u)
              {
                dwg->num_objects--;
                free (obj->tio.object->tio.BLOCK_HEADER);
                obj = &dwg->object[0];
                _obj = obj->tio.object->tio.APPID;
                LOG_TRACE ("Reuse existing BLOCK_HEADER.*Model_Space %X [0]\n",
                           pair->value.u)
              }
            dwg_add_handle (&obj->handle, 0, pair->value.u, obj);
            LOG_TRACE ("%s.handle = " FORMAT_H " [H 5]\n", name,
                       ARGS_H (obj->handle));
            if (ctrl_id)
              {
                // add to ctrl "entries" HANDLE_VECTOR
                Dwg_Object_BLOCK_CONTROL *_ctrl
                    = dwg->object[ctrl_id].tio.object->tio.BLOCK_CONTROL;
                BITCODE_H *hdls = NULL;
                BITCODE_BL num_entries = 0;

                dwg_dynapi_entity_value (_ctrl, ctrlname, "num_entries",
                                         &num_entries, NULL);
                if (num_entries <= i)
                  {
                    // DXF often lies about num_entries, skipping defaults
                    // e.g. BLOCK_CONTROL contains mspace+pspace in DXF, but in
                    // the DWG they are extra. But this is fixed at case 2, not here.
                    LOG_WARN ("Misleading %s.num_entries %d for %dth entry",
                              ctrlname, num_entries, i + 1);
                    num_entries = i + 1;
                    dwg_dynapi_entity_set_value (
                        _ctrl, ctrlname, "num_entries", &num_entries, 0);
                    LOG_TRACE ("%s.num_entries = %d [BL 70]\n", ctrlname,
                               num_entries);
                  }
                dwg_dynapi_entity_value (_ctrl, ctrlname, "entries", &hdls,
                                         NULL);
                if (!hdls)
                  {
                    hdls = calloc (num_entries, sizeof (Dwg_Object_Ref *));
                  }
                else
                  {
                    hdls = realloc (hdls,
                                    num_entries * sizeof (Dwg_Object_Ref *));
                  }
                hdls[i] = dwg_add_handleref (dwg, 2, pair->value.u, obj);
                dwg_dynapi_entity_set_value (_ctrl, ctrlname, "entries", &hdls,
                                             0);
                LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [H* 0]\n", ctrlname,
                           "entries", i, ARGS_REF (hdls[i]));
              }
          }
          break;
        case 8:
          if (is_entity)
            {
              BITCODE_H handle = find_tablehandle (dwg, pair);
              if (!handle)
                {
                  LOG_WARN ("TODO resolve handle name %s %s", "layer",
                            pair->value.s)
                }
              else
                {
                  dwg_dynapi_common_set_value (_obj, "layer", &handle, is_utf);
                  LOG_TRACE ("%s.layer = %s " FORMAT_REF " [H 8]\n", name,
                             pair->value.s, ARGS_REF (handle));
                }
              break;
            }
          // fall through
        case 100: // for nested structs
          if (pair->code == 100)
            {
              strncpy (subclass, pair->value.s, 79);
              // set the real objname
              if (strEQc (obj->name, "DIMENSION_ANG2LN")
                  || strEQc (obj->name, "DIMENSION"))
                {
                  if (strEQc (subclass, "AcDbRotatedDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_LINEAR;
                      obj->name = obj->dxfname = (char *)"DIMENSION_LINEAR";
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbAlignedDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_ALIGNED;
                      obj->name = obj->dxfname = (char *)"DIMENSION_ALIGNED";
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbOrdinateDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_ORDINATE;
                      obj->name = obj->dxfname = (char *)"DIMENSION_ORDINATE";
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbDiametricDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_DIAMETER;
                      obj->name = obj->dxfname = (char *)"DIMENSION_DIAMETER";
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbRadialDimension"))
                    {
                      UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_RADIUS)
                    }
                  else if (strEQc (subclass, "AcDb3PointAngularDimension"))
                    {
                      UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ANG3PT)
                    }
                }
              // set the real objname
              else if (strEQc (obj->name, "POLYLINE_2D"))
                {
                  if (strEQc (subclass, "AcDb3dPolyline"))
                    {
                      UPGRADE_ENTITY (POLYLINE_2D, POLYLINE_3D)
                    }
                  else if (strEQc (subclass, "AcDbPolyFaceMesh"))
                    {
                      UPGRADE_ENTITY (POLYLINE_2D, POLYLINE_PFACE)
                    }
                  else if (strEQc (subclass, "AcDbPolygonMesh"))
                    {
                      UPGRADE_ENTITY (POLYLINE_2D, POLYLINE_MESH)
                    }
                }
              else if (strEQc (obj->name, "VERTEX_2D"))
                {
                  if (strEQc (subclass, "AcDb3dPolylineVertex"))
                    {
                      UPGRADE_ENTITY (VERTEX_2D, VERTEX_3D)
                    }
                  else if (strEQc (subclass, "AcDbPolyFaceMeshVertex"))
                    { // _MESH or _PFACE:
                      Dwg_Object_Ref *owner = obj->tio.entity->ownerhandle;
                      Dwg_Object *parent = dwg_ref_object (dwg, owner);
                      if (parent
                          && parent->fixedtype == DWG_TYPE_POLYLINE_PFACE)
                        {
                          UPGRADE_ENTITY (VERTEX_2D, VERTEX_PFACE)
                        }
                      else
                        { // AcDbPolygonMesh
                          UPGRADE_ENTITY (VERTEX_2D, VERTEX_MESH)
                        }
                    }
                  else if (strEQc (subclass, "AcDbFaceRecord"))
                    {
                      UPGRADE_ENTITY (VERTEX_2D, VERTEX_PFACE_FACE)
                    }
                }
              // with ASSOC2DCONSTRAINTGROUP, ASSOCNETWORK, ASSOCACTION
              else if (strstr (obj->name, "ASSOC")
                       && strEQc (subclass, "AcDbAssocAction"))
                {
                  pair = add_ASSOCACTION (obj, dat, pair); // NULL for success
                  if (!pair)
                    {
                      // TODO: yet unsupported
                      if (strEQc (name, "ASSOC2DCONSTRAINTGROUP"))
                        return dxf_read_pair (dat);
                      else
                        goto next_pair;
                    }
                  else
                    goto start_loop; /* failure */
                }
            }
          break;
        case 102:
          if (strEQc (pair->value.s, "{ACAD_XDICTIONARY"))
            in_xdict = 1;
          else if (strEQc (pair->value.s, "{ACAD_REACTORS"))
            in_reactors = 1;
          else if (ctrl_id && strEQc (pair->value.s, "{BLKREFS"))
            in_blkrefs = 1; // unique handle 331
          else if (strEQc (pair->value.s, "}"))
            in_reactors = in_xdict = in_blkrefs = 0;
          else if (strEQc (name, "XRECORD"))
            pair = add_xdata (dat, obj, pair);
          else
            LOG_WARN ("Unknown DXF 102 %s in %s", pair->value.s, name)
          break;
        case 331:
          if (ctrl_id && in_blkrefs) // BLKREFS TODO
            {
              BITCODE_H *inserts = NULL;
              BITCODE_H hdl;
              BITCODE_RL num_inserts;
              dwg_dynapi_entity_value (_obj, obj->name, "num_inserts",
                                       &num_inserts, 0);
              if (curr_inserts)
                dwg_dynapi_entity_value (_obj, obj->name, "inserts", &inserts,
                                         0);
              if (curr_inserts + 1 > num_inserts)
                {
                  LOG_HANDLE ("  extending %s.num_inserts %d < %d\n",
                              obj->name, num_inserts, curr_inserts + 1);
                  num_inserts = curr_inserts + 1;
                  dwg_dynapi_entity_set_value (_obj, obj->name, "num_inserts",
                                               &num_inserts, 0);
                }
              if (inserts)
                inserts = realloc (inserts, num_inserts * sizeof (BITCODE_H));
              else
                inserts = calloc (num_inserts, sizeof (BITCODE_H));
              dwg_dynapi_entity_set_value (_obj, obj->name, "inserts",
                                           &inserts, 0);
              hdl = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.inserts[%d] = " FORMAT_REF " [H* 331]\n",
                         obj->name, curr_inserts, ARGS_REF (hdl));
              inserts[curr_inserts++] = hdl;
              break;
            }
          // fall through
        case 330:
          if (in_reactors)
            {
              BITCODE_BL num = is_entity ? obj->tio.entity->num_reactors
                                         : obj->tio.object->num_reactors;
              BITCODE_H reactor
                  = dwg_add_handleref (dwg, 4, pair->value.u, NULL); // always abs
              LOG_TRACE ("%s.reactors[%d] = " FORMAT_REF " [H* 330]\n", name,
                         num, ARGS_REF (reactor));
              if (is_entity)
                {
                  obj->tio.entity->reactors
                      = realloc (obj->tio.entity->reactors,
                                 (num + 1) * sizeof (BITCODE_H));
                  obj->tio.entity->reactors[num] = reactor;
                  obj->tio.entity->num_reactors++;
                }
              else
                {
                  obj->tio.object->reactors
                      = realloc (obj->tio.object->reactors,
                                 (num + 1) * sizeof (BITCODE_H));
                  obj->tio.object->reactors[num] = reactor;
                  obj->tio.object->num_reactors++;
                }
            }
          // valid ownerhandle, if not XRECORD with an ownerhandle already
          else if (obj->fixedtype != DWG_TYPE_XRECORD
                   || !obj->tio.object->ownerhandle)
            {
              BITCODE_H owh = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              if (is_entity)
                obj->tio.entity->ownerhandle = owh;
              else
                obj->tio.object->ownerhandle = owh;
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [H 330]\n", name,
                         ARGS_REF (owh));
            }
          break;
        case 350: // DICTIONARY softhandle
        case 360: // {ACAD_XDICTIONARY or some hardowner
          if (pair->code == 360 && in_xdict)
            {
              BITCODE_H xdic = dwg_add_handleref (dwg, 3, pair->value.u, obj);
              if (is_entity)
                {
                  obj->tio.entity->xdicobjhandle = xdic;
                  obj->tio.entity->xdic_missing_flag = 0;
                }
              else
                {
                  obj->tio.object->xdicobjhandle = xdic;
                  obj->tio.object->xdic_missing_flag = 0;
                }
              LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [H 360]\n", name,
                         ARGS_REF (xdic));
              break;
            }
          // // DICTIONARY or DICTIONARYWDFLT, but not DICTIONARYVAR
          else if (memBEGINc (name, "DICTIONARY")
                   && strNE (name, "DICTIONARYVAR"))
            {
              add_dictionary_itemhandles (obj, pair, text);
              break;
            }
          // fall through
        case 340:
          if (pair->code == 340 && strEQc (name, "GROUP"))
            {
              Dwg_Object_GROUP *_o = obj->tio.object->tio.GROUP;
              BITCODE_H hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
              LOG_TRACE ("GROUP.groups[%d] = " FORMAT_REF " [H* 340]\n",
                         _o->num_groups, ARGS_REF (hdl));
              _o->groups = realloc (_o->groups,
                                    (_o->num_groups + 1) * sizeof (BITCODE_H));
              _o->groups[_o->num_groups] = hdl;
              _o->num_groups++;
              break;
            }
          // fall through
        case 2:
          if (ctrl_id && pair->code == 2)
            {
              dwg_dynapi_entity_set_value (_obj, obj->name, "name",
                                           &pair->value, is_utf);
              LOG_TRACE ("%s.name = %s [T 2]\n", name, pair->value.s);
              if (strEQc (name, "BLOCK_RECORD"))
                {
                  // seperate mspace and pspace into its own fields
                  Dwg_Object_BLOCK_CONTROL *_ctrl
                      = ctrl->tio.object->tio.BLOCK_CONTROL;
                  if (!strcasecmp (pair->value.s, "*Paper_Space"))
                    {
                      const char *f = "paper_space";
                      _ctrl->paper_space
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->paper_space));
                      dwg->header_vars.BLOCK_RECORD_PSPACE
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      move_out_BLOCK_CONTROL (obj, _ctrl, f);
                      i--;
                    }
                  else if (!strcasecmp (pair->value.s, "*Model_Space"))
                    {
                      const char *f = "model_space";
                      _ctrl->model_space
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->model_space));
                      dwg->header_vars.BLOCK_RECORD_MSPACE
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      move_out_BLOCK_CONTROL (obj, _ctrl, f);
                      i--;
                    }
                }
              else if (strEQc (name, "LTYPE"))
                {
                  // seperate bylayer and byblock into its own fields
                  Dwg_Object_LTYPE_CONTROL *_ctrl
                      = ctrl->tio.object->tio.LTYPE_CONTROL;
                  if (!strcasecmp (pair->value.s, "ByLayer"))
                    {
                      const char *f = "bylayer";
                      _ctrl->bylayer
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->bylayer));
                      dwg->header_vars.LTYPE_BYLAYER
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      move_out_LTYPE_CONTROL (obj, _ctrl, f);
                      i--;
                    }
                  else if (!strcasecmp (pair->value.s, "ByBlock"))
                    {
                      const char *f = "byblock";
                      _ctrl->byblock
                          = dwg_add_handleref (dwg, 3, obj->handle.value, obj);
                      LOG_TRACE ("%s.%s = " FORMAT_REF " [H 0]\n", ctrlname, f,
                                 ARGS_REF (_ctrl->byblock));
                      dwg->header_vars.LTYPE_BYBLOCK
                          = dwg_add_handleref (dwg, 5, obj->handle.value, obj);
                      // move out of entries
                      move_out_LTYPE_CONTROL (obj, _ctrl, f);
                      i--;
                    }
                }
              break;
            }
          if (pair->code == 2 && strEQc (name, "MLINE"))
            {
              // ignore name of mlinestyle, already set by ->mlinestyle
              break;
            }
          // fall through
        case 70:
          if (ctrl_id && pair->code == 70)
            {
              dwg_dynapi_entity_set_value (_obj, obj->name, "flag",
                                           &pair->value, is_utf);
              LOG_TRACE ("%s.flag = %d [RC 70]\n", name, pair->value.i);
              break;
            }
          // fall through
        case 420: // color.rgb's
        case 421:
        case 422:
        case 423:
        case 424:
        case 425:
        case 426:
        case 427:
          if (pair->code >= 420 && pair->code <= 427)
            {
              const char *fname = NULL;
              if (pair->code == 421
                  && (strEQc (name, "VPORT") || strEQc (name, "VIEWPORT")
                      || strEQc (name, "VIEW")))
                fname = "ambient_color";
              else if (pair->code == 421 && strEQc (name, "MTEXT"))
                fname = "bg_fill_color";
              else if (pair->code == 420 && strEQc (name, "MLINESTYLE"))
                // TODO or lines[].color
                fname = "fill_color";
              else if (pair->code == 420 && strEQc (name, "VISUALSTYLE"))
                fname = "color";
              else if (pair->code == 421 && strEQc (name, "VISUALSTYLE"))
                fname = "face_mono_color";
              else if (pair->code == 422 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_intersection_color";
              else if (pair->code == 423 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_obscured_color";
              else if (pair->code == 424 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_color";
              else if (pair->code == 425 && strEQc (name, "VISUALSTYLE"))
                fname = "edge_silhouette_color";
              else if (strEQc (name, "TABLE"))
                {
                  BITCODE_BL table_flag
                      = obj->tio.entity->tio.TABLE->table_flag_override;
                  BITCODE_BL border_color = obj->tio.entity->tio.TABLE
                                                ->border_color_overrides_flag;
                  if (pair->code == 421)
                    {
                      if (table_flag & 0x0800)
                        fname = "title_row_fill_color";
                      else if (table_flag & 0x01000)
                        fname = "header_row_fill_color";
                      else if (table_flag & 0x02000)
                        fname = "data_row_fill_color";
                      else if (border_color & 0x0008)
                        fname = "title_vert_left_color";
                      else if (border_color & 0x0200)
                        fname = "header_vert_left_color";
                      else if (border_color & 0x8000)
                        fname = "data_vert_left_color";
                    }
                  else if (pair->code == 422)
                    {
                      if (table_flag & 0x0020)
                        fname = "title_row_color";
                      else if (table_flag & 0x0040)
                        fname = "header_row_color";
                      else if (table_flag & 0x0080)
                        fname = "data_row_color";
                      else if (border_color & 0x0001)
                        fname = "title_horiz_top_color";
                      else if (border_color & 0x0040)
                        fname = "header_horiz_top_color";
                      else if (border_color & 0x1000)
                        fname = "data_horiz_top_color";
                    }
                  else if (pair->code == 423)
                    {
                      if (border_color & 0x0002)
                        fname = "title_horiz_ins_color";
                      else if (border_color & 0x0080)
                        fname = "header_horiz_ins_color";
                      else if (border_color & 0x2000)
                        fname = "data_horiz_ins_color";
                    }
                  else if (pair->code == 424)
                    {
                      if (border_color & 0x0004)
                        fname = "title_horiz_bottom_color";
                      else if (border_color & 0x0100)
                        fname = "header_horiz_bottom_color";
                      else if (border_color & 0x4000)
                        fname = "data_horiz_bottom_color";
                    }
                  else if (pair->code == 426)
                    {
                      if (border_color & 0x0010)
                        fname = "title_vert_ins_color";
                      else if (border_color & 0x0400)
                        fname = "header_vert_ins_color";
                      else if (border_color & 0x10000)
                        fname = "data_vert_ins_color";
                    }
                  else if (pair->code == 427)
                    {
                      if (border_color & 0x0020)
                        fname = "title_vert_right_color";
                      else if (border_color & 0x0800)
                        fname = "header_vert_right_color";
                      else if (border_color & 0x20000)
                        fname = "data_vert_right_color";
                    }
                }

              if (fname)
                {
                  BITCODE_CMC color;
                  dwg_dynapi_entity_value (_obj, obj->name, fname, &color,
                                           NULL);
                  color.rgb = pair->value.l;
                  LOG_TRACE ("%s.%s.rgb = %06X [CMC %d]\n", name, fname,
                             pair->value.u, pair->code);
                  dwg_dynapi_entity_set_value (_obj, obj->name, fname, &color,
                                               is_utf);
                  break;
                }
            }
          // fall through
        default:
        object_default:
          if (pair->code >= 1000 && pair->code < 1999)
            add_eed (obj, obj->name, pair);
          else if (pair->code != 280 && strEQc (name, "XRECORD"))
            pair = add_xdata (dat, obj, pair);
          else if (pair->code == 310 && strEQc (obj->name, "BLOCK_HEADER"))
            {
              pair = add_block_preview (obj, dat, pair);
              goto start_loop;
            }
          else if (pair->code == 1
                   && (strEQc (name, "_3DSOLID") || strEQc (name, "BODY")
                       || strEQc (name, "REGION")
                       || strEQc (subclass, "AcDbModelerGeometry")))
            {
              j = 0;
              k = 0;
              pair = add_3DSOLID_encr (obj, dat, pair);
              goto start_loop;
            }
          else if (pair->code == 370 && strEQc (name, "LAYER"))
            {
              Dwg_Object_LAYER *layer = obj->tio.object->tio.LAYER;
              layer->linewt = dxf_find_lweight (pair->value.i);
              LOG_TRACE ("LAYER.linewt = %d\n", layer->linewt);
              layer->flag |= layer->linewt << 5;
              LOG_TRACE ("LAYER.flag = 0x%x [BS 70]\n", layer->flag);
            }
          else if (pair->code == 71 && strEQc (name, "MLINESTYLE")
                   && pair->value.i != 0)
            {
              pair = new_MLINESTYLE_lines (obj, dat, pair);
              if (pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (pair->code == 90 && strEQc (name, "LWPOLYLINE"))
            {
              pair = new_LWPOLYLINE (obj, dat, pair);
              if (pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (strEQc (name, "MLINE"))
            {
              int status = add_MLINE (obj, dat, pair, &j, &k, &l);
              if (status == 0)
                goto search_field;
              else if (status == 2)
                break;
            }
          else if (strEQc (name, "VERTEX_PFACE_FACE") && pair->code >= 71
                   && pair->code <= 74)
            {
              Dwg_Entity_VERTEX_PFACE_FACE *_o
                  = (Dwg_Entity_VERTEX_PFACE_FACE *)_obj;
              j = pair->code - 71;
              _o->vertind[j] = pair->value.i;
              LOG_TRACE ("VERTEX_PFACE_FACE.vertind[%d] = %d [BS %d]\n", j,
                         pair->value.i, pair->code);
            }
          else if (strEQc (name, "SPLINE"))
            {
              if (pair->code == 210 || pair->code == 220 || pair->code == 230)
                break; // ignore extrusion in the dwg (planar only)
              if (add_SPLINE (obj->tio.entity->tio.SPLINE, dat, pair, &j,
                              &flag))
                goto next_pair;
              else
                goto search_field;
            }
          else if (strEQc (name, "HATCH"))
            {
              if (pair->code == 10 || pair->code == 20)
                break; // elevation
              else if (pair->code == 91 || pair->code == 78
                       || pair->code == 453)
                {
                  pair = add_HATCH (obj, dat, pair);
                  if (pair->code == 0) // end or unknown
                    return pair;
                  goto search_field;
                }
              else
                goto search_field;
            }
          else if (strEQc (name, "MESH"))
            {
              if (pair->code == 91)
                {
                  pair = add_MESH (obj, dat, pair);
                  if (pair->code == 0) // end or unknown
                    return pair;
                  goto search_field;
                }
              else
                goto search_field;
            }
          else if (strEQc (name, "DBCOLOR"))
            {
              Dwg_Object_DBCOLOR *o = obj->tio.object->tio.DBCOLOR;
              if (pair->code == 62)
                {
                  o->color.index = pair->value.i;
                  LOG_TRACE ("DBCOLOR.color.index = %d [CMC %d]\n",
                             pair->value.i, pair->code);
                  goto next_pair;
                }
              else if (pair->code == 430)
                {
                  char *x;
                  o->catalog = strdup (pair->value.s);
                  x = strchr (o->catalog, '$');
                  if (!x)
                    goto search_field;
                  o->name = strdup (x + 1);
                  x[0] = '\0';
                  LOG_TRACE ("DBCOLOR.catalog = %s [CMC %d]\n", o->catalog,
                             pair->code);
                  LOG_TRACE ("DBCOLOR.name = %s [CMC %d]\n", o->name,
                             pair->code);
                  goto next_pair;
                }
              else
                goto search_field;
            }
          else if (strEQc (name, "MLEADERSTYLE"))
            {
              Dwg_Object_MLEADERSTYLE *o = obj->tio.object->tio.MLEADERSTYLE;
              if (pair->code == 47)
                o->block_scale.x = pair->value.d;
              else if (pair->code == 49)
                o->block_scale.y = pair->value.d;
              else if (pair->code == 140)
                {
                  o->block_scale.z = pair->value.d;
                  LOG_TRACE (
                      "MLEADERSTYLE.block_scale = (%f, %f, %f) [3BD 47]\n",
                      o->block_scale.x, o->block_scale.y, o->block_scale.z);
                }
              else
                goto search_field;
            }
          else if (strEQc (name, "MTEXT") && pair->code == 50)
            {
              Dwg_Entity_MTEXT *o = obj->tio.entity->tio.MTEXT;
              if (!o->num_column_heights)
                o->num_column_heights = 1;
              if (!j)
                o->column_heights
                    = calloc (o->num_column_heights, sizeof (BITCODE_BD));
              assert (j < (int)o->num_column_heights);
              o->column_heights[j] = pair->value.d;
              LOG_TRACE ("MTEXT.column_heights[%d] = %f [BD* 50]\n", j,
                         pair->value.d);
            }
          else if (strEQc (name, "LEADER")
                   && (pair->code == 10 || pair->code == 20
                       || pair->code == 30))
            {
              Dwg_Entity_LEADER *o = obj->tio.entity->tio.LEADER;
              if (!j && pair->code == 10)
                o->points = calloc (o->num_points, sizeof (BITCODE_3BD));
              assert (j < (int)o->num_points);
              if (pair->code == 10)
                o->points[j].x = pair->value.d;
              else if (pair->code == 20)
                o->points[j].y = pair->value.d;
              else if (pair->code == 30)
                {
                  o->points[j].z = pair->value.d;
                  LOG_TRACE ("LEADER.points[%d] = (%f, %f, %f) [3BD* 10]\n", j,
                             o->points[j].x, o->points[j].y, o->points[j].z);
                  j++;
                }
            }
          else
          search_field:
            { // search all specific fields and common fields for the DXF
              const Dwg_DYNAPI_field *f;
              const Dwg_DYNAPI_field *fields
                  = dwg_dynapi_entity_fields (obj->name);
              if (!fields)
                {
                  LOG_ERROR ("Illegal object name %s, no dynapi fields",
                             obj->name);
                  break;
                }
              for (f = &fields[0]; f->name; f++)
                {
                  // VECTORs. need to be malloced, and treated specially
                  if (pair->code != 3 && f->is_malloc && !f->is_string
                      && strNE (f->name, "parent")) // parent set in NEW_OBJECT
                    {
                      // FIELD_2RD_VECTOR (clip_verts, num_clip_verts, 11|14);
                      if (pair->code >= 10 && pair->code <= 24
                          && strEQc (f->name, "clip_verts")) // 11 or 14
                        {
                          BITCODE_BL num_clip_verts = 0;
                          BITCODE_2RD *clip_verts;
                          // 11 has no num_clip_verts: realloc
                          if (pair->code == 14)
                            {
                              dwg_dynapi_entity_value (_obj, obj->name,
                                                       "num_clip_verts",
                                                       &num_clip_verts, NULL);
                              LOG_INSANE ("%s.num_clip_verts = %d, j = %d\n",
                                          name, num_clip_verts, j);
                            }
                          dwg_dynapi_entity_value (_obj, obj->name,
                                                   "clip_verts", &clip_verts,
                                                   NULL);
                          // assert (j == 0 || j < (int)num_clip_verts);
                          if (pair->code < 20)
                            {
                              // no need to realloc
                              if (!j && pair->code == 14)
                                {
                                  clip_verts = calloc (num_clip_verts,
                                                       sizeof (BITCODE_2RD));
                                  dwg_dynapi_entity_set_value (_obj, obj->name,
                                                               f->name,
                                                               &clip_verts, 0);
                                }
                              else if (pair->code == 11)
                                {
                                  clip_verts = realloc (
                                      clip_verts,
                                      (j + 1) * sizeof (BITCODE_2RD));
                                  dwg_dynapi_entity_set_value (_obj, obj->name,
                                                               f->name,
                                                               &clip_verts, 0);
                                }
                              clip_verts[j].x = pair->value.d;
                            }
                          else if (pair->code < 30)
                            {
                              clip_verts[j].y = pair->value.d;
                              LOG_TRACE ("%s.%s[%d] = (%f, %f) [2RD* %d]\n",
                                         name, "clip_verts", j,
                                         clip_verts[j].x, clip_verts[j].y,
                                         pair->code - 10);
                              j++;
                              if (pair->code == 21)
                                {
                                  dwg_dynapi_entity_set_value (
                                      _obj, obj->name, "num_clip_verts", &j,
                                      0);
                                  LOG_TRACE ("%s.num_clip_verts = %d\n", name,
                                             j);
                                }
                            }
                          goto next_pair;
                        }
                      else if (f->dxf == pair->code)
                        LOG_TRACE ("Warning: Ignore %s.%s VECTOR [%s %d]\n",
                                   name, f->name, f->type, pair->code);
                    }
                  else if (f->dxf == pair->code) // matching DXF code
                    {
                      // exceptions, where there's another field 92:
                      if ((pair->code == 92) && is_entity
                          && obj->fixedtype > DWG_TYPE_LAYOUT
                          && strEQc (subclass, "AcDbEntity"))
                        // not MULTILEADER.text_color, nor MESH.num_vertex
                        {
                          pair = add_ent_preview (obj, dat, pair);
                          goto start_loop;
                        }
                      else if (pair->code == 3
                               && memBEGINc (obj->name, "DICTIONARY")
                               && strNE (obj->name, "DICTIONARYVAR"))
                        {
                          strncpy (text, pair->value.s, 254);
                          text[255] = '\0';
                          goto next_pair; // skip setting texts TV*
                        }
                      // convert angle to radians
                      else if (pair->code >= 50 && pair->code <= 55)
                        {
                          BITCODE_BD ang;
                          if (pair->value.d == 0.0)
                            goto next_pair;
                          ang = deg2rad (pair->value.d);
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &ang, 0);
                          LOG_TRACE ("%s.%s = %f (from DEG %f°) [%s %d]\n",
                                     name, f->name, ang, pair->value.d,
                                     f->type, pair->code);
                          goto next_pair; // found
                        }
                      // resolve handle, by name or ref
                      else if (strEQc (f->type, "H"))
                        {
                          BITCODE_H ref = find_tablehandle (dwg, pair);
                          if (!ref)
                            /*
                              ref = dwg_add_handleref (dwg, 5, 0, obj);
                          if (0)
                            */
                            {
                              if (pair->code > 300)
                                {
                                  LOG_WARN ("TODO resolve handle %s.%s %X",
                                            name, f->name, pair->value.u)
                                }
                              else if (0) // TODO check alias names: BYLAYER,
                                          // BYBLOCK, CONTINUOUS
                                {
                                }
                              else
                                {
                                  LOG_WARN (
                                      "TODO resolve handle name %s.%s %s",
                                      name, f->name, pair->value.s)
                                }
                            }
                          else
                            {
                              dwg_dynapi_entity_set_value (
                                  _obj, obj->name, f->name, &ref, is_utf);
                              LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n",
                                         name, f->name, ARGS_REF (ref),
                                         pair->code);
                            }
                          goto next_pair; // found
                        }
                      // only 2D or 3D points .x
                      else if (f->size > 8
                               && (strchr (f->type, '2')
                                   || strchr (f->type, '3')
                                   || strEQc (f->type, "BE")))
                        {
                          pt.x = 0.0;
                          if (pair->value.d == 0.0) // ignore defaults
                            goto next_pair;
                          pt.x = pair->value.d;
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &pt, is_utf);
                          LOG_TRACE ("%s.%s.x = %f [%s %d]\n", name, f->name,
                                     pair->value.d, f->type, pair->code);
                          goto next_pair; // found
                        }
                      else if (f->size > 8 && strEQc (f->type, "CMC"))
                        {
                          BITCODE_CMC color;
                          dwg_dynapi_entity_value (_obj, obj->name, f->name,
                                                   &color, NULL);
                          if (pair->code < 100)
                            {
                              color.index = pair->value.i;
                              LOG_TRACE ("%s.%s.index = %d [%s %d]\n", name,
                                         f->name, pair->value.i, "CMC", pair->code);
                            }
                          else if (pair->code < 430)
                            {
                              color.rgb = pair->value.l & 0x00FFFFFF;
                              color.alpha = (pair->value.l & 0xFF000000) >> 24;
                              if (color.alpha)
                                color.alpha_type = 3;
                              LOG_TRACE ("%s.%s.rgb = %08X [%s %d]\n", name,
                                         f->name, pair->value.u, "CMC", pair->code);
                            }
                          else if (pair->code < 440)
                            {
                              color.flag |= 1;
                              color.name = strdup (pair->value.s);
                              LOG_TRACE ("%s.%s.name = %s [%s %d]\n", name,
                                         f->name, pair->value.s, "CMC", pair->code);
                            }
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, f->name, &color, is_utf);
                          goto next_pair; // found, early exit
                        }
                      else
                        dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                     &pair->value, is_utf);
                      if (f->is_string)
                        {
                          LOG_TRACE ("%s.%s = %s [%s %d]\n", name, f->name,
                                     pair->value.s, f->type, pair->code);
                        }
                      else if (strchr (&f->type[1], 'D'))
                        {
                          LOG_TRACE ("%s.%s = %f [%s %d]\n", name, f->name,
                                     pair->value.d, f->type, pair->code);
                        }
                      else
                        {
                          LOG_TRACE ("%s.%s = %ld [%s %d]\n", name, f->name,
                                     pair->value.l, f->type, pair->code);
                        }
                      goto next_pair; // found, early exit
                    }
                  // wrong code, maybe a point .y or .z
                  else if ((*f->type == '3' || *f->type == '2'
                            || strEQc (f->type, "BE"))
                           && (strstr (f->type, "_1")
                                   ? f->dxf + 1 == pair->code // 2BD_1
                                   : f->dxf + 10 == pair->code))
                    {
                      pt.y = 0.0;
                      if (pair->value.d == 0.0) // ignore defaults
                        goto next_pair;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name, &pt,
                                               NULL);
                      pt.y = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &pt, is_utf);
                      LOG_TRACE ("%s.%s.y = %f [%s %d]\n", name, f->name,
                                 pair->value.d, f->type, pair->code);
                      goto next_pair; // found, early exit
                    }
                  else if ((*f->type == '3' || *f->type == '2'
                            || strEQc (f->type, "BE"))
                           && (strstr (f->type, "_1")
                                   ? f->dxf + 2 == pair->code // 2BD_1
                                   : f->dxf + 20 == pair->code))
                    {
                      pt.z = 0.0;
                      // can ignore z or 0.0?
                      if (strNE (name, "_3DFACE") && strNE (f->name, "scale")
                          && (pair->value.d == 0.0 || *f->type == '2'))
                        goto next_pair;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name, &pt,
                                               NULL);
                      pt.z = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &pt, 0);
                      LOG_TRACE ("%s.%s.z = %f [%s %d]\n", name, f->name,
                                 pair->value.d, f->type, pair->code);

                      // 3DD scale
                      if (strEQc (f->name, "scale")
                          && dwg->header.version >= R_2000
                          && dwg_dynapi_entity_field (obj->name, "scale_flag")
                          && dwg_dynapi_entity_value (_obj, obj->name,
                                                      "scale_flag",
                                                      &scale_flag, NULL))
                        { // set scale_flag
                          scale_flag = 0;
                          if (pt.x == 1.0 && pt.y == 1.0 && pt.z == 1.0)
                            scale_flag = 3;
                          else if (pt.x == 1.0)
                            scale_flag = 1;
                          else if (pt.x == pt.y && pt.x == pt.z)
                            scale_flag = 2;
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, "scale_flag", &scale_flag, 0);
                          LOG_TRACE ("%s.scale_flag = %d [BB 0]\n", name,
                                     scale_flag);
                        }
                      // 3DFACE.z_is_zero
                      else if (strEQc (name, "_3DFACE")
                               && strEQc (f->name, "corner1")
                               && dwg->header.version >= R_2000 && pt.z == 0.0)
                        {
                          BITCODE_B z_is_zero = 1;
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, "z_is_zero", &z_is_zero, 0);
                          LOG_TRACE ("%s.z_is_zero = 1 [B 0]\n", name);
                        }

                      goto next_pair; // found, early exit
                    }
                  // FIELD_VECTOR_N BITCODE_BD transmatrix[16]:
                  else if (strEQc (f->type, "BD*")
                           && (strEQc (name, "EXTRUDEDSURFACE")
                               || strEQc (name, "LOFTEDSURFACE")
                               || strEQc (name, "SWEPTSURFACE")
                               || strEQc (name, "REVOLVEDSURFACE")
                               || strEQc (name, "MATERIAL")
                               || strEQc (name, "SPATIAL_FILTER")
                               || /* max 12 */
                               strEQc (name, "ACSH_SWEEP_CLASS"))
                           && ((pair->code >= 40 && pair->code <= 49)
                               || (pair->code <= 142 && pair->code <= 147)))
                    {
                      // 16x BD, via j
                      BITCODE_BD *matrix;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name,
                                               &matrix, NULL);
                      if (!matrix)
                        {
                          matrix = calloc (16, sizeof (BITCODE_BD));
                          j = 0;
                        }
                      assert (j >= 0 && j < 16);
                      matrix[j] = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &matrix, 0);
                      LOG_TRACE ("%s.%s[%d] = %f [%s %d]\n", name, f->name, j,
                                 pair->value.d, f->type, pair->code);
                      j++;
                      goto next_pair;
                    }
                }

              fields = is_entity ? dwg_dynapi_common_entity_fields ()
                                 : dwg_dynapi_common_object_fields ();
              for (f = &fields[0]; f->name; f++)
                {
                  if ((pair->code == 62 || pair->code == 420
                       || pair->code == 430)
                      && (f->size > 8
                          && strEQc (f->type, "CMC"))) // alt. color fields
                    {
                      BITCODE_CMC color;
                      dwg_dynapi_common_value (_obj, f->name, &color, NULL);
                      if (pair->code == 62)
                        {
                          color.index = pair->value.i;
                          LOG_TRACE ("COMMON.%s.index = %d [%s %d]\n", f->name,
                                     pair->value.i, "CMC", pair->code);
                        }
                      else if (pair->code == 420)
                        {
                          color.rgb = pair->value.l;
                          color.alpha = (pair->value.l & 0xFF000000) >> 24;
                          if (color.alpha)
                            color.alpha_type = 3;
                          LOG_TRACE ("COMMON.%s.rgb = %08X [%s %d]\n", f->name,
                                     pair->value.u, "CMC", pair->code);
                        }
                      else if (pair->code == 430)
                        {
                          color.flag |= 1;
                          color.name = strdup (pair->value.s);
                          // TODO: book_name or name?
                          LOG_TRACE ("COMMON.%s.name = %s [%s %d]\n", f->name,
                                     pair->value.s, "CMC", pair->code);
                        }
                      dwg_dynapi_common_set_value (_obj, f->name, &color,
                                                   is_utf);
                      goto next_pair; // found, early exit
                    }
                  else if (f->dxf == pair->code)
                    {
                      /// resolve handle (table entry) given by name or ref
                      if (strEQc (f->type, "H"))
                        {
                          BITCODE_H handle = find_tablehandle (dwg, pair);
                          if (!handle)
                            {
                              if (pair->code > 300)
                                {
                                  handle = dwg_add_handleref (
                                      dwg, 5, pair->value.u, obj);
                                  dwg_dynapi_common_set_value (_obj, f->name,
                                                               &handle, 0);
                                  LOG_TRACE ("COMMON.%s = %X [H %d]\n",
                                             f->name, pair->value.u, pair->code)
                                }
                              else
                                {
                                  LOG_WARN (
                                      "TODO resolve common handle name %s %s",
                                      f->name, pair->value.s)
                                }
                            }
                          else
                            {
                              if (pair->code > 300)
                                LOG_TRACE ("COMMON.%s = %lX [H %d]\n",
                                           f->name, pair->value.l, 
                                           pair->code)
                              else
                                LOG_TRACE ("COMMON.%s = %s [H %d]\n", f->name,
                                           pair->value.s, pair->code)
                              dwg_dynapi_common_set_value (_obj, f->name,
                                                           &handle, 0);
                            }
                          if (is_entity && pair->code == 6
                              && dwg->header.version >= R_2000)
                            {
                              BITCODE_BB flags = 3;
                              if (!strcasecmp (pair->value.s, "BYLAYER"))
                                flags = 0;
                              if (!strcasecmp (pair->value.s, "BYBLOCK"))
                                flags = 1;
                              if (!strcasecmp (pair->value.s, "CONTINUOUS"))
                                flags = 2;
                              dwg_dynapi_common_set_value (_obj, "ltype_flags",
                                                           &flags, 0);
                              LOG_TRACE ("COMMON.%s = %d [BB 0]\n",
                                         "ltype_flags", flags);
                            }
                          if (is_entity && pair->code == 390
                              && dwg->header.version >= R_2000)
                            {
                              BITCODE_BB flags = 3;
                              /*
                              if (!strcasecmp (pair->value.s, "BYLAYER"))
                                flags = 0;
                              if (!strcasecmp (pair->value.s, "BYBLOCK"))
                                flags = 1;
                              */
                              dwg_dynapi_common_set_value (
                                  _obj, "plotstyle_flags", &flags, 0);
                              LOG_TRACE ("COMMON.%s = %d [BB 0]\n",
                                         "plotstyle_flags", flags);
                            }
                          if (is_entity && pair->code == 347
                              && dwg->header.version >= R_2007)
                            {
                              BITCODE_BB flags = 3;
                              /*
                              if (!strcasecmp (pair->value.s, "BYLAYER"))
                                flags = 0;
                              if (!strcasecmp (pair->value.s, "BYBLOCK"))
                                flags = 1;
                              */
                              dwg_dynapi_common_set_value (
                                  _obj, "material_flags", &flags, 0);
                              LOG_TRACE ("COMMON.%s = %d [BB 0]\n",
                                         "material_flags", flags);
                            }
                          goto next_pair; // found, early exit
                        }
                      else
                        {
                          dwg_dynapi_common_set_value (_obj, f->name,
                                                       &pair->value, is_utf);
                          if (f->is_string)
                            {
                              LOG_TRACE ("COMMON.%s = %s [%s %d]\n", f->name,
                                         pair->value.s, f->type, pair->code)
                            }
                          else
                            {
                              if (is_entity && pair->code == 160) //
                                {
                                  pair = add_ent_preview (obj, dat, pair);
                                  goto start_loop; // already fresh pair
                                }
                              if (strchr (f->type, 'D'))
                                LOG_TRACE ("COMMON.%s = %f [%s %d]\n", f->name,
                                           pair->value.d, f->type, pair->code)
                              else
                                LOG_TRACE ("COMMON.%s = %ld [%s %d]\n", f->name,
                                           pair->value.l, f->type, pair->code)
                            }
                          goto next_pair; // found, early exit
                        }
                    }
                }
              // still needed? already handled above
              // not in dynapi: 92 as 310 size prefix for PROXY vector preview
              // data
              if ((pair->code == 92) && is_entity
                  && obj->fixedtype > DWG_TYPE_LAYOUT
                  && strEQc (subclass, "AcDbEntity"))
                /*
                  (obj->fixedtype == DWG_TYPE_WIPEOUT ||
                   obj->fixedtype == DWG_TYPE_MESH ||
                   obj->fixedtype == DWG_TYPE_MULTILEADER ||
                   obj->fixedtype == DWG_TYPE_UNDERLAY ||
                   obj->fixedtype == DWG_TYPE_HELIX ||
                   obj->fixedtype == DWG_TYPE_LIGHT ||
                   obj->fixedtype == DWG_TYPE_ARC_DIMENSION)) */
                {
                  pair = add_ent_preview (obj, dat, pair);
                  goto start_loop;
                }
              else if (strEQc (name, "MULTILEADER"))
                {
                  // for the unknown subfields: 300, 140, 145, 302, 304, ...
                  pair = add_MULTILEADER (obj, dat, pair);
                  // returns with 0 or 301
                  if (pair && pair->code == 301)
                    goto next_pair;
                  goto start_loop;
                }
              else if (strEQc (name, "BLOCK")
                       && (pair->code == 70 || pair->code == 10
                           || pair->code == 20 || pair->code == 30
                           || pair->code == 3 || pair->code == 1))
                ; // ignore those BLOCK fields. DXF artifacts
              else if (strEQc (name, "DIMENSION")
                       && (pair->code == 2 || pair->code == 210
                           || pair->code == 220 || pair->code == 230))
                ; // ignore the POLYLINE elevation.x,y. DXF artifacts
              else if ((strEQc (name, "POLYLINE_2D")
                        || strEQc (name, "POLYLINE_3D")
                        || strEQc (name, "HATCH"))
                       && (pair->code == 10 || pair->code == 20))
                ; // ignore the VERTEX_PFACE_FACE 3BD 10
              else if ((strEQc (name, "VERTEX_PFACE_FACE")
                        || strEQc (name, "POLYLINE_PFACE"))
                       && (pair->code == 10 || pair->code == 20
                           || pair->code == 30))
                ; // ignore the POLYLINE_PFACE flag 70
              else if (strEQc (name, "POLYLINE_PFACE") && pair->code == 70)
                ;
              else if (strEQc (name, "POLYLINE_3D") && pair->code == 30)
                ;
              else
                LOG_WARN ("Unknown DXF code %d for %s", pair->code, name);
            }
        }
    next_pair:
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_RETURN_EOF (pair);
    }

  if (strEQc (name, "SEQEND"))
    postprocess_SEQEND (obj);
  // set defaults not in dxf:
  else if (strEQc (name, "_3DFACE") && dwg->header.version >= R_2000)
    {
      Dwg_Entity__3DFACE *o = obj->tio.entity->tio._3DFACE;
      o->has_no_flags = 1;
      LOG_TRACE ("_3DFACE.has_no_flags = 1 [B]\n");
    }

  // common_entity_handle_data:
  // set xdic_missing_flag and xdicobjhandle if <2004
  if (is_entity ? !obj->tio.entity->xdicobjhandle
                : !obj->tio.object->xdicobjhandle)
    {
      if (dwg->header.version >= R_2004)
        {
          if (is_entity)
            obj->tio.entity->xdic_missing_flag = 1;
          else
            obj->tio.object->xdic_missing_flag = 1;
        }
      else if (dwg->header.version >= R_13 && !is_entity)
        obj->tio.object->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
      else if (dwg->header.version >= R_13 && is_entity)
        obj->tio.entity->xdicobjhandle = dwg_add_handleref (dwg, 3, 0, obj);
    }
  if (is_entity)
    {
      Dwg_Object_Entity *ent = obj->tio.entity;
      if (dwg->header.version >= R_13 && dwg->header.version <= R_14)
        {
          if (ent->ltype_flags < 3)
            ent->isbylayerlt = 1;
        }
      if (dwg->header.version >= R_13 && dwg->header.version <= R_2000
          && obj->type != DWG_TYPE_SEQEND && obj->type != DWG_TYPE_ENDBLK)
        {
          Dwg_Object *prev = find_prev_entity (obj);
          ent->next_entity = NULL; // temp.
          if (prev)
            {
              if (prev->tio.entity->prev_entity)
                prev->tio.entity->nolinks = 0;
              if (prev->type != DWG_TYPE_SEQEND
                  && prev->type != DWG_TYPE_ENDBLK)
                {
                  prev->tio.entity->nolinks = 0;
                  prev->tio.entity->next_entity
                      = dwg_add_handleref (dwg, 4, obj->handle.value, prev);
                  LOG_TRACE ("prev %s(%lX).next_entity = " FORMAT_REF "\n",
                             prev->name, prev->handle.value,
                             ARGS_REF (prev->tio.entity->next_entity));
                  ent->nolinks = 0;
                  ent->prev_entity
                    = dwg_add_handleref (dwg, 4, prev->handle.value, obj);
                  LOG_TRACE ("%s.prev_entity = " FORMAT_REF "\n", name,
                             ARGS_REF (ent->prev_entity));
                }
              else
                {
                  LOG_TRACE ("%s.prev_entity = NULL HANDLE 4\n", name);
                  ent->prev_entity = NULL;
                  ent->nolinks = 1;
                }
            }
          else
            {
              LOG_TRACE ("%s.prev_entity = NULL HANDLE 4\n", name);
              ent->prev_entity = NULL;
              ent->nolinks = 1;
            }
        }
      else if (obj->type != DWG_TYPE_SEQEND && obj->type != DWG_TYPE_ENDBLK)
        ent->nolinks = 1;
    }

  return pair;
}

static int
dxf_tables_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char table[80];
  Dxf_Pair *pair = dxf_read_pair (dat);

  table[0] = '\0'; // init
  while (1)        // read next 0 TABLE
    {
      if (pair->code == 0) // TABLE or ENDTAB
        {
          if (strEQc (pair->value.s, "TABLE"))
            table[0] = '\0'; // new table coming up
          else if (strEQc (pair->value.s, "BLOCK_RECORD"))
            strncpy (table, pair->value.s, 79);
          else if (strEQc (pair->value.s, "ENDTAB"))
            table[0] = '\0'; // close table
          else if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            LOG_WARN ("Unknown 0 %s (%s)", pair->value.s, "tables");
        }
      if (pair->code == 2 && strlen (pair->value.s) < 80
          && is_table_name (pair->value.s)) // new table NAME
        {
          BITCODE_BL i = 0;
          BITCODE_BL ctrl_id;
          strncpy (table, pair->value.s, 79);
          pair = new_table_control (table, dat, dwg); // until 0 table
          ctrl_id = dwg->num_objects - 1;             // dwg->object might move
          while (pair && pair->code == 0 && strEQ (pair->value.s, table))
            {
              // until 0 table or 0 ENDTAB
              pair = new_object (table, pair->value.s, dat, dwg, ctrl_id, i++);
              // undo BLOCK_CONTROL.entries and LTYPE_CONTROL.entries
              if (strEQc (table, "BLOCK_RECORD"))
                {
                  Dwg_Object *obj = &dwg->object[dwg->num_objects - 1];
                  Dwg_Object *ctrl = &dwg->object[ctrl_id];
                  Dwg_Object_BLOCK_CONTROL *_ctrl
                      = ctrl->tio.object->tio.BLOCK_CONTROL;
                  if (_ctrl->model_space
                      && obj->handle.value == _ctrl->model_space->absolute_ref)
                    i--;
                  else if (_ctrl->paper_space
                           && obj->handle.value
                                  == _ctrl->paper_space->absolute_ref)
                    i--;
                }
              else if (strEQc (table, "LTYPE"))
                {
                  Dwg_Object *obj = &dwg->object[dwg->num_objects - 1];
                  Dwg_Object_LTYPE *_obj = obj->tio.object->tio.LTYPE;
                  Dwg_Object *ctrl = &dwg->object[ctrl_id];
                  Dwg_Object_LTYPE_CONTROL *_ctrl
                      = ctrl->tio.object->tio.LTYPE_CONTROL;
                  if (_ctrl->bylayer
                      && obj->handle.value == _ctrl->bylayer->absolute_ref)
                    i--;
                  else if (_ctrl->byblock
                           && obj->handle.value
                                  == _ctrl->byblock->absolute_ref)
                    i--;
                  else if (dwg->header.version > R_2004 &&
                           strNE (_obj->name, "Continuous"))
                    {
                      _obj->text_area_is_present = 0;
                      //or _obj->strings_area = calloc (512, 1);
                    }
                  if (dwg->header.version <= R_2004)
                    _obj->strings_area = calloc (256, 1);
                }
            }
          // next table
          // fixup entries vs num_entries (no NULL entries)
          {
            Dwg_Object *ctrl = &dwg->object[ctrl_id];
            Dwg_Object_BLOCK_CONTROL *_ctrl
              = ctrl->tio.object->tio.BLOCK_CONTROL;
            int at_end = 1;
            for (int j = _ctrl->num_entries - 1; j >= 0; j--)
              {
                BITCODE_H ref = _ctrl->entries[j];
                if (!ref)
                  {
                    if (at_end)
                      {
                        _ctrl->num_entries--;
                        _ctrl->entries = realloc (_ctrl->entries,
                                                  _ctrl->num_entries
                                                      * sizeof (BITCODE_H));
                        LOG_TRACE ("%s.num_entries-- => %d\n", ctrl->name,
                                    _ctrl->num_entries);
                      }
                    else
                      {
                        _ctrl->entries[j] = dwg_add_handleref (dwg, 2, 0, NULL);
                        LOG_TRACE ("%s.entries[%d] = (2.0.0)\n", ctrl->name,
                                   j);
                      }
                  }
                else
                  at_end = 0;
              }
            // leave room for one active entry
            if (_ctrl->num_entries == 1 && !_ctrl->entries[0])
              {
                _ctrl->entries[0] = dwg_add_handleref (dwg, 2, 0, NULL);
                LOG_TRACE ("%s.entries[0] = (2.0.0)\n", ctrl->name);
              }
          }
        }
      DXF_RETURN_ENDSEC (0) // next TABLE or ENDSEC
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_blocks_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char name[80];
  Dxf_Pair *pair = dxf_read_pair (dat);

  name[0] = '\0'; // init
  while (1)       // read next 0 TABLE
    {
      if (pair->code == 0)
        {
          BITCODE_BL i = 0;
          BITCODE_BB entmode = 0;
          while (pair->code == 0 && strNE (pair->value.s, "ENDSEC"))
            {
              Dwg_Object *obj, *blkhdr = NULL;
              BITCODE_BL idx = dwg->num_objects;
              strncpy (name, pair->value.s, 79);
              entity_alias (name);
              pair = new_object (name, pair->value.s, dat, dwg, 0, i++);
              obj = &dwg->object[idx];
              if (obj->type == DWG_TYPE_BLOCK)
                {
                  Dwg_Object_Entity *ent = obj->tio.entity;
                  Dwg_Entity_BLOCK *_obj = obj->tio.entity->tio.BLOCK;
                  i = 0;
                  if (ent->ownerhandle)
                    {
                      if ((blkhdr = dwg_ref_object (dwg, ent->ownerhandle)))
                        {
                          Dwg_Object_BLOCK_HEADER *_hdr
                              = blkhdr->tio.object->tio.BLOCK_HEADER;
                          ent->ownerhandle->obj = NULL; // still dirty
                          _hdr->block_entity = dwg_add_handleref (
                              dwg, 3, obj->handle.value, blkhdr);
                          LOG_TRACE ("BLOCK_HEADER.block_entity = " FORMAT_REF
                                     " [H] (blocks)\n",
                                     ARGS_REF (_hdr->block_entity));
                        }
                    }
                  else
                    blkhdr = NULL;
                  // TODO R2007+
                  if (strEQc (_obj->name, "*Model_Space"))
                    entmode = ent->entmode = 2;
                  else if (strEQc (_obj->name, "*Paper_Space"))
                    entmode = ent->entmode = 1;
                  else
                    entmode = 0;
                }
              else if (obj->type == DWG_TYPE_ENDBLK)
                {
                  obj->tio.entity->entmode = entmode;
                  LOG_TRACE ("%s.entmode = %d [BB] (blocks)\n", obj->name,
                             entmode);
                  entmode = 0;
                  // set BLOCK_HEADER.endblk_entity handle
                  if (blkhdr)
                    {
                      BITCODE_H ref = dwg_add_handleref (
                          dwg, 3, obj->handle.value, blkhdr);
                      LOG_TRACE ("BLOCK_HEADER.endblk_entity = " FORMAT_REF
                                 " [H] (blocks)\n",
                                 ARGS_REF (ref));
                    }
                }
              // normal entity
              else if (obj->supertype == DWG_SUPERTYPE_ENTITY)
                {
                  obj->tio.entity->entmode = entmode;
                  LOG_TRACE ("%s.entmode = %d [BB] (blocks)\n", obj->name,
                             entmode);
                  // blkhdr.entries[] array already done in TABLES section
                  if (blkhdr && dwg->header.version >= R_13
                      && dwg->header.version < R_2004)
                    {
                      Dwg_Object_BLOCK_HEADER *_hdr
                          = blkhdr->tio.object->tio.BLOCK_HEADER;
                      if (i == 1)
                        {
                          _hdr->first_entity = dwg_add_handleref (
                              dwg, 4, obj->handle.value, blkhdr);
                          LOG_TRACE ("BLOCK_HEADER.first_entity = " FORMAT_REF
                                     " [H] (blocks)\n",
                                     ARGS_REF (_hdr->first_entity));
                        }
                      else
                        {
                          _hdr->last_entity = dwg_add_handleref (
                              dwg, 4, obj->handle.value, blkhdr);
                          LOG_TRACE ("BLOCK_HEADER.last_entity = " FORMAT_REF
                                     " [H] (blocks)\n",
                                     ARGS_REF (_hdr->last_entity));
                        }
                    }
                }
            }
          DXF_RETURN_ENDSEC (0) // next BLOCK or ENDSEC
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return 0;
}

void
entity_alias (char *name)
{
  const int len = strlen (name);
  // check aliases (dxfname => name)
  if (strEQc (name, "ACAD_TABLE"))
    strcpy (name, "TABLE");
  else if (strEQc (name, "ACAD_PROXY_ENTITY"))
    strcpy (name, "PROXY_ENTITY");
  else if (strEQc (name, "ACDBPLACEHOLDER"))
    strcpy (name, "PLACEHOLDER");
  else if (strEQc (name, "POLYLINE"))
    strcpy (name, "POLYLINE_2D"); // other POLYLINE_* by flag or subclass?
  else if (strEQc (name, "VERTEX"))
    strcpy (name, "VERTEX_2D"); // other VERTEX_* by flag?
  else if (len == strlen ("PDFUNDERLAY") && strEQc (&name[3], "UNDERLAY"))
    strcpy (name, "UNDERLAY");
  // if (strEQc (name, "BLOCK"))
  //  strcpy (name, "BLOCK_HEADER");
  // else if (strEQc (name, "VERTEX_MESH") || strEQc (name, "VERTEX_PFACE"))
  //  strcpy (name, "VERTEX_3D");
  // else if (strEQc (name, "DIMENSION"))
  //  strcpy (name, "DIMENSION_ANG2LN");   // allocate room for the largest
  // strip a ACAD_ prefix
  else if (memBEGINc (name, "ACAD_") && is_dwg_entity (&name[5]))
    memmove (name, &name[5], len - 4);
  // strip the ACDB prefix
  else if (memBEGINc (name, "ACDB") && is_dwg_entity (&name[4]))
    memmove (name, &name[4], len - 3);
}

static int
dxf_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  char name[80];
  unsigned long mspace = dwg->header_vars.BLOCK_RECORD_MSPACE ?
    dwg->header_vars.BLOCK_RECORD_MSPACE->absolute_ref : 0x1F;
  unsigned long pspace = dwg->header_vars.BLOCK_RECORD_PSPACE ?
    dwg->header_vars.BLOCK_RECORD_PSPACE->absolute_ref : 0UL;

  while (pair->code == 0)
    {
      strncpy (name, pair->value.s, 79);
      entity_alias (name);
      // until 0 ENDSEC
      while (pair->code == 0
             && (is_dwg_entity (name) || strEQc (name, "DIMENSION")))
        {
          pair = new_object (name, pair->value.s, dat, dwg, 0, 0);
          if (pair->code == 0)
            {
              Dwg_Object *obj = &dwg->object[dwg->num_objects - 1];
              Dwg_Object_Entity *ent = obj->tio.entity;
              if (ent->ownerhandle)
                {
                  if (ent->ownerhandle->absolute_ref == mspace)
                    ent->entmode = 2;
                  else if (pspace && ent->ownerhandle->absolute_ref == pspace)
                    ent->entmode = 1;
                }
              else
                ent->entmode = 2;

              strncpy (name, pair->value.s, 79);
              entity_alias (name);
            }
        }
      DXF_RETURN_ENDSEC (0)
      else LOG_WARN ("Unhandled 0 %s (%s)", name, "entities");
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return 0;
}

void
object_alias (char *name)
{
  const int len = strlen (name);
  // check aliases (dxfname => name)
  if (len == strlen ("PDFDEFINITION") && strEQc (&name[3], "DEFINITION"))
    strcpy (name, "UNDERLAYDEFINITION");
  else if (strEQc (name, "ACDB_DYNAMICBLOCKPURGEPREVENTER_VERSION"))
    strcpy (name, "DYNAMICBLOCKPURGEPREVENTER");
  else if (strEQc (name, "PROXY"))
    strcpy (name, "PROXY_OBJECT");
  else if (strEQc (name, "CSACDOCUMENTOPTIONS"))
    strcpy (name, "DOCUMENTOPTIONS");
  // TODO: not sure yet, if all of them can be one object. i.e. MLEADER looks
  // larger
  else if (strEQc (name, "ACDB_LEADEROBJECTCONTEXTDATA_CLASS"))
    strcpy (name, "ANNOTSCALEOBJECTCONTEXTDATA");
  else if (strEQc (name, "ACDB_MLEADEROBJECTCONTEXTDATA_CLASS"))
    strcpy (name, "ANNOTSCALEOBJECTCONTEXTDATA");
  else if (strEQc (name, "ACDB_MTEXTOBJECTCONTEXTDATA_CLASS"))
    strcpy (name, "ANNOTSCALEOBJECTCONTEXTDATA");
  else if (strEQc (name, "ACDB_MTEXTATTRIBUTEOBJECTCONTEXTDATA_CLASS"))
    strcpy (name, "ANNOTSCALEOBJECTCONTEXTDATA");
  else if (strEQc (name, "ACDB_BLKREFOBJECTCONTEXTDATA_CLASS"))
    strcpy (name, "ANNOTSCALEOBJECTCONTEXTDATA");
  else if (strEQc (name, "ACDB_ALDIMOBJECTCONTEXTDATA_CLASS"))
    strcpy (name, "ANNOTSCALEOBJECTCONTEXTDATA");
  else if (strEQc (name, "EXACXREFPANELOBJECT"))
    strcpy (name, "XREFPANELOBJECT");
  // strip ACAD_ prefix
  else if (memBEGINc (name, "ACAD_") && is_dwg_object (&name[5]))
    memmove (name, &name[5], len - 4);
  // strip the ACDB prefix
  else if (memBEGINc (name, "ACDB") && is_dwg_object (&name[4]))
    memmove (name, &name[4], len - 3);
}

static int
dxf_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  char name[80];
  while (1)
    {
      while (pair->code == 0)
        {
          strncpy (name, pair->value.s, 79);
          object_alias (name);
          if (is_dwg_object (name))
            pair = new_object (name, pair->value.s, dat, dwg, 0, 0);
          else
            DXF_RETURN_ENDSEC (0)
          else
          {
            LOG_WARN ("Unhandled 0 %s (%s)", name, "objects");
            break;
          }
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_unknownsection_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char name[80];
  Dxf_Pair *pair = dxf_read_pair (dat);

  // until 0 ENDSEC
  while (1)
    {
      while (pair->code == 0)
        {
          strncpy (name, pair->value.s, 79);
          object_alias (name);
          if (is_dwg_object (name))
            pair = new_object (name, pair->value.s, dat, dwg, 0, 0);
          else
            DXF_RETURN_ENDSEC (0)
          else
          {
            LOG_WARN ("Unhandled 0 %s (%s)", name, "unknownsection");
            break;
          }
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_CHECK_EOF;
    }
  dxf_free_pair (pair);
  return 0;
}

// read to THUMBNAIL dwg->thumbnail, size 90. not entity->preview
static int
dxf_thumbnail_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);
  unsigned written = 0;

  while (1)
    {
      switch (pair->code)
        {
        case 0: // ENDSEC
          return 0;
        case 90:
          dwg->thumbnail.size = pair->value.l; // INT32 => long
          dwg->thumbnail.chain = calloc (dwg->thumbnail.size, 1);
          if (!dwg->thumbnail.chain)
            {
              LOG_ERROR ("Out of memory");
              return DWG_ERR_OUTOFMEM;
            }
          LOG_TRACE ("PREVIEW.size = %ld\n", dwg->thumbnail.size);
          break;
        case 310:
          {
            unsigned len = strlen (pair->value.s);
            unsigned blen = len / 2;
            const char *pos = pair->value.s;
            unsigned char *s = &dwg->thumbnail.chain[written];
            if (blen + written > dwg->thumbnail.size)
              {
                dxf_free_pair (pair);
                LOG_ERROR ("PREVIEW.size overflow: %u + written %u > "
                           "size: %lu",
                           blen, written, dwg->thumbnail.size);
                return 1;
              }
            for (unsigned i = 0; i < blen; i++)
              {
                sscanf (pos, "%2hhX", &s[i]);
                pos += 2;
              }
            written += blen;
            LOG_TRACE ("PREVIEW.chain += %u (%u/%lu)\n", blen, written,
                       dwg->thumbnail.size);
          }
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                     "THUMBNAILIMAGE");
          break;
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  dxf_free_pair (pair);
  return 0;
}

void
resolve_postponed_header_refs (Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  uint32_t i;
  LOG_TRACE ("resolve %d postponed header ref names\n", header_hdls->nitems);
  for (i = 0; i < header_hdls->nitems; i++)
    {
      char *field = header_hdls->items[i].field;
      Dxf_Pair p = { 0, VT_STRING };
      BITCODE_H hdl = NULL;
      p.value.s = header_hdls->items[i].name;
      if (!p.value.s || !*p.value.s)
        {
          LOG_WARN ("HEADER.%s empty dxf:%d", field, (int)p.code);
          continue;
        }
      p.code = header_hdls->items[i].code;
      if (strEQc (field, "DIMSTYLE"))
        p.code = 3;
      else if (strstr (field, "UCS"))
        p.code = 345;
      hdl = find_tablehandle (dwg, &p);
      if (hdl)
        {
          if (hdl->handleref.code != 5)
            hdl = dwg_add_handleref (dwg, 5, hdl->absolute_ref, NULL);
          dwg_dynapi_header_set_value (dwg, field, &hdl, 1);
          LOG_TRACE ("HEADER.%s %s => " FORMAT_REF " [H %d]\n", field,
                     p.value.s, ARGS_REF (hdl), (int)p.code);
        }
      else if (strstr (field, "CMLSTYLE"))
        {
          hdl = dwg_find_tablehandle (dwg, p.value.s, "MLINESTYLE");
          if (hdl)
            {
              if (hdl->handleref.code != 5)
                hdl = dwg_add_handleref (dwg, 5, hdl->absolute_ref, NULL);
              dwg_dynapi_header_set_value (dwg, field, &hdl, 1);
              LOG_TRACE ("HEADER.%s %s => " FORMAT_REF " [H %d]\n", field,
                         p.value.s, ARGS_REF (hdl), (int)p.code)
            }
          else
            LOG_WARN ("Unknown HEADER.%s %s dxf:%d", field,
                      p.value.s, (int)p.code)
        }
      else
        LOG_WARN ("Unknown HEADER.%s %s dxf:%d", field,
                  p.value.s, (int)p.code)
    }
}

#define CHECK_DICTIONARY_HDR(name)                                            \
  if (!vars->DICTIONARY_##name)                                               \
    {                                                                         \
      vars->DICTIONARY_##name = dwg_find_dictionary (dwg, #name);             \
      if (vars->DICTIONARY_##name)                                            \
        LOG_TRACE ("HEADER.DICTIONARY_" #name " = " FORMAT_REF "\n",          \
                   ARGS_REF (vars->DICTIONARY_##name))                        \
      else                                                                    \
        if ((vars->DICTIONARY_##name = dwg_find_dictionary (dwg, "ACAD_"#name))) \
          LOG_TRACE ("HEADER.DICTIONARY_" #name " = " FORMAT_REF "\n",        \
                     ARGS_REF (vars->DICTIONARY_##name))                      \
    }

void
resolve_header_dicts (Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;

  if (!vars->DICTIONARY_NAMED_OBJECT)
    vars->DICTIONARY_NAMED_OBJECT = dwg_add_handleref (dwg, 3, 0xC, NULL);
  // only possible after OBJECTS
  CHECK_DICTIONARY_HDR (ACAD_GROUP)
  CHECK_DICTIONARY_HDR (ACAD_MLINESTYLE)
  if (dwg->header.version >= R_2000)
    {
      CHECK_DICTIONARY_HDR (LAYOUT)
      CHECK_DICTIONARY_HDR (PLOTSETTINGS)
      CHECK_DICTIONARY_HDR (PLOTSTYLENAME)
    }
  if (dwg->header.version >= R_2004)
    {
      CHECK_DICTIONARY_HDR (MATERIAL)
      CHECK_DICTIONARY_HDR (COLOR)
    }
  if (dwg->header.version >= R_2007)
    {
      CHECK_DICTIONARY_HDR (VISUALSTYLE)
    }
}
#undef CHECK_DICTIONARY_HDR

void
resolve_postponed_eed_refs (Dwg_Data *restrict dwg)
{
  LOG_TRACE ("resolve %d postponed eed APPID refs\n", eed_hdls->nitems);
  for (uint32_t i = 0; i < eed_hdls->nitems; i++)
    {
      char *name = eed_hdls->items[i].name;
      BITCODE_H ref
          = dwg_find_tablehandle (dwg, eed_hdls->items[i].name, "APPID");
      if (ref)
        {
          // copy to eed[i].handle. need: objid + eed[i]
          BITCODE_RL objid;
          Dwg_Eed *eed;
          int j = (int)eed_hdls->items[i].code;
          sscanf (eed_hdls->items[i].field, "%d", &objid);
          eed = dwg->object[objid].tio.object->eed;
          memcpy (&eed[j].handle, &ref->handleref, sizeof (Dwg_Handle));
          eed[j].handle.code = 5;
          LOG_TRACE ("postponed eed[%d].handle for APPID.%s => " FORMAT_H
                     " [H]\n",
                     j, name, ARGS_H (eed[j].handle));
        }
      else
        LOG_WARN ("Unknown eed[].handle for APPID.%s", name)
    }
}

int
dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  // const int minimal = dwg->opts & 0x10;
  Dxf_Pair *pair;

  loglevel = dwg->opts & 0xf;
  num_dxf_objs = 0;
  size_dxf_objs = 1000;
  dxf_objs = malloc (1000 * sizeof (Dxf_Objs));
  if (!dwg->object_map)
    dwg->object_map = hash_new (dat->size / 1000);
  // cannot rely on ref->obj during realloc's
  dwg->dirty_refs = 1;

  header_hdls = calloc (1, 8 + 16 * sizeof (struct array_hdl));
  header_hdls->size = 16;
  eed_hdls = calloc (1, 8 + 16 * sizeof (struct array_hdl));
  eed_hdls->size = 16;

  // start with the BLOCK_HEADER at objid 0
  if (!dwg->num_objects)
    {
      Dwg_Object *obj;
      Dwg_Object_BLOCK_HEADER *_obj;
      char *dxfname = (char *)"BLOCK_HEADER";
      char *name = (char *)"*Model_Space";
      NEW_OBJECT (dwg, obj);
      ADD_OBJECT (BLOCK_HEADER);
      // dwg->header.version here still unknown. <r2000: 0x17
      // later fixed up when reading the BLOCK_HEADER.name
      if (dwg->header.version >= R_2007)
        _obj->name = (BITCODE_T)bit_utf8_to_TU (name);
      else
        _obj->name = strdup (name);
      obj->tio.object->xdic_missing_flag = 1;
      _obj->xrefref = 1;
      dwg_add_handle (&obj->handle, 0, 0x1F, obj);
      obj->tio.object->ownerhandle = dwg_add_handleref (dwg, 4, 1, NULL);
    }

  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      pair = dxf_expect_code (dat, pair, 0);
      DXF_BREAK_EOF;
      if (pair->type == VT_STRING && strEQc (pair->value.s, "SECTION"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          pair = dxf_expect_code (dat, pair, 2);
          DXF_BREAK_EOF;
          if (strEQc (pair->value.s, "HEADER"))
            {
              dxf_free_pair (pair);
              dxf_header_read (dat, dwg);
              dxf_fixup_header (dwg);
              // skip minimal DXF
              /*
              if (!dwg->header_vars.DIMPOST) // T in all versions
                {
                  LOG_ERROR ("Unsupported minimal DXF");
                  return DWG_ERR_INVALIDDWG;
                }
              */
            }
          else if (strEQc (pair->value.s, "CLASSES"))
            {
              dxf_free_pair (pair);
              dxf_classes_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "TABLES"))
            {
              BITCODE_H hdl;
              dxf_free_pair (pair);
              dxf_tables_read (dat, dwg);

              resolve_postponed_header_refs (dwg);
              resolve_postponed_eed_refs (dwg);

              // should not happen
              if (!dwg->header_vars.LTYPE_BYLAYER
                  && (hdl = dwg_find_tablehandle (dwg, (char *)"ByLayer",
                                                  "LTYPE")))
                dwg->header_vars.LTYPE_BYLAYER
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              // should not happen
              if (!dwg->header_vars.LTYPE_BYBLOCK
                  && (hdl = dwg_find_tablehandle (dwg, (char *)"ByBlock",
                                                  "LTYPE")))
                dwg->header_vars.LTYPE_BYBLOCK
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              // but this is needed
              if (!dwg->header_vars.LTYPE_CONTINUOUS
                  && (hdl = dwg_find_tablehandle (dwg, (char *)"Continuous",
                                                  "LTYPE")))
                dwg->header_vars.LTYPE_CONTINUOUS
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
            }
          else if (strEQc (pair->value.s, "BLOCKS"))
            {
              BITCODE_H hdl;
              dxf_free_pair (pair);
              dxf_blocks_read (dat, dwg);

              // resolve_postponed_header_refs (dwg);
              if (!dwg->header_vars.BLOCK_RECORD_PSPACE
                  && (hdl = dwg_find_tablehandle (dwg, (char *)"*Paper_Space",
                                                  "BLOCK")))
                dwg->header_vars.BLOCK_RECORD_PSPACE
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              if (!dwg->header_vars.BLOCK_RECORD_MSPACE
                  && (hdl = dwg_find_tablehandle (dwg, (char *)"*Model_Space",
                                                  "BLOCK")))
                dwg->header_vars.BLOCK_RECORD_MSPACE
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
            }
          else if (strEQc (pair->value.s, "ENTITIES"))
            {
              dxf_free_pair (pair);
              dxf_entities_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "OBJECTS"))
            {
              dxf_free_pair (pair);
              dxf_objects_read (dat, dwg);
              resolve_header_dicts (dwg);
            }
          else if (strEQc (pair->value.s, "THUMBNAILIMAGE"))
            {
              dxf_free_pair (pair);
              dxf_thumbnail_read (dat, dwg);
            }
          else // if (strEQc (pair->value.s, "ACDSDATA"))
            {
              LOG_WARN ("SECTION %s ignored for now", pair->value.s);
              dxf_free_pair (pair);
              dxf_unknownsection_read (dat, dwg);
            }
        }
    }
  resolve_postponed_header_refs (dwg);
  LOG_HANDLE ("Resolving pointers from ObjectRef vector:\n");
  dwg_resolve_objectrefs_silent (dwg);
  dwg->dirty_refs = 0;
  free_array_hdls (header_hdls);
  free_array_hdls (eed_hdls);
  dwg->opts |= 0x2f; // from DXF
  LOG_TRACE ("import from DXF\n");
  return dwg->num_objects ? 1 : 0;
}

#undef IS_INDXF

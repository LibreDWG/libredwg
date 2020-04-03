/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2020 Free Software Foundation, Inc.                   */
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
#include <errno.h>
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
#include "classes.h"
#include "free.h"

#ifndef _DWG_API_H_
Dwg_Object *dwg_obj_generic_to_object (const void *restrict obj,
                                       int *restrict error);
#endif

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "logging.h"

// from dwg.c
BITCODE_H
dwg_find_tablehandle_silent (Dwg_Data *restrict dwg, const char *restrict name,
                             const char *restrict table);

/* the current version per spec block */
static unsigned int cur_ver = 0;
static char buf[4096];
static long start, end; // stream offsets
static array_hdls *header_hdls = NULL;
static array_hdls *eed_hdls = NULL;
static array_hdls *obj_hdls = NULL;

static long num_dxf_objs;  // how many elements are added
static long size_dxf_objs; // how many elements are allocated
static Dxf_Objs *dxf_objs;

static void *
xcalloc (size_t n, size_t s)
{
  void *p;
  if ((n * s) > INT32_MAX)
    goto err;
  p = calloc (n, s);
  if (!p)
    {
    err:
      LOG_ERROR ("Out of memory with calloc %ld * %ld\n", (long)n,
                 (long)s);
      return NULL;
    }
  return p;
}

static inline void
dxf_skip_ws (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    return;
  // clang-format off
  for (; (!dat->chain[dat->byte] ||
          dat->chain[dat->byte] == ' ' ||
          dat->chain[dat->byte] == '\t' ||
          dat->chain[dat->byte] == '\r');
       dat->byte++)
    if (dat->byte >= dat->size)
      return;
  // clang-format on
}

static BITCODE_RS
dxf_read_rs (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RS (dat);
    }
  else
    {
      char *endptr;
      long num;
      if (dat->byte + 2 >= dat->size)
        return (BITCODE_RS)-1;
      num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
      if (endptr)
        dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
      if (errno == ERANGE)
        return (BITCODE_RS)num;
      if (dat->byte + 1 >= dat->size)
        return (BITCODE_RS)num;
      if (dat->chain[dat->byte] == '\r')
        dat->byte++;
      if (dat->chain[dat->byte] == '\n')
        dat->byte++;
      if (num > 65534)
        LOG_ERROR ("%s: RS overflow %ld (at %lu)", __FUNCTION__, num, dat->byte);
      return (BITCODE_RS)num;
    }
}

static BITCODE_RL
dxf_read_rl (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RL (dat);
    }
  else
    {
      char *endptr;
      long num;
      if (dat->byte + 2 >= dat->size)
        return (BITCODE_RL)-1;
      num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
      if (endptr)
        dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
      if (errno == ERANGE)
        return (BITCODE_RL)num;
      if (dat->byte + 1 >= dat->size)
        return 0;
      if (dat->chain[dat->byte] == '\r')
        dat->byte++;
      if (dat->chain[dat->byte] == '\n')
        dat->byte++;
      if (num > INT_MAX)
        LOG_ERROR ("%s: RL overflow %ld (at %lu)", __FUNCTION__, num, dat->byte);
      return (BITCODE_RL)num;
    }
}

static BITCODE_RLL
dxf_read_rll (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RLL (dat);
    }
  else
    {
      char *endptr;
      BITCODE_RLL num;
      if (dat->byte + 2 >= dat->size)
        return (BITCODE_RLL)-1;
      num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
      if (endptr)
        dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
      if (errno == ERANGE)
        return (BITCODE_RLL)num;
      if (dat->byte + 1 >= dat->size)
        return 0L;
      if (dat->chain[dat->byte] == '\r')
        dat->byte++;
      if (dat->chain[dat->byte] == '\n')
        dat->byte++;
      if ((unsigned long)num > LONG_MAX) // or wrap to negative
        LOG_ERROR ("%s: long overflow %ld (at %lu)", __FUNCTION__, (long)num, dat->byte);
      return num;
    }
}

static BITCODE_RD
dxf_read_rd (Bit_Chain *dat)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      return bit_read_RD (dat);
    }
  else
    {
      char *str, *endptr;
      BITCODE_RD num;
      dxf_skip_ws (dat);
      if (dat->byte + 2 >= dat->size)
        return (double)NAN;
      str = (char *)&dat->chain[dat->byte];
      num = strtod (str, &endptr);
      if (endptr)
        dat->byte += endptr - str;
      if (errno == ERANGE)
        return (double)NAN;
      return num;
    }
}

// TODO: TV only, no unicode. We encode only r2000 so far.
static void
dxf_read_string (Bit_Chain *dat, char **string)
{
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (is_binary)
    {
      int len = dxf_read_rs (dat);
      if (dat->byte + len >= dat->size)
        return;
      if (!string)
        {
          if (len > 4096)
            return;
          memcpy (buf, &dat->chain[dat->byte], len);
          buf[len] = '\0';
          return; // ignore, just advanced dat
        }
      if (!*string)
        *string = malloc (len + 1);
      else
        *string = realloc (*string, len + 1);
      memcpy (*string, &dat->chain[dat->byte], len);
    }
  else
    {
      int i;
      if (dat->byte >= dat->size)
        return;
      dxf_skip_ws (dat);
      for (i = 0;
           dat->byte < dat->size && dat->chain[dat->byte] != '\n' && i < 4096;
           dat->byte++)
        {
          buf[i++] = dat->chain[dat->byte];
        }
      if (dat->byte >= dat->size || i >= 4096)
        return;
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
  pair = NULL;
}

static Dxf_Pair *ATTRIBUTE_MALLOC
dxf_read_pair (Bit_Chain *dat)
{
  Dxf_Pair *pair = xcalloc (1, sizeof (Dxf_Pair));
  const int is_binary = dat->opts & DWG_OPTS_DXFB;
  if (!pair)
    return NULL;
  if (dat->size - dat->byte < 6) // at least 0\nEOF\n
    {
    err:
      LOG_ERROR ("Unexpected DXF end-of-file");
      free (pair);
      return NULL;
    }
  pair->code = (short)dxf_read_rs (dat);
  if (dat->size - dat->byte < 4) // at least EOF\n
    goto err;
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
      pair->value.i = dxf_read_rs (dat);
      LOG_TRACE ("  dxf (%d, %d)\n", (int)pair->code, pair->value.i);
      break;
    case VT_INT32:
      pair->value.l = dxf_read_rl (dat);
      LOG_TRACE ("  dxf (%d, %ld)\n", (int)pair->code, pair->value.l);
      break;
    case VT_INT64:
      pair->value.bll = dxf_read_rll (dat);
      LOG_TRACE ("  dxf (%d, " FORMAT_BLL ")\n", (int)pair->code,
                 pair->value.bll);
      break;
    case VT_REAL:
    case VT_POINT3D:
      dxf_skip_ws (dat);
      pair->value.d = dxf_read_rd (dat);
      LOG_TRACE ("  dxf (%d, %f)\n", pair->code, pair->value.d);
      break;
    case VT_BINARY:
      dxf_read_string (dat, &pair->value.s);
      LOG_TRACE ("  dxf (%d, %s)\n", (int)pair->code, pair->value.s);
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      if (is_binary)
        {
          pair->value.u = dxf_read_rl (dat);
        }
      else
        {
          dxf_read_string (dat, NULL);
          sscanf (buf, "%X", &pair->value.u);
        }
      LOG_TRACE ("  dxf (%d, %X)\n", (int)pair->code, pair->value.u);
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid DXF group code: %d", pair->code);
      dxf_free_pair (pair);
      return NULL;
    }
  return pair;
}

#define DXF_CHECK_EOF                                                         \
  if (dat->byte >= dat->size                                                  \
      || (pair == NULL)                                                       \
      || (pair->code == 0 && !pair->value.s)                                  \
      || (pair->code == 0 && strEQc (pair->value.s, "EOF")))                  \
    {                                                                         \
      if (pair)                                                               \
        dxf_free_pair (pair);                                                 \
      pair = NULL;                                                            \
      return 1;                                                               \
    }
#define DXF_RETURN_EOF(what)                                                  \
  if (dat->byte >= dat->size                                                  \
      || (pair == NULL)                                                       \
      || (pair->code == 0 && !pair->value.s)                                  \
      || (pair->code == 0 && strEQc (pair->value.s, "EOF")))                  \
    {                                                                         \
      if (pair)                                                               \
        dxf_free_pair (pair);                                                 \
      pair = NULL;                                                            \
      return what;                                                            \
    }
#define DXF_BREAK_EOF                                                         \
  if (dat->byte >= dat->size                                                  \
      || (pair == NULL)                                                       \
      || (pair->code == 0 && !pair->value.s)                                  \
      || (pair->code == 0 && strEQc (pair->value.s, "EOF")))                  \
  break

static Dxf_Pair *
dxf_skip_comment (Bit_Chain *dat, Dxf_Pair *pair)
{
  while (pair != NULL && pair->code == 999)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_RETURN_EOF (pair);
    }
  return pair;
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
array_push (array_hdls *restrict hdls, const char *restrict field,
            const char *restrict name, const int code)
{
  uint32_t i = hdls->nitems;
  if (i >= hdls->size)
    {
      hdls->size += 16;
      hdls = realloc (hdls, 8 + (hdls->size * sizeof (struct array_hdl)));
      if (!hdls)
        {
          LOG_ERROR ("Out of memory");
          return NULL;
        }
      //memset (hdls, 0, 8 + (hdls->size * sizeof (struct array_hdl));
    }
  hdls->nitems = i + 1;
  hdls->items[i].field = strdup (field);
  hdls->items[i].name = strdup (name);
  hdls->items[i].code = code;
  return hdls;
}

array_hdls *
new_array_hdls (int size)
{
  array_hdls *hdls = xcalloc (1, 8 + size * sizeof (struct array_hdl));
  if (!hdls)
    return NULL;
  hdls->size = size;
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
  if (pair != NULL                                                            \
      && (dat->byte >= dat->size                                              \
          || (pair->code == 0 && !pair->value.s)                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))          \
    return 0
#define DXF_BREAK_ENDSEC                                                      \
  if (pair != NULL                                                            \
      && (dat->byte >= dat->size                                              \
          || (pair->code == 0 && !pair->value.s)                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))          \
  break
#define DXF_RETURN_ENDSEC(what)                                               \
  if (pair != NULL                                                            \
      && (dat->byte >= dat->size                                              \
          || (pair->code == 0 && !pair->value.s)                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))          \
    {                                                                         \
      dxf_free_pair (pair);                                                   \
      return what;                                                            \
    }

static Dxf_Pair *
dxf_expect_code (Bit_Chain *restrict dat, Dxf_Pair *restrict pair, int code)
{
  while (pair != NULL && pair->code != code)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      pair = dxf_skip_comment (dat, pair);
      DXF_RETURN_EOF (pair);
      if (pair && pair->code != code)
        {
          LOG_ERROR ("Expecting DXF code %d, got %d (at %lu)", code,
                     pair->code, dat->byte);
        }
    }
  return pair;
}

static int
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

/* convert to flag */
BITCODE_RC
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

static int
dxf_header_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *_obj = &dwg->header_vars;
  Dwg_Object *obj = NULL;
  // const int minimal = dwg->opts & DWG_OPTS_MINIMAL;
  int is_utf = 1;
  int i = 0;
  Dxf_Pair *pair;

  // defaults, not often found in a DXF
  _obj->ISOLINES = 4;
  _obj->TEXTQLTY = 50;
  _obj->FACETRES = 0.5;

  // here SECTION (HEADER) was already consumed
  // read the first group 9, $field pair
  pair = dxf_read_pair (dat);

  while (pair != NULL && pair->code == 9 && pair->value.s)
    {
      char field[80];
      strncpy (field, pair->value.s, 79);
      field[79] = '\0';
      i = 0;

      // now read the code, value pair. for points it may be multiple (index i)
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (!pair)
        {
          pair = dxf_read_pair (dat);
          if (!pair)
            return 1;
        }
      DXF_BREAK_ENDSEC;
    next_hdrvalue:
      if (pair->code == 1 && strEQc (field, "$ACADVER") && pair->value.s != NULL)
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
                  if (is_utf && dwg->num_objects && dwg->object[0].fixedtype == DWG_TYPE_BLOCK_HEADER)
                    {
                      Dwg_Object_BLOCK_HEADER *_o = dwg->object[0].tio.object->tio.BLOCK_HEADER;
                      free (_o->name);
                      _o->name = (char*)bit_utf8_to_TU ((char*)"*Model_Space");
                    }
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
              if (pair->code == 40 && strEQc (field, "$3DDWFPREC"))
                {
                  LOG_TRACE ("HEADER.%s [%s %d]\n", &field[1], "BD", pair->code);
                  dwg->header_vars._3DDWFPREC = pair->value.d;
                }

#define SUMMARY_T(name)                                                       \
  (pair->code == 1 && strEQc (field, "$" #name) && pair->value.s != NULL)     \
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
                  LOG_TRACE ("HEADER.%s [%s %d][%d] = %f\n", &field[1], f->type,
                             pair->code, i, pair->value.d);
                  dwg_dynapi_header_set_value (dwg, &field[1], &pt, 1);
                  i++;
                }
            }
          else if (pair->type == VT_STRING && strEQc (f->type, "H"))
            {
              char *key, *str;
              if (pair->value.s && strlen (pair->value.s))
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
                  dwg_dynapi_header_set_value (dwg, &field[1], &hdl, 1);
                }
            }
          else if (strEQc (f->type, "H"))
            {
              BITCODE_H hdl;
              hdl = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("HEADER.%s %X [H %d]\n", &field[1], pair->value.u,
                         pair->code);
              dwg_dynapi_header_set_value (dwg, &field[1], &hdl, 1);
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
              double ms;
              date.value = pair->value.d;
              date.days = (BITCODE_BL)trunc (pair->value.d);
              date.ms = (BITCODE_BL)(86400.0 * (date.value - date.days));
              LOG_TRACE ("HEADER.%s %.09f (%u, %u) [TIMEBLL %d]\n", &field[1],
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
              dwg_dynapi_header_set_value (dwg, &field[1], &pair->value, 1);
            }
        }
      else
        {
          LOG_ERROR ("skipping HEADER: 9 %s, missing the $", field);
        }

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (!pair)
        {
          pair = dxf_read_pair (dat);
          if (!pair)
            return 1;
        }
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

  if (vars->HANDSEED)
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

  while (pair)
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
    restart:
      klass = &dwg->dwg_class[i];
      memset (klass, 0, sizeof (Dwg_Class));
      if (pair != NULL && pair->code == 0 && pair->value.s
          && strEQc (pair->value.s, "CLASS"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            return 1;
        }
      else
        {
          LOG_ERROR ("2 CLASSES must be followed by 0 CLASS")
          pair = dxf_read_pair (dat);
        }
      klass->number = 500 + i;
      while (pair != NULL && pair->code != 0)
        { // read until next 0 CLASS
          switch (pair->code)
            {
            case 1:
              if (pair->value.s)
                {
                  const char *n = strEQc (pair->value.s, "ACDBDATATABLE")
                    ? "DATATABLE" : pair->value.s;
                  if (klass->dxfname)
                    {
                      LOG_ERROR ("Group 1 for CLASS %s already read", klass->dxfname);
                      break;
                    }
                  STRADD (klass->dxfname, n);
                  LOG_TRACE ("CLASS[%d].dxfname = %s [TV 1]\n", i, n);
                }
              break;
            case 2:
              if (klass->cppname)
                {
                  LOG_ERROR ("Group 2 for CLASS %s already read", klass->dxfname);
                  break;
                }
              if (pair->value.s)
                {
                  STRADD (klass->cppname, pair->value.s);
                }
              LOG_TRACE ("CLASS[%d].cppname = %s [TV 2]\n", i, pair->value.s);
              break;
            case 3:
              if (klass->appname)
                {
                  LOG_ERROR ("Group 3 for CLASS %s already read", klass->dxfname);
                  break;
                }
              if (pair->value.s)
                {
                  STRADD (klass->appname, pair->value.s);
                }
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
            case 280: // ie was_proxy
              klass->is_zombie = (BITCODE_B)pair->value.i;
              LOG_TRACE ("CLASS[%d].is_zombie = %d [B 280]\n", i,
                         pair->value.i);
              break;
            case 281: // ie is_entity
              // 1f2 for entities, 1f3 for objects
              klass->item_class_id = pair->value.i ? 0x1f2 : 0x1f3;
              LOG_TRACE ("CLASS[%d].item_class_id = 0x%x [BSx 281] (%s)\n", i,
                         klass->item_class_id, pair->value.i ? "is_entity" : "is_object");
              break;
            default:
              LOG_WARN ("Unknown DXF code for class[%d].%d", i, pair->code);
              break;
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            {
              pair = dxf_read_pair (dat);
              if (!pair)
                return 1;
            }
        }
      if (klass->dxfname && klass->cppname && klass->appname && klass->item_class_id)
        {
          dwg->num_classes++;
          DXF_RETURN_ENDSEC (0)
        }
      else
        {
          DXF_RETURN_ENDSEC (0);
          goto restart; // without alloc
        }
    }
  dxf_free_pair (pair);
  return 0;
}

static void
add_eed (Dwg_Object *restrict obj, const char *restrict name,
         Dxf_Pair *restrict pair)
{
  int code, size, j;
  int i, prev = 0;
  Dwg_Eed *eed;
  Dwg_Data *dwg = obj->parent;

  i = obj->tio.object->num_eed; // same layout for Object and Entity
  eed = obj->tio.object->eed;

  // new eed pair
  if (pair->code < 1020 || pair->code > 1035) // no followup y and z pairs
    {
      if (i || eed)
        {
          eed = (Dwg_Eed *)realloc (eed, (i + 1) * sizeof (Dwg_Eed));
          if (!eed)
            {
              LOG_ERROR ("Out of memory");
              dwg_free_eed (obj);
              return;
            }
          if (i)
            memset (&eed[i], 0, sizeof (Dwg_Eed));
        }
      else
        {
          eed = (Dwg_Eed *)xcalloc (1, sizeof (Dwg_Eed));
          if (!eed)
            {
              LOG_ERROR ("Out of memory");
              dwg_free_eed (obj);
              return;
            }
        }
      obj->tio.object->eed = eed;
      obj->tio.object->num_eed++;
    }
  else // add to old eed
    i--;
  // search for previous size index
  for (j = i; j >= 0; j--)
    if (eed[j].handle.code)
      prev = j;
  assert (prev >= 0 && prev <= i);
  code = pair->code - 1000; // 1000
  assert (code >= 0 && code < 100);
  LOG_TRACE ("EED[%d] code: %d ", i, code);
  switch (code)
    {
    case 0:
      {
        int len = strlen (pair->value.s);
        if (dwg->header.version < R_2007)
          {
            /* code [RC] + len [RC] + cp [RS] + str[len] */
            size = 1 + 1 + 2 + len;
            eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size+1);
            if (!eed[i].data)
              {
                LOG_ERROR ("Out of memory");
                dwg_free_eed (obj);
                return;
              }
            eed[i].data->code = code; // 1000
            eed[i].data->u.eed_0.length = len;
            eed[i].data->u.eed_0.codepage = dwg->header.codepage;
            if (len && len < 256)
              {
                LOG_TRACE ("string: \"%s\" [TV %d]\n", pair->value.s, size-1);
                memcpy (eed[i].data->u.eed_0.string, pair->value.s, len + 1);
              }
          }
        else
          {
            /* code [RC] + length [RS] + 2*len [TU] */
            size = 1 + 2 + (len * 2);
            eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size + 2);
            if (!eed[i].data)
              {
                LOG_ERROR ("Out of memory");
                dwg_free_eed (obj);
                return;
              }
            eed[i].data->code = code;
            eed[i].data->u.eed_0_r2007.length = len;
            eed[i].data->u.eed_0.codepage = obj->parent->header.codepage; /* UTF-8 */
            if (len && len < 32767)
              {
                BITCODE_TU tu = bit_utf8_to_TU (pair->value.s);
                LOG_TRACE ("wstring: \"%s\" [TU %d]\n", pair->value.s, size - 1);
                memcpy (eed[i].data->u.eed_0_r2007.string, tu, 2 * (len + 1));
                free (tu);
              }
          }
        eed[i].size += size;
      }
      break;
    // 1001 is the APPID handle, not part of size nor data
    case 1:
      obj->tio.object->num_eed--;
      prev = i;
      if (strEQc (pair->value.s, "ACAD"))
        {
          dwg_add_handle (&eed[i].handle, 5, 0x12, NULL);
          LOG_TRACE ("handle: 5.1.12 [H] for APPID.%s\n", pair->value.s);
        }
      else
        {
          // search name in APPID table (if already added)
          BITCODE_H hdl;
          hdl = dwg_find_tablehandle_silent (dwg, pair->value.s, "APPID");
          if (hdl)
            {
              memcpy (&eed[i].handle, &hdl->handleref, sizeof (Dwg_Handle));
              eed[i].handle.code = 5;
              LOG_TRACE ("handle: %lX [H] for APPID.%s\n", hdl->absolute_ref,
                         pair->value.s);
            }
          // needs to be postponed, because we don't have the tables yet
          else
            {
              char idx[12];
              snprintf (idx, 12, "%d", obj->index);
              eed[i].handle.code = 5;
              eed_hdls = array_push (eed_hdls, idx, pair->value.s, i);
              LOG_TRACE ("handle: ? [H} for APPID.%s later\n",
                          pair->value.s);
            }
        }
      break;
    case 2:
      /* code [RC] + byte [RC] */
      size = 1 + 1;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code; // 1002
      eed[i].data->u.eed_2.byte = (BITCODE_RC)pair->value.i;
      LOG_TRACE ("byte: %d\n", pair->value.i);
      eed[i].size += size;
      break;
    case 4:
      {
        // BINARY
        const char *pos = pair->value.s;
        int len = strlen (pos);
        /* code [RC] + len+0 + length [RC] */
        size = 1 + len / 2 + 1 + 1;
        eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
        if (!eed[i].data)
          {
            LOG_ERROR ("Out of memory");
            dwg_free_eed (obj);
            return;
          }
        eed[i].data->code = code; // 1004
        eed[i].data->u.eed_4.length = len / 2;
        LOG_TRACE ("binary[%d]: ", len);
        for (j = 0; j < len / 2; j++)
          {
            sscanf (pos, "%2hhX", &eed[i].data->u.eed_4.data[j]);
            LOG_TRACE ("%02X", (unsigned char)*pos);
            pos += 2;
          }
        LOG_TRACE (" [TF]\n");
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
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
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
      if (!eed[i].data || eed[i].data->code != code - 10)
        {
          LOG_ERROR ("Wrong EED DXF code %d, expected %d", code + 1000,
                     eed[i].data ? eed[i].data->code + 1010 : 1020)
          return;
        }
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
      if (!eed[i].data || eed[i].data->code != code - 20)
        {
          LOG_ERROR ("Wrong EED DXF code %d, expected %d", code + 1000,
                     eed[i].data ? eed[i].data->code + 1020 : 1030)
          return;
        }
      eed[i].data->u.eed_10.point.z = pair->value.d;
      LOG_TRACE ("3dpoint: (%f,%f,%f)\n", eed[i].data->u.eed_10.point.x,
                  eed[i].data->u.eed_10.point.y, pair->value.d);
      break;
    case 40:
    case 41:
    case 42:
      /* code [RC] + 3*RD */
      size = 1 + 8;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_40.real = pair->value.d;
      LOG_TRACE ("real: %f\n", pair->value.d);
      eed[i].size += size;
      break;
    case 70:
      /* code [RC] + RS */
      size = 1 + 2;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_70.rs = pair->value.i;
      LOG_TRACE ("short: %d\n", pair->value.i);
      eed[i].size += size;
      break;
    case 71:
      /* code [RC] + RL */
      size = 1 + 4;
      eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
      if (!eed[i].data)
        {
          LOG_ERROR ("Out of memory");
          dwg_free_eed (obj);
          return;
        }
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_71.rl = pair->value.l;
      LOG_TRACE ("long: %ld\n", pair->value.l);
      eed[i].size += size;
      break;
    case 5:
      {
        // HANDLE (absref)
        const char *pos = pair->value.s;
        unsigned long l = 0;
        /* code [RC] + RLL */
        size = 1 + 8;
        eed[i].data = (Dwg_Eed_Data *)xcalloc (1, size);
        if (!eed[i].data)
          {
            LOG_ERROR ("Out of memory");
            dwg_free_eed (obj);
            return;
          }
        eed[i].data->code = code; // 1005
        sscanf (pos, "%lX", &l);
        eed[i].data->u.eed_5.entity = (BITCODE_RLL)l;
        LOG_TRACE ("entity: %lX [RLL]\n", l);
        eed[i].size += size;
        break;
      }
    default:
      LOG_ERROR ("Not yet implemented EED[%d] code %d", i, pair->code);
    }
  // new size block or not?
  if (!eed[i].handle.code)
    {
      // add to prev. size
      if (i != prev)
        {
          eed[prev].size += eed[i].size;
          eed[i].size = 0;
        }
      LOG_TRACE ("EED[%d] size: %d\n", prev, eed[prev].size);
    }
  else if (eed[i].size)
    LOG_TRACE ("EED[%d] size: %d\n", i, eed[i].size);
  return;
}

int
is_table_name (const char *restrict name)
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
add_LTYPE_dashes (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                  Dxf_Pair *restrict pair)
{
  Dwg_Object_LTYPE *_o = obj->tio.object->tio.LTYPE;
  Dwg_Data *dwg = obj->parent;
  int num_dashes = (int)_o->num_dashes;
  _o->dashes = xcalloc (_o->num_dashes, sizeof (Dwg_LTYPE_dash));
  if (!_o->dashes)
    {
      _o->num_dashes = 0;
      return NULL;
    }
  for (int j = -1; j < num_dashes;)
    {
      if (!pair || pair->code == 0)
        return pair;
      else if (pair->code == 49)
        {
          j++;
          assert (j < num_dashes);
          _o->dashes[j].length = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].length = %f [BD 49]\n", j,
                     pair->value.d);
          PRE (R_13)
            _o->pattern_len += pair->value.d;
        }
      else if (pair->code == 74)
        {
          _o->dashes[j].shape_flag = pair->value.i;
          LOG_TRACE ("LTYPE.dashes[%d].shape_flag = 0%x [RSx 74]\n", j,
                     pair->value.i);
          if (_o->dashes[j].shape_flag & 0x4)
            _o->has_strings_area = 1;
        }
      else if (pair->code == 75)
        {
          if (j < 0)
            j++;
          assert (j < num_dashes);
          _o->dashes[j].complex_shapecode = pair->value.i;
          LOG_TRACE ("LTYPE.dashes[%d].complex_shapecode = %d [RS 75]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 340)
        {
          if (j < 0)
            j++;
          assert (j < num_dashes);
          _o->dashes[j].style = dwg_add_handleref (obj->parent, 5, pair->value.u, obj);
          LOG_TRACE ("LTYPE.dashes[%d].style = " FORMAT_REF " [H 340]\n", j,
                     ARGS_REF (_o->dashes[j].style));
        }
      else if (pair->code == 44)
        {
          _o->dashes[j].x_offset = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].x_offset = %f [BD 44]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 45)
        {
          _o->dashes[j].y_offset = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].y_offset = %f [BD 45]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 46)
        {
          _o->dashes[j].scale = pair->value.d;
          LOG_TRACE ("LTYPE.dashes[%d].scale = %f [BD 46]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 50)
        {
          _o->dashes[j].rotation = deg2rad (pair->value.d);
          LOG_TRACE ("LTYPE.dashes[%d].rotation = %f [BD 50]\n", j,
                     _o->dashes[j].rotation);
        }
      else
        break; // not a Dwg_LTYPE_dash

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

static Dxf_Pair *
add_MLINESTYLE_lines (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                      Dxf_Pair *restrict pair)
{
  int num_lines = pair->value.i;
  Dwg_Object_MLINESTYLE *_o = obj->tio.object->tio.MLINESTYLE;
  Dwg_Data *dwg = obj->parent;
  _o->num_lines = num_lines;
  LOG_TRACE ("MLINESTYLE.num_lines = %d [RC 71]\n", num_lines);
  _o->lines = xcalloc (num_lines, sizeof (Dwg_MLINESTYLE_line));
  if (!_o->lines)
    {
      _o->num_lines = 0;
      return NULL;
    }

  for (int j = -1; j < (int)num_lines;)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (pair == NULL || pair->code == 0)
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
              if ((hdl = dwg_find_tablehandle_silent (dwg, pair->value.s, "LTYPE")))
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
  LOG_TRACE ("LWPOLYLINE.num_points = %u [BS 90]\n", num_points);
  _o->points = xcalloc (num_points, sizeof (BITCODE_2RD));
  if (!_o->points)
    {
      _o->num_points = 0;
      return NULL;
    }

  while (pair != NULL && pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (pair == NULL || pair->code == 0)
        {
          LOG_TRACE ("LWPOLYLINE.flag = %d [BS 70]\n", _o->flag);
          return pair;
        }
      else if (pair->code == 43)
        {
          _o->const_width = pair->value.d;
          if (pair->value.d != 0.0)
            _o->flag |= 4;
          LOG_TRACE ("LWPOLYLINE.const_width = %f [BD 43]\n", pair->value.d);
        }
      else if (pair->code == 70)
        {
          if (pair->value.i & 1) /* only if closed or not */
            _o->flag |= 512;
          else if (pair->value.i & 128) /* plinegen? */
            _o->flag |= 128;
          LOG_TRACE ("LWPOLYLINE.flag = %d [BS 70]\n", _o->flag);
        }
      else if (pair->code == 38)
        {
          _o->elevation = pair->value.d;
          if (pair->value.d != 0.0)
            _o->flag |= 8;
          LOG_TRACE ("LWPOLYLINE.elevation = %f [38 BD]\n", pair->value.d);
        }
      else if (pair->code == 39)
        {
          _o->thickness = pair->value.d;
          if (pair->value.d != 0.0)
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
          if (_o->extrusion.x != 0.0 || _o->extrusion.y != 0.0 || _o->extrusion.z != 1.0)
            _o->flag |= 1;
          LOG_TRACE ("LWPOLYLINE.extrusion = (%f, %f, %f) [BE 210]\n",
                     _o->extrusion.x, _o->extrusion.y, _o->extrusion.z);
        }
      else if (pair->code == 10)
        {
          j++; // we always start with 10 (I hope)
          assert (_o->num_points > 0);
          assert (j < (int)_o->num_points);
          _o->points[j].x = pair->value.d;
        }
      else if (pair->code == 20)
        {
          assert (j >= 0);
          assert (_o->num_points > 0);
          assert (j < (int)_o->num_points);
          LOG_TRACE ("LWPOLYLINE.points[%d] = (%f, %f) [2RD 10]\n", j,
                     _o->points[j].x, pair->value.d);
          _o->points[j].y = pair->value.d;
        }
      else if (pair->code == 42)
        {
          if (!j)
            {
              _o->bulges = xcalloc (num_points, sizeof (BITCODE_BD));
              if (!_o->bulges)
                {
                  _o->num_bulges = 0;
                  return NULL;
                }
              _o->num_bulges = num_points;
            }
          assert (j >= 0);
          assert (_o->num_bulges > 0);
          assert (j < (int)_o->num_bulges);
          _o->bulges[j] = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.bulges[%d] = %f [BD 42]\n", j, pair->value.d);
        }
      else if (pair->code == 91)
        {
          if (!j)
            {
              _o->vertexids = xcalloc (num_points, sizeof (BITCODE_BL));
              if (!_o->vertexids)
                {
                  _o->num_vertexids = 0;
                  return NULL;
                }
              _o->num_vertexids = num_points;
            }
          assert (j >= 0);
          assert (_o->num_vertexids > 0);
          assert (j < (int)_o->num_vertexids);
          _o->vertexids[j] = pair->value.i;
          LOG_TRACE ("LWPOLYLINE.vertexids[%d] = %d [BL 91]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 40) // not const_width
        {
          if (!j)
            {
              _o->widths = xcalloc (num_points, sizeof (Dwg_LWPOLYLINE_width));
              if (!_o->widths)
                {
                  _o->num_widths = 0;
                  return NULL;
                }
              _o->flag |= 4;
              _o->num_widths = num_points;
            }
          assert (j >= 0);
          assert (_o->num_widths > 0);
          assert (j < (int)_o->num_widths);
          _o->widths[j].start = pair->value.d;
          LOG_TRACE ("LWPOLYLINE.widths[%d].start = %f [BD 40]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 41 && (_o->flag & 4)) // not const_width
        {
          assert (j >= 0);
          assert (_o->num_widths > 0);
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
  o->encr_sat_data = xcalloc (2, sizeof (char *));
  if (!o->encr_sat_data)
    {
      o->num_blocks = 0;
      return NULL;
    }
  o->encr_sat_data[0] = NULL;
  o->block_size = xcalloc (2, sizeof (BITCODE_BL));
  if (!o->block_size)
    {
      o->num_blocks = 0;
      return NULL;
    }

  while (pair != NULL && pair->code == 1)
    {
      int len = strlen (pair->value.s) + 1; // + the \n
      if (!total)
        {
          total = len;
          o->encr_sat_data[0] = malloc (total + 1); // + the \0
          if (!o->encr_sat_data[0])
            {
              LOG_ERROR ("Out of memory");
              return NULL;
            }
          // memcpy (o->encr_sat_data[0], pair->value.s, len + 1);
          strcpy ((char*)o->encr_sat_data[0], pair->value.s);
        }
      else
        {
          total += len;
          o->encr_sat_data[0] = realloc (o->encr_sat_data[0], total + 1);
          if (!o->encr_sat_data[0])
            {
              LOG_ERROR ("Out of memory");
              return NULL;
            }
          strcat ((char*)o->encr_sat_data[0], pair->value.s);
        }
      strcat ((char*)o->encr_sat_data[0], "\n");
      i++;

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  LOG_TRACE ("%s.block_size[0]: %d\n", obj->name, total);

  if (o->version == 1)
    {
      int idx = 0;
      o->unknown = 1; // ??
      o->acis_data = xcalloc (1, total + 1);
      if (!o->acis_data)
        {
          return NULL;
        }
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
      LOG_TRACE ("MESH.num_subdiv_vertex = %u [BL 91]\n", pair->value.u);
      if (pair->value.u)
        {
          o->subdiv_vertex = xcalloc (o->num_subdiv_vertex, sizeof (BITCODE_3BD));
          if (!o->subdiv_vertex)
            {
              o->num_subdiv_vertex = 0;
              return NULL;
            }
        }
    }

  while (pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);

      if (pair == NULL || pair->code == 0)
        return pair;
      else if (pair->code == 92)
        {
          j = 0;
          vector = pair->code;
          o->num_vertex = pair->value.u;
          LOG_TRACE ("MESH.num_vertex = %u [BL 92]\n", pair->value.u);
          if (pair->value.u)
            {
              o->vertex = xcalloc (o->num_vertex, sizeof (BITCODE_3BD));
              if (!o->vertex)
                {
                  o->num_vertex = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 93)
        {
          j = 0;
          vector = pair->code;
          o->num_faces = pair->value.u;
          LOG_TRACE ("MESH.num_faces = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u)
            {
              o->faces = xcalloc (o->num_faces, sizeof (BITCODE_BL));
              if (!o->faces)
                {
                  o->num_faces = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 94)
        {
          j = 0;
          vector = pair->code;
          o->num_edges = pair->value.u;
          LOG_TRACE ("MESH.num_edges = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u) // from face - to face
            {
              o->edges = xcalloc (o->num_edges, sizeof (Dwg_MESH_edge));
              if (!o->edges)
                {
                  o->num_edges = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 95)
        {
          j = 0;
          vector = pair->code;
          o->num_crease = pair->value.u;
          LOG_TRACE ("MESH.num_crease = %u [BL %d]\n", pair->value.u,
                     pair->code);
          if (pair->value.u)
            {
              o->crease = xcalloc (o->num_crease, sizeof (BITCODE_BD));
              if (!o->crease)
                {
                  o->num_crease = 0;
                  return NULL;
                }
            }
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
      LOG_TRACE ("HATCH.num_paths = %u [BS 91]\n", o->num_paths);
      o->paths = xcalloc (o->num_paths, sizeof (Dwg_HATCH_Path));
      if (!o->paths)
        {
          o->num_paths = 0;
          return NULL;
        }
    }
  else if (pair->code == 78)
    {
      o->num_deflines = pair->value.l;
      LOG_TRACE ("HATCH.num_deflines = %ld [BS 78]\n", pair->value.l);
      o->deflines = xcalloc (pair->value.l, sizeof (Dwg_HATCH_DefLine));
      if (!o->deflines)
        {
          o->num_deflines = 0;
          return NULL;
        }
    }
  if (pair->code == 453)
    {
      o->num_colors = pair->value.l;
      LOG_TRACE ("HATCH.num_colors = %ld [BL 453]\n", pair->value.l);
      o->colors = xcalloc (pair->value.l, sizeof (Dwg_HATCH_Color));
      if (!o->colors)
        {
          o->num_colors = 0;
          return NULL;
        }
    }

  while (pair->code != 0)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);

      if (pair == NULL || pair->code == 0 || pair->code == 75)
        return pair;
      else if (pair->code == 92)
        {
          j++;
          assert (j >= 0);
          assert (j < (int)o->num_paths);
          o->paths[j].flag = pair->value.u;
          LOG_TRACE ("HATCH.paths[%d].flag = %u [BL 92]\n", j, pair->value.u);
          is_plpath = pair->value.u & 2;
          o->has_derived = pair->value.u & 4;
          LOG_TRACE ("HATCH.has_derived = %d [B 0]\n", o->has_derived);
        }
      else if (pair->code == 93)
        {
          assert (j >= 0);
          assert (j < (int)o->num_paths);
          o->paths[j].num_segs_or_paths = pair->value.u;
          LOG_TRACE ("HATCH.paths[%d].num_segs_or_paths = %u [BL 93]\n", j,
                     pair->value.u);
          k = -1;
          if (pair->value.u && !is_plpath)
            { /* segs */
              o->paths[j].segs
                  = xcalloc (pair->value.u, sizeof (Dwg_HATCH_PathSeg));
              if (!o->paths[j].segs)
                {
                  o->paths[j].num_segs_or_paths = 0;
                  return NULL;
                }
            }
          else if (pair->value.u)
            { /* polyline path */
              o->paths[j].polyline_paths
                  = xcalloc (pair->value.u, sizeof (Dwg_HATCH_PolylinePath));
              if (!o->paths[j].polyline_paths)
                {
                  o->paths[j].num_segs_or_paths = 0;
                  return NULL;
                }
            }
        }
      else if (pair->code == 72)
        {
          assert (j >= 0);
          assert (j < (int)o->num_paths);
          if (!is_plpath)
            {
              k++;
              assert (k >= 0);
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
          assert (j >= 0);
          assert (j < (int)o->num_paths);
          o->paths[j].closed = pair->value.i;
          LOG_TRACE ("HATCH.paths[%d].closed = %d [RC 73]\n", j,
                     pair->value.i);
        }
      else if (pair->code == 94 && !is_plpath && pair->value.l)
        {
          assert (j >= 0);
          assert (k >= 0);
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].degree = pair->value.l;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].degree = %ld [BL 94]\n", j, k,
                     pair->value.l);
        }
      else if (pair->code == 74 && !is_plpath && pair->value.i)
        {
          assert (j >= 0);
          assert (k >= 0);
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].is_periodic = pair->value.i;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].is_periodic = %d [B 74]\n", j,
                     k, pair->value.i);
        }
      else if (pair->code == 95 && !is_plpath)
        {
          assert (j >= 0);
          assert (k >= 0);
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].num_knots = pair->value.l;
          LOG_TRACE ("HATCH.paths[%d].segs[%d].num_knots = %ld [BL 95]\n", j,
                     k, pair->value.l);
          o->paths[j].segs[k].knots = xcalloc (pair->value.l, sizeof (double));
          if (!o->paths[j].segs[k].knots)
            {
              o->paths[j].segs[k].num_knots = 0;
              return NULL;
            }
          l = -1;
        }
      else if (pair->code == 96 && !is_plpath)
        {
          assert (j >= 0);
          assert (k >= 0);
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].segs[k].num_control_points = pair->value.l;
          LOG_TRACE (
              "HATCH.paths[%d].segs[%d].num_control_points = %ld [BL 96]\n", j,
              k, pair->value.l);
          o->paths[j].segs[k].control_points
              = xcalloc (pair->value.l, sizeof (Dwg_HATCH_ControlPoint));
          if (!o->paths[j].segs[k].control_points)
            {
              o->paths[j].segs[k].num_control_points = 0;
              return NULL;
            }
          l = -1;
        }
      else if (pair->code == 10 && !is_plpath && !o->num_seeds)
        {
          assert (j >= 0);
          assert (k >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
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
              assert (j >= 0);
              assert (k >= 0);
              assert (l >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
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
                  assert (l >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
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
          assert (j >= 0);
          assert (k >= 0);
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].polyline_paths[k].point.x = pair->value.d;
          // LOG_TRACE ("HATCH.paths[%d].polyline_paths[%d].point.x = %f [BD 10
          // ]\n",
          //           j, k, pair->value.d);
        }
      else if (pair->code == 20 && is_plpath && !o->num_seeds)
        {
          assert (j >= 0);
          assert (k >= 0);
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].polyline_paths[k].point.y = pair->value.d;
          LOG_TRACE (
              "HATCH.paths[%d].polyline_paths[%d].point = (%f, %f) [2RD 10]\n",
              j, k, o->paths[j].polyline_paths[k].point.x, pair->value.d);
        }
      else if (pair->code == 42 && is_plpath)
        {
          assert (j >= 0);
          assert (k >= 0);
          assert (j < (int)o->num_paths);
          assert (k < (int)o->paths[j].num_segs_or_paths);
          o->paths[j].polyline_paths[k].bulge = pair->value.d;
          LOG_TRACE ("HATCH.paths[%d].polyline_paths[%d].bulge = %f [BD 42]\n",
                     j, k, pair->value.d);
        }
      else if (pair->code == 97 && !is_plpath)
        {
          assert (j >= 0);
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
          assert (j >= 0);
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
          o->deflines = xcalloc (pair->value.l, sizeof (Dwg_HATCH_DefLine));
          if (!o->deflines)
            {
              o->num_deflines = 0;
              return NULL;
            }
          j = -1;
        }
      else if (pair->code == 53 && o->num_deflines)
        {
          j++;
          assert (j >= 0);
          assert (j < (int)o->num_deflines);
          o->deflines[j].angle = deg2rad (pair->value.d);
          LOG_TRACE ("HATCH.deflines[%d].angle = %f [BD 53]\n", j,
                     o->deflines[j].angle);
        }
      else if (pair->code == 43 && o->num_deflines)
        {
          assert (j >= 0);
          assert (j < (int)o->num_deflines);
          o->deflines[j].pt0.x = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].pt0.x = %f [BD 43]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 44 && o->num_deflines)
        {
          assert (j >= 0);
          assert (j < (int)o->num_deflines);
          o->deflines[j].pt0.y = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].pt0.y = %f [BD 44]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 45 && o->num_deflines)
        {
          assert (j >= 0);
          assert (j < (int)o->num_deflines);
          o->deflines[j].offset.x = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].offset.x = %f [BD 45]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 46 && o->num_deflines)
        {
          assert (j >= 0);
          assert (j < (int)o->num_deflines);
          o->deflines[j].offset.y = pair->value.d;
          LOG_TRACE ("HATCH.deflines[%d].offset.y = %f [BD 46]\n", j,
                     pair->value.d);
        }
      else if (pair->code == 79 && o->num_deflines)
        {
          assert (j >= 0);
          assert (j < (int)o->num_deflines);
          o->deflines[j].num_dashes = pair->value.u;
          LOG_TRACE ("HATCH.deflines[%d].num_dashes = %u [BS 79]\n", j,
                     pair->value.u);
          if (pair->value.u)
            {
              o->deflines[j].dashes
                = xcalloc (pair->value.u, sizeof (BITCODE_BD));
              if (!o->deflines[j].dashes)
                {
                  o->deflines[j].num_dashes = 0;
                  return NULL;
                }
            }
          k = -1;
        }
      else if (pair->code == 49 && o->num_deflines && j >= 0)
        {
          assert (j >= 0);
          assert (j < (int)o->num_deflines);
          if (!o->deflines[j].dashes)
            {
              LOG_ERROR ("DXF 79 num_dashes missing for HATCH.dashes 49")
              goto unknown_HATCH;
            }
          k++;
          assert (k >= 0);
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
            {
              o->seeds = xcalloc (pair->value.u, sizeof (BITCODE_2RD));
              if (!o->seeds)
                {
                  o->num_seeds = 0;
                  return NULL;
                }
            }
          k = -1;
        }
      else if (pair->code == 10 && o->num_seeds)
        {
          k++;
          assert (k >= 0);
          assert (k < (int)o->num_seeds);
          o->seeds[k].x = pair->value.d;
          // LOG_TRACE ("HATCH.seeds[%d].x = %f [10 2RD]\n",
          //           k, pair->value.d);
        }
      else if (pair->code == 20 && o->num_seeds)
        {
          assert (k >= 0);
          assert (k < (int)o->num_seeds);
          o->seeds[k].y = pair->value.d;
          LOG_TRACE ("HATCH.seeds[%d] = (%f, %f) [2RD 10]\n", k, o->seeds[k].x,
                     pair->value.d);
        }
      else if (pair->code == 330 && o->num_boundary_handles && (BITCODE_BL)k < o->num_boundary_handles)
        {
          BITCODE_H ref
              = dwg_add_handleref (obj->parent, 3, pair->value.u, obj);
          if (!o->boundary_handles)
            o->boundary_handles = xcalloc (o->num_boundary_handles, sizeof (BITCODE_H));
          if (!o->boundary_handles)
            {
              o->num_boundary_handles = 0;
              return NULL;
            }
          o->boundary_handles[k] = ref;
          LOG_TRACE ("HATCH.boundary_handles[%d] = " FORMAT_REF " [H 330]\n",
                     k, ARGS_REF (ref));
        }
      else if (pair->code == 453)
        {
          o->num_colors = pair->value.u;
          LOG_TRACE ("HATCH.num_colors = %u [BL 453]\n", pair->value.u);
          if (pair->value.u)
            {
              o->colors = xcalloc (pair->value.u, sizeof (Dwg_HATCH_Color));
              if (!o->colors)
                {
                  o->num_colors = 0;
                  return NULL;
                }
            }
          j = -1;
        }
      else if (pair->code == 463 && o->num_colors)
        {
          j++;
          assert (j >= 0);
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
          assert (j >= 0);
          assert (j < (int)o->num_colors);
          o->colors[j].color.rgb = pair->value.u;
          LOG_TRACE ("HATCH.colors[%d].color.rgb = %06X [CMC 421]\n", j,
                     pair->value.u);
        }
      else if (pair->code == 431 && o->num_colors)
        {
          assert (j >= 0);
          assert (j < (int)o->num_colors);
          if (dat->version >= R_2007)
            o->colors[j].color.name = (BITCODE_T)bit_utf8_to_TU (pair->value.s);
          else
            o->colors[j].color.name = strdup (pair->value.s);
          LOG_TRACE ("HATCH.colors[%d].color.name = %s [CMC 431]\n", j,
                     pair->value.s);
        }
      else if (pair->code == 470)
        {
          dwg_dynapi_entity_set_value (o, "HATCH", "gradient_name",
                                       &pair->value, 1);
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
        {
        unknown_HATCH:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code, "HATCH");
        }
    }
  return pair;
}

static Dxf_Pair *
add_MULTILEADER_lines (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                       Dxf_Pair *restrict pair, Dwg_LEADER_Node *lnode)
{
  Dwg_Entity_MULTILEADER *o = obj->tio.entity->tio.MULTILEADER;
  if (!lnode)
    return NULL;
  lnode->num_lines = 0;
  if (pair->code == 304 && strEQc (pair->value.s, "LEADER_LINE{"))
    {
      int i = -1, j = -1, k = -1;
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      lnode->lines = xcalloc (1, sizeof (Dwg_LEADER_Line));
      if (!lnode->lines)
        {
          lnode->num_lines = 0;
          return NULL;
        }

      // lines and breaks
      while (pair->code != 305 && pair->code != 0)
        {
          Dwg_LEADER_Line *lline = &lnode->lines[0];
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            return NULL;
          switch (pair->code)
            {
            case 10:
              i++;
              lnode->num_lines = i + 1;
              LOG_TRACE ("%s.leaders[].num_lines = %d\n", obj->name, i + 1);
              if (i > 0)
                {
                  lnode->lines
                      = realloc (lnode->lines,
                                 lnode->num_lines * sizeof (Dwg_LEADER_Line));
                  if (!lnode->lines)
                    {
                      lnode->num_lines = 0;
                      LOG_ERROR ("Out of memory");
                      return NULL;
                    }
                }
              lline = &lnode->lines[i];
              memset (lline, 0, sizeof (Dwg_LEADER_Line));
              lline->num_breaks = 0;
              j++;
              lline->num_points = j + 1;
              lline->points
                  = realloc (lline->points, (j + 1) * sizeof (BITCODE_3BD));
              memset (&lline->points[j], 0, sizeof (BITCODE_3BD));
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
              memset (&ctx->leaders[k], 0, sizeof (Dwg_LEADER_Break));
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
              lline->linewt = dxf_find_lweight (pair->value.i);
              LOG_TRACE ("%s.leaders[].lines[%d].linewt = %d [BL %d]\n",
                         obj->name, i, lline->linewt, pair->code);
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
              return pair;
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
  if (pair != NULL && pair->code == 302 && strEQc (pair->value.s, "LEADER{"))
    {
      int i = -1, j = -1;
      Dwg_MLEADER_AnnotContext *ctx = &o->ctx;
      ctx->num_leaders = 0;
      while (pair != NULL && pair->code != 303 && pair->code != 0)
        {
          Dwg_LEADER_Node *lnode = i >= 0 ? &ctx->leaders[i] : NULL;
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          if (!pair)
            return NULL;
          if (!lnode && pair->code != 290 && pair->code != 304
              && pair->code != 303)
            {
              LOG_ERROR ("Missing MULTILEADER.LEADER{ 290 start");
              return pair;
            }
          if (lnode)
            lnode->num_breaks = 0;
          switch (pair->code)
            {
            case 290:
              i++;
              ctx->num_leaders = i + 1;
              LOG_TRACE ("%s.ctx.num_leaders = %d\n", obj->name, i + 1);
              ctx->leaders
                  = realloc (ctx->leaders, (i + 1) * sizeof (Dwg_LEADER_Node));
              memset (&ctx->leaders[i], 0, sizeof (Dwg_LEADER_Node));
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
                  LOG_TRACE ("%s.leaders[%d].num_breaks = %d\n", obj->name, i, j + 1);
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
                  assert (j >= 0);
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
                  assert (j >= 0);
                  lnode->breaks[j].start.z = pair->value.d;
                  LOG_TRACE (
                      "%s.ctx.leaders[%d].breaks[%d].start.z = %f [3BD %d]\n",
                      obj->name, i, j, pair->value.d, pair->code);
                }
              break;
            case 12:
              assert (j >= 0);
              lnode->breaks[j].end.x = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.x = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 22:
              assert (j >= 0);
              lnode->breaks[j].end.y = pair->value.d;
              LOG_TRACE ("%s.ctx.leaders[%d].breaks[%d].end.y = %f [3BD %d]\n",
                         obj->name, i, j, pair->value.d, pair->code);
              break;
            case 32:
              assert (j >= 0);
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
              return pair;
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
      while (pair != NULL && pair->code != 301 && pair->code != 0)
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
              if (ctx->has_content_blk)
                goto unknown_mleader;
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
                  if (ctx->has_content_blk)
                    goto unknown_mleader;
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
                  if (ctx->has_content_blk)
                    goto unknown_mleader;
                  ctx->content.txt.style
                      = dwg_add_handleref (obj->parent, 5, pair->value.u, obj);
                  LOG_TRACE ("%s.ctx.content.txt.style = " FORMAT_REF
                             " [%d H]\n",
                             obj->name, ARGS_REF (ctx->content.txt.style),
                             pair->code);
                }
              break;
            case 11:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.normal.x = pair->value.d;
              break;
            case 21:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.normal.y = pair->value.d;
              break;
            case 31:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.normal.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.normal = (%f, %f, %f) [11 3BD]\n",
                         obj->name, ctx->content.txt.normal.x,
                         ctx->content.txt.normal.y, ctx->content.txt.normal.z);
              break;
            case 12:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.location.x = pair->value.d;
              break;
            case 22:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.location.y = pair->value.d;
              break;
            case 32:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.location.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.location = (%f, %f, %f) [12 3BD]\n",
                  obj->name, ctx->content.txt.location.x,
                  ctx->content.txt.location.y, ctx->content.txt.location.z);
              break;
            case 13:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.direction.x = pair->value.d;
              break;
            case 23:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.direction.y = pair->value.d;
              break;
            case 33:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.direction.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.direction = (%f, %f, %f) [13 3BD]\n",
                  obj->name, ctx->content.txt.direction.x,
                  ctx->content.txt.direction.y, ctx->content.txt.direction.z);
              break;
            case 42:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.rotation = deg2rad (pair->value.d);
              LOG_TRACE ("%s.ctx.content.txt.rotation = %f [BD %d]\n",
                         obj->name, ctx->content.txt.rotation, pair->code);
              break;
            case 43:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.width = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.width = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 44:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.height = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.height = %f [BD %d]\n", obj->name,
                         pair->value.d, pair->code);
              break;
            case 45:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.line_spacing_factor = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.txt.line_spacing_factor = %f [BD %d]\n",
                  obj->name, pair->value.d, pair->code);
              break;
            case 170:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.line_spacing_style = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.content.txt.line_spacing_style = %d [BS %d]\n",
                  obj->name, pair->value.i, pair->code);
              break;
            case 171:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.alignment = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.alignment = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 172:
              if (!ctx->has_content_txt)
                break;
              ctx->content.txt.flow = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.flow = %d [BS %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 90:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.color.index = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 91:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.bg_color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.bg_color.index = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 141:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.bg_scale = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.bg_scale = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 142:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.col_width = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_width = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 143:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.col_gutter = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_gutter = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 92:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.bg_transparency = pair->value.u;
              LOG_TRACE ("%s.ctx.content.txt.bg_transparency = %u [BL %d]\n",
                         obj->name, pair->value.u, pair->code);
              break;
            case 291:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_bg_fill = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_bg_fill = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 292:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_bg_mask_fill = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_bg_mask_fill = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 293:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_height_auto = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.is_height_auto = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 294:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.is_col_flow_reversed = pair->value.i;
              LOG_TRACE (
                  "%s.ctx.content.txt.is_col_flow_reversed = %i [B %d]\n",
                  obj->name, pair->value.i, pair->code);
              break;
            case 295:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.word_break = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.word_break = %i [B %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 173:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              ctx->content.txt.col_type = pair->value.i;
              LOG_TRACE ("%s.ctx.content.txt.col_type = %d [BS %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 144:
              if (!ctx->has_content_txt)
                goto unknown_mleader;
              i++;
              ctx->content.txt.num_col_sizes = i + 1;
              ctx->content.txt.col_sizes = realloc (ctx->content.txt.col_sizes,
                                                    (i + 1) * sizeof (double));
              ctx->content.txt.col_sizes[i] = pair->value.d;
              LOG_TRACE ("%s.ctx.content.txt.col_sizes[%d] = %f [BD %d]\n",
                         obj->name, i, pair->value.d, pair->code);
              break;
            case 296:
              if (ctx->has_content_txt)
                goto unknown_mleader;
              ctx->has_content_blk = pair->value.i;
              LOG_TRACE ("%s.ctx.has_content_blk = %i [B %d]\n", obj->name,
                         pair->value.i, pair->code);
              break;
            case 14: // has_block
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.x = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.x = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 24:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.y = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.y = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 34:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.z = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 341:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.block_table
                  = dwg_add_handleref (obj->parent, 4, pair->value.u, obj);
              LOG_TRACE ("%s.ctx.content.blk.block_table = " FORMAT_REF
                         " [%d H]\n",
                         obj->name, ARGS_REF (ctx->content.blk.block_table),
                         pair->code);
              break;
            case 15: // has_block
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.normal.x = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.normal.x = %f [3BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 25:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.location.y = pair->value.d;
              break;
            case 35:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.location.z = pair->value.d;
              LOG_TRACE (
                  "%s.ctx.content.blk.location = (%f, %f, %f) [3BD %d]\n",
                  obj->name, ctx->content.blk.location.x,
                  ctx->content.blk.location.y, ctx->content.blk.location.z,
                  pair->code);
              break;
            case 16: // has_block
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.scale.x = pair->value.d;
              break;
            case 26:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.scale.y = pair->value.d;
              break;
            case 36:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.scale.z = pair->value.d;
              LOG_TRACE ("%s.ctx.content.blk.scale = (%f, %f, %f) [3BD %d]\n",
                         obj->name, ctx->content.blk.scale.x,
                         ctx->content.blk.scale.y, ctx->content.blk.scale.z,
                         pair->code);
              break;
            case 46:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.rotation = deg2rad (pair->value.d);
              LOG_TRACE ("%s.ctx.content.blk.rotation = %f [BD %d]\n",
                         obj->name, pair->value.d, pair->code);
              break;
            case 93:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              ctx->content.blk.color.index = pair->value.i;
              LOG_TRACE ("%s.ctx.content.blk.color.index = %d [CMC %d]\n",
                         obj->name, pair->value.i, pair->code);
              break;
            case 47:
              if (!ctx->has_content_blk)
                goto unknown_mleader;
              j++;
              if (!j)
                {
                  ctx->content.blk.transform = xcalloc (16, sizeof (double));
                  if (!ctx->content.blk.transform)
                    {
                      return NULL;
                    }
                }
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
            unknown_mleader:
              LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                         "MULTILEADER");
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
    }
  return pair;
}

// returns with 0
static Dxf_Pair *
add_TABLESTYLE (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_TABLESTYLE *o = obj->tio.object->tio.TABLESTYLE;
  BITCODE_H hdl;
  int i = -1, j = -1;

  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 7:
          i++;
          assert (i >= 0 && i < 3);
          hdl = find_tablehandle (dwg, pair);
          assert (hdl);
          assert (o->num_rowstyles);
          o->rowstyles[i].text_style = hdl;
          LOG_TRACE ("%s.rowstyles[%d].text_style = " FORMAT_REF " [H %d]\n",
                       obj->name, i, ARGS_REF(hdl), pair->code);
          break;
        case 140:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].text_height = pair->value.d;
          LOG_TRACE ("%s.rowstyles[%d].text_height = %f [BD %d]\n",
                       obj->name, i, pair->value.d, pair->code);
          break;
        case 170:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].text_alignment = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].text_alignment = " FORMAT_BS " [BS %d]\n",
                     obj->name, i, o->rowstyles[i].text_alignment, pair->code);
          break;
        case 62:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].text_color.index = pair->value.i;
          //TODO rgb, alpha with 420,430?
          LOG_TRACE ("%s.rowstyles[%d].text_color.index = %d [CMC %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 63:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].fill_color.index = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].fill_color.index = %d [CMC %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 283:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].has_bgcolor = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].has_bgcolor = %d [B %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 90:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].data_type = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].data_type = %d [BL %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 91:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].unit_type = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].unit_type = %d [BL %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 1:
          assert (i >= 0 && i < 3);
          assert (o->num_rowstyles);
          o->rowstyles[i].format_string = bit_utf8_to_TU (pair->value.s);
          LOG_TRACE ("%s.rowstyles[%d].format_string = %s [TU %d]\n",
                     obj->name, i, pair->value.s, pair->code);
          break;
        case 274:
        case 275:
        case 276:
        case 277:
        case 278:
        case 279:
          j = pair->code - 274;
          assert (i >= 0 && i < 3);
          assert (j >= 0 && j <= 6);
          assert (o->num_rowstyles);
          if (!o->rowstyles[i].borders)
            {
              o->rowstyles[i].borders = xcalloc (6, sizeof (Dwg_TABLESTYLE_border));
              if (!o->rowstyles[i].borders)
                {
                  o->rowstyles[i].num_borders = 0;
                  return NULL;
                }
              o->rowstyles[i].num_borders = 6;
            }
          assert (o->rowstyles[i].num_borders);
          o->rowstyles[i].borders[j].linewt = dxf_find_lweight (pair->value.i);
          LOG_TRACE ("%s.rowstyles[%d].borders[%d].linewt = %d [BSd %d]\n",
                     obj->name, i, j, o->rowstyles[i].borders[j].linewt, pair->code);
          break;
        case 284:
        case 285:
        case 286:
        case 287:
        case 288:
        case 289:
          j = pair->code - 284;
          assert (i >= 0 && i < 3);
          assert (j >= 0 && j <= 6);
          assert (o->num_rowstyles);
          o->rowstyles[i].borders[j].visible = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].borders[%d].visible = %d [B %d]\n",
                     obj->name, i, j, pair->value.i, pair->code);
          break;
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
          j = pair->code - 64;
          assert (i >= 0 && i < 3);
          assert (j >= 0 && j <= 6);
          assert (o->num_rowstyles);
          o->rowstyles[i].borders[j].color.index = pair->value.i;
          LOG_TRACE ("%s.rowstyles[%d].borders[%d].color.index = %d [CMC %d]\n",
                     obj->name, i, j, pair->value.i, pair->code);
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                     "TABLESTYLE");
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

// returns with 0
static Dxf_Pair *
add_TABLEGEOMETRY_Cell (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                        Dxf_Pair *restrict pair)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_TABLEGEOMETRY *o = obj->tio.object->tio.TABLEGEOMETRY;
  BITCODE_H hdl;
  BITCODE_BL num_cells = o->num_cells;
  int i = -1, j = -1;

  o->cells = xcalloc (num_cells, sizeof (Dwg_TABLEGEOMETRY_Cell));
  if (!o->cells)
    {
      o->num_cells = 0;
      return NULL;
    }

  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 93:
          i++; // the first
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          o->cells[i].geom_data_flag = pair->value.i;
          LOG_TRACE ("%s.cells[%d].geom_data_flag = " FORMAT_BL " [BL %d]\n",
                       obj->name, i, o->cells[i].geom_data_flag, pair->code);
          break;
        case 40:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          o->cells[i].width_w_gap = pair->value.d;
          LOG_TRACE ("%s.cells[%d].width_w_gap = %f [BD %d]\n",
                       obj->name, i, pair->value.d, pair->code);
          break;
        case 41:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          o->cells[i].height_w_gap = pair->value.d;
          LOG_TRACE ("%s.cells[%d].height_w_gap = %f [BD %d]\n",
                       obj->name, i, pair->value.d, pair->code);
          break;
        case 330:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          hdl = find_tablehandle (dwg, pair);
          assert (hdl);
          o->cells[i].tablegeometry = hdl;
          LOG_TRACE ("%s.cells[%d].tablegeometry = " FORMAT_REF " [H %d]\n",
                       obj->name, i, ARGS_REF(hdl), pair->code);
          break;
        case 94:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          o->cells[i].num_geometry = pair->value.i;
          LOG_TRACE ("%s.cells[%d].num_geometry = " FORMAT_BL " [BL %d]\n",
                       obj->name, i, o->cells[i].num_geometry, pair->code);
          o->cells[i].geometry = xcalloc (pair->value.i, sizeof (Dwg_CellContentGeometry));
          if (!o->cells[i].geometry)
            {
              o->cells[i].num_geometry = 0;
              return NULL;
            }
          j = -1;
          break;
        case 10:
          j++;
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].dist_top_left.x = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].dist_top_left.x = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 20:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].dist_top_left.y = pair->value.d;
          break;
        case 30:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].dist_top_left.z = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].dist_top_left = ( %f, %f, %f) "
                     "[3BD 10]\n",
                     obj->name, i, j, o->cells[i].geometry[j].dist_top_left.x,
                     o->cells[i].geometry[j].dist_top_left.y, pair->value.d);
          break;
        case 11:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].dist_center.x = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].dist_center.x = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 21:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].dist_center.y = pair->value.d;
          break;
        case 31:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].dist_center.z = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].dist_center = ( %f, %f, %f) "
                     "[3BD 10]\n",
                     obj->name, i, j, o->cells[i].geometry[j].dist_center.x,
                     o->cells[i].geometry[j].dist_center.y, pair->value.d);
          break;
        case 43:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].content_width = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].content_width = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 44:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].content_height = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].content_height = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 45:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].width = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].width = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 46:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].height = pair->value.d;
          LOG_TRACE ("%s.cells[%d].geometry[%d].height = %f [BD %d]\n",
                     obj->name, i, j, pair->value.d, pair->code);
          break;
        case 95:
          assert (i >= 0 && i < (int)num_cells);
          assert (o->cells);
          assert (j >= 0 && j < (int)o->cells[i].num_geometry);
          assert (o->cells[i].geometry);
          o->cells[i].geometry[j].unknown = pair->value.i;
          LOG_TRACE ("%s.cells[%d].geometry[%d].unknown = %d [BL %d]\n",
                     obj->name, i, j, pair->value.i, pair->code);
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                     "TABLESTYLE");
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

// starts with 71 or 75
static Dxf_Pair *
add_DIMASSOC (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
              Dxf_Pair *restrict pair)
{
  Dwg_Object_DIMASSOC *o = obj->tio.object->tio.DIMASSOC;
  Dwg_Data *dwg = obj->parent;
  int i = -1;
  int have_rotated_type = 0;
  o->ref = xcalloc (4, sizeof (Dwg_DIMASSOC_Ref));
  if (!o->ref)
    {
      return NULL;
    }

  while (pair != NULL && pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          break;
        case 71: // always before each ref
          i++;
          while (!(o->associativity & (1<<i)) && i < 4) // popcount x
            i++;
          if (!(i >= 0 && i <= 3))
            {
              LOG_ERROR ("Invalid DIMASSOC_Ref index %d", i)
              return pair;
            }
          o->rotated_type = pair->value.i;
          have_rotated_type = 1; // early bump
          LOG_TRACE ("%s.rotated_type = %d [BS %d]\n",
                     obj->name, pair->value.i, pair->code);
          break;
        case 1:
          if (strNE (pair->value.s, "AcDbOsnapPointRef"))
            {
              LOG_ERROR ("Invalid DIMASSOC subclass %s", pair->value.s);
              return pair;
            }
          if (!(i >= 0 && i <= 3))
            {
              LOG_ERROR ("Invalid DIMASSOC_Ref index %d", i)
              return pair;
            }
          if (!have_rotated_type) // not already bumped
            {
              i++;
              while (!(o->associativity & (1<<i)) && i < 4) // popcount x
                i++;
              assert (i >= 0 && i <= 3);
            }
          if (dwg->header.version >= R_2007)
            o->ref[i].classname = (BITCODE_T)bit_utf8_to_TU (pair->value.s);
          else
            o->ref[i].classname = strdup (pair->value.s);
          LOG_TRACE ("%s.ref[%d].classname = %s [T %d]\n",
                     obj->name, i, pair->value.s, pair->code);
          have_rotated_type = 0;
          break;
        case 72:
          if (i < 0) break;
          o->ref[i].osnap_type = pair->value.i;
          LOG_TRACE ("%s.ref[%d].osnap_type = %d [RC %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 331:
          {
            BITCODE_BS n;
            if (i < 0) break;
            n = o->ref[i].num_mainobjs;
            o->ref[i].mainobjs = realloc (o->ref[i].mainobjs, (n + 1) * sizeof (BITCODE_H));
            o->ref[i].mainobjs[n] = dwg_add_handleref (dwg, 5, pair->value.u, obj);
            LOG_TRACE ("%s.ref[%d].mainobjs[%d] = " FORMAT_REF " [H* %d]\n",
                       obj->name, i, n, ARGS_REF(o->ref[i].mainobjs[n]), pair->code);
            o->ref[i].num_mainobjs++;
          }
          break;
        case 332:
          if (i < 0) break;
          o->ref[i].intsectobj = dwg_add_handleref (dwg, 5, pair->value.u, obj);
          LOG_TRACE ("%s.ref[%d].intsectobj = " FORMAT_REF " [H %d]\n",
                       obj->name, i, ARGS_REF(o->ref[i].intsectobj), pair->code);
          break;
        case 73:
          if (i < 0) break;
          o->ref[i].main_subent_type = pair->value.i;
          LOG_TRACE ("%s.ref[%d].main_subent_type = %d [BS %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 74:
          if (i < 0) break;
          o->ref[i].intsect_subent_type = pair->value.i;
          LOG_TRACE ("%s.ref[%d].intsect_subent_type = %d [BS %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 75:
          if (i < 0) break;
          o->ref[i].has_lastpt_ref = pair->value.i;
          LOG_TRACE ("%s.ref[%d].has_lastpt_ref = %d [B %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 91:
          if (i < 0) break;
          o->ref[i].main_gsmarker = pair->value.i;
          LOG_TRACE ("%s.ref[%d].main_gsmarker = %d [BL %d]\n",
                     obj->name, i, pair->value.i, pair->code);
          break;
        case 40:
          if (i < 0) break;
          o->ref[i].osnap_dist = pair->value.d;
          LOG_TRACE ("%s.ref[%d].osnap_dist = %f [BD %d]\n",
                     obj->name, i, pair->value.d, pair->code);
          break;
        case 10:
          if (i < 0) break;
          o->ref[i].osnap_pt.x = pair->value.d;
          break;
        case 20:
          if (i < 0) break;
          o->ref[i].osnap_pt.y = pair->value.d;
          break;
        case 30:
          if (i < 0) break;
          o->ref[i].osnap_pt.z = pair->value.d;
          LOG_TRACE ("%s.ref[%d].osnap_pt = (%f, %f, %f) [3BD 10]\n",
                     obj->name, i, o->ref[i].osnap_pt.x, o->ref[i].osnap_pt.y,
                     pair->value.d);
          break;
        default:
          LOG_ERROR ("Unknown DXF code %d for %s", pair->code,
                     "DIMASSOC");
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
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
  if (pair == NULL || pair->code != dxf)                                      \
    {                                                                         \
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s", obj->name, \
                 pair ? pair->code : -1, dxf, field);                         \
      return pair;                                                            \
    }                                                                         \
  dwg_dynapi_entity_set_value (o, obj->name, field, &pair->value, 1);         \
  LOG_TRACE ("%s.%s = %d [" #type " %d]\n", obj->name, field, pair->value.i,  \
             pair->code);                                                     \
  dxf_free_pair (pair)

#define EXPECT_H_DXF(field, htype, dxf, type)                                 \
  if (pair == NULL || pair->code != dxf)                                      \
    {                                                                         \
      LOG_ERROR ("%s: Unexpected DXF code %d, expected %d for %s", obj->name, \
                 pair ? pair->code : -1, dxf, field);                         \
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
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("geometry_status", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_H_DXF ("readdep", 5, 330, H); // or vector?
  pair = dxf_read_pair (dat);
  EXPECT_H_DXF ("writedep", 5, 360, H);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("constraint_status", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("dof", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("is_body_a_proxy", 90, B);

  return NULL;
}

static Dxf_Pair *
add_PERSSUBENTMANAGER (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                       Dxf_Pair *restrict pair)
{
  Dwg_Object_PERSSUBENTMANAGER *o = obj->tio.object->tio.PERSSUBENTMANAGER;
  Dwg_Data *dwg = obj->parent;

  EXPECT_INT_DXF ("class_version", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_bl1", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_bl2", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_bl3", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_bl4", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_bl5", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_bl6", 90, BL);

  return NULL;
}

static Dxf_Pair *
add_ASSOCDEPENDENCY (Dwg_Object *restrict obj, Bit_Chain *restrict dat,
                       Dxf_Pair *restrict pair)
{
  Dwg_Object_ASSOCDEPENDENCY *o = obj->tio.object->tio.ASSOCDEPENDENCY;
  Dwg_Data *dwg = obj->parent;

  EXPECT_INT_DXF ("class_version", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("status", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("isread_dep", 290, B);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("iswrite_dep", 290, B);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("isobjectstate_dep", 290, B);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_b4", 290, B);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("order", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_H_DXF ("readdep", 5, 330, H); //?
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("unknown_b5", 290, B);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("depbodyid", 90, BL);
  pair = dxf_read_pair (dat);
  EXPECT_H_DXF ("readdep", 5, 330, H);
  pair = dxf_read_pair (dat);
  EXPECT_H_DXF ("node", 5, 330, H);
  pair = dxf_read_pair (dat);
  EXPECT_H_DXF ("writedep", 5, 360, H);
  pair = dxf_read_pair (dat);
  EXPECT_INT_DXF ("depbodyid", 90, BL); //?

  return NULL;
}

static Dwg_Object *
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
        {
          if (prev->fixedtype == DWG_TYPE_SEQEND ||
              prev->fixedtype == DWG_TYPE_ENDBLK)
            return NULL;
          else
            return prev;
        }
    }
  return NULL;
}

static int
is_obj_absref (Dwg_Object *obj)
{
  /* With DICTIONARY it may vary */
  if (obj->type < DWG_TYPE_GROUP && // needs to be absolute 4.1.X
      /* obj->fixedtype != DWG_TYPE_DICTIONARY && */
      obj->fixedtype != DWG_TYPE_XRECORD)
    return 1;
  else if (obj->fixedtype == DWG_TYPE_LAYOUT)
    return 1;
  else // may have relative ref: 8.0.0
    return 0;
}

static Dxf_Pair *
new_table_control (const char *restrict name, Bit_Chain *restrict dat,
                   Dwg_Data *restrict dwg)
{
  // VPORT_CONTROL.num_entries
  // VPORT_CONTROL.entries[num_entries] handles
  Dwg_Object *obj;
  Dxf_Pair *pair = NULL;
  Dwg_Object_LTYPE_CONTROL *_obj = NULL; // the largest
  int j = 0;
  int is_utf = dwg->header.version >= R_2007 ? 1 : 0;
  char *fieldname;
  char ctrlname[80];
  char *dxfname;
  BITCODE_B xrefref;

  NEW_OBJECT (dwg, obj);

  if (strEQc (name, "BLOCK_RECORD"))
    strcpy (ctrlname, "BLOCK_CONTROL");
  else
    {
      strncpy (ctrlname, name, 70);
      ctrlname[69] = '\0';
      strcat (ctrlname, "_CONTROL");
    }
  LOG_TRACE ("add %s\n", ctrlname);
  dxfname = strdup (ctrlname);

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
    //obj->name = "UNKNOWN_OBJ";
    obj->fixedtype = DWG_TYPE_UNKNOWN_OBJ;
    // undo NEW_OBJECT
    free (dxfname);
    free (obj->tio.object);
    dwg->num_objects--;
    LOG_ERROR ("Unknown DXF TABLE %s nor %s_CONTROL", name, name);
    return pair;
  }
  if (!_obj)
    {
      //obj->name = "UNKNOWN_OBJ";
      obj->fixedtype = DWG_TYPE_UNKNOWN_OBJ;
      // undo NEW_OBJECT
      free (obj->tio.object);
      dwg->num_objects--;
      LOG_ERROR ("Empty _obj at DXF TABLE %s nor %s_CONTROL", name, name);
      return pair;
    }
  dwg_dynapi_entity_set_value (_obj, obj->name, "objid", &obj->index, 1);
  xrefref = 1;
  if (dwg_dynapi_entity_field (obj->name, "xrefref"))
    dwg_dynapi_entity_set_value (_obj, obj->name, "xrefref", &xrefref, 1);
  // default xdic_missing_flag
  if (dwg->header.version >= R_2004)
    obj->tio.object->xdic_missing_flag = 1;

  pair = dxf_read_pair (dat);
  // read common table until next 0 table or endtab
  while (pair != NULL && pair->code != 0)
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
            ctrlobj[69] = '\0';
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
              BITCODE_H owh;
              if (is_obj_absref (obj))
                owh = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              else // relative
                owh = dwg_add_handleref (dwg, 4, pair->value.u, obj);
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
              if (!_o->num_morehandles || j < 0 || j >= _o->num_morehandles)
                {
                  LOG_ERROR ("Invalid DIMSTYLE_CONTROL.num_morehandles %d or index %d",
                             _o->num_morehandles, j)
                  break;
                }
              assert (_o->morehandles);
              assert (j >= 0);
              assert (j < _o->num_morehandles);
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
              // can be -1
              BITCODE_BL num_entries = pair->value.i < 0 ? 0 : pair->value.i;
              hdls = xcalloc (num_entries, sizeof (Dwg_Object_Ref *));
              if (!hdls)
                num_entries = 0;
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_entries",
                                           &num_entries, 1);
              LOG_TRACE ("%s.num_entries = %u [BL 70]\n", ctrlname, num_entries);
              dwg_dynapi_entity_set_value (_obj, obj->name, "entries", &hdls,
                                           0);
              LOG_TRACE ("Add %d %s.%s\n", num_entries, ctrlname, "entries");
            }
          break;
        case 71:
          if (strEQc (ctrlname, "DIMSTYLE_CONTROL"))
            {
              if (pair->value.u)
                {
                  BITCODE_H *hdls;
                  hdls = xcalloc (pair->value.u, sizeof (Dwg_Object_Ref *));
                  if (!hdls)
                    pair->value.u = 0;
                  dwg_dynapi_entity_set_value (_obj, obj->name,
                                               "num_morehandles", &pair->value,
                                               is_utf);
                  LOG_TRACE ("%s.num_morehandles = %u [BL 71]\n", ctrlname,
                             pair->value.u);
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
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "LAYER");
  else if (pair->code == 1) // $DIMBLK
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "BLOCK");
  // some name: $DIMSTYLE, $UCSBASE, $UCSORTHOREF, $CMLSTYLE
  // not enough info, decide later
  else if (pair->code == 2)
    ;
  else if (pair->code == 3)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "DIMSTYLE");
  // what is/was 4 and 5? VIEW? VPORT_ENTITY?
  else if (pair->code == 6)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "LTYPE");
  else if (pair->code == 7)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "STYLE");

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
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "VPORT");
  else if (pair->code == 390)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "PLOTSTYLENAME");
  else if (pair->code == 347)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "MATERIAL");
  else if (pair->code == 345 || pair->code == 346)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "UCS");
  else if (pair->code == 361) // SUN
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "SHADOW");
  else if (pair->code == 340) // or TABLESTYLE or LAYOUT or MLINESTYLE ...
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "STYLE");
  else if (pair->code == 342 || pair->code == 343)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "STYLE");
  else if (pair->code == 348)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "VISUALSTYLE");
  else if (pair->code == 332)
    ref = dwg_find_tablehandle_silent (dwg, pair->value.s, "BACKGROUND");
#endif
  return ref;
}

// add pair to XRECORD
static Dxf_Pair *
add_xdata (Bit_Chain *restrict dat, Dwg_Object *restrict obj,
           Dxf_Pair *restrict pair)
{
  BITCODE_BL num_xdata, xdata_size;
  // add pairs to xdata linked list
  Dwg_Resbuf *rbuf;
  Dwg_Object_XRECORD *_obj = obj->tio.object->tio.XRECORD;

  num_xdata = _obj->num_xdata;
  xdata_size = _obj->xdata_size;
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
    }
  else
    _obj->xdata = rbuf;

  xdata_size += 2; // RS
  rbuf->type = pair->code;
  switch (get_base_value_type (rbuf->type))
    {
    case VT_STRING:
      if (!pair->value.s)
        goto invalid;
      PRE (R_2007)
      {
        Dwg_Data *dwg = obj->parent;
        rbuf->value.str.size = strlen (pair->value.s);
        rbuf->value.str.codepage = dwg->header.codepage;
        rbuf->value.str.u.data = strdup (pair->value.s);
        LOG_TRACE ("xdata[%d]: \"%s\" [%d]\n", num_xdata,
                   rbuf->value.str.u.data, rbuf->type);
        xdata_size += 3 + rbuf->value.str.size;
      }
      LATER_VERSIONS
      {
        int length = rbuf->value.str.size = strlen (pair->value.s);
        if (length > 0)
          {
            rbuf->value.str.u.wdata = bit_utf8_to_TU (pair->value.s);
          }
        xdata_size += 2 + 2 * rbuf->value.str.size;
      }
      break;
    case VT_REAL:
      rbuf->value.dbl = pair->value.d;
      LOG_TRACE ("xdata[%d]: %f [%d]\n", num_xdata, rbuf->value.dbl,
                 rbuf->type);
      xdata_size += 8;
      break;
    case VT_BOOL:
    case VT_INT8:
      rbuf->value.i8 = pair->value.i;
      LOG_TRACE ("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i8,
                 rbuf->type);
      xdata_size += 1;
      break;
    case VT_INT16:
      rbuf->value.i16 = pair->value.i;
      LOG_TRACE ("xdata[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i16,
                 rbuf->type);
      xdata_size += 2;
      break;
    case VT_INT32:
      rbuf->value.i32 = pair->value.l;
      LOG_TRACE ("xdata[%d]: %ld [%d]\n", num_xdata, (long)rbuf->value.i32,
                 rbuf->type);
      xdata_size += 4;
      break;
    case VT_INT64:
      rbuf->value.i64 = (BITCODE_BLL)pair->value.bll;
      LOG_TRACE ("xdata[%d]: " FORMAT_BLL " [%d]\n", num_xdata,
                 rbuf->value.i64, rbuf->type);
      xdata_size += 8;
      break;
    case VT_POINT3D:
      rbuf->value.pt[0] = pair->value.d;
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      if (!pair)
        return NULL;
      rbuf->value.pt[1] = pair->value.d;
      dxf_free_pair (pair);
      xdata_size += 24;
      { // if 30
        long pos = bit_position (dat);
        pair = dxf_read_pair (dat);
        if (!pair)
          return NULL;
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
      if (!pair->value.s)
        goto invalid;
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
        xdata_size += 1 + len;
        LOG_TRACE ("xdata[%d]: ", num_xdata);
        // LOG_TRACE_TF (rbuf->value.str.u.data, rbuf->value.str.size);
      }
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      xdata_size += 8;
      dwg_add_handle (&rbuf->value.h, 0, pair->value.u, obj);
      LOG_TRACE ("xdata[%d]: " FORMAT_H " [H %d]\n", num_xdata,
                 ARGS_H (rbuf->value.h), rbuf->type);
      break;
    case VT_INVALID:
    default:
    invalid:
      LOG_ERROR ("Invalid group code in rbuf: %d", rbuf->type)
    }

  num_xdata++;
  _obj->num_xdata = num_xdata;
  _obj->xdata_size = xdata_size;
  return pair;
}

// 350 or 360
static void
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
  while (pair != NULL && pair->code == 310 && pair->value.s)
    {
      unsigned len = strlen (pair->value.s);
      unsigned blen = len / 2;
      const char *pos = pair->value.s;
      BITCODE_TF s = &ent->preview[written];
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

// read to BLOCK_HEADER.preview (310), filling in the size
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
      LOG_ERROR ("Invalid %s.preview code %d, need 310", obj->name,
                 pair->code);
      return pair;
    }
  while (pair != NULL && pair->code == 310)
    {
      unsigned len = strlen (pair->value.s);
      unsigned blen = len / 2;
      const char *pos = pair->value.s;
      BITCODE_TF s;

      _obj->preview = realloc (_obj->preview, written + blen);
      s = &_obj->preview[written];
      for (unsigned i = 0; i < blen; i++)
        {
          sscanf (pos, "%2hhX", &s[i]);
          pos += 2;
        }
      written += blen;
      LOG_TRACE ("BLOCK_HEADER.preview += %u (%u)\n", blen, written);

      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  _obj->preview_size = written;
  LOG_TRACE ("BLOCK_HEADER.preview_size = %u [BL 0]\n", written);
  return pair;
}

// only need to process conflicting SPLINE DXF codes here. the rest is done via dynapi.
// TODO: also for HELIX (where SPLINE is a subclass), pass obj then.
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
      LOG_TRACE ("SPLINE.flag = %d [70]\n", flag);
      if (flag & 1)
        {
          _o->closed_b = 1;
          LOG_TRACE ("=> SPLINE.closed_b = 1 [B 0] (bit 0)\n");
        }
      if (flag & 2)
        {
          _o->periodic = 1;
          LOG_TRACE ("=> SPLINE.periodic = 1 [B 0] (bit 1)\n");
        }
      if (flag & 4)
        {
          _o->rational = 1;
          LOG_TRACE ("=> SPLINE.rational = 1 [B 0] (bit 2)\n");
        }
      if (flag & 16)
        {
          _o->weighted = 1;
          LOG_TRACE ("=> SPLINE.weighted = 1 [B 0] (bit 4)\n");
        }
      if (flag & 32)
        _o->scenario = 2; // bezier: planar
      else
        _o->scenario = 1;
      LOG_TRACE ("=> SPLINE.scenario = %d [BL 0]\n", _o->scenario);
      return 1; // found
    }
  else if (pair->code == 71)
    {
      _o->degree = pair->value.i;
      LOG_TRACE ("SPLINE.degree = %d [BL 71]\n", _o->degree);
      return 1; // found
    }
  else if (pair->code == 72 && _o->scenario & 1)
    {
      _o->num_knots = pair->value.i;
      *jp = 0;
      _o->knots = xcalloc (_o->num_knots, sizeof (BITCODE_BD));
      if (!_o->knots)
        {
          _o->num_knots = 0;
          return 0;
        }
      LOG_TRACE ("SPLINE.num_knots = %d [BS 72]\n", _o->num_knots);
      return 1; // found
    }
  else if (pair->code == 73 && _o->scenario & 1)
    {
      _o->num_ctrl_pts = pair->value.i;
      *jp = 0;
      _o->ctrl_pts
          = xcalloc (_o->num_ctrl_pts, sizeof (Dwg_SPLINE_control_point));
      if (!_o->ctrl_pts)
        {
          _o->num_ctrl_pts = 0;
          return 0;
        }
      LOG_TRACE ("SPLINE.num_ctrl_pts = %d [BS 73]\n", _o->num_ctrl_pts);
      return 1; // found
    }
  else if (pair->code == 74 && _o->scenario & 2)
    {
      _o->num_fit_pts = pair->value.i;
      *jp = 0;
      _o->fit_pts = xcalloc (_o->num_fit_pts, sizeof (BITCODE_3BD));
      if (!_o->fit_pts)
        {
          _o->num_fit_pts = 0;
          return 0;
        }
      _o->scenario = 2;
      _o->flag |= 1024;
      LOG_TRACE ("SPLINE.num_fit_pts = %d [BS 74]\n", _o->num_fit_pts);
      return 1; // found
    }
  else if (pair->code == 40 && _o->scenario & 1) // knots[] BD*
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
  else if (pair->code == 10 && _o->scenario & 1) // ctrl_pts[].x 3BD
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
  else if (pair->code == 20 && _o->scenario & 1) // ctrl_pts[].y 3BD
    {
      if (j >= (int)_o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", j,
                     _o->num_ctrl_pts);
          return 1; // found
        }
      _o->ctrl_pts[j].y = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 30 && _o->scenario & 1) // ctrl_pts[].z 3BD
    {
      if (j >= (int)_o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", j,
                     _o->num_ctrl_pts);
          return 1; // found
        }
      _o->ctrl_pts[j].z = pair->value.d;
      LOG_TRACE ("SPLINE.ctrl_pts[%d] = (%f, %f, %f) [3BD* 10]\n", *jp,
                 _o->ctrl_pts[j].x, _o->ctrl_pts[j].y, _o->ctrl_pts[j].z);
      j++;
      *jp = j;
      if (j == (int)_o->num_ctrl_pts)
        *jp = 0;
      return 1; // found
    }
  else if (pair->code == 41 && _o->scenario & 1) // ctrl_pts[].z 3BD
    { 
      if (j >= (int)_o->num_ctrl_pts)
        {
          LOG_ERROR ("SPLINE.ctrl_pts[%d] overflow, max %d", j,
                     _o->num_ctrl_pts);
          return 1; // found
        }
     _o->ctrl_pts[j].w = pair->value.d;
      LOG_TRACE ("SPLINE.ctrl_pts[%d].w = %f [BD* 41]\n", *jp,
                 _o->ctrl_pts[j].w);
      j++;
      *jp = j;
      return 1; // found
    }
  else if (pair->code == 11 && _o->scenario & 2) // fit_pts[].x 3BD
    {
      if (j >= _o->num_fit_pts)
        {
          LOG_ERROR ("SPLINE.fit_pts[%d] overflow, max %d", j,
                     _o->num_fit_pts);
          return 1; // found
        }
      _o->fit_pts[j].x = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 21 && _o->scenario & 2) // fit_pts[].y 3BD
    {
      if (j >= _o->num_fit_pts)
        {
          LOG_ERROR ("SPLINE.fit_pts[%d] overflow, max %d", j,
                     _o->num_fit_pts);
          return 1; // found
        }
      _o->fit_pts[j].y = pair->value.d;
      return 1; // found
    }
  else if (pair->code == 31 && _o->scenario & 2) // fit_pts[].z 3BD
    {
      if (j >= _o->num_fit_pts)
        {
          LOG_ERROR ("SPLINE.fit_pts[%d] overflow, max %d", j,
                     _o->num_fit_pts);
          return 1; // found
        }
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
      _o->verts = xcalloc (_o->num_verts, sizeof (Dwg_MLINE_vertex));
      if (!_o->verts)
        {
          _o->num_verts = 0;
          return 0;
        }
      LOG_TRACE ("MLINE.num_verts = %d [BS 72]\n", _o->num_verts);
      *jp = 0;
    }
  else if (pair->code == 73)
    {
      _o->num_lines = pair->value.i;
      for (int _j = 0; _j < _o->num_verts; _j++)
        {
          _o->verts[_j].lines
              = xcalloc (_o->num_lines, sizeof (Dwg_MLINE_line));
          if (!_o->verts[_j].lines)
            {
              _o->num_lines = 0;
              return 2;
            }
        }
      LOG_TRACE ("MLINE.num_lines = %d [BS 73]\n", _o->num_lines);
      *kp = 0;
    }
  else if (pair->code == 11 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].parent = _o;
      _o->verts[j].vertex.x = pair->value.d;
    }
  else if (pair->code == 21 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].vertex.y = pair->value.d;
    }
  else if (pair->code == 31 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].vertex.z = pair->value.d;
      LOG_TRACE ("MLINE.verts[%d] = (%f, %f, %f) [3BD* 11]\n", j,
                 _o->verts[j].vertex.x, _o->verts[j].vertex.y,
                 _o->verts[j].vertex.z);
    }
  else if (pair->code == 12 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].vertex_direction.x = pair->value.d;
    }
  else if (pair->code == 22 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].vertex_direction.y = pair->value.d;
    }
  else if (pair->code == 32 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].vertex_direction.z = pair->value.d;
      LOG_TRACE ("MLINE.vertex_direction[%d] = (%f, %f, %f) [3BD* 12]\n", j,
                 _o->verts[j].vertex_direction.x,
                 _o->verts[j].vertex_direction.y,
                 _o->verts[j].vertex_direction.z);
    }
  else if (pair->code == 13 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].miter_direction.x = pair->value.d;
    }
  else if (pair->code == 23 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      _o->verts[j].miter_direction.y = pair->value.d;
    }
  else if (pair->code == 33 && _o->num_verts)
    {
      if (j >= _o->num_verts || !_o->verts)
        {
          LOG_ERROR ("MLINE.verts[%d] out of bounds", j);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
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
      if (j >= _o->num_verts || k >= _o->num_lines || !_o->verts || !_o->verts[j].lines)
        {
          LOG_ERROR ("MLINE.verts[%d].lines[%d] out of bounds", j, k);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (k >= 0);
      _o->verts[j].lines[k].parent = &_o->verts[j];
      _o->verts[j].lines[k].num_segparms = pair->value.i;
      _o->verts[j].lines[k].segparms
          = xcalloc (pair->value.i, sizeof (BITCODE_BD));
      if (!_o->verts[j].lines[k].segparms)
        {
          _o->verts[j].lines[k].num_segparms = 0;
          return 2;
        }
      LOG_TRACE ("MLINE.v[%d].l[%d].num_segparms = %d [BS 74]\n", j, k,
                 pair->value.i);
      *lp = 0;
    }
  else if (pair->code == 41 && _o->num_lines)
    {
      if (j >= _o->num_verts || k >= _o->num_lines || !_o->verts || !_o->verts[j].lines)
        {
          LOG_ERROR ("MLINE.verts[%d].lines[%d] out of bounds", j, k);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      assert (k >= 0);
      assert (k < _o->num_lines);
      assert (l >= 0);
      assert (_o->verts[j].lines);
      assert (l < _o->verts[j].lines[k].num_segparms);
      _o->verts[j].lines[k].segparms[l] = pair->value.d;
      LOG_TRACE ("MLINE.v[%d].l[%d].segparms[%d] = %f [BD 41]\n", j, k, l,
                 pair->value.d);
      l++;
      *lp = l;
    }
  else if (pair->code == 75 && _o->num_lines)
    {
      if (j >= _o->num_verts || k >= _o->num_lines || !_o->verts || !_o->verts[j].lines)
        {
          LOG_ERROR ("MLINE.verts[%d].lines[%d] out of bounds", j, k);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      assert (k >= 0);
      assert (k < _o->num_lines);
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
      if (j >= _o->num_verts || k >= _o->num_lines || !_o->verts || !_o->verts[j].lines)
        {
          LOG_ERROR ("MLINE.verts[%d].lines[%d] out of bounds", j, k);
          return 2;
        }
      assert (_o->verts);
      assert (j >= 0);
      assert (j < _o->num_verts);
      assert (k >= 0);
      assert (k < _o->num_lines);
      assert (l >= 0);
      assert (l < _o->verts[j].lines[k].num_areafillparms);
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

// Also exported to in_json. To set list of children in POLYLINE_*/*INSERT
void
in_postprocess_SEQEND (Dwg_Object *restrict obj, BITCODE_BL num_owned, BITCODE_H *owned)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Entity_SEQEND *o = obj->tio.entity->tio.SEQEND;
  Dwg_Object *owner;
  Dwg_Entity_POLYLINE_2D *ow;
  const char *owhdls; // the name of the H*
  const char *firstfield;
  const char *lastfield;

  if (obj->fixedtype != DWG_TYPE_SEQEND)
    return;
  owner = dwg_ref_object (dwg, obj->tio.entity->ownerhandle);
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
  ow = owner->tio.entity->tio.POLYLINE_2D; // not the same layout for all possible owners
  if (!num_owned && !owned)
    {
      dwg_dynapi_entity_value (ow, owner->name, "num_owned", &num_owned, 0);
      dwg_dynapi_entity_value (ow, owner->name, owhdls, &owned, 0);
    }
  if (!num_owned)
    return;

  if (memBEGINc (owner->name, "POLYLINE_"))
    {
      firstfield = "first_vertex"; lastfield = "last_vertex";
    }
  else
    {
      firstfield = "first_attrib"; lastfield = "last_attrib";
    }
  // store all these fields, or just the ones for the requested version?
  if (dwg->header.version >= R_13 && dwg->header.version <= R_2000) // if downconvert to r2000
    {
      dwg_dynapi_entity_set_value (ow, owner->name, firstfield, &owned[0], 0);
      LOG_TRACE ("%s.%s = " FORMAT_REF "[H 0]\n", owner->name, firstfield,
                 ARGS_REF (owned[0]));
      dwg_dynapi_entity_set_value (ow, owner->name, lastfield,
                                   &owned[num_owned - 1], 0);
      LOG_TRACE ("%s.%s = " FORMAT_REF "[H 0]\n", owner->name, lastfield,
                 ARGS_REF (owned[num_owned - 1]));
    }
  else if (dwg->header.version >= R_2004 && !owned) // we cannot write r2004, TODO
    {
      BITCODE_H *first, *last;
      dwg_dynapi_entity_value (ow, owner->name, firstfield, &first, 0);
      dwg_dynapi_entity_value (ow, owner->name, lastfield, &last, 0);
      //..
      LOG_ERROR ("Cannot yet create the owner array from %s to %s", firstfield, lastfield)
    }
}

// see GH #138. add vertices / attribs
static void
dxf_postprocess_SEQEND (Dwg_Object *restrict obj)
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

  dwg_dynapi_entity_set_value (ow, owner->name, owhdls, &owned, 0);
  in_postprocess_SEQEND (obj, num_owned, owned);
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
          if (j < _ctrl->num_entries) // if last, skip move, realloc is enough
            /* 1 < 4 (was 5, i.e 0-4): 1, 2, 4-1-1: 2 */
            memmove (&_ctrl->entries[j], &_ctrl->entries[j + 1],
                     (_ctrl->num_entries - j - 1) * sizeof (BITCODE_H));
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
                     (_ctrl->num_entries - j - 1) * sizeof (BITCODE_H));
          _ctrl->entries = realloc (_ctrl->entries,
                                    _ctrl->num_entries * sizeof (BITCODE_H));
          break;
        }
    }
}

static void
postprocess_TEXTlike (Dwg_Object *obj)
{
  BITCODE_RC dataflags;
  BITCODE_2RD alignment_pt;
  BITCODE_RD oblique_ang, rotation, width_factor;
  BITCODE_BS generation, horiz_alignment, vert_alignment;
  BITCODE_H style;
  Dwg_Entity_TEXT *_obj = obj->tio.entity->tio.TEXT;

  dwg_dynapi_entity_value (_obj, obj->name, "dataflags", &dataflags, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "alignment_pt", &alignment_pt, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "oblique_ang", &oblique_ang, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "rotation", &rotation, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "width_factor", &width_factor, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "generation", &generation, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "horiz_alignment", &horiz_alignment, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "vert_alignment", &vert_alignment, NULL);
  dwg_dynapi_entity_value (_obj, obj->name, "style", &style, NULL);

  if (alignment_pt.x == 0.0 && alignment_pt.y == 0.0)
    dataflags |= 2;
  if (oblique_ang == 0.0)
    dataflags |= 4;
  if (rotation == 0.0)
    dataflags |= 8;
  if (width_factor == 0.0)
    dataflags |= 0x10;
  if (generation == 0)
    dataflags |= 0x20;
  if (horiz_alignment == 0)
    dataflags |= 0x40;
  if (vert_alignment == 0)
    dataflags |= 0x80;

  if (!style)
    {
      Dwg_Data *dwg = obj->parent;
      // set style to Standard (5.1.11)
      style = dwg_find_tablehandle_silent (dwg, "Standard", "STYLE");
      if (style)
        {
          if (style->handleref.code != 5)
            style = dwg_add_handleref (dwg, 5, style->absolute_ref, NULL);
          dwg_dynapi_entity_set_value (_obj, obj->name, "style", &style, 0);
          LOG_TRACE ("%s.style = " FORMAT_REF "\n", obj->name, ARGS_REF (style));
        }
    }
  dwg_dynapi_entity_set_value (_obj, obj->name, "dataflags", &dataflags, 0);
  LOG_TRACE ("%s.dataflags = 0x%x\n", obj->name, dataflags);
}

int
is_textlike (Dwg_Object *restrict obj)
{
  // has dataflags and common text fields
  return obj->fixedtype == DWG_TYPE_TEXT ||
    obj->fixedtype == DWG_TYPE_ATTDEF ||
    obj->fixedtype == DWG_TYPE_ATTRIB;
}

void
in_postprocess_handles (Dwg_Object *restrict obj)
{
  Dwg_Data *dwg = obj->parent;
  const char *name = obj->name;
  int is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;

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
              if (obj->type == DWG_TYPE_BLOCK
                  || (prev->type != DWG_TYPE_SEQEND
                      && prev->type != DWG_TYPE_ENDBLK))
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
          else if (obj->type == DWG_TYPE_BLOCK)
            {
              ent->nolinks = 0;
              ent->prev_entity = dwg_add_handleref (dwg, 4, 0, NULL);
              ent->next_entity = dwg_add_handleref (dwg, 4, 0, NULL);
              LOG_TRACE ("%s.prev_entity = next_entity = " FORMAT_REF "\n",
                         name, ARGS_REF (ent->prev_entity));
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
}

/* For tables, entities and objects.
 */
static Dxf_Pair *
new_object (char *restrict name, char *restrict dxfname,
            Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
            BITCODE_BL ctrl_id, BITCODE_BL i)
{
  const int is_utf = 1;
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
  unsigned written = 0;
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
        { // the biggest
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
              goto found_ent;                   \
            }                                   \
          else

          #include "objects.inc"
          //final else
          LOG_WARN ("Unknown object %s", name);

          #undef DWG_ENTITY
          #define DWG_ENTITY(token)
          // clang-format on
        found_ent:
          ;
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
                goto found_obj;             \
              }

          #include "objects.inc"

          #undef DWG_OBJECT
          #define DWG_OBJECT(token)
        found_obj:
          ;
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
              ctrlname[69] = '\0';
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
            dwg->num_objects--;
            LOG_ERROR ("Unknown DXF AcDbSymbolTableRecord %s, skipping", name);
            return pair;
          }
        }
    }

  if (!_obj)
    {
      dwg->num_objects--;
      LOG_ERROR ("Empty _obj at DXF AcDbSymbolTableRecord %s, skipping", name);
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
  // more DXF defaults
  if (obj->fixedtype == DWG_TYPE_LAYOUT)
    {
      Dwg_Object_LAYOUT *_o = obj->tio.object->tio.LAYOUT;
      _o->paper_units = 1.0; // default
    }
  else if (obj->fixedtype == DWG_TYPE_DIMSTYLE)
    {
      Dwg_Object_DIMSTYLE *_o = obj->tio.object->tio.DIMSTYLE;
      _o->DIMSCALE = _o->DIMLFAC = _o->DIMTFAC = 1.0; // default
      _o->DIMALTU = _o->DIMLUNIT = 2; // default
      _o->DIMFIT = 3;
      _o->DIMLWD = _o->DIMLWE = -2;
    }
  else if (obj->fixedtype == DWG_TYPE_TABLESTYLE)
    {
      Dwg_Object_TABLESTYLE *_o = obj->tio.object->tio.TABLESTYLE;
      _o->num_rowstyles = 3;
      _o->rowstyles = xcalloc (3, sizeof (Dwg_TABLESTYLE_rowstyles));
      if (!_o->rowstyles)
        {
          _o->num_rowstyles = 0;
          return NULL;
        }
      for (j = 0; j < 3; j++)
        {
          _o->rowstyles[j].borders = xcalloc (6, sizeof (Dwg_TABLESTYLE_border));
          _o->rowstyles[j].num_borders = 6;
          for (k = 0; k < 3; k++) // defaults: ByLayer
            {
              _o->rowstyles[j].borders[k].visible = 1;
              _o->rowstyles[j].borders[k].linewt = 29;
              _o->rowstyles[j].borders[k].color.index = 256;
            }
        }
      k = 0;
      j = 0;
    }
  /*
  else if (is_textlike (obj))
    {
      BITCODE_RC dataflags = 0x2 + 0x4 + 0x8;
      dwg_dynapi_entity_set_value (_obj, obj->name, "dataflags",
                                   &dataflags, 0);
    }
  */
  else if (obj->fixedtype == DWG_TYPE_MTEXT)
    {
      BITCODE_H style;
      Dwg_Entity_MTEXT *_o = obj->tio.entity->tio.MTEXT;
      _o->x_axis_dir.x = 1.0;
      // set style to Standard (5.1.11)
      style = dwg_find_tablehandle_silent (dwg, "Standard", "STYLE");
      if (style)
        {
          if (style->handleref.code != 5)
            style = dwg_add_handleref (dwg, 5, style->absolute_ref, NULL);
          _o->style = style;
        }
    }

  // read table fields until next 0 table or 0 ENDTAB
  while (pair != NULL && pair->code != 0)
    {
    start_loop:
      if (pair == NULL)
        {
          pair = dxf_read_pair (dat);
          DXF_RETURN_EOF (pair);
        }
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
            dxf_postprocess_SEQEND (obj);
          return pair;
        case 105: /* DIMSTYLE only for 5 */
          if (strNE (name, "DIMSTYLE"))
            goto object_default;
          // fall through
        case 5:
          {
            obj->handle.value = pair->value.u;
            // check for existing BLOCK_HEADER.*Model_Space
            if (obj->fixedtype == DWG_TYPE_BLOCK_HEADER
                && dwg->object[0].handle.value == pair->value.u
                && obj->tio.object->tio.BLOCK_HEADER
                       != dwg->object[0].tio.object->tio.BLOCK_HEADER
                && dwg->num_objects)
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

                if ((int)i < 0)
                  i = 0;
                dwg_dynapi_entity_value (_ctrl, ctrlname, "num_entries",
                                         &num_entries, NULL);
                if (i >= num_entries)
                  {
                    // DXF often lies about num_entries, skipping defaults
                    // e.g. BLOCK_CONTROL contains mspace+pspace in DXF, but in
                    // the DWG they are extra. But this is fixed at case 2, not here.
                    LOG_TRACE ("Misleading %s.num_entries %d for %dth entry\n",
                               ctrlname, num_entries, i);
                    i = num_entries;
                    num_entries++;
                    dwg_dynapi_entity_set_value (
                        _ctrl, ctrlname, "num_entries", &num_entries, 0);
                    LOG_TRACE ("%s.num_entries = %d [BL 70]\n", ctrlname,
                               num_entries);
                  }
                dwg_dynapi_entity_value (_ctrl, ctrlname, "entries", &hdls,
                                         NULL);
                if (!hdls)
                  {
                    hdls = xcalloc (num_entries, sizeof (Dwg_Object_Ref *));
                  }
                else
                  {
                    hdls = realloc (hdls,
                                    num_entries * sizeof (Dwg_Object_Ref *));
                  }
                if (pair->value.u && !hdls)
                  return NULL;
                hdls[i] = dwg_add_handleref (dwg, 2, pair->value.u, obj);
                dwg_dynapi_entity_set_value (_ctrl, ctrlname, "entries", &hdls,
                                             0);
                LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [H* 0]\n", ctrlname,
                           "entries", i, ARGS_REF (hdls[i]));
              }
          }
          break;
        case 8:
          if (is_entity && pair->value.s)
            {
              BITCODE_H handle = find_tablehandle (dwg, pair);
              if (!handle)
                {
                  obj_hdls = array_push (obj_hdls, "layer", pair->value.s,
                                         obj->tio.object->objid);
                  LOG_TRACE ("%s.layer: name %s -> H later\n", obj->name,
                             pair->value.s)
                }
              else
                {
                  dwg_dynapi_common_set_value (_obj, "layer", &handle, 1);
                  LOG_TRACE ("%s.layer = %s " FORMAT_REF " [H 8]\n", name,
                             pair->value.s, ARGS_REF (handle));
                }
              break;
            }
          // fall through
        case 100: // for nested structs
          if (pair->code == 100 && pair->value.s)
            {
              strncpy (subclass, pair->value.s, 79);
              subclass[79] = '\0';
              // set the real objname
              if (strEQc (obj->name, "DIMENSION_ANG2LN")
                  || strEQc (obj->name, "DIMENSION"))
                {
                  // we rather checked the flag before
                  if (strEQc (subclass, "AcDbRotatedDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_LINEAR;
                      obj->name = (char *)"DIMENSION_LINEAR";
                      obj->dxfname = strdup (obj->name);
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbAlignedDimension"))
                    {
                      // FIXME: could be DIMENSION_LINEAR also
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_ALIGNED;
                      obj->name = (char *)"DIMENSION_ALIGNED";
                      obj->dxfname = strdup (obj->name);
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbOrdinateDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_ORDINATE;
                      obj->name = (char *)"DIMENSION_ORDINATE";
                      obj->dxfname = strdup (obj->name);
                      strcpy (name, obj->name);
                      LOG_TRACE ("change type to %s\n", name);
                    }
                  else if (strEQc (subclass, "AcDbDiametricDimension"))
                    {
                      obj->type = obj->fixedtype = DWG_TYPE_DIMENSION_DIAMETER;
                      obj->name = (char *)"DIMENSION_DIAMETER";
                      obj->dxfname = strdup (obj->name);
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
              if (strEQc (obj->name, "DIMENSION_ALIGNED")
                  && strEQc (subclass, "AcDbRotatedDimension"))
                {
                  UPGRADE_ENTITY (DIMENSION_ALIGNED, DIMENSION_LINEAR)
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
              // with PERSSUBENTMANAGER
              else if (obj->fixedtype == DWG_TYPE_PERSSUBENTMANAGER
                       && strEQc (subclass, "AcDbPersSubentManager"))
                {
                  dxf_free_pair (pair);
                  pair = dxf_read_pair (dat);
                  pair = add_PERSSUBENTMANAGER (obj, dat, pair); // NULL for success
                  if (!pair)
                    goto next_pair;
                  else
                    goto start_loop; /* failure */
                }
              // with ASSOCDEPENDENCY or ACDBASSOCGEOMDEPENDENCY
              else if (strstr (obj->name, "ASSOC") &&
                       strstr (obj->name, "DEPENDENCY") &&
                       strEQc (subclass, "AcDbAssocDependency"))
                {
                  dxf_free_pair (pair);
                  pair = dxf_read_pair (dat);
                  pair = add_ASSOCDEPENDENCY (obj, dat, pair); // NULL for success
                  if (!pair)
                    goto next_pair;
                  else
                    goto start_loop; /* failure */
                }
              // with ASSOC2DCONSTRAINTGROUP, ASSOCNETWORK, ASSOCACTION
              else if (strstr (obj->name, "ASSOC")
                       && strEQc (subclass, "AcDbAssocAction"))
                {
                  dxf_free_pair (pair);
                  pair = dxf_read_pair (dat);
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
          if (pair->value.s && strEQc (pair->value.s, "{ACAD_XDICTIONARY"))
            in_xdict = 1;
          else if (pair->value.s && strEQc (pair->value.s, "{ACAD_REACTORS"))
            in_reactors = 1;
          else if (ctrl_id && pair->value.s && strEQc (pair->value.s, "{BLKREFS"))
            in_blkrefs = 1; // unique handle 331
          else if (pair->value.s && strEQc (pair->value.s, "}"))
            in_reactors = in_xdict = in_blkrefs = 0;
          else if (pair->value.s && strEQc (name, "XRECORD"))
            pair = add_xdata (dat, obj, pair);
          else
            LOG_WARN ("Unknown DXF code 102 %s in %s", pair->value.s, name)
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
                inserts = xcalloc (num_inserts, sizeof (BITCODE_H));
              if (num_inserts && !inserts)
                return NULL;
              dwg_dynapi_entity_set_value (_obj, obj->name, "inserts",
                                           &inserts, 0);
              hdl = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.inserts[%d] = " FORMAT_REF " [H* 331]\n",
                         obj->name, curr_inserts, ARGS_REF (hdl));
              inserts[curr_inserts++] = hdl;
              break;
            }
          else if (pair->code == 331 && obj->fixedtype == DWG_TYPE_LAYOUT)
            {
              Dwg_Object_LAYOUT *_o = obj->tio.object->tio.LAYOUT;
              _o->active_viewport = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("%s.active_viewport = " FORMAT_REF " [H 331]\n",
                         obj->name, ARGS_REF (_o->active_viewport));
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
          else if (pair->code == 330 && obj->fixedtype == DWG_TYPE_LAYOUT
                   && obj->tio.object->ownerhandle)
            {
              Dwg_Object_LAYOUT *_o = obj->tio.object->tio.LAYOUT;
              _o->block_header
                  = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              LOG_TRACE ("%s.block_header = " FORMAT_REF " [H 330]\n",
                         obj->name, ARGS_REF (_o->block_header));
            }
          // valid ownerhandle, if not XRECORD with an ownerhandle already
          else if (pair->code == 330
                   && (obj->fixedtype != DWG_TYPE_XRECORD
                       || !obj->tio.object->ownerhandle))
            {
              BITCODE_H owh;
              if (is_obj_absref (obj))
                owh = dwg_add_handleref (dwg, 4, pair->value.u, NULL);
              else // relative
                owh = dwg_add_handleref (dwg, 4, pair->value.u, obj);
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
          else if (pair->code == 360 && // hardowner, not soft
                   (obj->fixedtype == DWG_TYPE_IMAGE ||
                    obj->fixedtype == DWG_TYPE_WIPEOUT))
            {
              BITCODE_H ref = dwg_add_handleref (dwg, 3, pair->value.u, obj);
              dwg_dynapi_entity_set_value (_obj, obj->name, "imagedefreactor",
                                           ref, 0);
              LOG_TRACE ("%s.imagedefreactor = " FORMAT_REF " [H %d]\n", name,
                         ARGS_REF (ref), pair->code);
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
                                           &pair->value, 1);
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
                                           &pair->value, 1);
              LOG_TRACE ("%s.flag = %d [RC 70]\n", name, pair->value.i);
              break;
            }
          else if (pair->code == 70 && obj->fixedtype == DWG_TYPE_LAYOUT)
            {
              Dwg_Object_LAYOUT *_o = obj->tio.object->tio.LAYOUT;
              if (strEQc (subclass, "AcDbPlotSettings"))
                {
                  _o->plot_layout_flags = pair->value.i;
                  LOG_TRACE ("LAYOUT.plot_layout_flags = %d [BS 70]", pair->value.i);
                }
              else if (strEQc (subclass, "AcDbLayout"))
                {
                  _o->flag = pair->value.i;
                  LOG_TRACE ("LAYOUT.flag = %d [BS 70]", pair->value.i);
                }
              else
                {
                  LOG_WARN ("Unhandled LAYOUT.70 in subclass %s", subclass);
                  _o->flag = pair->value.i;
                  LOG_TRACE ("LAYOUT.flag = %d [BS 70]", pair->value.i);
                }
              break;
            }
          else if (pair->code == 70 && obj->fixedtype == DWG_TYPE_DIMENSION_ANG2LN)
            {
              Dwg_Entity_DIMENSION_ANG2LN *_o = obj->tio.entity->tio.DIMENSION_ANG2LN;
              _o->flag = _o->flag1 = pair->value.i;
              LOG_TRACE ("DIMENSION.flag = %d [RC 70]\n", pair->value.i);
              _o->flag1 &= 0xE0; /* clear the upper flag bits, and fix them: */
              _o->flag1 = (_o->flag1 & 1) ? _o->flag1 & 0x7F : _o->flag1 | 0x80;
              _o->flag1 = (_o->flag1 & 2) ? _o->flag1 | 0x20 : _o->flag1 & 0xDF;
              LOG_TRACE ("DIMENSION.flag1 => %d [RC]\n", _o->flag1);
              // Skip this flag logic, it is unreliable. Detecting subclasses is far better.
              switch (_o->flag & 31)
                {
                case 0: // rotated, horizontal or vertical
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_LINEAR");
                  // UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_LINEAR);
                  break;
                case 1:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_ALIGNED");
                  //UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ALIGNED);
                  break;
                case 2: // already?
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_ANG2LN");
                  //UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ANG2LN);
                  break;
                case 3:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_DIAMETER");
                  //UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_DIAMETER);
                  break;
                case 4:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_RADIUS");
                  //UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_RADIUS);
                  break;
                case 5:
                  LOG_TRACE ("Looks like %s\n", "DIMENSION_ANG3PT");
                  //UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ANG3PT);
                  break;
                case 6:
                  LOG_TRACE ("Looks like DIMENSION_LINEAR\n");
                  //UPGRADE_ENTITY (DIMENSION_ANG2LN, DIMENSION_ORDINATE);
                  break;
                default:
                  LOG_ERROR ("Invalid DIMENSION.flag %d", _o->flag & 31);
                  error |= DWG_ERR_INVALIDTYPE;
                  break;
                }
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
          else if (pair->code == 90 && obj->fixedtype == DWG_TYPE_OLE2FRAME)
            {
              Dwg_Entity_OLE2FRAME *_o = obj->tio.entity->tio.OLE2FRAME;
              _o->data_size = pair->value.l;
              _o->data = xcalloc (pair->value.l, 1);
              if (!_o->data)
                {
                  _o->data_size = 0;
                  return NULL;
                }
              LOG_TRACE ("OLE2FRAME.data_size = %ld [BL 90]\n", pair->value.l);
            }
          else if (pair->code == 90 && obj->fixedtype == DWG_TYPE_PERSSUBENTMANAGER)
            {
              pair = add_PERSSUBENTMANAGER (obj, dat, pair); // NULL for success
              if (!pair)
                goto next_pair;
              else
                goto start_loop; /* failure */
            }
          else if ((pair->code == 71 || pair->code == 75 || pair->code == 1) &&
                   obj->fixedtype == DWG_TYPE_DIMASSOC)
            {
              pair = add_DIMASSOC (obj, dat, pair);
              // returns with 0
              if (pair != NULL && pair->code == 0)
                goto start_loop;
              else
                goto search_field;
            }
          else if (pair->code == 310 && obj->fixedtype == DWG_TYPE_OLE2FRAME)
            {
              Dwg_Entity_OLE2FRAME *_o = obj->tio.entity->tio.OLE2FRAME;
              unsigned len = strlen (pair->value.s);
              unsigned blen = len / 2;
              const char *pos = pair->value.s;
              unsigned char *s = (unsigned char *)&_o->data[written];
              assert (_o->data);
              if (blen + written > _o->data_size)
                {
                  dxf_free_pair (pair);
                  LOG_ERROR ("OLE2FRAME.data overflow: %u + written %u > "
                             "data_size: %u",
                             blen, written, _o->data_size);
                  return NULL;
                }
              for (unsigned _i = 0; _i < blen; _i++)
                {
                  sscanf (pos, "%2hhX", &s[_i]);
                  pos += 2;
                }
              written += blen;
              LOG_TRACE ("OLE2FRAME.data += %u (%u/%u) [TF 310]\n", blen,
                         written, _o->data_size);
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
          else if (pair->code == 1 && obj->fixedtype == DWG_TYPE_LAYOUT)
            {
              Dwg_Object_LAYOUT *_o = obj->tio.object->tio.LAYOUT;
              if (strEQc (subclass, "AcDbPlotSettings"))
                {
                  dwg_dynapi_entity_set_value (_obj, obj->name, "page_setup_name",
                                               &pair->value, 1);
                  LOG_TRACE ("%s.page_setup_name = %s [T 1]\n",
                             obj->name, pair->value.s);
                }
              else if (strEQc (subclass, "AcDbLayout"))
                {
                  dwg_dynapi_entity_set_value (_obj, obj->name, "layout_name",
                                               &pair->value, 1);
                  LOG_TRACE ("%s.layout_name = %s [T 1]\n",
                             obj->name, pair->value.s);
                }
              else
                LOG_WARN ("Unhandled LAYOUT.1 in subclass %s", subclass);
              goto next_pair;
            }
          else if (pair->code == 370 && obj->fixedtype == DWG_TYPE_LAYER)
            {
              Dwg_Object_LAYER *layer = obj->tio.object->tio.LAYER;
              layer->linewt = dxf_find_lweight (pair->value.i);
              LOG_TRACE ("LAYER.linewt = %d\n", layer->linewt);
              layer->flag |= layer->linewt << 5;
              LOG_TRACE ("LAYER.flag = 0x%x [BS 70]\n", layer->flag);
            }
          else if (pair->code == 49 && obj->fixedtype == DWG_TYPE_LTYPE)
            {
              pair = add_LTYPE_dashes (obj, dat, pair);
              if (pair != NULL && pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (pair->code == 71 && obj->fixedtype == DWG_TYPE_MLINESTYLE
                   && pair->value.i != 0)
            {
              pair = add_MLINESTYLE_lines (obj, dat, pair);
              if (pair != NULL && pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (pair->code == 65 && obj->fixedtype == DWG_TYPE_VPORT)
            {
              Dwg_Object_VPORT *o = obj->tio.object->tio.VPORT;
              o->UCSVP = pair->value.i;
              o->UCSFOLLOW = o->VIEWMODE & 4 ? 1 : 0;
              o->VIEWMODE |= o->UCSVP;
              LOG_TRACE ("VPORT.UCSVP = %d [B 65]\n", o->UCSVP)
              LOG_TRACE ("VPORT.UCSFOLLOW => %d [B 0] (calc)\n", o->UCSFOLLOW)
              LOG_TRACE ("VPORT.VIEWMODE => %d [4BITS 71] (calc)\n", o->VIEWMODE)
              goto next_pair;
            }
          else if (pair->code == 90 && obj->fixedtype == DWG_TYPE_LWPOLYLINE)
            {
              pair = new_LWPOLYLINE (obj, dat, pair);
              if (pair != NULL && pair->code == 0)
                return pair;
              goto next_pair;
            }
          else if (pair->code == 350 && strEQc (subclass, "AcDb3dSolid"))
            {
              Dwg_Entity__3DSOLID *o = obj->tio.entity->tio._3DSOLID;
              BITCODE_H hdl = dwg_add_handleref (dwg, 5, pair->value.u, obj);
              LOG_TRACE ("%s.history_id = " FORMAT_REF " [H 350]\n",
                         obj->name, ARGS_REF (hdl));
              o->history_id = hdl;
            }
          else if (obj->fixedtype == DWG_TYPE_MLINE)
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
          else if (obj->fixedtype == DWG_TYPE_SPLINE)
            {
              if (pair->code == 210 || pair->code == 220 || pair->code == 230)
                break; // ignore extrusion in the dwg (planar only)
              if (add_SPLINE (obj->tio.entity->tio.SPLINE, dat, pair, &j,
                              &flag))
                goto next_pair;
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_HATCH)
            {
              if (pair->code == 10 || pair->code == 20)
                break; // elevation
              else if (pair->code == 91 || pair->code == 78
                       || pair->code == 453)
                {
                  pair = add_HATCH (obj, dat, pair);
                  if (!pair || pair->code == 0) // end or unknown
                    return pair;
                  goto search_field;
                }
              else
                goto search_field;
            }
          else if (is_textlike (obj))
            {
              BITCODE_RC dataflags;
              if (pair->code == 10 || pair->code == 20)
                goto search_field;
              else if (pair->code == 30 && pair->value.d == 0.0)
                {
                  dwg_dynapi_entity_value (_obj, obj->name, "dataflags",
                                           &dataflags, NULL);
                  dataflags |= 1;
                  LOG_TRACE ("%s.elevation 0.0 => dataflags = 0x%x\n",
                             obj->name, dataflags);
                  dwg_dynapi_entity_set_value (_obj, obj->name, "dataflags",
                                               &dataflags, 0);
                }
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_MESH)
            {
              if (pair->code == 91)
                {
                  pair = add_MESH (obj, dat, pair);
                  if (!pair || pair->code == 0) // end or unknown
                    return pair;
                  goto search_field;
                }
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_DBCOLOR)
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
                  //FIXME TU
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
                  if (dwg->header.version >= R_2007)
                    {
                      char *tmp = o->catalog;
                      o->catalog = (BITCODE_T)bit_utf8_to_TU (o->catalog);
                      free (tmp);
                      tmp = o->name;
                      o->name = (BITCODE_T)bit_utf8_to_TU (o->name);
                      free (tmp);
                    }
                  goto next_pair;
                }
              else
                goto search_field;
            }
          else if (obj->fixedtype == DWG_TYPE_MLEADERSTYLE)
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
          else if (obj->fixedtype == DWG_TYPE_MTEXT && pair->code == 50)
            {
              Dwg_Entity_MTEXT *o = obj->tio.entity->tio.MTEXT;
              if (!o->num_column_heights)
                o->num_column_heights = 1;
              if (!j)
                {
                  o->column_heights
                    = xcalloc (o->num_column_heights, sizeof (BITCODE_BD));
                  if (!o->column_heights)
                    {
                      o->num_column_heights = 0;
                      return NULL;
                    }
                }
              assert (j < (int)o->num_column_heights);
              o->column_heights[j] = pair->value.d;
              LOG_TRACE ("MTEXT.column_heights[%d] = %f [BD* 50]\n", j,
                         pair->value.d);
            }
          else if (obj->fixedtype == DWG_TYPE_VPORT && pair->code == 41)
            {
              Dwg_Object_VPORT *o = obj->tio.object->tio.VPORT;
              o->aspect_ratio = pair->value.d;
              o->viewwidth = o->aspect_ratio * o->VIEWSIZE;
              LOG_TRACE ("VPORT.aspect_ratio = %f [BD 41]\n", o->aspect_ratio);
              LOG_TRACE ("VPORT.viewwidth = %f [BD 0]\n", o->viewwidth);
            }
          else if (obj->fixedtype == DWG_TYPE_LEADER
                   && (pair->code == 10 || pair->code == 20
                       || pair->code == 30))
            {
              Dwg_Entity_LEADER *o = obj->tio.entity->tio.LEADER;
              if (!j && pair->code == 10)
                {
                  o->points = xcalloc (o->num_points, sizeof (BITCODE_3BD));
                  if (!o->points)
                    {
                      o->num_points = 0;
                      return NULL;
                    }
                }
              assert (j >= 0);
              assert (j < (int)o->num_points);
              assert (o->points);
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
              if (!pair)
                break;
              if (!fields)
                {
                  LOG_ERROR ("Illegal object name %s, no dynapi fields",
                             obj->name);
                  break;
                }
              for (f = &fields[0]; f->name; f++)
                {
                  LOG_INSANE ("-%s.%s [%d %s] vs %d\n", obj->name, f->name, f->dxf,
                              f->type, pair->code)
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
                          if (pair->code == 14 || pair->code == 24)
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
                                  clip_verts = xcalloc (num_clip_verts,
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
                                  memset (&clip_verts[j], 0, sizeof (BITCODE_2RD));
                                  dwg_dynapi_entity_set_value (_obj, obj->name,
                                                               f->name,
                                                               &clip_verts, 0);
                                }
                              if (j >= 0 && j < (int)num_clip_verts
                                  && clip_verts)
                                clip_verts[j].x = pair->value.d;
                            }
                          else if (pair->code < 30)
                            {
                              if (j >= 0 && j < (int)num_clip_verts
                                  && clip_verts)
                                {
                                  clip_verts[j].y = pair->value.d;
                                  LOG_TRACE (
                                      "%s.%s[%d] = (%f, %f) [2RD* %d]\n", name,
                                      "clip_verts", j, clip_verts[j].x,
                                      clip_verts[j].y, pair->code - 10);
                                  j++;
                                }
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
                               && pair->value.s
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
                            {
                              if (pair->code > 300)
                                {
                                  ref = dwg_add_handleref (dwg, 5,
                                                           pair->value.u, obj);
                                  LOG_TRACE ("%s.%s = " FORMAT_REF " [H %d]\n",
                                             name, f->name, ARGS_REF (ref),
                                             pair->code);
                                }
                              else if (pair->value.s)
                                {
                                  obj_hdls = array_push (
                                      obj_hdls, f->name, pair->value.s,
                                      obj->tio.object->objid);
                                  LOG_TRACE ("%s.%s: name %s -> H for code "
                                             "%d later\n", name, f->name,
                                             pair->value.s, pair->code);
                                }
                            }
                          else
                            {
                              dwg_dynapi_entity_set_value (
                                  _obj, obj->name, f->name, &ref, 1);
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
                          //pt.x = 0.0;
                          //if (pair->value.d == 0.0) // ignore defaults
                          //  goto next_pair;
                          pt.x = pair->value.d;
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &pt, 1);
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
                              color.flag |= 0x10;
                              if (dwg->header.version >= R_2007)
                                color.name = (BITCODE_T)bit_utf8_to_TU (pair->value.s);
                              else
                                color.name = strdup (pair->value.s);
                              LOG_TRACE ("%s.%s.name = %s [%s %d]\n", name,
                                         f->name, pair->value.s, "CMC", pair->code);
                            }
                          else if (pair->code < 450)
                            {
                              color.alpha = (pair->value.l & 0xFF000000) >> 24;
                              if (color.alpha)
                                color.alpha_type = 3;
                              LOG_TRACE ("%s.%s.alpha = %08X [%s %d]\n", name,
                                         f->name, pair->value.u, "CMC", pair->code);
                            }
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, f->name, &color, 1);
                          goto next_pair; // found, early exit
                        }
                      else
                        dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                     &pair->value, 1);
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
                      //pt.y = 0.0;
                      //if (pair->value.d == 0.0) // ignore defaults
                      //  goto next_pair;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name, &pt,
                                               NULL);
                      pt.y = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &pt, 1);
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
                      // can ignore z or 0.0? e.g. no VPORT.view_target
                      if (strNE (name, "_3DFACE") && strNE (f->name, "scale")
                          && *f->type == '2')
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
                          matrix = xcalloc (16, sizeof (BITCODE_BD));
                          if (!matrix)
                            return NULL;
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
              LOG_INSANE ("----\n")

              fields = is_entity ? dwg_dynapi_common_entity_fields ()
                                 : dwg_dynapi_common_object_fields ();
              for (f = &fields[0]; f->name; f++)
                {
                  LOG_INSANE ("-%s.%s [%d %s] vs %d\n", is_entity ? "ENTITY" : "OBJECT",
                              f->name, f->dxf, f->type, pair->code)
                  if ((pair->code == 62 || pair->code == 420
                       || pair->code == 430 || pair->code == 440)
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
                      else if (pair->code == 440)
                        {
                          color.flag |= 0x20;
                          color.alpha = (pair->value.l & 0xFF000000) >> 24;
                          color.alpha_type = pair->value.u >> 8;
                          if (color.alpha && !color.alpha_type)
                            color.alpha_type = 3;
                          LOG_TRACE ("COMMON.%s.alpha = %08X [%s %d]\n", f->name,
                                     pair->value.u, "CMC", pair->code);
                        }
                      else if (pair->code == 430)
                        {
                          color.flag |= 0x10;
                          if (dwg->header.version >= R_2007)
                            color.name = (BITCODE_T)bit_utf8_to_TU (pair->value.s);
                          else
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
                          if (is_entity && pair->code == 6 && pair->value.s
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
                      else if (pair->code == 310 && is_entity
                               && obj->tio.entity->preview_size
                               && obj->fixedtype > DWG_TYPE_LAYOUT
                               && strEQc (subclass, "AcDbEntity"))
                        {
                          // This would corrupt the previous preview chain,
                          // don't append
                          LOG_ERROR ("Skip duplicate/interrupted %s.preview",
                                     obj->name)
                          goto next_pair;
                        }
                      else
                        {
                          // Don't write a ptr twice. This will fuckup the num_
                          // counter. Just add to 310 preview, when prefixed by 92
                          if (f->is_malloc || f->is_string)
                            {
                              char *ptr = NULL;
                              if (dwg_dynapi_common_value (_obj, f->name, &ptr,
                                                           NULL)
                                  && ptr != NULL)
                                {
                                  LOG_ERROR ("Skip duplicate %s.%s [%s %d]", obj->name,
                                             f->name, f->type, pair->code)
                                  goto next_pair;
                                }
                            }
                          dwg_dynapi_common_set_value (_obj, f->name,
                                                       &pair->value, 1);
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
              LOG_INSANE ("----\n")
              // still needed? already handled above
              // not in dynapi: 92 as 310 size prefix for PROXY vector preview
              if ((pair->code == 92) && is_entity
                  && obj->fixedtype > DWG_TYPE_LAYOUT
                  && (strEQc (subclass, "AcDbEntity")
                      || strEQc (subclass, "AcDbProxyEntity")
                      || strstr (subclass, "Surface")))
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
                }
              else if (obj->fixedtype == DWG_TYPE_TABLESTYLE)
                {
                  // for the unknown subfields: 7, 140, ...
                  pair = add_TABLESTYLE (obj, dat, pair);
                  // returns with 0
                  if (pair && pair->code == 0)
                    goto start_loop;
                }
              else if (obj->fixedtype == DWG_TYPE_TABLEGEOMETRY)
                {
                  // for the unknown subfields: 93, 40, ...
                  pair = add_TABLEGEOMETRY_Cell (obj, dat, pair);
                  // returns with 0
                  if (pair && pair->code == 0)
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
              else if (pair->code == 70 && strEQc (name, "POLYLINE_PFACE"))
                ;
              else if (pair->code == 30 && strEQc (name, "POLYLINE_3D"))
                ;
              else if (obj->fixedtype == DWG_TYPE_PROXY_ENTITY && pair->code == 92)
                {
                  pair = add_ent_preview (obj, dat, pair);
                  goto start_loop;
                }
              else if (obj->fixedtype == DWG_TYPE_DIMENSION_ALIGNED && pair->code == 52)
                {
                  BITCODE_BD ang = deg2rad (pair->value.d);
                  free (obj->dxfname);
                  UPGRADE_ENTITY (DIMENSION_ALIGNED, DIMENSION_LINEAR)
                  dwg_dynapi_entity_set_value (_obj, "DIMENSION_LINEAR",
                                               "ext_line_rotation", &ang, 1);
                  LOG_TRACE ("%s.%s = %f (from DEG %f°) [%s %d]\n",
                             name, "ext_line_rotation", ang, pair->value.d,
                             "BD", 52);
                }
              else
                LOG_WARN ("Unknown DXF code %d for %s", pair->code, name);
            }
        }
    next_pair:
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      DXF_RETURN_EOF (pair);
    }

  if (obj->type == DWG_TYPE_SEQEND)
    dxf_postprocess_SEQEND (obj);
  // set defaults not in dxf:
  else if (obj->type == DWG_TYPE__3DFACE && dwg->header.version >= R_2000)
    {
      Dwg_Entity__3DFACE *o = obj->tio.entity->tio._3DFACE;
      o->has_no_flags = 1;
      LOG_TRACE ("_3DFACE.has_no_flags = 1 [B]\n");
    }
  else if (is_textlike (obj))
    postprocess_TEXTlike (obj);

  in_postprocess_handles (obj);

  return pair;
}

static int
dxf_tables_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char table[80];
  Dxf_Pair *pair = dxf_read_pair (dat);

  table[0] = '\0'; // init
  while (pair)     // read next 0 TABLE
    {
      if (pair->code == 0 && pair->value.s) // TABLE or ENDTAB
        {
          if (strEQc (pair->value.s, "TABLE"))
            table[0] = '\0'; // new table coming up
          else if (strEQc (pair->value.s, "BLOCK_RECORD"))
            {
              strncpy (table, pair->value.s, 79);
              table[79] = '\0';
            }
          else if (strEQc (pair->value.s, "ENDTAB"))
            table[0] = '\0'; // close table
          else if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            {
              LOG_ERROR ("Unknown 0 %s (%s)", pair->value.s, "tables");
              dxf_free_pair (pair);
              return 1;
            }
        }
      else if (pair->code == 2 && pair->value.s && strlen (pair->value.s) < 80
               && is_table_name (pair->value.s)) // new table NAME
        {
          BITCODE_BL i = 0;
          BITCODE_BL ctrl_id;
          strncpy (table, pair->value.s, 79);
          table[79] = '\0';
          pair = new_table_control (table, dat, dwg); // until 0 table
          ctrl_id = dwg->num_objects - 1;             // dwg->object might move
          while (pair && pair->code == 0 && pair->value.s
                 && strEQ (pair->value.s, table))
            {
              char *dxfname = strdup (pair->value.s);
              dxf_free_pair (pair);
              // until 0 table or 0 ENDTAB
              pair = new_object (table, dxfname, dat, dwg, ctrl_id, i++);
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
                  else if (dwg->header.version > R_2004 && _obj->name
                           && _obj->has_strings_area)
                    {
                      _obj->strings_area = xcalloc (512, 1);
                      if (!_obj->strings_area)
                        return DWG_ERR_OUTOFMEM;
                    }
                  if (dwg->header.version <= R_2004)
                    {
                      _obj->strings_area = xcalloc (256, 1);
                      if (!_obj->strings_area)
                        return DWG_ERR_OUTOFMEM;
                    }
                }
            }
          // next table
          // fixup entries vs num_entries (no NULL entries)
          {
            Dwg_Object *ctrl = &dwg->object[ctrl_id];
            Dwg_Object_BLOCK_CONTROL *_ctrl
              = ctrl->tio.object->tio.BLOCK_CONTROL;
            int at_end = 1;
            if (_ctrl)
              {
                for (int j = _ctrl->num_entries - 1; j >= 0; j--)
                  {
                    BITCODE_H ref = _ctrl->entries[j];
                    if (!ref)
                      {
                        if (at_end)
                          {
                            _ctrl->num_entries--;
                            _ctrl->entries = realloc (
                                _ctrl->entries,
                                _ctrl->num_entries * sizeof (BITCODE_H));
                            if (_ctrl->num_entries && !_ctrl->entries)
                              return DWG_ERR_OUTOFMEM;
                            LOG_TRACE ("%s.num_entries-- => %d\n", ctrl->name,
                                       _ctrl->num_entries);
                          }
                        else
                          {
                            _ctrl->entries[j]
                                = dwg_add_handleref (dwg, 2, 0, NULL);
                            LOG_TRACE ("%s.entries[%d] = (2.0.0)\n",
                                       ctrl->name, j);
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
  while (pair)    // read next 0 TABLE
    {
      if (pair != NULL && pair->code == 0 && pair->value.s)
        {
          BITCODE_BL i = 0;
          BITCODE_BB entmode = 0;
          while (pair != NULL && pair->code == 0 && pair->value.s
                 && strNE (pair->value.s, "ENDSEC"))
            {
              Dwg_Object *obj, *blkhdr = NULL;
              BITCODE_BL idx = dwg->num_objects;
              char *dxfname = strdup (pair->value.s);
              strncpy (name, dxfname, 79);
              name[79] = '\0';
              entity_alias (name);
              dxf_free_pair (pair);
              pair = new_object (name, dxfname, dat, dwg, 0, i++);
              obj = &dwg->object[idx];
              if (obj->type == DWG_TYPE_BLOCK)
                {
                  Dwg_Object_Entity *ent = obj->tio.entity;
                  Dwg_Entity_BLOCK *_obj = obj->tio.entity->tio.BLOCK;
                  i = 0;
                  if (ent->ownerhandle
                      && (blkhdr = dwg_ref_object (dwg, ent->ownerhandle)))
                    {
                      if (blkhdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
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
                      else if (blkhdr->fixedtype == DWG_TYPE_BLOCK_CONTROL)
                        {
                          Dwg_Object_BLOCK_CONTROL *_ctrl
                              = blkhdr->tio.object->tio.BLOCK_CONTROL;
                          ent->ownerhandle->obj = NULL; // still dirty
                          // TODO R2007+
                          if (!_ctrl->model_space && _obj->name
                              && strEQc (_obj->name, "*Model_Space"))
                            {
                              _ctrl->model_space = dwg_add_handleref (
                                  dwg, 3, obj->handle.value, blkhdr);
                              LOG_TRACE (
                                  "BLOCK_CONTROL.model_space = " FORMAT_REF
                                  " [H] (blocks)\n",
                                  ARGS_REF (_ctrl->model_space));
                            }
                          else if (!_ctrl->paper_space && _obj->name
                                   && strEQc (_obj->name, "*Paper_Space"))
                            {
                              _ctrl->paper_space = dwg_add_handleref (
                                  dwg, 3, obj->handle.value, blkhdr);
                              LOG_TRACE (
                                  "BLOCK_CONTROL.paper_space = " FORMAT_REF
                                  " [H] (blocks)\n",
                                  ARGS_REF (_ctrl->paper_space));
                            }
                        }
                    }
                  else
                    blkhdr = NULL;
                  if (!_obj || !_obj->name)
                    ;
                  // TODO R2007+
                  else if (strEQc (_obj->name, "*Model_Space"))
                    entmode = ent->entmode = 2;
                  else if (strEQc (_obj->name, "*Paper_Space"))
                    entmode = ent->entmode = 1;
                  else
                    entmode = 0;
                }
              else if (obj->type == DWG_TYPE_ENDBLK)
                {
                  Dwg_Object_Entity *ent = obj->tio.entity;
                  Dwg_Entity_BLOCK *_obj = obj->tio.entity->tio.BLOCK;
                  ent->entmode = entmode;
                  LOG_TRACE ("%s.entmode = %d [BB] (blocks)\n", obj->name,
                             entmode);
                  entmode = 0;
                  // set BLOCK_HEADER.endblk_entity handle
                  if ((blkhdr = dwg_ref_object (dwg, ent->ownerhandle))
                      && blkhdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
                    {
                      Dwg_Object_BLOCK_HEADER *_hdr
                          = blkhdr->tio.object->tio.BLOCK_HEADER;
                      ent->ownerhandle->obj = NULL; // still dirty
                      _hdr->endblk_entity = dwg_add_handleref (
                          dwg, 3, obj->handle.value, blkhdr);
                      LOG_TRACE ("BLOCK_HEADER.endblk_entity = " FORMAT_REF
                                 " [H] (blocks)\n",
                                 ARGS_REF (_hdr->endblk_entity));
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
                      && dwg->header.version < R_2004
                      && blkhdr->fixedtype == DWG_TYPE_BLOCK_HEADER)
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

static void
postprocess_BLOCK_HEADER (Dwg_Object *restrict obj,
                          Dwg_Object_Ref *restrict ownerhandle)
{
  Dwg_Data *dwg = obj->parent;
  Dwg_Object_BLOCK_HEADER *_ctrl;
  Dwg_Object *ctrl = dwg_ref_object (dwg, ownerhandle);

  if (!ctrl || ctrl->type != DWG_TYPE_BLOCK_HEADER)
    return;
  _ctrl = ctrl->tio.object->tio.BLOCK_HEADER;
  if (obj->type == DWG_TYPE_ENDBLK)
    {
      if (!_ctrl->endblk_entity)
        _ctrl->endblk_entity = dwg_add_handleref (dwg, 3, obj->handle.value, ctrl);
    }
  else if (obj->type == DWG_TYPE_BLOCK)
    {
      if (!_ctrl->block_entity)
        _ctrl->block_entity
            = dwg_add_handleref (dwg, 3, obj->handle.value, ctrl);
    }
  else if (!_ctrl->first_entity)
    _ctrl->first_entity = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
  else // always overwrite
    _ctrl->last_entity = dwg_add_handleref (dwg, 4, obj->handle.value, NULL);
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

  while (pair != NULL && pair->code == 0 && pair->value.s)
    {
      strncpy (name, pair->value.s, 79);
      name[79] = '\0';
      entity_alias (name);
      // until 0 ENDSEC
      while (pair != NULL
             && pair->code == 0
             && pair->value.s
             && (is_dwg_entity (name) || strEQc (name, "DIMENSION")))
        {
          char *dxfname = strdup (pair->value.s);
          dxf_free_pair (pair);
          pair = new_object (name, dxfname, dat, dwg, 0, 0);
          if (pair != NULL && pair->code == 0 && pair->value.s)
            {
              Dwg_Object *obj = &dwg->object[dwg->num_objects - 1];
              Dwg_Object_Entity *ent = obj->tio.entity;
              if (ent->ownerhandle)
                {
                  if (ent->ownerhandle->absolute_ref == mspace)
                    ent->entmode = 2;
                  else if (pspace && ent->ownerhandle->absolute_ref == pspace)
                    ent->entmode = 1;
                  postprocess_BLOCK_HEADER (obj, ent->ownerhandle);
                }
              else
                ent->entmode = 2;

              strncpy (name, pair->value.s, 79);
              name[79] = '\0';
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

static int
dxf_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  char name[80];
  Dxf_Pair *pair = dxf_read_pair (dat);
  while (pair != NULL)
    {
      while (pair != NULL && pair->code == 0 && pair->value.s)
        {
          strncpy (name, pair->value.s, 79);
          name[79] = '\0';
          object_alias (name);
          if (is_dwg_object (name))
            {
              char *dxfname = strdup (pair->value.s);
              dxf_free_pair (pair);
              pair = new_object (name, dxfname, dat, dwg, 0, 0);
            }
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
  while (pair)
    {
      while (pair && pair->code == 0 && pair->value.s)
        {
          char *dxfname = strdup (pair->value.s);
          strncpy (name, dxfname, 79);
          name[79] = '\0';
          object_alias (name);
          if (is_dwg_object (name))
            {
              dxf_free_pair (pair);
              pair = new_object (name, dxfname, dat, dwg, 0, 0);
            }
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

  while (pair)
    {
      switch (pair->code)
        {
        case 0: // ENDSEC
          dxf_free_pair (pair);
          return 0;
        case 90:
          dwg->thumbnail.size = pair->value.l; // INT32 => long
          dwg->thumbnail.chain = calloc (dwg->thumbnail.size, 1);
          if (!dwg->thumbnail.chain)
            {
              dxf_free_pair (pair);
              LOG_ERROR ("Out of memory");
              return DWG_ERR_OUTOFMEM;
            }
          LOG_TRACE ("PREVIEW.size = %ld\n", dwg->thumbnail.size);
          break;
        case 310:
          if (pair->value.s)
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
      if (!pair)
        break;
    }
  dxf_free_pair (pair);
  return 0;
}

// read to AcDs, TODO
static int
dxf_acds_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  //Dxf_Pair *pair = dxf_read_pair (dat);
  //unsigned written = 0;
  LOG_WARN ("SECTION %s ignored for now", "ACDSDATA");
  dxf_unknownsection_read (dat, dwg);
  return 0;
}

static void
resolve_postponed_header_refs (Dwg_Data *restrict dwg)
{
  Dwg_Header_Variables *vars = &dwg->header_vars;
  uint32_t i;
  LOG_TRACE ("resolve %d postponed header ref names:\n", header_hdls->nitems);
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
      else if (strEQc (field, "CMLSTYLE"))
        {
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "MLINESTYLE");
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

// i.e. layer or block name
static void
resolve_postponed_object_refs (Dwg_Data *restrict dwg)
{
  uint32_t i;
  LOG_TRACE ("resolve %d postponed object ref names:\n", obj_hdls->nitems);
  for (i = 0; i < obj_hdls->nitems; i++)
    {
      char *field = obj_hdls->items[i].field;
      Dxf_Pair p = { 0, VT_STRING };
      BITCODE_H hdl = NULL;
      int objid = obj_hdls->items[i].code;
      Dwg_Object *obj = &dwg->object[objid];
      int is_entity = obj->supertype == DWG_SUPERTYPE_ENTITY;

      p.value.s = obj_hdls->items[i].name;
      if (!p.value.s || !*p.value.s)
        {
          LOG_WARN ("%s.%s empty", obj->name, field);
          continue;
        }
      // TODO find field type => dxf code
      if (strEQc (field, "block_header"))
        hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "BLOCK");
      else if (strEQc (field, "style"))
        {
          p.code = 7;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "STYLE");
        }
      else if (strEQc (field, "dimstyle"))
        {
          p.code = 3;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "DIMSTYLE");
        }
      else if (is_entity && strEQc (field, "layer"))
        {
          p.code = 8;
          hdl = find_tablehandle (dwg, &p);
        }
      else if (is_entity && strEQc (field, "ltype"))
        {
          p.code = 6;
          hdl = find_tablehandle (dwg, &p);
        }
      else if (is_entity && strEQc (field, "material"))
        {
          p.code = 347;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "MATERIAL");
        }
      else if (is_entity && strEQc (field, "shadow"))
        {
          p.code = 361;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "SHADOW");
        }
      else if (is_entity && strEQc (field, "plotstyle"))
        {
          p.code = 390;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "PLOTSTYLENAME");
        }
      else if (is_entity && strEQc (field, "full_visualstyle"))
        {
          p.code = 348;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "VISUALSTYLE");
        }
      else if (is_entity && strEQc (field, "face_visualstyle"))
        {
          p.code = 348;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "VISUALSTYLE");
        }
      else if (is_entity && strEQc (field, "edge_visualstyle"))
        {
          p.code = 348;
          hdl = dwg_find_tablehandle_silent (dwg, p.value.s, "VISUALSTYLE");
        }
      else
        LOG_WARN ("missing code for %s", field)
      if (hdl)
        {
          Dwg_Object_APPID *_obj = obj->tio.object->tio.APPID;
          if (hdl->handleref.code != 5)
            hdl = dwg_add_handleref (dwg, 5, hdl->absolute_ref, NULL);
          if (p.code > 0)
            dwg_dynapi_common_set_value (_obj, field, &hdl, 0);
          else
            dwg_dynapi_entity_set_value (_obj, obj->name, field, &hdl, 0);
          LOG_TRACE ("%s.%s %s => " FORMAT_REF " [H %d]\n", obj->name, field,
                     p.value.s, ARGS_REF (hdl), (int)p.code);
        }
      else
        LOG_WARN ("Unknown %s.%s %s", obj->name, field, p.value.s)
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

static void
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

static void
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
  // const int minimal = dwg->opts & DWG_OPTS_MINIMAL;
  Dxf_Pair *pair;

  loglevel = dwg->opts & DWG_OPTS_LOGLEVEL;
  num_dxf_objs = 0;
  size_dxf_objs = 1000;
  dxf_objs = malloc (1000 * sizeof (Dxf_Objs));
  if (!dwg->object_map)
    dwg->object_map = hash_new (dat->size / 1000);
  // cannot rely on ref->obj during realloc's
  dwg->dirty_refs = 1;

  header_hdls = new_array_hdls (16);
  eed_hdls = new_array_hdls (16);
  obj_hdls = new_array_hdls (16);

  // start with the BLOCK_HEADER at objid 0
  if (!dwg->num_objects)
    {
      Dwg_Object *obj;
      Dwg_Object_BLOCK_HEADER *_obj;
      char *dxfname = strdup ((char*)"BLOCK_HEADER");
      NEW_OBJECT (dwg, obj);
      ADD_OBJECT (BLOCK_HEADER);
      // dwg->header.version probably here still unknown. <r2000: 0x17
      // later fixed up when reading $ACADVER and the BLOCK_HEADER.name
      if (dwg->header.version >= R_2007)
        _obj->name = (char*)bit_utf8_to_TU ((char*)"*Model_Space");
      else
        _obj->name = strdup ((char*)"*Model_Space");
      _obj->xrefref = 1;
      obj->tio.object->xdic_missing_flag = 1;
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
          if (!pair->value.s)
            {
              LOG_ERROR ("Expected SECTION string code 2, got code %d", pair->code);
              dxf_free_pair (pair);
              break;
            }
          else if (strEQc (pair->value.s, "HEADER"))
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
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"ByLayer", "LTYPE")))
                dwg->header_vars.LTYPE_BYLAYER
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              // should not happen
              if (!dwg->header_vars.LTYPE_BYBLOCK
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"ByBlock", "LTYPE")))
                dwg->header_vars.LTYPE_BYBLOCK
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              // but this is needed
              if (!dwg->header_vars.LTYPE_CONTINUOUS
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"Continuous", "LTYPE")))
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
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"*Paper_Space", "BLOCK")))
                dwg->header_vars.BLOCK_RECORD_PSPACE
                    = dwg_add_handleref (dwg, 5, hdl->handleref.value, NULL);
              if (!dwg->header_vars.BLOCK_RECORD_MSPACE
                  && (hdl = dwg_find_tablehandle_silent (
                          dwg, (char *)"*Model_Space", "BLOCK")))
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
          else if (strEQc (pair->value.s, "ACDSDATA"))
            {
              dxf_free_pair (pair);
              dxf_acds_read (dat, dwg);
            }
          else // if (strEQc (pair->value.s, ""))
            {
              LOG_WARN ("SECTION %s ignored for now", pair->value.s);
              dxf_free_pair (pair);
              dxf_unknownsection_read (dat, dwg);
            }
        }
    }

  resolve_postponed_header_refs (dwg);
  resolve_postponed_object_refs (dwg);              
  LOG_HANDLE ("Resolving pointers from ObjectRef vector:\n");
  dwg_resolve_objectrefs_silent (dwg);
  dwg->dirty_refs = 0;
  free_array_hdls (header_hdls);
  free_array_hdls (eed_hdls);
  free_array_hdls (obj_hdls);
  LOG_TRACE ("import from DXF\n");
  return dwg->num_objects ? 1 : 0;
}

int
dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  dwg->opts |= DWG_OPTS_DXFB; // binary
  dat->opts |= DWG_OPTS_DXFB;
  return dwg_read_dxf (dat, dwg);
}

#undef IS_INDXF

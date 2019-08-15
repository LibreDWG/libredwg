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
    LOG_ERROR ("%s: int overflow %ld (at %lu)", __FUNCTION__, num, dat->byte)
  return (int)num;
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
    case VT_INT64:
      pair->value.l = dxf_read_code (dat);
      LOG_TRACE ("  dxf (%d, %ld)\n", (int)pair->code, pair->value.l);
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
      // TODO convert %02X to string
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
      || (pair->code == 0 && strEQc (pair->value.s, "EOF")))                  \
  return 1

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

#define DXF_CHECK_ENDSEC                                                      \
  if (pair != NULL && (dat->byte >= dat->size || pair->code == 0))            \
  return 0
#define DXF_BREAK_ENDSEC                                                      \
  if (pair != NULL                                                            \
      && (dat->byte >= dat->size                                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC"))))          \
  break
#define DXF_RETURN_ENDSEC(what)                                               \
  if (pair != NULL)                                                           \
    {                                                                         \
      if (dat->byte >= dat->size                                              \
          || (pair->code == 0 && strEQc (pair->value.s, "ENDSEC")))           \
        {                                                                     \
          dxf_free_pair (pair);                                               \
          return what;                                                        \
        }                                                                     \
    }

static int
dxf_expect_code (Bit_Chain *restrict dat, Dxf_Pair *restrict pair, int code)
{
  while (pair->code != code)
    {
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      dxf_skip_comment (dat, pair);
      DXF_CHECK_EOF;
      if (pair->code != code)
        {
          LOG_ERROR ("Expecting DXF code %d, got %d (at %lu)", code,
                     pair->code, dat->byte);
        }
    }
  return 0;
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
      // 3BD or 3RD or 3DPOINT
      if (f->size == 24 && f->type[0] == '3')
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
  const int minimal = dwg->opts & 0x10;
  int is_utf = 1;
  int i = 0;

  // here SECTION(HEADER) was already consumed
  // read the first group 9, $field pair
  Dxf_Pair *pair = dxf_read_pair (dat);

  while (pair->code == 9)
    {
      char field[80];
      strcpy (field, pair->value.s);
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
                  dwg->header.version = v;
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
              LOG_ERROR ("skipping HEADER: 9 %s, unknown field with code %d",
                         field, pair->code);
            }
          else if (!matches_type (pair, f))
            {
              LOG_ERROR (
                  "skipping HEADER: 9 %s, wrong type code %d <=> field %s",
                  field, pair->code, f->type);
            }
          else if (pair->type == VT_POINT3D)
            {
              BITCODE_3BD pt = { 0.0, 0.0, 0.0 };
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
                  LOG_TRACE ("HEADER.%s [%s][%d]\n", &field[1], f->type, i);
                  dwg_dynapi_header_set_value (dwg, &field[1], &pt, is_utf);
                  i++;
                }
            }
          else if (pair->type == VT_STRING && strEQc (f->type, "H"))
            {
              char *key, *str;
              LOG_TRACE ("HEADER.%s %s [%s] %d later\n", &field[1], pair->value.s,
                         f->type, (int)pair->code);
              // name (which table?) => handle
              // needs to be postponed, because we don't have the tables yet.
              header_hdls = array_push (header_hdls, &field[1], pair->value.s,
                                        pair->code);
            }
          else if (strEQc (f->type, "H"))
            {
              BITCODE_H hdl;
              hdl = add_handleref (dwg, 0, pair->value.u, NULL);
              LOG_TRACE ("HEADER.%s %X [H]\n", &field[1], pair->value.u);
              dwg_dynapi_header_set_value (dwg, &field[1], &hdl, is_utf);
            }
          else if (strEQc (f->type, "CMC"))
            {
              static BITCODE_CMC color = { 0 };
              if (pair->code <= 70)
                {
                  LOG_TRACE ("HEADER.%s.index %d [CMC]\n", &field[1],
                             pair->value.i);
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
              LOG_TRACE ("HEADER.%s %f (%u, %u) [TIMEBLL]\n", &field[1],
                         date.value, date.days, date.ms);
              dwg_dynapi_header_set_value (dwg, &field[1], &date, 0);
            }
          else
            {
              LOG_TRACE ("HEADER.%s [%s]\n", &field[1], f->type);
              dwg_dynapi_header_set_value (dwg, &field[1], &pair->value,
                                           is_utf);
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

  dxf_free_pair (pair);
  if (_obj->DWGCODEPAGE && strEQc (_obj->DWGCODEPAGE, "ANSI_1252"))
    dwg->header.codepage = 30;
  return 0;
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
      while (pair->code != 0)
        { // read until next 0 CLASS
          switch (pair->code)
            {
            case 1:
              STRADD (klass->dxfname, pair->value.s);
              LOG_TRACE ("CLASS[%d].dxfname = %s [1]\n", i, pair->value.s);
              break;
            case 2:
              STRADD (klass->cppname, pair->value.s);
              LOG_TRACE ("CLASS[%d].cppname = %s [2]\n", i, pair->value.s);
              break;
            case 3:
              STRADD (klass->appname, pair->value.s);
              LOG_TRACE ("CLASS[%d].appname = %s [3]\n", i, pair->value.s);
              break;
            case 90:
              klass->proxyflag = pair->value.l;
              LOG_TRACE ("CLASS[%d].proxyflag = %ld [90]\n", i, pair->value.l);
              break;
            case 91:
              klass->num_instances = pair->value.l;
              LOG_TRACE ("CLASS[%d].num_instances = %ld [91]\n", i,
                         pair->value.l);
              break;
            case 280:
              klass->wasazombie = (BITCODE_B)pair->value.i;
              LOG_TRACE ("CLASS[%d].wasazombie = %d [280]\n", i,
                         pair->value.i);
              break;
            case 281:
              klass->item_class_id = pair->value.i ? 0x1f3 : 0x1f2;
              LOG_TRACE ("CLASS[%d].item_class_id = %x [281]\n", i,
                         klass->item_class_id);
              break;
            default:
              LOG_WARN ("Unknown DXF code for class[%d].%d", i, pair->code);
              break;
            }
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
        }
      DXF_RETURN_ENDSEC (0); // next class or ENDSEC
      dwg->num_classes++;
    }
  dxf_free_pair (pair);
  return 0;
}

// May need obj to shorten the code to relative offset, but not in header_vars.
int
add_handle (Dwg_Handle *restrict hdl, BITCODE_RC code, BITCODE_RL value,
            Dwg_Object *restrict obj)
{
  int offset = obj ? (value - (int)obj->handle.value) : 0;
  int i;
  unsigned char *val = (unsigned char *)&hdl->value;
  hdl->code = code;
  hdl->value = value;
  if (obj && !offset) // same obj
    {
      LOG_HANDLE ("object_map{%X} = %u\n", (unsigned)value, obj->index);
      hash_set (obj->parent->object_map, value, (uint32_t)obj->index);
    }

  // FIXME: little endian only
  for (i = 3; i >= 0; i--)
    if (val[i])
      break;
  hdl->size = i + 1;
  if (code != 5 && obj && abs (offset) == 1)
    {
      // change code to 6.0.0 or 8.0.0
      if (offset == 1)
        {
          hdl->code = 6;
          hdl->value = 0;
          hdl->size = 0;
        }
      else if (offset == -1)
        {
          hdl->code = 8;
          hdl->value = 0;
          hdl->size = 0;
        }
    }
  return 0;
}

Dwg_Object_Ref *
add_handleref (Dwg_Data *restrict dwg, BITCODE_RC code, BITCODE_RL value,
               Dwg_Object *restrict obj)
{
  Dwg_Object_Ref *ref = dwg_new_ref (dwg);
  add_handle (&ref->handleref, code, value, obj);
  ref->absolute_ref = value;
  // fill ->obj later
  return ref;
}

void
add_eed (Dwg_Object *restrict obj, const char *restrict name,
         Dxf_Pair *restrict pair)
{
  int i = obj->tio.object->num_eed; // same layout for Object and Entity
  int code, size, j = 0;
  Dwg_Eed *eed = obj->tio.object->eed;

  eed = (Dwg_Eed *)realloc (eed, (i + 1) * sizeof (Dwg_Eed));
  memset (&eed[i], 0, sizeof (Dwg_Eed));
  obj->tio.object->num_eed++;
  // handle: usually APPID "ACAD"
  code = pair->code - 1000; // 1000
  switch (code)
    {
    case 0:
      /* code [RC] + len+0 + length [RC] + codepage [RS] */
      size = 1 + strlen (pair->value.s) + 1 + 1 + 2;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1000
      eed[i].data->u.eed_0.length = strlen (pair->value.s);
      eed[i].data->u.eed_0.codepage = obj->parent->header.codepage;
      strcpy (eed[i].data->u.eed_0.string, pair->value.s);
      eed[i].size += size;
      break;
    case 2:
      /* code [RC] + byte [RC] */
      size = 1 + 1;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1002
      eed[i].data->u.eed_2.byte = (BITCODE_RC)pair->value.i;
      eed[i].size += size;
      break;
    case 4:
      /* code [RC] + len+0 + length [RC] */
      size = 1 + strlen (pair->value.s) + 1 + 1;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1004
      eed[i].data->u.eed_4.length = strlen (pair->value.s);
      strcpy (eed[i].data->u.eed_4.data, pair->value.s);
      eed[i].size += size;
      break;
    case 40:
      /* code [RC] + RD */
      size = 1 + 8;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_40.real = pair->value.d;
      eed[i].size += size;
      break;
    case 70:
      /* code [RC] + RS */
      size = 1 + 2;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_70.rs = pair->value.i;
      eed[i].size += size;
      break;
    case 71:
      /* code [RC] + RL */
      size = 1 + 4;
      eed[i].data = (Dwg_Eed_Data *)calloc (1, size);
      eed[i].data->code = code; // 1071
      eed[i].data->u.eed_71.rl = pair->value.l;
      eed[i].size += size;
      break;
    case 1:
      {
        if (strEQc (pair->value.s, "ACAD"))
          {
            // search in APPID table
            Dwg_Handle hdl = { 5, 1, 0x12 };
            size = sizeof (Dwg_Handle);
            add_handle (&hdl, 5, 12, NULL);
            memcpy (&eed[i].handle, &hdl, sizeof (hdl));
            eed[i].size += size;
            break;
          }
      }
      // fall through
    default:
      LOG_ERROR ("Not yet implemented EED.code %d", pair->code);
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
  char ctrl_hdlv[80];
  ctrl_hdlv[0] = '\0';

  NEW_OBJECT (dwg, obj);

  if (strEQc (name, "BLOCK_RECORD"))
    strcpy (ctrlname, "BLOCK_CONTROL");
  else
    {
      strcpy (ctrlname, name);
      strcat (ctrlname, "_CONTROL");
    }
  LOG_TRACE ("add %s\n", ctrlname);

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

  pair = dxf_read_pair (dat);
  // read common table until next 0 table or endtab
  while (pair->code != 0)
    {
      switch (pair->code)
        {
        case 0:
          return pair;
        case 5:
        case 105: // for DIMSTYLE
          {
            Dwg_Object_Ref *ref;
            char ctrlobj[80];
            add_handle (&obj->handle, 0, pair->value.u, obj);
            ref = add_handleref (dwg, 3, pair->value.u, obj);
            LOG_TRACE ("%s.handle = " FORMAT_H " [%d]\n", ctrlname,
                       ARGS_H (obj->handle), pair->code);
            // also set the matching HEADER.*_CONTROL_OBJECT
            strcpy (ctrlobj, ctrlname);
            strcat (ctrlobj, "_OBJECT");
            dwg_dynapi_header_set_value (dwg, ctrlobj, &ref, 0);
            LOG_TRACE ("HEADER.%s = " FORMAT_REF " [0]\n", ctrlobj,
                       ARGS_REF (ref));
          }
          break;
        case 100: // AcDbSymbolTableRecord, ... ignore
          break;
        case 102: // TODO {ACAD_XDICTIONARY {ACAD_REACTORS {BLKREFS
          break;
        case 330: // TODO: most likely {ACAD_REACTORS
          if (pair->value.u)
            {
              obj->tio.object->ownerhandle
                  = add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [330]\n", ctrlname,
                         ARGS_REF (obj->tio.object->ownerhandle));
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
              _o->morehandles[j] = add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.morehandles[%d] = " FORMAT_REF " [330]\n",
                         ctrlname, j, ARGS_REF (_o->morehandles[j]));
              j++;
            }
          break;
        case 360: // {ACAD_XDICTIONARY TODO
          obj->tio.object->xdicobjhandle
              = add_handleref (dwg, 0, pair->value.u, obj);
          LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [330]\n", ctrlname,
                     ARGS_REF (obj->tio.object->xdicobjhandle));
          break;
        case 70:
          if (pair->value.u)
            {
              BITCODE_H *hdls;
              BITCODE_BL num_entries = (BITCODE_BL)pair->value.u;
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_entries",
                                           &num_entries, is_utf);
              LOG_TRACE ("%s.num_entries = %u [70]\n", ctrlname, num_entries);
              hdls = calloc (num_entries, sizeof (Dwg_Object_Ref *));
              dwg_dynapi_entity_set_value (_obj, obj->name, ctrl_hdlv, &hdls,
                                           0);
              LOG_TRACE ("Add %s.%s[%d]\n", ctrlname, ctrl_hdlv, num_entries);
            }
          break;
        case 71:
          if (strEQc (name, "DIMSTYLE") && pair->value.u)
            {
              BITCODE_H *hdls;
              dwg_dynapi_entity_set_value (_obj, obj->name, "num_morehandles",
                                           &pair->value, is_utf);
              LOG_TRACE ("%s.num_morehandles = %u [71]\n", ctrlname,
                         pair->value.u)
              hdls = calloc (pair->value.u, sizeof (Dwg_Object_Ref *));
              dwg_dynapi_entity_set_value (_obj, obj->name, "morehandles",
                                           &hdls, 0);
              LOG_TRACE ("Add %s.morehandles[%d]\n", ctrlname, pair->value.u);
              break;
            }
          // fall through
        default:
          if (pair->code >= 1000 && pair->code < 1999)
            {
              add_eed (obj, ctrlname, pair);
            }
          else
            LOG_ERROR ("Unknown DXF code %d for %s", pair->code, ctrlname);
        }
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  return pair;
}

/* by name or by ref */
BITCODE_H
find_tablehandle (Dwg_Data *restrict dwg, Dxf_Pair *restrict pair)
{
  BITCODE_H handle = NULL;
  if (pair->code == 8)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "LAYER");
  else if (pair->code == 1) // $DIMBLK
    handle = dwg_find_tablehandle (dwg, pair->value.s, "BLOCK");
  else if (pair->code == 2) // some name: $DIMSTYLE, $UCSBASE, $UCSORTHOREF, $CMLSTYLE
    ; // not enough info, decide later
  else if (pair->code == 3)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "DIMSTYLE");
  // what is/was 4 and 5? VIEW? VPORT_ENTITY?
  else if (pair->code == 6)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "LTYPE");
  else if (pair->code == 7)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "STYLE");
  /* I think all these >300 are given by hex value, not by name */
  else if (pair->code > 300)
    {
      BITCODE_BL i;
      for (i = 0; i < dwg->num_object_refs; i++)
        {
          Dwg_Object_Ref *ref = dwg->object_ref[i];
          if (ref->absolute_ref == (BITCODE_BL)pair->value.u)
            {
              handle = ref;
              break;
            }
        }
      if (!handle)
        {
          Dwg_Object *obj = dwg_resolve_handle (dwg, (BITCODE_BL)pair->value.u);
          handle = add_handleref (dwg, 0, pair->value.u, obj);
        }
    }
#if 0
  else if (pair->code == 331)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "VPORT");
  else if (pair->code == 390)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "PLOTSTYLE");
  else if (pair->code == 347)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "MATERIAL");
  else if (pair->code == 345 || pair->code == 346)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "UCS");
  else if (pair->code == 361) // SUN
    handle = dwg_find_tablehandle (dwg, pair->value.s, "SHADOW");
  else if (pair->code == 340) // or TABLESTYLE or LAYOUT ...
    handle = dwg_find_tablehandle (dwg, pair->value.s, "STYLE");
  else if (pair->code == 342 || pair->code == 343)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "STYLE");
  else if (pair->code == 348)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "VISUALSTYLE");
  else if (pair->code == 332)
    handle = dwg_find_tablehandle (dwg, pair->value.s, "BACKGROUND");
#endif
  return handle;
}

// with XRECORD only
Dxf_Pair *
add_xdata (Bit_Chain *restrict dat,
           Dwg_Object *restrict obj, Dxf_Pair *restrict pair)
{
  BITCODE_BL num_xdata;
  // add pairs to xdata linked list
  Dwg_Resbuf *xdata, *rbuf;
  Dwg_Object_XRECORD *_obj = obj->tio.object->tio.XRECORD;

  dwg_dynapi_entity_value (_obj, obj->name, "num_xdata",
                           &num_xdata, 0);
  rbuf = calloc (1, sizeof (Dwg_Resbuf));
  if (num_xdata)
    {
      dwg_dynapi_entity_value (_obj, obj->name, "xdata",
                               &xdata, 0);
      rbuf->next = xdata;
    }
  else
    xdata = rbuf;

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
          LOG_TRACE ("rbuf[%d]: \"%s\" [%d]\n", num_xdata,
                     rbuf->value.str.u.data, rbuf->type);
        }
      LATER_VERSIONS
        {
          int length = rbuf->value.str.size = strlen (pair->value.s);
          if (length > 0)
            {
              rbuf->value.str.u.wdata = bit_utf8_to_TU (pair->value.s);
            }
        }
      break;
    case VT_REAL:
      rbuf->value.dbl = pair->value.d;
      LOG_TRACE ("rbuf[%d]: %f [%d]\n", num_xdata, rbuf->value.dbl,
                 rbuf->type);
      break;
    case VT_BOOL:
    case VT_INT8:
      rbuf->value.i8 = pair->value.i;
      LOG_TRACE ("rbuf[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i8,
                 rbuf->type);
      break;
    case VT_INT16:
      rbuf->value.i16 = pair->value.i;
      LOG_TRACE ("rbuf[%d]: %d [%d]\n", num_xdata, (int)rbuf->value.i16,
                 rbuf->type);
      break;
    case VT_INT32:
      rbuf->value.i32 = pair->value.l;
      LOG_TRACE ("rbuf[%d]: %ld [%d]\n", num_xdata, (long)rbuf->value.i32,
                 rbuf->type);
      break;
    case VT_INT64:
      rbuf->value.i64 = (BITCODE_BLL)pair->value.l;
      LOG_TRACE ("rbuf[%d]: " FORMAT_BLL " [%d]\n", num_xdata, rbuf->value.i64,
                 rbuf->type);
      break;
    case VT_POINT3D:
      rbuf->value.pt[0] = pair->value.d;
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
      rbuf->value.pt[1] = pair->value.d;
      dxf_free_pair (pair);
      { // if 30
        long pos = bit_position (dat);
        pair = dxf_read_pair (dat);
        if (get_base_value_type (pair->code) == VT_POINT3D)
          {
            rbuf->value.pt[2] = pair->value.d;
            LOG_TRACE ("rbuf[%d]: (%f,%f,%f) [%d]\n", num_xdata,
                       rbuf->value.pt[0], rbuf->value.pt[1], rbuf->value.pt[2],
                       rbuf->type);
          }
        else
          {
            bit_set_position (dat, pos); // reset stream
            rbuf->value.pt[2] = 0;
            LOG_TRACE ("rbuf[%d]: (%f,%f) [%d]\n", num_xdata,
                       rbuf->value.pt[0], rbuf->value.pt[1], rbuf->type);
          }
      }
      break;
    case VT_BINARY:
      // convert from hex
      {
        int i;
        int len = strlen (pair->value.s);
        int blen = len/2;
        char *s = malloc (blen);
        const char* pos = pair->value.s;
        rbuf->value.str.u.data = s;
        rbuf->value.str.size = blen;
        for (i = 0; i < blen; i++)
          {
            sscanf(pos, "%2hhX", &s[i]);
            pos += 2;
          }
        LOG_TRACE ("rbuf[%d]: ", num_xdata);
        //LOG_TRACE_TF (rbuf->value.str.u.data, rbuf->value.str.size);
      }
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      add_handle (&rbuf->value.h, 0, pair->value.u, obj);
      LOG_TRACE ("rbuf[%d]: " FORMAT_H " [H %d]\n", num_xdata,
                 ARGS_H (rbuf->value.h), rbuf->type);
      break;
    case VT_INVALID:
    default:
      LOG_ERROR ("Invalid group code in rbuf: %d", rbuf->type)
    }

  dwg_dynapi_entity_set_value (_obj, obj->name, "xdata",
                               &rbuf, 0);
  num_xdata++;
  dwg_dynapi_entity_set_value (_obj, obj->name, "num_xdata",
                               &num_xdata, 0);
  //TODO fixup num_databytes
  return pair;
}

void add_dictionary_handle (Dwg_Object *restrict obj, Dxf_Pair *restrict pair,
                            char *restrict text)
{
  // but not DICTIONARYVAR
  Dwg_Object_DICTIONARY *_obj = obj->tio.object->tio.DICTIONARY; // also DICTIONARYWDFLT
  Dwg_Data *dwg = obj->parent;
  BITCODE_BL num;
  BITCODE_H hdl;

  if (pair->code == 360)
    _obj->hard_owner = 1;
  num = _obj->numitems;
  hdl = add_handleref (dwg, 2, pair->value.u, obj);
  LOG_TRACE ("%s.itemhandles[%d] = " FORMAT_REF " [330 H]\n", obj->name,
             num, ARGS_REF (hdl));
  _obj->itemhandles = realloc (_obj->itemhandles, (num + 1) * sizeof (BITCODE_H));
  _obj->itemhandles[num] = hdl;
  _obj->texts = realloc (_obj->texts, (num + 1) * sizeof (BITCODE_TV));
  if (dwg->header.version >= R_2007)
    _obj->texts[num] = (char *)bit_utf8_to_TU (text);
  else
    _obj->texts[num] = strdup (text);
  LOG_TRACE ("%s.text[%d] = %s [3 T]\n", obj->name, num, text);
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

/* For tables, entities and objects.
 */
static Dxf_Pair *
new_object (char *restrict name, Bit_Chain *restrict dat,
            Dwg_Data *restrict dwg, Dwg_Object *restrict ctrl, BITCODE_BL i)
{
  int is_utf = dwg->header.version >= R_2007 ? 1 : 0;
  Dwg_Object *obj;
  Dxf_Pair *pair = dxf_read_pair (dat);
  Dwg_Object_APPID *_obj = NULL; // the smallest
  // we'd really need a Dwg_Object_TABLE or Dwg_Object_Generic type
  char ctrl_hdlv[80];
  char ctrlname[80];
  int in_xdict = 0;
  int in_reactors = 0;
  int in_blkrefs = 0;
  int is_entity = is_dwg_entity (name);
  BITCODE_BL rcount1, rcount2, rcount3, vcount;
  Bit_Chain *hdl_dat, *str_dat;
  int error = 0;
  BITCODE_RL curr_inserts = 0;
  char text[256];

  ctrl_hdlv[0] = '\0';
  if (ctrl || i)
    {
      LOG_TRACE ("add %s [%d]\n", name, i)
    }
  else
    {
      LOG_TRACE ("add %s\n", name)
    }

  if (is_entity)
    {
      NEW_ENTITY (dwg, obj);

      if (*name == '3')
        {
          // Looks dangerous but name[80] is big enough
          memmove (&name[1], &name[0], strlen (name) + 1);
          *name = '_';
        }

      // clang-format off
      // ADD_ENTITY by name
      // check all objects
      #undef DWG_ENTITY
      #define DWG_ENTITY(token)       \
        if (strEQc (name, #token))    \
          {                           \
            ADD_ENTITY (token);       \
          }

      #include "objects.inc"

      #undef DWG_ENTITY
      #define DWG_ENTITY(token)

      // clang-format on
    }
  else
    {
      NEW_OBJECT (dwg, obj);

      if (!ctrl) // no table
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
              strcpy (ctrlname, name);
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

  // read table fields until next 0 table or 0 ENDTAB
  while (pair->code != 0)
    {
      switch (pair->code)
        { // common flags: name, xrefref, xrefdep, ...
        case 0:
          return pair;
        case 105: /* DIMSTYLE only for 5 */
          if (strNE (name, "DIMSTYLE"))
            goto object_default;
          // fall through
        case 5:
          {
            add_handle (&obj->handle, 0, pair->value.u, obj);
            LOG_TRACE ("%s.handle = " FORMAT_H " [5 H]\n", name,
                       ARGS_H (obj->handle));
            if (*ctrl_hdlv)
              {
                // add to ctrl HANDLE_VECTOR "ctrl_hdlv"
                Dwg_Object_APPID_CONTROL *_ctrl
                    = ctrl->tio.object->tio.APPID_CONTROL;
                BITCODE_H *hdls = NULL;
                BITCODE_BL num_entries;
                dwg_dynapi_entity_value (_ctrl, ctrlname, "num_entries",
                                         &num_entries, NULL);
                if (num_entries <= i)
                  {
                    // DXF often lies about num_entries, skipping defaults
                    LOG_WARN ("Misleading %s.num_entries %d for %dth entry",
                              ctrlname, num_entries, i + 1);
                    num_entries = i + 1;
                    dwg_dynapi_entity_set_value (
                        _ctrl, ctrlname, "num_entries", &num_entries, 0);
                    LOG_TRACE ("%s.num_entries = %d [70 BL]\n", ctrlname,
                               num_entries);
                  }

                dwg_dynapi_entity_value (_ctrl, ctrlname, ctrl_hdlv, &hdls,
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
                hdls[i] = add_handleref (dwg, 2, pair->value.u, obj);
                dwg_dynapi_entity_set_value (_ctrl, ctrlname, ctrl_hdlv, &hdls,
                                             0);
                LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [0]\n", ctrlname,
                           ctrl_hdlv, i, ARGS_REF (hdls[i]));
              }
          }
          break;
        case 8:
          if (is_entity)
            {
              BITCODE_H handle = find_tablehandle (dwg, pair);
              if (!handle)
                {
                  LOG_WARN ("TODO resolve handle name %s %s",
                            "layer", pair->value.s)
                }
              else
                {
                  dwg_dynapi_common_set_value (_obj, "layer", &handle, is_utf);
                  LOG_TRACE ("%s.layer = %s " FORMAT_REF " [8 H]\n", obj->name,
                             pair->value.s, ARGS_REF (handle));
                }
              break;
            }
          // fall through
        case 100: // TODO for nested structs
          break;
        case 102:
          if (strEQc (pair->value.s, "{ACAD_XDICTIONARY"))
            in_xdict = 1;
          else if (strEQc (pair->value.s, "{ACAD_REACTORS"))
            in_reactors = 1;
          else if (ctrl && strEQc (pair->value.s, "{BLKREFS"))
            in_blkrefs = 1; // unique handle 331
          else if (strEQc (pair->value.s, "}"))
            in_reactors = in_xdict = in_blkrefs = 0;
          else if (strEQc (obj->name, "XRECORD"))
            pair = add_xdata (dat, obj, pair);
          else
            LOG_WARN ("Unknown DXF 102 %s in %s", pair->value.s, name)
          break;
        case 331:
          if (ctrl && in_blkrefs) // BLKREFS
            {
              BITCODE_H *insert_handles = NULL;
              BITCODE_RL num_inserts;
              dwg_dynapi_entity_value (_obj, obj->name, "num_inserts",
                                       &num_inserts, 0);
              if (curr_inserts)
                dwg_dynapi_entity_value (_obj, obj->name, "insert_handles",
                                         &insert_handles, 0);
              if (curr_inserts + 1 > num_inserts)
                {
                  LOG_WARN ("Misleading %s.num_inserts %d < %d", ctrlname,
                            num_inserts, curr_inserts + 1);
                  num_inserts = curr_inserts + 1;
                  dwg_dynapi_entity_set_value (_obj, obj->name, "num_inserts",
                                               &num_inserts, 0);
                }
              if (insert_handles)
                insert_handles
                  = realloc (insert_handles, num_inserts * sizeof (BITCODE_H));
              else
                insert_handles
                  = calloc (num_inserts, sizeof (BITCODE_H));
              dwg_dynapi_entity_set_value (_obj, obj->name, "insert_handles",
                                           &insert_handles, 0);
              insert_handles[curr_inserts++]
                  = add_handleref (dwg, 4, pair->value.u, obj);
              break;
            }
          // fall through
        case 330:
          if (in_reactors)
            {
              BITCODE_BL num = obj->tio.object->num_reactors;
              BITCODE_H reactor = add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.reactors[%d] = " FORMAT_REF " [330 H]\n", name,
                         num, ARGS_REF (reactor));
              obj->tio.object->reactors = realloc (
                  obj->tio.object->reactors, (num + 1) * sizeof (BITCODE_H));
              obj->tio.object->reactors[num] = reactor;
              obj->tio.object->num_reactors++;
            }
          else if (pair->value.u) // valid ownerhandle
            {
              obj->tio.object->ownerhandle
                  = add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [330 H]\n", name,
                         ARGS_REF (obj->tio.object->ownerhandle));
            }
          break;
        case 350: // DICTIONARY softhandle
        case 360: // {ACAD_XDICTIONARY or some hardowner
          if (pair->code == 360 && in_xdict)
            {
              obj->tio.object->xdicobjhandle
                = add_handleref (dwg, 3, pair->value.u, obj);
              obj->tio.object->xdic_missing_flag = 0;
              LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [360 H]\n", name,
                         ARGS_REF (obj->tio.object->xdicobjhandle));
              break;
            }
          // // DICTIONARY or DICTIONARYWDFLT, but not DICTIONARYVAR
          else if (memBEGINc (obj->name, "DICTIONARY") &&
                   strNE (obj->name, "DICTIONARYVAR"))
            {
              add_dictionary_handle (obj, pair, text);
              break;
            }
          // fall through
        case 2:
          if (ctrl && pair->code == 2)
            {
              dwg_dynapi_entity_set_value (_obj, obj->name, "name", &pair->value,
                                       is_utf);
              LOG_TRACE ("%s.name = %s [2 T]\n", name, pair->value.s);
              break;
            }
          // fall through
        case 70:
          if (ctrl && pair->code == 70)
            {
              dwg_dynapi_entity_set_value (_obj, obj->name, "flag", &pair->value,
                                       is_utf);
              LOG_TRACE ("%s.flag = %d [70 RC]\n", name, pair->value.i);
              break;
            }
          // fall through
        default:
        object_default:
          if (pair->code >= 1000 && pair->code < 1999)
            add_eed (obj, name, pair);
          else if (pair->code != 280 && strEQc (obj->name, "XRECORD"))
            pair = add_xdata (dat, obj, pair);
          else if (pair->code == 370 && strEQc (obj->name, "LAYER"))
            {
              Dwg_Object_LAYER *layer = obj->tio.object->tio.LAYER;
              layer->linewt = dxf_find_lweight(pair->value.i);
              LOG_TRACE ("LAYER.linewt = %d\n", layer->linewt);
              layer->flag |= layer->linewt << 5;
              LOG_TRACE ("LAYER.flag = 0x%x [70 BS]\n", layer->flag);
            }
          else
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
                  if (f->dxf == pair->code)
                    {
                      if (pair->code == 3 &&
                          memBEGINc (obj->name, "DICTIONARY") &&
                          strNE (obj->name, "DICTIONARYVAR"))
                        {
                          strncpy (text, pair->value.s, 255);
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
                          dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                       &ang, 0);
                          LOG_TRACE ("%s.%s = %f (from DEG %f°) [%d %s]\n", name,
                                     f->name, ang, pair->value.d, pair->code, f->type);
                          goto next_pair; // found
                        }
                      // resolve handle, by name or ref
                      else if (strEQc (f->type, "H"))
                        {
                          BITCODE_H handle = find_tablehandle (dwg, pair);
                          if (!handle)
                            {
                              if (pair->code > 300)
                                {
                                  LOG_WARN ("TODO resolve handle name %s.%s %X",
                                            obj->name, f->name, pair->value.u)
                                }
                              else
                                {
                                  LOG_WARN ("TODO resolve handle name %s.%s %s",
                                            obj->name, f->name, pair->value.s)
                                }
                            }
                          else
                            {
                              dwg_dynapi_entity_set_value (
                                  _obj, obj->name, f->name, &handle, is_utf);
                              LOG_TRACE ("%s.%s = " FORMAT_REF " [%d H]\n",
                                         name, f->name, ARGS_REF (handle),
                                         pair->code);
                            }
                          goto next_pair; // found
                        }
                      // only 2D or 3D points
                      else if (f->size > 8
                          && (strchr (f->type, '2') || strchr (f->type, '3')))
                        {
                          BITCODE_3BD pt;
                          pt.x = pair->value.d;
                          dwg_dynapi_entity_set_value (_obj, obj->name,
                                                       f->name, &pt, is_utf);
                          LOG_TRACE ("%s.%s.x = %f [%d %s]\n", name, f->name,
                                     pair->value.d, pair->code, f->type);
                        }
                      else if (f->size > 8 && strEQc (f->type, "CMC"))
                        {
                          BITCODE_CMC color;
                          dwg_dynapi_entity_value (_obj, obj->name, f->name,
                                                   &color, NULL);
                          if (pair->code < 100)
                            color.index = pair->value.i;
                          else if (pair->code < 430)
                            color.rgb = pair->value.l;
                          else if (pair->code < 440)
                            {
                              color.flag |= 1;
                              strcpy (color.name, pair->value.s);
                            }
                          else
                            {
                              color.alpha_type = 3;
                              color.alpha = pair->value.i;
                            }
                          dwg_dynapi_entity_set_value (
                              _obj, obj->name, f->name, &color, is_utf);
                          LOG_TRACE ("%s.%s = %d [%d %s]\n", name, f->name,
                                     pair->value.i, pair->code, "CMC");
                        }
                      else
                        dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                     &pair->value, is_utf);
                      if (f->is_string)
                        {
                          LOG_TRACE ("%s.%s = %s [%d %s]\n", name, f->name,
                                     pair->value.s, pair->code, f->type);
                        }
                      else if (strchr (&f->type[1], 'D'))
                        {
                          LOG_TRACE ("%s.%s = %f [%d %s]\n", name, f->name,
                                     pair->value.d, pair->code, f->type);
                        }
                      else
                        {
                          LOG_TRACE ("%s.%s = %ld [%d %s]\n", name, f->name,
                                     pair->value.l, pair->code, f->type);
                        }
                      goto next_pair; // found, early exit
                    }
                  else if ((*f->type == '3' || *f->type == '2')
                           && f->dxf + 10 == pair->code)
                    {
                      BITCODE_3DPOINT pt;
                      if (pair->value.d == 0.0)
                        goto next_pair;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name, &pt,
                                               NULL);
                      pt.y = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &pt, is_utf);
                      LOG_TRACE ("%s.%s.y = %f [%d %s]\n", name, f->name,
                                 pair->value.d, pair->code, f->type);
                      goto next_pair; // found, early exit
                    }
                  else if (*f->type == '3' && f->dxf + 20 == pair->code)
                    {
                      BITCODE_3DPOINT pt;
                      if (pair->value.d == 0.0)
                        goto next_pair;
                      dwg_dynapi_entity_value (_obj, obj->name, f->name, &pt,
                                               NULL);
                      pt.z = pair->value.d;
                      dwg_dynapi_entity_set_value (_obj, obj->name, f->name,
                                                   &pt, is_utf);
                      LOG_TRACE ("%s.%s.z = %f [%d %s]\n", name, f->name,
                                 pair->value.d, pair->code, f->type);
                      goto next_pair; // found, early exit
                    }
                }

              fields = is_entity ? dwg_dynapi_common_entity_fields ()
                                 : dwg_dynapi_common_object_fields ();
              for (f = &fields[0]; f->name; f++)
                {
                  if (f->dxf == pair->code) // TODO alt. color fields
                    {
                      /// resolve handle (table entry) given by name or ref
                      if (strEQc (f->type, "H"))
                        {
                          BITCODE_H handle = find_tablehandle (dwg, pair);
                          if (!handle)
                            {
                              if (pair->code > 300)
                                {
                                  LOG_WARN ("TODO resolve common handle name %s %X",
                                            f->name, pair->value.u)
                                }
                              else
                                {
                                  LOG_WARN ("TODO resolve common handle name %s %s",
                                            f->name, pair->value.s)
                                }
                            }
                          else
                            dwg_dynapi_common_set_value (_obj, f->name,
                                                         &handle, is_utf);
                        }
                      else
                        {
                          dwg_dynapi_common_set_value (_obj, f->name,
                                                       &pair->value, is_utf);
                          if (f->is_string)
                            {
                              LOG_TRACE ("COMMON.%s = %s [%d %s]\n", f->name,
                                         pair->value.s, pair->code, f->type);
                            }
                          else
                            {
                              LOG_TRACE ("COMMON.%s = %ld [%d %s]\n", f->name,
                                         pair->value.l, pair->code, f->type);
                            }
                          goto next_pair; // found, early exit
                        }
                    }
                }

              LOG_WARN ("Unknown DXF code %d for %s", pair->code, name);
            }
        }
    next_pair:
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
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
            strcpy (table, pair->value.s);
          else if (strEQc (pair->value.s, "ENDTAB"))
            table[0] = '\0'; // close table
          else if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            LOG_WARN ("Unknown 0 %s", pair->value.s);
        }
      if (pair->code == 2 && strlen (pair->value.s) < 80
          && is_table_name (pair->value.s)) // new table NAME
        {
          Dwg_Object *ctrl;
          BITCODE_BL i = 0;
          strcpy (table, pair->value.s);
          pair = new_table_control (table, dat, dwg); // until 0 table
          ctrl = &dwg->object[dwg->num_objects - 1];
          while (pair && pair->code == 0 && strEQ (pair->value.s, table))
            {
              // until 0 table or 0 ENDTAB
              pair = new_object (table, dat, dwg, ctrl, i++);
            }
        }
      DXF_RETURN_ENDSEC (0); // next TABLE or ENDSEC
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
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
          while (pair->code == 0 && strEQc (pair->value.s, "BLOCK"))
            {
              // until 0 BLOCK or 0 ENDBLK
              pair = new_object ((char *)"BLOCK", dat, dwg, NULL, i++);
            }
          if (strEQc (pair->value.s, "ENDBLK"))
            name[0] = '\0'; // close table
          else if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            LOG_WARN ("Unknown 0 %s", pair->value.s);
        }
      DXF_RETURN_ENDSEC (0); // next BLOCK or ENDSEC
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);

  while (1)
    {
      if (pair->code == 0)
        {
          char name[80];
          // until 0 ENDSEC
          while (pair->code == 0 && is_dwg_entity (pair->value.s))
            {
              strcpy (name, pair->value.s);
              pair = new_object (name, dat, dwg, NULL, 0);
            }
          if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            LOG_WARN ("Unknown 0 %s", pair->value.s);
        }
      DXF_RETURN_ENDSEC (0);
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);

  while (1)
    {
      if (pair->code == 0)
        {
          char name[80];
          // until 0 ENDSEC
          while (pair->code == 0 && is_dwg_object (pair->value.s))
            {
              strcpy (name, pair->value.s);
              pair = new_object (name, dat, dwg, NULL, 0);
            }
          if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            LOG_WARN ("Unknown 0 %s", pair->value.s);
        }
      DXF_RETURN_ENDSEC (0);
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_unknownsection_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  Dxf_Pair *pair = dxf_read_pair (dat);

  while (1)
    {
      if (pair->code == 0)
        {
          char name[80];
          // until 0 ENDSEC
          while (pair->code == 0 && is_dwg_object (pair->value.s))
            {
              strcpy (name, pair->value.s);
              pair = new_object (name, dat, dwg, NULL, 0);
            }
          if (strEQc (pair->value.s, "ENDSEC"))
            {
              dxf_free_pair (pair);
              return 0;
            }
          else
            LOG_WARN ("Unknown 0 %s", pair->value.s);
        }
      DXF_RETURN_ENDSEC (0);
      dxf_free_pair (pair);
      pair = dxf_read_pair (dat);
    }
  dxf_free_pair (pair);
  return 0;
}

static int
dxf_preview_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;
  // SECTION (THUMBNAILIMAGE);
  // VALUE_RL(pic->size, 90);
  // VALUE_BINARY(pic->chain, pic->size, 310);
  // ENDSEC ();
  return 0;
}

void resolve_postponed_header_refs (Dwg_Data *restrict dwg)
{
  uint32_t i;
  for (i = 0; i < header_hdls->nitems; i++)
    {
      char *field = header_hdls->items[i].field;
      Dxf_Pair p = { 0, VT_STRING };
      BITCODE_H hdl = NULL;
      p.value.s = header_hdls->items[i].name;
      if (!p.value.s || !*p.value.s)
        return;
      p.code = header_hdls->items[i].code;
      if (strEQc (p.value.s, "DIMSTYLE"))
        p.code = 3;
      else if (strstr (p.value.s, "UCS"))
        p.code = 345;
      hdl = find_tablehandle (dwg, &p);
      if (hdl)
        {
          dwg_dynapi_header_set_value (dwg, field, &hdl, 1);
          LOG_TRACE ("HEADER.%s %s => " FORMAT_REF " [H] %d\n", field,
                     p.value.s, ARGS_REF (hdl), (int)p.code);
        }
      else if (strstr (p.value.s, "CMLSTYLE"))
        {
          hdl = dwg_find_tablehandle (dwg, p.value.s, "MLSTYLE");
          if (hdl)
            {
              dwg_dynapi_header_set_value (dwg, field, &hdl, 1);
              LOG_TRACE ("HEADER.%s %s => " FORMAT_REF " [H] %d\n", field,
                         p.value.s, ARGS_REF (hdl), (int)p.code);
            }
        }
    }
}

int
dwg_read_dxf (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  const int minimal = dwg->opts & 0x10;
  Dxf_Pair *pair;
  // warn if minimal != 0
  // struct Dwg_Header *obj = &dwg->header;
  loglevel = dwg->opts & 0xf;

  num_dxf_objs = 0;
  size_dxf_objs = 1000;
  dxf_objs = malloc (1000 * sizeof (Dxf_Objs));
  if (!dwg->object_map)
    dwg->object_map = hash_new (dat->size / 1000);

  header_hdls = calloc (1, 8 + 16 * sizeof (struct array_hdl));
  header_hdls->size = 16;

  // start with the BLOCK_HEADER at objid 0
  if (!dwg->num_objects)
    {
      Dwg_Object *obj;
      Dwg_Object_BLOCK_HEADER *_obj;
      NEW_OBJECT (dwg, obj);
      ADD_OBJECT (BLOCK_HEADER);
    }

  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      dxf_expect_code (dat, pair, 0);
      DXF_CHECK_EOF;
      if (pair->type == VT_STRING && strEQc (pair->value.s, "SECTION"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          dxf_expect_code (dat, pair, 2);
          DXF_CHECK_EOF;
          if (strEQc (pair->value.s, "HEADER"))
            {
              dxf_free_pair (pair);
              dxf_header_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "CLASSES"))
            {
              dxf_free_pair (pair);
              dxf_classes_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "TABLES"))
            {
              dxf_free_pair (pair);
              dxf_tables_read (dat, dwg);
              resolve_postponed_header_refs (dwg);
            }
          else if (strEQc (pair->value.s, "BLOCKS"))
            {
              dxf_free_pair (pair);
              dxf_blocks_read (dat, dwg);
              resolve_postponed_header_refs (dwg);
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
            }
          /*
          else if (strEQc (pair->value.s, "THUMBNAIL"))
            {
              dxf_free_pair (pair);
              dxf_preview_read (dat, dwg);
            }
          */
          else // if (strEQc (pair->value.s, "ACDSDATA"))
            {
              LOG_WARN ("SECTION %s ignored for now", pair->value.s);
              dxf_free_pair (pair);
              dxf_unknownsection_read (dat, dwg);
            }
        }
    }
  return dwg->num_objects ? 1 : 0;
}

#undef IS_INDXF

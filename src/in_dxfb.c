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
 * in_dxfb.c: read Binary DXF to dwg WIP
 * written by Reini Urban
 *
 * See in_dxf.c for the plan. Only the input methods are customized here.
 */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <math.h> // trunc

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

static int
dxf_read_code (Bit_Chain *dat)
{
  char *endptr;
  long num = strtol ((char *)&dat->chain[dat->byte], &endptr, 10);
  dat->byte += (unsigned char *)endptr - &dat->chain[dat->byte];
  // dxf_skip_ws(dat);
  if (num > INT_MAX)
    LOG_ERROR ("%s: int overflow %ld (at %lu)", __FUNCTION__, num, dat->byte)
  return (int)num;
}

// TODO: binary strings are size prefixed
static void
dxfb_read_string (Bit_Chain *dat, char **string, int len)
{
  if (!len)
    len = strlen (buf);
  if (len)
    {
      if (!string)
        return; // ignore
      if (!*string)
        *string = malloc (len + 1);
      memcpy (*string, buf, len + 1);
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
      dxfb_read_string (dat, &pair->value.s, 0);
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
      // dxf_skip_ws(dat);
      sscanf ((char *)&dat->chain[dat->byte], "%lf", &pair->value.d);
      LOG_TRACE ("  dxf (%d, %f)\n", pair->code, pair->value.d);
      break;
    case VT_BINARY:
      // read into buf only?
      dxfb_read_string (dat, &pair->value.s, 0);
      // TODO convert %02X to string
      LOG_TRACE ("  dxf (%d, %s)\n", (int)pair->code, pair->value.s);
      break;
    case VT_HANDLE:
    case VT_OBJECTID:
      dxfb_read_string (dat, NULL, 0);
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

#define ACTION indxfb
//#define IS_ENCODER
//#define IS_DXF
#define IS_INDXF

#define DXF_CHECK_ENDSEC                                                      \
  if (dat->byte >= dat->size || pair->code == 0)                              \
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

static Dxf_Pair *
dxfb_expect_code (Bit_Chain *restrict dat, Dxf_Pair *restrict pair, int code)
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

// see
// https://www.autodesk.com/techpubs/autocad/acad2000/dxf/header_section_group_codes_dxf_02.htm
static int
dxfb_header_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
              LOG_TRACE ("HEADER.%s %s [%s] %d later\n", &field[1],
                         pair->value.s, f->type, (int)pair->code);
              // name (which table?) => handle
              // needs to be postponed, because we don't have the tables yet.
              header_hdls = array_push (header_hdls, &field[1], pair->value.s,
                                        pair->code);
            }
          else if (strEQc (f->type, "H"))
            {
              BITCODE_H hdl;
              hdl = dwg_add_handleref (dwg, 0, pair->value.u, NULL);
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
dxfb_classes_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
              LOG_TRACE ("CLASS[%d].num_instances = %d [280]\n", i,
                         pair->value.i);
              break;
            case 281:
              klass->item_class_id = pair->value.i ? 0x1f3 : 0x1f2;
              LOG_TRACE ("CLASS[%d].num_instances = %x [281]\n", i,
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
            dwg_add_handle (&obj->handle, 0, pair->value.u, obj);
            ref = dwg_add_handleref (dwg, 3, pair->value.u, obj);
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
                  = dwg_add_handleref (dwg, 4, pair->value.u, obj);
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
              _o->morehandles[j]
                  = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.morehandles[%d] = " FORMAT_REF " [330]\n",
                         ctrlname, j, ARGS_REF (_o->morehandles[j]));
              j++;
            }
          break;
        case 360: // {ACAD_XDICTIONARY TODO
          obj->tio.object->xdicobjhandle
              = dwg_add_handleref (dwg, 0, pair->value.u, obj);
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

/* Most of that code is object specific, not just for tables.
   TODO: rename to new_object, and special-case the table code.
   How to initialize _obj? To generic only?
 */
static Dxf_Pair *
new_object (char *restrict name, char *restrict dxfname,
            Bit_Chain *restrict dat, Dwg_Data *restrict dwg,
            Dwg_Object *restrict ctrl, BITCODE_BL i)
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
  LOG_TRACE ("add %s [%d]\n", name, i);
  NEW_OBJECT (dwg, obj);

  if (is_entity)
    {
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
            dwg_add_handle (&obj->handle, 0, pair->value.u, obj);
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
                hdls[i] = dwg_add_handleref (dwg, 2, pair->value.u, obj);
                dwg_dynapi_entity_set_value (_ctrl, ctrlname, ctrl_hdlv, &hdls,
                                             0);
                LOG_TRACE ("%s.%s[%d] = " FORMAT_REF " [0]\n", ctrlname,
                           ctrl_hdlv, i, ARGS_REF (hdls[i]));
              }
          }
          break;
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
                insert_handles = realloc (insert_handles,
                                          num_inserts * sizeof (BITCODE_H));
              else
                insert_handles = calloc (num_inserts, sizeof (BITCODE_H));
              dwg_dynapi_entity_set_value (_obj, obj->name, "insert_handles",
                                           &insert_handles, 0);
              insert_handles[curr_inserts++]
                  = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              break;
            }
          // fall through
        case 330:
          if (in_reactors)
            {
              BITCODE_BL num = obj->tio.object->num_reactors;
              BITCODE_H reactor
                  = dwg_add_handleref (dwg, 4, pair->value.u, obj);
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
                  = dwg_add_handleref (dwg, 4, pair->value.u, obj);
              LOG_TRACE ("%s.ownerhandle = " FORMAT_REF " [330 H]\n", name,
                         ARGS_REF (obj->tio.object->ownerhandle));
            }
          break;
        case 350: // DICTIONARY softhandle
        case 360: // {ACAD_XDICTIONARY or some hardowner
          if (pair->code == 360 && in_xdict)
            {
              obj->tio.object->xdicobjhandle
                  = dwg_add_handleref (dwg, 3, pair->value.u, obj);
              obj->tio.object->xdic_missing_flag = 0;
              LOG_TRACE ("%s.xdicobjhandle = " FORMAT_REF " [360 H]\n", name,
                         ARGS_REF (obj->tio.object->xdicobjhandle));
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
              LOG_TRACE ("GROUP.groups[%d] = " FORMAT_REF " [340 H]\n",
                         _o->num_groups, ARGS_REF (hdl));
              _o->groups = realloc (_o->groups,
                                    (_o->num_groups + 1) * sizeof (BITCODE_H));
              _o->groups[_o->num_groups] = hdl;
              _o->num_groups++;
              break;
            }
          // fall through
        case 2:
          if (ctrl && pair->code == 2)
            {
              dwg_dynapi_entity_set_value (_obj, obj->name, "name",
                                           &pair->value, is_utf);
              LOG_TRACE ("%s.name = %s [2 T]\n", name, pair->value.s);
              break;
            }
          // fall through
        case 70:
          if (ctrl && pair->code == 70)
            {
              dwg_dynapi_entity_set_value (_obj, obj->name, "flag",
                                           &pair->value, is_utf);
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
                      if (pair->code == 3)
                        {
                          strncpy (text, pair->value.s, 255);
                          text[255] = '\0';
                        }
                      // only 2D or 3D points
                      if (f->size > 8
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
                      /// resolve handle (table entry) given by name
                      if (strEQc (f->type, "H") && pair->type == VT_STRING)
                        {
                          BITCODE_H handle = find_tablehandle (dwg, pair);
                          if (!handle)
                            {
                              LOG_WARN ("TODO resolve handle name %s %s",
                                        f->name, pair->value.s)
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
dxfb_tables_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
              pair = new_object (table, pair->value.s, dat, dwg, ctrl, i++);
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
dxfb_blocks_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
              if (strEQc (obj->name, "BLOCK"))
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
                  if (strEQc (_obj->name, "*Model_Space"))
                    entmode = ent->entmode = 2;
                  else if (strEQc (_obj->name, "*Paper_Space"))
                    entmode = ent->entmode = 1;
                }
              else if (obj->type == DWG_TYPE_ENDBLK)
                {
                  obj->tio.entity->entmode = entmode;
                  LOG_TRACE ("%s.entmode = %d [BB] (blocks)\n", obj->name,
                             entmode);
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

static int
dxfb_entities_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
              pair = new_object (name, pair->value.s, dat, dwg, NULL, 0);
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
dxfb_objects_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
              pair = new_object (name, pair->value.s, dat, dwg, NULL, 0);
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
dxfb_unknownsection_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
              pair = new_object (name, pair->value.s, dat, dwg, NULL, 0);
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
dxfb_preview_read (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
{
  (void)dwg;
  // SECTION (THUMBNAILIMAGE);
  // VALUE_RL(pic->size, 90);
  // VALUE_BINARY(pic->chain, pic->size, 310);
  // ENDSEC ();
  return 0;
}

int
dwg_read_dxfb (Bit_Chain *restrict dat, Dwg_Data *restrict dwg)
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
      char *dxfname = (char *)"BLOCK_HEADER";
      NEW_OBJECT (dwg, obj);
      ADD_OBJECT (BLOCK_HEADER);
    }

  while (dat->byte < dat->size)
    {
      pair = dxf_read_pair (dat);
      pair = dxfb_expect_code (dat, pair, 0);
      DXF_CHECK_EOF;
      if (pair->type == VT_STRING && strEQc (pair->value.s, "SECTION"))
        {
          dxf_free_pair (pair);
          pair = dxf_read_pair (dat);
          pair = dxfb_expect_code (dat, pair, 2);
          DXF_CHECK_EOF;
          if (strEQc (pair->value.s, "HEADER"))
            {
              dxf_free_pair (pair);
              dxfb_header_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "CLASSES"))
            {
              dxf_free_pair (pair);
              dxfb_classes_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "TABLES"))
            {
              dxf_free_pair (pair);
              dxfb_tables_read (dat, dwg);
              resolve_postponed_header_refs (dwg);
            }
          else if (strEQc (pair->value.s, "BLOCKS"))
            {
              dxf_free_pair (pair);
              dxfb_blocks_read (dat, dwg);
              resolve_postponed_header_refs (dwg);
            }
          else if (strEQc (pair->value.s, "ENTITIES"))
            {
              dxf_free_pair (pair);
              dxfb_entities_read (dat, dwg);
            }
          else if (strEQc (pair->value.s, "OBJECTS"))
            {
              dxf_free_pair (pair);
              dxfb_objects_read (dat, dwg);
            }
          /*
          else if (strEQc (pair->value.s, "THUMBNAIL"))
            {
              dxf_free_pair (pair);
              dxfb_preview_read (dat, dwg);
            }
          */
          else // if (strEQc (pair->value.s, "ACDSDATA"))
            {
              LOG_WARN ("SECTION %s ignored for now", pair->value.s);
              dxf_free_pair (pair);
              dxfb_unknownsection_read (dat, dwg);
            }
        }
    }
  return dwg->num_objects ? 1 : 0;
}

#undef IS_INDXF

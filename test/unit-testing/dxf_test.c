/* ex: ft=c: -*- mode: c; -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/* compare against dxf values, generated from examples/unknown */
/* written by: Reini Urban */

#define DXF_TEST_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include "../../programs/my_stat.h"
#include "common.h"
#include "decode.h"
#include "tests_common.h"
#include "dwg.h"
#include "dwg_api.h"

int g_counter;
#define MAX_COUNTER 10
int g_max_count;
int g_all;

void object_alias (char *restrict name);
void entity_alias (char *restrict name);
ATTRIBUTE_MALLOC char *dwg_dynapi_subclass_name (const char *restrict type);
int dwg_dynapi_subclass_size (const char *restrict name);

#include "../../examples/unknown.h"

static struct _unknown_dxf unknown_dxf[] = {
// see log_unknown_dxf.pl
#include "../../examples/alldxf_0.inc"
  { NULL, NULL, 0, "", 0, 0, 0, 0, 0, 0, 0, NULL }
};

#include "../../examples/alldxf_1.inc"

static void
test_subclass (const Dwg_Data *restrict dwg, const void *restrict ptr,
               const struct _unknown_field *restrict f,
               const Dwg_DYNAPI_field *restrict fp,
               const char *restrict subclass, const char *restrict fieldname,
               const char *restrict key, int index)
{
  Dwg_DYNAPI_field field;
  enum RESBUF_VALUE_TYPE vtype;
  Dwg_Version_Type dwg_version = dwg->header.version;

  if (!ptr)
    {
      fail ("test_subclass %s.%s: empty ptr", subclass, key);
      return;
    }
  if (strEQc (fp->type, "CMC"))
    {
      BITCODE_CMC color;
      if (dwg_dynapi_subclass_value (ptr, subclass, key, &color, &field))
        {
          BITCODE_BS i = (BITCODE_BS)strtol (f->value, NULL, 10);
          if (i == color.index)
            {
              if (g_counter > g_max_count)
                pass ();
              else
                ok ("%s[%d].%s: %s", fieldname, index, key, f->value);
            }
          else if (field.type)
            fail ("%s[%d].%s: %d <=> \"%s\" [%s]", fieldname, index, key,
                  (int)color.index, f->value, field.type);
          else
            {
              if (g_counter > g_max_count)
                pass ();
              else
                ok ("%s[%d].%s: %d <=> \"%s\" [CMC] (TODO)", fieldname, index,
                    key, (int)color.index, f->value);
            }
        }
      return;
    }
  vtype = dwg_resbuf_value_type (f->code);
  if (vtype == DWG_VT_REAL && fp->size >= 16)
    goto DWG_VT_POINT3D;
  if (vtype == DWG_VT_INT8 && fp->size == 1 && strEQc (fp->type, "B"))
    goto DWG_VT_BOOL;
  if ((vtype == DWG_VT_BOOL || vtype == DWG_VT_INT16) && fp->size == 1
      && strEQc (fp->type, "RC"))
    goto DWG_VT_INT8;
  if (vtype == DWG_VT_INT8 && fp->size == 2)
    goto DWG_VT_INT16;
  if (vtype == DWG_VT_INT16 && fp->size == 4)
    goto DWG_VT_INT32;
  if (vtype == DWG_VT_INT32 && fp->size == 8)
    goto DWG_VT_INT64;
  switch (vtype)
    {
    case DWG_VT_STRING:
      {
        char *value;
        int isnew = 0;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            // convert to UTF8
            if (value && dwg_version >= R_2007
                && strNE (field.type, "TF")) /* not TF */
              {
                value = bit_convert_TU ((BITCODE_TU)value);
                if (!value) // some conversion error, invalid wchar (nyi)
                  {
                    fail ("%s[%d].%s: NULL [STRING %s]", fieldname, index, key,
                          field.type);
                  }
                else
                  {
                    isnew = 1;
                  }
              }
            if (!value || strEQ (value, f->value))
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %s", fieldname, index, key, value);
              }
            else
              fail ("%s[%d].%s: %s [STRING %s]", fieldname, index, key, value,
                    field.type);
          }
        if (isnew)
          free (value);
      }
      break;
    case DWG_VT_POINT3D:
    DWG_VT_POINT3D:
      {
        BITCODE_3BD pt;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &pt, &field))
          {
            double d = strtod (f->value, NULL);
            double ptv;
            int offset = f->code;
            if (strstr (field.type, "_1"))
              {
                while (offset > 10) // 10,11,12
                  offset -= 10;
                if (offset == 2 && field.size > 2 * sizeof (double))
                  ptv = pt.z;
                else if (offset == 1)
                  ptv = pt.y;
                else
                  ptv = pt.x;
              }
            else // 10/20/30
              {
                offset = offset % 100;
                if (offset >= 30 && field.size > 2 * sizeof (double))
                  ptv = pt.z;
                else if (offset >= 20)
                  ptv = pt.y;
                else
                  ptv = pt.x;
              }
            if (fabs (ptv - d) < 1e-6)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %f [%s %d]", fieldname, index, key, ptv,
                      field.type, f->code);
              }
            else
              fail ("%s[%d].%s: %f <=> \"%s\" [%s %d]", fieldname, index, key,
                    ptv, f->value, field.type, f->code);
          }
      }
      break;
    case DWG_VT_REAL:
      {
        double value;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            double d = strtod (f->value, NULL);
            if (f->code >= 50 && f->code < 59)
              d = deg2rad (d);
            if (fabs (value - d) < 1e-6)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %f", fieldname, index, key, value);
              }
            else
              fail ("%s[%d].%s: %f <=> \"%s\" [REAL %s]", fieldname, index,
                    key, value, f->value, field.type);
          }
      }
      break;
    case DWG_VT_BOOL:
    DWG_VT_BOOL:
      {
        BITCODE_B value;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            BITCODE_B i = (BITCODE_B)strtol (f->value, NULL, 10);
            if (i == value)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %d", fieldname, index, key, value);
              }
            else
              fail ("%s[%d].%s: %d <=> \"%s\" [BOOL %s]", fieldname, index,
                    key, value, f->value, field.type);
          }
      }
      break;
    case DWG_VT_INT8:
    DWG_VT_INT8:
      {
        BITCODE_RC value;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            BITCODE_RC i = (BITCODE_RC)strtol (f->value, NULL, 10);
            if (i == value)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %d", fieldname, index, key, value);
              }
            else if (field.type)
              fail ("%s[%d].%s: %d <=> \"%s\" [INT8 %s]", fieldname, index,
                    key, value, f->value, field.type);
            else
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %d <=> \"%s\" [INT8] (TODO)", fieldname,
                      index, key, value, f->value);
              }
          }
      }
      break;
    case DWG_VT_INT16:
    DWG_VT_INT16:
      {
        BITCODE_BS value;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            BITCODE_BS i = (BITCODE_BS)strtol (f->value, NULL, 10);
            if (i == value)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %d", fieldname, index, key, (int)value);
              }
            else if (field.type)
              fail ("%s[%d].%s: %d <=> \"%s\" [INT16 %s]", fieldname, index,
                    key, (int)value, f->value, field.type);
            else
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %d <=> \"%s\" [INT16] (TODO)", fieldname,
                      index, key, (int)value, f->value);
              }
          }
      }
      break;
    case DWG_VT_INT32:
    DWG_VT_INT32:
      {
        BITCODE_BL value;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            long l = strtol (f->value, NULL, 10);
            BITCODE_BL i = (BITCODE_BL)l;
            if (strEQc (key, "rgb") && i == (value & 0xffffff))
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: 0x%x", fieldname, index, key,
                      (unsigned)value);
              }
            else if (i == value)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %u", fieldname, index, key, (unsigned)value);
              }
            else if (field.type)
              fail ("%s[%d].%s: %u <=> \"%s\" [INT32 %s]", fieldname, index,
                    key, (unsigned)value, f->value, field.type);
            else
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %u <=> \"%s\" [INT32] (TODO)", fieldname,
                      index, key, (unsigned)value, f->value);
              }
          }
      }
      break;
    case DWG_VT_INT64:
    DWG_VT_INT64:
      {
        BITCODE_RLL value;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            BITCODE_RLL i = (BITCODE_RLL)strtol (f->value, NULL, 10);
            if (i == value)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %ld", fieldname, index, key, (long)value);
              }
            else if (field.type)
              fail ("%s[%d].%s: %ld <=> \"%s\" [INT64]", fieldname, index, key,
                    (long)value, f->value);
            else
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %ld <=> \"%s\" [INT64] (TODO)", fieldname,
                      index, key, (long)value, f->value);
              }
          }
      }
      break;
    case DWG_VT_BINARY:
      break;
    case DWG_VT_HANDLE:
      {
        BITCODE_H value;
        if (dwg_dynapi_subclass_value (ptr, subclass, key, &value, &field))
          {
            unsigned long l;
            sscanf (f->value, "%lX", &l);
            if (l == value->absolute_ref || l == value->handleref.value)
              {
                if (g_counter > g_max_count)
                  pass ();
                else
                  ok ("%s[%d].%s: %s", fieldname, index, key, f->value);
              }
            else
              fail ("%s[%d].%s: %lX <=> \"%s\" [H]", fieldname, index, key, l,
                    f->value);
          }
      }
      break;
    case DWG_VT_OBJECTID:
    case DWG_VT_INVALID:
    default:
      break;
    }
}

static int
DIMASSOC_index (const Dwg_Object *restrict obj, int sub_i)
{
  // check associativity bitmask for the index
  Dwg_Object_DIMASSOC *_obj = obj->tio.object->tio.DIMASSOC;
  while (!(_obj->associativity & (1 << sub_i)) && sub_i < 4)
    sub_i++;
  return sub_i;
}

static int
test_object (const Dwg_Data *restrict dwg, const Dwg_Object *restrict obj,
             const struct _unknown_dxf *restrict dxf,
             const char *restrict name)
{
  int isnew;
  const struct _unknown_field *f = dxf->fields;
  int sub_i = 0;
  char firstkey[80];

  *firstkey = '\0';
  g_counter++;

  // check all fields against dxf->fields
  for (; f->value; f++)
    {
      Dwg_DYNAPI_field field;
      const Dwg_DYNAPI_field *fp, *fp1;
      enum RESBUF_VALUE_TYPE vtype;
      if (!f->name || !*f->name)
        continue;
      // support subclass, as in in_json
      if (strchr (f->name, '.'))
        {
          char *subf = strdup (f->name);
          char *key = strchr (subf, '.');
          char *subclass;
          char *p;
          char *ptr;

          *key = '\0';
          key++;
          if (!*firstkey)
            {
              strcpy (firstkey, key);
              if (strEQc (name, "DIMASSOC"))
                sub_i = DIMASSOC_index (obj, sub_i);
            }
          else if (strEQ (key, firstkey)) // next index, like ref[1]
            {
              if (strEQc (name, "DIMASSOC"))
                {
                  sub_i = DIMASSOC_index (obj, sub_i + 1);
                  if (sub_i > 3)
                    break;
                }
              else
                sub_i++;
            }
          // unused
          if ((p = strchr (subf, '['))) // ref[0].osnap_type
            *p = '\0';
          // generalize. lookup type of subclass field
          if (!(fp1 = dwg_dynapi_entity_field (name, subf)))
            {
              free (subf);
              continue;
            }
          subclass = dwg_dynapi_subclass_name (fp1->type);
          if (!subclass)
            {
              free (subf);
              continue;
            }
          fp = dwg_dynapi_subclass_field (subclass, key);
          if (!fp)
            {
              free (subclass);
              free (subf);
              continue;
            }
          // embedded or reference?
          if (fp1->is_malloc) // vector
            {
              // ptr = ref[i].key
              int subsize = sub_i ? dwg_dynapi_subclass_size (subclass) : 0;
              ptr = *(char **)((char *)obj->tio.object->tio.APPID
                               + fp1->offset); // deref
              ptr += (sub_i * subsize);        // index offset
            }
          else
            { // embedded. no deref, and also no index offset. ptr = &ref.key
              ptr = &((char *)obj->tio.object->tio.APPID)[fp1->offset];
            }
          if (ptr)
            test_subclass (dwg, ptr, f, fp, subclass, subf, key, sub_i);
          free (subclass);
          free (subf);
          continue;
        }
      else if (!(fp = dwg_dynapi_entity_field (name, f->name)))
        continue;
      if (strEQc (fp->type, "CMC"))
        {
          BITCODE_CMC color;
          if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                       f->name, &color, &field))
            {
              BITCODE_BS i = (BITCODE_BS)strtol (f->value, NULL, 10);
              if (i == color.index)
                {
                  if (g_counter > g_max_count)
                    pass ();
                  else
                    ok ("%s.%s: %s", name, f->name, f->value);
                }
              else if (field.type)
                fail ("%s.%s: %d <=> \"%s\" [%s %d]", name, f->name,
                      (int)color.index, f->value, field.type, field.dxf);
              else
                {
                  if (g_counter > g_max_count)
                    pass ();
                  else
                    ok ("%s.%s: %d <=> \"%s\" [CMC] (TODO)", name, f->name,
                        (int)color.index, f->value);
                }
            }
          continue;
        }
      // TODO: inlined array support, as with subfields
      vtype = dwg_resbuf_value_type (f->code);
      if (vtype == DWG_VT_REAL && fp->size >= 16)
        goto DWG_VT_POINT3D;
      if (vtype == DWG_VT_INT8 && fp->size == 1 && strEQc (fp->type, "B"))
        goto DWG_VT_BOOL;
      if (vtype == DWG_VT_INT16 && fp->size == 1 && strEQc (fp->type, "RC"))
        goto DWG_VT_INT8;
      if (vtype == DWG_VT_INT8 && fp->size == 2)
        goto DWG_VT_INT16;
      if (vtype == DWG_VT_INT16 && fp->size == 4)
        goto DWG_VT_INT32;
      if (vtype == DWG_VT_INT16 && fp->size == 1)
        goto DWG_VT_INT8;
      if (vtype == DWG_VT_INT32 && fp->size == 8)
        goto DWG_VT_INT64;
      switch (vtype)
        {
        case DWG_VT_STRING:
          {
            char *value = NULL;
            if (fp->is_malloc
                && dwg_dynapi_entity_utf8text (obj->tio.object->tio.APPID,
                                               name, f->name, &value, &isnew,
                                               &field))
              {
                if (!value || strEQ (value, f->value))
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %s", name, f->name, value);
                  }
                else
                  fail ("%s.%s: %s [%s %d] STRING", name, f->name, value,
                        field.type, field.dxf);
              }
            if (isnew && value)
              free (value);
          }
          break;
        case DWG_VT_POINT3D:
        DWG_VT_POINT3D:
          {
            BITCODE_3BD pt;
            if (fp->is_malloc
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &pt, &field))
              {
                double d = strtod (f->value, NULL);
                double ptv;
                int offset = f->code;
                if (strstr (field.type, "_1"))
                  {
                    while (offset > 10) // 10,11,12
                      offset -= 10;
                    if (offset == 2 && field.size > 2 * sizeof (double))
                      ptv = pt.z;
                    else if (offset == 1)
                      ptv = pt.y;
                    else
                      ptv = pt.x;
                  }
                else // 10/20/30
                  {
                    offset = offset % 100;
                    if (offset >= 30 && field.size > 2 * sizeof (double))
                      ptv = pt.z;
                    else if (offset >= 20)
                      ptv = pt.y;
                    else
                      ptv = pt.x;
                  }
                if (fabs (ptv - d) < 1e-6)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %f [%s %d]", name, f->name, ptv, field.type,
                          f->code);
                  }
                else
                  fail ("%s.%s: %f <=> \"%s\" [%s %d]", name, f->name, ptv,
                        f->value, field.type, f->code);
              }
          }
          break;
        case DWG_VT_REAL:
          {
            double value;
            if (fp->is_malloc
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &field))
              {
                double d = strtod (f->value, NULL);
                if (f->code >= 50 && f->code < 59)
                  d = deg2rad (d);
                if (fabs (value - d) < 1e-6)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %f", name, f->name, value);
                  }
                else
                  fail ("%s.%s: %f <=> \"%s\" [%s %d] REAL", name, f->name,
                        value, f->value, field.type, field.dxf);
              }
          }
          break;
        case DWG_VT_BOOL:
        DWG_VT_BOOL:
          {
            BITCODE_B value;
            if (fp->is_malloc
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &field))
              {
                BITCODE_B i = (BITCODE_B)strtol (f->value, NULL, 10);
                if (i == value)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %d", name, f->name, value);
                  }
                else
                  fail ("%s.%s: %d <=> \"%s\" [%s %d] BOOL", name, f->name,
                        value, f->value, field.type, field.dxf);
              }
          }
          break;
        case DWG_VT_INT8:
        DWG_VT_INT8:
          {
            BITCODE_RC value;
            if (fp->is_malloc
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &field))
              {
                BITCODE_RC i = (BITCODE_RC)strtol (f->value, NULL, 10);
                if (i == value)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %d", name, f->name, value);
                  }
                else if (field.type)
                  fail ("%s.%s: %d <=> \"%s\" [%s %d] INT8", name, f->name,
                        value, f->value, field.type, field.dxf);
                else
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %d <=> \"%s\" INT8 (TODO)", name, f->name,
                          value, f->value);
                  }
              }
          }
          break;
        case DWG_VT_INT16:
        DWG_VT_INT16:
          {
            BITCODE_BS value;
            if (fp->is_malloc // vector of shorts, RS[] or BS[]
                && strlen (fp->type) == 2 && fp->type[1] == 'S'
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &field))
              {
                BITCODE_BS i = (BITCODE_BS)strtol (f->value, NULL, 10);
                if (i == value)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %d", name, f->name, (int)value);
                  }
                else if (field.type)
                  fail ("%s.%s: %d <=> \"%s\" [%s %d] INT16", name, f->name,
                        (int)value, f->value, field.type, field.dxf);
                else
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %d <=> \"%s\" INT16 (TODO)", name, f->name,
                          (int)value, f->value);
                  }
              }
          }
          break;
        case DWG_VT_INT32:
        DWG_VT_INT32:
          {
            BITCODE_BL value;
            if (fp->is_malloc // vector of longs, RL[] or BL[]
                && strlen (fp->type) == 2 && fp->type[1] == 'L'
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &field))
              {
                long l = strtol (f->value, NULL, 10);
                BITCODE_BL i = (BITCODE_BL)l;
                if (strEQc (f->name, "rgb") && i == (value & 0xffffff))
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: 0x%x", name, f->name, (unsigned)value);
                  }
                else if (i == value)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %u", name, f->name, (unsigned)value);
                  }
                else if (field.type)
                  fail ("%s.%s: %u <=> \"%s\" [%s %d] INT32", name, f->name,
                        (unsigned)value, f->value, field.type, field.dxf);
                else
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %u <=> \"%s\" INT32 (TODO)", name, f->name,
                          (unsigned)value, f->value);
                  }
              }
          }
          break;
        case DWG_VT_INT64:
        DWG_VT_INT64:
          {
            BITCODE_RLL value;
            if (fp->is_malloc // vector of LL's, RLL[] or BLL[]
                && strlen (fp->type) == 3 && fp->type[1] == 'L'
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &field))
              {
                BITCODE_RLL i = (BITCODE_RLL)strtol (f->value, NULL, 10);
                if (i == value)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %ld", name, f->name, (long)value);
                  }
                else if (field.type)
                  fail ("%s.%s: %ld <=> \"%s\" [%s %d] INT64", name, f->name,
                        (long)value, f->value, field.type, field.dxf);
                else
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %ld <=> \"%s\" INT64 (TODO)", name, f->name,
                          (long)value, f->value);
                  }
              }
          }
          break;
        case DWG_VT_BINARY:
          break;
        case DWG_VT_HANDLE:
          {
            BITCODE_H value;
            if (fp->is_malloc
                && dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &field))
              {
                unsigned long l;
                sscanf (f->value, "%lX", &l);
                if (l == value->absolute_ref || l == value->handleref.value)
                  {
                    if (g_counter > g_max_count)
                      pass ();
                    else
                      ok ("%s.%s: %s", name, f->name, f->value);
                  }
                else
                  fail ("%s.%s: %lX <=> \"%s\" [H %d]", name, f->name, l,
                        f->value, field.dxf);
              }
          }
          break;
        case DWG_VT_OBJECTID:
        case DWG_VT_INVALID:
        default:
          break;
        }
    }
  return failed;
}

static int
test_dxf (const struct _unknown_dxf *dxf, const char *restrict name,
          const char *restrict dwgfile)
{
  int error = 0;
  static char prev_dwgfile[256];
  static Dwg_Data dwg;
  BITCODE_BL i;

  loglevel = loglevel_from_env ();
  LOG_TRACE ("%s %X %s\n", dxf->name, dxf->handle, dwgfile);
  num = passed = failed = 0;

  if (dwg.num_objects && strEQ (dwgfile, prev_dwgfile))
    ;
  else
    {
      if (dwg.num_objects && dwg.header.version > R_INVALID)
        dwg_free (&dwg);
      dwg.opts = loglevel;
      if (dwg_read_file (dwgfile, &dwg) >= DWG_ERR_CRITICAL)
        {
          dwg_free (&dwg);
          return 1;
        }
    }
  strcpy (prev_dwgfile, dwgfile);

  // find the object
  for (i = 0; i < dwg.num_objects; i++)
    {
      if (dwg.object[i].handle.value == dxf->handle)
        {
          if (dwg.object[i].fixedtype >= DWG_TYPE_UNKNOWN_ENT)
            break;
          if (strNE (dwg.object[i].dxfname, dxf->name))
            LOG_WARN ("Invalid handle 0x%X for %s", dxf->handle, dxf->name)
          else
            error += test_object (&dwg, &dwg.object[i], dxf, name);
          break;
        }
    }
  /* This value is the return value for `main',
     so clamp it to either 0 or 1.  */
  return error ? 1 : 0;
}

int
main (int argc, char *argv[])
{
  int i = 1, error = 0;
  struct _unknown_dxf *dxf;
  char *class = NULL;
  char *file = NULL;
  char name[80];
  char olddxf[80];
  int big = 0;
  int is_docker = 0;
  char *docker;
  // clang-format off
  #include "../../examples/alldxf_2.inc"
  // clang-format on

  docker = getenv ("DOCKER");
  if (docker && strNE (docker, "0"))
    is_docker = 1;
  g_max_count = MAX_COUNTER;
  g_all = 0;
  name[0] = '\0';
  olddxf[0] = '\0';
  if (argc > 2 && !strcmp (argv[i], "--class"))
    {
      class = argv[++i];
      ++i;
    }
  if (argc - i >= 2 && !strcmp (argv[i], "--file"))
    {
      file = argv[++i];
      ++i;
    }
  if (argc - i >= 1 && !strcmp (argv[i], "-a"))
    {
      ++i;
      g_all = 1;
      g_max_count = 1000;
    }
  if (argc - i >= 1 && !strcmp (argv[i], "--big"))
    {
      ++i;
      big = 1;
    }

  g_counter = 0;
  for (dxf = &unknown_dxf[0]; dxf->name; dxf++)
    {
      const char *dxffile = dxf->dxf;
      struct stat attrib;
      size_t len = strlen (dxffile);
      char *dwgfile = strdup (dxffile);
      char *s = strrchr (dwgfile, '.');
      *(s + 2) = 'w';
      *(s + 3) = 'g';

      // display ok values only for the first 6 object types per file
      if (strNE (name, dxf->name) && strNE (olddxf, dxf->dxf))
        g_counter = 0;
      if (!big && strstr (dxffile, "/test-big/"))
        {
          free (dwgfile);
          continue;
        }

      strcpy (olddxf, dxf->dxf);
      strcpy (name, dxf->name);
      if (!is_dwg_object (name) && !is_dwg_entity (name))
        {
          object_alias (name);
          if (!is_dwg_object (name))
            {
              strcpy (name, dxf->name);
              entity_alias (name);
              if (!is_dwg_entity (name) && !class)
                {
                  free (dwgfile);
                  if (!g_counter) // use --enable-debug
                    LOG_WARN ("Unhandled %s", dxf->name)
                  continue;
                }
            }
        }
      if (class && strNE (class, name))
        {
          free (dwgfile);
          continue;
        }
      if (file && strNE (file, dwgfile))
        {
          free (dwgfile);
          continue;
        }
      // GH #268. skip 2018/Helix.dwg. podman works fine.
      if (is_docker && strEQ (dxffile, "test/test-data/2018/Helix.dxf"))
        {
          LOG_ERROR ("Skip %s in docker", dwgfile)
          free (dwgfile);
          continue;
        }
      if (stat (dwgfile, &attrib)) // not found
        {
          char path[256];
          char *top_srcdir = getenv ("top_srcdir");
          // fixup wrong alldxf_0.inc paths
          if (len > 3 && dwgfile[0] == '.' && dwgfile[1] == '.'
              && dwgfile[2] == '/')
            memmove (dwgfile, &dwgfile[3], len - 2); // include the final \0
          if (top_srcdir)
            {
              strncpy (path, top_srcdir, sizeof (path) - 1);
              strncat (path, "/", sizeof (path) - 1);
            }
          else
            strncpy (path, "../../../", sizeof (path) - 1);
          strncat (path, dwgfile, sizeof (path) - 1);
          if (stat (path, &attrib))
            LOG_WARN ("%s not found\n", path)
          else
            error += test_dxf (dxf, name, path);
        }
      else
        error += test_dxf (dxf, name, dwgfile);
      free (dwgfile);
    }
  // so far all unknown objects are debugging or unstable. ignore all errors
  return 0;
}

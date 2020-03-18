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
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "../../src/config.h"
#include "../../src/common.h"
#include "../../src/decode.h"
#include "dwg.h"
#include "dwg_api.h"
#include "tests_common.h"

void object_alias (char *restrict name);
void entity_alias (char *restrict name);

#include "../../examples/unknown.h"

static struct _unknown_dxf unknown_dxf[] = {
  // see log_unknown_dxf.pl
  #include "../../examples/alldxf_0.inc"
  { NULL, NULL, 0, "", 0, 0, 0, 0, 0, 0, 0, NULL }
};

#include "../../examples/alldxf_1.inc"

static int
test_object (const Dwg_Data *restrict dwg, const Dwg_Object *restrict obj,
             const struct _unknown_dxf *restrict dxf, const char *restrict name)
{
  int isnew;
  const struct _unknown_field *f = dxf->fields;

  // check all fields against dxf->fields
  for (; f->value; f++)
    {
      Dwg_DYNAPI_field field;
      const Dwg_DYNAPI_field *fp;
      enum RES_BUF_VALUE_TYPE vtype;
      if (!f->name || !*f->name)
        continue;
      if (!(fp = dwg_dynapi_entity_field (name, f->name)))
        continue;
      if (strEQc (fp->type, "CMC"))
        {
          BITCODE_CMC color;
          if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                       f->name, &color, &field))
            {
              BITCODE_BS i = (BITCODE_BS)strtol (f->value, NULL, 10);
              if (i == color.index)
                ok ("%s.%s: %s", name, f->name, f->value);
              else if (field.type)
                fail ("%s.%s: %d <=> %s [%s]", name, f->name,
                      (int)color.index, f->value, field.type);
              else
                ok ("%s.%s: %d <=> %s [CMC] (TODO)", name, f->name,
                    (int)color.index, f->value);
            }
          continue;
        }
      vtype = get_base_value_type (f->code);
      if (vtype == VT_REAL && (*fp->type == '2' || *fp->type == '3'))
        goto VT_POINT3D;
      switch (vtype)
        {
        case VT_STRING:
          {
            char *value;
            if (dwg_dynapi_entity_utf8text (obj->tio.object->tio.APPID, name,
                                            f->name, &value, &isnew, &field))
              {
                if (!value || strEQ (value, f->value))
                  ok ("%s.%s: %s", name, f->name, value);
                else
                  fail ("%s.%s: %s [STRING %s]", name, f->name, value, field.type);
              }
            if (isnew)
              free (value);
          }
          break;
        case VT_POINT3D:
        VT_POINT3D: {
            BITCODE_3BD pt;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
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
                  ok ("%s.%s: %f [%s %d]", name, f->name, ptv, field.type, f->code);
                else
                  fail ("%s.%s: %f <=> %s [%s %d]", name, f->name, ptv,
                        f->value, field.type, f->code);
              }
          }
          break;
        case VT_REAL:
          {
            double value;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                         f->name, &value, &field))
              {
                double d = strtod (f->value, NULL);
                if (fabs (value - d) < 1e-6)
                  ok ("%s.%s: %f", name, f->name, value);
                else
                  fail ("%s.%s: %f <=> %s [REAL %s]", name, f->name, value,
                        f->value, field.type);
              }
          }
          break;
        case VT_BOOL:
          {
            BITCODE_B value;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                         f->name, &value, &field))
              {
                BITCODE_B i = (BITCODE_B)strtol (f->value, NULL, 10);
                if (i == value)
                  ok ("%s.%s: %d", name, f->name, value);
                else
                  fail ("%s.%s: %d <=> %s [BOOL %s]", name, f->name, value,
                        f->value, field.type);
              }
          }
          break;
        case VT_INT8:
          {
            BITCODE_RC value;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                         f->name, &value, &field))
              {
                BITCODE_RC i = (BITCODE_RC)strtol (f->value, NULL, 10);
                if (i == value)
                  ok ("%s.%s: %d", name, f->name, value);
                else if (field.type)
                  fail ("%s.%s: %d <=> %s [INT8 %s]", name, f->name, value,
                        f->value, field.type);
                else
                  ok ("%s.%s: %d <=> %s [INT8] (TODO)", name, f->name, value,
                        f->value);
              }
          }
          break;
        case VT_INT16:
          {
            BITCODE_BS value;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                         f->name, &value, &field))
              {
                BITCODE_BS i = (BITCODE_BS)strtol (f->value, NULL, 10);
                if (i == value)
                  ok ("%s.%s: %d", name, f->name, (int)value);
                else if (field.type)
                  fail ("%s.%s: %d <=> %s [INT16 %s]", name, f->name,
                        (int)value, f->value, field.type);
                else
                  ok ("%s.%s: %d <=> %s [INT16] (TODO)", name, f->name, (int)value,
                      f->value);
              }
          }
          break;
        case VT_INT32:
          {
            BITCODE_BL value;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                         f->name, &value, &field))
              {
                long l = strtol (f->value, NULL, 10);
                BITCODE_BL i = (BITCODE_BL)l;
                if (strEQc (f->name, "rgb") && i == (value & 0xffffff))
                  ok ("%s.%s: 0x%x", name, f->name, (unsigned)value);
                else if (i == value)
                  ok ("%s.%s: %u", name, f->name, (unsigned)value);
                else if (field.type)
                  fail ("%s.%s: %u <=> %s [INT32 %s]", name, f->name, (unsigned)value,
                        f->value, field.type);
                else
                  ok ("%s.%s: %u <=> %s [INT32] (TODO)", name, f->name, (unsigned)value,
                        f->value);
              }
          }
          break;
        case VT_INT64:
          {
            BITCODE_BLL value;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                         f->name, &value, &field))
              {
                BITCODE_BLL i = (BITCODE_BLL)strtol (f->value, NULL, 10);
                if (i == value)
                  ok ("%s.%s: %ld", name, f->name, (long)value);
                else if (field.type)
                  fail ("%s.%s: %ld <=> %s [INT64]", name, f->name, (long)value, f->value);
                else
                  ok ("%s.%s: %ld <=> %s [INT64] (TODO)", name, f->name, (long)value,
                        f->value);
              }
          }
          break;
        case VT_BINARY:
          break;
        case VT_HANDLE:
          {
            BITCODE_H value;
            if (dwg_dynapi_entity_value (obj->tio.object->tio.APPID, name,
                                         f->name, &value, &field))
              {
                unsigned long l;
                sscanf (f->value, "%lX", &l);
                if (l == value->absolute_ref || l == value->handleref.value)
                  ok ("%s.%s: %s", name, f->name, f->value);
                else
                  fail ("%s.%s: %lX <=> %s [H]", name, f->name, (unsigned long)value, f->value);
              }
          }
          break;
        case VT_OBJECTID:
        case VT_INVALID:
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
  static char prev_dwgfile[128];
  static Dwg_Data dwg;
  BITCODE_BL i;
  char *trace;

  loglevel = 0;
  trace = getenv ("LIBREDWG_TRACE");
  if (trace)
    loglevel = atoi (trace);

  printf ("%s %X %s\n", dxf->name, dxf->handle, dwgfile);
  num = passed = failed = 0;
  dwg.opts = loglevel;

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
            fprintf (stderr, "Invalid handle 0x%X for %s\n", dxf->handle, dxf->name);
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
  // clang-format off
  #include "../../examples/alldxf_2.inc"
  // clang-format on

  if (argc > 2 && !strcmp (argv[i], "--class"))
    {
      class = argv[i + 1];
      i = 3;
    }
  if (argc - i >= 2 && !strcmp (argv[i], "--file"))
    file = argv[i + 1];

  for (dxf = &unknown_dxf[0]; dxf->name; dxf++)
    {
      char name[80];
      const char *dxffile = dxf->dxf;
      struct stat attrib;
      int len = strlen (dxffile);
      char *dwgfile = strdup (dxffile);
      char *s = strrchr (dwgfile, '.');
      *(s+2) = 'w';
      *(s+3) = 'g';

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
                  fprintf (stderr, "Unknown %s\n", dxf->name);
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

      if (stat (dwgfile, &attrib)) // not found
        {
          char path[80];
          char *top_srcdir = getenv ("top_srcdir");
          // fixup wrong alldxf_0.inc paths
          if (len > 3 && dwgfile[0] == '.' && dwgfile[1] == '.' && dwgfile[2] == '/')
            memmove (dwgfile, &dwgfile[3], len - 2); // include the final \0
          if (top_srcdir)
            {
              strcpy (path, top_srcdir);
              strcat (path, "/");
            }
          else
            strcpy (path, "../../../");
          strcat (path, dwgfile);
          if (stat (path, &attrib))
            fprintf (stderr, "%s not found\n", path);
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

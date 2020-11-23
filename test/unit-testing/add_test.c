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

/* test the dwg_add API for properly written DWG files */
/* written by: Reini Urban */

#define ADD_TEST_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>

static unsigned int loglevel;
#define DWG_LOGLEVEL loglevel
#include "../../src/config.h"
//#include "../../src/common.h"
//#include "../../src/decode.h"
#include "dwg.h"
#include "dwg_api.h"
#include "tests_common.h"
#include "../../src/classes.h"

static int
test_add (const Dwg_Object_Type type, const char *restrict dwgfile)
{
  int error;
  struct stat attrib;
  Dwg_Data *dwg = dwg_add_document(0);
  Dwg_Object *mspace =  dwg_model_space_object (dwg);
  Dwg_Object_Ref *mspace_ref =  dwg_model_space_ref (dwg);
  dwg_point_3d pt1 = {1.5, 2.5, 0.2};
  dwg_point_3d pt2 = {2.5, 1.5, 0.0};
  Dwg_Object_BLOCK_HEADER *hdr = mspace->tio.object->tio.BLOCK_HEADER;
  const char *name = dwg_type_name (type);
  int n_failed;

  if (!mspace)
    {
      fail ("empty mspace");
      return 1;
    }
  switch ((int)type)
    {
    case DWG_TYPE_LINE:
      dwg_add_LINE (hdr, &pt1, &pt2);
      break;
    case DWG_TYPE_TEXT:
      dwg_add_TEXT (hdr, (char *const)"testtekst", &pt1, 0.5);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_add_CIRCLE (hdr, &pt1, 0.5);
      break;
    case DWG_TYPE_ARC:
      dwg_add_ARC (hdr, &pt1, 0.5, 0.0, M_PI_2);
      break;
    default:
      fail ("Unknown type %s", name);
    }

  if (!stat (dwgfile, &attrib))
      unlink (dwgfile);
  error = dwg_write_file (dwgfile, dwg);
  if (error >= DWG_ERR_CRITICAL)
    return 1;
  dwg_free (dwg);

  error = dwg_read_file (dwgfile, dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fail ("read %s", name);
      return 2;
    }
  // now we have a different ref!
  mspace_ref =  dwg_model_space_ref (dwg);

  // look for a single written entity
#define TEST_ENTITY(token)                                              \
  case DWG_TYPE_##token:                                                \
  {                                                                     \
    Dwg_Entity_##token **objs = dwg_getall_##token (mspace_ref);        \
    if (objs && objs[0] && !objs[1])                                    \
      ok ("found 1 " #token);                                           \
    else if (!objs)                                                     \
      fail ("found no " #token " at all");                              \
    else if (!objs[0])                                                  \
      fail ("found no " #token);                                        \
  }                                                                     \
  break

  switch ((int)type)
    {
      TEST_ENTITY (LINE);
      TEST_ENTITY (TEXT);
      TEST_ENTITY (CIRCLE);
      TEST_ENTITY (ARC);
    default:
      fail ("Unknown type %s", name);
    }
  
  ok ("read %s", name);
  n_failed = numfailed();
  if (!n_failed)
    unlink (dwgfile);
  return n_failed;
}

int
main (int argc, char *argv[])
{
  int error;
  char *trace = getenv ("LIBREDWG_TRACE");
  if (trace)
    loglevel = atoi (trace);
  else
    loglevel = 0;

  error = test_add (DWG_TYPE_LINE, "add_line_2000.dwg");
  error = test_add (DWG_TYPE_TEXT, "add_text_2000.dwg");
  error = test_add (DWG_TYPE_CIRCLE, "add_circle_2000.dwg");
  error = test_add (DWG_TYPE_ARC, "add_arc_2000.dwg");

  return error;
}

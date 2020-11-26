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
static int debug;
static int cnt = 0;
#define DWG_LOGLEVEL loglevel
#include "../../src/config.h"
#include "../../src/logging.h"
#include "dwg.h"
#include "dwg_api.h"
#include "tests_common.h"
#include "../../src/classes.h"

static int
test_add (const Dwg_Object_Type type, const char *restrict dwgfile)
{
  int error;
  struct stat attrib;
  const char *name = dwg_type_name (type);
  Dwg_Data *dwg;
  Dwg_Object *mspace;
  Dwg_Object_Ref *mspace_ref;
  dwg_point_3d pt1 = {1.5, 2.5, 0.2};
  dwg_point_3d pt2 = {2.5, 1.5, 0.0};
  Dwg_Object_BLOCK_HEADER *hdr;
  int n_failed;

  cnt++;
  if (debug)
    {
      if (debug && debug != cnt && debug != -1)
        {
          ok ("cnt %d %s skipped", cnt, name);
          return 0;
        }
      ok ("LIBREDWG_DEBUG cnt %d %s", cnt, name);
    }

  dwg = dwg_add_Document(R_2000, 0 /*metric/iso */, loglevel /* static global */);
  mspace =  dwg_model_space_object (dwg);
  mspace_ref =  dwg_model_space_ref (dwg);
  hdr = mspace->tio.object->tio.BLOCK_HEADER;

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
      dwg_add_TEXT (hdr, (const BITCODE_T) "testtekst", &pt1, 0.5);
      break;
    case DWG_TYPE_CIRCLE:
      dwg_add_CIRCLE (hdr, &pt1, 0.5);
      break;
    case DWG_TYPE_ARC:
      dwg_add_ARC (hdr, &pt1, 0.5, 0.0, M_PI_2);
      break;
    case DWG_TYPE_LWPOLYLINE:
      {
        const dwg_point_2d pts[] = {
          { 2.5, 0.0 }, { 0.5, 0.0 }, { 0.5, 2.0 }, { 0.5, 1.0 }, { 1.5, 1.0 }
        };
        dwg_add_LWPOLYLINE (hdr, 5, pts);
      }
      break;
    case DWG_TYPE_POLYLINE_2D:
      {
        const dwg_point_2d pts[] = {
          { 2.5, 0.0 }, { 0.5, 0.0 }, { 0.5, 2.0 }, { 0.5, 1.0 }, { 1.5, 1.0 }
        };
        dwg_add_POLYLINE_2D (hdr, 5, pts);
      }
      break;
    case DWG_TYPE_POLYLINE_3D:
      {
        const dwg_point_3d pts[] = { { 2.5, 0.0, 0.0 },
                                     { 0.5, 0.0, 0.0 },
                                     { 0.5, 2.0, 1.0 },
                                     { 0.5, 1.0, 1.0 },
                                     { 1.5, 1.0, 0.0 } };
        dwg_add_POLYLINE_3D (hdr, 5, pts);
      }
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      {
        const dwg_point_3d verts[] = { { 2.5, 0.0, 0.0 },
                                       { 0.5, 0.0, 0.0 },
                                       { 0.5, 2.0, 1.0 },
                                       { 0.5, 1.0, 1.0 },
                                       { 1.5, 1.0, 0.0 } };
        const dwg_face faces[]
            = { { 0, 1, 2, 3 }, { 1, 2, 3, 4 }, { 2, 3, 4, 5 } };
        dwg_add_POLYLINE_PFACE (hdr, 5, 3, verts, faces);
      }
      break;
    case DWG_TYPE_POLYLINE_MESH:
      {
        const dwg_point_3d verts[]
            = { { 2.5, 0.0, 0.0 }, { 0.5, 0.0, 0.0 }, { 0.5, 2.0, 1.0 },
                { 0.5, 1.0, 1.0 }, { 0.5, 1.0, 0.0 }, { 1.5, 1.0, 0.0 } };
        dwg_add_POLYLINE_MESH (hdr, 3, 2, verts);
      }
      break;
    case DWG_TYPE_SPLINE:
      {
        const dwg_point_3d fit_pts[]
            = { { 2.5, 0.0, 0.0 }, { 0.5, 0.0, 0.0 }, { 0.5, 2.0, 1.0 },
                { 0.5, 1.0, 1.0 }, { 0.5, 1.0, 0.0 }, { 1.5, 1.0, 0.0 } };
        dwg_add_SPLINE (hdr, 6, fit_pts, &pt1, &pt2);
      }
      break;
    case DWG_TYPE_INSERT:
      {
        Dwg_Object_BLOCK_HEADER *blk;
        blk = dwg_add_BLOCK_HEADER (dwg, (const BITCODE_T) "bloko");
        dwg_add_BLOCK (blk, (const BITCODE_T) "bloko");
        dwg_add_LINE (blk, &pt1, &pt2);
        dwg_add_ENDBLK (blk);
        dwg_add_INSERT (hdr, &pt1, (const BITCODE_T) "bloko", 1.0, 1.0, 1.0, 0.0);
      }
      break;
    case DWG_TYPE_MINSERT:
      {
        Dwg_Object_BLOCK_HEADER *blk;
        blk = dwg_add_BLOCK_HEADER (dwg, (const BITCODE_T) "bloko");
        dwg_add_BLOCK (blk, (const BITCODE_T) "bloko");
        dwg_add_LINE (blk, &pt1, &pt2);
        dwg_add_ENDBLK (blk);
        dwg_add_MINSERT (hdr, &pt1, (const BITCODE_T) "bloko", 1.0, 1.0, 1.0,
                         0.0, 2, 1, 1.0, 0.0);
      }
      break;
    case DWG_TYPE_ATTRIB:
      {
        Dwg_Entity_INSERT *insert;
        Dwg_Object_BLOCK_HEADER *newhdr;
        newhdr = dwg_add_BLOCK_HEADER (dwg, (const BITCODE_T) "bloko");
        dwg_add_BLOCK (newhdr, (const BITCODE_T) "bloko");
        dwg_add_LINE (newhdr, &pt1, &pt2);
        dwg_add_ENDBLK (newhdr);
        insert = dwg_add_INSERT (hdr, &pt1, (const BITCODE_T) "bloko", 1.0, 1.0, 1.0, 0.0);
        // adds ATTDEF to BLOCK, redefines it (??)
        dwg_add_Attribute (insert, 1.0, 0, (const BITCODE_T) "blokoprompt", &pt1,
                           (const BITCODE_T) "blokotag",
                           (const BITCODE_T) "blokotekst");
      }
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      {
        const dwg_point_3d def_pt = { 2.5, 0.0, 0.0 };
        const dwg_point_3d text_midpt = { 2.5, 0.0, 0.0 };
        dwg_add_DIMENSION_ALIGNED (hdr, &pt1, &pt2, &def_pt, &text_midpt);
      }
      break;
    case DWG_TYPE_DIMENSION_ANG2LN:
      {
        const dwg_point_3d center_pt = { 2.5, 0.0, 0.0 };
        const dwg_point_3d text_midpt = { 2.5, 0.0, 0.0 };
        dwg_add_DIMENSION_ANG2LN (hdr, &center_pt, &pt1, &pt2, &text_midpt);
      }
      break;
    case DWG_TYPE_DIMENSION_ANG3PT:
      {
        const dwg_point_3d center_pt = { 2.5, 0.0, 0.0 };
        const dwg_point_3d text_midpt = { 2.5, 0.0, 0.0 };
        dwg_add_DIMENSION_ANG3PT (hdr, &center_pt, &pt1, &pt2, &text_midpt);
      }
      break;
    case DWG_TYPE_DIMENSION_DIAMETER:
      dwg_add_DIMENSION_DIAMETER (hdr, &pt1, &pt2, 2.0);
      break;
    case DWG_TYPE_DIMENSION_ORDINATE:
      dwg_add_DIMENSION_ORDINATE (hdr, &pt1, &pt2, true);
      break;
    case DWG_TYPE_DIMENSION_RADIUS:
      dwg_add_DIMENSION_RADIUS (hdr, &pt1, &pt2, 2.0);
      break;
    case DWG_TYPE_DIMENSION_LINEAR:
      {
        const dwg_point_3d def_pt = { 2.5, 0.0, 0.0 };
        dwg_add_DIMENSION_LINEAR (hdr, &pt1, &pt2, &def_pt, deg2rad (90.0));
      }
      break;
    case DWG_TYPE_POINT:
      dwg_add_POINT (hdr, &pt1);
      break;
    case DWG_TYPE__3DFACE:
      {
        const dwg_point_3d pt3 = { 2.5, 0.0, 0.0 };
        dwg_add_3DFACE (hdr, &pt1, &pt2, &pt3, NULL);
      }
      break;
    case DWG_TYPE_SOLID:
      {
        const dwg_point_2d pt2_2d = { 1.5, 0.0 };
        const dwg_point_2d pt3 = { 2.5, 0.0 };
        const dwg_point_2d pt4 = { 3.5, 2.0 };
        dwg_add_SOLID (hdr, &pt1, &pt2_2d, &pt3, &pt4);
      }
      break;
    case DWG_TYPE_TRACE:
      {
        const dwg_point_2d pt2_2d = { 1.5, 0.0 };
        const dwg_point_2d pt3 = { 2.5, 0.0 };
        const dwg_point_2d pt4 = { 3.5, 2.0 };
        dwg_add_TRACE (hdr, &pt1, &pt2_2d, &pt3, &pt4);
      }
      break;
    case DWG_TYPE_SHAPE:
      dwg_add_SHAPE (hdr, (const BITCODE_T) "romand.shx", &pt1, 1.0, 0.0); //??
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_add_VIEWPORT (hdr, (const BITCODE_T) "viewport1"); // FIXME
      break;
    case DWG_TYPE_ELLIPSE:
      dwg_add_ELLIPSE (hdr, &pt1, 0.0, 2.0);
      break;
    case DWG_TYPE_REGION:
      dwg_add_REGION (hdr,
        "400 26 1 0\n"
        "16 Autodesk AutoCAD 20 ASM 223.0.1.1930 OSX 24 Wed Mar 18 07:23:29 2020 \n"
        "1 9.999999999999999547e-07 1.000000000000000036e-10 \n"
        "body $-1 $1 $-1 $-1 #\n"
        "lump $-1 $-1 $2 $0 #\n"
        "shell $-1 $-1 $-1 $3 $-1 $1 #\n"
        "face $-1 $-1 $4 $2 $-1 $5 forward double out #\n"
        "loop $-1 $-1 $6 $3 #\n"
        "plane-surface $-1 -5234.186669031270867 1968.766094356862595 0 0 0 1 1 0 0 forward_v I I I I #\n"
        "coedge $-1 $7 $8 $-1 $9 reversed $4 $-1 #\n"
        "coedge $-1 $10 $6 $-1 $11 reversed $4 $-1 #\n"
        "coedge $-1 $6 $10 $-1 $12 reversed $4 $-1 #\n"
        "edge $-1 $13 $14 $6 $15 forward #\n"
        "coedge $-1 $8 $7 $-1 $16 reversed $4 $-1 #\n"
        "edge $-1 $17 $13 $7 $18 forward #\n"
        "edge $-1 $14 $19 $8 $20 forward #\n"
        "vertex $-1 $9 $21 #\n"
        "vertex $-1 $9 $22 #\n"
        "straight-curve $-1 -6836.331483613786986 3983.339979056661832 0 1 0 0 I I #\n"
        "edge $-1 $19 $17 $10 $23 forward #\n"
        "vertex $-1 $16 $24 #\n"
        "straight-curve $-1 -6836.331483613786986 -45.80779034293664154 0 0 1 0 I I #\n"
        "vertex $-1 $12 $25 #\n"
        "straight-curve $-1 -3632.041854448754748 3983.339979056661832 0 0 -1 0 I I #\n"
        "point $-1 -6836.331483613786986 3983.339979056661832 0 #\n"
        "point $-1 -3632.041854448754748 3983.339979056661832 0 #\n"
        "straight-curve $-1 -3632.041854448754748 -45.80779034293664154 0 -1 0 0 I I #\n"
        "point $-1 -6836.331483613786986 -45.80779034293664154 0 #\n"
        "point $-1 -3632.041854448754748 -45.80779034293664154 0 #\n");
      break;
    case DWG_TYPE_RAY:
      dwg_add_RAY (hdr, &pt1, &pt2);
      break;
    case DWG_TYPE_XLINE:
      dwg_add_XLINE (hdr, &pt1, &pt2);
      break;
    case DWG_TYPE_DICTIONARY:
      dwg_add_DICTIONARY (dwg, (const BITCODE_T) "TEST_DICT",
                          (const BITCODE_T) "testkey", NULL);
      break;
    case DWG_TYPE_DICTIONARYWDFLT:
      dwg_add_DICTIONARYWDFLT (dwg, (const BITCODE_T) "TEST_DICT",
                               (const BITCODE_T) "testkey", NULL);
      break;
    // case DWG_TYPE_DICTIONARYVAR:
    //  {
    //    dwg_add_DICTIONARYVAR (dict, "teststring");
    //  }
    //  break;
    case DWG_TYPE_XRECORD:
      {
        // but you can also add a DICT to any object/entity to its xdicobjhandle
        Dwg_Object_DICTIONARY *dict
            = dwg_add_DICTIONARY (dwg, (const BITCODE_T) "ACAD_MATERIAL",
                                  (const BITCODE_T) "Global", NULL);
        Dwg_Object_XRECORD *xrecord = dwg_add_XRECORD (dict, (const BITCODE_T) "REFRACTIONTILE");
        dwg_add_XRECORD_int16 (xrecord, 270, 1);
        dwg_add_XRECORD_int16 (xrecord, 271, 1);
      }
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_add_OLE2FRAME (hdr, &pt1, &pt2);
      break;
    case DWG_TYPE_MTEXT:
      dwg_add_MTEXT (hdr, &pt1, 10.0, (const BITCODE_T) "test\ntext");
      break;
    case DWG_TYPE_LEADER:
      {
        const dwg_point_3d pts[] = { { 2.5, 0.0, 0.0 }, { 0.5, 0.0, 0.0 } };
        dwg_add_LEADER (hdr, 2, pts, NULL, 15);
      }
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_add_TOLERANCE (hdr, (const BITCODE_T) "testtekst", &pt1, NULL);
      break;
    case DWG_TYPE_MLINE:
      {
        const dwg_point_3d pts[]
            = { { 2.5, 0.0, 0.0 }, { 0.5, 0.0, 0.0 }, { 0.5, 2.0, 1.0 },
                { 0.5, 1.0, 1.0 }, { 0.5, 1.0, 0.0 }, { 1.5, 1.0, 0.0 } };
        dwg_add_MLINE (hdr, 6, pts);
      }
      break;
    case DWG_TYPE_DIMSTYLE:
      {
        Dwg_Object_DIMSTYLE *dim = dwg_add_DIMSTYLE (dwg, (const BITCODE_T) "dim1");
        dim->DIMSCALE = 2.0;
        dim->DIMUPT = 1;
      }
      break;
    case DWG_TYPE_UCS:
      {
        const dwg_point_3d ydir = { 2.5, 0.0, 0.0 };
        Dwg_Object_UCS *ucs = dwg_add_UCS (dwg, &pt1, &pt2, &ydir, (const BITCODE_T) "ucs1");
        ucs->ucs_elevation = 1.0;
      }
      break;
      // VX_TABLE_RECORD
    case DWG_TYPE_HATCH:
      {
        const dwg_point_2d pts[] = {
          { 2.5, 0.0 }, { 0.5, 0.0 }, { 0.5, 2.0 }, { 0.5, 1.0 }, { 1.5, 1.0 }
        };
        Dwg_Entity_LWPOLYLINE *pline = dwg_add_LWPOLYLINE (hdr, 5, pts);
        Dwg_Object *obj;
        const Dwg_Object *objs[1];
        obj = dwg_obj_generic_to_object ((const dwg_obj_generic *)pline, &error);
        objs[0] = obj;
        dwg_add_HATCH (hdr, 0, (const BITCODE_T) "SOLID", false, 1, objs);
      }
      break;

    default:
      fail ("No add method yet type %s", name);
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

// look for a single written object
#define TEST_OBJECT(token)                                              \
  case DWG_TYPE_##token:                                                \
  {                                                                     \
    Dwg_Object_##token **objs = dwg_getall_##token (dwg);               \
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
      TEST_ENTITY (LWPOLYLINE);
      TEST_ENTITY (POLYLINE_2D);
      TEST_ENTITY (POLYLINE_3D);
      TEST_ENTITY (POLYLINE_MESH);
      TEST_ENTITY (POLYLINE_PFACE);
      TEST_ENTITY (SPLINE);
      TEST_ENTITY (INSERT);
      TEST_ENTITY (MINSERT);
      TEST_ENTITY (ATTRIB);
      TEST_ENTITY (DIMENSION_ALIGNED);
      TEST_ENTITY (DIMENSION_ANG2LN);
      TEST_ENTITY (DIMENSION_ANG3PT);
      TEST_ENTITY (DIMENSION_DIAMETER);
      TEST_ENTITY (DIMENSION_ORDINATE);
      TEST_ENTITY (DIMENSION_RADIUS);
      TEST_ENTITY (DIMENSION_LINEAR);
      TEST_ENTITY (POINT);
      TEST_ENTITY (_3DFACE);
      TEST_ENTITY (SOLID);
      TEST_ENTITY (TRACE);
      TEST_ENTITY (SHAPE);
      TEST_ENTITY (VIEWPORT);
      TEST_ENTITY (ELLIPSE);
      TEST_ENTITY (REGION);
      TEST_ENTITY (RAY);
      TEST_ENTITY (XLINE);
      TEST_OBJECT (DICTIONARY);
      TEST_OBJECT (DICTIONARYWDFLT);
      TEST_OBJECT (DICTIONARYVAR);
      TEST_ENTITY (OLE2FRAME);
      TEST_ENTITY (MTEXT);
      TEST_ENTITY (LEADER);
      TEST_ENTITY (TOLERANCE);
      TEST_ENTITY (MLINE);
      TEST_OBJECT (DIMSTYLE);
      TEST_OBJECT (UCS);
      //TEST_OBJECT (VX_TABLE_RECORD);
      TEST_ENTITY (HATCH);
      TEST_OBJECT (XRECORD);
    default:
      fail ("No test yet for type %s", name);
    }
  
  ok ("read %s", name);
  n_failed = numfailed();
  if (!n_failed && (!debug || debug != -1))
    unlink (dwgfile);
  return n_failed;
}

int
main (int argc, char *argv[])
{
  int error;
  char *trace = getenv ("LIBREDWG_TRACE");
  char *debugenv = getenv ("LIBREDWG_DEBUG");
  if (trace)
    loglevel = atoi (trace);
  else
    loglevel = 0;
  if (debugenv) // -1 for all, 0 for none, 12 for ATTRIB only
    debug = atoi (debugenv);
  else
    debug = 0;

  error = test_add (DWG_TYPE_LINE, "add_line_2000.dwg");
  error = test_add (DWG_TYPE_TEXT, "add_text_2000.dwg");
  error = test_add (DWG_TYPE_CIRCLE, "add_circle_2000.dwg");
  error = test_add (DWG_TYPE_ARC, "add_arc_2000.dwg");
  error = test_add (DWG_TYPE_LWPOLYLINE, "add_lwpline_2000.dwg");
  error = test_add (DWG_TYPE_POLYLINE_2D, "add_pl2d_2000.dwg");
  error = test_add (DWG_TYPE_POLYLINE_3D, "add_pl3d_2000.dwg");
  error = test_add (DWG_TYPE_POLYLINE_MESH, "add_pmesh_2000.dwg");
  error = test_add (DWG_TYPE_POLYLINE_PFACE, "add_pface_2000.dwg");
  error = test_add (DWG_TYPE_SPLINE, "add_spline_2000.dwg");
  error = test_add (DWG_TYPE_INSERT, "add_insert_2000.dwg");
  error = test_add (DWG_TYPE_MINSERT, "add_minsert_2000.dwg");
  if (debug == cnt || debug == -1)
    error = test_add (DWG_TYPE_ATTRIB, "add_attrib_2000.dwg");
  else
    ok ("skip ATTRIB TODO add_Attribute");
  error = test_add (DWG_TYPE_DIMENSION_ALIGNED, "add_dimali_2000.dwg");
  error = test_add (DWG_TYPE_DIMENSION_ANG2LN, "add_dimang_2000.dwg");
  error = test_add (DWG_TYPE_DIMENSION_ANG3PT, "add_dim3pt_2000.dwg");
  error = test_add (DWG_TYPE_DIMENSION_DIAMETER, "add_dimdia_2000.dwg");
  error = test_add (DWG_TYPE_DIMENSION_ORDINATE, "add_dimord_2000.dwg");
  error = test_add (DWG_TYPE_DIMENSION_RADIUS, "add_dimrad_2000.dwg");
  error = test_add (DWG_TYPE_DIMENSION_LINEAR, "add_dimlin_2000.dwg");
  error = test_add (DWG_TYPE_POINT, "add_point_2000.dwg");
  error = test_add (DWG_TYPE__3DFACE, "add_3dface_2000.dwg");
  error = test_add (DWG_TYPE_SOLID, "add_solid_2000.dwg");
  error = test_add (DWG_TYPE_TRACE, "add_trace_2000.dwg");
  error = test_add (DWG_TYPE_SHAPE, "add_shape_2000.dwg");
  error = test_add (DWG_TYPE_VIEWPORT, "add_viewport_2000.dwg");
  error = test_add (DWG_TYPE_ELLIPSE, "add_ellipse_2000.dwg");
  error = test_add (DWG_TYPE_REGION, "add_region_2000.dwg");
  error = test_add (DWG_TYPE_RAY, "add_ray_2000.dwg");
  error = test_add (DWG_TYPE_XLINE, "add_xline_2000.dwg");
  error = test_add (DWG_TYPE_DICTIONARY, "add_dict_2000.dwg");
  if (debug == cnt || debug == -1)
    error = test_add (DWG_TYPE_DICTIONARYWDFLT, "add_dictwdflt_2000.dwg");
  else
    ok ("skip DICTIONARYWDFLT TODO wrong variable type");
  error = test_add (DWG_TYPE_OLE2FRAME, "add_ole2frame_2000.dwg");
  error = test_add (DWG_TYPE_MTEXT, "add_mtext_2000.dwg");
  error = test_add (DWG_TYPE_LEADER, "add_leader_2000.dwg");
  error = test_add (DWG_TYPE_TOLERANCE, "add_tolerance_2000.dwg");
  error = test_add (DWG_TYPE_MLINE, "add_mline_2000.dwg");
  error = test_add (DWG_TYPE_DIMSTYLE, "add_dimstyle_2000.dwg");
  error = test_add (DWG_TYPE_UCS, "add_ucs_2000.dwg");
  //error = test_add (DWG_TYPE_VX_TABLE_RECORD, "add_vx_2000.dwg");
  error = test_add (DWG_TYPE_HATCH, "add_hatch_2000.dwg");
  error = test_add (DWG_TYPE_XRECORD, "add_xrecord_2000.dwg");

  return error;
}

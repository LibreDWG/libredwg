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

/* Test the dwg_add API for properly written DWG and DXF files */
/* written by: Reini Urban */

#define ADD_TEST_C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

static int tracelevel;
static int debug;
static int cnt = 0;

#include "config.h"
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include "../../programs/my_stat.h"
#include "dwg.h"
#include "tests_common.h"
#include "dwg_api.h"
#include "classes.h"
#include "bits.h"
#include "out_dxf.h"

enum _temp_complex_types
{ // and test-cases
  TEMP_ELLIPTICAL_CONE = 4000,
  TEMP_ELLIPTICAL_CYLINDER,
  TEMP_EXTRUDED_SOLID,
  TEMP_EXTRUDED_PATH,
  TEMP_REVOLVED_SOLID,
  TEMP_PDFDEFINITION1,
  TEMP_PDFDEFINITION2,
  TEMP_PDFDEFINITION3,
};

static unsigned
num_objects (void *vents)
{
  unsigned i = 0;
  Dwg_Object_DICTIONARY **ents = (Dwg_Object_DICTIONARY **)vents;
  if (!ents)
    return 0;
  while (*ents)
    {
      i++;
      ents++;
    };
  return i;
}

static int
test_add (const Dwg_Object_Type type, const char *restrict file,
          const int as_dxf)
{
  int error;
  struct stat attrib;
  const char *name = dwg_type_name (type);
  Dwg_Data *dwg;
  Dwg_Object *mspace;
  Dwg_Object_Ref *mspace_ref;
  dwg_point_3d pt1 = { 1.5, 2.5, 0.2 };
  dwg_point_3d pt2 = { 2.5, 1.5, 0.0 };
  Dwg_Object_BLOCK_HEADER *hdr;
  int n_failed;
  char dwgfile[256];
  int todo = 0;

  strcpy (dwgfile, file);
  if (!name)
    {
      switch ((enum _temp_complex_types)type)
        {
        case TEMP_ELLIPTICAL_CONE:
          name = "ELLIPTICAL_CONE";
          break;
        case TEMP_ELLIPTICAL_CYLINDER:
          name = "ELLIPTICAL_CYLINDER";
          break;
        case TEMP_EXTRUDED_SOLID:
          name = "EXTRUDED_SOLID";
          break;
        case TEMP_EXTRUDED_PATH:
          name = "EXTRUDED_PATH";
          break;
        case TEMP_REVOLVED_SOLID:
          name = "REVOLVED_SOLID";
          break;
        case TEMP_PDFDEFINITION1:
        case TEMP_PDFDEFINITION2:
        case TEMP_PDFDEFINITION3:
          name = "PDFDEFINITION";
          break;
        default:
          assert (name);
        }
    }
  assert (name);
  failed = 0;
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

  dwg = dwg_new_Document (as_dxf ? R_2018 : R_2000, 0 /*metric/iso */,
                          tracelevel);
  mspace = dwg_model_space_object (dwg);
  mspace_ref = dwg_model_space_ref (dwg);
  if (!mspace)
    {
      fail ("empty mspace");
      return 1;
    }
  hdr = mspace->tio.object->tio.BLOCK_HEADER;
  switch ((int)type)
    {
    case DWG_TYPE_LINE:
      dwg_add_LINE (hdr, &pt1, &pt2);
      break;
    case DWG_TYPE_TEXT:
      dwg_add_TEXT (hdr, "testtekst", &pt1, 0.5);
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
          { 0.0, 0.0 }, { 2.5, 0.0 }, { 2.5, 2.0 }, { 0.0, 2.0 }, { 1.5, 1.0 }
        };
        dwg_add_LWPOLYLINE (hdr, 5, pts);
      }
      break;
    case DWG_TYPE_POLYLINE_2D:
      {
        const dwg_point_2d pts[] = {
          { 0.0, 0.0 }, { 2.5, 0.0 }, { 2.5, 2.0 }, { 0.0, 2.0 }, { 1.5, 1.0 }
        };
        dwg_add_POLYLINE_2D (hdr, 5, pts);
      }
      break;
    case DWG_TYPE_POLYLINE_3D:
      {
        const dwg_point_3d pts[] = { { 0.0, 0.0, 0.0 },
                                     { 2.5, 0.0, 0.0 },
                                     { 2.5, 2.0, 1.0 },
                                     { 0.5, 2.0, 1.0 },
                                     { 1.5, 1.0, 0.0 } };
        dwg_add_POLYLINE_3D (hdr, 5, pts);
      }
      break;
    case DWG_TYPE_POLYLINE_PFACE:
      {
        const dwg_point_3d verts[] = { { 0.0, 0.0, 0.0 },
                                       { 2.5, 0.0, 0.0 },
                                       { 2.5, 2.0, 1.0 },
                                       { 0.5, 2.0, 1.0 },
                                       { 1.5, 1.0, 0.0 } };
        const dwg_face faces[]
            = { { 0, 1, 2, 3 }, { 1, 2, 3, 4 }, { 2, 3, 4, 5 } };
        dwg_add_POLYLINE_PFACE (hdr, 5, 3, verts, faces);
      }
      break;
    case DWG_TYPE_POLYLINE_MESH:
      {
        const dwg_point_3d verts[]
            = { { 0.0, 0.0, 0.0 }, { 2.5, 0.0, 0.0 }, { 2.5, 2.0, 0.0 },
                { 0.5, 2.0, 0.0 }, { 1.5, 1.0, 0.0 }, { 0.0, 1.0, 0.0 } };
        dwg_add_POLYLINE_MESH (hdr, 3, 2, verts);
      }
      break;
    case DWG_TYPE_SPLINE:
      {
        const dwg_point_3d fit_pts[]
            = { { 0.0, 0.0, 0.0 }, { 2.5, 0.0, 0.0 }, { 2.5, 2.0, 0.0 },
                { 0.5, 2.0, 0.0 }, { 1.5, 1.0, 0.0 }, { 0.0, 1.0, 0.0 } };
        dwg_add_SPLINE (hdr, 6, fit_pts, &pt1, &pt2);
      }
      break;
    case DWG_TYPE_INSERT:
      {
        Dwg_Object_BLOCK_HEADER *blk;
        blk = dwg_add_BLOCK_HEADER (dwg, "bloko");
        dwg_add_BLOCK (blk, "bloko");
        dwg_add_LINE (blk, &pt1, &pt2);
        dwg_add_ENDBLK (blk);
        dwg_add_INSERT (hdr, &pt1, "bloko", 1.0, 1.0, 1.0, 0.0);
      }
      break;
    case DWG_TYPE_MINSERT:
      {
        Dwg_Object_BLOCK_HEADER *blk;
        blk = dwg_add_BLOCK_HEADER (dwg, "bloko");
        dwg_add_BLOCK (blk, "bloko");
        dwg_add_LINE (blk, &pt1, &pt2);
        dwg_add_ENDBLK (blk);
        dwg_add_MINSERT (hdr, &pt1, "bloko", 1.0, 1.0, 1.0, 0.0, 2, 1, 1.0,
                         0.0);
      }
      break;
    case DWG_TYPE_ATTRIB:
      {
        Dwg_Entity_INSERT *insert;
        Dwg_Object_BLOCK_HEADER *newhdr;
        newhdr = dwg_add_BLOCK_HEADER (dwg, "bloko");
        dwg_add_BLOCK (newhdr, "bloko");
        dwg_add_LINE (newhdr, &pt1, &pt2);
        dwg_add_ENDBLK (newhdr);
        insert = dwg_add_INSERT (hdr, &pt1, "bloko", 1.0, 1.0, 1.0, 0.0);
        // adds ATTDEF to BLOCK, redefines it (??)
        dwg_add_Attribute (insert, 1.0, 0, "blokoprompt", &pt1, "blokotag",
                           "blokotekst");
      }
      break;
    case DWG_TYPE_DIMENSION_ALIGNED:
      {
        const dwg_point_3d text_midpt = { 2.5, 0.0, 0.0 };
        dwg_add_DIMENSION_ALIGNED (hdr, &pt1, &pt2, &text_midpt);
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
      dwg_add_SHAPE (hdr, "romand.shx", &pt1, 1.0, 0.0); //??
      break;
    case DWG_TYPE_VIEWPORT:
      dwg_add_VIEWPORT (hdr, "viewport1"); // FIXME
      break;
    case DWG_TYPE_ELLIPSE:
      {
        const dwg_point_3d ctr = { 11.143259, 9.537395, 0.0 };
        Dwg_Entity_ELLIPSE *ellipse
            = dwg_add_ELLIPSE (hdr, &ctr, -8.750802, 0.336109);
      }
      break;
    case DWG_TYPE_REGION:
      dwg_add_REGION (
          hdr,
          "400 26 1 0\n"
          "8 LibreDWG 20 ASM 223.0.1.1930 OSX 24 Wed Mar 18 07:23:29 2020 \n"
          "1 9.999999999999999547e-07 1.000000000000000036e-10 \n"
          "body $-1 $1 $-1 $-1 #\n"
          "lump $-1 $-1 $2 $0 #\n"
          "shell $-1 $-1 $-1 $3 $-1 $1 #\n"
          "face $-1 $-1 $4 $2 $-1 $5 forward double out #\n"
          "loop $-1 $-1 $6 $3 #\n"
          "plane-surface $-1 -5234.186669031270867 1968.766094356862595 0 0 0 "
          "1 1 0 0 forward_v I I I I #\n"
          "coedge $-1 $7 $8 $-1 $9 reversed $4 $-1 #\n"
          "coedge $-1 $10 $6 $-1 $11 reversed $4 $-1 #\n"
          "coedge $-1 $6 $10 $-1 $12 reversed $4 $-1 #\n"
          "edge $-1 $13 $14 $6 $15 forward #\n"
          "coedge $-1 $8 $7 $-1 $16 reversed $4 $-1 #\n"
          "edge $-1 $17 $13 $7 $18 forward #\n"
          "edge $-1 $14 $19 $8 $20 forward #\n"
          "vertex $-1 $9 $21 #\n"
          "vertex $-1 $9 $22 #\n"
          "straight-curve $-1 -6836.331483613786986 3983.339979056661832 0 1 "
          "0 0 I I #\n"
          "edge $-1 $19 $17 $10 $23 forward #\n"
          "vertex $-1 $16 $24 #\n"
          "straight-curve $-1 -6836.331483613786986 -45.80779034293664154 0 0 "
          "1 0 I I #\n"
          "vertex $-1 $12 $25 #\n"
          "straight-curve $-1 -3632.041854448754748 3983.339979056661832 0 0 "
          "-1 0 I I #\n"
          "point $-1 -6836.331483613786986 3983.339979056661832 0 #\n"
          "point $-1 -3632.041854448754748 3983.339979056661832 0 #\n"
          "straight-curve $-1 -3632.041854448754748 -45.80779034293664154 0 "
          "-1 0 0 I I #\n"
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
      dwg_add_DICTIONARY (dwg, "TEST_DICT", "testkey", 0);
      break;
    case DWG_TYPE_DICTIONARYWDFLT:
      dwg_add_DICTIONARYWDFLT (dwg, "TEST_DICT", "testkey", 0);
      break;
    // case DWG_TYPE_DICTIONARYVAR:
    //  {
    //    dwg_add_DICTIONARYVAR (dict, "teststring");
    //  }
    //  break;
    case DWG_TYPE_XRECORD:
      {
        // but you can also add a DICT to any object/entity to its
        // xdicobjhandle
        Dwg_Object_DICTIONARY *dict
            = dwg_add_DICTIONARY (dwg, "ACAD_MATERIAL", "Global", 0);
        Dwg_Object_XRECORD *xrecord = dwg_add_XRECORD (dict, "REFRACTIONTILE");
        dwg_add_XRECORD_int16 (xrecord, 270, 1);
        dwg_add_XRECORD_int32 (xrecord, 90, 1);
      }
      break;
    case DWG_TYPE_OLE2FRAME:
      dwg_add_OLE2FRAME (hdr, &pt1, &pt2);
      break;
    case DWG_TYPE_MTEXT:
      dwg_add_MTEXT (hdr, &pt1, 10.0, "test\ntext");
      break;
    case DWG_TYPE_LEADER:
      {
        const dwg_point_3d pts[] = { { 2.5, 0.0, 0.0 }, { 0.5, 0.0, 0.0 } };
        Dwg_Entity_MTEXT *annot
            = dwg_add_MTEXT (hdr, &pt1, 10.0, "test\ntext");
        dwg_add_LEADER (hdr, 2, pts, annot, 15);
      }
      break;
    case DWG_TYPE_TOLERANCE:
      dwg_add_TOLERANCE (hdr, "testtekst", &pt1, NULL);
      break;
    case DWG_TYPE_MLINESTYLE:
      {
        Dwg_Object_MLINESTYLE *mlsty = dwg_add_MLINESTYLE (dwg, "Double");
        mlsty->start_angle = deg2rad (15.0);
      }
      break;
    case DWG_TYPE_MLINE:
      {
        const dwg_point_3d pts[] = { { 3467.902646, 1494.544557, 0.0 },
                                     { 3975.226253, 1705.812678, 0.0 },
                                     { 4186.611142, 1579.051827, 0.0 },
                                     { 3763.841434, 1198.769165, 0.0 } };
        dwg_add_MLINE (hdr, 4, pts);
      }
      break;
    case DWG_TYPE_DIMSTYLE:
      {
        Dwg_Object_DIMSTYLE *dim = dwg_add_DIMSTYLE (dwg, "dim1");
        dim->DIMSCALE = 2.0;
        dim->DIMUPT = 1;
      }
      break;
    case DWG_TYPE_UCS:
      {
        const dwg_point_3d ydir = { 2.5, 0.0, 0.0 };
        Dwg_Object_UCS *ucs = dwg_add_UCS (dwg, &pt1, &pt2, &ydir, "ucs1");
        ucs->ucs_elevation = 1.0;
      }
      break;
      // VX_TABLE_RECORD
    case DWG_TYPE_HATCH:
      {
        const dwg_point_2d pts[] = {
          { 0.0, 0.0 }, { 2.5, 0.0 }, { 2.5, 2.0 }, { 0.0, 2.0 }, { 1.5, 1.0 }
        };
        BITCODE_2RD *seeds
            = malloc (sizeof (BITCODE_2RD)); // this cannot be constant
        Dwg_Entity_HATCH *hatch;
        Dwg_Entity_LWPOLYLINE *pline = dwg_add_LWPOLYLINE (hdr, 5, pts);
        Dwg_Object *obj;
        const Dwg_Object *objs[1];
        pline->flag |= 512; // closed
        obj = dwg_obj_generic_to_object ((const dwg_obj_generic *)pline,
                                         &error);
        objs[0] = obj;
        hatch = dwg_add_HATCH (hdr, 1, "SOLID", true, 1, objs);
        hatch->num_seeds = 1;
        seeds[0] = (BITCODE_2RD){ 1.5, 0.3 }; // a pick point
        hatch->seeds = seeds;
      }
      break;
    case DWG_TYPE_VBA_PROJECT:
      {
        // VBAProject Section from some example dwg (in MS-CFB format, as hex)
        // https://github.com/microsoft/compoundfilereader/blob/master/src/include/compoundfilereader.h
        // cfb signature at offset 0x10
        // Interesting are the VBA_Project/VBA/ThisDrawing and
        // VBA_Project/VBA/_VBA_PROJECT streams eg: pip install compoundfiles
        char hex[]
            = "0000000000000000001C000019000000D0CF11E0A1B11AE1000000000000000"
              "000000000000000003E000300FEFF0900060000000000000000000000010000"
              "000100000000000000001000000200000001000000FEFFFFFF0000000000000"
              "000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDFFFFFF0400000"
              "0FEFFFFFF050000000C000000060000000700000008000000090000000A0000"
              "000B000000FEFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "F52006F006F007400200045006E007400720079000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "00016000500FFFFFFFFFFFFFFFF010000000000000000000000000000000000"
              "0000000000000000000000000000A03A4BF06E1CD50103000000800E0000000"
              "000005600420041005F00500072006F006A0065006300740000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000018000101FFFFFFFF0200000007000000000000000000000000000000"
              "0000000000000000602D48F06E1CD501602D48F06E1CD501000000000000000"
              "0000000005600420041005F00500072006F006A006500630074005F00560065"
              "007200730069006F006E0000000000000000000000000000000000000000000"
              "0000000000028000200FFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000"
              "0000000000000000000000000000000000000000000000000000000000000C0"
              "000000000000056004200410000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "00000000000000008000100FFFFFFFFFFFFFFFF050000000000000000000000"
              "000000000000000000000000008369457AA6D301602D48F06E1CD5010000000"
              "00000000000000000FEFFFFFF02000000030000000400000005000000FEFFFF"
              "FF0700000008000000090000000A0000000B0000000C0000000D0000000E000"
              "0000F000000100000001100000012000000130000001400000015000000FEFF"
              "FFFF1700000018000000190000001A0000001B0000001C0000001D0000001E0"
              "000001F00000020000000210000002200000023000000240000002500000026"
              "0000002700000028000000290000002A0000002B0000002C0000002D0000002"
              "E0000002F000000300000003100000032000000FEFFFFFF3400000035000000"
              "360000003700000038000000FEFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF110000000000000040000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000138B1800100040000000100302A"
              "020290090070140648030082020064E40404000B001C004143414450726F6A1"
              "0656374050038000040D5021406020A3D020A07027401140A080612090212C0"
              "E1F7502846000C024A3C020A16000101727374646F6C653E010219730074006"
              "4006F00006C0065000D00680500255E00032A5C477B3000303032303433302D"
              "3B0008040443000A020E011230300034367D23322E3023003023433A5C57696"
              "E00646F77735C7379734074656D33325C036532002E746C62234F4C45002041"
              "75746F6D617458696F6E003000010F027401440013820381781902785400686"
              "9734472617769A06E67470016001354004A0069007300440072000061007700"
              "69006E00A867001A0D163218161C002328000048820231828C25035800001E8"
              "204010A2C82407F549A2282102B8202108202000000000116010000F0000000"
              "C4020000D400000000020000FFFFFFFFCB0200001F030000000000000100000"
              "081787F9A0000FFFF2300000088000000B600FFFF010100000000FFFFFFFF00"
              "000000FFFFFFFFFFFF000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000064006900720000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "00000000000000000000000000000000000000000000000000008000200FFFF"
              "FFFFFFFFFFFFFFFFFFFF0000000000000000000000000000000000000000000"
              "00000000000000000000000000000010000003C010000000000005400680069"
              "007300440072006100770069006E00670000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000018000201"
              "0400000006000000FFFFFFFF000000000000000000000000000000000000000"
              "00000000000000000000000000000000006000000E5030000000000005F0056"
              "00420041005F00500052004F004A00450043005400000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000001A00"
              "0200FFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000000000000000000"
              "0000000000000000000000000000000000000160000002C0700000000000050"
              "0052004F004A004500430054000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "100002010300000008000000FFFFFFFF0000000000000000000000000000000"
              "000000000000000000000000000000000000000003300000041010000000000"
              "000000000000000010000000030000000500000007000000FFFFFFFFFFFFFFF"
              "F010108000000FFFFFFFF780000000800000000000000000000000000000000"
              "00000000000000000000000000000000FFFF000000004D450000FFFFFFFFFFF"
              "F00000000FFFF00000000FFFF010100000000DF00FFFF000000000C00FFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF28"
              "0000000200534CFFFFFFFF000001005310FFFFFFFF000001005394FFFFFFFF0"
              "0000000023CFFFFFFFF0000FFFF01010000000001004E0030007B0038004500"
              "3700350044003900310033002D0033004400320031002D00310031004400320"
              "02D0038003500430034002D0030003800300030003000390041003000430036"
              "00320036007D0000000000FFFFFFFF0101380000000280FEFFFFFFFFFF1000F"
              "FFF280000000201FFFF0000000000000000FFFFFFFFFFFFFFFF000000001D00"
              "000025000000FFFFFFFF480000000000FFFF000001000000000000000000FFF"
              "FFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFF00000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000"
              "0000000FFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000DF000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000FECA01000000FFFFFFFF0101080"
              "00000FFFFFFFF78000000FFFFFFFF000001BCB0004174747269627574006520"
              "56425F4E616D0065203D2022546869007344726177696E6788220D0A0A88426"
              "173028800307B3845373544390031332D334432312D00313144322D38354300"
              "342D303830303039804130433632367D0D7C40476C6F62616C01CE530870616"
              "3019246616C7302650C6443726561746104626C151F507265646548636C6100"
              "06496400B0540472750D424578706F730265141C54656D706C6180746544657"
              "2697602120192427573746F6D697A0304440332000000000000000000000000"
              "000000000000000000000000000000CC616D00000100FF0A0C000009040000E"
              "40401000000000000000000010003000200FA002A005C0047007B0030003000"
              "3000320030003400450046002D0030003000300030002D00300030003000300"
              "02D0043003000300030002D0030003000300030003000300030003000300030"
              "00340036007D00230034002E00300023003900230043003A005C00500052004"
              "F004700520041007E0032005C0043004F004D004D004F004E007E0031005C00"
              "4D004900430052004F0053007E0031005C005600420041005C0056004200410"
              "036005C0056004200450036002E0044004C004C002300560069007300750061"
              "006C00200042006100730069006300200046006F00720020004100700070006"
              "C00690063006100740069006F006E0073000000000000000000000000001601"
              "2A005C0047007B00310045004600440038004500380035002D0037004600330"
              "042002D0034003800450036002D0039003300340031002D0033004300380042"
              "00320046003600300031003300360042007D00230031002E003100230030002"
              "30043003A005C00500072006F006700720061006D002000460069006C006500"
              "73002000280078003800360029005C0043006F006D006D006F006E002000460"
              "069006C00650073005C004100750074006F006400650073006B002000530068"
              "0061007200650064005C0061006300610078003100360065006E0075002E007"
              "4006C00620023004100750074006F0043004100440020003200300030003600"
              "2000540079007000650020004C0069006200720061007200790000000000000"
              "0000000000000BC002A005C0047007B00300030003000320030003400330030"
              "002D0030003000300030002D0030003000300030002D0043003000300030002"
              "D003000300030003000300030003000300030003000340036007D0023003200"
              "2E00300023003000230043003A005C00570069006E0064006F00770073005C0"
              "0730079007300740065006D00330032005C007300740064006F006C00650032"
              "002E0074006C00620023004F004C00450020004100750074006F006D0061007"
              "40069006F006E00000000000000000000000000010002000400040200000602"
              "0100080200000A0201000C02FFFFFFFFFFFF00000000FFFF0000C0E1F750460"
              "0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFF"
              "FFF010000000000000000000000000000000000000081780100160054006800"
              "69007300440072006100770069006E006700140030003100350063003500620"
              "03100300065006500FFFF110216005400680069007300440072006100770069"
              "006E006700FFFF7F9A000000000000000200000025030000FFFFFFFFFFFF010"
              "120020000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFF00020000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
              "FFFFFFFFFFFFFFFFFFFFFFFFFEA84FC4325F81B43A95792F0F8815206FFFFFF"
              "FF01000000FFFFFFFF600000008000000000000A010B00FF006025000007044"
              "175746F43414422F810000304564241F7E21000050457696E3136C17E100005"
              "0457696E3332077F100003044D6163B3B21000040456424136AD2310000B044"
              "143414450726F6A656374A6D4100006007374646F6C65936010000B04546869"
              "7344726177696E67B935100009800000FF03FFFF5F4576616C7561746518D91"
              "0000C0441636164446F63756D656E741B5A100002FFFF01013C000000FFFFFF"
              "FFFFFFFFFFFFFFFFFF0C02FFFFFFFF0E020200FFFF110200000400FFFFFFFFF"
              "FFF00020100FFFF02020000FFFFFFFFFFFFFFFFFFFFFFFFFFFF04000A000000"
              "010012000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000049443D227B32464644323531392D383032362D"
              "344132362D383141352D4239353537373836374145317D220D0A446F63756D6"
              "56E743D5468697344726177696E672F264830303030303030300D0A4E616D65"
              "3D224143414450726F6A656374220D0A48656C70436F6E7465787449443D223"
              "0220D0A56657273696F6E436F6D70617469626C6533323D2233393332323230"
              "3030220D0A434D473D224139414231333631313736313137363131373631313"
              "7220D0A4450423D22353235304538413339304134393041343930220D0A4743"
              "3D22464246393431314145423142454231423134220D0A0D0A5B486F7374204"
              "57874656E64657220496E666F5D0D0A264830303030303030313D7B33383332"
              "443634302D434639302D313143462D384534332D30304130433931313030354"
              "17D3B5642453B264830303030303030300D0A00000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "00000000000000000000000000000000000005468697344726177696E670054"
              "00680069007300440072006100770069006E006700000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000500052004F00"
              "4A0045004300540077006D00000000000000000000000000000000000000000"
              "0000000000000000000000000000000000000000000000000000014000200FF"
              "FFFFFFFFFFFFFFFFFFFFFF00000000000000000000000000000000000000000"
              "000000000000000000000000000000039000000260000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "00FFFFFFFFFFFFFFFFFFFFFFFF0000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "0000000000FFFFFFFFFFFFFFFFFFFFFFFF00000000000000000000000000000"
              "000000000000000000000000000000000000000000000000000000000000000"
              "000000000000";
        const BITCODE_BL blen = ((sizeof (hex) - 1) / 2) & 0xFFFFFFFF;
        BITCODE_TF data = malloc (blen + 1);
        in_hex2bin (data, hex, blen);
        dwg_add_VBA_PROJECT (dwg, blen, data);
        free (data);
      }
      break;
    case DWG_TYPE_LAYOUT:
      {
        Dwg_Object *ps = dwg_paper_space_object (dwg);
        if (ps)
          dwg_add_LAYOUT (ps, "Layout2", "ANSI_A_(8.50_x_11.00_Inches)");
        else
          fail ("no *Paper_Space BLOCK_HEADER found");
      }
      break;
    case TEMP_PDFDEFINITION3: // same def
      dwg_add_PDFUNDERLAY (hdr, "test.pdf", &pt2, 1.0, 0);
      /* fallthrough */
    case TEMP_PDFDEFINITION2: // new def
      dwg_add_PDFUNDERLAY (hdr, "test1.pdf", &pt1, 1.0, 0);
      /* fallthrough */
    case TEMP_PDFDEFINITION1: // same base => "test - 2"
      dwg_add_PDFUNDERLAY (hdr, "../test.pdf", &pt2, 1.0, 0);
      /* fallthrough */
    case DWG_TYPE_PDFUNDERLAY:
      dwg_add_PDFUNDERLAY (hdr, "test.pdf", &pt1, 1.0, 0);
      break;
    case DWG_TYPE_ACSH_TORUS_CLASS:
      {
        const dwg_point_3d pt = { 1383.62, 418.5, 1158.76 };
        dwg_add_TORUS (hdr, &pt, NULL, 19.0, 2.789983);
      }
      break;
    case DWG_TYPE_ACSH_SPHERE_CLASS:
      {
        const dwg_point_3d pt = { 10.0, 10.0, 0 };
        dwg_add_SPHERE (hdr, &pt, NULL, 15.0);
      }
      break;
    case DWG_TYPE_ACSH_CYLINDER_CLASS:
      {
        const dwg_point_3d pt = { 10.0, 10.0, 0 };
        dwg_add_CYLINDER (hdr, &pt, NULL, 15.0, 5.0, 5.0, 5.0);
      }
      break;
    case DWG_TYPE_ACSH_CONE_CLASS:
      {
        const dwg_point_3d pt = { 10.0, 10.0, 0 };
        dwg_add_CONE (hdr, &pt, NULL, 15.0, 5.0, 5.0, 0.0);
      }
      break;
    case DWG_TYPE_ACSH_WEDGE_CLASS:
      {
        const dwg_point_3d pt = { 10.0, 10.0, 0 };
        dwg_add_WEDGE (hdr, &pt, NULL, 3.348158, 2.489864, 4.886064);
      }
      break;
    case DWG_TYPE_ACSH_BOX_CLASS:
      {
        const dwg_point_3d pt = { 7.791946, 11.0222066, 1.271660 };
        dwg_add_BOX (hdr, &pt, NULL, 4.416106, 2.044413, 2.543320);
      }
      break;
    case DWG_TYPE_ACSH_PYRAMID_CLASS:
      {
        const dwg_point_3d pt = { 7.791946, 11.0222066, 1.271660 };
        dwg_add_PYRAMID (hdr, &pt, NULL, 4.5, 4, 2.0, 2.5);
      }
      break;
    case DWG_TYPE_ACSH_CHAMFER_CLASS:
      {
        const dwg_point_3d pt = { 7.791946, 11.0222066, 1.271660 };
        const int32_t edges[] = { 151 };
        //?? normally you chamfer an existing 3dsolid
        // dwg_add_CHAMFER (hdr, &pt, NULL, 1, 10.0, 10.0, 1, edges, 152);
      }
      break;
    case DWG_TYPE_WIPEOUTVARIABLES:
      dwg_add_WIPEOUTVARIABLES (dwg);
      break;
    case DWG_TYPE_ACSH_FILLET_CLASS:
    case TEMP_ELLIPTICAL_CONE:
    case TEMP_ELLIPTICAL_CYLINDER:
    case TEMP_EXTRUDED_SOLID:
    case TEMP_EXTRUDED_PATH:
    case TEMP_REVOLVED_SOLID:
    case DWG_TYPE_TABLE:
    case DWG_TYPE_TABLECONTENT:
    case DWG_TYPE_TABLEGEOMETRY:
    case DWG_TYPE_TABLESTYLE:
    case DWG_TYPE_LAYERFILTER:
    case DWG_TYPE_LAYER_INDEX:
    case DWG_TYPE_SPATIAL_FILTER:
    case DWG_TYPE_SPATIAL_INDEX:

    default:
      fail ("No add method yet type %s", name);
    }

  if (as_dxf)
    strcat (dwgfile, ".dxf");
  else
    strcat (dwgfile, ".dwg");
  if (!stat (dwgfile, &attrib))
    unlink (dwgfile);
  if (as_dxf)
    {
#ifndef DISABLE_DXF
      Bit_Chain dat = { 0 };
      dat.version = dwg->header.version;
      dat.from_version = dwg->header.from_version;
      dat.opts = dwg->opts;
      dat.fh = fopen (dwgfile, "wb");
      error = dwg_write_dxf (&dat, dwg);
      fclose (dat.fh);
#endif
    }
  else
    error = dwg_write_file (dwgfile, dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      fail ("write %s to %s: %x", name, dwgfile, error);
      return 1;
    }
  else
    ok ("write %s to %s", name, dwgfile);
  dwg_free (dwg);

#ifndef DISABLE_DXF
  if (as_dxf)
    error = dxf_read_file (dwgfile, dwg);
  else
#endif
    error = dwg_read_file (dwgfile, dwg);
  if (error >= DWG_ERR_CRITICAL)
    {
      // TODO GH #572 flaky dxf tests
#ifndef DISABLE_DXF
      if (as_dxf && error == 0x800
          && (type == DWG_TYPE_MTEXT || type == DWG_TYPE_LEADER))
        {
          ok ("TODO dxfread %s from %s: %x (GH #572)", name, dwgfile, error);
          todo = 1;
        }
      else
#endif
        {
          fail ("read %s from %s: %x", name, dwgfile, error);
          return 1;
        }
    }
  else
    ok ("read %s from %s", name, dwgfile);
  // now we have a different ref!
  mspace_ref = dwg_model_space_ref (dwg);

// look for a single written entity
#define TEST_ENTITY(token)                                                    \
  case DWG_TYPE_##token:                                                      \
    {                                                                         \
      Dwg_Entity_##token **objs = dwg_getall_##token (mspace_ref);            \
      if (objs && objs[0] && !objs[1])                                        \
        ok ("found 1 " #token);                                               \
      else if (todo)                                                          \
        ok ("TODO found no " #token);                                         \
      else if (!objs)                                                         \
        fail ("found no " #token " at all (mspace %p)", mspace_ref);          \
      else if (!objs[0])                                                      \
        fail ("found no " #token);                                            \
      else if (objs && objs[0] && objs[1])                                    \
        ok ("found many " #token "'s");                                       \
      free (objs);                                                            \
    }                                                                         \
    break

// look for a single written object
#define TEST_OBJECT(token)                                                    \
  case DWG_TYPE_##token:                                                      \
    {                                                                         \
      Dwg_Object_##token **objs = dwg_getall_##token (dwg);                   \
      if (objs && objs[0] && !objs[1])                                        \
        ok ("found 1 " #token);                                               \
      else if (!objs)                                                         \
        fail ("found no " #token " at all");                                  \
      else if (!objs[0])                                                      \
        fail ("found no " #token);                                            \
      else if (objs && objs[0] && objs[1])                                    \
        ok ("found many " #token "'s");                                       \
      free (objs);                                                            \
    }                                                                         \
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
      TEST_OBJECT (MLINESTYLE);
    case DWG_TYPE_MLINE:
      {
        Dwg_Entity_MLINE **ents = dwg_getall_MLINE (mspace_ref);
        Dwg_Object_MLINESTYLE **objs = dwg_getall_MLINESTYLE (dwg);
        if (ents && ents[0] && !ents[1])
          ok ("found 1 "
              "MLINE");
        else if (!ents)
          fail ("found no "
                "MLINE"
                " at all");
        else if (!ents[0])
          fail ("found no "
                "MLINE");
        free (ents);
        if (objs && objs[0] && !objs[1])
          ok ("found 1 "
              "MLINESTYLE");
        else if (!objs)
          fail ("found no "
                "MLINESTYLE"
                " at all");
        else if (!objs[0])
          fail ("found no "
                "MLINESTYLE");
        free (objs);
      }
      break;
      TEST_OBJECT (DIMSTYLE);
      TEST_OBJECT (UCS);
      // TEST_OBJECT (VX_TABLE_RECORD);
      TEST_ENTITY (HATCH);
      TEST_OBJECT (XRECORD);
      TEST_OBJECT (VBA_PROJECT);
      TEST_OBJECT (LAYOUT);
      TEST_ENTITY (PDFUNDERLAY);
    case TEMP_PDFDEFINITION3: // 2 defs, 4 ents
    case TEMP_PDFDEFINITION2: // 2 defs (test1 - 1), 3 ents
    case TEMP_PDFDEFINITION1: // 2 defs (test -2), 2 ents
      {
        Dwg_Entity_PDFUNDERLAY **ents = dwg_getall_PDFUNDERLAY (mspace_ref);
        Dwg_Object_PDFDEFINITION **objs = dwg_getall_PDFDEFINITION (dwg);
        Dwg_Object_DICTIONARY **dicts = dwg_getall_DICTIONARY (dwg);
        unsigned numents = num_objects (ents);
        unsigned numobjs = num_objects (objs);
        unsigned numdicts = num_objects (dicts);
        if (numents == 2 && (int)type == (int)TEMP_PDFDEFINITION1)
          ok ("found 2 PDFUNDERLAY");
        else if (numents == 3 && (int)type == (int)TEMP_PDFDEFINITION2)
          ok ("found 3 PDFUNDERLAY");
        else if (numents == 4 && (int)type == (int)TEMP_PDFDEFINITION3)
          ok ("found 4 PDFUNDERLAY");
        else
          fail ("found %d PDFUNDERLAY", numents);
        if (numobjs == 3 && (int)type != (int)TEMP_PDFDEFINITION1)
          ok ("found 3 PDFDEFINITION");
        else if (numobjs == 2 && (int)type == (int)TEMP_PDFDEFINITION1)
          ok ("found 1 PDFDEFINITION");
        else
          fail ("found %d PDFDEFINITION", numobjs);
        if (numdicts == 6)
          ok ("found 6 DICTIONARY");
        else
          fail ("found %d DICTIONARY, not 6", numdicts);
        free (ents);
        free (objs);
        free (dicts);
      }
      break;
      // TEST_OBJECT (LAYERFILTER);
      // TEST_OBJECT (LAYER_INDEX);
      // TEST_OBJECT (SPATIAL_FILTER);
      // TEST_OBJECT (SPATIAL_INDEX);
      TEST_OBJECT (ACSH_TORUS_CLASS);
      TEST_OBJECT (ACSH_SPHERE_CLASS);
      TEST_OBJECT (ACSH_CYLINDER_CLASS);
      TEST_OBJECT (ACSH_CONE_CLASS);
      TEST_OBJECT (ACSH_WEDGE_CLASS);
      TEST_OBJECT (ACSH_BOX_CLASS);
      // TEST_OBJECT (ACSH_PYRAMID_CLASS);
      // TEST_OBJECT (ACSH_CHAMFER_CLASS);
      // TEST_OBJECT (ACSH_FILLET_CLASS);
      TEST_OBJECT (WIPEOUTVARIABLES); // just for testing, not for real yet

    case TEMP_ELLIPTICAL_CONE:
    case TEMP_ELLIPTICAL_CYLINDER:
    case TEMP_EXTRUDED_SOLID:
    case TEMP_EXTRUDED_PATH:
    case TEMP_REVOLVED_SOLID:
    // TEST_OBJECT (TABLE);
    // TEST_OBJECT (TABLECONTENT);
    // TEST_OBJECT (TABLEGEOMETRY);
    // TEST_OBJECT (TABLESTYLE);
    default:
      fail ("No test yet for type %s", name);
    }

  ok ("read %s", name);
  dwg_free (dwg);
  free (dwg);
  if (debug >= 2)
    {
      char cmd[280];
      snprintf (cmd, sizeof(cmd), "../../oda %s", dwgfile);
      if (system(cmd))
        fail ("oda %s", dwgfile);
    }

  n_failed = numfailed ();
  if (!n_failed && (!debug || debug != -1))
    unlink (dwgfile);
  return n_failed;
}

static int
test_names (void)
{
  Dwg_Data *dwg;
  // more common add API tests
  if (dwg_is_valid_tag (""))
    fail ("!dwg_is_valid_tag(\"\")");
  if (dwg_is_valid_tag ("A!"))
    fail ("!dwg_is_valid_tag(\"A!\")");
  if (dwg_is_valid_tag ("A B"))
    fail ("!dwg_is_valid_tag(\"A B\")");
  if (dwg_is_valid_tag ("a"))
    fail ("!dwg_is_valid_tag(\"a\")");
  if (!dwg_is_valid_tag ("ABC"))
    fail ("dwg_is_valid_tag(\"ABC\")");

  dwg = dwg_new_Document (R_2000, 0, tracelevel);
  dwg->header.codepage = CP_CP869; // DOS Greek
  if (dwg_is_valid_name (dwg, ""))
    fail ("!dwg_is_valid_name (\"\")");
  if (!dwg_is_valid_name (dwg, "0"))
    fail ("dwg_is_valid_name (\"0\")");
  if (!dwg_is_valid_name (dwg, "*U"))
    fail ("dwg_is_valid_name (\"U*\")");
  if (!dwg_is_valid_name (dwg, "SAB_X"))
    fail ("dwg_is_valid_name (\"SAB_X\")");
  if (!dwg_is_valid_name (dwg, "SAB-X"))
    fail ("dwg_is_valid_name (\"SAB-X\")");
  if (!dwg_is_valid_name (dwg, "$SAB"))
    fail ("dwg_is_valid_name (\"$SAB\")");
  if (dwg_is_valid_name (dwg, "#SAB"))
    fail ("!dwg_is_valid_name (\"#SAB\")");
  if (dwg_is_valid_name (dwg, "%SAB"))
    fail ("!dwg_is_valid_name (\"%%SAB\")");

  if (!dwg_is_valid_name_u8 (dwg, "0"))
    fail ("dwg_is_valid_name_u8 (\"0\") CP869/r2000");
  if (dwg_is_valid_name_u8 (dwg,
                            "█")) // valid greek but no letter. \xe2\x96\x88
    fail ("!dwg_is_valid_name_u8(█) U+2588 FULL BLOCK CP869/r2000");
#ifndef HAVE_WCTYPE_H                   // locale specific
  if (!dwg_is_valid_name_u8 (dwg, "δ")) // SMALL DELTA
    fail ("dwg_is_valid_name_u8(δ) U+03B4 SMALL DELTA CP869/r2000");
#endif
  // asis as in the CP869 codepage
  if (dwg_is_valid_name (dwg, "\xdb")) // FULL BLOCK, valid greek at DB
    fail ("!dwg_is_valid_name(DB) FULL BLOCK CP869/r2000");
  if (!dwg_is_valid_name (dwg, "\xdd")) // SMALL LETTER DELTA at DD
    fail ("dwg_is_valid_name(DD) SMALL LETTER DELTA CP869/r2000");

  dwg->header.codepage = CP_CP949; // multibyte korean
  if (!dwg_is_valid_name_u8 (dwg, "0"))
    fail ("dwg_is_valid_name_u8 (\"0\")");
  if (dwg_is_valid_name_u8 (dwg, "갂")) // HANGUL SYLLABLE KIYEOK A SSANGKIYEOK
    fail ("!dwg_is_valid_name_u8(갂) U+AC02 SYLLABLE");
  if (!dwg_is_valid_name_u8 (dwg, "Ａ"))
    fail ("dwg_is_valid_name_u8(Ａ) U+FF21 LETTER CP949/r2000");
  // asis as in the CP949 codepage
  if (!dwg_is_valid_name (dwg, "0"))
    fail ("dwg_is_valid_name (\"0\")");
  if (!dwg_is_valid_name (dwg, "A")) // LATIN CAPITAL LETTER A
    fail ("dwg_is_valid_name(41) LETTER A single-byte");
  if (dwg_is_valid_name (dwg,
                         "\x81\x41")) // HANGUL SYLLABLE KIYEOK A SSANGKIYEOK
    fail ("!dwg_is_valid_name(8141) U+AC02 SYLLABLE");
  if (!dwg_is_valid_name (dwg, "\xa3\xc1"))
    fail ("dwg_is_valid_name(A3C1) U+FF21 LETTER CP949/r2000");
  dwg_free (dwg);

  dwg = dwg_new_Document (R_2007, 0, tracelevel);
  dwg->header.codepage = CP_ANSI_1253; // ANSI Greek
  if (!dwg_is_valid_name_u8 (dwg, "0"))
    fail ("dwg_is_valid_name_u8 (\"0\")");
  if (dwg_is_valid_name_u8 (dwg, "»")) // 0xbb
    fail ("!dwg_is_valid_name_u8(») U+00BB RIGHT-POINTING DOUBLE ANGLE "
          "QUOTATION MARK");
  if (!dwg_is_valid_name_u8 (dwg, "δ")) // 0xe4
    fail ("dwg_is_valid_name_u8(δ) U+03B4 SMALL DELTA ANSI_1253/r2007");
  // asis as in the CP949 codepage
  if (dwg_is_valid_name (dwg, "\xbb\x00\x00"))
    fail ("!dwg_is_valid_name(BB) U+00BB RIGHT-POINTING DOUBLE ANGLE "
          "QUOTATION MARK");
  if (!dwg_is_valid_name (dwg, "\xdd\x00\x00"))
    fail ("dwg_is_valid_name(E4) U+03B4 SMALL LETTER DELTA ANSI_1253/r2007");
  if (!dwg_is_valid_name (dwg, "0\x00\x00"))
    fail ("dwg_is_valid_name (L\"0\") ANSI_1253/r2007");

  dwg->header.codepage = CP_ANSI_949; // multibyte korean
  if (!dwg_is_valid_name_u8 (dwg, "0"))
    fail ("dwg_is_valid_name_u8 (\"0\")");
  if (dwg_is_valid_name_u8 (dwg, "갂")) // HANGUL SYLLABLE KIYEOK A SSANGKIYEOK
    fail ("!dwg_is_valid_name_u8(갂) U+AC02 SYLLABLE");
  if (!dwg_is_valid_name_u8 (dwg, "Ａ"))
    fail ("dwg_is_valid_name_u8(Ａ) U+FF21 LETTER at A3C1");
  // asis as in the CP949 codepage
  if (!dwg_is_valid_name (dwg, "B\x00\x00")) // LATIN CAPITAL LETTER A
    fail ("dwg_is_valid_name(42) LETTER B ANSI_949/r2007");
  if (dwg_is_valid_name (
          dwg, "\x41\x81\x00")) // HANGUL SYLLABLE KIYEOK A SSANGKIYEOK
    fail ("!dwg_is_valid_name(8141) U+AC02 SYLLABLE");
  if (!dwg_is_valid_name (dwg, "\xc1\xa3\x00"))
    fail ("dwg_is_valid_name(A3C1) U+FF21 LETTER ANSI_949/r2007");
  if (!dwg_is_valid_name (dwg, "0\x00\x00"))
    fail ("dwg_is_valid_name (\"0\") ANSI_949/r2007");

  dwg_free (dwg);
  if (!numfailed ())
    ok ("names");
  return numfailed ();
}

static int
test_api_version (void)
{
  const char *version = dwg_api_version_string ();
  long i0, i1, i2;
  char *d0, *d1, *d2;
  const int major = dwg_api_version_major ();
  const int minor = dwg_api_version_minor ();

  assert (version);
  assert (strEQc (dwg_api_version_string (), PACKAGE_VERSION));
  assert (major == LIBREDWG_VERSION_MAJOR);
  assert (minor == LIBREDWG_VERSION_MINOR);

  d0 = strchr (version, '.');
  if (d0) // or git hash only. no tags fetched
    {
      assert (d0);
      d1 = strchr (&d0[1], '.');
      assert (d1);
      // d2 = strchr (&d1[1], '.');

      // assert (strEQc(dwg_api_so_version (), "0:13:0")); //
      // LIBREDWG_SO_VERSION check that major and minor match the tag
      i0 = atoi (version);
      i1 = atoi (&d0[1]);
      assert (major == i0);
      assert (minor == i1);
    }
  else
    {
#ifdef IS_RELEASE
      fail ("no tags in version_string %s", version);
#else
      ok ("TODO no tags in version_string %s. Need to git fetch --depth 50 "
          "and re-configure",
          version);
#endif
    }

  // check libtool versioning
  version = dwg_api_so_version ();
  d0 = strchr (version, ':');
  d1 = strchr (&d0[1], ':');
  assert (d0);
  assert (d1);
  i0 = atoi (version); // current 0
  i1 = atoi (&d0[1]);  // revision
  i2 = atoi (&d1[1]);  // age
  if (major + minor == i0 + i1 + i2)
    ok ("so_version %s matches major %d + minor %d", version, major, minor);
  else
    fail ("so_version %s: %d + %d != %ld + %ld + %ld", version, major, minor,
          i0, i1, i2);

#ifndef IS_RELEASE
  assert (!dwg_api_version_is_release ());
#endif
  ok ("api_version %s", dwg_api_version_string ());
  return numfailed ();
}

int
main (int argc, char *argv[])
{
  int error = 0;
  char *trace = getenv ("LIBREDWG_TRACE");    // read_dwg
  char *debugenv = getenv ("LIBREDWG_DEBUG"); // keep files
  int dxf = 0;

  loglevel = loglevel_from_env ();
  if (trace)
    tracelevel = atoi (trace);
  else
    tracelevel = 0;
  if (debugenv) // -1 for all, 0 for none, 12 for ATTRIB only
    debug = atoi (debugenv);
  else
    debug = 0;

  error = test_names ();
  error += test_api_version ();

#ifndef DISABLE_DXF
  for (; dxf < 2; dxf++)
#endif
    {
      error += test_add (DWG_TYPE_LINE, "add_line_2000", dxf);
      error += test_add (DWG_TYPE_TEXT, "add_text_2000", dxf);
      error += test_add (DWG_TYPE_CIRCLE, "add_circle_2000", dxf);
      error += test_add (DWG_TYPE_ARC, "add_arc_2000", dxf);
      error += test_add (DWG_TYPE_LWPOLYLINE, "add_lwpline_2000", dxf);
      error += test_add (DWG_TYPE_POLYLINE_2D, "add_pl2d_2000", dxf);
      error += test_add (DWG_TYPE_POLYLINE_3D, "add_pl3d_2000", dxf);
      error += test_add (DWG_TYPE_POLYLINE_MESH, "add_pmesh_2000", dxf);
      error += test_add (DWG_TYPE_POLYLINE_PFACE, "add_pface_2000", dxf);
      error += test_add (DWG_TYPE_SPLINE, "add_spline_2000", dxf);
      error += test_add (DWG_TYPE_INSERT, "add_insert_2000", dxf);
      error += test_add (DWG_TYPE_MINSERT, "add_minsert_2000", dxf);
      if (debug == cnt || debug == -1)
        error += test_add (DWG_TYPE_ATTRIB, "add_attrib_2000", dxf);
      else
        ok ("skip ATTRIB TODO add_Attribute");
      error += test_add (DWG_TYPE_DIMENSION_ALIGNED, "add_dimali_2000", dxf);
      error += test_add (DWG_TYPE_DIMENSION_ANG2LN, "add_dimang_2000", dxf);
      error += test_add (DWG_TYPE_DIMENSION_ANG3PT, "add_dim3pt_2000", dxf);
      error += test_add (DWG_TYPE_DIMENSION_DIAMETER, "add_dimdia_2000", dxf);
      error += test_add (DWG_TYPE_DIMENSION_ORDINATE, "add_dimord_2000", dxf);
      error += test_add (DWG_TYPE_DIMENSION_RADIUS, "add_dimrad_2000", dxf);
      error += test_add (DWG_TYPE_DIMENSION_LINEAR, "add_dimlin_2000", dxf);
      error += test_add (DWG_TYPE_POINT, "add_point_2000", dxf);
      error += test_add (DWG_TYPE__3DFACE, "add_3dface_2000", dxf);
      error += test_add (DWG_TYPE_SOLID, "add_solid_2000", dxf);
      error += test_add (DWG_TYPE_TRACE, "add_trace_2000", dxf);
      error += test_add (DWG_TYPE_SHAPE, "add_shape_2000", dxf);
      error += test_add (DWG_TYPE_VIEWPORT, "add_viewport_2000", dxf);
      error += test_add (DWG_TYPE_ELLIPSE, "add_ellipse_2000", dxf);
      error += test_add (DWG_TYPE_REGION, "add_region_2000", dxf);
      error += test_add (DWG_TYPE_RAY, "add_ray_2000", dxf);
      error += test_add (DWG_TYPE_XLINE, "add_xline_2000", dxf);
      error += test_add (DWG_TYPE_DICTIONARY, "add_dict_2000", dxf);
      error += test_add (DWG_TYPE_DICTIONARYWDFLT, "add_dictwdflt_2000", dxf);
      error += test_add (DWG_TYPE_OLE2FRAME, "add_ole2frame_2000", dxf);
      error += test_add (DWG_TYPE_MTEXT, "add_mtext_2000", dxf);
      error += test_add (DWG_TYPE_LEADER, "add_leader_2000", dxf);
      error += test_add (DWG_TYPE_TOLERANCE, "add_tolerance_2000", dxf);
      error += test_add (DWG_TYPE_MLINESTYLE, "add_mlstyle_2000", dxf);
      error += test_add (DWG_TYPE_MLINE, "add_mline_2000", dxf);
      error += test_add (DWG_TYPE_DIMSTYLE, "add_dimstyle_2000", dxf);
      error += test_add (DWG_TYPE_UCS, "add_ucs_2000", dxf);
      // error += test_add (DWG_TYPE_VX_TABLE_RECORD, "add_vx_2000", dxf);
      error += test_add (DWG_TYPE_HATCH, "add_hatch_2000", dxf);
      error += test_add (DWG_TYPE_XRECORD, "add_xrecord_2000", dxf);
      error += test_add (DWG_TYPE_VBA_PROJECT, "add_vba_2000", dxf);
      error += test_add (DWG_TYPE_LAYOUT, "add_layout_2000", dxf);
#ifdef HAVE_DWG_ADD_PDFUNDERLAY
      error += test_add (DWG_TYPE_PDFUNDERLAY, "add_pdfunderlay_2000", dxf);
      error += test_add ((const Dwg_Object_Type)TEMP_PDFDEFINITION1,
                         "add_pdfdef1_2000", dxf);
      error += test_add ((const Dwg_Object_Type)TEMP_PDFDEFINITION2,
                         "add_pdfdef2_2000", dxf);
      error += test_add ((const Dwg_Object_Type)TEMP_PDFDEFINITION3,
                         "add_pdfdef3_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_LAYERFILTER
      error += test_add (DWG_TYPE_LAYERFILTER, "add_layfilt_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_LAYER_INDEX
      error += test_add (DWG_TYPE_LAYER_INDEX, "add_layidx_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_SPATIAL_FILTER
      error += test_add (DWG_TYPE_SPATIAL_FILTER, "add_spatialfilt_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_SPATIAL_INDEX
      error += test_add (DWG_TYPE_SPATIAL_INDEX, "add_spatialidx_2000", dxf);
#endif
      error += test_add (DWG_TYPE_ACSH_TORUS_CLASS, "add_torus_2000", dxf);
      error += test_add (DWG_TYPE_ACSH_SPHERE_CLASS, "add_sphere_2000", dxf);
      error
          += test_add (DWG_TYPE_ACSH_CYLINDER_CLASS, "add_cylinder_2000", dxf);
      error += test_add (DWG_TYPE_ACSH_CONE_CLASS, "add_cone_2000", dxf);
      error += test_add (DWG_TYPE_ACSH_WEDGE_CLASS, "add_wedge_2000", dxf);
      error += test_add (DWG_TYPE_ACSH_BOX_CLASS, "add_box_2000", dxf);
#ifdef HAVE_DWG_ADD_CHAMFER
      error += test_add (DWG_TYPE_ACSH_CHAMFER_CLASS, "add_chamfer_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_PYRAMID
      error += test_add (DWG_TYPE_ACSH_PYRAMID_CLASS, "add_pyramid_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_ELLIPTICAL_CONE
      // error += test_add (TEMP_ELLIPTICAL_CONE, "add_ellcone_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_ELLIPTICAL_CYLINDER
      // error += test_add (TEMP_ELLIPTICAL_CYLINDER, "add_ellcyl_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_EXTRUDED_SOLID
      // error += test_add (TEMP_EXTRUDED_SOLID, "add_extsolid_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_EXTRUDED_PATH
      // error += test_add (TEMP_EXTRUDED_PATH, "add_extpath_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_REVOLVED_SOLID
      // error += test_add (TEMP_REVOLVED_SOLID, "add_revsolid_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_TABLE
      error += test_add (DWG_TYPE_TABLE, "add_table_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_TABLECONTENT
      error += test_add (DWG_TYPE_TABLECONTENT, "add_tablecontent_2000", dxf);
#endif
#ifdef HAVE_DWG_ADD_TABLEGEOMETRY
      error
          += test_add (DWG_TYPE_TABLEGEOMETRY, "add_tablegeometry_2000", dxf);
#endif

      // just for testing yet
      error
          += test_add (DWG_TYPE_WIPEOUTVARIABLES, "add_wipeoutvars_2000", dxf);
    }

  return error;
}

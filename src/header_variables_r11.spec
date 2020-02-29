/* -*- c -*- */
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
 * header_variables_r11.spec: DWG pre-R13 header variables specification
 * written by Reini Urban
 */

#include "spec.h"

  FIELD_3RD (INSBASE, 10); //ok 0x5e
  FIELD_CAST (PLINEGEN, RS, B, 70); //ok
  FIELD_3RD (EXTMIN, 10);
  FIELD_3RD (EXTMAX, 10);
  FIELD_2RD (LIMMIN, 10);
  FIELD_2RD (LIMMAX, 10);
  FIELD_2RD (VIEWCTR, 12);
  FIELD_RD (unknown_0, 0); //0d8
  FIELD_RD (VIEWSIZE, 40);
  FIELD_RS (SNAPMODE, 70);    //ineffective
  FIELD_2RD (SNAPUNIT, 14);   //??
  FIELD_2RD (SNAPBASE, 13);   //??
  FIELD_RD (SNAPANG, 50);     //??
  FIELD_RS (SNAPSTYL, 70);    //77 ??
  FIELD_RS (SNAPISOPAIR, 70); //78 ??
  FIELD_RS (GRIDMODE, 70);    //76 ??
  FIELD_2RD (GRIDUNIT, 15);
  FIELD_CAST (ORTHOMODE, RS, B, 70); //ok
  FIELD_CAST (REGENMODE, RS, B, 70); //ok
  FIELD_CAST (FILLMODE, RS, B, 70);  //ok
  FIELD_CAST (QTEXTMODE, RS, B, 70); //ok
  FIELD_RS (DRAGMODE, 70); // 2 ineffective with r12
  FIELD_RD (LTSCALE, 40);  // 16.0 confirmed
  FIELD_RD (TEXTSIZE, 40); //unconfirmed, ineffective with r12
  FIELD_RD (TRACEWID, 40); //ok
  DEBUG_HERE // 14a
  //RS TEXTSTYLE STYLE index?
  //RS 15

  dat->byte = 0x156;
  FIELD_CAST (PSLTSCALE, RS, B, 70);

  dat->byte = 0x164;
  FIELD_RS (LUNITS, 70); //ok
  FIELD_RS (LUPREC, 70); //ok
  DEBUG_HERE//@168
  //RS 2xRD
  dat->byte = 0x17a;
  FIELD_RD (SKETCHINC, 40); //ok
  FIELD_RD (FILLETRAD, 40); //ok
  FIELD_RS (AUNITS, 70);    //ok
  FIELD_RS (AUPREC, 70);    //ok
  //DEBUG_HERE//@18e
  FIELD_RS (OSMODE, 70);    //registry?
  // 05000000 01004143 41440043 41440000
  //RC 0
  //RS 25
  FIELD_CAST (unknown_12, RS, BL, 70); //??
  //DEBUG_POS; dat->byte = 0x192; DEBUG_POS
  FIELD_RS (ATTMODE, 70);
  FIELD_TFv (MENU, 15, 1);
  FIELD_RD (DIMSCALE, 40); //ok 0x1a3
  FIELD_RD (DIMASZ, 40); //ok
  FIELD_RD (DIMEXO, 40); //ok
  FIELD_RD (DIMDLI, 40); //ok
  FIELD_RD (DIMEXE, 40); //ok
  FIELD_RD (DIMTP, 40);  //ok
  FIELD_RD (DIMTM, 40);  //ok
  FIELD_RD (DIMTXT, 40); //ok
  FIELD_RD (DIMCEN, 40); //ok
  FIELD_RD (DIMTSZ, 40); //ok
  FIELD_RC (DIMTOL, 70); //ok 1f3
  FIELD_RC (DIMLIM, 70); //ok 1f4
  FIELD_RC (DIMTIH, 70); //ok 1f5
  FIELD_RC (DIMTOH, 70); //ok 1f6
  FIELD_RC (DIMSE1, 70); //ok
  FIELD_RC (DIMSE2, 70); //ok
  FIELD_CAST (DIMTAD, RC, RS, 70); //ok
  FIELD_RC (LIMCHECK, 70); //ok 1fa
  DEBUG_HERE //1fb

  // not in DWG, resp. not converted by ODA from r12 dxf to r12 dwg:
  // SNAPMODE, DRAGMODE, BLIPMODE, CHAMFERA, CHAMFERB,
  // COORDS, ATTDIA, ATTREQ, HANDLING, WORLDVIEW, SHADEDGE

  //the names?
  dat->byte = 0x229;
  FIELD_RD (ELEVATION, 40); //ok
  FIELD_RD (THICKNESS, 40); //ok
  DEBUG_HERE //239

  dat->byte = 0x2e5;
  FIELD_CAST (DIMZIN, RC, B, 70); //ok
  FIELD_RD (DIMRND, 40);
  FIELD_RD (DIMDLE, 40);
  DEBUG_HERE //2ee

  /*
  TODO...
  FIELD_HANDLE (UCSNAME, ANYCODE);

  FIELD_HANDLE (BLOCK_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (LAYER_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (STYLE_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (LINETYPE_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (VIEW_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (UCS_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (VPORT_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (APPID_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (DIMSTYLE_CONTROL_OBJECT, ANYCODE);

  FIELD_HANDLE (DICTIONARY_ACAD_GROUP, ANYCODE);
  FIELD_HANDLE (DICTIONARY_ACAD_MLINESTYLE, ANYCODE);
  FIELD_HANDLE (DICTIONARY_NAMED_OBJECTS, ANYCODE);

  FIELD_HANDLE (BLOCK_RECORD_PSPACE, ANYCODE);
  FIELD_HANDLE (BLOCK_RECORD_MSPACE, ANYCODE);
  FIELD_HANDLE (LTYPE_BYLAYER, ANYCODE);
  FIELD_HANDLE (LTYPE_BYBLOCK, ANYCODE);
  FIELD_HANDLE (LTYPE_CONTINUOUS, ANYCODE);

  */
  dat->byte = 0x31b;
  FIELD_RS (CECOLOR.index, 62);
  DECODER {
    _obj->CELTYPE = (BITCODE_H)calloc(1, sizeof(Dwg_Object_Ref));
    _obj->CELTYPE->absolute_ref = (BITCODE_RL)bit_read_RS (dat); // 6, ff for BYLAYER, fe for BYBLOCK
    LOG_TRACE ("CELTYPE: %lu [long 6]\n", _obj->CELTYPE->absolute_ref)
  }
  FIELD_TIMEBLL (TDCREATE, 40);
  FIELD_TIMEBLL (TDUPDATE, 40);
  FIELD_TIMEBLL (TDINDWG, 40);
  FIELD_TIMEBLL (TDUSRTIMER, 40);
  DEBUG_HERE //

  dat->byte = 0x33f;
  FIELD_CAST (USRTIMER, RS, B, 70);
  FIELD_RS (unknown_10, 70);
  FIELD_CAST (SKPOLY, RS, B, 70);
  DEBUG_HERE //345

  dat->byte = 0x353;
  FIELD_RD (ANGBASE, 50);
  FIELD_CAST (ANGDIR, RS, B, 70);
  FIELD_RS (PDMODE, 70);
  FIELD_RD (PDSIZE, 40);
  FIELD_RD (PLINEWID, 40);
  FIELD_RS (USERI1, 70);
  FIELD_RS (USERI2, 70);
  FIELD_RS (USERI3, 70);
  FIELD_RS (USERI4, 70);
  FIELD_RS (USERI5, 70);
  FIELD_RD (USERR1, 40);
  FIELD_RD (USERR2, 40);
  FIELD_RD (USERR3, 40);
  FIELD_RD (USERR4, 40);
  FIELD_RD (USERR5, 40);
  FIELD_RC (DIMALT, 70); //ok
  FIELD_CAST (DIMALTD, RC, RS, 70); //ok
  FIELD_RC (DIMASO, 70); //ok
  FIELD_RC (DIMSHO, 70); //ok
  FIELD_CAST (DIMTOFL, RS, B, 70);
  DEBUG_HERE //3a7

  dat->byte = 0x3c5;
  FIELD_RD (DIMALTF, 40);
  FIELD_RD (DIMLFAC, 40);
  FIELD_RS (SPLINESEGS, 70);
  FIELD_CAST (SPLFRAME, RS, B, 70);
  DEBUG_HERE //3d9

  dat->byte = 0x3ed;
  FIELD_CAST (MIRRTEXT, RS, B, 70); //confirmed
  DEBUG_HERE //3ef
  // UCS: 3ef

  dat->byte = 0x3fb;
  FIELD_3RD (UCSORG, 10); //ok
  FIELD_3RD (UCSXDIR, 11); //ok
  FIELD_3RD (UCSYDIR, 12); //ok
  DEBUG_HERE //443

  dat->byte = 0x47d;
  FIELD_RC (DIMTOFL, 70); //ok
  DEBUG_HERE //47e

  dat->byte = 0x4c0;
  FIELD_RC (DIMSAH, 70); //ok
  FIELD_RC (DIMTIX, 70); //ok
  FIELD_RC (DIMSOXD, 70); //ok
  FIELD_RD (DIMTVP, 40); //ok
  DEBUG_HERE //4cb

  dat->byte = 0x4ee;
  DECODER {
    _obj->HANDSEED = (BITCODE_H)calloc(1, sizeof(Dwg_Object_Ref));
    _obj->HANDSEED->absolute_ref = (BITCODE_RL)bit_read_RS (dat);
    LOG_TRACE ("HANDSEED: %lX [RS 5]\n", _obj->HANDSEED->absolute_ref)
  }
  DEBUG_HERE //4f0

  dat->byte = 0x4f6;
  FIELD_RS (SURFU, 70); //ok
  FIELD_RS (SURFV, 70); //ok
  FIELD_RS (SURFTYPE, 70); //ok
  FIELD_RS (SURFTAB1, 70); //ok
  FIELD_RS (SURFTAB2, 70); //ok
  DEBUG_HERE //500 VPORT

  dat->byte = 0x50c;
  FIELD_RS (SPLINETYPE, 70);
  DEBUG_HERE //50e
  // 3

  dat->byte = 0x531;
  FIELD_RS (DIMCLRD_C, 70); //ok
  FIELD_RS (DIMCLRE_C, 70); //ok
  FIELD_RS (DIMCLRT_C, 70); //ok
  FIELD_RS (SHADEDGE, 70); //ok
  FIELD_RS (SHADEDIF, 70); //ok
  FIELD_RS (UNITMODE, 70); //ok
  DEBUG_HERE //53d

  dat->byte = 0x5df;
  FIELD_RD (DIMTFAC, 40); //ok
  FIELD_3RD (PUCSORG, 10); //ok
  FIELD_3RD (PUCSXDIR, 11); //ok
  FIELD_3RD (PUCSYDIR, 12); //ok
  FIELD_RS (unknown_10, 70); //ff ff/-1
  FIELD_RC (TILEMODE, 70); //ok
  FIELD_CAST (PLIMCHECK, RS, B, 70); //ok
  FIELD_RS (unknown_10, 70); //0
  FIELD_RC (unknown_11, 70); //0
  FIELD_3RD (PEXTMIN, 10); //ok 637
  FIELD_3RD (PEXTMAX, 10); //
  FIELD_2RD (PLIMMIN, 10); //
  FIELD_2RD (PLIMMAX, 10); //
  DEBUG_HERE //687

  dat->byte = 0x6a9;
  FIELD_RS (MAXACTVP, 70); //ok
  FIELD_RD (DIMGAP, 40);   //ok
  FIELD_RD (PELEVATION, 40); //ok
  FIELD_CAST (VISRETAIN, RS, B, 70); //ok



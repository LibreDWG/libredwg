/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2022 Free Software Foundation, Inc.                   */
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
 * improved by Michal Josef Špaček
 */

  #include "spec.h"

  FIELD_3RD (INSBASE, 10); //ok 0x5e
  VERSIONS (R_10, R_11) {
    FIELD_CAST (PLINEGEN, RS, B, 70); //ok
  } else {
    PRE (R_2_0)
      FIELD_RL (num_bytes, 0);
    PRE (R_10)
      FIELD_RS (num_entities, 0);
  }
  FIELD_3RD (EXTMIN, 10);
  FIELD_3RD (EXTMAX, 10);
  FIELD_2RD (LIMMIN, 10);
  FIELD_2RD (LIMMAX, 10);
  FIELD_3RD (VIEWCTR, 10);
  FIELD_RD (VIEWSIZE, 40);
  FIELD_RS (SNAPMODE, 70);    //unhandled by ODA
  PRE (R_2_0) {
    FIELD_RD (SNAPUNIT.x, 10);
  }
  LATER_VERSIONS {
    FIELD_2RD (SNAPUNIT, 14);   //??
    FIELD_2RD (SNAPBASE, 13);   //??
    FIELD_RD (SNAPANG, 50);     //??
    FIELD_RS (SNAPSTYL, 70);    //77 ??
    FIELD_RS (SNAPISOPAIR, 70); //78 ??
  }
  FIELD_RS (GRIDMODE, 70);    //76 ??
  PRE (R_2_0) {
    FIELD_RD (GRIDUNIT.x, 10);
  }
  LATER_VERSIONS {
    FIELD_2RD (GRIDUNIT, 10);
  }
  FIELD_CAST (ORTHOMODE, RS, B, 70); //ok
  FIELD_CAST (REGENMODE, RS, B, 70); //ok
  FIELD_CAST (FILLMODE, RS, B, 70);  //ok
  SINCE (R_2_0) {
    FIELD_CAST (QTEXTMODE, RS, B, 70); //ok
    FIELD_RS (DRAGMODE, 70); // 2 ineffective with r12
    FIELD_RD (LTSCALE, 40);  // 16.0 confirmed
  }
  FIELD_RD (TEXTSIZE, 40); //ok ineffective with r12
  FIELD_RD (TRACEWID, 40); //ok
  FIELD_HANDLE (CLAYER, 2, 8);
  PRE (R_2_0) {
    FIELD_RS (CECOLOR.index, 62);
  } else {
    FIELD_RL (oldCECOLOR_lo, 0); // CECOLOR converted from older DWG file
    FIELD_RL (oldCECOLOR_hi, 0); //            -"-
  }
  VERSIONS (R_1_2, R_1_4) {
    BITCODE_BL vcount;
    FIELD_VECTOR_INL (layer_colors, RS, 128, 0); // color of each layer
    FIELD_RD (DIMASZ, 40); //ok
    FIELD_RD (unknown_7, 40); //?
    VERSION (R_1_2) {
      dwg->header.dwg_version = 1;
      return error;
    }
  } else {
    FIELD_RS (unknown_5, 0);
    FIELD_CAST (PSLTSCALE, RS, B, 70);
    FIELD_RS (TREEDEPTH, 70);
    FIELD_RS (unknown_6, 0);
    FIELD_RD (unknown_7, 0); // converted from older DWG file (0x01d0)
  }
  FIELD_RS (LUNITS, 70); //ok
  FIELD_RS (LUPREC, 70); //ok
  VERSION (R_1_4) {
    FIELD_RS (DIMTOL, 70); // dim_text_within_dimension
    FIELD_RS (DIMLIM, 70); // dim_text_outside_of_dimension
  }
  FIELD_RS (AXISMODE, 70);
  FIELD_2RD (AXISUNIT, 10);
  FIELD_RD (SKETCHINC, 40); //ok default 0.1
  FIELD_RD (FILLETRAD, 40); //ok
  VERSION (R_1_4) {
    dwg->header.dwg_version = 2;
    return error;
  }
  FIELD_RS (AUNITS, 70);    //ok
  FIELD_RS (AUPREC, 70);    //ok
  FIELD_HANDLE (TEXTSTYLE, 2, 7);
  FIELD_CAST (OSMODE, RS, BL, 70);
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
  PRE (R_2_0) // AC1.2 definitely
    return 0;
  FIELD_RC (DIMTOL, 70); //ok 1f3
  FIELD_RC (DIMLIM, 70); //ok 1f4
  FIELD_RC (DIMTIH, 70); //ok 1f5
  FIELD_RC (DIMTOH, 70); //ok 1f6
  FIELD_RC (DIMSE1, 70); //ok
  FIELD_RC (DIMSE2, 70); //ok
  FIELD_CAST (DIMTAD, RC, RS, 70); //ok
  if (dwg->header.numheader_vars <= 74)
    return 0;
  FIELD_RC (LIMCHECK, 70); //ok 1fa

  /* TODO Unknown structure (0x01fc-0x0228) */
  // PLATFORM was until r11
  DEBUG_HERE //1fb

  dat->byte = 0x229;
  FIELD_RD (ELEVATION, 40); //ok
  FIELD_RD (THICKNESS, 40); //ok
  FIELD_3RD (VIEWDIR, 10);

  /* TODO Unknown repeating variable - 18 floats, probably 6x 3d point */
  // probably RD sysvars: LASTANGLE, LASTPOINT, LASTPT3D. until r11
  // also there is VPOINTX/VPOINTY/VPOINTZ (replaced by VIEWDIR with r11)
  DEBUG_HERE //252

  dat->byte = 0x2e1;
  FIELD_RS (unknown_18, 0);
  FIELD_CAST (BLIPMODE, RS, B, 70);
  if (dwg->header.numheader_vars <= 83) // PRE(R_2_21)
     return 0;
  FIELD_CAST (DIMZIN, RC, B, 70); //ok
  FIELD_RD (DIMRND, 40);
  FIELD_RD (DIMDLE, 40);
  FIELD_TFv (DIMBLK_T, 33, 1);
  FIELD_RS (circle_zoom_percent, 0);
  FIELD_RS (COORDS, 0);
  FIELD_RS (CECOLOR.index, 62);
  DECODER {
    _obj->CELTYPE = (BITCODE_H)calloc(1, sizeof(Dwg_Object_Ref));
    // 6, ff for BYLAYER, fe for BYBLOCK
    _obj->CELTYPE->absolute_ref = (BITCODE_RL)bit_read_RS (dat);
    LOG_TRACE ("CELTYPE: %lu [long 6]\n", _obj->CELTYPE->absolute_ref)
  }
  FIELD_TIMERLL (TDCREATE, 40);
  FIELD_TIMERLL (TDUPDATE, 40);
  FIELD_TIMERLL (TDINDWG, 40);
  FIELD_TIMERLL (TDUSRTIMER, 40);
  FIELD_CAST (USRTIMER, RS, B, 70);
  FIELD_CAST (FASTZOOM, RS, B, 70);
  FIELD_RS (unknown_10, 0);
  FIELD_CAST (SKPOLY, RS, B, 70);

  /* TODO Unknown date structure (month, day, year, hour, minute, second, ms - all RS) */
  DEBUG_HERE //345

  dat->byte = 0x353;
  FIELD_RD (ANGBASE, 50);
  FIELD_CAST (ANGDIR, RS, B, 70);
  if (dwg->header.numheader_vars <= 101)
    return 0;
  FIELD_RS (PDMODE, 70);
  FIELD_RD (PDSIZE, 40);
  FIELD_RD (PLINEWID, 40);
  if (dwg->header.numheader_vars <= 104)
    return 0;

  /* TODO Signed */
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
  FIELD_TFv (DIMPOST, 16, 1);
  FIELD_TFv (DIMAPOST, 16, 1);
  if (dwg->header.numheader_vars <= 120)
    return 0;
  FIELD_RD (DIMALTF, 40);
  FIELD_RD (DIMLFAC, 40);
  if (dwg->header.numheader_vars <= 122)
    return 0;
  FIELD_RS (SPLINESEGS, 70);
  FIELD_CAST (SPLFRAME, RS, B, 70);
  FIELD_RS (ATTREQ, 70);
  FIELD_RS (ATTDIA, 70);
  FIELD_RD (CHAMFERA, 40);
  FIELD_RD (CHAMFERB, 40);
  FIELD_CAST (MIRRTEXT, RS, B, 70);
  if (dwg->header.numheader_vars <= 129)
    return 0;

  /* Skip table UCS (0x3ef-0x3f9) */
  dat->byte = 0x3fa;

  FIELD_RC (unknown_58, 0);
  FIELD_3RD (UCSORG, 10); //ok
  FIELD_3RD (UCSXDIR, 11); //ok
  FIELD_3RD (UCSYDIR, 12); //ok
  FIELD_3RD (TARGET, 0);
  FIELD_RD (LENSLENGTH, 0);
  FIELD_RD (VIEWTWIST, 0);
  FIELD_RD (FRONTZ, 0);
  FIELD_RD (BACKZ, 0);
  FIELD_CAST (VIEWMODE, RS, B, 70);
  FIELD_RC (DIMTOFL, 70); //ok
  FIELD_TFv (DIMBLK1_T, 33, 1);
  FIELD_TFv (DIMBLK2_T, 33, 1);
  FIELD_RC (DIMSAH, 70); //ok
  FIELD_RC (DIMTIX, 70); //ok
  FIELD_RC (DIMSOXD, 70); //ok
  FIELD_RD (DIMTVP, 40); //ok
  FIELD_TFv (unknown_string, 33, 1);
  FIELD_RS (HANDLING, 70); // use new HEX handles (should be RC)

  /* TODO fix HANDSEED - 00 00 00 00 00 00 12 35 mean 0x1235 */
  dat->byte = 0x4ee;
  DECODER {
    _obj->HANDSEED = (BITCODE_H)calloc(1, sizeof(Dwg_Object_Ref));
    _obj->HANDSEED->absolute_ref = (BITCODE_RL)bit_read_RS (dat);
    LOG_TRACE ("HANDSEED: %lX [RS 5]\n", _obj->HANDSEED->absolute_ref)
  }
  DEBUG_HERE

  dat->byte = 0x4f6;
  FIELD_RS (SURFU, 70); //ok
  FIELD_RS (SURFV, 70); //ok
  FIELD_RS (SURFTYPE, 70); //ok
  FIELD_RS (SURFTAB1, 70); //ok
  FIELD_RS (SURFTAB2, 70); //ok

  /* Skip table VPORT (0x500-0x509 )*/
  dat->byte = 0x50a;

  FIELD_CAST (FLATLAND, RS, B, 70);
  FIELD_RS (SPLINETYPE, 70);
  FIELD_RS (UCSICON, 0);
  FIELD_RS (unknown_59, 0); // ff ff
  if (dwg->header.numheader_vars <= 158) // r10
    return 0;

  /* Skip table APPID (0x512-0x51c) */
  dat->byte = 0x51d;

  FIELD_CAST (WORLDVIEW, RS, B, 70);
  if (dwg->header.numheader_vars <= 160) // r10
    return 0;
  FIELD_RS (unknown_51e, 0);
  FIELD_RS (unknown_520, 0);
  // TILEMODE came with r11

  /* Skip table DIMSTYLE (0x522-0x52b) */
  dat->byte = 0x52c;

  /* TODO Unknown 5 bytes. (first two bytes sometimes ff ff) */
  DEBUG_HERE

  dat->byte = 0x531;
  FIELD_RS (DIMCLRD_C, 70); //ok
  FIELD_RS (DIMCLRE_C, 70); //ok
  FIELD_RS (DIMCLRT_C, 70); //ok
  FIELD_RS (SHADEDGE, 70); //ok
  FIELD_RS (SHADEDIF, 70); //ok
  FIELD_RS (UNITMODE, 70); //ok, new with r11

  /* TODO Unknown 34 bytes */
  DEBUG_HERE //53d

  dat->byte = 0x55f;
  FIELD_TFv (unknown_unit1, 32, 1);
  FIELD_TFv (unknown_unit2, 32, 1);
  FIELD_TFv (unknown_unit3, 32, 1);
  FIELD_TFv (unknown_unit4, 32, 1);
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
  FIELD_3RD (PINSBASE, 10);

  /* Skip table VX (0x69f-0x6a8) */
  dat->byte = 0x6a9;

  FIELD_RS (MAXACTVP, 70); //ok
  FIELD_RD (DIMGAP, 40);   //ok
  FIELD_RD (PELEVATION, 40); //ok
  if (dwg->header.numheader_vars <= 204)
    return 0;
  FIELD_CAST (VISRETAIN, RS, B, 70); //ok


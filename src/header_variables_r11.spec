/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2025 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * header_variables_r11.spec: DWG pre-R13 header variables specification.
 * For DXF see header_variables_dxf.spec
 * written by Reini Urban
 * modified by Michal Josef Špaček
 */

  #include "spec.h"

  FIELD_3RD (INSBASE, 10); //ok 0x5e
  VERSIONS (R_10, R_11) {
    FIELD_CAST (PLINEGEN, RS, B, 70); //ok
  } else {
    PRE (R_2_0)
      FIELD_RL (dwg_size, 0);
    PRE (R_10)
      FIELD_RS (numentities, 0);
  }
  FIELD_3RD (EXTMIN, 10);
  FIELD_3RD (EXTMAX, 10);
  FIELD_2RD (LIMMIN, 10);
  FIELD_2RD (LIMMAX, 10);
  FIELD_3RD (VIEWCTR, 10);
  FIELD_RD (VIEWSIZE, 40);
  FIELD_RS (SNAPMODE, 70);    //unhandled by ODA
  PRE (R_2_0) {
#ifdef IS_JSON
    FIELD_2RD (SNAPUNIT, 10);
#else
    FIELD_RD (SNAPUNIT.x, 10);
#endif
  }
  LATER_VERSIONS {
    FIELD_2RD (SNAPUNIT, 14);   //??
    FIELD_2RD (SNAPBASE, 13);   //??
    FIELD_RD (SNAPANG, 50);     //??
    FIELD_RS (SNAPSTYLE, 70);   //77 ??
    FIELD_RS (SNAPISOPAIR, 70); //78 ??
  }
  FIELD_RS (GRIDMODE, 70);    //76 ??
  PRE (R_2_0) {
#ifdef IS_JSON
    FIELD_2RD (GRIDUNIT, 10);
#else
    FIELD_RD (GRIDUNIT.x, 10);
#endif
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
#ifdef IS_JSON
    FIELD_CMC (CECOLOR, 62);
#else
    FIELD_RS (CECOLOR.index, 62);
#endif
  } else {
    FIELD_RL (oldCECOLOR_lo, 0); // CECOLOR converted from older DWG file
    FIELD_RL (oldCECOLOR_hi, 0); //            -"-
  }
  VERSIONS (R_1_1, R_1_4) {
    BITCODE_BL vcount;
    FIELD_VECTOR_INL (layer_colors, RS, 128, 0); // color of each layer
    VERSION (R_1_1) {
      dwg->header.dwg_version = 1;
      return error;
    }
  }
  VERSIONS (R_1_2, R_1_4) {
    FIELD_RD (DIMARROW, 40); //ok
    FIELD_RD (aspect_ratio, 40); // calculated
    VERSION (R_1_2) {
      dwg->header.dwg_version = 2;
      return error;
    }
  } else {
    FIELD_RS (unknown_5, 0);
    VERSIONS (R_2_0b, R_9) {
      FIELD_RS (unknown_6a, 0);
      FIELD_RS (unknown_6b, 0);
      FIELD_RS (unknown_6c, 0);
    } else {
      FIELD_CAST (PSLTSCALE, RS, B, 70);
      FIELD_RS (TREEDEPTH, 70);
      FIELD_RS (unknown_6, 0);
    }
    FIELD_RD (aspect_ratio, 0); // calculated
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
    dwg->header.dwg_version = 3;
    return error;
  }
  FIELD_RS (AUNITS, 70);    //ok
  FIELD_RS (AUPREC, 70);    //ok
  FIELD_HANDLE (TEXTSTYLE, 2, 7);
  FIELD_RS (OSMODE, 70);
  FIELD_RS (ATTMODE, 70);
  DECODER {
    if (FIELD_VALUE (MENU)) // already created by add_Document
      FREE (FIELD_VALUE (MENU));
  }
  // cppcheck-suppress doubleFree
  FIELD_TFv (MENU, 15, 1); // optionally extended by MENUEXT below
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
#ifndef IS_JSON
  FIELD_TFF (MENUEXT, 46, 0);
#endif
  DECODER {
    if (_obj->MENUEXT[1]) {
      size_t len = strlen ((char*)&_obj->MENUEXT[1]) + 1;
      _obj->MENU = (char*)REALLOC (_obj->MENU, strlen (_obj->MENU) + len + 1);
      strncat (_obj->MENU, (char*)&_obj->MENUEXT[1], len);
      LOG_TRACE ("MENU => \"%s\"\n", _obj->MENU);
    }
  }
  FIELD_RD (ELEVATION, 40); //ok
  FIELD_RD (THICKNESS, 40); //ok
  FIELD_3RD (VIEWDIR, 10);
  // replaced by VIEWDIR with r11
  FIELD_3RD (VPOINTX, 0); //(1,0,0)
  FIELD_3RD (VPOINTY, 0); //(0,1,0)
  FIELD_3RD (VPOINTZ, 0); //(0,0,1)
  FIELD_3RD (VPOINTXALT, 0); //(1,0,0)
  FIELD_3RD (VPOINTYALT, 0); //(0,1,0)
  FIELD_3RD (VPOINTZALT, 0); //(0,0,1)
  FIELD_RS (flag_3d, 0);
  FIELD_CAST (BLIPMODE, RS, B, 70);
  if (dwg->header.numheader_vars <= 83) // PRE(R_2_21)
     return 0;
  FIELD_CAST (DIMZIN, RC, B, 70); //ok
  FIELD_RD (DIMRND, 40);
  FIELD_RD (DIMDLE, 40);
  FIELD_TFv (DIMBLK_T, 33, 1);
  FIELD_RS (circle_zoom_percent, 0);
  FIELD_RS (COORDS, 0);
#ifdef IS_JSON
  FIELD_CMC (CECOLOR, 62);
#else
  FIELD_RS (CECOLOR.index, 62);
#endif
  FIELD_HANDLE (CELTYPE, 2, 6); // ff for BYLAYER, fe for BYBLOCK
  FIELD_TIMERLL (TDCREATE, 40);
  FIELD_TIMERLL (TDUPDATE, 40);
  DECODER {
    if (!_obj->TDUCREATE.days && dat->version >= R_13) {
      long off = tm_offset() * 1000;
      _obj->TDUCREATE.days  = _obj->TDCREATE.days;
      // adjust for timezone offset
      _obj->TDUCREATE.ms    = _obj->TDUCREATE.ms - off;
      _obj->TDUCREATE.value = _obj->TDUCREATE.days + (_obj->TDUCREATE.ms * 1e-8);
      LOG_TRACE ("=> TDUCREATE: [" FORMAT_BL ", " FORMAT_BL "] %f [TIMEBLL 40]\n",
                   _obj->TDUCREATE.days, _obj->TDUCREATE.ms, _obj->TDUCREATE.value);
      _obj->TDUUPDATE.days  = _obj->TDUPDATE.days;
      _obj->TDUUPDATE.ms    = _obj->TDUPDATE.ms - off;
      _obj->TDUUPDATE.value = _obj->TDUPDATE.days + (_obj->TDUPDATE.ms * 1e-8);
      LOG_TRACE ("=> TDUUPDATE: [" FORMAT_BL ", " FORMAT_BL "] %f [TIMEBLL 40]\n",
                   _obj->TDUUPDATE.days, _obj->TDUUPDATE.ms, _obj->TDUUPDATE.value);
    }
  }
  FIELD_TIMERLL (TDINDWG, 40);
  FIELD_TIMERLL (TDUSRTIMER, 40);
  FIELD_CAST (USRTIMER, RS, B, 70);
  FIELD_CAST (FASTZOOM, RS, B, 70);
  FIELD_CAST (SKPOLY, RS, B, 70);
  FIELD_RS (unknown_mon, 0);
  FIELD_RS (unknown_day, 0);
  FIELD_RS (unknown_year, 0);
  FIELD_RS (unknown_hour, 0);
  FIELD_RS (unknown_min, 0);
  FIELD_RS (unknown_sec, 0);
  FIELD_RS (unknown_ms, 0);
  FIELD_RD (ANGBASE, 50);
  FIELD_CAST (ANGDIR, RS, B, 70);
  if (dwg->header.numheader_vars <= 101)
    return 0;
  FIELD_RS (PDMODE, 70);
  FIELD_RD (PDSIZE, 40);
  FIELD_RD (PLINEWID, 40);
  if (dwg->header.numheader_vars <= 104)
    return 0;
  FIELD_RSd (USERI1, 70);
  FIELD_RSd (USERI2, 70);
  FIELD_RSd (USERI3, 70);
  FIELD_RSd (USERI4, 70);
  FIELD_RSd (USERI5, 70);
  FIELD_RD (USERR1, 40);
  FIELD_RD (USERR2, 40);
  FIELD_RD (USERR3, 40);
  FIELD_RD (USERR4, 40);
  FIELD_RD (USERR5, 40);
  if (dwg->header.numheader_vars <= 114)
    return 0;
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
  PRER13_SECTION_HDR (UCS); /* (0x3ef-0x3f8) 10 byte */
  ENCODER {
    FIELD_VALUE (codepage) = dwg->header.codepage;
  }
  FIELD_RS (codepage, 0);
  DECODER {
    dwg->header.codepage = FIELD_VALUE (codepage);
  }
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
#ifdef IS_DECODER
  {
    _obj->HANDSEED = (BITCODE_H)CALLOC (1, sizeof (Dwg_Object_Ref));
    _obj->HANDSEED->handleref.code = 0;
    _obj->HANDSEED->handleref.size = 8;
    _obj->HANDSEED->handleref.value = bit_read_RLL_BE (dat);
    _obj->HANDSEED->absolute_ref = _obj->HANDSEED->handleref.value;
    LOG_TRACE ("HANDSEED: " FORMAT_H " [H 5]\n",
               ARGS_H (_obj->HANDSEED->handleref));
  }
#elif defined IS_ENCODER
  if (_obj->HANDSEED)
    {
      bit_write_RLL_BE (dat, _obj->HANDSEED->absolute_ref);
      LOG_TRACE ("HANDSEED: " FORMAT_H " [H 5]\n",
                 ARGS_H (_obj->HANDSEED->handleref));
    }
  else
    {
      unsigned long handseed = dwg_next_handle (dwg);
      bit_write_RLL_BE (dat, handseed);
      LOG_TRACE ("HANDSEED: (0.8.%lX) [H 5]\n", handseed);
    }
#else
  FIELD_HANDLE (HANDSEED, 0, 5)
#endif
  FIELD_RS (SURFU, 70); //ok
  FIELD_RS (SURFV, 70); //ok
  FIELD_RS (SURFTYPE, 70); //ok
  FIELD_RS (SURFTAB1, 70); //ok
  FIELD_RS (SURFTAB2, 70); //ok
  PRER13_SECTION_HDR (VPORT); /* (0x500-0x509) */
  FIELD_CAST (FLATLAND, RS, B, 70);
  FIELD_RS (SPLINETYPE, 70);
  FIELD_RS (UCSICON, 0);
  FIELD_HANDLE (UCSNAME, 2, 2);
  if (dwg->header.numheader_vars <= 158) // r10
    return 0;
  PRER13_SECTION_HDR (APPID); /* (0x512-0x51b) */
  FIELD_CAST (WORLDVIEW, RS, B, 70);
  if (dwg->header.numheader_vars <= 160) // r10
    return 0;
  FIELD_RS (unknown_51e, 0);
  FIELD_RS (unknown_520, 0);
  PRER13_SECTION_HDR (DIMSTYLE); /* (0x522-0x52b) */
  /* TODO Unknown 5 bytes. (first two bytes sometimes ff ff) */
  FIELD_RSd (unknown_52c, 0);
  FIELD_RS (unknown_52e, 0);
  FIELD_RC (unknown_530, 0);
  FIELD_RS (DIMCLRD_C, 70); //ok
  FIELD_RS (DIMCLRE_C, 70); //ok
  FIELD_RS (DIMCLRT_C, 70); //ok
  FIELD_RS (SHADEDGE, 70); //ok
  FIELD_RS (SHADEDIF, 70); //ok
  FIELD_RS (unknown_59, 0); // todo hex
  FIELD_RS (UNITMODE, 70); //ok, new with r11
  FIELD_RD (unit1_ratio, 0); // unit conversions. i.e. meter / inch
  FIELD_RD (unit2_ratio, 0);
  FIELD_RD (unit3_ratio, 0);
  FIELD_RD (unit4_ratio, 0);
  DECODER {
    if (FIELD_VALUE (unit1_name))
      FREE (FIELD_VALUE (unit1_name));
  }
  FIELD_TFv (unit1_name, 32, 1);
  FIELD_TFv (unit2_name, 32, 1);
  FIELD_TFv (unit3_name, 32, 1);
  FIELD_TFv (unit4_name, 32, 1);
  FIELD_RD (DIMTFAC, 40); //ok
  FIELD_3RD (PUCSORG, 10); //ok
  FIELD_3RD (PUCSXDIR, 11); //ok
  FIELD_3RD (PUCSYDIR, 12); //ok
  FIELD_HANDLE (PUCSNAME, 2, 2);
  FIELD_CAST (TILEMODE, RS, B, 70); //ok
  FIELD_CAST (PLIMCHECK, RS, B, 70); //ok
  FIELD_RSx (unknown_10, 70); //0
  FIELD_3RD (PEXTMIN, 10); //ok 637
  FIELD_3RD (PEXTMAX, 10); //
  FIELD_2RD (PLIMMIN, 10); //
  FIELD_2RD (PLIMMAX, 10); //
  FIELD_3RD (PINSBASE, 10);
  PRER13_SECTION_HDR (VX); /* (0x69f-0x6a8) */
  FIELD_RS (MAXACTVP, 70); //ok
  FIELD_RD (DIMGAP, 40);   //ok
  FIELD_RD (PELEVATION, 40); //ok
  if (dwg->header.numheader_vars <= 204)
    return 0;
  FIELD_CAST (VISRETAIN, RS, B, 70); // r11

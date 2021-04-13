/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2010, 2018-2024 Free Software Foundation, Inc.             */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * header_variables.spec: DWG header variables specification
 * written by Felipe Corrêa da Silva Sances
 * modified by Anderson Cardoso
 * modified by Reini Urban
 * modified by Michal Josef Špaček
 */

//TODO: (here unknwon_* or in dwg.spec)
//  GEOLATLONGFORMAT [B] default 0
//  GEOMARKERVISIBILITY [B] default 1
//  GEOMARKPOSITIONSIZE (?)
//  FRAME [0-3] default 3
//  IMAGEFRAME [0-2] default 1
//  PDFFRAME [0-2] default 1
//  ANNOALLVISIBLE [B 0] (per Layout, LAYOUT.layout_flags?)
//  ANNOTATIVEDWG [B 0]
//  CTAB [T 0] default: "Model"
//
// Computed:
//  CMLEADERSTYLE [H 0] (via NOD ACAD_MLEADERSTYLE)
//  CTABLESTYLE [H 0] (via NOD ACAD_TABLESTYLE)
//  CVPORT [BS 0] default: 2 (current viewport id), via VPORT *Active

#include "spec.h"

// size, bitsize_hi and bitsize read before

  SINCE (R_2013b)
    {
      FIELD_BLL (REQUIREDVERSIONS, 160);
    }

  SINCE (R_13b1)
    {
      IF_ENCODE_FROM_EARLIER {
        FIELD_VALUE (unit1_ratio) = 412148564080.0;
        VERSIONS (R_13b1, R_2004) {
          FIELD_VALUE (unit2_ratio) = 6.162483e-14;
          FIELD_VALUE (unit3_ratio) = 1.62263e+13;
          FIELD_VALUE (unit4_ratio) = 2.63294e+26;
        } else {
          FIELD_VALUE (unit2_ratio) = 1.0;
          FIELD_VALUE (unit3_ratio) = 1.0;
          FIELD_VALUE (unit4_ratio) = 1.0;
        }
      }
      FIELD_BD (unit1_ratio, 0); // unit conversions. i.e. meter / inch
      FIELD_BD (unit2_ratio, 0);
      FIELD_BD (unit3_ratio, 0);
      FIELD_BD (unit4_ratio, 0);
    }
  VERSIONS (R_13b1, R_2004) { // undocumented as such in the ODA spec
      IF_ENCODE_FROM_EARLIER
      {
        if (!(FIELD_VALUE (unit1_name)))
        {
          VERSIONS (R_13b1, R_2004) {
            FIELD_VALUE (unit1_name) = strdup ("meter");
            FIELD_VALUE (unit2_name) = strdup ("inch");
            FIELD_VALUE (unit3_name) = strdup ("inch");
            FIELD_VALUE (unit4_name) = strdup ("sq inch");
          } else {
            FIELD_VALUE (unit1_name) = strdup ("m");
          }
        }
      }
      FIELD_TV (unit1_name, 0);
      FIELD_TV (unit2_name, 0);
      FIELD_TV (unit3_name, 0);
      FIELD_TV (unit4_name, 0);
  }
  ON_FREE {
    FIELD_TV (unit1_name, 0);
  }
  SINCE (R_13b1)
    {
      IF_ENCODE_FROM_EARLIER_OR_DXF {
        FIELD_VALUE (unknown_8) = 24;
      }
      FIELD_BLx (unknown_8, 0);
      FIELD_BL (unknown_9, 0);
    }
  VERSIONS (R_13b1, R_14) // or maybe UNTIL (R_14)
    {
      FIELD_BS (unknown_10, 0);
    }
  VERSIONS (R_13b1, R_2000)
    {
      // => VX. VX.viewport <=> active_viewport in LAYOUT
      FIELD_HANDLE (VX_TABLE_RECORD, 5, 0); // current view
    }
  SINCE (R_13b1)
    {
      FIELD_B (DIMASO, 70);
      FIELD_B (DIMSHO, 70);
    }
  VERSIONS (R_13b1, R_14) {
    FIELD_B (DIMSAV, 70);
  }
  SINCE (R_13b1)
    {
      FIELD_B (PLINEGEN, 70);
      FIELD_B (ORTHOMODE, 70);
      FIELD_B (REGENMODE, 70);
      FIELD_B (FILLMODE, 70);
      FIELD_B (QTEXTMODE, 70);
      FIELD_B (PSLTSCALE, 70);
      FIELD_B (LIMCHECK, 70);
    }
  VERSIONS (R_13b1, R_14) {
    FIELD_B (BLIPMODE, 70);
  }
  SINCE (R_2004a) {
    FIELD_B (unknown_11, 0); //undocumented
  }

  FIELD_B (USRTIMER, 70);
  FIELD_B (SKPOLY, 70);
  FIELD_B (ANGDIR, 70);
  FIELD_B (SPLFRAME, 70);

  VERSIONS (R_13b1, R_14)
    {
      IF_ENCODE_FROM_EARLIER {
         FIELD_VALUE (ATTREQ) = 1;
         FIELD_VALUE (ATTDIA) = 1;
         FIELD_VALUE (HANDLING) = 1;
      }
      FIELD_B (ATTREQ, 70);
      FIELD_B (ATTDIA, 70);
    }

  FIELD_B (MIRRTEXT, 70);
  FIELD_B (WORLDVIEW, 70); // default: 1

  VERSIONS (R_13b1, R_14)
    {
      FIELD_B (WIREFRAME, 0); //Undocumented, not in DXF
    }

  FIELD_B (TILEMODE, 70);  // default: 1
  FIELD_B (PLIMCHECK, 70);
  FIELD_B (VISRETAIN, 70); // default: 1

  VERSIONS (R_13b1, R_14)
    {
      FIELD_B (DELOBJ, 70);
    }

  FIELD_B (DISPSILH, 70);
  FIELD_B (PELLIPSE, 70);

#if 0
  VERSION (R_13b1) {
    FIELD_BS (SAVEIMAGES, 70); // not in R13. maybe R13C3 only? also not in ODA
  }
#endif
  FIELD_BS (PROXYGRAPHICS, 70);
  UNTIL (R_14) {
    IF_ENCODE_FROM_EARLIER {
      if (!FIELD_VALUE (DRAGMODE))
        FIELD_VALUE (DRAGMODE) = 2;
    }
    FIELD_BS (DRAGMODE, 70);
  }
  SINCE (R_13b1) {
    IF_ENCODE_FROM_EARLIER {
      FIELD_VALUE (TREEDEPTH) = 3020;
    }
    FIELD_BSd (TREEDEPTH, 70);
  }
  FIELD_BS (LUNITS, 70);
  FIELD_BS (LUPREC, 70);
  FIELD_BS (AUNITS, 70);
  FIELD_BS (AUPREC, 70);
  UNTIL (R_14) {
    FIELD_BS (OSMODE, 70);
  }
  FIELD_BS (ATTMODE, 70);
  UNTIL (R_14) {
    FIELD_BS (COORDS, 70);
  }
  FIELD_BS (PDMODE, 70);
  UNTIL (R_14) {
    FIELD_BS (PICKSTYLE, 70);
  }
  SINCE (R_2004a) {
    FIELD_BL (unknown_12, 0);
    FIELD_BL (unknown_13, 0);
    FIELD_BL (unknown_14, 0);
  }

  IF_ENCODE_FROM_PRE_R13 {
    FIELD_VALUE (SHADEDGE) = 3;
    FIELD_VALUE (SHADEDIF) = 70;
    FIELD_VALUE (MAXACTVP) = 64;
  }
  FIELD_BSd (USERI1, 70);
  FIELD_BSd (USERI2, 70);
  FIELD_BSd (USERI3, 70);
  FIELD_BSd (USERI4, 70);
  FIELD_BSd (USERI5, 70);
  FIELD_BS (SPLINESEGS, 70);
  FIELD_BS (SURFU, 70);
  FIELD_BS (SURFV, 70);
  FIELD_BS (SURFTYPE, 70);
  FIELD_BS (SURFTAB1, 70);
  FIELD_BS (SURFTAB2, 70);
  FIELD_BS (SPLINETYPE, 70);
  FIELD_BS (SHADEDGE, 70); // default: 3
  FIELD_BS (SHADEDIF, 70); // default: 70. percent ambient/diffuse light 1-100
  FIELD_BS (UNITMODE, 70);
  FIELD_BS (MAXACTVP, 70); // default: 64
  FIELD_BS (ISOLINES, 70);
  FIELD_BS (CMLJUST, 70); // 0-2
  FIELD_BS (TEXTQLTY, 70);
  FIELD_BD (LTSCALE, 40);
  FIELD_BD (TEXTSIZE, 40);
  FIELD_BD (TRACEWID, 40);
  FIELD_BD (SKETCHINC, 40);
  FIELD_BD (FILLETRAD, 40);
  FIELD_BD (THICKNESS, 40);
  FIELD_BD (ANGBASE, 50);
  FIELD_BD (PDSIZE, 40);
  FIELD_BD (PLINEWID, 40);
  FIELD_BD (USERR1, 40);
  FIELD_BD (USERR2, 40);
  FIELD_BD (USERR3, 40);
  FIELD_BD (USERR4, 40);
  FIELD_BD (USERR5, 40);
  FIELD_BD (CHAMFERA, 40);
  FIELD_BD (CHAMFERB, 40);
  FIELD_BD (CHAMFERC, 40);
  FIELD_BD (CHAMFERD, 40);
  FIELD_BD (FACETRES, 40);
  FIELD_BD (CMLSCALE, 40);
  FIELD_BD (CELTSCALE, 40);
  PRE (R_2007a) {
    FIELD_TV (MENU, 1);
  }
  FIELD_TIMEBLL (TDUCREATE, 40);
  FIELD_TIMEBLL (TDUUPDATE, 40);
  DECODER {
    if (!_obj->TDCREATE.days) {
      struct tm tm;
      long off = tm_offset() * 1000;
      char _buf[60] = "";
      _obj->TDCREATE.days  = _obj->TDUCREATE.days;
      // adjust for timezone offset
      _obj->TDCREATE.ms    = _obj->TDUCREATE.ms + off;
      _obj->TDCREATE.value = _obj->TDCREATE.days + (_obj->TDCREATE.ms * 1e-8);
      strftime (_buf, 60, STRFTIME_DATE, cvt_TIMEBLL (&tm, _obj->TDCREATE));
      LOG_TRACE ("=> TDCREATE: [" FORMAT_BL ", " FORMAT_BL "] %s [TIMEBLL 40]\n",
                   _obj->TDCREATE.days, _obj->TDCREATE.ms, _buf);
      _obj->TDUPDATE.days  = _obj->TDUUPDATE.days;
      _obj->TDUPDATE.ms    = _obj->TDUUPDATE.ms + off;
      _obj->TDUPDATE.value = _obj->TDUPDATE.days + (_obj->TDUPDATE.ms * 1e-8);
      strftime (_buf, 60, STRFTIME_DATE, cvt_TIMEBLL (&tm, _obj->TDUPDATE));
      LOG_TRACE ("=> TDUPDATE: [" FORMAT_BL ", " FORMAT_BL "] %s [TIMEBLL 40]\n",
                   _obj->TDUPDATE.days, _obj->TDUPDATE.ms, _buf);
    }
  }
  SINCE (R_2004a) {
    FIELD_BL (unknown_15, 0);
    FIELD_BL (unknown_16, 0);
    FIELD_BL (unknown_17, 0);
  }
  FIELD_TIMEBLL (TDINDWG, 40);
  FIELD_TIMEBLL (TDUSRTIMER, 40);
  FIELD_CMC (CECOLOR, 62);
  FIELD_DATAHANDLE (HANDSEED, ANYCODE, 0);
  FIELD_HANDLE (CLAYER, 5, 8);
  FIELD_HANDLE (TEXTSTYLE, 5, 7);
  FIELD_HANDLE (CELTYPE, 5, 6);

  IF_FREE_OR_SINCE (R_2007a)
    {
      FIELD_HANDLE (CMATERIAL, 5, 0);
    }

  FIELD_HANDLE (DIMSTYLE, 5, 2);
  FIELD_HANDLE (CMLSTYLE, 5, 2);

  SINCE (R_2000b)
    {
      FIELD_BD (PSVPSCALE, 40);
    }

  SINCE (R_13b1)
    {
      FIELD_3BD (PINSBASE, 10);
      FIELD_3BD (PEXTMIN, 10);
      FIELD_3BD (PEXTMAX, 10);
      FIELD_2RD (PLIMMIN, 10);
      FIELD_2RD (PLIMMAX, 10);
      FIELD_BD (PELEVATION, 40);
      FIELD_3BD (PUCSORG, 10);
      FIELD_3BD (PUCSXDIR, 10);
      FIELD_3BD (PUCSYDIR, 10);
      FIELD_HANDLE (PUCSNAME, 5, 2);
    }

  IF_FREE_OR_SINCE (R_2000b)
    {
      FIELD_HANDLE (PUCSORTHOREF, 5, 2);
      FIELD_BS (PUCSORTHOVIEW, 70);
      FIELD_HANDLE (PUCSBASE, 5, 2);
      FIELD_3BD (PUCSORGTOP, 10);
      FIELD_3BD (PUCSORGBOTTOM, 10);
      FIELD_3BD (PUCSORGLEFT, 10);
      FIELD_3BD (PUCSORGRIGHT, 10);
      FIELD_3BD (PUCSORGFRONT, 10);
      FIELD_3BD (PUCSORGBACK, 10);
    }

  FIELD_3BD (INSBASE, 10);
  FIELD_3BD (EXTMIN, 10);
  FIELD_3BD (EXTMAX, 10);
  FIELD_2RD (LIMMIN, 10);
  FIELD_2RD (LIMMAX, 10);
  FIELD_BD (ELEVATION, 40);
  FIELD_3BD (UCSORG, 10);
  FIELD_3BD (UCSXDIR, 10);
  FIELD_3BD (UCSYDIR, 10);
  FIELD_HANDLE (UCSNAME, 5, 2);

  IF_FREE_OR_SINCE (R_2000b)
    {
      FIELD_HANDLE (UCSORTHOREF, 5, 2);
      FIELD_BS (UCSORTHOVIEW, 70);
      FIELD_HANDLE (UCSBASE, 5, 2);
      FIELD_3BD (UCSORGTOP, 10);
      FIELD_3BD (UCSORGBOTTOM, 10);
      FIELD_3BD (UCSORGLEFT, 10);
      FIELD_3BD (UCSORGRIGHT, 10);
      FIELD_3BD (UCSORGFRONT, 10);
      FIELD_3BD (UCSORGBACK, 10);
      PRE (R_2007a) {
        FIELD_TV (DIMPOST, 1);
        FIELD_TV (DIMAPOST, 1);
      }
    }

  VERSIONS (R_13b1, R_14)
    {
      FIELD_B (DIMTOL, 70);
      FIELD_B (DIMLIM, 70);
      FIELD_B (DIMTIH, 70);
      FIELD_B (DIMTOH, 70);
      FIELD_B (DIMSE1, 70);
      FIELD_B (DIMSE2, 70);
      FIELD_B (DIMALT, 70);
      FIELD_B (DIMTOFL, 70);
      FIELD_B (DIMSAH, 70);
      FIELD_B (DIMTIX, 70);
      FIELD_B (DIMSOXD, 70);
      FIELD_CAST (DIMALTD, RC, BS, 70);
      FIELD_CAST (DIMZIN, RC, BS, 70);
      FIELD_B (DIMSD1, 70);
      FIELD_B (DIMSD2, 70);
      FIELD_CAST (DIMTOLJ, RC, BS, 70);
      FIELD_CAST (DIMJUST, RC, BS, 70);
      FIELD_CAST (DIMFIT, RC, BS, 70);
      FIELD_B (DIMUPT, 70);
      FIELD_CAST (DIMTZIN, RC, BS, 70);
      FIELD_CAST (DIMALTZ, RC, BS, 70);
      FIELD_CAST (DIMALTTZ, RC, BS, 70);
      FIELD_CAST (DIMTAD, RC, BS, 70);
      FIELD_BS (DIMUNIT, 70);
      FIELD_BS (DIMAUNIT, 70);
      FIELD_BS (DIMDEC, 70);
      FIELD_BS (DIMTDEC, 70);
      FIELD_BS (DIMALTU, 70);
      FIELD_BS (DIMALTTD, 70);
      FIELD_HANDLE (DIMTXSTY, 5, 7);
    }

  FIELD_BD (DIMSCALE, 40);
  FIELD_BD (DIMASZ, 40);
  FIELD_BD (DIMEXO, 40);
  FIELD_BD (DIMDLI, 40);
  FIELD_BD (DIMEXE, 40);
  FIELD_BD (DIMRND, 40);
  FIELD_BD (DIMDLE, 40);
  FIELD_BD (DIMTP, 40);
  FIELD_BD (DIMTM, 40);

  SINCE (R_2007a)
    {
      FIELD_BD (DIMFXL, 40);
      FIELD_BD (DIMJOGANG, 40);
      FIELD_BS (DIMTFILL, 70);
      FIELD_CMC (DIMTFILLCLR, 70);
    }

  SINCE (R_2000b)
    {
      FIELD_B (DIMTOL, 70);
      FIELD_B (DIMLIM, 70);
      FIELD_B (DIMTIH, 70);
      FIELD_B (DIMTOH, 70);
      FIELD_B (DIMSE1, 70);
      FIELD_B (DIMSE2, 70);
      FIELD_BS (DIMTAD, 70);
      FIELD_BS (DIMZIN, 70);
      FIELD_BS (DIMAZIN, 70);
    }

  SINCE (R_2007a)
    {
      FIELD_BS (DIMARCSYM, 70);
    }

  FIELD_BD (DIMTXT, 40);
  FIELD_BD (DIMCEN, 40);
  FIELD_BD (DIMTSZ, 40);
  FIELD_BD (DIMALTF, 40);
  FIELD_BD (DIMLFAC, 40);
  FIELD_BD (DIMTVP, 40);
  FIELD_BD (DIMTFAC, 40);
  FIELD_BD (DIMGAP, 40);

  IF_FREE_OR_VERSIONS (R_13b1, R_14)
    {
      FIELD_TV (DIMPOST, 1);
      FIELD_TV (DIMAPOST, 1);
      FIELD_TV (DIMBLK_T, 1);
      FIELD_TV (DIMBLK1_T, 1);
      FIELD_TV (DIMBLK2_T, 1);
    }

  SINCE (R_2000b)
    {
      FIELD_BD (DIMALTRND, 40);
      FIELD_B (DIMALT, 70);
      FIELD_BS (DIMALTD, 70);
      FIELD_B (DIMTOFL, 70);
      FIELD_B (DIMSAH, 70);
      FIELD_B (DIMTIX, 70);
      FIELD_B (DIMSOXD, 70);
    }

  FIELD_CMC (DIMCLRD, 70);
  FIELD_CMC (DIMCLRE, 70);
  FIELD_CMC (DIMCLRT, 70);

  SINCE (R_2000b)
    {
      FIELD_BS (DIMADEC, 70);
      FIELD_BS (DIMDEC, 70);
      FIELD_BS (DIMTDEC, 70);
      FIELD_BS (DIMALTU, 70);
      FIELD_BS (DIMALTTD, 70);
      FIELD_BS (DIMAUNIT, 70);
      FIELD_BS (DIMFRAC, 70);
      FIELD_BS (DIMLUNIT, 70);
      FIELD_BS (DIMDSEP, 70);
      FIELD_BS (DIMTMOVE, 70);
      FIELD_BS (DIMJUST, 70);
      FIELD_B (DIMSD1, 70);
      FIELD_B (DIMSD2, 70);
      FIELD_BS (DIMTOLJ, 70);
      FIELD_BS (DIMTZIN, 70);
      FIELD_BS (DIMALTZ, 70);
      FIELD_BS (DIMALTTZ, 70);
      FIELD_B (DIMUPT, 70);
      FIELD_BS (DIMATFIT, 70);
    }

  SINCE (R_2007a)
    {
      FIELD_B (DIMFXLON, 70);
    }

  SINCE (R_2010b)
    {
      FIELD_B (DIMTXTDIRECTION, 70);
      FIELD_BD (DIMALTMZF, 40);
      FIELD_BD (DIMMZF, 40);
    }

  SINCE (R_2000b)
    {
      FIELD_HANDLE (DIMTXSTY, 5, 7);
      FIELD_HANDLE (DIMLDRBLK, 5, 1);
      FIELD_HANDLE (DIMBLK, 5, 1);
      FIELD_HANDLE (DIMBLK1, 5, 1);
      FIELD_HANDLE (DIMBLK2, 5, 1);
    }

  SINCE (R_2007a)
    {
      FIELD_HANDLE (DIMLTYPE, 5, 6);
      FIELD_HANDLE (DIMLTEX1, 5, 6);
      FIELD_HANDLE (DIMLTEX2, 5, 6);
    }

  SINCE (R_2000b)
    {
      FIELD_BSd (DIMLWD, 70);
      FIELD_BSd (DIMLWE, 70);
    }

  FIELD_HANDLE (BLOCK_CONTROL_OBJECT, 3, 0);
  FIELD_HANDLE (LAYER_CONTROL_OBJECT, 3, 0);
  FIELD_HANDLE (STYLE_CONTROL_OBJECT, 3, 0);
  FIELD_HANDLE (LTYPE_CONTROL_OBJECT, 3, 0);
  FIELD_HANDLE (VIEW_CONTROL_OBJECT, 3, 0);
  FIELD_HANDLE (UCS_CONTROL_OBJECT, 3, 0);
  FIELD_HANDLE (VPORT_CONTROL_OBJECT, 3, 0);
  FIELD_HANDLE (APPID_CONTROL_OBJECT, 5, 0);
  FIELD_HANDLE (DIMSTYLE_CONTROL_OBJECT, 3, 0);
  VERSIONS (R_13b1, R_2000) {
    FIELD_HANDLE (VX_CONTROL_OBJECT, 3, 0);
  }

  FIELD_HANDLE (DICTIONARY_ACAD_GROUP, 5, 0);
  FIELD_HANDLE (DICTIONARY_ACAD_MLINESTYLE, 5, 0);
  FIELD_HANDLE (DICTIONARY_NAMED_OBJECT, 5, 0);

  SINCE (R_2000b)
    {
      IF_ENCODE_FROM_EARLIER_OR_DXF {
         FIELD_VALUE (TSTACKALIGN) = 1;
         FIELD_VALUE (TSTACKSIZE) = 70;
      }
      FIELD_BS (TSTACKALIGN, 70);
      FIELD_BS (TSTACKSIZE, 70);
      PRE (R_2007a) {
        FIELD_TV (HYPERLINKBASE, 1);
        FIELD_TV (STYLESHEET, 1);
      }
      FIELD_HANDLE (DICTIONARY_LAYOUT, 5, 0);
      FIELD_HANDLE (DICTIONARY_PLOTSETTINGS, 5, 0);
      FIELD_HANDLE (DICTIONARY_PLOTSTYLENAME, 5, 0); // should be CPLOTSTYLE
    }

  SINCE (R_2004a)
    {
      FIELD_HANDLE (DICTIONARY_MATERIAL, 5, 0);
      FIELD_HANDLE (DICTIONARY_COLOR, 5, 0);
    }

  SINCE (R_2007a)
    {
      FIELD_HANDLE (DICTIONARY_VISUALSTYLE, 5, 0);
    }

  SINCE (R_2013b)
    {
      FIELD_HANDLE (unknown_20, 5, 0); //  DICTIONARY_LIGHTLIST? since 2010
    }
  SINCE (R_2000b)
    {
      ENCODER {
        // unneeded here. done in in_dxf.c:1189
        FIELD_VALUE (FLAGS) |= dxf_revcvt_lweight (FIELD_VALUE (CELWEIGHT));
        if (FIELD_VALUE (ENDCAPS)) FIELD_VALUE (FLAGS) |= 0x60;
        // ...
      }
      FIELD_BLx (FLAGS, 70);
      DECODER {
          FIELD_VALUE (CELWEIGHT) = dxf_cvt_lweight (FIELD_VALUE (FLAGS) & 0x1f);
          FIELD_G_TRACE (CELWEIGHT, BSd, 370) // default: -1 ByLayer
          FIELD_VALUE (ENDCAPS)   = FIELD_VALUE (FLAGS) & 0x60 ? 1 : 0;
          FIELD_G_TRACE (ENDCAPS, RC, 280)
          FIELD_VALUE (JOINSTYLE) = FIELD_VALUE (FLAGS) & 0x180 ? 1 : 0;
          FIELD_G_TRACE (JOINSTYLE, RC, 280)
          FIELD_VALUE (LWDISPLAY) = FIELD_VALUE (FLAGS) & 0x200 ? 0 : 1;
          FIELD_G_TRACE (LWDISPLAY, B, 290)
          FIELD_VALUE (XEDIT)     = FIELD_VALUE (FLAGS) & 0x400 ? 0 : 1;
          FIELD_G_TRACE (XEDIT, B, 290)
          FIELD_VALUE (EXTNAMES)  = FIELD_VALUE (FLAGS) & 0x800 ? 1 : 0;
          FIELD_G_TRACE (EXTNAMES, B, 290)
          FIELD_VALUE (PSTYLEMODE) = FIELD_VALUE (FLAGS) & 0x2000 ? 1 : 0;
          FIELD_G_TRACE (PSTYLEMODE, B, 290)
          FIELD_VALUE (OLESTARTUP) = FIELD_VALUE (FLAGS) & 0x4000 ? 1 : 0;
          FIELD_G_TRACE (OLESTARTUP, B, 290)
      }
      FIELD_BS (INSUNITS, 70); // 0-20. default: 1 with imperial, 4 with metric
      FIELD_BS (CEPSNTYPE, 70);
      if (FIELD_VALUE (CEPSNTYPE) == 3)
        {
          FIELD_HANDLE (CPSNID, 5, 0);
        }
      PRE (R_2007a) {
        FIELD_TV (FINGERPRINTGUID, 2);
        FIELD_TV (VERSIONGUID, 2);
      }
    }

  SINCE (R_2004a)
    {
      FIELD_RC (SORTENTS, 280);   //bitmask, see docs
      FIELD_RC (INDEXCTL, 280);
      FIELD_RC (HIDETEXT, 280);
      FIELD_RC (XCLIPFRAME, 290); //2010+: 280
      FIELD_RC (DIMASSOC, 280);
      FIELD_RC (HALOGAP, 280);
      FIELD_BS (OBSCOLOR, 70);
      FIELD_BS (INTERSECTIONCOLOR, 280);
      FIELD_RC (OBSLTYPE, 280);
      FIELD_RC (INTERSECTIONDISPLAY, 290);
      PRE (R_2007a) {
        FIELD_TV (PROJECTNAME, 1);
      }
    }

  FIELD_HANDLE (BLOCK_RECORD_PSPACE, 5, 0);
  FIELD_HANDLE (BLOCK_RECORD_MSPACE, 5, 0);
  FIELD_HANDLE (LTYPE_BYLAYER, 5, 0);
  FIELD_HANDLE (LTYPE_BYBLOCK, 5, 0);
  FIELD_HANDLE (LTYPE_CONTINUOUS, 5, 0);

  SINCE (R_2007a)
    {
      IF_ENCODE_FROM_EARLIER {
         FIELD_VALUE (STEPSPERSEC) = 2.0;
         FIELD_VALUE (STEPSIZE)   = 50.0;
         FIELD_VALUE (LENSLENGTH) = 50.0;
         FIELD_VALUE (_3DDWFPREC) = 2.0;
         FIELD_VALUE (PSOLWIDTH)  = 5.0;
         FIELD_VALUE (PSOLHEIGHT) = 80.0;
         FIELD_VALUE (LOFTANG1) = M_PI_2;
         FIELD_VALUE (LOFTANG2) = M_PI_2;
         FIELD_VALUE (LOFTPARAM) = 7;
         FIELD_VALUE (LOFTNORMALS) = 1;
         FIELD_VALUE (LATITUDE) = 1.0;
         FIELD_VALUE (LONGITUDE) = 1.0;
         FIELD_VALUE (TIMEZONE) = -8000;
         FIELD_VALUE (LIGHTGLYPHDISPLAY) = 1;
         FIELD_VALUE (TILEMODELIGHTSYNCH) = 1;
         FIELD_VALUE (SOLIDHIST) = 1;
         FIELD_VALUE (SHOWHIST) = 1;
         FIELD_VALUE (DWFFRAME) = 2;
         FIELD_VALUE (REALWORLDSCALE) = 1;
      }
      FIELD_B (CAMERADISPLAY, 290);
      FIELD_BL (unknown_21, 0);
      FIELD_BL (unknown_22, 0);
      FIELD_BD (unknown_23, 0);
      FIELD_BD (STEPSPERSEC, 40);
      FIELD_BD (STEPSIZE, 40);
      FIELD_BD (_3DDWFPREC, 40);
      FIELD_BD (LENSLENGTH, 40);
      FIELD_BD (CAMERAHEIGHT, 40);
      FIELD_RC (SOLIDHIST, 280);
      FIELD_RC (SHOWHIST, 280);
      FIELD_BD (PSOLWIDTH, 40);
      FIELD_BD (PSOLHEIGHT, 40);
      FIELD_BD (LOFTANG1, 40);
      FIELD_BD (LOFTANG2, 40);
      FIELD_BD (LOFTMAG1, 40);
      FIELD_BD (LOFTMAG2, 40);
      FIELD_BS (LOFTPARAM, 70);
      FIELD_RC (LOFTNORMALS, 280);
      FIELD_BD (LATITUDE, 40);
      FIELD_BD (LONGITUDE, 40);
      FIELD_BD (NORTHDIRECTION, 40);
      FIELD_BLd (TIMEZONE, 70);
      FIELD_RC (LIGHTGLYPHDISPLAY, 280);
      FIELD_RC (TILEMODELIGHTSYNCH, 280);
      FIELD_RC (DWFFRAME, 280);
      FIELD_RC (DGNFRAME, 280);
      FIELD_B (REALWORLDSCALE, 290);
      FIELD_CMC (INTERFERECOLOR, 62);
      FIELD_HANDLE (INTERFEREOBJVS, 5, 345); // VISUALSTYLE
      FIELD_HANDLE (INTERFEREVPVS, 5, 346); // VISUALSTYLE
      FIELD_HANDLE (DRAGVS, 5, 349); // VISUALSTYLE
      FIELD_RC (CSHADOW, 280); // [0-3]
      FIELD_BD (SHADOWPLANELOCATION, 40); // z height
    }

  SINCE (R_14)
    {
      FIELD_BS (unknown_54, 0); /* (type 5/6 only) these do not seem to be required */
      FIELD_BS (unknown_55, 0);
      FIELD_BS (unknown_56, 0);
      FIELD_BS (unknown_57, 0);
    }

  IF_FREE_OR_SINCE (R_2007a) {
    // TODO split str_dat stream and get rid of this block
    SECTION_STRING_STREAM
    FIELD_T (unit1_name, 0);
    FIELD_T (unit2_name, 0);
    FIELD_T (unit3_name, 0);
    FIELD_T (unit4_name, 0);
    FIELD_T (MENU, 1);
    FIELD_T (DIMPOST, 1);
    FIELD_T (DIMAPOST, 1);
    SINCE (R_2010b) {
      FIELD_T (DIMALTMZS, 1);
      FIELD_T (DIMMZS, 1);
    }
    FIELD_T (HYPERLINKBASE, 1); // see SummaryInfo
    FIELD_T (STYLESHEET, 1);
    FIELD_T (FINGERPRINTGUID, 1);
    FIELD_T (VERSIONGUID, 1);
    FIELD_T (PROJECTNAME, 1);
    END_STRING_STREAM
  }

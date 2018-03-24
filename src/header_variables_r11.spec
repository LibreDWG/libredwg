/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018 Free Software Foundation, Inc.                        */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * header_variables.spec: DWG pre-R13 header variables specification
 * written by Reini Urban
 */

#include "spec.h"

  FIELD_RD(unknown_0);     // -12.0
  FIELD_RS(HANDSEED_R11);  // 65520

  FIELD_2RD (INSBASE);
  FIELD_3RD (EXTMIN);
  FIELD_3RD (EXTMAX);
  FIELD_2RD (LIMMIN);
  FIELD_2RD (LIMMAX);
  FIELD_2RD (VIEWCTR);
  FIELD_RD (ELEVATION);
  FIELD_RD (VIEWSIZE);
  FIELD_RS (unknown_54);
  FIELD_2RD (SNAPUNIT);

  FIELD_3RD (UCSORG);
  FIELD_3RD (UCSXDIR);
  FIELD_3RD (UCSYDIR);

  /*
  FIELD_HANDLE (UCSNAME, ANYCODE);
  VERSIONS(R_13, R_14)
    {
      FIELD_B (DIMTOL);
      FIELD_B (DIMLIM);
      FIELD_B (DIMTIH);
      FIELD_B (DIMTOH);
      FIELD_B (DIMSE1);
      FIELD_B (DIMSE2);
      FIELD_B (DIMALT);
      FIELD_B (DIMTOFL);
      FIELD_B (DIMSAH);
      FIELD_B (DIMTIX);
      FIELD_B (DIMSOXD);
      FIELD_CAST (DIMALTD, RC, BS);
      FIELD_CAST (DIMZIN, RC, BS);
      FIELD_B (DIMSD1);
      FIELD_B (DIMSD2);
      FIELD_CAST (DIMTOLJ, RC, BS);
      FIELD_CAST (DIMJUST, RC, BS);
      FIELD_CAST (DIMFIT, RC, BS);
      FIELD_B (DIMUPT);
      FIELD_CAST (DIMTZIN, RC, BS);
      FIELD_CAST (DIMMALTZ, RC, BS);
      FIELD_CAST (DIMMALTTZ, RC, BS);
      FIELD_CAST (DIMTAD, RC, BS);
      FIELD_BS (DIMUNIT);
      FIELD_BS (DIMAUNIT);
      FIELD_BS (DIMDEC);
      FIELD_BS (DIMTDEC);
      FIELD_BS (DIMALTU);
      FIELD_BS (DIMALTTD);
      FIELD_HANDLE (DIMTXSTY, ANYCODE);
    }

  FIELD_BD (DIMSCALE);
  FIELD_BD (DIMASZ);
  FIELD_BD (DIMEXO);
  FIELD_BD (DIMDLI);
  FIELD_BD (DIMEXE);
  FIELD_BD (DIMRND);
  FIELD_BD (DIMDLE);
  FIELD_BD (DIMMTP);
  FIELD_BD (DIMMTM);

  FIELD_BD (DIMTXT);
  FIELD_BD (DIMCEN);
  FIELD_BD (DIMTSZ);
  FIELD_BD (DIMALTF);
  FIELD_BD (DIMLFAC);
  FIELD_BD (DIMTVP);
  FIELD_BD (DIMTFAC);
  FIELD_BD (DIMGAP);

  VERSIONS(R_13, R_14)
    {
      FIELD_T (DIMPOST_T);
      FIELD_T (DIMAPOST_T);
      FIELD_T (DIMBLK_T);
      FIELD_T (DIMBLK1_T);
      FIELD_T (DIMBLK2_T);
    }

  FIELD_CMC (DIMCLRD);
  FIELD_CMC (DIMCLRE);
  FIELD_CMC (DIMCLRT);


  FIELD_HANDLE (BLOCK_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (LAYER_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (STYLE_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (LINETYPE_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (VIEW_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (UCS_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (VPORT_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (APPID_CONTROL_OBJECT, ANYCODE);
  FIELD_HANDLE (DIMSTYLE_CONTROL_OBJECT, ANYCODE);

  VERSIONS(R_13, R_2000)
    {
      FIELD_HANDLE (VIEWPORT_ENTITY_HEADER_CONTROL_OBJECT, ANYCODE);
    }

  FIELD_HANDLE (DICTIONARY_ACAD_GROUP, ANYCODE);
  FIELD_HANDLE (DICTIONARY_ACAD_MLINESTYLE, ANYCODE);
  FIELD_HANDLE (DICTIONARY_NAMED_OBJECTS, ANYCODE);

  FIELD_HANDLE (BLOCK_RECORD_PAPER_SPACE, ANYCODE);
  FIELD_HANDLE (BLOCK_RECORD_MODEL_SPACE, ANYCODE);
  FIELD_HANDLE (LTYPE_BYLAYER, ANYCODE);
  FIELD_HANDLE (LTYPE_BYBLOCK, ANYCODE);
  FIELD_HANDLE (LTYPE_CONTINUOUS, ANYCODE);

  */
  dat->byte = 0x23a - 2;
  FIELD_RS (CRC);


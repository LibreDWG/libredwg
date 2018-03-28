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
 * header_variables_r11.spec: DWG pre-R13 header variables specification
 * written by Reini Urban
 */

#include "spec.h"

  FIELD_RD(unknown_0);  // -12.0
  FIELD_RS(unknown_54); // 65520 / no: 0xB8BC

  FIELD_2RD (INSBASE);
  FIELD_3RD (EXTMIN);
  FIELD_3RD (EXTMAX);
  FIELD_2RD (LIMMIN);
  FIELD_2RD (LIMMAX);
  FIELD_2RD (VIEWCTR);
  FIELD_RD (ELEVATION);
  FIELD_RD (VIEWSIZE);
  FIELD_RS (SNAPMODE);
  FIELD_2RD (SNAPUNIT);
  FIELD_2RD (SNAPBASE);
  FIELD_RD (SNAPANG);
  FIELD_RS (SNAPSTYL);
  FIELD_RS (SNAPISOPAIR);
  FIELD_RS (GRIDMODE);
  FIELD_2RD (GRIDUNIT);
  FIELD_RS (ORTHOMODE);
  FIELD_RS (REGENMODE);
  FIELD_RS (FILLMODE);
  FIELD_RS (MIRRTEXT);
  FIELD_RS (DRAGMODE); // 2
  FIELD_RD (LTSCALE);  // 16.0
  FIELD_RS (OSMODE);
  FIELD_RS (QTEXTMODE);
  FIELD_RS (ATTMODE);
  FIELD_RD (TEXTSIZE);
  FIELD_RD (TRACEWID);

  FIELD_3RD (UCSORG);
  FIELD_3RD (UCSXDIR);
  FIELD_3RD (UCSYDIR);

  /*
  FIELD_HANDLE (UCSNAME, ANYCODE);

  FIELD_RD (DIMSCALE);
  FIELD_RD (DIMASZ);
  FIELD_RD (DIMEXO);
  FIELD_RD (DIMDLI);
  FIELD_RD (DIMEXE);
  FIELD_RD (DIMRND);
  FIELD_RD (DIMDLE);
  FIELD_RD (DIMMTP);
  FIELD_RD (DIMMTM);

  FIELD_RD (DIMTXT);
  FIELD_RD (DIMCEN);
  FIELD_RD (DIMTSZ);
  FIELD_RD (DIMALTF);
  FIELD_RD (DIMLFAC);
  FIELD_RD (DIMTVP);
  FIELD_RD (DIMTFAC);
  FIELD_RD (DIMGAP);

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


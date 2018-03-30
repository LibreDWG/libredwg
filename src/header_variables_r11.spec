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

  FIELD_RD(unknown_0, 0);  // -12.0
  FIELD_RS(unknown_54, 0); // 65520 / no: 0xB8BC

  FIELD_2RD (INSBASE, 10);
  FIELD_3RD (EXTMIN, 10);
  FIELD_3RD (EXTMAX, 10);
  FIELD_2RD (LIMMIN, 10);
  FIELD_2RD (LIMMAX, 10);
  FIELD_2RD (VIEWCTR, 12);
  FIELD_RD (ELEVATION, 40);
  FIELD_RD (VIEWSIZE, 40);
  FIELD_RS (SNAPMODE, 75);
  FIELD_2RD (SNAPUNIT, 14);
  FIELD_2RD (SNAPBASE, 13);
  FIELD_RD (SNAPANG, 50);
  FIELD_RS (SNAPSTYL, 77);
  FIELD_RS (SNAPISOPAIR, 78);
  FIELD_RS (GRIDMODE, 76);
  FIELD_2RD (GRIDUNIT, 15);
  FIELD_RS (ORTHOMODE, 70);
  FIELD_RS (REGENMODE, 70);
  FIELD_RS (FILLMODE, 70);
  FIELD_RS (MIRRTEXT, 70);
  FIELD_RS (DRAGMODE, 70); // 2
  FIELD_RD (LTSCALE, 40);  // 16.0
  FIELD_RS (OSMODE, 70);
  FIELD_RS (QTEXTMODE, 70);
  FIELD_RS (ATTMODE, 70);
  FIELD_RD (TEXTSIZE, 40);
  FIELD_RD (TRACEWID, 40);

  FIELD_3RD (UCSORG, 10);
  FIELD_3RD (UCSXDIR, 11);
  FIELD_3RD (UCSYDIR, 12);

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

  FIELD_HANDLE (BLOCK_RECORD_PSPACE, ANYCODE);
  FIELD_HANDLE (BLOCK_RECORD_MSPACE, ANYCODE);
  FIELD_HANDLE (LTYPE_BYLAYER, ANYCODE);
  FIELD_HANDLE (LTYPE_BYBLOCK, ANYCODE);
  FIELD_HANDLE (LTYPE_CONTINUOUS, ANYCODE);

  */
  dat->byte = 0x23a - 2;
  FIELD_RS (CRC, 0);


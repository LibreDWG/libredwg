/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020,2023 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * appinfo.spec: AcDb:AppInfo section specification
 * written by Reini Urban
 */

  #include "spec.h"

  if (dwg->header.version < R_2007 && _obj->class_version < 3)
    {
      DECODER {
        FIELD_VALUE (class_version) = 2;
      }
      FIELD_T16 (appinfo_name, 0);    // AppInfoDataList
#ifndef IS_JSON
      FIELD_RL (num_strings, 0);      // 3
#endif
      FIELD_T16 (comment, 0);
      FIELD_T16 (product_info, 0);
      FIELD_T16 (version, 0);
    }
  else
    {
      FIELD_RL (class_version, 0);     // 3
      FIELD_T16 (appinfo_name, 0);     // AppInfoDataList
#ifndef IS_JSON
      FIELD_RL (num_strings, 0);       // 3
#endif
      FIELD_TFFx (version_checksum, 16, 0);
      FIELD_T16 (version, 0); // "Teigha(R) 4.3.2.0", AutoCAD: "19.0.55.0.0"
      FIELD_TFFx (comment_checksum, 16, 0);
      // "Autodesk DWG.  This file is a Trusted DWG last saved by an Autodesk application or Autodesk licensed application.",
      // "This file was last saved by an Open Design Alliance (ODA) application or an ODA licensed application."
      FIELD_T16 (comment, 0);
      DECODER {
        // some DWG's miss that
        if (dat->byte + 16 < dat->size) {
          FIELD_TFFx (product_checksum, 16, 0);
          FIELD_T16 (product_info, 0); // XML ProductInformation with:
          /*
  name: "AutoCAD"/"AutoCAD LT", build_version: "A.<num>...", "F.<num>...", "M.<num>..",
        registry_version, install_id_string: "ACAD-<num>:<num>", "ACADLT-<num>:<num>", registry_localeID
  name: "Teigha(R)", CompanyName: ""Open Design Alliance", registry_version, install_id_string: "ODA",
        registry_localeID
          */
        }
      }
      else {
        FIELD_TFFx (product_checksum, 16, 0);
        FIELD_T16 (product_info, 0);
      }
    }

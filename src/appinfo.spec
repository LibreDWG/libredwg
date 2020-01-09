/* -*- c -*- */
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

/*
 * appinfo.spec: AcDb:AppInfo section specification
 * written by Reini Urban
 */

  #include "spec.h"

  FIELD_RL (class_version, 0);      // 2-3
  if (dwg->header.version < R_2007 && _obj->class_version < 3)
    {
      FIELD_TU16 (appinfo_name, 0);  // AppInfoDataList
      FIELD_RL (unknown_rl, 0);      // 3
      FIELD_TU16 (comment, 0);
      FIELD_TU16 (product_info, 0);
      FIELD_TU16 (version, 0);
    }
  else
    {
      FIELD_TU16 (appinfo_name, 0);    // AppInfoDataList
      FIELD_RL (unknown_rl, 0);        // 3
      FIELD_TFFx (version_checksum, 16, 0);
      FIELD_TU16 (version, 0);
      FIELD_TFFx (comment_checksum, 16, 0);
      FIELD_TU16 (comment, 0);
      FIELD_TFFx (product_checksum, 16, 0);
      FIELD_TU16 (product_info, 0);
    }

/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2020-2023 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * objfreespace.spec: AcDb:ObjFreeSpace section specification
 * written by Reini Urban
 */

  #include "spec.h"

  ENCODER {
      if (!FIELD_VALUE (numnums)) {
        FIELD_VALUE (numhandles) = dwg->num_objects;
        FIELD_VALUE (TDUPDATE) = dwg->header_vars.TDUPDATE;
        FIELD_VALUE (numnums) = 4;
        FIELD_VALUE (max32) = 0x32;
        FIELD_VALUE (max64) = 0x64;
        FIELD_VALUE (maxtbl) = 0x200;
        FIELD_VALUE (maxrl) = 0xffffffff;
      }
  }
  UNTIL (R_2007)
  {
    FIELD_CAST (zero, RL, RLL, 0);
    FIELD_CAST (numhandles, RL, RLL, 0);
    FIELD_TIMERLL (TDUPDATE, 0);
    FIELD_RLx (objects_address, 0);
    FIELD_RC (numnums, 0);
    FIELD_RLL (max32, 0);    // 0x32
    FIELD_RLL (max64, 0);    // 0x64
    FIELD_RLL (maxtbl, 0);   // 0x200
    FIELD_RLL (maxrl, 0);    // 0xffffffff
  }
  LATER_VERSIONS {
    FIELD_RLL (zero, 0);
    FIELD_RLL (numhandles, 0);
    FIELD_TIMERLL (TDUPDATE, 0);
    FIELD_RC (numnums, 0);
    // num types are not 64 bit, but 128
    FIELD_RLL (max32, 0);       // 0x32
    FIELD_RLL (max32_hi, 0);    //
    FIELD_RLL (max64, 0);       // 0x64
    FIELD_RLL (max64_hi, 0);    // 0x0
    FIELD_RLL (maxtbl, 0);      // 0x200
    FIELD_RLL (maxtbl_hi, 0);   // 0x0
    FIELD_RLL (maxrl, 0);       // 0xffffffff
    FIELD_RLL (maxrl_hi, 0);    // 0x0
  }


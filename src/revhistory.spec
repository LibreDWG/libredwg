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
 * revhistory.spec: AcDb:RevHistory section specification
 * written by Reini Urban
 */

  #include "spec.h"

  FIELD_RL (class_version, 0);
  FIELD_RL (class_minor, 0);
  FIELD_RL (num_histories, 0);
  REPEAT (num_histories, histories, BITCODE_RL)
  REPEAT_BLOCK
    FIELD_RL (histories[rcount1], 0);
  END_REPEAT_BLOCK
  END_REPEAT (histories)

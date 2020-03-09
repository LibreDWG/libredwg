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
 * filedeplist.spec: AcDb:FileDepList section specification
 * written by Reini Urban
 */

  #include "spec.h"

  FIELD_RL (num_features, 0);
  FIELD_VECTOR_T (features, TU32, num_features, 0);
  FIELD_RL (num_files, 0);
  REPEAT (num_files, files, Dwg_FileDepList_Files)
  REPEAT_BLOCK
    FIELD_T32 (files[rcount1].filename, 0);
    FIELD_T32 (files[rcount1].filepath, 0);
    FIELD_T32 (files[rcount1].fingerprint, 0);
    FIELD_T32 (files[rcount1].version, 0);
    FIELD_RL (files[rcount1].feature_index, 0);
    FIELD_RL (files[rcount1].timestamp, 0);
    FIELD_RL (files[rcount1].filesize, 0);
    FIELD_RS (files[rcount1].affects_graphics, 0);
    FIELD_RL (files[rcount1].refcount, 0);
  END_REPEAT_BLOCK
  END_REPEAT (files)

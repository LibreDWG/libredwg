/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2019 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * r2004_file_header.spec: DWG file header specification
 * the encrypted part.
 * written by Reini Urban
 */

#include "spec.h"

  // to be done after encryption, resp. before when encoding
  SINCE (R_2004a) {
    IF_ENCODE_FROM_EARLIER {
      memcpy (_obj->file_ID_string, "AcFssFcAJMB", 12);
      FIELD_VALUE (header_size) = 108;
      FIELD_VALUE (unknown_long) = 1;
      FIELD_VALUE (x04) = 4;
      FIELD_VALUE (x20) = 0x20;
      FIELD_VALUE (x80) = 0x80;
      FIELD_VALUE (x40) = 0x40;
    }
  }
  FIELD_TFF (file_ID_string, 12, 0) //pre-allocated: "AcFssFcAJMB"
  FIELD_RLx (header_address, 0);
  FIELD_RL (header_size, 0);
  FIELD_RL (x04, 0);

  FIELD_RLd (root_tree_node_gap, 0);   // @0x18 - compr_len?
  FIELD_RLd (lowermost_left_tree_node_gap, 0);   // @0x1c
  FIELD_RLd (lowermost_right_tree_node_gap, 0);  // @0x20 - start of 2007_file_header
  FIELD_RL (unknown_long, 0);         // =1
  FIELD_RL (last_section_id, 0);      // @0x28 =0x13
  FIELD_RLL (last_section_address, 0);// @0x2c =0x9c80
  FIELD_RLL (secondheader_address, 0);// @0x34 =0x984b
  FIELD_RL (numgaps, 0);              // @0x3c =0
  FIELD_RL (numsections, 0);          // @0x40 =0x11
  FIELD_RL (x20, 0);        	      // @0x44 =0x20
  FIELD_RL (x80, 0);                  // @0x48 =0x80
  FIELD_RL (x40, 0);                  // @0x4c =0x40
  FIELD_RL (section_map_id, 0);       // @0x50 =0x13
  FIELD_RLL (section_map_address, 0); // @0x54 (+ 0x100) =0x97a0
  FIELD_RLd (section_info_id, 0);     // @0x5c
  FIELD_RL (section_array_size, 0);   // @0x60 =0x13
  FIELD_RL (gap_array_size, 0);       // @0x64 =0
  FIELD_RLx (crc32, 0);               // @0x68
  //end of encrypted header // @108 0x6c

  // well, the padding is also encrypted, but ODA didn't grok that.
  // 12 byte encrypted via 0
  FIELD_TFFx (padding, (int)sizeof(FIELD_VALUE (padding)), 0) // @120


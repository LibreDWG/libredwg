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
 * security.spec: AcDb:Security section specification,
 * when saved with a password.
 * written by Reini Urban
 */

  #include "spec.h"

  FIELD_RLx (unknown_1, 0);
  FIELD_RLx (unknown_2, 0);
  FIELD_RLx (unknown_3, 0);
  FIELD_RL (crypto_id, 0);
  FIELD_T32 (crypto_name, 0);
  FIELD_RL (algo_id, 0);
  FIELD_RL (key_len, 0);
  FIELD_RL (encr_size, 0);
  FIELD_BINARY (encr_buffer, FIELD_VALUE (encr_size), 0);

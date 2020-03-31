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
 * datastorage.spec: AcDb:AcDsProtoype_1b section specification, containing
 *                   new SAB ACIS content.
 * written by Reini Urban
 */

  #include "spec.h"

  // header
  FIELD_RLx (file_signature, 0);
  FIELD_RLd (file_header_size, 0);
  FIELD_RLd (unknown_1, 0); // acis version? always 2
  FIELD_RLd (version, 0); // always 2
  FIELD_RLd (unknown_2, 0); // always 0
  FIELD_RLd (ds_version, 0); // datastorage revision
  FIELD_RLd (segidx_offset, 0);
  FIELD_RLd (segidx_unknown, 0);
  FIELD_RLd (segidx_numentries, 0);
  FIELD_RLd (schidx_segidx, 0);
  FIELD_RLd (datidx_segidx, 0);
  FIELD_RLd (search_segidx, 0);
  FIELD_RLd (prev_save_idx, 0);
  FIELD_RL (file_size, 0);
  // TODO segments ...

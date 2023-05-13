/* -*- c -*- */
/*****************************************************************************/
/*  LibreDWG - free implementation of the DWG file format                    */
/*                                                                           */
/*  Copyright (C) 2018-2023 Free Software Foundation, Inc.                   */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 3 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

/*
 * common_object_handle_data.spec: common object handles
 * written by Reini Urban
 */

  #include "spec.h"

  SINCE (R_13b1)
  {
    FIELD_BL (num_reactors, 0); // max 15
#ifdef IS_DECODER
    if (FIELD_VALUE (num_reactors) * dwg_bits_size[BITS_HANDLE]
        > AVAIL_BITS (hdl_dat))
      {
        LOG_ERROR ("num_reactors: " FORMAT_BL " > AVAIL_BITS(hdl_dat): %" PRId64 "\n",
                   FIELD_VALUE (num_reactors), AVAIL_BITS (hdl_dat))
          FIELD_VALUE (num_reactors) = 0;
        return DWG_ERR_VALUEOUTOFBOUNDS;
      }
#endif
  }
  SINCE (R_2004) { FIELD_B (is_xdic_missing, 0); }
  SINCE (R_2013) { FIELD_B (has_ds_data, 0); }

#ifdef IS_DXF
  // unused. see out_dxf.c
  SINCE (R_13b1) {
    XDICOBJHANDLE (3);
    REACTORS (4);
  }
#endif

#if !defined(IS_FREE) && !defined(IS_JSON)
  // done later in the dwg.spec, because of num_entries
  if (!dwg_obj_is_control (obj))
#endif
    {
      SINCE (R_13b1) { FIELD_HANDLE (ownerhandle, 4, 330); }

#ifndef IS_DXF
      SINCE (R_13b1) {
        REACTORS (4)
        XDICOBJHANDLE (3)
      }
#endif
    }

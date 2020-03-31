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
  FIELD_RLd (segidx_num_entries, 0);
  FIELD_RLd (schidx_segidx, 0);
  FIELD_RLd (datidx_segidx, 0);
  FIELD_RLd (search_segidx, 0);
  FIELD_RLd (prev_save_idx, 0);
  FIELD_RL (file_size, 0);
  DECODER {
    _obj->num_segments = 1; //FIXME poke the idx's
  }
  REPEAT (num_segments, segments, Dwg_AcDsProtoype_Segment)
  REPEAT_BLOCK
      SUB_FIELD (segments[rcount1],signature, RL, 0); /* always 0xd5ac */
      /* segidx, datidx, _data_, schidx, schdat, search, blob01 */
      FIELD_TFF (segments[rcount1].name, 6, 0);
      SUB_FIELD (segments[rcount1],segment_idx, RL, 0);
      SUB_FIELD (segments[rcount1],is_blob01, RL, 0);
      SUB_FIELD (segments[rcount1],segsize, RL, 0);
      SUB_FIELD (segments[rcount1],unknown_2, RL, 0);
      SUB_FIELD (segments[rcount1],ds_version, RL, 0); // datastorage revision
      SUB_FIELD (segments[rcount1],unknown_3, RL, 0);
      SUB_FIELD (segments[rcount1],data_algn_offset, RL, 0);
      SUB_FIELD (segments[rcount1],objdata_algn_offset, RL, 0);
      FIELD_TFF (segments[rcount1].padding, 8, 0); // always 8x 0x55
      DECODER {
        if (strEQc ((char*)_obj->segments[rcount1].name, "segidx"))
          _obj->segments[rcount1].type = 0;
        else if (strEQc ((char*)_obj->segments[rcount1].name, "datidx"))
          _obj->segments[rcount1].type = 1;
        else if (strEQc ((char*)_obj->segments[rcount1].name, "_data_"))
          _obj->segments[rcount1].type = 2;
        else if (strEQc ((char*)_obj->segments[rcount1].name, "schidx"))
          _obj->segments[rcount1].type = 3;
        else if (strEQc ((char*)_obj->segments[rcount1].name, "schdat"))
          _obj->segments[rcount1].type = 4;
        else if (strEQc ((char*)_obj->segments[rcount1].name, "search"))
          _obj->segments[rcount1].type = 5;
        else if (strEQc ((char*)_obj->segments[rcount1].name, "blob01"))
          _obj->segments[rcount1].type = 6;
        else
          LOG_ERROR ("Invalid AcDsProtoype.segments.name %s", (char*)_obj->segments[rcount1].name);
      }
      JSON {
        SUB_FIELD (segments[rcount1],type, RC, 0);
      }
      //TODO
      switch (_obj->segments[rcount1].type) {
      case 0: break;
      case 1: break;
      case 2: break;
      case 3: break;
      case 4: break;
      case 5: break;
      case 6: break;
      default: break;
      }
  END_REPEAT_BLOCK
  END_REPEAT (segments)

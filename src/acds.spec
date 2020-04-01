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
 * acds.spec: AcDb:AcDsProtoype_1b datastorage section specification,
 *            containing new SAB ACIS content.
 * written by Reini Urban
 */

  #include "spec.h"

  static char _AcDs_Schema_Prop_types[] = {0, 0, 2, 1, 2, 4, 8, 1, 2, 4, 8, 4, 8, 0, 0, 0};

  DECODER {
    LOG_TRACE ("unknown_bits [%ld (%u,%d,%d) %ld TF]: ", dat->size * 8,
               0, 0, 0, dat->size);
    LOG_TRACE_TF (dat->chain, (int)dat->size);
    LOG_TRACE ("\n");
  }
  // header
  FIELD_RLx (file_signature, 0);
  FIELD_RL (file_header_size, 0);
  FIELD_RL (unknown_1, 0);  // always 2
  FIELD_RL (version, 0);    // always 2
  FIELD_RL (unknown_2, 0);  // always 0
  FIELD_RL (ds_version, 0); // datastorage revision
  FIELD_RL (segidx_offset, 0);
  FIELD_RL (segidx_unknown, 0);
  FIELD_RL (num_segidx, 0);
  FIELD_RL (schidx_segidx, 0);
  FIELD_RL (datidx_segidx, 0);
  FIELD_RL (search_segidx, 0);
  FIELD_RL (prvsav_segidx, 0);
  FIELD_RL (file_size, 0);
#ifdef IS_DECODER
  dat->byte = _obj->segidx_offset;
  _obj->segments = calloc (_obj->num_segidx, sizeof (Dwg_AcDs_Segment));
#endif
#ifndef IS_JSON
  SUB_FIELD_RSx (segments[0],signature, 0); /* always 0xD5AC (ACD5 in the TF) */
  FIELD_TFF (segments[0].name, 6, 0); /* always segidx */
  DECODER { _obj->segments[0].type = 0; }
  //JSON { SUB_FIELD (segments[0],type, RC, 0); }
  SUB_FIELD (segments[0],segment_idx, RL, 0);
  SUB_FIELD (segments[0],is_blob01, RL, 0);
  SUB_FIELD (segments[0],segsize, RL, 0);
  SUB_FIELD (segments[0],unknown_2, RL, 0);
  SUB_FIELD (segments[0],ds_version, RL, 0); // datastorage revision
  SUB_FIELD (segments[0],unknown_3, RL, 0);
  SUB_FIELD (segments[0],data_algn_offset, RL, 0);
  SUB_FIELD (segments[0],objdata_algn_offset, RL, 0);
  FIELD_TFF (segments[0].padding, 8, 0); // always 8x 0x55
#endif

  REPEAT (num_segidx, segidx, Dwg_AcDs_SegmentIndex)
  REPEAT_BLOCK
#ifdef IS_JSON
      KEY (index); VALUE_RL (rcount1, 0);
#endif
      SUB_FIELD_RLL (segidx[rcount1],offset, 0);
      SUB_FIELD_RL (segidx[rcount1],size, 0);
      DECODER { if (_obj->segidx[rcount1].offset) _obj->total_segments++; }
  END_REPEAT_BLOCK
#ifdef IS_JSON
  END_REPEAT (segidx) // still needed, later
#endif

  _REPEAT_NF (_obj->num_segidx, segments, Dwg_AcDs_Segment, 1)
  REPEAT_BLOCK
      if (_obj->segidx[rcount1].offset)
        {
#ifdef IS_JSON
          KEY (index); VALUE_RL (rcount1, 0);
#endif
          LOG_TRACE ("\nsegments[%d] offset: %ld\n", rcount1, _obj->segidx[rcount1].offset)
          dat->byte = _obj->segidx[rcount1].offset;
        }
      else
        {
#ifdef IS_JSON
          ENDHASH;
#endif
          continue;
        }
#ifdef IS_JSON
      if (!_obj->segments[rcount1].signature)
        {
          ENDHASH;
          continue;
        }
#endif

      SUB_FIELD_RSx (segments[rcount1],signature, 0); /* always 0xD5AC (ACD5 in the TF) */
      /* segidx, datidx, _data_, schidx, schdat, search, blob01, prvsav */
      FIELD_TFF (segments[rcount1].name, 6, 0);
      DECODER {
        _obj->segments[rcount1].name[6] = '\0';
        _obj->segments[rcount1].type = -1;
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
        else if (strEQc ((char*)_obj->segments[rcount1].name, "prvsav")) // ODA undocumented
          _obj->segments[rcount1].type = 7;
        else
          {
            LOG_ERROR ("Invalid AcDs.segments[%d].name %s", rcount1,
                       (char *)_obj->segments[rcount1].name);
            return DWG_ERR_SECTIONNOTFOUND;
          }
      }
      JSON {
        SUB_FIELD (segments[rcount1],type, RC, 0);
      }
      SUB_FIELD (segments[rcount1],segment_idx, RL, 0);
      SUB_FIELD (segments[rcount1],is_blob01, RL, 0);
      SUB_FIELD (segments[rcount1],segsize, RL, 0);
      SUB_FIELD (segments[rcount1],unknown_2, RL, 0);
      SUB_FIELD (segments[rcount1],ds_version, RL, 0); // datastorage revision
      SUB_FIELD (segments[rcount1],unknown_3, RL, 0);
      SUB_FIELD (segments[rcount1],data_algn_offset, RL, 0);
      SUB_FIELD (segments[rcount1],objdata_algn_offset, RL, 0);
      FIELD_TFF (segments[rcount1].padding, 8, 0); // always 8x 0x55

      switch (_obj->segments[rcount1].type) {
      case 0: // hmm, more than 1 segidx?
#if 0
        REPEAT2 (num_segidx, segidx, Dwg_AcDs_SegmentIndex)
        REPEAT_BLOCK
#ifdef IS_JSON
            KEY (index); VALUE_RL (rcount2, 0);
#endif
            SUB_FIELD_RLL (segidx[rcount2],offset, 0);
            SUB_FIELD_RL (segidx[rcount2],size, 0);
        END_REPEAT_BLOCK
        END_REPEAT (segidx)
#endif
        break;
      case 1:
        if (_obj->datidx_segidx != rcount1)
          LOG_WARN ("Possibly wrong datidx_segidx %d for %d", _obj->datidx_segidx, rcount1);
        SUB_FIELD_RL (datidx,num_entries, 0);
        SUB_FIELD_RL (datidx,unknown, 0);
        REPEAT2 (datidx.num_entries, datidx.entries, Dwg_AcDs_DataIndex_Entry)
        REPEAT_BLOCK
#ifdef IS_JSON
            KEY (index); VALUE_RL (rcount2, 0);
#endif
            SUB_FIELD_RL (datidx.entries[rcount2],segidx, 0);
            SUB_FIELD_RL (datidx.entries[rcount2],offset, 0);
            SUB_FIELD_RL (datidx.entries[rcount2],schidx, 0);
        END_REPEAT_BLOCK
        END_REPEAT (datidx.entries)
        break;
      case 3:
        if (_obj->schidx_segidx != rcount1)
          LOG_WARN ("Possibly wrong schidx_segidx %d for %d", _obj->schidx_segidx, rcount1);
        SUB_FIELD_RL (schidx,num_props, 0); // or RLL
        SUB_FIELD_RL (schidx,unknown_1, 0);
        REPEAT2 (schidx.num_props, schidx.props, Dwg_AcDs_SchemaIndex_Prop)
        REPEAT_BLOCK
            SUB_FIELD_RL (schidx.props[rcount2],index, 0);
            SUB_FIELD_RL (schidx.props[rcount2],segidx, 0);
            SUB_FIELD_RL (schidx.props[rcount2],offset, 0);
        END_REPEAT_BLOCK
        END_REPEAT (schidx.props)
        SUB_FIELD_RLL (schidx,tag, 0); /* 0x0af10c */
        SUB_FIELD_RL (schidx,num_prop_entries, 0); // or RLL
        SUB_FIELD_RL (schidx,unknown_2, 0);
        REPEAT2 (schidx.num_prop_entries, schidx.prop_entries, Dwg_AcDs_SchemaIndex_Prop)
        REPEAT_BLOCK
            SUB_FIELD_RL (schidx.prop_entries[rcount2],index, 0);
            SUB_FIELD_RL (schidx.prop_entries[rcount2],segidx, 0);
            SUB_FIELD_RL (schidx.prop_entries[rcount2],offset, 0);
        END_REPEAT_BLOCK
        END_REPEAT (schidx.prop_entries)
        break;
      case 4:
        DEBUG_HERE
        DECODER { _obj->schdat.num_uprops = 1; } // FIXME
        REPEAT2 (schdat.num_uprops, schdat.uprops, Dwg_AcDs_SchemaData_UProp)
        REPEAT_BLOCK
          SUB_FIELD_RL (schdat.uprops[rcount2], size, 0);
          SUB_FIELD_RL (schdat.uprops[rcount2], flags, 0);
        END_REPEAT_BLOCK
        END_REPEAT (schdat.uprops)
        DEBUG_HERE
        REPEAT2 (schdat.num_schemas, schdat.schemas, Dwg_AcDs_Schema)
        REPEAT_BLOCK
            SUB_FIELD_RS (schdat.schemas[rcount2],num_index, 0);
            SUB_FIELD_VECTOR (schdat.schemas[rcount2],index, num_index, RLL, 0);
            SUB_FIELD_RS (schdat.schemas[rcount2],num_props, 0);
            REPEAT2 (schdat.schemas[rcount2].num_props, schdat.schemas[rcount2].props, Dwg_AcDs_Schema_Prop)
#define prop schdat.schemas[rcount2].props[rcount3] 
            REPEAT_BLOCK
                SUB_FIELD_RL (prop,flags, 91); // 1, 2, 8
                SUB_FIELD_RL (prop,namidx, 2);
                if (!(_obj->prop.flags & 2))
                {
                  SUB_FIELD_RL (prop,type, 280);
                  if (_obj->prop.type  == 0xe)
                    {
                      SUB_FIELD_RL (prop,type_size, 0)
                    }
                  else
                    _obj->prop.type_size = _AcDs_Schema_Prop_types[_obj->prop.type];
                }
                if (_obj->prop.flags == 1)
                  {
                    SUB_FIELD_RL (prop,unknown_1, 0)
                  }
                else if (_obj->prop.flags == 8)
                  {
                    SUB_FIELD_RL (prop,unknown_2, 0)
                  }
                SUB_FIELD_RS (prop,num_values, 0);
                if (_obj->prop.type_size)
                  {
                    SUB_FIELD_VECTOR_TYPESIZE (prop, values, num_values,
                                               _obj->prop.type_size, 0);
                  }
            END_REPEAT_BLOCK
#undef prop
            END_REPEAT (schdat.schemas[rcount2].props)
        END_REPEAT_BLOCK
        END_REPEAT (schdat.schemas)
        LOG_WARN ("AcDs %s segment yet unhandled", _obj->segments[rcount1].name)
        break;
      case 5: // search
        if (_obj->search_segidx != rcount1)
          LOG_WARN ("Possibly wrong search_segidx %d for %d", _obj->search_segidx, rcount1);
        SUB_FIELD_RL (search,num_search, 0);
        REPEAT2 (search.num_search, search.search, Dwg_AcDs_Search_Data)
        REPEAT_BLOCK
          SUB_FIELD_RL (search.search[rcount2],schema_namidx, 0);
          SUB_FIELD_RL (search.search[rcount2],num_sortedidx, 0);
          SUB_FIELD_VECTOR (search.search[rcount2],sortedidx, num_sortedidx, RLL, 0);
          SUB_FIELD_RL (search.search[rcount2],num_ididxs, 0);
          SUB_FIELD_RL (search.search[rcount2],unknown, 0);
          REPEAT3 (search.search[rcount2].num_ididxs, search.search[rcount2].ididxs, Dwg_AcDs_Search_IdIdxs)
          REPEAT_BLOCK
            SUB_FIELD_RL (search.search[rcount2].ididxs[rcount3],num_ididx, 0);
            REPEAT4 (search.search[rcount2].ididxs[rcount3].num_ididx, search.search[rcount2].ididxs[rcount3].ididx, Dwg_AcDs_Search_IdIdx)
            REPEAT_BLOCK
              SUB_FIELD_RLL (search.search[rcount2].ididxs[rcount3].ididx[rcount4],handle, 0);
              SUB_FIELD_RL (search.search[rcount2].ididxs[rcount3].ididx[rcount4],num_ididx, 0);
              SUB_FIELD_VECTOR (search.search[rcount2].ididxs[rcount3].ididx[rcount4],ididx, num_ididx, RLL, 0);
            END_REPEAT_BLOCK
            END_REPEAT (search.search[rcount2].ididxs[rcount3].ididx)
          END_REPEAT_BLOCK
          END_REPEAT (search.search[rcount2].ididxs)
        END_REPEAT_BLOCK
        END_REPEAT (search.search)
        break;
      case 7: // prvsav
        if (_obj->prvsav_segidx != rcount1)
          LOG_WARN ("Possibly wrong prvsav_segidx %d for %d", _obj->prvsav_segidx, rcount1);
        // fallthru
        //break;
      case 2: // _data_
        //break;
      case 6: // blob01
        //break;
      default:
        LOG_WARN ("AcDs %s segment yet unhandled", _obj->segments[rcount1].name)
        break;
      }
  END_REPEAT_BLOCK
  END_REPEAT (segments)
#ifdef IS_FREE
  END_REPEAT (segidx)
#endif

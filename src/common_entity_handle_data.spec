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
 * common_entity_handle_data.spec: common entity handles
 * written by Reini Urban
 */

  #include "spec.h"

#ifdef IS_DXF
  ENT_REACTORS (4)
  ENT_XDICOBJHANDLE (3)
#endif
  if (FIELD_VALUE (entmode) == 0)
    {
      FIELD_HANDLE (ownerhandle, 4, 330);
    }
#ifdef IS_DXF
    /* parent: {m,p}space block_record or polyline for vertex, block until blkend */
  if (FIELD_VALUE(entmode) != 0)
    {
      if (_ent->ownerhandle || _ent->entmode == 3) {
        //assert(_ent->entmode == 3); /* does not exist */
        VALUE_HANDLE (_ent->ownerhandle, ownerhandle, 5, 330);
      } else if (_ent->entmode == 1) {
        VALUE_HANDLE (dwg->header_vars.BLOCK_RECORD_PSPACE, BLOCK_RECORD_PSPACE, 5, 330);
      } else {
        //assert(_ent->entmode == 2);
        VALUE_HANDLE (dwg->header_vars.BLOCK_RECORD_MSPACE, BLOCK_RECORD_MSPACE, 5, 330);
      }
    }
#else
# ifdef IS_DECODER
  if (_obj->num_reactors > 100000)
    {
      LOG_ERROR ("num_reactors: " FORMAT_BL ", AVAIL_BITS(hdl_dat): % " PRId64
                 "\n", _obj->num_reactors, AVAIL_BITS (hdl_dat))
      _obj->num_reactors = 0;
      return DWG_ERR_VALUEOUTOFBOUNDS;
    }
# endif
  ENT_REACTORS (4)
  ENT_XDICOBJHANDLE (3)
#endif
  SUBCLASS (AcDbEntity)
#ifdef IS_DXF
  // PaperSpace0 BLOCK may have entmode 0
  if (_ent->entmode == 1 ||
      (_ent->entmode == 0 && _ent->ownerhandle == obj->parent->header_vars.BLOCK_RECORD_PSPACE))
    FIELD_BB (entmode, 67); // is paperspace
#endif

  VERSIONS (R_13b1, R_14)
    {
      FIELD_HANDLE (layer, 5, 8);
#ifdef IS_ENCODER
      if (dat->from_version == R_2000)
        FIELD_VALUE (isbylayerlt) = FIELD_VALUE (ltype_flags) < 3 ? 1 : 0;
#endif
#ifdef IS_DXF
      PRE (R_2000) {
        switch (FIELD_VALUE (ltype_flags)) {
        case 0: break; //VALUE_TV ("BYLAYER", 6); break;
        case 1: VALUE_TV ("BYBLOCK", 6); break;
        case 2: VALUE_TV ("CONTINUOUS", 6); break;
        default: break;
        }
      } LATER_VERSIONS {
        switch (FIELD_VALUE (ltype_flags)) {
        case 0: break; //VALUE_TV ("ByLayer", 6); break;
        case 1: VALUE_TV ("ByBlock", 6); break;
        case 2: VALUE_TV ("Continuous", 6); break;
        default: break;
        }
      }
#endif
      if (!FIELD_VALUE (isbylayerlt))
        FIELD_HANDLE (ltype, 5, 6);
    }

  VERSIONS (R_13b1, R_2000)
    {
      if (!FIELD_VALUE (nolinks))
        {
          FIELD_HANDLE (prev_entity, 4, 0);
          FIELD_HANDLE (next_entity, 4, 0);
        }
    }

  SINCE (R_2000b)
    {
      FIELD_HANDLE (layer, 5, 8);

#ifdef IS_DXF
      switch (FIELD_VALUE (ltype_flags)) {
      case 0: break; //VALUE_TV ("ByLayer", 6); break;
      case 1: VALUE_TV ("ByBlock", 6); break;
      case 2: VALUE_TV ("Continuous", 6); break;
      default: break;
      }
#endif
      if (FIELD_VALUE (ltype_flags) == 3)
        FIELD_HANDLE (ltype, 5, 6);
#ifdef IS_DXF
      FIELD_BD1 (ltype_scale, 48);
      // for entities with lines 29 is the default, without lines 28
      if (FIELD_VALUE (entmode) == 0 && FIELD_VALUE (linewt) == 28)
        ; // SEQEND, VERTEX* have entmode 0
      else if (FIELD_VALUE (linewt) != 29) { /* ByLayer */
        int lw = dxf_cvt_lweight (FIELD_VALUE (linewt));
        KEY (linewt); VALUE_BSd (lw, 370);
      }
#endif
    }

  SINCE (R_2007a)
    {
      if (FIELD_VALUE (material_flags) == 3)
        FIELD_HANDLE (material, 5, 347);
      if (FIELD_VALUE (shadow_flags) == 3)
        FIELD_HANDLE (shadow, 5, 0);
    }

  SINCE (R_2000b)
    {
      if (FIELD_VALUE (plotstyle_flags) == 3)
        FIELD_HANDLE (plotstyle, 5, 390);
    }

  SINCE (R_2010b)
    {
      // DXF 348 but which?
      if (FIELD_VALUE (has_full_visualstyle))
        FIELD_HANDLE (full_visualstyle, 5, 348);
      if (FIELD_VALUE (has_face_visualstyle))
        FIELD_HANDLE (face_visualstyle, 5, 348);
      if (FIELD_VALUE (has_edge_visualstyle))
        FIELD_HANDLE (edge_visualstyle, 5, 348);
    }

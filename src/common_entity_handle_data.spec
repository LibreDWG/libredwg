/* -*- c -*- */

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
      if (ent->ownerhandle || ent->entmode == 3) {
        //assert(ent->entmode == 3); /* does not exist */
        VALUE_HANDLE (ent->ownerhandle, ownerhandle, 5, 330);
      } else if (ent->entmode == 1) {
        VALUE_HANDLE (dwg->header_vars.BLOCK_RECORD_PSPACE, BLOCK_RECORD_PSPACE, 5, 330);
      } else {
        //assert(ent->entmode == 2);
        VALUE_HANDLE (dwg->header_vars.BLOCK_RECORD_MSPACE, BLOCK_RECORD_MSPACE, 5, 330);
      }
    }
#else
# ifdef IS_DECODER
  if (_obj->num_reactors > 100000)
    {
      LOG_ERROR ("num_reactors: " FORMAT_BL ", AVAIL_BITS(hdl_dat): %lld\n",
                 _obj->num_reactors, AVAIL_BITS (hdl_dat))
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
  if (ent->entmode == 1 ||
      (ent->entmode == 0 && ent->ownerhandle == obj->parent->header_vars.BLOCK_RECORD_PSPACE))
    FIELD_BB (entmode, 67); // is paperspace
#endif

  VERSIONS (R_13, R_14)
    {
      FIELD_HANDLE (layer, 5, 8);
#ifdef IS_ENCODER
      if (dat->from_version == R_2000)
        FIELD_VALUE (isbylayerlt) = FIELD_VALUE (ltype_flags) < 3 ? 1 : 0;
#endif
#ifdef IS_DXF
      switch (FIELD_VALUE (ltype_flags)) {
      case 0: break; //VALUE_TV ("ByLayer", 6); break;
      case 1: VALUE_TV ("ByBlock", 6); break;
      case 2: VALUE_TV ("Continuous", 6); break;
      default: break;
      }
#endif
      if (!FIELD_VALUE (isbylayerlt))
        FIELD_HANDLE (ltype, 5, 6);
    }

  VERSIONS (R_13, R_2000)
    {
      if (!FIELD_VALUE (nolinks))
        { // TODO: in R13, R14 these are optional. Look at page 53 in the spec
          //       for condition.
          FIELD_HANDLE (prev_entity, 4, 0);
          FIELD_HANDLE (next_entity, 4, 0);
        }
    }

  SINCE (R_2000)
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
      if (FIELD_VALUE (linewt) != 29) {
        int lw = dxf_cvt_lweight (FIELD_VALUE (linewt));
        KEY (linewt); VALUE_RC ((signed char)lw, 370);
      }
#endif
    }

  SINCE (R_2007)
    {
      if (FIELD_VALUE (material_flags) == 3)
        FIELD_HANDLE (material, 5, 347);
      if (FIELD_VALUE (shadow_flags) == 3)
        FIELD_HANDLE (shadow, 5, 0);
    }

  SINCE (R_2000)
    {
      if (FIELD_VALUE (plotstyle_flags) == 3)
        FIELD_HANDLE (plotstyle, 5, 390);
    }

  SINCE (R_2010)
    {
      // DXF 348 but which?
      if (FIELD_VALUE (has_full_visualstyle))
        FIELD_HANDLE (full_visualstyle, 5, 348);
      if (FIELD_VALUE (has_face_visualstyle))
        FIELD_HANDLE (face_visualstyle, 5, 348);
      if (FIELD_VALUE (has_edge_visualstyle))
        FIELD_HANDLE (edge_visualstyle, 5, 348);
    }

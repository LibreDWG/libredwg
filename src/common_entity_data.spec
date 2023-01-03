/* -*- c -*- */

  #include "spec.h"
  //Dwg_Object_Entity* _obj = _ent;

  PRE (R_2_0b) {
    FIELD_HANDLE (layer, 2, 8);
  }
  LATER_VERSIONS {
    PRE (R_13b1) {
      SINCE (R_2_0b) {
        FIELD_RC (flag_r11, 0);
#ifdef IS_DECODER
        if (_obj->flag_r11)
          {
            LOG_TRACE ("          ");
            // stringify the flag bits
#define LOG_FLAG_R11(w)                                 \
            if (R11FLAG (FLAG_R11_##w))                 \
              LOG_TRACE (#w " (%02x) ", FLAG_R11_##w)
            LOG_FLAG_R11 (COLOR);
            LOG_FLAG_R11 (LTYPE);
            LOG_FLAG_R11 (ELEVATION);
            LOG_FLAG_R11 (THICKNESS);
            LOG_FLAG_R11 (UNKNOWN_16);
            LOG_FLAG_R11 (HANDLING);
            LOG_FLAG_R11 (PSPACE);
            LOG_FLAG_R11 (HAS_ATTRIBS);
            LOG_TRACE ("\n");
#undef LOG_FLAG_R11
          }
        obj->size = bit_read_RS (dat);
#elif defined IS_ENCODER
        bit_write_RS (dat, obj->size);
#elif defined IS_JSON
        //KEY (size); VALUE_RS (obj->size, 0);
#endif
        LOG_TRACE("size: %d [RS]", obj->size);
        LOG_POS
        FIELD_HANDLE (layer, 2, 8);
      }
      FIELD_RSx (opts_r11, 0); // i.e. dataflags
#ifdef IS_DECODER
        if (_obj->opts_r11)
          {
            LOG_TRACE ("          ");
            // stringify the flag bits
#define LOG_OPTS_R11(e, w)                                                      \
            if (obj->fixedtype == DWG_TYPE_##e && (R11OPTS (OPTS_R11_##e##_##w)))  \
              LOG_TRACE (#w " (0x%d) ", OPTS_R11_##e##_##w)

            LOG_OPTS_R11 (_3DFACE, HAS_Z_FIRST);
            LOG_OPTS_R11 (_3DFACE, HAS_Z_SECOND);
            LOG_OPTS_R11 (_3DFACE, HAS_Z_THIRD);
            LOG_OPTS_R11 (_3DFACE, HAS_Z_FOURTH);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_16);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_32);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_64);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_128);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_256);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_512);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_1024);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_2048);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_4096);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_8192);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_16384);
            LOG_OPTS_R11 (_3DFACE, UNKNOWN_32768);

            LOG_OPTS_R11 (ARC, HAS_EXTRUSION);
            LOG_OPTS_R11 (ARC, UNKNOWN_2);
            LOG_OPTS_R11 (ARC, UNKNOWN_4);
            LOG_OPTS_R11 (ARC, UNKNOWN_8);
            LOG_OPTS_R11 (ARC, UNKNOWN_16);
            LOG_OPTS_R11 (ARC, UNKNOWN_32);
            LOG_OPTS_R11 (ARC, UNKNOWN_64);
            LOG_OPTS_R11 (ARC, UNKNOWN_128);
            LOG_OPTS_R11 (ARC, UNKNOWN_256);
            LOG_OPTS_R11 (ARC, UNKNOWN_512);
            LOG_OPTS_R11 (ARC, UNKNOWN_1024);
            LOG_OPTS_R11 (ARC, UNKNOWN_2048);
            LOG_OPTS_R11 (ARC, UNKNOWN_4096);
            LOG_OPTS_R11 (ARC, UNKNOWN_8192);
            LOG_OPTS_R11 (ARC, UNKNOWN_16384);
            LOG_OPTS_R11 (ARC, UNKNOWN_32768);

            LOG_OPTS_R11 (ATTDEF, UNKNOWN_1);
            LOG_OPTS_R11 (ATTDEF, HAS_ROTATION);
            LOG_OPTS_R11 (ATTDEF, HAS_WIDTH_FACTOR);
            LOG_OPTS_R11 (ATTDEF, HAS_OBLIQUE_ANGLE);
            LOG_OPTS_R11 (ATTDEF, HAS_STYLE);
            LOG_OPTS_R11 (ATTDEF, HAS_GENERATION);
            LOG_OPTS_R11 (ATTDEF, HAS_HORIZ_ALIGNMENT);
            LOG_OPTS_R11 (ATTDEF, HAS_ALIGNMENT_POINT);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_256);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_512);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_1024);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_2048);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_4096);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_8192);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_16384);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_32768);

            LOG_OPTS_R11 (ATTRIB, UNKNOWN_1);
            LOG_OPTS_R11 (ATTRIB, HAS_ROTATION);
            LOG_OPTS_R11 (ATTRIB, HAS_WIDTH_FACTOR);
            LOG_OPTS_R11 (ATTRIB, HAS_OBLIQUE_ANGLE);
            LOG_OPTS_R11 (ATTRIB, HAS_STYLE);
            LOG_OPTS_R11 (ATTRIB, HAS_GENERATION);
            LOG_OPTS_R11 (ATTRIB, HAS_HORIZ_ALIGNMENT);
            LOG_OPTS_R11 (ATTRIB, HAS_ALIGNMENT_POINT);
            LOG_OPTS_R11 (ATTRIB, UNKNOWN_256);
            LOG_OPTS_R11 (ATTRIB, UNKNOWN_512);
            LOG_OPTS_R11 (ATTRIB, UNKNOWN_1024);
            LOG_OPTS_R11 (ATTRIB, UNKNOWN_2048);
            LOG_OPTS_R11 (ATTRIB, UNKNOWN_4096);
            LOG_OPTS_R11 (ATTRIB, UNKNOWN_8192);
            LOG_OPTS_R11 (ATTRIB, UNKNOWN_16384);
            LOG_OPTS_R11 (ATTDEF, UNKNOWN_32768);

            LOG_OPTS_R11 (BLOCK, UNKNOWN_1);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_2);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_4);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_8);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_16);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_32);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_64);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_128);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_256);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_512);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_1024);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_2048);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_4096);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_8192);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_16384);
            LOG_OPTS_R11 (BLOCK, UNKNOWN_32768);

            LOG_OPTS_R11 (CIRCLE, HAS_EXTRUSION);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_2);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_4);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_8);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_16);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_32);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_64);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_128);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_256);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_512);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_1024);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_2048);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_4096);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_8192);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_16384);
            LOG_OPTS_R11 (CIRCLE, UNKNOWN_32768);

            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_1);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_2);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_4);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_8);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_16);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_32);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_64);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_128);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_256);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_512);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_1024);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_2048);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_4096);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_8192);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_16384);
            LOG_OPTS_R11 (DIMENSION_ANG2LN, UNKNOWN_32768);

            LOG_OPTS_R11 (INSERT, HAS_SCALE_X);
            LOG_OPTS_R11 (INSERT, HAS_SCALE_Y);
            LOG_OPTS_R11 (INSERT, HAS_ROTATION);
            LOG_OPTS_R11 (INSERT, HAS_SCALE_Z);
            LOG_OPTS_R11 (INSERT, HAS_NUM_COLS);
            LOG_OPTS_R11 (INSERT, HAS_NUM_ROWS);
            LOG_OPTS_R11 (INSERT, HAS_COL_SPACING);
            LOG_OPTS_R11 (INSERT, HAS_ROW_SPACING);
            LOG_OPTS_R11 (INSERT, HAS_EXTRUSION);
            LOG_OPTS_R11 (INSERT, UNKNOWN_512);
            LOG_OPTS_R11 (INSERT, UNKNOWN_1024);
            LOG_OPTS_R11 (INSERT, UNKNOWN_2048);
            LOG_OPTS_R11 (INSERT, UNKNOWN_4096);
            LOG_OPTS_R11 (INSERT, UNKNOWN_8192);
            LOG_OPTS_R11 (INSERT, UNKNOWN_16384);
            LOG_OPTS_R11 (INSERT, UNKNOWN_32768);

            LOG_OPTS_R11 (LINE, HAS_EXTRUSION);
            LOG_OPTS_R11 (LINE, UNKNOWN_2);
            LOG_OPTS_R11 (LINE, UNKNOWN_4);
            LOG_OPTS_R11 (LINE, UNKNOWN_8);
            LOG_OPTS_R11 (LINE, UNKNOWN_16);
            LOG_OPTS_R11 (LINE, UNKNOWN_32);
            LOG_OPTS_R11 (LINE, UNKNOWN_64);
            LOG_OPTS_R11 (LINE, UNKNOWN_128);
            LOG_OPTS_R11 (LINE, UNKNOWN_256);
            LOG_OPTS_R11 (LINE, UNKNOWN_512);
            LOG_OPTS_R11 (LINE, UNKNOWN_1024);
            LOG_OPTS_R11 (LINE, UNKNOWN_2048);
            LOG_OPTS_R11 (LINE, UNKNOWN_4096);
            LOG_OPTS_R11 (LINE, UNKNOWN_8192);
            LOG_OPTS_R11 (LINE, UNKNOWN_16384);
            LOG_OPTS_R11 (LINE, UNKNOWN_32768);

            LOG_OPTS_R11 (POLYLINE_2D, HAS_POLYLINE_FLAG);
            LOG_OPTS_R11 (POLYLINE_2D, HAS_START_WIDTH);
            LOG_OPTS_R11 (POLYLINE_2D, HAS_END_WIDTH);
            LOG_OPTS_R11 (POLYLINE_2D, HAS_EXTRUSION);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_16);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_32);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_64);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_128);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_256);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_512);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_1024);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_2048);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_4096);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_8192);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_16384);
            LOG_OPTS_R11 (POLYLINE_2D, UNKNOWN_32768);

            LOG_OPTS_R11 (POLYLINE_3D, HAS_POLYLINE_FLAG);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_2);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_4);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_8);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_16);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_32);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_64);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_128);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_256);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_512);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_1024);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_2048);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_4096);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_8192);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_16384);
            LOG_OPTS_R11 (POLYLINE_3D, UNKNOWN_32768);

            LOG_OPTS_R11 (POLYLINE_MESH, HAS_POLYLINE_FLAG);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_2);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_4);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_8);
            LOG_OPTS_R11 (POLYLINE_MESH, HAS_M_VERTS);
            LOG_OPTS_R11 (POLYLINE_MESH, HAS_N_VERTS);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_64);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_128);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_256);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_512);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_1024);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_2048);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_4096);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_8192);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_16384);
            LOG_OPTS_R11 (POLYLINE_MESH, UNKNOWN_32768);

            LOG_OPTS_R11 (SHAPE, HAS_ROTATION);
            LOG_OPTS_R11 (SHAPE, HAS_LOAD_NUM);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_4);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_8);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_16);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_32);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_64);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_128);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_256);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_512);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_1024);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_2048);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_4096);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_8192);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_16384);
            LOG_OPTS_R11 (SHAPE, UNKNOWN_32768);

            LOG_OPTS_R11 (TEXT, HAS_ROTATION);
            LOG_OPTS_R11 (TEXT, HAS_WIDTH_FACTOR);
            LOG_OPTS_R11 (TEXT, HAS_OBLIQUE_ANGLE);
            LOG_OPTS_R11 (TEXT, HAS_STYLE);
            LOG_OPTS_R11 (TEXT, HAS_GENERATION);
            LOG_OPTS_R11 (TEXT, HAS_HORIZ_ALIGNMENT);
            LOG_OPTS_R11 (TEXT, HAS_ALIGNMENT_POINT);
            LOG_OPTS_R11 (TEXT, UNKNOWN_128);
            LOG_OPTS_R11 (TEXT, ALIGNED_VERT_TO);
            LOG_OPTS_R11 (TEXT, UNKNOWN_512);
            LOG_OPTS_R11 (TEXT, UNKNOWN_1024);
            LOG_OPTS_R11 (TEXT, UNKNOWN_2048);
            LOG_OPTS_R11 (TEXT, UNKNOWN_4096);
            LOG_OPTS_R11 (TEXT, UNKNOWN_8192);
            LOG_OPTS_R11 (TEXT, UNKNOWN_16384);
            LOG_OPTS_R11 (TEXT, UNKNOWN_32768);

            LOG_OPTS_R11 (VERTEX_2D, HAS_START_WIDTH);
            LOG_OPTS_R11 (VERTEX_2D, HAS_END_WIDTH);
            LOG_OPTS_R11 (VERTEX_2D, HAS_BULGE);
            LOG_OPTS_R11 (VERTEX_2D, HAS_VERTEX_FLAG);
            LOG_OPTS_R11 (VERTEX_2D, HAS_TANGENT_DIR);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_32);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_64);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_128);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_256);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_512);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_1024);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_2048);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_4096);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_8192);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_16384);
            LOG_OPTS_R11 (VERTEX_2D, UNKNOWN_32768);

            LOG_TRACE ("\n");
#undef LOG_OPTS_R11
          }
#endif
      if (R11FLAG (FLAG_R11_COLOR)) // 1
        FIELD_RCd (color_r11, 0);
      if (R11FLAG (FLAG_R11_LTYPE)) { // 2
        PRE (R_11) {
          FIELD_HANDLE (ltype, 1, 6);
        }
        LATER_VERSIONS {
          FIELD_HANDLE (ltype, 2, 6);
        }
      }

      // TODO: maybe move that to the entity
      PRE (R_10) { // XXX Check precise version
        if (R11FLAG (FLAG_R11_ELEVATION)) // 4
          FIELD_RD (elevation_r11, 38);
      } LATER_VERSIONS {
        if (R11FLAG (FLAG_R11_ELEVATION) // 4
            // 1 = LINE, 2 = POINT, 22 = 3DFACE
            && obj->type != 1 && obj->type != 2 && obj->type != 22)
          FIELD_RD (elevation_r11, 38);
      }
      if (R11FLAG (FLAG_R11_THICKNESS)) // 8
        FIELD_RD (thickness_r11, 39);
      if (R11FLAG (FLAG_R11_PSPACE)) { // 64
#ifdef IS_DECODER
        _ent->entmode = 1;
#endif
        DXF { VALUE_RC (1, 67); }
        if (obj->type != DWG_TYPE_VIEWPORT_R11) {
          // which viewport?
          FIELD_HANDLE (viewport, 1, 0);
        } else {
          FIELD_RSd (extra_r11, 0); // viewport index?
          // ??
        }
      }
      if (R11FLAG (FLAG_R11_HANDLING)) { // 32
#ifdef IS_DXF
        VALUE_H (obj->handle.value, 5);
#elif defined IS_JSON
        KEY (size); VALUE_H (obj->handle, 5);
#else
        VALUE_H (obj->handle, 5);
#endif
      }
      // some unknown pspace RS
      if (R11FLAG (FLAG_R11_PSPACE) && obj->type != DWG_TYPE_VIEWPORT_R11) {
        FIELD_RSd (extra_r11, 0); // unknown
      }
    }
  }

  // p20.4.1
  SINCE (R_13b1) {
    FIELD_B (preview_exists, 0);
    if (_ent->preview_exists)
      {
        // was DXF 160 (used for block previews?).
        // 92 also for all PROXY vector preview data with klass->is_zombie
#ifdef IS_DECODER
        if (obj->klass && obj->klass->is_zombie)
          _ent->preview_is_proxy = 1;
#endif
#if defined(IS_JSON)
        FIELD_B (preview_is_proxy, 0);
        FIELD_BLL (preview_size, 0);
#else
        VERSIONS (R_13b1, R_2007)
          {
            FIELD_CAST (preview_size, RL, BLL, 92);
          }
        SINCE (R_2010)
          {
            FIELD_BLL (preview_size, 160);
          }
#endif
        if ((int)_ent->preview_size >= 0 && _ent->preview_size < 210210)
          {
            FIELD_BINARY (preview, _ent->preview_size, 310);
          }
#ifndef IS_FREE
        else
          {
            LOG_ERROR ("Invalid preview_size: %lu kB",
                      (unsigned long)(_ent->preview_size / 1000));
            error |= DWG_ERR_VALUEOUTOFBOUNDS;
          }
#endif
      }
  }

  VERSIONS (R_13b1, R_14)
    {
#ifdef IS_DECODER
      obj->bitsize_pos = bit_position (dat);
      obj->bitsize = bit_read_RL (dat); // until the handles
#endif
#ifdef IS_ENCODER
      obj->bitsize_pos = bit_position (dat);
      bit_write_RL (dat, obj->bitsize);
#endif
#ifndef IS_FREE
      LOG_TRACE ("bitsize: " FORMAT_BL " @%lu.%u\n", obj->bitsize,
                 dat->byte, dat->bit)
#endif
#ifdef IS_DECODER
    if (obj->bitsize > obj->size * 8)
      {
        LOG_ERROR ("Invalid bitsize " FORMAT_RL " => " FORMAT_RL, obj->bitsize,
                   obj->size * 8);
        obj->bitsize = obj->size * 8;
        error |= DWG_ERR_VALUEOUTOFBOUNDS;
      }
    else
      error |= obj_handle_stream (dat, obj, hdl_dat);
#endif
    }

  SINCE (R_13b1) {
    // TODO: r13-r14: 6B flags + 6B common params
    FIELD_BB (entmode, 0);
    FIELD_BL (num_reactors, 0); //ODA bug: BB as BS
  }

  VERSIONS (R_13b1, R_14) //ODA bug
    {
      FIELD_B (isbylayerlt, 0);
#ifdef IS_DECODER
      if (FIELD_VALUE (isbylayerlt))
        FIELD_VALUE (ltype_flags) = FIELD_VALUE (isbylayerlt) ? 0 : 3;
#endif
    }
  SINCE (R_2004) //ODA bug
    {
      FIELD_B (is_xdic_missing, 0);
    }
  VERSIONS (R_13b1, R_2002) //ODA bug
    {
      FIELD_B (nolinks, 0)
    }
  SINCE (R_2013)
    {
      FIELD_B (has_ds_data, 0);
#ifdef IS_DECODER
      if (_obj->has_ds_data && dwg_obj_is_3dsolid (obj))
        {
          // not global, needs to be freed individually. (SHIFT_HV)
          BITCODE_H ref = dwg_add_handleref_free (5, obj->handle.value);
          PUSH_HV (obj->parent, num_acis_sab_hdl, acis_sab_hdl, ref);
        }
#endif
    }

  // no ENC type as it's only used once, here, and we would need to write
  // handlers for all 3 importers, and 3 exporters.
  SINCE (R_2004) // ENC (entity color encoding)
    {
      BITCODE_BS flags;
#ifdef IS_JSON
      field_cmc (dat, "color", &_ent->color);
#else
      DXF {
        // 0: byblock
        if (_ent->color.index != 256) // not bylayer
          FIELD_BS (color.index, 62);
      } else {
        FIELD_BSx (color.raw, 0);
      }
#ifdef IS_DECODER
      _ent->color.flag = _ent->color.raw >> 8;
      _ent->color.index = _ent->color.raw & 0x1ff; // 256 / -1 needed for ByLayer
      _ent->color.rgb = 0L;
      if (_ent->color.index != _ent->color.raw)
        LOG_TRACE (" color.index: %d [ENC 62]\n", _ent->color.index);
      if (_ent->color.flag > 1)
        LOG_TRACE (" color.flag: 0x%x\n", _ent->color.flag);
#endif
      flags = _ent->color.flag;
      if (flags & 0x20)
        {
#ifndef IS_DXF
          FIELD_BLx (color.alpha, 0);
#endif
#ifdef IS_DECODER
          /* 0 BYLAYER, 1 BYBLOCK, 3 alpha */
          _ent->color.alpha_type = _ent->color.alpha >> 24;
          _ent->color.alpha = _ent->color.alpha & 0xFF;
          LOG_TRACE (" color.alpha_type: %d [ENC 440]\n", _ent->color.alpha_type);
#endif
          JSON {
            FIELD_BB (color.alpha_type, 0);
          }
        }
      if (flags & 0x40)
        {
          FIELD_HANDLE (color.handle, 0, 430); // DBCOLOR 1E9F74 => 1F05B9
        }
      else if (flags & 0x80) // and not a reference
        {
          DXF {
            VALUE_BL (_ent->color.rgb & 0x00ffffff, 420);
          } else {
            FIELD_BLx (color.rgb, 420); // ODA bug, documented as BS
          }
        }
      if ((flags & 0x41) == 0x41)
        {
          FIELD_TV (color.name, 430);
        }
      if ((flags & 0x42) == 0x42)
        {
          FIELD_TV (color.book_name, 430);
        }
      DXF {
        if (flags & 0x20 && _ent->color.alpha_type == 3) {
          FIELD_BL (color.alpha, 440);
        }
      }
#endif
    }
  OTHER_VERSIONS {
    DXF {
      if (_ent->color.index != 256) // not bylayer
        FIELD_BS (color.index, 62);
    }
    else {
      SINCE (R_13b1)
        FIELD_CMC (color, 62);
    }
  }

#ifndef IS_DXF
  SINCE (R_13b1)
    FIELD_BD1 (ltype_scale, 48);
#endif
  SINCE (R_2000)
    {
      // 00 BYLAYER, 01 BYBLOCK, 10 CONTINUOUS, 11 ltype handle
      FIELD_BB (ltype_flags, 0);
      // 00 BYLAYER, 01 BYBLOCK, 10 CONTINUOUS, 11 plotstyle handle
      FIELD_BB (plotstyle_flags, 0);
    }
  SINCE (R_2007)
    {
      FIELD_BB (material_flags, 0); //if not BYLAYER 00: 347 material handle
      DXF {
        if (FIELD_VALUE (material_flags))
          FIELD_HANDLE (material, 0, 347)
      }
      FIELD_RC0 (shadow_flags, 284); /* r2007+: 0 both, 1 receives, 2 casts, 3 no */
    }
  SINCE (R_2010)
    {
      FIELD_B (has_full_visualstyle, 0); // DXF?
      FIELD_B (has_face_visualstyle, 0);
      FIELD_B (has_edge_visualstyle, 0);
    }

  DXF {
    if (FIELD_VALUE (invisible) & 1) { // skip bit 1
      VALUE_BS (1, 60)
    }
  } else {
    SINCE (R_13b1) {
      FIELD_BS (invisible, 60); //bit 0: 0 visible, 1 invisible
    }
  }

  SINCE (R_2000) {
    // DXF later after 6, see common_entity_handle_data.spec
    // Ideally CMC 60 should be deferred after layer 8, before linewt 370 also
#ifndef IS_DXF
    FIELD_RC (linewt, 370);
#endif
  }

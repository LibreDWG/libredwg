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
 * common_entity_data.spec: common entity non-handle specification
 * written by Reini Urban
 */

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
#  define LOG_FLAG_R11(w)                                                     \
    if (R11FLAG (FLAG_R11_##w))                                               \
      LOG_TRACE (#w "(0x%x) ", FLAG_R11_##w)
#  define LOG_FLAG_R11_MAX(v)                                                 \
    if (_obj->flag_r11 > v)                                                   \
      LOG_WARN ("Unknown flag_r11 (0x%x)", _obj->flag_r11 & ~255)
            LOG_FLAG_R11 (HAS_COLOR);
            LOG_FLAG_R11 (HAS_LTYPE);
            LOG_FLAG_R11 (HAS_ELEVATION);
            LOG_FLAG_R11 (HAS_THICKNESS);
            LOG_FLAG_R11 (HAS_UNKNOWN_16);
            LOG_FLAG_R11 (HAS_HANDLING);
            LOG_FLAG_R11 (HAS_PSPACE);
            LOG_FLAG_R11 (HAS_ATTRIBS);
            LOG_FLAG_R11_MAX (255);
            LOG_TRACE ("\n");
#  undef LOG_FLAG_R11
#  undef LOG_FLAG_R11_MAX
          }
        obj->size = bit_read_RS (dat);
#elif defined IS_ENCODER
        bit_write_RS (dat, obj->size);
#elif defined IS_JSON
        //KEY (size); VALUE_RS (obj->size, 0);
#endif
        DECODER_OR_ENCODER {
          LOG_TRACE("size: %d [RS]", obj->size);
          LOG_POS
        }
        if (obj->type != DWG_TYPE_JUMP_r11)
          FIELD_HANDLE (layer, 2, 8);
      }
      if (obj->type != DWG_TYPE_JUMP_r11)
        {
          FIELD_RSx (opts_r11, 0); // i.e. dataflags
#ifdef IS_DECODER
          if (_obj->opts_r11)
            {
              LOG_TRACE ("          ");
              // stringify the flag bits
#  define LOG_OPTS_R11(e, w)                                                  \
    if (obj->type == DWG_TYPE_##e##_r11 && (R11OPTS (OPTS_R11_##e##_##w)))    \
      LOG_TRACE (#w "(0x%x) ", OPTS_R11_##e##_##w)
#  define LOG_OPTS_R11_MAX(e, v)                                              \
    if (obj->type == DWG_TYPE_##e##_r11 && _obj->opts_r11 > v)                \
        LOG_WARN ("Unknown " #e " opts_r11 (0x%x)", _obj->opts_r11 & ~(v))
#  define LOG_OPTS_R11_POLYLINE(w)                                            \
    if (obj->type == DWG_TYPE_POLYLINE_r11                                    \
        && (R11OPTS (OPTS_R11_POLYLINE_##w)))                                 \
    LOG_TRACE (#w "(0x%x) ", OPTS_R11_POLYLINE_##w)
#  define LOG_OPTS_R11_POLYLINE_MAX(v)                                        \
    if (obj->type == DWG_TYPE_POLYLINE_r11                                    \
        && _obj->opts_r11 > v)                                                \
    LOG_WARN ("Unknown POLYLINE opts_r11 (0x%x)", _obj->opts_r11 & ~(v))

              LOG_OPTS_R11 (3DFACE, HAS_Z_FIRST);
              LOG_OPTS_R11 (3DFACE, HAS_Z_SECOND);
              LOG_OPTS_R11 (3DFACE, HAS_Z_THIRD);
              LOG_OPTS_R11 (3DFACE, HAS_Z_FOURTH);
              LOG_OPTS_R11_MAX (3DFACE, 15);

              LOG_OPTS_R11 (3DLINE, HAS_Z_FIRST);
              LOG_OPTS_R11 (3DLINE, HAS_Z_SECOND);
              LOG_OPTS_R11_MAX (3DLINE, 3);

              LOG_OPTS_R11 (ARC, HAS_EXTRUSION);
              LOG_OPTS_R11_MAX (ARC, 1);

              LOG_OPTS_R11 (ATTDEF, UNKNOWN_1);
              LOG_OPTS_R11 (ATTDEF, HAS_ROTATION);
              LOG_OPTS_R11 (ATTDEF, HAS_WIDTH_FACTOR);
              LOG_OPTS_R11 (ATTDEF, HAS_OBLIQUE_ANGLE);
              LOG_OPTS_R11 (ATTDEF, HAS_STYLE);
              LOG_OPTS_R11 (ATTDEF, HAS_GENERATION);
              LOG_OPTS_R11 (ATTDEF, HAS_HORIZ_ALIGNMENT);
              LOG_OPTS_R11 (ATTDEF, HAS_ALIGNMENT_POINT);
              LOG_OPTS_R11 (ATTDEF, HAS_EXTRUSION);
              LOG_OPTS_R11 (ATTDEF, HAS_VERT_ALIGNMENT);
              LOG_OPTS_R11_MAX (ATTRIB, 1023);

              LOG_OPTS_R11 (ATTRIB, UNKNOWN_1);
              LOG_OPTS_R11 (ATTRIB, HAS_ROTATION);
              LOG_OPTS_R11 (ATTRIB, HAS_WIDTH_FACTOR);
              LOG_OPTS_R11 (ATTRIB, HAS_OBLIQUE_ANGLE);
              LOG_OPTS_R11 (ATTRIB, HAS_STYLE);
              LOG_OPTS_R11 (ATTRIB, HAS_GENERATION);
              LOG_OPTS_R11 (ATTRIB, HAS_HORIZ_ALIGNMENT);
              LOG_OPTS_R11 (ATTRIB, HAS_ALIGNMENT_POINT);
              LOG_OPTS_R11 (ATTRIB, HAS_EXTRUSION);
              LOG_OPTS_R11 (ATTRIB, HAS_VERT_ALIGNMENT);
              LOG_OPTS_R11_MAX (ATTRIB, 1023);

              LOG_OPTS_R11 (BLOCK, UNKNOWN_1);
              LOG_OPTS_R11 (BLOCK, HAS_XREF_PNAME);
              LOG_OPTS_R11 (BLOCK, HAS_BLOCK_NAME);
              LOG_OPTS_R11_MAX (BLOCK, 7);

              LOG_OPTS_R11 (CIRCLE, HAS_EXTRUSION);
              LOG_OPTS_R11_MAX (CIRCLE, 1);

              LOG_OPTS_R11 (DIMENSION, HAS_DXF12);
              LOG_OPTS_R11 (DIMENSION, HAS_FLAG);
              LOG_OPTS_R11 (DIMENSION, HAS_TEXT);
              LOG_OPTS_R11 (DIMENSION, HAS_DXF13);
              LOG_OPTS_R11 (DIMENSION, HAS_DXF14);
              LOG_OPTS_R11 (DIMENSION, HAS_DXF15);
              LOG_OPTS_R11 (DIMENSION, HAS_ANGLES);
              LOG_OPTS_R11 (DIMENSION, HAS_DXF40);
              LOG_OPTS_R11 (DIMENSION, HAS_ROTATION);
              LOG_OPTS_R11 (DIMENSION, UNKNOWN_512);
              LOG_OPTS_R11 (DIMENSION, HAS_DXF53);
              LOG_OPTS_R11 (DIMENSION, UNKNOWN_2048);
              LOG_OPTS_R11 (DIMENSION, UNKNOWN_4096);
              LOG_OPTS_R11 (DIMENSION, UNKNOWN_8192);
              LOG_OPTS_R11 (DIMENSION, HAS_EXTRUSION);
              LOG_OPTS_R11 (DIMENSION, HAS_DIMSTYLE);
              LOG_OPTS_R11_MAX (DIMENSION, 65535);

              LOG_OPTS_R11 (INSERT, HAS_SCALE_X);
              LOG_OPTS_R11 (INSERT, HAS_SCALE_Y);
              LOG_OPTS_R11 (INSERT, HAS_ROTATION);
              LOG_OPTS_R11 (INSERT, HAS_SCALE_Z);
              LOG_OPTS_R11 (INSERT, HAS_NUM_COLS);
              LOG_OPTS_R11 (INSERT, HAS_NUM_ROWS);
              LOG_OPTS_R11 (INSERT, HAS_COL_SPACING);
              LOG_OPTS_R11 (INSERT, HAS_ROW_SPACING);
              LOG_OPTS_R11 (INSERT, HAS_EXTRUSION);
              LOG_OPTS_R11_MAX (INSERT, 511);

              LOG_OPTS_R11 (LINE, HAS_EXTRUSION);
              LOG_OPTS_R11_MAX (LINE, 1);

              LOG_OPTS_R11 (POINT, HAS_EXTRUSION);
              LOG_OPTS_R11 (POINT, HAS_X_ANG);
              LOG_OPTS_R11_MAX (POINT, 3);

              LOG_OPTS_R11_POLYLINE (HAS_FLAG);
              LOG_OPTS_R11_POLYLINE (HAS_START_WIDTH);
              LOG_OPTS_R11_POLYLINE (HAS_END_WIDTH);
              LOG_OPTS_R11_POLYLINE (HAS_EXTRUSION);
              LOG_OPTS_R11_POLYLINE (HAS_M_VERTS);
              LOG_OPTS_R11_POLYLINE (HAS_N_VERTS);
              LOG_OPTS_R11_POLYLINE (HAS_M_DENSITY);
              LOG_OPTS_R11_POLYLINE (HAS_N_DENSITY);
              LOG_OPTS_R11_POLYLINE (HAS_CURVETYPE);
              LOG_OPTS_R11_POLYLINE (IN_EXTRA); // then layer is the extras_offset
              LOG_OPTS_R11_POLYLINE_MAX (OPTS_R11_POLYLINE_IN_EXTRA);

              LOG_OPTS_R11 (SHAPE, HAS_ROTATION);
              LOG_OPTS_R11 (SHAPE, HAS_LOAD_NUM);
              LOG_OPTS_R11 (SHAPE, HAS_WIDTH_FACTOR);
              LOG_OPTS_R11 (SHAPE, HAS_OBLIQUE_ANGLE);
              LOG_OPTS_R11_MAX (SHAPE, 15);

              LOG_OPTS_R11 (SOLID, HAS_EXTRUSION);
              LOG_OPTS_R11_MAX (SOLID, 1);

              LOG_OPTS_R11 (TEXT, HAS_ROTATION);
              LOG_OPTS_R11 (TEXT, HAS_WIDTH_FACTOR);
              LOG_OPTS_R11 (TEXT, HAS_OBLIQUE_ANGLE);
              LOG_OPTS_R11 (TEXT, HAS_STYLE);
              LOG_OPTS_R11 (TEXT, HAS_GENERATION);
              LOG_OPTS_R11 (TEXT, HAS_HORIZ_ALIGNMENT);
              LOG_OPTS_R11 (TEXT, HAS_ALIGNMENT_POINT);
              LOG_OPTS_R11 (TEXT, HAS_EXTRUSION);
              LOG_OPTS_R11 (TEXT, ALIGNED_VERT_TO);
              LOG_OPTS_R11_MAX (TEXT, 511);

              LOG_OPTS_R11 (VERTEX, HAS_START_WIDTH);
              LOG_OPTS_R11 (VERTEX, HAS_END_WIDTH);
              LOG_OPTS_R11 (VERTEX, HAS_BULGE);
              LOG_OPTS_R11 (VERTEX, HAS_FLAG);
              LOG_OPTS_R11 (VERTEX, HAS_TANGENT_DIR);
              LOG_OPTS_R11 (VERTEX, HAS_INDEX1);
              LOG_OPTS_R11 (VERTEX, HAS_INDEX2);
              LOG_OPTS_R11 (VERTEX, HAS_INDEX3);
              LOG_OPTS_R11 (VERTEX, UNKNOWN_256);
              LOG_OPTS_R11 (VERTEX, HAS_INDEX4);
              LOG_OPTS_R11 (VERTEX, UNKNOWN_1024);
              LOG_OPTS_R11 (VERTEX, UNKNOWN_2048);
              LOG_OPTS_R11 (VERTEX, UNKNOWN_4096);
              LOG_OPTS_R11 (VERTEX, UNKNOWN_8192);
              LOG_OPTS_R11 (VERTEX, HAS_NOT_X_Y);
              LOG_OPTS_R11_MAX (VERTEX, 32704);

              LOG_TRACE ("\n");
#  undef LOG_OPTS_R11
#  undef LOG_OPTS_R11_MAX
            }
#endif
        }
      if (R11FLAG (FLAG_R11_HAS_PSPACE)) { // 16
#ifdef IS_DECODER
        _ent->entmode = 1;
#endif
        DXF { VALUE_RC (1, 67); }
        FIELD_RC (extra_r11, 0);
#ifdef IS_DECODER
        if (_obj->extra_r11)
          {
            LOG_TRACE ("           ");
#define LOG_EXTRA_R11(w)                                                      \
            if ((R11EXTRA (EXTRA_R11_##w)))  \
              LOG_TRACE (#w "(0x%x) ", EXTRA_R11_##w)

            LOG_EXTRA_R11 (UNKNOWN_1);
            LOG_EXTRA_R11 (HAS_EED);
            LOG_EXTRA_R11 (HAS_VIEWPORT);
            LOG_EXTRA_R11 (UNKNOWN_8);
            LOG_EXTRA_R11 (UNKNOWN_16);
            LOG_EXTRA_R11 (UNKNOWN_32);
            LOG_EXTRA_R11 (UNKNOWN_64);
            LOG_EXTRA_R11 (UNKNOWN_128);
            LOG_TRACE ("\n");
#undef LOG_EXTRA_R11
          }
#endif
      }
      if (R11EXTRA (EXTRA_R11_HAS_EED)) {
#ifdef IS_DECODER
        error |= dwg_decode_eed (dat, obj->tio.object);
#elif defined IS_ENCODER
        error |= dwg_encode_eed (dat, obj);
        // DXF, JSON and FREE handled elsewhere
#endif
      }
      if (R11FLAG (FLAG_R11_HAS_COLOR)) // 1
        FIELD_RCd (color_r11, 0);
      if (R11FLAG (FLAG_R11_HAS_LTYPE)) { // 2
        PRE (R_11) {
          FIELD_HANDLE (ltype, 1, 6);
        }
        LATER_VERSIONS {
          FIELD_HANDLE (ltype, 2, 6);
        }
      }

      // TODO: maybe move that to the entity
      PRE (R_10) { // XXX Check precise version
        if (R11FLAG (FLAG_R11_HAS_ELEVATION)) // 4
          FIELD_RD (elevation_r11, 38);
      } LATER_VERSIONS {
        // skip flag 4 for LINE, POINT, 3DFACE (also the deleted variants)
        if (R11FLAG (FLAG_R11_HAS_ELEVATION) // 4
            && obj->fixedtype != DWG_TYPE_LINE
            && obj->fixedtype != DWG_TYPE_POINT
            && obj->fixedtype != DWG_TYPE__3DFACE)
          FIELD_RD (elevation_r11, 38);
      }
      if (R11FLAG (FLAG_R11_HAS_THICKNESS)) // 8
        FIELD_RD (thickness_r11, 39);
      if (R11FLAG (FLAG_R11_HAS_HANDLING)) { // 32
#ifdef IS_DXF
        VALUE_H (obj->handle.value, 5);
#elif defined IS_JSON
        KEY (handle); VALUE_H (obj->handle, 5);
#else
        VALUE_H (obj->handle, 5);
#endif
      }
      if (R11EXTRA (EXTRA_R11_HAS_VIEWPORT)) {
        FIELD_HANDLE (viewport, 2, 0);
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
        if ((long)_ent->preview_size >= 0 && _ent->preview_size < 210210)
          {
            FIELD_BINARY (preview, _ent->preview_size, 310);
          }
#ifndef IS_FREE
        else
          {
            LOG_ERROR ("Invalid preview_size: " FORMAT_BLL " kB",
                      _ent->preview_size / 1000);
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
      LOG_TRACE ("bitsize: " FORMAT_BL " @%zu.%u\n", obj->bitsize,
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
#ifdef IS_ENCODER
          _ent->color.alpha_raw
              = _ent->color.alpha_type << 24 | _ent->color.alpha;
#endif
#ifndef IS_DXF
          FIELD_BLx (color.alpha_raw, 0);
#endif
#ifdef IS_DECODER
          /* 0 BYLAYER, 1 BYBLOCK, 3 alpha */
          _ent->color.alpha_type = _ent->color.alpha_raw >> 24;
          LOG_TRACE (" color.alpha_type: %d [ENC]\n", _ent->color.alpha_type);
          _ent->color.alpha = _ent->color.alpha_raw & 0xFF;
          LOG_TRACE (" color.alpha: %u [ENC 440]\n", _ent->color.alpha);
#endif
          JSON {
            FIELD_BL (color.alpha_raw, 0);
            FIELD_BB (color.alpha_type, 0);
            FIELD_RC (color.alpha, 0);
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

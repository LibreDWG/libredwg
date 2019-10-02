/* -*- c -*- */

  #include "spec.h"
  //Dwg_Object_Entity* _obj = ent;

  {
    // unsigned long pos = bit_position(dat);
    FIELD_B (preview_exists, 0);
    if (ent->preview_exists)
      {
        VERSIONS(R_13, R_2007)
          {
            // or DXF 92 for all PROXY vector preview data with classes
            FIELD_CAST (preview_size, RL, BLL, 160);
          }
        SINCE(R_2010)
          {
            FIELD_BLL (preview_size, 160);
          }
        if ((int)ent->preview_size >= 0 && ent->preview_size < 210210)
          {
            FIELD_BINARY (preview, ent->preview_size, 310);
          }
#ifndef IS_FREE
        else
          {
            LOG_ERROR("Invalid preview_size: %lu kB",
                      (unsigned long)(ent->preview_size / 1000));
            //bit_set_position(dat, pos+1);
            error |= DWG_ERR_VALUEOUTOFBOUNDS;
          }
#endif
      }
  }

  VERSIONS(R_13, R_14)
    {
#ifdef IS_DECODER
      obj->bitsize = bit_read_RL (dat); // until the handles
#endif
#ifdef IS_ENCODER
      bit_write_RL (dat, obj->bitsize);
#endif
      LOG_TRACE("bitsize: " FORMAT_BL " @%lu.%u\n", obj->bitsize,
                dat->byte - 4, dat->bit)
    }

  // TODO: r13-r14: 6B flags + 6B common params
  FIELD_BB (entmode, 0);
  // TODO: 2 more BB's
  FIELD_BL (num_reactors, 0); //ODA bug: BB as BS

  VERSIONS(R_13, R_14) //ODA bug
    {
      FIELD_B (isbylayerlt, 0);
      if (FIELD_VALUE(isbylayerlt))
        FIELD_VALUE(ltype_flags) = FIELD_VALUE(isbylayerlt) ? 0 : 3;
    }
  SINCE(R_2004) //ODA bug
    {
      FIELD_B (xdic_missing_flag, 0);
    }
  PRE(R_2004) //ODA bug
    {
      FIELD_B (nolinks, 0)
    }
  SINCE(R_2013)
    {
      FIELD_B (has_ds_binary_data, 0)
    }

  // TODO:
  // group 92 num_proxydata
  // group 310 proxydata
  // color as FIELD_ENC (DXF, ENCODE)

  SINCE(R_2004) // ENC (entity color encoding)
    {
#if 0 /* FIXME */
      //FIELD_CMC(color, 62,420);
      FIELD_ENC(color,62,420); // in ODA as CMC(B)
#else
      BITCODE_BS flags;
      FIELD_BSx (color.flag, 0);
      flags = ent->color.flag >> 8;
      DECODER {
        ent->color.rgb = 0L;
        ent->color.index = ent->color.flag & 0x1ff; // or 0xff?
        ent->color.flag = flags;
        LOG_HANDLE(" color.index: %d [ENC 62]\n", ent->color.index);
      }
      DXF {
        // 0: byblock
        if (FIELD_VALUE(color.index) != 256) // not bylayer
          FIELD_BS (color.index, 62);
      }

      if (flags & 0x40 && dat->version < R_2007)
        { // r2004+ in handle stream
          FIELD_HANDLE(color.handle, 0, 430); // DBCOLOR 1E9F74 => 1F05B9
        }
      if (flags & 0x20)
        {
          int type;
          BITCODE_BL alpha;
#ifndef IS_DXF
          FIELD_BL (color.alpha, 0);
#endif
          /* 0 BYLAYER, 1 BYBLOCK, 3 alpha */
          ent->color.alpha_type = ent->color.alpha & 0xff;
          alpha = ent->color.alpha >> 8;
          //LOG_HANDLE("alpha: %06x [BL 0]\n", ent->color.alpha);
#ifdef IS_DECODER
          LOG_TRACE(" color.alpha_type: %d\n", ent->color.alpha_type);
#endif
          if (ent->color.alpha_type == 3) {
#ifdef IS_ENCODER
            DXF { FIELD_BL (color.alpha >> 8, 430); }
#else
            DXF { FIELD_BL (color.alpha, 430); }
#endif
            //else LOG_TRACE("color.alpha: %d [ENC 430]\n", ent->color.alpha >> 8);
          }
        }
      if (flags & 0x80 && !(flags & 0x40)) // and not a reference
        {
          int type;
          FIELD_BL (color.rgb, 0); //ODA bug, documented as BS
          type = ent->color.rgb & 0xff;
#ifdef IS_DECODER
          LOG_TRACE("color.rgb: %06x [ENC.BL 420] (%d)\n", (unsigned)ent->color.rgb,
                    (int32_t)(ent->color.rgb & 0x00ffffff));
#elif defined(IS_ENCODER) && defined(IS_DXF)
          DXF { FIELD_BL (color.rgb, 420); }
#elif defined(IS_DXF)
          DXF { FIELD_BL (color.rgb & 0x00ffffff, 420); }
#endif
        }
      /* not with entities, only with CMC or dbcolor handle */
      if ((flags & 0x41) == 0x41)
        {
          FIELD_TV (color.name, 430);
          //ent->color.name = bit_read_TV(dat);
          //LOG_TRACE("color.name: %s [ENC.TV 430]\n", ent->color.name);
        }
      if ((flags & 0x42) == 0x42)
        {
          FIELD_TV (color.book_name, 430);
          //ent->color.book_name = bit_read_TV(dat);
          //LOG_TRACE("color.book_name: %s [ENC.TV 430]\n", ent->color.book_name);
        }
#if !defined(IS_DECODER) && !defined(IS_FREE)
      if (!(flags & 0xf0))
        {
          LOG_TRACE("color.index: %u [ENC 62]\n", (unsigned)ent->color.index);
        }
#endif
#endif
    }
  OTHER_VERSIONS
    FIELD_CMC(color, 62,420);

  DXF {
    if (FIELD_VALUE(ltype_scale) != 1.0)
      FIELD_BD (ltype_scale, 48);
  } else {
    FIELD_BD (ltype_scale, 48);
  }
  SINCE(R_2000)
    {
      // 00 BYLAYER, 01 BYBLOCK, 10 CONTINUOUS, 11 ltype handle
      FIELD_BB (ltype_flags, 0);
      // 00 BYLAYER, 01 BYBLOCK, 10 CONTINUOUS, 11 plotstyle handle
      FIELD_BB (plotstyle_flags, 0);
    }
  SINCE(R_2007)
    {
      FIELD_BB (material_flags, 0); //if not BYLAYER 00: 347 material handle
      DXF {
        if (FIELD_VALUE(material_flags))
          FIELD_HANDLE (material, 0, 347)
      }
      FIELD_RC (shadow_flags, 284); /* r2007+: 0 both, 1 casts, 2, receives, 3 no */
    }
  SINCE(R_2010)
    {
      FIELD_B (has_full_visualstyle, 0); // DXF?
      FIELD_B (has_face_visualstyle, 0);
      FIELD_B (has_edge_visualstyle, 0);
    }

  DXF {
    if (FIELD_VALUE(invisible))
      FIELD_BS (invisible, 60);
  } else {
    FIELD_BS (invisible, 60); //bit 0: 0 visible, 1 invisible
  }

  SINCE(R_2000) {
    DXF_OR_PRINT {
      if (FIELD_VALUE(linewt) != 29) {
        int lw = dxf_cvt_lweight (FIELD_VALUE(linewt));
        KEY(linewt); VALUE_RC((signed char)lw, 370);
      }
    } else {
      FIELD_RC (linewt, 370);
    }
  }


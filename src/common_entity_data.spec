/* -*- c -*- */

  #include "spec.h"
  //Dwg_Object_Entity* _obj = ent;

  {
    unsigned long pos = bit_position(dat);
    // no picture for TABLE, ...??
    FIELD_B (picture_exists, 0);
    if (ent->picture_exists)
      {
        VERSIONS(R_13, R_2007)
          {
            ent->picture_size = (BITCODE_BLL)bit_read_RL(dat);
          }
        SINCE(R_2010)
          {
            ent->picture_size = bit_read_BLL(dat);
          }
        LOG_TRACE("picture_size: " FORMAT_BLL " \n", ent->picture_size);
        if (ent->picture_size < 210210)
          {
            if (ent->picture_size)
              ent->picture = bit_read_TF(dat, ent->picture_size); // DXF 310 BINARY
          }
        else
          {
            LOG_ERROR("Invalid picture-size: %lu kB. Object: %lX (handle)",
                      (unsigned long)(ent->picture_size / 1000), obj->handle.value);
            bit_set_position(dat, pos);
            error |= DWG_ERR_VALUEOUTOFBOUNDS;
          }
      }
  }

  VERSIONS(R_13, R_14)
    {
      obj->bitsize = bit_read_RL(dat); // until the handles
      LOG_TRACE("Entity bitsize: " FORMAT_BL " @%lu.%u\n", obj->bitsize,
                dat->byte, dat->bit)
    }

  FIELD_BB (entity_mode, 0);  //ODA bug
  FIELD_BL (num_reactors, 0); //ODA bug

  VERSIONS(R_13, R_14) //ODA bug
    {
      FIELD_B (isbylayerlt, 0);
      if (FIELD_VALUE(isbylayerlt))
        FIELD_VALUE(linetype_flags) = FIELD_VALUE(isbylayerlt) ? 0 : 3;
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

  SINCE(R_2004) // ENC (entity color encoding)
    {
      BITCODE_BS flags = bit_read_BS(dat);
      LOG_HANDLE("color flags: 0x%X [BS 0]\n", (unsigned)flags);
      ent->color.rgb = 0L;
      ent->color.index = flags && 0xff; // or 0x1ff?
      flags = flags >> 8;
      ent->color.flag = flags;

      if (flags & 0x40)
        {
          LOG_HANDLE("color: has handle\n");
        }
      if (flags & 0x20)
        {
          BITCODE_BL trlong = bit_read_BL(dat);
          LOG_HANDLE("transparency: %x [BL 0]\n", trlong);
          ent->color.transparency_type = trlong & 0xff;
          ent->color.alpha = trlong >> 8;
          /* 0 BYLAYER, 1 BYBLOCK, 3 alpha */
          LOG_TRACE("color.transparency_type: %d\n", ent->color.transparency_type);
          if (ent->color.transparency_type == 3)
            LOG_TRACE("color.alpha: %d [BL 440]\n", ent->color.alpha);
        }
      if (flags & 0x80)
        {
          ent->color.rgb = bit_read_BL(dat); //ODA bug, documented as BS
          LOG_TRACE("color.rgb: %06X [BL 420]\n", (unsigned)ent->color.rgb);
        }
      /* not with entities, only with CMC  
      if (flags & 1)
        {
          ent->color.name = bit_read_TV(dat);
          LOG_TRACE("color.name: %s\n", ent->color.name);
        }
      if (flags & 2)
        {
          ent->color.book_name = bit_read_TV(dat);
          LOG_TRACE("color.book_name: %s\n", ent->color.book_name);
        }
      */
      if (!(flags & 0xf0))
        {
          LOG_TRACE("color.index: %u [62]\n", (unsigned)ent->color.index);
        }
    }
  OTHER_VERSIONS
    bit_read_CMC(dat, &ent->color);

  FIELD_BD (linetype_scale, 0);    
  SINCE(R_2000)
    {
      // 00 BYLAYER, 01 BYBLOCK, 10 CONTINUOUS, 11 ltype handle
      FIELD_BB (linetype_flags, 0);
      // 00 BYLAYER, 01 BYBLOCK, 10 CONTINUOUS, 11 plotstyle handle
      FIELD_BB (plotstyle_flags, 0);
    }
  SINCE(R_2007)
    {
      FIELD_BB (material_flags, 0);
      FIELD_RC (shadow_flags, 0);
    }
  SINCE(R_2010)
    {
      FIELD_B (has_full_visualstyle, 0);
      FIELD_B (has_face_visualstyle, 0);
      FIELD_B (has_edge_visualstyle, 0);
    }

  FIELD_BS (invisible, 0); //bit 0: 0 visible, 1 invisible

  SINCE(R_2000)
    {
      FIELD_RCu (lineweight, 0);
    }

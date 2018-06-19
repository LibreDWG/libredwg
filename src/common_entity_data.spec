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

  FIELD_BB (entity_mode, 0);
  FIELD_BL (num_reactors, 0);

  SINCE(R_2004)
    {
      FIELD_B (xdic_missing_flag, 0);
    }
  SINCE(R_2013)
    {
      FIELD_B (has_ds_binary_data, 0);
    }
  VERSIONS(R_13, R_14)
    {
      FIELD_B (isbylayerlt, 0);
    }

  FIELD_B (nolinks, 0);

  SINCE(R_2004)
    {
      BITCODE_B color_indexed;
      unsigned int color_flags;

      if (FIELD_VALUE(nolinks) == 0)
        {
          color_indexed = bit_read_B(dat);
          LOG_TRACE("color_indexed: " FORMAT_B " \n", color_indexed) //indexed or rgb

            if (color_indexed)
              {
                ent->color.index = bit_read_RC(dat);  // color index
                ent->color.rgb = 0L;
                LOG_TRACE("color.index: %u\n", (unsigned)ent->color.index);
              }
            else
              {
                color_flags = bit_read_RS(dat);
                LOG_TRACE("color.flags: 0x%X\n", (unsigned)color_flags);

                if (color_flags & 0x8000)
                  {
                    unsigned char c1, c2, c3, c4;
                    char *name;

                    c1 = bit_read_RC(dat);  // rgb color
                    c2 = bit_read_RC(dat);
                    c3 = bit_read_RC(dat);
                    c4 = bit_read_RC(dat);
                    name = bit_read_TV(dat);
                    ent->color.index = 0;
                    ent->color.rgb   = c1 << 24 | c2 << 16 | c3 << 8 | c4;
                    ent->color.name  = name;
                    LOG_TRACE("color.rgb: 0x%X\n", (unsigned)ent->color.rgb);
                    if (name && *name)
                      LOG_TRACE("color.name: %s\n", name);
                  }

                /*if (flags & 0x4000)
                  flags = flags;   // has AcDbColor reference (handle)
                */
                if (color_flags & 0x2000)
                  {
                    ent->color.transparency_type = bit_read_BL(dat);
                    LOG_TRACE("color.transparency_type: 0x%X\n",
                              (unsigned)ent->color.transparency_type);
                  }
              }
        }
      else
        {
          char color = bit_read_B(dat);
          LOG_TRACE("color.index: %d\n", (int)color)
            ent->color.index = color;
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
      FIELD_RC (lineweight, 0);
    }

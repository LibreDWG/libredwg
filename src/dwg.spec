DWG_ENTITY(UNUSED);

  COMMON_ENTITY_HANDLE_DATA;

DWG_ENTITY_END

/*(1)*/
DWG_ENTITY (TEXT);

  VERSIONS(R_13,R_14)
    {
      FIELD(elevation, BD);
      FIELD_2RD(insertion_pt);
      FIELD_2RD(alignment_pt);
      FIELD_3BD(extrusion);
      FIELD(thickness, BD);
      FIELD(oblique_ang, BD);
      FIELD(rotation_ang, BD);
      FIELD(height, BD);
      FIELD(width_factor, BD);
      FIELD(text_value, TV);
      FIELD(generation, BS);
      FIELD(horiz_alignment, BS);
      FIELD(vert_alignment, BS);
    }

  SINCE(R_2000)
    {
      FIELD(dataflags, RC);

      if (!(GET_FIELD(dataflags) & 0x01))
        {
          FIELD(elevation, RD);
        }

      FIELD_2RD(insertion_pt);

      if (!(GET_FIELD(dataflags) & 0x02))
        {
          FIELD_2DD(alignment_pt, 10, 20);
        }

      FIELD_BE(extrusion);
      FIELD(thickness, BT);

      if (!(GET_FIELD(dataflags) & 0x04))
        {
          FIELD(oblique_ang, RD);
        }

      if (!(GET_FIELD(dataflags) & 0x08))
        {
          FIELD(rotation_ang, RD);
        }

      FIELD(height, RD);

      if (!(GET_FIELD(dataflags) & 0x10))
        {
          FIELD(width_factor, RD);
        }

      FIELD(text_value, TV);

      if (!(GET_FIELD(dataflags) & 0x20))
        {
          FIELD(generation, BS);
        }

      if (!(GET_FIELD(dataflags) & 0x40)) 
        {
          FIELD(horiz_alignment, BS);
        }

      if (!(GET_FIELD(dataflags) & 0x80))
        {
          FIELD(vert_alignment, BS);
        }
    }

  COMMON_ENTITY_HANDLE_DATA;

  FIELD_HANDLE(style, 5);
DWG_ENTITY_END


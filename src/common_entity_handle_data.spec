/* -*- c -*- */

#undef IF_ENCODE_FROM_EARLIER
#define IF_ENCODE_FROM_EARLIER if (0)
#ifdef IS_DECODER
# define IF_IS_DECODER 1
# define IF_IS_ENCODER 0
#else
# define IF_IS_ENCODER 0
# define IF_IS_DECODER 1
#endif

  if (FIELD_VALUE(entity_mode) == 0
      || IF_IS_ENCODER)
    {
      FIELD_HANDLE(subentity, 3);
    }

  ENT_REACTORS(4)
  ENT_XDICOBJHANDLE(3)

  VERSIONS(R_13,R_14)
    {
      FIELD_HANDLE(layer, 5);
      if (!FIELD_VALUE(isbylayerlt))
        {
          FIELD_HANDLE(ltype, 5);
        }
    }

  VERSIONS(R_13,R_2000)
    {
      if (!FIELD_VALUE(nolinks))
        { //TODO: in R13, R14 these are optional. Look at page 53 in the spec
          //      for condition.
          FIELD_HANDLE(prev_entity, ANYCODE); // 4 or 8
          FIELD_HANDLE(next_entity, ANYCODE); // 4 or 6
        }
    }

  SINCE(R_2004)
    {
      FIELD_HANDLE(color_handle, ANYCODE);
    }

  SINCE(R_2000)
    {
      FIELD_HANDLE(layer, 5);
      if (FIELD_VALUE(linetype_flags)==3)
        {
          FIELD_HANDLE(ltype, 5);
        }
    }

  SINCE(R_2007)
    {
      if (FIELD_VALUE(material_flags)==3)
        {
          FIELD_HANDLE(material, ANYCODE);
        }
    }

  SINCE(R_2000)
    {
      if (FIELD_VALUE(plotstyle_flags)==3)
        {
          FIELD_HANDLE(plotstyle, 5);
        }
    }

  SINCE(R_2010)
    {
      if (FIELD_VALUE(has_full_visualstyle))
        {
          FIELD_HANDLE(full_visualstyle, ANYCODE);
        }
      if (FIELD_VALUE(has_face_visualstyle))
        {
          FIELD_HANDLE(face_visualstyle, ANYCODE);
        }
      if (FIELD_VALUE(has_edge_visualstyle))
        {
          FIELD_HANDLE(edge_visualstyle, ANYCODE);
        }
    }


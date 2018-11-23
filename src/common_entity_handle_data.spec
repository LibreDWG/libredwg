/* -*- c -*- */

  #include "spec.h"

  //free: avoid double-free #43
  if ((FIELD_VALUE(entity_mode) == 0 && !IF_IS_FREE) || IF_IS_ENCODER)
    {
      FIELD_HANDLE(subentity, 4, 0); // doc: owner ref always?
    }
  ENT_REACTORS(4)
  ENT_XDICOBJHANDLE(3)
  SUBCLASS(AcDbEntity)

  VERSIONS(R_13, R_14)
    {
      FIELD_HANDLE(layer, 5, 8);
      ENCODER {
        if (dat->from_version == R_2000)
          FIELD_VALUE(isbylayerlt) = FIELD_VALUE(linetype_flags) == 3 ? 1 : 0;
      }
#ifdef IS_DXF
      switch (FIELD_VALUE(linetype_flags)) {
      case 0: VALUE_TV("BYLAYER", 6); break;
      case 1: VALUE_TV("BYBLOCK", 6); break;
      case 3: VALUE_TV("CONTINUOUS", 6); break;
      default: break;
      }
#endif
      if (!FIELD_VALUE(isbylayerlt))
        FIELD_HANDLE(ltype, 5, 6);
    }

  VERSIONS(R_13, R_2000)
    {
      if (!FIELD_VALUE(nolinks))
        { //TODO: in R13, R14 these are optional. Look at page 53 in the spec
          //      for condition.
          FIELD_HANDLE(prev_entity, 4, 0);
          FIELD_HANDLE(next_entity, 4, 0);
        }
    }

  SINCE(R_2004)
    {
      if (FIELD_VALUE(color.flag) & 0x40) {
        FIELD_HANDLE(color_handle, 5, 0);
      }
    }

  VERSION(R_2000)
    {
      FIELD_HANDLE(layer, 5, 8);
#ifdef IS_DXF
      switch (FIELD_VALUE(linetype_flags)) {
      case 0: VALUE_TV("BYLAYER", 6); break;
      case 1: VALUE_TV("BYBLOCK", 6); break;
      case 3: VALUE_TV("CONTINUOUS", 6); break;
      default: break;
      }
#endif
      if (FIELD_VALUE(linetype_flags) == 3)
        FIELD_HANDLE(ltype, 5, 6);
    }

  SINCE(R_2007)
    {
      if (FIELD_VALUE(material_flags) == 3)
        FIELD_HANDLE(material, ANYCODE, 0); // 5 hard pointer?
      if (FIELD_VALUE(shadow_flags) == 3)
        FIELD_HANDLE(shadow, ANYCODE, 0); // 5 hard pointer?
    }

  SINCE(R_2000)
    {
      if (FIELD_VALUE(plotstyle_flags) == 3)
        FIELD_HANDLE(plotstyle, 5, 0);
    }

  SINCE(R_2010)
    {
      if (FIELD_VALUE(has_full_visualstyle))
        FIELD_HANDLE(full_visualstyle, 5, 0);
      if (FIELD_VALUE(has_face_visualstyle))
        FIELD_HANDLE(face_visualstyle, 5, 0);
      if (FIELD_VALUE(has_edge_visualstyle))
        FIELD_HANDLE(edge_visualstyle, 5, 0);
    }

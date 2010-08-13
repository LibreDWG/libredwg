#ifdef IS_DECODER
  if (FIELD_VALUE(entity_mode)==0)
    {
      FIELD_HANDLE(subentity, 3);
    }
#endif //IS_DECODER

#ifdef IS_ENCODER
  FIELD_HANDLE(subentity, 3);
#endif  //IS_ENCODER
  ENT_REACTORS(4)
  ENT_XDICOBJHANDLE(3)

  VERSIONS(R_13,R_14)
    {
      FIELD_HANDLE(layer, 5);
      if (!FIELD_VALUE(isbylayerlt))
        FIELD_HANDLE(ltype, 5);
    }

  VERSIONS(R_13,R_14)
    {
      if (!FIELD_VALUE(nolinks))
        { //TODO: in R13, R14 these are optional. Look at page 53 in the spec
          //      for condition.
          FIELD_HANDLE(prev_entity, 4);
          FIELD_HANDLE(next_entity, 4);
        }
    }

  SINCE(R_2000)
    {
      VERSION(R_2000)   // not in R2004
        {
          if (!FIELD_VALUE(nolinks))
            { //TODO: these are optional. see page 52
              FIELD_HANDLE(prev_entity, 4);
              FIELD_HANDLE(next_entity, 4);
            }
        }
    }

  SINCE(R_2000)
    {
      FIELD_HANDLE(layer, 5);
      if (FIELD_VALUE(linetype_flags)==3)
        {
          FIELD_HANDLE(ltype, 5);
        }
      if (FIELD_VALUE(plotstyle_flags)==3)
        {
          FIELD_HANDLE(plotstyle, 5);
        }
    }

  SINCE(R_2007)
    {
      if (FIELD_VALUE(material_flags)==3)
        {
          FIELD_HANDLE(material, ANYCODE);
        }
    }

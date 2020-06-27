// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKFLIPPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCK2PTPARAMETER_fields;
  Dwg_BLOCKPARAMETER_propinfo *props;
  BITCODE_T flip_label;
  BITCODE_T flip_label_desc;
  BITCODE_T base_state_label;
  BITCODE_T flipped_state_label;
  BITCODE_3BD def_label_pt;
  BITCODE_BL i, bl96;
  BITCODE_T tooltip;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockflipparameter *_obj = dwg_object_to_BLOCKFLIPPARAMETER (obj);

  CHK_EVALEXPR (BLOCKFLIPPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKFLIPPARAMETER, def_endpt);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, num_infos, BL);
  if (!dwg_dynapi_entity_value (_obj, "BLOCKFLIPPARAMETER", "infos",
                                &infos, NULL))
    fail ("BLOCKFLIPPARAMETER.infos");
  else
    for (i = 0; i < num_infos; i++)
      {
        CHK_SUBCLASS_TYPE (_obj->infos[i], BLOCKPARAMETER_info, num_props, BL);
        if (!dwg_dynapi_subclass_value (&_obj->infos[i], "BLOCKPARAMETER_info", "props",
                                        &props, NULL))
          fail ("BLOCKPARAMETER_info.props");
        else
          for (unsigned j = 0; j < _obj->infos[i].num_props; j++)
            {
              CHK_SUBCLASS_TYPE (props[j], "BLOCKPARAMETER_propinfo", propnum, BL);
              CHK_SUBCLASS_UTF8TEXT (props[j], "BLOCKPARAMETER_propinfo", proptext);
            }
      }
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKFLIPPARAMETER, bl_infos, num_infos, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, parameter_base_location, BS);

  // AcDbBlockFlipParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, flip_label);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, flip_label_desc);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, base_state_label);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, flipped_state_label);
  CHK_ENTITY_3RD  (_obj, BLOCKFLIPPARAMETER, def_label_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPPARAMETER, bl96, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPPARAMETER, tooltip);
#endif
}

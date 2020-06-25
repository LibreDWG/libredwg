// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKVERTICALCONSTRAINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKCONSTRAINTPARAMETER_fields;
  BITCODE_BL i;
  Dwg_BLOCKPARAMETER_propinfo *props;
  // ..
  BLOCKPARAMVALUESET_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockverticalconstraintparameter *_obj = dwg_object_to_BLOCKVERTICALCONSTRAINTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKVERTICALCONSTRAINTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, def_endpt);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, num_infos, BL);
  if (!dwg_dynapi_entity_value (_obj, "BLOCKVERTICALCONSTRAINTPARAMETER", "infos", &infos, NULL))
    fail ("BLOCKVERTICALCONSTRAINTPARAMETER.infos");
  else
    for (i = 0; i < num_infos; i++)
      {
        CHK_SUBCLASS_TYPE (_obj->infos[i], BLOCKPARAMETER_info, num_props, BL);
        if (!dwg_dynapi_subclass_value (&_obj->infos[i], "BLOCKPARAMETER_info", "props", &props, NULL))
          fail ("BLOCKPARAMETER_info.props");
        else
          for (unsigned j = 0; j < _obj->infos[i].num_props; j++)
            {
              CHK_SUBCLASS_TYPE (props[j], "BLOCKPARAMETER_propinfo", propnum, BL);
              CHK_SUBCLASS_UTF8TEXT (props[j], "BLOCKPARAMETER_propinfo", proptext);
            }
      }
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, bl_infos, num_infos, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, parameter_base_location, BS);
  // AcDbBlockConstraintParameter
  CHK_ENTITY_H (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, dependency);
#endif
}

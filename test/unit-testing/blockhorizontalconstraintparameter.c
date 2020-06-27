// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKHORIZONTALCONSTRAINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  Dwg_BLOCKPARAMETER_propinfo *props;
  BLOCKLINEARCONSTRAINTPARAMETER_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockhorizontalconstraintparameter *_obj = dwg_object_to_BLOCKHORIZONTALCONSTRAINTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKHORIZONTALCONSTRAINTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, def_endpt);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, num_infos, BL);
  if (!dwg_dynapi_entity_value (_obj, "BLOCKHORIZONTALCONSTRAINTPARAMETER", "infos",
                                &infos, NULL))
    fail ("BLOCKHORIZONTALCONSTRAINTPARAMETER.infos");
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
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, bl_infos, num_infos, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, parameter_base_location, BS);
  // AcDbBlockConstraintParameter
  CHK_ENTITY_H (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, dependency);
  // AcDbBlockLinearConstraintParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLINEARCONSTRAINTPARAMETER, expr_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLINEARCONSTRAINTPARAMETER, expr_description);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARCONSTRAINTPARAMETER, value, BD);
  // AcDbBlockParamValueSet
  CHK_SUBCLASS_UTF8TEXT (_obj->value_set, "BLOCKPARAMVALUESET", desc);
  CHK_SUBCLASS_TYPE (_obj->value_set, "BLOCKPARAMVALUESET", flags, BL);
  CHK_SUBCLASS_TYPE (_obj->value_set, "BLOCKPARAMVALUESET", minimum, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, "BLOCKPARAMVALUESET", maximum, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, "BLOCKPARAMVALUESET", increment, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, "BLOCKPARAMVALUESET", num_valuelist, BS);
  CHK_SUBCLASS_VECTOR_TYPE (_obj->value_set, "BLOCKPARAMVALUESET", valuelist,
                            _obj->value_set.num_valuelist, BD);
#endif
}

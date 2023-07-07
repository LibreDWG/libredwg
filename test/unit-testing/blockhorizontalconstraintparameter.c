#define DWG_TYPE DWG_TYPE_BLOCKHORIZONTALCONSTRAINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  Dwg_BLOCKPARAMETER_PropInfo *props;
  BLOCKLINEARCONSTRAINTPARAMETER_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockhorizontalconstraintparameter *_obj
      = dwg_object_to_BLOCKHORIZONTALCONSTRAINTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKHORIZONTALCONSTRAINTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, show_properties,
                   B);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
          CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER,
                                  prop_states, 4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER,
                   parameter_base_location, BS);
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, upd_basept);
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, basept);
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, upd_endpt);
  CHK_ENTITY_3RD (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, endpt);
  // AcDbBlockConstraintParameter
  CHK_ENTITY_H (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, dependency);
  // AcDbBlockLinearConstraintParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, expr_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER,
                       expr_description);
  CHK_ENTITY_TYPE (_obj, BLOCKHORIZONTALCONSTRAINTPARAMETER, value, BD);
  // AcDbBlockParamValueSet
  CHK_SUBCLASS_UTF8TEXT (_obj->value_set, BLOCKPARAMVALUESET, desc);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, flags, BL);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, minimum, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, maximum, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, increment, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, num_valuelist, BS);
  CHK_SUBCLASS_VECTOR_TYPE (_obj->value_set, BLOCKPARAMVALUESET, valuelist,
                            _obj->value_set.num_valuelist, BD);
  // #endif
}

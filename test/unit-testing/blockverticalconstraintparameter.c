#define DWG_TYPE DWG_TYPE_BLOCKVERTICALCONSTRAINTPARAMETER
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
  dwg_obj_blockverticalconstraintparameter *_obj
      = dwg_object_to_BLOCKVERTICALCONSTRAINTPARAMETER (obj);

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
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
          CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER,
                                  prop_states, 4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER,
                   parameter_base_location, BS);
  CHK_ENTITY_3RD (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, upd_basept);
  CHK_ENTITY_3RD (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, basept);
  CHK_ENTITY_3RD (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, upd_endpt);
  CHK_ENTITY_3RD (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, endpt);
  // AcDbBlockConstraintParameter
  CHK_ENTITY_H (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, dependency);
  // AcDbBlockLinearConstraintParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, expr_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVERTICALCONSTRAINTPARAMETER,
                       expr_description);
  CHK_ENTITY_TYPE (_obj, BLOCKVERTICALCONSTRAINTPARAMETER, value, BD);
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

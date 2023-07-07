#define DWG_TYPE DWG_TYPE_BLOCKDIAMETRICCONSTRAINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKCONSTRAINTPARAMETER_fields;
  BITCODE_BL i;
  BITCODE_T expr_name;
  BITCODE_T expr_description;
  BITCODE_BD distance;
  BLOCKPARAMVALUESET_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockdiametricconstraintparameter *_obj
      = dwg_object_to_BLOCKDIAMETRICCONSTRAINTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKDIAMETRICCONSTRAINTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, show_properties,
                   B);
  CHK_ENTITY_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
          CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER,
                                  prop_states, 4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER,
                   parameter_base_location, BS);
  CHK_ENTITY_3RD (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, upd_basept);
  CHK_ENTITY_3RD (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, basept);
  CHK_ENTITY_3RD (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, upd_endpt);
  CHK_ENTITY_3RD (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, endpt);
  // AcDbBlockConstraintParameter
  CHK_ENTITY_H (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, dependency);
  // AcDbBlockDiametricConstraintParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, expr_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER,
                       expr_description);
  CHK_ENTITY_TYPE (_obj, BLOCKDIAMETRICCONSTRAINTPARAMETER, distance, BD);
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

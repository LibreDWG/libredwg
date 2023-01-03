// looks stable
#define DWG_TYPE DWG_TYPE_BLOCKLINEARPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK2PTPARAMETER_fields;
  BITCODE_T distance_name;
  BITCODE_T distance_desc;
  BITCODE_BD distance;
  // Dwg_BLOCKPARAMVALUESET value_set;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blocklinearparameter *_obj
      = dwg_object_to_BLOCKLINEARPARAMETER (obj);

  CHK_EVALEXPR (BLOCKLINEARPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLINEARPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKLINEARPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKLINEARPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (
      prop2) BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
      CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKLINEARPARAMETER, prop_states, 4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARPARAMETER, parameter_base_location, BS);
  CHK_ENTITY_3RD (_obj, BLOCKLINEARPARAMETER, upd_basept);
  CHK_ENTITY_3RD (_obj, BLOCKLINEARPARAMETER, basept);
  CHK_ENTITY_3RD (_obj, BLOCKLINEARPARAMETER, upd_endpt);
  CHK_ENTITY_3RD (_obj, BLOCKLINEARPARAMETER, endpt);
  // AcDbBlockLinearParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLINEARPARAMETER, distance_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKLINEARPARAMETER, distance_desc);
  CHK_ENTITY_TYPE (_obj, BLOCKLINEARPARAMETER, distance, BD);
  // AcDbBlockParamValueSet
  CHK_SUBCLASS_UTF8TEXT (_obj->value_set, BLOCKPARAMVALUESET, desc);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, flags, BL);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, minimum, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, maximum, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, increment, BD);
  CHK_SUBCLASS_TYPE (_obj->value_set, BLOCKPARAMVALUESET, num_valuelist, BS);
  CHK_SUBCLASS_VECTOR_TYPE (_obj->value_set, BLOCKPARAMVALUESET, valuelist,
                            _obj->value_set.num_valuelist, BD);
}

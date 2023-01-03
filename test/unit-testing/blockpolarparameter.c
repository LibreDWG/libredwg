#define DWG_TYPE DWG_TYPE_BLOCKPOLARPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK2PTPARAMETER_fields;
  BITCODE_T angle_name;
  BITCODE_T angle_desc;
  BITCODE_T distance_name;
  BITCODE_T distance_desc;
  BITCODE_BD offset;
  Dwg_BLOCKPARAMVALUESET angle_value_set;
  Dwg_BLOCKPARAMVALUESET distance_value_set;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockpolarparameter *_obj = dwg_object_to_BLOCKPOLARPARAMETER (obj);

  CHK_EVALEXPR (BLOCKPOLARPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOLARPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKPOLARPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKPOLARPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (
      prop2) BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
      CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKPOLARPARAMETER, prop_states, 4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARPARAMETER, parameter_base_location, BS);
  // CHK_ENTITY_3RD  (_obj, BLOCKPOLARPARAMETER, upd_basept);
  // CHK_ENTITY_3RD  (_obj, BLOCKPOLARPARAMETER, basept);
  // CHK_ENTITY_3RD  (_obj, BLOCKPOLARPARAMETER, upd_endpt);
  // CHK_ENTITY_3RD  (_obj, BLOCKPOLARPARAMETER, endpt);
  //  AcDbBlockPolarParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOLARPARAMETER, angle_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOLARPARAMETER, angle_desc);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOLARPARAMETER, distance_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKPOLARPARAMETER, distance_desc);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARPARAMETER, offset, BD);
  // CHK_ENTITY_3BD (_obj, BLOCKPOLARPARAMETER, base_angle_pt);
  //  AcDbBlockParamValueSet
  CHK_SUBCLASS_UTF8TEXT (_obj->angle_value_set, BLOCKPARAMVALUESET, desc);
  CHK_SUBCLASS_TYPE (_obj->angle_value_set, BLOCKPARAMVALUESET, flags, BL);
  CHK_SUBCLASS_TYPE (_obj->angle_value_set, BLOCKPARAMVALUESET, minimum, BD);
  CHK_SUBCLASS_TYPE (_obj->angle_value_set, BLOCKPARAMVALUESET, maximum, BD);
  CHK_SUBCLASS_TYPE (_obj->angle_value_set, BLOCKPARAMVALUESET, increment, BD);
  CHK_SUBCLASS_TYPE (_obj->angle_value_set, BLOCKPARAMVALUESET, num_valuelist,
                     BS);
  CHK_SUBCLASS_VECTOR_TYPE (_obj->angle_value_set, BLOCKPARAMVALUESET,
                            valuelist, _obj->angle_value_set.num_valuelist,
                            BD);
  CHK_SUBCLASS_UTF8TEXT (_obj->distance_value_set, BLOCKPARAMVALUESET, desc);
  CHK_SUBCLASS_TYPE (_obj->distance_value_set, BLOCKPARAMVALUESET, flags, BL);
  CHK_SUBCLASS_TYPE (_obj->distance_value_set, BLOCKPARAMVALUESET, minimum,
                     BD);
  CHK_SUBCLASS_TYPE (_obj->distance_value_set, BLOCKPARAMVALUESET, maximum,
                     BD);
  CHK_SUBCLASS_TYPE (_obj->distance_value_set, BLOCKPARAMVALUESET, increment,
                     BD);
  CHK_SUBCLASS_TYPE (_obj->distance_value_set, BLOCKPARAMVALUESET,
                     num_valuelist, BS);
  CHK_SUBCLASS_VECTOR_TYPE (_obj->distance_value_set, BLOCKPARAMVALUESET,
                            valuelist, _obj->distance_value_set.num_valuelist,
                            BD);
}

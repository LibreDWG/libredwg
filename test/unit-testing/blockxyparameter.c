// looks stable
#define DWG_TYPE DWG_TYPE_BLOCKXYPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK2PTPARAMETER_fields;
  BITCODE_T x_label;
  BITCODE_T x_label_desc;
  BITCODE_T y_label;
  BITCODE_T y_label_desc;
  BITCODE_BD x_value;
  BITCODE_BD y_value;
  Dwg_BLOCKPARAMVALUESET x_value_set;
  Dwg_BLOCKPARAMVALUESET y_value_set;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockxyparameter *_obj = dwg_object_to_BLOCKXYPARAMETER (obj);

  CHK_EVALEXPR (BLOCKXYPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKXYPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKXYPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKXYPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
          CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKXYPARAMETER, prop_states, 4, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, parameter_base_location, BS);
  CHK_ENTITY_3RD (_obj, BLOCKXYPARAMETER, upd_basept);
  CHK_ENTITY_3RD (_obj, BLOCKXYPARAMETER, basept);
  CHK_ENTITY_3RD (_obj, BLOCKXYPARAMETER, upd_endpt);
  CHK_ENTITY_3RD (_obj, BLOCKXYPARAMETER, endpt);
  // AcDbBlockXYParameter
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKXYPARAMETER, x_label);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKXYPARAMETER, x_label_desc);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKXYPARAMETER, y_label);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKXYPARAMETER, y_label_desc);
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, x_value, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKXYPARAMETER, y_value, BD);
  // AcDbBlockParamValueSet
  CHK_SUBCLASS_UTF8TEXT (_obj->x_value_set, BLOCKPARAMVALUESET, desc);
  CHK_SUBCLASS_TYPE (_obj->x_value_set, BLOCKPARAMVALUESET, flags, BL);
  CHK_SUBCLASS_TYPE (_obj->x_value_set, BLOCKPARAMVALUESET, minimum, BD);
  CHK_SUBCLASS_TYPE (_obj->x_value_set, BLOCKPARAMVALUESET, maximum, BD);
  CHK_SUBCLASS_TYPE (_obj->x_value_set, BLOCKPARAMVALUESET, increment, BD);
  CHK_SUBCLASS_TYPE (_obj->x_value_set, BLOCKPARAMVALUESET, num_valuelist, BS);
  CHK_SUBCLASS_VECTOR_TYPE (_obj->x_value_set, BLOCKPARAMVALUESET, valuelist,
                            _obj->x_value_set.num_valuelist, BD);

  CHK_SUBCLASS_UTF8TEXT (_obj->y_value_set, BLOCKPARAMVALUESET, desc);
  CHK_SUBCLASS_TYPE (_obj->y_value_set, BLOCKPARAMVALUESET, flags, BL);
  CHK_SUBCLASS_TYPE (_obj->y_value_set, BLOCKPARAMVALUESET, minimum, BD);
  CHK_SUBCLASS_TYPE (_obj->y_value_set, BLOCKPARAMVALUESET, maximum, BD);
  CHK_SUBCLASS_TYPE (_obj->y_value_set, BLOCKPARAMVALUESET, increment, BD);
  CHK_SUBCLASS_TYPE (_obj->y_value_set, BLOCKPARAMVALUESET, num_valuelist, BS);
  CHK_SUBCLASS_VECTOR_TYPE (_obj->y_value_set, BLOCKPARAMVALUESET, valuelist,
                            _obj->y_value_set.num_valuelist, BD);
}

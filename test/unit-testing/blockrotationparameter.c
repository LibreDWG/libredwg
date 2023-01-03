// unstable
#define DWG_TYPE DWG_TYPE_BLOCKROTATIONPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCK2PTPARAMETER_fields;
  BITCODE_3BD def_base_angle_pt;
  BITCODE_T angle_name; /*!< DXF 305 */
  BITCODE_T angle_desc; /*!< DXF 306 */
  BITCODE_BD angle;     /*!< DXF 306 */
  // Dwg_BLOCKPARAMVALUESET value_set;
  BITCODE_3BD base_angle_pt;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockrotationparameter *_obj
      = dwg_object_to_BLOCKROTATIONPARAMETER (obj);

  CHK_EVALEXPR (BLOCKROTATIONPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKROTATIONPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, def_endpt);
  BLOCKPARAMETER_PropInfo (prop1) BLOCKPARAMETER_PropInfo (prop2)
      BLOCKPARAMETER_PropInfo (prop3) BLOCKPARAMETER_PropInfo (prop4)
          CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKROTATIONPARAMETER, prop_states, 4,
                                  BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONPARAMETER, parameter_base_location, BS);
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, upd_basept);
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, basept);
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, upd_endpt);
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, endpt);
  // AcDbBlockLinearParameter
  CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, def_base_angle_pt);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKROTATIONPARAMETER, angle_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKROTATIONPARAMETER, angle_desc);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATIONPARAMETER, angle, BD);
  // CHK_ENTITY_3RD (_obj, BLOCKROTATIONPARAMETER, base_angle_pt);
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
}

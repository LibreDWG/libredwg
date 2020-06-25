#define DWG_TYPE DWG_TYPE_BLOCKANGULARCONSTRAINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKCONSTRAINTPARAMETER_fields;
  BITCODE_BL i;
  Dwg_BLOCKPARAMETER_propinfo *props;
  BITCODE_3BD origin;
  BITCODE_3BD end_pt;
  BITCODE_T expr_name;
  BITCODE_T param_name;
  BITCODE_BD angle;
  BITCODE_B b280;
  BLOCKPARAMVALUESET_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockangularconstraintparameter *_obj = dwg_object_to_BLOCKANGULARCONSTRAINTPARAMETER (obj);

  CHK_EVALEXPR (BLOCKANGULARCONSTRAINTPARAMETER);
  // AcDbBlockElement
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKANGULARCONSTRAINTPARAMETER, name);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, be_major, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, be_minor, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, eed1071, BL);
  // AcDbBlockParameter
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, show_properties, B);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, chain_actions, B);
  // AcDbBlock2PtParameter
  CHK_ENTITY_3RD (_obj, BLOCKANGULARCONSTRAINTPARAMETER, def_basept);
  CHK_ENTITY_3RD (_obj, BLOCKANGULARCONSTRAINTPARAMETER, def_endpt);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, num_infos, BL);
  if (!dwg_dynapi_entity_value (_obj, "BLOCKANGULARCONSTRAINTPARAMETER", "infos",
                                &infos, NULL))
    fail ("BLOCKANGULARCONSTRAINTPARAMETER.infos");
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
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, bl_infos, num_infos, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, parameter_base_location, BS);
  // AcDbBlockConstraintParameter
  CHK_ENTITY_H (_obj, BLOCKANGULARCONSTRAINTPARAMETER, dependency);
  // AcDbBlockAngularConstraintParameter
  CHK_ENTITY_3RD (_obj, BLOCKANGULARCONSTRAINTPARAMETER, origin);
  CHK_ENTITY_3RD (_obj, BLOCKANGULARCONSTRAINTPARAMETER, end_pt);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKANGULARCONSTRAINTPARAMETER, expr_name);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKANGULARCONSTRAINTPARAMETER, param_name);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, angle, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKANGULARCONSTRAINTPARAMETER, b280, B);
  // AcDbBlockParamValueSet
  // ..
#endif
}

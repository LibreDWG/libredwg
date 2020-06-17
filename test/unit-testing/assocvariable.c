// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCVARIABLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS av_class_version;
  ASSOCACTION_fields;
  BITCODE_T name;
  BITCODE_T t58;
  BITCODE_T evaluator;
  BITCODE_T desc;
  BITCODE_BSd value_type;
  Dwg_EvalVariant value;
  BITCODE_B has_t78;
  BITCODE_T t78;
  BITCODE_B b290;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assocvariable *_obj = dwg_object_to_ASSOCVARIABLE (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, av_class_version, BS);

  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, geometry_status, BL);
  CHK_ENTITY_MAX (_obj, ASSOCVARIABLE, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOCVARIABLE, owningnetwork);
  CHK_ENTITY_H (_obj, ASSOCVARIABLE, actionbody);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, action_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, max_assoc_dep_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, num_deps, BL);
  //Dwg_ASSOCACTION_Deps *deps;
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, num_owned_params, BL);
  CHK_ENTITY_HV (_obj, ASSOCVARIABLE, owned_params, num_owned_params);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, num_owned_value_param_names, BL);
  CHK_ENTITY_HV (_obj, ASSOCVARIABLE, owned_value_param_names, num_owned_value_param_names);

  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, name);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, t58);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, evaluator);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, desc);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, value_type, BSd);
  // TODO switch value_type
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, has_t78, B);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCVARIABLE, t78);
  CHK_ENTITY_TYPE (_obj, ASSOCVARIABLE, b290, B);
#endif
}

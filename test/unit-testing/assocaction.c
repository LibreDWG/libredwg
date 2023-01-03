// stable
#define DWG_TYPE DWG_TYPE_ASSOCACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  BITCODE_BL geometry_status;
  BITCODE_H owningnetwork;
  BITCODE_H actionbody;
  BITCODE_BL action_index;
  BITCODE_BL max_assoc_dep_index;
  BITCODE_BL num_deps;
  Dwg_ASSOCACTION_Deps *deps;
  BITCODE_BL num_owned_params;
  BITCODE_H *owned_params;
  BITCODE_BL num_values;
  Dwg_VALUEPARAM *values;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_assocaction *_obj = dwg_object_to_ASSOCACTION (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCACTION, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, geometry_status, BL);
  CHK_ENTITY_MAX (_obj, ASSOCACTION, geometry_status, BL, 10);
  CHK_ENTITY_H (_obj, ASSOCACTION, owningnetwork);
  CHK_ENTITY_H (_obj, ASSOCACTION, actionbody);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, action_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, max_assoc_dep_index, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, num_deps, BL);
  for (unsigned i = 0; i < num_deps; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->deps[i], ASSOCACTION_Deps, is_owned, B);
      CHK_SUBCLASS_H (_obj->deps[i], ASSOCACTION_Deps, dep);
    }
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, num_owned_params, BL);
  CHK_ENTITY_HV (_obj, ASSOCACTION, owned_params, num_owned_params);
  CHK_ENTITY_TYPE (_obj, ASSOCACTION, num_values, BL);
  CHK_VALUEPARAM (num_values, values);
}

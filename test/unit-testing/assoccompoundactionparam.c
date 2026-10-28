// unstable, no coverage
#define DWG_TYPE DWG_TYPE_ASSOCCOMPOUNDACTIONPARAM
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;
  ASSOCCOMPOUNDACTIONPARAM_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assoccompoundactionparam *_obj
      = dwg_object_to_ASSOCCOMPOUNDACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCCOMPOUNDACTIONPARAM, name);

  // see also ASSOCOSNAPPOINTREFACTIONPARAM
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, acap_version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, bs1, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, num_params, BL);
  CHK_ENTITY_HV (_obj, ASSOCCOMPOUNDACTIONPARAM, params, num_params);
  // #endif
}

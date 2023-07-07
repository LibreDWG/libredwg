// unstable
#define DWG_TYPE DWG_TYPE_ASSOCCOMPOUNDACTIONPARAM
#include "common.c"

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
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, num_params, BL);
  CHK_ENTITY_HV (_obj, ASSOCCOMPOUNDACTIONPARAM, params, num_params);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, has_child_param, B);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, child_status, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, child_id, BL);
  CHK_ENTITY_H (_obj, ASSOCCOMPOUNDACTIONPARAM, child_param);
  CHK_ENTITY_H (_obj, ASSOCCOMPOUNDACTIONPARAM, h330_2);
  CHK_ENTITY_TYPE (_obj, ASSOCCOMPOUNDACTIONPARAM, bl2, BL);
  CHK_ENTITY_H (_obj, ASSOCCOMPOUNDACTIONPARAM, h330_3);
  // #endif
}

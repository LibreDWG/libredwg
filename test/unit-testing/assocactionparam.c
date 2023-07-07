// unstable
#define DWG_TYPE DWG_TYPE_ASSOCACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocactionparam *_obj = dwg_object_to_ASSOCACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCACTIONPARAM, name);
  // #endif
}

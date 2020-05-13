// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCVERTEXACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;

#ifdef DEBUG_CLASSES
  dwg_obj_assocvertexactionparam *_obj = dwg_object_to_ASSOCVERTEXACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCVERTEXACTIONPARAM, class_version, BS);
#endif
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_LAYOUTPRINTCONFIG
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  BITCODE_BS flag;

#ifdef DEBUG_CLASSES
  dwg_obj_layoutprintconfig *_obj = dwg_object_to_LAYOUTPRINTCONFIG (obj);

  CHK_ENTITY_TYPE (_obj, LAYOUTPRINTCONFIG, class_version, BS);
  CHK_ENTITY_TYPE (_obj, LAYOUTPRINTCONFIG, flag, BS);
#endif
}

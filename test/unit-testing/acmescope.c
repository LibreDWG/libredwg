// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACMESCOPE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;

#ifdef DEBUG_CLASSES
  dwg_obj_acmescope *_obj = dwg_object_to_ACMESCOPE (obj);

  CHK_ENTITY_TYPE (_obj, ACMESCOPE, class_version, BS);
#endif
}

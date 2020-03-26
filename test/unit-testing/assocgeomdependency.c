// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCGEOMDEPENDENCY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;

#ifdef DEBUG_CLASSES
  dwg_obj_assocgeomdependency *_obj = dwg_object_to_ASSOCGEOMDEPENDENCY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, class_version, BS, class_version);
#endif
}

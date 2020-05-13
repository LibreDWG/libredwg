// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACMESTATEMGR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;

#ifdef DEBUG_CLASSES
  dwg_obj_acmestatemgr *_obj = dwg_object_to_ACMESTATEMGR (obj);

  CHK_ENTITY_TYPE (_obj, ACMESTATEMGR, class_version, BS);
#endif
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ACMECOMMANDHISTORY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;

#ifdef DEBUG_CLASSES
  dwg_obj_acmecommandhistory *_obj = dwg_object_to_ACMECOMMANDHISTORY (obj);

  CHK_ENTITY_TYPE (_obj, ACMECOMMANDHISTORY, class_version, BS);
#endif
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_CSACDOCUMENTOPTIONS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;

#ifdef DEBUG_CLASSES
  dwg_obj_csacdocumentoptions *_obj = dwg_object_to_CSACDOCUMENTOPTIONS (obj);

  CHK_ENTITY_TYPE (_obj, CSACDOCUMENTOPTIONS, class_version, BS);
#endif
}

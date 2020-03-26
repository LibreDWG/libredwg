// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_DOCUMENTOPTIONS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;

#ifdef DEBUG_CLASSES
  dwg_obj_documentoptions *_obj = dwg_object_to_DOCUMENTOPTIONS (obj);

  CHK_ENTITY_TYPE (_obj, DOCUMENTOPTIONS, class_version, BS, class_version);
#endif
}

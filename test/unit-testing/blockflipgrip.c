// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKFLIPGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockflipgrip *_obj = dwg_object_to_BLOCKFLIPGRIP (obj);

#endif
}

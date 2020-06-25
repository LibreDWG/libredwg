// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKROTATIONGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKGRIP_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockrotationgrip *_obj = dwg_object_to_BLOCKROTATIONGRIP (obj);

#endif
}

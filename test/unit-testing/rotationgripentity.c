// TODO Unknown
#define DWG_TYPE DWG_TYPE_ROTATIONGRIPENTITY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_rotationgripentity *_obj = dwg_object_to_ROTATIONGRIPENTITY (obj);
  // ..
#endif
}

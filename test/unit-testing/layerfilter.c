// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_LAYERFILTER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T name, description;

#ifdef DEBUG_CLASSES
  dwg_obj_layerfilter *_obj = dwg_object_to_LAYERFILTER (obj);

  CHK_ENTITY_UTF8TEXT (_obj, LAYERFILTER, name);
  CHK_ENTITY_UTF8TEXT (_obj, LAYERFILTER, description);
#endif
}

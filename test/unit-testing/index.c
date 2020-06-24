#define DWG_TYPE DWG_TYPE_INDEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_TIMEBLL last_updated;

  dwg_obj_index *_obj = dwg_object_to_INDEX (obj);

  CHK_ENTITY_TIMEBLL (_obj, LAYER_INDEX, last_updated);
}

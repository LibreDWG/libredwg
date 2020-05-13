#define DWG_TYPE DWG_TYPE_WIPEOUTVARIABLES
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS display_frame;

  dwg_obj_wipeoutvariables *_obj = dwg_object_to_WIPEOUTVARIABLES (obj);

  CHK_ENTITY_TYPE (_obj, WIPEOUTVARIABLES, display_frame, BS);
}

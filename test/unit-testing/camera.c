// no coverage, named view, not persistent in a DWG. CAMERADISPLAY=1
#define DWG_TYPE DWG_TYPE_CAMERA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0;
  BITCODE_H view;

  dwg_ent_camera *camera = dwg_object_to_CAMERA (obj);
  CHK_ENTITY_H (camera, CAMERA, view);
}

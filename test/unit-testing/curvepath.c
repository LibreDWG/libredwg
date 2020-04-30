// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_CURVEPATH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS class_version;
  BITCODE_H camera_path;
  BITCODE_H target_path;
  BITCODE_H viewtable;
  BITCODE_BS frames;
  BITCODE_BS frame_rate;
  BITCODE_B corner_decel;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_curvepath *_obj = dwg_object_to_CURVEPATH (obj);

  CHK_ENTITY_TYPE (_obj, CURVEPATH, class_version, BS, class_version);
  CHK_ENTITY_MAX (_obj, CURVEPATH, class_version, BS, 3);
  CHK_ENTITY_H (_obj, CURVEPATH, camera_path, camera_path);
  CHK_ENTITY_H (_obj, CURVEPATH, target_path, target_path);
  CHK_ENTITY_H (_obj, CURVEPATH, viewtable, viewtable);
  CHK_ENTITY_TYPE (_obj, CURVEPATH, frames, BS, frames);
  CHK_ENTITY_TYPE (_obj, CURVEPATH, frame_rate, BS, frame_rate);
  CHK_ENTITY_TYPE (_obj, CURVEPATH, corner_decel, BS, corner_decel);
#endif
}

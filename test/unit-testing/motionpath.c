// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_MOTIONPATH
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
  dwg_obj_motionpath *_obj = dwg_object_to_MOTIONPATH (obj);

  CHK_ENTITY_TYPE (_obj, MOTIONPATH, class_version, BS, class_version);
  CHK_ENTITY_MAX (_obj, MOTIONPATH, class_version, BS, 3);
  CHK_ENTITY_H (_obj, MOTIONPATH, camera_path, camera_path);
  CHK_ENTITY_H (_obj, MOTIONPATH, target_path, target_path);
  CHK_ENTITY_H (_obj, MOTIONPATH, viewtable, viewtable);
  CHK_ENTITY_TYPE (_obj, MOTIONPATH, frames, BS, frames);
  CHK_ENTITY_TYPE (_obj, MOTIONPATH, frame_rate, BS, frame_rate);
  CHK_ENTITY_TYPE (_obj, MOTIONPATH, corner_decel, BS, corner_decel);
#endif
}

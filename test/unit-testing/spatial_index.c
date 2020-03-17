#define DWG_TYPE DWG_TYPE_SPATIAL_INDEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL timestamp1;
  BITCODE_BL timestamp2;
  BITCODE_TV unknown;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_spatial_index *_obj = dwg_object_to_SPATIAL_INDEX (obj);

  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, timestamp1, BL, timestamp1);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, timestamp2, BL, timestamp2);
  CHK_ENTITY_UTF8TEXT (_obj, SPATIAL_INDEX, unknown, unknown);
}

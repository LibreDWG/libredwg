// unstable
#define DWG_TYPE DWG_TYPE_SPATIAL_INDEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_TIMEBLL last_updated;
  BITCODE_BD num1, num2, num3, num4, num5, num6;
  BITCODE_BL num_hdls;
  BITCODE_H *hdls;
  BITCODE_BL bindata_size;
  BITCODE_TF bindata;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_spatial_index *_obj = dwg_object_to_SPATIAL_INDEX (obj);

  CHK_ENTITY_TIMEBLL (_obj, SPATIAL_INDEX, last_updated);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, num1, BD);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, num2, BD);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, num3, BD);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, num4, BD);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, num5, BD);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, num6, BD);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, num_hdls, BL);
  CHK_ENTITY_HV (_obj, SPATIAL_INDEX, hdls, num_hdls);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, bindata_size, BL);
  CHK_ENTITY_TYPE (_obj, SPATIAL_INDEX, bindata, TF);
}

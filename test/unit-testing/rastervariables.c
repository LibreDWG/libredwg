#define DWG_TYPE DWG_TYPE_RASTERVARIABLES
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_BS display_frame;
  BITCODE_BS display_quality;
  BITCODE_BS units;

  dwg_obj_rastervariables *_obj = dwg_object_to_RASTERVARIABLES (obj);

  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, class_version, BL, class_version);
  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, display_frame, BS, display_frame);
  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, display_quality, BS, display_quality);
  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, units, BS, units);
}

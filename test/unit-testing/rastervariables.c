#define DWG_TYPE DWG_TYPE_RASTERVARIABLES
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_BS image_frame;
  BITCODE_BS image_quality;
  BITCODE_BS units;

  dwg_obj_rastervariables *_obj = dwg_object_to_RASTERVARIABLES (obj);

  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, class_version, BL);
  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, image_frame, BS);
  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, image_quality, BS);
  CHK_ENTITY_TYPE (_obj, RASTERVARIABLES, units, BS);
}

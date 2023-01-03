#define DWG_TYPE DWG_TYPE_DYNAMICBLOCKPURGEPREVENTER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS flag;
  BITCODE_H block;

  dwg_obj_dynamicblockpurgepreventer *_obj
      = dwg_object_to_DYNAMICBLOCKPURGEPREVENTER (obj);

  CHK_ENTITY_TYPE (_obj, DYNAMICBLOCKPURGEPREVENTER, flag, BS);
  CHK_ENTITY_H (_obj, DYNAMICBLOCKPURGEPREVENTER, block);
}

// unstable
#define DWG_TYPE DWG_TYPE_BLOCKREPRESENTATION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS flag;
  BITCODE_H block;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockrepresentation *_obj = dwg_object_to_BLOCKREPRESENTATION (obj);

  CHK_ENTITY_TYPE (_obj, BLOCKREPRESENTATION, flag, BS);
  CHK_ENTITY_H (_obj, BLOCKREPRESENTATION, block);
}

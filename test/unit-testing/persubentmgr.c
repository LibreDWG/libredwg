// TODO unstable
#define DWG_TYPE DWG_TYPE_PERSUBENTMGR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  BITCODE_BL unknown_bl1;
  BITCODE_BL unknown_bl2;
  BITCODE_BL unknown_bl3;
  BITCODE_BL unknown_bl4;
  BITCODE_BL unknown_bl5;
  BITCODE_BL unknown_bl6;

  dwg_obj_persubentmgr *_obj = dwg_object_to_PERSUBENTMGR (obj);
 
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, class_version, BL, class_version);
  CHK_ENTITY_MAX (_obj, PERSUBENTMGR, class_version, BL, 2);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_bl1, BL, unknown_bl1);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_bl2, BL, unknown_bl2);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_bl3, BL, unknown_bl3);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_bl4, BL, unknown_bl4);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_bl5, BL, unknown_bl5);
  CHK_ENTITY_TYPE (_obj, PERSUBENTMGR, unknown_bl6, BL, unknown_bl6);
}

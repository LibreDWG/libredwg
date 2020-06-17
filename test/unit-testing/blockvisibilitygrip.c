// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKVISIBILITYGRIP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  Dwg_EvalExpr evalexpr;
  BITCODE_T be_t;
  BITCODE_BL be_bl1;
  BITCODE_BL be_bl2;
  BITCODE_BL be_bl3;
  BITCODE_BL bg_bl1;
  BITCODE_BL bg_bl2;
  BITCODE_3BD bg_pt;
  BITCODE_B bg_insert_cycling;
  BITCODE_BL bg_insert_cycling_weight;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockvisibilitygrip *_obj = dwg_object_to_BLOCKVISIBILITYGRIP (obj);

  CHK_EVALEXPR (BLOCKVISIBILITYGRIP);

  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYGRIP, be_t);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, be_bl1, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, be_bl2, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, be_bl3, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_bl1, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_bl2, BL);
  CHK_ENTITY_3RD (_obj, BLOCKVISIBILITYGRIP, bg_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_insert_cycling, B);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, bg_insert_cycling_weight, BL);
#endif
}

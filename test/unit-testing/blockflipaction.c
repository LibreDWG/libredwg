// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKFLIPACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_fields;
  BITCODE_BL i, info_num1;
  BITCODE_T info_text1;
  BITCODE_BL info_num2;
  BITCODE_T info_text2;
  BITCODE_BL info_num3;
  BITCODE_T info_text3;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockflipaction *_obj = dwg_object_to_BLOCKFLIPACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKFLIPACTION, ba_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKFLIPACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKFLIPACTION, deps, num_deps);
  // AcDbBlockFlipAction
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPACTION, info_num1, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPACTION, info_text1);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPACTION, info_num2, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPACTION, info_text2);
  CHK_ENTITY_TYPE (_obj, BLOCKFLIPACTION, info_num3, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKFLIPACTION, info_text3);
  // ..
#endif
}

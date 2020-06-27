// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKARRAYACTION
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
  BITCODE_BL info_num4;
  BITCODE_T info_text4;
  BITCODE_BD column_offset;
  BITCODE_BD row_offset;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockarrayaction *_obj = dwg_object_to_BLOCKARRAYACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKARRAYACTION, ba_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKARRAYACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKARRAYACTION, deps, num_deps);
  // AcDbBlockArrayAction
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, info_num1, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKARRAYACTION, info_text1);
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, info_num2, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKARRAYACTION, info_text2);
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, info_num3, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKARRAYACTION, info_text3);
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, info_num4, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKARRAYACTION, info_text4);
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, column_offset, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKARRAYACTION, row_offset, BD);
#endif
}

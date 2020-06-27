// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKLOOKUPACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCKACTION_fields;
  BITCODE_BL info_num1;
  BITCODE_BL info_num2;
  BITCODE_T info_text1;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blocklookupaction *_obj = dwg_object_to_BLOCKLOOKUPACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKLOOKUPACTION, ba_pt);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKLOOKUPACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKLOOKUPACTION, deps, num_deps);
  // AcDbBlockLookupAction
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, info_num1, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKMOVEACTION, info_num2, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKMOVEACTION, info_text1);
  // ..
#endif
}

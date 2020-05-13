// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKGRIPLOCATIONCOMPONENT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // AcDbEvalExpr  
  BITCODE_BL ee_bl1;
  BITCODE_BL ee_bl2;
  BITCODE_BL ee_bl3;
  BITCODE_BL ee_bs1;
  BITCODE_BS ee_type;
  BITCODE_BL ee_bl4;
  // AcDbBlockGripExpr
  BITCODE_BL eval_type;
  BITCODE_T eval_expr; // one of: X Y UpdatedX UpdatedY DisplacementX DisplacementY

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockgriplocationcomponent *_obj = dwg_object_to_BLOCKGRIPLOCATIONCOMPONENT (obj);

  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, ee_bl1, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, ee_bl2, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, ee_bl3, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, ee_bs1, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, ee_type, BS);
  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, ee_bl4, BL);

  CHK_ENTITY_TYPE (_obj, BLOCKVISIBILITYGRIP, eval_type, BL);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYGRIP, eval_expr, eval_expr);
#endif
}

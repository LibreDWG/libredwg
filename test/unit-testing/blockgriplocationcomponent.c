// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKGRIPLOCATIONCOMPONENT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ACDBEVALEXPR_fields;
  // AcDbBlockGripExpr
  BITCODE_T eval_expr; // one of: X Y UpdatedX UpdatedY DisplacementX DisplacementY

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockgriplocationcomponent *_obj = dwg_object_to_BLOCKGRIPLOCATIONCOMPONENT (obj);

  CHK_EVALEXPR (BLOCKVISIBILITYGRIP);
  CHK_ENTITY_UTF8TEXT (_obj, BLOCKVISIBILITYGRIP, eval_expr);
#endif
}

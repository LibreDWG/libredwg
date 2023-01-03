#define DWG_TYPE DWG_TYPE_BLOCKSCALEACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_WITHBASEPT_fields (5);
  BITCODE_BL i;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockscaleaction *_obj = dwg_object_to_BLOCKSCALEACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKSCALEACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKSCALEACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKSCALEACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKSCALEACTION, deps, num_deps);
  // AcDbBlockActionWithBasePt
  CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, offset);
  for (i = 0; i < 2; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->conn_pts[i], BLOCKACTION_connectionpts, code,
                         BL);
      CHK_SUBCLASS_UTF8TEXT (_obj->conn_pts[i], BLOCKACTION_connectionpts,
                             name);
    }
  CHK_ENTITY_TYPE (_obj, BLOCKSCALEACTION, dependent, B);
  CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, base_pt);
  // CHK_ENTITY_3RD (_obj, BLOCKSCALEACTION, stretch_pt);
  //  AcDbBlockScaleAction
  for (i = 2; i < 5; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->conn_pts[i], BLOCKACTION_connectionpts, code,
                         BL);
      CHK_SUBCLASS_UTF8TEXT (_obj->conn_pts[i], BLOCKACTION_connectionpts,
                             name);
    }
}

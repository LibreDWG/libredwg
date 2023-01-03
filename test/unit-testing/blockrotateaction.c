#define DWG_TYPE DWG_TYPE_BLOCKROTATEACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCKACTION_WITHBASEPT_fields (3);
  BITCODE_BL i;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_blockrotateaction *_obj = dwg_object_to_BLOCKROTATEACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKROTATEACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATEACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKROTATEACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKROTATEACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKROTATEACTION, deps, num_deps);
  // AcDbBlockActionWithBasePt
  CHK_ENTITY_3RD (_obj, BLOCKROTATEACTION, offset);
  for (i = 0; i < 2; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->conn_pts[i], BLOCKACTION_connectionpts, code,
                         BL);
      CHK_SUBCLASS_UTF8TEXT (_obj->conn_pts[i], BLOCKACTION_connectionpts,
                             name);
    }
  CHK_ENTITY_TYPE (_obj, BLOCKROTATEACTION, dependent, B);
  CHK_ENTITY_3RD (_obj, BLOCKROTATEACTION, base_pt);
  // CHK_ENTITY_3RD (_obj, BLOCKROTATEACTION, stretch_pt);
  //  AcDbBlockRotateAction
  for (i = 2; i < 3; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->conn_pts[i], BLOCKACTION_connectionpts, code,
                         BL);
      CHK_SUBCLASS_UTF8TEXT (_obj->conn_pts[i], BLOCKACTION_connectionpts,
                             name);
    }
}

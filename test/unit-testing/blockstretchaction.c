// looks stable, but some structs might be missing. indxf definitely broken
#define DWG_TYPE DWG_TYPE_BLOCKSTRETCHACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCKACTION_fields;
  BITCODE_BL num_pts;
  BITCODE_2RD *pts;
  BITCODE_BL num_hdls;
  BITCODE_H *hdls;
  BITCODE_BS *shorts;
  BITCODE_BL num_codes;
  BITCODE_BL *codes;
  BLOCKACTION_doubles_fields;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockstretchaction *_obj = dwg_object_to_BLOCKSTRETCHACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKSTRETCHACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKSTRETCHACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKSTRETCHACTION, deps, num_deps);
  // AcDbBlockStretchAction
  for (i = 0; i < 2; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->conn_pts[i], BLOCKACTION_connectionpts, code,
                         BL);
      CHK_SUBCLASS_UTF8TEXT (_obj->conn_pts[i], BLOCKACTION_connectionpts,
                             name);
    }
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_pts, BL);
  CHK_ENTITY_2DPOINTS (_obj, BLOCKSTRETCHACTION, pts, num_pts);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_hdls, BL);
  CHK_ENTITY_HV (_obj, BLOCKSTRETCHACTION, hdls, num_hdls);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKSTRETCHACTION, shorts, num_hdls, BS);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_codes, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKSTRETCHACTION, codes, num_codes, BL);
  // ..
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, action_offset_x, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, action_offset_y, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, angle_offset, BD);
  // #endif
}

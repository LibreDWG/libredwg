// some structs in work. indxf definitely broken
#define DWG_TYPE DWG_TYPE_BLOCKSTRETCHACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i, j;
  BLOCKACTION_fields;
  BITCODE_BL num_pts;
  BITCODE_2RD *pts;
  BITCODE_BL num_hdls;
  Dwg_BLOCKSTRETCHACTION_handles *hdls;
  BITCODE_BL num_codes;
  Dwg_BLOCKSTRETCHACTION_codes *codes;
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
  for (i = 0; i < num_hdls; i++)
    {
      CHK_SUBCLASS_H (_obj->hdls[i], BLOCKSTRETCHACTION_handles, hdl);
      CHK_SUBCLASS_TYPE (_obj->hdls[i], BLOCKSTRETCHACTION_handles,
                         num_indexes, BS);
      CHK_SUBCLASS_VECTOR_TYPE (_obj->hdls[i], BLOCKSTRETCHACTION_handles,
                                indexes, _obj->hdls[i].num_indexes, BL);
    }
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, num_codes, BL);
  for (i = 0; i < num_codes; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->codes[i], BLOCKSTRETCHACTION_codes, bl95, BL);
      CHK_SUBCLASS_TYPE (_obj->codes[i], BLOCKSTRETCHACTION_codes, num_indexes,
                         BS);
      CHK_SUBCLASS_VECTOR_TYPE (_obj->codes[i], BLOCKSTRETCHACTION_codes,
                                indexes, _obj->codes[i].num_indexes, BL);
    }

  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, action_offset_x, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, action_offset_y, BD);
  CHK_ENTITY_TYPE (_obj, BLOCKSTRETCHACTION, angle_offset, BD);
  // #endif
}

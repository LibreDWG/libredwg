// TODO DEBUGGING GRUE_LTM_1090+_from_cadforum.cz,
// Annotation_-_Metric_from_autocad_2021_2018
#define DWG_TYPE DWG_TYPE_BLOCKPOLARSTRETCHACTION
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

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blockpolarstretchaction *_obj
      = dwg_object_to_BLOCKPOLARSTRETCHACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKPOLARSTRETCHACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARSTRETCHACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKPOLARSTRETCHACTION, actions, num_actions,
                          BL);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARSTRETCHACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKPOLARSTRETCHACTION, deps, num_deps);
  // AcDbBlockPolarStretchAction
  for (i = 0; i < 6; i++)
    {
      CHK_SUBCLASS_TYPE (_obj->conn_pts[i], BLOCKACTION_connectionpts, code,
                         BL);
      CHK_SUBCLASS_UTF8TEXT (_obj->conn_pts[i], BLOCKACTION_connectionpts,
                             name);
    }
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARSTRETCHACTION, num_pts, BL);
  CHK_ENTITY_2DPOINTS (_obj, BLOCKPOLARSTRETCHACTION, pts, num_pts);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARSTRETCHACTION, num_hdls, BL);
  CHK_ENTITY_HV (_obj, BLOCKPOLARSTRETCHACTION, hdls, num_hdls);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKPOLARSTRETCHACTION, shorts, num_hdls, BS);
  CHK_ENTITY_TYPE (_obj, BLOCKPOLARSTRETCHACTION, num_codes, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKPOLARSTRETCHACTION, codes, num_codes, BL);
  // ..
  // #endif
}

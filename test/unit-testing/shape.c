#define DWG_TYPE DWG_TYPE_SHAPE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness, scale, rotation, width_factor, oblique_angle;
  BITCODE_BS style_id;
  dwg_point_3d ins_pt, extrusion;
  BITCODE_H style;

  dwg_ent_shape *shape = dwg_object_to_SHAPE (obj);

  CHK_ENTITY_3RD_W_OLD (shape, SHAPE, ins_pt);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, scale, BD);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, rotation, BD);
  CHK_ENTITY_MAX (shape, SHAPE, rotation, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, width_factor, BD);
  CHK_ENTITY_TYPE (shape, SHAPE, oblique_angle, BD);
  CHK_ENTITY_MAX (shape, SHAPE, oblique_angle, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, thickness, BD);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, style_id, BS);
  CHK_ENTITY_3RD_W_OLD (shape, SHAPE, extrusion);
  CHK_ENTITY_H (shape, SHAPE, style);
}

#define DWG_TYPE DWG_TYPE_SHAPE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness, scale, rotation, width_factor, oblique;
  BITCODE_BS style_id;
  dwg_point_3d ins_pt, ext;
  BITCODE_H style;

  dwg_ent_shape *shape = dwg_object_to_SHAPE (obj);

  CHK_ENTITY_3RD_W_OLD (shape, SHAPE, ins_pt, ins_pt);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, scale, BD, scale);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, rotation, BD, rotation);
  CHK_ENTITY_MAX (shape, SHAPE, rotation, BD, 6.284);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, width_factor, BD, width_factor);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, oblique, BD, oblique);
  CHK_ENTITY_MAX (shape, SHAPE, oblique, BD, 6.284);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, thickness, BD, thickness);
  CHK_ENTITY_TYPE_W_OLD (shape, SHAPE, style_id, BS, style_id);
  CHK_ENTITY_3RD_W_OLD (shape, SHAPE, extrusion, ext);
  CHK_ENTITY_H (shape, SHAPE, style, style);
}

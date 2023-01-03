#define DWG_TYPE DWG_TYPE_DWFUNDERLAY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  dwg_point_3d extrusion, ins_pt, scale;
  BITCODE_RC flag;
  BITCODE_RC contrast;
  BITCODE_RC fade;
  BITCODE_BL num_clip_verts;
  dwg_point_2d *clip_verts;
  BITCODE_BS num_clip_inverts;
  dwg_point_2d *clip_inverts;
  BITCODE_H definition_id;

  dwg_ent_underlay *underlay = dwg_object_to_DWFUNDERLAY (obj);

  CHK_ENTITY_3RD (underlay, DWFUNDERLAY, extrusion);
  CHK_ENTITY_3RD (underlay, DWFUNDERLAY, ins_pt);
  CHK_ENTITY_3RD (underlay, DWFUNDERLAY, scale);
  CHK_ENTITY_TYPE (underlay, DWFUNDERLAY, flag, RC);
  CHK_ENTITY_TYPE (underlay, DWFUNDERLAY, contrast, RC);
  CHK_ENTITY_TYPE (underlay, DWFUNDERLAY, fade, RC);
  CHK_ENTITY_TYPE (underlay, DWFUNDERLAY, num_clip_verts, BL);
  if (!dwg_dynapi_entity_value (underlay, "DWFUNDERLAY", "clip_verts",
                                &clip_verts, NULL))
    fail ("DWFUNDERLAY.clip_verts");
  else
    {
      for (BITCODE_BL i = 0; i < num_clip_verts; i++)
        {
          ok ("DWFUNDERLAY.clip_verts[%d]: (%f, %f)", i, clip_verts[i].x,
              clip_verts[i].y);
        }
    }
  CHK_ENTITY_TYPE (underlay, DWFUNDERLAY, num_clip_inverts, BS);
  if (!dwg_dynapi_entity_value (underlay, "DWFUNDERLAY", "clip_inverts",
                                &clip_inverts, NULL))
    fail ("DWFUNDERLAY.clip_inverts");
  else
    {
      for (BITCODE_BL i = 0; i < num_clip_inverts; i++)
        {
          ok ("UNDERLAY.clip_inverts[%d]: (%f, %f)", i, clip_inverts[i].x,
              clip_inverts[i].y);
        }
    }
  CHK_ENTITY_H (underlay, DWFUNDERLAY, definition_id);
}

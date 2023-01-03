#define DWG_TYPE DWG_TYPE_WIPEOUT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  dwg_point_3d pt0, uvec, vvec;
  dwg_point_2d size; /*!< DXF 13/23; width, height in pixel */
  BITCODE_BS display_props;
  BITCODE_B clipping;
  BITCODE_RC brightness;
  BITCODE_RC contrast;
  BITCODE_RC fade;
  BITCODE_B clip_mode;
  BITCODE_BS clip_boundary_type;
  BITCODE_BL num_clip_verts;
  dwg_point_2d *clip_verts;
  BITCODE_2RD *cpts;
  BITCODE_H imagedef;
  BITCODE_H imagedefreactor;

  dwg_ent_wipeout *wipeout = dwg_object_to_WIPEOUT (obj);

  CHK_ENTITY_3RD (wipeout, WIPEOUT, pt0);
  CHK_ENTITY_3RD (wipeout, WIPEOUT, uvec);
  CHK_ENTITY_3RD (wipeout, WIPEOUT, vvec);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, display_props, BS);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, clipping, B);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, brightness, RC);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, contrast, RC);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, fade, RC);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, clip_mode, B);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, clip_boundary_type, BS);
  CHK_ENTITY_TYPE (wipeout, WIPEOUT, num_clip_verts, BL);

  if (!dwg_dynapi_entity_value (wipeout, "WIPEOUT", "clip_verts", &clip_verts,
                                NULL))
    fail ("WIPEOUT.clip_verts");
  else
    {
      for (BITCODE_BL i = 0; i < num_clip_verts; i++)
        {
          ok ("WIPEOUT.clip_verts[%d]: (%f, %f)", i, clip_verts[i].x,
              clip_verts[i].y);
        }
    }
  CHK_ENTITY_H (wipeout, WIPEOUT, imagedef);
  CHK_ENTITY_H (wipeout, WIPEOUT, imagedefreactor);
}

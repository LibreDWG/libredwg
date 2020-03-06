#define DWG_TYPE DWG_TYPE_IMAGE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
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
  dwg_point_2d* clip_verts;
  BITCODE_2RD *cpts;
  BITCODE_H imagedef;
  BITCODE_H imagedefreactor;

  dwg_ent_image *image = dwg_object_to_IMAGE (obj);

  CHK_ENTITY_3RD_W_OLD (image, IMAGE, pt0, pt0);
  CHK_ENTITY_3RD (image, IMAGE, uvec, uvec); // still old api name u_vector
  CHK_ENTITY_3RD (image, IMAGE, vvec, uvec);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, display_props, BS, display_props);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, clipping, B, clipping);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, brightness, RC, brightness);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, contrast, RC, contrast);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, fade, RC, fade);
  CHK_ENTITY_TYPE (image, IMAGE, clip_mode, B, clip_mode); // no old api
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, clip_boundary_type, BS, clip_boundary_type);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, num_clip_verts, BL, num_clip_verts);

  if (!dwg_dynapi_entity_value (image, "IMAGE", "clip_verts", &clip_verts, NULL))
    fail ("IMAGE.clip_verts");
  cpts = dwg_ent_image_get_clip_verts (image, &error);
  if (error)
    fail ("IMAGE.clip_verts");
  else
    {
      for (BITCODE_BL i = 0; i < num_clip_verts; i++)
        {
          ok ("IMAGE.clip_verts[%d]: (%f, %f)", i, cpts[i].x,
              cpts[i].y);
          if (memcmp (&clip_verts[i], &cpts[i], sizeof (clip_verts[i])))
            fail ("IMAGE.clip_verts[%d]", i);
        }
    }
  free (cpts);

  CHK_ENTITY_H (image, IMAGE, imagedef, imagedef);
  CHK_ENTITY_H (image, IMAGE, imagedefreactor, imagedefreactor);
}

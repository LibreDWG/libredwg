#define DWG_TYPE DWG_TYPE_IMAGE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL class_version;
  dwg_point_3d pt0, uvec, vvec;
  dwg_point_2d image_size; /*!< DXF 13/23; width, height in pixel */
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

  dwg_ent_image *image = dwg_object_to_IMAGE (obj);

  CHK_ENTITY_3RD_W_OLD (image, IMAGE, pt0);
  CHK_ENTITY_3RD (image, IMAGE, uvec); // still old api name u_vector
  CHK_ENTITY_3RD (image, IMAGE, vvec);
  CHK_ENTITY_2RD (image, IMAGE, image_size);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, display_props, BS);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, clipping, B);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, brightness, RC);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, contrast, RC);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, fade, RC);
  CHK_ENTITY_TYPE (image, IMAGE, clip_mode, B); // no old api
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, clip_boundary_type, BS);
  CHK_ENTITY_TYPE_W_OLD (image, IMAGE, num_clip_verts, BL);

  if (!dwg_dynapi_entity_value (image, "IMAGE", "clip_verts", &clip_verts,
                                NULL))
    fail ("IMAGE.clip_verts");
#ifdef USE_DEPRECATED_API
  cpts = dwg_ent_image_get_clip_verts (image, &error);
  if (error)
    fail ("IMAGE.clip_verts");
  else
#else
  cpts = image->clip_verts;
#endif
    {
      for (BITCODE_BL i = 0; i < num_clip_verts; i++)
        {
          ok ("IMAGE.clip_verts[%d]: (%f, %f)", i, cpts[i].x, cpts[i].y);
          if (memcmp (&clip_verts[i], &cpts[i], sizeof (clip_verts[i])))
            fail ("IMAGE.clip_verts[%d]", i);
        }
    }
#ifdef USE_DEPRECATED_API
  FREE (cpts);
#endif

  CHK_ENTITY_H (image, IMAGE, imagedef);
  CHK_ENTITY_H (image, IMAGE, imagedefreactor);
}

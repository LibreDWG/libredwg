#define DWG_TYPE DWG_TYPE_SPATIAL_FILTER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS i, num_clip_verts;
  BITCODE_2RD *clip_verts;
  BITCODE_BE extrusion;
  BITCODE_3BD origin;
  BITCODE_BS display_boundary_on;
  BITCODE_BS front_clip_on;
  BITCODE_BD front_clip_z;
  BITCODE_BS back_clip_on;
  BITCODE_BD back_clip_z;
  BITCODE_BD *inverse_transform;
  BITCODE_BD *transform;

  dwg_obj_spatial_filter *_obj = dwg_object_to_SPATIAL_FILTER (obj);

  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, num_clip_verts, BS);
  if (!dwg_dynapi_entity_value (_obj, "SPATIAL_FILTER", "clip_verts",
                                &clip_verts, NULL))
    fail ("SPATIAL_FILTER.clip_verts");
  for (i = 0; i < num_clip_verts; i++)
    {
      ok ("SPATIAL_FILTER.clip_verts[%d]: (%f, %f)", i, clip_verts[i].x,
          clip_verts[i].y);
    }
  CHK_ENTITY_3RD (_obj, SPATIAL_FILTER, extrusion);
  CHK_ENTITY_3RD (_obj, SPATIAL_FILTER, origin);
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, display_boundary_on, BS);
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, front_clip_on, BS);
  if (front_clip_on)
    {
      CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, front_clip_z, BD);
    }
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, back_clip_on, BS);
  if (back_clip_on)
    {
      CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, back_clip_z, BD);
    }
  if (!dwg_dynapi_entity_value (_obj, "SPATIAL_FILTER", "inverse_transform",
                                &inverse_transform, NULL))
    fail ("SPATIAL_FILTER.inverse_transform");
  else
    for (i = 0; i < 12; i += 4)
      {
        ok ("SPATIAL_FILTER.inverse_transform[%d]: (%f, %f, %f, %f)", i,
            inverse_transform[i], inverse_transform[i + 1],
            inverse_transform[i + 2], inverse_transform[i + 3]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SPATIAL_FILTER", "transform",
                                &transform, NULL))
    fail ("SPATIAL_FILTER.transform");
  else
    for (i = 0; i < 12; i += 4)
      {
        ok ("SPATIAL_FILTER.transform[%d]: (%f, %f, %f, %f)", i, transform[i],
            transform[i + 1], transform[i + 2], transform[i + 3]);
      }
}

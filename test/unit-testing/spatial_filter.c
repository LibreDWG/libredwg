#define DWG_TYPE DWG_TYPE_SPATIAL_FILTER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS i, num_points;
  BITCODE_2RD* points;
  BITCODE_BE extrusion;
  BITCODE_3BD clip_bound_origin;
  BITCODE_BS display_boundary;
  BITCODE_BS front_clip_on;
  BITCODE_BD front_clip_dist;
  BITCODE_BS back_clip_on;
  BITCODE_BD back_clip_dist;
  BITCODE_BD* inverse_block_transform;
  BITCODE_BD* clip_bound_transform;

  dwg_obj_spatial_filter *_obj = dwg_object_to_SPATIAL_FILTER (obj);

  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, num_points, BS);
  if (!dwg_dynapi_entity_value (_obj, "SPATIAL_FILTER", "points", &points, NULL))
    fail ("SPATIAL_FILTER.points");
  for (i = 0; i < num_points; i++)
    {
      ok ("SPATIAL_FILTER.points[%d]: (%f, %f)", i, points[i].x, points[i].y);
    }
  CHK_ENTITY_3RD (_obj, SPATIAL_FILTER, extrusion, extrusion);
  CHK_ENTITY_3RD (_obj, SPATIAL_FILTER, clip_bound_origin, clip_bound_origin);
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, display_boundary, BS);
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, front_clip_on, BS);
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, front_clip_dist, BD);
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, back_clip_on, BS);
  CHK_ENTITY_TYPE (_obj, SPATIAL_FILTER, back_clip_dist, BD);
  if (!dwg_dynapi_entity_value (_obj, "SPATIAL_FILTER",
                                "inverse_block_transform",
                                &inverse_block_transform, NULL))
    fail ("SPATIAL_FILTER.inverse_block_transform");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("SPATIAL_FILTER.inverse_block_transform[%d]: %f", i,
            inverse_block_transform[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SPATIAL_FILTER",
                                "clip_bound_transform",
                                &clip_bound_transform, NULL))
    fail ("SPATIAL_FILTER.clip_bound_transform");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("SPATIAL_FILTER.clip_bound_transform[%d]: %f", i,
            clip_bound_transform[i]);
      }
}

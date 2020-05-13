#define DWG_TYPE DWG_TYPE_RAY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d point, vector;
  dwg_ent_ray *ray = dwg_object_to_RAY (obj);

  CHK_ENTITY_3RD_W_OLD (ray, RAY, point);
  CHK_ENTITY_3RD_W_OLD (ray, RAY, vector);
}

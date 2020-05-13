#define DWG_TYPE DWG_TYPE_XLINE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_point_3d point, vector;
  dwg_ent_xline *xline = dwg_object_to_XLINE (obj); // i.e. RAY

  CHK_ENTITY_3RD_W_OLD (xline, XLINE, point);
  CHK_ENTITY_3RD_W_OLD (xline, XLINE, vector);
}

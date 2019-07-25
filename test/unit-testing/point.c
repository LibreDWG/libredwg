#define DWG_TYPE DWG_TYPE_POINT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double thickness, x_ang;
  dwg_point_3d ext, pt, pt1;

  dwg_ent_point *point = dwg_object_to_POINT (obj);

  CHK_ENTITY_TYPE (point, POINT, x, BD, pt.x);
  CHK_ENTITY_TYPE (point, POINT, y, BD, pt.y);
  CHK_ENTITY_TYPE (point, POINT, z, BD, pt.z);
  dwg_ent_point_get_point (point, &pt1, &error);
  if (error || memcmp (&pt, &pt1, sizeof (pt)))
    fail ("old API dwg_ent_point_get_point");
  else
    pass ();

  CHK_ENTITY_TYPE_W_OLD (point, POINT, thickness, BD, thickness);
  CHK_ENTITY_3RD_W_OLD (point, POINT, extrusion, ext);
  CHK_ENTITY_TYPE (point, POINT, x_ang, BD, x_ang);
}

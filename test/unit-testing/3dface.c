#define DWG_TYPE DWG_TYPE__3DFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_B has_no_flags;
  BITCODE_B z_is_zero;
  BITCODE_BS invis_flags;
  dwg_point_3d corner1, corner2, corner3, corner4;
  dwg_point_3d _pt3d;                                                       

  dwg_ent_3dface *_3dface = dwg_object_to__3DFACE (obj);

  CHK_ENTITY_TYPE (_3dface, 3DFACE, has_no_flags, B);
  CHK_ENTITY_TYPE (_3dface, 3DFACE, z_is_zero, B);
  CHK_ENTITY_TYPE (_3dface, 3DFACE, invis_flags, BS);
  if ( dwg_ent_3dface_get_invis_flags (_3dface, &error) != invis_flags || error)
    fail ("old API dwg_ent_3dface_get_invis_flags");

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner1, corner1);
  dwg_ent_3dface_get_corner1 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner1, &_pt3d, sizeof (corner1)))
    fail ("old API dwg_ent_3dface_get_corner1");

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner2, corner2);
  dwg_ent_3dface_get_corner2 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner2, &_pt3d, sizeof (corner2)))
    fail ("old API dwg_ent_3dface_get_corner2");

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner3, corner3);
  dwg_ent_3dface_get_corner3 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner3, &_pt3d, sizeof (corner3)))
    fail ("old API dwg_ent_3dface_get_corner3");

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner4, corner4);
  dwg_ent_3dface_get_corner4 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner4, &_pt3d, sizeof (corner4)))
    fail ("old API dwg_ent_3dface_get_corner4");
}

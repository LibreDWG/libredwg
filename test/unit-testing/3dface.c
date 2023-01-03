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
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dface_get_invis_flags (_3dface, &error) != invis_flags || error)
    fail ("old API dwg_ent_3dface_get_invis_flags");
#endif
  if (has_no_flags && invis_flags)
    fail ("has_no_flags && invis_flags");
  CHK_ENTITY_MAX (_3dface, 3DFACE, invis_flags, BS, 15);

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner1);
#ifdef USE_DEPRECATED_API
  dwg_ent_3dface_get_corner1 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner1, &_pt3d, sizeof (corner1)))
    fail ("old API dwg_ent_3dface_get_corner1");
  if (z_is_zero != 0 && corner1.z != 0.0)
    fail ("z_is_zero && corner1.z != 0.0");
#endif

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner2);
#ifdef USE_DEPRECATED_API
  dwg_ent_3dface_get_corner2 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner2, &_pt3d, sizeof (corner2)))
    fail ("old API dwg_ent_3dface_get_corner2");
#endif

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner3);
#ifdef USE_DEPRECATED_API
  dwg_ent_3dface_get_corner3 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner3, &_pt3d, sizeof (corner3)))
    fail ("old API dwg_ent_3dface_get_corner3");
#endif

  CHK_ENTITY_3RD (_3dface, 3DFACE, corner4);
#ifdef USE_DEPRECATED_API
  dwg_ent_3dface_get_corner4 (_3dface, &_pt3d, &error);
  if (error || memcmp (&corner4, &_pt3d, sizeof (corner4)))
    fail ("old API dwg_ent_3dface_get_corner4");
#endif
}

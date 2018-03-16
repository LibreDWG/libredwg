#define DWG_TYPE DWG_TYPE_POLYLINE_3D
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casts dwg object to 3d polyline
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj);
 
  // prints flag 1
  printf("flag1 of polyline : %ud\n", polyline_3d->flags_1);

  // prints flag 2
  printf("flag2 of polyline : %ud\n", polyline_3d->flags_2);

  // prints owned object count
  printf("owned object of polyline : " FORMAT_BL "\n", polyline_3d->owned_obj_count);
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_BL owned_obj;
  BITCODE_RC flags1, flags2;
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj);

  // returns flag 1
  flags1 = dwg_ent_polyline_3d_get_flags_1(polyline_3d, &error);
  if ( !error )
  {
     printf("flag1 of polyline : " FORMAT_RC "\n", flags1);
  }
  else
  {
    printf("error in reading flag1");
  }

  // returns flag 2
  flags2 = dwg_ent_polyline_3d_get_flags_2(polyline_3d, &error);
  if ( !error )
  {
     printf("flag2 of polyline : " FORMAT_RC "\n", flags2);
  }
  else
  {
    printf("error in reading flag2");
  }

  // returns owned object count
  owned_obj = dwg_ent_polyline_3d_get_owned_obj_count(polyline_3d,
                                                      &error);
  if ( !error )
  {
     printf("owned object of polyline : " FORMAT_BL "\n", owned_obj);
  }
  else
  {
    printf("error in reading owned object count");
  }


}

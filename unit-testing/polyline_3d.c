#include "common.c"

void
output_process(dwg_object *obj);

void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_POLYLINE_3D)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casts dwg object to 3d polyline
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj);
 
  // prints flag 1
  printf("flag1 of polyline : %ud\t\n", polyline_3d->flags_1);

  // prints flag 2
  printf("flag2 of polyline : %ud\t\n", polyline_3d->flags_2);

  // prints owned object count
  printf("owned object of polyline : %ld\t\n", polyline_3d->owned_obj_count);
}

void
api_process(dwg_object *obj)
{
  int flags1_error, flags2_error, owned_obj_error;
  long owned_obj;
  unsigned int flags1, flags2;
  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D(obj);

  // returns flag 1
  flags1 = dwg_ent_polyline_3d_get_flags_1(polyline_3d, &flags1_error);
  if( flags1_error == 0 ) // error check
  {
     printf("flag1 of polyline : %ud\t\n", flags1);
  }
  else
  {
    printf("error in reading flag1");
  }

  // returns flag 2
  flags2 = dwg_ent_polyline_3d_get_flags_2(polyline_3d, &flags2_error);
  if( flags2_error == 0 ) // error check
  {
     printf("flag2 of polyline : %ud\t\n", flags2);
  }
  else
  {
    printf("error in reading flag2");
  }

  // returns owned object count
  owned_obj = dwg_ent_polyline_3d_get_owned_obj_count(polyline_3d,
               &owned_obj_error);
  if( owned_obj_error == 0 ) // error check
  {
     printf("owned object of polyline : %ld\t\n", owned_obj);
  }
  else
  {
    printf("error in reading owned object count");
  }


}

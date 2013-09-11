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

  if (dwg_get_type(obj)== DWG_TYPE_VERTEX_3D)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D(obj);
  printf("flag of vertex_3d : %d\t\n", vertex_3d->flags);
  printf("point of vertex_3d : x = %f, y = %f, z = %f\t\n",
         vertex_3d->point.x, vertex_3d->point.y, vertex_3d->point.z);
}

void
api_process(dwg_object *obj)
{
  int flag_error, point_error, end_w_error, start_w_error, bulge_error,
      dir_error;
  double start_width, end_width, bulge, tan_dir;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D(obj);


  flags = dwg_ent_vertex_3d_get_flags(vertex_3d, &flag_error);
  if(flag_error == 0 )
    {  
      printf("flag of vertex_3d : %d\t\n", flags);
    }
  else
    {
      printf("error in reading flag \n");
    }

  dwg_ent_vertex_3d_get_point(vertex_3d, &point, &point_error);
  if(point_error == 0 )
    {
      printf("point of vertex_3d : x = %f, y = %f, z = %f\t\n",
	     point.x, point.y, point.z);
    }
  else
    {
      printf("error in reading point \n");
    }

}

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

  if (dwg_get_type(obj)== DWG_TYPE_VERTEX_PFACE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_vertex_pface *vertex_pface = dwg_object_to_VERTEX_PFACE(obj);
  printf("flag of vertex_pface : %d\t\n", vertex_pface->flags);
  printf("point of vertex_pface : x = %f, y = %f, z = %f\t\n",
         vertex_pface->point.x, vertex_pface->point.y, vertex_pface->point.z);
}

void
api_process(dwg_object *obj)
{
  int flag_error, point_error, end_w_error, start_w_error, bulge_error,
      dir_error;
  double start_width, end_width, bulge, tan_dir;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_pface *vertex_pface = dwg_object_to_VERTEX_PFACE(obj);


  flags = dwg_ent_vertex_pface_get_flags(vertex_pface, &flag_error);
  if(flag_error == 0 )
    {  
      printf("flag of vertex_pface : %d\t\n", flags);
    }
  else
    {
      printf("error in reading flag \n");
    }

  dwg_ent_vertex_pface_get_point(vertex_pface, &point, &point_error);
  if(point_error == 0 )
    {
      printf("point of vertex_pface : x = %f, y = %f, z = %f\t\n",
	     point.x, point.y, point.z);
    }
  else
    {
      printf("error in reading point \n");
    }

}

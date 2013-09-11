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

  if (dwg_get_type(obj)== DWG_TYPE_VERTEX_2D)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_vertex_2d *vertex_2d = dwg_object_to_VERTEX_2D(obj);
  printf("flag of vertex_2d : %d\t\n", vertex_2d->flags);
  printf("point of vertex_2d : x = %f, y = %f, z = %f\t\n",
         vertex_2d->point.x, vertex_2d->point.y, vertex_2d->point.z);
  printf("start width of vertex_2d : %f\t\n", vertex_2d->start_width);
  printf("end width of vertex_2d : %f\t\n",vertex_2d-> end_width);
  printf("bulge of vertex_2d : %f\t\n", vertex_2d->bulge);
  printf("tangent dir of vertex_2d : %f\t\n", vertex_2d->tangent_dir);
}

void
api_process(dwg_object *obj)
{
  int flag_error, point_error, end_w_error, start_w_error, bulge_error,
      dir_error;
  double start_width, end_width, bulge, tan_dir;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_2d *vertex_2d = dwg_object_to_VERTEX_2D(obj);


  flags = dwg_ent_vertex_2d_get_flags(vertex_2d, &flag_error);
  if(flag_error == 0 )
    {  
      printf("flag of vertex_2d : %d\t\n", flags);
    }
  else
    {
      printf("error in reading flag \n");
    }

  dwg_ent_vertex_2d_get_point(vertex_2d, &point, &point_error);
  if(point_error == 0 )
    {
      printf("point of vertex_2d : x = %f, y = %f, z = %f\t\n",
	     point.x, point.y, point.z);
    }
  else
    {
      printf("error in reading point \n");
    }

  start_width = dwg_ent_vertex_2d_get_start_width(vertex_2d, &start_w_error);
  if(start_w_error == 0 )
    {
      printf("start width of vertex_2d : %f\t\n", start_width);
    }
  else
    {
      printf("error in reading start width \n");
    }

  end_width = dwg_ent_vertex_2d_get_end_width(vertex_2d, &end_w_error);
  if(end_w_error == 0 )
    {
      printf("end width of vertex_2d : %f\t\n", end_width);
    }
  else
    {
      printf("error in reading end width \n");
    }
  bulge = dwg_ent_vertex_2d_get_bulge(vertex_2d, &bulge_error);
  if(bulge_error == 0 )
    {
      printf("bulge of vertex_2d : %f\t\n", bulge);
    }
  else
    {
      printf("error in reading bulge \n");
    }
  tan_dir = dwg_ent_vertex_2d_get_bulge(vertex_2d, &dir_error);
  if(dir_error == 0 )
    {
      printf("tangent dir of vertex_2d : %f\t\n", tan_dir);
    }
  else
    {
      printf("error in reading tangent dir \n");
    }
}

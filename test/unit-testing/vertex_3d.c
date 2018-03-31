#define DWG_TYPE DWG_TYPE_VERTEX_3D
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D(obj);

  printf("flag of vertex_3d : " FORMAT_RC "\n", vertex_3d->flag);
  printf("point of vertex_3d : x = %f, y = %f, z = %f\n",
         vertex_3d->point.x, vertex_3d->point.y, vertex_3d->point.z);
}

void
api_process(dwg_object *obj)
{
  int error;
  double start_width, end_width, bulge, tan_dir;
  BITCODE_RC flags;
  dwg_point_3d point;

  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D(obj);

  flags = dwg_ent_vertex_3d_get_flag(vertex_3d, &error);
  if ( !error )
      printf("flag of vertex_3d : " FORMAT_RC "\n", flags);
  else
      printf("error in reading flag \n");

  dwg_ent_vertex_3d_get_point(vertex_3d, &point, &error);
  if ( !error )
      printf("point of vertex_3d : x = %f, y = %f, z = %f\n",
	     point.x, point.y, point.z);
  else
      printf("error in reading point \n");
}

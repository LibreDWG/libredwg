#define DWG_TYPE DWG_TYPE_VERTEX_MESH
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_vertex_mesh *vertex_mesh = dwg_object_to_VERTEX_MESH(obj);

  printf("flag of vertex_mesh : " FORMAT_RC "\n", vertex_mesh->flag);
  printf("point of vertex_mesh : x = %f, y = %f, z = %f\n",
         vertex_mesh->point.x, vertex_mesh->point.y, vertex_mesh->point.z);
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_RC flag;
  dwg_point_3d point;
  dwg_ent_vertex_mesh *vertex_mesh = dwg_object_to_VERTEX_MESH(obj);

  flag = dwg_ent_vertex_mesh_get_flag(vertex_mesh, &error);
  if ( !error )
      printf("flag of vertex_mesh : " FORMAT_RC "\n", flag);
  else
      printf("error in reading flag \n");

  dwg_ent_vertex_mesh_get_point(vertex_mesh, &point, &error);
  if ( !error )
      printf("point of vertex_mesh : x = %f, y = %f, z = %f\n",
	     point.x, point.y, point.z);
  else
      printf("error in reading point \n");
}

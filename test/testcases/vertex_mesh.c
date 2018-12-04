#define DWG_TYPE DWG_TYPE_VERTEX_MESH
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_vertex_mesh *vertex_mesh = dwg_object_to_VERTEX_MESH (obj);

  printf ("flag of vertex_mesh : " FORMAT_RC "\n", vertex_mesh->flag);
  printf ("point of vertex_mesh : x = %f, y = %f, z = %f\n",
          vertex_mesh->point.x, vertex_mesh->point.y, vertex_mesh->point.z);
}

void
api_process (dwg_object * obj)
{
  int error;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_mesh *vertex_mesh = dwg_object_to_VERTEX_MESH (obj);


  flags = dwg_ent_vertex_mesh_get_flag (vertex_mesh, &error);
  if (!error  && flags == vertex_mesh->flag)    // error checking
    pass ("Working Properly");
  else
    fail ("error in reading flag");


  dwg_ent_vertex_mesh_get_point (vertex_mesh, &point, &error);
  if (!error  && point.x == vertex_mesh->point.x && point.y == vertex_mesh->point.y && point.z == vertex_mesh->point.z) // error check
    pass ("Working Properly");
  else
    fail ("error in reading point");
}

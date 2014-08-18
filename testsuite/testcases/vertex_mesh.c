#include "common.c"
#include <dejagnu.h>

void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_VERTEX_MESH)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{

  // casts object to vertex mesh
  dwg_ent_vertex_mesh *vertex_mesh = dwg_object_to_VERTEX_MESH (obj);

  // prints flag of vertex_mesh
  printf ("flag of vertex_mesh : %d\t\n", vertex_mesh->flags);

  // prints the point of vertex_mesh
  printf ("point of vertex_mesh : x = %f, y = %f, z = %f\t\n",
	  vertex_mesh->point.x, vertex_mesh->point.y, vertex_mesh->point.z);
}

void
api_process (dwg_object * obj)
{
  int flag_error, point_error;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_mesh *vertex_mesh = dwg_object_to_VERTEX_MESH (obj);

  // returns flag
  flags = dwg_ent_vertex_mesh_get_flags (vertex_mesh, &flag_error);
  if (flag_error == 0 && flags == vertex_mesh->flags)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flag");
    }

  // returns point of vertex_mesh
  dwg_ent_vertex_mesh_get_point (vertex_mesh, &point, &point_error);
  if (point_error == 0 && point.x == vertex_mesh->point.x && point.y == vertex_mesh->point.y && point.z == vertex_mesh->point.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading point");
    }
}

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

  if (dwg_get_type (obj) == DWG_TYPE_VERTEX_PFACE)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{

  // casts dwg object to vertex pface entity
  dwg_ent_vertex_pface *vertex_pface = dwg_object_to_VERTEX_PFACE (obj);

  // prints flag
  printf ("flag of vertex_pface : %d\t\n", vertex_pface->flags);

  // prints points of vertex_pface
  printf ("point of vertex_pface : x = %f, y = %f, z = %f\t\n",
	  vertex_pface->point.x, vertex_pface->point.y,
	  vertex_pface->point.z);
}

void
api_process (dwg_object * obj)
{
  int flag_error, point_error;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_pface *vertex_pface = dwg_object_to_VERTEX_PFACE (obj);

  // returns flags
  flags = dwg_ent_vertex_pface_get_flags (vertex_pface, &flag_error);
  if (flag_error == 0 && flags == vertex_pface->flags)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flag");
    }

  // returns point of vertex_pface
  dwg_ent_vertex_pface_get_point (vertex_pface, &point, &point_error);
  if (point_error == 0 && point.x == vertex_pface->point.x && point.y == vertex_pface->point.y && point.z == vertex_pface->point.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading point");
    }

}

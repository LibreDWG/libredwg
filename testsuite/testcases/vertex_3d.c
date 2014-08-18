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

  if (dwg_get_type (obj) == DWG_TYPE_VERTEX_3D)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{

  // casts dwg object to vertex_3d entity
  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D (obj);

  // prints flag
  printf ("flag of vertex_3d : %d\t\n", vertex_3d->flags);

  // prints values of point
  printf ("point of vertex_3d : x = %f, y = %f, z = %f\t\n",
	  vertex_3d->point.x, vertex_3d->point.y, vertex_3d->point.z);
}

void
api_process (dwg_object * obj)
{
  int flag_error, point_error, end_w_error, start_w_error, bulge_error,
    dir_error;
  double start_width, end_width, bulge, tan_dir;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D (obj);

  // returns flags
  flags = dwg_ent_vertex_3d_get_flags (vertex_3d, &flag_error);
  if (flag_error == 0 && flags == vertex_3d->flags)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading flag");
    }

  // returns point of vertex_3d
  dwg_ent_vertex_3d_get_point (vertex_3d, &point, &point_error);
  if (point_error == 0 && point.x == vertex_3d->point.x && point.y == vertex_3d->point.y && point.z == vertex_3d->point.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading point");
    }

}

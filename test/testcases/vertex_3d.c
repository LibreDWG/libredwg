#define DWG_TYPE DWG_TYPE_VERTEX_3D
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D (obj);

  printf ("flag of vertex_3d : " FORMAT_BS "\n", vertex_3d->flags);
  printf ("point of vertex_3d : x = %f, y = %f, z = %f\n",
	  vertex_3d->point.x, vertex_3d->point.y, vertex_3d->point.z);
}

void
api_process (dwg_object * obj)
{
  int error;
  double start_width, end_width, bulge, tan_dir;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_3d *vertex_3d = dwg_object_to_VERTEX_3D (obj);


  flags = dwg_ent_vertex_3d_get_flags (vertex_3d, &error);
  if (!error  && flags == vertex_3d->flags)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading flag");


  dwg_ent_vertex_3d_get_point (vertex_3d, &point, &error);
  if (!error  && point.x == vertex_3d->point.x && point.y == vertex_3d->point.y && point.z == vertex_3d->point.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading point");

}

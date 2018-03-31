#define DWG_TYPE DWG_TYPE_VERTEX_2D
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_vertex_2d *vertex_2d = dwg_object_to_VERTEX_2D (obj);

  printf ("flag of vertex_2d : " FORMAT_BS "\n", vertex_2d->flag);
  printf ("point of vertex_2d : x = %f, y = %f, z = %f\n",
	  vertex_2d->point.x, vertex_2d->point.y, vertex_2d->point.z);
  printf ("start width of vertex_2d : %f\n", vertex_2d->start_width);
  printf ("end width of vertex_2d : %f\n", vertex_2d->end_width);
  printf ("bulge of vertex_2d : %f\n", vertex_2d->bulge);
  printf ("tangent dir of vertex_2d : %f\n", vertex_2d->tangent_dir);
}

void
api_process (dwg_object * obj)
{
  int error;
  double start_width, end_width, bulge, tan_dir;
  char flags;
  dwg_point_3d point;
  dwg_ent_vertex_2d *vertex_2d = dwg_object_to_VERTEX_2D (obj);


  flags = dwg_ent_vertex_2d_get_flag (vertex_2d, &error);
  if (!error  && flags == vertex_2d->flag)
    pass ("Working Properly");
  else
    fail ("error in reading flag");


  dwg_ent_vertex_2d_get_point (vertex_2d, &point, &error);
  if (!error  && point.x == vertex_2d->point.x && point.y == vertex_2d->point.y && point.z == vertex_2d->point.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading point");


  start_width = dwg_ent_vertex_2d_get_start_width (vertex_2d, &error);
  if (!error  && start_width == vertex_2d->start_width)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading start width");


  end_width = dwg_ent_vertex_2d_get_end_width (vertex_2d, &error);
  if (!error  && end_width == vertex_2d->end_width)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading end width");


  bulge = dwg_ent_vertex_2d_get_bulge (vertex_2d, &error);
  if (!error  && bulge == vertex_2d->bulge)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading bulge");


  tan_dir = dwg_ent_vertex_2d_get_bulge (vertex_2d, &error);
  if (!error  && tan_dir == vertex_2d->tangent_dir)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading tangent dir");
}

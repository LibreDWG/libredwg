#define DWG_TYPE DWG_TYPE_VERTEX_2D
#include "common.c"

void
low_level_process (dwg_object *obj)
{
  dwg_ent_vertex_2d *vertex_2d = dwg_object_to_VERTEX_2D (obj);

  printf ("flag of vertex_2d : " FORMAT_RC "\n", vertex_2d->flag);
  printf ("point of vertex_2d : x = %f, y = %f, z = %f\n", vertex_2d->point.x,
          vertex_2d->point.y, vertex_2d->point.z);
  printf ("start width of vertex_2d : %f\n", vertex_2d->start_width);
  printf ("end width of vertex_2d : %f\n", vertex_2d->end_width);
  printf ("bulge of vertex_2d : %f\n", vertex_2d->bulge);
  printf ("tangent dir of vertex_2d : %f\n", vertex_2d->tangent_dir);
}

void
api_process (dwg_object *obj)
{
  int error;
  double start_width, end_width, bulge, tan_dir;
  BITCODE_RC flags;
  dwg_point_3d point;

  dwg_ent_vertex_2d *vertex_2d = dwg_object_to_VERTEX_2D (obj);

  flags = dwg_ent_vertex_2d_get_flag (vertex_2d, &error);
  if (!error)
    printf ("flag of vertex_2d : " FORMAT_RC "\n", flags);
  else
    printf ("error in reading flag \n");

  dwg_ent_vertex_2d_get_point (vertex_2d, &point, &error);
  if (!error)
    printf ("point of vertex_2d : x = %f, y = %f, z = %f\n", point.x, point.y,
            point.z);
  else
    printf ("error in reading point \n");

  start_width = dwg_ent_vertex_2d_get_start_width (vertex_2d, &error);
  if (!error)
    printf ("start width of vertex_2d : %f\n", start_width);
  else
    printf ("error in reading start width \n");

  end_width = dwg_ent_vertex_2d_get_end_width (vertex_2d, &error);
  if (!error)
    printf ("end width of vertex_2d : %f\n", end_width);
  else
    printf ("error in reading end width \n");

  bulge = dwg_ent_vertex_2d_get_bulge (vertex_2d, &error);
  if (!error)
    printf ("bulge of vertex_2d : %f\n", bulge);
  else
    printf ("error in reading bulge \n");

  tan_dir = dwg_ent_vertex_2d_get_bulge (vertex_2d, &error);
  if (!error)
    printf ("tangent dir of vertex_2d : %f\n", tan_dir);
  else
    printf ("error in reading tangent dir \n");
}

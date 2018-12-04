#define DWG_TYPE DWG_TYPE_VERTEX_PFACE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_vertex_pface *vertex_pface = dwg_object_to_VERTEX_PFACE(obj);

  printf("flag of vertex_pface : " FORMAT_RC "\n", vertex_pface->flag);
  printf("point of vertex_pface : x = %f, y = %f, z = %f\n",
         vertex_pface->point.x, vertex_pface->point.y, vertex_pface->point.z);
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_RC flags;
  dwg_point_3d point;

  dwg_ent_vertex_pface *vertex_pface = dwg_object_to_VERTEX_PFACE(obj);

  flags = dwg_ent_vertex_pface_get_flag(vertex_pface, &error);
  if ( !error )
      printf("flag of vertex_pface : %c\n", flags);
  else
      printf("error in reading flag \n");

  dwg_ent_vertex_pface_get_point(vertex_pface, &point,
                                 &error);
  if ( !error )
      printf("point of vertex_pface : x = %f, y = %f, z = %f\n",
             point.x, point.y, point.z);
  else
      printf("error in reading point \n");
}

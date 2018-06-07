#define DWG_TYPE DWG_TYPE_POLYLINE_PFACE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_polyline_pface *polyline_pface = dwg_object_to_POLYLINE_PFACE(obj);

  printf("num verts of polyline_pface : %ud\n",
          polyline_pface->numverts);
  printf("num faces of polyline_pface : " FORMAT_BL "\n", polyline_pface->numfaces);
}

void
api_process(dwg_object *obj)
{
  int error;
  BITCODE_BL numpoints, numfaces;
  dwg_ent_polyline_pface *polyline_pface = dwg_object_to_POLYLINE_PFACE(obj);

  numpoints = dwg_ent_polyline_pface_get_numpoints(polyline_pface, &error);
  if (!error)
    printf("Num points of polyline_pface : " FORMAT_BL "\n", numpoints);
  else
    printf("error in reading numpoints \n");

  numfaces = dwg_ent_polyline_pface_get_numfaces(polyline_pface, &error);
  if (!error)
    printf("Num faces of polyline_pface : %ud\n", numfaces);
  else
    printf("error in reading num faces \n");
}

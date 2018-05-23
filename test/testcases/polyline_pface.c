#define DWG_TYPE DWG_TYPE_POLYLINE_PFACE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_polyline_pface *polyline_pface = dwg_object_to_POLYLINE_PFACE (obj);

  printf ("num verts of polyline_pface : " FORMAT_BL "\n", polyline_pface->numverts);
  printf ("owned obj count of polyline_pface : " FORMAT_BL "\n",
	  polyline_pface->owned_obj_count);
  printf ("num faces of polyline_pface : " FORMAT_BL "\n", polyline_pface->numfaces);

}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BS num_verts, num_faces;
  BITCODE_BL owned_obj_count;
  dwg_ent_polyline_pface *polyline_pface = dwg_object_to_POLYLINE_PFACE (obj);


  num_verts =
    dwg_ent_polyline_pface_get_numpoints (polyline_pface, &error);
  if (!error  && polyline_pface->numverts == num_verts)
    pass ("Working Properly");
  else
    fail ("error in reading num_verts");

  num_faces = dwg_ent_polyline_pface_get_numfaces (polyline_pface,
						   &error);
  if (!error  && polyline_pface->numfaces == num_faces)
    pass ("Working Properly");
  else
    fail ("error in reading num faces");

  //TODO get_points
  //TODO get_faces
}

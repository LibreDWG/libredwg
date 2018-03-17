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
    dwg_ent_polyline_pface_get_numverts (polyline_pface, &error);
  if (!error  && polyline_pface->numverts == num_verts)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading num_verts");


  owned_obj_count =
    dwg_ent_polyline_pface_get_owned_obj_count (polyline_pface,
						&error);
  if (!error  && owned_obj_count == polyline_pface->owned_obj_count)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading owned_obj_count");


  num_faces = dwg_ent_polyline_pface_get_numfaces (polyline_pface,
						   &error);
  if (!error  && polyline_pface->numfaces == num_faces)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading num faces");

}

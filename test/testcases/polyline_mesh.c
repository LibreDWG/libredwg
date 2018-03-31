#define DWG_TYPE DWG_TYPE_POLYLINE_MESH
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{

  // casts dwg object to polyline mesh
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH (obj);


  printf ("flags of polyline_mesh : " FORMAT_BS "\n", polyline_mesh->flag);
  printf ("owned obj count of polyline_mesh : " FORMAT_BL "\n",
	  polyline_mesh->owned_obj_count);
  printf ("curve type of polyline_mesh : " FORMAT_BS "\n", polyline_mesh->curve_type);
  printf ("M density of polyline_mesh : " FORMAT_BS "\n", polyline_mesh->m_density);
  printf ("n density of polyline_mesh : " FORMAT_BS "\n", polyline_mesh->n_density);
  printf ("m vert count of polyline_mesh : " FORMAT_BS "\n",
	  polyline_mesh->m_vert_count);
  printf ("n vert count of polyline_mesh : " FORMAT_BS "\n",
	  polyline_mesh->n_vert_count);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BS flags, curve_type, mvert_count, nvert_count, m_density,
    n_density;
  BITCODE_BL owned_obj_count;
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH (obj);


  flags = dwg_ent_polyline_mesh_get_flag (polyline_mesh, &error);
  if (!error  && flags == polyline_mesh->flag)
    pass ("Working Properly");
  else
    fail ("error in reading flags");


  owned_obj_count = dwg_ent_polyline_mesh_get_owned_obj_count (polyline_mesh,
							       &error);
  if (!error  && owned_obj_count == polyline_mesh->owned_obj_count)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading owned_obj_count");

  // retuns curve type
  curve_type = dwg_ent_polyline_mesh_get_curve_type (polyline_mesh,
						     &error);
  if (!error  && curve_type == polyline_mesh->curve_type)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading curve type");


  m_density = dwg_ent_polyline_mesh_get_m_density (polyline_mesh,
						   &error);
  if (!error  && m_density == polyline_mesh->m_density)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading m_density");


  n_density = dwg_ent_polyline_mesh_get_n_density (polyline_mesh,
						   &error);
  if (!error  && n_density == polyline_mesh->n_density)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading n density");


  mvert_count = dwg_ent_polyline_mesh_get_m_vert_count (polyline_mesh,
							&error);
  if (!error  && mvert_count == polyline_mesh->m_vert_count)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading m vert count");


  nvert_count = dwg_ent_polyline_mesh_get_n_vert_count (polyline_mesh,
							&error);
  if (!error  && nvert_count == polyline_mesh->n_vert_count)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading n vert count");
}

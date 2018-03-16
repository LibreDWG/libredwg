#define DWG_TYPE DWG_TYPE_POLYLINE_MESH
#include "common.c"

void
low_level_process(dwg_object *obj)
{

  // casts dwg object to polyline mesh
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH(obj);

  // prints flags
  printf("flags of polyline_mesh : %ud\n",
          polyline_mesh->flags);

  // prints owned object count
  printf("owned obj count of polyline_mesh : " FORMAT_BL "\n",
          polyline_mesh->owned_obj_count);

  // prints curve type
  printf("curve type of polyline_mesh : %ud\n", polyline_mesh->curve_type);

  // prints M density
  printf("M density of polyline_mesh : %ud\n", polyline_mesh->m_density);

  // prints n density
  printf("n density of polyline_mesh : %ud\n", polyline_mesh->n_density);

  // prints m vertice count
  printf("m vert count of polyline_mesh : %ud\n", polyline_mesh->m_vert_count);

  // prints n vertice count
  printf("n vert count of polyline_mesh : %ud\n", polyline_mesh->n_vert_count);   
}

void
api_process(dwg_object *obj)
{
  int error;
  unsigned int flags, curve_type, mvert_count, nvert_count, m_density,
               n_density;
  BITCODE_BL owned_obj_count;
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH(obj);

  // returns flags
  flags = dwg_ent_polyline_mesh_get_flags(polyline_mesh, &error);
  if ( !error )
    {  
      printf("flags of polyline_mesh : %ud\n",flags);
    }
  else
    {
      printf("error in reading flags \n");
    }

  // returns owned object count
  owned_obj_count = dwg_ent_polyline_mesh_get_owned_obj_count(polyline_mesh,
                    &error);
  if ( !error )
    {
      printf("owned object count of polyline_mesh : " FORMAT_BL "\n", owned_obj_count);
    }
  else
    {
      printf("error in reading owned_obj_count \n");
    }

  // retuns curve type
  curve_type = dwg_ent_polyline_mesh_get_curve_type(polyline_mesh,
              &error);
  if ( !error )
    {
      printf("curve type of polyline_mesh : %ud\n", curve_type);
    }
  else
    {
      printf("error in reading curve type \n");
    }

  // returns m density
  m_density = dwg_ent_polyline_mesh_get_m_density(polyline_mesh,
              &error);
  if ( !error )
    {
      printf("M density of polyline_mesh : %ud\n", m_density);
    }
  else
    {
      printf("error in reading m_density \n");
    }

  // returns n density
  n_density = dwg_ent_polyline_mesh_get_n_density(polyline_mesh,
              &error);
  if ( !error )
    {
      printf("n density of polyline_mesh : %ud\n", n_density);
    }
  else
    {
      printf("error in reading n density \n");
    }

  // returns m vertice count
  mvert_count = dwg_ent_polyline_mesh_get_m_vert_count(polyline_mesh,
              &error);
  if ( !error )
    {
      printf("M vert count of polyline_mesh : %ud\n", mvert_count);
    }
  else
    {
      printf("error in reading m vert count \n");
    }

  // returns n vertice count
  nvert_count = dwg_ent_polyline_mesh_get_n_vert_count(polyline_mesh,
              &error);
  if ( !error )
    {
      printf("n vert count of polyline_mesh : %ud\n", nvert_count);
    }
  else
    {
      printf("error in reading n vert count \n");
    }
}

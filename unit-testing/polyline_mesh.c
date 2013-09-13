#include "common.c"

void
output_process(dwg_object *obj);

void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_POLYLINE_MESH)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts dwg object to polyline mesh
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH(obj);

  // prints flags
  printf("flags of polyline_mesh : %ud\t\n",
          polyline_mesh->flags);

  // prints owned object count
  printf("owned obj count of polyline_mesh : %ld\t\n",
          polyline_mesh->owned_obj_count);

  // prints curve type
  printf("curve type of polyline_mesh : %ud\t\n", polyline_mesh->curve_type);

  // prints M density
  printf("M density of polyline_mesh : %ud\t\n", polyline_mesh->m_density);

  // prints n density
  printf("n density of polyline_mesh : %ud\t\n", polyline_mesh->n_density);

  // prints m vertice count
  printf("m vert count of polyline_mesh : %ud\t\n", polyline_mesh->m_vert_count);

  // prints n vertice count
  printf("n vert count of polyline_mesh : %ud\t\n", polyline_mesh->n_vert_count);   
}

void
api_process(dwg_object *obj)
{
  int flags_error, owned_obj_error, curve_type_error, nvert_error, mvert_error,
      m_density_error, n_density_error;
  unsigned int flags, curve_type, mvert_count, nvert_count, m_density,
               n_density;
  long owned_obj_count;
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH(obj);

  // returns flags
  flags = dwg_ent_polyline_mesh_get_flags(polyline_mesh, &flags_error);
  if(flags_error == 0 ) // error check
    {  
      printf("flags of polyline_mesh : %ud\t\n",flags);
    }
  else
    {
      printf("error in reading flags \n");
    }

  // returns owned object count
  owned_obj_count = dwg_ent_polyline_mesh_get_owned_obj_count(polyline_mesh,
                    &owned_obj_error);
  if(owned_obj_error == 0 ) // error check
    {
      printf("owned object count of polyline_mesh : %ld\t\n", owned_obj_count);
    }
  else
    {
      printf("error in reading owned_obj_count \n");
    }

  // retuns curve type
  curve_type = dwg_ent_polyline_mesh_get_curve_type(polyline_mesh,
              &curve_type_error);
  if(curve_type_error == 0 ) // error check
    {
      printf("curve type of polyline_mesh : %ud\t\n", curve_type);
    }
  else
    {
      printf("error in reading curve type \n");
    }

  // returns m density
  m_density = dwg_ent_polyline_mesh_get_m_density(polyline_mesh,
              &m_density_error);
  if(m_density_error == 0 ) // error check
    {
      printf("M density of polyline_mesh : %ud\t\n", m_density);
    }
  else
    {
      printf("error in reading m_density \n");
    }

  // returns n density
  n_density = dwg_ent_polyline_mesh_get_n_density(polyline_mesh,
              &n_density_error);
  if(n_density_error == 0 ) // error check
    {
      printf("n density of polyline_mesh : %ud\t\n", n_density);
    }
  else
    {
      printf("error in reading n density \n");
    }

  // returns m vertice count
  mvert_count = dwg_ent_polyline_mesh_get_m_vert_count(polyline_mesh,
              &mvert_error);
  if(mvert_error == 0 ) // error check
    {
      printf("M vert count of polyline_mesh : %ud\t\n", mvert_count);
    }
  else
    {
      printf("error in reading m vert count \n");
    }

  // returns n vertice count
  nvert_count = dwg_ent_polyline_mesh_get_n_vert_count(polyline_mesh,
              &nvert_error);
  if(nvert_error == 0 ) // error check
    {
      printf("n vert count of polyline_mesh : %ud\t\n", nvert_count);
    }
  else
    {
      printf("error in reading n vert count \n");
    }
}

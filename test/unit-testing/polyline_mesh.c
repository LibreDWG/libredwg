#define DWG_TYPE DWG_TYPE_POLYLINE_MESH
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH(obj);

  printf("flags of polyline_mesh : %ud\n", polyline_mesh->flag);
  printf("owned obj count of polyline_mesh : " FORMAT_BL "\n",
          polyline_mesh->num_owned);
  printf("curve type of polyline_mesh : %ud\n", polyline_mesh->curve_type);
  printf("m density of polyline_mesh : %ud\n", polyline_mesh->m_density);
  printf("n density of polyline_mesh : %ud\n", polyline_mesh->n_density);
  printf("m verts of polyline_mesh : %ud\n", polyline_mesh->num_m_verts);
  printf("n verts of polyline_mesh : %ud\n", polyline_mesh->num_n_verts);
}

void
api_process(dwg_object *obj)
{
  int error;
  unsigned int flags, curve_type, num_m_verts, num_n_verts, m_density,
               n_density;
  BITCODE_BL num_owned;
  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH(obj);


  flags = dwg_ent_polyline_mesh_get_flag(polyline_mesh, &error);
  if ( !error )
    printf("flags of polyline_mesh : %ud\n",flags);
  else
    printf("in reading flags \n");


  num_owned = dwg_ent_polyline_mesh_get_num_owned(polyline_mesh,
                                                  &error);
  if ( !error )
    printf("owned object count of polyline_mesh : " FORMAT_BL "\n", num_owned);
  else
    printf("in reading num_owned \n");

  // returns curve type
  curve_type = dwg_ent_polyline_mesh_get_curve_type(polyline_mesh,
                                                    &error);
  if ( !error )
    printf("curve type of polyline_mesh : %ud\n", curve_type);
  else
    printf("in reading curve type \n");


  m_density = dwg_ent_polyline_mesh_get_m_density(polyline_mesh,
                                                  &error);
  if ( !error )
    printf("M density of polyline_mesh : %ud\n", m_density);
  else
    printf("in reading m_density \n");


  n_density = dwg_ent_polyline_mesh_get_n_density(polyline_mesh,
                                                  &error);
  if ( !error )
    printf("n density of polyline_mesh : %ud\n", n_density);
  else
    printf("in reading n density \n");


  num_m_verts = dwg_ent_polyline_mesh_get_num_m_verts(polyline_mesh,
                                                      &error);
  if ( !error )
    printf("num m verts of polyline_mesh : %ud\n", num_m_verts);
  else
    printf("in reading m vert count \n");


  num_n_verts = dwg_ent_polyline_mesh_get_num_n_verts(polyline_mesh,
                                                      &error);
  if ( !error )
    printf("num n verts of polyline_mesh : %ud\n", num_n_verts);
  else
    printf("in reading n vert count \n");
}

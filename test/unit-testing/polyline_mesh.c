#define DWG_TYPE DWG_TYPE_POLYLINE_MESH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS flag, curve_type, num_m_verts, num_n_verts, m_density, n_density;
  BITCODE_BL num_owned;
  BITCODE_H first_vertex, last_vertex, *vertex, seqend;
  Dwg_Version_Type version = obj->parent->header.version;

  dwg_ent_polyline_mesh *polyline_mesh = dwg_object_to_POLYLINE_MESH (obj);

  CHK_ENTITY_TYPE_W_OLD (polyline_mesh, POLYLINE_MESH, flag, BS, flag);
  CHK_ENTITY_TYPE_W_OLD (polyline_mesh, POLYLINE_MESH, curve_type, BS,
                         curve_type);
  CHK_ENTITY_TYPE_W_OLD (polyline_mesh, POLYLINE_MESH, num_owned, BL,
                         num_owned);
  CHK_ENTITY_TYPE_W_OLD (polyline_mesh, POLYLINE_MESH, m_density, BS,
                         m_density);
  CHK_ENTITY_TYPE_W_OLD (polyline_mesh, POLYLINE_MESH, n_density, BS,
                         n_density);
  CHK_ENTITY_TYPE_W_OLD (polyline_mesh, POLYLINE_MESH, num_m_verts, BS,
                         num_m_verts);
  CHK_ENTITY_TYPE_W_OLD (polyline_mesh, POLYLINE_MESH, num_n_verts, BS,
                         num_n_verts);

  if (version >= R_13 && version <= R_2000)
    {
      CHK_ENTITY_H (polyline_mesh, POLYLINE_MESH, first_vertex, first_vertex);
      CHK_ENTITY_H (polyline_mesh, POLYLINE_MESH, last_vertex, last_vertex);
    }
  if (version >= R_2004)
    {
      CHK_ENTITY_HV (polyline_mesh, POLYLINE_MESH, vertex, vertex, num_owned);
    }
  CHK_ENTITY_H (polyline_mesh, POLYLINE_MESH, seqend, seqend);
}

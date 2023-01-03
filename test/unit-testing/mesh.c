#define DWG_TYPE DWG_TYPE_MESH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS dlevel;
  BITCODE_B is_watertight;
  BITCODE_BL i, num_subdiv_vertex;
  BITCODE_3BD *subdiv_vertex;
  BITCODE_BL num_vertex;
  BITCODE_3BD *vertex;
  BITCODE_BL num_faces;
  BITCODE_BL *faces;
  BITCODE_BL num_edges;
  Dwg_MESH_edge *edges;
  BITCODE_BL num_crease;
  BITCODE_BD *crease;

  // Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_mesh *_obj = dwg_object_to_MESH (obj);

  CHK_ENTITY_TYPE (_obj, MESH, dlevel, BS);
  CHK_ENTITY_TYPE (_obj, MESH, is_watertight, B);
  CHK_ENTITY_TYPE (_obj, MESH, num_subdiv_vertex, BL);
  if (!dwg_dynapi_entity_value (_obj, "MESH", "subdiv_vertex", &subdiv_vertex,
                                NULL))
    fail ("MESH.subdiv_vertex");
  for (i = 0; i < num_subdiv_vertex; i++)
    {
      ok ("MESH.subdiv_vertex[%u]: (%f, %f, %f)", i, subdiv_vertex[i].x,
          subdiv_vertex[i].y, subdiv_vertex[i].z);
    }
  CHK_ENTITY_TYPE (_obj, MESH, num_vertex, BL);
  if (!dwg_dynapi_entity_value (_obj, "MESH", "vertex", &vertex, NULL))
    fail ("MESH.vertex");
  for (i = 0; i < num_vertex; i++)
    {
      ok ("MESH.vertex[%u]: (%f, %f, %f)", i, vertex[i].x, vertex[i].y,
          vertex[i].z);
    }
  CHK_ENTITY_TYPE (_obj, MESH, num_faces, BL);
  if (!dwg_dynapi_entity_value (_obj, "MESH", "faces", &faces, NULL))
    fail ("MESH.faces");
  for (i = 0; i < num_faces; i++)
    {
      ok ("MESH.faces[%u]: " FORMAT_BL, i, faces[i]);
    }
  CHK_ENTITY_TYPE (_obj, MESH, num_edges, BL);
  if (!dwg_dynapi_entity_value (_obj, "MESH", "edges", &edges, NULL))
    fail ("MESH.edges");
  for (i = 0; i < num_edges; i++)
    {
      CHK_SUBCLASS_TYPE (edges[i], MESH_edge, idxfrom, BL);
      CHK_SUBCLASS_TYPE (edges[i], MESH_edge, idxto, BL);
    }
  CHK_ENTITY_TYPE (_obj, MESH, num_crease, BL);
  if (!dwg_dynapi_entity_value (_obj, "MESH", "crease", &crease, NULL))
    fail ("MESH.crease");
  for (i = 0; i < num_crease; i++)
    {
      ok ("MESH.crease[%u]: %f", i, crease[i]);
    }
}

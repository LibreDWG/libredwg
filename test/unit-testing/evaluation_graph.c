// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_EVALUATION_GRAPH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL has_graph;
  BITCODE_BL unknown1;
  BITCODE_BL unknown2;
  BITCODE_BL nodeid;
  BITCODE_BL edge_flags;
  BITCODE_BL num_evalexpr;
  BITCODE_BL node_edge1;
  BITCODE_BL node_edge2;
  BITCODE_BL node_edge3;
  BITCODE_BL node_edge4;
  BITCODE_H  *evalexpr;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_evaluation_graph *_obj = dwg_object_to_EVALUATION_GRAPH (obj);

  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, has_graph, BL, has_graph);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, unknown1, BL, unknown1);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, unknown2, BL, unknown2);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, nodeid, BL, nodeid);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, edge_flags, BL, edge_flags);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, num_evalexpr, BL, num_evalexpr);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge1, BLd, node_edge1);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge2, BLd, node_edge2);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge3, BLd, node_edge3);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge4, BLd, node_edge4);
  if (!dwg_dynapi_entity_value (_obj, "EVALUATION_GRAPH", "evalexpr", &evalexpr, NULL))
    fail ("EVALUATION_GRAPH.evalexpr");
  for (BITCODE_BL i = 0; i < num_evalexpr; i++)
    {
      ok ("EVALUATION_GRAPH.evalexpr[%d]: " FORMAT_REF, i, ARGS_REF (evalexpr[i]));
    }
#endif
}

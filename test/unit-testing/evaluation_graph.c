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
  BITCODE_BLd node_edge1;
  BITCODE_BLd node_edge2;
  BITCODE_BLd node_edge3;
  BITCODE_BLd node_edge4;
  BITCODE_H  *evalexpr;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_evaluation_graph *_obj = dwg_object_to_EVALUATION_GRAPH (obj);

  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, has_graph, BL);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, unknown1, BL);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, unknown2, BL);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, nodeid, BL);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, edge_flags, BL);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, num_evalexpr, BL);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge1, BLd);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge2, BLd);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge3, BLd);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, node_edge4, BLd);
  CHK_ENTITY_HV (_obj, EVALUATION_GRAPH, evalexpr, num_evalexpr);
#endif
}

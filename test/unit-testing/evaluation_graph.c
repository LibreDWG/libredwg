// TODO unstable.
// Needed for ACSH_CLASS primitives
#define DWG_TYPE DWG_TYPE_EVALUATION_GRAPH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BLd first_nodeid, first_nodeid_copy;
  BITCODE_BL num_nodes, i;
  Dwg_EVAL_Node *nodes;
  BITCODE_B has_graph;
  BITCODE_BL num_edges;
  Dwg_EVAL_Edge *edges;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_evaluation_graph *_obj = dwg_object_to_EVALUATION_GRAPH (obj);

  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, first_nodeid, BLd);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, first_nodeid_copy, BLd);
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, num_nodes, BL);
  if (!dwg_dynapi_entity_value (_obj, "EVALUATION_GRAPH", "nodes", &nodes,
                                NULL))
    fail ("EVAL_Node.nodes");
  else
    for (i = 0; i < _obj->num_nodes; i++)
      {
        BITCODE_BLd node[4];
        CHK_SUBCLASS_TYPE (_obj->nodes[i], EVAL_Node, id, BL);
        CHK_SUBCLASS_TYPE (_obj->nodes[i], EVAL_Node, edge_flags, BL);
        CHK_SUBCLASS_TYPE (_obj->nodes[i], EVAL_Node, nextid, BLd);
        CHK_SUBCLASS_H (_obj->nodes[i], EVAL_Node, evalexpr);
        // CHK_SUBCLASS_VECTOR_TYPE (_obj->nodes[i], EVAL_Node, node, 4, BLd);
        if (dwg_dynapi_subclass_value (&_obj->nodes[i], "EVAL_Node", "node[4]",
                                       &node, NULL))
          {
            for (int j = 0; j < 4; j++)
              {
                ok ("EVAL_Node.node[%d]: " FORMAT_BLd, j, node[j]);
              }
          }
        else
          fail ("EVAL_Node.node[4]");
        CHK_SUBCLASS_TYPE (_obj->nodes[i], EVAL_Node, active_cycles, B);
      }
  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, has_graph, B);

  CHK_ENTITY_TYPE (_obj, EVALUATION_GRAPH, num_edges, BL);
  if (!dwg_dynapi_entity_value (_obj, "EVALUATION_GRAPH", "edges", &edges,
                                NULL))
    fail ("EVAL_Edge.edges");
  else
    for (i = 0; i < _obj->num_edges; i++)
      {
        BITCODE_BLd edge[5];
        CHK_SUBCLASS_TYPE (_obj->edges[i], EVAL_Edge, id, BL);
        CHK_SUBCLASS_TYPE (_obj->edges[i], EVAL_Edge, nextid, BLd);
        CHK_SUBCLASS_TYPE (_obj->edges[i], EVAL_Edge, e1, BLd);
        CHK_SUBCLASS_TYPE (_obj->edges[i], EVAL_Edge, e2, BLd);
        CHK_SUBCLASS_TYPE (_obj->edges[i], EVAL_Edge, e3, BLd);
        // CHK_SUBCLASS_VECTOR_TYPE (edges, EVAL_Edge, edge, 5, BLd);
        if (dwg_dynapi_subclass_value (&_obj->edges[i], "EVAL_Edge",
                                       "out_edge[5]", &edge, NULL))
          {
            for (int j = 0; j < 5; j++)
              {
                ok ("EVAL_Edge.out_edge[%d]: " FORMAT_BLd, j, edge[j]);
              }
          }
        else
          fail ("EVAL_Edge.out_edge[5]");
      }
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_REVOLVEDSURFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  _3DSOLID_FIELDS;

  BITCODE_BS modeler_format_version;
  BITCODE_BS u_isolines;
  BITCODE_BS v_isolines;
  BITCODE_BL i, class_version;

  BITCODE_BL id;
  BITCODE_3BD axis_point;
  BITCODE_3BD axis_vector;
  BITCODE_BD revolve_angle;
  BITCODE_BD start_angle;
  BITCODE_BD *revolved_entity_transmatrix;
  BITCODE_BD draft_angle;
  BITCODE_BD draft_start_distance;
  BITCODE_BD draft_end_distance;
  BITCODE_BD twist_angle;
  BITCODE_B solid;
  BITCODE_B close_to_axis;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_revolvedsurface *_obj = dwg_object_to_REVOLVEDSURFACE (obj);

  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, acis_empty, B);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, version, BS);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, acis_data, TF);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, wireframe_data_present, B);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, point_present, B);
  CHK_ENTITY_3RD (_obj, REVOLVEDSURFACE, point);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, isoline_present, B);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, isolines, BL);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, num_wires, BL);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, num_silhouettes, BL);
  if (!dwg_dynapi_entity_value (_obj, "REVOLVEDSURFACE", "wires", &wires,
                                NULL))
    fail ("REVOLVEDSURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("REVOLVEDSURFACE.wires[%u]: " FORMAT_BLd "\n", i,
                wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "REVOLVEDSURFACE", "silhouettes",
                                &silhouettes, NULL))
    fail ("REVOLVEDSURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("REVOLVEDSURFACE.silhouettes[%u]: " FORMAT_BL "\n", i,
                silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_H (_obj, REVOLVEDSURFACE, history_id);
    }

  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, modeler_format_version, BS);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, u_isolines, BS);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, v_isolines, BS);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, class_version, BL);

  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, id, BL);
  CHK_ENTITY_3RD (_obj, REVOLVEDSURFACE, axis_point);
  CHK_ENTITY_3RD (_obj, REVOLVEDSURFACE, axis_vector);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, revolve_angle, BD);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, start_angle, BD);
  if (!dwg_dynapi_entity_value (_obj, "REVOLVEDSURFACE",
                                "revolved_entity_transmatrix",
                                &revolved_entity_transmatrix, NULL)
      || !revolved_entity_transmatrix)
    fail ("REVOLVEDSURFACE.revolved_entity_transmatrix");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("REVOLVEDSURFACE.revolved_entity_transmatrix[%d]: %f", i,
            revolved_entity_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, draft_angle, BD);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, draft_start_distance, BD);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, draft_end_distance, BD);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, twist_angle, BD);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, solid, B);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, close_to_axis, B);
#endif
}

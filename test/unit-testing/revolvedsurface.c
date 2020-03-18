// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_REVOLVEDSURFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  //_3DSOLID_FIELDS;
  BITCODE_B acis_empty;
  BITCODE_B unknown;
  BITCODE_BS version;
  BITCODE_BL num_blocks;
  BITCODE_BL* block_size;
  char** encr_sat_data;
  BITCODE_RC* acis_data;
  BITCODE_B wireframe_data_present;
  BITCODE_B point_present;
  BITCODE_3BD point;
  BITCODE_BL num_isolines;
  BITCODE_B isoline_present;
  BITCODE_BL i, num_wires;
  Dwg_3DSOLID_wire * wires;
  BITCODE_BL num_silhouettes;
  Dwg_3DSOLID_silhouette * silhouettes;
  BITCODE_B acis_empty2;
  struct _dwg_entity_3DSOLID* extra_acis_data;
  BITCODE_BL unknown_2007;
  BITCODE_H history_id;
  BITCODE_B acis_empty_bit;

  BITCODE_BS modeler_format_version;
  BITCODE_BS u_isolines;
  BITCODE_BS v_isolines;
  BITCODE_BL class_version;

  BITCODE_BL id;
  BITCODE_3BD axis_point;
  BITCODE_3BD axis_vector;
  BITCODE_BD revolve_angle;
  BITCODE_BD start_angle;
  BITCODE_BD* revolved_entity_transmatrix;
  BITCODE_BD draft_angle;
  BITCODE_BD draft_start_distance;
  BITCODE_BD draft_end_distance;
  BITCODE_BD twist_angle;
  BITCODE_B solid;
  BITCODE_B close_to_axis;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_revolvedsurface *_obj = dwg_object_to_REVOLVEDSURFACE (obj);

  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, acis_empty, B, acis_empty);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, version, BS, version);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, acis_data, TF, acis_data);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, wireframe_data_present, B, wireframe_data_present);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, point_present, B, point_present);
  CHK_ENTITY_3RD (_obj, REVOLVEDSURFACE, point, point);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, isoline_present, B, isoline_present);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, num_isolines, BL, num_isolines);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, num_wires, BL, num_wires);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, num_silhouettes, BL, num_silhouettes);
  if (!dwg_dynapi_entity_value (_obj, "REVOLVEDSURFACE", "wires", &wires, NULL))
    fail ("REVOLVEDSURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("REVOLVEDSURFACE.wires[%u]: " FORMAT_BL "\n", i, wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "REVOLVEDSURFACE", "silhouettes", &silhouettes, NULL))
    fail ("REVOLVEDSURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("REVOLVEDSURFACE.silhouettes[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (_obj, REVOLVEDSURFACE, history_id, history_id);
    }

  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, modeler_format_version, BS, modeler_format_version);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, u_isolines, BS, u_isolines);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, v_isolines, BS, v_isolines);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, class_version, BL, class_version);
  
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, id, BL, id);
  CHK_ENTITY_3RD (_obj, REVOLVEDSURFACE, axis_point, axis_point);
  CHK_ENTITY_3RD (_obj, REVOLVEDSURFACE, axis_vector, axis_vector);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, revolve_angle, BD, revolve_angle);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, start_angle, BD, start_angle);
  if (!dwg_dynapi_entity_value (_obj, "REVOLVEDSURFACE",
                                "revolved_entity_transmatrix",
                                &revolved_entity_transmatrix, NULL))
    fail ("REVOLVEDSURFACE.revolved_entity_transmatrix");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("REVOLVEDSURFACE.revolved_entity_transmatrix[%d]: %f", i,
            revolved_entity_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, draft_angle, BD, draft_angle);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, draft_start_distance, BD, draft_start_distance);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, draft_end_distance, BD, draft_end_distance);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, twist_angle, BD, twist_angle);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, solid, B, solid);
  CHK_ENTITY_TYPE (_obj, REVOLVEDSURFACE, close_to_axis, B, close_to_axis);
#endif
}

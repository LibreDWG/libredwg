// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_SWEPTSURFACE
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

  BITCODE_BL sweep_entity_id;
  BITCODE_BL sweepdata_size;
  BITCODE_TF sweepdata;
  BITCODE_BL path_entity_id;
  BITCODE_BL pathdata_size;
  BITCODE_TF pathdata;
  BITCODE_BD *sweep_entity_transmatrix;
  BITCODE_BD *path_entity_transmatrix;

  BITCODE_BD draft_angle;
  BITCODE_BD draft_start_distance;
  BITCODE_BD draft_end_distance;
  BITCODE_BD twist_angle;
  BITCODE_BD *sweep_entity_transmatrix1;
  BITCODE_BD *path_entity_transmatrix1;
  BITCODE_BD scale_factor;
  BITCODE_BD align_angle;
  BITCODE_B is_solid;
  BITCODE_BS sweep_alignment_flags;
  BITCODE_BS path_flags;
  BITCODE_B align_start;
  BITCODE_B bank;
  BITCODE_B base_point_set;
  BITCODE_B sweep_entity_transform_computed;
  BITCODE_B path_entity_transform_computed;
  BITCODE_3BD reference_vector_for_controlling_twist;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_sweptsurface *_obj = dwg_object_to_SWEPTSURFACE (obj);

  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, acis_empty, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, version, BS);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, acis_data, TF);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, wireframe_data_present, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, point_present, B);
  CHK_ENTITY_3RD (_obj, SWEPTSURFACE, point);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, isoline_present, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, isolines, BL);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, num_wires, BL);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, num_silhouettes, BL);
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE", "wires", &wires, NULL))
    fail ("SWEPTSURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("SWEPTSURFACE.wires[%u]: " FORMAT_BLd "\n", i,
                wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE", "silhouettes",
                                &silhouettes, NULL))
    fail ("SWEPTSURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("SWEPTSURFACE.silhouettes[%u]: " FORMAT_BL "\n", i,
                silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_H (_obj, SWEPTSURFACE, history_id);
    }

  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, modeler_format_version, BS);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, u_isolines, BS);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, v_isolines, BS);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, class_version, BL);

  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "sweep_entity_transmatrix",
                                &sweep_entity_transmatrix, NULL)
      || !sweep_entity_transmatrix)
    fail ("SWEPTSURFACE.sweep_entity_transmatrix");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.sweep_entity_transmatrix[%d]: %f", i,
            sweep_entity_transmatrix[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "path_entity_transmatrix",
                                &path_entity_transmatrix, NULL)
      || !path_entity_transmatrix)
    fail ("SWEPTSURFACE.path_entity_transmatrix");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.path_entity_transmatrix[%d]: %f", i,
            path_entity_transmatrix[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "sweep_entity_transmatrix1",
                                &sweep_entity_transmatrix1, NULL)
      || !sweep_entity_transmatrix1)
    fail ("SWEPTSURFACE.sweep_entity_transmatrix1");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.sweep_entity_transmatrix1[%d]: %f", i,
            sweep_entity_transmatrix1[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "path_entity_transmatrix1",
                                &path_entity_transmatrix1, NULL)
      || !path_entity_transmatrix1)
    fail ("SWEPTSURFACE.path_entity_transmatrix1");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.path_entity_transmatrix1[%d]: %f", i,
            path_entity_transmatrix1[i]);
      }
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweep_entity_id, BL);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweepdata_size, BL);
  // CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweepdata, TF);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, path_entity_id, BL);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, pathdata_size, BL);
  // CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, pathdata, TF);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, draft_angle, BD);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, draft_start_distance, BD);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, draft_end_distance, BD);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, twist_angle, BD);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, scale_factor, BD);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, align_angle, BD);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, is_solid, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweep_alignment_flags, BS);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, path_flags, BS);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, align_start, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, bank, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, base_point_set, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweep_entity_transform_computed, B);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, path_entity_transform_computed, B);
  CHK_ENTITY_3RD (_obj, SWEPTSURFACE, reference_vector_for_controlling_twist);
#endif
}

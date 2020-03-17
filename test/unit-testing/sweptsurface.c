// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_SWEPTSURFACE
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
  BITCODE_BL num_wires;
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

  BITCODE_BL sweep_entity_id;
  BITCODE_BL sweepdata_size;
  BITCODE_TF sweepdata;
  BITCODE_BL path_entity_id;
  BITCODE_BL pathdata_size;
  BITCODE_TF pathdata;
  BITCODE_BD* sweep_entity_transmatrix;
  BITCODE_BD* path_entity_transmatrix;

  BITCODE_BD draft_angle;
  BITCODE_BD draft_start_distance;
  BITCODE_BD draft_end_distance;
  BITCODE_BD twist_angle;
  BITCODE_BD* sweep_entity_transmatrix1;
  BITCODE_BD* path_entity_transmatrix1;
  BITCODE_BD scale_factor;
  BITCODE_BD align_angle;
  BITCODE_B solid;
  BITCODE_RC sweep_alignment;
  BITCODE_B align_start;
  BITCODE_B bank;
  BITCODE_B base_point_set;
  BITCODE_B sweep_entity_transform_computed;
  BITCODE_B path_entity_transform_computed;
  BITCODE_3BD reference_vector_for_controlling_twist;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_sweptsurface *_obj = dwg_object_to_SWEPTSURFACE (obj);

  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, acis_empty, B, acis_empty);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, version, BS, version);
  CHK_ENTITY_UTF8TEXT (_obj, SWEPTSURFACE, acis_data, acis_data);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, wireframe_data_present, B, wireframe_data_present);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, point_present, B, point_present);
  CHK_ENTITY_3RD (_obj, SWEPTSURFACE, point, point);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, isoline_present, B, isoline_present);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, num_isolines, BL, num_isolines);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, num_wires, BL, num_wires);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, num_silhouettes, BL, num_silhouettes);
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE", "wires", &wires, NULL))
    fail ("SWEPTSURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("SWEPTSURFACE.wires[%u]: " FORMAT_BL "\n", i, wire[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE", "silhouettes", &silhouettes, NULL))
    fail ("SWEPTSURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("SWEPTSURFACE.silhouettes[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  else
    fail ("silhouettes");
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (_obj, SWEPTSURFACE, history_id, history_id);
    }

  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, modeler_format_version, BS, modeler_format_version);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, u_isolines, BS, u_isolines);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, v_isolines, BS, v_isolines);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, class_version, BL, class_version);

  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "sweep_entity_transmatrix",
                                &sweep_entity_transmatrix, NULL))
    fail ("SWEPTSURFACE.sweep_entity_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.sweep_entity_transmatrix[%d]: %f", i,
            sweep_entity_transmatrix[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "path_entity_transmatrix",
                                &path_entity_transmatrix, NULL))
    fail ("SWEPTSURFACE.path_entity_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.path_entity_transmatrix[%d]: %f", i,
            path_entity_transmatrixx[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "sweep_entity_transmatrix1",
                                &sweep_entity_transmatrix1, NULL))
    fail ("SWEPTSURFACE.sweep_entity_transmatrix1");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.sweep_entity_transmatrix1[%d]: %f", i,
            sweep_entity_transmatrix1[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SWEPTSURFACE",
                                "path_entity_transmatrix1",
                                &path_entity_transmatrix1, NULL))
    fail ("SWEPTSURFACE.path_entity_transmatrix1");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("SWEPTSURFACE.path_entity_transmatrix1[%d]: %f", i,
            path_entity_transmatrix1x[i]);
      }
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweep_entity_id, BL, sweep_entity_id);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweepdata_size, BL, sweepdata_size);
  //CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweepdata, TF, sweepdata);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, path_entity_id, BL, path_entity_id);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, pathdata_size, BL, pathdata_size);
  //CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, pathdata, TF, pathdata);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, draft_angle, BD, draft_angle);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, draft_start_distance, BD, draft_start_distance);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, draft_end_distance, BD, draft_end_distance);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, twist_angle, BD, twist_angle);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, scale_factor, BD, scale_factor);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, align_angle, BD, align_angle);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, solid, B, solid);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweep_alignment, RC, sweep_alignment);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, align_start, B, align_start);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, bank, B, bank);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, base_point_set, B, base_point_set);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, sweep_entity_transform_computed, B, sweep_entity_transform_computed);
  CHK_ENTITY_TYPE (_obj, SWEPTSURFACE, path_entity_transform_computed, B, path_entity_transform_computed);
  CHK_ENTITY_3RD (_obj, SWEPTSURFACE, reference_vector_for_controlling_twist, reference_vector_for_controlling_twist);
#endif
}

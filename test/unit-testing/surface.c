// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_SURFACE
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

  //? sweep_profile, taper_angle
  BITCODE_BS modeler_format_version; /*!< DXF 70 */
  BITCODE_BL bindata_size; // 90
  BITCODE_TF bindata; // 310|1
  BITCODE_BS u_isolines;         /*!< DXF 71 */
  BITCODE_BS v_isolines;         /*!< DXF 72 */
  BITCODE_BL class_version; /*!< DXF 90 */
  //sweep_options?
  BITCODE_BD height;
  BITCODE_3BD sweep_vector; /*!< DXF 10 */
  BITCODE_BD* sweep_transmatrix; /*!< DXF 40: 16x BD */
  BITCODE_BD draft_angle;   /*!< DXF 42 */
  BITCODE_BD draft_start_distance; /*!< DXF 43 */
  BITCODE_BD draft_end_distance;   /*!< DXF 44 */
  BITCODE_BD twist_angle;   /*!< DXF 45 */
  BITCODE_BD scale_factor;  /*!< DXF 48 */
  BITCODE_BD align_angle;   /*!< DXF 49 */
  BITCODE_BD* sweep_entity_transmatrix; /*!< DXF 46: 16x BD */
  BITCODE_BD* path_entity_transmatrix;  /*!< DXF 47: 16x BD */
  BITCODE_B solid;          /*!< DXF 290 */
  BITCODE_BS sweep_alignment_flags; /*!< DXF 290.
                                      0=No alignment, 1=Align sweep entity to path,
                                      2=Translate sweep entity to path,
                                      3=Translate path to sweep entity */
  BITCODE_B align_start;                        /*!< DXF 292 */
  BITCODE_B bank;                               /*!< DXF 293 */
  BITCODE_B base_point_set;                     /*!< DXF 294 */
  BITCODE_B sweep_entity_transform_computed;    /*!< DXF 295 */
  BITCODE_B path_entity_transform_computed;     /*!< DXF 296 */
  BITCODE_3BD reference_vector_for_controlling_twist; /*!< DXF 11 */
  BITCODE_H sweep_entity;
  BITCODE_H path_entity;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_surface *_obj = dwg_object_to_SURFACE (obj);

  CHK_ENTITY_TYPE (_obj, SURFACE, acis_empty, B, acis_empty);
  CHK_ENTITY_TYPE (_obj, SURFACE, version, BS, version);
  CHK_ENTITY_UTF8TEXT (_obj, SURFACE, acis_data, acis_data);
  CHK_ENTITY_TYPE (_obj, SURFACE, wireframe_data_present, B, wireframe_data_present);
  CHK_ENTITY_TYPE (_obj, SURFACE, point_present, B, point_present);
  CHK_ENTITY_3RD (_obj, SURFACE, point, point);
  CHK_ENTITY_TYPE (_obj, SURFACE, isoline_present, B, isoline_present);
  CHK_ENTITY_TYPE (_obj, SURFACE, num_isolines, BL, num_isolines);
  CHK_ENTITY_TYPE (_obj, SURFACE, num_wires, BL, num_wires);
  CHK_ENTITY_TYPE (_obj, SURFACE, num_silhouettes, BL, num_silhouettes);
  if (!dwg_dynapi_entity_value (_obj, "SURFACE", "wires", &wires, NULL))
    fail ("SURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("SURFACE.wires[%u]: " FORMAT_BL "\n", i, wire[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "SURFACE", "silhouettes", &silhouettes, NULL))
    fail ("SURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("SURFACE.silhouettes[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  else
    fail ("silhouettes");
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (_obj, SURFACE, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (_obj, SURFACE, history_id, history_id);
    }

  CHK_ENTITY_TYPE (_obj, SURFACE, modeler_format_version, BS, modeler_format_version);
  //CHK_ENTITY_TYPE (_obj, SURFACE, bindata_size, BL, bindata_size);
  //CHK_ENTITY_TYPE (_obj, SURFACE, bindata, TF, bindata); // 310|1
  CHK_ENTITY_TYPE (_obj, SURFACE, u_isolines, BS, u_isolines);
  CHK_ENTITY_TYPE (_obj, SURFACE, v_isolines, BS, v_isolines);
  CHK_ENTITY_TYPE (_obj, SURFACE, class_version, BL, class_version);
  CHK_ENTITY_TYPE (_obj, SURFACE, height, BD, height);
  CHK_ENTITY_3RD (_obj, SURFACE, sweep_vector, sweep_vector);
  if (!dwg_dynapi_entity_value (_obj, "SURFACE", "sweep_transmatrix", &sweep_transmatrix, NULL))
    fail ("SURFACE.sweep_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("SURFACE.sweep_transmatrix[%d]: %f", i, sweep_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, SURFACE, draft_angle, BD, draft_angle);
  CHK_ENTITY_TYPE (_obj, SURFACE, draft_start_distance, BD, draft_start_distance);
  CHK_ENTITY_TYPE (_obj, SURFACE, draft_end_distance, BD, draft_end_distance);
  CHK_ENTITY_TYPE (_obj, SURFACE, twist_angle, BD, twist_angle);
  CHK_ENTITY_TYPE (_obj, SURFACE, scale_factor, BD, scale_factor);
  CHK_ENTITY_TYPE (_obj, SURFACE, align_angle, BD, align_angle);
  if (!dwg_dynapi_entity_value (_obj, "SURFACE",
                                "sweep_entity_transmatrix",
                                &sweep_entity_transmatrix, NULL))
    fail ("SURFACE.sweep_entity_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("SURFACE.sweep_entity_transmatrix[%d]: %f", i,
            sweep_entity_transmatrix[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "SURFACE",
                                "path_entity_transmatrix",
                                &path_entity_transmatrix, NULL))
    fail ("SURFACE.path_entity_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("SURFACE.path_entity_transmatrix[%d]: %f", i,
            path_entity_transmatrixx[i]);
      }
  CHK_ENTITY_TYPE (_obj, SURFACE, solid, B, solid);
  CHK_ENTITY_TYPE (_obj, SURFACE, sweep_alignment_flags, BS, sweep_alignment_flags);
  CHK_ENTITY_TYPE (_obj, SURFACE, align_start, B, align_start);
  CHK_ENTITY_TYPE (_obj, SURFACE, bank, B, bank);
  CHK_ENTITY_TYPE (_obj, SURFACE, base_point_set, B, base_point_set);
  CHK_ENTITY_TYPE (_obj, SURFACE, sweep_entity_transform_computed, B,
                   sweep_entity_transform_computed);
  CHK_ENTITY_TYPE (_obj, SURFACE, path_entity_transform_computed, B,
                   path_entity_transform_computed);
  CHK_ENTITY_3RD (_obj, SURFACE,
                  reference_vector_for_controlling_twist,
                  reference_vector_for_controlling_twist);
  CHK_ENTITY_TYPE (_obj, SURFACE, sweep_entity, sweep_entity);
  CHK_ENTITY_TYPE (_obj, SURFACE, path_entity, path_entity);
#endif
}

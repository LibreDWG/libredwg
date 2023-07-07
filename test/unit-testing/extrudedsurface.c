// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_EXTRUDEDSURFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  _3DSOLID_FIELDS;

  //? sweep_profile, taper_angle
  BITCODE_BS modeler_format_version; /*!< DXF 70 */
  BITCODE_BL bindata_size;           // 90
  BITCODE_TF bindata;                // 310|1
  BITCODE_BS u_isolines;             /*!< DXF 71 */
  BITCODE_BS v_isolines;             /*!< DXF 72 */
  BITCODE_BL class_version;          /*!< DXF 90 */
  // sweep_options?
  // BITCODE_BD height;
  BITCODE_3BD sweep_vector;                           /*!< DXF 10 */
  BITCODE_BD *sweep_transmatrix;                      /*!< DXF 40: 16x BD */
  BITCODE_BD draft_angle;                             /*!< DXF 42 */
  BITCODE_BD draft_start_distance;                    /*!< DXF 43 */
  BITCODE_BD draft_end_distance;                      /*!< DXF 44 */
  BITCODE_BD twist_angle;                             /*!< DXF 45 */
  BITCODE_BD scale_factor;                            /*!< DXF 48 */
  BITCODE_BD align_angle;                             /*!< DXF 49 */
  BITCODE_BD *sweep_entity_transmatrix;               /*!< DXF 46: 16x BD */
  BITCODE_BD *path_entity_transmatrix;                /*!< DXF 47: 16x BD */
  BITCODE_B is_solid;                                 /*!< DXF 290 */
  BITCODE_BS sweep_alignment_flags;                   /*!< DXF 290 */
  BITCODE_BS path_flags;                              /*!< DXF 71 */
  BITCODE_B align_start;                              /*!< DXF 292 */
  BITCODE_B bank;                                     /*!< DXF 293 */
  BITCODE_B base_point_set;                           /*!< DXF 294 */
  BITCODE_B sweep_entity_transform_computed;          /*!< DXF 295 */
  BITCODE_B path_entity_transform_computed;           /*!< DXF 296 */
  BITCODE_3BD reference_vector_for_controlling_twist; /*!< DXF 11 */
  BITCODE_H sweep_entity;
  BITCODE_H path_entity;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_extrudedsurface *_obj = dwg_object_to_EXTRUDEDSURFACE (obj);

  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, acis_empty, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, version, BS);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, acis_data, TF);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, wireframe_data_present, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, point_present, B);
  CHK_ENTITY_3RD (_obj, EXTRUDEDSURFACE, point);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, isoline_present, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, isolines, BL);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, num_wires, BL);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, num_silhouettes, BL);
  if (!dwg_dynapi_entity_value (_obj, "EXTRUDEDSURFACE", "wires", &wires,
                                NULL))
    fail ("EXTRUDEDSURFACE.wires");
  else
    {
      for (BITCODE_BL i = 0; i < num_wires; i++)
        printf ("EXTRUDEDSURFACE.wires[%u]: " FORMAT_BLd "\n", i,
                wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "EXTRUDEDSURFACE", "silhouettes",
                                &silhouettes, NULL))
    fail ("EXTRUDEDSURFACE.silhouettes");
  else
    {
      for (BITCODE_BL i = 0; i < num_silhouettes; i++)
        printf ("EXTRUDEDSURFACE.silhouettes[%u]: " FORMAT_BL "\n", i,
                silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_H (_obj, EXTRUDEDSURFACE, history_id);
    }

  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, modeler_format_version, BS);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, bindata_size, BL);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, bindata, TF); // 310|1
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, u_isolines, BS);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, v_isolines, BS);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, class_version, BL);

  // CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, height, BD);
  CHK_ENTITY_3RD (_obj, EXTRUDEDSURFACE, sweep_vector);
  if (!dwg_dynapi_entity_value (_obj, "EXTRUDEDSURFACE", "sweep_transmatrix",
                                &sweep_transmatrix, NULL)
      || !sweep_transmatrix)
    fail ("EXTRUDEDSURFACE.sweep_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("EXTRUDEDSURFACE.sweep_transmatrix[%d]: %f", i,
            sweep_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, draft_angle, BD);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, draft_start_distance, BD);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, draft_end_distance, BD);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, twist_angle, BD);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, scale_factor, BD);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, align_angle, BD);
  if (!dwg_dynapi_entity_value (_obj, "EXTRUDEDSURFACE",
                                "sweep_entity_transmatrix",
                                &sweep_entity_transmatrix, NULL)
      || !sweep_entity_transmatrix)
    fail ("EXTRUDEDSURFACE.sweep_entity_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("EXTRUDEDSURFACE.sweep_entity_transmatrix[%d]: %f", i,
            sweep_entity_transmatrix[i]);
      }
  if (!dwg_dynapi_entity_value (_obj, "EXTRUDEDSURFACE",
                                "path_entity_transmatrix",
                                &path_entity_transmatrix, NULL)
      || !path_entity_transmatrix)
    fail ("EXTRUDEDSURFACE.path_entity_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("EXTRUDEDSURFACE.path_entity_transmatrix[%d]: %f", i,
            path_entity_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, is_solid, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, sweep_alignment_flags, BS);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, path_flags, BS);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, align_start, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, bank, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, base_point_set, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, sweep_entity_transform_computed, B);
  CHK_ENTITY_TYPE (_obj, EXTRUDEDSURFACE, path_entity_transform_computed, B);
  CHK_ENTITY_3RD (_obj, EXTRUDEDSURFACE,
                  reference_vector_for_controlling_twist);
  CHK_ENTITY_H (_obj, EXTRUDEDSURFACE, sweep_entity);
  CHK_ENTITY_H (_obj, EXTRUDEDSURFACE, path_entity);
#endif
}

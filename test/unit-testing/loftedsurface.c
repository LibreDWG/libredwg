// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_LOFTEDSURFACE
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
  BITCODE_BD* loft_entity_transmatrix;
  BITCODE_BL plane_normal_lofting_type;
  BITCODE_BD start_draft_angle;
  BITCODE_BD end_draft_angle;
  BITCODE_BD start_draft_magnitude;
  BITCODE_BD end_draft_magnitude;
  BITCODE_B arc_length_parameterization;
  BITCODE_B no_twist;
  BITCODE_B align_direction;
  BITCODE_B simple_surfaces;
  BITCODE_B closed_surfaces;
  BITCODE_B solid;
  BITCODE_B ruled_surface;
  BITCODE_B virtual_guide;
  BITCODE_BS num_cross_sections;
  BITCODE_BS num_guide_curves;
  BITCODE_H *cross_sections;
  BITCODE_H *guide_curves;
  BITCODE_H path_curve;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_loftedsurface *_obj = dwg_object_to_LOFTEDSURFACE (obj);

  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, acis_empty, B, acis_empty);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, version, BS, version);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, acis_data, TF, acis_data);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, wireframe_data_present, B, wireframe_data_present);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, point_present, B, point_present);
  CHK_ENTITY_3RD (_obj, LOFTEDSURFACE, point, point);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, isoline_present, B, isoline_present);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, num_isolines, BL, num_isolines);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, num_wires, BL, num_wires);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, num_silhouettes, BL, num_silhouettes);
  if (!dwg_dynapi_entity_value (_obj, "LOFTEDSURFACE", "wires", &wires, NULL))
    fail ("LOFTEDSURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("LOFTEDSURFACE.wires[%u]: " FORMAT_BL "\n", i, wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "LOFTEDSURFACE", "silhouettes", &silhouettes, NULL))
    fail ("LOFTEDSURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("LOFTEDSURFACE.silhouettes[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (_obj, LOFTEDSURFACE, history_id, history_id);
    }

  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, modeler_format_version, BS, modeler_format_version);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, u_isolines, BS, u_isolines);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, v_isolines, BS, v_isolines);

  if (!dwg_dynapi_entity_value (_obj, "LOFTEDSURFACE",
                                "loft_entity_transmatrix",
                                &loft_entity_transmatrix, NULL))
    fail ("LOFTEDSURFACE.loft_entity_transmatrix");
  else
    for (i = 0; i < 16; i++)
      {
        ok ("LOFTEDSURFACE.loft_entity_transmatrix[%d]: %f", i,
            loft_entity_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, plane_normal_lofting_type, BL, plane_normal_lofting_type);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, start_draft_angle, BD, start_draft_angle);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, end_draft_angle, BD, end_draft_angle);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, start_draft_magnitude, BD, start_draft_magnitude);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, end_draft_magnitude, BD, end_draft_magnitude);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, arc_length_parameterization, B, arc_length_parameterization);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, no_twist, B, no_twist);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, align_direction, B, align_direction);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, simple_surfaces, B, simple_surfaces);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, closed_surfaces, B, closed_surfaces);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, solid, B, solid);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, ruled_surface, B, ruled_surface);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, virtual_guide, B, virtual_guide);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, num_cross_sections, BS, num_cross_sections);
  CHK_ENTITY_TYPE (_obj, LOFTEDSURFACE, num_guide_curves, BS, num_guide_curves);
  CHK_ENTITY_H (_obj, LOFTEDSURFACE, path_curve, path_curve);
  if (!dwg_dynapi_entity_value (_obj, "LOFTEDSURFACE", "cross_sections", &cross_sections, NULL))
    fail ("LOFTEDSURFACE.cross_sections");
  for (i = 0; i < num_cross_sections; i++)
    {
      ok ("LOFTEDSURFACE.cross_sections[%d]: " FORMAT_REF, i, ARGS_REF (cross_sections[i]));
    }
  if (!dwg_dynapi_entity_value (_obj, "LOFTEDSURFACE", "guide_curves", &guide_curves, NULL))
    fail ("LOFTEDSURFACE.guide_curves");
  for (i = 0; i < num_guide_curves; i++)
    {
      ok ("LOFTEDSURFACE.guide_curves[%d]: " FORMAT_REF, i, ARGS_REF (guide_curves[i]));
    }
#endif
}

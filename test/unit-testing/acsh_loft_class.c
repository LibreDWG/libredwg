#define DWG_TYPE DWG_TYPE_ACSH_LOFT_CLASS
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BD *trans;
  BITCODE_BL num_crosssects;
  Dwg_ACSH_LOFT_profile *crosssects;
  BITCODE_BL num_guides;
  Dwg_ACSH_LOFT_profile *guides;
  BITCODE_BL shlf_bl98;
  BITCODE_BD start_draft_angle, end_draft_angle;
  BITCODE_BD start_draft_magnitude, end_draft_magnitude;
  BITCODE_B arc_length_parameterization, no_twist, align_direction;
  BITCODE_B simple_surfaces, closed_surfaces, solid, ruled_surface;
  BITCODE_B virtual_guide;
  BITCODE_BL plane_normal_lofting_type;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_loft_class *_obj = dwg_object_to_ACSH_LOFT_CLASS (obj);

  CHK_EVALEXPR (ACSH_LOFT_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, num_crosssects, BL);
  if (!dwg_dynapi_entity_value (_obj, "ACSH_LOFT_CLASS", "crosssects",
                                &crosssects, NULL))
    fail ("ACSH_LOFT_CLASS.crosssects");
  else if (crosssects)
    {
      BITCODE_BL i;
      for (i = 0; i < num_crosssects; i++)
        {
          CHK_SUBCLASS_TYPE (crosssects[i], ACSH_LOFT_profile, method, BL);
          CHK_SUBCLASS_TYPE (crosssects[i], ACSH_LOFT_profile, size, BL);
          if (crosssects[i].size && !crosssects[i].data)
            fail ("ACSH_LOFT_CLASS.crosssects[%u].data", i);
        }
    }
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, num_guides, BL);
  if (!dwg_dynapi_entity_value (_obj, "ACSH_LOFT_CLASS", "guides", &guides,
                                NULL))
    fail ("ACSH_LOFT_CLASS.guides");
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, shlf_bl98, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, start_draft_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, end_draft_angle, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, start_draft_magnitude, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, end_draft_magnitude, BD);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, arc_length_parameterization, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, no_twist, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, align_direction, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, simple_surfaces, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, closed_surfaces, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, solid, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, ruled_surface, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, virtual_guide, B);
  CHK_ENTITY_TYPE (_obj, ACSH_LOFT_CLASS, plane_normal_lofting_type, BL);
}

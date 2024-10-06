#define DWG_TYPE DWG_TYPE_ATTRIB
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  double elevation, thickness, rotation, height, oblique_angle, width_factor;
  BITCODE_BS generation, vert_alignment, horiz_alignment, is_really_locked,
    num_secondary_atts, i;
  BITCODE_RC dataflags, flags, is_locked_in_block, keep_duplicate_records, mtext_type;
  BITCODE_B lock_position_flag;
  char *text_value;
  dwg_point_3d extrusion;
  dwg_point_2d ins_pt, alignment_pt;
  BITCODE_H style;
  BITCODE_H *secondary_atts, *hdls;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB (obj);

  CHK_ENTITY_UTF8TEXT (attrib, ATTRIB, text_value);
#ifdef USE_DEPRECATED_API
  if (version < R_2007
      && (strcmp (dwg_ent_attrib_get_text (attrib, &error), text_value)
          || error))
    fail ("old API dwg_ent_attrib_get_text");
#endif
  CHK_ENTITY_2RD (attrib, ATTRIB, ins_pt);
  CHK_ENTITY_2RD (attrib, ATTRIB, alignment_pt);
  CHK_ENTITY_3RD_W_OLD (attrib, ATTRIB, extrusion);
  CHK_ENTITY_TYPE (attrib, ATTRIB, elevation, BD);
  CHK_ENTITY_TYPE (attrib, ATTRIB, dataflags, RC);
  CHK_ENTITY_TYPE_W_OLD (attrib, ATTRIB, flags, RC);
  CHK_ENTITY_TYPE_W_OLD (attrib, ATTRIB, height, RD);
  CHK_ENTITY_TYPE_W_OLD (attrib, ATTRIB, thickness, RD);
  CHK_ENTITY_TYPE_W_OLD (attrib, ATTRIB, rotation, RD);
  CHK_ENTITY_MAX (attrib, ATTRIB, rotation, RD, MAX_ANGLE);
  CHK_ENTITY_TYPE (attrib, ATTRIB, oblique_angle, RD);
  CHK_ENTITY_MAX (attrib, ATTRIB, oblique_angle, RD, MAX_ANGLE);
  CHK_ENTITY_TYPE (attrib, ATTRIB, width_factor, RD);
  CHK_ENTITY_TYPE (attrib, ATTRIB, generation, BS);

  CHK_ENTITY_TYPE_W_OLD (attrib, ATTRIB, vert_alignment, BS);
  CHK_ENTITY_TYPE_W_OLD (attrib, ATTRIB, horiz_alignment, BS);
  CHK_ENTITY_H (attrib, ATTRIB, style);
  if (version >= R_2007)
    {
      CHK_ENTITY_TYPE (attrib, ATTRIB, is_locked_in_block, RC);
      CHK_ENTITY_MAX (attrib, ATTRIB, is_locked_in_block, RC, 1);
      CHK_ENTITY_TYPE (attrib, ATTRIB, keep_duplicate_records, RC);
      CHK_ENTITY_MAX (attrib, ATTRIB, keep_duplicate_records, RC, 1);
      CHK_ENTITY_TYPE (attrib, ATTRIB, lock_position_flag, B);
    }
  if (version >= R_2018)
    {
      CHK_ENTITY_TYPE (attrib, ATTRIB, mtext_type, RC);
      CHK_ENTITY_MAX (attrib, ATTRIB, mtext_type, RC, 4);
      CHK_ENTITY_TYPE (attrib, ATTRIB, is_really_locked, BS);
      CHK_ENTITY_TYPE (attrib, ATTRIB, num_secondary_atts, BS);
      if (!dwg_dynapi_entity_value (attrib, "ATTRIB", "secondary_atts",
                                    &secondary_atts, NULL))
        fail ("ATTRIB.secondary_atts");
      hdls = attrib->secondary_atts;
      for (i = 0; i < num_secondary_atts; i++)
        {
          if (hdls[i] == secondary_atts[i])
            ok ("ATTRIB.secondary_atts[%d]: " FORMAT_REF, i,
                ARGS_REF (secondary_atts[i]));
          else
            fail ("ATTRIB.secondary_atts[%d]: " FORMAT_REF, i,
                  ARGS_REF (secondary_atts[i]));
        }
    }
}

#define DWG_TYPE DWG_TYPE_ATTDEF
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0;
  double elevation, thickness, rotation, height, oblique_angle, width_factor,
      rdvalue;
  BITCODE_BS generation, vert_alignment, horiz_alignment, field_length,
      annotative_data_size, annotative_short, bsvalue;
  BITCODE_RC dataflags, class_version, type, flags, attdef_class_version,
      rcvalue;
  BITCODE_B lock_position_flag;
  char *tag, *default_value, *prompt;
  dwg_point_3d extrusion, pt3d;
  dwg_point_2d ins_pt, alignment_pt, pt2d;
  BITCODE_H style, annotative_app;
  BITCODE_H mtext_handles;
  Dwg_Version_Type version = obj->parent->header.version;
  int isnew;

  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF (obj);

  CHK_ENTITY_UTF8TEXT_W_OLD (attdef, ATTDEF, default_value);
  CHK_ENTITY_UTF8TEXT (attdef, ATTDEF, tag);
  CHK_ENTITY_UTF8TEXT (attdef, ATTDEF, prompt);

  CHK_ENTITY_2RD (attdef, ATTDEF, ins_pt);
  CHK_ENTITY_2RD (attdef, ATTDEF, alignment_pt);
  CHK_ENTITY_3RD_W_OLD (attdef, ATTDEF, extrusion);
  CHK_ENTITY_TYPE (attdef, ATTDEF, elevation, BD);
  CHK_ENTITY_TYPE (attdef, ATTDEF, dataflags, RC);
  CHK_ENTITY_TYPE_W_OLD (attdef, ATTDEF, height, RD);
  CHK_ENTITY_TYPE_W_OLD (attdef, ATTDEF, thickness, RD);
  CHK_ENTITY_TYPE_W_OLD (attdef, ATTDEF, rotation, RD);
  CHK_ENTITY_MAX (attdef, ATTDEF, rotation, RD, MAX_ANGLE);
  CHK_ENTITY_TYPE (attdef, ATTDEF, oblique_angle, RD);
  CHK_ENTITY_MAX (attdef, ATTDEF, oblique_angle, RD, MAX_ANGLE);
  CHK_ENTITY_TYPE (attdef, ATTDEF, width_factor, RD);
  CHK_ENTITY_TYPE (attdef, ATTDEF, generation, BS);

  CHK_ENTITY_TYPE_W_OLD (attdef, ATTDEF, vert_alignment, BS);
  CHK_ENTITY_TYPE_W_OLD (attdef, ATTDEF, horiz_alignment, BS);
  CHK_ENTITY_TYPE (attdef, ATTDEF, field_length, BS);
  CHK_ENTITY_TYPE (attdef, ATTDEF, flags, RC);
  CHK_ENTITY_H (attdef, ATTDEF, style);
  if (version >= R_2010)
    {
      CHK_ENTITY_TYPE (attdef, ATTDEF, class_version, RC);
      CHK_ENTITY_TYPE (attdef, ATTDEF, attdef_class_version, RC);
    }
  if (version >= R_2018)
    {
      CHK_ENTITY_H (attdef, ATTDEF, mtext_handles);
      CHK_ENTITY_H (attdef, ATTDEF, annotative_app);
      CHK_ENTITY_TYPE (attdef, ATTDEF, annotative_data_size, BS);
      CHK_ENTITY_TYPE (attdef, ATTDEF, annotative_short, BS);
      CHK_ENTITY_TYPE (attdef, ATTDEF, type, RC);
    }
  if (version >= R_2007)
    {
      CHK_ENTITY_TYPE (attdef, ATTDEF, lock_position_flag, B);
    }
}

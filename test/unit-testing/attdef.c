#define DWG_TYPE DWG_TYPE_ATTDEF
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0;
  double elevation, thickness, rotation, height, oblique_ang, width_factor,
      rdvalue;
  BITCODE_BS generation, vert_align, horiz_align, field_length,
      annotative_data_size, annotative_short, bsvalue;
  BITCODE_RC dataflags, class_version, type, flags, attdef_class_version,
      rcvalue;
  BITCODE_B lock_position_flag;
  char *text_value, text1;
  dwg_point_3d ext, pt3d;
  dwg_point_2d ins_pt, alignment_pt, pt2d;
  BITCODE_H style, annotative_app;
  BITCODE_H mtext_handles;
  Dwg_Version_Type version = obj->parent->header.version;

  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF (obj);

  CHK_ENTITY_UTF8TEXT_W_OLD (attdef, ATTDEF, default_value, text_value);
  /*if (version < R_2007 &&
      (strcmp (dwg_ent_attdef_get_default_value (attdef, &error), text_value)
       || error))
       fail ("old API dwg_ent_attdef_get_default_value"); */
  CHK_ENTITY_UTF8TEXT (attdef, ATTDEF, tag, text_value);
  CHK_ENTITY_UTF8TEXT (attdef, ATTDEF, prompt, text_value);

  CHK_ENTITY_2RD (attdef, ATTDEF, insertion_pt, ins_pt);
  dwg_ent_attdef_get_insertion_point (attdef, &pt2d, &error);
  if (error || memcmp (&ins_pt, &pt2d, sizeof (ins_pt)))
    fail ("old API dwg_ent_attdef_get_insertion_point");
  CHK_ENTITY_2RD (attdef, ATTDEF, alignment_pt, alignment_pt);
  CHK_ENTITY_3RD (attdef, ATTDEF, extrusion, ext);
  dwg_ent_attdef_get_extrusion (attdef, &pt3d, &error);
  if (error || memcmp (&ext, &pt3d, sizeof (ext)))
    fail ("old API dwg_ent_attdef_get_extrusion");
  CHK_ENTITY_TYPE (attdef, ATTDEF, elevation, BD, elevation);
  CHK_ENTITY_TYPE (attdef, ATTDEF, dataflags, RC, dataflags);
  CHK_ENTITY_TYPE (attdef, ATTDEF, height, RD, height);
  rdvalue = dwg_ent_attdef_get_height (attdef, &error);
  if (error || height != rdvalue)
    fail ("old API dwg_ent_attdef_get_height");
  CHK_ENTITY_TYPE (attdef, ATTDEF, thickness, RD, thickness);
  rdvalue = dwg_ent_attdef_get_thickness (attdef, &error);
  if (error || thickness != rdvalue)
    fail ("old API dwg_ent_attdef_get_thickness");
  CHK_ENTITY_TYPE (attdef, ATTDEF, rotation, RD, rotation);
  rdvalue = dwg_ent_attdef_get_rotation (attdef, &error);
  if (error || rotation != rdvalue)
    fail ("old API dwg_ent_attdef_get_rotation");
  CHK_ENTITY_TYPE (attdef, ATTDEF, oblique_ang, RD, oblique_ang);
  CHK_ENTITY_TYPE (attdef, ATTDEF, width_factor, RD, width_factor);
  CHK_ENTITY_TYPE (attdef, ATTDEF, generation, BS, generation);

  CHK_ENTITY_TYPE (attdef, ATTDEF, vert_alignment, BS, vert_align);
  bsvalue = dwg_ent_attdef_get_vert_alignment (attdef, &error);
  if (error || vert_align != bsvalue)
    fail ("old API dwg_ent_attdef_get_vert_alignment");
  CHK_ENTITY_TYPE (attdef, ATTDEF, horiz_alignment, BS, horiz_align);
  bsvalue = dwg_ent_attdef_get_horiz_alignment (attdef, &error);
  if (error || horiz_align != bsvalue)
    fail ("old API dwg_ent_attdef_horiz_alignment");
  CHK_ENTITY_TYPE (attdef, ATTDEF, field_length, BS, field_length);
  CHK_ENTITY_TYPE (attdef, ATTDEF, flags, RC, flags);
  CHK_ENTITY_H (attdef, ATTDEF, style, style);
  if (version >= R_2010)
    {
      CHK_ENTITY_TYPE (attdef, ATTDEF, class_version, RC, class_version);
      CHK_ENTITY_TYPE (attdef, ATTDEF, attdef_class_version, RC,
                       attdef_class_version);
    }
  if (version >= R_2018)
    {
      CHK_ENTITY_H (attdef, ATTDEF, mtext_handles, mtext_handles);
      CHK_ENTITY_H (attdef, ATTDEF, annotative_app, annotative_app);

      CHK_ENTITY_TYPE (attdef, ATTDEF, annotative_data_size, BS,
                       annotative_data_size);
      CHK_ENTITY_TYPE (attdef, ATTDEF, annotative_short, BS, annotative_short);

      CHK_ENTITY_TYPE (attdef, ATTDEF, type, RC, type);
    }
  if (version >= R_2007)
    {
      CHK_ENTITY_TYPE (attdef, ATTDEF, lock_position_flag, B,
                       lock_position_flag);
    }
}

#define DWG_TYPE DWG_TYPE_ATTDEF
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  double elevation, thickness, rotation, height, oblique_angle, width_factor;
  BITCODE_BS generation, vert_alignment, horiz_alignment, field_length,
      annotative_data_size, i;
  BITCODE_RC dataflags, flags, is_locked_in_block, keep_duplicate_records,
      mtext_type;
  BITCODE_B lock_position_flag, is_really_locked;
  char *tag, *default_value, *prompt;
  dwg_point_3d extrusion, pt3d;
  dwg_point_2d ins_pt, alignment_pt, pt2d;
  BITCODE_H style;
  Dwg_AcDbMTextObjectEmbedded mtext;
  Dwg_Version_Type version = obj->parent->header.version;

  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF (obj);

  CHK_ENTITY_UTF8TEXT_W_OLD (attdef, ATTDEF, default_value);
  CHK_ENTITY_UTF8TEXT (attdef, ATTDEF, tag);
  CHK_ENTITY_UTF8TEXT (attdef, ATTDEF, prompt);

  CHK_ENTITY_2RD (attdef, ATTDEF, ins_pt);
  CHK_ENTITY_2RD (attdef, ATTDEF, alignment_pt);
  CHK_ENTITY_3RD_W_OLD (attdef, ATTDEF, extrusion);
  CHK_ENTITY_TYPE (attdef, ATTDEF, elevation, BD);
  CHK_ENTITY_TYPE (attdef, ATTDEF, dataflags, RC);
  CHK_ENTITY_TYPE_W_OLD (attdef, ATTDEF, flags, RC);
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
  if (version >= R_2007)
    {
      CHK_ENTITY_TYPE (attdef, ATTDEF, is_locked_in_block, RC);
      CHK_ENTITY_MAX (attdef, ATTDEF, is_locked_in_block, RC, 1);
      CHK_ENTITY_TYPE (attdef, ATTDEF, keep_duplicate_records, RC);
      CHK_ENTITY_MAX (attdef, ATTDEF, keep_duplicate_records, RC, 1);
      CHK_ENTITY_TYPE (attdef, ATTDEF, lock_position_flag, B);
    }
  if (version >= R_2018)
    {
      CHK_ENTITY_TYPE (attdef, ATTDEF, mtext_type, RC);
      CHK_ENTITY_MAX (attdef, ATTDEF, mtext_type, RC, 4);
      CHK_ENTITY_TYPE (attdef, ATTDEF, is_really_locked, B);
      CHK_ENTITY_TYPE (attdef, ATTDEF, annotative_data_size, BS);
    }
  if (attdef->mtext_type > 1)
    {
      if (!dwg_dynapi_entity_value (attdef, "ATTDEF", "mtext", &mtext, NULL))
        fail ("ATTDEF.mtext");
      else
        {
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, attachment, BS);
          CHK_SUBCLASS_3RD (mtext, AcDbMTextObjectEmbedded, ins_pt);
          // CHK_SUBCLASS_3RD (mtext, AcDbMTextObjectEmbedded, x_axis_dir);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, rect_height, BD);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, rect_width, BD);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded, column_type, BS);
          CHK_SUBCLASS_TYPE (mtext, AcDbMTextObjectEmbedded,
                             num_column_heights, BS);
        }
    }
}

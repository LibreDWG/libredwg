//DEBUGGING
#define DWG_TYPE DWG_TYPE_ATEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0;
  BITCODE_RD elevation, thickness, rotation, height, oblique_ang, width_factor,
      rdvalue;
  BITCODE_BS generation, vert_align, horiz_align, bsvalue;
  BITCODE_RC dataflags, rcvalue;
  char *text_value;
  int isnew;
  dwg_point_3d ext;
  dwg_point_2d pt2d, ins_pt, alignment_pt;
  BITCODE_H style;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_atext *atext = dwg_object_to_AATEXT (obj);

  CHK_ENTITY_UTF8ATEXT (text, ATEXT, text_value, text_value);
  CHK_ENTITY_2RD (text, ATEXT, insertion_pt, ins_pt);
  CHK_ENTITY_2RD (atext, ATEXT, alignment_pt, alignment_pt);
  CHK_ENTITY_3RD (atext, ATEXT, extrusion, ext);
  CHK_ENTITY_TYPE (atext, ATEXT, elevation, BD, elevation);
  CHK_ENTITY_TYPE (atext, ATEXT, dataflags, RC, dataflags);
  CHK_ENTITY_TYPE (atext, ATEXT, height, RD, height);
  CHK_ENTITY_TYPE (atext, ATEXT, thickness, RD, thickness);
  CHK_ENTITY_TYPE (atext, ATEXT, rotation, RD, rotation);
  CHK_ENTITY_MAX (atext, ATEXT, rotation, RD, 6.284);
  CHK_ENTITY_TYPE (atext, ATEXT, oblique_ang, RD, oblique_ang);
  CHK_ENTITY_MAX (atext, ATEXT, oblique_ang, RD, 6.284);
  CHK_ENTITY_TYPE (atext, ATEXT, width_factor, RD, width_factor);
  CHK_ENTITY_TYPE (atext, ATEXT, generation, BS, generation);

  CHK_ENTITY_TYPE (atext, ATEXT, vert_alignment, BS, vert_align);
  CHK_ENTITY_TYPE (atext, ATEXT, horiz_alignment, BS, horiz_align);
  CHK_ENTITY_H (atext, ATEXT, style, style);
}

#define DWG_TYPE DWG_TYPE_ATTRIB
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0;
  double elevation, thickness, rotation, height, oblique_ang, width_factor,
      rdvalue;
  BITCODE_BS generation, vert_align, horiz_align, bsvalue;
  BITCODE_RC dataflags, rcvalue;
  char *text_value;
  dwg_point_3d ext, pt3d;
  dwg_point_2d ins_pt, alignment_pt, pt2d;
  BITCODE_H style;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB (obj);

  CHK_ENTITY_UTF8TEXT (attrib, ATTRIB, text_value, text_value);
  if (version < R_2007
      && (strcmp (dwg_ent_attrib_get_text (attrib, &error), text_value)
          || error))
    fail ("old API dwg_ent_attrib_get_text");
  CHK_ENTITY_2RD (attrib, ATTRIB, insertion_pt, ins_pt);
  dwg_ent_attrib_get_insertion_point (attrib, &pt2d, &error);
  if (error || memcmp (&ins_pt, &pt2d, sizeof (ins_pt)))
    fail ("old API dwg_ent_attrib_get_insertion_point");
  CHK_ENTITY_2RD (attrib, ATTRIB, alignment_pt, alignment_pt);
  CHK_ENTITY_3RD (attrib, ATTRIB, extrusion, ext);
  dwg_ent_attrib_get_extrusion (attrib, &pt3d, &error);
  if (error || memcmp (&ext, &pt3d, sizeof (ext)))
    fail ("old API dwg_ent_attrib_get_extrusion");
  CHK_ENTITY_TYPE (attrib, ATTRIB, elevation, BD, elevation);
  CHK_ENTITY_TYPE (attrib, ATTRIB, dataflags, RC, dataflags);
  CHK_ENTITY_TYPE (attrib, ATTRIB, height, RD, height);
  rdvalue = dwg_ent_attrib_get_height (attrib, &error);
  if (error || height != rdvalue)
    fail ("old API dwg_ent_attrib_get_height");
  CHK_ENTITY_TYPE (attrib, ATTRIB, thickness, RD, thickness);
  rdvalue = dwg_ent_attrib_get_thickness (attrib, &error);
  if (error || thickness != rdvalue)
    fail ("old API dwg_ent_attrib_get_thickness");
  CHK_ENTITY_TYPE (attrib, ATTRIB, rotation, RD, rotation);
  rdvalue = dwg_ent_attrib_get_rotation (attrib, &error);
  if (error || rotation != rdvalue)
    fail ("old API dwg_ent_attrib_get_rotation");
  CHK_ENTITY_TYPE (attrib, ATTRIB, oblique_ang, RD, oblique_ang);
  CHK_ENTITY_TYPE (attrib, ATTRIB, width_factor, RD, width_factor);
  CHK_ENTITY_TYPE (attrib, ATTRIB, generation, BS, generation);

  CHK_ENTITY_TYPE (attrib, ATTRIB, vert_alignment, BS, vert_align);
  bsvalue = dwg_ent_attrib_get_vert_alignment (attrib, &error);
  if (error || vert_align != bsvalue)
    fail ("old API dwg_ent_attrib_get_vert_alignment");
  CHK_ENTITY_TYPE (attrib, ATTRIB, horiz_alignment, BS, horiz_align);
  bsvalue = dwg_ent_attrib_get_horiz_alignment (attrib, &error);
  if (error || horiz_align != bsvalue)
    fail ("old API dwg_ent_attrib_horiz_alignment");
  CHK_ENTITY_H (attrib, ATTRIB, style, style);
}

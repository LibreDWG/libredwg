#define DWG_TYPE DWG_TYPE_ATTRIB
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  double elevation, thickness, rotation, height, oblique_angle, width_factor,
      rdvalue;
  BITCODE_BS generation, vert_alignment, horiz_alignment, bsvalue;
  BITCODE_RC dataflags, rcvalue;
  char *text_value;
  dwg_point_3d extrusion;
  dwg_point_2d ins_pt, alignment_pt;
  BITCODE_H style;

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
}

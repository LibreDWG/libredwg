#define DWG_TYPE DWG_TYPE_TEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_RD elevation, thickness, rotation, height, oblique_ang,
    width_factor, rdvalue;
  BITCODE_BS generation, vert_align, horiz_align, bsvalue;
  BITCODE_RC dataflags, rcvalue;
  char *text_value;
  dwg_point_3d ext, pt3d;
  dwg_point_2d pt2d, ins_pt, alignment_pt;
  BITCODE_H style;

  dwg_ent_text *text = dwg_object_to_TEXT (obj);

  CHK_ENTITY_UTF8TEXT (text, TEXT, text_value, text_value);
  if (strcmp (dwg_ent_text_get_text (text, &error), text_value))
    {
      printf ("Error with old API dwg_ent_text_get_text\n");
      exit (1);
    }

  CHK_ENTITY_2RD (text, TEXT, insertion_pt, ins_pt);
  dwg_ent_text_get_insertion_point (text, &pt2d, &error);
  if (error || memcmp (&ins_pt, &pt2d, sizeof (ins_pt)))
    {
      printf ("Error with old API dwg_ent_text_get_insertion_point\n");
      exit (1);
    }
  CHK_ENTITY_2RD (text, TEXT, alignment_pt, alignment_pt);
  CHK_ENTITY_3RD (text, TEXT, extrusion, ext);
  dwg_ent_text_get_extrusion (text, &pt3d, &error);
  if (error || memcmp (&ext, &pt3d, sizeof (ext)))
    {
      printf ("Error with old API dwg_ent_text_get_extrusion\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (text, TEXT, elevation, BD, elevation);
  CHK_ENTITY_TYPE (text, TEXT, dataflags, RC, dataflags);
  CHK_ENTITY_TYPE (text, TEXT, height, RD, height);
  rdvalue = dwg_ent_text_get_height (text, &error);
  if (error || height != rdvalue)
    {
      printf ("Error with old API dwg_ent_text_get_height\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (text, TEXT, thickness, RD, thickness);
  rdvalue = dwg_ent_text_get_thickness (text, &error);
  if (error || thickness != rdvalue)
    {
      printf ("Error with old API dwg_ent_text_get_thickness\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (text, TEXT, rotation, RD, rotation);
  rdvalue = dwg_ent_text_get_rotation (text, &error);
  if (error || rotation != rdvalue)
    {
      printf ("Error with old API dwg_ent_text_get_rotation\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (text, TEXT, oblique_ang, RD, oblique_ang);
  CHK_ENTITY_TYPE (text, TEXT, width_factor, RD, width_factor);
  CHK_ENTITY_TYPE (text, TEXT, generation, BS, generation);

  CHK_ENTITY_TYPE (text, TEXT, vert_alignment, BS, vert_align);
  bsvalue = dwg_ent_text_get_vert_alignment (text, &error);
  if (error || vert_align != bsvalue)
    {
      printf ("Error with old API dwg_ent_text_get_vert_alignment\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (text, TEXT, horiz_alignment, BS, horiz_align);
  bsvalue = dwg_ent_text_get_horiz_alignment (text, &error);
  if (error || horiz_align != bsvalue)
    {
      printf ("Error with old API dwg_ent_text_horiz_alignment\n");
      exit (1);
    }
  CHK_ENTITY_H (text, TEXT, style, style);
}

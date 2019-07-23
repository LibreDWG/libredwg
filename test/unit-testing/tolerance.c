#define DWG_TYPE DWG_TYPE_TOLERANCE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double height, dimgap;
  dwg_point_3d ins_pt, x_dir, ext, pt3d;
  char *text_string;
  BITCODE_BS unknown_short;
  BITCODE_H dimstyle; 

  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE (obj);

  CHK_ENTITY_UTF8TEXT (tolerance, TOLERANCE, text_string, text_string);
  if (strcmp (dwg_ent_tolerance_get_text_string (tolerance, &error), text_string))
    {
      printf ("Error with old API dwg_ent_tolerance_get_text_string\n");
      exit (1);
    }
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, ins_pt, ins_pt);
  CHK_ENTITY_TYPE_W_OLD (tolerance, TOLERANCE, height, BD, height);
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, extrusion, ext);
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, x_direction, x_dir);
  CHK_ENTITY_TYPE_W_OLD (tolerance, TOLERANCE, dimgap, BD, dimgap);
  CHK_ENTITY_TYPE (tolerance, TOLERANCE, unknown_short, BS, unknown_short);
  CHK_ENTITY_H (tolerance, TOLERANCE, dimstyle, dimstyle);
}

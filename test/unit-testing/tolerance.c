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

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE (obj);

  if (tolerance->text_string)
    {
      CHK_ENTITY_UTF8TEXT_W_OLD (tolerance, TOLERANCE, text_string,
                                 text_string);
      /*if (version < R_2007 &&
        (strcmp (dwg_ent_tolerance_get_text_string (tolerance, &error),
        text_string)
        || error))
        fail ("old API dwg_ent_tolerance_get_text_string");*/
    }
  else
    ok ("empty TOLERANCE.text_string");
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, ins_pt, ins_pt);
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, extrusion, ext);
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, x_direction, x_dir);
  if (version <= R_14)
    {
      CHK_ENTITY_TYPE (tolerance, TOLERANCE, unknown_short, BS, unknown_short);
      CHK_ENTITY_TYPE_W_OLD (tolerance, TOLERANCE, height, BD, height);
      CHK_ENTITY_TYPE_W_OLD (tolerance, TOLERANCE, dimgap, BD, dimgap);
    }
  CHK_ENTITY_H (tolerance, TOLERANCE, dimstyle, dimstyle);
}

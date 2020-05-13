#define DWG_TYPE DWG_TYPE_TOLERANCE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double height, dimgap;
  dwg_point_3d ins_pt, x_direction, extrusion;
  char *text_value;
  int isnew;
  BITCODE_BS unknown_short;
  BITCODE_H dimstyle;

  Dwg_Version_Type version = obj->parent->header.version;
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE (obj);

  if (tolerance->text_value)
    {
      CHK_ENTITY_UTF8TEXT (tolerance, TOLERANCE, text_value);
    }
  else
    ok ("empty TOLERANCE.text_value");
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, ins_pt);
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, extrusion);
  CHK_ENTITY_3RD_W_OLD (tolerance, TOLERANCE, x_direction);
  if (version <= R_14)
    {
      CHK_ENTITY_TYPE (tolerance, TOLERANCE, unknown_short, BS);
      CHK_ENTITY_TYPE_W_OLD (tolerance, TOLERANCE, height, BD);
      CHK_ENTITY_TYPE_W_OLD (tolerance, TOLERANCE, dimgap, BD);
    }
  CHK_ENTITY_H (tolerance, TOLERANCE, dimstyle);
}

#define DWG_TYPE DWG_TYPE_MLINESTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_TV name;
  BITCODE_TV description;
  BITCODE_BS flag;
  BITCODE_CMC fill_color;
  BITCODE_BD start_angle;
  BITCODE_BD end_angle;
  BITCODE_RC i, num_lines;
  Dwg_MLINESTYLE_line* lines;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_mlinestyle *msty = dwg_object_to_MLINESTYLE (obj);

  CHK_ENTITY_UTF8TEXT (msty, MLINESTYLE, name, name);
  CHK_ENTITY_UTF8TEXT (msty, MLINESTYLE, description, description);
  CHK_ENTITY_TYPE (msty, MLINESTYLE, flag, BS, flag);
  CHK_ENTITY_MAX (msty, MLINESTYLE, flag, BS, 2047);
  CHK_ENTITY_CMC (msty, MLINESTYLE, fill_color, fill_color);
  CHK_ENTITY_TYPE (msty, MLINESTYLE, start_angle, BD, start_angle);
  CHK_ENTITY_MAX (msty, MLINESTYLE, start_angle, BD, 6.284);
  CHK_ENTITY_TYPE (msty, MLINESTYLE, end_angle, BD, end_angle);
  CHK_ENTITY_MAX (msty, MLINESTYLE, end_angle, BD, 6.284);
  CHK_ENTITY_TYPE (msty, MLINESTYLE, num_lines, RCd, num_lines);
  
  if (!dwg_dynapi_entity_value (msty, "MLINESTYLE", "lines", &lines, NULL))
    fail ("MLINESTYLE.lines");
  else
    {
      for (i = 0; i < num_lines; i++)
        {
          CHK_SUBCLASS_TYPE (lines[i], MLINESTYLE_line, offset, BD);
          CHK_SUBCLASS_CMC (lines[i], MLINESTYLE_line, color);
          if (dwg_version < R_2018)
            CHK_SUBCLASS_TYPE (lines[i], MLINESTYLE_line, lt.index, BSd)
          else
            CHK_SUBCLASS_H (lines[i], MLINESTYLE_line, lt.ltype);
        }
    }
}

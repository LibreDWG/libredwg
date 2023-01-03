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
  Dwg_MLINESTYLE_line *lines;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_mlinestyle *mlinestyle = dwg_object_to_MLINESTYLE (obj);

  CHK_ENTITY_UTF8TEXT_W_OBJ (mlinestyle, MLINESTYLE, name);
  CHK_ENTITY_UTF8TEXT (mlinestyle, MLINESTYLE,
                       description); // old API called desc
  CHK_ENTITY_TYPE_W_OBJ (mlinestyle, MLINESTYLE, flag, BS);
  CHK_ENTITY_MAX (mlinestyle, MLINESTYLE, flag, BS, 2047);
  CHK_ENTITY_CMC (mlinestyle, MLINESTYLE, fill_color);
  CHK_ENTITY_TYPE_W_OBJ (mlinestyle, MLINESTYLE, start_angle, BD);
  CHK_ENTITY_MAX (mlinestyle, MLINESTYLE, start_angle, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OBJ (mlinestyle, MLINESTYLE, end_angle, BD);
  CHK_ENTITY_MAX (mlinestyle, MLINESTYLE, end_angle, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE_W_OBJ (mlinestyle, MLINESTYLE, num_lines, RCd);

  if (!dwg_dynapi_entity_value (mlinestyle, "MLINESTYLE", "lines", &lines,
                                NULL))
    fail ("MLINESTYLE.lines");
  else
    {
      for (i = 0; i < num_lines; i++)
        {
          CHK_SUBCLASS_TYPE (lines[i], MLINESTYLE_line, offset, BD);
          CHK_SUBCLASS_CMC (lines[i], MLINESTYLE_line, color);
          if (dwg_version < R_2018)
            CHK_SUBCLASS_TYPE (lines[i], MLINESTYLE_line, lt_index, BSd)
          else
            CHK_SUBCLASS_H (lines[i], MLINESTYLE_line, lt_ltype);
        }
    }
}

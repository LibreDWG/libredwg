#define DWG_TYPE DWG_TYPE_LTYPE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_T name;
  BITCODE_RSd used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;

  BITCODE_TV description;
  BITCODE_BD pattern_len;
  BITCODE_RC alignment;
  BITCODE_RC numdashes;
  Dwg_LTYPE_dash *dashes;
  BITCODE_RD dashes_r11[12];
  BITCODE_B has_strings_area; /* if some shape_flag & 4 (ODA bug) */
  BITCODE_TF strings_area;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_ltype *ltype = dwg_object_to_LTYPE (obj);

  CHK_ENTITY_TYPE (ltype, LTYPE, flag, RC);
  CHK_ENTITY_UTF8TEXT (ltype, LTYPE, name);
  CHK_ENTITY_TYPE (ltype, LTYPE, used, RSd);
  CHK_ENTITY_TYPE (ltype, LTYPE, is_xref_ref, B);
  CHK_ENTITY_TYPE (ltype, LTYPE, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (ltype, LTYPE, is_xref_dep, B);
  CHK_ENTITY_H (ltype, LTYPE, xref);

  CHK_ENTITY_UTF8TEXT (ltype, LTYPE, description);
  CHK_ENTITY_TYPE (ltype, LTYPE, pattern_len, BD);
  CHK_ENTITY_TYPE (ltype, LTYPE, alignment, RC);
  CHK_ENTITY_TYPE (ltype, LTYPE, numdashes, RCd);
  if (!dwg_dynapi_entity_value (ltype, "LTYPE", "dashes", &dashes, NULL))
    fail ("LTYPE.dashes");
  else
    {
      if (dwg_version <= R_13b1)
        {
          if (!dwg_dynapi_entity_value (ltype, "LTYPE", "dashes_r11",
                                        &dashes_r11, NULL))
            fail ("LTYPE.dashes_r11");
          numdashes = 12;
        }
      for (BITCODE_BL i = 0; i < numdashes; i++)
        {
          if (dwg_version <= R_13b1)
            {
              ok ("dashes_r11[%u]: %f", i, dashes_r11[i]);
            }
          else
            {
              CHK_SUBCLASS_TYPE (dashes[i], LTYPE_dash, length, BD);
              CHK_SUBCLASS_TYPE (dashes[i], LTYPE_dash, complex_shapecode, BS);
              CHK_SUBCLASS_H (dashes[i], LTYPE_dash, style);
              CHK_SUBCLASS_TYPE (dashes[i], LTYPE_dash, x_offset, RD);
              CHK_SUBCLASS_TYPE (dashes[i], LTYPE_dash, y_offset, RD);
              CHK_SUBCLASS_TYPE (dashes[i], LTYPE_dash, scale, BD);
              CHK_SUBCLASS_TYPE (dashes[i], LTYPE_dash, rotation, BD);
              CHK_SUBCLASS_TYPE (dashes[i], LTYPE_dash, shape_flag, BS);
            }
        }
    }
  if (dwg_version >= R_13b1)
    {
      CHK_ENTITY_TYPE (ltype, LTYPE, has_strings_area, B);
      CHK_ENTITY_TYPE (ltype, LTYPE, strings_area, TF);
    }
}

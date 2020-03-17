#define DWG_TYPE DWG_TYPE_LTYPE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_TV name;
  BITCODE_RS used;
  BITCODE_B xrefref;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;

  BITCODE_TV description;
  BITCODE_BD pattern_len;
  BITCODE_RC alignment;
  BITCODE_RC num_dashes;
  Dwg_LTYPE_dash* dashes;
  BITCODE_RD* dashes_r11;
  BITCODE_B has_strings_area; /* if some shape_flag & 4 (ODA bug) */
  BITCODE_TF strings_area;
  BITCODE_H extref_handle;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_ltype *ltype = dwg_object_to_LTYPE (obj);

  CHK_ENTITY_TYPE (ltype, LTYPE, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT (ltype, LTYPE, name, name);
  CHK_ENTITY_TYPE (ltype, LTYPE, used, RS, used);
  CHK_ENTITY_TYPE (ltype, LTYPE, xrefref, B, xrefref);
  CHK_ENTITY_TYPE (ltype, LTYPE, xrefindex_plus1, BS, xrefindex_plus1);
  CHK_ENTITY_TYPE (ltype, LTYPE, xrefdep, B, xrefdep);

  CHK_ENTITY_UTF8TEXT (ltype, LTYPE, description, description);
  CHK_ENTITY_TYPE (ltype, LTYPE, pattern_len, BD, pattern_len);
  CHK_ENTITY_TYPE (ltype, LTYPE, alignment, RC, alignment);
  CHK_ENTITY_TYPE (ltype, LTYPE, num_dashes, RC, num_dashes);
  //Dwg_LTYPE_dash* dashes;
  //BITCODE_RD* dashes_r11;
  CHK_ENTITY_TYPE (ltype, LTYPE, has_strings_area, B, has_strings_area);
  //CHK_ENTITY_TYPE (ltype, LTYPE, strings_area, TF, strings_area);
  CHK_ENTITY_H (ltype, LTYPE, extref_handle, extref_handle);
}

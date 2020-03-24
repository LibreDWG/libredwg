#define DWG_TYPE DWG_TYPE_SCALE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew; 
  BITCODE_BS flag;
  BITCODE_TV name;
  BITCODE_BD paper_units;
  BITCODE_BD drawing_units;
  BITCODE_B has_unit_scale;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_scale *_obj = dwg_object_to_SCALE (obj);

  CHK_ENTITY_TYPE (_obj, SCALE, flag, BS, flag);
  CHK_ENTITY_UTF8TEXT (_obj, SCALE, name, name);
  CHK_ENTITY_TYPE (_obj, SCALE, paper_units, BD, paper_units);
  CHK_ENTITY_TYPE (_obj, SCALE, drawing_units, BD, drawing_units);
  CHK_ENTITY_TYPE (_obj, SCALE, has_unit_scale, B, has_unit_scale);
}

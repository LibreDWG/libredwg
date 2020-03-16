// TODO unstable
#define DWG_TYPE DWG_TYPE_DBCOLOR
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_BB unknown1;
  BITCODE_RL rgb;
  BITCODE_RC unknown2;
  BITCODE_T name;
  BITCODE_T catalog;
  BITCODE_CMC color;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_dbcolor *dbcolor = dwg_object_to_DBCOLOR (obj);

  CHK_ENTITY_TYPE (dbcolor, DBCOLOR, class_version, BL, class_version);
  CHK_ENTITY_MAX (dbcolor, DBCOLOR, class_version, BL, 2);
  CHK_ENTITY_TYPE (dbcolor, DBCOLOR, unknown1, BB, unknown1);
  CHK_ENTITY_TYPE (dbcolor, DBCOLOR, rgb, BLx, rgb);
  CHK_ENTITY_TYPE (dbcolor, DBCOLOR, unknown2, RC, unknown2);
  CHK_ENTITY_UTF8TEXT (dbcolor, DBCOLOR, name, name);
  CHK_ENTITY_UTF8TEXT (dbcolor, DBCOLOR, catalog, catalog);
  CHK_ENTITY_CMC (dbcolor, DBCOLOR, color, color);
}

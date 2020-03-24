#define DWG_TYPE DWG_TYPE_OBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS class_version; /*!< r2010+ =3 */
  BITCODE_B has_file;
  BITCODE_B defaultflag;

  //Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_objectcontextdata *_obj = dwg_object_to_OBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, OBJECTCONTEXTDATA, class_version, BS, class_version);
  CHK_ENTITY_TYPE (_obj, OBJECTCONTEXTDATA, has_file, B, has_file);
  CHK_ENTITY_TYPE (_obj, OBJECTCONTEXTDATA, defaultflag, B, defaultflag);
}

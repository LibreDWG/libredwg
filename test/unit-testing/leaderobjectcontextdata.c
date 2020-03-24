// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_LEADEROBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS class_version; /*!< r2010+ =3 */
  BITCODE_B has_file;
  BITCODE_B defaultflag;
  BITCODE_H scale; /* DXF 340 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_leaderobjectcontextdata *_obj = dwg_object_to_LEADEROBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, LEADEROBJECTCONTEXTDATA, class_version, BL, class_version);
  CHK_ENTITY_TYPE (_obj, LEADEROBJECTCONTEXTDATA, has_file, B, has_file);
  CHK_ENTITY_TYPE (_obj, LEADEROBJECTCONTEXTDATA, defaultflag, B, defaultflag);
  CHK_ENTITY_H (_obj, LEADEROBJECTCONTEXTDATA, scale, scale);
#endif
}

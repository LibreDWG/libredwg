// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_MLEADEROBJECTCONTEXTDATA
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
  dwg_obj_mleaderobjectcontextdata *_obj = dwg_object_to_MLEADEROBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, MLEADEROBJECTCONTEXTDATA, class_version, BL, class_version);
  CHK_ENTITY_TYPE (_obj, MLEADEROBJECTCONTEXTDATA, has_file, B, has_file);
  CHK_ENTITY_TYPE (_obj, MLEADEROBJECTCONTEXTDATA, defaultflag, B, defaultflag);
  CHK_ENTITY_H (_obj, MLEADEROBJECTCONTEXTDATA, scale, scale);
#endif
}

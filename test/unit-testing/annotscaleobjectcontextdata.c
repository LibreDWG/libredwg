// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ANNOTSCALEOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  //ANNOTSCALEOBJECTCONTEXTDATA_fields;
  BITCODE_BS class_version; /*!< r2010+ =4 */
  BITCODE_B is_default;
  BITCODE_B has_xdic;
  BITCODE_H scale;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_annotscaleobjectcontextdata *_obj = dwg_object_to_ANNOTSCALEOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, ANNOTSCALEOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ANNOTSCALEOBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_TYPE (_obj, ANNOTSCALEOBJECTCONTEXTDATA, has_xdic, B);
  CHK_ENTITY_H (_obj, ANNOTSCALEOBJECTCONTEXTDATA, scale);

#endif
}

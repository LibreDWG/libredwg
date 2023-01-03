#define DWG_TYPE DWG_TYPE_IMAGEDEF
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_2RD image_size;
  BITCODE_TV file_path;
  BITCODE_B is_loaded;
  BITCODE_RC resunits;
  BITCODE_2RD pixel_size;
  // BITCODE_H xrefctrl;    /*!< r2010+ */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_imagedef *_obj = dwg_object_to_IMAGEDEF (obj);

  CHK_ENTITY_TYPE (_obj, IMAGEDEF, class_version, BL);
  CHK_ENTITY_2RD (_obj, IMAGEDEF, image_size);
  CHK_ENTITY_UTF8TEXT (_obj, IMAGEDEF, file_path);
  CHK_ENTITY_TYPE (_obj, IMAGEDEF, is_loaded, B);
  CHK_ENTITY_TYPE (_obj, IMAGEDEF, resunits, RC);
  CHK_ENTITY_2RD (_obj, IMAGEDEF, pixel_size);
}

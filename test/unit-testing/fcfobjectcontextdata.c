// unstable. no coverage
#define DWG_TYPE DWG_TYPE_FCFOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;
  /*BITCODE_BS class_version; // r2010+ =3
  BITCODE_B is_default;
  BITCODE_H scale; */

  BITCODE_3BD location;  /*!< DXF 10-30 */
  BITCODE_3BD horiz_dir; /*!< DXF 11-31 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_fcfobjectcontextdata *_obj
      = dwg_object_to_FCFOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, FCFOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, FCFOBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_H (_obj, FCFOBJECTCONTEXTDATA, scale);

  CHK_ENTITY_3RD (_obj, FCFOBJECTCONTEXTDATA, location);
  CHK_ENTITY_3RD (_obj, FCFOBJECTCONTEXTDATA, horiz_dir);
}

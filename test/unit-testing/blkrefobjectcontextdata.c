// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLKREFOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS class_version; /*!< r2010+ =3 */
  BITCODE_B is_default;
  BITCODE_B has_xdic;
  BITCODE_H scale; /* DXF 340 */

  BITCODE_BD rotation;
  BITCODE_3BD insertion_pt;
  BITCODE_3BD scale_factor;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blkrefobjectcontextdata *_obj = dwg_object_to_BLKREFOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, BLKREFOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, BLKREFOBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_TYPE (_obj, BLKREFOBJECTCONTEXTDATA, has_xdic, B);
  CHK_ENTITY_H (_obj, BLKREFOBJECTCONTEXTDATA, scale);
  CHK_ENTITY_TYPE (_obj, BLKREFOBJECTCONTEXTDATA, rotation, BD);
  CHK_ENTITY_3RD (_obj, BLKREFOBJECTCONTEXTDATA, insertion_pt);
  CHK_ENTITY_3RD (_obj, BLKREFOBJECTCONTEXTDATA, scale_factor);
#endif
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_MTEXTOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*!< r2010+ =3 */
  BITCODE_B is_default;
  BITCODE_B in_dwg;
  BITCODE_H scale;
  BITCODE_3BD insertion_pt;
  BITCODE_3BD x_axis_dir;
  BITCODE_BD text_height;
  BITCODE_BD rect_width;
  BITCODE_BD extents_height;
  BITCODE_BD extents_width;
  BITCODE_BS attachment;
  BITCODE_BS flow_dir;
  BITCODE_BS linespace_style;
  BITCODE_BD linespace_factor;
  BITCODE_BD bd45;
  BITCODE_BS bs74;
  BITCODE_BD bd46;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_mtextobjectcontextdata *_obj = dwg_object_to_MTEXTOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, in_dwg, B);
  CHK_ENTITY_H (_obj, MTEXTOBJECTCONTEXTDATA, scale);
  CHK_ENTITY_3RD (_obj, MTEXTOBJECTCONTEXTDATA, insertion_pt);
  CHK_ENTITY_3RD (_obj, MTEXTOBJECTCONTEXTDATA, x_axis_dir);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, text_height, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, rect_width, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, extents_height, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, extents_width, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, attachment, BS);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, flow_dir, BS);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, linespace_style, BS);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, linespace_factor, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, bd45, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, bs74, BS);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, bd46, BD);
#endif
}

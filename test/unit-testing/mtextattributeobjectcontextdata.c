// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_MTEXTATTRIBUTEOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*!< r2010+ =3 */
  BITCODE_B is_default;
  BITCODE_B in_dwg;
  BITCODE_H scale;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_mtextattributeobjectcontextdata *_obj = dwg_object_to_MTEXTATTRIBUTEOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, class_version, BS, class_version);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, is_default, B, is_default);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, in_dwg, B, in_dwg);
  CHK_ENTITY_H (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, scale, scale);
#if 0
  CHK_ENTITY_3RD (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, insertion_pt, insertion_pt);
  CHK_ENTITY_3RD (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, x_axis_dir, x_axis_dir);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, text_height, BD, text_height);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, rect_width, BD, rect_width);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, extents_height, BD, extents_height);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, extents_width, BD, extents_width);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, attachment, BS, attachment);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, drawing_dir, BS, drawing_dir);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, linespace_style, BS, linespace_style);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, linespace_factor, BD, linespace_factor);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, bd45, BD, bd45);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, bs74, BS, bs74);
  CHK_ENTITY_TYPE (_obj, MTEXTATTRIBUTEOBJECTCONTEXTDATA, bd46, BD, bd46);
#endif
#endif
}

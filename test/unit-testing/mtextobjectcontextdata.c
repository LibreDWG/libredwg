// unstable
#define DWG_TYPE DWG_TYPE_MTEXTOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;
  /*BITCODE_BS class_version;
  BITCODE_B is_default;
  BITCODE_H scale;*/
  BITCODE_BL attachment;
  BITCODE_3BD ins_pt;
  BITCODE_3BD x_axis_dir;
  BITCODE_BD rect_height;
  BITCODE_BD rect_width;
  BITCODE_BD extents_height;
  BITCODE_BD extents_width;
  BITCODE_BD column_width;
  BITCODE_BD gutter;
  BITCODE_B auto_height;
  BITCODE_B flow_reversed;
  BITCODE_BL column_type;
  BITCODE_BL i, num_column_heights;
  BITCODE_BD *column_heights;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_mtextobjectcontextdata *_obj
      = dwg_object_to_MTEXTOBJECTCONTEXTDATA (obj);

  // AcDbObjectContextData
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, is_default, B);
  // AcDbAnnotScaleObjectContextData
  CHK_ENTITY_H (_obj, MTEXTOBJECTCONTEXTDATA, scale);
  // AcDbMTextObjectContextData
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, attachment, BL);
  CHK_ENTITY_3RD (_obj, MTEXTOBJECTCONTEXTDATA, ins_pt);
  CHK_ENTITY_3RD (_obj, MTEXTOBJECTCONTEXTDATA, x_axis_dir);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, rect_height, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, rect_width, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, extents_height, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, extents_width, BD);

  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, column_type, BL);
  CHK_ENTITY_MAX (_obj, MTEXTOBJECTCONTEXTDATA, column_type, BL, 2);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, num_column_heights, BL);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, column_width, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, gutter, BD);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, auto_height, B);
  CHK_ENTITY_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, flow_reversed, B);
  CHK_ENTITY_VECTOR_TYPE (_obj, MTEXTOBJECTCONTEXTDATA, column_heights,
                          num_column_heights, BD);
}

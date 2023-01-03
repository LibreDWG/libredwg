// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ORDDIMOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;
  Dwg_OCD_Dimension dimension;
  BITCODE_3BD feature_location_pt; /*!< DXF 11-31 = origin */
  BITCODE_3BD leader_endpt;        /*!< DXF 12-32 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_orddimobjectcontextdata *_obj
      = dwg_object_to_ORDDIMOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, ORDDIMOBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, ORDDIMOBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_H (_obj, ORDDIMOBJECTCONTEXTDATA, scale);

  CHK_SUBCLASS_2RD (_obj->dimension, OCD_Dimension, def_pt);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, b293, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, is_def_textloc, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, text_rotation, BD);
  CHK_SUBCLASS_H (_obj->dimension, OCD_Dimension, block);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, dimtofl, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, dimosxd, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, dimatfit, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, dimtix, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, dimtmove, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, override_code, RC);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, has_arrow2, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, flip_arrow2, B);
  CHK_SUBCLASS_TYPE (_obj->dimension, OCD_Dimension, flip_arrow1, B);

  CHK_ENTITY_3RD (_obj, ORDDIMOBJECTCONTEXTDATA, feature_location_pt);
  CHK_ENTITY_3RD (_obj, ORDDIMOBJECTCONTEXTDATA, leader_endpt);
#endif
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_GEOPOSITIONMARKER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS type;
  BITCODE_3BD position;
  BITCODE_BE extrusion;
  BITCODE_BD radius;
  BITCODE_BD landing_gap;
  BITCODE_T text;
  BITCODE_BS text_alignment;
  BITCODE_B mtext_visible;
  BITCODE_B enable_frame_text;
  BITCODE_T notes;
  BITCODE_H mtext_handle;
  BITCODE_H text_style;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_geopositionmarker *_obj = dwg_object_to_GEOPOSITIONMARKER (obj);

  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, type, BS, type);
  CHK_ENTITY_3RD (_obj, GEOPOSITIONMARKER, position, position);
  CHK_ENTITY_3RD (_obj, GEOPOSITIONMARKER, extrusion, extrusion);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, radius, BD, radius);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, landing_gap, BD, landing_gap);
  CHK_ENTITY_UTF8TEXT (_obj, GEOPOSITIONMARKER, text, text);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, text_alignment, BS, text_alignment);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, mtext_visible, B, mtext_visible);
  CHK_ENTITY_TYPE (_obj, GEOPOSITIONMARKER, enable_frame_text, B, enable_frame_text);
  CHK_ENTITY_UTF8TEXT (_obj, GEOPOSITIONMARKER, notes, notes);
  CHK_ENTITY_H (_obj, GEOPOSITIONMARKER, mtext_handle, mtext_handle);
  CHK_ENTITY_H (_obj, GEOPOSITIONMARKER, text_style, text_style);
#endif
}

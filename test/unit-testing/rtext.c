// DEBUGGING
#define DWG_TYPE DWG_TYPE_RTEXT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error = 0, isnew;
  BITCODE_3BD pt;
  BITCODE_BE extrusion;
  BITCODE_BD rotation;
  BITCODE_BD height;
  BITCODE_BS flags;
  BITCODE_T text_value;
  BITCODE_H style;

  Dwg_Version_Type version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_rtext *rtext = dwg_object_to_RTEXT (obj);

  CHK_ENTITY_UTF8TEXT (rtext, RTEXT, text_value);
  CHK_ENTITY_3RD (rtext, RTEXT, pt);
  CHK_ENTITY_3RD (rtext, RTEXT, extrusion);
  CHK_ENTITY_TYPE (rtext, RTEXT, rotation, BD);
  CHK_ENTITY_MAX (rtext, RTEXT, rotation, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE (rtext, RTEXT, height, BD);
  CHK_ENTITY_TYPE (rtext, RTEXT, flags, BS);
  CHK_ENTITY_H (rtext, RTEXT, style);
#endif
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_TEXTOBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*!< r2010+ =4 */
  BITCODE_B has_file;
  BITCODE_B defaultflag;
  BITCODE_H scale;
  BITCODE_BS flag;	/*<! DXF 70 */ // 0
  BITCODE_BD rotation;	/*!< DXF 50 */ // 0.0 or 90.0
  BITCODE_2RD insertion_pt; 	/*!< DXF 10-20 */
  BITCODE_2RD alignment_pt; 	/*!< DXF 11-21 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_textobjectcontextdata *_obj = dwg_object_to_TEXTOBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, class_version, BL, class_version);
  //CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, has_file, B, has_file);
  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, defaultflag, B, defaultflag);
  CHK_ENTITY_H (_obj, TEXTOBJECTCONTEXTDATA, scale, scale);
  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, flag, BS, flag);
  CHK_ENTITY_TYPE (_obj, TEXTOBJECTCONTEXTDATA, rotation, BD, rotation);
  CHK_ENTITY_2RD (_obj, TEXTOBJECTCONTEXTDATA, insertion_pt, insertion_pt);
  CHK_ENTITY_2RD (_obj, TEXTOBJECTCONTEXTDATA, alignment_pt, alignment_pt);
#endif
}

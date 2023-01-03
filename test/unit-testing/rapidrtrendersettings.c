#define DWG_TYPE DWG_TYPE_RAPIDRTRENDERSETTINGS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // AcDbRenderSettings
  BITCODE_BL class_version;
  BITCODE_T name;
  BITCODE_B fog_enabled;
  BITCODE_B fog_background_enabled;
  BITCODE_B backfaces_enabled;
  BITCODE_B environ_image_enabled;
  BITCODE_T environ_image_filename;
  BITCODE_T description;
  BITCODE_BL display_index;
  BITCODE_B has_predefined; /*!< DXF 290, r2013 only */
  // AcDbRapidRTRenderSettings
  BITCODE_BL rapidrt_version; /*!< DXF 90 */
  BITCODE_BL render_target;   /*!< DXF 70 */
  BITCODE_BL render_level;    /*!< DXF 90 */
  BITCODE_BL render_time;     /*!< DXF 90 */
  BITCODE_BL lighting_model;  /*!< DXF 70 */
  BITCODE_BL filter_type;     /*!< DXF 70 */
  BITCODE_BD filter_width;    /*!< DXF 40 */
  BITCODE_BD filter_height;   /*!< DXF 40 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_rapidrtrendersettings *_obj
      = dwg_object_to_RAPIDRTRENDERSETTINGS (obj);

  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, class_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, RAPIDRTRENDERSETTINGS, name);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_enabled, B);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_background_enabled, B);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, backfaces_enabled, B);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, environ_image_enabled, B);
  CHK_ENTITY_UTF8TEXT (_obj, RAPIDRTRENDERSETTINGS, environ_image_filename);
  CHK_ENTITY_UTF8TEXT (_obj, RAPIDRTRENDERSETTINGS, description);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, display_index, BL);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, has_predefined, B);

  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, rapidrt_version, BL);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, render_target, BL);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, render_level, BL);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, render_time, BL);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, lighting_model, BL);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, filter_type, BL);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, filter_width, BD);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, filter_height, BD);
}

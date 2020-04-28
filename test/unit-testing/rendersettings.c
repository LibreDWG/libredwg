#define DWG_TYPE DWG_TYPE_RENDERSETTINGS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // AcDbRenderSettings
  BITCODE_BL class_version;     /*!< DXF 90, default: 1 */
  BITCODE_T name;		/*!< DXF 1 */
  BITCODE_B fog_enabled;        /*!< DXF 290 */
  BITCODE_B fog_background_enabled;  /*!< DXF 290 */
  BITCODE_B b290_1;                  /*!< DXF 290 */
  BITCODE_B environ_image_enabled;   /*!< DXF 290 */
  BITCODE_T environ_image_filename;  /*!< DXF 1 */
  BITCODE_T description;	     /*!< DXF 1 */
  BITCODE_BL bl90;
  BITCODE_B b290_2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_rendersettings *_obj = dwg_object_to_RENDERSETTINGS (obj);

  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, class_version, BL, class_version);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERSETTINGS, name, name);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, fog_enabled, B, fog_enabled);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, fog_background_enabled, B, fog_background_enabled);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, b290_1, B, b290_1);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, environ_image_enabled, B, environ_image_enabled);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERSETTINGS, environ_image_filename, environ_image_filename);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, fog_density_near, BD, fog_density_near);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, fog_density_far, BD, fog_density_far);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERSETTINGS, description, description);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, bl90, BL, bl90);
  CHK_ENTITY_TYPE (_obj, RENDERSETTINGS, b290_2, B, b290_2);
#endif
}

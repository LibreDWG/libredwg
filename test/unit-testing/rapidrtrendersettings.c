#define DWG_TYPE DWG_TYPE_RAPIDRTRENDERSETTINGS
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
  BITCODE_BS bs90;
  // AcDbRapidRTRenderSettings
  BITCODE_BS bs90_0;
  BITCODE_RC rc70_1;
  BITCODE_BS bs90_1;
  BITCODE_BS bs90_2;
  BITCODE_RC rc70_2;
  BITCODE_RC rc70_3;
  BITCODE_BD fog_density_near;  /*!< DXF 40 */
  BITCODE_BD fog_density_far;   /*!< DXF 40 */
  BITCODE_B b290_2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_rapidrtrendersettings *_obj = dwg_object_to_RAPIDRTRENDERSETTINGS (obj);

  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, class_version, BL, class_version);
  CHK_ENTITY_UTF8TEXT (_obj, RAPIDRTRENDERSETTINGS, name, name);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_enabled, B, fog_enabled);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_background_enabled, B, fog_background_enabled);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, b290_1, B, b290_1);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, environ_image_enabled, B, environ_image_enabled);
  CHK_ENTITY_UTF8TEXT (_obj, RAPIDRTRENDERSETTINGS, environ_image_filename, environ_image_filename);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_density_near, BD, fog_density_near);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_density_far, BD, fog_density_far);
  CHK_ENTITY_UTF8TEXT (_obj, RAPIDRTRENDERSETTINGS, description, description);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, bs90, BS, bs90);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, bs90_0, BS, bs90_0);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, rc70_1, RC, rc70_1);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, bs90_1, BS, bs90_1);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, bs90_2, BS, bs90_2);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, rc70_2, RC, rc70_2);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, rc70_3, RC, rc70_3);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_density_near, BD, fog_density_near);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, fog_density_far, BD, fog_density_far);
  CHK_ENTITY_TYPE (_obj, RAPIDRTRENDERSETTINGS, b290_2, B, b290_2);
#endif
}

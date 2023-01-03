// unstable
#define DWG_TYPE DWG_TYPE_RENDERENTRY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;  /*!< DXF 90 */
  BITCODE_T image_file_name; /*!< DXF 1 */
  BITCODE_T preset_name;     /*!< DXF 1 */
  BITCODE_T view_name;       /*!< DXF 1 */
  BITCODE_BL dimension_x;    /*!< DXF 90 */
  BITCODE_BL dimension_y;    /*!< DXF 90 */
  BITCODE_BS start_year;     /*!< DXF 70 */
  BITCODE_BS start_month;    /*!< DXF 70 */
  BITCODE_BS start_day;      /*!< DXF 70 */
  BITCODE_BS start_minute;   /*!< DXF 70 */
  BITCODE_BS start_second;   /*!< DXF 70 */
  BITCODE_BS start_msec;     /*!< DXF 70 */
  BITCODE_BD render_time;    /*!< DXF 40 */
  BITCODE_BL memory_amount;  /*!< DXF 90 */
  BITCODE_BL material_count; /*!< DXF 90 */
  BITCODE_BL light_count;    /*!< DXF 90 */
  BITCODE_BL triangle_count; /*!< DXF 90 */
  BITCODE_BL display_index;  /*!< DXF 90 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_renderentry *_obj = dwg_object_to_RENDERENTRY (obj);

  CHK_ENTITY_TYPE (_obj, RENDERENTRY, class_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERENTRY, image_file_name);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERENTRY, preset_name);
  CHK_ENTITY_UTF8TEXT (_obj, RENDERENTRY, view_name);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, dimension_x, BL);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, dimension_y, BL);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, start_year, BS);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, start_month, BS);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, start_day, BS);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, start_minute, BS);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, start_second, BS);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, start_msec, BS);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, render_time, BD);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, memory_amount, BL);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, material_count, BL);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, light_count, BL);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, triangle_count, BL);
  CHK_ENTITY_TYPE (_obj, RENDERENTRY, display_index, BL);
}

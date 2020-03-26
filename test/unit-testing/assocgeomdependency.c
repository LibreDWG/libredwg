// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_ASSOCGEOMDEPENDENCY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;			/*<! DXF 90 */
  BITCODE_BS dependent_on_object_status;	/*<! DXF 90 */
  BITCODE_B has_cached_value;			/*<! DXF 290 */
  BITCODE_B is_actionevaluation_in_progress;  	/*<! DXF 290 */
  BITCODE_B is_attached_to_object;		/*<! DXF 290 */
  BITCODE_B is_delegating_to_owning_action;	/*<! DXF 290 */
  BITCODE_BS bs90_2;	/*<! DXF 90 */
  BITCODE_H h330_1;	/*<! DXF 330 */
  BITCODE_B b290_5;	/*<! DXF 290 */
  BITCODE_H h330_2;	/*<! DXF 330 */
  BITCODE_H h330_3;	/*<! DXF 330 */
  BITCODE_H h360;	/*<! DXF 360 */
  BITCODE_BS bs90_3;	/*<! DXF 90 */
  // AcDbAssocGeomDependency
  BITCODE_BS bs90_4;	/*<! DXF 90 */
  BITCODE_B b290_6;	/*<! DXF 290 */
  BITCODE_T t;	/*<! DXF 1 */
  BITCODE_B dependent_on_compound_object;   /*<! DXF 290 */

#ifdef DEBUG_CLASSES
  dwg_obj_assocgeomdependency *_obj = dwg_object_to_ASSOCGEOMDEPENDENCY (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, class_version, BS, class_version);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, dependent_on_object_status, BS, dependent_on_object_status);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, has_cached_value, B, has_cached_value);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, is_actionevaluation_in_progress, B, is_actionevaluation_in_progress);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, is_attached_to_object, B, is_attached_to_object);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, is_delegating_to_owning_action, B, is_delegating_to_owning_action);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, bs90_2, BS, bs90_2);
  CHK_ENTITY_H (_obj, ASSOCGEOMDEPENDENCY, h330_1, h330_1);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, b290_5, B, b290_5);
  CHK_ENTITY_H (_obj, ASSOCGEOMDEPENDENCY, h330_2, h330_2);
  CHK_ENTITY_H (_obj, ASSOCGEOMDEPENDENCY, h330_3, h330_3);
  CHK_ENTITY_H (_obj, ASSOCGEOMDEPENDENCY, h360, h360);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, bs90_3, BS, bs90_3);
  // AcDbAssocGeomDependency
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, bs90_4, BS, bs90_4);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, b290_6, B, b290_6);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCGEOMDEPENDENCY, t, t);
  CHK_ENTITY_TYPE (_obj, ASSOCGEOMDEPENDENCY, dependent_on_compound_object, B, dependent_on_compound_object);
#endif
}

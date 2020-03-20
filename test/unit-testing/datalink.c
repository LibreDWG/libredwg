// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_DATALINK
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*<! DXF 70 1/10 */
  BITCODE_T appname;	/*<! DXF 1  */
  BITCODE_T description;/*<! DXF 300  */
  BITCODE_T link;	/*<! DXF 301  */
  BITCODE_T cell;	/*<! DXF 302  */
  BITCODE_BS bs90;	/*<! DXF 90  2 */
  BITCODE_BS bs91;	/*<! DXF 91  1179649 */
  BITCODE_BS bs92;	/*<! DXF 92  1 */
  BITCODE_BS year;	/*<! DXF 170  */
  BITCODE_BS month;	/*<! DXF 171  */
  BITCODE_BS day;	/*<! DXF 172  */
  BITCODE_BS hour;	/*<! DXF 173  */
  BITCODE_BS minute;	/*<! DXF 174  */
  BITCODE_BS seconds;	/*<! DXF 175  */
  BITCODE_BS bs176;	/*<! DXF 176  0 */
  BITCODE_BS bs171;	/*<! DXF 177  1 */
  BITCODE_BS bs93;	/*<! DXF 93   0 */
  BITCODE_T t304;	/*<! DXF 304  */
  BITCODE_BS num_deps;	/*<! DXF 94   2 */
  BITCODE_H *deps;	/*<! DXF 330  */
  BITCODE_H writedep;	/*<! DXF 360  */
  BITCODE_T t305;	/*<! DXF 305 CUSTOMDATA */

#ifdef DEBUG_CLASSES
  dwg_obj_datalink *_obj = dwg_object_to_DATALINK (obj);

  CHK_ENTITY_TYPE (_obj, DATALINK, class_version, BS, class_version);
  CHK_ENTITY_UTF8TEXT (_obj, DATALINK, appname, appname);
  CHK_ENTITY_UTF8TEXT (_obj, DATALINK, description, description);
  CHK_ENTITY_UTF8TEXT (_obj, DATALINK, link, link);
  CHK_ENTITY_UTF8TEXT (_obj, DATALINK, cell, cell);
  CHK_ENTITY_TYPE (_obj, DATALINK, bs90, BS, bs90);
  CHK_ENTITY_TYPE (_obj, DATALINK, bs91, BS, bs91);
  CHK_ENTITY_TYPE (_obj, DATALINK, bs92, BS, bs92);
  CHK_ENTITY_TYPE (_obj, DATALINK, year, BS, year);
  CHK_ENTITY_MAX (_obj, DATALINK, year, BS, 2100);
  CHK_ENTITY_TYPE (_obj, DATALINK, month, BS, month);
  CHK_ENTITY_MAX (_obj, DATALINK, month, BS, 12);
  CHK_ENTITY_TYPE (_obj, DATALINK, day, BS, day);
  CHK_ENTITY_MAX (_obj, DATALINK, day, BS, 31);
  CHK_ENTITY_TYPE (_obj, DATALINK, hour, BS, hour);
  CHK_ENTITY_MAX (_obj, DATALINK, hour, BS, 24);
  CHK_ENTITY_TYPE (_obj, DATALINK, minute, BS, minute);
  CHK_ENTITY_MAX (_obj, DATALINK, minute, BS, 60);
  CHK_ENTITY_TYPE (_obj, DATALINK, seconds, BS, seconds);
  CHK_ENTITY_MAX (_obj, DATALINK, seconds, BS, 60);
  CHK_ENTITY_TYPE (_obj, DATALINK, bs176, BS, bs176);
  CHK_ENTITY_TYPE (_obj, DATALINK, bs171, BS, bs171);
  CHK_ENTITY_TYPE (_obj, DATALINK, bs93, BS, bs93);
  CHK_ENTITY_UTF8TEXT (_obj, DATALINK, t304, t304);
  CHK_ENTITY_TYPE (_obj, DATALINK, num_deps, BS, num_deps);
  CHK_ENTITY_HV (_obj, DATALINK, deps, deps, num_deps);
  CHK_ENTITY_H (_obj, DATALINK, writedep, writedep);
  CHK_ENTITY_UTF8TEXT (_obj, DATALINK, t305, t305);
#endif
}

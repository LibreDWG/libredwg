// unstable
#define DWG_TYPE DWG_TYPE_LEADEROBJECTCONTEXTDATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;
  /*BITCODE_BS class_version; // r2010+ =3
  BITCODE_B is_default;
  BITCODE_H scale; */
  BITCODE_BL i, num_points;
  BITCODE_3DPOINT *points;      /*!< DXF 10 */
  BITCODE_B b290;               /*!< DXF 290 */
  BITCODE_3DPOINT x_direction;  /*!< DXF 11 */
  BITCODE_3DPOINT inspt_offset; /*!< DXF 12 */
  BITCODE_3DPOINT endptproj;    /*!< DXF 13 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_leaderobjectcontextdata *_obj
      = dwg_object_to_LEADEROBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, LEADEROBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, LEADEROBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_H (_obj, LEADEROBJECTCONTEXTDATA, scale);

  CHK_ENTITY_TYPE (_obj, LEADEROBJECTCONTEXTDATA, num_points, BL);
  if (!dwg_dynapi_entity_value (_obj, "LEADEROBJECTCONTEXTDATA", "points",
                                &points, NULL))
    fail ("LEADEROBJECTCONTEXTDATA.points");
  else
    for (i = 0; i < _obj->num_points; i++)
      ok ("LEADEROBJECTCONTEXTDATA.points[%d]: (%f, %f, %f)", i, points[i].x,
          points[i].y, points[i].z);
  CHK_ENTITY_3RD (_obj, LEADEROBJECTCONTEXTDATA, x_direction);
  CHK_ENTITY_3RD (_obj, LEADEROBJECTCONTEXTDATA, inspt_offset);
  CHK_ENTITY_3RD (_obj, LEADEROBJECTCONTEXTDATA, endptproj);
  CHK_ENTITY_TYPE (_obj, LEADEROBJECTCONTEXTDATA, b290, B);
}

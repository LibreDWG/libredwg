#define DWG_TYPE DWG_TYPE_POLYLINE_3D
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL num_owned, numpoints;
  BITCODE_RC flag, curve_type;
  dwg_point_3d *points;
  BITCODE_H first_vertex, last_vertex, *vertex, seqend;
  Dwg_Version_Type version = obj->parent->header.version;

  dwg_ent_polyline_3d *polyline_3d = dwg_object_to_POLYLINE_3D (obj);

  CHK_ENTITY_TYPE_W_OLD (polyline_3d, POLYLINE_3D, flag, RC);
  CHK_ENTITY_TYPE_W_OLD (polyline_3d, POLYLINE_3D, curve_type, RC);
  CHK_ENTITY_TYPE (polyline_3d, POLYLINE_3D, num_owned, BL);
  numpoints = dwg_object_polyline_3d_get_numpoints (obj, &error);
  if (error)
    fail ("polyline_3d_get_numpoints");
  if (numpoints != num_owned)
    ok ("TODO polyline_3d_get_numpoints: %d != num_owned: %d", numpoints,
        num_owned);

  points = dwg_object_polyline_3d_get_points (obj, &error);
  if (!error)
    for (BITCODE_BL i = 0; i < numpoints; i++)
      ok ("POLYLINE_3D.points[%d]: (%f, %f)", (int)i, points[i].x,
          points[i].y);
  else
    fail ("POLYLINE_3D.points");
  FREE (points);

  if (version >= R_13b1 && version <= R_2000)
    {
      CHK_ENTITY_H (polyline_3d, POLYLINE_3D, first_vertex);
      CHK_ENTITY_H (polyline_3d, POLYLINE_3D, last_vertex);
    }
  if (version >= R_2004)
    {
      CHK_ENTITY_HV (polyline_3d, POLYLINE_3D, vertex, num_owned);
    }
  CHK_ENTITY_H (polyline_3d, POLYLINE_3D, seqend);
}

#define DWG_TYPE DWG_TYPE_POLYLINE_2D
#include "tests_common.c"

// numpoints must count first_vertex .. last_vertex, both included
static void
check_vertex_chain (dwg_object *obj, BITCODE_H first_vertex,
                    BITCODE_H last_vertex, BITCODE_BL numpoints)
{
  BITCODE_BL num_vertex = 0;
  Dwg_Object *vobj = dwg_ref_object (obj->parent, first_vertex);
  Dwg_Object *vlast = dwg_ref_object (obj->parent, last_vertex);
  while (vobj)
    {
      if (vobj->fixedtype == DWG_TYPE_VERTEX_2D)
        num_vertex++;
      if (vobj == vlast)
        break;
      vobj = dwg_next_object (vobj);
    }
  if (numpoints == num_vertex)
    ok ("polyline_2d_get_numpoints: %u", (unsigned)numpoints);
  else
    fail ("polyline_2d_get_numpoints: %u != %u vertices", (unsigned)numpoints,
          (unsigned)num_vertex);
}

static void
check_bulges (dwg_object *obj, BITCODE_BL numpoints)
{
  int error;
  double *bulges = dwg_object_polyline_2d_get_bulges (obj, &error);
  if (!error)
    for (BITCODE_BL i = 0; bulges && i < numpoints; i++)
      ok ("POLYLINE_2D.bulges[%d]: %f", (int)i, bulges[i]);
  else
    fail ("POLYLINE_2D.bulges");
  free (bulges);
}

void
api_process (dwg_object *obj)
{
  int error;
  double start_width, end_width, elevation, thickness;
  BITCODE_BL num_owned, numpoints;
  BITCODE_BS flag, curve_type;
  dwg_point_2d *points;
  dwg_point_3d extrusion;
  BITCODE_H first_vertex, last_vertex, *vertex, seqend;
  Dwg_Version_Type version = obj->parent->header.version;

  dwg_ent_polyline_2d *polyline_2d = dwg_object_to_POLYLINE_2D (obj);

  CHK_ENTITY_TYPE_W_OLD (polyline_2d, POLYLINE_2D, flag, BS);
  CHK_ENTITY_TYPE_W_OLD (polyline_2d, POLYLINE_2D, curve_type, BS);
  CHK_ENTITY_TYPE_W_OLD (polyline_2d, POLYLINE_2D, start_width, BD);
  CHK_ENTITY_TYPE_W_OLD (polyline_2d, POLYLINE_2D, end_width, BD);
  CHK_ENTITY_TYPE (polyline_2d, POLYLINE_2D, num_owned, BL);
  numpoints = dwg_object_polyline_2d_get_numpoints (obj, &error);
  if (error)
    fail ("polyline_2d_get_numpoints");
  if (numpoints != num_owned)
    ok ("TODO polyline_2d_get_numpoints: %d != num_owned: %d", numpoints,
        num_owned);

  CHK_ENTITY_TYPE_W_OLD (polyline_2d, POLYLINE_2D, thickness, BD);
  CHK_ENTITY_TYPE_W_OLD (polyline_2d, POLYLINE_2D, elevation, BD);
  CHK_ENTITY_3RD_W_OLD (polyline_2d, POLYLINE_2D, extrusion);

  points = dwg_object_polyline_2d_get_points (obj, &error);
  if (!error)
    for (BITCODE_BL i = 0; i < numpoints; i++)
      ok ("POLYLINE_2D.points[%d]: (%f, %f)", (int)i, points[i].x,
          points[i].y);
  else
    fail ("POLYLINE_2D.points");
  FREE (points);

  if (version >= R_2004)
    {
      CHK_ENTITY_HV (polyline_2d, POLYLINE_2D, vertex, num_owned);
    }
  if (version >= R_13b1 && version <= R_2000)
    {
      CHK_ENTITY_H (polyline_2d, POLYLINE_2D, first_vertex);
      CHK_ENTITY_H (polyline_2d, POLYLINE_2D, last_vertex);
      check_vertex_chain (obj, first_vertex, last_vertex, numpoints);
    }
  check_bulges (obj, numpoints);
  CHK_ENTITY_H (polyline_2d, POLYLINE_2D, seqend);
}

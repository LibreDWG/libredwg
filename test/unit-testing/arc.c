#define DWG_TYPE DWG_TYPE_ARC
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);

  printf("arc->radius      : %f\n",arc->radius);
  printf("arc->thickness   : %f\n",arc->thickness);
  printf("arc->extrusion   : %f, %f, %f\n",
          arc->extrusion.x, arc->extrusion.y, arc->extrusion.z);
  printf("arc->center      : %f, %f, %f\n",
          arc->center.x, arc->center.y, arc->center.z);
  printf("arc->start_angle : %f\n", arc->start_angle);
  printf("arc->end_angle   : %f\n", arc->end_angle);
}

void
api_process(dwg_object *obj)
{
  int error;
  double radius, thickness, start_angle, end_angle;
  dwg_point_3d center, ext;

  // casts an object to arc entity
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);

  radius = dwg_ent_arc_get_radius(arc, &error);
  if ( !error )
    printf("arc_get_radius   : %f\n", radius);
  else
    printf("error in reading radius \n");
  if (dwg_dynapi_entity_value(arc, "ARC", "radius", &radius, NULL) &&
      radius == arc->radius)
    printf("ARC.radius       : %f\n", radius);
  else
    printf("error dynapi ARC.radius\n");

  thickness = dwg_ent_arc_get_thickness(arc, &error);
  if ( !error )
      printf("arc_get_thickness: %f\n", thickness);
  else
      printf("error in reading thickness \n");
  if (dwg_dynapi_entity_value(arc, "ARC", "thickness", &thickness, NULL) &&
      thickness == arc->thickness)
    printf("ARC.thickness    : %f\n", thickness);
  else
    printf("error dynapi ARC.thickness\n");

  // Returns arc extrusion
  dwg_ent_arc_get_extrusion(arc, &ext, &error);
  if ( !error )
    printf("arc_get_extrusion: %f, %f, %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");
  if (dwg_dynapi_entity_value(arc, "ARC", "extrusion", &ext, NULL) &&
      ext.x == arc->extrusion.x &&
      ext.y == arc->extrusion.y &&
      ext.z == arc->extrusion.z)
    printf("ARC.extrusion    : %f, %f, %f\n", ext.x,ext.y,ext.z);
  else
    printf("error dynapi ARC.extrusion\n");

  dwg_ent_arc_get_center(arc, &center, &error);
  if ( !error )
      printf("arc_get_center   : %f, %f, %f\n",
              center.x, center.y, center.z);
  else
      printf("error in reading center \n");
  if (dwg_dynapi_entity_value(arc, "ARC", "center", &center, NULL) &&
      center.x == arc->center.x &&
      center.y == arc->center.y &&
      center.z == arc->center.z)
    printf("ARC.center       : %f, %f, %f\n", center.x,center.y,center.z);
  else
    printf("error dynapi ARC.center\n");

  start_angle = dwg_ent_arc_get_start_angle(arc, &error);
  if ( !error )
    printf("get_start_angle  : %f\n", start_angle);
  else
    printf("error in reading start angle\n");
  if (dwg_dynapi_entity_value(arc, "ARC", "start_angle", &start_angle, NULL) &&
      start_angle == arc->start_angle)
      printf("ARC.start_angle  : %f\n", start_angle);
  else
      printf("error dynapi ARC.start_angle\n");

  end_angle = dwg_ent_arc_get_end_angle(arc, &error);
  if ( !error )
      printf("arc_get_end_angle: %f\n", end_angle);
  else
      printf("error in reading end angle \n");
  if (dwg_dynapi_entity_value(arc, "ARC", "end_angle", &end_angle, NULL) &&
      end_angle == arc->end_angle)
      printf("ARC.end_angle    : %f\n", end_angle);
  else
      printf("error dynapi ARC.end_angle\n");
}

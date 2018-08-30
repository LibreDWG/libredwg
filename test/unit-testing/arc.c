#define DWG_TYPE DWG_TYPE_ARC
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);

  printf("Radius of arc : %f\n",arc->radius);
  printf("Thickness of arc : %f\n",arc->thickness);
  printf("extrusion of arc : x = %f, y = %f, z = %f\n", 
          arc->extrusion.x, arc->extrusion.y, arc->extrusion.z);
  printf("center of arc : x = %f,y = %f,z = %f\n",
          arc->center.x, arc->center.y, arc->center.z);
  printf("start angle of arc : %f\n", arc->start_angle);
  printf("end angle of arc : %f\n", arc->end_angle);
}

void
api_process(dwg_object *obj)
{
  int error;
  double radius, thickness, start_angle, end_angle;
  dwg_point_3d center, ext;

  // casts an object to arc entity 
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);

  // returns radius of arc
  radius = dwg_ent_arc_get_radius(arc, &error);
  if ( !error )
      printf("Radius of arc : %f\n", radius);
  else
      printf("error in reading radius \n");


  thickness = dwg_ent_arc_get_thickness(arc, &error);
  if ( !error )
      printf("Thickness of arc : %f\n", thickness);
  else
      printf("error in reading thickness \n");
 
  // Returns arc extrusion
  dwg_ent_arc_get_extrusion(arc, &ext,
                            &error);
  if ( !error )
    {
      printf("extrusion of arc : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }


  dwg_ent_arc_get_center(arc, &center,
                         &error);
  if ( !error )
    {
      printf("center of arc : x = %f, y = %f, z = %f\n",
              center.x, center.y, center.z);
    }
  else
    {
      printf("error in reading center \n");
    }


  start_angle = dwg_ent_arc_get_start_angle(arc, &error);
  if ( !error )
    {
      printf("start angle of arc : %f\n", start_angle);
    }
  else
    {
      printf("error in reading start angle\n");
    }


  thickness = dwg_ent_arc_get_end_angle(arc, &error);
  if ( !error )
    {
      printf("End angle of arc : %f\n",thickness);
    }
  else
    {
      printf("error in reading end angle \n");
    }
}

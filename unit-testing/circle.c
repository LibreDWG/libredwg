#define DWG_TYPE DWG_TYPE_CIRCLE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casting object to circle entity
  dwg_ent_circle *circle = dwg_object_to_CIRCLE(obj);

  // prints circle radius
  printf("Radius of circle : %f\n",circle->radius);

  // prints circle thickness
  printf("Thickness of circle : %f\n",circle->thickness);

  // prints circle extrusion
  printf("extrusion of circle : x = %f, y = %f, z = %f\n", 
          circle->extrusion.x, circle->extrusion.y, circle->extrusion.z);

  // prints circle center
  printf("center of circle : x = %f,y = %f,z = %f\n",
          circle->center.x, circle->center.y, circle->center.z);
}

void
api_process(dwg_object *obj)
{
  int error; // Error reporting
  double radius, thickness;
  dwg_point_3d center, ext;  //3d_points 

  // casting object to circle entity
  dwg_ent_circle *circle = dwg_object_to_CIRCLE(obj);

  // returns circle radius
  radius = dwg_ent_circle_get_radius(circle, &error);
  if ( !error )
    {  
      printf("Radius of circle : %f\n",radius);
    }
  else
    {
      printf("error in reading radius \n");
    }

  // returns circle thickness
  thickness = dwg_ent_circle_get_thickness(circle, &error);
  if ( !error )
    {
      printf("Thickness of circle : %f\n",thickness);
    }
  else
    {
      printf("error in reading thickness \n");
    }

  // returns circle extrusion
  dwg_ent_circle_get_extrusion(circle, &ext,
                               &error);
  if ( !error )
    {
      printf("extrusion of circle : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // return circle center points
  dwg_ent_circle_get_center(circle, &center,
                            &error);
  if ( !error )
    {
      printf("center of circle : x = %f, y = %f, z = %f\n",
              center.x, center.y, center.z);
    }
  else
    {
      printf("error in reading center \n");
    }

}

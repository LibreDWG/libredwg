#include "common.c"

/// Output processing function
void
output_process(dwg_object *obj);

/// Checks the respective DWG entity/object type and then calls the output_process()
void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_CIRCLE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casting object to circle entity
  dwg_ent_circle *circle = dwg_object_to_CIRCLE(obj);

  // prints circle radius
  printf("Radius of circle : %f\t\n",circle->radius);

  // prints circle thickness
  printf("Thickness of circle : %f\t\n",circle->thickness);

  // prints circle extrusion
  printf("extrusion of circle : x = %f, y = %f, z = %f\t\n", 
          circle->extrusion.x, circle->extrusion.y, circle->extrusion.z);

  // prints circle center
  printf("center of circle : x = %f,y = %f,z = %f\t\n",
          circle->center.x, circle->center.y, circle->center.z);
}

void
api_process(dwg_object *obj)
{
  int radius_error, thickness_error, ext_error, center_error; // Error reporting
  float radius, thickness;
  dwg_point_3d center, ext;  //3d_points 

  // casting object to circle entity
  dwg_ent_circle *circle = dwg_object_to_CIRCLE(obj);

  // returns circle radius
  radius = dwg_ent_circle_get_radius(circle, &radius_error);
  if(radius_error == 0 ) // Error checking
    {  
      printf("Radius of circle : %f\t\n",radius);
    }
  else
    {
      printf("error in reading radius \n");
    }

  // returns circle thickness
  thickness = dwg_ent_circle_get_thickness(circle, &thickness_error);
  if(thickness_error == 0 ) // error checking
    {
      printf("Thickness of circle : %f\t\n",thickness);
    }
  else
    {
      printf("error in reading thickness \n");
    }

  // returns circle extrusion
  dwg_ent_circle_get_extrusion(circle, &ext,&ext_error);
  if(ext_error == 0 ) // error checking
    {
      printf("extrusion of circle : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // return circle center points
  dwg_ent_circle_get_center(circle, &center,&center_error);
  if(center_error == 0 ) // error checking
    {
      printf("center of circle : x = %f, y = %f, z = %f\t\n",
              center.x, center.y, center.z);
    }
  else
    {
      printf("error in reading center \n");
    }

}

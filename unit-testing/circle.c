#include "common.c"

void
output_process(dwg_object *obj);

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
  dwg_ent_circle *circle = dwg_object_to_CIRCLE(obj);
  printf("Radius of circle : %f\t\n",circle->radius);
  printf("Thickness of circle : %f\t\n",circle->thickness);
  printf("extrusion of circle : x = %f, y = %f, z = %f\t\n", 
          circle->extrusion.x, circle->extrusion.y, circle->extrusion.z);
  printf("center of circle : x = %f,y = %f,z = %f\t\n",
          circle->center.x, circle->center.y, circle->center.z);
}

void
api_process(dwg_object *obj)
{
  int radius_error, thickness_error, ext_error, center_error;
  float radius, thickness;
  dwg_point_3d center, ext;
  dwg_ent_circle *circle = dwg_object_to_CIRCLE(obj);

  radius = dwg_ent_circle_get_radius(circle, &radius_error);
  if(radius_error == 0 )
    {  
      printf("Radius of circle : %f\t\n",radius);
    }
  else
    {
      printf("error in reading radius \n");
    }

  thickness = dwg_ent_circle_get_thickness(circle, &thickness_error);
  if(thickness_error == 0 )
    {
      printf("Thickness of circle : %f\t\n",thickness);
    }
  else
    {
      printf("error in reading thickness \n");
    }

  dwg_ent_circle_get_extrusion(circle, &ext,&ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of circle : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_circle_get_center(circle, &center,&center_error);
  if(center_error == 0 )
    {
      printf("center of circle : x = %f, y = %f, z = %f\t\n",
              center.x, center.y, center.z);
    }
  else
    {
      printf("error in reading center \n");
    }

}

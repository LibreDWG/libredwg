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

  if (dwg_get_type(obj)== DWG_TYPE_ARC)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts a dwg object to arc entity
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);

  // prints radius of arc
  printf("Radius of arc : %f\t\n",arc->radius);

  // prints thickness of arc
  printf("Thickness of arc : %f\t\n",arc->thickness);

  // prints extrusion value
  printf("extrusion of arc : x = %f, y = %f, z = %f\t\n", 
          arc->extrusion.x, arc->extrusion.y, arc->extrusion.z);

  // prints cemter value of arc
  printf("center of arc : x = %f,y = %f,z = %f\t\n",
          arc->center.x, arc->center.y, arc->center.z);

  // prints start angle of arc
  printf("start angle of arc : %f\t\n", arc->start_angle);

  // prints end angle of arc
  printf("end angle of arc : %f\t\n", arc->end_angle);
}

void
api_process(dwg_object *obj)
{
  int radius_error, thickness_error, ext_error, center_error,start_error, end_error;
  float radius, thickness, start_angle, end_angle;
  dwg_point_3d center, ext;

  // casts an object to arc entity 
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);

  // retursn radius of arc
  radius = dwg_ent_arc_get_radius(arc, &radius_error);
  if(radius_error == 0 ) // error checking 
    {  
      printf("Radius of arc : %f\t\n",radius);
    }
  else
    {
      printf("error in reading radius \n");
    }

  // returns thickness of arc
  thickness = dwg_ent_arc_get_thickness(arc, &thickness_error);
  if(thickness_error == 0 )  // error checking
    {
      printf("Thickness of arc : %f\t\n",thickness);
    }
  else
    {
      printf("error in reading thickness \n");
    }
 
  // Returns arc extrusion
  dwg_ent_arc_get_extrusion(arc, &ext,&ext_error);
  if(ext_error == 0 ) // error checking 
    {
      printf("extrusion of arc : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns center of arc
  dwg_ent_arc_get_center(arc, &center,&center_error);
  if(center_error == 0 ) // arc checking
    {
      printf("center of arc : x = %f, y = %f, z = %f\t\n",
              center.x, center.y, center.z);
    }
  else
    {
      printf("error in reading center \n");
    }

  // returns start angle  
  start_angle = dwg_ent_arc_get_start_angle(arc, &start_error);
  if(start_error == 0 )  // error checking
    {
      printf("start angle of arc : %f\t\n", start_angle);
    }
  else
    {
      printf("error in reading start angle\n");
    }

  // returns thickness angle
  thickness = dwg_ent_arc_get_end_angle(arc, &end_error);
  if(end_error == 0 )
    {
      printf("End angle of arc : %f\t\n",thickness);
    }
  else
    {
      printf("error in reading end angle \n");
    }
}

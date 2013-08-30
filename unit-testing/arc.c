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
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);
  printf("Radius of arc : %f\t\n",arc->radius);
  printf("Thickness of arc : %f\t\n",arc->thickness);
  printf("extrusion of arc : x = %f, y = %f, z = %f\t\n", 
          arc->extrusion.x, arc->extrusion.y, arc->extrusion.z);
  printf("center of arc : x = %f,y = %f,z = %f\t\n",
          arc->center.x, arc->center.y, arc->center.z);
  printf("start angle of arc : %f\t\n", arc->start_angle);
  printf("end angle of arc : %f\t\n", arc->end_angle);
}

void
api_process(dwg_object *obj)
{
  int radius_error, thickness_error, ext_error, center_error,start_error, end_error;
  float radius, thickness, start_angle, end_angle;
  dwg_point_3d center, ext;
  dwg_ent_arc *arc = dwg_object_to_ARC(obj);

  radius = dwg_ent_arc_get_radius(arc, &radius_error);
  if(radius_error == 0 )
    {  
      printf("Radius of arc : %f\t\n",radius);
    }
  else
    {
      printf("error in reading radius \n");
    }

  thickness = dwg_ent_arc_get_thickness(arc, &thickness_error);
  if(thickness_error == 0 )
    {
      printf("Thickness of arc : %f\t\n",thickness);
    }
  else
    {
      printf("error in reading thickness \n");
    }

  dwg_ent_arc_get_extrusion(arc, &ext,&ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of arc : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_arc_get_center(arc, &center,&center_error);
  if(center_error == 0 )
    {
      printf("center of arc : x = %f, y = %f, z = %f\t\n",
              center.x, center.y, center.z);
    }
  else
    {
      printf("error in reading center \n");
    }
  
  start_angle = dwg_ent_arc_get_start_angle(arc, &start_error);
  if(start_error == 0 )
    {
      printf("start angle of arc : %f\t\n", start_angle);
    }
  else
    {
      printf("error in reading start angle\n");
    }
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

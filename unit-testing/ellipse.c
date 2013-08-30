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

  if (dwg_get_type(obj)== DWG_TYPE_ELLIPSE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE(obj);
  printf("Ratio of ellipse : %f\t\n",ellipse->axis_ratio);
  printf("sm axis of ellipse : x = %f, y = %f, z = %f\t\n", 
          ellipse->sm_axis.x, ellipse->sm_axis.y, ellipse->sm_axis.z);
  printf("extrusion of ellipse : x = %f, y = %f, z = %f\t\n", 
          ellipse->extrusion.x, ellipse->extrusion.y, ellipse->extrusion.z);
  printf("center of ellipse : x = %f,y = %f,z = %f\t\n",
          ellipse->center.x, ellipse->center.y, ellipse->center.z);
  printf("start angle of ellipse : %f\t\n", ellipse->start_angle);
  printf("end angle of ellipse : %f\t\n", ellipse->end_angle);
}

void
api_process(dwg_object *obj)
{
  int ratio_error, sm_error, ext_error, center_error, start_error, end_error;
  float axis_ratio, start_angle, end_angle;
  dwg_point_3d center, sm, ext;
  dwg_ent_ellipse *ellipse = dwg_object_to_ELLIPSE(obj);

  axis_ratio = dwg_ent_ellipse_get_axis_ratio(ellipse, &ratio_error);
  if(ratio_error == 0 )
    {  
      printf("axis ratio of ellipse : %f\t\n",axis_ratio);
    }
  else
    {
      printf("error in reading axis_ratio \n");
    }
  
  dwg_ent_ellipse_get_sm_axis(ellipse, &sm,&sm_error);
  if(sm_error == 0 )
    {
      printf("sm axis of ellipse : x = %f, y = %f, z = %f\t\n",
              sm.x, sm.y, sm.z);
    }
  else
    {
      printf("error in reading sm axis \n");
    }

  dwg_ent_ellipse_get_extrusion(ellipse, &ext,&ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of ellipse : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_ellipse_get_center(ellipse, &center,&center_error);
  if(center_error == 0 )
    {
      printf("center of ellipse : x = %f, y = %f, z = %f\t\n",
              center.x, center.y, center.z);
    }
  else
    {
      printf("error in reading center \n");
    }
  
  start_angle = dwg_ent_ellipse_get_start_angle(ellipse, &start_error);
  if(start_error == 0 )
    {
      printf("start angle of ellipse : %f\t\n", start_angle);
    }
  else
    {
      printf("error in reading start angle\n");
    }
  end_angle = dwg_ent_ellipse_get_end_angle(ellipse, &end_error);
  if(end_error == 0 )
    {
      printf("End angle of ellipse : %f\t\n", end_angle);
    }
  else
    {
      printf("error in reading end angle \n");
    }
 
}

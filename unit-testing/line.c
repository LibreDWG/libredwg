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

  if (dwg_get_type(obj)== DWG_TYPE_LINE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts dwg object to line entity
  dwg_ent_line *line = dwg_object_to_LINE(obj);

  // prints start points
  printf("start points of line : x = %f,y = %f\t\n",line->start.x,
          line->start.y);

  // prints end points
  printf("end of line : x = %f,y = %f\t\n",
          line->end.x, line->end.y);

  // prints thickness
  printf("Thickness of line : %f\t\n",line->thickness);

  // prints extrusion
  printf("extrusion of line : x = %f, y = %f, z = %f\t\n", 
          line->extrusion.x, line->extrusion.y, line->extrusion.z);
}

void
api_process(dwg_object *obj)
{
  int start_error, thickness_error, ext_error, end_error;
  float thickness;
  dwg_point_3d ext, start, end;
  dwg_ent_line *line = dwg_object_to_LINE(obj);


  // returns start point
  dwg_ent_line_get_start_point(line, &start ,&start_error);
  if(start_error == 0 ) // error check
    {
      printf("start points of line : x = %f, y = %f\t\n",
              start.x, start.y);
    }
  else
    {
      printf("error in reading start points \n");
    }

  // returns end point
  dwg_ent_line_get_end_point(line, &end ,&end_error);
  if(end_error == 0 ) // error check
    {
      printf("end points of line : x = %f, y = %f\t\n",
              end.x, end.y);
    }
  else
    {
      printf("error in reading end points \n");
    }

  // returns thickness
  thickness = dwg_ent_line_get_thickness(line, &thickness_error);
  if(thickness_error == 0 ) // error check
    {
      printf("Thickness of line : %f\t\n",thickness);
    }
  else
    {
      printf("error in reading thickness \n");
    }

  // returns extrusion
  dwg_ent_line_get_extrusion(line, &ext, &ext_error);
  if(ext_error == 0 ) // error check
    {
      printf("extrusion of line : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }
}

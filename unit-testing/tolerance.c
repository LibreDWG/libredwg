#define DWG_TYPE DWG_TYPE_TOLERANCE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casting object to tolerance entity
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE(obj);

  // prints tolerance radius
  printf("Radius of tolerance : %f\n",tolerance->height);

  // prints tolerance thickness
  printf("Thickness of tolerance : %f\n",tolerance->dimgap);

  // prints text string
  printf("text string of tolerance : %s\n",tolerance->text_string);

  // prints tolerance extrusion
  printf("extrusion of tolerance : x = %f, y = %f, z = %f\n", 
          tolerance->extrusion.x, tolerance->extrusion.y, tolerance->extrusion.z);

  // prints tolerance ins_pt
  printf("ins_pt of tolerance : x = %f, y = %f, z = %f\n", 
          tolerance->ins_pt.x, tolerance->ins_pt.y, tolerance->ins_pt.z);

  // prints tolerance center
  printf("center of tolerance : x = %f,y = %f,z = %f\n",
          tolerance->x_direction.x, tolerance->x_direction.y, 
          tolerance->x_direction.z);
}

void
api_process(dwg_object *obj)
{
  int error;
  double height, dimgap;
  dwg_point_3d ins_pt, x_dir, ext;  //3d_points 
  char * text_string;

  // casting object to tolerance entity
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE(obj);

  // returns tolerance height
  height = dwg_ent_tolerance_get_height(tolerance, &error);
  if ( !error )
    {  
      printf("height of tolerance : %f\n", height);
    }
  else
    {
      printf("error in reading height \n");
    }

  // returns tolerance dimgap
  dimgap = dwg_ent_tolerance_get_dimgap(tolerance, &error);
  if ( !error )
    {
      printf("dimgap of tolerance : %f\n", dimgap);
    }
  else
    {
      printf("error in reading dimgao \n");
    }

  // returns tolerance text
  text_string = dwg_ent_tolerance_get_text_string(tolerance, &error);
  if ( !error )
    {
      printf("text of tolerance : %s\n", text_string);
    }
  else
    {
      printf("error in reading text \n");
    }

  // returns tolerance extrusion
  dwg_ent_tolerance_get_ins_pt(tolerance, &ins_pt,
                               &error);
  if ( !error )
    {
      printf("ins pt of tolerance : x = %f, y = %f, z = %f\n",
              ins_pt.x, ins_pt.y, ins_pt.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // return tolerance center points
  dwg_ent_tolerance_get_x_direction(tolerance, &x_dir, &error);
  if ( !error )
    {
      printf("x direction of tolerance : x = %f, y = %f, z = %f\n",
              x_dir.x, x_dir.y, x_dir.z);
    }
  else
    {
      printf("error in reading x direction \n");
    }

}

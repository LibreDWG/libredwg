#include "common.c"

// Output processing function
void
output_process(dwg_object *obj);

// Checks the respective DWG entity/object type and then calls the output_process()
void
output_object(dwg_object* obj){
  if (!obj)
    {
      printf("object is NULL\n");
      return;
    }

  if (dwg_get_type(obj)== DWG_TYPE_TOLERANCE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casting object to tolerance entity
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE(obj);

  // prints tolerance radius
  printf("Radius of tolerance : %f\t\n",tolerance->height);

  // prints tolerance thickness
  printf("Thickness of tolerance : %f\t\n",tolerance->dimgap);

  // prints text string
  printf("text string of tolerance : %s\t\n",tolerance->text_string);

  // prints tolerance extrusion
  printf("extrusion of tolerance : x = %f, y = %f, z = %f\t\n", 
          tolerance->extrusion.x, tolerance->extrusion.y, tolerance->extrusion.z);

  // prints tolerance ins_pt
  printf("ins_pt of tolerance : x = %f, y = %f, z = %f\t\n", 
          tolerance->ins_pt.x, tolerance->ins_pt.y, tolerance->ins_pt.z);

  // prints tolerance center
  printf("center of tolerance : x = %f,y = %f,z = %f\t\n",
          tolerance->x_direction.x, tolerance->x_direction.y, 
          tolerance->x_direction.z);
}

void
api_process(dwg_object *obj)
{
  int height_error, dimgap_error, ext_error, ins_pt_error, x_dir_error, text_error;
  float height, dimgap;
  dwg_point_3d ins_pt, x_dir, ext;  //3d_points 
  char * text_string;

  // casting object to tolerance entity
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE(obj);

  // returns tolerance height
  height = dwg_ent_tolerance_get_height(tolerance, &height_error);
  if(height_error == 0 ) // Error checking
    {  
      printf("height of tolerance : %f\t\n", height);
    }
  else
    {
      printf("error in reading height \n");
    }

  // returns tolerance dimgap
  dimgap = dwg_ent_tolerance_get_dimgap(tolerance, &dimgap_error);
  if(dimgap_error == 0 ) // error checking
    {
      printf("dimgap of tolerance : %f\t\n", dimgap);
    }
  else
    {
      printf("error in reading dimgao \n");
    }

  // returns tolerance text
  text_string = dwg_ent_tolerance_get_text_string(tolerance, &text_error);
  if(text_error == 0 ) // error checking
    {
      printf("text of tolerance : %s\t\n", text_string);
    }
  else
    {
      printf("error in reading text \n");
    }

  // returns tolerance extrusion
  dwg_ent_tolerance_get_ins_pt(tolerance, &ins_pt, &ins_pt_error);
  if(ins_pt_error == 0 ) // error checking
    {
      printf("ins pt of tolerance : x = %f, y = %f, z = %f\t\n",
              ins_pt.x, ins_pt.y, ins_pt.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // return tolerance center points
  dwg_ent_tolerance_get_x_direction(tolerance, &x_dir, &x_dir_error);
  if( x_dir_error == 0 ) // error checking
    {
      printf("x direction of tolerance : x = %f, y = %f, z = %f\t\n",
              x_dir.x, x_dir.y, x_dir.z);
    }
  else
    {
      printf("error in reading x direction \n");
    }

}

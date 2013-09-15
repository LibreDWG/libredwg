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

  if (dwg_get_type(obj)== DWG_TYPE_TEXT)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{

  // casts a dwg object of text
  dwg_ent_text *text = dwg_object_to_TEXT(obj);

  // prints text value of text
  printf("text of text : %s\t\n", text->text_value);

  // prints insertion point of text
  printf("insertion point of text : x = %f, y = %f\t\n", 
          text->insertion_pt.x, text->insertion_pt.y);

  // prints extrusion
  printf("extrusion of text : x = %f, y = %f, z = %f\t\n",
          text->extrusion.x, text->extrusion.y, text->extrusion.z);

  // prints height of text
  printf("height of text : %f\t\n", text->height);

  // prints thickness of text
  printf("thickness of text : %f\t\n", text->thickness);

  // prints rotation of text
  printf("rotation of text : %f\t\n", text->rotation_ang);

  // prints vertical alignment
  printf("vertical align of text : %du\t\n", text->vert_alignment);

  // prints horizontal alignment
  printf("horizontal align of text : %du\t\n", text->horiz_alignment);
}

void
api_process(dwg_object *obj)
{
  int value_error, ins_pt_error, ext_error, thickness_error, rot_error, 
      vert_error, horiz_error, height_error;
  float thickness, rotation, vert_align, horiz_align, height;
  char * text_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_text *text = dwg_object_to_TEXT(obj);

  // returns text value
  text_value = dwg_ent_text_get_text(text, &value_error);
  if( value_error== 0 ) // error check
    {  
      printf("text value : %s\t\n",text_value);
    }
  else
    {
      printf("error in reading text_value \n");
    }

  // returns insertion point  
  dwg_ent_text_get_insertion_point(text, &ins_pt, &ins_pt_error);
  if(ins_pt_error == 0 ) // error checks
    {
      printf("insertion point of text : x = %f, y = %f\t\n",
              ins_pt.x, ins_pt.y);
    }
  else
    {
      printf("error in reading insertion \n");
    }

  // returns extrusion of text
  dwg_ent_text_get_extrusion(text, &ext, &ext_error);
  if(ext_error == 0 ) // error checking
    {
      printf("extrusion of text : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // retruns height of text
  height = dwg_ent_text_get_height(text, &height_error);
  if(height_error == 0 ) // error checking
    {
      printf("height of text : %f\t\n", height);
    }
  else
    {
      printf("error in reading height \n");
    }

  // returns thickness 
  thickness = dwg_ent_text_get_thickness(text, &thickness_error);
  if(thickness_error == 0 ) // error checking
    {
      printf("thickness of text : %f\t\n", thickness);
    }
  else
    {
      printf("error in reading thickness\n");
    }

  // returns rotation angle
  rotation = dwg_ent_text_get_rot_angle(text, &rot_error);
  if(rot_error == 0 ) // error check
    {
      printf("rotation of text : %f\t\n", rotation);
    }
  else
    {
      printf("error in reading rotation \n");
    }

  // returns vertical alignment
  vert_align = dwg_ent_text_get_vert_align(text, &vert_error);
  if( vert_error == 0 ) // error check
    {
      printf("Vertical alignment of text : %f\t\n", vert_align);
    }
  else
    {
      printf("error in reading vertical alignment");
    }

  // returns horizonatal alignment
  horiz_align = dwg_ent_text_get_horiz_align(text, &horiz_error);
  if(horiz_error == 0) // error checks
    {
      printf("Horizontal alignment of text : %f\t\n", horiz_align);
    }
  else
    {
      printf("error in reading horizontal alignment");
    }
}

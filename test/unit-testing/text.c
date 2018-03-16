#define DWG_TYPE DWG_TYPE_TOLERANCE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casts a dwg object of text
  dwg_ent_text *text = dwg_object_to_TEXT(obj);

  // prints text value of text
  printf("text of text : %s\n", text->text_value);

  // prints insertion point of text
  printf("insertion point of text : x = %f, y = %f\n", 
          text->insertion_pt.x, text->insertion_pt.y);

  // prints extrusion
  printf("extrusion of text : x = %f, y = %f, z = %f\n",
          text->extrusion.x, text->extrusion.y, text->extrusion.z);

  // prints height of text
  printf("height of text : %f\n", text->height);

  // prints thickness of text
  printf("thickness of text : %f\n", text->thickness);

  // prints rotation of text
  printf("rotation of text : %f\n", text->rotation_ang);

  // prints vertical alignment
  printf("vertical align of text : %u\n", text->vert_alignment);

  // prints horizontal alignment
  printf("horizontal align of text : %u\n", text->horiz_alignment);
}

void
api_process(dwg_object *obj)
{
  int error;
  double thickness, rotation, vert_align, horiz_align, height;
  char * text_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_text *text = dwg_object_to_TEXT(obj);

  // returns text value
  text_value = dwg_ent_text_get_text(text, &error);
  if (!error)
    {  
      printf("text value : %s\n",text_value);
    }
  else
    {
      printf("error in reading text_value \n");
    }

  // returns insertion point  
  dwg_ent_text_get_insertion_point(text, &ins_pt, &error);
  if (!error)
    {
      printf("insertion point of text : x = %f, y = %f\n",
              ins_pt.x, ins_pt.y);
    }
  else
    {
      printf("error in reading insertion \n");
    }

  // returns extrusion of text
  dwg_ent_text_get_extrusion(text, &ext, &error);
  if (!error)
    {
      printf("extrusion of text : x = %f, y = %f, z = %f\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // retruns height of text
  height = dwg_ent_text_get_height(text, &error);
  if (!error)
    {
      printf("height of text : %f\n", height);
    }
  else
    {
      printf("error in reading height \n");
    }

  // returns thickness 
  thickness = dwg_ent_text_get_thickness(text, &error);
  if (!error)
    {
      printf("thickness of text : %f\n", thickness);
    }
  else
    {
      printf("error in reading thickness\n");
    }

  // returns rotation angle
  rotation = dwg_ent_text_get_rot_angle(text, &error);
  if (!error)
    {
      printf("rotation of text : %f\n", rotation);
    }
  else
    {
      printf("error in reading rotation \n");
    }

  // returns vertical alignment
  vert_align = dwg_ent_text_get_vert_align(text, &error);
  if (!error)
    {
      printf("Vertical alignment of text : %f\n", vert_align);
    }
  else
    {
      printf("error in reading vertical alignment");
    }

  // returns horizonatal alignment
  horiz_align = dwg_ent_text_get_horiz_align(text, &error);
  if (!error)
    {
      printf("Horizontal alignment of text : %f\n", horiz_align);
    }
  else
    {
      printf("error in reading horizontal alignment");
    }
}

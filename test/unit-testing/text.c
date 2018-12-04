#define DWG_TYPE DWG_TYPE_TOLERANCE
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_text *text = dwg_object_to_TEXT(obj);

  printf("text of text : %s\n", text->text_value);
  printf("insertion point of text : x = %f, y = %f\n",
          text->insertion_pt.x, text->insertion_pt.y);
  printf("extrusion of text : x = %f, y = %f, z = %f\n",
          text->extrusion.x, text->extrusion.y, text->extrusion.z);
  printf("height of text : %f\n", text->height);
  printf("thickness of text : %f\n", text->thickness);
  printf("rotation of text : %f\n", text->rotation);
  printf("vertical align of text : %u\n", text->vert_alignment);
  printf("horizontal align of text : %u\n", text->horiz_alignment);
}

void
api_process(dwg_object *obj)
{
  int error;
  double thickness, rotation, height;
  BITCODE_BS vert_align, horiz_align;
  char * text_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;

  dwg_ent_text *text = dwg_object_to_TEXT(obj);

  text_value = dwg_ent_text_get_text(text, &error);
  if (!error)
    printf("text value : %s\n",text_value);
  else
    printf("error in reading text_value \n");

  dwg_ent_text_get_insertion_point(text, &ins_pt, &error);
  if (!error)
    printf("insertion point of text : x = %f, y = %f\n",
           ins_pt.x, ins_pt.y);
  else
    printf("error in reading insertion \n");

  dwg_ent_text_get_extrusion(text, &ext, &error);
  if (!error)
    printf("extrusion of text : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");

  height = dwg_ent_text_get_height(text, &error);
  if (!error)
    printf("height of text : %f\n", height);
  else
    printf("error in reading height \n");

  thickness = dwg_ent_text_get_thickness(text, &error);
  if (!error)
    printf("thickness of text : %f\n", thickness);
  else
    printf("error in reading thickness\n");

  rotation = dwg_ent_text_get_rotation(text, &error);
  if (!error)
    printf("rotation of text : %f\n", rotation);
  else
    printf("error in reading rotation \n");


  vert_align = dwg_ent_text_get_vert_alignment(text, &error);
  if (!error)
    printf("Vertical alignment of text : " FORMAT_BS "\n", vert_align);
  else
    printf("error in reading vertical alignment");


  horiz_align = dwg_ent_text_get_horiz_alignment(text, &error);
  if (!error)
      printf("Horizontal alignment of text : " FORMAT_BS "\n", horiz_align);
  else
      printf("error in reading horizontal alignment");
}

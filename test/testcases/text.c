#define DWG_TYPE DWG_TYPE_TEXT
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_text *text = dwg_object_to_TEXT (obj);

  printf ("text of text : %s\n", text->text_value);
  printf ("insertion point of text : x = %f, y = %f\n",
	  text->insertion_pt.x, text->insertion_pt.y);
  printf ("extrusion of text : x = %f, y = %f, z = %f\n",
	  text->extrusion.x, text->extrusion.y, text->extrusion.z);
  printf ("height of text : %f\n", text->height);
  printf ("thickness of text : %f\n", text->thickness);
  printf ("rotation of text : %f\n", text->rotation_ang);
  printf ("vertical align of text : %du\n", text->vert_alignment);
  printf ("horizontal align of text : %du\n", text->horiz_alignment);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD thickness, rotation, height;
  BITCODE_BS vert_align, horiz_align;
  char *text_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_text *text = dwg_object_to_TEXT (obj);


  text_value = dwg_ent_text_get_text (text, &error);
  if (!error  && !strcmp (text_value, text->text_value))	// error check
    pass ("Working Properly");
  else
    fail ("error in reading text_value");


  dwg_ent_text_get_insertion_point (text, &ins_pt, &error);
  if (!error  && ins_pt.x == text->insertion_pt.x && ins_pt.y == text->insertion_pt.y)	// error checks
    pass ("Working Properly");
  else
    fail ("error in reading insertion");


  dwg_ent_text_get_extrusion (text, &ext, &error);
  if (!error  && text->extrusion.x == ext.x && text->extrusion.y == ext.y && text->extrusion.z == ext.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  // retruns height of text
  height = dwg_ent_text_get_height (text, &error);
  if (!error  && text->height == height)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading height");


  thickness = dwg_ent_text_get_thickness (text, &error);
  if (!error  && thickness == text->thickness)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading thickness");


  rotation = dwg_ent_text_get_rot_angle (text, &error);
  if (!error  && rotation == text->rotation_ang)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading rotation");


  vert_align = dwg_ent_text_get_vert_align (text, &error);
  if (!error  && vert_align == text->vert_alignment)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading vertical alignment");


  horiz_align = dwg_ent_text_get_horiz_align (text, &error);
  if (!error  && horiz_align == text->horiz_alignment)	// error checks
    pass ("Working Properly");
  else
    fail ("error in reading horizontal alignment");
}

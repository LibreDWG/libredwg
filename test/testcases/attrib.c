#define DWG_TYPE DWG_TYPE_ATTRIB
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD thickness, rotation, height;
  BITCODE_BS vert_align, horiz_align;
  char *attrib_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;

  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB (obj);

  attrib_value = dwg_ent_attrib_get_text (attrib, &error);
  if (!error  && attrib_value == attrib->text_value)
    pass ("Working Properly");
  else
    fail ("error in reading attrib_value");

  dwg_ent_attrib_get_insertion_point (attrib, &ins_pt, &error);
  if (!error  && ins_pt.x == attrib->insertion_pt.x && ins_pt.y == attrib->insertion_pt.y)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading insertion");

  dwg_ent_attrib_get_extrusion (attrib, &ext, &error);
  if (!error  && ext.x == attrib->extrusion.x && ext.y == attrib->extrusion.y && ext.z == attrib->extrusion.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");

  height = dwg_ent_attrib_get_height (attrib, &error);
  if (!error  && height == attrib->height)
    pass ("Working Properly");
  else
    fail ("error in reading height");

  thickness = dwg_ent_attrib_get_thickness (attrib, &error);
  if (!error  && attrib->thickness == thickness)
    pass ("Working Properly");
  else
    fail ("error in reading thickness");

  rotation = dwg_ent_attrib_get_rotation (attrib, &error);
  if (!error  && attrib->rotation == rotation)
    pass ("Working Properly");
  else
    fail ("error in reading rotation");

  vert_align = dwg_ent_attrib_get_vert_alignment (attrib, &error);
  if (!error  && vert_align == attrib->vert_alignment)
    pass ("Working Properly");
  else
    fail ("error in reading vertical alignment");

  horiz_align = dwg_ent_attrib_get_horiz_alignment (attrib, &error);
  if (!error  && attrib->horiz_alignment == horiz_align)
    pass ("Working Properly");
  else
    fail ("error in reading horizontal alignment");
}

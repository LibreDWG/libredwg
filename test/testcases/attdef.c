#define DWG_TYPE DWG_TYPE_ATTDEF
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
  char *attdef_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF (obj);

  // return attdef value
  attdef_value = dwg_ent_attdef_get_text (attdef, &error);
  if (!error  && attdef_value == attdef->default_value)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading attdef_value");


  dwg_ent_attdef_get_insertion_point (attdef, &ins_pt, &error);
  if (!error  && ins_pt.x == attdef->insertion_pt.x && ins_pt.y == attdef->insertion_pt.y)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading insertion");


  dwg_ent_attdef_get_extrusion (attdef, &ext, &error);
  if (!error  && ext.x == attdef->extrusion.x && ext.y == attdef->extrusion.y && ext.z == attdef->extrusion.z)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  height = dwg_ent_attdef_get_height (attdef, &error);
  if (!error  && height == attdef->height)	// error checking
    pass ("Working Properly");
  else
    fail ("error in reading height");


  thickness = dwg_ent_attdef_get_thickness (attdef, &error);
  if (!error  && attdef->thickness == thickness)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading thickness");


  rotation = dwg_ent_attdef_get_rot_angle (attdef, &error);
  if (!error  && attdef->rotation_ang == rotation)
    pass ("Working Properly");
  else
    fail ("error in reading rotation");


  vert_align = dwg_ent_attdef_get_vert_align (attdef, &error);
  if (!error  && attdef->vert_alignment == vert_align)
    pass ("Working Properly");
  else
    fail ("error in reading vertical alignment");


  horiz_align = dwg_ent_attdef_get_horiz_align (attdef, &error);
  if (!error  && horiz_align == attdef->horiz_alignment)
    pass ("Working Properly");
  else
    fail ("error in reading horizontal alignment");

}

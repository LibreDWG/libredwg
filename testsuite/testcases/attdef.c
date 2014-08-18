#include "common.c"
#include <dejagnu.h>

void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_ATTDEF)
    {
      output_process (obj);
    }
}

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int value_error, ins_pt_error, ext_error, thickness_error, rot_error,
    vert_error, horiz_error, height_error;
  float thickness, rotation, vert_align, horiz_align, height;
  char *attdef_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF (obj);

  // return attdef value
  attdef_value = dwg_ent_attdef_get_text (attdef, &value_error);
  if (value_error == 0 && attdef_value == attdef->default_value)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading attdef_value");
    }

  // returns insertion point   
  dwg_ent_attdef_get_insertion_point (attdef, &ins_pt, &ins_pt_error);
  if (ins_pt_error == 0 && ins_pt.x == attdef->insertion_pt.x && ins_pt.y == attdef->insertion_pt.y)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading insertion");
    }

  // returns extrusion of attdef
  dwg_ent_attdef_get_extrusion (attdef, &ext, &ext_error);
  if (ext_error == 0 && ext.x == attdef->extrusion.x && ext.y == attdef->extrusion.y && ext.z == attdef->extrusion.z)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns height of attdef
  height = dwg_ent_attdef_get_height (attdef, &height_error);
  if (height_error == 0 && height == attdef->height)	// error checking
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading height");
    }

  // returns thickness of attdef  
  thickness = dwg_ent_attdef_get_thickness (attdef, &thickness_error);
  if (thickness_error == 0 && attdef->thickness == thickness)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading thickness");
    }

  // returns rotation of attdef
  rotation = dwg_ent_attdef_get_rot_angle (attdef, &rot_error);
  if (rot_error == 0 && attdef->rotation_ang == rotation)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading rotation");
    }

  // returns vertical alignment 
  vert_align = dwg_ent_attdef_get_vert_align (attdef, &vert_error);
  if (vert_error == 0 && attdef->vert_alignment == vert_align)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading vertical alignment");
    }

  // returns horizontal alignment
  horiz_align = dwg_ent_attdef_get_horiz_align (attdef, &horiz_error);
  if (horiz_error == 0 && horiz_align == attdef->horiz_alignment)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading horizontal alignment");
    }

}

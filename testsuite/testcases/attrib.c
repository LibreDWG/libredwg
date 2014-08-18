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

  if (dwg_get_type (obj) == DWG_TYPE_ATTRIB)
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
  char *attrib_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB (obj);

  // returns attrib value
  attrib_value = dwg_ent_attrib_get_text (attrib, &value_error);
  if (value_error == 0 && attrib_value == attrib->text_value)	// error reporting
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading attrib_value");
    }

  // returns insertion point  
  dwg_ent_attrib_get_insertion_point (attrib, &ins_pt, &ins_pt_error);
  if (ins_pt_error == 0 && ins_pt.x == attrib->insertion_pt.x && ins_pt.y == attrib->insertion_pt.y)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading insertion");
    }

  // returns extrusion
  dwg_ent_attrib_get_extrusion (attrib, &ext, &ext_error);
  if (ext_error == 0 && ext.x == attrib->extrusion.x && ext.y == attrib->extrusion.y && ext.z == attrib->extrusion.z)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading extrusion");
    }

  // returns height of attrib
  height = dwg_ent_attrib_get_height (attrib, &height_error);
  if (height_error == 0 && height == attrib->height)	// error report
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading height");
    }

  // Returns thickness of attrib  
  thickness = dwg_ent_attrib_get_thickness (attrib, &thickness_error);
  if (thickness_error == 0 && attrib->thickness == thickness)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading thickness");
    }

  // Returns rotation angle of attrib
  rotation = dwg_ent_attrib_get_rot_angle (attrib, &rot_error);
  if (rot_error == 0 && attrib->rotation_ang == rotation)	// error checks
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading rotation");
    }

  // returns vertical alignment
  vert_align = dwg_ent_attrib_get_vert_align (attrib, &vert_error);
  if (vert_error == 0 && vert_align == attrib->vert_alignment)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading vertical alignment");
    }

  // returns horizontal alignement
  horiz_align = dwg_ent_attrib_get_horiz_align (attrib, &horiz_error);
  if (horiz_error == 0 && attrib->horiz_alignment == horiz_align)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading horizontal alignment");
    }

}

#define DWG_TYPE DWG_TYPE_SHAPE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_shape *shape = dwg_object_to_SHAPE (obj);

  printf ("Thickness of shape : %f\n", shape->thickness);
  printf ("extrusion of shape : x = %f, y = %f, z = %f\n",
          shape->extrusion.x, shape->extrusion.y, shape->extrusion.z);
  printf ("ins_pt of shape : x = %f, y = %f, z = %f\n",
          shape->ins_pt.x, shape->ins_pt.y, shape->ins_pt.z);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BD thickness;
  dwg_point_3d ins_pt, ext;
  dwg_ent_shape *shape = dwg_object_to_SHAPE (obj);


  thickness = dwg_ent_shape_get_thickness (shape, &error);
  if (!error  && thickness == shape->thickness) // error check
    pass ("Working Properly");
  else
    fail ("error in reading thickness");


  dwg_ent_shape_get_extrusion (shape, &ext, &error);
  if (!error  && shape->extrusion.x == ext.x && shape->extrusion.y == ext.y && shape->extrusion.z == ext.z)     // error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  dwg_ent_shape_get_ins_pt (shape, &ins_pt, &error);
  if (!error  && shape->ins_pt.x == ins_pt.x && shape->ins_pt.y == ins_pt.y && shape->ins_pt.z == ins_pt.z)     // error check
    pass ("Working Properly");
  else
    fail ("error in reading ins_pt");

}

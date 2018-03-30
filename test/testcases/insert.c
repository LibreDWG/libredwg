#define DWG_TYPE DWG_TYPE_INSERT
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  // casts dwg object to insert entity
  dwg_ent_insert *insert = dwg_object_to_INSERT (obj);


  printf ("insert points : x = %f, y = %f, z = %f\n",
	  insert->ins_pt.x, insert->ins_pt.y, insert->ins_pt.z);
  printf ("scale flag for insert : " FORMAT_BS "\n", insert->scale_flag);
  printf ("scale points : x = %f, y = %f, z = %f\n",
	  insert->scale.x, insert->scale.y, insert->scale.z);
  printf ("angle for insert : %f\n", insert->rotation);
  printf ("extrusion points : x = %f, y = %f, z = %f\n",
	  insert->extrusion.x, insert->extrusion.y, insert->extrusion.z);
  printf ("attribs for insert : " FORMAT_BS "\n", insert->has_attribs);
  printf ("object count for insert : " FORMAT_BL "\n", insert->owned_obj_count);
}

void
api_process (dwg_object * obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  char scale_flag, attribs;
  unsigned long obj_count;
  dwg_ent_insert *insert = dwg_object_to_INSERT (obj);


  dwg_ent_insert_get_ins_pt (insert, &ins_pt, &error);
  if (!error  && ins_pt.x == insert->ins_pt.x && ins_pt.y == insert->ins_pt.y && ins_pt.z == insert->ins_pt.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading insertion point");


  scale_flag = dwg_ent_insert_get_scale_flag (insert, &error);
  if (!error  && scale_flag == insert->scale_flag)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading scale flag");


  dwg_ent_insert_get_scale (insert, &scale, &error);
  if (!error  && scale.x == insert->scale.x && scale.y == insert->scale.y && scale.z == insert->scale.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading scale");


  rot_angle = dwg_ent_insert_get_rotation (insert, &error);
  if (!error  && rot_angle == insert->rotation)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading rotation angle");


  dwg_ent_insert_get_extrusion (insert, &ext, &error);
  if (!error  && ext.x == insert->extrusion.x && ext.y == insert->extrusion.y && ext.z == insert->extrusion.z)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading extrusion");


  attribs = dwg_ent_insert_get_has_attribs (insert, &error);
  if (!error  && attribs == insert->has_attribs)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading attribs");


  obj_count = dwg_ent_insert_get_owned_obj_count (insert, &error);
  if (!error  && obj_count == insert->owned_obj_count)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading object counts");
}

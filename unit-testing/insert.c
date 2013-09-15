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

  if (dwg_get_type(obj)== DWG_TYPE_INSERT)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casts dwg object to insert entity
  dwg_ent_insert *insert = dwg_object_to_INSERT(obj);

  // prints insertion points
  printf("insert points : x = %f, y = %f, z = %f\t\n",
	  insert->ins_pt.x, insert->ins_pt.y, insert->ins_pt.z);

  // prints scale flag
  printf("scale flag for insert : %d\t\n", insert->scale_flag);

  // prints scale points
  printf("scale points : x = %f, y = %f, z = %f\t\n",
	  insert->scale.x, insert->scale.y, insert->scale.z);

  // prints angle
  printf("angle for insert : %f\t\n", insert->rotation_ang);

  // prints extrusion points
  printf("extrusion points : x = %f, y = %f, z = %f\t\n",
	  insert->extrusion.x, insert->extrusion.y, insert->extrusion.z);

  // prints attrib
  printf("attribs for insert : %d\t\n", insert->has_attribs);

  // prints owned object count
  printf("object count for insert : %ld\t\n", insert->owned_obj_count);
}

void
api_process(dwg_object *obj)
{
  int ins_pt_error, ext_error, scale_error, ang_error, flag_error, attr_error,
  count_error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  char scale_flag, attribs;
  long obj_count;
  dwg_ent_insert *insert = dwg_object_to_INSERT(obj);

  // returns insertion point
  dwg_ent_insert_get_ins_pt(insert, &ins_pt, &ins_pt_error);
  if(ins_pt_error == 0 ) // error check
    {
      printf("insert points : x = %f, y = %f, z = %f\t\n",
	     ins_pt.x, ins_pt.y, ins_pt.z);
    }
  else
    {
      printf("error in reading insertion point\n");
    }

  // returns scale flag
  scale_flag = dwg_ent_insert_get_scale_flag(insert, &flag_error); 
  if(flag_error == 0) // error check
    {
      printf("scale flag for insert : %d\t\n", scale_flag);
    }
  else
    {
      printf("error in reading scale flag\n");
    } 

  // returns scale points
  dwg_ent_insert_get_scale(insert, &scale, &scale_error); 
  if(scale_error == 0) // error check
    {
      printf("scale points : x = %f, y = %f, z = %f\t\n",
	     scale.x, scale.y, scale.z);
    }
  else
    {
      printf("error in reading scale \n");
    } 

  // returns rotation angle
  rot_angle = dwg_ent_insert_get_rotation_angle(insert, &ang_error); 
  if(ang_error == 0) // error check
    {
      printf("angle for insert : %f\t\n", rot_angle);
    }
  else
    {
      printf("error in reading rotation angle\n");
    } 

  // returns extrusion points
  dwg_ent_insert_get_extrusion(insert, &ext, &ext_error); 
  if(ext_error == 0) // error check
    {
      printf("extrusion points : x = %f, y = %f, z = %f\t\n",
	     ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    } 

  // returns attriute
  attribs = dwg_ent_insert_get_has_attribs(insert, &attr_error); 
  if(attr_error == 0) // error check
    {
      printf("attribs for insert : %d\t\n", attribs);
    }
  else
    {
      printf("error in reading attribs\n");
    } 

  // returns owned object count
  obj_count = dwg_ent_insert_get_owned_obj_count(insert, &count_error); 
  if(count_error == 0) // error check
    {
      printf("object count for insert : %ld\t\n", obj_count);
    }
  else
    {
      printf("error in reading object counts\n");
    } 
} 

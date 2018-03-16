#define DWG_TYPE DWG_TYPE_INSERT
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  // casts dwg object to insert entity
  dwg_ent_insert *insert = dwg_object_to_INSERT(obj);

  // prints insertion points
  printf("insert points : x = %f, y = %f, z = %f\n",
	  insert->ins_pt.x, insert->ins_pt.y, insert->ins_pt.z);

  // prints scale flag
  printf("scale flag for insert : " FORMAT_BS "\n", insert->scale_flag);

  // prints scale points
  printf("scale points : x = %f, y = %f, z = %f\n",
	  insert->scale.x, insert->scale.y, insert->scale.z);

  // prints angle
  printf("angle for insert : %f\n", insert->rotation_ang);

  // prints extrusion points
  printf("extrusion points : x = %f, y = %f, z = %f\n",
	  insert->extrusion.x, insert->extrusion.y, insert->extrusion.z);

  // prints attrib
  printf("attribs for insert : " FORMAT_BS "\n", insert->has_attribs);

  // prints owned object count
  printf("object count for insert : " FORMAT_BL "\n", insert->owned_obj_count);
}

void
api_process(dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  char scale_flag, attribs;
  BITCODE_BL obj_count;
  dwg_ent_insert *insert = dwg_object_to_INSERT(obj);

  // returns insertion point
  dwg_ent_insert_get_ins_pt(insert, &ins_pt,
                            &error);
  if ( !error )
    {
      printf("insert points : x = %f, y = %f, z = %f\n",
	     ins_pt.x, ins_pt.y, ins_pt.z);
    }
  else
    {
      printf("error in reading insertion point\n");
    }

  // returns scale flag
  scale_flag = dwg_ent_insert_get_scale_flag(insert, &error); 
  if ( !error )
    {
      printf("scale flag for insert : " FORMAT_BS "\n", scale_flag);
    }
  else
    {
      printf("error in reading scale flag\n");
    } 

  // returns scale points
  dwg_ent_insert_get_scale(insert, &scale,
                           &error); 
  if ( !error )
    {
      printf("scale points : x = %f, y = %f, z = %f\n",
	     scale.x, scale.y, scale.z);
    }
  else
    {
      printf("error in reading scale \n");
    } 

  // returns rotation angle
  rot_angle = dwg_ent_insert_get_rotation_angle(insert, &error); 
  if ( !error )
    {
      printf("angle for insert : %f\n", rot_angle);
    }
  else
    {
      printf("error in reading rotation angle\n");
    } 

  // returns extrusion points
  dwg_ent_insert_get_extrusion(insert, &ext,
                               &error); 
  if ( !error )
    {
      printf("extrusion points : x = %f, y = %f, z = %f\n",
	     ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    } 

  // returns attriute
  attribs = dwg_ent_insert_get_has_attribs(insert, &error); 
  if ( !error )
    {
      printf("attribs for insert : " FORMAT_BS "\n", attribs);
    }
  else
    {
      printf("error in reading attribs\n");
    } 

  // returns owned object count
  obj_count = dwg_ent_insert_get_owned_obj_count(insert, &error); 
  if ( !error )
    {
      printf("object count for insert : " FORMAT_BL "\n", obj_count);
    }
  else
    {
      printf("error in reading object counts\n");
    } 
} 

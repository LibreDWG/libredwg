#define DWG_TYPE DWG_TYPE_INSERT
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_insert *insert = dwg_object_to_INSERT(obj);

  printf("insert points : x = %f, y = %f, z = %f\n",
	  insert->ins_pt.x, insert->ins_pt.y, insert->ins_pt.z);
  printf("scale flag for insert : " FORMAT_BB "\n", insert->scale_flag);
  printf("scale points : x = %f, y = %f, z = %f\n",
	  insert->scale.x, insert->scale.y, insert->scale.z);
  printf("angle for insert : %f\n", insert->rotation);
  printf("extrusion points : x = %f, y = %f, z = %f\n",
	  insert->extrusion.x, insert->extrusion.y, insert->extrusion.z);
  printf("attribs for insert : " FORMAT_B "\n", insert->has_attribs);
  printf("object count for insert : " FORMAT_BL "\n", insert->owned_obj_count);
}

void
api_process(dwg_object *obj)
{
  int error;
  dwg_point_3d ins_pt, ext, scale;
  double rot_angle;
  char scale_flag, has_attribs;
  BITCODE_BL obj_count;
  dwg_ent_insert *insert = dwg_object_to_INSERT(obj);

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


  scale_flag = dwg_ent_insert_get_scale_flag(insert, &error); 
  if ( !error )
    printf("scale flag for insert : " FORMAT_BB "\n", scale_flag);
  else
    printf("in reading scale flag\n"); 


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


  rot_angle = dwg_ent_insert_get_rotation(insert, &error); 
  if ( !error )
    printf("angle for insert : %f\n", rot_angle);
  else
    printf("in reading rotation angle\n"); 


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


  has_attribs = dwg_ent_insert_has_attribs(insert, &error); 
  if ( !error )
    printf("attribs for insert : " FORMAT_B "\n", has_attribs);
  else
    printf("in reading attribs\n"); 


  obj_count = dwg_ent_insert_get_owned_obj_count(insert, &error); 
  if ( !error )
    printf("object count for insert : " FORMAT_BL "\n", obj_count);
  else
    printf("in reading object counts\n"); 
} 

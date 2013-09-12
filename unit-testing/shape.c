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

  if (dwg_get_type(obj)== DWG_TYPE_SHAPE)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_shape *shape = dwg_object_to_SHAPE(obj);
 printf("Thickness of shape : %f\t\n",shape->thickness);
  printf("extrusion of shape : x = %f, y = %f, z = %f\t\n", 
          shape->extrusion.x, shape->extrusion.y, shape->extrusion.z);
  printf("ins_pt of shape : x = %f,y = %f,z = %f\t\n",
          shape->ins_pt.x, shape->ins_pt.y, shape->ins_pt.z);
}

void
api_process(dwg_object *obj)
{
  int scale_error, rot_error, width_error, oblique_error, thickness_error,
      shape_no_error, ins_pt_error, ext_error;
  float scale, rotation, width_factor, oblique, thickness, shape_no;
  dwg_point_3d ins_pt, ext;
  dwg_ent_shape *shape = dwg_object_to_SHAPE(obj);

  thickness = dwg_ent_shape_get_thickness(shape, &thickness_error);
  if(thickness_error == 0 )
    {
      printf("Thickness of shape : %f\t\n",thickness);
    }
  else
    {
      printf("error in reading thickness \n");
    }

  dwg_ent_shape_get_extrusion(shape, &ext,&ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of shape : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  dwg_ent_shape_get_ins_pt(shape, &ins_pt,&ins_pt_error);
  if(ins_pt_error == 0 )
    {
      printf("ins_pt of shape : x = %f, y = %f, z = %f\t\n",
              ins_pt.x, ins_pt.y, ins_pt.z);
    }
  else
    {
      printf("error in reading ins_pt \n");
    }
  
 
}

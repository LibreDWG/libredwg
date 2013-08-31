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

  if (dwg_get_type(obj)== DWG_TYPE_ATTRIB)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB(obj);
  printf("attrib of attrib : %s\t\n", attrib->text_value);
  printf("insertion point of attrib : x = %f, y = %f\t\n", 
          attrib->insertion_pt.x, attrib->insertion_pt.y);
  printf("extrusion of attrib : x = %f, y = %f, z = %f\t\n",
          attrib->extrusion.x, attrib->extrusion.y, attrib->extrusion.z);
  printf("height of attrib : %f\t\n", attrib->height);
  printf("thickness of attrib : %f\t\n", attrib->thickness);
  printf("rotation of attrib : %f\t\n", attrib->rotation_ang);
  printf("vertical align of attrib : %du\t\n", attrib->vert_alignment);
  printf("horizontal align of attrib : %du\t\n", attrib->horiz_alignment);
}

void
api_process(dwg_object *obj)
{
  int value_error, ins_pt_error, ext_error, thickness_error, rot_error, 
    vert_error, horiz_error, height_error;
  float thickness, rotation, vert_align, horiz_align, height;
  char * attrib_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB(obj);

  attrib_value = dwg_ent_attrib_get_text(attrib, &value_error);
  if( value_error== 0 )
    {  
      printf("attrib value : %s\t\n",attrib_value);
    }
  else
    {
      printf("error in reading attrib_value \n");
    }
  
  dwg_ent_attrib_get_insertion_point(attrib, &ins_pt, &ins_pt_error);
  if(ins_pt_error == 0 )
    {
      printf("insertion point of attrib : x = %f, y = %f\t\n",
              ins_pt.x, ins_pt.y);
    }
  else
    {
      printf("error in reading insertion \n");
    }

  dwg_ent_attrib_get_extrusion(attrib, &ext, &ext_error);
  if(ext_error == 0 )
    {
      printf("extrusion of attrib : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  height = dwg_ent_attrib_get_height(attrib, &height_error);
  if(height_error == 0 )
    {
      printf("height of attrib : %f\t\n", height);
    }
  else
    {
      printf("error in reading height \n");
    }
  
  thickness = dwg_ent_attrib_get_thickness(attrib, &thickness_error);
  if(thickness_error == 0 )
    {
      printf("thickness of attrib : %f\t\n", thickness);
    }
  else
    {
      printf("error in reading thickness\n");
    }

  rotation = dwg_ent_attrib_get_rot_angle(attrib, &rot_error);
  if(rot_error == 0 )
    {
      printf("rotation of attrib : %f\t\n", rotation);
    }
  else
    {
      printf("error in reading rotation \n");
    }

  vert_align = dwg_ent_attrib_get_vert_align(attrib, &vert_error);
  if(vert_error == 0)
    {
      printf("Vertical alignment of attrib : %f\t\n", vert_align);
    }
  else
    {
      printf("error in reading vertical alignment");
    }

  horiz_align = dwg_ent_attrib_get_horiz_align(attrib, &horiz_error);
  if(horiz_error == 0)
    {
      printf("Horizontal alignment of attrib : %f\t\n", horiz_align);
    }
  else
    {
      printf("error in reading horizontal alignment");
    }

}

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

  if (dwg_get_type(obj)== DWG_TYPE_ATTDEF)
    {
      output_process(obj);
    }
}

void
low_level_process(dwg_object *obj)
{
  // casts a dwg object to attdef
  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF(obj);

  // prints attdef of attdef
  printf("attdef of attdef : %s\t\n", attdef->default_value);

  // prints insertion point of attdef
  printf("insertion point of attdef : x = %f, y = %f\t\n", 
          attdef->insertion_pt.x, attdef->insertion_pt.y);

  // prints extrusion of attdef
  printf("extrusion of attdef : x = %f, y = %f, z = %f\t\n",
          attdef->extrusion.x, attdef->extrusion.y, attdef->extrusion.z);
  
  // prints height of attdef
  printf("height of attdef : %f\t\n", attdef->height);

  // prints thickness of attdef
  printf("thickness of attdef : %f\t\n", attdef->thickness);

  // prints rotation of attef
  printf("rotation of attdef : %f\t\n", attdef->rotation_ang);

  // prints vertical alignment
  printf("vertical align of attdef : %du\t\n", attdef->vert_alignment);

  // prints horizontal align
  printf("horizontal align of attdef : %du\t\n", attdef->horiz_alignment);
}

void
api_process(dwg_object *obj)
{
  int value_error, ins_pt_error, ext_error, thickness_error, rot_error, 
    vert_error, horiz_error, height_error;
  float thickness, rotation, vert_align, horiz_align, height;
  char * attdef_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF(obj);

  // return attdef value
  attdef_value = dwg_ent_attdef_get_text(attdef, &value_error);
  if( value_error== 0 ) // error checking
    {  
      printf("attdef value : %s\t\n",attdef_value);
    }
  else
    {
      printf("error in reading attdef_value \n");
    }

  // returns insertion point   
  dwg_ent_attdef_get_insertion_point(attdef, &ins_pt, &ins_pt_error);
  if(ins_pt_error == 0 ) // error checking
    {
      printf("insertion point of attdef : x = %f, y = %f\t\n",
              ins_pt.x, ins_pt.y);
    }
  else
    {
      printf("error in reading insertion \n");
    }

  // returns extrusion of attdef
  dwg_ent_attdef_get_extrusion(attdef, &ext, &ext_error);
  if(ext_error == 0 ) // error checking
    {
      printf("extrusion of attdef : x = %f, y = %f, z = %f\t\n",
              ext.x, ext.y, ext.z);
    }
  else
    {
      printf("error in reading extrusion \n");
    }

  // returns height of attdef
  height = dwg_ent_attdef_get_height(attdef, &height_error);
  if(height_error == 0 ) // error checking
    {
      printf("height of attdef : %f\t\n", height);
    }
  else
    {
      printf("error in reading height \n");
    }

  // returns thickness of attdef  
  thickness = dwg_ent_attdef_get_thickness(attdef, &thickness_error);
  if(thickness_error == 0 ) // error check
    {
      printf("thickness of attdef : %f\t\n", thickness);
    }
  else
    {
      printf("error in reading thickness\n");
    }

  // returns rotation of attdef
  rotation = dwg_ent_attdef_get_rot_angle(attdef, &rot_error);
  if(rot_error == 0 )
    {
      printf("rotation of attdef : %f\t\n", rotation);
    }
  else
    {
      printf("error in reading rotation \n");
    }

  // returns vertical alignment 
  vert_align = dwg_ent_attdef_get_vert_align(attdef, &vert_error);
  if(vert_error == 0)
    {
      printf("Vertical alignment of attdef : %f\t\n", vert_align);
    }
  else
    {
      printf("error in reading vertical alignment");
    }

  // returns horizontal alignment
  horiz_align = dwg_ent_attdef_get_horiz_align(attdef, &horiz_error);
  if(horiz_error == 0)
    {
      printf("Horizontal alignment of attdef : %f\t\n", horiz_align);
    }
  else
    {
      printf("error in reading horizontal alignment");
    }

}

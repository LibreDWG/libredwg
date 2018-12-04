#define DWG_TYPE DWG_TYPE_ATTDEF
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF(obj);

  printf("attdef of attdef : %s\n", attdef->default_value);
  printf("insertion point of attdef : x = %f, y = %f\n",
          attdef->insertion_pt.x, attdef->insertion_pt.y);
  printf("extrusion of attdef : x = %f, y = %f, z = %f\n",
          attdef->extrusion.x, attdef->extrusion.y, attdef->extrusion.z);
  printf("height of attdef : %f\n", attdef->height);
  printf("thickness of attdef : %f\n", attdef->thickness);
  printf("rotation of attdef : %f\n", attdef->rotation);
  printf("vertical align of attdef : " FORMAT_BS "\n", attdef->vert_alignment);
  printf("horizontal align of attdef : " FORMAT_BS "\n", attdef->horiz_alignment);
}

void
api_process(dwg_object *obj)
{
  int error;
  double thickness, rotation, height;
  BITCODE_BS vert_align, horiz_align;
  char * attdef_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;
  dwg_ent_attdef *attdef = dwg_object_to_ATTDEF(obj);

  attdef_value = dwg_ent_attdef_get_default_value(attdef, &error);
  if (!error)
      printf("attdef value : \"%s\"\n", attdef_value);
  else
      printf("error in reading attdef_value \n");

  dwg_ent_attdef_get_insertion_point(attdef, &ins_pt, &error);
  if (!error)
      printf("insertion point of attdef : x = %f, y = %f\n",
              ins_pt.x, ins_pt.y);
  else
      printf("error in reading insertion \n");

  dwg_ent_attdef_get_extrusion(attdef, &ext, &error);
  if (!error)
    printf("extrusion of attdef : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");

  height = dwg_ent_attdef_get_height(attdef, &error);
  if ( !error )
    printf("height of attdef : %f\n", height);
  else
    printf("error in reading height \n");

  thickness = dwg_ent_attdef_get_thickness(attdef, &error);
  if (!error)
    printf("thickness of attdef : %f\n", thickness);
  else
    printf("error in reading thickness\n");

  rotation = dwg_ent_attdef_get_rotation(attdef, &error);
  if (!error)
    printf("rotation of attdef : %f\n", rotation);
  else
    printf("error in reading rotation \n");

  vert_align = dwg_ent_attdef_get_vert_alignment(attdef, &error);
  if (!error)
    printf("vertical alignment of attdef : " FORMAT_BS "\n", vert_align);
  else
    printf("error in reading vertical alignment");

  horiz_align = dwg_ent_attdef_get_horiz_alignment(attdef, &error);
  if (!error)
      printf("horizontal alignment of attdef : " FORMAT_BS "\n", horiz_align);
  else
      printf("error in reading horizontal alignment");
}

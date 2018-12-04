#define DWG_TYPE DWG_TYPE_ATTRIB
#include "common.c"

void
low_level_process(dwg_object *obj)
{
  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB(obj);

  printf("attrib of attrib : %s\n", attrib->text_value);
  printf("insertion point of attrib : x = %f, y = %f\n",
          attrib->insertion_pt.x, attrib->insertion_pt.y);
  printf("extrusion of attrib : x = %f, y = %f, z = %f\n",
          attrib->extrusion.x, attrib->extrusion.y, attrib->extrusion.z);
  printf("height of attrib : %f\n", attrib->height);
  printf("thickness of attrib : %f\n", attrib->thickness);
  printf("rotation of attrib : %f\n", attrib->rotation);
  printf("vertical align of attrib : " FORMAT_BS "\n", attrib->vert_alignment);
  printf("horizontal align of attrib : " FORMAT_BS "\n", attrib->horiz_alignment);
}

void
api_process(dwg_object *obj)
{
  int error;
  double thickness, rotation, height;
  BITCODE_BS vert_align, horiz_align;
  char * attrib_value;
  dwg_point_3d ext;
  dwg_point_2d ins_pt;

  dwg_ent_attrib *attrib = dwg_object_to_ATTRIB(obj);

  attrib_value = dwg_ent_attrib_get_text(attrib, &error);
  if (!error)
    printf("attrib value : \"%s\"\n", attrib_value);
  else
    printf("error in reading attrib_value \n");

  dwg_ent_attrib_get_insertion_point(attrib, &ins_pt, &error);
  if (!error)
    printf("insertion point of attrib : x = %f, y = %f\n",
           ins_pt.x, ins_pt.y);
  else
    printf("error in reading insertion \n");


  dwg_ent_attrib_get_extrusion(attrib, &ext, &error);
  if (!error)
    printf("extrusion of attrib : x = %f, y = %f, z = %f\n",
           ext.x, ext.y, ext.z);
  else
    printf("error in reading extrusion \n");

  height = dwg_ent_attrib_get_height(attrib, &error);
  if (!error)
    printf("height of attrib : %f\n", height);
  else
    printf("error in reading height \n");

  thickness = dwg_ent_attrib_get_thickness(attrib, &error);
  if (!error)
    printf("thickness of attrib : %f\n", thickness);
  else
    printf("error in reading thickness\n");

  rotation = dwg_ent_attrib_get_rotation(attrib, &error);
  if (!error)
    printf("rotation of attrib : %f\n", rotation);
  else
    printf("error in reading rotation \n");

  vert_align = dwg_ent_attrib_get_vert_alignment(attrib, &error);
  if (!error)
    printf("Vertical alignment of attrib : " FORMAT_BS "\n", vert_align);
  else
    printf("error in reading vertical alignment");

  horiz_align = dwg_ent_attrib_get_horiz_alignment(attrib, &error);
  if (!error)
    printf("Horizontal alignment of attrib : " FORMAT_BS "\n", horiz_align);
  else
    printf("error in reading horizontal alignment");
}

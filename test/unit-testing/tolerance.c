#define DWG_TYPE DWG_TYPE_TOLERANCE
#include "common.c"

void
low_level_process (dwg_object *obj)
{
  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE (obj);

  printf ("Radius of tolerance : %f\n", tolerance->height);
  printf ("Thickness of tolerance : %f\n", tolerance->dimgap);
  printf ("text string of tolerance : %s\n", tolerance->text_string);
  printf ("extrusion of tolerance : x = %f, y = %f, z = %f\n",
          tolerance->extrusion.x, tolerance->extrusion.y,
          tolerance->extrusion.z);
  printf ("ins_pt of tolerance : x = %f, y = %f, z = %f\n",
          tolerance->ins_pt.x, tolerance->ins_pt.y, tolerance->ins_pt.z);
  printf ("center of tolerance : x = %f,y = %f,z = %f\n",
          tolerance->x_direction.x, tolerance->x_direction.y,
          tolerance->x_direction.z);
}

void
api_process (dwg_object *obj)
{
  int error;
  double height, dimgap;
  dwg_point_3d ins_pt, x_dir, ext; // 3d_points
  char *text_string;

  dwg_ent_tolerance *tolerance = dwg_object_to_TOLERANCE (obj);

  height = dwg_ent_tolerance_get_height (tolerance, &error);
  if (!error)
    printf ("height of tolerance : %f\n", height);
  else
    printf ("in reading height \n");

  dimgap = dwg_ent_tolerance_get_dimgap (tolerance, &error);
  if (!error)
    printf ("dimgap of tolerance : %f\n", dimgap);
  else
    printf ("in reading dimgao \n");

  text_string = dwg_ent_tolerance_get_text_string (tolerance, &error);
  if (!error)
    printf ("text of tolerance : %s\n", text_string);
  else
    printf ("in reading text \n");

  dwg_ent_tolerance_get_ins_pt (tolerance, &ins_pt, &error);
  if (!error)
    {
      printf ("ins pt of tolerance : x = %f, y = %f, z = %f\n", ins_pt.x,
              ins_pt.y, ins_pt.z);
    }
  else
    {
      printf ("error in reading extrusion \n");
    }

  // return tolerance center points
  dwg_ent_tolerance_get_x_direction (tolerance, &x_dir, &error);
  if (!error)
    {
      printf ("x direction of tolerance : x = %f, y = %f, z = %f\n", x_dir.x,
              x_dir.y, x_dir.z);
    }
  else
    {
      printf ("error in reading x direction \n");
    }
}

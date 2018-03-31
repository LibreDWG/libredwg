#define DWG_TYPE DWG_TYPE_DIMENSION_RADIUS
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_dim_radius *dim = dwg_object_to_DIMENSION_RADIUS (obj);

  printf ("horiz dir of dim_radius : %f\n", dim->horiz_dir);
  printf ("lspace factor of dim_radius : %f\n", dim->lspace_factor);
  printf ("lspace style of dim_radius : " FORMAT_BS "\n", dim->lspace_style);
  printf ("attachment of dim_radius : " FORMAT_BS "\n", dim->attachment);
  printf ("Radius of dim_radius : %f\n", dim->elevation.ecs_11);
  printf ("Thickness of dim_radius : %f\n", dim->elevation.ecs_12);
  printf ("extrusion of dim_radius : x = %f, y = %f, z = %f\n",
	  dim->extrusion.x, dim->extrusion.y, dim->extrusion.z);
  printf ("ins_scale of dim_radius : x = %f, y = %f, z = %f\n",
	  dim->ins_scale.x, dim->ins_scale.y, dim->ins_scale.z);
  printf ("text_mid_pt of dim_radius : x = %f, y = %f\n",
	  dim->text_midpt.x, dim->text_midpt.y);
  printf ("user text of dim_radius : %s\n", dim->user_text);
  printf ("text rotation of dim_radius : %f\n", dim->text_rot);
  printf ("ins rotation of dim_radius : %f\n", dim->ins_rotation);
  printf ("arrow 1 of dim_radius : " FORMAT_BS "\n", dim->flip_arrow1);
  printf ("arrow 2 of dim_radius : " FORMAT_BS "\n", dim->flip_arrow2);
  printf ("flags2 of dim_radius : " FORMAT_BS "\n", dim->flags_1);
  printf ("act_measurement of dim_radius : %f\n", dim->act_measurement);
  printf ("leader length of dim radius : %f\n", dim->leader_len);
}

void
api_process (dwg_object * obj)
{
  int error;
  double length;
  BITCODE_RC flags2;
  dwg_ent_dim_radius *dim_radius = dwg_object_to_DIMENSION_RADIUS (obj);

  length = dwg_ent_dim_radius_get_leader_length (dim_radius, &error);
  if (!error  && length == dim_radius->leader_len)
    pass ("Working Properly");
  else
    fail ("error in reading leader length");
}

#define DWG_TYPE DWG_TYPE_DIMENSION_ORDINATE
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  dwg_ent_dim_ordinate *dim = dwg_object_to_DIMENSION_ORDINATE (obj);

  printf ("horiz dir of dim_ordinate : %f\n", dim->horiz_dir);
  printf ("lspace factor of dim_ordinate : %f\n", dim->lspace_factor);
  printf ("lspace style of dim_ordinate : " FORMAT_BS "\n", dim->lspace_style);
  printf ("attachment of dim_ordinate : " FORMAT_BS "\n", dim->attachment);
  printf ("Radius of dim_ordinate : %f\n", dim->elevation.ecs_11);
  printf ("Thickness of dim_ordinate : %f\n", dim->elevation.ecs_12);
  printf ("extrusion of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim->extrusion.x, dim->extrusion.y, dim->extrusion.z);
  printf ("ins_scale of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim->ins_scale.x, dim->ins_scale.y, dim->ins_scale.z);
  printf ("ucsorigin_pt of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim->ucsorigin_pt.x, dim->ucsorigin_pt.y, dim->ucsorigin_pt.z);
  printf ("feature_location_pt of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim->feature_location_pt.x, dim->feature_location_pt.y,
	  dim->feature_location_pt.z);
  printf ("leader_endpt of dim_ordinate : x = %f, y = %f, z = %f\n",
	  dim->leader_endpt.x, dim->leader_endpt.y, dim->leader_endpt.z);
  printf ("clone_ins_pt of dim_ordinate : x = %f, y = %f\n",
	  dim->clone_ins_pt.x, dim->clone_ins_pt.y);
  printf ("text_mid_pt of dim_ordinate : x = %f, y = %f\n",
	  dim->text_midpt.x, dim->text_midpt.y);
  printf ("user text of dim_ordinate : %s\n", dim->user_text);
  printf ("text rotation of dim_ordinate : %f\n", dim->text_rot);
  printf ("ins rotation of dim_ordinate : %f\n", dim->ins_rotation);
  printf ("arrow1 of dim_ordinate : " FORMAT_BS "\n", dim->flip_arrow1);
  printf ("arrow2 of dim_ordinate : " FORMAT_BS "\n", dim->flip_arrow2);
  printf ("flags1 of dim_ordinate : " FORMAT_BS "\n", dim->flags_1);
  printf ("flags2 of dim_ordinate : " FORMAT_BS "\n", dim->flags_2);
  printf ("act_measurement of dim_ordinate : %f\n", dim->act_measurement);

}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_RC flags2;
  dwg_point_3d pt10, pt13, pt14;
  dwg_ent_dim_ordinate *dim_ordinate = dwg_object_to_DIMENSION_ORDINATE (obj);
  dwg_ent_dim *dim = dwg_object_to_DIMENSION(obj);

  dwg_ent_dim_ordinate_get_ucsorigin_pt (dim_ordinate, &pt10, &error);
  if (!error  && pt10.x == dim_ordinate->ucsorigin_pt.x && pt10.y == dim_ordinate->ucsorigin_pt.y && pt10.z == dim_ordinate->ucsorigin_pt.z)
    pass ("Working Properly");
  else
    fail ("error in reading ucsorigin_pt");

  dwg_ent_dim_ordinate_get_feature_location_pt (dim_ordinate, &pt13, &error);
  if (!error  && pt13.x == dim_ordinate->feature_location_pt.x && pt13.y == dim_ordinate->feature_location_pt.y && pt13.z == dim_ordinate->feature_location_pt.z)
    pass ("Working Properly");
  else
    fail ("error in reading feature_location_pt");

  dwg_ent_dim_ordinate_get_leader_endpt (dim_ordinate, &pt14, &error);
  if (!error  && dim_ordinate->leader_endpt.x == pt14.x && dim_ordinate->leader_endpt.y == pt14.y && dim_ordinate->leader_endpt.z == pt14.z)
    pass ("Working Properly");
  else
    fail ("error in reading leader_endpt");

}

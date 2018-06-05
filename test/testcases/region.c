#define DWG_TYPE DWG_TYPE_REGION
#include "common.c"
#include <dejagnu.h>

void
low_level_process (dwg_object * obj)
{
  // casts dwg object to region entity
  dwg_ent_region *region = dwg_object_to_REGION (obj);
  BITCODE_BL i;

  printf ("acis empty of region : " FORMAT_B "\n", region->acis_empty);
  printf ("version of region : " FORMAT_BS "\n", region->version);
  printf ("acis data of region : %s\n", region->acis_data);
  printf ("wireframe data of region : " FORMAT_B "\n", region->wireframe_data_present);
  printf ("point present of region : " FORMAT_B "\n", region->point_present);
  printf ("point of region : x = %f, y = %f, z = %f\n", region->point.x,
	  region->point.y, region->point.z);
  printf ("num isolines of region : " FORMAT_BL "\n", region->num_isolines);
  printf ("isoline present of region : " FORMAT_B "\n", region->isoline_present);
  printf ("num wires of region : " FORMAT_BL "\n", region->num_wires);

  for (i = 0; i < region->num_wires; i++)
    printf ("wire[%d] of region : " FORMAT_BL "\n", i, region->wires[i].selection_marker);

  printf ("num sil of region : " FORMAT_BL "\n", region->num_silhouettes);

  for (i = 0; i < region->num_silhouettes; i++)
    printf ("silhouette[%d] of region : " FORMAT_BL "\n", i, region->silhouettes[i].vp_id);
}

void
api_process (dwg_object * obj)
{
  int error;
  BITCODE_BS version;
  BITCODE_BL *block_size, num_isolines, num_wires, num_sil;
  unsigned char *acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present; 
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point;
  dwg_ent_region *region = dwg_object_to_REGION (obj);
  dwg_3dsolid_wire *wire;
  dwg_3dsolid_silhouette *sil;
  BITCODE_BL i;


  acis_empty = dwg_ent_region_get_acis_empty (region, &error);
  if (!error  && acis_empty == region->acis_empty)	// error check
    pass ("Working Properly");
  else
    fail ("error in reading acis empty");

  version = dwg_ent_region_get_version (region, &error);
  if (!error  && version == region->version)
    pass ("Working Properly");
  else
    fail ("error in reading version");

  acis_data = dwg_ent_region_get_acis_data (region, &error);
  if (!error  && acis_data == region->acis_data)
    pass ("Working Properly");
  else
    fail ("error in reading acis data");

  wireframe_data_present = dwg_ent_region_get_wireframe_data_present (region, &error);
  if (!error  && wireframe_data_present == region->wireframe_data_present)
    pass ("Working Properly");
  else
    fail ("error in reading wireframe data present");

  point_present =
    dwg_ent_region_get_point_present (region, &error);
  if (!error  && point_present == region->point_present)
    pass ("Working Properly");
  else
    fail ("error in reading point present");

  dwg_ent_region_get_point (region, &point, &error);
  if (!error  && point.x == region->point.x
      && point.y == region->point.y && point.z == region->point.z)
    pass ("Working Properly");
  else
    fail ("error in reading point");

  num_isolines = dwg_ent_region_get_num_isolines (region, &error);
  if (!error && region->num_isolines == num_isolines)
    pass ("Working Properly");
  else
    fail ("error in reading num isolines");

  isoline_present = dwg_ent_region_get_isoline_present (region, &error);
  if (!error && isoline_present == region->isoline_present)
    pass ("Working Properly");
  else
    fail ("error in reading isoline present");

  num_wires = dwg_ent_region_get_num_wires (region, &error);
  if (!error )
    pass ("Working Properly");
  else
    fail ("error in reading num wires");

  wire = dwg_ent_region_get_wires (region, &error);
  if (!error )
    {
      int matches = 1;
      for (i = 0; i < num_wires; i++)
	{
	  if (wire[i].selection_marker != region->wires[i].selection_marker)
	    {
	      matches = 0;
	    }
	}
      if (matches)
	  pass ("Working Properly");
      else
	  fail ("no match found");
    }
  else
    {
      fail ("error in reading num wires");
    }

  num_sil = dwg_ent_region_get_num_silhouettes (region, &error);
  if (!error  && region->num_silhouettes == num_sil)
    pass ("Working Properly");
  else
    fail ("error in reading num silhouettes");

  sil = dwg_ent_region_get_silhouettes (region, &error);
  if (!error )
    {
      int matches = 1;
      for (i = 0; i < num_sil; i++)
	{
	  if (region->silhouettes[i].vp_id != sil[i].vp_id)
	    {
	      matches = 0;
	    }
	}
      if (matches)
	{
	  pass ("Working Properly");
	}
      else
	{
	  fail ("error in reading silhouettes");
	}
    }
  else
    {
      fail ("error in reading silhouettes");
    }

}

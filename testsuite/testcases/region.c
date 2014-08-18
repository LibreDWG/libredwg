#include "common.c"
#include <dejagnu.h>

void output_process (dwg_object * obj);

void
output_object (dwg_object * obj)
{
  if (!obj)
    {
      printf ("object is NULL\n");
      return;
    }

  if (dwg_get_type (obj) == DWG_TYPE_REGION)
    {
      output_process (obj);
    }
}

void
low_level_process (dwg_object * obj)
{
  // casts dwg object to region entity
  dwg_ent_region *region = dwg_object_to_REGION (obj);

  // prints acis empty
  printf ("acis empty of region : %d", region->acis_empty);

  // prints version
  printf ("version of region : %d", region->version);

  // prints acis data
  printf ("acis data of region : %s", region->acis_data);

  // prints wireframe data present 
  printf ("wireframe data of region : %d", region->wireframe_data_present);

  // prints point present
  printf ("point present of region : %d", region->point_present);

  // prints point values
  printf ("point of region : x = %f, y = %f, z = %f\t\n", region->point.x,
	  region->point.y, region->point.z);

  // prints num isolines
  printf ("num isolines of region : %ld", region->num_isolines);

  // prints isoline present
  printf ("isoline present of region : %d", region->isoline_present);

  // prints num wires
  printf ("num wires of region : %ld", region->num_wires);

  // prints wires
  int i;
  for (i = 0; i < region->num_wires; i++)
    {
      printf ("wire of region : %ld", region->wires[i].selection_marker);
    }
  // prints num silhouette
  printf ("num sil of region : %ld", region->num_silhouettes);

  // prints silhouettes
  for (i = 0; i < region->num_silhouettes; i++)
    {
      printf ("silhouette of region : %ld", region->silhouettes[i].vp_id);
    }
}

void
api_process (dwg_object * obj)
{
  int block_size_error, num_isolines_error, num_wires_error, acis_data_error,
    isoline_present_error, point_present_error, wireframe_data_present_error,
    version_error, acis_empty_error, point_error, acis2_empty_error,
    sil_error, wire_error, num_sil_error;
  unsigned int version;
  long block_size, num_isolines, num_wires, num_sil;
  char *acis_data;
  char wireframe_data_present, point_present, isoline_present;
  unsigned char acis_empty, acis2_empty;
  dwg_point_3d point;
  dwg_ent_region *region = dwg_object_to_REGION (obj);
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  // returns acis empty
  acis_empty = dwg_ent_region_get_acis_empty (region, &acis_empty_error);
  if (acis_empty_error == 0 && acis_empty == region->acis_empty)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading acis empty");
    }

  // returns version
  version = dwg_ent_region_get_version (region, &version_error);
  if (version_error == 0 && version == region->version)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading version");
    }

  // returns acus data
  acis_data = dwg_ent_region_get_acis_data (region, &acis_data_error);
  if (acis_data_error == 0 && acis_data == region->acis_data)	// error checks
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading acis data");
    }

  // returns wireframe data present value
  wireframe_data_present = dwg_ent_region_get_wireframe_data_present (region,
								      &wireframe_data_present_error);
  if (wireframe_data_present_error == 0 && wireframe_data_present == region->wireframe_data_present)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading wireframe data present");
    }

  point_present =
    dwg_ent_region_get_point_present (region, &point_present_error);
  if (point_present_error == 0 && point_present == region->point_present)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading point present");
    }
  dwg_ent_region_get_point (region, &point, &point_error);
  if (point_error == 0 && point.x == region->point.x
      && point.y == region->point.y && point.z == region->point.z)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading point");
    }

  num_isolines =
    dwg_ent_region_get_num_isolines (region, &num_isolines_error);
  if (num_isolines_error == 0 && region->num_isolines == num_isolines)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num isolines");
    }
  isoline_present =
    dwg_ent_region_get_isoline_present (region, &isoline_present_error);
  if (isoline_present_error == 0
      && isoline_present == region->isoline_present)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading isoline present");
    }

  num_wires = dwg_ent_region_get_num_wires (region, &num_wires_error);
  if (num_wires_error == 0)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num wires");
    }

  wire = dwg_ent_region_get_wire (region, &wire_error);
  if (wire_error == 0)
    {
      int i, matches = 1;
      for (i = 0; i < num_wires; i++)
	{
	  if (wire[i].selection_marker != region->wires[i].selection_marker)
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
	  fail ("Working Properly");
	}
    }
  else
    {
      fail ("error in reading num wires");
    }

  num_sil = dwg_ent_region_get_num_silhouettes (region, &num_sil_error);
  if (num_sil_error == 0 && region->num_silhouettes == num_sil)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num silhouette");
    }
  sil = dwg_ent_region_get_silhouette (region, &sil_error);
  if (sil_error == 0)
    {
      int i, matches = 1;
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
	  fail ("error in reading silhouette");
	}
    }
  else
    {
      fail ("error in reading silhouette");
    }

}

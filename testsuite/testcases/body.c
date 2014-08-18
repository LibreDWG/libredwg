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

  if (dwg_get_type (obj) == DWG_TYPE_BODY)
    {
      output_process (obj);
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
  dwg_ent_body *body = dwg_object_to_BODY (obj);
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  // returns acis empty
  acis_empty = dwg_ent_body_get_acis_empty (body, &acis_empty_error);
  if (acis_empty_error == 0 && acis_empty == body->acis_empty)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading acis empty");
    }

  // returns version
  version = dwg_ent_body_get_version (body, &version_error);
  if (version_error == 0 && version == body->version)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading version");
    }

  // returns acus data
  acis_data = dwg_ent_body_get_acis_data (body, &acis_data_error);
  if (acis_data_error == 0 && acis_data == body->acis_data)	// error checks
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading acis data");
    }

  // returns wireframe data present value
  wireframe_data_present = dwg_ent_body_get_wireframe_data_present (body,
								    &wireframe_data_present_error);
  if (wireframe_data_present_error == 0 && wireframe_data_present == body->wireframe_data_present)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading wireframe data present");
    }

  point_present = dwg_ent_body_get_point_present (body, &point_present_error);
  if (point_present_error == 0 && point_present == body->point_present)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading point present");
    }
  dwg_ent_body_get_point (body, &point, &point_error);
  if (point_error == 0 && point.x == body->point.x
      && point.y == body->point.y && point.z == body->point.z)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading point");
    }

  num_isolines = dwg_ent_body_get_num_isolines (body, &num_isolines_error);
  if (num_isolines_error == 0 && body->num_isolines == num_isolines)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num isolines");
    }
  isoline_present =
    dwg_ent_body_get_isoline_present (body, &isoline_present_error);
  if (isoline_present_error == 0 && isoline_present == body->isoline_present)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading isoline present");
    }

  num_wires = dwg_ent_body_get_num_wires (body, &num_wires_error);
  if (num_wires_error == 0)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num wires");
    }

  wire = dwg_ent_body_get_wire (body, &wire_error);
  if (wire_error == 0)
    {
      int i, matches = 1;
      for (i = 0; i < num_wires; i++)
	{
	  if (wire[i].selection_marker != body->wires[i].selection_marker)
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

  num_sil = dwg_ent_body_get_num_silhouettes (body, &num_sil_error);
  if (num_sil_error == 0 && body->num_silhouettes == num_sil)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num silhouette");
    }
  sil = dwg_ent_body_get_silhouette (body, &sil_error);
  if (sil_error == 0)
    {
      int i, matches = 1;
      for (i = 0; i < num_sil; i++)
	{
	  if (body->silhouettes[i].vp_id != sil[i].vp_id)
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

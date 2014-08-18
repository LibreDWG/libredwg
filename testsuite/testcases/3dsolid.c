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

  if (dwg_get_type (obj) == DWG_TYPE_3DSOLID)
    {
      output_process (obj);
    }
}

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
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
  dwg_ent_3dsolid *_3dsolid = obj->tio.entity->tio._3DSOLID;
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  // returns acis_empty value
  acis_empty = dwg_ent_3dsolid_get_acis_empty (_3dsolid, &acis_empty_error);
  if (acis_empty_error == 0 && acis_empty == _3dsolid->acis_empty)	// error reporting
    {
      pass ("Working Properly");
    }
  else
    {
      fail
	("error in reading acis empty. dwg_ent_3dsolid_get_acis_empty not working");
    }

  // Returns version value
  version = dwg_ent_3dsolid_get_version (_3dsolid, &version_error);
  if (version_error == 0 && version == _3dsolid->version)	// error reporting
    {
      pass ("Working Properly");
    }
  else
    {
      fail
	("error in reading version. dwg_ent_3dsolid_get_version is not working");
    }

  // returns acis data value
  acis_data = dwg_ent_3dsolid_get_acis_data (_3dsolid, &acis_data_error);
  if (acis_data_error == 0 && acis_data == _3dsolid->acis_data)	// error reporting
    {
      pass ("Working Properly");
    }
  else
    {
      fail
	("error in reading acis data. dwg_ent_3dsolid_get_acis_data not working");
    }

  // Returns wireframe_data_present value
  wireframe_data_present =
    dwg_ent_3dsolid_get_wireframe_data_present (_3dsolid,
						&wireframe_data_present_error);
  if (wireframe_data_present_error == 0 && wireframe_data_present == _3dsolid->wireframe_data_present)	// error reporting
    {
      pass ("Working Properly");
    }
  else
    {
      fail
	("error in reading wireframe data present. dwg_ent_3dsolid_get_wireframe_data_present is not working");
    }

  // returns point present value
  point_present =
    dwg_ent_3dsolid_get_point_present (_3dsolid, &point_present_error);
  if (point_present_error == 0 && point_present == _3dsolid->point_present)	// error reporting
    {
      pass ("Working Properly");
    }
  else
    {
      fail
	("error in reading point present. dwg_ent_3dsolid_get_point is not working");
    }

  // Returns point values 
  dwg_ent_3dsolid_get_point (_3dsolid, &point, &point_error);
  if (point_error == 0 && point.x == _3dsolid->point.x && point.y == _3dsolid->point.y && point.z == _3dsolid->point.z)	// error reporting
    {
      pass ("Working properly");
    }
  else
    {
      fail
	("error in reading point. dwg_ent_3dsolid_get_point is not working properly");
    }

  // returns num_isolines
  num_isolines =
    dwg_ent_3dsolid_get_num_isolines (_3dsolid, &num_isolines_error);
  if (num_isolines_error == 0 && _3dsolid->num_isolines == num_isolines)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num isolines");
    }

  // returns isolines_present value
  isoline_present =
    dwg_ent_3dsolid_get_isoline_present (_3dsolid, &isoline_present_error);
  if (isoline_present_error == 0
      && _3dsolid->isoline_present == isoline_present)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading isoline present");
    }

  // returns number of wires
  num_wires = dwg_ent_3dsolid_get_num_wires (_3dsolid, &num_wires_error);
  if (num_wires_error == 0 && _3dsolid->num_wires)
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num wires");
    }

  // returns wire value
  wire = dwg_ent_3dsolid_get_wire (_3dsolid, &wire_error);
  if (wire_error == 0)		// error checking
    {
      int i, matches = 1;
      for (i = 0; i < num_wires; i++)
	{
	  if (_3dsolid->wires[i].selection_marker != wire[i].selection_marker)
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
	  fail ("error in reading num wires");
	}
    }
  else
    {
      fail ("error in reading num wires");
    }

  // returns number of silhpuettes
  num_sil = dwg_ent_3dsolid_get_num_silhouettes (_3dsolid, &num_sil_error);
  if (num_sil_error == 0 && _3dsolid->num_silhouettes == num_sil)	// error check
    {
      pass ("Working Properly");
    }
  else
    {
      fail ("error in reading num silhouette");
    }

  // returns silhouette value
  sil = dwg_ent_3dsolid_get_silhouette (_3dsolid, &sil_error);
  if (sil_error == 0)		// error checking
    {
      int i, matches = 1;
      for (i = 0; i < num_sil; i++)
	{
	  if (_3dsolid->silhouettes[i].vp_id != sil[i].vp_id)
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

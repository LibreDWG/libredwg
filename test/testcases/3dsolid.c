#define DWG_TYPE DWG_TYPE_3DSOLID
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj 
 */
void
api_process (dwg_object * obj)
{
  int error;
  unsigned int version;
  unsigned long *block_size;
  unsigned long num_wires, num_isolines, num_sil;
  char *acis_data;
  char wireframe_data_present, point_present, isoline_present;
  unsigned char acis_empty;
  dwg_point_3d point;
  dwg_ent_3dsolid *_3dsolid = obj->tio.entity->tio._3DSOLID;
  dwg_ent_solid_wire *wire;
  dwg_ent_solid_silhouette *sil;

  // returns acis_empty value
  acis_empty = dwg_ent_3dsolid_get_acis_empty (_3dsolid, &error);
  if (error == 0 && acis_empty == _3dsolid->acis_empty)
    pass ("3dsolid_get_acis_empty");
  else
    fail ("3dsolid_get_acis_empty %d %uc <=> %uc",
          error, acis_empty, _3dsolid->acis_empty);

  // Returns version value
  version = dwg_ent_3dsolid_get_version (_3dsolid, &error);
  if (error == 0 && version == _3dsolid->version)
    pass ("3dsolid_get_version");
  else
    fail ("3dsolid_get_version %d %u <=> %u",
          error, version, version);

  // returns acis data value
  acis_data = dwg_ent_3dsolid_get_acis_data (_3dsolid, &error);
  if (error == 0 && !strcmp(acis_data, _3dsolid->acis_data))
    pass ("3dsolid_get_acis_data");
  else
    fail ("3dsolid_get_acis_data %d \"%s\" <=> \"%s\"",
          error, acis_data, _3dsolid->acis_data);

  // Returns wireframe_data_present value
  wireframe_data_present =
    dwg_ent_3dsolid_get_wireframe_data_present (_3dsolid, &error);
  if (error == 0 && wireframe_data_present == _3dsolid->wireframe_data_present)
    pass ("3dsolid_get_wireframe_data_present");
  else
    fail ("3dsolid_get_wireframe_data_present %d %c <=> %c",
          error, wireframe_data_present, _3dsolid->wireframe_data_present);

  // returns point present value
  point_present =
    dwg_ent_3dsolid_get_point_present (_3dsolid, &error);
  if (error == 0 && point_present == _3dsolid->point_present)
    pass ("3dsolid_get_point_present");
  else
    fail ("3dsolid_get_point_present %d %c <=> %c",
          error, point_present, _3dsolid->point_present);
  
  // Returns point values 
  dwg_ent_3dsolid_get_point (_3dsolid, &point, &error);
  if (error == 0 &&
      point.x == _3dsolid->point.x &&
      point.y == _3dsolid->point.y &&
      point.z == _3dsolid->point.z)
    {
      pass ("3dsolid_get_point");
    }
  else
    {
      fail ("3dsolid_get_point %d", error);
    }

  // returns pointer to block_size
  block_size = dwg_ent_3dsolid_get_block_size (_3dsolid, &error);
  if (error == 0 && _3dsolid->block_size == block_size)
    pass ("3dsolid_get_block_size");
  else
    fail ("3dsolid_get_block_size %d", error);

  // returns num_isolines
  num_isolines = dwg_ent_3dsolid_get_num_isolines (_3dsolid, &error);
  if (error == 0 && _3dsolid->num_isolines == num_isolines)
    pass ("3dsolid_get_num_isolines");
  else
    fail ("3dsolid_get_num_isolines %d", error);

  // returns isolines_present value
  isoline_present = dwg_ent_3dsolid_get_isoline_present (_3dsolid, &error);
  if (error == 0 && _3dsolid->isoline_present == isoline_present)
    pass ("3dsolid_get_isoline_present");
  else
    fail ("3dsolid_get_isoline_present %d", error);

  // returns number of wires
  num_wires = dwg_ent_3dsolid_get_num_wires (_3dsolid, &error);
  if (error == 0 && num_wires == _3dsolid->num_wires)
    pass ("3dsolid_get_num_wires");
  else
    fail ("3dsolid_get_num_wires %d", error);

  // returns wire value
  wire = dwg_ent_3dsolid_get_wire (_3dsolid, &error);
  if (error == 0)
    {
      unsigned long i, matches = 1;
      for (i = 0; i < num_wires; i++)
	{
	  if (_3dsolid->wires[i].selection_marker != wire[i].selection_marker)
	    {
	      matches = 0; break;
	    }
	}

      if (matches)
        pass ("3dsolid_get_wire matches [%ld]", i);
      else
        fail ("3dsolid_get_wire matches");
    }
  else
    fail ("3dsolid_get_wire matches error %d", error);
  
  // returns number of silhouettes
  num_sil = dwg_ent_3dsolid_get_num_silhouettes (_3dsolid, &error);
  if (error == 0 && _3dsolid->num_silhouettes == num_sil)
    pass ("get_num_silhouettes");
  else
    fail ("get_num_silhouettes %d", error);

  // returns silhouette value
  sil = dwg_ent_3dsolid_get_silhouette (_3dsolid, &error);
  if (error == 0)
    {
      unsigned long i, matches = 1;
      for (i = 0; i < num_sil; i++)
	{
	  if (_3dsolid->silhouettes[i].vp_id != sil[i].vp_id)
	    {
	      matches = 0; break;
	    }
	}
      if (matches)
        pass ("3dsolid_get_silhouette matches");
      else
        fail ("3dsolid_get_silhouette matches [%d]", i);
    }
  else
    fail ("error in reading silhouette");
  
}

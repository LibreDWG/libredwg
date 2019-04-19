#define DWG_TYPE DWG_TYPE__3DSOLID
#include "common.c"
#include <dejagnu.h>

/* This function checks API functions for integrity
   @params dwg_object* obj
 */
void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS version;
  BITCODE_BL *block_size, num_isolines, num_wires, num_sil;
  unsigned char *acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present;
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point;
  dwg_3dsolid_wire *wire;
  dwg_3dsolid_silhouette *sil;

  dwg_ent_3dsolid *_3dsolid = obj->tio.entity->tio._3DSOLID;

  acis_empty = dwg_ent_3dsolid_get_acis_empty (_3dsolid, &error);
  if (error == 0 && acis_empty == _3dsolid->acis_empty)
    pass ("3dsolid_get_acis_empty");
  else
    fail ("3dsolid_get_acis_empty %d " FORMAT_B " <=> " FORMAT_B, error,
          acis_empty, _3dsolid->acis_empty);

  version = dwg_ent_3dsolid_get_version (_3dsolid, &error);
  if (error == 0 && version == _3dsolid->version)
    pass ("3dsolid_get_version");
  else
    fail ("3dsolid_get_version %d " FORMAT_BS " <=> " FORMAT_BS, error,
          version, version);

  acis_data = dwg_ent_3dsolid_get_acis_data (_3dsolid, &error);
  if (error == 0 && !strcmp ((char *)acis_data, (char *)_3dsolid->acis_data))
    pass ("3dsolid_get_acis_data");
  else
    fail ("3dsolid_get_acis_data %d \"%s\" <=> \"%s\"", error, acis_data,
          _3dsolid->acis_data);

  wireframe_data_present
      = dwg_ent_3dsolid_get_wireframe_data_present (_3dsolid, &error);
  if (error == 0 && wireframe_data_present == _3dsolid->wireframe_data_present)
    pass ("3dsolid_get_wireframe_data_present");
  else
    fail ("3dsolid_get_wireframe_data_present %d " FORMAT_B " <=> " FORMAT_B,
          error, wireframe_data_present, _3dsolid->wireframe_data_present);

  point_present = dwg_ent_3dsolid_get_point_present (_3dsolid, &error);
  if (error == 0 && point_present == _3dsolid->point_present)
    pass ("3dsolid_get_point_present");
  else
    fail ("3dsolid_get_point_present %d " FORMAT_B " <=> " FORMAT_B, error,
          point_present, _3dsolid->point_present);

  dwg_ent_3dsolid_get_point (_3dsolid, &point, &error);
  if (error == 0 && point.x == _3dsolid->point.x
      && point.y == _3dsolid->point.y && point.z == _3dsolid->point.z)
    {
      pass ("3dsolid_get_point");
    }
  else
    {
      fail ("3dsolid_get_point %d", error);
    }

  num_isolines = dwg_ent_3dsolid_get_num_isolines (_3dsolid, &error);
  if (error == 0 && _3dsolid->num_isolines == num_isolines)
    pass ("3dsolid_get_num_isolines");
  else
    fail ("3dsolid_get_num_isolines %d", error);

  isoline_present = dwg_ent_3dsolid_get_isoline_present (_3dsolid, &error);
  if (error == 0 && _3dsolid->isoline_present == isoline_present)
    pass ("3dsolid_get_isoline_present");
  else
    fail ("3dsolid_get_isoline_present %d", error);

  num_wires = dwg_ent_3dsolid_get_num_wires (_3dsolid, &error);
  if (error == 0 && num_wires == _3dsolid->num_wires)
    pass ("3dsolid_get_num_wires");
  else
    fail ("3dsolid_get_num_wires %d", error);

  wire = dwg_ent_3dsolid_get_wires (_3dsolid, &error);
  if (error == 0)
    {
      BITCODE_BL i, matches = 1;
      for (i = 0; i < num_wires; i++)
        {
          if (_3dsolid->wires[i].selection_marker != wire[i].selection_marker)
            {
              matches = 0;
              break;
            }
        }

      if (matches)
        pass ("3dsolid_get_wires matches [" FORMAT_BL "]", i);
      else
        fail ("3dsolid_get_wires matches");
    }
  else
    fail ("3dsolid_get_wire matches error %d", error);

  num_sil = dwg_ent_3dsolid_get_num_silhouettes (_3dsolid, &error);
  if (error == 0 && _3dsolid->num_silhouettes == num_sil)
    pass ("get_num_silhouettes");
  else
    fail ("get_num_silhouettes %d", error);

  sil = dwg_ent_3dsolid_get_silhouettes (_3dsolid, &error);
  if (error == 0)
    {
      BITCODE_BL i, matches = 1;
      for (i = 0; i < num_sil; i++)
        {
          if (_3dsolid->silhouettes[i].vp_id != sil[i].vp_id)
            {
              matches = 0;
              break;
            }
        }
      if (matches)
        pass ("3dsolid_get_silhouettes matches");
      else
        fail ("3dsolid_get_silhouettes matches [%d]", i);
    }
  else
    fail ("error in reading silhouettes");
}

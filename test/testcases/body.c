#define DWG_TYPE DWG_TYPE_BODY
#include "common.c"
#include <dejagnu.h>

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
  dwg_ent_body *body = dwg_object_to_BODY (obj);
  dwg_3dsolid_wire *wire;
  dwg_3dsolid_silhouette *sil;

  acis_empty = dwg_ent_body_get_acis_empty (body, &error);
  if (!error && acis_empty == body->acis_empty) // error check
    pass ("Working Properly");
  else
    fail ("error in reading acis empty");

  version = dwg_ent_body_get_version (body, &error);
  if (!error && version == body->version) // error check
    pass ("Working Properly");
  else
    fail ("error in reading version");

  acis_data = dwg_ent_body_get_acis_data (body, &error);
  if (!error && acis_data == body->acis_data) // error checks
    pass ("Working Properly");
  else
    fail ("error in reading acis data");

  wireframe_data_present
      = dwg_ent_body_get_wireframe_data_present (body, &error);
  if (!error && wireframe_data_present == body->wireframe_data_present)
    pass ("Working Properly");
  else
    fail ("error in reading wireframe data present");

  point_present = dwg_ent_body_get_point_present (body, &error);
  if (!error && point_present == body->point_present)
    pass ("Working Properly");
  else
    fail ("error in reading point present");

  dwg_ent_body_get_point (body, &point, &error);
  if (!error && point.x == body->point.x && point.y == body->point.y
      && point.z == body->point.z)
    pass ("Working Properly");
  else
    fail ("error in reading point");

  num_isolines = dwg_ent_body_get_num_isolines (body, &error);
  if (!error && body->num_isolines == num_isolines)
    pass ("Working Properly");
  else
    fail ("error in reading num isolines");
  isoline_present = dwg_ent_body_get_isoline_present (body, &error);
  if (!error && isoline_present == body->isoline_present)
    pass ("Working Properly");
  else
    fail ("error in reading isoline present");

  num_wires = dwg_ent_body_get_num_wires (body, &error);
  if (!error)
    pass ("Working Properly");
  else
    fail ("error in reading num wires");

  wire = dwg_ent_body_get_wires (body, &error);
  if (!error)
    {
      BITCODE_BL i;
      int matches = 1;
      for (i = 0; i < num_wires; i++)
        {
          if (wire[i].selection_marker != body->wires[i].selection_marker)
            matches = 0;
        }
      if (matches)
        pass ("Working Properly");
      else
        fail ("Working Properly");
    }
  else
    fail ("error in reading num wires");

  num_sil = dwg_ent_body_get_num_silhouettes (body, &error);
  if (!error && body->num_silhouettes == num_sil)
    pass ("Working Properly");
  else
    fail ("error in reading num silhouettes");
  sil = dwg_ent_body_get_silhouettes (body, &error);
  if (!error)
    {
      BITCODE_BL i;
      int matches = 1;
      for (i = 0; i < num_sil; i++)
        {
          if (body->silhouettes[i].vp_id != sil[i].vp_id)
            matches = 0;
        }
      if (matches)
        pass ("Working Properly");
      else
        fail ("error in reading silhouettes");
    }
  else
    fail ("error in reading silhouettes");
}

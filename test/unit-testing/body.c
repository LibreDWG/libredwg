#define DWG_TYPE DWG_TYPE_BODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  BITCODE_BS version;
  BITCODE_BL block_size, num_isolines, num_wires, num_sil, unknown_2007;
  unsigned char *acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present;
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point, pt3d;
  dwg_3dsolid_wire *wire;
  dwg_3dsolid_silhouette *sil;
  BITCODE_H history_id;

  dwg_ent_body *body = dwg_object_to_BODY (obj);
  Dwg_Version_Type dwg_version = obj->parent->header.version;

  CHK_ENTITY_TYPE (body, BODY, acis_empty, B, acis_empty);
  if (dwg_ent_body_get_acis_empty (body, &error) != acis_empty || error)
    fail ("old API dwg_ent_body_get_acis_empty");
  CHK_ENTITY_TYPE (body, BODY, version, BS, version);
  if (dwg_ent_body_get_version (body, &error) != version || error)
    fail ("old API dwg_ent_body_get_version");
  CHK_ENTITY_TYPE (body, BODY, acis_data, TV, acis_data);
  if (strcmp ((char *)dwg_ent_body_get_acis_data (body, &error),
              (char *)acis_data)
      || error)
    fail ("old API dwg_ent_body_get_acis_data");
  CHK_ENTITY_TYPE (body, BODY, wireframe_data_present, B,
                   wireframe_data_present);
  if (dwg_ent_body_get_wireframe_data_present (body, &error)
          != wireframe_data_present
      || error)
    fail ("old API dwg_ent_body_get_wireframe_data_present");
  CHK_ENTITY_TYPE (body, BODY, point_present, B, point_present);
  if (dwg_ent_body_get_point_present (body, &error) != point_present || error)
    fail ("old API dwg_ent_body_get_point_present");
  CHK_ENTITY_3RD (body, BODY, point, point);
  dwg_ent_body_get_point (body, &pt3d, &error);
  if (error || memcmp (&point, &pt3d, sizeof (point)))
    fail ("old API dwg_ent_body_get_point");

  CHK_ENTITY_TYPE (body, BODY, isoline_present, B, isoline_present);
  if (dwg_ent_body_get_isoline_present (body, &error) != isoline_present
      || error)
    fail ("old API dwg_ent_body_get_isoline_present");
  CHK_ENTITY_TYPE (body, BODY, num_isolines, BL, num_isolines);
  if (dwg_ent_body_get_num_isolines (body, &error) != num_isolines || error)
    fail ("old API dwg_ent_body_get_num_isolines");
  CHK_ENTITY_TYPE (body, BODY, num_wires, BL, num_wires);
  if (dwg_ent_body_get_num_wires (body, &error) != num_wires || error)
    fail ("old API dwg_ent_body_get_num_wires");
  CHK_ENTITY_TYPE (body, BODY, num_silhouettes, BL, num_sil);
  if (dwg_ent_body_get_num_silhouettes (body, &error) != num_sil || error)
    fail ("old API dwg_ent_body_get_num_sil");

  wire = dwg_ent_body_get_wires (body, &error);
  if (!error)
    {
      for (i = 0; i < num_wires; i++)
        printf ("BODY.wire[%u]: " FORMAT_BL "\n", i, wire[i].selection_marker);
    }
  else
    fail ("num wires");

  sil = dwg_ent_body_get_silhouettes (body, &error);
  if (!error)
    {
      for (i = 0; i < num_sil; i++)
        printf ("BODY.silhouette[%u]: " FORMAT_BL "\n", i, sil[i].vp_id);
    }
  else
    fail ("silhouettes");

  if (dwg_version >= R_2007 && body->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (body, BODY, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (body, BODY, history_id, history_id);
    }
}

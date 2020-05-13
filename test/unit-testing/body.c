#define DWG_TYPE DWG_TYPE_BODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  BITCODE_BS version;
  BITCODE_BL block_size, isolines, num_wires, num_silhouettes, unknown_2007;
  unsigned char *acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present;
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point, pt3d;
  dwg_3dsolid_wire *wires;
  dwg_3dsolid_silhouette *silhouettes;
  BITCODE_H history_id;

  dwg_ent_body *body = dwg_object_to_BODY (obj);
  Dwg_Version_Type dwg_version = obj->parent->header.version;

  CHK_ENTITY_TYPE_W_OLD (body, BODY, acis_empty, B);
  CHK_ENTITY_TYPE_W_OLD (body, BODY, version, BS);
  CHK_ENTITY_TYPE (body, BODY, acis_data, TV);
  if (strcmp ((char *)dwg_ent_body_get_acis_data (body, &error),
              (char *)acis_data)
      || error)
    fail ("old API dwg_ent_body_get_acis_data");
  CHK_ENTITY_TYPE_W_OLD (body, BODY, wireframe_data_present, B);
  CHK_ENTITY_TYPE_W_OLD (body, BODY, point_present, B);
  CHK_ENTITY_3RD_W_OLD (body, BODY, point);

  CHK_ENTITY_TYPE_W_OLD (body, BODY, isoline_present, B);
  CHK_ENTITY_TYPE_W_OLD (body, BODY, isolines, BL);
  CHK_ENTITY_TYPE_W_OLD (body, BODY, num_wires, BL);
  CHK_ENTITY_TYPE_W_OLD (body, BODY, num_silhouettes, BL);

  wires = dwg_ent_body_get_wires (body, &error);
  if (!error)
    {
      for (i = 0; i < num_wires; i++)
        printf ("BODY.wire[%u]: " FORMAT_BL "\n", i, wires[i].selection_marker);
    }
  else
    fail ("num wires");

  silhouettes = dwg_ent_body_get_silhouettes (body, &error);
  if (!error)
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("BODY.silhouette[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  else
    fail ("silhouettes");

  if (dwg_version >= R_2007 && body->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (body, BODY, unknown_2007, BL);
      CHK_ENTITY_H (body, BODY, history_id);
    }
}

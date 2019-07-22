#define DWG_TYPE DWG_TYPE__3DSOLID
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  BITCODE_BS version;
  BITCODE_BL num_blocks, num_isolines, num_wires, num_sil, blvalue;
  unsigned char *acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present;
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point, pt3d;
  dwg_3dsolid_wire *wire;
  dwg_3dsolid_silhouette *sil;
  BITCODE_H history_id;

  dwg_ent_3dsolid *_3dsolid = obj->tio.entity->tio._3DSOLID;
  Dwg_Version_Type dwg_version = obj->parent->header.version;

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, acis_empty, B, acis_empty);
  if (dwg_ent_3dsolid_get_acis_empty (_3dsolid, &error) != acis_empty || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_acis_empty\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, version, BS, version);
  if (dwg_ent_3dsolid_get_version (_3dsolid, &error) != version || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_version\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_blocks, BL, num_blocks);
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, acis_data, TV, acis_data);
  if ((acis_data
       && strcmp ((char *)dwg_ent_3dsolid_get_acis_data (_3dsolid, &error),
                  (char *)acis_data))
      || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_acis_data\n");
      exit (1);
    }

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, wireframe_data_present, B,
                   wireframe_data_present);
  if (dwg_ent_3dsolid_get_wireframe_data_present (_3dsolid, &error)
          != wireframe_data_present
      || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_wireframe_data_present\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, point_present, B, point_present);
  if (dwg_ent_3dsolid_get_point_present (_3dsolid, &error) != point_present
      || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_point_present\n");
      exit (1);
    }

  CHK_ENTITY_3RD (_3dsolid, 3DSOLID, point, point);
  dwg_ent_3dsolid_get_point (_3dsolid, &pt3d, &error);
  if (error || memcmp (&point, &pt3d, sizeof (point)))
    {
      printf ("Error with old API dwg_ent_3dsolid_get_point\n");
      exit (1);
    }

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, isoline_present, B, isoline_present);
  if (dwg_ent_3dsolid_get_isoline_present (_3dsolid, &error) != isoline_present
      || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_isoline_present\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_isolines, BL, num_isolines);
  if (dwg_ent_3dsolid_get_num_isolines (_3dsolid, &error) != num_isolines || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_num_isolines\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_wires, BL, num_wires);
  if (dwg_ent_3dsolid_get_num_wires (_3dsolid, &error) != num_wires || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_num_wires\n");
      exit (1);
    }
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_silhouettes, BL, num_sil);
  if (dwg_ent_3dsolid_get_num_silhouettes (_3dsolid, &error) != num_sil || error)
    {
      printf ("Error with old API dwg_ent_3dsolid_get_num_sil\n");
      exit (1);
    }

  wire = dwg_ent_3dsolid_get_wires (_3dsolid, &error);
  if (!error)
    {
      for (i = 0; i < num_wires; i++)
        printf ("wire of _3dsolid :" FORMAT_BL "\n", wire[i].selection_marker);
      free (wire);
    }
  else
    printf ("error in reading num wires\n");

  sil = dwg_ent_3dsolid_get_silhouettes (_3dsolid, &error);
  if (!error)
    {
      for (i = 0; i < num_sil; i++)
        printf ("silhouettes of _3dsolid :" FORMAT_BL "\n", sil[i].vp_id);
      free (sil);
    }
  else
    printf ("error in reading silhouettes\n");

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, unknown_2007, BL, blvalue);
  if (dwg_version >= R_2007)
    {
      CHK_ENTITY_H (_3dsolid, 3DSOLID, history_id, history_id);
    }
}

#define DWG_TYPE DWG_TYPE_REGION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  BITCODE_BS version;
  BITCODE_BL num_blocks, num_isolines, num_wires, num_sil, unknown_2007;
  unsigned char *acis_data;
  BITCODE_B wireframe_data_present, point_present, isoline_present;
  BITCODE_B acis_empty, acis2_empty;
  dwg_point_3d point, pt3d;
  dwg_3dsolid_wire *wire;
  dwg_3dsolid_silhouette *sil;
  BITCODE_H history_id;

  dwg_ent_region *region = dwg_object_to_REGION (obj);
  Dwg_Version_Type dwg_version = obj->parent->header.version;

  CHK_ENTITY_TYPE (region, REGION, acis_empty, B, acis_empty);
  if (dwg_ent_region_get_acis_empty (region, &error) != acis_empty || error)
    fail ("old API dwg_ent_region_get_acis_empty");
  CHK_ENTITY_TYPE (region, REGION, version, BS, version);
  if (dwg_ent_region_get_version (region, &error) != version || error)
    fail ("old API dwg_ent_region_get_version");
  CHK_ENTITY_TYPE (region, REGION, num_blocks, BL, num_blocks);
  CHK_ENTITY_TYPE (region, REGION, acis_data, TV, acis_data);
  if ((acis_data
       && strcmp ((char *)dwg_ent_region_get_acis_data (region, &error),
                  (char *)acis_data))
      || error)
    {
      fail ("old API dwg_ent_region_get_acis_data");
    }

  CHK_ENTITY_TYPE (region, REGION, wireframe_data_present, B,
                   wireframe_data_present);
  if (dwg_ent_region_get_wireframe_data_present (region, &error)
          != wireframe_data_present
      || error)
    {
      fail ("old API dwg_ent_region_get_wireframe_data_present");
    }
  CHK_ENTITY_TYPE (region, REGION, point_present, B, point_present);
  if (dwg_ent_region_get_point_present (region, &error) != point_present
      || error)
    {
      fail ("old API dwg_ent_region_get_point_present");
    }

  CHK_ENTITY_3RD (region, REGION, point, point);
  dwg_ent_region_get_point (region, &pt3d, &error);
  if (error || memcmp (&point, &pt3d, sizeof (point)))
    {
      fail ("old API dwg_ent_region_get_point");
    }

  CHK_ENTITY_TYPE (region, REGION, isoline_present, B, isoline_present);
  if (dwg_ent_region_get_isoline_present (region, &error) != isoline_present
      || error)
    {
      fail ("old API dwg_ent_region_get_isoline_present");
    }
  CHK_ENTITY_TYPE (region, REGION, num_isolines, BL, num_isolines);
  if (dwg_ent_region_get_num_isolines (region, &error) != num_isolines
      || error)
    {
      fail ("old API dwg_ent_region_get_num_isolines");
    }
  CHK_ENTITY_TYPE (region, REGION, num_wires, BL, num_wires);
  if (dwg_ent_region_get_num_wires (region, &error) != num_wires || error)
    {
      fail ("old API dwg_ent_region_get_num_wires");
    }
  CHK_ENTITY_TYPE (region, REGION, num_silhouettes, BL, num_sil);
  if (dwg_ent_region_get_num_silhouettes (region, &error) != num_sil || error)
    {
      fail ("old API dwg_ent_region_get_num_sil");
    }

  wire = dwg_ent_region_get_wires (region, &error);
  if (!error)
    {
      for (i = 0; i < num_wires; i++)
        printf ("wire of region :" FORMAT_BL "\n", wire[i].selection_marker);
      free (wire);
    }
  else
    printf ("error in reading num wires");

  sil = dwg_ent_region_get_silhouettes (region, &error);
  if (!error)
    {
      for (i = 0; i < num_sil; i++)
        printf ("silhouettes of region :" FORMAT_BL "\n", sil[i].vp_id);
      free (sil);
    }
  else
    printf ("error in reading silhouettes");

  if (dwg_version >= R_2007 && region->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (region, REGION, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (region, REGION, history_id, history_id);
    }
}

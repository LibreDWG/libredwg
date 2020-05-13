#define DWG_TYPE DWG_TYPE_REGION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  _3DSOLID_FIELDS;

  dwg_ent_region *region = dwg_object_to_REGION (obj);
  Dwg_Version_Type dwg_version = obj->parent->header.version;

  CHK_ENTITY_TYPE_W_OLD (region, REGION, acis_empty, B);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, version, BS);
  CHK_ENTITY_TYPE (region, REGION, num_blocks, BL);
  CHK_ENTITY_TYPE (region, REGION, acis_data, TV);
  if ((acis_data
       && strcmp ((char *)dwg_ent_region_get_acis_data (region, &error),
                  (char *)acis_data))
      || error)
    {
      fail ("old API dwg_ent_region_get_acis_data");
    }

  CHK_ENTITY_TYPE_W_OLD (region, REGION, wireframe_data_present, B);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, point_present, B);
  CHK_ENTITY_3RD_W_OLD (region, REGION, point);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, isoline_present, B);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, isolines, BL);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, num_wires, BL);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, num_silhouettes, BL);

  wires = dwg_ent_region_get_wires (region, &error);
  if (!error)
    {
      for (i = 0; i < num_wires; i++)
        printf ("wire of region :" FORMAT_BL "\n", wires[i].selection_marker);
      free (wires);
    }
  else
    printf ("error in reading num wires");

  silhouettes = dwg_ent_region_get_silhouettes (region, &error);
  if (!error)
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("silhouettes of region :" FORMAT_BL "\n", silhouettes[i].vp_id);
      free (silhouettes);
    }
  else
    printf ("error in reading silhouettes");

  if (dwg_version >= R_2007 && region->history_id) // if it did not fail before
    {
      CHK_ENTITY_H (region, REGION, history_id);
    }
}

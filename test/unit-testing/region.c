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
#ifdef USE_DEPRECATED_API
  if ((acis_data
       && strcmp ((char *)dwg_ent_region_get_acis_data (region, &error),
                  (char *)acis_data))
      || error)
    {
      fail ("old API dwg_ent_region_get_acis_data");
    }
#endif

  CHK_ENTITY_TYPE_W_OLD (region, REGION, wireframe_data_present, B);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, point_present, B);
  CHK_ENTITY_3RD_W_OLD (region, REGION, point);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, isoline_present, B);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, isolines, BL);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, num_wires, BL);
  CHK_ENTITY_TYPE_W_OLD (region, REGION, num_silhouettes, BL);

#ifdef USE_DEPRECATED_API
  wires = dwg_ent_region_get_wires (region, &error);
#else
  wires = region->wires;
  error = 0;
#endif
  if (!error)
    {
      for (i = 0; i < num_wires; i++)
        {
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, type, RC);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, selection_marker, BLd);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, color, BL);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, acis_index, BLd);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, num_points, BL);
          CHK_SUBCLASS_3DPOINTS (wires[i], 3DSOLID_wire, points,
                                 wires[i].num_points);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, transform_present, B);
          if (wires[i].transform_present)
            {
              CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_x);
              CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_y);
              CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_z);
              CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, translation);
              CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, scale);
              CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, has_rotation, B);
              CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, has_reflection, B);
              CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, has_shear, B);
            }
        }
#ifdef USE_DEPRECATED_API
      FREE (wires);
#endif
    }
  else
    fail ("dwg_ent_region_get_wires");

#ifdef USE_DEPRECATED_API
  silhouettes = dwg_ent_region_get_silhouettes (region, &error);
#else
  silhouettes = region->silhouettes;
#endif
  if (!error)
    {
      for (i = 0; i < num_silhouettes; i++)
        {
          CHK_SUBCLASS_TYPE (silhouettes[i], 3DSOLID_silhouette, vp_id, BL);
          CHK_SUBCLASS_3RD (silhouettes[i], 3DSOLID_silhouette, vp_target);
          CHK_SUBCLASS_3RD (silhouettes[i], 3DSOLID_silhouette,
                            vp_dir_from_target);
          CHK_SUBCLASS_3RD (silhouettes[i], 3DSOLID_silhouette, vp_up_dir);
          CHK_SUBCLASS_TYPE (silhouettes[i], 3DSOLID_silhouette,
                             vp_perspective, B);
          CHK_SUBCLASS_TYPE (silhouettes[i], 3DSOLID_silhouette, has_wires, B);
          if (silhouettes[i].has_wires)
            {
              wires = silhouettes[i].wires;
              CHK_SUBCLASS_TYPE (silhouettes[i], 3DSOLID_silhouette, num_wires,
                                 BL);
              for (unsigned j = 0; j < silhouettes[i].num_wires; j++)
                {
                  CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, type, RC);
                  CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, selection_marker,
                                     BLd);
                  CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, color, BL);
                  CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, acis_index, BLd);
                  CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, num_points, BL);
                  CHK_SUBCLASS_3DPOINTS (wires[j], 3DSOLID_wire, points,
                                         wires[i].num_points);
                  CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, transform_present,
                                     B);
                  if (wires[j].transform_present)
                    {
                      CHK_SUBCLASS_3RD (wires[j], 3DSOLID_wire, axis_x);
                      CHK_SUBCLASS_3RD (wires[j], 3DSOLID_wire, axis_y);
                      CHK_SUBCLASS_3RD (wires[j], 3DSOLID_wire, axis_z);
                      CHK_SUBCLASS_3RD (wires[j], 3DSOLID_wire, translation);
                      CHK_SUBCLASS_3RD (wires[j], 3DSOLID_wire, scale);
                      CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, has_rotation,
                                         B);
                      CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire,
                                         has_reflection, B);
                      CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, has_shear, B);
                    }
                }
            }
        }
#ifdef USE_DEPRECATED_API
      FREE (silhouettes);
#endif
    }
  else
    fail ("dwg_ent_body_get_silhouettes");

  if (version > 1)
    {
      CHK_ENTITY_TYPE (region, REGION, num_materials, BL);
      SINCE (R_2007a)
      {
        if (!dwg_dynapi_entity_value (region, "REGION", "materials",
                                      &materials, NULL))
          fail ("REGION.materials");
        else
          {
            for (i = 0; i < num_materials; i++)
              {
                CHK_SUBCLASS_TYPE (materials[i], 3DSOLID_material, array_index,
                                   BL);
                CHK_SUBCLASS_TYPE (materials[i], 3DSOLID_material, mat_absref,
                                   BL);
                CHK_SUBCLASS_H (materials[i], 3DSOLID_material,
                                material_handle);
              }
          }
      }
    }
  CHK_ENTITY_TYPE (region, REGION, has_revision_guid, B);
  if (has_revision_guid)
    {
      CHK_ENTITY_TYPE (region, REGION, revision_major, BL);
      CHK_ENTITY_TYPE (region, REGION, revision_minor1, BS);
      CHK_ENTITY_TYPE (region, REGION, revision_minor2, BS);
      CHK_ENTITY_TYPE (region, REGION, revision_bytes, TV);
      CHK_ENTITY_TYPE (region, REGION, end_marker, BL);
    }

  SINCE (R_2007a)
  {
    CHK_ENTITY_H (region, REGION, history_id);
  }
}

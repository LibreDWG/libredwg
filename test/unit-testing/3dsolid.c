#define DWG_TYPE DWG_TYPE__3DSOLID
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  _3DSOLID_FIELDS;
  dwg_point_3d pt3d;

  dwg_ent_3dsolid *_3dsolid = obj->tio.entity->tio._3DSOLID;
  Dwg_Version_Type dwg_version = obj->parent->header.version;
  error = 0;

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, acis_empty, B);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_acis_empty (_3dsolid, &error) != acis_empty || error)
    fail ("old API dwg_ent_3dsolid_get_acis_empty");
#endif
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, version, BS);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_version (_3dsolid, &error) != version || error)
    fail ("old API dwg_ent_3dsolid_get_version");
#endif
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_blocks, BL);
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, acis_data, TV);
#ifdef USE_DEPRECATED_API
  if ((acis_data
       && strcmp ((char *)dwg_ent_3dsolid_get_acis_data (_3dsolid, &error),
                  (char *)acis_data))
      || error)
    fail ("old API dwg_ent_3dsolid_get_acis_data");
#endif

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, wireframe_data_present, B);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_wireframe_data_present (_3dsolid, &error)
          != wireframe_data_present
      || error)
    fail ("old API dwg_ent_3dsolid_get_wireframe_data_present");
#endif
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, point_present, B);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_point_present (_3dsolid, &error) != point_present
      || error)
    fail ("old API dwg_ent_3dsolid_get_point_present");
#endif

  CHK_ENTITY_3RD (_3dsolid, 3DSOLID, point);
#ifdef USE_DEPRECATED_API
  dwg_ent_3dsolid_get_point (_3dsolid, &pt3d, &error);
  if (error || memcmp (&point, &pt3d, sizeof (point)))
    fail ("old API dwg_ent_3dsolid_get_point");
#endif

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, isoline_present, B);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_isoline_present (_3dsolid, &error) != isoline_present
      || error)
    fail ("old API dwg_ent_3dsolid_get_isoline_present");
#endif
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, isolines, BL);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_isolines (_3dsolid, &error) != isolines || error)
    fail ("old API dwg_ent_3dsolid_get_isolines");
#endif
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_wires, BL);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_num_wires (_3dsolid, &error) != num_wires || error)
    fail ("old API dwg_ent_3dsolid_get_num_wires");
#endif
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_silhouettes, BL);
#ifdef USE_DEPRECATED_API
  if (dwg_ent_3dsolid_get_num_silhouettes (_3dsolid, &error) != num_silhouettes
      || error)
    fail ("old API dwg_ent_3dsolid_get_num_sil");
#endif

#ifdef USE_DEPRECATED_API
  wires = dwg_ent_3dsolid_get_wires (_3dsolid, &error);
#else
  wires = _3dsolid->wires;
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
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_x);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_y);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_z);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, translation);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, scale);
        }
#ifdef USE_DEPRECATED_API
      FREE (wires);
#endif
    }
  else
    fail ("wires");

#ifdef USE_DEPRECATED_API
  silhouettes = dwg_ent_3dsolid_get_silhouettes (_3dsolid, &error);
#else
  silhouettes = _3dsolid->silhouettes;
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
    fail ("silhouettes");

  SINCE (R_2007a)
  {
    CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_materials, BL);
    if (!dwg_dynapi_entity_value (_3dsolid, "3DSOLID", "materials", &materials,
                                  NULL))
      fail ("3DSOLID.materials");
    else
      for (i = 0; i < num_materials; i++)
        {
          CHK_SUBCLASS_TYPE (_3dsolid->materials[i], 3DSOLID_material,
                             array_index, BL);
          CHK_SUBCLASS_TYPE (_3dsolid->materials[i], 3DSOLID_material,
                             mat_absref, BL);
          CHK_SUBCLASS_H (_3dsolid->materials[i], 3DSOLID_material,
                          material_handle);
        }
    CHK_ENTITY_H (_3dsolid, 3DSOLID, history_id);
  }
  SINCE (R_2013)
  {
    CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, has_revision_guid, B);
    if (has_revision_guid)
      {
        CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, revision_guid, TF);
        CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, revision_major, BL);
        CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, revision_minor1, BS);
        CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, revision_minor2, BS);
        CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, revision_bytes, TF);
      }
    CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, end_marker, BL);
  }
}

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

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, acis_empty, B);
  if (dwg_ent_3dsolid_get_acis_empty (_3dsolid, &error) != acis_empty || error)
    fail ("old API dwg_ent_3dsolid_get_acis_empty");
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, version, BS);
  if (dwg_ent_3dsolid_get_version (_3dsolid, &error) != version || error)
    fail ("old API dwg_ent_3dsolid_get_version");
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_blocks, BL);
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, acis_data, TV);
  if ((acis_data
       && strcmp ((char *)dwg_ent_3dsolid_get_acis_data (_3dsolid, &error),
                  (char *)acis_data))
      || error)
    fail ("old API dwg_ent_3dsolid_get_acis_data");

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, wireframe_data_present, B);
  if (dwg_ent_3dsolid_get_wireframe_data_present (_3dsolid, &error)
          != wireframe_data_present
      || error)
    fail ("old API dwg_ent_3dsolid_get_wireframe_data_present");
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, point_present, B);
  if (dwg_ent_3dsolid_get_point_present (_3dsolid, &error) != point_present
      || error)
    fail ("old API dwg_ent_3dsolid_get_point_present");

  CHK_ENTITY_3RD (_3dsolid, 3DSOLID, point);
  dwg_ent_3dsolid_get_point (_3dsolid, &pt3d, &error);
  if (error || memcmp (&point, &pt3d, sizeof (point)))
    fail ("old API dwg_ent_3dsolid_get_point");

  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, isoline_present, B);
  if (dwg_ent_3dsolid_get_isoline_present (_3dsolid, &error) != isoline_present
      || error)
    fail ("old API dwg_ent_3dsolid_get_isoline_present");
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, isolines, BL);
  if (dwg_ent_3dsolid_get_isolines (_3dsolid, &error) != isolines
      || error)
    fail ("old API dwg_ent_3dsolid_get_isolines");
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_wires, BL);
  if (dwg_ent_3dsolid_get_num_wires (_3dsolid, &error) != num_wires || error)
    fail ("old API dwg_ent_3dsolid_get_num_wires");
  CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_silhouettes, BL);
  if (dwg_ent_3dsolid_get_num_silhouettes (_3dsolid, &error) != num_silhouettes
      || error)
    fail ("old API dwg_ent_3dsolid_get_num_sil");

  wires = dwg_ent_3dsolid_get_wires (_3dsolid, &error);
  if (!error)
    {
      for (i = 0; i < num_wires; i++)
        {
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, type, RC);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, selection_marker, BL);
          PRE (R_2004) {
            CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, color, BS);
          } else {
            CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, color, BL);
          }
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, acis_index, BL);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, num_points, BL);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_x);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_y);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, axis_z);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, translation);
          CHK_SUBCLASS_3RD (wires[i], 3DSOLID_wire, scale);
        }
      free (wires);
    }
  else
    fail ("wires");

  silhouettes = dwg_ent_3dsolid_get_silhouettes (_3dsolid, &error);
  if (!error)
    {
      for (i = 0; i < num_silhouettes; i++)
        ok ("3DSOLID.silhouettes[%d]:" FORMAT_BL, i, silhouettes[i].vp_id);
      free (silhouettes);
    }
  else
    fail ("silhouettes");

  SINCE (R_2007) {
    CHK_ENTITY_TYPE (_3dsolid, 3DSOLID, num_materials, BL);
    if (!dwg_dynapi_entity_value (_3dsolid, "3DSOLID", "materials", &materials, NULL))
      fail ("3DSOLID.materials");
    else
      for (i = 0; i < num_materials; i++)
        {
          CHK_SUBCLASS_TYPE (_3dsolid->materials[i], 3DSOLID_material, array_index, BL);
          CHK_SUBCLASS_TYPE (_3dsolid->materials[i], 3DSOLID_material, mat_absref, BL);
          CHK_SUBCLASS_H (_3dsolid->materials[i], 3DSOLID_material, material_handle);
        }
    CHK_ENTITY_H (_3dsolid, 3DSOLID, history_id);
  }
  SINCE (R_2013) {
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

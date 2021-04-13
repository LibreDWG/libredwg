// unstable
#define DWG_TYPE DWG_TYPE_ASSOCASMBODYACTIONPARAM
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCACTIONPARAM_fields;
  /* AcDbAssocSingleDependencyActionParam */
  BITCODE_BL asdap_class_version; // 0
  BITCODE_H dep;
  /* AcDbAssocAsmbodyActionParam */
  BITCODE_BS class_version;
  _3DSOLID_FIELDS;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_assocasmbodyactionparam *_obj
      = dwg_object_to_ASSOCASMBODYACTIONPARAM (obj);

  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, is_r2013, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, aap_version, BL);
  CHK_ENTITY_UTF8TEXT (_obj, ASSOCASMBODYACTIONPARAM, name);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, asdap_class_version, BL);
  CHK_ENTITY_H (_obj, ASSOCEDGEACTIONPARAM, dep);

  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, class_version, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, acis_empty, B);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, version, BS);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, acis_data, TV);
  if (!dwg_dynapi_entity_value (_obj, "ASSOCASMBODYACTIONPARAM", "acis_data",
                                &acis_data, NULL))
    fail ("ASSOCASMBODYACTIONPARAM.acis_data");
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, wireframe_data_present, B);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, point_present, B);
  CHK_ENTITY_3RD (_obj, ASSOCASMBODYACTIONPARAM, point);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, isoline_present, B);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, isolines, BL);
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, num_wires, BL);
  if (dwg_dynapi_entity_value (_obj, "ASSOCASMBODYACTIONPARAM", "wires",
                               &wires, NULL))
    {
      for (unsigned i = 0; i < num_wires; i++)
        {
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, type, RC);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, selection_marker, BLd);
          PRE (R_2004a)
          {
            CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, color, BS);
          }
          else
          {
            CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, color, BL);
          }
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
    }
  else
    fail ("ASSOCASMBODYACTIONPARAM.wires");

  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, num_silhouettes, BL);
  if (dwg_dynapi_entity_value (_obj, "ASSOCASMBODYACTIONPARAM", "silhouettes",
                               &silhouettes, NULL))
    {
      for (unsigned i = 0; i < num_silhouettes; i++)
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
                  CHK_SUBCLASS_TYPE (wires[j], 3DSOLID_wire, color, BL);
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
      FREE (silhouettes);
    }
  else
    fail ("ASSOCASMBODYACTIONPARAM.silhouettes");

  if (version > 1)
    {
      CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, num_materials, BL);
      SINCE (R_2007a)
      {
        if (!dwg_dynapi_entity_value (_obj, "BREP", "materials", &materials,
                                      NULL))
          fail ("BODY.materials");
        else
          {
            for (unsigned i = 0; i < num_materials; i++)
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
  CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, has_revision_guid, B);
  if (has_revision_guid)
    {
      CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, revision_major, BL);
      CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, revision_minor1, BS);
      CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, revision_minor2, BS);
      CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, revision_bytes, TV);
      CHK_ENTITY_TYPE (_obj, ASSOCASMBODYACTIONPARAM, end_marker, BL);
    }

  SINCE (R_2007a)
  {
    CHK_ENTITY_H (_obj, ASSOCASMBODYACTIONPARAM, history_id);
  }
  // #endif
}

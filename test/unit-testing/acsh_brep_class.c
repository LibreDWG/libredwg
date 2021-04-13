// unstable. 3dsolid point still wrong
#define DWG_TYPE DWG_TYPE_ACSH_BREP_CLASS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // Dwg_EvalExpr evalexpr;
  // Dwg_ACSH_HistoryNode history_node;
  BITCODE_BD *trans;
  // AcDbShBrep
  BITCODE_BL major; /*!< DXF 90 (33) */
  BITCODE_BL minor; /*!< DXF 91 (29) */
  _3DSOLID_FIELDS;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_acsh_brep_class *_obj = dwg_object_to_ACSH_BREP_CLASS (obj);

  CHK_EVALEXPR (ACSH_BREP_CLASS);
  CHK_ACSH_HISTORYNODE ();

  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, major, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, minor, BL);

  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, acis_empty, B);
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, version, BS);
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, acis_data, TV);
  if (!dwg_dynapi_entity_value (_obj, "ACSH_BREP_CLASS", "acis_data",
                                &acis_data, NULL))
    fail ("ACSH_BREP_CLASS.acis_data");
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, wireframe_data_present, B);
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, point_present, B);
  CHK_ENTITY_3RD (_obj, ACSH_BREP_CLASS, point);
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, isoline_present, B);
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, isolines, BL);
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, num_wires, BL);
  if (dwg_dynapi_entity_value (_obj, "ACSH_BREP_CLASS", "wires", &wires, NULL))
    {
      for (unsigned i = 0; i < num_wires; i++)
        {
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, type, RC);
          CHK_SUBCLASS_TYPE (wires[i], 3DSOLID_wire, selection_marker, BLd);
          PRE (R_2004a)
          {
            CHK_SUBCLASS_TYPE_CAST (wires[i], 3DSOLID_wire, color, BS, BL);
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
    fail ("ACSH_BREP_CLASS.wires");

  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, num_silhouettes, BL);
  if (dwg_dynapi_entity_value (_obj, "ACSH_BREP_CLASS", "silhouettes",
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
    fail ("ACSH_BREP_CLASS.silhouettes");

  if (version > 1)
    {
      CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, num_materials, BL);
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
  CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, has_revision_guid, B);
  if (has_revision_guid)
    {
      CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, revision_major, BL);
      CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, revision_minor1, BS);
      CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, revision_minor2, BS);
      CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, revision_bytes, TV);
      CHK_ENTITY_TYPE (_obj, ACSH_BREP_CLASS, end_marker, BL);
    }

  SINCE (R_2007a)
  {
    CHK_ENTITY_H (_obj, ACSH_BREP_CLASS, history_id);
  }
}

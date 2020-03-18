// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_SURFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  //_3DSOLID_FIELDS;
  BITCODE_B acis_empty;
  BITCODE_B unknown;
  BITCODE_BS version;
  BITCODE_BL i, num_blocks;
  BITCODE_BL* block_size;
  char** encr_sat_data;
  BITCODE_RC* acis_data;
  BITCODE_B wireframe_data_present;
  BITCODE_B point_present;
  BITCODE_3BD point;
  BITCODE_BL num_isolines;
  BITCODE_B isoline_present;
  BITCODE_BL num_wires;
  Dwg_3DSOLID_wire * wires;
  BITCODE_BL num_silhouettes;
  Dwg_3DSOLID_silhouette * silhouettes;
  BITCODE_B acis_empty2;
  struct _dwg_entity_3DSOLID* extra_acis_data;
  BITCODE_BL unknown_2007;
  BITCODE_H history_id;
  BITCODE_B acis_empty_bit;

  BITCODE_BS modeler_format_version;
  BITCODE_BS u_isolines;
  BITCODE_BS v_isolines;
  BITCODE_BL class_version;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_surface *_obj = dwg_object_to_SURFACE (obj);

  CHK_ENTITY_TYPE (_obj, SURFACE, acis_empty, B, acis_empty);
  CHK_ENTITY_TYPE (_obj, SURFACE, version, BS, version);
  CHK_ENTITY_TYPE (_obj, SURFACE, acis_data, TF, acis_data);
  CHK_ENTITY_TYPE (_obj, SURFACE, wireframe_data_present, B, wireframe_data_present);
  CHK_ENTITY_TYPE (_obj, SURFACE, point_present, B, point_present);
  CHK_ENTITY_3RD (_obj, SURFACE, point, point);
  CHK_ENTITY_TYPE (_obj, SURFACE, isoline_present, B, isoline_present);
  CHK_ENTITY_TYPE (_obj, SURFACE, num_isolines, BL, num_isolines);
  CHK_ENTITY_TYPE (_obj, SURFACE, num_wires, BL, num_wires);
  CHK_ENTITY_TYPE (_obj, SURFACE, num_silhouettes, BL, num_silhouettes);
  if (!dwg_dynapi_entity_value (_obj, "SURFACE", "wires", &wires, NULL))
    fail ("SURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("SURFACE.wires[%u]: " FORMAT_BL "\n", i, wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "SURFACE", "silhouettes", &silhouettes, NULL))
    fail ("SURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("SURFACE.silhouettes[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (_obj, SURFACE, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (_obj, SURFACE, history_id, history_id);
    }

  CHK_ENTITY_TYPE (_obj, SURFACE, modeler_format_version, BS, modeler_format_version);
  CHK_ENTITY_TYPE (_obj, SURFACE, u_isolines, BS, u_isolines);
  CHK_ENTITY_TYPE (_obj, SURFACE, v_isolines, BS, v_isolines);
  CHK_ENTITY_TYPE (_obj, SURFACE, class_version, BL, class_version);
#endif
}

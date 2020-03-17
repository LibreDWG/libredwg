// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_PLANESURFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  //_3DSOLID_FIELDS;
  BITCODE_B acis_empty;
  BITCODE_B unknown;
  BITCODE_BS version;
  BITCODE_BL num_blocks;
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

  //? sweep_profile, taper_angle
  BITCODE_BS modeler_format_version; /*!< DXF 70 */
  BITCODE_BS u_isolines;         /*!< DXF 71 */
  BITCODE_BS v_isolines;         /*!< DXF 72 */
  BITCODE_BL class_version; /*!< DXF 90 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_loftedsurface *_obj = dwg_object_to_PLANESURFACE (obj);

  CHK_ENTITY_TYPE (_obj, PLANESURFACE, acis_empty, B, acis_empty);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, version, BS, version);
  //CHK_ENTITY_UTF8TEXT (_obj, PLANESURFACE, acis_data, acis_data);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, wireframe_data_present, B, wireframe_data_present);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, point_present, B, point_present);
  CHK_ENTITY_3RD (_obj, PLANESURFACE, point, point);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, isoline_present, B, isoline_present);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, num_isolines, BL, num_isolines);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, num_wires, BL, num_wires);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, num_silhouettes, BL, num_silhouettes);
  if (!dwg_dynapi_entity_value (_obj, "PLANESURFACE", "wires", &wires, NULL))
    fail ("PLANESURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("PLANESURFACE.wires[%u]: " FORMAT_BL "\n", i, wire[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "PLANESURFACE", "silhouettes", &silhouettes, NULL))
    fail ("PLANESURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("PLANESURFACE.silhouettes[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  else
    fail ("silhouettes");
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (_obj, PLANESURFACE, unknown_2007, BL, unknown_2007);
      CHK_ENTITY_H (_obj, PLANESURFACE, history_id, history_id);
    }

  CHK_ENTITY_TYPE (_obj, PLANESURFACE, modeler_format_version, BS, modeler_format_version);
  //CHK_ENTITY_TYPE (_obj, PLANESURFACE, bindata_size, BL, bindata_size);
  //CHK_ENTITY_TYPE (_obj, PLANESURFACE, bindata, TF, bindata); // 310|1
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, u_isolines, BS, u_isolines);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, v_isolines, BS, v_isolines);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, class_version, BL, class_version);
#endif
}

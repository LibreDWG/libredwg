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
  BITCODE_BL isolines;
  BITCODE_B isoline_present;
  BITCODE_BL i, num_wires;
  Dwg_3DSOLID_wire * wires;
  BITCODE_BL num_silhouettes;
  Dwg_3DSOLID_silhouette * silhouettes;
  BITCODE_B acis_empty2;
  struct _dwg_entity_3DSOLID* extra_acis_data;
  BITCODE_BL unknown_2007;
  BITCODE_H history_id;
  BITCODE_B acis_empty_bit;

  BITCODE_BS modeler_format_version; /*!< DXF 70 */
  BITCODE_BS u_isolines;         /*!< DXF 71 */
  BITCODE_BS v_isolines;         /*!< DXF 72 */
  BITCODE_BL class_version; /*!< DXF 90 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_planesurface *_obj = dwg_object_to_PLANESURFACE (obj);

  CHK_ENTITY_TYPE (_obj, PLANESURFACE, acis_empty, B);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, version, BS);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, acis_data, TF);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, wireframe_data_present, B);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, point_present, B);
  CHK_ENTITY_3RD (_obj, PLANESURFACE, point);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, isoline_present, B);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, isolines, BL);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, num_wires, BL);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, num_silhouettes, BL);
  if (!dwg_dynapi_entity_value (_obj, "PLANESURFACE", "wires", &wires, NULL))
    fail ("PLANESURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("PLANESURFACE.wires[%u]: " FORMAT_BL "\n", i, wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "PLANESURFACE", "silhouettes", &silhouettes, NULL))
    fail ("PLANESURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("PLANESURFACE.silhouettes[%u]: " FORMAT_BL "\n", i, silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_TYPE (_obj, PLANESURFACE, unknown_2007, BL);
      CHK_ENTITY_H (_obj, PLANESURFACE, history_id);
    }

  CHK_ENTITY_TYPE (_obj, PLANESURFACE, modeler_format_version, BS);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, u_isolines, BS);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, v_isolines, BS);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, class_version, BL);
#endif
}

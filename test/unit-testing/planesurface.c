// unstable
#define DWG_TYPE DWG_TYPE_PLANESURFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  _3DSOLID_FIELDS;

  BITCODE_BS modeler_format_version;
  BITCODE_BS u_isolines;
  BITCODE_BS v_isolines;
  BITCODE_BL i, class_version;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
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
        printf ("PLANESURFACE.wires[%u]: " FORMAT_BLd "\n", i,
                wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "PLANESURFACE", "silhouettes",
                                &silhouettes, NULL))
    fail ("PLANESURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("PLANESURFACE.silhouettes[%u]: " FORMAT_BL "\n", i,
                silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      CHK_ENTITY_H (_obj, PLANESURFACE, history_id);
    }

  CHK_ENTITY_TYPE (_obj, PLANESURFACE, modeler_format_version, BS);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, u_isolines, BS);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, v_isolines, BS);
  CHK_ENTITY_TYPE (_obj, PLANESURFACE, class_version, BL);
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_NURBSURFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  _3DSOLID_FIELDS;
  BITCODE_BS u_isolines;
  BITCODE_BS v_isolines;
  BITCODE_BL i;
  // AcDbNurbSurface
  BITCODE_BS short170;
  BITCODE_B cv_hull_display; /* DXF 290 */
  BITCODE_3BD uvec1;
  BITCODE_3BD vvec1;
  BITCODE_3BD uvec2;
  BITCODE_3BD vvec2;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_nurbsurface *_obj = dwg_object_to_NURBSURFACE (obj);

  CHK_ENTITY_TYPE (_obj, NURBSURFACE, acis_empty, B);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, version, BS);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, acis_data, TF);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, wireframe_data_present, B);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, point_present, B);
  CHK_ENTITY_3RD (_obj, NURBSURFACE, point);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, isoline_present, B);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, isolines, BL);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, num_wires, BL);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, num_silhouettes, BL);
  if (!dwg_dynapi_entity_value (_obj, "NURBSURFACE", "wires", &wires, NULL))
    fail ("NURBSURFACE.wires");
  else
    {
      for (i = 0; i < num_wires; i++)
        printf ("NURBSURFACE.wires[%u]: " FORMAT_BLd "\n", i,
                wires[i].selection_marker);
    }
  if (!dwg_dynapi_entity_value (_obj, "NURBSURFACE", "silhouettes",
                                &silhouettes, NULL))
    fail ("NURBSURFACE.silhouettes");
  else
    {
      for (i = 0; i < num_silhouettes; i++)
        printf ("NURBSURFACE.silhouettes[%u]: " FORMAT_BL "\n", i,
                silhouettes[i].vp_id);
    }
  if (dwg_version >= R_2007 && _obj->history_id) // if it did not fail before
    {
      // CHK_ENTITY_TYPE (_obj, NURBSURFACE, unknown_2007, BL);
      CHK_ENTITY_H (_obj, NURBSURFACE, history_id);
    }

  CHK_ENTITY_TYPE (_obj, NURBSURFACE, u_isolines, BS);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, v_isolines, BS);

  CHK_ENTITY_TYPE (_obj, NURBSURFACE, short170, BS);
  CHK_ENTITY_TYPE (_obj, NURBSURFACE, cv_hull_display, BS);
  CHK_ENTITY_3RD (_obj, NURBSURFACE, uvec1);
  CHK_ENTITY_3RD (_obj, NURBSURFACE, vvec1);
  CHK_ENTITY_3RD (_obj, NURBSURFACE, uvec2);
  CHK_ENTITY_3RD (_obj, NURBSURFACE, vvec2);
#endif
}

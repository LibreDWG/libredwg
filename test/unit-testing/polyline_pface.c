#define DWG_TYPE DWG_TYPE_POLYLINE_PFACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS numverts, numfaces;
  BITCODE_BL num_owned;
  BITCODE_H first_vertex, last_vertex, *vertex, seqend;
  Dwg_Version_Type version = obj->parent->header.version;

  dwg_ent_polyline_pface *polyline_pface = dwg_object_to_POLYLINE_PFACE (obj);

  // TODO: rename fields to num_verts, num_faces
  CHK_ENTITY_TYPE (polyline_pface, POLYLINE_PFACE, numverts, BS);
  // dwg_ent_polyline_pface_get_numpoints
  CHK_ENTITY_TYPE_W_OLD (polyline_pface, POLYLINE_PFACE, numfaces, BS);
  CHK_ENTITY_TYPE (polyline_pface, POLYLINE_PFACE, num_owned, BL);
  // dwg_ent_polyline_pface_get_points NI

  if (numverts + numfaces == num_owned)
    ok ("numverts + numfaces == num_owned");
  else
    ok ("TODO numverts %d + numfaces %d != num_owned %d", numverts, numfaces,
        num_owned);

  if (version >= R_13b1 && version <= R_2000)
    {
      CHK_ENTITY_H (polyline_pface, POLYLINE_PFACE, first_vertex);
      CHK_ENTITY_H (polyline_pface, POLYLINE_PFACE, last_vertex);
    }
  if (version >= R_2004)
    {
      CHK_ENTITY_HV (polyline_pface, POLYLINE_PFACE, vertex, num_owned);
    }
  CHK_ENTITY_H (polyline_pface, POLYLINE_PFACE, seqend);
}

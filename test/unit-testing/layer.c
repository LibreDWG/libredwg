#define DWG_TYPE DWG_TYPE_LAYER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS flag;
  BITCODE_TV name;
  BITCODE_RS used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;

  BITCODE_B frozen;
  BITCODE_B on;
  BITCODE_B frozen_in_new;
  BITCODE_B locked;
  BITCODE_B plotflag;
  BITCODE_RC linewt;
  BITCODE_CMC color;
  //short      color_rs;    /* preR13, needs to be signed */
  BITCODE_RS ltype_rs;    /* preR13 */
  BITCODE_H plotstyle;
  BITCODE_H material;
  BITCODE_H ltype;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_layer *layer = dwg_object_to_LAYER (obj);

  CHK_ENTITY_TYPE (layer, LAYER, flag, BS);
  CHK_ENTITY_UTF8TEXT_W_OBJ (layer, LAYER, name, name);
  CHK_ENTITY_TYPE (layer, LAYER, used, RS);
  CHK_ENTITY_TYPE (layer, LAYER, is_xref_ref, B);
  CHK_ENTITY_TYPE (layer, LAYER, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (layer, LAYER, is_xref_dep, B);
  CHK_ENTITY_H (layer, LAYER, xref, xref);

  CHK_ENTITY_TYPE (layer, LAYER, frozen, B);
  CHK_ENTITY_TYPE (layer, LAYER, on, B);
  CHK_ENTITY_TYPE (layer, LAYER, frozen_in_new, B);
  CHK_ENTITY_TYPE (layer, LAYER, locked, B);
  CHK_ENTITY_TYPE (layer, LAYER, plotflag, B);
  CHK_ENTITY_TYPE (layer, LAYER, linewt, RC);
  CHK_ENTITY_CMC (layer, LAYER, color, color);
  CHK_ENTITY_TYPE (layer, LAYER, ltype_rs, RS);
  CHK_ENTITY_H (layer, LAYER, plotstyle, plotstyle);
  CHK_ENTITY_H (layer, LAYER, material, material);
  CHK_ENTITY_H (layer, LAYER, ltype, ltype);
}

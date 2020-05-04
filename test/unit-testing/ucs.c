#define DWG_TYPE DWG_TYPE_UCS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RS flag;
  BITCODE_T name;
  BITCODE_RS used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;
  BITCODE_3BD origin;
  BITCODE_3BD x_direction;
  BITCODE_3BD y_direction;
  BITCODE_BD elevation;
  BITCODE_BS orthographic_view_type;
  BITCODE_BS orthographic_type;
  BITCODE_H base_ucs;  /*! DXF 346 */
  BITCODE_H named_ucs; /*! DXF 345 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_ucs *_obj = dwg_object_to_UCS (obj);

  CHK_ENTITY_TYPE (_obj, UCS, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT (_obj, UCS, name, name);
  CHK_ENTITY_TYPE (_obj, UCS, used, RS, used);
  CHK_ENTITY_TYPE (_obj, UCS, is_xref_ref, B, is_xref_ref);
  CHK_ENTITY_TYPE (_obj, UCS, is_xref_resolved, BS, is_xref_resolved);
  CHK_ENTITY_TYPE (_obj, UCS, is_xref_dep, B, is_xref_dep);
  CHK_ENTITY_H (_obj, UCS, xref, xref);

  CHK_ENTITY_3RD (_obj, UCS, origin, origin);
  CHK_ENTITY_3RD (_obj, UCS, x_direction, x_direction);
  CHK_ENTITY_3RD (_obj, UCS, y_direction, y_direction);
  CHK_ENTITY_TYPE (_obj, UCS, elevation, BD, elevation);
  CHK_ENTITY_TYPE (_obj, UCS, orthographic_view_type, BS, orthographic_view_type);
  CHK_ENTITY_TYPE (_obj, UCS, orthographic_type, BS, orthographic_type);
  CHK_ENTITY_H (_obj, UCS, base_ucs, base_ucs);
  CHK_ENTITY_H (_obj, UCS, named_ucs, named_ucs);
}

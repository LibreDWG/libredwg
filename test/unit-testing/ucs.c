#define DWG_TYPE DWG_TYPE_UCS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_T name;
  BITCODE_RSd used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;
  BITCODE_3BD ucsorg;
  BITCODE_3BD ucsxdir;
  BITCODE_3BD ucsydir;
  BITCODE_BD ucs_elevation;
  BITCODE_BS UCSORTHOVIEW;
  BITCODE_BS orthographic_type;
  BITCODE_H base_ucs;
  BITCODE_H named_ucs;
  BITCODE_BS num_orthopts, i;
  Dwg_UCS_orthopts *orthopts;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_ucs *_obj = dwg_object_to_UCS (obj);

  CHK_ENTITY_TYPE (_obj, UCS, flag, RC);
  CHK_ENTITY_UTF8TEXT (_obj, UCS, name);
  CHK_ENTITY_TYPE (_obj, UCS, used, RSd);
  CHK_ENTITY_TYPE (_obj, UCS, is_xref_ref, B);
  CHK_ENTITY_TYPE (_obj, UCS, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (_obj, UCS, is_xref_dep, B);
  CHK_ENTITY_H (_obj, UCS, xref);

  CHK_ENTITY_3RD (_obj, UCS, ucsorg);
  CHK_ENTITY_3RD (_obj, UCS, ucsxdir);
  CHK_ENTITY_3RD (_obj, UCS, ucsydir);
  CHK_ENTITY_TYPE (_obj, UCS, ucs_elevation, BD);
  CHK_ENTITY_TYPE (_obj, UCS, UCSORTHOVIEW, BS);
  CHK_ENTITY_H (_obj, UCS, base_ucs);
  CHK_ENTITY_H (_obj, UCS, named_ucs);
  CHK_ENTITY_TYPE (_obj, UCS, num_orthopts, BS);
  if (!dwg_dynapi_entity_value (_obj, "UCS", "orthopts", &orthopts, NULL))
    fail ("UCS.orthopts");
  else
    for (i = 0; i < num_orthopts; i++)
      {
        CHK_SUBCLASS_TYPE (orthopts[i], UCS_orthopts, type, BS);
        CHK_SUBCLASS_3RD (orthopts[i], UCS_orthopts, pt);
      }
}

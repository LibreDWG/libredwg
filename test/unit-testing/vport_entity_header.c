// unstable. Called VXTable
#define DWG_TYPE DWG_TYPE_VPORT_ENTITY_HEADER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_T name;
  BITCODE_RS used;
  BITCODE_B is_xref_ref;
  BITCODE_BS is_xref_resolved;
  BITCODE_B is_xref_dep;
  BITCODE_H xref;

  BITCODE_B is_on;
  BITCODE_H viewport;
  BITCODE_H prev_entry;

  //Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_vport_entity_header *_obj = dwg_object_to_VPORT_ENTITY_HEADER (obj);

  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, flag, RC);
  CHK_ENTITY_UTF8TEXT (_obj, VPORT_ENTITY_HEADER, name);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, used, RS);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, is_xref_ref, B);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, is_xref_dep, B);
  CHK_ENTITY_H (_obj, VPORT_ENTITY_HEADER, xref);

  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, is_on, B);
  CHK_ENTITY_H (_obj, VPORT_ENTITY_HEADER, viewport);
  CHK_ENTITY_H (_obj, VPORT_ENTITY_HEADER, prev_entry);
}

// unstable.
#define DWG_TYPE DWG_TYPE_VX_TABLE_RECORD
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

  BITCODE_B is_on;
  BITCODE_H viewport;
  BITCODE_H prev_entry;

  // Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_vx_table_record *_obj = dwg_object_to_VX_TABLE_RECORD (obj);

  CHK_ENTITY_TYPE (_obj, VX_TABLE_RECORD, flag, RC);
  CHK_ENTITY_UTF8TEXT (_obj, VX_TABLE_RECORD, name);
  CHK_ENTITY_TYPE (_obj, VX_TABLE_RECORD, used, RSd);
  CHK_ENTITY_TYPE (_obj, VX_TABLE_RECORD, is_xref_ref, B);
  CHK_ENTITY_TYPE (_obj, VX_TABLE_RECORD, is_xref_resolved, BS);
  CHK_ENTITY_TYPE (_obj, VX_TABLE_RECORD, is_xref_dep, B);
  CHK_ENTITY_H (_obj, VX_TABLE_RECORD, xref);

  CHK_ENTITY_TYPE (_obj, VX_TABLE_RECORD, is_on, B);
  CHK_ENTITY_H (_obj, VX_TABLE_RECORD, viewport);
  CHK_ENTITY_H (_obj, VX_TABLE_RECORD, prev_entry);
}

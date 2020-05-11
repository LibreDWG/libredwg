#define DWG_TYPE DWG_TYPE_APPID
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
  BITCODE_RC unknown;

  dwg_obj_appid *appid = dwg_object_to_APPID (obj);

  CHK_ENTITY_TYPE (appid, APPID, flag, RC, flag);
  // the api variant returns a mask: appid->flag & 1 || appid->is_xref_ref >> 6 || appid->is_xref_dep >> 4
  CHK_ENTITY_UTF8TEXT_W_OBJ (appid, APPID, name, name);
  CHK_ENTITY_TYPE (appid, APPID, used, RS, used);
  CHK_ENTITY_TYPE (appid, APPID, is_xref_ref, B, is_xref_ref);
  CHK_ENTITY_TYPE (appid, APPID, is_xref_resolved, BS, is_xref_resolved);
  CHK_ENTITY_TYPE (appid, APPID, is_xref_dep, B, is_xref_dep);
  CHK_ENTITY_H (appid, APPID, xref, xref);

  CHK_ENTITY_TYPE (appid, APPID, unknown, RC, unknown);
}

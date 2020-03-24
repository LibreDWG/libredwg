#define DWG_TYPE DWG_TYPE_APPID
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_RC flag;
  BITCODE_TV name;
  BITCODE_RS used;
  BITCODE_B xrefref;
  BITCODE_BS xrefindex_plus1;
  BITCODE_B xrefdep;
  BITCODE_RC unknown;
  BITCODE_H null_handle;

  dwg_obj_appid *appid = dwg_object_to_APPID (obj);

  CHK_ENTITY_TYPE (appid, APPID, flag, RC, flag);
  // the api variant returns a mask: appid->flag & 1 || appid->xrefref >> 6 || appid->xrefdep >> 4
  CHK_ENTITY_UTF8TEXT_W_OBJ (appid, APPID, name, name);
  CHK_ENTITY_TYPE (appid, APPID, used, RS, used);
  CHK_ENTITY_TYPE (appid, APPID, xrefref, B, xrefref);
  CHK_ENTITY_TYPE (appid, APPID, xrefindex_plus1, BS, xrefindex_plus1);
  CHK_ENTITY_TYPE (appid, APPID, xrefdep, B, xrefdep);

  CHK_ENTITY_TYPE (appid, APPID, unknown, RC, unknown);
  CHK_ENTITY_H (appid, APPID, null_handle, null_handle);
}

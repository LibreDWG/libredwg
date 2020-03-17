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

  dwg_obj_appid *_obj = dwg_object_to_APPID (obj);

  CHK_ENTITY_TYPE (_obj, APPID, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT (_obj, APPID, name, name);
  CHK_ENTITY_TYPE (_obj, APPID, used, RS, used);
  CHK_ENTITY_TYPE (_obj, APPID, xrefref, B, xrefref);
  CHK_ENTITY_TYPE (_obj, APPID, xrefindex_plus1, BS, xrefindex_plus1);
  CHK_ENTITY_TYPE (_obj, APPID, xrefdep, B, xrefdep);

  CHK_ENTITY_TYPE (_obj, APPID, unknown, RC, unknown);
  CHK_ENTITY_H (_obj, APPID, null_handle, null_handle);
}

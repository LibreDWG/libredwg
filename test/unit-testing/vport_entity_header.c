// unstable. there might be more viewport H*
#define DWG_TYPE DWG_TYPE_VPORT_ENTITY_HEADER
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
  BITCODE_B flag1;
  BITCODE_H extref;
  BITCODE_H viewport;

  //Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_vport_entity_header *_obj = dwg_object_to_VPORT_ENTITY_HEADER (obj);

  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, flag, RC, flag);
  CHK_ENTITY_UTF8TEXT (_obj, VPORT_ENTITY_HEADER, name, name);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, used, RS, used);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, xrefref, B, xrefref);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, xrefindex_plus1, BS, xrefindex_plus1);
  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, xrefdep, B, xrefdep);

  CHK_ENTITY_TYPE (_obj, VPORT_ENTITY_HEADER, flag1, B, flag1);
  CHK_ENTITY_H (_obj, VPORT_ENTITY_HEADER, extref, extref);
  CHK_ENTITY_H (_obj, VPORT_ENTITY_HEADER, viewport, viewport);
}

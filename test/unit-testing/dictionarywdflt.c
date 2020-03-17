#define DWG_TYPE DWG_TYPE_DICTIONARYWDFLT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL numitems;
  BITCODE_BS cloning;
  BITCODE_RC hard_owner;
  BITCODE_TV* texts;
  BITCODE_H* itemhandles;
  BITCODE_RL cloning_r14;
  BITCODE_H defaultid;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_dictionarywdflt *_obj = dwg_object_to_DICTIONARYWDFLT (obj);

  CHK_ENTITY_TYPE (_obj, DICTIONARYWDFLT, numitems, BL, numitems);
}

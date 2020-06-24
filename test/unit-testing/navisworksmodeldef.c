#define DWG_TYPE DWG_TYPE_NAVISWORKSMODELDEF
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T path;
  BITCODE_BS flags;
  BITCODE_B status;
  BITCODE_3BD min_extent;
  BITCODE_3BD max_extent;
  BITCODE_B host_drawing_visibility;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_navisworksmodeldef *_obj = dwg_object_to_NAVISWORKSMODELDEF (obj);

  CHK_ENTITY_TYPE (_obj, NAVISWORKSMODELDEF, flags, BS);
  CHK_ENTITY_UTF8TEXT (_obj, NAVISWORKSMODELDEF, path);
  CHK_ENTITY_TYPE (_obj, NAVISWORKSMODELDEF, status, B);
  CHK_ENTITY_3RD (_obj, NAVISWORKSMODELDEF, min_extent);
  CHK_ENTITY_3RD (_obj, NAVISWORKSMODELDEF, max_extent);
  CHK_ENTITY_TYPE (_obj, NAVISWORKSMODELDEF, host_drawing_visibility, B);
#endif
}

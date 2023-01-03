// TODO debugging
#define DWG_TYPE DWG_TYPE_ASSOC3POINTANGULARDIMACTIONBODY
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  ASSOCANNOTATIONACTIONBODY_fields;
  Dwg_ASSOCPARAMBASEDACTIONBODY pab;
  BITCODE_H *deps;
  BITCODE_BS class_version; /*!< DXF 90  */
  BITCODE_H r_node;         /*!< DXF 330  */
  BITCODE_H d_node;         /*!< DXF 330  */
  BITCODE_H assocdep;       /*!< DXF 330  */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_assoc3pointangulardimactionbody *_obj
      = dwg_object_to_ASSOC3POINTANGULARDIMACTIONBODY (obj);

  // ASSOCANNOTATIONACTIONBODY
  CHK_ENTITY_TYPE (_obj, ASSOC3POINTANGULARDIMACTIONBODY, aaab_version, BS);
  CHK_ENTITY_MAX (_obj, ASSOC3POINTANGULARDIMACTIONBODY, aaab_version, BS, 2);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, assoc_dep);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, actionbody);

  CHK_ASSOCPARAMBASEDACTIONBODY (ASSOC3POINTANGULARDIMACTIONBODY);

  CHK_ENTITY_TYPE (_obj, ASSOC3POINTANGULARDIMACTIONBODY, class_version, BS);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, r_node);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, d_node);
  CHK_ENTITY_H (_obj, ASSOC3POINTANGULARDIMACTIONBODY, assocdep);
#endif
}

#define DWG_TYPE DWG_TYPE_SECTION_MANAGER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_B is_live;
  BITCODE_BS num_sections;
  BITCODE_H *sections;

  dwg_obj_section_manager *_obj = dwg_object_to_SECTION_MANAGER (obj);

  CHK_ENTITY_TYPE (_obj, SECTION_MANAGER, is_live, B);
  CHK_ENTITY_TYPE (_obj, SECTION_MANAGER, num_sections, BS);
  CHK_ENTITY_MAX (_obj, SECTION_MANAGER, num_sections, BS, 2000);
  CHK_ENTITY_HV (_obj, SECTION_MANAGER, sections, num_sections);
}

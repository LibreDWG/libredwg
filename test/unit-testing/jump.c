// r10-r11 only
#define DWG_TYPE DWG_TYPE_JUMP
#include "common.c"

void
api_process (dwg_object *obj)
{
#ifdef USE_WRITE
  int error;
  BITCODE_RL jump_address_raw;
  BITCODE_RL jump_address;
  unsigned jump_entity_section;
  dwg_ent_jump *jump = dwg_object_to_JUMP (obj);

  CHK_ENTITY_TYPE (jump, JUMP, jump_address_raw, RLx);
  CHK_ENTITY_TYPE (jump, JUMP, jump_address, RLx);
  CHK_ENTITY_TYPE (jump, JUMP, jump_entity_section, RLx);
#endif
}

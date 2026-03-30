// not in any DWG
#define DWG_TYPE DWG_TYPE_DUMMY
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error;
  dwg_obj_dummy *_obj = dwg_object_to_DUMMY (obj);
}

// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKPOINTPARAMETER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BLOCK1PTPARAMETER_fields;
  BITCODE_T position_name;  /* DXF 303 */
  BITCODE_T position_desc;  /* DXF 304 */
  BITCODE_3BD def_label_pt; /* DXF 1011 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_blockpointparameter *_obj = dwg_object_to_BLOCKPOINTPARAMETER (obj);

#endif
}

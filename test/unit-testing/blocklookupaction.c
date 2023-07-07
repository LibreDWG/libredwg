// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_BLOCKLOOKUPACTION
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BLOCKACTION_fields;
  BITCODE_BL numelems; /* computed */
  BITCODE_BL numrows;  /* DXF 92 */
  BITCODE_BL numcols;  /* DXF 93 */
  Dwg_BLOCKLOOKUPACTION_lut *lut;
  BITCODE_T *exprs;
  BITCODE_B b280;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_blocklookupaction *_obj = dwg_object_to_BLOCKLOOKUPACTION (obj);

  // AcDbBlockAction
  CHK_ENTITY_3RD (_obj, BLOCKLOOKUPACTION, display_location);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, num_actions, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, BLOCKLOOKUPACTION, actions, num_actions, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, num_deps, BL);
  CHK_ENTITY_HV (_obj, BLOCKLOOKUPACTION, deps, num_deps);
  // AcDbBlockLookupAction
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, numrows, BL);
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, numcols, BL);
  if (!_obj->lut)
    {
      fail ("BLOCKLOOKUPACTION.lut[]: NULL");
      return;
    }
  for (i = 0; i < numrows * numcols; i++)
    {
      for (int j = 0; j < 3; j++)
        {
          CHK_SUBCLASS_TYPE (_obj->lut[i].conn_pts[j],
                             BLOCKACTION_connectionpts, code, BL);
          CHK_SUBCLASS_UTF8TEXT (_obj->lut[i].conn_pts[j],
                                 BLOCKACTION_connectionpts, name);
        }
      CHK_SUBCLASS_TYPE (_obj->lut[i], "BLOCKLOOKUPACTION_lut", b282, BL);
      CHK_SUBCLASS_TYPE (_obj->lut[i], "BLOCKLOOKUPACTION_lut", b281, BL);
    }
  CHK_ENTITY_TYPE (_obj, BLOCKLOOKUPACTION, b280, B);
  // ..
  // #endif
}

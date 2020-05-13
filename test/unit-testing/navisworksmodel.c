// DEBUGGING, no coverage
#define DWG_TYPE DWG_TYPE_NAVISWORKSMODEL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, i;
  BITCODE_H defhandle;
  BITCODE_BD *transmatrix;
  BITCODE_BD unitfactor;

  dwg_ent_navisworksmodel *_obj = dwg_object_to_NAVISWORKSMODEL (obj);

#ifdef DEBUG_CLASSES
  CHK_ENTITY_H (_obj, NAVISWORKSMODEL, defhandle, defhandle);
  CHK_ENTITY_TYPE (_obj, NAVISWORKSMODEL, unitfactor, BD);
  if (!dwg_dynapi_entity_value (_obj, "NAVISWORKSMODEL", "transmatrix", &transmatrix, NULL))
    fail ("NAVISWORKSMODEL.transmatrix");
  for (i = 0; i < 16; i++)
    {
      ok ("NAVISWORKSMODEL.transmatrix[%u]: %f", i, transmatrix[i]);
    }
#endif
}

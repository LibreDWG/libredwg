#define DWG_TYPE DWG_TYPE_LIGHTLIST
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_H dictionary; /* (hard-pointer to ACAD_LIGHT dictionary entry) */
  BITCODE_BS class_version;
  BITCODE_BS i, num_lights;
  BITCODE_H*  lights_handles, *hdls;
  BITCODE_TV* lights_names;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_lightlist *_obj = dwg_object_to_LIGHTLIST (obj);

  CHK_ENTITY_TYPE (_obj, LIGHTLIST, class_version, BL, class_version);
  CHK_ENTITY_H (_obj, LIGHTLIST, dictionary, dictionary);
  CHK_ENTITY_TYPE (_obj, LIGHTLIST, num_lights, BL, num_lights);
  if (!dwg_dynapi_entity_value (_obj, "LIGHTLIST", "lights_handles", &lights_handles, NULL))
    fail ("LIGHTLIST.lights_handles");
  hdls = _obj->lights_handles;
  for (i = 0; i < num_lights; i++)
    {
      ok ("LIGHTLIST.lights_names[%d]: %s", i, _obj->lights_names[i]);
      if (hdls[i] == lights_handles[i])
        ok ("LIGHTLIST.lights_handles[%d]: " FORMAT_REF, i, ARGS_REF (lights_handles[i]));
      else
        fail ("LIGHTLIST.lights_handles[%d]: " FORMAT_REF, i, ARGS_REF (lights_handles[i]));
    }
#endif
}

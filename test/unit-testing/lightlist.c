#define DWG_TYPE DWG_TYPE_LIGHTLIST
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version;
  BITCODE_BS i, num_lights;
  Dwg_LIGHTLIST_light *lights;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_lightlist *_obj = dwg_object_to_LIGHTLIST (obj);

  CHK_ENTITY_TYPE (_obj, LIGHTLIST, class_version, BS);
  CHK_ENTITY_TYPE (_obj, LIGHTLIST, num_lights, BS);
  if (!dwg_dynapi_entity_value (_obj, "LIGHTLIST", "lights", &lights, NULL))
    fail ("LIGHTLIST.lights");
  for (i = 0; i < num_lights; i++)
    {
      CHK_SUBCLASS_UTF8TEXT (lights[i], LIGHTLIST_light, name);
      CHK_SUBCLASS_H (lights[i], LIGHTLIST_light, handle);
    }
}

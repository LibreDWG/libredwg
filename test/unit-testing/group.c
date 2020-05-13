#define DWG_TYPE DWG_TYPE_GROUP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_TV name;
  BITCODE_BS unnamed;
  BITCODE_BS selectable;
  BITCODE_BL i, num_groups;
  BITCODE_H *groups, *hdls;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_group *_obj = dwg_object_to_GROUP (obj);

  CHK_ENTITY_UTF8TEXT (_obj, GROUP, name);
  CHK_ENTITY_TYPE (_obj, GROUP, unnamed, BS);
  CHK_ENTITY_TYPE (_obj, GROUP, selectable, BS);
  CHK_ENTITY_TYPE (_obj, GROUP, num_groups, BL);
  if (!dwg_dynapi_entity_value (_obj, "GROUP", "groups", &groups, NULL))
    fail ("GROUP.groups");
  hdls = _obj->groups;
  for (i = 0; i < num_groups; i++)
    {
      if (hdls[i] == groups[i])
        ok ("GROUP.groups[%d]: " FORMAT_REF, i, ARGS_REF (groups[i]));
      else
        fail ("GROUP.groups[%d]: " FORMAT_REF, i, ARGS_REF (groups[i]));
    }
}

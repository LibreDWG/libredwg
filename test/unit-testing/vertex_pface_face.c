#define DWG_TYPE DWG_TYPE_VERTEX_PFACE_FACE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS vertind[4];

  dwg_ent_vert_pface_face *ent = dwg_object_to_VERTEX_PFACE_FACE (obj);

  if (!dwg_dynapi_entity_value (ent, "VERTEX_PFACE_FACE", "vertind", &vertind,
                                NULL))
    fail ("dwg_dynapi_entity_value");
  else
    for (int i = 0; i < 4; i++)
      {
        if (vertind[i] == ent->vertind[i])
          ok ("vertind[%d]: " FORMAT_BS, i, vertind[i]);
        else
          fail ("vertind[%d] " FORMAT_BS " != ent->vertind[i] " FORMAT_BS, i,
                vertind[i], ent->vertind[i]);
      }
}

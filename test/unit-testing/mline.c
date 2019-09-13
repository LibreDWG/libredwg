#define DWG_TYPE DWG_TYPE_MLINE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double scale;
  BITCODE_RC just;
  BITCODE_RC num_lines;
  BITCODE_BS i, num_verts, flags;
  dwg_point_3d base_point, ext; // 3d_points
  dwg_mline_vertex *verts, *v1;
  BITCODE_H mlinestyle;

  dwg_ent_mline *mline = dwg_object_to_MLINE (obj);

  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, scale, BD, scale);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, justification, RC, just);
  CHK_ENTITY_3RD_W_OLD (mline, MLINE, base_point, base_point);
  CHK_ENTITY_3RD_W_OLD (mline, MLINE, extrusion, ext);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, flags, BS, flags);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, num_lines, RC, num_lines);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, num_verts, BS, num_verts);

  if (!dwg_dynapi_entity_value (mline, "MLINE", "verts", &v1, NULL))
    fail ("MLINE.verts");
  verts = dwg_ent_mline_get_verts (mline, &error);
  if (error)
    fail ("MLINE.verts");
  else
    {
      for (i = 0; i < num_verts; i++)
        {
          ok ("MLINE.vertex[%d]: (%f, %f, %f)", i, verts[i].vertex.x,
              verts[i].vertex.y, verts[i].vertex.z);
          if (memcmp (&v1[i].vertex, &verts[i].vertex,
                      sizeof (verts[i].vertex)))
            fail ("MLINE.verts[%d]", i);
        }
    }
  CHK_ENTITY_H (mline, MLINE, mlinestyle, mlinestyle);
}

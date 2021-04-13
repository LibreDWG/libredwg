#define DWG_TYPE DWG_TYPE_MLINE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  double scale;
  BITCODE_RC justification;
  BITCODE_RC num_lines;
  BITCODE_BS i, num_verts, flags;
  dwg_point_3d base_point, extrusion;
  dwg_mline_vertex *verts, *v1;
  BITCODE_H mlinestyle;

  dwg_ent_mline *mline = dwg_object_to_MLINE (obj);

  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, scale, BD);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, justification, RC);
  CHK_ENTITY_3RD_W_OLD (mline, MLINE, base_point);
  CHK_ENTITY_3RD_W_OLD (mline, MLINE, extrusion);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, flags, BS);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, num_lines, RCd);
  CHK_ENTITY_TYPE_W_OLD (mline, MLINE, num_verts, BS);

  if (!dwg_dynapi_entity_value (mline, "MLINE", "verts", &v1, NULL))
    fail ("MLINE.verts");
#ifdef USE_DEPRECATED_API
  verts = dwg_ent_mline_get_verts (mline, &error);
#else
  verts = (dwg_mline_vertex *)mline->verts;
  error = 0;
#endif
  if (error)
    fail ("MLINE.verts");
  else
    {
      for (i = 0; i < num_verts; i++)
        {
#ifdef USE_DEPRECATED_API
          dwg_mline_line *lines
              = dwg_mline_vertex_get_lines (&verts[i], &error);
#else
          dwg_mline_line *lines = (dwg_mline_line *)verts[i].lines;
#endif
          if (error)
            fail ("MLINE.verts[%d].lines", i);
          else
            {
              for (int j = 0; j < num_lines; j++)
                {
                  for (int k = 0; k < lines[j].num_segparms; k++)
                    {
                      ok ("MLINE.verts[%d].lines[%d].segparms[%d]: %f", i, j,
                          k, lines[j].segparms[k]);
                    }
                  for (int k = 0; k < lines[j].num_areafillparms; k++)
                    {
                      ok ("MLINE.verts[%d].lines[%d].areafillparms[%d]: %f", i,
                          j, k, lines[j].areafillparms[k]);
                    }
                }
            }
#ifdef USE_DEPRECATED_API
          FREE (lines);
#endif

          ok ("MLINE.verts[%d]: (%f, %f, %f)", i, verts[i].vertex.x,
              verts[i].vertex.y, verts[i].vertex.z);
          if (memcmp (&v1[i].vertex, &verts[i].vertex,
                      sizeof (verts[i].vertex)))
            fail ("MLINE.verts[%d]", i);
        }
    }
#ifdef USE_DEPRECATED_API
  FREE (verts);
#endif

  CHK_ENTITY_H (mline, MLINE, mlinestyle);
}

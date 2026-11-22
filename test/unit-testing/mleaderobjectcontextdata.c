// unstable
#define DWG_TYPE DWG_TYPE_MLEADEROBJECTCONTEXTDATA
#include "tests_common.c"

void
api_process (dwg_object *obj)
{
  int error;
  ANNOTSCALEOBJECTCONTEXTDATA_fields;
  Dwg_MLEADER_AnnotContext ctx;
  BITCODE_BL i;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_mleaderobjectcontextdata *_obj
      = dwg_object_to_MLEADEROBJECTCONTEXTDATA (obj);

  CHK_ENTITY_TYPE (_obj, MLEADEROBJECTCONTEXTDATA, class_version, BS);
  CHK_ENTITY_TYPE (_obj, MLEADEROBJECTCONTEXTDATA, is_default, B);
  CHK_ENTITY_H (_obj, MLEADEROBJECTCONTEXTDATA, scale);
  if (!dwg_dynapi_entity_value (_obj, "MLEADEROBJECTCONTEXTDATA", "ctx", &ctx,
                                NULL))
    fail ("MLEADEROBJECTCONTEXTDATA.ctx");
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, num_leaders, BL);
  if (ctx.num_leaders && !ctx.leaders)
    fail ("!MLEADEROBJECTCONTEXTDATA.ctx.leaders but ctx.num_leaders");
  for (i = 0; ctx.leaders && i < ctx.num_leaders; i++)
    {
      CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, has_lastleaderlinepoint,
                         B);
      CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, has_dogleg, B);
      CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, branch_index, BL);
      CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, dogleg_length, BD);
      CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, num_lines, BL);
    }
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, scale_factor, BD);
  CHK_SUBCLASS_3RD (ctx, MLEADER_AnnotContext, content_base);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_height, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, arrow_size, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, landing_gap, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, has_content_txt, B);
  CHK_SUBCLASS_3RD (ctx, MLEADER_AnnotContext, base);
  CHK_SUBCLASS_3RD (ctx, MLEADER_AnnotContext, base_dir);
  CHK_SUBCLASS_3RD (ctx, MLEADER_AnnotContext, base_vert);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, is_normal_reversed, B);
}

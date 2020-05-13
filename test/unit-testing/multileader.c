#define DWG_TYPE DWG_TYPE_MULTILEADER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  BITCODE_BS class_version;
  Dwg_MLEADER_AnnotContext ctx;
  BITCODE_H mleaderstyle;
  BITCODE_BL flags;
  BITCODE_BS type;
  BITCODE_CMC color;
  BITCODE_H ltype;
  BITCODE_BLd linewt;
  BITCODE_B has_landing;
  BITCODE_B has_dogleg;
  BITCODE_BD landing_dist;
  BITCODE_H arrow_handle;
  BITCODE_BD arrow_size;
  BITCODE_BS style_content;
  BITCODE_H text_style;
  BITCODE_BS text_left;
  BITCODE_BS text_right;
  BITCODE_BS text_angletype;
  BITCODE_BS text_alignment;
  BITCODE_CMC text_color;
  BITCODE_B has_text_frame;
  BITCODE_H block_style;
  BITCODE_CMC block_color;
  dwg_point_3d block_scale;
  BITCODE_BD block_rotation;
  BITCODE_BS style_attachment;
  BITCODE_B is_annotative;
  /* until r2007: */
  BITCODE_BL num_arrowheads;
  Dwg_LEADER_ArrowHead *arrowheads;
  BITCODE_BL num_blocklabels;
  Dwg_LEADER_BlockLabel *blocklabels;
  BITCODE_B neg_textdir;
  BITCODE_BS ipe_alignment;
  BITCODE_BS justification;
  BITCODE_BD scale_factor;
  BITCODE_BS attach_dir;
  BITCODE_BS attach_top;
  BITCODE_BS attach_bottom;
  BITCODE_B text_extended;
  BITCODE_3RD *points;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_mleader *mleader = dwg_object_to_MULTILEADER (obj);

  CHK_ENTITY_TYPE (mleader, MULTILEADER, class_version, BS);
  if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "ctx", &ctx, NULL))
    fail ("MULTILEADER.ctx");
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, num_leaders, BL);
  if (!dwg_dynapi_subclass_value (&ctx, "MLEADER_AnnotContext", "leaders", &ctx.leaders, NULL))
    fail ("MULTILEADER.ctx.leaders");
  if (ctx.num_leaders)
    {
      if (!ctx.leaders)
        fail ("!MULTILEADER.ctx.leaders but ctx.num_leaders");
      else
        for (i = 0; i < ctx.num_leaders; i++)
          {
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, has_lastleaderlinepoint, B);
            if (ctx.leaders[i].has_lastleaderlinepoint)
              CHK_SUBCLASS_3RD (ctx.leaders[i], LEADER_Node, lastleaderlinepoint);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, has_dogleg, B);
            if (ctx.leaders[i].has_dogleg)
              CHK_SUBCLASS_3RD (ctx.leaders[i], LEADER_Node, dogleg_vector);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, num_breaks, BL);
            for (BITCODE_BL j = 0; j < ctx.leaders[i].num_breaks; j++)
              {
                CHK_SUBCLASS_3RD (ctx.leaders[i].breaks[j], LEADER_Break, start);
                CHK_SUBCLASS_3RD (ctx.leaders[i].breaks[j], LEADER_Break, end);
              }
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, branch_index, BL);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, dogleg_length, BD);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, num_lines, BL);
            for (BITCODE_BL j = 0; j < ctx.leaders[i].num_lines; j++)
              {
                CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line, num_points, BL);
                if (!dwg_dynapi_subclass_value (&ctx.leaders[i].lines[j], "LEADER_Line", "points",
                                                &points, NULL))
                  fail ("LEADER_Line.points");
                else
                  for (BITCODE_BL k = 0; k < ctx.leaders[i].lines[j].num_points; k++)
                    {
                      ok ("LEADER_Line.points[%d]: (%f, %f, %f)", k,
                          points[k].x, points[k].y, points[k].z);
                    }
                CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                   num_breaks, BL);
                for (BITCODE_BL k = 0; k < ctx.leaders[i].lines[j].num_breaks; k++)
                  {
                    CHK_SUBCLASS_3RD (ctx.leaders[i].lines[j].breaks[k], LEADER_Break, start);
                    CHK_SUBCLASS_3RD (ctx.leaders[i].lines[j].breaks[k], LEADER_Break, end);
                  }
                CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line, line_index, BL);
                if (dwg_version >= R_2010)
                  {
                    type = ctx.leaders[i].lines[j].type;
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line, type, BS);
                    if (type > 2)
                      fail ("Invalid LEADER_Line.type " FORMAT_BS " > 2", type);
                    CHK_SUBCLASS_CMC (ctx.leaders[i].lines[j], LEADER_Line, color);
                    CHK_SUBCLASS_H (ctx.leaders[i].lines[j], LEADER_Line, ltype);
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line, linewt, BLd);
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line, arrow_size, BD);
                    CHK_SUBCLASS_H (ctx.leaders[i].lines[j], LEADER_Line, arrow_handle);
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line, flags, BL);
                    flags = ctx.leaders[i].lines[j].flags;
                    if (flags > 63)
                      fail ("Invalid LEADER_Line.flags " FORMAT_BLx " > 63", flags);
                  }
              }
            if (dwg_version >= R_2010)
              {
                CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, attach_dir, BS);
                attach_dir = ctx.leaders[i].attach_dir;
                if (attach_dir > 1)
                  fail ("Invalid LEADER_Node.attach_dir " FORMAT_BS " > 1", attach_dir);
              }
          }
        }
      else
        if (ctx.leaders)
          fail ("MULTILEADER.ctx.leaders but 0 ctx.num_leaders");
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, scale_factor, BD);
  CHK_SUBCLASS_3RD (ctx, MLEADER_AnnotContext, content_base);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_height, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, arrow_size, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, landing_gap, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_left, BS);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_right, BS);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_angletype, BS);
  CHK_ENTITY_MAX (ctx.content, MLEADER_Content, ctx.text_angletype, BS, 2);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_alignment, BS);
  CHK_ENTITY_MAX (ctx.content, MLEADER_Content, ctx.text_alignment, BS, 1);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, has_content_txt, B);
  if (ctx.has_content_txt)
    {
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.default_text, TV);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, txt.normal);
      CHK_SUBCLASS_H (ctx.content, MLEADER_Content, txt.style);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, txt.location);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, txt.direction);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.rotation, BD);
      CHK_ENTITY_MAX (ctx.content, MLEADER_Content, ctx.content.txt.rotation, BD, MAX_ANGLE);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.width, BD);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.line_spacing_factor, BD);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.line_spacing_style, BS);
      CHK_ENTITY_MAX (ctx.content, MLEADER_Content, ctx.content.txt.line_spacing_style, BS, 2);
      CHK_SUBCLASS_CMC (ctx.content, MLEADER_Content, txt.color);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.alignment, BS);
      CHK_ENTITY_MAX (ctx.content, MLEADER_Content, ctx.content.txt.alignment, BS, 3);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.flow, BS);
      CHK_ENTITY_MAX (ctx.content, MLEADER_Content, ctx.content.txt.flow, BS, 6);
      CHK_SUBCLASS_CMC (ctx.content, MLEADER_Content, txt.bg_color);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.bg_scale, BD); // FIXME! r2000
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.bg_transparency, BL);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_bg_fill, B);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_bg_mask_fill, B);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.col_type, BS);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_height_auto, B);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.col_width, BD);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.col_gutter, BD);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_col_flow_reversed, B);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.num_col_sizes, BL);
      for (i = 0; i < ctx.content.txt.num_col_sizes; i++)
        {
          ok ("MULTILEADER.ctx.content.txt.col_sizes[%d]: %f", i, ctx.content.txt.col_sizes[i]);
        }
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.word_break, B);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.unknown, B);
    }
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, has_content_blk, B);
  if (ctx.has_content_blk)
    {
      if (ctx.has_content_txt)
        fail ("ctx.has_content_blk && ctx.has_content_txt");
      CHK_SUBCLASS_H (ctx.content, MLEADER_Content, blk.block_table);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, blk.normal);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, blk.location);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, blk.scale);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, blk.rotation, BD);
      CHK_ENTITY_MAX (ctx.content, MLEADER_Content, ctx.content.blk.rotation, BD, MAX_ANGLE);
      CHK_SUBCLASS_CMC (ctx.content, MLEADER_Content, blk.color);
      ok ("MULTILEADER.ctx.content.txt.transform:");
      for (i = 0; i < 16; i++)
        {
          printf (" %f", ctx.content.blk.transform[i]);
        }
      printf ("\n");
    }
  else
    if (!ctx.has_content_txt)
      fail ("!ctx.has_content_blk && !ctx.has_content_txt");

  CHK_ENTITY_H (mleader, MULTILEADER, mleaderstyle);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, flags, BL);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, type, BS);
  CHK_ENTITY_CMC (mleader, MULTILEADER, color);
  CHK_ENTITY_H (mleader, MULTILEADER, ltype);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, linewt, BLd, linewt);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, has_landing, B);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, has_dogleg, B);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, landing_dist, BD);
  CHK_ENTITY_H (mleader, MULTILEADER, arrow_handle);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, arrow_size, BD);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, style_content, BS);
  CHK_ENTITY_H (mleader, MULTILEADER, text_style);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, text_left, BS);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, text_right, BS);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, text_angletype, BS);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, text_alignment, BS);
  CHK_ENTITY_CMC (mleader, MULTILEADER, text_color);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, has_text_frame, B);
  CHK_ENTITY_H (mleader, MULTILEADER, block_style);
  CHK_ENTITY_CMC (mleader, MULTILEADER, block_color);
  CHK_ENTITY_3RD (mleader, MULTILEADER, block_scale);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, block_rotation, BD);
  CHK_ENTITY_MAX (mleader, MULTILEADER, block_rotation, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, style_attachment, BS);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, is_annotative, B);

  if (dwg_version >= R_2000 && dwg_version <= R_2007)
    {
      // TODO no coverage
      CHK_ENTITY_TYPE (mleader, MULTILEADER, num_arrowheads, BL);
      if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "arrowheads", &arrowheads, NULL))
        fail ("MULTILEADER.arrowheads");
      if (num_arrowheads)
        {
          if (!arrowheads)
            fail ("!MULTILEADER.arrowheads but num_arrowheads");
          else
            for (i = 0; i < num_arrowheads; i++)
              {
                ok ("MULTILEADER.arrowheads[%d].is_default: " FORMAT_B, i,
                    arrowheads[i].is_default);
                ok ("MULTILEADER.arrowheads[%d].arrowhead: " FORMAT_REF, i,
                    ARGS_REF (arrowheads[i].arrowhead));
              }
        }
      else
        if (arrowheads)
          fail ("MULTILEADER.arrowheads but 0 num_arrowheads");

      // TODO no coverage
      CHK_ENTITY_TYPE (mleader, MULTILEADER, num_blocklabels, BL);
      if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "blocklabels", &blocklabels, NULL))
        fail ("MULTILEADER.blocklabels");
      if (num_blocklabels)
        {
          if (!blocklabels)
            fail ("!MULTILEADER.blocklabels but num_blocklabels");
          else
            for (i = 0; i < num_blocklabels; i++)
              {
                ok ("MULTILEADER.blocklabels[%d].attdef: " FORMAT_REF, i,
                    ARGS_REF (blocklabels[i].attdef));
                ok ("MULTILEADER.blocklabels[%d].label_text: %s", i,
                    blocklabels[i].label_text);
                ok ("MULTILEADER.blocklabels[%d].ui_index: " FORMAT_BS, i,
                    blocklabels[i].ui_index);
                ok ("MULTILEADER.blocklabels[%d].width: " FORMAT_BD, i,
                    blocklabels[i].width);
              }
        }
      else
        if (blocklabels)
          fail ("MULTILEADER.blocklabels but 0 num_blocklabels");

      CHK_ENTITY_TYPE (mleader, MULTILEADER, neg_textdir, B);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, ipe_alignment, BS);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, justification, BS);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, scale_factor, BD);
    }
  if (dwg_version >= R_2010)
    {
      attach_dir = mleader->attach_dir;
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_dir, BS);
      CHK_ENTITY_MAX (mleader, MULTILEADER, attach_dir, BS, 1);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_top, BS);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_bottom, BS);
    }
  if (dwg_version >= R_2013)
    {
      CHK_ENTITY_TYPE (mleader, MULTILEADER, text_extended, B);
    }
}

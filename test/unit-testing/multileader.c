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
  BITCODE_CMC line_color;
  BITCODE_H line_ltype;
  BITCODE_BLd line_linewt;
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
  BITCODE_B is_neg_textdir;
  BITCODE_BS ipe_alignment;
  BITCODE_BS justification;
  BITCODE_BD scale_factor;
  BITCODE_BS attach_dir;
  BITCODE_BS attach_top;
  BITCODE_BS attach_bottom;
  BITCODE_B is_text_extended;
  BITCODE_3RD *points;
  BITCODE_BD *transform;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_mleader *mleader = dwg_object_to_MULTILEADER (obj);

  CHK_ENTITY_TYPE (mleader, MULTILEADER, class_version, BS);
  if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "ctx", &ctx, NULL))
    fail ("MULTILEADER.ctx");
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, num_leaders, BL);
  if (!dwg_dynapi_subclass_value (&ctx, "MLEADER_AnnotContext", "leaders",
                                  &ctx.leaders, NULL))
    fail ("MULTILEADER.ctx.leaders");
  if (ctx.num_leaders)
    {
      if (!ctx.leaders)
        fail ("!MULTILEADER.ctx.leaders but ctx.num_leaders");
      else
        for (i = 0; i < ctx.num_leaders; i++)
          {
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node,
                               has_lastleaderlinepoint, B);
            if (ctx.leaders[i].has_lastleaderlinepoint)
              CHK_SUBCLASS_3RD (ctx.leaders[i], LEADER_Node,
                                lastleaderlinepoint);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, has_dogleg, B);
            if (ctx.leaders[i].has_dogleg)
              CHK_SUBCLASS_3RD (ctx.leaders[i], LEADER_Node, dogleg_vector);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, num_breaks, BL);
            for (BITCODE_BL j = 0; j < ctx.leaders[i].num_breaks; j++)
              {
                CHK_SUBCLASS_3RD (ctx.leaders[i].breaks[j], LEADER_Break,
                                  start);
                CHK_SUBCLASS_3RD (ctx.leaders[i].breaks[j], LEADER_Break, end);
              }
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, branch_index, BL);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, dogleg_length, BD);
            CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, num_lines, BL);
            for (BITCODE_BL j = 0; j < ctx.leaders[i].num_lines; j++)
              {
                BITCODE_BL num_points = ctx.leaders[i].lines[j].num_points;
                CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                   num_points, BL);
                CHK_SUBCLASS_3DPOINTS (ctx.leaders[i].lines[j], LEADER_Line,
                                       points, num_points);
                CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                   num_breaks, BL);
                for (BITCODE_BL k = 0; k < ctx.leaders[i].lines[j].num_breaks;
                     k++)
                  {
                    CHK_SUBCLASS_3RD (ctx.leaders[i].lines[j].breaks[k],
                                      LEADER_Break, start);
                    CHK_SUBCLASS_3RD (ctx.leaders[i].lines[j].breaks[k],
                                      LEADER_Break, end);
                  }
                CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                   line_index, BL);
                if (dwg_version >= R_2010)
                  {
                    type = ctx.leaders[i].lines[j].type;
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                       type, BS);
                    CHK_SUBCLASS_MAX (ctx.leaders[i].lines[j], LEADER_Line,
                                      type, BS, 3);
                    CHK_SUBCLASS_CMC (ctx.leaders[i].lines[j], LEADER_Line,
                                      color);
                    CHK_SUBCLASS_H (ctx.leaders[i].lines[j], LEADER_Line,
                                    ltype);
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                       linewt, BLd);
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                       arrow_size, BD);
                    CHK_SUBCLASS_H (ctx.leaders[i].lines[j], LEADER_Line,
                                    arrow_handle);
                    CHK_SUBCLASS_TYPE (ctx.leaders[i].lines[j], LEADER_Line,
                                       flags, BL);
                    CHK_SUBCLASS_MAX (ctx.leaders[i].lines[j], LEADER_Line,
                                      flags, BL, 63);
                  }
              }
            if (dwg_version >= R_2010)
              {
                CHK_SUBCLASS_TYPE (ctx.leaders[i], LEADER_Node, attach_dir,
                                   BS);
                CHK_SUBCLASS_MAX (ctx.leaders[i], LEADER_Node, attach_dir, BS,
                                  1);
              }
          }
    }
  else if (ctx.leaders)
    fail ("MULTILEADER.ctx.leaders but 0 ctx.num_leaders");
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, scale_factor, BD);
  CHK_SUBCLASS_3RD (ctx, MLEADER_AnnotContext, content_base);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_height, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, arrow_size, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, landing_gap, BD);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_left, BS);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_right, BS);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_angletype, BS);
  CHK_SUBCLASS_MAX (ctx, MLEADER_AnnotContext, text_angletype, BS, 2);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_alignment, BS);
  CHK_SUBCLASS_MAX (ctx, MLEADER_AnnotContext, text_alignment, BS, 1);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, has_content_txt, B);
  if (ctx.has_content_txt)
    {
      CHK_SUBCLASS_UTF8TEXT (ctx.content.txt, MLEADER_Content_MText,
                             default_text);
      CHK_SUBCLASS_3RD (ctx.content.txt, MLEADER_Content_MText, normal);
      CHK_SUBCLASS_H (ctx.content.txt, MLEADER_Content_MText, style);
      CHK_SUBCLASS_3RD (ctx.content.txt, MLEADER_Content_MText, location);
      CHK_SUBCLASS_3RD (ctx.content.txt, MLEADER_Content_MText, direction);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, rotation, BD);
      CHK_SUBCLASS_MAX (ctx.content.txt, MLEADER_Content_MText, rotation, BD,
                        MAX_ANGLE);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, width, BD);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText,
                         line_spacing_factor, BD);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText,
                         line_spacing_style, BS);
      CHK_SUBCLASS_MAX (ctx.content.txt, MLEADER_Content_MText,
                        line_spacing_style, BS, 2);
      CHK_SUBCLASS_CMC (ctx.content.txt, MLEADER_Content_MText, color);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, alignment,
                         BS);
      CHK_SUBCLASS_MAX (ctx.content.txt, MLEADER_Content_MText, alignment, BS,
                        3);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, flow, BS);
      CHK_SUBCLASS_MAX (ctx.content.txt, MLEADER_Content_MText, flow, BS, 6);
      CHK_SUBCLASS_CMC (ctx.content.txt, MLEADER_Content_MText, bg_color);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, bg_scale,
                         BD); // FIXME! r2000
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText,
                         bg_transparency, BL);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, is_bg_fill,
                         B);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText,
                         is_bg_mask_fill, B);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, col_type, BS);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText,
                         is_height_auto, B);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, col_width,
                         BD);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, col_gutter,
                         BD);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText,
                         is_col_flow_reversed, B);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, num_col_sizes,
                         BL);
      CHK_SUBCLASS_VECTOR_TYPE (ctx.content.txt, MLEADER_Content_MText,
                                col_sizes, ctx.content.txt.num_col_sizes, BD);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, word_break,
                         B);
      CHK_SUBCLASS_TYPE (ctx.content.txt, MLEADER_Content_MText, unknown, B);
    }
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, has_content_blk, B);
  if (ctx.has_content_blk)
    {
      if (ctx.has_content_txt)
        fail ("ctx.has_content_blk && ctx.has_content_txt");
      CHK_SUBCLASS_H (ctx.content.blk, MLEADER_Content_Block, block_table);
      CHK_SUBCLASS_3RD (ctx.content.blk, MLEADER_Content_Block, normal);
      CHK_SUBCLASS_3RD (ctx.content.blk, MLEADER_Content_Block, location);
      CHK_SUBCLASS_3RD (ctx.content.blk, MLEADER_Content_Block, scale);
      CHK_SUBCLASS_TYPE (ctx.content.blk, MLEADER_Content_Block, rotation, BD);
      CHK_SUBCLASS_MAX (ctx.content.blk, MLEADER_Content_Block, rotation, BD,
                        MAX_ANGLE);
      CHK_SUBCLASS_CMC (ctx.content.blk, MLEADER_Content_Block, color);
      CHK_SUBCLASS_VECTOR_TYPE (ctx.content.blk, MLEADER_Content_Block,
                                transform, 16, BD);
    }
  else if (!ctx.has_content_txt)
    fail ("!ctx.has_content_blk && !ctx.has_content_txt");

  CHK_ENTITY_H (mleader, MULTILEADER, mleaderstyle);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, flags, BL);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, type, BS);
  // CHK_ENTITY_MAX (mleader, MULTILEADER, type, BS, 3);
  CHK_ENTITY_CMC (mleader, MULTILEADER, line_color);
  CHK_ENTITY_H (mleader, MULTILEADER, line_ltype);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, line_linewt, BLd);
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
      if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "arrowheads",
                                    &arrowheads, NULL))
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
      else if (arrowheads)
        fail ("MULTILEADER.arrowheads but 0 num_arrowheads");

      // TODO no coverage
      CHK_ENTITY_TYPE (mleader, MULTILEADER, num_blocklabels, BL);
      if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "blocklabels",
                                    &blocklabels, NULL))
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
      else if (blocklabels)
        fail ("MULTILEADER.blocklabels but 0 num_blocklabels");

      CHK_ENTITY_TYPE (mleader, MULTILEADER, is_neg_textdir, B);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, ipe_alignment, BS);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, justification, BS);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, scale_factor, BD);
    }
  if (dwg_version >= R_2010)
    {
      attach_dir = mleader->attach_dir;
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_dir, BS);
      CHK_ENTITY_MAX (mleader, MULTILEADER, attach_dir, BS, 2);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_top, BS);
      // CHK_ENTITY_MAX (mleader, MULTILEADER, attach_top, BS, 160);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_bottom, BS);
      // CHK_ENTITY_MAX (mleader, MULTILEADER, attach_bottom, BS, 4786);
    }
  if (dwg_version >= R_2013)
    {
      CHK_ENTITY_TYPE (mleader, MULTILEADER, is_text_extended, B);
    }
}

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
  BITCODE_BS attach_type;
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

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_mleader *mleader = dwg_object_to_MULTILEADER (obj);

  CHK_ENTITY_TYPE (mleader, MULTILEADER, class_version, BS, class_version);
  if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "ctx", &ctx, NULL))
    fail ("MULTILEADER.ctx");
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, num_leaders, BL, ctx.num_leaders);
  if (!dwg_dynapi_subclass_value (&ctx, "MLEADER_AnnotContext", "leaders", &ctx.leaders, NULL))
    fail ("MULTILEADER.ctx.leaders");
  if (ctx.num_leaders)
    {
      if (!ctx.leaders)
        fail ("!MULTILEADER.ctx.leaders but ctx.num_leaders");
      else
        for (i = 0; i < ctx.num_leaders; i++)
          {
            ok ("MULTILEADER.ctx.leaders[%d].has_lastleaderlinepoint: " FORMAT_B, i,
                ctx.leaders[i].has_lastleaderlinepoint);
            if (ctx.leaders[i].has_lastleaderlinepoint)
              ok ("MULTILEADER.ctx.leaders[%d].lastleaderlinepoint: (%f, %f, %f)", i,
                  ctx.leaders[i].lastleaderlinepoint.x, ctx.leaders[i].lastleaderlinepoint.y,
                  ctx.leaders[i].lastleaderlinepoint.z);
            ok ("MULTILEADER.ctx.leaders[%d].has_dogleg: " FORMAT_B, i,
                ctx.leaders[i].has_dogleg);
            if (ctx.leaders[i].has_dogleg)
              ok ("MULTILEADER.ctx.leaders[%d].dogleg_vector: (%f, %f, %f)", i,
                  ctx.leaders[i].dogleg_vector.x, ctx.leaders[i].dogleg_vector.y,
                  ctx.leaders[i].dogleg_vector.z);
            ok ("MULTILEADER.ctx.leaders[%d].num_breaks: " FORMAT_BL, i,
                ctx.leaders[i].num_breaks);
            for (BITCODE_BL j = 0; j < ctx.leaders[i].num_breaks; j++)
              {
                ok ("MULTILEADER.ctx.leaders[%d].breaks[%d].start: (%f, %f, "
                    "%f)",
                    i, j, ctx.leaders[i].breaks[j].start.x,
                    ctx.leaders[i].breaks[j].start.y,
                    ctx.leaders[i].breaks[j].start.z);
                ok ("MULTILEADER.ctx.leaders[%d].breaks[%d].end: (%f, %f, "
                    "%f)",
                    i, j, ctx.leaders[i].breaks[j].end.x,
                    ctx.leaders[i].breaks[j].end.y,
                    ctx.leaders[i].breaks[j].end.z);
              }
            ok ("MULTILEADER.ctx.leaders[%d].branch_index: " FORMAT_BL, i,
                ctx.leaders[i].branch_index);
            ok ("MULTILEADER.ctx.leaders[%d].dogleg_length: " FORMAT_BD, i,
                ctx.leaders[i].dogleg_length);
            ok ("MULTILEADER.ctx.leaders[%d].num_lines: " FORMAT_BL, i,
                ctx.leaders[i].num_lines);
            for (BITCODE_BL j = 0; j < ctx.leaders[i].num_lines; j++)
              {
                #define lline ctx.leaders[i].lines[j]
                ok ("MULTILEADER.ctx.leaders[%d].lines[%d].num_points: " FORMAT_BL,
                    i, j, lline.num_points);
                for (BITCODE_BL k = 0; k < lline.num_points; k++)
                  {
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].points[%d]: "
                        "(%f, %f, %f)", i, j, k, lline.points[k].x,
                        lline.points[k].y, lline.points[k].z);
                  }
                ok ("MULTILEADER.ctx.leaders[%d].lines[%d].num_breaks: " FORMAT_BL,
                    i, j, lline.num_breaks);
                for (BITCODE_BL k = 0; k < lline.num_breaks; k++)
                  {
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].breaks[%d].start: (%f, %f, "
                        "%f)", i, j, k, lline.breaks[k].start.x, lline.breaks[k].start.y,
                        lline.breaks[k].start.z);
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].breaks[%d].end: (%f, %f, "
                        "%f)", i, j, k, lline.breaks[k].end.x,
                        lline.breaks[k].end.y, lline.breaks[k].end.z);
                  }
                ok ("MULTILEADER.ctx.leaders[%d].lines[%d].line_index: " FORMAT_BL,
                    i, j, lline.line_index);
                if (dwg_version >= R_2010)
                  {
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].type: " FORMAT_BL,
                        i, j, lline.type);
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].color.index: " FORMAT_BSd,
                        i, j, lline.color.index);
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].ltype: " FORMAT_REF,
                        i, j, ARGS_REF (lline.ltype));
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].linewt: " FORMAT_BLd,
                        i, j, lline.linewt);
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].arrow_size: " FORMAT_BD,
                        i, j, lline.arrow_size);
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].arrow_handle: " FORMAT_REF,
                        i, j, ARGS_REF (lline.arrow_handle));
                    ok ("MULTILEADER.ctx.leaders[%d].lines[%d].flags: " FORMAT_BL,
                        i, j, lline.flags);
                  }
                #undef lline
              }
          }
        }
      else
        if (ctx.leaders)
          fail ("MULTILEADER.ctx.leaders but 0 ctx.num_leaders");
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, scale, BD, ctx.scale);
  //CHK_SUBCLASS_3RD (ctx, MLEADER_AnnotContext, content_base, ctx.content_base);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_height, BD, ctx.text_height);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, arrow_size, BD, ctx.arrow_size);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, landing_gap, BD, ctx.landing_gap);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_left, BS, ctx.text_left);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_right, BS, ctx.text_right);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, text_alignment, BS, ctx.text_alignment);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, attach_type, BS, ctx.attach_type);
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, has_content_txt, B, ctx.has_content_txt);
  if (ctx.has_content_txt)
    {
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.default_text, TV, ctx.content.txt.default_text);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, txt.normal, ctx.content.txt.normal);
      CHK_SUBCLASS_H (ctx.content, MLEADER_Content, txt.style, ctx.content.txt.style);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, txt.location, ctx.content.txt.location);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, txt.direction, ctx.content.txt.direction);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.rotation, BD, ctx.content.txt.rotation);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.width, BD, ctx.content.txt.width);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.line_spacing_factor, BD,
                         ctx.content.txt.line_spacing_factor);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.line_spacing_style, BS,
                         ctx.content.txt.line_spacing_style);
      CHK_SUBCLASS_CMC (ctx.content, MLEADER_Content, txt.color, ctx.content.txt.color);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.alignment, BS, ctx.content.txt.alignment);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.flow, BS, ctx.content.txt.flow);
      CHK_SUBCLASS_CMC (ctx.content, MLEADER_Content, txt.bg_color, ctx.content.txt.bg_color);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.bg_scale, BD, ctx.content.txt.bg_scale);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.bg_transparency, BL, ctx.content.txt.bg_transparency);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_bg_fill, B, ctx.content.txt.is_bg_fill);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_bg_mask_fill, B, ctx.content.txt.is_bg_mask_fill);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.col_type, BS, ctx.content.txt.col_type);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_height_auto, B, ctx.content.txt.is_height_auto);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.col_width, BD, ctx.content.txt.col_width);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.col_gutter, BD, ctx.content.txt.col_gutter);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.is_col_flow_reversed, B,
                         ctx.content.txt.is_col_flow_reversed);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.num_col_sizes, BL,
                         ctx.content.txt.num_col_sizes);
      for (i = 0; i < ctx.content.txt.num_col_sizes; i++)
        {
          ok ("MULTILEADER.ctx.content.txt.col_sizes[%d]: %f", i, ctx.content.txt.col_sizes[i]);
        }
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.word_break, B,
                         ctx.content.txt.word_break);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, txt.unknown, B,
                         ctx.content.txt.unknown);
    }
  CHK_SUBCLASS_TYPE (ctx, MLEADER_AnnotContext, has_content_blk, B, ctx.has_content_blk);
  if (ctx.has_content_blk)
    {
      if (ctx.has_content_txt)
        fail ("ctx.has_content_blk && ctx.has_content_txt");
      CHK_SUBCLASS_H (ctx.content, MLEADER_Content, blk.block_table, ctx.content.blk.block_table);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, blk.normal, ctx.content.blk.normal);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, blk.location, ctx.content.blk.location);
      CHK_SUBCLASS_3RD (ctx.content, MLEADER_Content, blk.scale, ctx.content.blk.scale);
      CHK_SUBCLASS_TYPE (ctx.content, MLEADER_Content, blk.rotation, BD, ctx.content.blk.rotation);
      CHK_SUBCLASS_CMC (ctx.content, MLEADER_Content, blk.color, ctx.content.blk.color);
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

  CHK_ENTITY_H (mleader, MULTILEADER, mleaderstyle, mleaderstyle);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, flags, BL, flags);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, type, BS, type);
  CHK_ENTITY_CMC (mleader, MULTILEADER, color, color);
  CHK_ENTITY_H (mleader, MULTILEADER, ltype, ltype);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, linewt, BLd, linewt);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, has_landing, B, has_landing);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, has_dogleg, B, has_dogleg);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, landing_dist, BD, landing_dist);
  CHK_ENTITY_H (mleader, MULTILEADER, arrow_handle, arrow_handle);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, arrow_size, BD, arrow_size);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, style_content, BS, style_content);
  CHK_ENTITY_H (mleader, MULTILEADER, text_style, text_style);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, text_left, BS, text_left);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, text_right, BS, text_right);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, text_angletype, BS, text_angletype);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_type, BS, attach_type);
  CHK_ENTITY_CMC (mleader, MULTILEADER, text_color, text_color);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, has_text_frame, B, has_text_frame);
  CHK_ENTITY_H (mleader, MULTILEADER, block_style, block_style);
  CHK_ENTITY_CMC (mleader, MULTILEADER, block_color, block_color);
  CHK_ENTITY_3RD (mleader, MULTILEADER, block_scale, block_scale);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, block_rotation, BD, block_rotation);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, style_attachment, BS, style_attachment);
  CHK_ENTITY_TYPE (mleader, MULTILEADER, is_annotative, B, is_annotative);

  if (dwg_version >= R_2000 && dwg_version <= R_2007)
    {
      // TODO no coverage
      CHK_ENTITY_TYPE (mleader, MULTILEADER, num_arrowheads, BL, num_arrowheads);
      if (!dwg_dynapi_entity_value (mleader, "MULTILEADER", "arrowheads", &arrowheads, NULL))
        fail ("MULTILEADER.arrowheads");
      if (num_arrowheads)
        {
          if (!arrowheads)
            fail ("!MULTILEADER.arrowheads but num_arrowheads");
          else
            for (i = 0; i < num_arrowheads; i++)
              {
                ok ("MULTILEADER.arrowheads[%d].is_default: " FORMAT_BL, i,
                    arrowheads[i].is_default);
                ok ("MULTILEADER.arrowheads[%d].arrowhead: " FORMAT_REF, i,
                    ARGS_REF (arrowheads[i].arrowhead));
              }
        }
      else
        if (arrowheads)
          fail ("MULTILEADER.arrowheads but 0 num_arrowheads");

      // TODO no coverage
      CHK_ENTITY_TYPE (mleader, MULTILEADER, num_blocklabels, BL, num_blocklabels);
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

      CHK_ENTITY_TYPE (mleader, MULTILEADER, neg_textdir, B, neg_textdir);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, ipe_alignment, BS, ipe_alignment);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, justification, BS, justification);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, scale_factor, BD, scale_factor);
    }
  if (dwg_version >= R_2010)
    {
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_dir, BS, attach_dir);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_top, BS, attach_top);
      CHK_ENTITY_TYPE (mleader, MULTILEADER, attach_bottom, BS, attach_bottom);
    }
  if (dwg_version >= R_2013)
    {
      CHK_ENTITY_TYPE (mleader, MULTILEADER, text_extended, B, text_extended);
    }
}

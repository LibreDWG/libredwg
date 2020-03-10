#define DWG_TYPE DWG_TYPE_MULTILEADER
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BS class_version;
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
      CHK_ENTITY_TYPE (mleader, MULTILEADER, num_arrowheads, BL, num_arrowheads);
      //  Dwg_LEADER_ArrowHead *arrowheads;
      CHK_ENTITY_TYPE (mleader, MULTILEADER, num_blocklabels, BL, num_blocklabels);
      // Dwg_LEADER_BlockLabel *blocklabels;
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

#define DWG_TYPE DWG_TYPE_SECTIONVIEWSTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*!< DXF 70 0 */
  BITCODE_T desc;
  BITCODE_B is_modified_for_recompute;
  BITCODE_T display_name;
  BITCODE_BL viewstyle_flags;

  BITCODE_BL flags;
  BITCODE_H identifier_style;
  BITCODE_CMC identifier_color;
  BITCODE_BD identifier_height;
  BITCODE_H arrow_start_symbol;
  BITCODE_H arrow_end_symbol;
  BITCODE_CMC arrow_symbol_color;
  BITCODE_BD arrow_symbol_size;
  BITCODE_T identifier_exclude_characters;
  BITCODE_BLd identifier_position;
  BITCODE_BD identifier_offset;
  BITCODE_BLd arrow_position;
  BITCODE_BD arrow_symbol_extension_length;
  BITCODE_H plane_ltype;
  BITCODE_BLd plane_linewt;
  BITCODE_CMC plane_line_color;
  BITCODE_H bend_ltype;
  BITCODE_BLd bend_linewt;
  BITCODE_CMC bend_line_color;
  BITCODE_BD bend_line_length;
  BITCODE_BD end_line_overshoot;
  BITCODE_BD end_line_length;
  BITCODE_H viewlabel_text_style;
  BITCODE_CMC viewlabel_text_color;
  BITCODE_BD viewlabel_text_height;
  BITCODE_BL viewlabel_attachment;
  BITCODE_BD viewlabel_offset;
  BITCODE_BL viewlabel_alignment;
  BITCODE_T viewlabel_pattern;
  BITCODE_CMC hatch_color;
  BITCODE_CMC hatch_bg_color;
  BITCODE_T hatch_pattern;
  BITCODE_BD hatch_scale;
  BITCODE_BLd hatch_transparency;
  BITCODE_B unknown_b1;
  BITCODE_B unknown_b2;
  BITCODE_BL i, num_hatch_angles;
  BITCODE_BD *hatch_angles;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_sectionviewstyle *_obj = dwg_object_to_SECTIONVIEWSTYLE (obj);

  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, class_version, BS);
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, desc);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, is_modified_for_recompute, B);
  SINCE (R_2018)
  {
    CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, display_name);
    CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewstyle_flags, BLx);
  }

  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, flags, BLx);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, identifier_style);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, identifier_color);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, identifier_height, BD);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, arrow_start_symbol);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, arrow_end_symbol);
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, identifier_exclude_characters);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, identifier_position, BLd);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, identifier_offset, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, arrow_position, BLd);

  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, arrow_symbol_color);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, arrow_symbol_size, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, arrow_symbol_extension_length, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, plane_linewt, BLd);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, plane_line_color);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, plane_ltype);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, bend_linewt, BLd);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, bend_line_color);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, bend_ltype);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, bend_line_length, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, end_line_overshoot, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, end_line_length, BD);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, viewlabel_text_style);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, viewlabel_text_color);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_text_height, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_offset, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_attachment, BL);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_alignment, BL);
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, viewlabel_pattern);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, hatch_color);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, hatch_bg_color);
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, hatch_pattern);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, hatch_scale, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, hatch_transparency, BLd);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, unknown_b1, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, unknown_b2, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, num_hatch_angles, BL);
  CHK_ENTITY_VECTOR_TYPE (_obj, SECTIONVIEWSTYLE, hatch_angles,
                          num_hatch_angles, BD);
  // flag bits:
  // CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, is_continuous_labeling, B);
  // CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_arrowheads, B);
  // CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_viewlabel, B);
  // CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_all_plane_lines, B);
  // CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_all_bend_identifiers, B);
  // CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_end_and_bend_lines, B);
  // CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_hatching, B);
}

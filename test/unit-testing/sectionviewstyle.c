// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_SECTIONVIEWSTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*!< DXF 70 0 */
  BITCODE_T name;
  BITCODE_B is_modified_for_recompute;
  BITCODE_T description;

  BITCODE_H identifier_style;
  BITCODE_CMC identifier_color;
  BITCODE_BD identifier_height;
  BITCODE_BL identifier_exclude_characters;
  BITCODE_BD identifier_position;
  BITCODE_BD identifier_offset;
  BITCODE_BD arrow_position;
  BITCODE_H arrow_start_symbol;
  BITCODE_H arrow_end_symbol;
  BITCODE_CMC arrow_symbol_color;
  BITCODE_BD arrow_symbol_size;
  BITCODE_BD arrow_symbol_extension_length;
  BITCODE_BS plane_line_weight;
  BITCODE_CMC plane_line_color;
  BITCODE_H plane_line_type;
  BITCODE_BS bend_line_weight;
  BITCODE_CMC bend_line_color;
  BITCODE_H bend_line_type;
  BITCODE_BD bend_line_length;
  BITCODE_BD end_line_length;
  BITCODE_H viewlabel_text_style;
  BITCODE_CMC viewlabel_text_color;
  BITCODE_BD viewlabel_text_height;
  BITCODE_T viewlabel_field;
  BITCODE_BD viewlabel_offset;
  BITCODE_BS viewlabel_attachment;
  BITCODE_BS viewlabel_alignment;
  BITCODE_BS viewlabel_pattern;
  BITCODE_BS hatch_pattern;
  BITCODE_CMC hatch_color;
  BITCODE_CMC hatch_bg_color;
  BITCODE_BD hatch_scale;
  BITCODE_BD hatch_angles;
  BITCODE_BS hatch_transparency;
  BITCODE_B is_continuous_labeling;
  BITCODE_B show_arrowheads;
  BITCODE_B show_viewlabel;
  BITCODE_B show_all_plane_lines;
  BITCODE_B show_all_bend_indentifiers;
  BITCODE_B show_end_and_bend_lines;
  BITCODE_B show_hatching;

#ifdef DEBUG_CLASSES
  dwg_obj_sectionviewstyle *_obj = dwg_object_to_SECTIONVIEWSTYLE (obj);

  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, class_version, BS);
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, name, name);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, is_modified_for_recompute, B);
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, description, description);

  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, identifier_style, identifier_style);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, identifier_color, identifier_color);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, identifier_height, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, identifier_exclude_characters, BL);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, identifier_position, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, identifier_offset, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, arrow_position, BD);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, arrow_start_symbol, arrow_start_symbol);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, arrow_end_symbol, arrow_end_symbol);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, arrow_symbol_color, arrow_symbol_color);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, arrow_symbol_size, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, arrow_symbol_extension_length, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, plane_line_weight, BS);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, plane_line_color, plane_line_color);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, plane_line_type, plane_line_type);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, bend_line_weight, BS);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, bend_line_color, bend_line_color);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, bend_line_type, bend_line_type);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, bend_line_length, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, end_line_length, BD);
  CHK_ENTITY_H (_obj, SECTIONVIEWSTYLE, viewlabel_text_style, viewlabel_text_style);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, viewlabel_text_color, viewlabel_text_color);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_text_height, BD);
  CHK_ENTITY_UTF8TEXT (_obj, SECTIONVIEWSTYLE, viewlabel_field, viewlabel_field);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_offset, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_attachment, BS);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_alignment, BS);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, viewlabel_pattern, BS);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, hatch_pattern, BS);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, hatch_color, hatch_color);
  CHK_ENTITY_CMC (_obj, SECTIONVIEWSTYLE, hatch_bg_color, hatch_bg_color);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, hatch_scale, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, hatch_angles, BD);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, hatch_transparency, BS);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, is_continuous_labeling, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_arrowheads, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_viewlabel, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_all_plane_lines, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_all_bend_indentifiers, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_end_and_bend_lines, B);
  CHK_ENTITY_TYPE (_obj, SECTIONVIEWSTYLE, show_hatching, B);
#endif
}

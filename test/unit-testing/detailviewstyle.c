// TODO DEBUGGING
#define DWG_TYPE DWG_TYPE_DETAILVIEWSTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS class_version; /*!< DXF 70 0 */
  BITCODE_T name;
  BITCODE_T description;
  BITCODE_B is_modified_for_recompute;
  BITCODE_H identifier_style;
  BITCODE_CMC identifier_color;
  BITCODE_BD identifier_height;
  BITCODE_BD identifier_offset;
  BITCODE_BS identifier_placement;
  BITCODE_H arrow_symbol;
  BITCODE_CMC arrow_symbol_color;
  BITCODE_BD arrow_symbol_size;
  BITCODE_B show_arrowheads;
  BITCODE_BS boundary_line_weight;
  BITCODE_CMC boundary_line_color;
  BITCODE_H boundary_line_type;
  BITCODE_BS connection_line_weight;
  BITCODE_CMC connection_line_color;
  BITCODE_H connection_line_type;
  BITCODE_H viewlabel_text_style;
  BITCODE_CMC viewlabel_text_color;
  BITCODE_BD viewlabel_text_height;
  BITCODE_T viewlabel_field;
  BITCODE_BD viewlabel_offset;
  BITCODE_BS viewlabel_attachment;
  BITCODE_BS viewlabel_alignment;
  BITCODE_BS viewlabel_pattern;
  BITCODE_B show_viewlabel;
  BITCODE_B model_edge;
  BITCODE_BD borderline_weight;
  BITCODE_CMC borderline_color;
  BITCODE_H borderline_type;

#ifdef DEBUG_CLASSES
  dwg_obj_detailviewstyle *_obj = dwg_object_to_DETAILVIEWSTYLE (obj);

  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, class_version, BS);
  CHK_ENTITY_UTF8TEXT (_obj, DETAILVIEWSTYLE, name, name);
  //CHK_ENTITY_UTF8TEXT (_obj, DETAILVIEWSTYLE, description, description);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, is_modified_for_recompute, B);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, identifier_style, identifier_style);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, identifier_color, identifier_color);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, identifier_height, BD);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, identifier_offset, BD);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, identifier_placement, BS);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, arrow_symbol, arrow_symbol);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, arrow_symbol_color, arrow_symbol_color);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, arrow_symbol_size, BD);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, show_arrowheads, B);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, boundary_line_weight, BS);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, boundary_line_color, boundary_line_color);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, boundary_line_type, boundary_line_type);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, connection_line_type, connection_line_type);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, connection_line_weight, BS);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, connection_line_color, connection_line_color);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, viewlabel_text_style, viewlabel_text_style);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, viewlabel_text_color, viewlabel_text_color);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_text_height, BD);
  CHK_ENTITY_UTF8TEXT (_obj, DETAILVIEWSTYLE, viewlabel_field, viewlabel_field);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_offset, BD);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_attachment, BS);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_alignment, BS);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_pattern, BS);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, show_viewlabel, B);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, model_edge, B);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, borderline_weight, BD);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, borderline_color, borderline_color);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, borderline_type, borderline_type);
#endif
}

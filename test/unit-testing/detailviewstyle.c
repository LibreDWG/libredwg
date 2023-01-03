#define DWG_TYPE DWG_TYPE_DETAILVIEWSTYLE
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
  BITCODE_T identifier_exclude_characters;
  BITCODE_BD identifier_offset;
  BITCODE_RC identifier_placement;
  BITCODE_H arrow_symbol;
  BITCODE_CMC arrow_symbol_color;
  BITCODE_BD arrow_symbol_size;
  BITCODE_BLd boundary_linewt;
  BITCODE_CMC boundary_line_color;
  BITCODE_H boundary_ltype;
  BITCODE_BLd connection_linewt;
  BITCODE_CMC connection_line_color;
  BITCODE_H connection_ltype;
  BITCODE_H viewlabel_text_style;
  BITCODE_CMC viewlabel_text_color;
  BITCODE_BD viewlabel_text_height;
  BITCODE_BD viewlabel_offset;
  BITCODE_BL viewlabel_attachment;
  BITCODE_BL viewlabel_alignment;
  BITCODE_T viewlabel_pattern;
  BITCODE_H borderline_ltype;
  BITCODE_BLd borderline_linewt;
  BITCODE_CMC borderline_color;
  BITCODE_RC model_edge;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_detailviewstyle *_obj = dwg_object_to_DETAILVIEWSTYLE (obj);

  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, class_version, BS);
  CHK_ENTITY_UTF8TEXT (_obj, DETAILVIEWSTYLE, desc);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, is_modified_for_recompute, B);
  SINCE (R_2018)
  {
    CHK_ENTITY_UTF8TEXT (_obj, DETAILVIEWSTYLE, display_name);
    CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewstyle_flags, BL);
  }

  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, flags, BL);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, identifier_style);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, identifier_color);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, identifier_height, BD);
  CHK_ENTITY_UTF8TEXT (_obj, DETAILVIEWSTYLE, identifier_exclude_characters);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, identifier_offset, BD);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, identifier_placement, RC);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, arrow_symbol);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, arrow_symbol_color);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, arrow_symbol_size, BD);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, boundary_ltype);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, boundary_linewt, BLd);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, boundary_line_color);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, connection_ltype);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, connection_linewt, BLd);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, connection_line_color);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, viewlabel_text_style);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, viewlabel_text_color);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_text_height, BD);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_offset, BD);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_attachment, BL);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, viewlabel_alignment, BL);
  CHK_ENTITY_UTF8TEXT (_obj, DETAILVIEWSTYLE, viewlabel_pattern);
  CHK_ENTITY_H (_obj, DETAILVIEWSTYLE, borderline_ltype);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, borderline_linewt, BLd);
  CHK_ENTITY_CMC (_obj, DETAILVIEWSTYLE, borderline_color);
  CHK_ENTITY_TYPE (_obj, DETAILVIEWSTYLE, model_edge, RC);
}

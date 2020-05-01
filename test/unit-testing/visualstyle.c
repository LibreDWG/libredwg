// TODO unstable
#define DWG_TYPE DWG_TYPE_VISUALSTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T description;
  BITCODE_BL style_type;
  BITCODE_BL face_lighting_model;
  BITCODE_BL face_lighting_quality;
  BITCODE_BL face_color_mode;
  BITCODE_BD face_opacity;
  BITCODE_BD face_specular;
  BITCODE_BS face_modifier;
  BITCODE_CMC face_mono_color;
  BITCODE_BS edge_model;
  BITCODE_BL edge_style;
  BITCODE_CMC edge_intersection_color;
  BITCODE_CMC edge_obscured_color;
  BITCODE_BS edge_obscured_line_pattern;
  BITCODE_BS edge_intersection_line_pattern;
  BITCODE_BD edge_crease_angle;
  BITCODE_BS edge_modifier;
  BITCODE_CMC edge_color;
  BITCODE_BD edge_opacity;
  BITCODE_BS edge_width;
  BITCODE_BS edge_overhang;
  BITCODE_BS edge_jitter;
  BITCODE_CMC edge_silhouette_color;
  BITCODE_BS edge_silhouette_width;
  BITCODE_BS edge_halo_gap;
  BITCODE_BS numedge_isolines;
  BITCODE_B edge_hide_precision_flag;
  BITCODE_BS edge_style_apply_flag;
  BITCODE_BS display_style;
  BITCODE_BL display_brightness;
  BITCODE_BS display_shadow_type;
  BITCODE_B int_only;
  BITCODE_BD unknown_float45;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_visualstyle *vsty = dwg_object_to_VISUALSTYLE (obj);
 
  CHK_ENTITY_UTF8TEXT (vsty, VISUALSTYLE, description, description);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, style_type, BL, style_type);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, style_type, BL, 32);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_lighting_model, BL, face_lighting_model);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_lighting_model, BL, 3);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_lighting_quality, BL, face_lighting_quality);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_lighting_quality, BL, 2);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_color_mode, BL, face_color_mode);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_color_mode, BS, 6);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_opacity, BD, face_opacity);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_specular, BD, face_specular);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_modifier, BL, face_modifier);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_modifier, BL, 2);
  //CHK_ENTITY_CMC (vsty, VISUALSTYLE, color, color);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, face_mono_color, face_mono_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_model, BL, edge_model);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, edge_model, BL, 2);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_style, BL, edge_style);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_intersection_color, edge_intersection_color);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_obscured_color, edge_obscured_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_obscured_line_pattern, BL, edge_obscured_line_pattern);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_intersection_line_pattern, BL, edge_intersection_line_pattern);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_crease_angle, BD, edge_crease_angle);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, edge_crease_angle, BD, 360.0);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_modifier, BL, edge_modifier);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_color, edge_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_opacity, BD, edge_opacity);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_width, BS, edge_width);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_overhang, BS, edge_overhang);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_jitter, BL, edge_jitter);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_silhouette_color, edge_silhouette_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_silhouette_width, BS, edge_silhouette_width);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_halo_gap, RC, edge_halo_gap);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, numedge_isolines, BS, numedge_isolines);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_hide_precision_flag, B, edge_hide_precision_flag);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_style_apply_flag, BS, edge_style_apply_flag);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_style, BS, display_style);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_shadow_type, BL, display_shadow_type);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_brightness, BL, display_brightness);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, int_only, B, int_only);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, unknown_float45, BD, unknown_float45);
}

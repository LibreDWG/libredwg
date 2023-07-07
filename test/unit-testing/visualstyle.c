#define DWG_TYPE DWG_TYPE_VISUALSTYLE
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T description;
  BITCODE_BL style_type;
  BITCODE_BS ext_lighting_model;
  BITCODE_B internal_only;
  BITCODE_BL face_lighting_model;
  BITCODE_BL face_lighting_quality;
  BITCODE_BL face_color_mode;
  BITCODE_BD face_opacity;
  BITCODE_BD face_specular;
  BITCODE_BL face_modifier;
  BITCODE_CMC face_mono_color;
  BITCODE_BS edge_model;
  BITCODE_BL edge_style;
  BITCODE_CMC edge_intersection_color;
  BITCODE_CMC edge_obscured_color;
  BITCODE_BL edge_obscured_ltype;
  BITCODE_BL edge_intersection_ltype;
  BITCODE_BD edge_crease_angle;
  BITCODE_BL edge_modifier;
  BITCODE_CMC edge_color;
  BITCODE_BD edge_opacity;
  BITCODE_BL edge_width;
  BITCODE_BL edge_overhang;
  BITCODE_BL edge_jitter;
  BITCODE_CMC edge_silhouette_color;
  BITCODE_BL edge_silhouette_width;
  BITCODE_BL edge_halo_gap;
  BITCODE_BL edge_isolines;
  BITCODE_B edge_do_hide_precision;
  BITCODE_BL edge_style_apply;
  BITCODE_BL display_settings;
  BITCODE_BLd display_brightness_bl; /*!< DXF 44 <=r2007 */
  BITCODE_BD display_brightness;     /*!< DXF 44  r2010+ */
  BITCODE_BL display_shadow_type;
  BITCODE_BD bd2007_45;

  BITCODE_BS num_props; /*!< r2013+ version3 58x */
  BITCODE_B b_prop1c;
  BITCODE_B b_prop1d;
  BITCODE_B b_prop1e;
  BITCODE_B b_prop1f;
  BITCODE_B b_prop20;
  BITCODE_B b_prop21;
  BITCODE_B b_prop22;
  BITCODE_B b_prop23;
  BITCODE_B b_prop24;
  BITCODE_BL bl_prop25;
  BITCODE_BD bd_prop26;
  BITCODE_BD bd_prop27;
  BITCODE_BL bl_prop28;
  BITCODE_CMC c_prop29;
  BITCODE_BL bl_prop2a;
  BITCODE_BL bl_prop2b;
  BITCODE_CMC c_prop2c;
  BITCODE_B b_prop2d;
  BITCODE_BL bl_prop2e;
  BITCODE_BL bl_prop2f;
  BITCODE_BL bl_prop30;
  BITCODE_B b_prop31;
  BITCODE_BL bl_prop32;
  BITCODE_CMC c_prop33;
  BITCODE_BD bd_prop34;
  BITCODE_BL edge_wiggle;
  BITCODE_T strokes;
  BITCODE_B b_prop37;
  BITCODE_BD bd_prop38;
  BITCODE_BD bd_prop39;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_visualstyle *vsty = dwg_object_to_VISUALSTYLE (obj);

  CHK_ENTITY_UTF8TEXT (vsty, VISUALSTYLE, description);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, style_type, BL);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, style_type, BL, 32);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, ext_lighting_model, BS);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_lighting_model, BL);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_lighting_model, BL, 3);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_lighting_quality, BL);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_lighting_quality, BL, 2);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_color_mode, BL);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_color_mode, BL, 6);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_opacity, BD);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_specular, BD);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, face_modifier, BL);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, face_modifier, BL, 2);
  // CHK_ENTITY_CMC (vsty, VISUALSTYLE, color);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, face_mono_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_model, BL);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, edge_model, BL, 2);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_style, BL);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_intersection_color);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_obscured_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_obscured_ltype, BL);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_crease_angle, BD);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, edge_crease_angle, BD, 360.0);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_modifier, BL);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_opacity, BD);
  PRE (R_2010)
  {
    CHK_ENTITY_TYPE_CAST (vsty, VISUALSTYLE, edge_width, BS, BL);
    CHK_ENTITY_TYPE_CAST (vsty, VISUALSTYLE, edge_overhang, BS, BL);
    CHK_ENTITY_TYPE_CAST (vsty, VISUALSTYLE, edge_silhouette_width, BS, BL);
    CHK_ENTITY_TYPE_CAST (vsty, VISUALSTYLE, edge_halo_gap, RC, BL);
    CHK_ENTITY_TYPE_CAST (vsty, VISUALSTYLE, edge_isolines, BS, BL);
    CHK_ENTITY_TYPE_CAST (vsty, VISUALSTYLE, edge_style_apply, BS, BL);
    CHK_ENTITY_TYPE_CAST (vsty, VISUALSTYLE, edge_intersection_ltype, BS, BL);
  }
  LATER_VERSIONS
  {
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_width, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_overhang, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_silhouette_width, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_halo_gap, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_isolines, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_style_apply, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_intersection_ltype, BL);
  }
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_jitter, BL);
  CHK_ENTITY_CMC (vsty, VISUALSTYLE, edge_silhouette_color);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_do_hide_precision, B);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_settings, BL);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_brightness_bl, BLd);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_brightness, BD);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_shadow_type, BL);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, internal_only, B);
  VERSION (R_2007)
  {
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd2007_45, BD);
  }
  SINCE (R_2013)
  {
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, num_props, BS);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1c, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1d, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1e, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1f, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop20, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop21, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop22, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop23, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop24, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop25, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop26, BD);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop27, BD);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop28, BL);
    CHK_ENTITY_CMC (vsty, VISUALSTYLE, c_prop29);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2a, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2b, BL);
    CHK_ENTITY_CMC (vsty, VISUALSTYLE, c_prop2c);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop2d, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2e, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2f, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop30, BL);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop31, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop32, BL);
    CHK_ENTITY_CMC (vsty, VISUALSTYLE, c_prop33);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop34, BD);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_wiggle, BL);
    CHK_ENTITY_UTF8TEXT (vsty, VISUALSTYLE, strokes);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop37, B);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop38, BD);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop39, BD);
  }
}

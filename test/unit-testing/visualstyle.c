// TODO unstable
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
  BITCODE_BS edge_intersection_ltype;
  BITCODE_BD edge_crease_angle;
  BITCODE_BL edge_modifier;
  BITCODE_CMC edge_color;
  BITCODE_BD edge_opacity;
  BITCODE_BS edge_width;
  BITCODE_BS edge_overhang;
  BITCODE_BS edge_jitter;
  BITCODE_CMC edge_silhouette_color;
  BITCODE_BL edge_silhouette_width;
  BITCODE_BL edge_halo_gap;
  BITCODE_BL edge_isolines;
  BITCODE_B edge_do_hide_precision;
  BITCODE_BS edge_style_apply;
  BITCODE_BL display_settings;
  BITCODE_BL display_brightness_bl;     /*!< DXF 44 <=r2007 */
  BITCODE_BD display_brightness;        /*!< DXF 44  r2010+ */
  BITCODE_BL display_shadow_type;
  BITCODE_BD bd2007_45;

  BITCODE_BS num_props; 		/*!< r2013+ version3 58x */
  BITCODE_B  b_prop1c;
  BITCODE_B  b_prop1d;
  BITCODE_B  b_prop1e;
  BITCODE_B  b_prop1f;
  BITCODE_B  b_prop20;
  BITCODE_B  b_prop21;
  BITCODE_B  b_prop22;
  BITCODE_B  b_prop23;
  BITCODE_B  b_prop24;
  BITCODE_BL  bl_prop25;
  BITCODE_BD bd_prop26;
  BITCODE_BD bd_prop27;
  BITCODE_BL bl_prop28;
  BITCODE_CMC c_prop29;
  BITCODE_BL bl_prop2a;
  BITCODE_BL bl_prop2b;
  BITCODE_CMC c_prop2c;
  BITCODE_B  b_prop2d;
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
 
  CHK_ENTITY_UTF8TEXT (vsty, VISUALSTYLE, description, description);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, style_type, BL, style_type);
  CHK_ENTITY_MAX (vsty, VISUALSTYLE, style_type, BL, 32);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, ext_lighting_model, BS, ext_lighting_model);
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
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_obscured_ltype, BL, edge_obscured_ltype);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_intersection_ltype, BS, edge_intersection_ltype);
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
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_isolines, BS, edge_isolines);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_do_hide_precision, B, edge_do_hide_precision);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_style_apply, BS, edge_style_apply);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_settings, BL, display_settings);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_brightness_bl, BL, display_brightness_bl);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_brightness, BD, display_brightness);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, display_shadow_type, BL, display_shadow_type);
  CHK_ENTITY_TYPE (vsty, VISUALSTYLE, internal_only, B, internal_only);
  if (dwg_version == R_2007) {
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd2007_45, BD, bd2007_45);
  }
  if (dwg_version >= R_2013) {
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, num_props, BS, num_props);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1c, B, b_prop1c);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1d, B, b_prop1d);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1e, B, b_prop1e);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop1f, B, b_prop1f);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop20, B, b_prop20);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop21, B, b_prop21);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop22, B, b_prop22);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop23, B, b_prop23);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop24, B, b_prop24);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop25, BL, bl_prop25);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop26, BD, bd_prop26);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop27, BD, bd_prop27);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop28, BL, bl_prop28);
    CHK_ENTITY_CMC (vsty, VISUALSTYLE, c_prop29, c_prop29);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2a, BL, bl_prop2a);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2b, BL, bl_prop2b);
    CHK_ENTITY_CMC (vsty, VISUALSTYLE, c_prop2c, c_prop2c);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop2d, B, b_prop2d);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2e, BL, bl_prop2e);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop2f, BL, bl_prop2f);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop30, BL, bl_prop30);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop31, B, b_prop31);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bl_prop32, BL, bl_prop32);
    CHK_ENTITY_CMC (vsty, VISUALSTYLE, c_prop33, c_prop33);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop34, BD, bd_prop34);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, edge_wiggle, BL, edge_wiggle);
    CHK_ENTITY_UTF8TEXT (vsty, VISUALSTYLE, strokes, strokes);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, b_prop37, B, b_prop37);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop38, BD, bd_prop38);
    CHK_ENTITY_TYPE (vsty, VISUALSTYLE, bd_prop39, BD, bd_prop39);
  }  
}

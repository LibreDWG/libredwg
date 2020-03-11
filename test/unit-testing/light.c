// TODO unstable
#define DWG_TYPE DWG_TYPE_LIGHT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_T name;
  BITCODE_BS type;
  BITCODE_B status;
  BITCODE_CMC color;
  BITCODE_B plot_glyph;
  BITCODE_BD intensity;
  BITCODE_3BD position;
  BITCODE_3BD target;
  BITCODE_BS attenuation_type;
  BITCODE_B use_attenuation_limits;
  BITCODE_BD attenuation_start_limit;
  BITCODE_BD attenuation_end_limit;
  BITCODE_BD hotspot_angle;
  BITCODE_BD falloff_angle;
  BITCODE_B cast_shadows;
  BITCODE_BS shadow_type;
  BITCODE_BS shadow_map_size;
  BITCODE_RC shadow_map_softness;
  BITCODE_H lights_layer;
#ifdef DEBUG_CLASSES
  /* yet unknown: */
  BITCODE_BS lamp_color_type;
  BITCODE_BD lamp_color_temp;
  BITCODE_BS lamp_color_preset;
  BITCODE_BL lamp_color_rgb;
  BITCODE_TV web_file;
  BITCODE_3BD web_rotation;
  BITCODE_B has_target_grip;
  BITCODE_BS glyph_display_type;
  BITCODE_BS physical_intensity_method;
  BITCODE_BS drawable_type;
#endif

  dwg_ent_light *light = dwg_object_to_LIGHT (obj);

  CHK_ENTITY_TYPE (light, LIGHT, class_version, BL, class_version);
  CHK_ENTITY_UTF8TEXT (light, LIGHT, name, name);
  CHK_ENTITY_TYPE (light, LIGHT, type, BS, type);
  if (type > 3)
    fail ("Invalid LIGHT.type " FORMAT_BS " > 3", type);
  CHK_ENTITY_TYPE (light, LIGHT, status, B, status);
  CHK_ENTITY_CMC (light, LIGHT, color, color);
  CHK_ENTITY_TYPE (light, LIGHT, plot_glyph, B, plot_glyph);
  CHK_ENTITY_TYPE (light, LIGHT, intensity, BD, intensity);
  CHK_ENTITY_3RD (light, LIGHT, position, position);
  CHK_ENTITY_3RD (light, LIGHT, target, target);
  CHK_ENTITY_TYPE (light, LIGHT, attenuation_type, BS, attenuation_type);
  if (attenuation_type > 2)
    fail ("Invalid LIGHT.attenuation_type " FORMAT_BS " > 2", attenuation_type);
  CHK_ENTITY_TYPE (light, LIGHT, use_attenuation_limits, B, use_attenuation_limits);
  CHK_ENTITY_TYPE (light, LIGHT, attenuation_start_limit, BD, attenuation_start_limit);
  CHK_ENTITY_TYPE (light, LIGHT, attenuation_end_limit, BD, attenuation_end_limit);
  CHK_ENTITY_TYPE (light, LIGHT, hotspot_angle, BD, hotspot_angle);
  if (hotspot_angle > 3.142)
    fail ("Invalid LIGHT.hotspot_angle > pi");
  CHK_ENTITY_TYPE (light, LIGHT, falloff_angle, BD, falloff_angle); // only with type=3
  if (falloff_angle > 3.142)
    fail ("Invalid LIGHT.falloff_angle > pi");
  CHK_ENTITY_TYPE (light, LIGHT, cast_shadows, B, cast_shadows);
  CHK_ENTITY_TYPE (light, LIGHT, shadow_type, BS, shadow_type);
  if (shadow_type > 1)
    fail ("Invalid LIGHT.shadow_type " FORMAT_BS " > 1", shadow_type);
  CHK_ENTITY_TYPE (light, LIGHT, shadow_map_size, BS, shadow_map_size);
  CHK_ENTITY_TYPE (light, LIGHT, shadow_map_softness, RC, shadow_map_softness);
  CHK_ENTITY_H (light, LIGHT, lights_layer, lights_layer);
#ifdef DEBUG_CLASSES
  CHK_ENTITY_TYPE (light, LIGHT, lamp_color_type, BS, lamp_color_type);
  if (lamp_color_type > 1)
    fail ("Invalid LIGHT.lamp_color_type " FORMAT_BS " > 1", lamp_color_type);
  CHK_ENTITY_TYPE (light, LIGHT, lamp_color_temp, BD, lamp_color_temp);
  CHK_ENTITY_TYPE (light, LIGHT, lamp_color_preset, BS, lamp_color_preset);
  CHK_ENTITY_TYPE (light, LIGHT, lamp_color_rgb, BL, lamp_color_rgb);
  CHK_ENTITY_UTF8TEXT (light, LIGHT, web_file, web_file);
  CHK_ENTITY_3RD (light, LIGHT, web_rotation, web_rotation);
  CHK_ENTITY_TYPE (light, LIGHT, has_target_grip, B, has_target_grip);
  CHK_ENTITY_TYPE (light, LIGHT, glyph_display_type, BS, glyph_display_type);
  if (glyph_display_type > 2)
    fail ("Invalid LIGHT.glyph_display_type " FORMAT_BS " > 2", glyph_display_type);
  CHK_ENTITY_TYPE (light, LIGHT, physical_intensity_method, BS, physical_intensity_method);
  CHK_ENTITY_TYPE (light, LIGHT, drawable_type, BS, drawable_type);
#endif  
}

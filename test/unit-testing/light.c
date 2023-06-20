// stable
#define DWG_TYPE DWG_TYPE_LIGHT
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL class_version;
  BITCODE_T name;
  BITCODE_BL type;
  BITCODE_B status;
  BITCODE_CMC light_color;
  BITCODE_B plot_glyph;
  BITCODE_BD intensity;
  BITCODE_3BD position;
  BITCODE_3BD target;
  BITCODE_BL attenuation_type;
  BITCODE_B use_attenuation_limits;
  BITCODE_BD attenuation_start_limit;
  BITCODE_BD attenuation_end_limit;
  BITCODE_BD hotspot_angle;
  BITCODE_BD falloff_angle;
  BITCODE_B cast_shadows;
  BITCODE_BL shadow_type;
  BITCODE_BS shadow_map_size;
  BITCODE_RC shadow_map_softness;

  BITCODE_B is_photometric;
  BITCODE_B has_photometric_data;
  BITCODE_B has_webfile;
  BITCODE_T webfile;
  BITCODE_BS physical_intensity_method;
  BITCODE_BD physical_intensity;
  BITCODE_BD illuminance_dist;
  BITCODE_BS lamp_color_type;
  BITCODE_BD lamp_color_temp;
  BITCODE_BS lamp_color_preset;
  BITCODE_BL lamp_color_rgb;
  BITCODE_3BD web_rotation;
  BITCODE_BS extlight_shape;
  BITCODE_BD extlight_length;
  BITCODE_BD extlight_width;
  BITCODE_BD extlight_radius;
  BITCODE_BS webfile_type;
  BITCODE_BS web_symetry;
  BITCODE_BS has_target_grip;
  BITCODE_BD web_flux;
  BITCODE_BD web_angle1;
  BITCODE_BD web_angle2;
  BITCODE_BD web_angle3;
  BITCODE_BD web_angle4;
  BITCODE_BD web_angle5;
  BITCODE_BS glyph_display_type;

  dwg_ent_light *light = dwg_object_to_LIGHT (obj);

  CHK_ENTITY_TYPE (light, LIGHT, class_version, BL);
  CHK_ENTITY_UTF8TEXT (light, LIGHT, name);
  CHK_ENTITY_TYPE (light, LIGHT, type, BL);
  CHK_ENTITY_MAX (light, LIGHT, type, BL, 3);
  CHK_ENTITY_TYPE (light, LIGHT, status, B);
  CHK_ENTITY_CMC (light, LIGHT, light_color);
  CHK_ENTITY_TYPE (light, LIGHT, plot_glyph, B);
  CHK_ENTITY_TYPE (light, LIGHT, intensity, BD); // max 100.0?
  CHK_ENTITY_3RD (light, LIGHT, position);
  CHK_ENTITY_3RD (light, LIGHT, target);
  CHK_ENTITY_TYPE (light, LIGHT, attenuation_type, BL);
  CHK_ENTITY_MAX (light, LIGHT, attenuation_type, BL, 2);
  CHK_ENTITY_TYPE (light, LIGHT, use_attenuation_limits, B);
  CHK_ENTITY_TYPE (light, LIGHT, attenuation_start_limit, BD);
  CHK_ENTITY_TYPE (light, LIGHT, attenuation_end_limit, BD);
  CHK_ENTITY_TYPE (light, LIGHT, hotspot_angle, BD);
  CHK_ENTITY_MAX (light, LIGHT, hotspot_angle, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE (light, LIGHT, falloff_angle, BD);
  CHK_ENTITY_MAX (light, LIGHT, falloff_angle, BD, MAX_ANGLE);
  CHK_ENTITY_TYPE (light, LIGHT, cast_shadows, B);
  CHK_ENTITY_TYPE (light, LIGHT, shadow_type, BL);
  CHK_ENTITY_MAX (light, LIGHT, shadow_type, BL, 1);
  CHK_ENTITY_TYPE (light, LIGHT, shadow_map_size, BS);
  CHK_ENTITY_MAX (light, LIGHT, shadow_map_size, BS, 4096);
  CHK_ENTITY_TYPE (light, LIGHT, shadow_map_softness, RC);
  CHK_ENTITY_MAX (light, LIGHT, shadow_map_softness, RC, 10);

  CHK_ENTITY_TYPE (light, LIGHT, is_photometric, B);
  CHK_ENTITY_TYPE (light, LIGHT, has_photometric_data, B);
  if (is_photometric && has_photometric_data)
    {
      CHK_ENTITY_TYPE (light, LIGHT, has_webfile, B);
      CHK_ENTITY_UTF8TEXT (light, LIGHT, webfile);
      CHK_ENTITY_TYPE (light, LIGHT, physical_intensity_method, BS);
      CHK_ENTITY_TYPE (light, LIGHT, physical_intensity, BD);
      CHK_ENTITY_TYPE (light, LIGHT, illuminance_dist, BD);
      CHK_ENTITY_TYPE (light, LIGHT, lamp_color_type, BS);
      CHK_ENTITY_MAX (light, LIGHT, lamp_color_type, BS, 1);
      CHK_ENTITY_TYPE (light, LIGHT, lamp_color_temp, BD);
      CHK_ENTITY_TYPE (light, LIGHT, lamp_color_preset, BS);
      // CHK_ENTITY_TYPE (light, LIGHT, lamp_color_rgb, BL);
      CHK_ENTITY_3RD (light, LIGHT, web_rotation);
      CHK_ENTITY_TYPE (light, LIGHT, extlight_shape, BS);
      CHK_ENTITY_MAX (light, LIGHT, extlight_shape, BS, 4);
      CHK_ENTITY_TYPE (light, LIGHT, extlight_length, BD);
      CHK_ENTITY_TYPE (light, LIGHT, extlight_width, BD);
      CHK_ENTITY_TYPE (light, LIGHT, extlight_radius, BD);
      CHK_ENTITY_TYPE (light, LIGHT, webfile_type, BS);
      CHK_ENTITY_TYPE (light, LIGHT, web_symetry, BS);
      CHK_ENTITY_TYPE (light, LIGHT, has_target_grip, BS);
      CHK_ENTITY_MAX (light, LIGHT, has_target_grip, BS, 1);
      CHK_ENTITY_TYPE (light, LIGHT, web_flux, BD);
      CHK_ENTITY_TYPE (light, LIGHT, web_angle1, BD);
      CHK_ENTITY_TYPE (light, LIGHT, web_angle2, BD);
      CHK_ENTITY_TYPE (light, LIGHT, web_angle3, BD);
      CHK_ENTITY_TYPE (light, LIGHT, web_angle4, BD);
      CHK_ENTITY_TYPE (light, LIGHT, web_angle5, BD);
      CHK_ENTITY_MAX (light, LIGHT, web_angle1, BD, MAX_ANGLE);
      CHK_ENTITY_MAX (light, LIGHT, web_angle2, BD, MAX_ANGLE);
      CHK_ENTITY_MAX (light, LIGHT, web_angle3, BD, MAX_ANGLE);
      CHK_ENTITY_MAX (light, LIGHT, web_angle4, BD, MAX_ANGLE);
      CHK_ENTITY_MAX (light, LIGHT, web_angle5, BD, MAX_ANGLE);
      CHK_ENTITY_TYPE (light, LIGHT, glyph_display_type, BS);
      CHK_ENTITY_MAX (light, LIGHT, glyph_display_type, BS, 2);
    }
}

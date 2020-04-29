#define DWG_TYPE DWG_TYPE_MENTALRAYRENDERSETTINGS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  // AcDbRenderSettings
  BITCODE_BL class_version;
  BITCODE_T name;
  BITCODE_B fog_enabled;
  BITCODE_B fog_background_enabled;
  BITCODE_B backfaces_enabled;
  BITCODE_B environ_image_enabled;
  BITCODE_T environ_image_filename;
  BITCODE_T description;
  BITCODE_BL display_index;
  BITCODE_B has_predefined; 		/*!< DXF 290, r2013 only */
  // AcDbMentalRayRenderSettings
  BITCODE_BL mr_version;		/*!< DXF 90, always 2 */
  BITCODE_BL sampling1;			/*!< DXF 90 */
  BITCODE_BL sampling2;			/*!< DXF 90 */
  BITCODE_BS sampling_mr_filter;	/*!< DXF 70 */
  BITCODE_BD sampling_filter1;		/*!< DXF 40 */
  BITCODE_BD sampling_filter2;		/*!< DXF 40 */
  BITCODE_BD sampling_contrast_color1;	/*!< DXF 40 */
  BITCODE_BD sampling_contrast_color2;	/*!< DXF 40 */
  BITCODE_BD sampling_contrast_color3;	/*!< DXF 40 */
  BITCODE_BD sampling_contrast_color4;	/*!< DXF 40 */
  BITCODE_BS shadow_mode;		/*!< DXF 70 */
  BITCODE_B  shadow_maps_enabled;	/*!< DXF 290 */
  BITCODE_B  ray_tracing_enabled;	/*!< DXF 290 */
  BITCODE_BL ray_trace_depth1;		/*!< DXF 90 */
  BITCODE_BL ray_trace_depth2;		/*!< DXF 90 */
  BITCODE_BL ray_trace_depth3;		/*!< DXF 90 */
  BITCODE_B  global_illumination_enabled;		/*!< DXF 290 */
  BITCODE_BL gi_sample_count;		/*!< DXF 90 */
  BITCODE_B  gi_sample_radius_enabled;		/*!< DXF 290 */
  BITCODE_BD gi_sample_radius;		/*!< DXF 40 */
  BITCODE_BL gi_photons_per_light;	/*!< DXF 90 */
  BITCODE_BL photon_trace_depth1;	/*!< DXF 90 */
  BITCODE_BL photon_trace_depth2;	/*!< DXF 90 */
  BITCODE_BL photon_trace_depth3;	/*!< DXF 90 */
  BITCODE_B  final_gathering_enabled;	/*!< DXF 290 */
  BITCODE_BL fg_ray_count;		/*!< DXF 90 */
  BITCODE_B  fg_sample_radius_state1;	/*!< DXF 290 */
  BITCODE_B  fg_sample_radius_state2;	/*!< DXF 290 */
  BITCODE_B  fg_sample_radius_state3;	/*!< DXF 290 */
  BITCODE_BD fg_sample_radius1;		/*!< DXF 40 */
  BITCODE_BD fg_sample_radius2;		/*!< DXF 40 */
  BITCODE_BD light_luminance_scale;		/*!< DXF 40 */
  BITCODE_BS diagnostics_mode;		/*!< DXF 70 */
  BITCODE_BS diagnostics_grid_mode;	/*!< DXF 70 */
  BITCODE_BD diagnostics_grid_float;	/*!< DXF 40 */
  BITCODE_BS diagnostics_photon_mode;	/*!< DXF 70 */
  BITCODE_BS diagnostics_bsp_mode;	/*!< DXF 70 */
  BITCODE_B  export_mi_enabled;		/*!< DXF 290 */
  BITCODE_T  mr_description;		/*!< DXF 1 */
  BITCODE_BL tile_size;			/*!< DXF 90 */
  BITCODE_BS tile_order;		/*!< DXF 70 */
  BITCODE_BL memory_limit;		/*!< DXF 90 */
  BITCODE_B  diagnostics_samples_mode;	/*!< DXF 290 */
  BITCODE_BD energy_multiplier;		/*!< DXF 40 */

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_mentalrayrendersettings *_obj = dwg_object_to_MENTALRAYRENDERSETTINGS (obj);

  // AcDbRenderSettings
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, class_version, BL, class_version);
  CHK_ENTITY_UTF8TEXT (_obj, MENTALRAYRENDERSETTINGS, name, name);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fog_enabled, B, fog_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fog_background_enabled, B, fog_background_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, backfaces_enabled, B, backfaces_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, environ_image_enabled, B, environ_image_enabled);
  CHK_ENTITY_UTF8TEXT (_obj, MENTALRAYRENDERSETTINGS, environ_image_filename, environ_image_filename);
  CHK_ENTITY_UTF8TEXT (_obj, MENTALRAYRENDERSETTINGS, description, description);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, display_index, BL, display_index);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, has_predefined, B, has_predefined);

  // AcDbMentalRayRenderSettings
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, mr_version, BL, mr_version);
  CHK_ENTITY_MAX (_obj, MENTALRAYRENDERSETTINGS, mr_version, BL, 3); // currently 2
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling1, BL, sampling1);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling2, BL, sampling2);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling_mr_filter, BS, sampling_mr_filter);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling_filter1, BD, sampling_filter1);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling_filter2, BD, sampling_filter2);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling_contrast_color1, BD, sampling_contrast_color1);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling_contrast_color2, BD, sampling_contrast_color2);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling_contrast_color3, BD, sampling_contrast_color3);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, sampling_contrast_color4, BD, sampling_contrast_color4);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, shadow_mode, BS, shadow_mode);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, shadow_maps_enabled, B , shadow_maps_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, ray_tracing_enabled, B , ray_tracing_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, ray_trace_depth1, BL, ray_trace_depth1);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, ray_trace_depth2, BL, ray_trace_depth2);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, ray_trace_depth3, BL, ray_trace_depth3);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, global_illumination_enabled, B , global_illumination_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, gi_sample_count, BL, gi_sample_count);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, gi_sample_radius_enabled, B , gi_sample_radius_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, gi_sample_radius, BD, gi_sample_radius);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, gi_photons_per_light, BL, gi_photons_per_light);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, photon_trace_depth1, BL, photon_trace_depth1);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, photon_trace_depth2, BL, photon_trace_depth2);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, photon_trace_depth3, BL, photon_trace_depth3);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, final_gathering_enabled, B, final_gathering_enabled);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fg_ray_count, BL, fg_ray_count);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fg_sample_radius_state1, B, fg_sample_radius_state1);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fg_sample_radius_state2, B, fg_sample_radius_state2);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fg_sample_radius_state3, B, fg_sample_radius_state3);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fg_sample_radius1, BD, fg_sample_radius1);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, fg_sample_radius2, BD, fg_sample_radius2);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, light_luminance_scale, BD, light_luminance_scale);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, diagnostics_mode, BS, diagnostics_mode);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, diagnostics_grid_mode, BS, diagnostics_grid_mode);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, diagnostics_grid_float, BD, diagnostics_grid_float);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, diagnostics_photon_mode, BS, diagnostics_photon_mode);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, diagnostics_bsp_mode, BS, diagnostics_bsp_mode);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, export_mi_enabled, B , export_mi_enabled);
  CHK_ENTITY_UTF8TEXT (_obj, MENTALRAYRENDERSETTINGS, mr_description, mr_description);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, tile_size, BL, tile_size);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, tile_order, BS, tile_order);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, memory_limit, BL, memory_limit);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, diagnostics_samples_mode, B , diagnostics_samples_mode);
  CHK_ENTITY_TYPE (_obj, MENTALRAYRENDERSETTINGS, energy_multiplier, BD, energy_multiplier);
}

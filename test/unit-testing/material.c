// unstable
#define DWG_TYPE DWG_TYPE_MATERIAL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T name;
  BITCODE_T description;
  BITCODE_BS ambient_color_flag;
  BITCODE_BD ambient_color_factor;
  BITCODE_CMC ambient_color;
  BITCODE_BS diffuse_color_flag;
  BITCODE_BD diffuse_color_factor;
  BITCODE_CMC diffuse_color;
  BITCODE_BS diffusemap_source;
  BITCODE_T diffusemap_filename;
  BITCODE_BD diffusemap_blendfactor;
  BITCODE_BS diffusemap_projection;
  BITCODE_BS diffusemap_tiling;
  BITCODE_BS diffusemap_autotransform;
  BITCODE_BD* diffusemap_transmatrix;
  BITCODE_BD specular_gloss_factor;
  BITCODE_BS specular_color_flag;
  BITCODE_BD specular_color_factor;
  BITCODE_CMC specular_color;
  BITCODE_BS specularmap_source;
  BITCODE_T specularmap_filename;
  BITCODE_BD specularmap_blendfactor;
  BITCODE_BS specularmap_projection;
  BITCODE_BS specularmap_tiling;
  BITCODE_BS specularmap_autotransform;
  BITCODE_BD* specularmap_transmatrix;
  BITCODE_BS reflectionmap_source;
  BITCODE_T reflectionmap_filename;
  BITCODE_BD reflectionmap_blendfactor;
  BITCODE_BS reflectionmap_projection;
  BITCODE_BS reflectionmap_tiling;
  BITCODE_BS reflectionmap_autotransform;
  BITCODE_BD* reflectionmap_transmatrix;
  BITCODE_BD opacity_percent;
  BITCODE_BS opacitymap_source;
  BITCODE_T opacitymap_filename;
  BITCODE_BD opacitymap_blendfactor;
  BITCODE_BS opacitymap_projection;
  BITCODE_BS opacitymap_tiling;
  BITCODE_BS opacitymap_autotransform;
  BITCODE_BD* opacitymap_transmatrix;
  BITCODE_BS bumpmap_source;
  BITCODE_T bumpmap_filename;
  BITCODE_BD bumpmap_blendfactor;
  BITCODE_BS bumpmap_projection;
  BITCODE_BS bumpmap_tiling;
  BITCODE_BS bumpmap_autotransform;
  BITCODE_BD* bumpmap_transmatrix;
  BITCODE_BD refraction_index;
  BITCODE_BS refractionmap_source;
  BITCODE_T refractionmap_filename;
  BITCODE_BD refractionmap_blendfactor;
  BITCODE_BS refractionmap_projection;
  BITCODE_BS refractionmap_tiling;
  BITCODE_BS refractionmap_autotransform;
  BITCODE_BD* refractionmap_transmatrix;
  BITCODE_BD color_bleed_scale;
  BITCODE_BD indirect_bump_scale;
  BITCODE_BD reflectance_scale;
  BITCODE_BD transmittance_scale;
  BITCODE_B two_sided_material;
  BITCODE_BD luminance;
  BITCODE_BS luminance_mode;
  BITCODE_BS normalmap_method;
  BITCODE_BD normalmap_strength;
  BITCODE_BS normalmap_source;
  BITCODE_T normalmap_filename;
  BITCODE_BD normalmap_blendfactor;
  BITCODE_BS normalmap_projection;
  BITCODE_BS normalmap_tiling;
  BITCODE_BS normalmap_autotransform;
  BITCODE_BD* normalmap_transmatrix;
  BITCODE_B is_anonymous;
  BITCODE_BS global_illumination;
  BITCODE_BS final_gather;
  BITCODE_T genprocname;
  BITCODE_B genprocvalbool;
  BITCODE_BS genprocvalint;
  BITCODE_BD genprocvalreal;
  BITCODE_T genprocvaltext;
  BITCODE_B genproctableend;
  BITCODE_CMC genprocvalcolorindex;
  BITCODE_BS genprocvalcolorrgb;
  BITCODE_T genprocvalcolorname;
  BITCODE_BS map_utile;
  BITCODE_BD translucence;
  BITCODE_BL self_illumination;
  BITCODE_BD reflectivity;
  BITCODE_BL illumination_model;
  BITCODE_BL channel_flags;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_obj_material *_obj = dwg_object_to_MATERIAL (obj);

  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, name, name);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, description, description);
  CHK_ENTITY_TYPE (_obj, MATERIAL, ambient_color_flag, BS, ambient_color_flag);
  CHK_ENTITY_TYPE (_obj, MATERIAL, ambient_color_factor, BD, ambient_color_factor);
  CHK_ENTITY_CMC (_obj, MATERIAL, ambient_color, ambient_color);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffuse_color_flag, BS, diffuse_color_flag);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffuse_color_factor, BD, diffuse_color_factor);
  CHK_ENTITY_CMC (_obj, MATERIAL, diffuse_color, diffuse_color);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_source, BS, diffusemap_source);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, diffusemap_filename, diffusemap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_blendfactor, BD, diffusemap_blendfactor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_projection, BS, diffusemap_projection);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_tiling, BS, diffusemap_tiling);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_autotransform, BS, diffusemap_autotransform);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "diffusemap_transmatrix",
                                &diffusemap_transmatrix, NULL) || !diffusemap_transmatrix)
    fail ("MATERIAL.diffusemap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.diffusemap_transmatrix[%d]: %f", i,
            diffusemap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, specular_gloss_factor, BD,
                   specular_gloss_factor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specular_color_flag, BS, specular_color_flag);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specular_color_factor, BD, specular_color_factor);
  CHK_ENTITY_CMC (_obj, MATERIAL, specular_color, specular_color);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_source, BS, specularmap_source);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, specularmap_filename, specularmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_blendfactor, BD, specularmap_blendfactor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_projection, BS, specularmap_projection);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_tiling, BS, specularmap_tiling);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_autotransform, BS, specularmap_autotransform);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "specularmap_transmatrix",
                                &specularmap_transmatrix, NULL) || !specularmap_transmatrix)
    fail ("MATERIAL.specularmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.specularmap_transmatrix[%d]: %f", i,
            specularmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_source, BS, reflectionmap_source);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, reflectionmap_filename, reflectionmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_blendfactor, BD, reflectionmap_blendfactor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_projection, BS, reflectionmap_projection);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_tiling, BS, reflectionmap_tiling);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_autotransform, BS, reflectionmap_autotransform);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "reflectionmap_transmatrix",
                                &reflectionmap_transmatrix, NULL) || !reflectionmap_transmatrix)
    fail ("MATERIAL.reflectionmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.reflectionmap_transmatrix[%d]: %f", i,
            reflectionmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacity_percent, BD, opacity_percent);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_source, BS, opacitymap_source);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, opacitymap_filename, opacitymap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_blendfactor, BD, opacitymap_blendfactor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_projection, BS, opacitymap_projection);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_tiling, BS, opacitymap_tiling);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_autotransform, BS, opacitymap_autotransform);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "opacitymap_transmatrix",
                                &opacitymap_transmatrix, NULL) || !opacitymap_transmatrix)
    fail ("MATERIAL.opacitymap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.opacitymap_transmatrix[%d]: %f", i,
            opacitymap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_source, BS, bumpmap_source);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, bumpmap_filename, bumpmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_blendfactor, BD, bumpmap_blendfactor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_projection, BS, bumpmap_projection);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_tiling, BS, bumpmap_tiling);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_autotransform, BS, bumpmap_autotransform);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "bumpmap_transmatrix",
                                &bumpmap_transmatrix, NULL) || !bumpmap_transmatrix)
    fail ("MATERIAL.bumpmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.bumpmap_transmatrix[%d]: %f", i,
            bumpmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, refraction_index, BD, refraction_index);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_source, BS, refractionmap_source);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, refractionmap_filename, refractionmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_blendfactor, BD, refractionmap_blendfactor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_projection, BS, refractionmap_projection);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_tiling, BS, refractionmap_tiling);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_autotransform, BS, refractionmap_autotransform);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "refractionmap_transmatrix",
                                &refractionmap_transmatrix, NULL) || !refractionmap_transmatrix)
    fail ("MATERIAL.refractionmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.refractionmap_transmatrix[%d]: %f", i,
            refractionmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, color_bleed_scale, BD, color_bleed_scale);
  CHK_ENTITY_TYPE (_obj, MATERIAL, indirect_bump_scale, BD, indirect_bump_scale);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectance_scale, BD, reflectance_scale);
  CHK_ENTITY_TYPE (_obj, MATERIAL, transmittance_scale, BD, transmittance_scale);
  CHK_ENTITY_TYPE (_obj, MATERIAL, two_sided_material, B, two_sided_material);
  CHK_ENTITY_TYPE (_obj, MATERIAL, luminance, BD, luminance);
  CHK_ENTITY_TYPE (_obj, MATERIAL, luminance_mode, BS, luminance_mode);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_method, BS, normalmap_method);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_strength, BD, normalmap_strength);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_source, BS, normalmap_source);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, normalmap_filename, normalmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_blendfactor, BD, normalmap_blendfactor);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_projection, BS, normalmap_projection);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_tiling, BS, normalmap_tiling);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_autotransform, BS, normalmap_autotransform);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "normalmap_transmatrix",
                                &normalmap_transmatrix, NULL) || !normalmap_transmatrix)
    fail ("MATERIAL.normalmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.normalmap_transmatrix[%d]: %f", i,
            normalmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, is_anonymous, B, is_anonymous);
  CHK_ENTITY_TYPE (_obj, MATERIAL, global_illumination_mode, BS, global_illumination);
  CHK_ENTITY_TYPE (_obj, MATERIAL, final_gather_mode, BS, final_gather);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocname, genprocname);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalbool, B, genprocvalbool);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalint, BS, genprocvalint);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalreal, BD, genprocvalreal);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocvaltext, genprocvaltext);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genproctableend, B, genproctableend);
  CHK_ENTITY_CMC (_obj, MATERIAL, genprocvalcolorindex, genprocvalcolorindex);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalcolorrgb, BS, genprocvalcolorrgb);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocvalcolorname, genprocvalcolorname);
  CHK_ENTITY_TYPE (_obj, MATERIAL, map_utile, BS, map_utile);
  CHK_ENTITY_TYPE (_obj, MATERIAL, translucence, BD, translucence);
  CHK_ENTITY_TYPE (_obj, MATERIAL, self_illumination, BL, self_illumination);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectivity, BD, reflectivity);
  CHK_ENTITY_TYPE (_obj, MATERIAL, illumination_model, BL, illumination_model);
  CHK_ENTITY_TYPE (_obj, MATERIAL, channel_flags, BL, channel_flags);
#endif
}

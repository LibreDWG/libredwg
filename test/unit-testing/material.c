// unstable
#define DWG_TYPE DWG_TYPE_MATERIAL
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_T name;
  BITCODE_T description;
  Dwg_MATERIAL_color ambient_color;
  Dwg_MATERIAL_color diffuse_color;
  Dwg_MATERIAL_mapper diffusemap;
  BITCODE_BD specular_gloss_factor;
  Dwg_MATERIAL_color specular_color;
  Dwg_MATERIAL_mapper specularmap;
  Dwg_MATERIAL_mapper reflectionmap;
  BITCODE_BD opacity_percent;
  Dwg_MATERIAL_mapper opacitymap;
  Dwg_MATERIAL_mapper bumpmap;
  BITCODE_BD refraction_index;
  Dwg_MATERIAL_mapper refractionmap;
  BITCODE_BD color_bleed_scale;
  BITCODE_BD indirect_bump_scale;
  BITCODE_BD reflectance_scale;
  BITCODE_BD transmittance_scale;
  BITCODE_B two_sided_material;
  BITCODE_BD luminance;
  BITCODE_BS luminance_mode;
  BITCODE_BD translucence;
  BITCODE_BD self_illumination;
  BITCODE_BD reflectivity;
  BITCODE_BL illumination_model;
  BITCODE_BL channel_flags;
  BITCODE_BL mode;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_material *_obj = dwg_object_to_MATERIAL (obj);

  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, name, name);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, description, description);

  CHK_SUBCLASS_TYPE (ambient_color, MATERIAL_color, flag, BS);
  CHK_SUBCLASS_TYPE (ambient_color, MATERIAL_color, factor, BD);
  CHK_SUBCLASS_TYPE (ambient_color, MATERIAL_color, rgb, BL);
  /*
  CHK_ENTITY_TYPE (_obj, MATERIAL, ambient_color_flag, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, ambient_color_factor, BD);
  CHK_ENTITY_CMC (_obj, MATERIAL, ambient_color, ambient_color);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffuse_color_flag, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffuse_color_factor, BD);
  CHK_ENTITY_CMC (_obj, MATERIAL, diffuse_color, diffuse_color);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_source, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, diffusemap_filename, diffusemap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_blendfactor, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_projection, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_tiling, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, diffusemap_autotransform, BS);
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
  CHK_ENTITY_TYPE (_obj, MATERIAL, specular_color_flag, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specular_color_factor, BD);
  CHK_ENTITY_CMC (_obj, MATERIAL, specular_color, specular_color);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_source, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, specularmap_filename, specularmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_blendfactor, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_projection, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_tiling, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specularmap_autotransform, BS);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "specularmap_transmatrix",
                                &specularmap_transmatrix, NULL) || !specularmap_transmatrix)
    fail ("MATERIAL.specularmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.specularmap_transmatrix[%d]: %f", i,
            specularmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_source, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, reflectionmap_filename, reflectionmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_blendfactor, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_projection, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_tiling, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectionmap_autotransform, BS);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "reflectionmap_transmatrix",
                                &reflectionmap_transmatrix, NULL) || !reflectionmap_transmatrix)
    fail ("MATERIAL.reflectionmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.reflectionmap_transmatrix[%d]: %f", i,
            reflectionmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacity_percent, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_source, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, opacitymap_filename, opacitymap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_blendfactor, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_projection, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_tiling, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacitymap_autotransform, BS);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "opacitymap_transmatrix",
                                &opacitymap_transmatrix, NULL) || !opacitymap_transmatrix)
    fail ("MATERIAL.opacitymap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.opacitymap_transmatrix[%d]: %f", i,
            opacitymap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_source, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, bumpmap_filename, bumpmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_blendfactor, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_projection, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_tiling, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, bumpmap_autotransform, BS);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "bumpmap_transmatrix",
                                &bumpmap_transmatrix, NULL) || !bumpmap_transmatrix)
    fail ("MATERIAL.bumpmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.bumpmap_transmatrix[%d]: %f", i,
            bumpmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, refraction_index, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_source, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, refractionmap_filename, refractionmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_blendfactor, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_projection, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_tiling, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, refractionmap_autotransform, BS);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "refractionmap_transmatrix",
                                &refractionmap_transmatrix, NULL) || !refractionmap_transmatrix)
    fail ("MATERIAL.refractionmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.refractionmap_transmatrix[%d]: %f", i,
            refractionmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, color_bleed_scale, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, indirect_bump_scale, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectance_scale, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, transmittance_scale, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, two_sided_material, B);
  CHK_ENTITY_TYPE (_obj, MATERIAL, luminance, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, luminance_mode, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_method, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_strength, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_source, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, normalmap_filename, normalmap_filename);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_blendfactor, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_projection, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_tiling, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, normalmap_autotransform, BS);
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", "normalmap_transmatrix",
                                &normalmap_transmatrix, NULL) || !normalmap_transmatrix)
    fail ("MATERIAL.normalmap_transmatrix");
  else
    for (int i = 0; i < 16; i++)
      {
        ok ("MATERIAL.normalmap_transmatrix[%d]: %f", i,
            normalmap_transmatrix[i]);
      }
  CHK_ENTITY_TYPE (_obj, MATERIAL, is_anonymous, B);
  CHK_ENTITY_TYPE (_obj, MATERIAL, global_illumination_mode, BS, global_illumination);
  CHK_ENTITY_TYPE (_obj, MATERIAL, final_gather_mode, BS, final_gather);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocname, genprocname);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalbool, B);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalint, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalreal, BD);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocvaltext, genprocvaltext);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genproctableend, B);
  CHK_ENTITY_CMC (_obj, MATERIAL, genprocvalcolorindex, genprocvalcolorindex);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalcolorrgb, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocvalcolorname, genprocvalcolorname);
  */
  CHK_ENTITY_TYPE (_obj, MATERIAL, translucence, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, self_illumination, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, reflectivity, BD);
  CHK_ENTITY_TYPE (_obj, MATERIAL, illumination_model, BL);
  CHK_ENTITY_TYPE (_obj, MATERIAL, channel_flags, BL);
  CHK_ENTITY_TYPE (_obj, MATERIAL, mode, BL);
}

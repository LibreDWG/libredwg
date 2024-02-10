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
  BITCODE_T genprocname;
  BITCODE_BS genproctype;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_material *_obj = dwg_object_to_MATERIAL (obj);

  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, name);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, description);

#define CHK_MATERIAL_COLOR(ambient_color)                                     \
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", #ambient_color,             \
                                &ambient_color, NULL))                        \
    fail (#ambient_color);                                                    \
  CHK_SUBCLASS_TYPE (ambient_color, MATERIAL_color, flag, RC);                \
  CHK_SUBCLASS_TYPE (ambient_color, MATERIAL_color, factor, BD);              \
  if (ambient_color.flag == 1)                                                \
  CHK_SUBCLASS_TYPE (ambient_color, MATERIAL_color, rgb, BLx)

  CHK_MATERIAL_COLOR (ambient_color);
  CHK_MATERIAL_COLOR (diffuse_color);

#define CHK_MATERIAL_MAP(diffusemap)                                          \
  if (!dwg_dynapi_entity_value (_obj, "MATERIAL", #diffusemap, &diffusemap,   \
                                NULL))                                        \
    fail (#diffusemap);                                                       \
  CHK_SUBCLASS_TYPE (diffusemap, MATERIAL_mapper, blendfactor, BD);           \
  CHK_SUBCLASS_TYPE (diffusemap, MATERIAL_mapper, projection, RC);            \
  CHK_SUBCLASS_TYPE (diffusemap, MATERIAL_mapper, tiling, RC);                \
  CHK_SUBCLASS_TYPE (diffusemap, MATERIAL_mapper, autotransform, RC);         \
  CHK_SUBCLASS_VECTOR_TYPE (diffusemap, MATERIAL_mapper, transmatrix, 16,     \
                            BD);                                              \
  CHK_SUBCLASS_TYPE (diffusemap, MATERIAL_mapper, source, RC);                \
  if (diffusemap.source == 1)                                                 \
    CHK_SUBCLASS_UTF8TEXT (diffusemap, MATERIAL_mapper, filename);            \
  if (diffusemap.source == 2)                                                 \
    {                                                                         \
      CHK_SUBCLASS_TYPE (diffusemap, MATERIAL_mapper, texturemode, BS);       \
      if (diffusemap.texturemode < 2) /* not procedural */                    \
        {                                                                     \
          CHK_SUBCLASS_TYPE (diffusemap.color1, MATERIAL_color, flag, RC);    \
          CHK_SUBCLASS_TYPE (diffusemap.color1, MATERIAL_color, factor, BD);  \
          if (_obj->diffusemap.color1.flag == 1)                              \
            CHK_SUBCLASS_TYPE (diffusemap.color1, MATERIAL_color, rgb, BLx);  \
          CHK_SUBCLASS_TYPE (diffusemap.color2, MATERIAL_color, flag, RC);    \
          CHK_SUBCLASS_TYPE (diffusemap.color2, MATERIAL_color, factor, BD);  \
          if (_obj->diffusemap.color2.flag == 1)                              \
            CHK_SUBCLASS_TYPE (diffusemap.color2, MATERIAL_color, rgb, BLx);  \
        }                                                                     \
      else                                                                    \
        {                                                                     \
          CHK_ENTITY_TYPE (_obj, MATERIAL, genproctype, BS);                  \
          /* TODO procedural...*/                                             \
        }                                                                     \
    }

  CHK_MATERIAL_MAP (diffusemap)
  CHK_MATERIAL_COLOR (specular_color);
  CHK_ENTITY_TYPE (_obj, MATERIAL, specular_gloss_factor, BD);
  CHK_MATERIAL_MAP (specularmap)
  CHK_MATERIAL_MAP (reflectionmap)
  CHK_ENTITY_TYPE (_obj, MATERIAL, opacity_percent, BD);
  CHK_MATERIAL_MAP (opacitymap)
  CHK_MATERIAL_MAP (bumpmap)
  CHK_ENTITY_TYPE (_obj, MATERIAL, refraction_index, BD);
  CHK_MATERIAL_MAP (refractionmap)

#if 0
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
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, normalmap_filename);
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
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocname);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalbool, B);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalint, BS);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalreal, BD);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocvaltext);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genproctableend, B);
  CHK_ENTITY_CMC (_obj, MATERIAL, genprocvalcolorindex);
  CHK_ENTITY_TYPE (_obj, MATERIAL, genprocvalcolorrgb, BS);
  CHK_ENTITY_UTF8TEXT (_obj, MATERIAL, genprocvalcolorname);
#endif

  SINCE (R_2007a)
  {
    CHK_ENTITY_TYPE (_obj, MATERIAL, translucence, BD);
    CHK_ENTITY_TYPE (_obj, MATERIAL, self_illumination, BD);
    CHK_ENTITY_TYPE (_obj, MATERIAL, reflectivity, BD);
    CHK_ENTITY_TYPE (_obj, MATERIAL, illumination_model, BL);
    CHK_ENTITY_TYPE (_obj, MATERIAL, channel_flags, BL);
    CHK_ENTITY_TYPE (_obj, MATERIAL, mode, BL);
  }
}

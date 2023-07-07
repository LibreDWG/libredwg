// TODO coverage
#define DWG_TYPE DWG_TYPE_POINTCLOUDEX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BITCODE_BS class_version;
  BITCODE_3BD extents_min;
  BITCODE_3BD extents_max;
  BITCODE_3BD ucs_origin;
  BITCODE_3BD ucs_x_dir;
  BITCODE_3BD ucs_y_dir;
  BITCODE_3BD ucs_z_dir;
  BITCODE_B is_locked;
  BITCODE_H pointclouddefex;
  BITCODE_H reactor;
  BITCODE_T name;
  BITCODE_B show_intensity;

  BITCODE_BS stylization_type;
  BITCODE_T intensity_colorscheme;
  BITCODE_T cur_colorscheme;
  BITCODE_T classification_colorscheme;
  BITCODE_BD elevation_min;
  BITCODE_BD elevation_max;
  BITCODE_BL intensity_min;
  BITCODE_BL intensity_max;
  BITCODE_BS intensity_out_of_range_behavior;
  BITCODE_BS elevation_out_of_range_behavior;
  BITCODE_B elevation_apply_to_fixed_range;
  BITCODE_B intensity_as_gradient;
  BITCODE_B elevation_as_gradient;
  BITCODE_B show_cropping;
  BITCODE_BL unknown_bl0;
  BITCODE_BL unknown_bl1;
  BITCODE_BL num_croppings;
  Dwg_POINTCLOUDEX_Croppings *croppings;
  BITCODE_3BD *vertices;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_ent_pointcloudex *_obj = dwg_object_to_POINTCLOUDEX (obj);

  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, class_version, BS);
  CHK_ENTITY_MAX (_obj, POINTCLOUDEX, class_version, BS, 2);
  CHK_ENTITY_3RD (_obj, POINTCLOUDEX, extents_min);
  CHK_ENTITY_3RD (_obj, POINTCLOUDEX, extents_max);
  CHK_ENTITY_3RD (_obj, POINTCLOUDEX, ucs_origin);
  CHK_ENTITY_3RD (_obj, POINTCLOUDEX, ucs_x_dir);
  CHK_ENTITY_3RD (_obj, POINTCLOUDEX, ucs_y_dir);
  CHK_ENTITY_3RD (_obj, POINTCLOUDEX, ucs_z_dir);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, is_locked, B);
  CHK_ENTITY_H (_obj, POINTCLOUDEX, pointclouddefex);
  CHK_ENTITY_H (_obj, POINTCLOUDEX, reactor);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDEX, name);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, show_intensity, B);

  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, stylization_type, BS);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDEX, intensity_colorscheme);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDEX, cur_colorscheme);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDEX, classification_colorscheme);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, elevation_min, BD);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, elevation_max, BD);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, intensity_min, BL);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, intensity_max, BL);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, intensity_out_of_range_behavior, BS);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, elevation_out_of_range_behavior, BS);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, elevation_apply_to_fixed_range, B);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, intensity_as_gradient, B);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, elevation_as_gradient, B);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, show_cropping, B);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, unknown_bl0, BL);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, unknown_bl1, BL);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDEX, num_croppings, BL);
  if (!dwg_dynapi_entity_value (_obj, "POINTCLOUDEX", "croppings", &croppings,
                                NULL))
    fail ("POINTCLOUDEX.croppings");
  else
    for (i = 0; i < num_croppings; i++)
      {
        CHK_SUBCLASS_TYPE (croppings[i], POINTCLOUDEX_Croppings, type, BS);
        CHK_SUBCLASS_TYPE (croppings[i], POINTCLOUDEX_Croppings, is_inside, B);
        CHK_SUBCLASS_TYPE (croppings[i], POINTCLOUDEX_Croppings, is_inverted,
                           B);
        CHK_SUBCLASS_3RD (croppings[i], POINTCLOUDEX_Croppings, crop_plane);
        CHK_SUBCLASS_3RD (croppings[i], POINTCLOUDEX_Croppings, crop_x_dir);
        CHK_SUBCLASS_3RD (croppings[i], POINTCLOUDEX_Croppings, crop_y_dir);
        CHK_SUBCLASS_TYPE (croppings[i], POINTCLOUDEX_Croppings, num_pts, BL);
        CHK_SUBCLASS_3DPOINTS (croppings[i], POINTCLOUDEX_Croppings, pts,
                               croppings[i].num_pts);
      }
  // #endif
}

// TODO
#define DWG_TYPE DWG_TYPE_POINTCLOUDCOLORMAP
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BITCODE_BS class_version;
  BITCODE_T def_intensity_colorscheme;
  BITCODE_T def_elevation_colorscheme;
  BITCODE_T def_classification_colorscheme;
  BITCODE_BL num_colorramps;
  Dwg_POINTCLOUDCOLORMAP_Ramp *colorramps;
  BITCODE_BL num_classification_colorramps;
  Dwg_POINTCLOUDCOLORMAP_Ramp *classification_colorramps;
  Dwg_ColorRamp *ramps;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_obj_pointcloudcolormap *_obj = dwg_object_to_POINTCLOUDCOLORMAP (obj);

  CHK_ENTITY_TYPE (_obj, POINTCLOUDCOLORMAP, class_version, BS);
  CHK_ENTITY_MAX (_obj, POINTCLOUDCOLORMAP, class_version, BS, 3);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDCOLORMAP, def_intensity_colorscheme);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDCOLORMAP, def_elevation_colorscheme);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUDCOLORMAP,
                       def_classification_colorscheme);
  CHK_ENTITY_TYPE (_obj, POINTCLOUDCOLORMAP, num_colorramps, BL);
  if (!dwg_dynapi_entity_value (_obj, "POINTCLOUDCOLORMAP", "colorramps",
                                &colorramps, NULL))
    fail ("POINTCLOUDCOLORMAP.colorramps");
  else
    for (i = 0; i < num_colorramps; i++)
      {
        CHK_SUBCLASS_TYPE (colorramps[i], POINTCLOUDCOLORMAP_Ramp,
                           class_version, BS);
        CHK_SUBCLASS_TYPE (colorramps[i], POINTCLOUDCOLORMAP_Ramp, num_ramps,
                           BL);
        if (!dwg_dynapi_subclass_value (&colorramps[i],
                                        "POINTCLOUDCOLORMAP_Ramp", "ramps",
                                        &ramps, NULL))
          fail ("POINTCLOUDCOLORMAP_Ramp.ramps");
        else
          for (unsigned j = 0; j < colorramps[i].num_ramps; j++)
            {
              CHK_SUBCLASS_UTF8TEXT (ramps[j], ColorRamp, colorscheme);
              CHK_SUBCLASS_TYPE (ramps[j], ColorRamp, unknown_bl, BL);
              CHK_SUBCLASS_TYPE (ramps[j], ColorRamp, unknown_b, B);
            }
      }

  CHK_ENTITY_TYPE (_obj, POINTCLOUDCOLORMAP, num_classification_colorramps,
                   BL);
  if (!dwg_dynapi_entity_value (_obj, "POINTCLOUDCOLORMAP",
                                "classification_colorramps",
                                &classification_colorramps, NULL))
    fail ("POINTCLOUDCOLORMAP.classification_colorramps");
  else
    for (i = 0; i < num_classification_colorramps; i++)
      {
        CHK_SUBCLASS_TYPE (classification_colorramps[i],
                           POINTCLOUDCOLORMAP_Ramp, class_version, BS);
        CHK_SUBCLASS_TYPE (classification_colorramps[i],
                           POINTCLOUDCOLORMAP_Ramp, num_ramps, BL);
        if (!dwg_dynapi_subclass_value (&classification_colorramps[i],
                                        "POINTCLOUDCOLORMAP_Ramp", "ramps",
                                        &ramps, NULL))
          fail ("POINTCLOUDCOLORMAP_Ramp.ramps");
        else
          for (unsigned j = 0; j < classification_colorramps[i].num_ramps; j++)
            {
              CHK_SUBCLASS_UTF8TEXT (ramps[j], ColorRamp, colorscheme);
              CHK_SUBCLASS_TYPE (ramps[j], ColorRamp, unknown_bl, BL);
              CHK_SUBCLASS_TYPE (ramps[j], ColorRamp, unknown_b, B);
            }
      }
  // #endif
}

// TODO coverage
#define DWG_TYPE DWG_TYPE_POINTCLOUD
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BITCODE_BS class_version;
  BITCODE_3BD origin;
  BITCODE_T saved_filename;
  BITCODE_BL num_source_files;
  BITCODE_T *source_files;
  BITCODE_3BD extents_min;
  BITCODE_3BD extents_max;
  BITCODE_RLL numpoints;
  BITCODE_T ucs_name;
  BITCODE_3BD ucs_origin;
  BITCODE_3BD ucs_x_dir;
  BITCODE_3BD ucs_y_dir;
  BITCODE_3BD ucs_z_dir;
  // r2013+:
  BITCODE_H pointclouddef;
  BITCODE_H reactor;
  BITCODE_B show_intensity;
  BITCODE_BS intensity_scheme;
  Dwg_POINTCLOUD_IntensityStyle intensity_style;
  BITCODE_B show_clipping;
  BITCODE_BL num_clippings;
  Dwg_POINTCLOUD_Clippings *clippings;
  BITCODE_2RD *vertices;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  // #ifdef DEBUG_CLASSES
  dwg_ent_pointcloud *_obj = dwg_object_to_POINTCLOUD (obj);

  CHK_ENTITY_TYPE (_obj, POINTCLOUD, class_version, BS);
  CHK_ENTITY_MAX (_obj, POINTCLOUD, class_version, BS, 3);
  CHK_ENTITY_3RD (_obj, POINTCLOUD, origin);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUD, saved_filename);
  CHK_ENTITY_TYPE (_obj, POINTCLOUD, num_source_files, BL);
  if (!dwg_dynapi_entity_value (_obj, "POINTCLOUD", "source_files",
                                &source_files, NULL))
    fail ("POINTCLOUD.source_files");
  else
    for (i = 0; i < num_source_files; i++)
      {
        ok ("POINTCLOUD.source_files[%u]: %s", i, source_files[i]);
      }
  // if !num_source_files
  CHK_ENTITY_3RD (_obj, POINTCLOUD, extents_min);
  CHK_ENTITY_3RD (_obj, POINTCLOUD, extents_max);
  CHK_ENTITY_TYPE (_obj, POINTCLOUD, numpoints, RLL);
  CHK_ENTITY_UTF8TEXT (_obj, POINTCLOUD, ucs_name);
  CHK_ENTITY_3RD (_obj, POINTCLOUD, ucs_origin);
  CHK_ENTITY_3RD (_obj, POINTCLOUD, ucs_x_dir);
  CHK_ENTITY_3RD (_obj, POINTCLOUD, ucs_y_dir);
  CHK_ENTITY_3RD (_obj, POINTCLOUD, ucs_z_dir);
  SINCE (R_2013)
  {
    CHK_ENTITY_H (_obj, POINTCLOUD, pointclouddef);
    CHK_ENTITY_H (_obj, POINTCLOUD, reactor);
    CHK_ENTITY_TYPE (_obj, POINTCLOUD, show_intensity, B);
    CHK_ENTITY_TYPE (_obj, POINTCLOUD, intensity_scheme, BS);
    CHK_SUBCLASS_TYPE (intensity_style, POINTCLOUD_IntensityStyle,
                       min_intensity, BD);
    CHK_SUBCLASS_TYPE (intensity_style, POINTCLOUD_IntensityStyle,
                       max_intensity, BD);
    CHK_SUBCLASS_TYPE (intensity_style, POINTCLOUD_IntensityStyle,
                       intensity_low_treshold, BD);
    CHK_SUBCLASS_TYPE (intensity_style, POINTCLOUD_IntensityStyle,
                       intensity_high_treshold, BD);
    CHK_ENTITY_TYPE (_obj, POINTCLOUD, show_clipping, B);
    CHK_ENTITY_TYPE (_obj, POINTCLOUD, num_clippings, BL);
    if (!dwg_dynapi_entity_value (_obj, "POINTCLOUD", "clippings", &clippings,
                                  NULL))
      fail ("POINTCLOUD.clippings");
    else
      for (i = 0; i < num_clippings; i++)
        {
          CHK_SUBCLASS_TYPE (clippings[i], POINTCLOUD_Clippings, is_inverted,
                             B);
          CHK_SUBCLASS_TYPE (clippings[i], POINTCLOUD_Clippings, type, BS);
          CHK_SUBCLASS_TYPE (clippings[i], POINTCLOUD_Clippings, num_vertices,
                             BL);
          CHK_SUBCLASS_2DPOINTS (clippings[i], POINTCLOUD_Clippings, vertices,
                                 clippings[i].num_vertices);
          CHK_SUBCLASS_TYPE (clippings[i], POINTCLOUD_Clippings, z_min, BD);
          CHK_SUBCLASS_TYPE (clippings[i], POINTCLOUD_Clippings, z_max, BD);
        }
  }
  // #endif
}

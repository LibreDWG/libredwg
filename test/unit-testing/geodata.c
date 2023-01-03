#define DWG_TYPE DWG_TYPE_GEODATA
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BITCODE_BL class_version;
  BITCODE_H host_block;
  BITCODE_BS coord_type; /* 0 unknown, 1 local grid, 2 projected grid,
                            3 geographic defined by latitude/longitude) */
  BITCODE_3BD design_pt;
  BITCODE_3BD ref_pt;
  BITCODE_2RD ref_pt2d; // x reversed with y
  BITCODE_3BD obs_pt;
  BITCODE_BD unit_scale_horiz;
  BITCODE_BL units_value_horiz;
  BITCODE_BD unit_scale_vert;
  BITCODE_BL units_value_vert;
  BITCODE_3BD up_dir;
  BITCODE_3BD north_dir;
  BITCODE_BL scale_est; /* None = 1, User specified scale factor = 2,
                           Grid scale at reference point = 3, Prismodial = 4 */
  BITCODE_BD user_scale_factor;
  BITCODE_B do_sea_level_corr;
  BITCODE_BD sea_level_elev;
  BITCODE_BD coord_proj_radius;
  BITCODE_T coord_system_def;
  BITCODE_T geo_rss_tag;
  BITCODE_T coord_system_datum; /* obsolete */
  BITCODE_T coord_system_wkt;   /* obsolete */
  BITCODE_T observation_from_tag;
  BITCODE_T observation_to_tag;
  BITCODE_T observation_coverage_tag;
  BITCODE_BL num_geomesh_pts;
  Dwg_GEODATA_meshpt *geomesh_pts;
  BITCODE_BL num_geomesh_faces;
  Dwg_GEODATA_meshface *geomesh_faces;
  BITCODE_B has_civil_data;
  BITCODE_B obsolete_false;
  BITCODE_3BD zero1, zero2;
  BITCODE_BL unknown1;
  BITCODE_BL unknown2;
  BITCODE_B unknown_b;
  BITCODE_BD north_dir_angle_deg;
  BITCODE_BD north_dir_angle_rad;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_geodata *geodata = dwg_object_to_GEODATA (obj);

  CHK_ENTITY_TYPE (geodata, GEODATA, class_version, BL);
  CHK_ENTITY_H (geodata, GEODATA, host_block);
  CHK_ENTITY_TYPE (geodata, GEODATA, coord_type, BS);
  /* 0 unknown, 1 local grid, 2 projected grid,
     3 geographic defined by latitude/longitude) */
  CHK_ENTITY_3RD (geodata, GEODATA, design_pt);
  CHK_ENTITY_3RD (geodata, GEODATA, ref_pt);
  CHK_ENTITY_3RD (geodata, GEODATA, obs_pt);
  CHK_ENTITY_TYPE (geodata, GEODATA, unit_scale_horiz, BD);
  CHK_ENTITY_3RD (geodata, GEODATA, up_dir);
  CHK_ENTITY_3RD (geodata, GEODATA, north_dir);
  CHK_ENTITY_TYPE (geodata, GEODATA, scale_est, BL);
  /* None = 1, User specified scale factor = 2,
     Grid scale at reference point = 3, Prismodial = 4 */
  if (geodata->scale_est > 4)
    fail ("Invalid GEODATA.scale_est " FORMAT_BL " > 4", geodata->scale_est);
  CHK_ENTITY_TYPE (geodata, GEODATA, user_scale_factor, BD);
  CHK_ENTITY_TYPE (geodata, GEODATA, do_sea_level_corr, B);
  CHK_ENTITY_TYPE (geodata, GEODATA, sea_level_elev, BD);
  CHK_ENTITY_TYPE (geodata, GEODATA, coord_proj_radius, BD);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, coord_system_def);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, geo_rss_tag);

  if (class_version >= 2)
    {
      CHK_ENTITY_TYPE (geodata, GEODATA, unit_scale_vert, BD);
      CHK_ENTITY_TYPE (geodata, GEODATA, units_value_vert, BL);
    }
  else
    {
      CHK_ENTITY_TYPE (geodata, GEODATA, units_value_horiz, BL);
      CHK_ENTITY_UTF8TEXT (geodata, GEODATA,
                           coord_system_datum);                 /* obsolete */
      CHK_ENTITY_UTF8TEXT (geodata, GEODATA, coord_system_wkt); /* obsolete */
      CHK_ENTITY_TYPE (geodata, GEODATA, has_civil_data, B);
      CHK_ENTITY_TYPE (geodata, GEODATA, obsolete_false, B);
      CHK_ENTITY_TYPE (geodata, GEODATA, north_dir_angle_deg, BD);
      CHK_ENTITY_MAX (geodata, GEODATA, north_dir_angle_deg, BD, 360.0);
      CHK_ENTITY_TYPE (geodata, GEODATA, north_dir_angle_rad, BD);
      CHK_ENTITY_MAX (geodata, GEODATA, north_dir_angle_rad, BD, MAX_ANGLE);
    }

  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, observation_from_tag);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, observation_to_tag);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, observation_coverage_tag);
  CHK_ENTITY_TYPE (geodata, GEODATA, num_geomesh_pts, BL);
  if (!dwg_dynapi_entity_value (geodata, "GEODATA", "geomesh_pts",
                                &geomesh_pts, NULL))
    fail ("GEODATA.geomesh_pts");
  if (num_geomesh_pts)
    {
      if (!geomesh_pts)
        fail ("HATCH.geomesh_pts");
      else
        for (i = 0; i < num_geomesh_pts; i++)
          {
            CHK_SUBCLASS_2RD (geomesh_pts[i], GEODATA_meshpt, source_pt);
            CHK_SUBCLASS_2RD (geomesh_pts[i], GEODATA_meshpt, dest_pt);
          }
    }
  CHK_ENTITY_TYPE (geodata, GEODATA, num_geomesh_faces, BL);
  if (!dwg_dynapi_entity_value (geodata, "GEODATA", "geomesh_faces",
                                &geomesh_faces, NULL))
    fail ("GEODATA.geomesh_faces");
  if (num_geomesh_faces)
    {
      if (!geomesh_faces)
        fail ("HATCH.geomesh_faces");
      else
        for (i = 0; i < num_geomesh_faces; i++)
          {
            CHK_SUBCLASS_TYPE (geomesh_faces[i], GEODATA_meshface, face1, BL);
            CHK_SUBCLASS_TYPE (geomesh_faces[i], GEODATA_meshface, face2, BL);
            CHK_SUBCLASS_TYPE (geomesh_faces[i], GEODATA_meshface, face3, BL);
          }
    }

  UNTIL (R_2007)
  {
    CHK_ENTITY_TYPE (geodata, GEODATA, has_civil_data, B);
    CHK_ENTITY_TYPE (geodata, GEODATA, obsolete_false, B);
    CHK_ENTITY_2RD (geodata, GEODATA, ref_pt2d);
    CHK_ENTITY_TYPE (geodata, GEODATA, unknown1, BL);
    CHK_ENTITY_TYPE (geodata, GEODATA, unknown2, BL);
    CHK_ENTITY_3RD (geodata, GEODATA, zero1);
    CHK_ENTITY_3RD (geodata, GEODATA, zero2);
    CHK_ENTITY_TYPE (geodata, GEODATA, unknown_b, B);
    CHK_ENTITY_TYPE (geodata, GEODATA, north_dir_angle_deg, BD);
    CHK_ENTITY_TYPE (geodata, GEODATA, north_dir_angle_rad, BD);
    CHK_ENTITY_TYPE (geodata, GEODATA, scale_est, BL);
    CHK_ENTITY_TYPE (geodata, GEODATA, user_scale_factor, BD);
    CHK_ENTITY_TYPE (geodata, GEODATA, do_sea_level_corr, B);
    CHK_ENTITY_TYPE (geodata, GEODATA, sea_level_elev, BD);
    CHK_ENTITY_TYPE (geodata, GEODATA, coord_proj_radius, BD);
  }
}

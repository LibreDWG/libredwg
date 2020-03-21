// TODO unstable
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
  BITCODE_B sea_level_corr;
  BITCODE_BD sea_level_elev;
  BITCODE_BD coord_proj_radius;
  BITCODE_T coord_system_def;
  BITCODE_T geo_rss_tag;
  BITCODE_T coord_system_datum; /* obsolete */
  BITCODE_T coord_system_wkt; /* obsolete */
  BITCODE_T observation_from_tag;
  BITCODE_T observation_to_tag;
  BITCODE_T observation_coverage_tag;
  BITCODE_BL num_geomesh_pts;
  Dwg_GEODATA_meshpt *geomesh_pts;
  BITCODE_BL num_geomesh_faces;
  Dwg_GEODATA_meshface *geomesh_faces;
  BITCODE_B has_civil_data;
  BITCODE_B obsolete_false;
  BITCODE_BD north_dir_angle_deg;
  BITCODE_BD north_dir_angle_rad;

  dwg_obj_geodata *geodata = dwg_object_to_GEODATA (obj);

  CHK_ENTITY_TYPE (geodata, GEODATA, class_version, BL, class_version);
  CHK_ENTITY_H (geodata, GEODATA, host_block, host_block);
  CHK_ENTITY_TYPE (geodata, GEODATA, coord_type, BS, coord_type);
  /* 0 unknown, 1 local grid, 2 projected grid,
     3 geographic defined by latitude/longitude) */
  CHK_ENTITY_3RD (geodata, GEODATA, design_pt, design_pt);
  CHK_ENTITY_3RD (geodata, GEODATA, ref_pt, ref_pt);
  CHK_ENTITY_3RD (geodata, GEODATA, obs_pt, obs_pt);
  CHK_ENTITY_TYPE (geodata, GEODATA, unit_scale_horiz, BD, unit_scale_horiz);
  CHK_ENTITY_3RD (geodata, GEODATA, up_dir, up_dir);
  CHK_ENTITY_3RD (geodata, GEODATA, north_dir, north_dir);
  CHK_ENTITY_TYPE (geodata, GEODATA, scale_est, BL, scale_est);
  /* None = 1, User specified scale factor = 2,
     Grid scale at reference point = 3, Prismodial = 4 */
  if (geodata->scale_est > 4)
    fail ("Invalid GEODATA.scale_est " FORMAT_BL " > 4", geodata->scale_est);
  CHK_ENTITY_TYPE (geodata, GEODATA, user_scale_factor, BD, user_scale_factor);
  CHK_ENTITY_TYPE (geodata, GEODATA, sea_level_corr, B, sea_level_corr);
  CHK_ENTITY_TYPE (geodata, GEODATA, sea_level_elev, BD, sea_level_elev);
  CHK_ENTITY_TYPE (geodata, GEODATA, coord_proj_radius, BD, coord_proj_radius);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, coord_system_def, coord_system_def);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, geo_rss_tag, geo_rss_tag);

  if (class_version >= 2)
    {
      CHK_ENTITY_TYPE (geodata, GEODATA, unit_scale_vert, BD, unit_scale_vert);
      CHK_ENTITY_TYPE (geodata, GEODATA, units_value_vert, BL, units_value_vert);
    }
  else
    {
      CHK_ENTITY_TYPE (geodata, GEODATA, units_value_horiz, BL, units_value_horiz);
      CHK_ENTITY_UTF8TEXT (geodata, GEODATA, coord_system_datum, coord_system_datum); /* obsolete */
      CHK_ENTITY_UTF8TEXT (geodata, GEODATA, coord_system_wkt, coord_system_wkt); /* obsolete */
      CHK_ENTITY_TYPE (geodata, GEODATA, has_civil_data, B, has_civil_data);
      CHK_ENTITY_TYPE (geodata, GEODATA, obsolete_false, B, obsolete_false);
      CHK_ENTITY_TYPE (geodata, GEODATA, north_dir_angle_deg, BD, north_dir_angle_deg);
      CHK_ENTITY_MAX (geodata, GEODATA, north_dir_angle_deg, BD, 360.0);
      CHK_ENTITY_TYPE (geodata, GEODATA, north_dir_angle_rad, BD, north_dir_angle_rad);
      CHK_ENTITY_MAX (geodata, GEODATA, north_dir_angle_rad, BD, MAX_ANGLE);
    }

  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, observation_from_tag, observation_from_tag);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, observation_to_tag, observation_to_tag);
  CHK_ENTITY_UTF8TEXT (geodata, GEODATA, observation_coverage_tag, observation_coverage_tag);
  CHK_ENTITY_TYPE (geodata, GEODATA, num_geomesh_pts, BL, num_geomesh_pts);
  if (!dwg_dynapi_entity_value (geodata, "GEODATA", "geomesh_pts", &geomesh_pts, NULL))
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
  CHK_ENTITY_TYPE (geodata, GEODATA, num_geomesh_faces, BL, num_geomesh_faces);
  if (!dwg_dynapi_entity_value (geodata, "GEODATA", "geomesh_faces", &geomesh_faces, NULL))
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
            //CHK_SUBCLASS_TYPE (geomesh_faces[i], GEODATA_meshface, face4, BL);
          }
    }
}

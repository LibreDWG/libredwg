// unstable
#define DWG_TYPE DWG_TYPE_SECTION_SETTINGS
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BS curr_type;
  BITCODE_BL i, j, num_types, num_geom;
  Dwg_SECTION_typesettings *types;
  Dwg_SECTION_geometrysettings *geom;
  BITCODE_BS type, generation;
  BITCODE_BL num_sources;
  BITCODE_H *sources;
  BITCODE_H destblock;
  BITCODE_T destfile;
  BITCODE_CMC color;
  BITCODE_T layer;
  BITCODE_T ltype;
  BITCODE_BD ltype_scale;
  BITCODE_T plotstyle;
  BITCODE_BLd linewt;
  BITCODE_BS face_transparency;
  BITCODE_BS edge_transparency;
  BITCODE_BS hatch_type;
  BITCODE_T hatch_pattern;
  BITCODE_BD hatch_angle;
  BITCODE_BD hatch_spacing;
  BITCODE_BD hatch_scale;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_obj_section_settings *_obj = dwg_object_to_SECTION_SETTINGS (obj);

  CHK_ENTITY_TYPE (_obj, SECTION_SETTINGS, curr_type, BS);
  CHK_ENTITY_MAX (_obj, SECTION_SETTINGS, curr_type, BS, 8);
  CHK_ENTITY_TYPE (_obj, SECTION_SETTINGS, num_types, BL);
  CHK_ENTITY_MAX (_obj, SECTION_SETTINGS, num_types, BL, 8);
  if (!dwg_dynapi_entity_value (_obj, "SECTION_SETTINGS", "types", &types,
                                NULL))
    fail ("SECTION_SETTINGS.types");
  else
    for (i = 0; i < num_types; i++)
      {
        ok ("SECTION_SETTINGS.types[%d]", i);
        CHK_SUBCLASS_TYPE (types[i], SECTION_typesettings, type, BS);
        CHK_SUBCLASS_MAX (types[i], SECTION_typesettings, type, BS, 8);
        CHK_SUBCLASS_TYPE (types[i], SECTION_typesettings, generation, BS);
        CHK_SUBCLASS_MAX (types[i], SECTION_typesettings, generation, BS, 128);
        CHK_SUBCLASS_TYPE (types[i], SECTION_typesettings, num_sources, BL);
        CHK_SUBCLASS_MAX (types[i], SECTION_typesettings, num_sources, BL,
                          5000);
        CHK_SUBCLASS_HV (types[i], SECTION_typesettings, sources,
                         types[i].num_sources);
        CHK_SUBCLASS_H (types[i], SECTION_typesettings, destblock);
        CHK_SUBCLASS_UTF8TEXT (types[i], SECTION_typesettings, destfile);
        CHK_SUBCLASS_TYPE (types[i], SECTION_typesettings, num_geom, BL);
        if (!dwg_dynapi_subclass_value (&types[i], "SECTION_typesettings",
                                        "geom", &geom, NULL))
          fail ("SECTION_SETTINGS.types[%d].geom", i);
        else
          for (j = 0; j < types[i].num_geom; j++)
            {
              ok ("SECTION_SETTINGS.types[%d].geom[%d]", i, j);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings, num_geoms,
                                 BL);
              CHK_SUBCLASS_MAX (geom[j], SECTION_geometrysettings, num_geoms,
                                BL, 8);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings, hexindex,
                                 BLx);
              CHK_SUBCLASS_MAX (geom[j], SECTION_geometrysettings, hexindex,
                                BL, 32);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings, flags, BL);
              CHK_SUBCLASS_CMC (geom[j], SECTION_geometrysettings, color);
              CHK_SUBCLASS_UTF8TEXT (geom[j], SECTION_geometrysettings, layer);
              CHK_SUBCLASS_UTF8TEXT (geom[j], SECTION_geometrysettings, ltype);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings,
                                 ltype_scale, BD);
              CHK_SUBCLASS_UTF8TEXT (geom[j], SECTION_geometrysettings,
                                     plotstyle);
              SINCE (R_2000)
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings, linewt,
                                 BLd);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings,
                                 face_transparency, BS);
              CHK_SUBCLASS_MAX (geom[j], SECTION_geometrysettings,
                                face_transparency, BS, 100);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings,
                                 edge_transparency, BS);
              CHK_SUBCLASS_MAX (geom[j], SECTION_geometrysettings,
                                edge_transparency, BS, 100);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings, hatch_type,
                                 BS);
              CHK_SUBCLASS_MAX (geom[j], SECTION_geometrysettings, hatch_type,
                                BS, 8);
              CHK_SUBCLASS_UTF8TEXT (geom[j], SECTION_geometrysettings,
                                     hatch_pattern);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings,
                                 hatch_angle, BD);
              CHK_SUBCLASS_MAX (geom[j], SECTION_geometrysettings, hatch_angle,
                                BD, MAX_ANGLE);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings,
                                 hatch_spacing, BD);
              CHK_SUBCLASS_TYPE (geom[j], SECTION_geometrysettings,
                                 hatch_scale, BD);
            }
      }
}

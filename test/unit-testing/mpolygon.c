// debugging
#define DWG_TYPE DWG_TYPE_MPOLYGON
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error, isnew;
  BITCODE_BL i;
  BITCODE_BL is_gradient_fill;
  BITCODE_BL reserved;
  BITCODE_BD gradient_angle;
  BITCODE_BD gradient_shift;
  BITCODE_BL single_color_gradient;
  BITCODE_BD gradient_tint;
  BITCODE_BL num_colors;
  BITCODE_T gradient_name;
  BITCODE_BD elevation;
  BITCODE_BE extrusion;
  BITCODE_TV name;
  BITCODE_B is_solid_fill;
  BITCODE_B is_associative;
  BITCODE_BL num_paths;
  BITCODE_BS style;
  BITCODE_BS pattern_type;
  BITCODE_BD angle;
  BITCODE_BD scale_spacing;
  BITCODE_B double_flag;
  BITCODE_BS num_deflines;
  BITCODE_BL num_boundary_handles;
  BITCODE_H *boundary_handles;
  Dwg_HATCH_Color *colors;
  Dwg_HATCH_Path *paths;
  Dwg_HATCH_DefLine *deflines;

  BITCODE_CMC hatch_color;
  BITCODE_2RD x_dir;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
#ifdef DEBUG_CLASSES
  dwg_ent_mpolygon *_obj = dwg_object_to_MPOLYGON (obj);

  CHK_ENTITY_TYPE (_obj, MPOLYGON, is_gradient_fill, BL);
  if (is_gradient_fill)
    {
      CHK_ENTITY_TYPE (_obj, MPOLYGON, reserved, BL);
      CHK_ENTITY_TYPE (_obj, MPOLYGON, gradient_angle, BD);
      CHK_ENTITY_MAX (_obj, MPOLYGON, gradient_angle, BD, MAX_ANGLE);
      CHK_ENTITY_TYPE (_obj, MPOLYGON, gradient_shift, BD);
      CHK_ENTITY_TYPE (_obj, MPOLYGON, single_color_gradient, BL);
      CHK_ENTITY_TYPE (_obj, MPOLYGON, gradient_tint, BD);
      CHK_ENTITY_UTF8TEXT (_obj, MPOLYGON, gradient_name);
    }
  CHK_ENTITY_TYPE (_obj, MPOLYGON, num_colors, BL);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, elevation, BD);
  CHK_ENTITY_3RD (_obj, MPOLYGON, extrusion);
  CHK_ENTITY_UTF8TEXT (_obj, MPOLYGON, name);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, is_solid_fill, B);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, is_associative, B);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, num_paths, BL);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, style, BS);
  if (_obj->style > 2)
    fail ("Invalid MPOLYGON.style " FORMAT_BS " > 2", _obj->style);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, pattern_type, BS);
  if (_obj->pattern_type > 2)
    fail ("Invalid MPOLYGON.pattern_type " FORMAT_BS " > 2",
          _obj->pattern_type);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, angle, BD);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, scale_spacing, BD);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, double_flag, B);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, num_deflines, BS);

  if (!dwg_dynapi_entity_value (_obj, "MPOLYGON", "colors", &colors, NULL))
    fail ("MPOLYGON.colors");
  if (num_colors)
    {
      if (!is_gradient_fill)
        fail ("MPOLYGON.num_colors without is_gradient_fill");
      if (!colors)
        fail ("MPOLYGON.colors");
      else
        for (i = 0; i < num_colors; i++)
          {
            CHK_SUBCLASS_TYPE (colors[i], HATCH_Color, shift_value, BD);
            CHK_SUBCLASS_CMC (colors[i], HATCH_Color, color);
          }
    }

  if (!dwg_dynapi_entity_value (_obj, "MPOLYGON", "paths", &paths, NULL))
    fail ("MPOLYGON.paths");
  if (num_paths)
    {
      if (!paths)
        fail ("MPOLYGON.paths");
      else
        for (i = 0; i < num_paths; i++)
          {
            CHK_SUBCLASS_TYPE (paths[i], HATCH_Path, flag, BLx);
            CHK_SUBCLASS_MAX (paths[i], HATCH_Path, flag, BL, 0x400);
            CHK_SUBCLASS_TYPE (paths[i], HATCH_Path, num_segs_or_paths, BL);
            if (!(paths[i].flag & 2))
              {
                for (BITCODE_BL j = 0; j < paths[i].num_segs_or_paths; j++)
                  {
                    CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                       curve_type, RC);
                    if (paths[i].segs[j].curve_type > 4)
                      fail ("Invalid MPOLYGON.paths[%d].segs[%d].curve_type "
                            "%d > 4",
                            i, j, paths[i].segs[j].curve_type);
                    if (paths[i].segs[j].curve_type == 1)
                      {
                        CHK_SUBCLASS_2RD (paths[i].segs[j], HATCH_PathSeg,
                                          first_endpoint);
                        CHK_SUBCLASS_2RD (paths[i].segs[j], HATCH_PathSeg,
                                          second_endpoint);
                      }
                    else if (paths[i].segs[j].curve_type == 2)
                      { /* CIRCULAR ARC */
                        CHK_SUBCLASS_2RD (paths[i].segs[j], HATCH_PathSeg,
                                          center);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           radius, BD);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           start_angle, BD);
                        // CHK_ENTITY_MAX (_obj, HATCH_PathSeg, start_angle,
                        // BD, MAX_ANGLE);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           end_angle, BD);
                        // CHK_ENTITY_MAX (_obj, HATCH_PathSeg, end_angle, BD,
                        // MAX_ANGLE);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           is_ccw, B);
                      }
                    else if (paths[i].segs[j].curve_type == 3)
                      { /* ELLIPTICAL ARC */
                        CHK_SUBCLASS_2RD (paths[i].segs[j], HATCH_PathSeg,
                                          center);
                        CHK_SUBCLASS_2RD (paths[i].segs[j], HATCH_PathSeg,
                                          endpoint);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           minor_major_ratio, BD);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           start_angle, BD);
                        // CHK_ENTITY_MAX (_obj, HATCH_PathSeg, start_angle,
                        // BD, MAX_ANGLE);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           end_angle, BD);
                        // CHK_ENTITY_MAX (_obj, HATCH_PathSeg, end_angle, BD,
                        // MAX_ANGLE);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           is_ccw, B);
                      }
                    else if (paths[i].segs[j].curve_type == 4)
                      { /* SPLINE */
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           degree, BL);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           is_rational, B);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           is_periodic, B);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           num_knots, BL);
                        CHK_SUBCLASS_TYPE (paths[i].segs[j], HATCH_PathSeg,
                                           num_control_points, BL);
                        // knots: BD vector
                        // control_points: HATCH_ControlPoint
                      }
                  }
              }
            else
              {
                CHK_SUBCLASS_TYPE (paths[i], HATCH_Path, bulges_present, B);
                CHK_SUBCLASS_TYPE (paths[i], HATCH_Path, closed, B);
                CHK_SUBCLASS_TYPE (paths[i], HATCH_Path, num_segs_or_paths,
                                   BL);
                for (BITCODE_BL j = 0; j < paths[i].num_segs_or_paths; j++)
                  {
                    CHK_SUBCLASS_2RD (paths[i].polyline_paths[j],
                                      HATCH_PolylinePath, point);
                    CHK_SUBCLASS_TYPE (paths[i].polyline_paths[j],
                                       HATCH_PolylinePath, bulge, BD);
                    if (paths[i].polyline_paths[j].bulge != 0.0
                        && paths[i].bulges_present)
                      fail ("Illegal HATCH_PolylinePath.bulge %f without "
                            "paths[i].bulges_present",
                            paths[i].polyline_paths[j].bulge);
                  }
              }
            CHK_SUBCLASS_TYPE (paths[i], HATCH_Path, num_boundary_handles, BL);
          }
    }

  if (!dwg_dynapi_entity_value (_obj, "MPOLYGON", "deflines", &deflines, NULL))
    fail ("MPOLYGON.deflines");
  // only with !solid_fill
  if (num_deflines)
    {
      if (is_solid_fill)
        fail ("MPOLYGON.num_deflines with solid_fill");
      if (!deflines)
        fail ("MPOLYGON.deflines");
      else
        {
          for (i = 0; i < num_deflines; i++)
            {
              CHK_SUBCLASS_TYPE (deflines[i], HATCH_DefLine, angle, BD);
              CHK_SUBCLASS_2RD (deflines[i], HATCH_DefLine, pt0);
              CHK_SUBCLASS_2RD (deflines[i], HATCH_DefLine, offset);
              CHK_SUBCLASS_TYPE (deflines[i], HATCH_DefLine, num_dashes, BS);
              for (BITCODE_BS j = 0; j < deflines[i].num_dashes; j++)
                {
                  ok ("MPOLYGON.deflines[%d].dashes[%d]: %f", i, j,
                      deflines[i].dashes[j]);
                }
            }
        }
    }

  CHK_ENTITY_TYPE (_obj, MPOLYGON, num_boundary_handles, BL);
  if (!dwg_dynapi_entity_value (_obj, "MPOLYGON", "boundary_handles",
                                &boundary_handles, NULL))
    fail ("MPOLYGON.boundary_handles");
  if (num_boundary_handles)
    {
      if (!boundary_handles)
        fail ("MPOLYGON.boundary_handles");
      else
        for (i = 0; i < num_boundary_handles; i++)
          {
            ok ("MPOLYGON.boundary_handles[%d]: " FORMAT_REF, i,
                ARGS_REF (boundary_handles[i]));
          }
    }
  else if (boundary_handles)
    fail ("MPOLYGON.boundary_handles with !num_boundary_handles");

  CHK_ENTITY_CMC (_obj, MPOLYGON, hatch_color);
  CHK_ENTITY_2RD (_obj, MPOLYGON, x_dir);
  CHK_ENTITY_TYPE (_obj, MPOLYGON, num_boundary_handles, BL);
#endif
}

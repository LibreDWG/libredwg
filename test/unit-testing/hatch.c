#define DWG_TYPE DWG_TYPE_HATCH
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  BITCODE_BL is_gradient_fill;
  BITCODE_BL reserved;
  BITCODE_BD gradient_angle;
  BITCODE_BD gradient_shift;
  BITCODE_BL single_color_gradient;
  BITCODE_BD gradient_tint;
  BITCODE_BL num_colors;
  BITCODE_TV gradient_name;
  BITCODE_BD elevation;
  BITCODE_BE extrusion;
  BITCODE_TV name;
  BITCODE_B solid_fill;
  BITCODE_B associative;
  BITCODE_BL num_paths;
  BITCODE_BS style;
  BITCODE_BS pattern_type;
  BITCODE_BD angle;
  BITCODE_BD scale_spacing;
  BITCODE_B double_flag;
  BITCODE_BS num_deflines;
  BITCODE_B has_derived;
  BITCODE_BD pixel_size;
  BITCODE_BL num_seeds;
  BITCODE_2RD *seeds;
  BITCODE_BL num_boundary_handles;
  BITCODE_H* boundary_handles;
  Dwg_HATCH_Color* colors;
  Dwg_HATCH_Path* paths;
  Dwg_HATCH_DefLine * deflines;

  dwg_ent_hatch *hatch = dwg_object_to_HATCH (obj);

  CHK_ENTITY_TYPE (hatch, HATCH, is_gradient_fill, BL, is_gradient_fill);
  // if is_gradient_fill
  CHK_ENTITY_TYPE (hatch, HATCH, reserved, BL, reserved);
  CHK_ENTITY_TYPE (hatch, HATCH, gradient_angle, BD, gradient_angle);
  CHK_ENTITY_TYPE (hatch, HATCH, gradient_shift, BD, gradient_shift);
  CHK_ENTITY_TYPE (hatch, HATCH, single_color_gradient, BL, single_color_gradient);
  CHK_ENTITY_TYPE (hatch, HATCH, gradient_tint, BD, gradient_tint);
  CHK_ENTITY_TYPE (hatch, HATCH, num_colors, BL, num_colors);
  CHK_ENTITY_TYPE (hatch, HATCH, gradient_name, TV, gradient_name);

  CHK_ENTITY_TYPE (hatch, HATCH, elevation, BD, elevation);
  CHK_ENTITY_3RD (hatch, HATCH, extrusion, extrusion);
  CHK_ENTITY_TYPE (hatch, HATCH, name, TV, name);
  CHK_ENTITY_TYPE (hatch, HATCH, solid_fill, B, solid_fill);
  CHK_ENTITY_TYPE (hatch, HATCH, associative, B, associative);
  CHK_ENTITY_TYPE (hatch, HATCH, num_paths, BL, num_paths);
  CHK_ENTITY_TYPE (hatch, HATCH, style, BS, style);
  CHK_ENTITY_TYPE (hatch, HATCH, pattern_type, BS, pattern_type);
  CHK_ENTITY_TYPE (hatch, HATCH, angle, BD, angle);
  CHK_ENTITY_TYPE (hatch, HATCH, scale_spacing, BD, scale_spacing);
  CHK_ENTITY_TYPE (hatch, HATCH, double_flag, B, double_flag);
  CHK_ENTITY_TYPE (hatch, HATCH, num_deflines, BS, num_deflines);
  CHK_ENTITY_TYPE (hatch, HATCH, has_derived, B, has_derived);
  CHK_ENTITY_TYPE (hatch, HATCH, pixel_size, BD, pixel_size);
  CHK_ENTITY_TYPE (hatch, HATCH, num_seeds, BL, num_seeds);
  CHK_ENTITY_TYPE (hatch, HATCH, num_boundary_handles, BL, num_boundary_handles);

  if (!dwg_dynapi_entity_value (hatch, "HATCH", "colors", &colors, NULL))
    fail ("HATCH.colors");
  if (num_colors)
    {
      if (!is_gradient_fill)
        fail ("HATCH.num_colors without is_gradient_fill");
      if (!colors)
        fail ("HATCH.colors");
      else
        for (i = 0; i < num_colors; i++)
          {
            ok ("HATCH.colors[%d]: %f, %d", i, colors[i].shift_value,
                colors[i].color.index);
          }
    }

  if (!dwg_dynapi_entity_value (hatch, "HATCH", "paths", &paths, NULL))
    fail ("HATCH.paths");
  if (num_paths)
    {
      if (!paths)
        fail ("HATCH.paths");
      else
        for (i = 0; i < num_paths; i++)
          {
            ok ("HATCH.paths[%d].flag: " FORMAT_BL, i, paths[i].flag);
            ok ("HATCH.paths[%d].num_segs_or_paths: " FORMAT_BL, i, paths[i].num_segs_or_paths);
            if (!(paths[i].flag & 2))
              {
                for (BITCODE_BL j = 0; j < paths[i].num_segs_or_paths; j++)
                  {
                    ok ("HATCH.paths[%d].segs[%d].type_status: %d", i, j,
                        paths[i].segs[j].type_status);
                    if (paths[i].segs[j].type_status == 1)
                      {
                        ok ("HATCH.paths[%d].segs[%d].first_endpoint: (%f ,%f)",
                            i, j, paths[i].segs[j].first_endpoint.x,
                            paths[i].segs[j].first_endpoint.y);
                        ok ("HATCH.paths[%d].segs[%d].second_endpoint: (%f ,%f)",
                            i, j, paths[i].segs[j].second_endpoint.x,
                            paths[i].segs[j].second_endpoint.y);
                      }
                    else if (paths[i].segs[j].type_status == 2)
                      { /* CIRCULAR ARC */
                        ok ("HATCH.paths[%d].segs[%d].center: (%f ,%f)",
                            i, j, paths[i].segs[j].center.x,
                            paths[i].segs[j].center.y);
                        ok ("HATCH.paths[%d].segs[%d].radius: %f",
                            i, j, paths[i].segs[j].radius);
                        ok ("HATCH.paths[%d].segs[%d].start_angle: %f",
                            i, j, paths[i].segs[j].start_angle);
                        ok ("HATCH.paths[%d].segs[%d].end_angle: %f",
                            i, j, paths[i].segs[j].end_angle);
                        ok ("HATCH.paths[%d].segs[%d].is_ccw: %d",
                            i, j, paths[i].segs[j].is_ccw);
                      }
                    else if (paths[i].segs[j].type_status == 3)
                      { /* ELLIPTICAL ARC */
                        ok ("HATCH.paths[%d].segs[%d].center: (%f ,%f)",
                            i, j, paths[i].segs[j].center.x,
                            paths[i].segs[j].center.y);
                        ok ("HATCH.paths[%d].segs[%d].endpoint: (%f ,%f)",
                            i, j, paths[i].segs[j].endpoint.x,
                            paths[i].segs[j].endpoint.y);
                        ok ("HATCH.paths[%d].segs[%d].minor_major_ratio: %f",
                            i, j, paths[i].segs[j].minor_major_ratio);
                        ok ("HATCH.paths[%d].segs[%d].start_angle: %f",
                            i, j, paths[i].segs[j].start_angle);
                        ok ("HATCH.paths[%d].segs[%d].end_angle: %f",
                            i, j, paths[i].segs[j].end_angle);
                        ok ("HATCH.paths[%d].segs[%d].is_ccw: %d",
                            i, j, paths[i].segs[j].is_ccw);
                      }
                    // TODO 4 SPLINE
                  }
              }
            else
              {
                ok ("HATCH.paths[%d].bulges_present: %d", i, paths[i].bulges_present);
                ok ("HATCH.paths[%d].closed: %d", i, paths[i].closed);
                for (BITCODE_BL j = 0; j < paths[i].num_segs_or_paths; j++)
                  {
                    ok ("HATCH.paths[%d].polyline_paths[%d].point: (%f ,%f)",
                        i, j, paths[i].polyline_paths[j].point.x,
                        paths[i].polyline_paths[j].point.y);
                    ok ("HATCH.paths[%d].polyline_paths[%d].bulge: %f", i, j,
                        paths[i].polyline_paths[j].bulge);
                  }
              }
            ok ("HATCH.paths[%d].num_boundary_handles: " FORMAT_BL, i,
                paths[i].num_boundary_handles);
          }
    }

  if (!dwg_dynapi_entity_value (hatch, "HATCH", "deflines", &deflines, NULL))
    fail ("HATCH.deflines");
  // only with !solid_fill
  if (num_deflines)
    {
      if (solid_fill)
        fail ("HATCH.num_deflines with solid_fill");
      if (!deflines)
        fail ("HATCH.deflines");
      else
        {
          for (i = 0; i < num_deflines; i++)
            {
              ok ("HATCH.deflines[%d].angle: %f", i, deflines[i].angle);
              ok ("HATCH.deflines[%d].pt0: (%f, %f)", i, deflines[i].pt0.x, deflines[i].pt0.y);
              ok ("HATCH.deflines[%d].offset: (%f, %f)", i, deflines[i].offset.x, deflines[i].offset.y);
              ok ("HATCH.deflines[%d].num_dashes: " FORMAT_BS, i, deflines[i].num_dashes);
              for (BITCODE_BS j = 0; j < deflines[i].num_dashes; j++)
                {
                  ok ("HATCH.deflines[%d].dashes[%d]: %f", i, j, deflines[i].dashes[j]);
                }
            }
        }
    }

  if (!dwg_dynapi_entity_value (hatch, "HATCH", "seeds", &seeds, NULL))
    fail ("HATCH.seeds");
  if (num_seeds)
    {
      if (!seeds)
        fail ("HATCH.seeds");
      else
        for (i = 0; i < num_seeds; i++)
          {
            ok ("HATCH.seeds[%d]: (%f, %f)", i, seeds[i].x, seeds[i].y);
          }
    }
  else if (seeds)
    fail ("HATCH.seeds with !num_seeds");

  if (!dwg_dynapi_entity_value (hatch, "HATCH", "boundary_handles", &boundary_handles, NULL))
    fail ("HATCH.boundary_handles");
  if (num_boundary_handles)
    {
      if (!boundary_handles)
        fail ("HATCH.boundary_handles");
      else
        for (i = 0; i < num_boundary_handles; i++)
          {
            ok ("HATCH.boundary_handles[%d]: " FORMAT_REF, i, ARGS_REF (boundary_handles[i]));
          }
    }
  else if (boundary_handles)
    fail ("HATCH.boundary_handles with !num_boundary_handles");
}

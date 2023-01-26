// TODO unstable
#define DWG_TYPE DWG_TYPE_HELIX
#include "common.c"

void
api_process (dwg_object *obj)
{
  int error;
  BITCODE_BL i;
  // AcDbSpline
  BITCODE_BS flag;
  BITCODE_BS scenario; /* 1 spline, 2 bezier */
  BITCODE_BS degree;
  BITCODE_BL splineflags; /* 2013+: method fit points = 1, CV frame show = 2,
                             closed = 4 */
  BITCODE_BL knotparam;   /* 2013+: Chord = 0, Square root = 1, Uniform = 2,
                             Custom = 15 */
  BITCODE_BD fit_tol;
  BITCODE_3BD beg_tan_vec;
  BITCODE_3BD end_tan_vec;
  BITCODE_B closed_b; /* bit 1 of 70 */
  BITCODE_B periodic; /* bit 2 of 70 */
  BITCODE_B rational; /* bit 3 of 70 */
  BITCODE_B weighted; /* bit 4 of 70 */
  BITCODE_BD knot_tol;
  BITCODE_BD ctrl_tol;
  BITCODE_BS num_fit_pts;
  BITCODE_3DPOINT *fit_pts;
  BITCODE_BL num_knots;
  BITCODE_BD *knots;
  BITCODE_BL num_ctrl_pts;
  Dwg_SPLINE_control_point *ctrl_pts;
  // AcDbHelix
  BITCODE_BL major_version;
  BITCODE_BL maint_version;
  BITCODE_3BD axis_base_pt;
  BITCODE_3BD start_pt;
  BITCODE_3BD axis_vector;
  BITCODE_BD radius;
  BITCODE_BD turns;
  BITCODE_BD turn_height;
  BITCODE_B handedness;
  BITCODE_RC constraint_type;

  Dwg_Version_Type dwg_version = obj->parent->header.version;
  dwg_ent_helix *helix = dwg_object_to_HELIX (obj);

  CHK_ENTITY_TYPE (helix, HELIX, flag, BS);
  CHK_ENTITY_TYPE (helix, HELIX, scenario, BS);
  if (scenario == 0 || scenario > 2)
    fail ("Illegal SPLINE.scenario %d", (int)scenario);
  CHK_ENTITY_TYPE (helix, HELIX, degree, BS);
  CHK_ENTITY_TYPE (helix, HELIX, splineflags, BL);
  CHK_ENTITY_TYPE (helix, HELIX, knotparam, BL);
  CHK_ENTITY_TYPE (helix, HELIX, fit_tol, BD);
  CHK_ENTITY_3RD (helix, HELIX, beg_tan_vec);
  CHK_ENTITY_3RD (helix, HELIX, end_tan_vec);
  CHK_ENTITY_TYPE (helix, HELIX, rational, B);
  CHK_ENTITY_TYPE (helix, HELIX, closed_b, B);
  CHK_ENTITY_TYPE (helix, HELIX, periodic, B);
  CHK_ENTITY_TYPE (helix, HELIX, weighted, B);
  CHK_ENTITY_TYPE (helix, HELIX, knot_tol, BD);
  CHK_ENTITY_TYPE (helix, HELIX, ctrl_tol, BD);
  CHK_ENTITY_TYPE (helix, HELIX, num_fit_pts, BS);
  CHK_ENTITY_TYPE (helix, HELIX, num_knots, BL);
  CHK_ENTITY_TYPE (helix, HELIX, num_ctrl_pts, BL);

  if (!dwg_dynapi_entity_value (helix, "HELIX", "fit_pts", &fit_pts, NULL))
    fail ("HELIX.fit_pts");
  if (!dwg_dynapi_entity_value (helix, "HELIX", "knots", &knots, NULL))
    fail ("HELIX.knots");
  if (!dwg_dynapi_entity_value (helix, "HELIX", "ctrl_pts", &ctrl_pts, NULL))
    fail ("HELIX.ctrl_pts");

  if (scenario == 1)
    {
      if (num_fit_pts)
        fail ("HELIX.num_fit_pts with scenario 1");
      for (i = 0; i < num_ctrl_pts; i++)
        {
          if (i < 10 || i == num_ctrl_pts - 1)
            {
              if (i == num_ctrl_pts - 1)
                printf ("\n");
              CHK_SUBCLASS_TYPE (ctrl_pts[i], SPLINE_control_point, x, BD);
              CHK_SUBCLASS_TYPE (ctrl_pts[i], SPLINE_control_point, y, BD);
              CHK_SUBCLASS_TYPE (ctrl_pts[i], SPLINE_control_point, z, BD);
              CHK_SUBCLASS_TYPE (ctrl_pts[i], SPLINE_control_point, w, BD);
            }
          else
            printf (".");
        }
      for (i = 0; i < num_knots; i++)
        {
          double d;
          if (i < 10)
            ok ("HELIX.knots[%d]: %f", i, knots[i]);
          else
            {
              d = knots[i];
              printf (".");
            }
        }
      if (num_knots >= 10)
        printf ("\n");
    }
  else
    {
      if (num_knots)
        fail ("HELIX.num_knots with scenario 2");
      if (num_ctrl_pts)
        fail ("HELIX.num_ctrl_pts with scenario 2");
      for (i = 0; i < num_fit_pts; i++)
        {
          ok ("HELIX.fit_pts[%d]: (%f, %f, %f)", i, fit_pts[i].x, fit_pts[i].y,
              fit_pts[i].z);
        }
    }

  // AcDbHelix
  CHK_ENTITY_TYPE (helix, HELIX, major_version, BL);
  CHK_ENTITY_TYPE (helix, HELIX, maint_version, BL);
  CHK_ENTITY_3RD (helix, HELIX, axis_base_pt);
  CHK_ENTITY_3RD (helix, HELIX, start_pt);
  CHK_ENTITY_3RD (helix, HELIX, axis_vector);
  CHK_ENTITY_TYPE (helix, HELIX, radius, BD);
  CHK_ENTITY_TYPE (helix, HELIX, turns, BD);
  CHK_ENTITY_TYPE (helix, HELIX, turn_height, BD);
  CHK_ENTITY_TYPE (helix, HELIX, handedness, B);
  CHK_ENTITY_TYPE (helix, HELIX, constraint_type, RC);
  CHK_ENTITY_MAX (helix, HELIX, constraint_type, RC, 2);
}

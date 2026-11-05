#define GEOM_TEST_C
#include <stdlib.h>
#include <math.h>
#include "../../src/geom.c"

#include "tests_common.h"

#define NEAR(a, b) (fabs ((a) - (b)) < 1e-9)
// MSVC math.h defines the M_ constants only with _USE_MATH_DEFINES
#ifndef M_SQRT1_2
#  define M_SQRT1_2 0.70710678118654752440
#endif

// bulge_to_arc must give an arc with this center, radius, start and sweep
static void
check_arc (const char *name, const BITCODE_2BD p1, const BITCODE_2BD p2,
           const double bulge, const double cx, const double cy,
           const double radius, const double start, const double sweep)
{
  BITCODE_2BD ctr = { 0.0, 0.0 };
  double r = 0.0, a = 0.0, s = 0.0;
  const int is_arc = bulge_to_arc (p1, p2, bulge, &ctr, &r, &a, &s);
  if (is_arc && NEAR (ctr.x, cx) && NEAR (ctr.y, cy) && NEAR (r, radius)
      && NEAR (a, start) && NEAR (s, sweep))
    ok ("bulge_to_arc %s", name);
  else
    fail ("bulge_to_arc %s: ctr (%g,%g) r %g start %g sweep %g", name, ctr.x,
          ctr.y, r, a, s);
}

// bulge_to_arc must take the segment as straight
static void
check_straight (const char *name, const BITCODE_2BD p1, const BITCODE_2BD p2,
                const double bulge)
{
  BITCODE_2BD ctr;
  double r, a, s;
  if (!bulge_to_arc (p1, p2, bulge, &ctr, &r, &a, &s))
    ok ("bulge_to_arc straight %s", name);
  else
    fail ("bulge_to_arc straight %s taken as an arc", name);
}

static void
bulge_to_arc_tests (void)
{
  const BITCODE_2BD p1 = { 0.0, 0.0 }, p2 = { 2.0, 0.0 };
  const BITCODE_2BD q1 = { 1.0, 0.0 }, q2 = { 0.0, 1.0 };

  // bulge 1: counter-clockwise semicircle below the chord
  check_arc ("1 semicircle", p1, p2, 1.0, 1.0, 0.0, 1.0, M_PI, M_PI);
  // bulge -1: the same circle, clockwise
  check_arc ("-1 clockwise", p1, p2, -1.0, 1.0, 0.0, 1.0, M_PI, -M_PI);
  // tan(PI/8): a counter-clockwise quarter of the unit circle
  check_arc ("quarter circle", q1, q2, tan (M_PI / 8.0), 0.0, 0.0, 1.0, 0.0,
             M_PI / 2.0);
  check_straight ("bulge 0", p1, p2, 0.0);
  check_straight ("coincident points", p1, p1, 1.0);
  check_straight ("NAN", p1, p2, (double)NAN);
  check_straight ("INFINITY", p1, p2, (double)INFINITY);
}

// bulge_split of (0,0) -> (2,0) into 4 points must give n points, starting
// at (0,0), on the unit circle around (1,0), with pts[2] at (x2,y2)
static void
check_split (const char *name, const double bulge, const int expect_n,
             const double x2, const double y2)
{
  const BITCODE_2BD p1 = { 0.0, 0.0 }, p2 = { 2.0, 0.0 };
  BITCODE_2BD pts[4] = { { 0.0, 0.0 } };
  const int n = bulge_split (pts, 4, p1, p2, bulge);
  int good = n == expect_n && pts[0].x == 0.0 && pts[0].y == 0.0;
  int i;
  for (i = 0; good && n > 1 && i < n; i++)
    good = NEAR (hypot (pts[i].x - 1.0, pts[i].y), 1.0);
  if (good && n > 2)
    good = NEAR (pts[2].x, x2) && NEAR (pts[2].y, y2);
  if (good)
    ok ("bulge_split %s", name);
  else
    fail ("bulge_split %s: n %d pts[2] (%g,%g)", name, n, pts[2].x, pts[2].y);
}

static void
bulge_split_tests (void)
{
  const BITCODE_2BD p1 = { 0.0, 0.0 }, p2 = { 2.0, 0.0 };
  BITCODE_2BD pts[4];

  // p2 excluded: angles PI, 5PI/4, 3PI/2, 7PI/4, below the chord
  check_split ("semicircle", 1.0, 4, 1.0, -1.0);
  // clockwise passes above the chord
  check_split ("clockwise", -1.0, 4, 1.0, 1.0);
  // a straight segment gives just its start point
  check_split ("straight", 0.0, 1, 0.0, 0.0);
  if (bulge_split (pts, 0, p1, p2, 1.0) == 0)
    ok ("bulge_split 0 points");
  else
    fail ("bulge_split with 0 points");
}

static void
arc_split_tests (void)
{
  const BITCODE_2BD ctr = { 0.0, 0.0 };
  BITCODE_2BD pts[4];

  // a full circle, as out_geojson does for CIRCLE; 2*PI must not
  // normalize to a zero span
  arc_split (pts, 4, ctr, 0.0, 2.0 * M_PI, 1.0);
  if (NEAR (pts[0].x, 1.0) && NEAR (pts[1].y, 1.0) && NEAR (pts[2].x, -1.0)
      && NEAR (pts[3].y, -1.0))
    ok ("arc_split full circle");
  else
    fail ("arc_split full circle: pts[1] (%g,%g) pts[2] (%g,%g)", pts[1].x,
          pts[1].y, pts[2].x, pts[2].y);

  // an arc across angle 0: from 3PI/2 to PI/2, counter-clockwise
  arc_split (pts, 2, ctr, 1.5 * M_PI, 0.5 * M_PI, 1.0);
  if (NEAR (pts[0].y, -1.0) && NEAR (pts[1].x, 1.0) && NEAR (pts[1].y, 0.0))
    ok ("arc_split across 0");
  else
    fail ("arc_split across 0: pts[1] (%g,%g)", pts[1].x, pts[1].y);
}

static void
ocs_mirrored_tests (void)
{
  const BITCODE_BE up = { 0.0, 0.0, 1.0 }, down = { 0.0, 0.0, -1.0 };
  const BITCODE_BE pre_r13 = { 0.0, 0.0, 0.0 };
  if (!transform_OCS_2d_is_mirrored (up) && transform_OCS_2d_is_mirrored (down)
      && !transform_OCS_2d_is_mirrored (pre_r13))
    ok ("transform_OCS_2d_is_mirrored");
  else
    fail ("transform_OCS_2d_is_mirrored");
}

// spline_eval of spline at t must give (x,y)
static void
check_spline_point (const char *name, const Dwg_Entity_SPLINE *spline,
                    const double t, const double x, const double y)
{
  BITCODE_3DPOINT pt = { 0.0, 0.0, 0.0 };
  if (spline_eval (&pt, spline, t) && NEAR (pt.x, x) && NEAR (pt.y, y)
      && NEAR (pt.z, 0.0))
    ok ("spline_eval %s", name);
  else
    fail ("spline_eval %s: (%g,%g,%g), expected (%g,%g)", name, pt.x, pt.y,
          pt.z, x, y);
}

static void
spline_eval_tests (void)
{
  // the DWG stores w = 0 for a non-rational spline
  Dwg_SPLINE_control_point bez[4] = { { NULL, 0.0, 0.0, 0.0, 0.0 },
                                      { NULL, 0.0, 4.0, 0.0, 0.0 },
                                      { NULL, 4.0, 4.0, 0.0, 0.0 },
                                      { NULL, 4.0, 0.0, 0.0, 0.0 } };
  double bez_knots[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };
  Dwg_SPLINE_control_point five[5] = { { NULL, 0.0, 0.0, 0.0, 0.0 },
                                       { NULL, 0.0, 4.0, 0.0, 0.0 },
                                       { NULL, 4.0, 4.0, 0.0, 0.0 },
                                       { NULL, 8.0, 4.0, 0.0, 0.0 },
                                       { NULL, 8.0, 0.0, 0.0, 0.0 } };
  double five_knots[9] = { 0, 0, 0, 0, 1, 2, 2, 2, 2 };
  // a quadratic NURBS quarter of the unit circle
  Dwg_SPLINE_control_point arc[3] = { { NULL, 1.0, 0.0, 0.0, 1.0 },
                                      { NULL, 1.0, 1.0, 0.0, M_SQRT1_2 },
                                      { NULL, 0.0, 1.0, 0.0, 1.0 } };
  double arc_knots[6] = { 0, 0, 0, 1, 1, 1 };
  double bad_knots[9] = { 0, 0, 0, 0, 2, 1, 2, 2, 2 };
  Dwg_Entity_SPLINE spline;
  BITCODE_3DPOINT pt;

  memset (&spline, 0, sizeof (spline));
  spline.degree = 3;
  spline.num_ctrl_pts = 4;
  spline.ctrl_pts = bez;
  spline.num_knots = 8;
  spline.knots = bez_knots;
  // a clamped cubic with 4 control points is one Bezier: de Casteljau
  check_spline_point ("bezier t=1/2", &spline, 0.5, 2.0, 3.0);
  check_spline_point ("bezier start", &spline, 0.0, 0.0, 0.0);
  check_spline_point ("bezier end", &spline, 1.0, 4.0, 0.0);
  check_spline_point ("bezier t clamped", &spline, 7.0, 4.0, 0.0);

  spline.num_ctrl_pts = 5;
  spline.ctrl_pts = five;
  spline.num_knots = 9;
  spline.knots = five_knots;
  check_spline_point ("5 points t=1/2", &spline, 0.5, 1.25, 3.5);
  check_spline_point ("5 points end", &spline, 2.0, 8.0, 0.0);

  spline.degree = 2;
  spline.rational = 1;
  spline.num_ctrl_pts = 3;
  spline.ctrl_pts = arc;
  spline.num_knots = 6;
  spline.knots = arc_knots;
  check_spline_point ("rational quarter circle", &spline, 0.5, M_SQRT1_2,
                      M_SQRT1_2);

  spline.degree = 3;
  spline.rational = 0;
  spline.num_ctrl_pts = 5;
  spline.ctrl_pts = five;
  spline.num_knots = 8; // must be 5 + 3 + 1
  spline.knots = five_knots;
  if (!spline_eval (&pt, &spline, 0.5))
    ok ("spline_eval rejects a wrong num_knots");
  else
    fail ("spline_eval accepted a wrong num_knots");
  spline.num_knots = 9;
  spline.knots = bad_knots;
  if (!spline_eval (&pt, &spline, 0.5))
    ok ("spline_eval rejects decreasing knots");
  else
    fail ("spline_eval accepted decreasing knots");
}

// spline_to_bezier of spline must give expect_k curves with these points
static void
check_bezier (const char *name, const Dwg_Entity_SPLINE *spline,
              const int expect_k, const double *xy)
{
  BITCODE_3DPOINT *pts = NULL;
  const int k = spline_to_bezier (spline, &pts);
  int i, good = k == expect_k;
  for (i = 0; good && i < k * spline->degree + 1; i++)
    good = NEAR (pts[i].x, xy[2 * i]) && NEAR (pts[i].y, xy[2 * i + 1]);
  if (good)
    ok ("spline_to_bezier %s", name);
  else
    fail ("spline_to_bezier %s: %d curves, expected %d", name, k, expect_k);
  free (pts);
}

static void
spline_to_bezier_tests (void)
{
  Dwg_SPLINE_control_point five[5] = { { NULL, 0.0, 0.0, 0.0, 0.0 },
                                       { NULL, 0.0, 4.0, 0.0, 0.0 },
                                       { NULL, 4.0, 4.0, 0.0, 0.0 },
                                       { NULL, 8.0, 4.0, 0.0, 0.0 },
                                       { NULL, 8.0, 0.0, 0.0, 0.0 } };
  double five_knots[9] = { 0, 0, 0, 0, 1, 2, 2, 2, 2 };
  double bez_knots[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };
  // the knot 1 inserted twice: two Bezier curves sharing (4,4)
  const double two[14] = { 0, 0, 0, 4, 2, 4, 4, 4, 6, 4, 8, 4, 8, 0 };
  const double one[8] = { 0, 0, 0, 4, 4, 4, 8, 4 };
  Dwg_Entity_SPLINE spline;
  BITCODE_3DPOINT *pts = NULL;

  memset (&spline, 0, sizeof (spline));
  spline.degree = 3;
  spline.num_ctrl_pts = 5;
  spline.ctrl_pts = five;
  spline.num_knots = 9;
  spline.knots = five_knots;
  check_bezier ("5 points", &spline, 2, two);
  // a single Bezier needs no insertion
  spline.num_ctrl_pts = 4;
  spline.num_knots = 8;
  spline.knots = bez_knots;
  check_bezier ("single bezier", &spline, 1, one);
  // a rational spline has no polynomial Bezier curves
  spline.rational = 1;
  if (!spline_to_bezier (&spline, &pts) && !pts)
    ok ("spline_to_bezier rejects a rational spline");
  else
    fail ("spline_to_bezier accepted a rational spline");
  free (pts);
}

// spline_from_fit_points must give the control points AutoCAD wrote to DXF
static void
check_fit (const char *name, Dwg_Entity_SPLINE *spline, const double *ctrl,
           const BITCODE_BL num_ctrl)
{
  Dwg_Entity_SPLINE out;
  BITCODE_BL i;
  int good;
  memset (&out, 0, sizeof (out));
  good = spline_from_fit_points (spline, &out) && out.num_ctrl_pts == num_ctrl
         && spline_is_valid (&out);
  for (i = 0; good && i < num_ctrl; i++)
    good = fabs (out.ctrl_pts[i].x - ctrl[2 * i]) < 1e-9
           && fabs (out.ctrl_pts[i].y - ctrl[2 * i + 1]) < 1e-9;
  if (good)
    ok ("spline_from_fit_points %s", name);
  else
    fail ("spline_from_fit_points %s", name);
  free (out.ctrl_pts);
  free (out.knots);
}

static void
spline_from_fit_points_tests (void)
{
  // example_2000.dxf SPLINE 16E, with both end tangents
  BITCODE_3DPOINT fit16e[6]
      = { { -1160.70828949499, 1466.29959801825, 0.0 },
          { -803.2584206390579, 1216.773557430618, 0.0 },
          { -1263.536333621469, 1021.066859620477, 0.0 },
          { -1297.812348330295, 1896.854333640411, 0.0 },
          { -1493.675290112675, 1867.498328089189, 0.0 },
          { -1449.606128933269, 1436.943592467027, 0.0 } };
  const double ctrl16e[16]
      = { -1160.70828949499,  1466.29959801825,   -1278.733898858219,
          1381.536363400083,  -393.9544459252916, 1231.131526494542,
          -1793.156271927829, 719.5692228288901,  -936.5505143705686,
          1967.430418368798,  -1656.363244849998, 1879.620298503901,
          -1444.406509911975, 1581.117926364117,  -1449.606128933269,
          1436.943592467027 };
  // 2018/Dynblocks.dxf SPLINE BD31, no tangents: the natural end condition
  BITCODE_3DPOINT fitbd31[3]
      = { { -0.8132274188101291, 0.2112191587584675, 0.0 },
          { -0.9233206883072853, 0.3861872703200788, 0.0 },
          { -0.9747296262066811, 0.6974653476281674, 0.0 } };
  const double ctrlbd31[10]
      = { -0.8132274188101291, 0.2112191587584675,  -0.8549662927869439,
          0.2676290920387539,  -0.9604058639024939, 0.4101302810989598,
          -0.9693350879702009, 0.5892507619518695,  -0.9747296262066811,
          0.6974653476281674 };
  // 2 points without tangents: a straight line, control points at thirds
  BITCODE_3DPOINT fit2[2] = { { 0.0, 0.0, 0.0 }, { 3.0, 6.0, 0.0 } };
  const double ctrl2[8] = { 0, 0, 1, 2, 2, 4, 3, 6 };
  BITCODE_3DPOINT fitrep[3]
      = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 0.0 } };
  Dwg_Entity_SPLINE spline, out;

  memset (&spline, 0, sizeof (spline));
  spline.degree = 3;
  spline.num_fit_pts = 6;
  spline.fit_pts = fit16e;
  spline.beg_tan_vec.x = -0.8122360233169471;
  spline.beg_tan_vec.y = -0.5833289315868635;
  spline.end_tan_vec.x = -0.0360413721468526;
  spline.end_tan_vec.y = -0.9993502986909906;
  check_fit ("16E with tangents", &spline, ctrl16e, 8);

  memset (&spline, 0, sizeof (spline));
  spline.degree = 3;
  spline.num_fit_pts = 3;
  spline.fit_pts = fitbd31;
  check_fit ("BD31 natural", &spline, ctrlbd31, 5);

  spline.num_fit_pts = 2;
  spline.fit_pts = fit2;
  check_fit ("2 points", &spline, ctrl2, 4);

  spline.num_fit_pts = 3;
  spline.fit_pts = fitrep;
  if (!spline_from_fit_points (&spline, &out))
    ok ("spline_from_fit_points rejects a repeated point");
  else
    {
      fail ("spline_from_fit_points accepted a repeated point");
      free (out.ctrl_pts);
      free (out.knots);
    }
}

// spline_flatten of spline with tol must give min_pts to max_pts points,
// ending at (x,y).  Returns the number of points.
static int
check_flatten (const char *name, const Dwg_Entity_SPLINE *spline,
               const double tol, const int min_pts, const int max_pts,
               const double x, const double y)
{
  BITCODE_3DPOINT *pts = NULL;
  const int n = spline_flatten (spline, tol, &pts);
  if (n >= min_pts && n <= max_pts && NEAR (pts[n - 1].x, x)
      && NEAR (pts[n - 1].y, y))
    ok ("spline_flatten %s: %d points", name, n);
  else
    fail ("spline_flatten %s: %d points", name, n);
  free (pts);
  return n;
}

static void
spline_flatten_tests (void)
{
  Dwg_SPLINE_control_point bez[4] = { { NULL, 0.0, 0.0, 0.0, 0.0 },
                                      { NULL, 0.0, 4.0, 0.0, 0.0 },
                                      { NULL, 4.0, 4.0, 0.0, 0.0 },
                                      { NULL, 4.0, 0.0, 0.0, 0.0 } };
  Dwg_SPLINE_control_point line[4] = { { NULL, 0.0, 0.0, 0.0, 0.0 },
                                       { NULL, 1.0, 1.0, 0.0, 0.0 },
                                       { NULL, 2.0, 2.0, 0.0, 0.0 },
                                       { NULL, 3.0, 3.0, 0.0, 0.0 } };
  double knots[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };
  BITCODE_3DPOINT fit[3]
      = { { 0.0, 0.0, 0.0 }, { 1.0, 1.0, 0.0 }, { 2.0, 0.0, 0.0 } };
  Dwg_Entity_SPLINE spline;
  int n;

  memset (&spline, 0, sizeof (spline));
  spline.degree = 3;
  spline.num_ctrl_pts = 4;
  spline.ctrl_pts = line;
  spline.num_knots = 8;
  spline.knots = knots;
  // aligned control points: a straight line needs no inner point
  check_flatten ("straight line", &spline, 1e-3, 2, 2, 3.0, 3.0);
  spline.ctrl_pts = bez;
  n = check_flatten ("bezier", &spline, 1e-3, 3, SPLINE_MAX_POINTS, 4.0, 0.0);
  // a finer tolerance needs more points
  check_flatten ("bezier finer", &spline, 1e-4, n + 1, SPLINE_MAX_POINTS, 4.0,
                 0.0);
  // only fit points: interpolated first
  memset (&spline, 0, sizeof (spline));
  spline.degree = 3;
  spline.num_fit_pts = 3;
  spline.fit_pts = fit;
  check_flatten ("fit points", &spline, 1e-3, 3, SPLINE_MAX_POINTS, 2.0, 0.0);
}

// hatch_path_points of path must give a closed ring of this signed area,
// within rel of it
static void
check_ring (const char *name, const Dwg_HATCH_Path *path, const double area,
            const double rel)
{
  BITCODE_2BD *pts = NULL;
  const int n = hatch_path_points (path, 1e-3, &pts);
  const double a = n ? polygon_area (pts, n) : 0.0;
  if (n >= 4 && pts[0].x == pts[n - 1].x && pts[0].y == pts[n - 1].y
      && fabs (a - area) <= rel * fabs (area))
    ok ("hatch_path_points %s: %d points, area %g", name, n, a);
  else
    fail ("hatch_path_points %s: %d points, area %g, expected %g", name, n, a,
          area);
  free (pts);
}

static void
polygon_tests (void)
{
  const BITCODE_2BD sq[4] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
  const BITCODE_2BD cw[4] = { { 0, 0 }, { 0, 1 }, { 1, 1 }, { 1, 0 } };
  const BITCODE_2BD big[5]
      = { { 0, 0 }, { 10, 0 }, { 10, 10 }, { 0, 10 }, { 0, 0 } };
  const BITCODE_2BD touching[5]
      = { { 10, 5 }, { 5, 8 }, { 2, 5 }, { 5, 2 }, { 10, 5 } };
  if (NEAR (polygon_area (sq, 4), 1.0) && NEAR (polygon_area (cw, 4), -1.0))
    ok ("polygon_area orientation");
  else
    fail ("polygon_area orientation");
  if (point_in_polygon (0.5, 0.5, sq, 4) && !point_in_polygon (1.5, 0.5, sq, 4)
      && !point_in_polygon (0.5, -0.1, sq, 4))
    ok ("point_in_polygon");
  else
    fail ("point_in_polygon");
  // a hole touching the right edge of a square at its first vertex: the
  // ray to the right from (10,5) crosses no edge, so point_in_polygon
  // says outside, but the hole is inside
  if (!point_in_polygon (10.0, 5.0, big, 5)
      && polygon_in_polygon (touching, 5, big, 5)
      && !polygon_in_polygon (big, 5, touching, 5)
      && !polygon_in_polygon (big, 5, big, 5))
    ok ("polygon_in_polygon touching");
  else
    fail ("polygon_in_polygon touching");
}

// corrupt or degenerate data gives no ring
static void
check_rejected (const char *name, const Dwg_HATCH_Path *path)
{
  BITCODE_2BD *pts = NULL;
  if (!hatch_path_points (path, 1e-3, &pts) && !pts)
    ok ("hatch_path_points rejects %s", name);
  else
    fail ("hatch_path_points accepted %s", name);
  free (pts);
}

static void
hatch_polyline_tests (void)
{
  Dwg_HATCH_PolylinePath square[4] = { { NULL, { 0, 0 }, 0 },
                                       { NULL, { 1, 0 }, 0 },
                                       { NULL, { 1, 1 }, 0 },
                                       { NULL, { 0, 1 }, 0 } };
  // a circle of radius 1 as two half circles
  Dwg_HATCH_PolylinePath circle[2]
      = { { NULL, { -1, 0 }, 1.0 }, { NULL, { 1, 0 }, 1.0 } };
  Dwg_HATCH_Path path;

  memset (&path, 0, sizeof (path));
  path.flag = 2; // polyline
  path.num_segs_or_paths = 4;
  path.polyline_paths = square;
  check_ring ("polyline square", &path, 1.0, 1e-12);
  // three points on a line enclose no area
  square[1].point.y = 0.0;
  square[2].point.x = 2.0;
  square[2].point.y = 0.0;
  path.num_segs_or_paths = 3;
  check_rejected ("a zero area", &path);
  path.num_segs_or_paths = 2;
  path.polyline_paths = circle;
  path.bulges_present = 1;
  check_ring ("polyline circle", &path, M_PI, 1e-2);
}

static void
hatch_edge_tests (void)
{
  Dwg_HATCH_PathSeg seg[2];
  Dwg_HATCH_ControlPoint cp[4];
  double knots[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };
  Dwg_HATCH_Path path;

  memset (&path, 0, sizeof (path));
  memset (seg, 0, sizeof (seg));
  path.flag = 1; // edges
  path.num_segs_or_paths = 1;
  path.segs = seg;
  seg[0].curve_type = 2; // full circle
  seg[0].radius = 1.0;
  seg[0].end_angle = 2.0 * M_PI;
  seg[0].is_ccw = 1;
  check_ring ("arc circle", &path, M_PI, 1e-2);
  seg[0].curve_type = 3; // full ellipse, semi-axes 2 and 1
  seg[0].endpoint.x = 2.0;
  seg[0].minor_major_ratio = 0.5;
  seg[0].is_ccw = 1;
  check_ring ("ellipse", &path, 2.0 * M_PI, 1e-2);

  // a cubic Bezier from (0,0) over (0,4) (4,4) to (4,0), back by a line:
  // clockwise, area 288 * integral of t^2 (1-t)^2 = 9.6
  memset (seg, 0, sizeof (seg));
  memset (cp, 0, sizeof (cp));
  cp[1].point.y = 4.0;
  cp[2].point.x = 4.0;
  cp[2].point.y = 4.0;
  cp[3].point.x = 4.0;
  seg[0].curve_type = 4;
  seg[0].degree = 3;
  seg[0].num_knots = 8;
  seg[0].knots = knots;
  seg[0].num_control_points = 4;
  seg[0].control_points = cp;
  seg[1].curve_type = 1;
  seg[1].first_endpoint.x = 4.0;
  path.num_segs_or_paths = 2;
  check_ring ("spline and line", &path, -9.6, 1e-2);
}

// the size of a path counts all SPLINE control points: a tall SPLINE
// closed by a half circle of radius 1 needs few points for that arc, as
// the tolerance is relative to the path size of about 100, not 2.8
static void
hatch_size_test (void)
{
  Dwg_HATCH_ControlPoint tall[4];
  Dwg_HATCH_PathSeg two[2];
  double knots[8] = { 0, 0, 0, 0, 1, 1, 1, 1 };
  Dwg_HATCH_Path path;
  BITCODE_2BD *pl = NULL, *pa = NULL;
  int nl, na;

  memset (&path, 0, sizeof (path));
  memset (tall, 0, sizeof (tall));
  memset (two, 0, sizeof (two));
  tall[1].point.y = 100.0;
  tall[2].point.x = 2.0;
  tall[2].point.y = 100.0;
  tall[3].point.x = 2.0;
  two[0].curve_type = 4;
  two[0].degree = 3;
  two[0].num_knots = 8;
  two[0].knots = knots;
  two[0].num_control_points = 4;
  two[0].control_points = tall;
  two[1].curve_type = 1; // closed by a line
  two[1].first_endpoint.x = 2.0;
  path.flag = 1; // edges
  path.num_segs_or_paths = 2;
  path.segs = two;
  nl = hatch_path_points (&path, 1e-3, &pl);
  two[1].curve_type = 2; // closed by a half circle below
  two[1].center.x = 1.0;
  two[1].radius = 1.0;
  two[1].start_angle = M_PI;
  two[1].end_angle = 2.0 * M_PI;
  two[1].is_ccw = 1;
  na = hatch_path_points (&path, 1e-3, &pa);
  if (nl > 4 && na > nl && na - nl <= 6)
    ok ("hatch_path_points size with a SPLINE: arc adds %d points", na - nl);
  else
    fail ("hatch_path_points size with a SPLINE: %d and %d points", nl, na);
  free (pl);
  free (pa);
}

// a clockwise arc as stored in DXF and DWG (from ezdxf: angles 450 and
// 270 degrees), defined in a mirrored system: a 10 x 10 square without
// the half circle of radius 5, clockwise
static void
hatch_cw_arc_test (void)
{
  Dwg_HATCH_PathSeg edges[4];
  Dwg_HATCH_Path path;

  memset (&path, 0, sizeof (path));
  memset (edges, 0, sizeof (edges));
  edges[0].curve_type = 1;
  edges[0].first_endpoint.x = 20.0;
  edges[0].second_endpoint.x = 20.0;
  edges[0].second_endpoint.y = 10.0;
  edges[1].curve_type = 1;
  edges[1].first_endpoint.x = 20.0;
  edges[1].first_endpoint.y = 10.0;
  edges[1].second_endpoint.x = 30.0;
  edges[1].second_endpoint.y = 10.0;
  edges[2].curve_type = 2;
  edges[2].center.x = 30.0;
  edges[2].center.y = 5.0;
  edges[2].radius = 5.0;
  edges[2].start_angle = 2.5 * M_PI;
  edges[2].end_angle = 1.5 * M_PI;
  edges[3].curve_type = 1;
  edges[3].first_endpoint.x = 30.0;
  edges[3].second_endpoint.x = 20.0;
  path.flag = 1; // edges
  path.num_segs_or_paths = 4;
  path.segs = edges;
  check_ring ("clockwise arc", &path, -(100.0 - 12.5 * M_PI), 1e-2);
}

// corrupt data gives no ring: a NaN angle, an infinite radius, and a
// single line encloses nothing
static void
hatch_reject_tests (void)
{
  Dwg_HATCH_PathSeg seg;
  Dwg_HATCH_Path path;

  memset (&path, 0, sizeof (path));
  memset (&seg, 0, sizeof (seg));
  path.flag = 1; // edges
  path.num_segs_or_paths = 1;
  path.segs = &seg;
  seg.curve_type = 2;
  seg.radius = 1.0;
  seg.start_angle = (double)NAN;
  seg.end_angle = 1.0;
  seg.is_ccw = 1;
  check_rejected ("a NaN angle", &path);
  seg.start_angle = 0.0;
  seg.radius = (double)INFINITY;
  check_rejected ("an infinite radius", &path);
  memset (&seg, 0, sizeof (seg));
  seg.curve_type = 1;
  seg.first_endpoint.x = 4.0;
  check_rejected ("a single line", &path);
}

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 2;
  bulge_to_arc_tests ();
  bulge_split_tests ();
  arc_split_tests ();
  ocs_mirrored_tests ();
  spline_eval_tests ();
  spline_to_bezier_tests ();
  spline_from_fit_points_tests ();
  spline_flatten_tests ();
  polygon_tests ();
  hatch_polyline_tests ();
  hatch_edge_tests ();
  hatch_size_test ();
  hatch_cw_arc_test ();
  hatch_reject_tests ();
  return failed;
}

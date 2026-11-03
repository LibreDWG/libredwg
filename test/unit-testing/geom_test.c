#define GEOM_TEST_C
#include <stdlib.h>
#include <math.h>
#include "../../src/geom.c"

#include "tests_common.h"

#define NEAR(a, b) (fabs ((a) - (b)) < 1e-9)

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
  return failed;
}

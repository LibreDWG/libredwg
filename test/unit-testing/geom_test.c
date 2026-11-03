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

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 2;
  bulge_to_arc_tests ();
  bulge_split_tests ();
  arc_split_tests ();
  ocs_mirrored_tests ();
  return failed;
}

#define GEOM_TEST_C
#include <stdlib.h>
#include <math.h>
#include "../../src/geom.c"

#include "tests_common.h"

#define NEAR(a, b) (fabs ((a) - (b)) < 1e-9)

static void
bulge_to_arc_tests (void)
{
  const BITCODE_2BD p1 = { 0.0, 0.0 }, p2 = { 2.0, 0.0 };
  const BITCODE_2BD q1 = { 1.0, 0.0 }, q2 = { 0.0, 1.0 };
  BITCODE_2BD ctr;
  double r, start, sweep;

  // bulge 1: counter-clockwise semicircle below the chord
  if (bulge_to_arc (p1, p2, 1.0, &ctr, &r, &start, &sweep) && NEAR (ctr.x, 1.0)
      && NEAR (ctr.y, 0.0) && NEAR (r, 1.0) && NEAR (start, M_PI)
      && NEAR (sweep, M_PI))
    ok ("bulge_to_arc 1 semicircle");
  else
    fail ("bulge 1: ctr (%g,%g) r %g start %g sweep %g", ctr.x, ctr.y, r,
          start, sweep);

  // bulge -1: the same circle, clockwise
  if (bulge_to_arc (p1, p2, -1.0, &ctr, &r, &start, &sweep)
      && NEAR (ctr.x, 1.0) && NEAR (ctr.y, 0.0) && NEAR (r, 1.0)
      && NEAR (sweep, -M_PI))
    ok ("bulge_to_arc -1 clockwise");
  else
    fail ("bulge -1: ctr (%g,%g) r %g sweep %g", ctr.x, ctr.y, r, sweep);

  // tan(PI/8): a counter-clockwise quarter of the unit circle
  if (bulge_to_arc (q1, q2, tan (M_PI / 8.0), &ctr, &r, &start, &sweep)
      && NEAR (ctr.x, 0.0) && NEAR (ctr.y, 0.0) && NEAR (r, 1.0)
      && NEAR (start, 0.0) && NEAR (sweep, M_PI / 2.0))
    ok ("bulge_to_arc quarter circle");
  else
    fail ("quarter: ctr (%g,%g) r %g start %g sweep %g", ctr.x, ctr.y, r,
          start, sweep);

  // straight segments
  if (!bulge_to_arc (p1, p2, 0.0, &ctr, &r, &start, &sweep)
      && !bulge_to_arc (p1, p1, 1.0, &ctr, &r, &start, &sweep)
      && !bulge_to_arc (p1, p2, NAN, &ctr, &r, &start, &sweep)
      && !bulge_to_arc (p1, p2, INFINITY, &ctr, &r, &start, &sweep))
    ok ("bulge_to_arc straight segments");
  else
    fail ("straight segment taken as an arc");
}

static void
bulge_split_tests (void)
{
  const BITCODE_2BD p1 = { 0.0, 0.0 }, p2 = { 2.0, 0.0 };
  BITCODE_2BD pts[4];
  int n, i, on_circle = 1;

  // 4 points of the semicircle, p2 excluded: angles PI, 5PI/4, 3PI/2, 7PI/4
  n = bulge_split (pts, 4, p1, p2, 1.0);
  for (i = 0; i < n; i++)
    if (!NEAR (hypot (pts[i].x - 1.0, pts[i].y), 1.0))
      on_circle = 0;
  if (n == 4 && pts[0].x == 0.0 && pts[0].y == 0.0 && NEAR (pts[2].x, 1.0)
      && NEAR (pts[2].y, -1.0) && on_circle)
    ok ("bulge_split semicircle");
  else
    fail ("bulge_split semicircle: n %d pts[2] (%g,%g)", n, pts[2].x,
          pts[2].y);

  // clockwise passes above the chord
  n = bulge_split (pts, 4, p1, p2, -1.0);
  if (n == 4 && NEAR (pts[2].x, 1.0) && NEAR (pts[2].y, 1.0))
    ok ("bulge_split clockwise");
  else
    fail ("bulge_split clockwise: n %d pts[2] (%g,%g)", n, pts[2].x, pts[2].y);

  // a straight segment gives just its start point
  n = bulge_split (pts, 4, p1, p2, 0.0);
  if (n == 1 && pts[0].x == 0.0 && pts[0].y == 0.0)
    ok ("bulge_split straight");
  else
    fail ("bulge_split straight: n %d", n);

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

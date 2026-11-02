#define GEOM_TEST_C
#include <stdlib.h>
#include <math.h>
#include "../../src/geom.c"

#include "tests_common.h"

#define NEAR(a, b) (fabs ((a) - (b)) < 1e-9)

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

int
main (int argc, char const *argv[])
{
  loglevel = is_make_silent () ? 0 : 2;
  arc_split_tests ();
  return failed;
}

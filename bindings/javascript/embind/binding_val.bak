
#include <emscripten/bind.h>
#include "dwg.h"
#include "dwg_api.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(libredwg_val) {
  value_object<dwg_point_3d>("dwg_point_3d")
    .field("x", &dwg_point_3d::x)
    .field("y", &dwg_point_3d::y)
    .field("z", &dwg_point_3d::z);

  value_object<dwg_point_2d>("dwg_point_2d")
    .field("x", &dwg_point_2d::x)
    .field("y", &dwg_point_2d::y);
}
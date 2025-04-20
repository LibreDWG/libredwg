#include "binding_common.h"

emscripten::val handle_to_js_object(Dwg_Handle* handle) {
  emscripten::val handle_obj = emscripten::val::object();
  handle_obj.set("code", handle->code);
  handle_obj.set("size", handle->size);
  handle_obj.set("value", handle->value);
  handle_obj.set("is_global", handle->is_global);
  return handle_obj;
}

emscripten::val object_ref_to_js_object(BITCODE_H object_ref) {
  emscripten::val handle_obj = emscripten::val::object();
  handle_obj.set("obj", reinterpret_cast<uintptr_t>(object_ref->obj));
  handle_obj.set("handleref", handle_to_js_object(&object_ref->handleref));
  handle_obj.set("absolute_ref", object_ref->absolute_ref);
  handle_obj.set("r11_idx", object_ref->r11_idx);
  return handle_obj;
}

emscripten::val color_to_js_object(const Dwg_Color* color) {
  emscripten::val color_obj = emscripten::val::object();
  color_obj.set("index", color->index);
  color_obj.set("flag", color->flag);
  // TODO: combine 'rgb' and 'alpha' together and convert them to 'rgba'
  color_obj.set("rgb", color->rgb);
  color_obj.set("name", std::string(color->name));
  color_obj.set("book_name", std::string(color->book_name));
  return color_obj;
}

emscripten::val point2d_to_js_object(const dwg_point_2d* point) {
  emscripten::val point_obj = emscripten::val::object();
  point_obj.set("x", point->x);
  point_obj.set("y", point->y);
  return point_obj;
}

emscripten::val bitcode_2rd_to_js_object(const BITCODE_2RD* point) {
  emscripten::val point_obj = emscripten::val::object();
  point_obj.set("x", point->x);
  point_obj.set("y", point->y);
  return point_obj;
}

emscripten::val point3d_to_js_object(const dwg_point_3d* point) {
  emscripten::val point_obj = emscripten::val::object();
  point_obj.set("x", point->x);
  point_obj.set("y", point->y);
  point_obj.set("z", point->z);
  return point_obj;
}

emscripten::val bitcode_3rd_to_js_object(const BITCODE_3RD* point) {
  emscripten::val point_obj = emscripten::val::object();
  point_obj.set("x", point->x);
  point_obj.set("y", point->y);
  point_obj.set("z", point->z);
  return point_obj;
}

emscripten::val point4d_to_js_object(const Dwg_SPLINE_control_point* point) {
  emscripten::val point_obj = emscripten::val::object();
  point_obj.set("x", point->x);
  point_obj.set("y", point->y);
  point_obj.set("z", point->z);
  point_obj.set("w", point->w);
  return point_obj;
}

/***********************************************************************/

/**
 * Return one JavaScript object for Dwg_Handle instance
 */
emscripten::val dwg_handle_wrapper(Dwg_Handle_Ptr handle_ptr) {
  Dwg_Handle* ptr = reinterpret_cast<Dwg_Handle*>(handle_ptr);
  return handle_to_js_object(ptr);
}

/**
 * Return one JavaScript object for Dwg_Object_Ref instance
 */
emscripten::val dwg_object_ref_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return object_ref_to_js_object(ref);
}

EMSCRIPTEN_BINDINGS(libredwg_type) {
  DEFINE_FUNC(dwg_handle);
  DEFINE_FUNC(dwg_object_ref);
}

/***********************************************************************/

#define DEFINE_ARRAY_FUNC(type)                                                        \
emscripten::val dwg_ptr_to_##type##_array_wrapper(uintptr_t array_ptr, size_t size) {  \
  type* array = reinterpret_cast<type*>(array_ptr);                                    \
  emscripten::val jsArray = emscripten::val::array();                                  \
  for (int index = 0; index < size; ++index) {                                         \
    jsArray.call<void>("push", array[index]);                                          \
  }                                                                                    \
  return jsArray;                                                                      \
}

DEFINE_ARRAY_FUNC(double)
DEFINE_ARRAY_FUNC(uint16_t)
DEFINE_ARRAY_FUNC(int16_t)
DEFINE_ARRAY_FUNC(uint32_t)
DEFINE_ARRAY_FUNC(int32_t)
DEFINE_ARRAY_FUNC(uint64_t)
DEFINE_ARRAY_FUNC(int64_t)


emscripten::val dwg_ptr_to_unsigned_char_array_wrapper(uintptr_t array_ptr, size_t size) {
  unsigned char* array = reinterpret_cast<unsigned char*>(array_ptr);
  emscripten::val jsArray = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    jsArray.call<void>("push", array[index]);
  }
  return jsArray;
}

emscripten::val dwg_ptr_to_signed_char_array_wrapper(uintptr_t array_ptr, size_t size) {
  signed char* array = reinterpret_cast<signed char*>(array_ptr);
  emscripten::val jsArray = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    jsArray.call<void>("push", array[index]);
  }
  return jsArray;
}

emscripten::val dwg_ptr_to_point2d_array_wrapper(uintptr_t array_ptr, size_t size) {
  dwg_point_2d* array = reinterpret_cast<dwg_point_2d*>(array_ptr);

  emscripten::val points_obj = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    auto point = array[index];
    emscripten::val point_obj = point2d_to_js_object(&point);
    points_obj.call<void>("push", point_obj);
  }
  return points_obj;
}

emscripten::val dwg_ptr_to_point3d_array_wrapper(uintptr_t array_ptr, size_t size) {
  dwg_point_3d* array = reinterpret_cast<dwg_point_3d*>(array_ptr);

  emscripten::val points_obj = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    auto point = array[index];
    emscripten::val point_obj = point3d_to_js_object(&point);
    points_obj.call<void>("push", point_obj);
  }
  return points_obj;
}

emscripten::val dwg_ptr_to_point4d_array_wrapper(uintptr_t array_ptr, size_t size) {
  Dwg_SPLINE_control_point* array = reinterpret_cast<Dwg_SPLINE_control_point*>(array_ptr);

  emscripten::val points_obj = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    emscripten::val point_obj = emscripten::val::object();
    auto point = array[index];
    point_obj.set("x", point.x);
    point_obj.set("y", point.y);
    point_obj.set("z", point.z);
    point_obj.set("w", point.w);
    points_obj.call<void>("push", point_obj);
  }
  return points_obj;
}

emscripten::val dwg_ptr_to_ltype_dash_array_wrapper(uintptr_t array_ptr, size_t size) {
  Dwg_LTYPE_dash* array = reinterpret_cast<Dwg_LTYPE_dash*>(array_ptr);

  emscripten::val dashes = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    emscripten::val dash_obj = emscripten::val::object();
    auto dash = array[index];
    dash_obj.set("length", dash.length);
    dash_obj.set("complex_shapecode", dash.complex_shapecode);
    dash_obj.set("style", reinterpret_cast<uintptr_t>(dash.style));
    dash_obj.set("x_offset", dash.x_offset);
    dash_obj.set("y_offset", dash.y_offset);
    dash_obj.set("scale", dash.scale);
    dash_obj.set("rotation", dash.rotation);
    dash_obj.set("shape_flag", dash.shape_flag);
    dash_obj.set("text", std::string(dash.text));
    dashes.call<void>("push", dash_obj);
  }
  return dashes;
}

emscripten::val dwg_ptr_to_table_cell_array_wrapper(uintptr_t array_ptr, size_t size) {
  Dwg_TABLE_Cell* array = reinterpret_cast<Dwg_TABLE_Cell*>(array_ptr);

  emscripten::val cells = emscripten::val::array();
  for (int i = 0; i < size; ++i) {
    emscripten::val cell_obj = emscripten::val::object();
    auto cell = array[i];
    cell_obj.set("type", cell.type);
    cell_obj.set("flags", cell.flags);
    cell_obj.set("is_merged_value", cell.is_merged_value);
    cell_obj.set("is_autofit_flag", cell.is_autofit_flag);
    cell_obj.set("merged_width_flag", cell.merged_width_flag);
    cell_obj.set("merged_height_flag", cell.merged_height_flag);
    cell_obj.set("rotation", cell.type);
    cell_obj.set("text_value", std::string(cell.text_value));
    cell_obj.set("text_style", reinterpret_cast<uintptr_t>(cell.text_style));
    cell_obj.set("block_handle", object_ref_to_js_object(cell.block_handle));
    cell_obj.set("block_scale", cell.block_scale);
    cell_obj.set("additional_data_flag", cell.additional_data_flag);
    cell_obj.set("cell_flag_override", cell.cell_flag_override);
    cell_obj.set("virtual_edge_flag", cell.virtual_edge_flag);
    cell_obj.set("cell_alignment", cell.cell_alignment);
    cell_obj.set("bg_fill_none", cell.bg_fill_none);
    cell_obj.set("bg_color", color_to_js_object(&cell.bg_color));
    cell_obj.set("content_color", color_to_js_object(&cell.content_color));
    cell_obj.set("text_height", cell.text_height);
    cell_obj.set("top_grid_color", color_to_js_object(&cell.top_grid_color));
    cell_obj.set("top_grid_linewt", cell.top_grid_linewt);
    cell_obj.set("top_visibility", cell.top_visibility);
    cell_obj.set("right_grid_color", color_to_js_object(&cell.right_grid_color));
    cell_obj.set("right_grid_linewt", cell.right_grid_linewt);
    cell_obj.set("right_visibility", cell.right_visibility);
    cell_obj.set("bottom_grid_color", color_to_js_object(&cell.bottom_grid_color));
    cell_obj.set("bottom_grid_linewt", cell.bottom_grid_linewt);
    cell_obj.set("bottom_visibility", cell.bottom_visibility);
    cell_obj.set("left_grid_color", color_to_js_object(&cell.left_grid_color));
    cell_obj.set("left_grid_linewt", cell.left_grid_linewt);
    cell_obj.set("left_visibility", cell.left_visibility);
    cell_obj.set("num_attr_defs", cell.num_attr_defs);

    emscripten::val attr_defs_obj = emscripten::val::array();
    for (int i = 0; i < size; ++i) {
      auto attr_def = cell.attr_defs[i];
      emscripten::val attr_def_obj = emscripten::val::object();
      attr_def_obj.set("attdef", object_ref_to_js_object(attr_def.attdef));
      attr_def_obj.set("text", std::string(attr_def.text));
      attr_def_obj.set("index", attr_def.index);
      attr_defs_obj.call<void>("push", attr_def_obj);
    }
    cell_obj.set("attr_defs", attr_defs_obj);
    
    cells.call<void>("push", cell_obj);
  }
  return cells;
}

emscripten::val dwg_ptr_to_hatch_defline_array_wrapper(uintptr_t array_ptr, size_t size) {
  Dwg_HATCH_DefLine* array = reinterpret_cast<Dwg_HATCH_DefLine*>(array_ptr);
  emscripten::val deflines = emscripten::val::array();
  for (int index = 0; index < size; ++index) {
    emscripten::val defline_obj = emscripten::val::object();
    auto defline = array[index];
    defline_obj.set("angle", defline.angle);
    defline_obj.set("pt0", point2d_to_js_object(reinterpret_cast<const dwg_point_2d*>(&defline.pt0)));
    defline_obj.set("offset", point2d_to_js_object(reinterpret_cast<const dwg_point_2d*>(&defline.offset)));
    defline_obj.set("dashes", dwg_ptr_to_double_array_wrapper(reinterpret_cast<uintptr_t>(defline.dashes), defline.num_dashes));
    deflines.call<void>("push", defline_obj);
  }
  return deflines;
}

emscripten::val dwg_ptr_to_hatch_path_array_wrapper(uintptr_t array_ptr, size_t size) {
  Dwg_HATCH_Path* array = reinterpret_cast<Dwg_HATCH_Path*>(array_ptr);
  emscripten::val pathes = emscripten::val::array();
  for (int i = 0; i < size; ++i) {
    emscripten::val path_obj = emscripten::val::object();
    auto path = array[i];
    // 2: is_polyline, 4: is_derived, 8: is_textbox,
    // 0x20: is_open, 0x80: is_textisland, 0x100: is_duplicate,
    // 0x200: is_annotative
    path_obj.set("flag", path.flag);
    auto num_segs_or_paths = path.num_segs_or_paths;
    path_obj.set("num_segs_or_paths", num_segs_or_paths);

    // Convert segment path
    if (path.segs) {
      emscripten::val segs = emscripten::val::array();
      for (int j = 0; j < num_segs_or_paths; ++j) {
        auto seg = path.segs[j];
        emscripten::val seg_obj = emscripten::val::object();

        // Edge type (only if boundary is not a polyline)
        // - 1: Line
        // - 2: Circular arc
        // - 3: Elliptic arc
        // - 4: Spline
        auto curve_type = seg.curve_type;
        seg_obj.set("curve_type", seg.curve_type);

        if (curve_type == 1) {
          // Line
          seg_obj.set("first_endpoint", bitcode_2rd_to_js_object(&seg.first_endpoint));
          seg_obj.set("second_endpoint", bitcode_2rd_to_js_object(&seg.second_endpoint));
        } else if (curve_type == 2) {
          // Circular arc
          seg_obj.set("center", bitcode_2rd_to_js_object(&seg.center));
          seg_obj.set("radius", seg.radius);
          seg_obj.set("start_angle", seg.start_angle);
          seg_obj.set("end_angle", seg.end_angle);
          seg_obj.set("is_ccw", seg.is_ccw);
        } else if (curve_type == 3) {
          // Elliptic arc
          seg_obj.set("center", bitcode_2rd_to_js_object(&seg.center));
          seg_obj.set("endpoint", bitcode_2rd_to_js_object(&seg.endpoint));
          seg_obj.set("minor_major_ratio", seg.minor_major_ratio);
          seg_obj.set("start_angle", seg.start_angle);
          seg_obj.set("end_angle", seg.end_angle);
          seg_obj.set("is_ccw", seg.is_ccw);
        } else if (curve_type == 4) {
          // Elliptic arc
          seg_obj.set("degree", seg.degree);
          seg_obj.set("is_rational", seg.is_rational);
          seg_obj.set("is_periodic", seg.is_periodic);

          // Convert knots
          auto num_knots = seg.num_knots;
          seg_obj.set("num_knots", num_knots);
          seg_obj.set("knots", dwg_ptr_to_double_array_wrapper(reinterpret_cast<uintptr_t>(seg.knots), num_knots));

          // Convert hatch control points
          auto num_control_points = seg.num_control_points;
          seg_obj.set("num_control_points", num_control_points);
          emscripten::val control_points = emscripten::val::array();
          for (int k = 0; k < num_control_points; ++k) {
            auto control_point = seg.control_points[k];
            emscripten::val control_point_obj = emscripten::val::object();
            control_point_obj.set("x", control_point.point.x);
            control_point_obj.set("y", control_point.point.y);
            control_point_obj.set("w", control_point.weight);
            control_points.call<void>("push", control_point_obj);
          }
          seg_obj.set("control_points", control_points);

          // Convert fit points
          auto num_fitpts = seg.num_fitpts;
          seg_obj.set("num_fitpts", num_fitpts);
          seg_obj.set("fitpts", dwg_ptr_to_point2d_array_wrapper(reinterpret_cast<uintptr_t>(seg.fitpts), num_fitpts));
          seg_obj.set("start_tangent", bitcode_2rd_to_js_object(&seg.start_tangent));
          seg_obj.set("end_tangent", bitcode_2rd_to_js_object(&seg.end_tangent));
        }

        segs.call<void>("push", seg_obj);
      }
      path_obj.set("segs", segs);
    }

    // Convert polyline path
    if (path.polyline_paths) {
      auto bulges_present = path.bulges_present;
      path_obj.set("bulges_present", bulges_present);
      path_obj.set("closed", path.closed);
      emscripten::val polyline_paths = emscripten::val::array();
      for (int j = 0; j < num_segs_or_paths; ++j) {
        auto polyline_path = path.polyline_paths[j];
        emscripten::val polyline_path_obj = emscripten::val::object();
        polyline_path_obj.set("point", bitcode_2rd_to_js_object(&polyline_path.point));
        if (bulges_present) {
          polyline_path_obj.set("bulge", polyline_path.bulge);
        }
        polyline_paths.call<void>("push", polyline_path_obj);
      }

      path_obj.set("polyline_paths", polyline_paths);
    }

    pathes.call<void>("push", path_obj);
  }
  return pathes;
}

EMSCRIPTEN_BINDINGS(libredwg_array) {
  DEFINE_FUNC(dwg_ptr_to_unsigned_char_array);
  DEFINE_FUNC(dwg_ptr_to_signed_char_array);
  DEFINE_FUNC(dwg_ptr_to_uint16_t_array);
  DEFINE_FUNC(dwg_ptr_to_int16_t_array);
  DEFINE_FUNC(dwg_ptr_to_uint32_t_array);
  DEFINE_FUNC(dwg_ptr_to_int32_t_array);
  DEFINE_FUNC(dwg_ptr_to_uint64_t_array);
  DEFINE_FUNC(dwg_ptr_to_int64_t_array);
  DEFINE_FUNC(dwg_ptr_to_double_array);
  DEFINE_FUNC(dwg_ptr_to_point2d_array);
  DEFINE_FUNC(dwg_ptr_to_point3d_array);
  DEFINE_FUNC(dwg_ptr_to_point4d_array);
  DEFINE_FUNC(dwg_ptr_to_ltype_dash_array);
  DEFINE_FUNC(dwg_ptr_to_table_cell_array);
  DEFINE_FUNC(dwg_ptr_to_hatch_defline_array);
  DEFINE_FUNC(dwg_ptr_to_hatch_path_array);
}

/***********************************************************************/

Dwg_Object_Object_Ptr dwg_object_get_tio_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  // The address of 'tio.entity' is same as the address of 'tio.object'.
  return (obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj->tio.object);
}

int dwg_object_get_type_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : obj->type;
}

int dwg_object_get_fixedtype_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : static_cast<int>(obj->fixedtype);
}

int dwg_object_get_supertype_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : static_cast<int>(obj->supertype);
}

/**
 * public entity/object name
 */
std::string dwg_object_get_name_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : std::string(obj->name);
}

/**
 * The internal dxf classname, often with a ACDB prefix
 */
std::string dwg_object_get_dxfname_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : std::string(obj->dxfname);
}

int dwg_object_get_handle_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return (obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(&(obj->handle));
}

emscripten::val dwg_object_get_handle_object_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return handle_to_js_object(&(obj->handle));
}

/**
 * Methods to access fields of Dwg_Object
 */
EMSCRIPTEN_BINDINGS(libredwg_dwg_object) {
  DEFINE_FUNC(dwg_object_get_tio);
  DEFINE_FUNC(dwg_object_get_type);
  DEFINE_FUNC(dwg_object_get_supertype);
  DEFINE_FUNC(dwg_object_get_fixedtype);
  DEFINE_FUNC(dwg_object_get_name);
  DEFINE_FUNC(dwg_object_get_dxfname);
  DEFINE_FUNC(dwg_object_get_handle);
  DEFINE_FUNC(dwg_object_get_handle_object);
}


/***********************************************************************/

Dwg_Object_Object_Ptr dwg_object_object_get_tio_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->tio.APPID);
}

int dwg_object_object_get_objid_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : obj_obj->objid;
}

uintptr_t dwg_object_object_get_ownerhandle_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->ownerhandle);
}

emscripten::val dwg_object_object_get_ownerhandle_object_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return object_ref_to_js_object(obj_obj->ownerhandle);
}

uintptr_t dwg_object_object_get_handle_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->handleref);
}

emscripten::val dwg_object_object_get_handle_object_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return handle_to_js_object(obj_obj->handleref);
}

int dwg_object_object_get_num_reactors_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : obj_obj->num_reactors;
}

uintptr_t dwg_object_object_get_reactors_wrapper(Dwg_Object_Object_Ptr obj_obj_ptr) {
  Dwg_Object_Object* obj_obj = reinterpret_cast<Dwg_Object_Object*>(obj_obj_ptr);
  return (obj_obj == NULL) ? 0 : reinterpret_cast<uintptr_t>(obj_obj->reactors);
}

/**
 * Methods to access fields of Dwg_Object_Object
 */
EMSCRIPTEN_BINDINGS(libredwg_dwg_object_object) {
  DEFINE_FUNC(dwg_object_object_get_tio);
  DEFINE_FUNC(dwg_object_object_get_objid);
  DEFINE_FUNC(dwg_object_object_get_ownerhandle);
  DEFINE_FUNC(dwg_object_object_get_ownerhandle_object);
  DEFINE_FUNC(dwg_object_object_get_handle);
  DEFINE_FUNC(dwg_object_object_get_handle_object);
  DEFINE_FUNC(dwg_object_object_get_num_reactors);
  DEFINE_FUNC(dwg_object_object_get_reactors);
}

/***********************************************************************/

uintptr_t dwg_object_entity_get_ownerhandle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return reinterpret_cast<uintptr_t>(dwg_ent_get_ownerhandle(ent, &error));
}

emscripten::val dwg_object_entity_get_ownerhandle_object_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  Dwg_Object_Ref* ownerhandle = dwg_ent_get_ownerhandle(ent, &error);
  return object_ref_to_js_object(ownerhandle);
}

uintptr_t dwg_object_entity_get_handle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  Dwg_Object* obj = dwg_ent_to_object(ent, &error);
  return reinterpret_cast<uintptr_t>(dwg_object_get_handle(obj, &error));
}

emscripten::val dwg_object_entity_get_handle_object_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  Dwg_Object* obj = dwg_ent_to_object(ent, &error);
  Dwg_Handle* handle = dwg_object_get_handle(obj, &error);
  return handle_to_js_object(handle);
}

emscripten::val dwg_object_entity_get_color_object_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  const Dwg_Color* color = dwg_ent_get_color(ent, &error);
  return color_to_js_object(color);
}

/**
 * Get the name of the layer referenced by this entity
 */
std::string dwg_object_entity_get_layer_name_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_layer_name(ent, &error);
}

/**
 * Get the name of the line type referenced by this entity
 */
std::string dwg_object_entity_get_ltype_name_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_ltype_name(ent, &error);
}

double dwg_object_entity_get_ltype_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_linetype_flags(ent, &error);
}

/**
 * Get The line type scale factor of this entity.
 */
double dwg_object_entity_get_ltype_scale_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_linetype_scale(ent, &error);
}

/**
 * Get the line weight used by this entity.
 */
BITCODE_RC dwg_object_entity_get_line_weight_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_linewt(ent, &error);
}

/**
 * Get the entity mode of this entity
 * - 0: has no ownerhandle
 * - 1: paper space
 * - 2: model space
 * - 3: has ownerhandle
 */
BITCODE_BB dwg_object_entity_get_entmode_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_entmode(ent, &error);
}

/**
 * Get the invisibility state of this entity.
 */
BITCODE_BS dwg_object_entity_get_invisible_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_invisible(ent, &error);
}

BITCODE_BB dwg_object_entity_get_plotstyle_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_plotstyle_flags(ent, &error);
}

BITCODE_BB dwg_object_entity_get_material_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_material_flags(ent, &error);
}

BITCODE_BB dwg_object_entity_get_shadow_flags_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_shadow_flags(ent, &error);
}

BITCODE_BB dwg_object_entity_has_full_visualstyle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_has_full_visualstyle(ent, &error);
}

BITCODE_BB dwg_object_entity_has_face_visualstyle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_has_face_visualstyle(ent, &error);
}

BITCODE_BB dwg_object_entity_has_edge_visualstyle_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_has_edge_visualstyle(ent, &error);
}

BITCODE_BL dwg_object_entity_get_num_reactors_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return dwg_ent_get_num_reactors(ent, &error);
}

uintptr_t dwg_object_entity_get_reactors_wrapper(Dwg_Object_Entity_Ptr ent_ptr) {
  Dwg_Object_Entity* ent = reinterpret_cast<Dwg_Object_Entity*>(ent_ptr);
  int error = 0;
  return reinterpret_cast<uintptr_t>(dwg_ent_get_reactors(ent, &error));
}

/**
 * Methods to access fields of Dwg_Object_Entity
 */
EMSCRIPTEN_BINDINGS(libredwg_dwg_object_entity) {
  DEFINE_FUNC(dwg_object_entity_get_ownerhandle);
  DEFINE_FUNC(dwg_object_entity_get_ownerhandle_object);
  DEFINE_FUNC(dwg_object_entity_get_handle);
  DEFINE_FUNC(dwg_object_entity_get_handle_object);
  DEFINE_FUNC(dwg_object_entity_get_color_object);
  DEFINE_FUNC(dwg_object_entity_get_layer_name);
  DEFINE_FUNC(dwg_object_entity_get_ltype_name);
  DEFINE_FUNC(dwg_object_entity_get_ltype_flags);
  DEFINE_FUNC(dwg_object_entity_get_ltype_scale);
  DEFINE_FUNC(dwg_object_entity_get_line_weight);
  DEFINE_FUNC(dwg_object_entity_get_entmode);
  DEFINE_FUNC(dwg_object_entity_get_invisible);
  DEFINE_FUNC(dwg_object_entity_get_plotstyle_flags);
  DEFINE_FUNC(dwg_object_entity_get_material_flags);
  DEFINE_FUNC(dwg_object_entity_get_shadow_flags);
  DEFINE_FUNC(dwg_object_entity_has_full_visualstyle);
  DEFINE_FUNC(dwg_object_entity_has_face_visualstyle);
  DEFINE_FUNC(dwg_object_entity_has_edge_visualstyle);
  DEFINE_FUNC(dwg_object_entity_get_num_reactors);
  DEFINE_FUNC(dwg_object_entity_get_reactors);
}

/***********************************************************************/

/**
 * Returns the absolute handle reference (field 'absolute_ref') of Dwg_Object_Ref*
 */
BITCODE_BL dwg_ref_get_absref_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return (ref == 0) ? 0 : ref->absolute_ref;
}

/**
 * Returns Dwg_Object* from Dwg_Object_Ref*
 */
Dwg_Object_Ptr dwg_ref_get_object_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  int error = 0;
  return reinterpret_cast<Dwg_Object_Ptr>(dwg_ref_get_object(ref, &error));
}

int dwg_ref_get_handle_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return (ref == NULL) ? 0 : reinterpret_cast<uintptr_t>(&(ref->handleref));
}

emscripten::val dwg_ref_get_handle_object_wrapper(Dwg_Object_Ref_Ptr ref_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return handle_to_js_object(&(ref->handleref));
}

EMSCRIPTEN_BINDINGS(libredwg_dwg_object_ref) {
  DEFINE_FUNC(dwg_ref_get_absref);
  DEFINE_FUNC(dwg_ref_get_object);
  DEFINE_FUNC(dwg_ref_get_handle);
  DEFINE_FUNC(dwg_ref_get_handle_object);
}

/***********************************************************************/

uintptr_t dwg_ref_object_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t ref_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return reinterpret_cast<uintptr_t>(dwg_ref_object(dwg, ref));
}

uintptr_t dwg_ref_object_relative_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t ref_ptr,
  uintptr_t obj_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return reinterpret_cast<uintptr_t>(dwg_ref_object_relative(dwg, ref, obj));
}

uintptr_t dwg_ref_object_silent_wrapper(
  uintptr_t dwg_ptr,
  uintptr_t ref_ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  return reinterpret_cast<uintptr_t>(dwg_ref_object_silent(dwg, ref));
}

/** 
 * Resolve handle absref value to Dwg_Object*. 
*/
uintptr_t dwg_resolve_handle_wrapper(
  uintptr_t dwg_ptr,
  BITCODE_RLL absref) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_resolve_handle(dwg, absref));
}

/** 
 * Resolve handle to Dwg_Object* without warning info
*/
uintptr_t dwg_resolve_handle_silent_wrapper(
  uintptr_t dwg_ptr,
  const BITCODE_RLL absref) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(dwg_ptr);
  return reinterpret_cast<uintptr_t>(dwg_resolve_handle_silent(dwg, absref));
}

/** 
 * Resolve Dwg_Object_Ref* to Dwg_Object*. 
*/
int dwg_resolve_handleref_wrapper(
  uintptr_t ref_ptr,
  uintptr_t obj_ptr) {
  Dwg_Object_Ref* ref = reinterpret_cast<Dwg_Object_Ref*>(ref_ptr);
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  return dwg_resolve_handleref(ref, obj);
}

EMSCRIPTEN_BINDINGS(libredwg_conversion) {
  DEFINE_FUNC(dwg_ref_object);
  DEFINE_FUNC(dwg_ref_object_relative);
  DEFINE_FUNC(dwg_ref_object_silent);
  DEFINE_FUNC(dwg_resolve_handle);
  DEFINE_FUNC(dwg_resolve_handle_silent);
  DEFINE_FUNC(dwg_resolve_handleref);
}
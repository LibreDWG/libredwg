#include <emscripten/bind.h>
#include <string>

#include "dwg.h"
#include "dwg_api.h"
#include "binding_common.h"


using namespace emscripten;

emscripten::val dwg_entity_polyline_2d_get_numpoints_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  int error = 0;
  auto numpoints = dwg_object_polyline_2d_get_numpoints(obj, &error);

  emscripten::val result = emscripten::val::object();
  result.set("error", error);
  result.set("data", numpoints);
  return result;
}

emscripten::val dwg_entity_polyline_2d_get_points_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  int error = 0;
  auto numpoints = dwg_object_polyline_2d_get_numpoints(obj, &error);
  if (error != 0) {
    emscripten::val result = emscripten::val::object();
    result.set("success", false);
    result.set("message", std::string("Failed to get the number of points!"));
    result.set("data", numpoints);
    return result;
  }
  
  auto points = dwg_object_polyline_2d_get_points(obj, &error);
  if (error != 0) {
    emscripten::val result = emscripten::val::object();
    result.set("success", false);
    result.set("message", std::string("Failed to get points!"));
    result.set("data", points);
    return result;
  }

  emscripten::val result = emscripten::val::object();
  emscripten::val points_obj = emscripten::val::array();
  for (int index = 0; index < numpoints; ++index) {
    auto point = points[index];
    emscripten::val point_obj = point2d_to_js_object(&point);
    points_obj.call<void>("push", point_obj);
  }

  result.set("success", true);
  result.set("data", points_obj);
  free(points);
  return result;
}

emscripten::val vertex2d_to_js_object(Dwg_Entity_VERTEX_2D* vertex) {
  emscripten::val vertex_obj = emscripten::val::object();
  vertex_obj.set("flag", vertex->flag);
  vertex_obj.set("point", point3d_to_js_object(reinterpret_cast<dwg_point_3d*>(&(vertex->point))));
  vertex_obj.set("start_width", vertex->start_width);
  vertex_obj.set("end_width", vertex->end_width);
  vertex_obj.set("bulge", vertex->bulge);
  vertex_obj.set("tangent_dir", vertex->tangent_dir);
  return vertex_obj;
}

emscripten::val dwg_entity_polyline_2d_get_vertices_wrapper(Dwg_Object_Ptr obj_ptr) {
  Dwg_Object* obj = reinterpret_cast<Dwg_Object*>(obj_ptr);
  emscripten::val result = emscripten::val::object();

  int error = 0;
  if (obj && obj->fixedtype == DWG_TYPE_POLYLINE_2D) {
    BITCODE_BL i;
    Dwg_Data *dwg = obj->parent;
    Dwg_Entity_POLYLINE_2D *_obj = obj->tio.entity->tio.POLYLINE_2D;
    BITCODE_BL num_points
        = dwg_object_polyline_2d_get_numpoints (obj, &error);
    Dwg_Entity_VERTEX_2D *vertex = NULL;
    dwg_point_2d *ptx;

    if (!num_points || error) {
      result.set("success", false);
      result.set("message", std::string("Failed to get the number of vertices!"));
      return result;
    }

    emscripten::val vertices_obj = emscripten::val::array();
    if (dwg->header.version >= R_2004) {
      for (i = 0; i < num_points; i++) {
        Dwg_Object *vobj = dwg_ref_object (dwg, _obj->vertex[i]);
        if (vobj && vobj->fixedtype == DWG_TYPE_VERTEX_2D
            && (vertex = dwg_object_to_VERTEX_2D (vobj))) {
          emscripten::val vertex_obj = vertex2d_to_js_object(vertex);
          vertices_obj.call<void>("push", vertex_obj);
        } else {
          result.set("success", false);
          result.set("message", std::string("Not all of vertices returned!"));
        }
      }
      result.set("data", vertices_obj);
    }
    // iterate over first_vertex - last_vertex
    else if (dwg->header.version >= R_13b1) {
      Dwg_Object *vobj = dwg_ref_object (dwg, _obj->first_vertex);
      Dwg_Object *vlast = dwg_ref_object (dwg, _obj->last_vertex);
      if (!vobj) {
        result.set("success", false);
        result.set("message", std::string("Failed to get points!"));
      } else {
        i = 0;
        do {
          if (vobj->fixedtype == DWG_TYPE_VERTEX_2D
              && (vertex = dwg_object_to_VERTEX_2D (vobj))) {
            emscripten::val vertex_obj = vertex2d_to_js_object(vertex);
            vertices_obj.call<void>("push", vertex_obj);
            i++;
            if (i > num_points) {
              result.set("success", false);
              result.set("message", std::string("Index error! Exceed the number of vertices!"));
              break;
            }
          } else {
            result.set("success", false);
            result.set("message", std::string("Not all of vertices returned!"));
          }
        } while ((vobj = dwg_next_object (vobj)) && vobj != vlast);
        result.set("data", vertices_obj);
      }
    }
    else { // <r13: iterate over vertices until seqend
      Dwg_Object *vobj = (Dwg_Object *)obj;
      i = 0;
      while ((vobj = dwg_next_object (vobj))
              && vobj->fixedtype != DWG_TYPE_SEQEND) {
        if (vobj->fixedtype == DWG_TYPE_VERTEX_2D
            && (vertex = dwg_object_to_VERTEX_2D (vobj))) {
          emscripten::val vertex_obj = vertex2d_to_js_object(vertex);
          vertices_obj.call<void>("push", vertex_obj);
          i++;
          if (i > num_points) {
            result.set("success", false);
            result.set("message", std::string("Index error! Exceed the number of vertices!"));
            break;
          }
        } else {
          result.set("success", false);
          result.set("message", std::string("Not all of vertices returned!"));
        }
      }
      result.set("data", vertices_obj);
    }
  } else {
    result.set("success", false);
    result.set("message", "Null pointer passed or object isn't a 2d poyline!");
  }
  return result;
}

emscripten::val dwg_entity_block_header_get_preview_wrapper(Dwg_Object_Object_Ptr obj_ptr) {
  Dwg_Object_BLOCK_HEADER* block_header = reinterpret_cast<Dwg_Object_BLOCK_HEADER*>(obj_ptr);
  emscripten::val result = emscripten::val::object();
  if (block_header) {
    result.set("data", dwg_ptr_to_unsigned_char_array(block_header->preview, block_header->preview_size));
  } else {
    result.set("success", false);
    result.set("message", "Null pointer passed!");
  }
  return result;
}

EMSCRIPTEN_BINDINGS(libredwg_dwg_object_ref) {
  DEFINE_FUNC(dwg_entity_polyline_2d_get_numpoints);
  DEFINE_FUNC(dwg_entity_polyline_2d_get_points);
  DEFINE_FUNC(dwg_entity_polyline_2d_get_vertices);
  DEFINE_FUNC(dwg_entity_block_header_get_preview);
}
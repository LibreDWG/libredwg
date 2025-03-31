#include <emscripten/bind.h>
#include <string>
#include <iostream>
#include <codecvt>
#include <locale>
#include <wchar.h>
#include "dwg.h"
#include "dwg_api.h"

using namespace emscripten;

#define GET_PROPERTY(methodName, className, propName, propType)           \
  propType get##methodName(const className& ref) {                        \
    return ref.propName;                                                  \
  }

#define SET_PROPERTY(methodName, className, propName, propType)           \
  void set##methodName(className& ref, propType value) {                  \
    ref.propName = value;                                                 \
  }

#define SET_CONST_PROPERTY(methodName, className, propName, propType)     \
  void set##methodName(className& ref, const propType value) {            \
    ref.propName = value;                                                 \
  }

#define GET_SET_PROPERTY(methodName, className, propName, propType)       \
  GET_PROPERTY(methodName, className, propName, propType)                 \
  SET_PROPERTY(methodName, className, propName, propType)

#define GET_UNSIGNED_SHORT_PROPERTY(methodName, className, propName)      \
  GET_PROPERTY(methodName, className, propName, unsigned short)

#define SET_UNSIGNED_SHORT_PROPERTY(methodName, className, propName)      \
  SET_PROPERTY(methodName, className, propName, unsigned short)

#define GET_SET_UNSIGNED_SHORT_PROPERTY(methodName, className, propName)  \
  GET_SET_PROPERTY(methodName, className, propName, unsigned short)

#define GET_CHAR_PROPERTY(methodName, className, propName)                \
  GET_PROPERTY(methodName, className, propName, char)

#define SET_CHAR_PROPERTY(methodName, className, propName)                \
  SET_PROPERTY(methodName, className, propName, char)

#define GET_SET_CHAR_PROPERTY(methodName, className, propName)            \
  GET_SET_PROPERTY(methodName, className, propName, char)

#define GET_SHORT_PROPERTY(methodName, className, propName)               \
  GET_PROPERTY(methodName, className, propName, short)

#define SET_SHORT_PROPERTY(methodName, className, propName)               \
  SET_PROPERTY(methodName, className, propName, short)

#define GET_SET_SHORT_PROPERTY(methodName, className, propName)           \
  GET_SET_PROPERTY(methodName, className, propName, short)

#define GET_INT_PROPERTY(methodName, className, propName)                 \
  GET_PROPERTY(methodName, className, propName, int)

#define SET_INT_PROPERTY(methodName, className, propName)                 \
  SET_PROPERTY(methodName, className, propName, int)

#define GET_SET_INT_PROPERTY(methodName, className, propName)             \
  GET_SET_PROPERTY(methodName, className, propName, int)

#define GET_UNSIGNED_INT64_PROPERTY(methodName, className, propName)      \
  GET_PROPERTY(methodName, className, propName, uint64_t)

#define SET_UNSIGNED_INT64_PROPERTY(methodName, className, propName)      \
  SET_PROPERTY(methodName, className, propName, uint64_t)

#define GET_SET_UNSIGNED_INT64_PROPERTY(methodName, className, propName)  \
  GET_SET_PROPERTY(methodName, className, propName, uint64_t)

#define GET_DOUBLE_PROPERTY(methodName, className, propName)              \
  GET_PROPERTY(methodName, className, propName, double)

#define SET_DOUBLE_PROPERTY(methodName, className, propName)              \
  SET_PROPERTY(methodName, className, propName, double)

#define GET_SET_DOUBLE_PROPERTY(methodName, className, propName)          \
  GET_SET_PROPERTY(methodName, className, propName, double)

#define GET_STRING_PROPERTY(methodName, className, propName)              \
  std::string get##methodName(const className& ref) {                     \
    return ref.propName ? std::string(ref.propName) : "";                 \
  }

#define SET_STRING_PROPERTY(methodName, className, propName)              \
  void set##methodName(className& ref, const std::string& value) {        \
    ref.propName = strdup(value.c_str());                                 \
  }

#define GET_SET_STRING_PROPERTY(methodName, className, propName)          \
  GET_STRING_PROPERTY(methodName, className, propName)                    \
  SET_STRING_PROPERTY(methodName, className, propName)

#define GET_ARRAY_PROPERTY(methodName, className, propName, arrayType, size)       \
  std::array<double, size> get##methodName(const className& ref) {                 \
    std::array<double, size> arr;                                                  \
    for (std::size_t i = 0; i < size; ++i) {                                       \
      arr[i] = ref.propName[i];                                                    \
    }                                                                              \
    return arr;                                                                    \
  }

#define SET_ARRAY_PROPERTY(methodName, className, propName, arrayType, size)       \
  void set##methodName(className& ref, const std::array<arrayType, size>& value) { \
    for (std::size_t i = 0; i < size; ++i) {                                       \
      ref.propName[i] = value[i];                                                  \
    }                                                                              \
  }

#define GET_SET_ARRAY_PROPERTY(methodName, className, propName, arrayType, size)   \
  GET_ARRAY_PROPERTY(methodName, className, propName, arrayType, size)             \
  SET_ARRAY_PROPERTY(methodName, className, propName, arrayType, size)

#define GET_BINARY_CHUNK_PROPERTY(methodName, className, propName)                 \
  _dwg_binary_chunk get##methodName(const className& ref) {                        \
    return ref.propName;                                                           \
  }

#define SET_BINARY_CHUNK_PROPERTY(methodName, className, propName)                 \
  void set##methodName(className& ref, const _dwg_binary_chunk& value) {           \
    ref.propName = value;                                                          \
  }

#define GET_SET_BINARY_CHUNK_PROPERTY(methodName, className, propName)             \
  GET_BINARY_CHUNK_PROPERTY(methodName, className, propName)                       \
  SET_BINARY_CHUNK_PROPERTY(methodName, className, propName)


GET_SET_STRING_PROPERTY(DwgVersionsType, Dwg_Versions, type)
GET_SET_STRING_PROPERTY(DwgVersionsHdr, Dwg_Versions, hdr)
GET_SET_STRING_PROPERTY(DwgVersionsDesc, Dwg_Versions, desc)


GET_SET_STRING_PROPERTY(DwgColorName, Dwg_Color, name)
GET_SET_STRING_PROPERTY(DwgColorBookName, Dwg_Color, book_name)


GET_SET_UNSIGNED_SHORT_PROPERTY(DwgBinaryChunkCodePage, _dwg_binary_chunk, codepage)
GET_SET_UNSIGNED_SHORT_PROPERTY(DwgBinaryChunkIsTu, _dwg_binary_chunk, is_tu)


// Helper function to convert wchar_t* to dwg_wchar_t*
dwg_wchar_t* convert_to_dwg_wchar(const std::wstring& wstr) {
  size_t length = wstr.size();
  dwg_wchar_t* dwg_wstr = new dwg_wchar_t[length + 1]; // Allocate memory for the new string
  for (size_t i = 0; i < length; ++i) {
    dwg_wstr[i] = static_cast<dwg_wchar_t>(wstr[i]); // Copy and cast each character
  }
  dwg_wstr[length] = 0; // Null-terminate the string
  return dwg_wstr;
}

// Getter and setter for the union members
std::string getDwgBinaryChunkData(const _dwg_binary_chunk& ref) {
  if (ref.is_tu) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return ref.u.wdata ? converter.to_bytes(std::wstring(reinterpret_cast<const wchar_t*>(ref.u.wdata))) : "";
  } else {
    return ref.u.data ? std::string(ref.u.data) : "";
  }
}

void setDwgBinaryChunkData(_dwg_binary_chunk& ref, const std::string& value) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  if (ref.is_tu) {
    std::wstring wstr = converter.from_bytes(value); // Convert to wchar_t string
    ref.u.wdata = convert_to_dwg_wchar(wstr); // Convert to dwg_wchar_t and assign
  } else {
    ref.u.data = strdup(value.c_str()); // Use strdup for regular char strings
  }
}


GET_SET_ARRAY_PROPERTY(DwgResbufValuePt, Dwg_Resbuf, value.pt, double, 3)
GET_SET_CHAR_PROPERTY(DwgResbufValueI8, Dwg_Resbuf, value.i8)
GET_SET_SHORT_PROPERTY(DwgResbufValueI16, Dwg_Resbuf, value.i16)
GET_SET_INT_PROPERTY(DwgResbufValueI32, Dwg_Resbuf, value.i32)
GET_SET_UNSIGNED_INT64_PROPERTY(DwgResbufValueI64, Dwg_Resbuf, value.i64)
GET_SET_UNSIGNED_INT64_PROPERTY(DwgResbufValueAbsRef, Dwg_Resbuf, value.absref)
GET_SET_DOUBLE_PROPERTY(DwgResbufValueDouble, Dwg_Resbuf, value.dbl)
GET_SET_BINARY_CHUNK_PROPERTY(DwgResbufValueStr, Dwg_Resbuf, value.str)
GET_SET_PROPERTY(DwgResbufValueNextRb, Dwg_Resbuf, nextrb, Dwg_Resbuf*)


GET_SET_STRING_PROPERTY(DwgHeaderVariablesDwgCodePage, Dwg_Header_Variables, DWGCODEPAGE)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesUnit1Name, Dwg_Header_Variables, unit1_name)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesUnit2Name, Dwg_Header_Variables, unit2_name)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesUnit3Name, Dwg_Header_Variables, unit3_name)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesUnit4Name, Dwg_Header_Variables, unit4_name)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesMenu, Dwg_Header_Variables, MENU)
GET_SET_ARRAY_PROPERTY(DwgHeaderVariablesMenuExt, Dwg_Header_Variables, MENUEXT, unsigned char, 46)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimPost, Dwg_Header_Variables, DIMPOST)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimaPost, Dwg_Header_Variables, DIMAPOST)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimPostT, Dwg_Header_Variables, DIMPOST_T)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimaPostT, Dwg_Header_Variables, DIMAPOST_T)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimBlkT, Dwg_Header_Variables, DIMBLK_T)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimBlk1T, Dwg_Header_Variables, DIMBLK1_T)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimBlk2T, Dwg_Header_Variables, DIMBLK2_T)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesUnknownString, Dwg_Header_Variables, unknown_string)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimalTMZS, Dwg_Header_Variables, DIMALTMZS)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesDimMZS, Dwg_Header_Variables, DIMMZS)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesHyperLinkBase, Dwg_Header_Variables, HYPERLINKBASE)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesStyleSheet, Dwg_Header_Variables, STYLESHEET)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesFingerPrintGuid, Dwg_Header_Variables, FINGERPRINTGUID)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesVisionGuid, Dwg_Header_Variables, VERSIONGUID)
GET_SET_STRING_PROPERTY(DwgHeaderVariablesProjectName, Dwg_Header_Variables, PROJECTNAME)
GET_SET_ARRAY_PROPERTY(DwgHeaderVariablesLayerColors, Dwg_Header_Variables, layer_colors, uint16_t, 128)


GET_SET_STRING_PROPERTY(DwgEntityTexTTextValue, Dwg_Entity_TEXT, text_value)
GET_SET_STRING_PROPERTY(DwgEntityAttribTextValue, Dwg_Entity_ATTRIB, text_value)
GET_SET_STRING_PROPERTY(DwgEntityAttribTag, Dwg_Entity_ATTRIB, tag)
GET_SET_STRING_PROPERTY(DwgEntityAttDefDefaultValue, Dwg_Entity_ATTDEF, default_value)
GET_SET_STRING_PROPERTY(DwgEntityAttDefTag, Dwg_Entity_ATTDEF, tag)
GET_SET_STRING_PROPERTY(DwgEntityAttDefPrompt, Dwg_Entity_ATTDEF, prompt)
GET_SET_STRING_PROPERTY(DwgEntityBlockName, Dwg_Entity_BLOCK, name)
GET_SET_STRING_PROPERTY(DwgEntityBlockXRefName, Dwg_Entity_BLOCK, xref_pname)
GET_SET_STRING_PROPERTY(DwgEntityInsertBlockName, Dwg_Entity_INSERT, block_name)
GET_SET_ARRAY_PROPERTY(DwgEntityVertexPfaceFaceVertInd, Dwg_Entity_VERTEX_PFACE_FACE, vertind, BITCODE_BS, 4)
GET_SET_STRING_PROPERTY(DwgEntityPolyline2dExtraR11Text, Dwg_Entity_POLYLINE_2D, extra_r11_text)
GET_SET_STRING_PROPERTY(DwgDimensionCommonUserText, Dwg_DIMENSION_common, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionOrdinateUserText, Dwg_Entity_DIMENSION_ORDINATE, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionLinearUserText, Dwg_Entity_DIMENSION_LINEAR, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionAlignedUserText, Dwg_Entity_DIMENSION_ALIGNED, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionAng3PtUserText, Dwg_Entity_DIMENSION_ANG3PT, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionAng2LnUserText, Dwg_Entity_DIMENSION_ANG2LN, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionRadiusUserText, Dwg_Entity_DIMENSION_RADIUS, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionDiameterUserText, Dwg_Entity_DIMENSION_DIAMETER, user_text)
GET_SET_STRING_PROPERTY(DwgEntityDimensionArcUserText, Dwg_Entity_ARC_DIMENSION, user_text)
GET_SET_STRING_PROPERTY(DwgEntityLargeRadialDimensionUserText, Dwg_Entity_LARGE_RADIAL_DIMENSION, user_text)
GET_SET_STRING_PROPERTY(DwgEntityViewportStyleSheet, Dwg_Entity_VIEWPORT, style_sheet)


#define DEFINE_CLASS_WITH_PARENT(jsClassName, cppClassName)                              \
  class_<jsClassName>(#cppClassName)                                                     \
    .constructor<>()                                                                     \
    /*.property("parent", &cppClassName::parent) */


#define DEFINE_COMMON_ENTITY_POLYLINE(className)                                         \
  .property("hasVertex", &className::has_vertex)                                         \
  .property("numOwned", &className::num_owned)                                           \
  .property("firstVertex", &className::first_vertex, return_value_policy::reference())   \
  .property("lastVertex", &className::last_vertex, return_value_policy::reference())     \
  /* TODO: Double check whether this binding is correct */                               \
  // .property("vertex", &className::vertex, return_value_policy::reference())           \
  .property("seqend", &className::seqend, return_value_policy::reference())

#define DEFINE_DIMENSION_COMMON(className)                                               \
  .property("classVersion", &className::class_version)                                   \
  .property("extrusion", &className::extrusion)                                          \
  .property("defPoint", &className::def_pt, return_value_policy::reference())            \
  .property("textMidPoint", &className::text_midpt, return_value_policy::reference())    \
  .property("elevation", &className::elevation)                                          \
  .property("flag", &className::flag)                                                    \
  .property("flag1", &className::flag1)                                                  \
  .property("textRotation", &className::text_rotation)                                   \
  .property("horizDir", &className::horiz_dir)                                           \
  .property("insScale", &className::ins_scale, return_value_policy::reference())         \
  .property("insRotation", &className::ins_rotation)                                     \
  .property("attachment", &className::attachment)                                        \
  .property("lspaceStyle", &className::lspace_style)                                     \
  .property("lspaceFactor", &className::lspace_factor)                                   \
  .property("actMeasurement", &className::act_measurement)                               \
  .property("unknown", &className::unknown)                                              \
  .property("flipArrow1", &className::flip_arrow1)                                       \
  .property("flipArrow2", &className::flip_arrow2)                                       \
  .property("cloneInsPoint", &className::clone_ins_pt, return_value_policy::reference()) \
  .property("dimStyle", &className::dimstyle, return_value_policy::reference())          \
  .property("block", &className::block, return_value_policy::reference())


GET_SET_PROPERTY(DwgObjectEntityTioUnused, Dwg_Object_Entity, tio.UNUSED, Dwg_Entity_UNUSED*)
GET_SET_PROPERTY(DwgObjectEntityTioDimensionCommon, Dwg_Object_Entity, tio.DIMENSION_common, Dwg_DIMENSION_common*)


EMSCRIPTEN_BINDINGS(libredwg_data) {
  class_<Dwg_Bitcode_TimeBLL>("Dwg_Bitcode_TimeBLL")
    .property("days", &Dwg_Bitcode_TimeBLL::days)
    .property("ms", &Dwg_Bitcode_TimeBLL::ms)
    .property("value", &Dwg_Bitcode_TimeBLL::value);

  class_<Dwg_Bitcode_2RD>("Dwg_Bitcode_2RD")
    .property("x", &Dwg_Bitcode_2RD::x)
    .property("y", &Dwg_Bitcode_2RD::y);

  class_<Dwg_Bitcode_2BD>("Dwg_Bitcode_2BD")
    .property("x", &Dwg_Bitcode_2BD::x)
    .property("y", &Dwg_Bitcode_2BD::y);

  class_<Dwg_Bitcode_3RD>("Dwg_Bitcode_3RD")
    .property("x", &Dwg_Bitcode_3RD::x)
    .property("y", &Dwg_Bitcode_3RD::y)
    .property("z", &Dwg_Bitcode_3RD::z);

    class_<Dwg_Bitcode_3BD>("Dwg_Bitcode_3BD")
    .property("x", &Dwg_Bitcode_3BD::x)
    .property("y", &Dwg_Bitcode_3BD::y)
    .property("z", &Dwg_Bitcode_3BD::z);

  enum_<Dwg_Version_Type>("Dwg_Version_Type")
    .value("R_INVALID", R_INVALID)
    .value("R_1_1", R_1_1)
    .value("R_1_2", R_1_2)
    .value("R_1_3", R_1_3)
    .value("R_1_4", R_1_4)
    .value("R_2_0b", R_2_0b)
    .value("R_2_0", R_2_0)
    .value("R_2_10", R_2_10)
    .value("R_2_21", R_2_21)
    .value("R_2_22", R_2_22)
    .value("R_2_4", R_2_4)
    .value("R_2_5", R_2_5)
    .value("R_2_6", R_2_6)
    .value("R_9", R_9)
    .value("R_9c1", R_9c1)
    .value("R_10", R_10)
    .value("R_11b1", R_11b1)
    .value("R_11b2", R_11b2)
    .value("R_11", R_11)
    .value("R_12", R_12)
    .value("R_13b1", R_13b1)
    .value("R_13b2", R_13b2)
    .value("R_13", R_13)
    .value("R_13c3", R_13c3)
    .value("R_14", R_14)
    .value("R_2000b", R_2000b)
    .value("R_2000", R_2000)
    .value("R_2000i", R_2000i)
    .value("R_2002", R_2002)
    .value("R_2004a", R_2004a)
    .value("R_2004b", R_2004b)
    .value("R_2004c", R_2004c)
    .value("R_2004", R_2004)
    .value("R_2007a", R_2007a)
    .value("R_2007b", R_2007b)
    .value("R_2007", R_2007)
    .value("R_2010b", R_2010b)
    .value("R_2010", R_2010)
    .value("R_2013b", R_2013b)
    .value("R_2013", R_2013)
    .value("R_2018b", R_2018b)
    .value("R_2018", R_2018)
    .value("R_2022b", R_2022b)
    .value("R_AFTER", R_AFTER);

  class_<Dwg_Versions>("Dwg_Versions")
    .constructor<>()
    .property("r", &Dwg_Versions::r)
    .property("type", &getDwgVersionsType, &setDwgVersionsType)
    .property("hdr", &getDwgVersionsHdr, &setDwgVersionsHdr)
    .property("desc", &getDwgVersionsDesc, &setDwgVersionsDesc)
    .property("dwg_version", &Dwg_Versions::dwg_version);

  enum_<Dwg_Class_Stability>("Dwg_Class_Stability")
    .value("STABLE", DWG_CLASS_STABLE)
    .value("UNSTABLE", DWG_CLASS_UNSTABLE)
    .value("DEBUGGING", DWG_CLASS_DEBUGGING)
    .value("UNHANDLED", DWG_CLASS_UNHANDLED);

  enum_<Dwg_Entity_Sections>("Dwg_Entity_Sections")
    .value("ENTITY_SECTION", DWG_ENTITY_SECTION)
    .value("BLOCKS_SECTION", DWG_BLOCKS_SECTION)
    .value("EXTRA_SECTION", DWG_EXTRA_SECTION);

  enum_<Dwg_Object_Supertype>("Dwg_Object_Supertype")
    .value("SUPERTYPE_ENTITY", DWG_SUPERTYPE_ENTITY)
    .value("SUPERTYPE_OBJECT", DWG_SUPERTYPE_OBJECT);

  enum_<Dwg_Object_Type>("Dwg_Object_Type")
    .value("TYPE_UNUSED", DWG_TYPE_UNUSED)
    .value("TYPE_TEXT", DWG_TYPE_TEXT)
    .value("TYPE_ATTRIB", DWG_TYPE_ATTRIB)
    .value("TYPE_ATTDEF", DWG_TYPE_ATTDEF)
    .value("TYPE_BLOCK", DWG_TYPE_BLOCK)
    .value("TYPE_ENDBLK", DWG_TYPE_ENDBLK)
    .value("TYPE_SEQEND", DWG_TYPE_SEQEND)
    .value("TYPE_INSERT", DWG_TYPE_INSERT)
    .value("TYPE_MINSERT", DWG_TYPE_MINSERT)
    .value("TYPE_VERTEX_2D", DWG_TYPE_VERTEX_2D)
    .value("TYPE_VERTEX_3D", DWG_TYPE_VERTEX_3D)
    .value("TYPE_VERTEX_MESH", DWG_TYPE_VERTEX_MESH)
    .value("TYPE_VERTEX_PFACE", DWG_TYPE_VERTEX_PFACE)
    .value("TYPE_VERTEX_PFACE_FACE", DWG_TYPE_VERTEX_PFACE_FACE)
    .value("TYPE_POLYLINE_2D", DWG_TYPE_POLYLINE_2D)
    .value("TYPE_POLYLINE_3D", DWG_TYPE_POLYLINE_3D)
    .value("TYPE_ARC", DWG_TYPE_ARC)
    .value("TYPE_CIRCLE", DWG_TYPE_CIRCLE)
    .value("TYPE_LINE", DWG_TYPE_LINE)
    .value("TYPE_DIMENSION_ORDINATE", DWG_TYPE_DIMENSION_ORDINATE)
    .value("TYPE_DIMENSION_LINEAR", DWG_TYPE_DIMENSION_LINEAR)
    .value("TYPE_DIMENSION_ALIGNED", DWG_TYPE_DIMENSION_ALIGNED)
    .value("TYPE_DIMENSION_ANG3PT", DWG_TYPE_DIMENSION_ANG3PT)
    .value("TYPE_DIMENSION_ANG2LN", DWG_TYPE_DIMENSION_ANG2LN)
    .value("TYPE_DIMENSION_RADIUS", DWG_TYPE_DIMENSION_RADIUS)
    .value("TYPE_DIMENSION_DIAMETER", DWG_TYPE_DIMENSION_DIAMETER)
    .value("TYPE_POINT", DWG_TYPE_POINT)
    .value("TYPE_3DFACE", DWG_TYPE__3DFACE)
    .value("TYPE_POLYLINE_PFACE", DWG_TYPE_POLYLINE_PFACE)
    .value("TYPE_POLYLINE_MESH", DWG_TYPE_POLYLINE_MESH)
    .value("TYPE_SOLID", DWG_TYPE_SOLID)
    .value("TYPE_TRACE", DWG_TYPE_TRACE)
    .value("TYPE_SHAPE", DWG_TYPE_SHAPE)
    .value("TYPE_VIEWPORT", DWG_TYPE_VIEWPORT)
    .value("TYPE_ELLIPSE", DWG_TYPE_ELLIPSE)
    .value("TYPE_SPLINE", DWG_TYPE_SPLINE)
    .value("TYPE_REGION", DWG_TYPE_REGION)
    .value("TYPE_3DSOLID", DWG_TYPE__3DSOLID)
    .value("TYPE_BODY", DWG_TYPE_BODY)
    .value("TYPE_RAY", DWG_TYPE_RAY)
    .value("TYPE_XLINE", DWG_TYPE_XLINE)
    .value("TYPE_DICTIONARY", DWG_TYPE_DICTIONARY)
    .value("TYPE_OLEFRAME", DWG_TYPE_OLEFRAME)
    .value("TYPE_MTEXT", DWG_TYPE_MTEXT)
    .value("TYPE_LEADER", DWG_TYPE_LEADER)
    .value("TYPE_TOLERANCE", DWG_TYPE_TOLERANCE)
    .value("TYPE_MLINE", DWG_TYPE_MLINE)
    .value("TYPE_BLOCK_CONTROL", DWG_TYPE_BLOCK_CONTROL)
    .value("TYPE_BLOCK_HEADER", DWG_TYPE_BLOCK_HEADER)
    .value("TYPE_LAYER_CONTROL", DWG_TYPE_LAYER_CONTROL)
    .value("TYPE_LAYER", DWG_TYPE_LAYER)
    .value("TYPE_STYLE_CONTROL", DWG_TYPE_STYLE_CONTROL)
    .value("TYPE_STYLE", DWG_TYPE_STYLE)
    .value("TYPE_LTYPE_CONTROL", DWG_TYPE_LTYPE_CONTROL)
    .value("TYPE_LTYPE", DWG_TYPE_LTYPE)
    .value("TYPE_VIEW_CONTROL", DWG_TYPE_VIEW_CONTROL)
    .value("TYPE_VIEW", DWG_TYPE_VIEW)
    .value("TYPE_UCS_CONTROL", DWG_TYPE_UCS_CONTROL)
    .value("TYPE_UCS", DWG_TYPE_UCS)
    .value("TYPE_VPORT_CONTROL", DWG_TYPE_VPORT_CONTROL)
    .value("TYPE_VPORT", DWG_TYPE_VPORT)
    .value("TYPE_APPID_CONTROL", DWG_TYPE_APPID_CONTROL)
    .value("TYPE_APPID", DWG_TYPE_APPID)
    .value("TYPE_DIMSTYLE_CONTROL", DWG_TYPE_DIMSTYLE_CONTROL)
    .value("TYPE_DIMSTYLE", DWG_TYPE_DIMSTYLE)
    .value("TYPE_GROUP", DWG_TYPE_GROUP)
    .value("TYPE_MLINESTYLE", DWG_TYPE_MLINESTYLE)
    .value("TYPE_OLE2FRAME", DWG_TYPE_OLE2FRAME)
    .value("TYPE_DUMMY", DWG_TYPE_DUMMY)
    .value("TYPE_LONG_TRANSACTION", DWG_TYPE_LONG_TRANSACTION)
    .value("TYPE_LWPOLYLINE", DWG_TYPE_LWPOLYLINE)
    .value("TYPE_HATCH", DWG_TYPE_HATCH)
    .value("TYPE_XRECORD", DWG_TYPE_XRECORD)
    .value("TYPE_PLACEHOLDER", DWG_TYPE_PLACEHOLDER)
    .value("TYPE_VBA_PROJECT", DWG_TYPE_VBA_PROJECT)
    .value("TYPE_LAYOUT", DWG_TYPE_LAYOUT)
    .value("TYPE_PROXY_ENTITY", DWG_TYPE_PROXY_ENTITY)
    .value("TYPE_PROXY_OBJECT", DWG_TYPE_PROXY_OBJECT);

  enum_<Dwg_Object_Type_r11>("Dwg_Object_Type_r11")
    .value("DWG_TYPE_UNUSED_r11", DWG_TYPE_UNUSED_r11)
    .value("DWG_TYPE_LINE_r11", DWG_TYPE_LINE_r11)
    .value("DWG_TYPE_POINT_r11", DWG_TYPE_POINT_r11)
    .value("DWG_TYPE_CIRCLE_r11", DWG_TYPE_CIRCLE_r11)
    .value("DWG_TYPE_SHAPE_r11", DWG_TYPE_SHAPE_r11)
    .value("DWG_TYPE_REPEAT_r11", DWG_TYPE_REPEAT_r11)
    .value("DWG_TYPE_ENDREP_r11", DWG_TYPE_ENDREP_r11)
    .value("DWG_TYPE_TEXT_r11", DWG_TYPE_TEXT_r11)
    .value("DWG_TYPE_ARC_r11", DWG_TYPE_ARC_r11)
    .value("DWG_TYPE_TRACE_r11", DWG_TYPE_TRACE_r11)
    .value("DWG_TYPE_LOAD_r11", DWG_TYPE_LOAD_r11)
    .value("DWG_TYPE_SOLID_r11", DWG_TYPE_SOLID_r11)
    .value("DWG_TYPE_BLOCK_r11", DWG_TYPE_BLOCK_r11)
    .value("DWG_TYPE_ENDBLK_r11", DWG_TYPE_ENDBLK_r11)
    .value("DWG_TYPE_INSERT_r11", DWG_TYPE_INSERT_r11)
    .value("DWG_TYPE_ATTDEF_r11", DWG_TYPE_ATTDEF_r11)
    .value("DWG_TYPE_ATTRIB_r11", DWG_TYPE_ATTRIB_r11)
    .value("DWG_TYPE_SEQEND_r11", DWG_TYPE_SEQEND_r11)
    .value("DWG_TYPE_JUMP_r11", DWG_TYPE_JUMP_r11)
    .value("DWG_TYPE_POLYLINE_r11", DWG_TYPE_POLYLINE_r11)
    .value("DWG_TYPE_VERTEX_r11", DWG_TYPE_VERTEX_r11)
    .value("DWG_TYPE_3DLINE_r11", DWG_TYPE_3DLINE_r11)
    .value("DWG_TYPE_3DFACE_r11", DWG_TYPE_3DFACE_r11)
    .value("DWG_TYPE_DIMENSION_r11", DWG_TYPE_DIMENSION_r11)
    .value("DWG_TYPE_VIEWPORT_r11", DWG_TYPE_VIEWPORT_r11)
    .value("DWG_TYPE_UNKNOWN_r11", DWG_TYPE_UNKNOWN_r11);

  enum_<Dwg_Error>("Dwg_Error")
    .value("DWG_NOERR", DWG_NOERR)
    .value("DWG_ERR_WRONGCRC", DWG_ERR_WRONGCRC)
    .value("DWG_ERR_NOTYETSUPPORTED", DWG_ERR_NOTYETSUPPORTED)
    .value("DWG_ERR_UNHANDLEDCLASS", DWG_ERR_UNHANDLEDCLASS)
    .value("DWG_ERR_INVALIDTYPE", DWG_ERR_INVALIDTYPE)
    .value("DWG_ERR_INVALIDHANDLE", DWG_ERR_INVALIDHANDLE)
    .value("DWG_ERR_INVALIDEED", DWG_ERR_INVALIDEED)
    .value("DWG_ERR_VALUEOUTOFBOUNDS", DWG_ERR_VALUEOUTOFBOUNDS)
    .value("DWG_ERR_CLASSESNOTFOUND", DWG_ERR_CLASSESNOTFOUND)
    .value("DWG_ERR_SECTIONNOTFOUND", DWG_ERR_SECTIONNOTFOUND)
    .value("DWG_ERR_PAGENOTFOUND", DWG_ERR_PAGENOTFOUND)
    .value("DWG_ERR_INTERNALERROR", DWG_ERR_INTERNALERROR)
    .value("DWG_ERR_INVALIDDWG", DWG_ERR_INVALIDDWG)
    .value("DWG_ERR_IOERROR", DWG_ERR_IOERROR)
    .value("DWG_ERR_OUTOFMEM", DWG_ERR_OUTOFMEM);

  class_<Dwg_Handle>("Dwg_Handle")
    .property("code", &Dwg_Handle::code)
    .property("size", &Dwg_Handle::size)
    .property("value", &Dwg_Handle::value)
    .property("is_global", &Dwg_Handle::is_global);

  class_<Dwg_Object_Ref>("Dwg_Object_Ref")
    .property("absolute_ref", &Dwg_Object_Ref::absolute_ref)
    .property("r11_idx", &Dwg_Object_Ref::r11_idx)
    .property("handleref", &Dwg_Object_Ref::handleref);

  enum_<Dwg_Hdl_Code>("Dwg_Hdl_Code")
    .value("DWG_HDL_OWNER", DWG_HDL_OWNER)
    .value("DWG_HDL_SOFTOWN", DWG_HDL_SOFTOWN)
    .value("DWG_HDL_HARDOWN", DWG_HDL_HARDOWN)
    .value("DWG_HDL_SOFTPTR", DWG_HDL_SOFTPTR)
    .value("DWG_HDL_HARDPTR", DWG_HDL_HARDPTR);

  class_<Dwg_Color>("Dwg_Color")
    .constructor<>()
    .property("index", &Dwg_Color::index)
    .property("flag", &Dwg_Color::flag)
    .property("raw", &Dwg_Color::raw)
    .property("rgb", &Dwg_Color::rgb)
    .property("method", &Dwg_Color::method)
    .property("name", &getDwgColorName, &setDwgColorName)
    .property("book_name", &getDwgColorBookName, &setDwgColorBookName)
    .property("alpha_raw", &Dwg_Color::alpha_raw)
    .property("alpha_type", &Dwg_Color::alpha_type)
    .property("alpha", &Dwg_Color::alpha);

  class_<_dwg_binary_chunk>("Dwg_Binary_Chunk")
    .constructor<>()
    .property("size", &_dwg_binary_chunk::size)
    .property("codepage", &getDwgBinaryChunkCodePage, &setDwgBinaryChunkCodePage)
    .property("is_tu", &getDwgBinaryChunkIsTu, &setDwgBinaryChunkIsTu)
    .property("data", &getDwgBinaryChunkData, &setDwgBinaryChunkData);

  // Register 'std::array<double, 3>' because property 'pt' in 'Dwg_Resbuf' is interpreted as such
  value_array<std::array<double, 3>>("Dwg_Array_Double3")
    .element(emscripten::index<0>())
    .element(emscripten::index<1>())
    .element(emscripten::index<2>());

  class_<Dwg_Resbuf>("Dwg_Resbuf")
    .constructor<>()
    .property("type", &Dwg_Resbuf::type)
    .property("pt", &getDwgResbufValuePt, &setDwgResbufValuePt)
    .property("i8", &getDwgResbufValueI8, &setDwgResbufValueI8)
    .property("i16", &getDwgResbufValueI16, &setDwgResbufValueI16)
    .property("i32", &getDwgResbufValueI32, &setDwgResbufValueI32)
    .property("i64", &getDwgResbufValueI64, &setDwgResbufValueI64)
    .property("absRef", &getDwgResbufValueAbsRef, &setDwgResbufValueAbsRef)
    .property("dbl", &getDwgResbufValueDouble, &setDwgResbufValueDouble)
    .property("str", &getDwgResbufValueStr, &setDwgResbufValueStr)
    .property("nextRb", &Dwg_Resbuf::nextrb, return_value_policy::reference());

  // Register 'std::array<unsigned char, 46>' because property 'MENUEXT' in 'Dwg_Header_Variables' is interpreted as such
  value_array<std::array<unsigned char, 46>>("Dwg_Array_UnsignedChar46")
    .element(emscripten::index<0>())
    .element(emscripten::index<1>())
    .element(emscripten::index<2>())
    .element(emscripten::index<3>())
    .element(emscripten::index<4>())
    .element(emscripten::index<5>())
    .element(emscripten::index<6>())
    .element(emscripten::index<7>())
    .element(emscripten::index<8>())
    .element(emscripten::index<9>())
    .element(emscripten::index<10>())
    .element(emscripten::index<11>())
    .element(emscripten::index<12>())
    .element(emscripten::index<13>())
    .element(emscripten::index<14>())
    .element(emscripten::index<15>())
    .element(emscripten::index<16>())
    .element(emscripten::index<17>())
    .element(emscripten::index<18>())
    .element(emscripten::index<19>())
    .element(emscripten::index<20>())
    .element(emscripten::index<21>())
    .element(emscripten::index<22>())
    .element(emscripten::index<23>())
    .element(emscripten::index<24>())
    .element(emscripten::index<25>())
    .element(emscripten::index<26>())
    .element(emscripten::index<27>())
    .element(emscripten::index<28>())
    .element(emscripten::index<29>())
    .element(emscripten::index<30>())
    .element(emscripten::index<31>())
    .element(emscripten::index<32>())
    .element(emscripten::index<33>())
    .element(emscripten::index<34>())
    .element(emscripten::index<35>())
    .element(emscripten::index<36>())
    .element(emscripten::index<37>())
    .element(emscripten::index<38>())
    .element(emscripten::index<39>())
    .element(emscripten::index<40>())
    .element(emscripten::index<41>())
    .element(emscripten::index<42>())
    .element(emscripten::index<43>())
    .element(emscripten::index<44>())
    .element(emscripten::index<45>());

  // Register 'std::array<uint16_t, 128>' because property 'layer_colors' in 'Dwg_Header_Variables' is interpreted as such
  value_array<std::array<uint16_t, 128>>("Dwg_Array_UnsignedInt16")
    .element(emscripten::index<0>())
    .element(emscripten::index<1>())
    .element(emscripten::index<2>())
    .element(emscripten::index<3>())
    .element(emscripten::index<4>())
    .element(emscripten::index<5>())
    .element(emscripten::index<6>())
    .element(emscripten::index<7>())
    .element(emscripten::index<8>())
    .element(emscripten::index<9>())
    .element(emscripten::index<10>())
    .element(emscripten::index<11>())
    .element(emscripten::index<12>())
    .element(emscripten::index<13>())
    .element(emscripten::index<14>())
    .element(emscripten::index<15>())
    .element(emscripten::index<16>())
    .element(emscripten::index<17>())
    .element(emscripten::index<18>())
    .element(emscripten::index<19>())
    .element(emscripten::index<20>())
    .element(emscripten::index<21>())
    .element(emscripten::index<22>())
    .element(emscripten::index<23>())
    .element(emscripten::index<24>())
    .element(emscripten::index<25>())
    .element(emscripten::index<26>())
    .element(emscripten::index<27>())
    .element(emscripten::index<28>())
    .element(emscripten::index<29>())
    .element(emscripten::index<30>())
    .element(emscripten::index<31>())
    .element(emscripten::index<32>())
    .element(emscripten::index<33>())
    .element(emscripten::index<34>())
    .element(emscripten::index<35>())
    .element(emscripten::index<36>())
    .element(emscripten::index<37>())
    .element(emscripten::index<38>())
    .element(emscripten::index<39>())
    .element(emscripten::index<40>())
    .element(emscripten::index<41>())
    .element(emscripten::index<42>())
    .element(emscripten::index<43>())
    .element(emscripten::index<44>())
    .element(emscripten::index<45>())
    .element(emscripten::index<46>())
    .element(emscripten::index<47>())
    .element(emscripten::index<48>())
    .element(emscripten::index<49>())
    .element(emscripten::index<50>())
    .element(emscripten::index<51>())
    .element(emscripten::index<52>())
    .element(emscripten::index<53>())
    .element(emscripten::index<54>())
    .element(emscripten::index<55>())
    .element(emscripten::index<56>())
    .element(emscripten::index<57>())
    .element(emscripten::index<58>())
    .element(emscripten::index<59>())
    .element(emscripten::index<60>())
    .element(emscripten::index<61>())
    .element(emscripten::index<62>())
    .element(emscripten::index<63>())
    .element(emscripten::index<64>())
    .element(emscripten::index<65>())
    .element(emscripten::index<66>())
    .element(emscripten::index<67>())
    .element(emscripten::index<68>())
    .element(emscripten::index<69>())
    .element(emscripten::index<70>())
    .element(emscripten::index<71>())
    .element(emscripten::index<72>())
    .element(emscripten::index<73>())
    .element(emscripten::index<74>())
    .element(emscripten::index<75>())
    .element(emscripten::index<76>())
    .element(emscripten::index<77>())
    .element(emscripten::index<78>())
    .element(emscripten::index<79>())
    .element(emscripten::index<80>())
    .element(emscripten::index<81>())
    .element(emscripten::index<82>())
    .element(emscripten::index<83>())
    .element(emscripten::index<84>())
    .element(emscripten::index<85>())
    .element(emscripten::index<86>())
    .element(emscripten::index<87>())
    .element(emscripten::index<88>())
    .element(emscripten::index<89>())
    .element(emscripten::index<90>())
    .element(emscripten::index<91>())
    .element(emscripten::index<92>())
    .element(emscripten::index<93>())
    .element(emscripten::index<94>())
    .element(emscripten::index<95>())
    .element(emscripten::index<96>())
    .element(emscripten::index<97>())
    .element(emscripten::index<98>())
    .element(emscripten::index<99>())
    .element(emscripten::index<100>())
    .element(emscripten::index<101>())
    .element(emscripten::index<102>())
    .element(emscripten::index<103>())
    .element(emscripten::index<104>())
    .element(emscripten::index<105>())
    .element(emscripten::index<106>())
    .element(emscripten::index<107>())
    .element(emscripten::index<108>())
    .element(emscripten::index<109>())
    .element(emscripten::index<110>())
    .element(emscripten::index<111>())
    .element(emscripten::index<112>())
    .element(emscripten::index<113>())
    .element(emscripten::index<114>())
    .element(emscripten::index<115>())
    .element(emscripten::index<116>())
    .element(emscripten::index<117>())
    .element(emscripten::index<118>())
    .element(emscripten::index<119>())
    .element(emscripten::index<120>())
    .element(emscripten::index<121>())
    .element(emscripten::index<122>())
    .element(emscripten::index<123>())
    .element(emscripten::index<124>())
    .element(emscripten::index<125>())
    .element(emscripten::index<126>())
    .element(emscripten::index<127>());

  // Define the Dwg_Header_Variables struct
  class_<Dwg_Header_Variables>("Dwg_Header_Variables")
    .constructor<>()
    .property("size", &Dwg_Header_Variables::size)
    .property("bitSizeHi", &Dwg_Header_Variables::bitsize_hi)
    .property("bitSize", &Dwg_Header_Variables::bitsize)
    .property("ACADMAINTVER", &Dwg_Header_Variables::ACADMAINTVER)
    .property("REQUIREDVERSIONS", &Dwg_Header_Variables::REQUIREDVERSIONS)
    .property("DWGCODEPAGE", &getDwgHeaderVariablesDwgCodePage, &setDwgHeaderVariablesDwgCodePage)
    .property("codepage", &Dwg_Header_Variables::codepage)
    .property("unit1Ratio", &Dwg_Header_Variables::unit1_ratio)
    .property("unit2Ratio", &Dwg_Header_Variables::unit2_ratio)
    .property("unit3Ratio", &Dwg_Header_Variables::unit3_ratio)
    .property("unit4Ratio", &Dwg_Header_Variables::unit4_ratio)
    .property("unit1Name", &getDwgHeaderVariablesUnit1Name, &setDwgHeaderVariablesUnit1Name)
    .property("unit2Name", &getDwgHeaderVariablesUnit2Name, &setDwgHeaderVariablesUnit2Name)
    .property("unit3Name", &getDwgHeaderVariablesUnit3Name, &setDwgHeaderVariablesUnit3Name)
    .property("unit4Name", &getDwgHeaderVariablesUnit4Name, &setDwgHeaderVariablesUnit4Name)
    .property("unknown8", &Dwg_Header_Variables::unknown_8)
    .property("unknown9", &Dwg_Header_Variables::unknown_9)
    .property("unknown10", &Dwg_Header_Variables::unknown_10)
    .property("VX_TABLE_RECORD", &Dwg_Header_Variables::VX_TABLE_RECORD, return_value_policy::reference())
    .property("DIMASO", &Dwg_Header_Variables::DIMASO)
    .property("DIMSHO", &Dwg_Header_Variables::DIMSHO)
    .property("DIMSAV", &Dwg_Header_Variables::DIMSAV)
    .property("PLINEGEN", &Dwg_Header_Variables::PLINEGEN)
    .property("ORTHOMODE", &Dwg_Header_Variables::ORTHOMODE)
    .property("REGENMODE", &Dwg_Header_Variables::REGENMODE)
    .property("FILLMODE", &Dwg_Header_Variables::FILLMODE)
    .property("QTEXTMODE", &Dwg_Header_Variables::QTEXTMODE)
    .property("PSLTSCALE", &Dwg_Header_Variables::PSLTSCALE)
    .property("LIMCHECK", &Dwg_Header_Variables::LIMCHECK)
    // .property("MENUEXT", &getDwgHeaderVariablesMenuText, &setDwgHeaderVariablesMenuText)
    .property("BLIPMODE", &Dwg_Header_Variables::BLIPMODE)
    .property("unknown_11", &Dwg_Header_Variables::unknown_11)
    .property("USRTIMER", &Dwg_Header_Variables::USRTIMER)
    .property("FASTZOOM", &Dwg_Header_Variables::FASTZOOM)
    .property("FLATLAND", &Dwg_Header_Variables::FLATLAND)
    .property("VIEWMODE", &Dwg_Header_Variables::VIEWMODE)
    .property("SKPOLY", &Dwg_Header_Variables::SKPOLY)
    .property("unknownMonth", &Dwg_Header_Variables::unknown_mon)
    .property("unknownDay", &Dwg_Header_Variables::unknown_day)
    .property("unknownYear", &Dwg_Header_Variables::unknown_year)
    .property("unknownHour", &Dwg_Header_Variables::unknown_hour)
    .property("unknownMinute", &Dwg_Header_Variables::unknown_min)
    .property("unknownSecond", &Dwg_Header_Variables::unknown_sec)
    .property("unknownMs", &Dwg_Header_Variables::unknown_ms)
    .property("ANGDIR", &Dwg_Header_Variables::ANGDIR)
    .property("SPLFRAME", &Dwg_Header_Variables::SPLFRAME)
    .property("ATTREQ", &Dwg_Header_Variables::ATTREQ)
    .property("ATTDIA", &Dwg_Header_Variables::ATTDIA)
    .property("MIRRTEXT", &Dwg_Header_Variables::MIRRTEXT)
    .property("WORLDVIEW", &Dwg_Header_Variables::WORLDVIEW)
    .property("WIREFRAME", &Dwg_Header_Variables::WIREFRAME)
    .property("TILEMODE", &Dwg_Header_Variables::TILEMODE)
    .property("PLIMCHECK", &Dwg_Header_Variables::PLIMCHECK)
    .property("VISRETAIN", &Dwg_Header_Variables::VISRETAIN)
    .property("DELOBJ", &Dwg_Header_Variables::DELOBJ)
    .property("DISPSILH", &Dwg_Header_Variables::DISPSILH)
    .property("PELLIPSE", &Dwg_Header_Variables::PELLIPSE)
    .property("SAVEIMAGES", &Dwg_Header_Variables::SAVEIMAGES)
    .property("PROXYGRAPHICS", &Dwg_Header_Variables::PROXYGRAPHICS)
    .property("MEASUREMENT", &Dwg_Header_Variables::MEASUREMENT)
    .property("DRAGMODE", &Dwg_Header_Variables::DRAGMODE)
    .property("TREEDEPTH", &Dwg_Header_Variables::TREEDEPTH)
    .property("LUNITS", &Dwg_Header_Variables::LUNITS)
    .property("LUPREC", &Dwg_Header_Variables::LUPREC)
    .property("AUNITS", &Dwg_Header_Variables::AUNITS)
    .property("AUPREC", &Dwg_Header_Variables::AUPREC)
    .property("ATTMODE", &Dwg_Header_Variables::ATTMODE)
    .property("COORDS", &Dwg_Header_Variables::COORDS)
    .property("PDMODE", &Dwg_Header_Variables::PDMODE)
    .property("PICKSTYLE", &Dwg_Header_Variables::PICKSTYLE)
    .property("OSMODE", &Dwg_Header_Variables::OSMODE)
    .property("unknown_12", &Dwg_Header_Variables::unknown_12)
    .property("unknown_13", &Dwg_Header_Variables::unknown_13)
    .property("unknown_14", &Dwg_Header_Variables::unknown_14)
    .property("USERI1", &Dwg_Header_Variables::USERI1)
    .property("USERI2", &Dwg_Header_Variables::USERI2)
    .property("USERI3", &Dwg_Header_Variables::USERI3)
    .property("USERI4", &Dwg_Header_Variables::USERI4)
    .property("USERI5", &Dwg_Header_Variables::USERI5)
    .property("SPLINESEGS", &Dwg_Header_Variables::SPLINESEGS)
    .property("SURFU", &Dwg_Header_Variables::SURFU)
    .property("SURFV", &Dwg_Header_Variables::SURFV)
    .property("SURFTYPE", &Dwg_Header_Variables::SURFTYPE)
    .property("SURFTAB1", &Dwg_Header_Variables::SURFTAB1)
    .property("SURFTAB2", &Dwg_Header_Variables::SURFTAB2)
    .property("SPLINETYPE", &Dwg_Header_Variables::SPLINETYPE)
    .property("SHADEDGE", &Dwg_Header_Variables::SHADEDGE)
    .property("SHADEDIF", &Dwg_Header_Variables::SHADEDIF)
    .property("UNITMODE", &Dwg_Header_Variables::UNITMODE)
    .property("MAXACTVP", &Dwg_Header_Variables::MAXACTVP)
    .property("ISOLINES", &Dwg_Header_Variables::ISOLINES)
    .property("CMLJUST", &Dwg_Header_Variables::CMLJUST)
    .property("TEXTQLTY", &Dwg_Header_Variables::TEXTQLTY)
    .property("unknown_14b", &Dwg_Header_Variables::unknown_14b)
    .property("LTSCALE", &Dwg_Header_Variables::LTSCALE)
    .property("TEXTSIZE", &Dwg_Header_Variables::TEXTSIZE)
    .property("TRACEWID", &Dwg_Header_Variables::TRACEWID)
    .property("SKETCHINC", &Dwg_Header_Variables::SKETCHINC)
    .property("FILLETRAD", &Dwg_Header_Variables::FILLETRAD)
    .property("THICKNESS", &Dwg_Header_Variables::THICKNESS)
    .property("ANGBASE", &Dwg_Header_Variables::ANGBASE)
    .property("PDSIZE", &Dwg_Header_Variables::PDSIZE)
    .property("PLINEWID", &Dwg_Header_Variables::PLINEWID)
    .property("USERR1", &Dwg_Header_Variables::USERR1)
    .property("USERR2", &Dwg_Header_Variables::USERR2)
    .property("USERR3", &Dwg_Header_Variables::USERR3)
    .property("USERR4", &Dwg_Header_Variables::USERR4)
    .property("USERR5", &Dwg_Header_Variables::USERR5)
    .property("CHAMFERA", &Dwg_Header_Variables::CHAMFERA)
    .property("CHAMFERB", &Dwg_Header_Variables::CHAMFERB)
    .property("CHAMFERC", &Dwg_Header_Variables::CHAMFERC)
    .property("CHAMFERD", &Dwg_Header_Variables::CHAMFERD)
    .property("FACETRES", &Dwg_Header_Variables::FACETRES)
    .property("CMLSCALE", &Dwg_Header_Variables::CMLSCALE)
    .property("CELTSCALE", &Dwg_Header_Variables::CELTSCALE)
    .property("VIEWTWIST", &Dwg_Header_Variables::VIEWTWIST)
    .property("MENU", &getDwgHeaderVariablesMenu, &setDwgHeaderVariablesMenu)
    .property("TDCREATE", &Dwg_Header_Variables::TDCREATE)
    .property("TDUPDATE", &Dwg_Header_Variables::TDUPDATE)
    .property("TDUCREATE", &Dwg_Header_Variables::TDUCREATE)
    .property("TDUUPDATE", &Dwg_Header_Variables::TDUUPDATE)
    .property("unknown_15", &Dwg_Header_Variables::unknown_15)
    .property("unknown_16", &Dwg_Header_Variables::unknown_16)
    .property("unknown_17", &Dwg_Header_Variables::unknown_17)
    .property("TDINDWG", &Dwg_Header_Variables::TDINDWG)
    .property("TDUSRTIMER", &Dwg_Header_Variables::TDUSRTIMER)
    .property("CECOLOR", &Dwg_Header_Variables::CECOLOR)
    .property("HANDLING", &Dwg_Header_Variables::HANDLING)
    .property("HANDSEED", &Dwg_Header_Variables::HANDSEED, return_value_policy::reference())
    .property("unknown_4f2", &Dwg_Header_Variables::unknown_4f2)
    .property("unknown_5", &Dwg_Header_Variables::unknown_5)
    .property("unknown_6", &Dwg_Header_Variables::unknown_6)
    .property("unknown_6a", &Dwg_Header_Variables::unknown_6a)
    .property("unknown_6b", &Dwg_Header_Variables::unknown_6b)
    .property("unknown_6c", &Dwg_Header_Variables::unknown_6c)
    .property("CLAYER", &Dwg_Header_Variables::CLAYER, return_value_policy::reference())
    .property("TEXTSTYLE", &Dwg_Header_Variables::TEXTSTYLE, return_value_policy::reference())
    .property("CELTYPE", &Dwg_Header_Variables::CELTYPE, return_value_policy::reference())
    .property("CMATERIAL", &Dwg_Header_Variables::CMATERIAL, return_value_policy::reference())
    .property("DIMSTYLE", &Dwg_Header_Variables::DIMSTYLE, return_value_policy::reference())
    .property("CMLSTYLE", &Dwg_Header_Variables::CMLSTYLE, return_value_policy::reference())
    .property("PSVPSCALE", &Dwg_Header_Variables::PSVPSCALE)
    .property("PINSBASE", &Dwg_Header_Variables::PINSBASE, return_value_policy::reference())
    .property("PEXTMIN", &Dwg_Header_Variables::PEXTMIN, return_value_policy::reference())
    .property("PEXTMAX", &Dwg_Header_Variables::PEXTMAX, return_value_policy::reference())
    .property("PLIMMIN", &Dwg_Header_Variables::PLIMMIN, return_value_policy::reference())
    .property("PLIMMAX", &Dwg_Header_Variables::PLIMMAX, return_value_policy::reference())
    .property("PELEVATION", &Dwg_Header_Variables::PELEVATION)
    .property("PUCSORG", &Dwg_Header_Variables::PUCSORG, return_value_policy::reference())
    .property("PUCSXDIR", &Dwg_Header_Variables::PUCSXDIR, return_value_policy::reference())
    .property("PUCSYDIR", &Dwg_Header_Variables::PUCSYDIR, return_value_policy::reference())
    .property("PUCSNAME", &Dwg_Header_Variables::PUCSNAME, return_value_policy::reference())
    .property("PUCSBASE", &Dwg_Header_Variables::PUCSBASE, return_value_policy::reference())
    .property("PUCSORTHOREF", &Dwg_Header_Variables::PUCSORTHOREF, return_value_policy::reference())
    .property("PUCSORTHOVIEW", &Dwg_Header_Variables::PUCSORTHOVIEW, return_value_policy::reference())
    .property("PUCSORGTOP", &Dwg_Header_Variables::PUCSORGTOP, return_value_policy::reference())
    .property("PUCSORGBOTTOM", &Dwg_Header_Variables::PUCSORGBOTTOM, return_value_policy::reference())
    .property("PUCSORGLEFT", &Dwg_Header_Variables::PUCSORGLEFT, return_value_policy::reference())
    .property("PUCSORGRIGHT", &Dwg_Header_Variables::PUCSORGRIGHT, return_value_policy::reference())
    .property("PUCSORGFRONT", &Dwg_Header_Variables::PUCSORGFRONT, return_value_policy::reference())
    .property("PUCSORGBACK", &Dwg_Header_Variables::PUCSORGBACK, return_value_policy::reference())
    .property("INSBASE", &Dwg_Header_Variables::INSBASE, return_value_policy::reference())
    .property("EXTMIN", &Dwg_Header_Variables::EXTMIN, return_value_policy::reference())
    .property("EXTMAX", &Dwg_Header_Variables::EXTMAX, return_value_policy::reference())
    .property("VIEWDIR", &Dwg_Header_Variables::VIEWDIR, return_value_policy::reference())
    .property("TARGET", &Dwg_Header_Variables::TARGET, return_value_policy::reference())
    .property("LIMMIN", &Dwg_Header_Variables::LIMMIN, return_value_policy::reference())
    .property("LIMMAX", &Dwg_Header_Variables::LIMMAX, return_value_policy::reference())
    .property("VIEWCTR", &Dwg_Header_Variables::VIEWCTR, return_value_policy::reference())
    .property("ELEVATION", &Dwg_Header_Variables::ELEVATION)
    .property("VIEWSIZE", &Dwg_Header_Variables::VIEWSIZE)
    .property("SNAPMODE", &Dwg_Header_Variables::SNAPMODE)
    .property("SNAPUNIT", &Dwg_Header_Variables::SNAPUNIT, return_value_policy::reference())
    .property("SNAPBASE", &Dwg_Header_Variables::SNAPBASE, return_value_policy::reference())
    .property("SNAPANG", &Dwg_Header_Variables::SNAPANG)
    .property("SNAPSTYLE", &Dwg_Header_Variables::SNAPSTYLE)
    .property("SNAPISOPAIR", &Dwg_Header_Variables::SNAPISOPAIR)
    .property("GRIDMODE", &Dwg_Header_Variables::GRIDMODE)
    .property("GRIDUNIT", &Dwg_Header_Variables::GRIDUNIT, return_value_policy::reference())
    .property("AXISMODE", &Dwg_Header_Variables::AXISMODE)
    .property("AXISUNIT", &Dwg_Header_Variables::AXISUNIT, return_value_policy::reference())
    .property("VPOINTX", &Dwg_Header_Variables::VPOINTX, return_value_policy::reference())
    .property("VPOINTY", &Dwg_Header_Variables::VPOINTY, return_value_policy::reference())
    .property("VPOINTZ", &Dwg_Header_Variables::VPOINTZ, return_value_policy::reference())
    .property("VPOINTXALT", &Dwg_Header_Variables::VPOINTXALT, return_value_policy::reference())
    .property("VPOINTYALT", &Dwg_Header_Variables::VPOINTYALT, return_value_policy::reference())
    .property("VPOINTZALT", &Dwg_Header_Variables::VPOINTZALT, return_value_policy::reference())
    .property("flag_3d", &Dwg_Header_Variables::flag_3d)
    .property("UCSORG", &Dwg_Header_Variables::UCSORG, return_value_policy::reference())
    .property("UCSXDIR", &Dwg_Header_Variables::UCSXDIR, return_value_policy::reference())
    .property("UCSYDIR", &Dwg_Header_Variables::UCSYDIR, return_value_policy::reference())
    .property("UCSNAME", &Dwg_Header_Variables::UCSNAME, return_value_policy::reference())
    .property("UCSBASE", &Dwg_Header_Variables::UCSBASE, return_value_policy::reference())
    .property("UCSORTHOVIEW", &Dwg_Header_Variables::UCSORTHOVIEW)
    .property("UCSORTHOREF", &Dwg_Header_Variables::UCSORTHOREF, return_value_policy::reference())
    .property("UCSORGTOP", &Dwg_Header_Variables::UCSORGTOP, return_value_policy::reference())
    .property("UCSORGBOTTOM", &Dwg_Header_Variables::UCSORGBOTTOM, return_value_policy::reference())
    .property("UCSORGLEFT", &Dwg_Header_Variables::UCSORGLEFT, return_value_policy::reference())
    .property("UCSORGRIGHT", &Dwg_Header_Variables::UCSORGRIGHT, return_value_policy::reference())
    .property("UCSORGFRONT", &Dwg_Header_Variables::UCSORGFRONT, return_value_policy::reference())
    .property("UCSORGBACK", &Dwg_Header_Variables::UCSORGBACK, return_value_policy::reference())
    .property("DIMPOST", &getDwgHeaderVariablesDimPost, &setDwgHeaderVariablesDimPost)
    .property("DIMAPOST", &getDwgHeaderVariablesDimaPost, &setDwgHeaderVariablesDimaPost)
    .property("DIMTOL", &Dwg_Header_Variables::DIMTOL)
    .property("DIMLIM", &Dwg_Header_Variables::DIMLIM)
    .property("DIMTIH", &Dwg_Header_Variables::DIMTIH)
    .property("DIMTOH", &Dwg_Header_Variables::DIMTOH)
    .property("DIMSE1", &Dwg_Header_Variables::DIMSE1)
    .property("DIMSE2", &Dwg_Header_Variables::DIMSE2)
    .property("DIMALT", &Dwg_Header_Variables::DIMALT)
    .property("DIMTOFL", &Dwg_Header_Variables::DIMTOFL)
    .property("DIMSAH", &Dwg_Header_Variables::DIMSAH)
    .property("DIMTIX", &Dwg_Header_Variables::DIMTIX)
    .property("DIMSOXD", &Dwg_Header_Variables::DIMSOXD)
    .property("DIMALTD", &Dwg_Header_Variables::DIMALTD)
    .property("DIMZIN", &Dwg_Header_Variables::DIMZIN)
    .property("DIMSD1", &Dwg_Header_Variables::DIMSD1)
    .property("DIMSD2", &Dwg_Header_Variables::DIMSD2)
    .property("DIMTOLJ", &Dwg_Header_Variables::DIMTOLJ)
    .property("DIMJUST", &Dwg_Header_Variables::DIMJUST)
    .property("DIMFIT", &Dwg_Header_Variables::DIMFIT)
    .property("DIMUPT", &Dwg_Header_Variables::DIMUPT)
    .property("DIMTZIN", &Dwg_Header_Variables::DIMTZIN)
    .property("DIMTAD", &Dwg_Header_Variables::DIMTAD)
    .property("DIMUNIT", &Dwg_Header_Variables::DIMUNIT)
    .property("DIMAUNIT", &Dwg_Header_Variables::DIMAUNIT)
    .property("DIMDEC", &Dwg_Header_Variables::DIMDEC)
    .property("DIMTDEC", &Dwg_Header_Variables::DIMTDEC)
    .property("DIMALTU", &Dwg_Header_Variables::DIMALTU)
    .property("DIMALTTD", &Dwg_Header_Variables::DIMALTTD)
    .property("DIMTXSTY", &Dwg_Header_Variables::DIMTXSTY, return_value_policy::reference())
    .property("DIMSCALE", &Dwg_Header_Variables::DIMSCALE)
    .property("DIMARROW", &Dwg_Header_Variables::DIMARROW)
    .property("DIMASZ", &Dwg_Header_Variables::DIMASZ)
    .property("DIMEXO", &Dwg_Header_Variables::DIMEXO)
    .property("DIMDLI", &Dwg_Header_Variables::DIMDLI)
    .property("DIMEXE", &Dwg_Header_Variables::DIMEXE)
    .property("DIMRND", &Dwg_Header_Variables::DIMRND)
    .property("DIMDLE", &Dwg_Header_Variables::DIMDLE)
    .property("DIMTP", &Dwg_Header_Variables::DIMTP)
    .property("DIMTM", &Dwg_Header_Variables::DIMTM)
    .property("DIMFXL", &Dwg_Header_Variables::DIMFXL)
    .property("DIMJOGANG", &Dwg_Header_Variables::DIMJOGANG)
    .property("DIMTFILL", &Dwg_Header_Variables::DIMTFILL)
    .property("DIMTFILLCLR", &Dwg_Header_Variables::DIMTFILLCLR)
    .property("DIMAZIN", &Dwg_Header_Variables::DIMAZIN)
    .property("DIMARCSYM", &Dwg_Header_Variables::DIMARCSYM)
    .property("DIMTXT", &Dwg_Header_Variables::DIMTXT)
    .property("DIMCEN", &Dwg_Header_Variables::DIMCEN)
    .property("DIMTSZ", &Dwg_Header_Variables::DIMTSZ)
    .property("DIMALTF", &Dwg_Header_Variables::DIMALTF)
    .property("DIMLFAC", &Dwg_Header_Variables::DIMLFAC)
    .property("DIMTVP", &Dwg_Header_Variables::DIMTVP)
    .property("DIMTFAC", &Dwg_Header_Variables::DIMTFAC)
    .property("DIMGAP", &Dwg_Header_Variables::DIMGAP)
    .property("DIMPOST_T", &getDwgHeaderVariablesDimPostT, &setDwgHeaderVariablesDimPostT)
    .property("DIMAPOST_T", &getDwgHeaderVariablesDimaPostT, &setDwgHeaderVariablesDimaPostT)
    .property("DIMBLK_T", &getDwgHeaderVariablesDimBlkT, &setDwgHeaderVariablesDimBlkT)
    .property("DIMBLK1_T", &getDwgHeaderVariablesDimBlk1T, &setDwgHeaderVariablesDimBlk1T)
    .property("DIMBLK2_T", &getDwgHeaderVariablesDimBlk2T, &setDwgHeaderVariablesDimBlk2T)
    .property("unknown_string", &getDwgHeaderVariablesUnknownString, &setDwgHeaderVariablesUnknownString)
    .property("DIMALTRND", &Dwg_Header_Variables::DIMALTRND)
    .property("DIMCLRD_C", &Dwg_Header_Variables::DIMCLRD_C)
    .property("DIMCLRE_C", &Dwg_Header_Variables::DIMCLRE_C)
    .property("DIMCLRT_C", &Dwg_Header_Variables::DIMCLRT_C)
    .property("DIMCLRD", &Dwg_Header_Variables::DIMCLRD)
    .property("DIMCLRE", &Dwg_Header_Variables::DIMCLRE)
    .property("DIMCLRT", &Dwg_Header_Variables::DIMCLRT)
    .property("DIMADEC", &Dwg_Header_Variables::DIMADEC)
    .property("DIMFRAC", &Dwg_Header_Variables::DIMFRAC)
    .property("DIMLUNIT", &Dwg_Header_Variables::DIMLUNIT)
    .property("DIMDSEP", &Dwg_Header_Variables::DIMDSEP)
    .property("DIMTMOVE", &Dwg_Header_Variables::DIMTMOVE)
    .property("DIMALTZ", &Dwg_Header_Variables::DIMALTZ)
    .property("DIMALTTZ", &Dwg_Header_Variables::DIMALTTZ)
    .property("DIMATFIT", &Dwg_Header_Variables::DIMATFIT)
    .property("DIMFXLON", &Dwg_Header_Variables::DIMFXLON)
    .property("DIMTXTDIRECTION", &Dwg_Header_Variables::DIMTXTDIRECTION)
    .property("DIMALTMZF", &Dwg_Header_Variables::DIMALTMZF)
    .property("DIMALTMZS", &getDwgHeaderVariablesDimalTMZS, &setDwgHeaderVariablesDimalTMZS)
    .property("DIMMZF", &Dwg_Header_Variables::DIMMZF)
    .property("DIMMZS", &getDwgHeaderVariablesDimMZS, &setDwgHeaderVariablesDimMZS)
    .property("DIMLDRBLK", &Dwg_Header_Variables::DIMLDRBLK, return_value_policy::reference())
    .property("DIMBLK", &Dwg_Header_Variables::DIMBLK, return_value_policy::reference())
    .property("DIMBLK1", &Dwg_Header_Variables::DIMBLK1, return_value_policy::reference())
    .property("DIMBLK2", &Dwg_Header_Variables::DIMBLK2, return_value_policy::reference())
    .property("DIMLTYPE", &Dwg_Header_Variables::DIMLTYPE, return_value_policy::reference())
    .property("DIMLTEX1", &Dwg_Header_Variables::DIMLTEX1, return_value_policy::reference())
    .property("DIMLTEX2", &Dwg_Header_Variables::DIMLTEX2, return_value_policy::reference())
    .property("DIMLWD", &Dwg_Header_Variables::DIMLWD)
    .property("DIMLWE", &Dwg_Header_Variables::DIMLWE)
    .property("BLOCK_CONTROL_OBJECT", &Dwg_Header_Variables::BLOCK_CONTROL_OBJECT, return_value_policy::reference())
    .property("LAYER_CONTROL_OBJECT", &Dwg_Header_Variables::LAYER_CONTROL_OBJECT, return_value_policy::reference())
    .property("STYLE_CONTROL_OBJECT", &Dwg_Header_Variables::STYLE_CONTROL_OBJECT, return_value_policy::reference())
    .property("LTYPE_CONTROL_OBJECT", &Dwg_Header_Variables::LTYPE_CONTROL_OBJECT, return_value_policy::reference())
    .property("VIEW_CONTROL_OBJECT", &Dwg_Header_Variables::VIEW_CONTROL_OBJECT, return_value_policy::reference())
    .property("UCS_CONTROL_OBJECT", &Dwg_Header_Variables::UCS_CONTROL_OBJECT, return_value_policy::reference())
    .property("VPORT_CONTROL_OBJECT", &Dwg_Header_Variables::VPORT_CONTROL_OBJECT, return_value_policy::reference())
    .property("APPID_CONTROL_OBJECT", &Dwg_Header_Variables::APPID_CONTROL_OBJECT, return_value_policy::reference())
    .property("DIMSTYLE_CONTROL_OBJECT", &Dwg_Header_Variables::DIMSTYLE_CONTROL_OBJECT, return_value_policy::reference())
    .property("VX_CONTROL_OBJECT", &Dwg_Header_Variables::VX_CONTROL_OBJECT, return_value_policy::reference())
    .property("DICTIONARY_ACAD_GROUP", &Dwg_Header_Variables::DICTIONARY_ACAD_GROUP, return_value_policy::reference())
    .property("DICTIONARY_ACAD_MLINESTYLE", &Dwg_Header_Variables::DICTIONARY_ACAD_MLINESTYLE, return_value_policy::reference())
    .property("DICTIONARY_NAMED_OBJECT", &Dwg_Header_Variables::DICTIONARY_NAMED_OBJECT, return_value_policy::reference())
    .property("TSTACKALIGN", &Dwg_Header_Variables::TSTACKALIGN)
    .property("TSTACKSIZE", &Dwg_Header_Variables::TSTACKSIZE)
    .property("HYPERLINKBASE", &getDwgHeaderVariablesHyperLinkBase, &setDwgHeaderVariablesHyperLinkBase)
    .property("STYLESHEET", &getDwgHeaderVariablesStyleSheet, &setDwgHeaderVariablesStyleSheet)
    .property("DICTIONARY_LAYOUT", &Dwg_Header_Variables::DICTIONARY_LAYOUT, return_value_policy::reference())
    .property("DICTIONARY_PLOTSETTINGS", &Dwg_Header_Variables::DICTIONARY_PLOTSETTINGS, return_value_policy::reference())
    .property("DICTIONARY_PLOTSTYLENAME", &Dwg_Header_Variables::DICTIONARY_PLOTSTYLENAME, return_value_policy::reference())
    .property("DICTIONARY_MATERIAL", &Dwg_Header_Variables::DICTIONARY_MATERIAL, return_value_policy::reference())
    .property("DICTIONARY_COLOR", &Dwg_Header_Variables::DICTIONARY_COLOR, return_value_policy::reference())
    .property("DICTIONARY_VISUALSTYLE", &Dwg_Header_Variables::DICTIONARY_VISUALSTYLE, return_value_policy::reference())
    .property("DICTIONARY_LIGHTLIST", &Dwg_Header_Variables::DICTIONARY_LIGHTLIST, return_value_policy::reference())
    .property("unknown_20", &Dwg_Header_Variables::unknown_20, return_value_policy::reference())
    .property("FLAGS", &Dwg_Header_Variables::FLAGS)
    .property("CELWEIGHT", &Dwg_Header_Variables::CELWEIGHT)
    .property("ENDCAPS", &Dwg_Header_Variables::ENDCAPS)
    .property("JOINSTYLE", &Dwg_Header_Variables::JOINSTYLE)
    .property("LWDISPLAY", &Dwg_Header_Variables::LWDISPLAY)
    .property("XEDIT", &Dwg_Header_Variables::XEDIT)
    .property("EXTNAMES", &Dwg_Header_Variables::EXTNAMES)
    .property("PSTYLEMODE", &Dwg_Header_Variables::PSTYLEMODE)
    .property("OLESTARTUP", &Dwg_Header_Variables::OLESTARTUP)
    .property("INSUNITS", &Dwg_Header_Variables::INSUNITS)
    .property("CEPSNTYPE", &Dwg_Header_Variables::CEPSNTYPE)
    .property("CPSNID", &Dwg_Header_Variables::CPSNID, return_value_policy::reference())
    .property("FINGERPRINTGUID", &getDwgHeaderVariablesFingerPrintGuid, &setDwgHeaderVariablesFingerPrintGuid)
    .property("VERSIONGUID", &getDwgHeaderVariablesVisionGuid, &setDwgHeaderVariablesVisionGuid)
    .property("SORTENTS", &Dwg_Header_Variables::SORTENTS)
    .property("INDEXCTL", &Dwg_Header_Variables::INDEXCTL)
    .property("HIDETEXT", &Dwg_Header_Variables::HIDETEXT)
    .property("XCLIPFRAME", &Dwg_Header_Variables::XCLIPFRAME)
    .property("DIMASSOC", &Dwg_Header_Variables::DIMASSOC)
    .property("HALOGAP", &Dwg_Header_Variables::HALOGAP)
    .property("OBSCOLOR", &Dwg_Header_Variables::OBSCOLOR)
    .property("INTERSECTIONCOLOR", &Dwg_Header_Variables::INTERSECTIONCOLOR)
    .property("OBSLTYPE", &Dwg_Header_Variables::OBSLTYPE)
    .property("INTERSECTIONDISPLAY", &Dwg_Header_Variables::INTERSECTIONDISPLAY)
    .property("PROJECTNAME", &getDwgHeaderVariablesProjectName, &setDwgHeaderVariablesProjectName)
    .property("BLOCK_RECORD_PSPACE", &Dwg_Header_Variables::BLOCK_RECORD_PSPACE, return_value_policy::reference())
    .property("BLOCK_RECORD_MSPACE", &Dwg_Header_Variables::BLOCK_RECORD_MSPACE, return_value_policy::reference())
    .property("LTYPE_BYLAYER", &Dwg_Header_Variables::LTYPE_BYLAYER, return_value_policy::reference())
    .property("LTYPE_BYBLOCK", &Dwg_Header_Variables::LTYPE_BYBLOCK, return_value_policy::reference())
    .property("LTYPE_CONTINUOUS", &Dwg_Header_Variables::LTYPE_CONTINUOUS, return_value_policy::reference())
    .property("CAMERADISPLAY", &Dwg_Header_Variables::CAMERADISPLAY)
    .property("unknown_21", &Dwg_Header_Variables::unknown_21)
    .property("unknown_22", &Dwg_Header_Variables::unknown_22)
    .property("unknown_23", &Dwg_Header_Variables::unknown_23)
    .property("STEPSPERSEC", &Dwg_Header_Variables::STEPSPERSEC)
    .property("STEPSIZE", &Dwg_Header_Variables::STEPSIZE)
    .property("_3DDWFPREC", &Dwg_Header_Variables::_3DDWFPREC)
    .property("LENSLENGTH", &Dwg_Header_Variables::LENSLENGTH)
    .property("CAMERAHEIGHT", &Dwg_Header_Variables::CAMERAHEIGHT)
    .property("SOLIDHIST", &Dwg_Header_Variables::SOLIDHIST)
    .property("SHOWHIST", &Dwg_Header_Variables::SHOWHIST)
    .property("PSOLWIDTH", &Dwg_Header_Variables::PSOLWIDTH)
    .property("PSOLHEIGHT", &Dwg_Header_Variables::PSOLHEIGHT)
    .property("LOFTANG1", &Dwg_Header_Variables::LOFTANG1)
    .property("LOFTANG2", &Dwg_Header_Variables::LOFTANG2)
    .property("LOFTMAG1", &Dwg_Header_Variables::LOFTMAG1)
    .property("LOFTMAG2", &Dwg_Header_Variables::LOFTMAG2)
    .property("LOFTPARAM", &Dwg_Header_Variables::LOFTPARAM)
    .property("LOFTNORMALS", &Dwg_Header_Variables::LOFTNORMALS)
    .property("LATITUDE", &Dwg_Header_Variables::LATITUDE)
    .property("LONGITUDE", &Dwg_Header_Variables::LONGITUDE)
    .property("NORTHDIRECTION", &Dwg_Header_Variables::NORTHDIRECTION)
    .property("TIMEZONE", &Dwg_Header_Variables::TIMEZONE)
    .property("LIGHTGLYPHDISPLAY", &Dwg_Header_Variables::LIGHTGLYPHDISPLAY)
    .property("TILEMODELIGHTSYNCH", &Dwg_Header_Variables::TILEMODELIGHTSYNCH)
    .property("DWFFRAME", &Dwg_Header_Variables::DWFFRAME)
    .property("DGNFRAME", &Dwg_Header_Variables::DGNFRAME)
    .property("REALWORLDSCALE", &Dwg_Header_Variables::REALWORLDSCALE)
    .property("INTERFERECOLOR", &Dwg_Header_Variables::INTERFERECOLOR)
    .property("INTERFEREOBJVS", &Dwg_Header_Variables::INTERFEREOBJVS, return_value_policy::reference())
    .property("INTERFEREVPVS", &Dwg_Header_Variables::INTERFEREVPVS, return_value_policy::reference())
    .property("DRAGVS", &Dwg_Header_Variables::DRAGVS, return_value_policy::reference())
    .property("CSHADOW", &Dwg_Header_Variables::CSHADOW)
    .property("SHADOWPLANELOCATION", &Dwg_Header_Variables::SHADOWPLANELOCATION)
    .property("unknown_54", &Dwg_Header_Variables::unknown_54)
    .property("unknown_55", &Dwg_Header_Variables::unknown_55)
    .property("unknown_56", &Dwg_Header_Variables::unknown_56)
    .property("unknown_57", &Dwg_Header_Variables::unknown_57)
    .property("dwg_size", &Dwg_Header_Variables::dwg_size)
    .property("numentities", &Dwg_Header_Variables::numentities)
    .property("circle_zoom_percent", &Dwg_Header_Variables::circle_zoom_percent)
    .property("FRONTZ", &Dwg_Header_Variables::FRONTZ)
    .property("BACKZ", &Dwg_Header_Variables::BACKZ)
    .property("UCSICON", &Dwg_Header_Variables::UCSICON)
    .property("oldCECOLOR_hi", &Dwg_Header_Variables::oldCECOLOR_hi)
    .property("oldCECOLOR_lo", &Dwg_Header_Variables::oldCECOLOR_lo)
    // .property("layer_colors", &getDwgHeaderVariablesLayerColors, &setDwgHeaderVariablesLayerColors)
    .property("unknown_51e", &Dwg_Header_Variables::unknown_51e)
    .property("unknown_520", &Dwg_Header_Variables::unknown_520)
    .property("unknown_52c", &Dwg_Header_Variables::unknown_52c)
    .property("unknown_52e", &Dwg_Header_Variables::unknown_52e)
    .property("unknown_530", &Dwg_Header_Variables::unknown_530)
    .property("unknown_59", &Dwg_Header_Variables::unknown_59)
    .property("aspect_ratio", &Dwg_Header_Variables::aspect_ratio);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_TEXT, Dwg_Entity_TEXT)
    .property("dataFlags", &Dwg_Entity_TEXT::dataflags)
    .property("elevation", &Dwg_Entity_TEXT::elevation)
    .property("insPoint", &Dwg_Entity_TEXT::ins_pt, return_value_policy::reference())
    .property("alignmentPoint", &Dwg_Entity_TEXT::alignment_pt, return_value_policy::reference())
    .property("extrusion", &Dwg_Entity_TEXT::extrusion)
    .property("thickness", &Dwg_Entity_TEXT::thickness)
    .property("obliqueAngle", &Dwg_Entity_TEXT::oblique_angle)
    .property("rotation", &Dwg_Entity_TEXT::rotation)
    .property("height", &Dwg_Entity_TEXT::height)
    .property("widthFactor", &Dwg_Entity_TEXT::width_factor)
    .property("textValue", &getDwgEntityTexTTextValue, &setDwgEntityTexTTextValue)
    .property("generation", &Dwg_Entity_TEXT::generation)
    .property("horizAlignment", &Dwg_Entity_TEXT::horiz_alignment)
    .property("vertAlignment", &Dwg_Entity_TEXT::vert_alignment)
    .property("style", &Dwg_Entity_TEXT::style, return_value_policy::reference());

  class_<Dwg_AcDbMTextObjectEmbedded>("Dwg_AcDbMTextObjectEmbedded")
    .constructor<>()
    .property("attachment", &Dwg_AcDbMTextObjectEmbedded::attachment)
    .property("insPoint", &Dwg_AcDbMTextObjectEmbedded::ins_pt, return_value_policy::reference())
    .property("xAxisDir", &Dwg_AcDbMTextObjectEmbedded::x_axis_dir, return_value_policy::reference())
    .property("rectHeight", &Dwg_AcDbMTextObjectEmbedded::rect_height)
    .property("rectWidth", &Dwg_AcDbMTextObjectEmbedded::rect_width)
    .property("extentsWidth", &Dwg_AcDbMTextObjectEmbedded::extents_width)
    .property("extentsHeight", &Dwg_AcDbMTextObjectEmbedded::extents_height)
    .property("columnType", &Dwg_AcDbMTextObjectEmbedded::column_type)
    .property("columnWidth", &Dwg_AcDbMTextObjectEmbedded::column_width)
    .property("gutter", &Dwg_AcDbMTextObjectEmbedded::gutter)
    .property("autoGeight", &Dwg_AcDbMTextObjectEmbedded::auto_height)
    .property("flowReversed", &Dwg_AcDbMTextObjectEmbedded::flow_reversed)
    .property("numColumnHeights", &Dwg_AcDbMTextObjectEmbedded::num_column_heights);
    // TODO: Double check whether this binding is correct
    // .property("columnHeights", &Dwg_AcDbMTextObjectEmbedded::column_heights, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_ATTRIB, Dwg_Entity_ATTRIB)
    .property("elevation", &Dwg_Entity_ATTRIB::elevation)
    .property("insPoint", &Dwg_Entity_ATTRIB::ins_pt, return_value_policy::reference())
    .property("alignmentPoint", &Dwg_Entity_ATTRIB::alignment_pt, return_value_policy::reference())
    .property("extrusion", &Dwg_Entity_ATTRIB::extrusion)
    .property("thickness", &Dwg_Entity_ATTRIB::thickness)
    .property("obliqueAngle", &Dwg_Entity_ATTRIB::oblique_angle)
    .property("rotation", &Dwg_Entity_ATTRIB::rotation)
    .property("height", &Dwg_Entity_ATTRIB::height)
    .property("widthFactor", &Dwg_Entity_ATTRIB::width_factor)
    .property("textValue", &getDwgEntityAttribTextValue, &setDwgEntityAttribTextValue)
    .property("generation", &Dwg_Entity_ATTRIB::generation)
    .property("horizAlignment", &Dwg_Entity_ATTRIB::horiz_alignment)
    .property("vertAlignment", &Dwg_Entity_ATTRIB::vert_alignment)
    .property("dataFlags", &Dwg_Entity_ATTRIB::dataflags)
    .property("isLockedInBlock", &Dwg_Entity_ATTRIB::is_locked_in_block)
    .property("keepDuplicateRecords", &Dwg_Entity_ATTRIB::keep_duplicate_records)
    .property("tag", &getDwgEntityAttribTag, &setDwgEntityAttribTag)
    .property("fieldLength", &Dwg_Entity_ATTRIB::field_length)
    .property("flags", &Dwg_Entity_ATTRIB::flags)
    .property("lockPositionFlag", &Dwg_Entity_ATTRIB::lock_position_flag)
    .property("style", &Dwg_Entity_ATTRIB::style, return_value_policy::reference())
    .property("mtextType", &Dwg_Entity_ATTRIB::mtext_type)
    .property("mtext", &Dwg_Entity_ATTRIB::mtext, return_value_policy::reference())
    .property("isReallyLocked", &Dwg_Entity_ATTRIB::is_really_locked)
    .property("annotativeDataSize", &Dwg_Entity_ATTRIB::annotative_data_size)
    // TODO: Double check whether this binding is correct
    // .property("annotativeData", &Dwg_Entity_ATTRIB::annotative_data, return_value_policy::reference())
    .property("annotativeFlag", &Dwg_Entity_ATTRIB::annotative_flag)
    .property("annotativeStyle", &Dwg_Entity_ATTRIB::annotative_style, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_ATTDEF, Dwg_Entity_ATTDEF)
    .property("elevation", &Dwg_Entity_ATTDEF::elevation)
    .property("insPoint", &Dwg_Entity_ATTDEF::ins_pt, return_value_policy::reference())
    .property("alignmentPoint", &Dwg_Entity_ATTDEF::alignment_pt, return_value_policy::reference())
    .property("extrusion", &Dwg_Entity_ATTDEF::extrusion)
    .property("thickness", &Dwg_Entity_ATTDEF::thickness)
    .property("obliqueAngle", &Dwg_Entity_ATTDEF::oblique_angle)
    .property("rotation", &Dwg_Entity_ATTDEF::rotation)
    .property("height", &Dwg_Entity_ATTDEF::height)
    .property("widthFactor", &Dwg_Entity_ATTDEF::width_factor)
    .property("defaultValue", &getDwgEntityAttDefDefaultValue, &setDwgEntityAttDefDefaultValue)
    .property("generation", &Dwg_Entity_ATTDEF::generation)
    .property("horizAlignment", &Dwg_Entity_ATTDEF::horiz_alignment)
    .property("vertAlignment", &Dwg_Entity_ATTDEF::vert_alignment)
    .property("dataFlags", &Dwg_Entity_ATTDEF::dataflags)
    .property("isLockedInBlock", &Dwg_Entity_ATTDEF::is_locked_in_block)
    .property("keepDuplicateRecords", &Dwg_Entity_ATTDEF::keep_duplicate_records)
    .property("tag", &getDwgEntityAttDefTag, &setDwgEntityAttDefTag)
    .property("fieldLength", &Dwg_Entity_ATTDEF::field_length)
    .property("flags", &Dwg_Entity_ATTDEF::flags)
    .property("lockPositionFlag", &Dwg_Entity_ATTDEF::lock_position_flag)
    .property("style", &Dwg_Entity_ATTDEF::style, return_value_policy::reference())
    .property("mtextType", &Dwg_Entity_ATTDEF::mtext_type)
    .property("mtext", &Dwg_Entity_ATTDEF::mtext, return_value_policy::reference())
    .property("isReallyLocked", &Dwg_Entity_ATTDEF::is_really_locked)
    .property("annotativeDataSize", &Dwg_Entity_ATTDEF::annotative_data_size)
    // TODO: Double check whether this binding is correct
    // .property("annotativeData", &Dwg_Entity_ATTDEF::annotative_data, return_value_policy::reference())
    .property("annotativeFlag", &Dwg_Entity_ATTDEF::annotative_flag)
    .property("annotativeStyle", &Dwg_Entity_ATTDEF::annotative_style, return_value_policy::reference())
    .property("prompt", &getDwgEntityAttDefPrompt, &setDwgEntityAttDefPrompt);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_BLOCK, Dwg_Entity_BLOCK)
    .property("name", &getDwgEntityBlockName, &setDwgEntityBlockName)
    .property("xref_pname", &getDwgEntityBlockXRefName, &setDwgEntityBlockXRefName)
    .property("basePoint", &Dwg_Entity_BLOCK::base_pt, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_ENDBLK, Dwg_Entity_ENDBLK);
  
  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_SEQEND, Dwg_Entity_SEQEND)
    .property("beginAddrR11", &Dwg_Entity_SEQEND::begin_addr_r11);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_INSERT, Dwg_Entity_INSERT)
    .property("insPoint", &Dwg_Entity_INSERT::ins_pt, return_value_policy::reference())
    .property("scaleFlag", &Dwg_Entity_INSERT::scale_flag)
    .property("scale", &Dwg_Entity_INSERT::scale, return_value_policy::reference())
    .property("rotation", &Dwg_Entity_INSERT::rotation)
    .property("extrusion", &Dwg_Entity_INSERT::extrusion)
    .property("hasAttribs", &Dwg_Entity_INSERT::has_attribs)
    .property("numOwned", &Dwg_Entity_INSERT::num_owned)
    .property("blockHeader", &Dwg_Entity_INSERT::block_header, return_value_policy::reference())
    .property("firstAttrib", &Dwg_Entity_INSERT::first_attrib, return_value_policy::reference())
    .property("lastAttrib", &Dwg_Entity_INSERT::last_attrib, return_value_policy::reference())
    // TODO: Double check whether this binding is correct
    // .property("attribs", &Dwg_Entity_INSERT::attribs, return_value_policy::reference())
    .property("seqend", &Dwg_Entity_INSERT::seqend, return_value_policy::reference())
    .property("numCols", &Dwg_Entity_INSERT::num_cols)
    .property("numRows", &Dwg_Entity_INSERT::num_rows)
    .property("colSpacing", &Dwg_Entity_INSERT::col_spacing)
    .property("rowSpacing", &Dwg_Entity_INSERT::row_spacing)
    .property("blockName", &getDwgEntityInsertBlockName, &setDwgEntityInsertBlockName);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_MINSERT, Dwg_Entity_MINSERT)
    .property("insPoint", &Dwg_Entity_MINSERT::ins_pt, return_value_policy::reference())
    .property("scaleFlag", &Dwg_Entity_MINSERT::scale_flag)
    .property("scale", &Dwg_Entity_MINSERT::scale, return_value_policy::reference())
    .property("rotation", &Dwg_Entity_MINSERT::rotation)
    .property("extrusion", &Dwg_Entity_MINSERT::extrusion)
    .property("hasAttribs", &Dwg_Entity_MINSERT::has_attribs)
    .property("numOwned", &Dwg_Entity_MINSERT::num_owned)
    .property("numCols", &Dwg_Entity_MINSERT::num_cols)
    .property("numRows", &Dwg_Entity_MINSERT::num_rows)
    .property("colSpacing", &Dwg_Entity_MINSERT::col_spacing)
    .property("rowSpacing", &Dwg_Entity_MINSERT::row_spacing)
    .property("blockHeader", &Dwg_Entity_MINSERT::block_header, return_value_policy::reference())
    .property("firstAttrib", &Dwg_Entity_MINSERT::first_attrib, return_value_policy::reference())
    .property("lastAttrib", &Dwg_Entity_MINSERT::last_attrib, return_value_policy::reference())
    // TODO: Double check whether this binding is correct
    // .property("attribs", &Dwg_Entity_MINSERT::attribs, return_value_policy::reference())
    .property("seqend", &Dwg_Entity_MINSERT::seqend, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_VERTEX_2D, Dwg_Entity_VERTEX_2D)
    .property("flag", &Dwg_Entity_VERTEX_2D::flag)
    .property("point", &Dwg_Entity_VERTEX_2D::point, return_value_policy::reference())
    .property("startWidth", &Dwg_Entity_VERTEX_2D::start_width)
    .property("endWidth", &Dwg_Entity_VERTEX_2D::end_width)
    .property("id", &Dwg_Entity_VERTEX_2D::id)
    .property("bulge", &Dwg_Entity_VERTEX_2D::bulge)
    .property("tangentDir", &Dwg_Entity_VERTEX_2D::tangent_dir);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_VERTEX_3D, Dwg_Entity_VERTEX_3D)
    .property("flag", &Dwg_Entity_VERTEX_3D::flag)
    .property("point", &Dwg_Entity_VERTEX_3D::point, return_value_policy::reference());

  // DEFINE_CLASS_WITH_PARENT(Dwg_Entity_VERTEX_MESH, Dwg_Entity_VERTEX_MESH)
  //   .property("flag", &Dwg_Entity_VERTEX_MESH::flag)
  //   .property("point", &Dwg_Entity_VERTEX_MESH::point, return_value_policy::reference());

  // DEFINE_CLASS_WITH_PARENT(Dwg_Entity_VERTEX_PFACE, Dwg_Entity_VERTEX_PFACE)
  //   .property("flag", &Dwg_Entity_VERTEX_PFACE::flag)
  //   .property("point", &Dwg_Entity_VERTEX_PFACE::point, return_value_policy::reference());

  // Register 'std::array<unsigned short, 4>' because property 'vertind' in 'Dwg_Entity_VERTEX_PFACE_FACE' is interpreted as such
  value_array<std::array<BITCODE_BS, 4>>("Dwg_Array_UnsignedShort4")
    .element(emscripten::index<0>())
    .element(emscripten::index<1>())
    .element(emscripten::index<2>())
    .element(emscripten::index<3>());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_VERTEX_PFACE_FACE, Dwg_Entity_VERTEX_PFACE_FACE)
    .property("flag", &Dwg_Entity_VERTEX_PFACE_FACE::flag);
    // .property("vertexIndex", &getDwgEntityVertexPfaceFaceVertInd, &setDwgEntityVertexPfaceFaceVertInd);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_POLYLINE_2D, Dwg_Entity_POLYLINE_2D)
    DEFINE_COMMON_ENTITY_POLYLINE(Dwg_Entity_POLYLINE_2D)
    .property("flag", &Dwg_Entity_POLYLINE_2D::flag)
    .property("curveType", &Dwg_Entity_POLYLINE_2D::curve_type)
    .property("startWidth", &Dwg_Entity_POLYLINE_2D::start_width)
    .property("endWidth", &Dwg_Entity_POLYLINE_2D::end_width)
    .property("thickness", &Dwg_Entity_POLYLINE_2D::thickness)
    .property("elevation", &Dwg_Entity_POLYLINE_2D::elevation)
    .property("extrusion", &Dwg_Entity_POLYLINE_2D::extrusion)
    .property("extraR11Size", &Dwg_Entity_POLYLINE_2D::extra_r11_size)
    .property("extraR11Text", &getDwgEntityPolyline2dExtraR11Text, &setDwgEntityPolyline2dExtraR11Text)
    .property("numMVerts", &Dwg_Entity_POLYLINE_2D::num_m_verts)
    .property("numNVerts", &Dwg_Entity_POLYLINE_2D::num_n_verts);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_POLYLINE_3D, Dwg_Entity_POLYLINE_3D)
    DEFINE_COMMON_ENTITY_POLYLINE(Dwg_Entity_POLYLINE_3D)
    .property("curveType", &Dwg_Entity_POLYLINE_3D::curve_type)
    .property("startWidth", &Dwg_Entity_POLYLINE_3D::start_width)
    .property("endWidth", &Dwg_Entity_POLYLINE_3D::end_width)
    .property("flag", &Dwg_Entity_POLYLINE_3D::flag)
    .property("extrusion", &Dwg_Entity_POLYLINE_3D::extrusion);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_ARC, Dwg_Entity_ARC)
    .property("center", &Dwg_Entity_ARC::center, return_value_policy::reference())
    .property("radius", &Dwg_Entity_ARC::radius)
    .property("thickness", &Dwg_Entity_ARC::thickness)
    .property("extrusion", &Dwg_Entity_ARC::extrusion)
    .property("startAngle", &Dwg_Entity_ARC::start_angle)
    .property("endAngle", &Dwg_Entity_ARC::end_angle);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_CIRCLE, Dwg_Entity_CIRCLE)
    .property("center", &Dwg_Entity_CIRCLE::center, return_value_policy::reference())
    .property("radius", &Dwg_Entity_CIRCLE::radius)
    .property("thickness", &Dwg_Entity_CIRCLE::thickness)
    .property("extrusion", &Dwg_Entity_CIRCLE::extrusion);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_LINE, Dwg_Entity_LINE)
    .property("zIsZero", &Dwg_Entity_LINE::z_is_zero)
    .property("start", &Dwg_Entity_LINE::start, return_value_policy::reference())
    .property("end", &Dwg_Entity_LINE::end, return_value_policy::reference())
    .property("thickness", &Dwg_Entity_LINE::thickness)
    .property("extrusion", &Dwg_Entity_LINE::extrusion)
    .property("unknownR11", &Dwg_Entity_LINE::unknown_r11, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_DIMENSION_common, Dwg_DIMENSION_common)
    DEFINE_DIMENSION_COMMON(Dwg_DIMENSION_common)
    .property("userText", &getDwgDimensionCommonUserText, &setDwgDimensionCommonUserText);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_DIMENSION_ORDINATE, Dwg_Entity_DIMENSION_ORDINATE)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_DIMENSION_ORDINATE)
    .property("userText", &getDwgEntityDimensionOrdinateUserText, &setDwgEntityDimensionOrdinateUserText)
    .property("featureLocationPoint", &Dwg_Entity_DIMENSION_ORDINATE::feature_location_pt, return_value_policy::reference())
    .property("leaderEndPoint", &Dwg_Entity_DIMENSION_ORDINATE::leader_endpt, return_value_policy::reference())
    .property("flag2", &Dwg_Entity_DIMENSION_ORDINATE::flag2);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_DIMENSION_LINEAR, Dwg_Entity_DIMENSION_LINEAR)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_DIMENSION_LINEAR)
    .property("userText", &getDwgEntityDimensionLinearUserText, &setDwgEntityDimensionLinearUserText)
    .property("xline1Point", &Dwg_Entity_DIMENSION_LINEAR::xline1_pt, return_value_policy::reference())
    .property("xline2Point", &Dwg_Entity_DIMENSION_LINEAR::xline2_pt, return_value_policy::reference())
    .property("obliqueAngle", &Dwg_Entity_DIMENSION_LINEAR::oblique_angle)
    .property("dimRotation", &Dwg_Entity_DIMENSION_LINEAR::dim_rotation);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_DIMENSION_ALIGNED, Dwg_Entity_DIMENSION_ALIGNED)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_DIMENSION_ALIGNED)
    .property("userText", &getDwgEntityDimensionAlignedUserText, &setDwgEntityDimensionAlignedUserText)
    .property("xline1Point", &Dwg_Entity_DIMENSION_ALIGNED::xline1_pt, return_value_policy::reference())
    .property("xline2Point", &Dwg_Entity_DIMENSION_ALIGNED::xline2_pt, return_value_policy::reference())
    .property("obliqueAngle", &Dwg_Entity_DIMENSION_ALIGNED::oblique_angle);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_DIMENSION_ANG3PT, Dwg_Entity_DIMENSION_ANG3PT)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_DIMENSION_ANG3PT)
    .property("userText", &getDwgEntityDimensionAng3PtUserText, &setDwgEntityDimensionAng3PtUserText)
    .property("xline1Point", &Dwg_Entity_DIMENSION_ANG3PT::xline1_pt, return_value_policy::reference())
    .property("xline2Point", &Dwg_Entity_DIMENSION_ANG3PT::xline2_pt, return_value_policy::reference())
    .property("centerPoint", &Dwg_Entity_DIMENSION_ANG3PT::center_pt, return_value_policy::reference())
    .property("xline2EndPoint", &Dwg_Entity_DIMENSION_ANG3PT::xline2end_pt, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_DIMENSION_ANG2LN, Dwg_Entity_DIMENSION_ANG2LN)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_DIMENSION_ANG2LN)
    .property("userText", &getDwgEntityDimensionAng2LnUserText, &setDwgEntityDimensionAng2LnUserText)
    .property("xline1StartPoint", &Dwg_Entity_DIMENSION_ANG2LN::xline1start_pt, return_value_policy::reference())
    .property("xline1EndPoint", &Dwg_Entity_DIMENSION_ANG2LN::xline1end_pt, return_value_policy::reference())
    .property("xline2StartPoint", &Dwg_Entity_DIMENSION_ANG2LN::xline2start_pt, return_value_policy::reference())
    .property("xline2EndPoint", &Dwg_Entity_DIMENSION_ANG2LN::xline2end_pt, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_DIMENSION_RADIUS, Dwg_Entity_DIMENSION_RADIUS)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_DIMENSION_RADIUS)
    .property("userText", &getDwgEntityDimensionRadiusUserText, &setDwgEntityDimensionRadiusUserText)
    .property("firstArcPoint", &Dwg_Entity_DIMENSION_RADIUS::first_arc_pt, return_value_policy::reference())
    .property("leaderLen", &Dwg_Entity_DIMENSION_RADIUS::leader_len);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_DIMENSION_DIAMETER, Dwg_Entity_DIMENSION_DIAMETER)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_DIMENSION_DIAMETER)
    .property("userText", &getDwgEntityDimensionDiameterUserText, &setDwgEntityDimensionDiameterUserText)
    .property("firstArcPoint", &Dwg_Entity_DIMENSION_DIAMETER::first_arc_pt, return_value_policy::reference())
    .property("leaderLen", &Dwg_Entity_DIMENSION_DIAMETER::leader_len);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_ARC_DIMENSION, Dwg_Entity_ARC_DIMENSION)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_ARC_DIMENSION)
    .property("userText", &getDwgEntityDimensionArcUserText, &setDwgEntityDimensionArcUserText)
    .property("xline1Point", &Dwg_Entity_ARC_DIMENSION::xline1_pt, return_value_policy::reference())
    .property("xline2Point", &Dwg_Entity_ARC_DIMENSION::xline2_pt, return_value_policy::reference())
    .property("centerPoint", &Dwg_Entity_ARC_DIMENSION::center_pt, return_value_policy::reference())
    .property("isPartial", &Dwg_Entity_ARC_DIMENSION::is_partial)
    .property("arcStartParam", &Dwg_Entity_ARC_DIMENSION::arc_start_param)
    .property("arcEndParam", &Dwg_Entity_ARC_DIMENSION::arc_end_param)
    .property("hasLeader", &Dwg_Entity_ARC_DIMENSION::has_leader)
    .property("leader1Point", &Dwg_Entity_ARC_DIMENSION::leader1_pt, return_value_policy::reference())
    .property("leader2Point", &Dwg_Entity_ARC_DIMENSION::leader2_pt, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_LARGE_RADIAL_DIMENSION, Dwg_Entity_LARGE_RADIAL_DIMENSION)
    DEFINE_DIMENSION_COMMON(Dwg_Entity_LARGE_RADIAL_DIMENSION)
    .property("userText", &getDwgEntityLargeRadialDimensionUserText, &setDwgEntityLargeRadialDimensionUserText)
    .property("firstArcPoint", &Dwg_Entity_LARGE_RADIAL_DIMENSION::first_arc_pt, return_value_policy::reference())
    .property("leaderLen", &Dwg_Entity_LARGE_RADIAL_DIMENSION::leader_len)
    .property("ovrCenter", &Dwg_Entity_LARGE_RADIAL_DIMENSION::ovr_center, return_value_policy::reference())
    .property("jogPoint", &Dwg_Entity_LARGE_RADIAL_DIMENSION::jog_point, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_POINT, Dwg_Entity_POINT)
    .property("x", &Dwg_Entity_POINT::x)
    .property("y", &Dwg_Entity_POINT::y)
    .property("z", &Dwg_Entity_POINT::z)
    .property("thickness", &Dwg_Entity_POINT::thickness)
    .property("extrusion", &Dwg_Entity_POINT::extrusion)
    .property("xAng", &Dwg_Entity_POINT::x_ang);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity__3DFACE, Dwg_Entity__3DFACE)
    .property("hasNoFlags", &Dwg_Entity__3DFACE::has_no_flags)
    .property("zIsZero", &Dwg_Entity__3DFACE::z_is_zero)
    .property("corner1", &Dwg_Entity__3DFACE::corner1, return_value_policy::reference())
    .property("corner2", &Dwg_Entity__3DFACE::corner2, return_value_policy::reference())
    .property("corner3", &Dwg_Entity__3DFACE::corner3, return_value_policy::reference())
    .property("corner4", &Dwg_Entity__3DFACE::corner4, return_value_policy::reference())
    .property("invisFlags", &Dwg_Entity__3DFACE::invis_flags);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_POLYLINE_PFACE, Dwg_Entity_POLYLINE_PFACE)
    .property("flag", &Dwg_Entity_POLYLINE_PFACE::flag)
    .property("numVerts", &Dwg_Entity_POLYLINE_PFACE::numverts)
    .property("numFaces", &Dwg_Entity_POLYLINE_PFACE::numfaces);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_POLYLINE_MESH, Dwg_Entity_POLYLINE_MESH)
    .property("flag", &Dwg_Entity_POLYLINE_MESH::flag)
    .property("curveType", &Dwg_Entity_POLYLINE_MESH::curve_type)
    .property("numMVerts", &Dwg_Entity_POLYLINE_MESH::num_m_verts)
    .property("numNVerts", &Dwg_Entity_POLYLINE_MESH::num_n_verts)
    .property("mDensity", &Dwg_Entity_POLYLINE_MESH::m_density)
    .property("nDensity", &Dwg_Entity_POLYLINE_MESH::n_density);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_SOLID, Dwg_Entity_SOLID)
    .property("thickness", &Dwg_Entity_SOLID::thickness)
    .property("elevation", &Dwg_Entity_SOLID::elevation)
    .property("corner1", &Dwg_Entity_SOLID::corner1, return_value_policy::reference())
    .property("corner2", &Dwg_Entity_SOLID::corner2, return_value_policy::reference())
    .property("corner3", &Dwg_Entity_SOLID::corner3, return_value_policy::reference())
    .property("corner4", &Dwg_Entity_SOLID::corner4, return_value_policy::reference())
    .property("extrusion", &Dwg_Entity_SOLID::extrusion);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_TRACE, Dwg_Entity_TRACE)
    .property("thickness", &Dwg_Entity_TRACE::thickness)
    .property("elevation", &Dwg_Entity_TRACE::elevation)
    .property("corner1", &Dwg_Entity_TRACE::corner1, return_value_policy::reference())
    .property("corner2", &Dwg_Entity_TRACE::corner2, return_value_policy::reference())
    .property("corner3", &Dwg_Entity_TRACE::corner3, return_value_policy::reference())
    .property("corner4", &Dwg_Entity_TRACE::corner4, return_value_policy::reference())
    .property("extrusion", &Dwg_Entity_TRACE::extrusion);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_SHAPE, Dwg_Entity_SHAPE)
    .property("insPoint", &Dwg_Entity_SHAPE::ins_pt, return_value_policy::reference())
    .property("scale", &Dwg_Entity_SHAPE::scale)
    .property("rotation", &Dwg_Entity_SHAPE::rotation)
    .property("widthFactor", &Dwg_Entity_SHAPE::width_factor)
    .property("obliqueAngle", &Dwg_Entity_SHAPE::oblique_angle)
    .property("thickness", &Dwg_Entity_SHAPE::thickness)
    .property("styleId", &Dwg_Entity_SHAPE::style_id)
    .property("extrusion", &Dwg_Entity_SHAPE::extrusion)
    .property("style", &Dwg_Entity_SHAPE::style, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_VIEWPORT, Dwg_Entity_VIEWPORT)
    .property("center", &Dwg_Entity_VIEWPORT::center, return_value_policy::reference())
    .property("width", &Dwg_Entity_VIEWPORT::width)
    .property("height", &Dwg_Entity_VIEWPORT::height)
    .property("onOff", &Dwg_Entity_VIEWPORT::on_off)
    .property("id", &Dwg_Entity_VIEWPORT::id)
    .property("viewTarget", &Dwg_Entity_VIEWPORT::view_target, return_value_policy::reference())
    .property("viewDir", &Dwg_Entity_VIEWPORT::VIEWDIR, return_value_policy::reference())
    .property("twistAngle", &Dwg_Entity_VIEWPORT::twist_angle)
    .property("viewSize", &Dwg_Entity_VIEWPORT::VIEWSIZE)
    .property("lensLength", &Dwg_Entity_VIEWPORT::lens_length)
    .property("frontClipZ", &Dwg_Entity_VIEWPORT::front_clip_z)
    .property("backClipZ", &Dwg_Entity_VIEWPORT::back_clip_z)
    .property("snapAng", &Dwg_Entity_VIEWPORT::SNAPANG)
    .property("viewCtr", &Dwg_Entity_VIEWPORT::VIEWCTR, return_value_policy::reference())
    .property("snapBase", &Dwg_Entity_VIEWPORT::SNAPBASE, return_value_policy::reference())
    .property("snapUnit", &Dwg_Entity_VIEWPORT::SNAPUNIT, return_value_policy::reference())
    .property("gridUnit", &Dwg_Entity_VIEWPORT::GRIDUNIT, return_value_policy::reference())
    .property("circleZoom", &Dwg_Entity_VIEWPORT::circle_zoom)
    .property("gridMajor", &Dwg_Entity_VIEWPORT::grid_major)
    .property("numFrozenLayers", &Dwg_Entity_VIEWPORT::num_frozen_layers)
    .property("statusFlag", &Dwg_Entity_VIEWPORT::status_flag)
    .property("styleSheet", &getDwgEntityViewportStyleSheet, &setDwgEntityViewportStyleSheet)
    .property("renderMode", &Dwg_Entity_VIEWPORT::render_mode)
    .property("ucsAtOrigin", &Dwg_Entity_VIEWPORT::ucs_at_origin)
    .property("ucsVp", &Dwg_Entity_VIEWPORT::UCSVP)
    .property("ucsOrg", &Dwg_Entity_VIEWPORT::ucsorg, return_value_policy::reference())
    .property("ucsXDir", &Dwg_Entity_VIEWPORT::ucsxdir, return_value_policy::reference())
    .property("ucsYDir", &Dwg_Entity_VIEWPORT::ucsydir, return_value_policy::reference())
    .property("ucsElevation", &Dwg_Entity_VIEWPORT::ucs_elevation)
    .property("ucsOrthoView", &Dwg_Entity_VIEWPORT::UCSORTHOVIEW)
    .property("shadePlotMode", &Dwg_Entity_VIEWPORT::shadeplot_mode)
    .property("useDefaultLights", &Dwg_Entity_VIEWPORT::use_default_lights)
    .property("defaultLightingType", &Dwg_Entity_VIEWPORT::default_lighting_type)
    .property("brightness", &Dwg_Entity_VIEWPORT::brightness)
    .property("contrast", &Dwg_Entity_VIEWPORT::contrast)
    .property("ambientColor", &Dwg_Entity_VIEWPORT::ambient_color)
    .property("vportEntityHeader", &Dwg_Entity_VIEWPORT::vport_entity_header, return_value_policy::reference())
    // TODO: Fix this binding laster
    // .property("frozenLayers", &Dwg_Entity_VIEWPORT::frozen_layers, return_value_policy::reference())
    .property("clipBoundary", &Dwg_Entity_VIEWPORT::clip_boundary, return_value_policy::reference())
    .property("namedUcs", &Dwg_Entity_VIEWPORT::named_ucs, return_value_policy::reference())
    .property("baseUcs", &Dwg_Entity_VIEWPORT::base_ucs, return_value_policy::reference())
    .property("background", &Dwg_Entity_VIEWPORT::background, return_value_policy::reference())
    .property("visualstyle", &Dwg_Entity_VIEWPORT::visualstyle, return_value_policy::reference())
    .property("shadeplot", &Dwg_Entity_VIEWPORT::shadeplot, return_value_policy::reference())
    .property("sun", &Dwg_Entity_VIEWPORT::sun, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_ELLIPSE, Dwg_Entity_ELLIPSE)
    .property("center", &Dwg_Entity_ELLIPSE::center, return_value_policy::reference())
    .property("majorAxis", &Dwg_Entity_ELLIPSE::sm_axis, return_value_policy::reference())
    .property("extrusion", &Dwg_Entity_ELLIPSE::extrusion)
    .property("axisRatio", &Dwg_Entity_ELLIPSE::axis_ratio)
    .property("startAngle", &Dwg_Entity_ELLIPSE::start_angle)
    .property("endAngle", &Dwg_Entity_ELLIPSE::end_angle);

  DEFINE_CLASS_WITH_PARENT(Dwg_SPLINE_control_point, Dwg_SPLINE_control_point)
    .property("x", &Dwg_SPLINE_control_point::x)
    .property("y", &Dwg_SPLINE_control_point::y)
    .property("z", &Dwg_SPLINE_control_point::z)
    .property("w", &Dwg_SPLINE_control_point::w);

  DEFINE_CLASS_WITH_PARENT(Dwg_Entity_SPLINE, Dwg_Entity_SPLINE)
    .property("flag", &Dwg_Entity_SPLINE::flag)
    .property("scenario", &Dwg_Entity_SPLINE::scenario)
    .property("degree", &Dwg_Entity_SPLINE::degree)
    .property("splineflags", &Dwg_Entity_SPLINE::splineflags)
    .property("knotparam", &Dwg_Entity_SPLINE::knotparam)
    .property("fitTol", &Dwg_Entity_SPLINE::fit_tol)
    .property("begTanVec", &Dwg_Entity_SPLINE::beg_tan_vec, return_value_policy::reference())
    .property("endTanVec", &Dwg_Entity_SPLINE::end_tan_vec, return_value_policy::reference())
    .property("closedB", &Dwg_Entity_SPLINE::closed_b)
    .property("periodic", &Dwg_Entity_SPLINE::periodic)
    .property("rational", &Dwg_Entity_SPLINE::rational)
    .property("weighted", &Dwg_Entity_SPLINE::weighted)
    .property("knotTol", &Dwg_Entity_SPLINE::knot_tol)
    .property("ctrlTol", &Dwg_Entity_SPLINE::ctrl_tol)
    .property("numFitPoints", &Dwg_Entity_SPLINE::num_fit_pts)
    // TODO: Fix this binding laster
    // .property("fitPoints", &Dwg_Entity_SPLINE::fit_pts, return_value_policy::reference())
    .property("numKnots", &Dwg_Entity_SPLINE::num_knots)
    // TODO: Fix this binding laster
    // .property("knots", &Dwg_Entity_SPLINE::knots, return_value_policy::reference())
    .property("numCtrlPoints", &Dwg_Entity_SPLINE::num_ctrl_pts)
    .property("ctrlPoints", &Dwg_Entity_SPLINE::ctrl_pts, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_3DSOLID_wire, Dwg_3DSOLID_wire)
    .property("type", &Dwg_3DSOLID_wire::type)
    .property("selectionMarker", &Dwg_3DSOLID_wire::selection_marker)
    .property("color", &Dwg_3DSOLID_wire::color)
    .property("acisIndex", &Dwg_3DSOLID_wire::acis_index)
    .property("numPoints", &Dwg_3DSOLID_wire::num_points)
    // TODO: Fix this binding laster
    // .property("points", &Dwg_3DSOLID_wire::points, return_value_policy::reference())
    .property("transformPresent", &Dwg_3DSOLID_wire::transform_present)
    .property("axisX", &Dwg_3DSOLID_wire::axis_x, return_value_policy::reference())
    .property("axisY", &Dwg_3DSOLID_wire::axis_y, return_value_policy::reference())
    .property("axisZ", &Dwg_3DSOLID_wire::axis_z, return_value_policy::reference())
    .property("translation", &Dwg_3DSOLID_wire::translation, return_value_policy::reference())
    .property("scale", &Dwg_3DSOLID_wire::scale, return_value_policy::reference())
    .property("hasRotation", &Dwg_3DSOLID_wire::has_rotation)
    .property("hasReflection", &Dwg_3DSOLID_wire::has_reflection)
    .property("hasShear", &Dwg_3DSOLID_wire::has_shear);

  DEFINE_CLASS_WITH_PARENT(Dwg_3DSOLID_silhouette, Dwg_3DSOLID_silhouette)
    .property("vpId", &Dwg_3DSOLID_silhouette::vp_id)
    .property("vpTarget", &Dwg_3DSOLID_silhouette::vp_target, return_value_policy::reference())
    .property("vpDirFromTarget", &Dwg_3DSOLID_silhouette::vp_dir_from_target, return_value_policy::reference())
    .property("vpUpDir", &Dwg_3DSOLID_silhouette::vp_up_dir, return_value_policy::reference())
    .property("vpPerspective", &Dwg_3DSOLID_silhouette::vp_perspective)
    .property("hasWires", &Dwg_3DSOLID_silhouette::has_wires)
    .property("numWires", &Dwg_3DSOLID_silhouette::num_wires);
    // TODO: Fix this binding laster
    // .property("wires", &Dwg_3DSOLID_silhouette::wires, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_3DSOLID_material, Dwg_3DSOLID_material)
    .property("arrayIndex", &Dwg_3DSOLID_material::array_index)
    .property("matAbsref", &Dwg_3DSOLID_material::mat_absref)
    .property("materialHandle", &Dwg_3DSOLID_material::material_handle, return_value_policy::reference());

  // DEFINE_CLASS_WITH_PARENT(Dwg_Entity_RAY, Dwg_Entity_RAY)
  //   .property("point", &Dwg_Entity_RAY::point, return_value_policy::reference())
  //   .property("vector", &Dwg_Entity_RAY::vector, return_value_policy::reference());

  // DEFINE_CLASS_WITH_PARENT(Dwg_Entity_XLINE, Dwg_Entity_XLINE)
  //   .property("point", &Dwg_Entity_XLINE::point, return_value_policy::reference())
  //   .property("vector", &Dwg_Entity_XLINE::vector, return_value_policy::reference());

  DEFINE_CLASS_WITH_PARENT(Dwg_Object_DICTIONARY, Dwg_Object_DICTIONARY)
    .property("numitems", &Dwg_Object_DICTIONARY::numitems)
    .property("isHardOwner", &Dwg_Object_DICTIONARY::is_hardowner)
    .property("cloning", &Dwg_Object_DICTIONARY::cloning);
    /* TODO: Double check whether this binding is correct */  
    // .property("texts", &Dwg_Object_DICTIONARY::texts, return_value_policy::reference())
    /* TODO: Double check whether this binding is correct */  
    // .property("itemHandles", &Dwg_Object_DICTIONARY::itemhandles, return_value_policy::reference());
    
  DEFINE_CLASS_WITH_PARENT(Dwg_Object_DICTIONARYWDFLT, Dwg_Object_DICTIONARYWDFLT)
    .property("numItems", &Dwg_Object_DICTIONARYWDFLT::numitems)
    .property("isHardOwner", &Dwg_Object_DICTIONARYWDFLT::is_hardowner)
    .property("cloning", &Dwg_Object_DICTIONARYWDFLT::cloning)
    // TODO: Double check whether this binding is correct
    // .property("texts", &Dwg_Object_DICTIONARYWDFLT::texts, return_value_policy::reference())
    // TODO: Double check whether this binding is correct
    // .property("itemHandles", &Dwg_Object_DICTIONARYWDFLT::itemhandles, return_value_policy::reference())
    .property("defaultId", &Dwg_Object_DICTIONARYWDFLT::defaultid, return_value_policy::reference());

  // class_<Dwg_Object_Entity>("Dwg_Object_Entity")
  //   .property("objId", &Dwg_Object_Entity::objid)
  //   .property("UNUSED", &getDwgObjectEntityTioUnused, &setDwgObjectEntityTioUnused, allow_raw_pointer<Dwg_Entity_UNUSED*>())
  //   .property("DIMENSION_common", &getDwgObjectEntityTioDimensionCommon, &setDwgObjectEntityTioDimensionCommon, allow_raw_pointer<Dwg_DIMENSION_common*>())
    // .property("_3DFACE", &Dwg_Object_Entity::tio._3DFACE)
    // .property("_3DSOLID", &Dwg_Object_Entity::tio._3DSOLID)
    // .property("ARC", &Dwg_Object_Entity::tio.ARC)
    // .property("ATTDEF", &Dwg_Object_Entity::tio.ATTDEF)
    // .property("ATTRIB", &Dwg_Object_Entity::tio.ATTRIB)
    // .property("BLOCK", &Dwg_Object_Entity::tio.BLOCK)
    // .property("BODY", &Dwg_Object_Entity::tio.BODY)
    // .property("CIRCLE", &Dwg_Object_Entity::tio.CIRCLE)
    // .property("DIMENSION_ALIGNED", &Dwg_Object_Entity::tio.DIMENSION_ALIGNED)
    // .property("DIMENSION_ANG2LN", &Dwg_Object_Entity::tio.DIMENSION_ANG2LN)
    // .property("DIMENSION_ANG3PT", &Dwg_Object_Entity::tio.DIMENSION_ANG3PT)
    // .property("DIMENSION_DIAMETER", &Dwg_Object_Entity::tio.DIMENSION_DIAMETER)
    // .property("DIMENSION_LINEAR", &Dwg_Object_Entity::tio.DIMENSION_LINEAR)
    // .property("DIMENSION_ORDINATE", &Dwg_Object_Entity::tio.DIMENSION_ORDINATE)
    // .property("DIMENSION_RADIUS", &Dwg_Object_Entity::tio.DIMENSION_RADIUS)
    // .property("ELLIPSE", &Dwg_Object_Entity::tio.ELLIPSE)
    // .property("ENDBLK", &Dwg_Object_Entity::tio.ENDBLK)
    // .property("INSERT", &Dwg_Object_Entity::tio.INSERT)
    // .property("LEADER", &Dwg_Object_Entity::tio.LEADER)
    // .property("LINE", &Dwg_Object_Entity::tio.LINE)
    // .property("LOAD", &Dwg_Object_Entity::tio.LOAD)
    // .property("MINSERT", &Dwg_Object_Entity::tio.MINSERT)
    // .property("MLINE", &Dwg_Object_Entity::tio.MLINE)
    // .property("MTEXT", &Dwg_Object_Entity::tio.MTEXT)
    // .property("OLEFRAME", &Dwg_Object_Entity::tio.OLEFRAME)
    // .property("POINT", &Dwg_Object_Entity::tio.POINT)
    // .property("POLYLINE_2D", &Dwg_Object_Entity::tio.POLYLINE_2D)
    // .property("POLYLINE_3D", &Dwg_Object_Entity::tio.POLYLINE_3D)
    // .property("POLYLINE_MESH", &Dwg_Object_Entity::tio.POLYLINE_MESH)
    // .property("POLYLINE_PFACE", &Dwg_Object_Entity::tio.POLYLINE_PFACE)
    // .property("PROXY_ENTITY", &Dwg_Object_Entity::tio.PROXY_ENTITY)
    // .property("RAY", &Dwg_Object_Entity::tio.RAY)
    // .property("REGION", &Dwg_Object_Entity::tio.REGION)
    // .property("SEQEND", &Dwg_Object_Entity::tio.SEQEND)
    // .property("SHAPE", &Dwg_Object_Entity::tio.SHAPE)
    // .property("SOLID", &Dwg_Object_Entity::tio.SOLID)
    // .property("SPLINE", &Dwg_Object_Entity::tio.SPLINE)
    // .property("TEXT", &Dwg_Object_Entity::tio.TEXT)
    // .property("TOLERANCE", &Dwg_Object_Entity::tio.TOLERANCE)
    // .property("TRACE", &Dwg_Object_Entity::tio.TRACE)
    // .property("UNKNOWN_ENT", &Dwg_Object_Entity::tio.UNKNOWN_ENT)
    // .property("VERTEX_2D", &Dwg_Object_Entity::tio.VERTEX_2D)
    // .property("VERTEX_3D", &Dwg_Object_Entity::tio.VERTEX_3D)
    // .property("VERTEX_MESH", &Dwg_Object_Entity::tio.VERTEX_MESH)
    // .property("VERTEX_PFACE", &Dwg_Object_Entity::tio.VERTEX_PFACE)
    // .property("VERTEX_PFACE_FACE", &Dwg_Object_Entity::tio.VERTEX_PFACE_FACE)
    // .property("VIEWPORT", &Dwg_Object_Entity::tio.VIEWPORT)
    // .property("XLINE", &Dwg_Object_Entity::tio.XLINE)
    // .property("_3DLINE", &Dwg_Object_Entity::tio._3DLINE)
    // .property("CAMERA", &Dwg_Object_Entity::tio.CAMERA)
    // .property("DGNUNDERLAY", &Dwg_Object_Entity::tio.DGNUNDERLAY)
    // .property("DWFUNDERLAY", &Dwg_Object_Entity::tio.DWFUNDERLAY)
    // .property("ENDREP", &Dwg_Object_Entity::tio.ENDREP)
    // .property("HATCH", &Dwg_Object_Entity::tio.HATCH)
    // .property("IMAGE", &Dwg_Object_Entity::tio.IMAGE)
    // .property("JUMP", &Dwg_Object_Entity::tio.JUMP)
    // .property("LIGHT", &Dwg_Object_Entity::tio.LIGHT)
    // .property("LWPOLYLINE", &Dwg_Object_Entity::tio.LWPOLYLINE)
    // .property("MESH", &Dwg_Object_Entity::tio.MESH)
    // .property("MULTILEADER", &Dwg_Object_Entity::tio.MULTILEADER)
    // .property("OLE2FRAME", &Dwg_Object_Entity::tio.OLE2FRAME)
    // .property("PDFUNDERLAY", &Dwg_Object_Entity::tio.PDFUNDERLAY)
    // .property("REPEAT", &Dwg_Object_Entity::tio.REPEAT)
    // .property("SECTIONOBJECT", &Dwg_Object_Entity::tio.SECTIONOBJECT)
    // .property("WIPEOUT", &Dwg_Object_Entity::tio.WIPEOUT)
    // .property("ARC_DIMENSION", &Dwg_Object_Entity::tio.ARC_DIMENSION)
    // .property("HELIX", &Dwg_Object_Entity::tio.HELIX)
    // .property("LARGE_RADIAL_DIMENSION", &Dwg_Object_Entity::tio.LARGE_RADIAL_DIMENSION)
    // .property("LAYOUTPRINTCONFIG", &Dwg_Object_Entity::tio.LAYOUTPRINTCONFIG)
    // .property("PLANESURFACE", &Dwg_Object_Entity::tio.PLANESURFACE)
    // .property("POINTCLOUD", &Dwg_Object_Entity::tio.POINTCLOUD)
    // .property("POINTCLOUDEX", &Dwg_Object_Entity::tio.POINTCLOUDEX)
    // .property("ALIGNMENTPARAMETERENTITY", &Dwg_Object_Entity::tio.ALIGNMENTPARAMETERENTITY)
    // .property("ARCALIGNEDTEXT", &Dwg_Object_Entity::tio.ARCALIGNEDTEXT)
    // .property("BASEPOINTPARAMETERENTITY", &Dwg_Object_Entity::tio.BASEPOINTPARAMETERENTITY)
    // .property("EXTRUDEDSURFACE", &Dwg_Object_Entity::tio.EXTRUDEDSURFACE)
    // .property("FLIPGRIPENTITY", &Dwg_Object_Entity::tio.FLIPGRIPENTITY)
    // .property("FLIPPARAMETERENTITY", &Dwg_Object_Entity::tio.FLIPPARAMETERENTITY)
    // .property("GEOPOSITIONMARKER", &Dwg_Object_Entity::tio.GEOPOSITIONMARKER)
    // .property("LINEARGRIPENTITY", &Dwg_Object_Entity::tio.LINEARGRIPENTITY)
    // .property("LINEARPARAMETERENTITY", &Dwg_Object_Entity::tio.LINEARPARAMETERENTITY)
    // .property("LOFTEDSURFACE", &Dwg_Object_Entity::tio.LOFTEDSURFACE)
    // .property("MPOLYGON", &Dwg_Object_Entity::tio.MPOLYGON)
    // .property("NAVISWORKSMODEL", &Dwg_Object_Entity::tio.NAVISWORKSMODEL)
    // .property("NURBSURFACE", &Dwg_Object_Entity::tio.NURBSURFACE)
    // .property("POINTPARAMETERENTITY", &Dwg_Object_Entity::tio.POINTPARAMETERENTITY)
    // .property("POLARGRIPENTITY", &Dwg_Object_Entity::tio.POLARGRIPENTITY)
    // .property("REVOLVEDSURFACE", &Dwg_Object_Entity::tio.REVOLVEDSURFACE)
    // .property("ROTATIONGRIPENTITY", &Dwg_Object_Entity::tio.ROTATIONGRIPENTITY)
    // .property("ROTATIONPARAMETERENTITY", &Dwg_Object_Entity::tio.ROTATIONPARAMETERENTITY)
    // .property("RTEXT", &Dwg_Object_Entity::tio.RTEXT)
    // .property("SWEPTSURFACE", &Dwg_Object_Entity::tio.SWEPTSURFACE)
    // .property("TABLE", &Dwg_Object_Entity::tio.TABLE)
    // .property("VISIBILITYGRIPENTITY", &Dwg_Object_Entity::tio.VISIBILITYGRIPENTITY)
    // .property("VISIBILITYPARAMETERENTITY", &Dwg_Object_Entity::tio.VISIBILITYPARAMETERENTITY)
    // .property("XYGRIPENTITY", &Dwg_Object_Entity::tio.XYGRIPENTITY)
    // .property("XYPARAMETERENTITY", &Dwg_Object_Entity::tio.XYPARAMETERENTITY)
    // // Add other properties for the struct members
    // .property("preview_exists", &Dwg_Object_Entity::preview_exists)
    // .property("preview_is_proxy", &Dwg_Object_Entity::preview_is_proxy)
    // .property("preview_size", &Dwg_Object_Entity::preview_size)
    // .property("preview", &Dwg_Object_Entity::preview)
    // .property("entmode", &Dwg_Object_Entity::entmode)
    // .property("num_reactors", &Dwg_Object_Entity::num_reactors)
    // .property("reactors", &Dwg_Object_Entity::reactors)
    // .property("xdicobjhandle", &Dwg_Object_Entity::xdicobjhandle)
    // .property("is_xdic_missing", &Dwg_Object_Entity::is_xdic_missing)
    // .property("has_ds_data", &Dwg_Object_Entity::has_ds_data)
    // .property("color", &Dwg_Object_Entity::color)
    // .property("ltype_scale", &Dwg_Object_Entity::ltype_scale)
    // .property("ltype_flags", &Dwg_Object_Entity::ltype_flags)
    // .property("plotstyle_flags", &Dwg_Object_Entity::plotstyle_flags)
    // .property("material_flags", &Dwg_Object_Entity::material_flags)
    // .property("shadow_flags", &Dwg_Object_Entity::shadow_flags)
    // .property("has_full_visualstyle", &Dwg_Object_Entity::has_full_visualstyle)
    // .property("has_face_visualstyle", &Dwg_Object_Entity::has_face_visualstyle)
    // .property("has_edge_visualstyle", &Dwg_Object_Entity::has_edge_visualstyle)
    // .property("invisible", &Dwg_Object_Entity::invisible)
    // .property("linewt", &Dwg_Object_Entity::linewt)
    // .property("flag_r11", &Dwg_Object_Entity::flag_r11)
    // .property("opts_r11", &Dwg_Object_Entity::opts_r11)
    // .property("extra_r11", &Dwg_Object_Entity::extra_r11)
    // .property("color_r11", &Dwg_Object_Entity::color_r11)
    // .property("elevation_r11", &Dwg_Object_Entity::elevation_r11)
    // .property("thickness_r11", &Dwg_Object_Entity::thickness_r11)
    // .property("viewport", &Dwg_Object_Entity::viewport)
    // .property("__iterator", &Dwg_Object_Entity::__iterator)
    // .property("ownerhandle", &Dwg_Object_Entity::ownerhandle)
    // .property("prev_entity", &Dwg_Object_Entity::prev_entity)
    // .property("next_entity", &Dwg_Object_Entity::next_entity)
    // .property("layer", &Dwg_Object_Entity::layer)
    // .property("ltype", &Dwg_Object_Entity::ltype)
    // .property("material", &Dwg_Object_Entity::material)
    // .property("shadow", &Dwg_Object_Entity::shadow)
    // .property("plotstyle", &Dwg_Object_Entity::plotstyle)
    // .property("full_visualstyle", &Dwg_Object_Entity::full_visualstyle)
    // .property("face_visualstyle", &Dwg_Object_Entity::face_visualstyle)
    // .property("edge_visualstyle", &Dwg_Object_Entity::edge_visualstyle);
    ;
    
  class_<Dwg_Data>("Dwg_Data")
    .constructor<>()
    // .property("header", &Dwg_Data::header)
    .property("numClasses", &Dwg_Data::num_classes)
    // .property("dwgClass", &Dwg_Data::dwg_class, return_value_policy::reference())
    .property("numObjects", &Dwg_Data::num_objects)
    .property("numAllocedObjects", &Dwg_Data::num_alloced_objects)
    // .property("object", &Dwg_Data::object, return_value_policy::reference())
    .property("numEntities", &Dwg_Data::num_entities)
    .property("numObjectRefs", &Dwg_Data::num_object_refs)
    .property("curIndex", &Dwg_Data::cur_index);
}

/*--------------------------------------------------
 * Exported Functions
 *--------------------------------------------------*/
EXPORT int dwg_read(emscripten::val jsBuffer, Dwg_Data* dwg) {
  // Get the byte length of the ArrayBuffer
  auto length = jsBuffer["byteLength"].as<unsigned>();
  std::cerr << "Data: length: " << length << std::endl;

  // Create a Uint8Array view of the JavaScript buffer
  emscripten::val memoryView = emscripten::val::global("Uint8Array").new_(jsBuffer);

  // Output the first 10 bytes of jsBuffer
  std::cerr << "First 10 bytes of jsBuffer: ";
  for (unsigned i = 0; i < (length < 10 ? length : 10); ++i) {
      std::cerr << static_cast<int>(memoryView[i].as<unsigned char>()) << " ";
  }

  // Copy data from Uint8Array to C++
  std::vector<uint8_t> data(length + 1);
  for (int i = 0; i < length; ++i) {
    data[i] = memoryView[i].as<uint8_t>();
  }

  // ensure zero-termination for strstr, strtol, ...
  data[length] = '\0';

  // Output the first 10 bytes of copied C++ memory
  std::cerr << "First 10 bytes of copied C++ memory: ";
  for (unsigned i = 0; i < (length < 10 ? length : 10); ++i) {
      std::cerr << static_cast<int>(data[i]) << " ";
  }
  std::cerr << std::endl;

  return dwg_read_data(data.data(), length, dwg);
}

EXPORT uintptr_t dwg_read_file_ex(const std::string& fileName) {
  Dwg_Data* dwg = new Dwg_Data(); 
  dwg_read_file(fileName.c_str(), dwg);
  std::cerr << "the original pointer address: " << reinterpret_cast<uintptr_t>(dwg) << std::endl;
  std::cerr << "num objects: " << dwg->num_objects << std::endl;
  return reinterpret_cast<uintptr_t>(dwg);
}

EXPORT int dwg_num_objects(uintptr_t ptr) {
  Dwg_Data* dwg = reinterpret_cast<Dwg_Data*>(ptr);
  std::cerr << "passed pointer address: " << reinterpret_cast<uintptr_t>(dwg) << std::endl;
  std::cerr << "num objects: " << dwg->num_objects << std::endl;
  return dwg->num_objects;
}

EMSCRIPTEN_BINDINGS(libredwg_functions) {
  function("dwg_read", &dwg_read, emscripten::allow_raw_pointers());
  function("dwg_read_file", &dwg_read_file_ex, emscripten::allow_raw_pointers());
  function("dwg_num_objects", &dwg_num_objects, emscripten::allow_raw_pointers());
}
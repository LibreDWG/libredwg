project(libredwg C)
cmake_minimum_required(VERSION 2.8)
# Supported options -DLIBREDWG_LIBONLY=On
#                   -DLIBREDWG_DISABLE_WRITE=On
#                   -DLIBREDWG_DISABLE_JSON=On
# for smaller builds and lib.

if(MSVC)
    # Disable some overly strict MSVC warnings.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -wd4244 -wd4800 -wd4805")
endif()

if (EXISTS ".version")
    file(READ .version NL_PACKAGE_VERSION)
else()
    find_package(Git)
    set(PACKAGE_VERSION "")
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --long --tags --always
                    OUTPUT_VARIABLE NL_PACKAGE_VERSION)
endif()
string(STRIP "${NL_PACKAGE_VERSION}" PACKAGE_VERSION)

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILE("strings.h" HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("libgen.h" HAVE_LIBGEN_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("ctype.h" HAVE_CTYPE_H)
CHECK_INCLUDE_FILE("wchar.h" HAVE_WCHAR_H)
CHECK_INCLUDE_FILE("alloca.h" HAVE_ALLOCA_H)
#CHECK_INCLUDE_FILE("getopt.h" HAVE_GETOPT_H)
CHECK_INCLUDE_FILE("malloc.h" HAVE_MALLOC_H)
CHECK_INCLUDE_FILE("pcre2.h" HAVE_PCRE2_H)
include(CheckTypeSize)
check_type_size(size_t SIZE_T)
check_type_size(wchar_t WCHAR_T)
if (WCHAR_T EQUAL 2)
  set(HAVE_PCRE2_16 1)
endif()
#include(CheckCCompilerFlag)
configure_file(src/cmakeconfig.h.in src/config.h)
if(CMAKE_MAJOR_VERSION GREATER 2)
  include(TestBigEndian)
  test_big_endian(IS_BIG_ENDIAN)
  if(IS_BIG_ENDIAN)
    message(ERROR "Big Endian not supported: <${error}>")
    add_definitions(-DBIG_ENDIAN)
  endif()
endif()

option(BUILD_SHARED_LIBS "shared libredwg library" ON)
option(LIBREDWG_LIBONLY "only the libredwg library" OFF)
option(LIBREDWG_DISABLE_WRITE "no libredwg write support" OFF)
option(LIBREDWG_DISABLE_JSON "no libredwg json support" OFF)
# Rather disable installing a crippled shared lib.
# Only use these for static in-tree libs.
if (LIBREDWG_DISABLE_JSON)
  set(BUILD_SHARED_LIBS OFF)
endif()
if (LIBREDWG_DISABLE_WRITE)
  set(BUILD_SHARED_LIBS OFF)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release"
      CACHE STRING "Choose the type of build, options are: Debug Release
  RelWithDebInfo MinSizeRel Asan." FORCE)
endif()

if((CMAKE_MAJOR_VERSION EQUAL 3 AND CMAKE_MINOR_VERSION GREATER_EQUAL 9)
   AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
  cmake_policy(SET CMP0069 NEW)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT ipo_supported OUTPUT error)
endif()

set(libredwg_HEADERS
    include/dwg.h
    include/dwg_api.h)

if(NOT LIBREDWG_DISABLE_JSON)
  set(outjson_SOURCES
    src/out_json.c
    src/out_geojson.c)
  set(injson_SOURCES
    src/in_json.c)
endif()

if(NOT LIBREDWG_DISABLE_WRITE)
 set(write_SOURCES
    src/encode.c
    src/in_dxf.c
    ${injson_SOURCES}
    src/reedsolomon.c)
endif()

set(libredwg_SOURCES
    src/bits.c
    src/common.c
    src/classes.c
    src/objects.c
    src/decode.c
    src/decode_r2007.c
    src/dwg.c
    src/hash.c
    src/dwg_api.c
    src/dynapi.c
    src/dxfclasses.c
    src/free.c
    src/out_dxf.c
    ${outjson_SOURCES}
    src/out_dxfb.c
    ${write_SOURCES}
    src/print.c)

add_library(redwg
    ${libredwg_HEADERS}
    ${libredwg_SOURCES})
#set_target_properties(${libredwg_HEADERS} PROPERTIES PUBLIC_HEADER)
set_target_properties(redwg PROPERTIES PUBLIC_HEADER include/dwg_api.h)
set_target_properties(redwg PROPERTIES PUBLIC_HEADER include/dwg.h)
target_include_directories(redwg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src)
target_include_directories(redwg PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

link_libraries(redwg -lm ${CMAKE_THREAD_LIBS_INIT})

if(NOT LIBREDWG_LIBONLY)

  add_executable(dwgread programs/dwgread.c)
  add_executable(dwg2dxf programs/dwg2dxf.c)
  if(NOT LIBREDWG_DISABLE_WRITE)
    add_executable(dwgrewrite programs/dwgrewrite.c)
    add_executable(dwgwrite programs/dwgwrite.c)
    add_executable(dxf2dwg programs/dxf2dwg.c)
  endif(NOT LIBREDWG_DISABLE_WRITE)
  add_executable(dwggrep programs/dwggrep.c)
  add_executable(dwglayers programs/dwglayers.c)
  add_executable(dwgbmp programs/dwgbmp.c)
  add_executable(dwg2SVG programs/dwg2SVG.c
                         programs/escape.c programs/geom.c)

  if (HAVE_PCRE2_H)
    if(HAVE_PCRE2_16)
      target_link_libraries(dwggrep redwg -lpcre2-8 -lpcre2-16)
    else()
      target_link_libraries(dwggrep redwg -lpcre2-8)
    endif(HAVE_PCRE2_16)
    #target_include_directories(dwggrep PUBLIC pcre2)
  endif(HAVE_PCRE2_H)

endif(NOT LIBREDWG_LIBONLY)

if(ipo_supported)
  message(STATUS "IPO / LTO enabled")
  set_property(GLOBAL PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLTO")
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()

include_directories(BEFORE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}/src)

if(NOT CMAKE_CROSSCOMPILING)
  enable_testing()
  if(NOT LIBREDWG_LIBONLY)
    set(top_builddir ${CMAKE_CURRENT_BINARY_DIR})
    set(top_srcdir ${CMAKE_CURRENT_SOURCE_DIR})
    set(CFLAGS ${CMAKE_C_FLAGS})
    set(SED "sed")
    set(JSON_SED_NAN "true")
    set(JQ "jq")
    set(RUN_JING "true")
    if (LIBREDWG_DISABLE_JSON)
      set(DISABLE_JSON 1)
    else()
      set(DISABLE_JSON )
    endif()
    configure_file(programs/alive.test.in alive.test @ONLY)
    add_test(alive.test alive.test)
    if(NOT LIBREDWG_DISABLE_DXF)
      configure_file(programs/dxf.test.in dxf.test @ONLY)
      add_test(dxf.test dxf.test)
      if(NOT LIBREDWG_DISABLE_JSON)
        configure_file(programs/json.test.in json.test @ONLY)
        add_test(json.test json.test)
      endif()
    endif()
    configure_file(programs/svg.test.in svg.test @ONLY)
    add_test(svg.test svg.test)
  endif(NOT LIBREDWG_LIBONLY)
  add_subdirectory(test/unit-testing)
endif(NOT CMAKE_CROSSCOMPILING)

add_custom_target(
  regen-dynapi
  COMMAND perl src/gen-dynapi.pl
  DEPENDS src/gen-dynapi.pl include/dwg.h)

add_custom_target(
  regen-gperf
  COMMAND gperf --output-file src/dxfclasses.c src/dxfclasses.in
  DEPENDS src/dxfclasses.in
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_target(
  TAGS
  COMMAND etags --language=c++ *.c *.h
  DEPENDS ${SRCS}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

if(MSVC)
  #untested
  install(TARGETS redwg RUNTIME PUBLIC_HEADER
          DESTINATION libredwg-${PACKAGE_VERSION})
else()
  include(GNUInstallDirs)
endif()
install(TARGETS redwg
  LIBRARY
  COMPONENT redwg)
install(TARGETS RUNTIME)

if(MSVC)
  #untested
  add_custom_target(dist
    COMMAND zip libredwg-${PACKAGE_VERSION}.zip dwg*.exe dxf*.exe libredwg-*.dll
    DEPENDS redwg)
endif()

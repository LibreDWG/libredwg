cmake_minimum_required(VERSION 2.8...3.27)
project(libredwg C)
# Supported options -DLIBREDWG_LIBONLY=On
#                   -DLIBREDWG_DISABLE_WRITE=On
#                   -DLIBREDWG_DISABLE_JSON=On
#                   -DLIBREDWG_DISABLE_DXF=On
#                   -DLIBREDWG_SKIP_INVALID_DXF=On
#                   -DENABLE_MIMALLOC=On
#                   -DENABLE_LTO=Off
#                   -DDXF_PRECISION=6
#                   -DGEOMJSON_PRECISION=6
# for smaller builds and lib.

if(MSVC)
  set(redwg libredwg)
  # Disable some overly strict MSVC warnings.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -wd4244 -wd4800 -wd4805 -wd4101 -D_CRT_NONSTDC_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS")
else()
  set(redwg redwg)
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
message(STATUS "PACKAGE_VERSION: ${PACKAGE_VERSION}")

set(LIBREDWG_SO_VERSION "0:13:0")
execute_process(COMMAND perl -an "${CMAKE_CURRENT_SOURCE_DIR}/build-aux/get-so-version.pl"
                        "${CMAKE_CURRENT_SOURCE_DIR}/configure.ac"
                OUTPUT_VARIABLE LIBREDWG_SO_VERSION)
message(STATUS "LIBREDWG_SO_VERSION: ${LIBREDWG_SO_VERSION}")

option(BUILD_SHARED_LIBS "shared libredwg library" ON)
option(LIBREDWG_LIBONLY "only the libredwg library" OFF)
option(LIBREDWG_DISABLE_WRITE "no libredwg write support" OFF)
option(DISABLE_WERROR "no -Werror" OFF)
option(ENABLE_LTO "IPO/LTO Link Time Optimizations (default ON)" ON)
# FIXME enable
# option(LIBREDWG_DISABLE_BINDINGS "no libredwg perl and python bindings" ON)
option(LIBREDWG_DISABLE_JSON "no libredwg json support" OFF)
option(LIBREDWG_DISABLE_DXF "no libredwg dxf support" OFF)
option(LIBREDWG_SKIP_INVALID_DXF "skip invalid DXF object/entities and continue" OFF)
# Rather disable installing a crippled shared lib.
# Only use these for static in-tree libs.
if (LIBREDWG_DISABLE_JSON)
  set(BUILD_SHARED_LIBS OFF)
endif()
if (LIBREDWG_DISABLE_WRITE)
  set(BUILD_SHARED_LIBS OFF)
endif()
option(DXF_PRECISION "number of DXF after-comma places" 16)
option(GEOJSON_PRECISION "number of GeoJSON after-comma places (6 recoomended by RFC)" 16)

include(CheckIncludeFile)
CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILE("strings.h" HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("libgen.h" HAVE_LIBGEN_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("float.h" HAVE_FLOAT_H)
CHECK_INCLUDE_FILE("ctype.h" HAVE_CTYPE_H)
CHECK_INCLUDE_FILE("wchar.h" HAVE_WCHAR_H)
CHECK_INCLUDE_FILE("wctype.h" HAVE_WCTYPE_H)
CHECK_INCLUDE_FILE("alloca.h" HAVE_ALLOCA_H)
CHECK_INCLUDE_FILE("getopt.h" HAVE_GETOPT_H)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("limits.h" HAVE_LIMITS_H)
CHECK_INCLUDE_FILE("malloc.h" HAVE_MALLOC_H)
CHECK_INCLUDE_FILE("memory.h" HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("dirent.h" HAVE_DIRENT_H)
CHECK_INCLUDE_FILE("endian.h" HAVE_ENDIAN_H)
CHECK_INCLUDE_FILE("sys/endian.h" HAVE_SYS_ENDIAN_H)
CHECK_INCLUDE_FILE("byteorder.h" HAVE_BYTEORDER_H)
CHECK_INCLUDE_FILE("sys/byteorder.h" HAVE_SYS_BYTEORDER_H)
CHECK_INCLUDE_FILE("byteswap.h" HAVE_BYTESWAP_H)
if (WIN32)
  CHECK_INCLUDE_FILE("winsock2.h" HAVE_WINSOCK2_H)
elseif (APPLE)
  CHECK_INCLUDE_FILE("machine/endian.h" HAVE_MACHINE_ENDIAN_H)
endif()
CHECK_INCLUDE_FILE("iconv.h" HAVE_ICONV_H)
CHECK_INCLUDE_FILE("pcre2.h" HAVE_PCRE2_H)
CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("direct.h" HAVE_DIRECT_H)
CHECK_INCLUDE_FILE("sys/param.h" HAVE_SYS_PARAM_H)
CHECK_INCLUDE_FILE("sys/time.h" HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("libps/pslib.h" HAVE_LIBPS_PSLIB_H)
CHECK_INCLUDE_FILE("mimalloc-override.h" HAVE_MIMALLOC_OVERRIDE_H)
include(CheckSymbolExists)
check_symbol_exists("floor" "math.h" HAVE_FLOOR)
check_symbol_exists("gettimeofday" "sys/time.h" HAVE_GETTIMEOFDAY)
check_symbol_exists("memchr" "string.h" HAVE_MEMCHR)
check_symbol_exists("memmem" "string.h" HAVE_MEMMEM)
check_symbol_exists("memmove" "string.h" HAVE_MEMMOVE)
check_symbol_exists("realloc" "stdlib.h" HAVE_REALLOC)
check_symbol_exists("scandir" "dirent.h" HAVE_SCANDIR)
check_symbol_exists("setenv" "stdlib.h" HAVE_SETENV)
check_symbol_exists("strcasecmp" "strings.h" HAVE_STRCASECMP)
check_symbol_exists("strcasestr" "string.h" HAVE_STRCASESTR)
check_symbol_exists("strchr" "string.h" HAVE_STRCHR)
check_symbol_exists("strnlen" "string.h" HAVE_STRNLEN)
check_symbol_exists("strrchr" "string.h" HAVE_STRRCHR)
check_symbol_exists("strtol" "stdlib.h" HAVE_STRTOL)
check_symbol_exists("strtoul" "stdlib.h" HAVE_STRTOUL)
check_symbol_exists("strtoull" "stdlib.h" HAVE_STRTOULL)
check_symbol_exists("wcscmp" "wchar.h" HAVE_WCSCMP)
check_symbol_exists("wcscpy" "wchar.h" HAVE_WCSCPY)
check_symbol_exists("wcslen" "wchar.h" HAVE_WCSLEN)
check_symbol_exists("wcsnlen" "wchar.h" HAVE_WCSNLEN)
include(CheckTypeSize)
check_type_size(size_t SIZE_T)
check_type_size(wchar_t WCHAR_T)
if (NOT SIZE_T)
  message(WARNING "SIZE_T detection failed, assuming 8")
  set(SIZE_T 8)
endif()
if (NOT WCHAR_T)
  message(WARNING "WCHAR_T detection failed, assuming 2")
  set(WCHAR_T 2)
endif()
if (WCHAR_T EQUAL 2)
  set(HAVE_PCRE2_16 1)
endif()

if (NOT HAVE_MIMALLOC_OVERRIDE_H)
  if (ENABLE_MIMALLOC)
    message(WARNING "Should not ENABLE_MIMALLOC without mimalloc-override.h")
  endif()
endif()
if (NOT MSVC)
  SET(LIBS "-lm")
endif()
if (ENABLE_MIMALLOC)
  SET(LIBS "${LIBS} -lmimalloc")
endif()
FIND_PACKAGE(Iconv)
if (Iconv_FOUND)
  SET(HAVE_ICONV TRUE)
  SET(HAVE_ICONV_H TRUE)
  if (NOT "${Iconv_LIBRARY}" STREQUAL "")
    message(STATUS "add ${Iconv_LIBRARY}")
    link_libraries(${Iconv_LIBRARY})
  endif()
  if (NOT "${Iconv_INCLUDE_DIR}" STREQUAL "")
    message(STATUS "add -I\"${Iconv_INCLUDE_DIR}\"")
    include_directories(${Iconv_INCLUDE_DIR})
  endif()
else()
  message(STATUS "iconv not used")
endif()
if(CMAKE_MAJOR_VERSION GREATER 2)
  if (NOT MSVC)
    include(TestBigEndian)
    test_big_endian(IS_BIG_ENDIAN)
    if(IS_BIG_ENDIAN)
      message(ERROR "Big Endian not supported: <${error}>")
      add_definitions(-DWORDS_BIGENDIAN)
    endif()
  endif()
endif()

if (MSVC)
  add_compile_options(/W3 /utf-8)
else()
  add_compile_options(-W -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter
      -Wno-unused-but-set-variable -Wcast-align -Wmissing-field-initializers)
  if (CMAKE_COMPILER_IS_GNUC)
    add_compile_options(-fno-semantic-interposition -fwrapv -fno-common
                        -fvisibility=hidden -fno-strict-aliasing)
  endif()
  if (${CMAKE_VERSION} VERSION_GREATER "3.18.0")
    include(CheckCompilerFlag)
    check_compiler_flag(C -ftrivial-auto-var-init=zero HAVE_C_FTRIVIAL_AUTO_VAR_INIT)
    check_compiler_flag(C -fstack-protector-strong HAVE_C_FSTACK_PROTECTOR_STRONG)
    check_compiler_flag(C -fstack-clash-protection HAVE_C_FSTACK_CLASH_PROTECTION)
    check_compiler_flag(C -fcf-protection=full HAVE_C_FCF_PROTECTION)
    check_compiler_flag(C -fno-delete-null-pointer-checks HAVE_C_FNO_DELETE_NULL_POINTER_CHECKS)
    check_compiler_flag(C -Warray-bounds HAVE_C_WARRAY_BOUNDS)
    check_compiler_flag(C -Walloc-size HAVE_C_WALLOC_SIZE)
    if (HAVE_C_FTRIVIAL_AUTO_VAR_INIT)
      add_compile_options(-ftrivial-auto-var-init=zero)
      add_link_options(-ftrivial-auto-var-init=zero)
    endif()
    if (HAVE_C_FSTACK_PROTECTOR_STRONG)
      add_compile_options(-fstack-protector-strong)
      add_link_options(-fstack-protector-strong)
    endif()
    if (HAVE_C_FSTACK_CLASH_PROTECTION)
      add_compile_options(-fstack-clash-protection)
      add_link_options(-fstack-clash-protection)
    endif()
    if (HAVE_C_FCF_PROTECTION)
      add_compile_options(-fcf-protection=full)
      add_link_options(-fcf-protection=full)
    endif()
    if (HAVE_C_FNO_DELETE_NULL_POINTER_CHECKS)
      add_compile_options(-fno-delete-null-pointer-checks)
      add_link_options(-fno-delete-null-pointer-checks)
    endif()
    if (HAVE_C_WARRAY_BOUNDS)
      add_compile_options(-Wno-error=array-bounds)
    endif()
    if (HAVE_C_WALLOC_SIZE)
      add_compile_options(-Wno-alloc-size)
    endif()
  endif()
endif()
if (NOT DISABLE_WERROR)
  if (MSVC)
    add_compile_options(/WX)
  else()
    add_compile_options(-Werror -Wno-error=cast-align)
  endif()
endif()

configure_file(src/cmakeconfig.h.in src/config.h)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release"
      CACHE STRING "Choose the type of build, options are: Debug Release
  RelWithDebInfo MinSizeRel Asan." FORCE)
endif()

if((CMAKE_MAJOR_VERSION EQUAL 3 AND CMAKE_MINOR_VERSION GREATER_EQUAL 9)
   AND (CMAKE_BUILD_TYPE STREQUAL "Release") AND (NOT MSVC) AND (ENABLE_LTO))
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
    src/codepages.c
    src/common.c
    src/classes.c
    src/objects.c
    src/decode.c
    src/decode_r11.c
    src/decode_r2007.c
    src/dwg.c
    src/hash.c
    src/dwg_api.c
    src/dynapi.c
    src/dxfclasses.c
    src/free.c
    src/geom.c
    src/out_dxf.c
    ${outjson_SOURCES}
    src/out_dxfb.c
    ${write_SOURCES}
    src/print.c)

add_library(${redwg}
    ${libredwg_HEADERS}
    ${libredwg_SOURCES})

set_property(TARGET ${redwg} PROPERTY C_STANDARD 99)
set_property(TARGET ${redwg} PROPERTY C_STANDARD_REQUIRED ON)

set_target_properties(${redwg} PROPERTIES
  PUBLIC_HEADER "${libredwg_HEADERS}")
if (BUILD_SHARED_LIBS)
  target_compile_definitions(${redwg} PUBLIC DLL_EXPORT;ENABLE_SHARED)
endif()
target_include_directories(${redwg} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src)
target_include_directories(${redwg} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

link_libraries(${redwg} ${LIBS} ${CMAKE_THREAD_LIBS_INIT})

if(NOT LIBREDWG_LIBONLY)
  if(NOT HAVE_GETOPT_H)
    set(getopt_c programs/getopt.c)
  endif(NOT HAVE_GETOPT_H)
  set(executables_TARGETS dwgread dwg2dxf dwggrep dwglayers dwgbmp dwg2SVG)
  add_executable(dwgread programs/dwgread.c ${getopt_c})
  add_executable(dwg2dxf programs/dwg2dxf.c ${getopt_c})
  if(NOT LIBREDWG_DISABLE_WRITE)
    add_executable(dwgrewrite programs/dwgrewrite.c ${getopt_c})
    add_executable(dwgwrite programs/dwgwrite.c ${getopt_c})
    add_executable(dxf2dwg programs/dxf2dwg.c ${getopt_c})
    set(executables_TARGETS ${executables_TARGETS} dwgrewrite dwgwrite dxf2dwg)
  endif(NOT LIBREDWG_DISABLE_WRITE)
  add_executable(dwggrep programs/dwggrep.c ${getopt_c})
  add_executable(dwglayers programs/dwglayers.c ${getopt_c})
  add_executable(dwgbmp programs/dwgbmp.c ${getopt_c})
  add_executable(dwg2SVG programs/dwg2SVG.c
                         programs/escape.c ${getopt_c})

  if (HAVE_PCRE2_H)
    if(HAVE_PCRE2_16)
      target_link_libraries(dwggrep ${redwg} -lpcre2-8 -lpcre2-16)
    else()
      target_link_libraries(dwggrep ${redwg} -lpcre2-8)
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

# no tests if included elsewhere
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()
# nor turned off with -DBUILD_TESTING=OFF
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  set(CAN_SH_TEST OFF cache bool "can shell test")
  if(NOT CMAKE_CROSSCOMPILING)
    set(CAN_SH_TEST ON FORCE)
  endif(NOT CMAKE_CROSSCOMPILING)
  if(CMAKE_HOST_WIN32)
    set(CAN_SH_TEST OFF FORCE)
  endif(CMAKE_HOST_WIN32)
  if(CAN_SH_TEST)
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
      if(UNIX)
        configure_file(programs/alive.test.in alive.test @ONLY)
        add_test(alive.test alive.test)
        if(NOT LIBREDWG_DISABLE_DXF)
          configure_file(programs/dxf.test.in dxf.test @ONLY)
          add_test(dxf.test dxf.test)
          if(NOT LIBREDWG_DISABLE_JSON)
            configure_file(programs/json.test.in json.test @ONLY)
            add_test(json.test json.test)
          endif(NOT LIBREDWG_DISABLE_JSON)
        endif(NOT LIBREDWG_DISABLE_DXF)
        configure_file(programs/svg.test.in svg.test @ONLY)
        add_test(svg.test svg.test)
      endif(UNIX)
    endif(NOT LIBREDWG_LIBONLY)
  endif(CAN_SH_TEST)

  add_subdirectory(test/unit-testing)
endif()

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
  install(TARGETS ${redwg} RUNTIME PUBLIC_HEADER
          DESTINATION libredwg-${PACKAGE_VERSION})
  if(NOT LIBREDWG_LIBONLY)
    install(TARGETS ${executables_TARGETS}
          DESTINATION libredwg-${PACKAGE_VERSION})
  endif()
  install(TARGETS ${redwg}
          LIBRARY
          COMPONENT ${redwg}
          DESTINATION libredwg-${PACKAGE_VERSION})
else()
  include(GNUInstallDirs)
  install(TARGETS ${redwg} PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
  if(NOT LIBREDWG_LIBONLY)
    install(TARGETS ${executables_TARGETS}
            DESTINATION "${CMAKE_INSTALL_BINDIR}")
  endif()
endif()
install(TARGETS RUNTIME)

if(WIN32)
  add_custom_target(dist
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/README README
    COMMAND zip libredwg-${PACKAGE_VERSION}.zip dwg*.exe dxf*.exe *.lib *.dll
                README doc/LibreDWG.pdf
    DEPENDS ${redwg})
endif()

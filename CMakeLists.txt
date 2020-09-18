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
configure_file(src/cmakeconfig.h.in src/config.h)

set(libredwg_HEADERS
    include/dwg.h
    include/dwg_api.h)

set(libredwg_SOURCES
    src/bits.c
    src/common.c
    src/classes.c
    src/decode.c
    src/decode_r2007.c
    src/encode.c
    src/dwg.c
    src/hash.c
    src/dwg_api.c
    src/dynapi.c
    src/free.c
    src/in_dxf.c
    src/out_dxf.c
    #src/in_json.c
    src/out_json.c
    src/out_dxfb.c
    src/out_geojson.c
    src/reedsolomon.c
    src/print.c)

add_library(redwg STATIC
    ${libredwg_HEADERS}
    ${libredwg_SOURCES})

target_include_directories(redwg PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/src)

target_include_directories(redwg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

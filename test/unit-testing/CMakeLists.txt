# target platform, not host
if(WIN32)
  set(EXE_EXT .exe)
#  if(CMAKE_CROSSCOMPILING)
#    set(CMAKE_CROSSCOMPILING_EMULATOR "wine")
#  endif(CMAKE_CROSSCOMPILING)
  add_custom_command(OUTPUT ${redwg}.dll
    COMMAND file(COPY ${top_builddir}/${redwg}.dll
                 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    DEPENDS ${redwg}
  )
endif(WIN32)

add_executable(bits_test
  bits_test.c
  ../../src/bits.c
  ../../src/classes.c
  ../../src/objects.c
  tests_common.h)
#set_property(TARGET bits_test PROPERTY LINK_LIBRARIES -lm ${CMAKE_THREAD_LIBS_INIT})
add_test(bits_test bits_test)
if(WIN32)
  set_tests_properties(bits_test PROPERTIES FIXTURES_SETUP ${redwg}.dll)
endif()

add_executable(hash_test
  hash_test.c
  ../../src/bits.c
  ../../src/hash.c
  ../../src/classes.c
  tests_common.h)
#set_property(TARGET hash_test PROPERTY LINK_LIBRARIES -lm ${CMAKE_THREAD_LIBS_INIT})
add_test(hash_test hash_test${EXE_EXT})

add_executable(common_test
  common_test.c
  ../../src/classes.c
  tests_common.h)
add_test(common_test common_test${EXE_EXT})
if(WIN32)
  set_tests_properties(common_test PROPERTIES FIXTURES_SETUP ${redwg}.dll)
endif()

add_executable(decode_test
  decode_test.c
  ../../src/dwg.c
  ../../src/common.c
  ../../src/bits.c
  ../../src/hash.c
  ../../src/decode_r2007.c
  ../../src/objects.c
  ../../src/classes.c
  ../../src/print.c
  tests_common.h)
#set_property(TARGET decode_test
#  PROPERTY LINK_LIBRARIES -lm ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(decode_test PUBLIC
  ${CMAKE_CURRENT_BINARY_DIR})
add_test(decode_test decode_test)

add_executable(dxf_test
  dxf_test.c
  ../../src/common.c
  ../../src/dynapi.c
  ../../src/objects.c
  ../../src/classes.c
  tests_common.h)
#set_property(TARGET dxf_test
#  PROPERTY LINK_LIBRARIES -lm ${CMAKE_THREAD_LIBS_INIT})
add_test(dxf_test dxf_test${EXE_EXT})

add_executable(dynapi_test
  dynapi_test.c
  ../../src/bits.c
  ../../src/objects.c
  ../../src/classes.c
  tests_common.h)
#set_property(TARGET dynapi_test
#  PROPERTY LINK_LIBRARIES -lm ${CMAKE_THREAD_LIBS_INIT})
add_test(dynapi_test dynapi_test${EXE_EXT})

if(NOT LIBREDWG_DISABLE_WRITE)
  add_executable(add_test
    add_test.c
    ../../src/classes.c
    tests_common.h)
  # this does not find test-data, because it is under Release/Debug
  # TODO: put @abs_top_srcdir@ in test_common.h.in
  if(NOT MSVC)
    add_test(add_test add_test${EXE_EXT})
  endif()
endif(NOT LIBREDWG_DISABLE_WRITE)

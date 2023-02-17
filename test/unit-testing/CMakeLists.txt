if (WIN32)
  if(CMAKE_CROSSCOMPILING)
    set(TEST_PRE "wine ")
  else(CMAKE_CROSSCOMPILING)
    set(TEST_PRE "")
    set(TEST_SUFF ".exe")
  endif(CMAKE_CROSSCOMPILING)
else(WIN32)
  set(TEST_PRE "./")
  set(TEST_SUFF "")
endif(WIN32)

add_executable(bits_test
  bits_test.c
  ../../src/bits.c
  ../../src/classes.c
  ../../src/objects.c
  tests_common.h)
add_test(bits_test ${TEST_PRE}bits_test${TEST_SUFF})

add_executable(hash_test
  hash_test.c
  ../../src/bits.c
  ../../src/hash.c
  ../../src/classes.c
  tests_common.h)
add_test(hash_test ${TEST_PRE}hash_test${TEST_SUFF})

add_executable(common_test
  common_test.c
  ../../src/classes.c
  tests_common.h)
add_test(common_test ${TEST_PRE}common_test${TEST_SUFF})

# works fine on mingw, but it's a linkage mess anyway
if (NOT WIN32)
  add_executable(decode_test
    decode_test.c
    ../../src/bits.c
    ../../src/hash.c
    ../../src/decode_r2007.c
    ../../src/common.c
    ../../src/objects.c
    ../../src/classes.c
    ../../src/print.c
    tests_common.h)
  target_include_directories(decode_test PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR})
  add_test(decode_test ${TEST_PRE}decode_test${TEST_SUFF})
endif(NOT WIN32)

add_executable(dxf_test
  dxf_test.c
  ../../src/common.c
  ../../src/dynapi.c
  ../../src/objects.c
  ../../src/classes.c
  tests_common.h)
add_test(dxf_test ${TEST_PRE}dxf_test${TEST_SUFF})

add_executable(dynapi_test
  dynapi_test.c
  ../../src/bits.c
  ../../src/objects.c
  ../../src/classes.c
  tests_common.h)
add_test(dynapi_test ${TEST_PRE}dynapi_test${TEST_SUFF})

if(NOT LIBREDWG_DISABLE_WRITE)
  add_executable(add_test
    add_test.c
    tests_common.h)
  add_test(add_test ${TEST_PRE}add_test${TEST_SUFF})
endif(NOT LIBREDWG_DISABLE_WRITE)

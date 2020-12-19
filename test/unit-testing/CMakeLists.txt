add_executable(bits_test
  bits_test.c
  ../../src/bits.c
  ../../src/classes.c
  ../../src/objects.c
  tests_common.h)
add_test(bits_test ./bits_test)

add_executable(hash_test
  hash_test.c
  ../../src/bits.c
  ../../src/hash.c
  ../../src/classes.c
  tests_common.h)
add_test(hash_test ./hash_test)

add_executable(common_test
  common_test.c
  ../../src/classes.c
  tests_common.h)
add_test(common_test ./common_test)

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
add_test(decode_test ./decode_test)

add_executable(dxf_test
  dxf_test.c
  ../../src/common.c
  ../../src/dynapi.c
  ../../src/objects.c
  ../../src/classes.c
  tests_common.h)
add_test(dxf_test ./dxf_test)

add_executable(dynapi_test
  dynapi_test.c
  ../../src/bits.c
  ../../src/objects.c
  ../../src/classes.c
  tests_common.h)
add_test(dynapi_test ./dynapi_test)

if(NOT LIBREDWG_DISABLE_WRITE)
  add_executable(add_test
    add_test.c
    tests_common.h)
  add_test(add_test ./add_test)
endif(NOT LIBREDWG_DISABLE_WRITE)

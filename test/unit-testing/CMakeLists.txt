add_executable(bits_test
  bits_test.c
  ../../src/bits.c
  tests_common.h)
add_test(bits_test ./bits_test)

add_executable(dxf_test
  dxf_test.c
  ../../src/common.c
  ../../src/dynapi.c
  tests_common.h)
add_test(dxf_test ./dxf_test)

if(NOT LIBREDWG_DISABLE_WRITE)
  add_executable(add_test
    add_test.c
    tests_common.h)
  add_test(add_test ./add_test)
endif(NOT LIBREDWG_DISABLE_WRITE)

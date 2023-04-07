set( TEST_DIRECTORY ${PROJECT_SOURCE_DIR}/test )
include( ${TEST_DIRECTORY}/AddTestHelpers.cmake )

set( UTIL_DIR util )

## テストケースの追加
create_test_case( stringTest.cpp )
create_test_case( ${UTIL_DIR}/comparable.cpp )

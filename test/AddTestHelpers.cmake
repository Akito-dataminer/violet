set( TEST_DIRECTORY ${PROJECT_SOURCE_DIR}/test )
set( TEST_MAIN TestMain.cpp )

set( EXTENTION_REGEX "\\....$" )
set( DIRECTORY_REGEX ".+/" )

set( INCLUDE_DIRECTORY ${PROJECT_SOURCE_DIR} )

include( ${PROJECT_SOURCE_DIR}/cmake/compile_option.cmake )

##############################
## テストケースを追加するための関数
##############################
function( create_test_case TEST_SOURCE_RELATIVE_PATH )
  find_package(Boost REQUIRED)
  include_directories(${Boost_INCLUDE_DIRS})

  # テスト名を${TEST_SOURCE_RELATIVE_PATH}から作成
  string( REGEX REPLACE "${EXTENTION_REGEX}" "" TEST_NAME_INCLUDE_DIRECTORY ${TEST_SOURCE_RELATIVE_PATH} )
  string( REGEX REPLACE "${DIRECTORY_REGEX}" "" TEST_NAME ${TEST_NAME_INCLUDE_DIRECTORY} )

  # ${TEST_NAME}に格納されているファイル名で実行ファイルを作成
  add_executable( ${TEST_NAME} ${TEST_DIRECTORY}/${TEST_SOURCE_RELATIVE_PATH} ${TEST_DIRECTORY}/${TEST_MAIN} )
  target_include_directories( ${TEST_NAME} PUBLIC ${INCLUDE_DIRECTORY} )

  # コンパイルフラグを追加
  add_compile_option( ${TEST_NAME} )

  target_compile_definitions( ${TEST_NAME} PUBLIC BOOST_TEST_NO_LIB=1 )

  add_test(
    NAME ${TEST_NAME}
    COMMAND ${TEST_NAME}
    )
endfunction( create_test_case )

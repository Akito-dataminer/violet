cmake_minimum_required( VERSION 3.22.1 )

set( build_dir ${CMAKE_CURRENT_LIST_DIR}/build )

if( NOT EXISTS ${build_dir} )
  file( MAKE_DIRECTORY ${build_dir} )
endif()

if( NOT DEFINED slib )
  set( slib libstdc++ )
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -Dslib=${slib}
  WORKING_DIRECTORY ${build_dir}
)

execute_process(
  COMMAND ${CMAKE_COMMAND} --build . -j8
  WORKING_DIRECTORY ${build_dir}
)

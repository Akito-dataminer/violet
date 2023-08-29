# MSVCとgcc系のコンパイラでは、オプションの指定方法が違うので、
# それに対応するためにジェネレータ式を利用している。
set( gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>" )
set( msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>" )
set( warning_options_for_clang_like -Wall -Wextra -Wshadow -Wformat=2 -Wunused )
set( warning_options_for_msvc -W3 )
set( warning_options
  $<${gcc_like_cxx}:${warning_options_for_clang_like}>
  $<${msvc_cxx}:${warning_options_for_msvc}>
)

set( target_if_msys2 "" )

if(MINGW)
  # -target x86_64-w64-windows-gnuというオプションが
  # windows上のすべての環境に必要なのかどうかが分からないので、
  # とりあえずMSYS2環境では有効となるようにしている。
  set( target_if_msys2 -target x86_64-w64-windows-gnu )
endif()

function( add_compile_option TARGET_NAME )
  # slib : command line argument
  set_target_properties(${TARGET_NAME} PROPERTIES
    STANDARD_LIBRARY $<$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU>:-stdlib=${slib}>
  )

  set( stdlib $<TARGET_GENEX_EVAL:${TARGET_NAME},$<TARGET_PROPERTY:${TARGET_NAME},STANDARD_LIBRARY>> )
  message( STATUS "used standard library: ${slib}" )

  target_compile_options(${TARGET_NAME} PUBLIC
    ${warning_options}
    ${stdlib_option}
    ${target_if_msys2}
  )

  target_link_options(${TARGET_NAME} PUBLIC
    ${stdlib_option}
  )
endfunction( add_compile_option )

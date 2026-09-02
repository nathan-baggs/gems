add_library(ddraw SHARED
  ddraw.cpp
  ddraw.def
)

generate_export_header(ddraw)

install(TARGETS ddraw DESTINATION bin)

target_compile_options(ddraw PRIVATE
  -Wall
  -Wextra
  -pedantic
  -Werror
  -Wconversion-null
  -Wmissing-declarations
  -Wno-error=missing-declarations
  -Woverlength-strings
  -Wpointer-arith
  -Wunused-local-typedefs
  -Wunused-result
  -Wvarargs
  -Wvla
  -Wwrite-strings
  -Wno-cast-function-type
  -Wno-missing-declarations
)

target_include_directories(ddraw PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(ddraw PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(ddraw PRIVATE cxx_std_26)
target_compile_definitions(ddraw PRIVATE -DNOMINMAX)
target_link_libraries(ddraw PRIVATE stdc++exp ddraw_dll_main)
target_link_options(ddraw PRIVATE "-Wl,--enable-stdcall-fixup")
set_target_properties(ddraw PROPERTIES PREFIX "")



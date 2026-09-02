add_library(ddraw_dll_main
  ddraw.cpp
)


install(TARGETS ddraw_dll_main DESTINATION bin)

target_compile_options(ddraw_dll_main PRIVATE
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

target_include_directories(ddraw_dll_main PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_features(ddraw_dll_main PRIVATE cxx_std_26)
target_compile_definitions(ddraw_dll_main PRIVATE -DNOMINMAX)
target_link_libraries(ddraw_dll_main PRIVATE stdc++exp)
target_link_options(ddraw_dll_main PRIVATE "-Wl,--enable-stdcall-fixup")



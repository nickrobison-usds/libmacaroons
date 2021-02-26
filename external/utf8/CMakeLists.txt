message(${CMAKE_CURRENT_SOURCE_DIR})
add_library(utf8
        vendor/range-sse.c vendor/range-neon.c vendor/naive.c vendor/range-avx2.c)

target_compile_definitions(utf8 PRIVATE __x86_64__)

target_include_directories(utf8
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(utf8 PROPERTIES COMPILE_FLAGS "-march=native")
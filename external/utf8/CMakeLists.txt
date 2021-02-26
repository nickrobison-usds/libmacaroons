message(${CMAKE_CURRENT_SOURCE_DIR})
add_library(utf8
        vendor/range-sse.c vendor/range-neon.c vendor/naive.c vendor/range-avx2.c)

# Special handling of Windows, which doesn't set the definitions correctly'
if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    target_compile_definitions(utf8 PRIVATE USE_NAIVE)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows" AND "${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
    target_compile_definitions(utf8 PRIVATE __x86_64__)
endif ()


target_include_directories(utf8
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(utf8 PROPERTIES COMPILE_FLAGS "-march=native")
find_package (Python3 COMPONENTS Interpreter Development)

set(SETUP_PY_IN "${CMAKE_CURRENT_SOURCE_DIR}/setup.py.in")
set(SETUP_PY    "${CMAKE_CURRENT_BINARY_DIR}/setup.py")
set(DEPS        "${CMAKE_CURRENT_SOURCE_DIR}/macaroons.pyx")
set(OUTPUT      "${CMAKE_CURRENT_BINARY_DIR}/build/timestamp")

get_target_property(MACAROON_INCLUDES macaroons INCLUDE_DIRECTORIES)

configure_file(${SETUP_PY_IN} ${SETUP_PY})

add_custom_command(OUTPUT ${OUTPUT}
        COMMAND ${Python3_EXECUTABLE} ${SETUP_PY} build
        COMMAND ${CMAKE_COMMAND} -E touch ${OUTPUT}
        DEPENDS ${DEPS} macaroons)

add_custom_target(python ALL DEPENDS ${OUTPUT})
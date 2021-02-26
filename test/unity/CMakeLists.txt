set(SRCS unit_tests.c test_helpers.c serialization_tests.c varint_tests.c verifier_tests.c macaroon_builder_tests.c request_verify_tests.c utf8check_tests.c)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	list(APPEND SRCS strptime.c)
endif ()

add_executable(unit_tests_app ${SRCS})
target_link_libraries(unit_tests_app unity macaroons)

add_test(MacaroonTests unit_tests_app)
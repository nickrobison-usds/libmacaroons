add_executable(unit_tests_app unit_tests.c test_helpers.c serialization_tests.c varint_tests.c verifier_tests.c)
target_link_libraries(unit_tests_app unity macaroons)

if (WITH_SANITIZERS)
    add_sanitizers(unit_tests_app)
endif ()

add_test(unit_tests unit_tests_app)
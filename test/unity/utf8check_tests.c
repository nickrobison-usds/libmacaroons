//
// Created by Nick Robison on 2/25/21.
//

/* c */
#include <string.h>
#include <stdbool.h>

/* macaroons */

/* unity */
#include "unity.h"
#include "unity_fixture.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpointer-sign"
struct utf8_test {
    const char *str;
    bool valid;
};

bool validate_utf8(const unsigned char *data, size_t len);

TEST_GROUP(UTF8CheckTests);


TEST_SETUP(UTF8CheckTests) {

}

TEST_TEAR_DOWN(UTF8CheckTests) {

}

TEST(UTF8CheckTests, test_utf8_strings) {
    struct utf8_test test_strings[] = {
            {.str = "hello", .valid = true},
            {.str = "\xC1", .valid = false},
            {.str = "\xEF\xBB\xBF", .valid = true},
            {.str = "\xEF\x0B\xBF", .valid = false},
            {.str = "\xE1\x0B\xBF", .valid = false},
            {.str = "\xEF\x0F\xBF", .valid = false},
            {.str = "\xEF\x0B\xC1", .valid = false},
            {.str = "\xEF\xBF", .valid = false},
            {.str = "\xEF", .valid = false},
            {.str = "\xEF\x0B", .valid = false},
            {.str = "", .valid = true},
            {.str = "\\xf0\\x28\\x8c\\xbc🤓", .valid = true},
            {.str = "OLsóS«\\u0090\\u0007Fµ«\\u0003ý×hJ&\\u0017,Ä\\u0015\\u0097#", .valid = true}
    };

    for (size_t i = 0; i < sizeof(test_strings) / sizeof(struct utf8_test); i++) {
        struct utf8_test t = test_strings[i];
        TEST_ASSERT_EQUAL_MESSAGE(t.valid, validate_utf8(t.str, strlen(t.str)), "Pass?");
    }
}

TEST_GROUP_RUNNER(UTF8CheckTests) {
    RUN_TEST_CASE(UTF8CheckTests, test_utf8_strings);
}
#pragma clang diagnostic pop
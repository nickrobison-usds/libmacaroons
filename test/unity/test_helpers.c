//
// Created by Nicholas Robison on 1/9/20.
//

// Ignore the fact that Windows wants us to use the secure CRT, which doesn't exist elsewhere.
#define _CRT_SECURE_NO_WARNINGS

/* C */
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* macaroons */
#include <macaroons.h>
#include <base64.h>
#include <unity.h>

size_t
int2size_t(int val) {
    return (val < 0) ? SIZE_MAX : (size_t) ((unsigned) val);
}

struct parsed_macaroon {
    unsigned char *B;
    struct macaroon *M;
    enum macaroon_format F;
};

struct verifier_test {
    int version;
    bool authorized;
    char *serialized;
    unsigned char *key;
    size_t num_caveats;
    char **caveats;
};

struct macaroon *deserialize_macaroon(const char *serialized) {
    size_t buf_sz = strlen(serialized);
    unsigned char *buf = malloc(buf_sz);
    TEST_ASSERT_NOT_NULL_MESSAGE(buf, "Buffer cannot be null");

    int rc = b64_pton(serialized, buf, buf_sz);
    if (rc < 0) {
        TEST_FAIL_MESSAGE("Unable to decode Base64 data");
    }

    enum macaroon_returncode err = MACAROON_SUCCESS;
    struct macaroon *M = macaroon_deserialize(buf, int2size_t(rc), &err);
    free(buf);
    TEST_ASSERT_EQUAL_INT_MESSAGE(MACAROON_SUCCESS, err, "Should have successfully deserialized");
    return M;
}


void verify_macaroon(const struct verifier_test *test) {

    struct macaroon_verifier *V = NULL;
    struct macaroon **macaroons = NULL;
    size_t macaroons_sz = 0;

    V = macaroon_verifier_create();
    TEST_ASSERT_NOT_NULL_MESSAGE(V, "Verifier should create ok");

    // Add all the caveats
    for (size_t i = 0; i < test->num_caveats; i++) {
        char *caveat = test->caveats[i];
        enum macaroon_returncode err = MACAROON_SUCCESS;
        macaroon_verifier_satisfy_exact(V, (const unsigned char *) caveat, strlen(caveat), &err);
        TEST_ASSERT_EQUAL_INT_MESSAGE(MACAROON_SUCCESS, err, "Should have added caveat");
    }

    // Deserialize the macaroon

    struct macaroon *M = deserialize_macaroon(test->serialized);

    TEST_ASSERT_NOT_NULL_MESSAGE(M, "Macaroon should deserialize correctly");

    struct macaroon **tmp;

    ++macaroons_sz;
    tmp = realloc(macaroons, macaroons_sz * sizeof(struct macaroon *));
    TEST_ASSERT_NOT_NULL(tmp);

    macaroons = tmp;
    (macaroons)[macaroons_sz - 1] = M;

    // Look at the first macaroon and make sure it has the correct number of caveats
    if (test->authorized) {
        TEST_ASSERT_EQUAL_size_t_MESSAGE(test->num_caveats, macaroon_num_caveats(macaroons[0]),
                                         "Should have the correct number of caveats");
        TEST_ASSERT_EQUAL_size_t_MESSAGE(test->num_caveats, macaroon_num_first_party_caveats(macaroons[0]),
                                         "All caveats should be first-party");
        TEST_ASSERT_EQUAL_size_t_MESSAGE(0, macaroon_num_third_party_caveats(macaroons[0]),
                                         "Should not have any third-party caveats");
    }

    // Ok, now let's verify it
    const size_t key_sz = strlen((const char *) test->key);
    enum macaroon_returncode err;
    int verify = macaroon_verify(V, macaroons[0], test->key, key_sz, macaroons + 1, macaroons_sz - 1, &err);

    if (verify != 0 && err != MACAROON_NOT_AUTHORIZED) {
        char *str = "";
        sprintf(str, "verification encountered exceptional error: %s\n", macaroon_error(err));
        TEST_FAIL_MESSAGE(str);
    } else if (verify == 0 && !test->authorized) {
        TEST_FAIL_MESSAGE("verification passed for \"unauthorized\" scenario\n");
    } else if (verify != 0 && err == MACAROON_NOT_AUTHORIZED && test->authorized) {
        TEST_FAIL_MESSAGE("verification failed for \"authorized\" scenario\n");
    }
}

//
// Created by Nick Robison on 2/24/21.
//

/* c */
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* macaroons */
#include <macaroons.h>
#include <port.h>

/* unity */
#include "unity.h"
#include "unity_fixture.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpointer-sign"
TEST_GROUP(PrepareVerifyTests);

const char *id = "we used our secret key";
const char *secret = "this is our super secret key; only we should know it";
const char *location = "http://mybank/";
const char *loc2 = "http://auth.mybank/";
const char *cav_key = "4; guaranteed random by a fair toss of the dice";
const char *cav_id = "this was how we remind auth of key/pred";
const char *cav = "account = 3735928559";;

bool prefix(const char *pre, const char *str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

time_t mk_time(const char *str) {
    struct tm tm;
    strptime(str, "%Y-%m-%dT%H:%M", &tm);
    return mktime(&tm);
}

int verify_timestamp(const char *pred) {
    const char *pre = "time";

    if (prefix(pre, pred)) {
        // Trim off the start
        char *timestamp = malloc(sizeof(char *) * 16);
        strncpy(timestamp, &pred[7], 16);
        const time_t cav_time = mk_time(timestamp);
        const time_t now = time(0);
        const double t = difftime(cav_time, now);
        return t < 0. ? 0 : -1;
    }

    return -1;
}

int general_cb(void *f, const unsigned char *pred, size_t pred_sz) {
    char *to_verify = malloc((sizeof(char *) * pred_sz));

    strncpy(to_verify, pred, pred_sz);
    to_verify[pred_sz] = '\0';
    return ((int (*)(const unsigned char *)) f)(to_verify);
}

TEST_SETUP(PrepareVerifyTests) {
}

TEST_TEAR_DOWN(PrepareVerifyTests) {
}

TEST(PrepareVerifyTests, prepare_request_verify_test_simple) {

    enum macaroon_returncode err = MACAROON_SUCCESS;
    struct macaroon *MV1 = macaroon_create(location, strlen(location), secret, strlen(secret), id, strlen(id), &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);

    struct macaroon *MV2 = macaroon_add_first_party_caveat(MV1, cav, strlen(cav), &err);
    macaroon_destroy(MV1);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    struct macaroon *MV = macaroon_add_third_party_caveat(MV2, loc2, strlen(loc2), cav_key, strlen(cav_key), cav_id,
                                                          strlen(cav_id), &err);
    macaroon_destroy(MV2);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);

    struct macaroon *M1 = macaroon_create(loc2, strlen(loc2), cav_key, strlen(cav_key), cav_id, strlen(cav_id), &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);

    const char *cav2 = "time < 2025-01-01T00:00";
    struct macaroon *D = macaroon_add_first_party_caveat(M1, cav2, strlen(cav2), &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_destroy(M1);

    const unsigned char *sig;
    size_t sig_sz = 0;
    macaroon_signature(D, &sig, &sig_sz);
    char *hex = malloc(sizeof(char *) * sig_sz);
    macaroon_bin2hex(sig, sig_sz, hex);

    TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE("b338d11fb136c4b95c86efe146f77978cd0947585375ba4d4da4ef68be2b3e8b", hex,
                                         sig_sz, "Signatures should be equal");

    struct macaroon *DP = macaroon_prepare_for_request(MV, D, &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_destroy(D);

    // Verify valid
    struct macaroon_verifier *V = macaroon_verifier_create();
    macaroon_verifier_satisfy_general(V, &general_cb, &verify_timestamp, &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_verifier_satisfy_exact(V, cav, strlen(cav), &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_verify(V, MV, secret, strlen(secret), &DP, 1, &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_verifier_destroy(V);

    // Verify unprepared macaroon fails
    struct macaroon_verifier *V2 = macaroon_verifier_create();
    macaroon_verifier_satisfy_general(V2, &general_cb, &verify_timestamp, &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_verifier_satisfy_exact(V2, cav, strlen(cav), &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_verify(V2, MV, secret, strlen(secret), &D, 1, &err);
    TEST_ASSERT_EQUAL(MACAROON_NOT_AUTHORIZED, err);
    macaroon_verifier_destroy(V2);

    // Verify macaroon without third-party caveat fails
    err = MACAROON_SUCCESS;
    struct macaroon_verifier *V3 = macaroon_verifier_create();
    macaroon_verifier_satisfy_general(V3, &general_cb, &verify_timestamp, &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_verifier_satisfy_exact(V3, cav, strlen(cav), &err);
    TEST_ASSERT_EQUAL(MACAROON_SUCCESS, err);
    macaroon_verify(V3, MV, secret, strlen(secret), NULL, 0, &err);
    TEST_ASSERT_EQUAL(MACAROON_NOT_AUTHORIZED, err);
    macaroon_verifier_destroy(V3);

}

TEST_GROUP_RUNNER(PrepareVerifyTests) {
    RUN_TEST_CASE(PrepareVerifyTests, prepare_request_verify_test_simple);
}

#pragma clang diagnostic pop
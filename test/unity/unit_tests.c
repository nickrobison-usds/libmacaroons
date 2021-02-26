#include "unity_fixture.h"

static void RunAllTests(void) {
    RUN_TEST_GROUP(SerializationTests);
    RUN_TEST_GROUP(VarintTests);
    RUN_TEST_GROUP(VerifierTests);
    RUN_TEST_GROUP(MacaroonBuilderTests);
    //RUN_TEST_GROUP(PrepareVerifyTests);
    RUN_TEST_GROUP(UTF8CheckTests);
}

int main(int argc, const char * argv[]) {
    return UnityMain(argc, argv, RunAllTests);
}
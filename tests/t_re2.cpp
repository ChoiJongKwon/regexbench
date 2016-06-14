#include <atf-c++.hpp>

#include <iostream>

#include "../src/regexbench.h"
#include "../src/RE2Engine.h"
#include "../src/PcapSource.h"

using namespace regexbench;

ATF_TEST_CASE_WITHOUT_HEAD(rule_compile);
ATF_TEST_CASE_BODY(rule_compile) {
  RE2Engine engine;
  engine.compile(regexbench::loadRules(DATA_DIR "/rule/malformat.re"));
  ATF_REQUIRE_EQ(0, engine.getRuleSize());
}

ATF_INIT_TEST_CASES(tcs) {
  ATF_ADD_TEST_CASE(tcs, rule_compile);
}

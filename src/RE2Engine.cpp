#include "RE2Engine.h"

using namespace regexbench;

void RE2Engine::compile(const std::vector<Rule> &rules) {
  for (const auto &rule : rules) {
    RE2::Options op;

    if (rule.isSet(MOD_CASELESS)) {
      op.set_case_sensitive(false);
    }
    if (rule.isSet(MOD_MULTILINE)) {
      op.set_one_line(false);
    }
    if (rule.isSet(MOD_DOTALL)) {
      op.set_dot_nl(true);
    }

    auto re = std::make_unique<re2::RE2>(rule.getRegexp(), op);

    if (re->ok()) {
      res.push_back(std::move(re));
    } else {
      throw std::runtime_error("fail to compile rule: " + rule.getRegexp());
    }
  }
}

size_t RE2Engine::match(const char *data, size_t len, size_t) {
  len = 0;
  for (const auto &re : res) {
    if (re2::RE2::PartialMatch(data, *re)) {
      return 1;
    }
  }
  return 0;
}

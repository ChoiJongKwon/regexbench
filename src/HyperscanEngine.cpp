#include <iostream>
#include <sstream>
#include <vector>

#include <hs/hs_compile.h>

#include "HyperscanEngine.h"

using namespace regexbench;

static int onMatch(unsigned int, unsigned long long, unsigned long long,
                   unsigned int, void* ctx)
{
  size_t& nmatches = *static_cast<size_t*>(ctx);
  nmatches++;
  return 0;
}

HyperscanEngine::HyperscanEngine()
    : db(nullptr), scratch(nullptr), platform{HS_TUNE_FAMILY_GENERIC, 0, 0, 0},
      nsessions(0)
{
}

HyperscanEngine::~HyperscanEngine()
{
  hs_free_database(db);
  hs_free_scratch(scratch);
}

void HyperscanEngine::reportFailedRules(const std::vector<Rule>& rules)
{
  std::stringstream msg;
  hs_compile_error_t* err = nullptr;

  for (const auto& rule : rules) {
    unsigned flag = HS_FLAG_ALLOWEMPTY;
    if (rule.isSet(MOD_CASELESS))
      flag |= HS_FLAG_CASELESS;
    if (rule.isSet(MOD_MULTILINE))
      flag |= HS_FLAG_MULTILINE;
    if (rule.isSet(MOD_DOTALL))
      flag |= HS_FLAG_DOTALL;
    auto result = hs_compile(rule.getRegexp().data(), flag, HS_MODE_BLOCK,
                             &platform, &db, &err);
    if (result != HS_SUCCESS) {
      msg << "id: " << rule.getID() << " " << err->message
          << " rule:" << rule.getRegexp() << "\n";
      hs_free_compile_error(err);
    }
  }
  if (msg.str().size()) {
    std::runtime_error error(msg.str());
    throw error;
  }
}

void HyperscanEngine::compile(const std::vector<Rule>& rules)
{
  std::vector<const char*> exps;
  std::vector<unsigned> flags;
  std::vector<unsigned> ids;
  for (const auto& rule : rules) {
    exps.push_back(rule.getRegexp().data());
    ids.push_back(static_cast<unsigned>(rule.getID()));
    unsigned flag = HS_FLAG_ALLOWEMPTY;
    if (rule.isSet(MOD_CASELESS))
      flag |= HS_FLAG_CASELESS;
    if (rule.isSet(MOD_MULTILINE))
      flag |= HS_FLAG_MULTILINE;
    if (rule.isSet(MOD_DOTALL))
      flag |= HS_FLAG_DOTALL;
    flags.push_back(flag);
  }

  hs_compile_error_t* err;
  unsigned int mode = HS_MODE_BLOCK;

  if (nsessions)
    mode = HS_MODE_STREAM;

  auto result = hs_compile_multi(exps.data(), flags.data(), ids.data(),
                                 static_cast<unsigned>(exps.size()), mode,
                                 &platform, &db, &err);
  if (result != HS_SUCCESS) {
    reportFailedRules(rules);
  }

  result = hs_alloc_scratch(db, &scratch);
  if (result != HS_SUCCESS)
    throw std::bad_alloc();
}

void HyperscanEngineStream::compile(const std::vector<Rule>& rules)
{
  HyperscanEngine::compile(rules);
  streams = std::make_unique<hs_stream* []>(nsessions);

  for (size_t i = 0; i < nsessions; i++) {
    hs_open_stream(db, 0, &streams[i]);
  }
}

HyperscanEngineStream::~HyperscanEngineStream()
{
  for (size_t i = 0; i < nsessions; i++) {
    hs_close_stream(streams[i], scratch, onMatch, nullptr);
  }
}

size_t HyperscanEngine::match(const char* data, size_t len, size_t)
{
  size_t nmatches = 0;
  hs_scan(db, data, static_cast<unsigned>(len), 0, scratch, onMatch, &nmatches);
  return nmatches > 0;
}

void HyperscanEngineStream::init(size_t nsessions_) { nsessions = nsessions_; }

size_t HyperscanEngineStream::match(const char* data, size_t len, size_t sid)
{
  size_t nmatches = 0;
  hs_scan_stream(streams[sid], data, static_cast<unsigned>(len), 0, scratch,
                 onMatch, &nmatches);
  return nmatches > 0;
}

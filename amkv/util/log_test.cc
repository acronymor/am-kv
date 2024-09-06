#include "util/log.h"

#include "gtest/gtest.h"

namespace amkv::util {
class LogTest : public testing::Test {
 protected:
  void SetUp() override {
    char* name = "a.out";
    amdb::init_log(name);
  }
};

TEST_F(LogTest, Simple) {
  ERROR("ERROR: {}", "Hello Error");
  WARN("WARN: {}", "Hello Warn");
  INFO("INFO: {}", "Hello Info");
  DEBUG("DEBUG: {}", "Hello Debug");
  TRACE("TRACE: {}", "Hello Trace");
}
}  // namespace amkv::util

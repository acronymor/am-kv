#include "comm/log.h"

#include "gtest/gtest.h"

namespace amkv::comm {
class LogLevelTest : public testing::Test {
 protected:
  void SetUp() override {
    char* name = "a.out";
    comm::init_log(name);
  }
};

TEST_F(LogLevelTest, Simple) {
  ERROR("ERROR: {}", "Hello Error");
  WARN("WARN: {}", "Hello Warn");
  INFO("INFO: {}", "Hello Info");
  DEBUG("DEBUG: {}", "Hello Debug");
  TRACE("TRACE: {}", "Hello Trace");
}
}  // namespace amkv::comm

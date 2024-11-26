#include "comm/env.h"

#include "gtest/gtest.h"

namespace amkv::comm {
TEST(EnvTest, Simple) {
  Env env;
  util::WritableFile* file = nullptr;
  comm::Status status = env.NewWritableFile("/tmp/a.txt", &file);
  ASSERT_EQ(comm::ErrorCode::kOk, status.Code());
}
}  // namespace amkv::db
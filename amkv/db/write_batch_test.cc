#include "write_batch.h"

#include "gtest/gtest.h"

namespace amkv::db {
class WriteBatchTest : public testing::Test {};

TEST_F(WriteBatchTest, Simple) {
  std::string key = "key-a";
  std::string value = "value";

  WriteBatch batch;
  batch.Put(key, value);
  batch.Delete(key);
}
}  // namespace amkv::db
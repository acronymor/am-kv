#include "write_batch_internal.h"

#include "db/write_batch.h"
#include "gtest/gtest.h"

namespace amkv::db {
class WriteBatchInternalTest : public testing::Test {};

TEST_F(WriteBatchInternalTest, Contents) {
  std::string key = "key-a";
  std::string value = "value";

  WriteBatch batch;
  batch.Put(key, value);

  std::string_view content = WriteBatchInternal::Contents(&batch);
  std::cout << content << std::endl;

  comm::Status status = WriteBatchInternal::InsertInto(&batch, nullptr);
  assert(comm::ErrorCode::kOk == status.Code());
}
}  // namespace amkv::db
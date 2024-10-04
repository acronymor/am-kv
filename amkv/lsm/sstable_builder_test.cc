#include "lsm/sstable_builder.h"

#include <fcntl.h>

#include "gtest/gtest.h"

namespace amkv::table {
class SSTableTest : public testing::Test {};

TEST_F(SSTableTest, Simple) {
  comm::Options options;
  util::WritableFile* file = new util::PosixWritableFile("/tmp/sstable_test.txt");
  file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);

  SSTableBuilder builder(options, file);
  builder.Add("key1", "value1");
  builder.Add("key2", "value2");

  comm::Status status = builder.Finish();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  status = file->Sync();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  status = file->Close();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  delete file;
}
}  // namespace amkv::table
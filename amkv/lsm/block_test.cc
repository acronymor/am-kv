#include <fcntl.h>

#include "gtest/gtest.h"
#include "lsm/block_builder.h"

namespace amkv::block {

class BlockTest : public testing::Test {};

TEST_F(BlockTest, Simple) {
  BlockBuilder data_block;
  data_block.Add("key1", "value1");
  data_block.Add("key2", "value2");
  std::string_view block_content = data_block.Build();

  util::WritableFile* file = new util::PosixWritableFile("/tmp/block_test.txt");
  file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);
  comm::Status status = file->Append(block_content);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  status = file->Append("CRC32");
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  file->Flush();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  status = file->Close();
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  delete file;
}
}  // namespace amkv::block
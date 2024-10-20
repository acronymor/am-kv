#include <fcntl.h>

#include "block_iter.h"
#include "gtest/gtest.h"
#include "lsm/block_builder.h"
#include "util/access_file.h"
#include "util/writable_file.h"

namespace amkv::block {

class BlockTest : public testing::Test {};

TEST_F(BlockTest, Write) {
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

TEST_F(BlockTest, Read) {
  util::PosixAccessFile* file = new util::PosixAccessFile("/tmp/block_test.txt");
  comm::Status status = file->Open(O_RDONLY | O_CLOEXEC);
  std::string block_content;
  file->Read(0, file->Size(), &block_content);
  file->Close();
  delete file;

  Block* block = new Block(block_content);
  BlockIterator iter(block);
  iter.SeekToFirst();
  EXPECT_TRUE(iter.Valid());
  EXPECT_EQ("key1", iter.Key());
  EXPECT_EQ("value1", iter.Value());
  iter.Next();
  EXPECT_TRUE(iter.Valid());
  EXPECT_EQ("key2", iter.Key());
  EXPECT_EQ("value2", iter.Value());
  delete block;
}
}  // namespace amkv::block
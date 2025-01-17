#include "lsm/version_edit.h"

#include "gtest/gtest.h"

namespace amkv::version {
class VersionEditTest : public testing::Test {};

TEST_F(VersionEditTest, Simple) {
  std::string record;

  VersionEdit edit1;
  FileMetaData meta1 = {.refs = 0, .number = 1, .file_size = 99, .smallest = "a", .largest = "b"};
  FileMetaData meta2 = {.refs = 1, .number = 2, .file_size = 35, .smallest = "d", .largest = "z"};
  edit1.AddFile(0, &meta1);
  edit1.AddFile(0, &meta2);
  edit1.RemoveFile(1, 2);
  edit1.EncodeTo(&record);

  VersionEdit edit2;
  comm::Status status = edit2.DecodeFrom(record);
  ASSERT_EQ(comm::ErrorCode::kOk, status.Code());
}
}  // namespace amkv::version

#include "version_set.h"

#include "gtest/gtest.h"
#include "lsm/sstable.h"

namespace amkv::version {
class VersionSetTest : public testing::Test {};

TEST_F(VersionSetTest, Simple) {
  std::string dbname = "dbname";
  comm::Options options;
  VersionSet* version_set = new VersionSet(dbname, &options);

  VersionEdit edit;
  FileMetaData meta1 = {.refs = 0, .number = 1, .file_size = 99, .smallest = "a", .largest = "c"};
  FileMetaData meta2 = {.refs = 1, .number = 2, .file_size = 35, .smallest = "d", .largest = "z"};
  edit.AddFile(0, &meta1);
  edit.AddFile(0, &meta2);

  comm::Status status = version_set->LogAndApply(&edit);
  std::cout << status.Message() << std::endl;
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());

  delete version_set;
}

TEST_F(VersionSetTest, Recover) {
  std::string dbname = "dbname";
  comm::Options options;
  VersionSet* version_set = new VersionSet(dbname, &options);

  bool save_manifest = false;
  comm::Status status = version_set->Recover(&save_manifest);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
}
}  // namespace amkv::version
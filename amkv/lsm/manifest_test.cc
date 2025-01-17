#include <fcntl.h>

#include "gtest/gtest.h"
#include "log_reader.h"
#include "lsm/version_edit.h"
#include "util/sequential_file.h"

namespace amkv::version {
class ManifestTest : public testing::Test {};

TEST_F(ManifestTest, Simple) {
  std::string manifest_file = "t_db/MANIFEST-000002";
  util::PosixSequentialFile* file = new util::PosixSequentialFile(manifest_file);
  comm::Status status = file->Open(O_RDONLY | O_CLOEXEC);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  log::Reader reader(file);
  std::string raw_edit;
  std::string record;
  while (reader.ReadRecord(&record, &raw_edit)) {
    VersionEdit edit;
    status = edit.DecodeFrom(raw_edit);
    std::cout << edit.DebugString() << std::endl;
    EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  }

  file->Close();
  delete file;
}
}  // namespace amkv::version
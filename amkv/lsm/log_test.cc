#include <fcntl.h>

#include "gtest/gtest.h"
#include "lsm/log_reader.h"
#include "lsm/log_writer.h"

namespace amkv::log {

class WALLogTest : public testing::Test {
 protected:
  std::string filename = "/tmp/amkv.log";
};

TEST_F(WALLogTest, Simple) {
  comm::Status status = comm::Status::OK();
  std::string_view raw_data{"hello world"};

  util::WritableFile* writable_log_file = new util::PosixWritableFile(filename);
  writable_log_file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);
  Writer log(writable_log_file);

  status = log.AddRecord(raw_data);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  writable_log_file->Close();
  delete writable_log_file;

  util::SequentialFile* readable_log_file = new util::PosixSequentialFile(filename);
  status = readable_log_file->Open(O_RDONLY | O_CLOEXEC);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  Reader reader(readable_log_file);
  std::string record;
  std::string scratch;
  bool res = reader.ReadRecord(&record, &scratch);
  EXPECT_TRUE(res);
  readable_log_file->Close();
  delete readable_log_file;
}
}  // namespace amkv::log
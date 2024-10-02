#include "lsm/log_writer.h"

#include <fcntl.h>

#include "db/write_batch.h"
#include "gtest/gtest.h"

namespace amkv::log {

class WriterTest : public testing::Test {};

TEST_F(WriterTest, Simple) {
  util::WritableFile* writable_file = new util::PosixWritableFile("/tmp/amkv.log");
  writable_file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);

  Writer writer(writable_file);
  std::string_view data_view{"hello world!"};
  comm::Status status = writer.AddRecord(data_view);
  assert(comm::ErrorCode::kOk == status.Code());

  delete writable_file;
}
}  // namespace amkv::log
#include "lsm/log_writer.h"

#include <fcntl.h>

#include "db/write_batch.h"
#include "gtest/gtest.h"

namespace amkv::lsm {
namespace log {

class WriterTest : public testing::Test {};

TEST_F(WriterTest, Simple) {
  util::WritableFile* writable_file = new util::PosixWritableFile("/tmp/amkv.log");
  writable_file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);

  lsm::log::Writer writer(writable_file);
  std::string_view data_view{"hello world!"};
  util::Status status = writer.AddRecord(data_view);
  assert(util::ErrorCode::kOk == status.Code());

  delete writable_file;
}
}  // namespace log
}  // namespace amkv::lsm
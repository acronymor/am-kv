#include "util/writable_file.h"

#include <fcntl.h>

#include "gtest/gtest.h"

namespace amkv::util {
class PosixWritableFileTest : public testing::Test {};

TEST_F(PosixWritableFileTest, Simple) {
  PosixWritableFile* posix_writable_file = new PosixWritableFile("/tmp/a.txt");
  Status status = posix_writable_file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);
  posix_writable_file->Append("Hello C++");
  posix_writable_file->Append("Hello Python");
  posix_writable_file->Flush();
  posix_writable_file->Close();
  delete posix_writable_file;
}
}  // namespace amkv::util

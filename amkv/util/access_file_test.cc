
#include "util/access_file.h"

#include <fcntl.h>

#include "gtest/gtest.h"

namespace amkv::util {
class PosixAccessFileTest : public testing::Test {};

TEST_F(PosixAccessFileTest, Simple) {
  PosixAccessFile* posix_access_file = new PosixAccessFile("/tmp/a.txt");
  comm::Status status = posix_access_file->Open(O_RDONLY | O_CLOEXEC);
  std::string out;
  posix_access_file->Read(0, 10, &out);
  std::cout << out << std::endl;
  posix_access_file->Close();
  delete posix_access_file;
}
}  // namespace amkv::util

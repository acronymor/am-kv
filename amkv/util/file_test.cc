#include <fcntl.h>

#include "gtest/gtest.h"
#include "util/access_file.h"
#include "util/sequential_file.h"
#include "util/writable_file.h"

namespace amkv::util {
class FileTest : public testing::Test {
 protected:
  void SetUp() override {}

  const std::string filename = "/tmp/file.txt";
};

TEST_F(FileTest, Simple) {
  comm::Status status = comm::Status::OK();

  std::string a = "Hello C++";
  std::string b = "Hello Python";
  std::size_t len = a.size() + b.size();

  PosixWritableFile* posix_writable_file = new PosixWritableFile(filename);
  status = posix_writable_file->Open(O_TRUNC | O_WRONLY | O_CREAT | O_CLOEXEC);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  posix_writable_file->Append(a);
  posix_writable_file->Append(b);
  posix_writable_file->Flush();
  posix_writable_file->Close();
  delete posix_writable_file;

  PosixSequentialFile* posix_sequential_file = new PosixSequentialFile(filename);
  status = posix_sequential_file->Open(O_RDONLY | O_CLOEXEC);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  std::string_view record;
  char* ch = new char[len];
  posix_sequential_file->Read(len, &record, ch);
  EXPECT_EQ("Hello C++Hello Python", record);
  delete ch;

  PosixAccessFile* posix_access_file = new PosixAccessFile(filename);
  status = posix_access_file->Open(O_RDONLY | O_CLOEXEC);
  EXPECT_EQ(comm::ErrorCode::kOk, status.Code());
  std::string out;
  posix_access_file->Read(0, len, &out);
  EXPECT_EQ("Hello C++Hello Python", out);
  posix_access_file->Close();
  delete posix_access_file;
}
}  // namespace amkv::util
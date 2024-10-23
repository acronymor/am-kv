#include "util/sequential_file.h"

#include <fcntl.h>
#include <unistd.h>

namespace amkv::util {
PosixSequentialFile::PosixSequentialFile(const std::string filename) : filename_(std::move(filename)) {}

comm::Status PosixSequentialFile::Open(int oflag) {
  this->fd_ = ::open(this->filename_.c_str(), oflag, 0644);
  if (this->fd_ < 0) {
    return comm::Status(comm::ErrorCode::kNotFound, this->filename_);
  }
  return comm::Status::OK();
}

comm::Status PosixSequentialFile::Close() {
  comm::Status status = comm::Status::OK();
  const int close_result = ::close(fd_);
  if (close_result < 0) {
    status = comm::Status(comm::ErrorCode::kNotFound, filename_);
  }
  fd_ = -1;
  return status;
}

comm::Status PosixSequentialFile::Read(std::size_t n, std::string_view* result, char* scratch) {
  comm::Status status = comm::Status::OK();
  while (true) {
    ::ssize_t read_size = ::read(fd_, scratch, n);
    if (read_size < 0) {
      if (errno == EINTR) {
        continue;
      }
      status = comm::Status(comm::ErrorCode::kNotFound, this->filename_);
      break;
    }
    *result = std::string_view(scratch, read_size);
    break;
  }
  return status;
}

comm::Status PosixSequentialFile::Skip(std::uint64_t n) {
  if (::lseek(fd_, n, SEEK_CUR) == static_cast<off_t>(-1)) {
    return comm::Status(comm::ErrorCode::kNotFound, this->filename_);
  }
  return comm::Status::OK();
}
}  // namespace amkv::util

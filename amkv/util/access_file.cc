#include "util/access_file.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "util/file.h"

namespace amkv::util {

PosixAccessFile::PosixAccessFile(const std::string& filename) : AccessFile(), fd_(0), filename_(filename) {
  GetFileSize(this->filename_, &this->file_size_);
}

comm::Status PosixAccessFile::Open(int oflag) {
  this->fd_ = ::open(this->filename_.c_str(), oflag, 0644);
  if (this->fd_ < 0) {
    return comm::Status(comm::ErrorCode::kNotFound, this->filename_);
  }
  this->mmap_base_ =
      static_cast<char*>(::mmap(/*addr=*/nullptr, this->file_size_, PROT_READ, MAP_SHARED, this->fd_, 0));
  return comm::Status::OK();
}

comm::Status PosixAccessFile::Close() {
  comm::Status status = comm::Status::OK();

  const int close_result = ::close(fd_);
  if (close_result < 0) {
    status = comm::Status(comm::ErrorCode::kNotFound, filename_);
  }
  fd_ = -1;

  return status;
}

comm::Status PosixAccessFile::Read(std::uint64_t offset, std::size_t n, std::string* out) {
  if (offset + n > this->file_size_) {
    *out = "";
    return comm::Status(comm::ErrorCode::kFileError, "file error");
  }

  *out = std::string(this->mmap_base_ + offset, n);
  return comm::Status::OK();
}

}  // namespace amkv::util
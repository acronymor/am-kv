#include "util/writable_file.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>

#include "util/dirname.h"

namespace amkv::util {
// TODO change to c++ style
comm::Status PosixWritableFile::syncFd() {
  if (::fdatasync(this->fd_) == 0) {
    return comm::Status::OK();
  }
  return comm::Status(comm::ErrorCode::kNotFound, filename_);
}

comm::Status PosixWritableFile::flushBuffer() {
  comm::Status status = writeUnbuffered(this->buf_, this->pos_);
  pos_ = 0;
  return status;
}

comm::Status PosixWritableFile::writeUnbuffered(const char* data, std::size_t size) {
  while (size > 0) {
    ssize_t write_result = ::write(fd_, data, size);
    if (write_result < 0) {
      if (errno == EINTR) {
        continue;  // Retry
      }
      return comm::Status(comm::ErrorCode::kNotFound, "not found data in " + this->filename_);
    }
    data += write_result;
    size -= write_result;
  }
  return comm::Status::OK();
}

PosixWritableFile::PosixWritableFile(const std::string& filename)
    : WritableFile(), pos_(0), fd_(0), filename_(filename), dirname_(Dirname(filename_)) {}

PosixWritableFile::~PosixWritableFile() {
  if (this->fd_ >= 0) {
    this->Close();
  }
}

comm::Status PosixWritableFile::Append(const std::string_view data) {
  std::size_t write_size = data.size();
  const char* write_data = data.data();
  size_t copy_size = std::min(write_size, kWritableFileBufferSize - this->pos_);
  std::memcpy(this->buf_ + this->pos_, write_data, copy_size);
  write_data += copy_size;
  write_size -= copy_size;
  this->pos_ += copy_size;
  if (write_size == 0) {
    return comm::Status::OK();
  }

  // Can't fit in buffer, so need to do at least one write.
  comm::Status status = this->flushBuffer();
  if (status.Code() == comm::ErrorCode::kOk) {
    return status;
  }

  // Small writes go to buffer, large writes are written directly.
  if (write_size < kWritableFileBufferSize) {
    std::memcpy(buf_, write_data, write_size);
    pos_ = write_size;
    return comm::Status::OK();
  }
  return this->writeUnbuffered(write_data, write_size);
}

comm::Status PosixWritableFile::Open(int oflag) {
  this->fd_ = ::open(this->filename_.c_str(), oflag, 0644);
  if (this->fd_ < 0) {
    return comm::Status(comm::ErrorCode::kNotFound, this->filename_);
  }
  return comm::Status::OK();
}

comm::Status PosixWritableFile::Close() {
  comm::Status status = this->flushBuffer();
  const int close_result = ::close(fd_);
  if (close_result < 0 && status.Code() == comm::ErrorCode::kOk) {
    status = comm::Status(comm::ErrorCode::kNotFound, filename_);
  }
  fd_ = -1;
  return status;
}

comm::Status PosixWritableFile::Flush() { return this->flushBuffer(); }

comm::Status PosixWritableFile::Sync() {
  // TODO sync dir if manifest

  comm::Status status = this->Flush();
  if (status.Code() != comm::ErrorCode::kOk) {
    return status;
  }
  return comm::Status::OK();
}

}  // namespace amkv::util

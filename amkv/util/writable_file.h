#pragma once

#include "comm/status.h"

namespace amkv::util {
constexpr const size_t kWritableFileBufferSize = 65536;

class WritableFile {
 public:
  WritableFile() = default;

  WritableFile(const WritableFile&) = delete;
  WritableFile& operator=(const WritableFile&) = delete;

  virtual ~WritableFile() = default;

  virtual comm::Status Append(const std::string_view& data) = 0;
  virtual comm::Status Open(int oflag) = 0;
  virtual comm::Status Close() = 0;
  virtual comm::Status Flush() = 0;
  virtual comm::Status Sync() = 0;
};

class PosixWritableFile final : public WritableFile {
 public:
  PosixWritableFile(const std::string& filename);
  ~PosixWritableFile();

  comm::Status Append(const std::string_view& data) override;
  comm::Status Open(int oflag) override;
  comm::Status Close() override;
  comm::Status Flush() override;
  comm::Status Sync() override;

 private:
  comm::Status flushBuffer();
  comm::Status writeUnbuffered(const char* data, size_t size);
  comm::Status syncFd();

 private:
  const std::string filename_;
  const std::string dirname_;

  char buf_[kWritableFileBufferSize];
  std::size_t pos_;
  int fd_;
};
}  // namespace amkv::util

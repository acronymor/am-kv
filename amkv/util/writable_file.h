#pragma once

#include "util/status.h"

namespace amkv::util {
constexpr const size_t kWritableFileBufferSize = 65536;

class WritableFile {
 public:
  WritableFile() = default;

  WritableFile(const WritableFile&) = delete;
  WritableFile& operator=(const WritableFile&) = delete;

  virtual ~WritableFile() = default;

  virtual Status Append(const std::string_view& data) = 0;
  virtual Status Open(int oflag) = 0;
  virtual Status Close() = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;
};

class PosixWritableFile final : public WritableFile {
 public:
  PosixWritableFile(const std::string& filename);
  ~PosixWritableFile();

  Status Append(const std::string_view& data) override;
  Status Open(int oflag) override;
  Status Close() override;
  Status Flush() override;
  Status Sync() override;

 private:
  Status flushBuffer();
  Status writeUnbuffered(const char* data, size_t size);
  Status syncFd();

 private:
  const std::string filename_;
  const std::string dirname_;

  char buf_[kWritableFileBufferSize];
  std::size_t pos_;
  int fd_;
};
}  // namespace amkv::util

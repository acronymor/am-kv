#pragma once

#include "comm/status.h"

namespace amkv::util {
class AccessFile {
 public:
  AccessFile() = default;

  AccessFile(const AccessFile&) = delete;
  AccessFile& operator=(const AccessFile&) = delete;

  virtual ~AccessFile() = default;

  virtual comm::Status Open(int oflag) = 0;
  virtual comm::Status Close() = 0;
  virtual comm::Status Read(std::uint64_t offset, std::size_t n, std::string* result) = 0;
  virtual std::size_t Size() = 0;
};

class PosixAccessFile final : public AccessFile {
 public:
  explicit PosixAccessFile(const std::string& filename);

  comm::Status Open(int oflag) override;
  comm::Status Close() override;
  comm::Status Read(std::uint64_t offset, std::size_t n, std::string* result) override;
  std::size_t Size() override;

private:
  int fd_;
  std::string filename_;
  std::size_t file_size_;
  char* mmap_base_;
};
}  // namespace amkv::util
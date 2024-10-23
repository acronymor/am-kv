#pragma once

#include "comm/status.h"

namespace amkv::util {

class SequentialFile {
 public:
  SequentialFile() = default;
  ~SequentialFile() = default;

  SequentialFile(const SequentialFile&) = delete;
  SequentialFile& operator=(const SequentialFile&) = delete;

  virtual comm::Status Open(int oflag) = 0;
  virtual comm::Status Close() = 0;
  virtual comm::Status Read(std::size_t n, std::string_view* result, char* scratch) = 0;
  virtual comm::Status Skip(std::uint64_t n) = 0;
};

class PosixSequentialFile final : public SequentialFile {
 public:
  explicit PosixSequentialFile(const std::string filename);

  comm::Status Open(int oflag) override;
  comm::Status Close() override;
  comm::Status Read(std::size_t n, std::string_view* result, char* scratch) override;
  comm::Status Skip(std::uint64_t n) override;

 private:
  const std::string filename_;
  int fd_;
};
}  // namespace amkv::util

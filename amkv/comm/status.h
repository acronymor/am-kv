#pragma once

#include <cstdint>
#include <string>

namespace amkv::comm {
enum struct ErrorCode : std::uint32_t { kOk = 0, kNotFound = 1 };

class Status {
 public:
  Status() = delete;
  ~Status() = default;

  Status(const ErrorCode code, const std::string& msg);

  Status(const Status& other);
  Status& operator=(const Status& other);

  Status(Status&& other) noexcept;
  Status& operator=(Status&& other) noexcept;

  static Status OK();

  [[nodiscard]] std::string Message() const;

  [[nodiscard]] ErrorCode Code() const;

 private:
  std::string message_;
  ErrorCode code_;
};
}  // namespace amkv::util

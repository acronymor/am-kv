#include "util/status.h"

#include <string>

namespace amkv::util {
Status::Status(const ErrorCode code, const std::string& message) {
  this->code_ = code;
  this->message_ = message;
}

Status::Status(const Status& other) : message_(other.message_), code_(other.code_) {}

Status::Status(Status&& other) noexcept : message_(std::move(other.message_)), code_(other.code_) {}

Status& Status::operator=(const Status& other) {
  if (this == &other) return *this;
  message_ = other.message_;
  code_ = other.code_;
  return *this;
}

Status& Status::operator=(Status&& other) noexcept {
  if (this == &other) return *this;
  message_ = std::move(other.message_);
  code_ = other.code_;
  return *this;
}

Status Status::OK() { return Status(ErrorCode::kOk, ""); }

std::string Status::Message() const { return message_; }

ErrorCode Status::Code() const { return code_; }

}  // namespace amkv::util

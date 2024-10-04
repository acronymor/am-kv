#pragma once

#include <cstdint>
#include <string>

#include "comm/status.h"

namespace amkv::block {
class BlockHandler {
 public:
  BlockHandler();

  void EncodeTo(std::string* dst);
  comm::Status DecodeFrom(std::string_view input);

  void SetOffset(std::uint64_t offset);
  void SetSize(std::uint64_t size);

 private:
  std::uint64_t offset_;
  std::uint64_t size_;
};
}  // namespace amkv::block

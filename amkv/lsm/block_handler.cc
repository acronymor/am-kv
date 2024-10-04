#include "lsm/block_handler.h"

#include <util/codec.h>

namespace amkv::block {

BlockHandler::BlockHandler() : offset_(0), size_(0) {}

void BlockHandler::EncodeTo(std::string* output) {
  util::EncodeUInt64(offset_, output);
  util::EncodeUInt64(size_, output);
}

comm::Status BlockHandler::DecodeFrom(std::string_view input) {
  std::size_t len = 0;
  len += util::DecodeUInt64(input.substr(len, sizeof(std::uint64_t)), &offset_);
  len += util::DecodeUInt64(input.substr(len, sizeof(std::uint64_t)), &size_);
  return comm::Status::OK();
}

void BlockHandler::SetOffset(std::uint64_t offset) { this->offset_ = offset; }

void BlockHandler::SetSize(std::uint64_t size) { this->size_ = size; }
}  // namespace amkv::block
#include "lsm/block_builder.h"

#include <util/codec.h>

namespace amkv::block {
BlockBuilder::BlockBuilder() {}

void BlockBuilder::Add(const std::string_view key, const std::string_view value) {
  this->buffer_.append(key);
  this->buffer_.append(value);
  this->counter_++;
}

void BlockBuilder::Reset() {
  this->buffer_.clear();
  this->counter_ = 0;
  this->finished_ = false;
  this->last_key_.clear();
}

std::string_view BlockBuilder::Build() {
  this->finished_ = true;
  return this->buffer_;
}

std::size_t BlockBuilder::Size() const { return this->buffer_.size(); }

bool BlockBuilder::Empty() const { return this->buffer_.empty(); }
}  // namespace amkv::block

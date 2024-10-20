#include "lsm/block_iter.h"

#include <util/codec.h>

#include <iostream>

namespace amkv::block {
std::uint32_t getBytesLength(std::string_view content, std::uint32_t offset) {
  std::uint32_t len = 0;
  util::DecodeUInt32(content.substr(offset, sizeof(std::uint32_t)), &len);
  len += sizeof(std::uint32_t);
  return len;
}

BlockIterator::BlockIterator(const Block* block) : block_(block), offset_(0) {}

BlockIterator::~BlockIterator() {}

bool BlockIterator::Valid() const {
  std::string s = "CRC32";
  return this->offset_ < this->block_->contents_.size() - s.length();
}

void BlockIterator::SeekToFirst() { this->offset_ = 0; }

void BlockIterator::SeekToLast() {}

void BlockIterator::Seek(const std::string& target) {
  while (this->Valid()) {
    if (this->Key() == target) {
      break;
    }
    this->Next();
  }
}

void BlockIterator::Next() {
  std::uint32_t len = 0;
  len += getBytesLength(this->block_->contents_, this->offset_);
  len += getBytesLength(this->block_->contents_, this->offset_ + len);

  this->offset_ += len;
}

void BlockIterator::Prev() {}

std::string BlockIterator::Key() const {
  std::string key;
  util::DecodeBytes(this->block_->contents_.substr(this->offset_), &key);
  return key;
}

std::string BlockIterator::Value() const {
  std::uint32_t len = getBytesLength(this->block_->contents_, this->offset_);

  std::string value;
  util::DecodeBytes(this->block_->contents_.substr(this->offset_ + len), &value);
  return value;
}

comm::Status BlockIterator::Status() const {}

std::uint64_t BlockIterator::nextEntryOffSet() const {}
}  // namespace amkv::block
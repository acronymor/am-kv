#include "util/arena.h"

#include <iostream>

namespace amkv::util {
static const std::size_t kBlockSize = 4096;

Arena::Arena() : alloc_ptr_(nullptr), alloc_bytes_remaining_(0), memory_usage_(0) {}

Arena::~Arena() {
  for (const auto& block : this->blocks_) {
    delete[] block;
  }
}
char* Arena::AllocateAligned(const std::size_t bytes) {
  constexpr int align = 8;
  const std::size_t currend_mod = reinterpret_cast<std::uintptr_t>(this->alloc_ptr_) & (align - 1);
  const std::size_t slop = currend_mod == 0 ? 0 : align - currend_mod;
  const std::size_t needed = bytes + slop;

  char* result;
  if (needed <= this->alloc_bytes_remaining_) {
    result = this->alloc_ptr_ + slop;
    this->alloc_ptr_ += needed;
    this->alloc_bytes_remaining_ -= needed;
  } else {
    result = this->allocateFallback(bytes);
  }

  return result;
}

char* Arena::allocateFallback(const std::size_t bytes) {
  if (bytes > kBlockSize / 4) {
    char* result = this->allocateNewBlock(bytes);
    return result;
  }

  this->alloc_ptr_ = this->allocateNewBlock(kBlockSize);
  this->alloc_bytes_remaining_ = kBlockSize;

  char* result = this->alloc_ptr_;
  this->alloc_ptr_ += bytes;
  this->alloc_bytes_remaining_ -= bytes;

  return result;
}

char* Arena::allocateNewBlock(const std::size_t bytes) {
  char* result = new char[bytes];
  this->blocks_.push_back(result);
  this->memory_usage_.fetch_add(bytes + sizeof(char*), std::memory_order_relaxed);
  return result;
}
}  // namespace amkv::util

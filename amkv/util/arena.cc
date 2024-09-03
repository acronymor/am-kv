#include "util/arena.h"

#include <iostream>

namespace amkv::util {
Arena::Arena() : alloc_ptr_(nullptr), alloc_bytes_remaining_(0), memory_usage_(0) {}

Arena::~Arena() {
  for (const auto& block : blocks_) {
    delete[] block;
  }
}
}  // namespace amkv::util

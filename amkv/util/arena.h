#pragma once

#include <atomic>
#include <iostream>
#include <vector>

namespace amkv::util {
class Arena {
 public:
  Arena();
  ~Arena();
  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  char* AllocateAligned(const std::size_t bytes);

  std::size_t Usage() const;

private:
  char* allocateFallback(const std::size_t bytes);
  char* allocateNewBlock(const std::size_t bytes);

 private:
  char* alloc_ptr_;
  std::size_t alloc_bytes_remaining_;

  std::vector<char*> blocks_;
  std::atomic_size_t memory_usage_;
};
}  // namespace amkv::util
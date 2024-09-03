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

 private:
  char* alloc_ptr_;
  std::size_t alloc_bytes_remaining_;

  std::vector<char*> blocks_;
  std::atomic_size_t memory_usage_;
};
}  // namespace amkv::util
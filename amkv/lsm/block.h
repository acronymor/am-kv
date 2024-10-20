#include <string_view>

namespace amkv::block {
class Block {
  friend class BlockIterator;

 public:
  explicit Block(const std::string_view contents);
  ~Block() = default;

  Block(const Block&) = delete;
  Block& operator=(const Block&) = delete;

 private:
  std::string_view contents_;
};
}  // namespace amkv::block

#pragma once

#include <vector>

#include "comm/options.h"

namespace amkv::block {
class BlockBuilder {
 public:
  explicit BlockBuilder();

  BlockBuilder(const BlockBuilder&) = delete;
  BlockBuilder& operator=(const BlockBuilder&) = delete;

  void Add(const std::string_view key, const std::string_view value);
  void Reset();
  std::string_view Build();

  std::size_t Size() const;

  bool Empty() const;

 private:
  std::size_t counter_;

  bool finished_;

  std::string buffer_;
  std::string last_key_;
};
}  // namespace amkv::block
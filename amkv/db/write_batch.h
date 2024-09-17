#pragma once

#include <string>
#include <string_view>
namespace amkv::db {

class WriteBatch {
 public:
  void Put(const std::string_view& key, const std::string_view& value);

  void Delete(const std::string_view& key);

 private:
  friend class WriteBatchInternal;

 private:
  std::string rep_;
};
}  // namespace amkv::db
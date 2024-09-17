#include "db/write_batch.h"

#include "db/db_format.h"
#include "util/codec.h"

namespace amkv::db {

void WriteBatch::Put(const std::string_view& key, const std::string_view& value) {
  this->rep_.push_back(static_cast<char>(ValueType::kTypeValue));
  util::EncodeBytes(key, &this->rep_);
  util::EncodeBytes(value, &this->rep_);
}

void WriteBatch::Delete(const std::string_view& key) {
  this->rep_.push_back(static_cast<char>(ValueType::kTypeValue));
  util::EncodeBytes(key, &this->rep_);
}

}  // namespace amkv::db
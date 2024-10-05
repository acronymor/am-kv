#include "db/write_batch.h"

#include "lsm/db_format.h"
#include "util/codec.h"

namespace amkv::db {

void WriteBatch::Put(const std::string_view key, const std::string_view value) {
  this->rep_.push_back(static_cast<char>(lsm::ValueType::kTypeValue));
  util::EncodeBytes(key, &this->rep_);
  util::EncodeBytes(value, &this->rep_);
}

void WriteBatch::Delete(const std::string_view key) {
  this->rep_.push_back(static_cast<char>(lsm::ValueType::kTypeDeletion));
  util::EncodeBytes(key, &this->rep_);
}

comm::Status WriteBatch::Iterate(WriteBatchHandler* handler) {
  auto func = [](const std::string_view input, std::string* output) -> std::size_t {
    std::uint32_t len2 = 0;
    const std::size_t len1 = util::DecodeUInt32(input.substr(0, sizeof(std::uint32_t)), &len2);
    const std::size_t len = util::DecodeBytes(input.substr(0, len1 + len2), output);
    return len;
  };

  std::size_t len = 0;
  std::string key;
  std::string value;

  std::string_view input(this->rep_);
  char tag = input[0];
  input.remove_prefix(1);

  switch (static_cast<lsm::ValueType>(tag)) {
    case lsm::ValueType::kTypeValue: {
      len = func(input, &key);
      input.remove_prefix(len);

      len = func(input, &value);
      input.remove_prefix(len);

      handler->Put(key, value);
    } break;
    case lsm::ValueType::kTypeDeletion: {
      len = func(input, &key);
      input.remove_prefix(len);

      handler->Delete(key);
    } break;
  }

  return comm::Status::OK();
}

}  // namespace amkv::db
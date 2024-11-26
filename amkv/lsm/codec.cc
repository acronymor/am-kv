#include "lsm/codec.h"

#include <format>

#include "util/codec.h"

constexpr std::size_t kSeqTypeLength = 8;

namespace amkv::lsm {

std::size_t EncodeMemTable(const std::string_view key, const std::string_view value, const SequenceNumber seq,
                           const ValueType type, std::string* out) {
  std::string internal_key;
  internal_key.append(key);

  util::EncodeUInt64(seq, &internal_key);
  util::EncodeUInt8(static_cast<std::uint8_t>(type), &internal_key);

  std::size_t len = util::EncodeBytes(internal_key, out);
  len += util::EncodeBytes(value, out);

  return len;
}
std::size_t DecodeMemTable(const std::string_view in, std::string* key, std::string* value, SequenceNumber* seq,
                           ValueType* type) {
  std::string internal_key;
  std::size_t len = util::DecodeBytes(in, &internal_key);

  *key = internal_key.substr(0, internal_key.length() - sizeof(std::uint64_t) - sizeof(std::uint8_t));
  util::DecodeUInt64(internal_key.substr(key->length(), sizeof(std::uint64_t)), seq);

  std::uint8_t type_tmp;
  util::DecodeUInt8(internal_key.substr(key->length() + sizeof(std::uint64_t), sizeof(std::uint8_t)), &type_tmp);
  *type = static_cast<ValueType>(type_tmp);

  switch (*type) {
    case ValueType::kTypeValue: {
      len += util::DecodeBytes(in.substr(len), value);
    } break;
    case ValueType::kTypeDeletion: {
    } break;
  }

  return len;
}
}  // namespace amkv::lsm
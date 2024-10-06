#include "lsm/codec.h"

#include "util/codec.h"

namespace amkv::lsm {
std::size_t EncodeMemTable(const std::string_view key, const std::string_view value, const SequenceNumber seq,
                           const ValueType type, std::string* out) {
  std::size_t len = 0;
  len += util::EncodeBytes(key, out);
  len += util::EncodeUInt64(seq, out);
  len += util::EncodeUInt8(static_cast<std::uint8_t>(type), out);
  len += util::EncodeBytes(value, out);
  return len;
}
std::size_t DecodeMemTable(const std::string_view in, std::string* key, std::string* value, SequenceNumber* seq,
                           ValueType* type) {
  std::uint32_t len = 0;

  util::DecodeUInt32(in.substr(len, sizeof(std::uint32_t)), &len);
  len = util::DecodeBytes(in.substr(0, len + sizeof(std::uint32_t)), key);
  len += util::DecodeUInt64(in.substr(len, sizeof(std::uint64_t)), seq);

  std::uint8_t type1 = 0;
  len += util::DecodeUInt8(in.substr(len, sizeof(std::uint8_t)), &type1);
  *type = static_cast<ValueType>(type1);

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
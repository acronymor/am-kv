#include "util/codec.h"

namespace amkv::util {
std::size_t EncodeUInt8(std::uint8_t in, std::string* out) {
  out->append(reinterpret_cast<char*>(&in), sizeof(std::uint8_t));
  return sizeof(in);
}

size_t DecodeUInt8(const std::string_view in, std::uint8_t* out) {
  const uint8_t* p = reinterpret_cast<const uint8_t*>(in.data());
  *out = *p;
  return sizeof(uint8_t);
}

std::size_t EncodeUInt32(std::uint32_t in, std::string* out) {
  uint32_t ret = htobe32(in);
  out->append(reinterpret_cast<char*>(&ret), sizeof(ret));
  return sizeof(ret);
}

size_t DecodeUInt32(const std::string_view in, std::uint32_t* out) {
  const uint32_t* p = reinterpret_cast<const uint32_t*>(in.data());
  *out = be32toh(*p);
  return sizeof(uint32_t);
}

std::size_t EncodeUInt64(std::uint64_t in, std::string* out) {
  std::uint64_t ret = htobe64(in);
  out->append(reinterpret_cast<char*>(&ret), sizeof(ret));
  return sizeof(ret);
}

size_t DecodeUInt64(const std::string_view in, std::uint64_t* out) {
  const uint64_t* p = reinterpret_cast<const std::uint64_t*>(in.data());
  *out = be64toh(*p);
  return sizeof(uint64_t);
}

std::size_t EncodeBytes(const std::string_view in, std::string* out) {
  const std::uint32_t len = in.size();
  const std::size_t ret = EncodeUInt32(len, out);
  out->append(in.data(), in.size());
  return in.size() + ret;
}

std::size_t DecodeBytes(const std::string_view in, std::string* out) {
  std::uint32_t len = 0;
  const std::size_t ret = DecodeUInt32(in, &len);
  out->append(in.data() + sizeof(std::uint32_t), len);
  return len + ret;
}
}  // namespace amkv::util

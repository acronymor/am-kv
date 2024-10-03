#include <cstdint>
#include <string>

namespace amkv::util {
std::size_t EncodeUInt8(std::uint8_t in, std::string* out);
std::size_t DecodeUInt8(const std::string_view in, std::uint8_t* out);

std::size_t EncodeUInt32(std::uint32_t in, std::string* out);
std::size_t DecodeUInt32(const std::string_view in, std::uint32_t* out);

std::size_t EncodeUInt64(std::uint64_t in, std::string* out);
std::size_t DecodeUInt64(const std::string_view in, std::uint64_t* out);

std::size_t EncodeBytes(const std::string_view in, std::string* out);
std::size_t DecodeBytes(const std::string_view in, std::string* out);
}  // namespace amkv::util
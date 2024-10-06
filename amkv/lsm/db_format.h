#pragma once

#include <cstdint>

namespace amkv::lsm {
typedef std::uint64_t SequenceNumber;

static const SequenceNumber kMaxSequenceNumber = ((0x1ull << 56) - 1);

enum struct ValueType : std::uint8_t { kTypeDeletion = 0, kTypeValue = 1 };

}  // namespace amkv::lsm
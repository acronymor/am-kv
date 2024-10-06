#include <iostream>

#include "lsm/db_format.h"

namespace amkv::lsm {
std::size_t EncodeMemTable(std::string_view key, std::string_view value, SequenceNumber seq, ValueType type,
                           std::string* out);
std::size_t DecodeMemTable(std::string_view in, std::string* key, std::string* value, SequenceNumber* seq,
                           ValueType* type);

}  // namespace amkv::lsm
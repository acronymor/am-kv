#pragma once

#include <cstdint>

#include "db/db_format.h"
#include "util/arena.h"
#include "util/skiplist.h"

namespace amkv::table {
class MemTable {
 public:
  MemTable();

  void Ref();
  void UnRef();

  void Add(db::SequenceNumber seq, db::ValueType type, const std::string_view& key, const std::string_view& value);

 private:
  std::uint64_t refs_;
  util::Arena arena_;

 // util::SkipList table_;
};

}  // namespace amkv::table
#pragma once

#include <comm/status.h>

#include <cstdint>

#include "lsm/comparator.h"
#include "lsm/db_format.h"
#include "util/arena.h"
#include "util/skiplist.h"

namespace amkv::table {
class LookupKey {
 public:
  LookupKey(const std::string_view user_key, lsm::SequenceNumber sequence);
  ~LookupKey() = default;

  LookupKey(const LookupKey&) = delete;
  LookupKey& operator=(const LookupKey&) = delete;

  std::string_view user_key() const;
  std::string_view memtable_key() const;
  std::string_view internal_key() const;

 private:
  std::size_t start_;
  std::size_t end_;
  std::size_t kstart_;

  std::string key_;
};

class MemTable {
 public:
  MemTable(const lsm::InternalKeyComparator& comparator);
  ~MemTable();

  MemTable(const MemTable&) = delete;
  MemTable& operator=(const MemTable&) = delete;

  void Ref();
  void UnRef();

  void Add(lsm::SequenceNumber seq, lsm::ValueType type, const std::string_view key, const std::string_view value);
  bool Get(const LookupKey& key, std::string* value, comm::Status* status);

 private:
  std::uint64_t refs_;
  util::Arena arena_;

  using Table = util::SkipList<const std::string_view, lsm::MemTableKeyComparator>;

  const lsm::MemTableKeyComparator comparator_;
  Table table_;
};

}  // namespace amkv::table
#pragma once

#include <comm/iterator.h>
#include <comm/status.h>

#include <cstdint>

#include "lsm/comparator.h"
#include "lsm/db_format.h"
#include "lsm/skiplist.h"
#include "util/arena.h"

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
  friend class MemTableIterator;

 public:
  MemTable(const lsm::InternalKeyComparator* comparator);
  ~MemTable();

  MemTable(const MemTable&) = delete;
  MemTable& operator=(const MemTable&) = delete;

  void Ref();
  void UnRef();

  void Add(lsm::SequenceNumber seq, lsm::ValueType type, const std::string_view key, const std::string_view value);
  bool Get(const LookupKey& key, std::string* value, comm::Status* status);

  [[nodiscard]] std::size_t Usage() const;

 private:
  std::uint64_t refs_;
  util::Arena arena_;

  using Table = lsm::SkipList<const std::string_view, const std::string_view>;

  lsm::MemTableKeyComparator* comparator_;
  Table* table_;
};

}  // namespace amkv::table
#pragma once

#include <lsm/db_format.h>
#include <lsm/memtable.h>

#include <string>
#include <string_view>
namespace amkv::db {
// MemTableInserter
class WriteBatchHandler {
 public:
  WriteBatchHandler(table::MemTable* memtable);

  void Put(const std::string_view key, const std::string_view value);

  void Delete(const std::string_view key);

 private:
  lsm::SequenceNumber sequence_;
  table::MemTable* memtable_;
};
}  // namespace amkv::db
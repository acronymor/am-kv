#include "lsm/compaction.h"

#include "lsm/memtable_iter.h"
#include "lsm/sstable.h"
#include "util/filename.h"

namespace amkv::lsm {
MinorCompaction::MinorCompaction() {}

bool MinorCompaction::CanDoCompaction() { return true; }

comm::Status MinorCompaction::Do(const table::MemTable* const memtable) {
  comm::Status status = this->WriteLevel0Table(memtable);
  return comm::Status::OK();
}

comm::Status MinorCompaction::WriteLevel0Table(const table::MemTable* const memtable) {
  std::string db_name = "db_name";
  std::uint64_t table_id = 0;
  std::string fname = util::TableFileName(db_name, table_id);

  table::SSTable sstable;
  comm::Status status = sstable.BuildTable(fname, memtable);

  return comm::Status::OK();
}
}  // namespace amkv::lsm
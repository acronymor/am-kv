#include "lsm/compaction.h"

#include "lsm/memtable_iter.h"
#include "lsm/sstable.h"
#include "util/filename.h"

namespace amkv::lsm {
MinorCompaction::MinorCompaction(std::string dbname): db_name_(std::move(dbname)) {}

bool MinorCompaction::CanDoCompaction() { return true; }

comm::Status MinorCompaction::Do(const table::MemTable* const memtable) {
  comm::Status status = this->WriteLevel0Table(memtable);

  version::FileMetaData meta = {
      .refs = 0,
      .number = 1,
      .file_size = 20,
      .smallest = "a",
      .largest = "z",
  };

  return comm::Status::OK();
}

comm::Status MinorCompaction::WriteLevel0Table(const table::MemTable* const memtable) {
  std::uint64_t table_id = 0;
  std::string fname = util::TableFileName(this->db_name_, table_id);

  table::SSTable sstable;
  comm::Status status = sstable.Put(fname, memtable);

  return status;
}
}  // namespace amkv::lsm
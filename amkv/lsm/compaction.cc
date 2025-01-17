#include "lsm/compaction.h"

#include "lsm/memtable_iter.h"
#include "lsm/sstable.h"
#include "util/filename.h"

namespace amkv::lsm {
MinorCompaction::MinorCompaction(std::string dbname) : db_name_(std::move(dbname)) {}

bool MinorCompaction::CanDoCompaction() { return true; }

comm::Status MinorCompaction::Do(const table::MemTable* const memtable, version::Version* base,
                                 version::VersionEdit* edit, std::uint64_t new_number) {
  version::FileMetaData meta{.number = new_number};
  comm::Status status = this->WriteLevel0Table(memtable, &meta);

  if (status.Code() == comm::ErrorCode::kOk && meta.file_size > 0) {
    std::size_t level = 0;
    if (base != nullptr) {
      level = base->PickLevelForMemTableOutput(meta.smallest, meta.largest);
    }
    edit->AddFile(level, &meta);
  }

  return status;
}

comm::Status MinorCompaction::WriteLevel0Table(const table::MemTable* const memtable, version::FileMetaData* meta) {
  std::string fname = util::TableFileName(this->db_name_, meta->number);

  table::SSTable sstable;
  comm::Status status = sstable.Put(fname, memtable, meta);

  return status;
}
}  // namespace amkv::lsm
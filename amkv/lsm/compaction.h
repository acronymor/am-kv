#include "comm/status.h"
#include "lsm/memtable.h"
#include "lsm/version.h"

namespace amkv::lsm {
class Compaction {
 public:
  void MaybeScheduleCompaction();
};

class MinorCompaction {
 public:
  MinorCompaction(const std::string db_name);

  bool CanDoCompaction();

  comm::Status Do(const table::MemTable* const memtable, version::Version* base, version::VersionEdit* edit, std::size_t new_number);
  comm::Status WriteLevel0Table(const table::MemTable* const memtable, version::FileMetaData* meta);

 private:
  table::MemTable* mem_;
  table::MemTable* imm_;

  std::string db_name_;
};

class MajorCompaction {};
}  // namespace amkv::lsm
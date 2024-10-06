#include "comm/status.h"
#include "lsm/memtable.h"

namespace amkv::lsm {
class Compaction {
 public:
  void MaybeScheduleCompaction();
};

class MinorCompaction {
 public:
  MinorCompaction();

  bool CanDoCompaction();

  comm::Status Do(const table::MemTable* const memtable);
  comm::Status WriteLevel0Table(const table::MemTable* const memtable);

 private:
  table::MemTable* mem_;
  table::MemTable* imm_;
};

class MajorCompaction {};
}  // namespace amkv::lsm
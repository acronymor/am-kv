#include <cstdint>

#include "comm/status.h"
#include "db/write_batch.h"
#include "lsm/db_format.h"
#include "lsm/memtable.h"

namespace amkv::db {

class WriteBatchInternal {
 public:
  void SetCount(std::uint32_t count);

  std::uint32_t Count();

  void SetSequence(lsm::SequenceNumber seq);

  lsm::SequenceNumber Sequence();

  std::string Contents(const WriteBatch* batch);

  comm::Status InsertInto(WriteBatch* batch, table::MemTable* memtable);

 private:
  std::uint32_t count_{0};
  lsm::SequenceNumber seq_{0};
};
}  // namespace amkv::db

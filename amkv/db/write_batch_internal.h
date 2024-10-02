#include <cstdint>
#include <string_view>

#include "db/db_format.h"
#include "db/write_batch.h"
#include "lsm/memtable.h"
#include "comm/status.h"

namespace amkv::db {

class WriteBatchInternal {
 public:
  static void SetCount(WriteBatch& batch, std::uint32_t count);

  static std::uint32_t Count(const WriteBatch* batch);

  static void SetSequence(WriteBatch* batch, SequenceNumber seq);

  static SequenceNumber Sequence(WriteBatch* batch);

  static std::string_view Contents(const WriteBatch* batch);

  static comm::Status InsertInto(const WriteBatch* batch, table::MemTable* memtable);
};
}  // namespace amkv::db
